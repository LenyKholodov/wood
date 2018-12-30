#include <pch.h>
#include <service\ogl.h>
#include <kernel\thread.h>
#include <ogl.h>
#include <graph\glext.h>
#include <misc.h>
#include <gl\gl.h>

accessptr_t<OGLService> oglService;

#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"glu32.lib")
#pragma comment (lib,"gdi32.lib")

void    OGLService::ChoosePF (HDC hdc,uint mDisplayMode)
{
  D_Try
  {
    PIXELFORMATDESCRIPTOR pfd;
    int                   pixelFormat;    

    memset (&pfd,0,sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize       = sizeof (PIXELFORMATDESCRIPTOR);
    pfd.nVersion    = 1;
    pfd.dwFlags     = PFD_SUPPORT_OPENGL | PFD_SWAP_COPY;
    pfd.dwFlags    |= (mDisplayMode & OGL_DOUBLE)      ? 
                       PFD_DOUBLEBUFFER                : 
                       PFD_SUPPORT_GDI;
    pfd.dwFlags    |= (mDisplayMode & OGL_DRAW_BITMAP) ? 
                       PFD_DRAW_TO_BITMAP              : 
                       PFD_DRAW_TO_WINDOW;
    pfd.dwFlags    |= (mDisplayMode & OGL_STEREO) ? 
                       PFD_STEREO                      : 
                       0;
                           
    pfd.iPixelType  = (mDisplayMode & OGL_INDEX) ? PFD_TYPE_COLORINDEX : PFD_TYPE_RGBA;
    pfd.cColorBits  = (mDisplayMode & OGL_COLOR32) ? 
                       32                          : 
                      (mDisplayMode & OGL_COLOR24) ?
                       24                          :
                       16;
    pfd.cDepthBits  = (mDisplayMode & OGL_DEPTH24) ? 
                       24                          : 
                       16;
    pfd.cStencilBits= (mDisplayMode & OGL_STENCIL) ? 16 : 0;
    pfd.cAccumBits  = (mDisplayMode & OGL_ACCUM)   ? 16 : 0;
    pfd.cAlphaBits  = (mDisplayMode & OGL_ALPHA)   ? 
                      (mDisplayMode & OGL_COLOR32) ? 32 :
                      (mDisplayMode & OGL_COLOR24) ? 24 : 16 : 0;
    pfd.iLayerType  = PFD_MAIN_PLANE;

    pixelFormat = ChoosePixelFormat (hdc,&pfd);

    SetPixelFormat      (hdc,pixelFormat,&pfd);

    DescribePixelFormat (hdc,pixelFormat,sizeof (PIXELFORMATDESCRIPTOR),&pfd);

    #ifdef DEBUG
      if (pfd.dwFlags & PFD_GENERIC_FORMAT)
         dout<<"OGLService: No hardware acceleration "<<endl;
    #endif                            
  }
  D_Exc ("OGLService: Error at choosePF");
}

int  OGLService::handler  (Window* wnd,UINT msg,WPARAM wparam,LPARAM lparam,DWORD param)
{
  OGLService* view = (OGLService*)param;

  D_Try
  {
    switch (msg)
    {
      case WM_SIZE:
      {
        D_Try
        {
          if (view->mReshape) 
              (*view->mReshape)((GLsizei)LOWORD(lparam),
                                (GLsizei)HIWORD(lparam));
        }
        D_Exc ("OGLService: Error at Reshape () "<<view);

        return 0;
      }
      case WM_PAINT:
      {
        PAINTSTRUCT           ps;

        D_Try
        {
          BeginPaint (view->mWindow [view->mCurContext]->wnd(),&ps);
          if (view->mDraw)
            (*view->mDraw)();
          EndPaint   (view->mWindow [view->mCurContext]->wnd(),&ps);
        }
        D_Exc ("OGLService: Error at Draw () "<<view);

        return 0;
      }  
      default: return wnd->handler (msg,wparam,lparam);
    }
  }
  D_ExcRet ("OGLService: Error at handler () "<<view,0);
}

void        OGLService::Start              (uint param)
{
  D_Try
  {
    Window*     wnd   = mWindow [mCurContext];

    if (wnd)
    {
      if (param & OGL_RESHAPE_CALL)
      {
        RECT  rect;

        GetWindowRect      (wnd->wnd(),&rect);
        SetWindowPos       (wnd->wnd(),NULL,
                            rect.left,rect.top,rect.right-rect.left+1,rect.bottom-rect.top+1,
                            SWP_NOZORDER|SWP_SHOWWINDOW);

        wnd->InsertHandler (WM_PAINT,handler,(DWORD)this);
        wnd->InsertHandler (WM_SIZE,handler,(DWORD)this);

        SetWindowPos       (wnd->wnd(),NULL,
                            rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
                            SWP_NOZORDER|SWP_SHOWWINDOW);
      }

      if (param & OGL_DRAW_CALL)
        UpdateWindow       (wnd->wnd());
    }
  }
  D_Exc ("OGLService: Error at Start ");
}

BOOL        OGLService::CreateContext      (int index,Window* wnd,ChoosePFFn fn)
{
  D_Try
  {
    if (!wnd || index < 0 || index >= OGL_CONTEXT_NUM || !fn)
      return NULL;

    HDC         hdc   = wnd->GetDC ();

    if (mContext [index])
      DeleteContext (index);
   
    (*fn)(hdc,mDisplayMode);

    mContext [index] = wglCreateContext (mHDC [index] = hdc);
    mWindow  [index] = wnd;             

    if (mContext [index])
      for (int i=0;i<OGL_CONTEXT_NUM;i++)
        if (mContext [i] && i != index)
            wglShareLists (mContext [i],mContext [index]);

    if (index == mCurContext)
      MakeCurrentContext (index);

    return TRUE;
  }
  D_ExcRet ("OGLService: Error at CreateContext ",FALSE);
}

BOOL        OGLService::CreateContext      (int index,HDC hdc,ChoosePFFn fn)
{
  D_Try
  {
    if (!hdc || index < 0 || index >= OGL_CONTEXT_NUM || !fn)
      return NULL;

    if (mContext [index])
      DeleteContext (index);
   
    (*fn)(hdc,mDisplayMode);

    mContext [index] = wglCreateContext (mHDC [index] = hdc);
    mWindow  [index] = NULL;

    if (mContext [index])
      for (int i=0;i<OGL_CONTEXT_NUM;i++)
        if (mContext [i] && i != index)
            wglShareLists (mContext [i],mContext [index]);

    if (index == mCurContext)
      MakeCurrentContext (index);   

    return TRUE;
  }
  D_ExcRet ("OGLService: Error at CreateContext ",FALSE);
}

BOOL        OGLService::DuplicateContext   (int src,int dest)
{
  D_Try
  {
    if (!mContext [src])
      return FALSE;

    if (mContext [dest])
      DeleteContext (dest);

    mContext [dest] = wglCreateContext (mHDC [dest] = mHDC [src]);

    return TRUE;
  }
  D_ExcRet ("OGLService: Error at DuplicateContext ",FALSE);
}

BOOL        OGLService::DeleteContext      (int index)
{
  D_Try
  {
    if (index < 0 || index >= OGL_CONTEXT_NUM)
      return FALSE;

    if (mContext [index]) wglDeleteContext (mContext [index]);
    
    if (index == mCurContext) wglMakeCurrent (NULL,NULL);

    if (mWindow [index])
    {     
      D_Try
      {
        mWindow [index]->DeleteHandler (WM_PAINT);
        mWindow [index]->DeleteHandler (WM_SIZE);
      }
      D_Exception (EXCEPTION_EXECUTE_HANDLER)
      {
      }  
    }  

    mContext [index] = NULL;
    mWindow  [index] = NULL;
    mHDC     [index] = NULL;

    return TRUE;
  }
  D_ExcRet ("OGLService: Error at DeleteContext ",FALSE);
}

void        OGLService::DisplayMode        (uint param)
{
  mDisplayMode = param;
}

BOOL        OGLService::MakeCurrentContext (int index)
{
  D_Try
  {
    if (index < 0 || index >= OGL_CONTEXT_NUM)
      return FALSE;

    mCurContext = index;

    wglMakeCurrent (mHDC [index],mContext [index]);

    return TRUE;
  }
  D_ExcRet ("OGLService: Error at MakeCurrentContext ",FALSE);
}

void        OGLService::SwapBuffers        ()
{
  D_Try
  {
    ::SwapBuffers (mHDC [mCurContext]);
  }
  D_Exc ("OGLService: Error at SwapBuffers ");
}

void        OGLService::Reshape            (ReshapeFn ptr)
{
  mReshape = ptr;
}

void        OGLService::Draw               (DrawFn ptr)
{
  mDraw = ptr;
}

BOOL        OGLService::IsExtPresent       (const char* ext)
{
  D_Try
  {
    static  char  str [16384];
    static  char  src [1024];
    
    strcpy (str,(const char*)glGetString (GL_EXTENSIONS));
    strcpy (src,ext);

    locase (src,src);
    locase (str,str);

    if (!strstr (str,src))      return FALSE;
    else                        return TRUE;
  }
  D_ExcRet ("OGLService: Error at check extension present ",FALSE);
}

OGLService::OGLService  ()
        : Service     (&oglService),
          mReshape    (NULL), mDraw (NULL), 
          mCurContext (0), mDisplayMode (OGL_DEPTH | OGL_COLOR16 | OGL_DRAW_WINDOW)
{
  for (int i=0;i<OGL_CONTEXT_NUM;i++)
  {
    mContext [i] = NULL;
    mWindow  [i] = NULL;
    mHDC     [i] = NULL;
  }

//  InitExtensions ();
}

OGLService::~OGLService ()
{
  for (int i=0;i<OGL_CONTEXT_NUM;i++)
    DeleteContext (i+OGL_CONTEXT0);
}

////////////////////////////////////////////////////////////////////////////////

OGLService*     OGL_Load               ()
{
  return new OGLService;
}

void     OGL_Done               (OGLService* srv)
{
  if (srv) delete srv;
}

