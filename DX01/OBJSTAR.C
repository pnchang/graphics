#define _USE_MATH_DEFINES	// for C

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

#ifndef	M_PI 
#define	M_PI	3.14159265358979323846
#endif  

#define num_faces	14
#define num_vect	16
#define num_points	4
#define num_stars	900

float cosine[360];
float sine[360];

typedef struct
{
	float x,y,z;
} vect_type;

typedef struct
{
	int x,y;
} pos_type;

typedef struct
{
	int x,y,z;
} star_type;

vect_type object[num_vect]={{-10,30,10,},{-30,10,30},{-30,-10,30},{-10,-30,10},
			    {10,-30,10},{30,-10,30},{30,10,30},{10,30,10},
			    {10,-30,-10},{30,-10,-30},{30,10,-30},{10,30,-10},
			    {-10,-30,-10},{-30,-10,-30},{-30,10,-30},{-10,30,-10}};
vect_type translate[num_vect];
vect_type temp;
vect_type normal[num_faces];
vect_type position[num_vect];
vect_type unit_vector[num_faces];
vect_type light={-1,-1,-1};
star_type stars[num_stars];
pos_type old_pos[num_stars];
pos_type new_pos[num_stars];

int face[num_faces][num_points]={{0,1,6,7},{1,2,5,6},{2,3,4,5},{7,6,10,11},
				{6,5,9,10},{5,4,8,9},{11,10,14,15},{10,9,13,14},
				{9,8,12,13},{15,14,1,0},{14,13,2,1},{13,12,3,2},
				{15,0,7,11},{3,12,8,4}};

int u1,u2,u3,v1,v2,v3;

int xangle=0;
int yangle=0;
int zangle=0;


int counter;


extern	HDC hDC;
unsigned int *framebuf;

int screen_width=800;
int screen_height=600;
long screen_size=screen_width*screen_height;	//0xffff;

char *screen=(char *) 0xA0000;

char *virt_screen;
long Palette[256];	// 256 Colors Palette Table

/* Border part */
int border_height=15;

/* Star part */
int star_xoff=screen_width/2;
int star_yoff=screen_height/2;
int star_zoff=800;

/* Main object part */
int obj_xoff=screen_width/2;
int obj_yoff=screen_height/2;
int obj_zoff=800;


void retrace(void)
{

}

void setvga(void)
{

}

void settext(void)
{

}

void putPIXEL(int x1, int y1, int color)
{
	//memset(framebuf+x1+(y1*screen_width),color,1);
	*(framebuf+x1+(y1*screen_width))=color;
}

void hline(int x1,int x2,int y,int color)
{
	int i;
	
	for(i=x1;i<=x2;i++)
	{
		*(framebuf+i+(y*screen_width))=color;
	}
	//memset(virt_screen+x1+(y*screen_width),color,x2-x1+1);
}

/*
an 18-bit RGB palette use 6 bits for each of the red, green, and blue color components.
 r, g, b : 0 to 63 
*/
void pal(unsigned char col,unsigned char r,unsigned char g,unsigned char b)
{
	Palette[col]=RGB(r,g,b); 
}

void setpal(void)
{
	int i;
	for(i=0;i<64;i++)
	{
		pal(i,i,i,i);
		pal(i+64,64,i-64,0);
	}
}

void draw_border(void)
{
	int i,color;
	for(i=0;i<border_height;i++)
	{
		color=(border_height-i)*4;
		color=RGB( color, color, color);
		hline(0,screen_width-1,i,color);
		hline(0,screen_width-1,screen_height-1-i,color);
	}
}

void create_lookup_tables(void)
{
	int i;
	for(i=0;i<360;i++)
	{
		cosine[i]=cos((i*M_PI)/180);
		sine[i]=sin((i*M_PI)/180);
	}
}

void calc_normal(void)
{
	int i,n;

	for(i=0;i<num_faces;i++)
	{
		u1=position[face[i][1]].x-position[face[i][0]].x;
		u2=position[face[i][1]].y-position[face[i][0]].y;
		u3=position[face[i][1]].z-position[face[i][0]].z;

		v1=position[face[i][2]].x-position[face[i][0]].x;
		v2=position[face[i][2]].y-position[face[i][0]].y;
		v3=position[face[i][2]].z-position[face[i][0]].z;

		normal[i].x=u2*v3-u3*v2;
		normal[i].y=u3*v1-u1*v3;
		normal[i].z=u1*v2-u2*v1;

		n=sqrt(normal[i].x*normal[i].x+
		       normal[i].y*normal[i].y+
		       normal[i].z*normal[i].z);

		unit_vector[i].x=(normal[i].x/n);
		unit_vector[i].y=(normal[i].y/n);
		unit_vector[i].z=(normal[i].z/n);
	}
}

