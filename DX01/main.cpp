/**********************************************************************************/
/*                                                                                */
/* File:   main.cpp                                                               */
/* Author: bkenwright@xbdev.net                                                   */
/* web:    www.xbdev.net                                                          */
/*                                                                                */
/**********************************************************************************/
/* 


*/
/**********************************************************************************/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <d3dx9.h>	// DirectX header files
#include "render.h"	//

#pragma comment(lib, "D3d9.lib")	// DirectX 9 library's
#pragma comment(lib, "D3dx9.lib")


#define CLIENT_WIDTH    800
#define CLIENT_HEIGHT   600
#define RELEASE(o)	if (o){o->Release();o=NULL;}


// These are our DirectX3D global variables.
LPDIRECT3D9			g_pD3D	= NULL;	// Used to create the D3DDevice
LPDIRECT3DDEVICE9	g_pd3dDevice	= NULL;	// Our rendering device



// forward declaration:
void ClientResize(HWND hWnd, int nWidth, int nHeight);

void ClientResize(HWND hWnd, int nWidth, int nHeight)
{
	RECT rcClient, rcWindow;
	POINT ptDiff;

	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWindow);
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	MoveWindow(hWnd,rcWindow.left, rcWindow.top, nWidth + ptDiff.x, nHeight + ptDiff.y, TRUE);
}

// This is nothing more than a feedback function, that displays to the user
// how to quit from the function, and how many frames per second are happening.
void DisplayFeedBack()
{
	static int iFramesPerSecond = 0;
	static int iTempFramesCounter = 0;

	unsigned int dwTicksNow = GetTickCount();
	static unsigned int dwPastTicks = 0;
	if( dwPastTicks < dwTicksNow )
	{
		dwPastTicks = dwTicksNow + 1000; // 1000 milliseconds = 1 second
		iFramesPerSecond = iTempFramesCounter;
		iTempFramesCounter = 0;
	}
	iTempFramesCounter++;

	ID3DXFont* aFont;

/*
	LOGFONT lfont={
			16, //height
			0,  //width; 
			0,  // lfEscapement; 
			0,  //lfOrientation; 
			FW_BOLD, // lfWeight; 
			FALSE, // lfItalic; 
			FALSE, // lfUnderline; 
			FALSE, // lfStrikeOut; 
			DEFAULT_CHARSET, // lfCharSet; 
			OUT_DEFAULT_PRECIS, //lfOutPrecision; 
			CLIP_DEFAULT_PRECIS, // lfClipPrecision; 
			ANTIALIASED_QUALITY,// lfQuality; 
			DEFAULT_PITCH,// lfPitchAndFamily; 
			"Arial"// lfFaceName[LF_FACESIZE]; 
			};
*/
	D3DXFONT_DESC lfont={
			16,
			0,
			400,	// Weight;
			0,	// MipLevels;
			false,	// Italic;
			DEFAULT_CHARSET,	// CharSet;
			OUT_TT_PRECIS,	// OutputPrecision;
			CLIP_DEFAULT_PRECIS,	// Quality;
			DEFAULT_PITCH,	// PitchAndFamily;
			"Arial"		// FaceName;
		};

	//This is all there is to creating a D3DXFont.  Hmmmm....
	D3DXCreateFontIndirect( g_pd3dDevice ,&lfont,&aFont );

	char szbuff[200];
	wsprintfA(szbuff, "Press Escape to Exit\n Frames Per Second: %d\n", iFramesPerSecond);
	RECT r = {0,0,400,200};
	aFont->DrawTextA(NULL,
						szbuff,
						-1,
						&r,
						DT_LEFT,
						0xFFBBBBFF);
	aFont->Release();
}
////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
// Well this function only locks our directx surface (e.g. the backbuffer)        //
// and we will draw on it.                                                        //
// Also a note for the wiser! If your going to lock and write to the back         //
// buffer you need to tell directx you are going to do this when you create       //
// your surface with "D3DPRESENTFLAG_LOCKABLE_BACKBUFFER" in the                  //
// D3DPRESENT_PARAMETERS...for example:                                           //
//                                                                                //
// D3DPRESENT_PARAMETERS d3dpp={0,0, d3ddm.Format, 1, (D3DMULTISAMPLE_TYPE)0,     //
//						D3DSWAPEFFECT_DISCARD, hwnd,TRUE,TRUE,                    //
//						D3DFMT_D16, D3DPRESENTFLAG_LOCKABLE_BACKBUFFER, 0,0 };    //
//                                                                                //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////
/*
Direct3D Surfaces (Direct3D 9)
Accessing Surface Memory Directly (Direct3D 9)
*/
void DirectRender(IDirect3DSurface9** pSurface)
{
	D3DSURFACE_DESC	pDesc;
	(*pSurface)->GetDesc(&pDesc);

	int iWidth  = pDesc.Width; 
	int iHeight = pDesc.Height;

	D3DLOCKED_RECT	d3dLockedRect;
	::ZeroMemory(&d3dLockedRect, sizeof(d3dLockedRect));

	(*pSurface)->LockRect(&d3dLockedRect, NULL, D3DLOCK_NOSYSLOCK);

	unsigned int *pSurfBits = static_cast<unsigned int*>(d3dLockedRect.pBits);

	Render(pSurfBits, iWidth, iHeight, d3dLockedRect.Pitch );

	(*pSurface)->UnlockRect();

} // End of DirectRender(..)

