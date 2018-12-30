#include <pch.h>
#include <render\viewport.h>
#include <render\render.h>

Matrix& Viewport::GetTransform ()
{
  if (mFlags.check (VP_CHANGE_TOTAL) ||
      mProj.IsChange () || (mCamera && mCamera->IsChange ()))
  {  
      //произошли изменения

    mFlags.clear (VP_CHANGE_TOTAL);

    if (mCamera) 
    {
      mTotal = mCamera->GetView ();
      mProj.GetFrustum (mTotal);
    }  
    else  
    {
      mProj.GetProjection (mTotal);
    }  

  }

  return mTotal;
}

bool Viewport::SetCurrent ()
{
  float kx = float (rScreenWidth)  / float (mScreenWidth),
        ky = float (rScreenHeight) / float (mScreenHeight);

  glViewport   (float (mViewLeft) * kx,float (mViewBottom) * ky,
                float (mViewWidth) * kx,float (mViewHeight) * ky
               );

  if (mFlags.check (VP_ENABLE_CLIP_RECT))
  {
    glEnable  (GL_SCISSOR_TEST);
    glScissor (float (mClipLeft) * kx,float (mClipBottom) * ky,
               float (mClipWidth) * kx,float (mClipHeight) * ky
              );
  }
  else
    glDisable (GL_SCISSOR_TEST);
 
  const GLfloat* m = (const GLfloat*)&mProj.GetProjection ();

  glMatrixMode              (GL_PROJECTION);
  glLoadTransposeMatrixfARB (m);

  glMatrixMode              (GL_MODELVIEW);
    
  if (mCamera)
  {
    const GLfloat* m = (GLfloat*)&mCamera->GetView ();
    
    glLoadTransposeMatrixfARB (m);
  }
  else
  {
    glLoadIdentity ();
  }

  return true;
}

bool Viewport::BindCamera (Camera& cam)
{
  if (mCamera)
    UnbindCamera ();

  if (cam.mViewport)
    cam.mViewport->UnbindCamera ();

  cam.mViewport = this;
  mCamera       = &cam;

  SetChangeState (true);
  mFlags.set     (VP_CHANGE_TOTAL);
}

void Viewport::UnbindCamera ()
{
  if (mCamera)
  {
    if (mCamera->mViewport == this)
      mCamera->mViewport = NULL;

    mCamera = NULL;

    SetChangeState (true);
    mFlags.set     (VP_CHANGE_TOTAL);
  }
}

void Viewport::SetScreenSize (int width,int height)
{
  mScreenWidth  = width;
  mScreenHeight = height;

  SetChangeState (true);  
}

void Viewport::SetViewport (int left,int bottom,int width,int height)
{
  mViewLeft   = left;
  mViewBottom = bottom;
  mViewWidth  = width;
  mViewHeight = height;

  SetChangeState (true);  
}

void Viewport::GetViewport (int& left,int& bottom,int& width,int& height) const
{
  left    = mViewLeft;
  bottom  = mViewBottom;
  width   = mViewWidth;
  height  = mViewHeight;  
}

void Viewport::SetViewport (int width,int height)
{
  mViewWidth  = width;
  mViewHeight = height;

  SetChangeState (true);  
}

void Viewport::SetClipRect (int left,int bottom,int width,int height)
{
  mClipLeft   = left;
  mClipBottom = bottom;
  mClipWidth  = width;
  mClipHeight = height;

  SetChangeState (true);  
}

void Viewport::GetClipRect (int& left,int& bottom,int& width,int& height) const
{
  left   = mClipLeft;
  bottom = mClipBottom;
  width  = mClipWidth;
  height = mClipHeight;
}

void Viewport::EnableClip ()
{
  mFlags.set     (VP_ENABLE_CLIP_RECT);
  SetChangeState (true);  
}

void Viewport::DisableClip ()
{
  mFlags.clear   (VP_ENABLE_CLIP_RECT);
  SetChangeState (true);  
}

bool Viewport::GetClipState () const
{
  return mFlags.check (VP_ENABLE_CLIP_RECT);
}

void Viewport::SetClipState (bool state)
{
  if (state)  EnableClip  ();
  else        DisableClip ();
}

void Viewport::Move (int x_offs,int y_offs)
{
  mViewLeft    += x_offs;
  mViewBottom  += y_offs;

  SetChangeState (true);
  mFlags.set     (VP_CHANGE_TOTAL);
}

void Viewport::Scale (float x_scale,float y_scale)
{
  mViewWidth  = float (mViewWidth)  * x_scale;
  mViewHeight = float (mViewHeight) * y_scale;

  SetChangeState (true);
  mFlags.set     (VP_CHANGE_TOTAL);
}

Viewport::Viewport ()
         : mTotal (1)
{
  mCamera = NULL;  

  SetScreenSize  (1000,1000);
  SetViewport    (0,0,1000,1000);
  SetClipRect    (0,0,1000,1000);

  mFlags.set     (VP_CHANGE_TOTAL);

  SetLoadState   (true);
  SetChangeState (true);
}

Viewport::~Viewport ()
{
  UnbindCamera ();
}
