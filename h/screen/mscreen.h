#ifndef __MEM_SCREEN_H__
#define __MEM_SCREEN_H__

#include <screen\screen.h>

/////////////////////////////////////////////////////////////////////////////////
///Экран в памяти
////////////////////////////////////////////////////////////////////////////////
class   memScreen: public Screen
{  
  public:
                       memScreen        (int,int,int,BOOL = TRUE);
                       memScreen        (image_t*);
           virtual     ~memScreen       ();

            void       GetRefreshRect   (crect_t&) const;
            void       SetRefreshRect   (int l,int t,int r,int b);

            void       SetRefreshPoint  (int x,int y);
            int        RefreshX         () const { return mRX;         } 
            int        RefreshY         () const { return mRY;         }

  virtual   Screen*    RefreshScreen    () const { return mRefreshScr; }
  virtual   BOOL       RefreshScreen    (Screen*);

  virtual   void       refresh          ();

  virtual   void*      ptr              () const { return mPicture ? mPicture->data : NULL; }

            image_t*   freeze           ();
            image_t*   picture          () const { return mPicture; } 

  protected:
    BOOL      mOwner;
    image_t*  mPicture;
    crect_t    mRefreshRect,  mReallyRefreshRect;
    Screen*   mRefreshScr;
    int       mRX,mRY;
};

#define mScreen32bit memScreen

#endif
