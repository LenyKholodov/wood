#include <pch.h>
#include <screen\mscreen.h>
#include <debug.h>
#include <misc.h>

//template <class T> inline T max(T a,T b) { return a>b?a:b; }
//template <class T> inline T min(T a,T b) { return a<b?a:b; }

void memScreen::GetRefreshRect(crect_t& cr) const
{
  cr = mRefreshRect;
}

void memScreen::SetRefreshRect(int l,int t,int r,int b)
{
  mRefreshRect.setup(max(0,l),max(0,t),r,b);  
  if (::IsValid (mRefreshScr))
    mReallyRefreshRect.setup(max(0,mRefreshRect.left+mRX),
                             max(0,mRefreshRect.top+mRY),
                             min(mRefreshRect.right+mRX,mRefreshScr->getmaxx()),
                             min(mRefreshRect.bottom+mRY,mRefreshScr->getmaxy()));
}

void memScreen::SetRefreshPoint(int x,int y)
{
  mRX = x, mRY = y;

  if (::IsValid (mRefreshScr))
    mReallyRefreshRect.setup(max(0,mRefreshRect.left+mRX),
                             max(0,mRefreshRect.top+mRY),
                             min(mRefreshRect.right+mRX,mRefreshScr->getmaxx()),
                             min(mRefreshRect.bottom+mRY,mRefreshScr->getmaxy()));
}

BOOL memScreen::RefreshScreen(Screen* scr)
{
  if (!::IsValid (scr))
     return FALSE;

  mRefreshScr = scr;
  mReallyRefreshRect.setup(max(0,mRefreshRect.left+mRX),
                           max(0,mRefreshRect.top+mRY),
                           min(mRefreshRect.right+mRX,mRefreshScr->getmaxx()),
                           min(mRefreshRect.bottom+mRY,mRefreshScr->getmaxy()));

  return TRUE;   
}

void memScreen::refresh(void)
{
  if (::IsValid (mRefreshScr))
    mRefreshScr->lo_PutImage (mRX,mRY,mPicture,mReallyRefreshRect);
}

image_t* memScreen::freeze ()
{
  mOwner = FALSE; 
  return mPicture;
}

memScreen::memScreen (image_t* img)
              : Screen   (img->width,img->height,img->bpp),
                mPicture (img), 
                mOwner   (FALSE),
                mRX      (0), mRY (0), 
                mRefreshScr (NULL)
{
  mRefreshRect.setup  (0,0,img->width-1,img->height-1);
}

memScreen::memScreen (int w,int h,int bpp,BOOL fullCreate)
          : mOwner (TRUE),
            mRX      (0), mRY (0), 
            mRefreshScr (NULL),
            mPicture    (fullCreate ? image_t::create (w,h,bpp) :
                                      image_t::create (w,h,bpp,NULL)),
            Screen   (w,h,bpp)
{
  mRefreshRect.setup  (0,0,w-1,h-1);  
}

memScreen::~memScreen  ()
{
  if (mOwner && mPicture)
    image_t::dispose (mPicture);
}

