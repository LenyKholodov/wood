#ifndef __GDI_SCREEN__
#define __GDI_SCREEN__

#include <screen\fscreen.h>

class    Window;

////////////////////////////////////////////////////////////////////////////////
///��࠭ �ਢ易��� � ����
////////////////////////////////////////////////////////////////////////////////
class    GDIScreen: public fScreen32bit
{
  public:
                GDIScreen  (Window*);
//                GDIScreen  (Window*,const crect_t&);
    virtual     ~GDIScreen ();

////////////////////////////////////////////////////////////////////////////////
///����������
////////////////////////////////////////////////////////////////////////////////
    virtual     void  refresh          ();

  protected:
    virtual     BOOL  RefreshScreen    (Screen*) { return FALSE; }

  private:
        HBITMAP         mBitmap;
        HDC             mDC;
        Window*         mWnd;
};

#endif