void rotate_object(void)
{
	int i;
	for(i=0;i<num_vect;i++)
	{
		/*rotate around x-axis*/
		temp.x=object[i].x;
		temp.y=object[i].y*cosine[xangle]-object[i].z*sine[xangle];
		temp.z=object[i].y*sine[xangle]+object[i].z*cosine[xangle];
		translate[i]=temp;

		/*rotate around y-axis*/
		temp.x=translate[i].x*cosine[yangle]-translate[i].z*sine[yangle];
		temp.y=translate[i].y;
		temp.z=translate[i].x*sine[yangle]+translate[i].z*cosine[yangle];
		translate[i]=temp;

		/*rotate around z-axis*/
		temp.x=translate[i].x*cosine[zangle]-translate[i].y*sine[zangle];
		temp.y=translate[i].x*sine[zangle]+translate[i].y*cosine[zangle];
		temp.z=translate[i].z;
		translate[i]=temp;
	}
}

void draw_points(void)
{
	int i;

	for(i=0;i<num_vect;i++)
	{
		/*plot the pixels to the screen in perspective*/
		position[i].x=(int)((translate[i].x*256)/(translate[i].z+obj_zoff)+obj_xoff);
		position[i].y=(int)((translate[i].y*256)/(translate[i].z+obj_zoff)+obj_yoff);
		position[i].z=(int)translate[i].z;
	}
}

void draw_poly(void)
{
	int i,color;
	float intensity;
	int xx,yy,maxx,maxy,minx,miny;
	int mul1,div1,mul2,div2,mul3,div3,mul4,div4;
	int x1,y1,x2,y2,x3,y3,x4,y4;
	for(i=0;i<num_faces;i++)
	{
		if(unit_vector[i].z<=0)
		{
			x1=position[face[i][0]].x;	y1=position[face[i][0]].y;
			x2=position[face[i][1]].x;	y2=position[face[i][1]].y;
			x3=position[face[i][2]].x;	y3=position[face[i][2]].y;
			x4=position[face[i][3]].x;	y4=position[face[i][3]].y;

			miny=y1;
			maxy=y1;

			if(y2<miny) miny=y2;
			if(y2>maxy) maxy=y2;
			if(y3<miny) miny=y3;
			if(y3>maxy) maxy=y3;
			if(y4<miny) miny=y4;
			if(y4>maxy) maxy=y4;

			if(miny<border_height)
				miny=border_height;
			if(maxy>(screen_height-1-border_height))
				maxy=screen_height-1-border_height;	// 200-1-15=184;

			mul1=x1-x4; div1=y1-y4;
			mul2=x2-x1; div2=y2-y1;
			mul3=x3-x2; div3=y3-y2;
			mul4=x4-x3; div4=y4-y3;

			if(obj_zoff>150) obj_zoff-=1;
			for(yy=miny;yy<maxy;yy++)
			{
				minx=screen_width;
				maxx=-1;
				if((y4>=yy) || (y1>=yy))
					if((y4<=yy) || (y1<=yy))
						if(y4!=y1)
						{
							xx=(int)((yy-y4)*mul1/div1)+x4;
							if(xx<minx) minx=xx;
							if(xx>maxx) maxx=xx;
						}
				if((y1>=yy) || (y2>=yy))
					if((y1<=yy) || (y2<=yy))
						if(y1!=y2)
						{
							xx=(int)((yy-y1)*mul2/div2)+x1;
							if(xx<minx) minx=xx;
							if(xx>maxx) maxx=xx;
						}
				if((y2>=yy) || (y3>=yy))
					if((y2<=yy) || (y3<=yy))
						if(y2!=y3)
						{
							xx=(int)((yy-y2)*mul3/div3)+x2;
							if(xx<minx) minx=xx;
							if(xx>maxx) maxx=xx;
						}
				if((y3>=yy) || (y4>=yy))
					if((y3<=yy) || (y4<=yy))
						if(y3!=y4)
						{
							xx=(int)((yy-y3)*mul4/div4)+x3;
							if(xx<minx) minx=xx;
							if(xx>maxx) maxx=xx;
						}
				if(minx<0)
					minx=0;
				if(maxx>screen_width-1)
					maxx=screen_width-1;
				intensity=((unit_vector[i].x*light.x+
					  unit_vector[i].y*light.y+
					  unit_vector[i].z*light.z+1)/2);
				color=(int)(intensity*16);
				color=RGB( 0,64-color,0);
				hline(minx,maxx,yy,color);
			}
		}
	}
}