void RenderToBackBuffer()
{
	if(!g_pd3dDevice)return; 

	Update();

	// Get our back buffer, and draw on it!
	IDirect3DSurface9* pBackBuffer;
	g_pd3dDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);

	DirectRender(&pBackBuffer);

	g_pd3dDevice->BeginScene();    
	DisplayFeedBack(); // Text feedback on the screen
	g_pd3dDevice->EndScene();

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

/*
	if(GetKeyState(VK_ESCAPE) & 0x80)
		PostQuitMessage( 0 );
*/
}

////////////////////////////////////////////////////////////////////////////////////
//                                                                                \\
//  InitD3D is called once at the start of our program, and CleanUpDX is called   \\
//  once at the end of the program before closing.                                \\
//                                                                                \\
////////////////////////////////////////////////////////////////////////////////////

// Delete any allocated memory etc here.
void Cleanup()
{
	// Release DirectX3D resources.
	g_pd3dDevice->Release();
	g_pD3D->Release();
}

/*
 Initializes Direct3D at the start.
Return:
	nonzero if an error occurred.
*/
HRESULT InitD3D(HWND hwnd, bool bFullScreen)
{
	bool bInAWindow = !bFullScreen;  // false for full screen, true for in a window.

	g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	D3DDISPLAYMODE d3ddm;
	g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

	int iWidth  = CLIENT_WIDTH; // Simply set to zero for windowed!..NULL...nothing...you get the idea :)
	int iHeight = CLIENT_HEIGHT;
                                  
	D3DPRESENT_PARAMETERS d3dpp={iWidth,iHeight, d3ddm.Format, 1, (D3DMULTISAMPLE_TYPE)0, 0, 
						D3DSWAPEFFECT_DISCARD, NULL, bInAWindow, TRUE, D3DFMT_D16, D3DPRESENTFLAG_LOCKABLE_BACKBUFFER, 0,0 };


	HRESULT e = g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
						D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice );
	
	//D3D_OK = S_OK = 0;
    return	e;
}


////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////
//                                                                                \\
//  Our Windows Code....this is the windows code...I cramed it together so that   \\
//  it has its bare essentials...couldn't get it any smaller than this...so that  \\
//  we could concentrate at the heart of our coding....Octee and not the windows  \\
//  code.                                                                         \\
//                                                                                \\
////////////////////////////////////////////////////////////////////////////////////

LRESULT WINAPI MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch( uMsg )
	{
		case WM_PAINT:    //Repaint our window
			hdc=BeginPaint(hWnd,&ps);
			EndPaint(hWnd,&ps);
			break;
		case WM_CREATE:
			ClientResize(hWnd,CLIENT_WIDTH,CLIENT_HEIGHT);
			Create();
				
			break;
        case WM_DESTROY:
			// Clean up everything and exit the app
			Release();
			Cleanup();
            PostQuitMessage( 0 );
            break;
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_ESCAPE:
				{
					SendMessage(hWnd, WM_DESTROY, wParam, lParam);	
				}
				break;
			}
			break;
		default:
			return DefWindowProc (hWnd, uMsg, wParam, lParam);
		break;
    }

    return	0;
}


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, INT nCmdShow)
{
	bool bFullScreen = true;
	// Be a good app, and ask the user if we want full screen or in a window.
	if(MessageBox(NULL, "Click Yes to go to full screen", "Options", MB_YESNO | MB_ICONQUESTION) == IDNO)
		bFullScreen = false;

	// Register the window class
	char szName[] = "OBJSTAR - Chris Hall";	// "www.xbdev.net";
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
					GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
					szName, NULL };

	RegisterClassEx( &wc );

	HWND hWnd;

	if(bFullScreen)
	{
	
		DEVMODE dmSettings;
		EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings);
		dmSettings.dmPelsWidth	= CLIENT_WIDTH;
		dmSettings.dmPelsHeight	= CLIENT_HEIGHT;
		ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);

		hWnd=CreateWindow( szName, szName, 
							WS_POPUP, 0, 0, 
							CLIENT_WIDTH, CLIENT_HEIGHT,
							NULL, NULL, wc.hInstance, NULL );

	}
	else
	{
		// Create the application's window
		hWnd=CreateWindow( szName, szName, 
                              WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 
							  8+CLIENT_WIDTH, 27+CLIENT_HEIGHT, // The reason that we have these extra values
												// for the width and height is because of the windows
												// border!  As the client rect is now 480x640, but we
												// have a caption bar etc...which is the extra size.
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );
	}


	// Initialize Direct3D
	bool e = InitD3D( hWnd, bFullScreen );
	if( e )
		return 0;

    
	// Show the window
	ShowWindow( hWnd, SW_SHOWDEFAULT );
	UpdateWindow( hWnd );
	SetFocus(hWnd);

	// Enter the message loop
	MSG Msg; 
	ZeroMemory( &Msg, sizeof(Msg) );
	while( Msg.message!=WM_QUIT )
	{
		if( PeekMessage( &Msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &Msg );
            DispatchMessage( &Msg );
		}
        else
			RenderToBackBuffer();
	}

	
	if(bFullScreen)
	{
		ChangeDisplaySettings(NULL,0); // If we went full screen, we restore it back to a window
	}

	UnregisterClass( szName, wc.hInstance );
	return 0;
}



