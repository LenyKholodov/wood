#ifndef __OPENGL_UNIT_SUPPORT__
#define __OPENGL_UNIT_SUPPORT__

#include <window.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <service\ogl.h>

extern accessptr_t<OGLService>    oglService;

OGLService*     OGL_Load       ();
void            OGL_Done       (OGLService* = oglService);

#define OGL_CreateContext      oglService->CreateContext
#define OGL_DiplicateContext   oglService->DuplicateContext
#define OGL_DeleteContext      oglService->DeleteContext
#define OGL_MakeCurrent        oglService->MakeCurrentContext 
#define OGL_DisplayMode        oglService->DisplayMode
#define OGL_ReshapeFunc        oglService->Reshape
#define OGL_DrawFunc           oglService->Draw
#define OGL_SwapBuffers        oglService->SwapBuffers
#define OGL_Start              oglService->Start
#define OGL_IsExtPresent       oglService->IsExtPresent

#endif