/**********************************************************************************/
/*                                                                                */
/* File:   render.cpp                                                             */
/* Author: bkenwright@xbdev.net                                                   */
/* web:    www.xbdev.net                                                          */
/*                                                                                */
/**********************************************************************************/
/*


*/
/**********************************************************************************/

#pragma comment(lib, "D3d9.lib")  // DirectX 9 library's
#include <d3dx9.h>                // DirectX header files

#include <stdio.h>
#include <time.h>
#if MINGW
#define	_ASSERT(x) {}	// MINGW
#else
#include <crtdbg.h>               // Contains ASSERT(..) Macro
#endif

#include "render.h"	//
#include "OBJSTAR.C"

#define NUMTRIANGLES 20

// Simple structure for so we can draw loads of triangles easier
struct stTriangle
{
	int x0, y0;
	int x1, y1;
	int x2, y2;
}
WireTriangles[NUMTRIANGLES];

#define RAND_RANGE(a,b) ( (a) + (rand()%((a)-(b)+1)))

// Q16.16
#define FIXEDPOINT_16_16 16
#define FIXEDPOINT_16_16_ROUND_UP 0x00008000	//	1<<(FIXEDPOINT_16_16) 

////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//  These four functions will be called externally from main, and update our      //
//  game, initilise it.... render it...etc.                                       //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////


bool Create()
{
	int min=10, max=400;

	srand( (unsigned int)time(0) );

	for( int i=0; i<NUMTRIANGLES; i++ )
	{
		WireTriangles[i].x0 = RAND_RANGE(min,max);
		WireTriangles[i].y0 = RAND_RANGE(min,max);

		WireTriangles[i].x1 = RAND_RANGE(min,max);
		WireTriangles[i].y1 = RAND_RANGE(min,max);

		WireTriangles[i].x2 = RAND_RANGE(min,max);
		WireTriangles[i].y2 = RAND_RANGE(min,max);
	}
	
	init_stars();
	create_lookup_tables();
	
	return true; 
};

void Release(){};
void Update(){};

////////////////////////////////////////////////////////////////////////////////////


void SetPixel(unsigned int* pBits, int w, int h, int pitch,
			  int x, int y, DWORD dwColour)
{
	pBits[ x + y*w ] = dwColour;
}// End of SetPixel(..)


void ClearScreen(unsigned int* pBits, int w, int h, int pitch)
{
	for(int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
			pBits[ x + y*w] = 0xff000000;
		}// End loop x
	}// End loop y
}// End of ClearScreen(..)


////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
// This is a simple "Line" drawing algorithm, using fixed point maths...          //
// its a little tricky to understand at first...and it took me a while to get     //
// it all ticking away perfect due to small overflow or off by one errors...or    //
// the last one was sign's of numbers where wrong :-/                             //
// But it all seems okay now.                                                     //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////


void line(unsigned int* pBits, int w, int h, int pitch,
		  int x0, int y0,
		  int x1, int y1,
		  DWORD dwColour)
{
	// Some simple debug checking which won't be in the release build, but checks
	// for simple overflow or underflow of passed numbers.
	_ASSERT( !( (x0>65536) || (x1>65536) || (y0>65536) || (y1>65536) ) ); // Overflow
	_ASSERT( !( (x0<0) || (x1<0) || (y0<0) || (y1<0) ) );                 // Underflow


	int dy = (y1-y0);
	int dx = (x1-x0);

	int sdx = ( dx<0 ) ? -1 : 1 ; // sign of dx and dy
	int sdy = ( dy<0 ) ? -1 : 1 ;

	dx = dx * sdx;
	dy = dy * sdy;

	int px = x0 << FIXEDPOINT_16_16; // starting point (x0,y0)
	int py = y0 << FIXEDPOINT_16_16;

	// Should check for divide by zero errors here.
	int dxdy = 0;
	if( dy!=0 ) dxdy = ((dx << FIXEDPOINT_16_16)/dy)*sdx;

	int dydx = 0;
	if( dx!=0 ) dydx = ((dy << FIXEDPOINT_16_16)/dx)*sdy;

	sdx = (1 << FIXEDPOINT_16_16)*sdx;
	sdy = (1 << FIXEDPOINT_16_16)*sdy;


	if( dx > dy )
	{
		for(int x=0; x<dx; x++)
		{
			SetPixel(pBits, w, h, pitch,
					px>>FIXEDPOINT_16_16, 
					py>>FIXEDPOINT_16_16, dwColour);
			
			py += dydx;
			px += sdx;

			_ASSERT( !(py<0) || !(px<0) ); // If we get an assert here
			                               // then there's a bug in my algorithm

		}// End for loop x
	}
	else
	{
		for(int y=0; y<dy; y++)
		{
			SetPixel(pBits, w, h, pitch,
					px>>FIXEDPOINT_16_16, 
					py>>FIXEDPOINT_16_16, dwColour);
			
			px += dxdy;
			py += sdy;

			_ASSERT( !(py<0) || !(px<0) ); // If we get an assert here
			                               // then there's a bug in my algorithm
		}// End for loop y
	}

}// End of line(..)


void wire_triangle(unsigned int* pBits, int w, int h, int pitch,
				   int x0, int y0,
				   int x1, int y1,
				   int x2, int y2,
				   DWORD dwColour)
{

	line(pBits, w, h, pitch,   x0, y0,  x1, y1, dwColour);
	line(pBits, w, h, pitch,   x1, y1,  x2, y2, dwColour);
	line(pBits, w, h, pitch,   x2, y2,  x0, y0, dwColour);

}// End of wire_triangle(..)

void Render(unsigned int* pBits, int w, int h, int pitch)
{
	ClearScreen(pBits, w, h, pitch);
	
/*
	// Multiple Line Debug Testing
	for( int i=0; i<NUMTRIANGLES; i++ )
	{
		int x0 = WireTriangles[i].x0; int y0 = WireTriangles[i].y0;
		int x1 = WireTriangles[i].x1; int y1 = WireTriangles[i].y1;
		int x2 = WireTriangles[i].x2; int y2 = WireTriangles[i].y2;


		wire_triangle(pBits, w, h, pitch,
					x0, y0,
					x1, y1,
					x2, y2, 0xff00ff00);

	}// End of for loop
*/
	
		framebuf = pBits;
		screen_width = w;
		screen_height = h;
		screen_size = screen_width*screen_height;
		
		clear_stars();
		calc_stars();
		draw_stars();
		move_stars();
		draw_border();
		
		xangle=(xangle+5)%360;
		yangle=(yangle+5)%360;
		zangle=(zangle+10)%360;
		rotate_object();
		draw_points();
		calc_normal();
		draw_poly();	
	

}// End of Render(..)