void init_stars (void)
{
	int loop;
	for(loop=0;loop<num_stars;loop++)
	{
		stars[loop].x=rand()%screen_width-star_xoff;
		stars[loop].y=rand()%screen_height-star_yoff;
		stars[loop].z=loop+1;
	}

}

void calc_stars (void)
{
	int loop;
	for(loop=0;loop<num_stars;loop++)
	{
		new_pos[loop].x=(int)((stars[loop].x*256)/stars[loop].z)+star_xoff;
		new_pos[loop].y=(int)((stars[loop].y*256)/stars[loop].z)+star_yoff;
	}
}

void draw_stars (void)
{
	int loop;
	int color;
	for(loop=0;loop<num_stars;loop++)
	{
		if((new_pos[loop].x>0) && (new_pos[loop].x<screen_width) &&
		   (new_pos[loop].y>border_height) && (new_pos[loop].y<(screen_height-1-border_height)))
		{
			if(stars[loop].z>550)
			{
				 color = RGB( 40, 40, 40);
			}
			else
				if(stars[loop].z>350)
				{
					color = RGB( 80, 80, 80);
				}
				else
					if(stars[loop].z>150)
					{
						color = RGB(120,120,120);
					}
					else
					{
						color = RGB(160,160,160);
					}
			putPIXEL(new_pos[loop].x,new_pos[loop].y,color);
			old_pos[loop]=new_pos[loop];
		}
	}
}

void clear_stars (void)
{
	int loop;
	for(loop=0;loop<num_stars;loop++)
		putPIXEL(old_pos[loop].x,old_pos[loop].y,RGB(0,0,0));
}

void move_stars (void)
{
	int loop;
	for(loop=0;loop<num_stars;loop++)
	{
		stars[loop].z-=10;
		if(stars[loop].z<1)
			stars[loop].z+=num_stars;
	}
}

/*
void main (void)
{
	int i;
	clrscr();
	printf("Hi.  Well, this is the first code I have ever released.\n");
	printf("Nothing spectacular, just a rotating cube, light source\n");
	printf("shaded...hopefully some budding coder (like myself) will\n");
	printf("have some use for it.  I want to thank Grant Smith (Denthor)\n");
	printf("and Chris Mann (Snowman) for the VGA Trainer Series and\n");
	printf("Gooroo for WGT, your offerings to the demoscene have helped\n");
	printf("me immensely.  I don't know how fast this will be on lower\n");
	printf("end processors (it was coded on a 486dx4-100).  This is\n");
	printf("obviously coded in c and i'm sure can be made MUCH faster.\n\n");
	printf("Last but not least, thanks to everyone on #coders for all\n");
	printf("the help.\n\n");
	printf("Chris Hall-9/10/95\n");
	printf("fook@gate.net\n");
	printf("fook irc #coders\n\n");
	printf("Hit any key to continue...");
	do
	{
	} while(!kbhit());
	getch();
	setvga();
	randomize();
	init_stars();
	create_lookup_tables();
	setpal();
	draw_border();
	calc_stars();
	draw_stars();
	do
	{
		for(i=0;i<num_stars;i++)
			old_pos[i]=new_pos[i];
		xangle=(xangle+5)%360;
		yangle=(yangle+5)%360;
		zangle=(zangle+10)%360;
		rotate_object();
		draw_points();
		calc_normal();
		draw_poly();
		retrace();
		memcpy(screen,virt_screen,0xffff);
		memset(virt_screen,0,0xffff);
		calc_stars();
		draw_stars();
		move_stars();
		draw_border();
	} while(!kbhit());
	getch();
	farfree(virt_screen);
	settext();
}
*/