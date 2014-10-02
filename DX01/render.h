/*

*/

#ifndef _RENDER_H_
#define _RENDER_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
// These four functions are our game!  .... Yup...and all our game data is in     //
// render.cpp... this file is mainly for initilisation.                           //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////

bool Create();                  //defined in render.cpp
void Release();
void Update();
void Render(unsigned int* pBits, int w, int h, int pitch);


#ifdef __cplusplus
}
#endif

#endif