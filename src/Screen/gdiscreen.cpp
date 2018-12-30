#include <pch.h>
#include <window.h>
#include <screen\gdiscreen.h>
#include <debug.h>

void  GDIScreen::refresh ()  
{
//    mRefreshScr->lo_PutImage (mRX,mRY,mPicture,mReallyRefreshRect);
  crect_t&  r = mReallyRefreshRect;

  if (mDC && mWnd)
  {
    BitBlt (mWnd->GetDC (),mRX,mRY,r.width (),r.height (),mDC,r.left,r.top,SRCCOPY);  
  }  
}

GDIScreen::GDIScreen (Window* window)
          : fScreen32bit (window->width (),window->height (),NULL),
            mWnd         (window), mDC (NULL), mBitmap (NULL)
{
  int               w    = window->width (),
                    h    = window->height ();

  BITMAPINFOHEADER* info = (BITMAPINFOHEADER*)new char [sizeof (BITMAPINFOHEADER) +
                                                        w * h * 4 
                                                       ];

  info->biSize           = sizeof( BITMAPINFOHEADER );
  info->biWidth          = w;
  info->biHeight         = -h;
  info->biPlanes         = 1;
  info->biBitCount       = 32;
  info->biCompression    = BI_RGB;
  info->biSizeImage      = w * h * 4; //0

  info->biXPelsPerMeter  = 0;
  info->biYPelsPerMeter  = 0;
  info->biClrUsed        = 0;
  info->biClrImportant   = 0;    
  
  mDC                    = CreateCompatibleDC (window->GetDC ());
//  mBitmap                = CreateDIBSection   (mDC,  
  mBitmap                = CreateDIBSection   (window->GetDC (),  
                                               (LPBITMAPINFO)info,
                                               DIB_RGB_COLORS, 
                                               (void**)&mData, 
                                               NULL, 
                                               0 
                                              );

  mPicture->data         = (uchar*)mData;

  mReallyRefreshRect     = mRefreshRect;

  SelectObject (mDC,mBitmap);
                                             
  delete [] ((char*)info);
}

GDIScreen::~GDIScreen ()
{
  if (mDC)     ReleaseDC    (mWnd->wnd (),mDC);
  if (mBitmap) DeleteObject (mBitmap);
}
