#ifndef __RENDER_HEIGHT_MAP_STRIPER__
#define __RENDER_HEIGHT_MAP_STRIPER__

#include "r_hmap.h"

class    HMapRender;

union hmedge_t
{
  ulong         flag;
  uchar         outside [4];
};

#define         HM_EDGE_X1_Y1   0xFF
#define         HM_EDGE_X1_Y2   0xFF00
#define         HM_EDGE_X2_Y2   0xFF0000
#define         HM_EDGE_X2_Y1   0xFF000000

////////////////////////////////////////////////////////////////////////////////
///Класс для построения непрерывного ландшафта
////////////////////////////////////////////////////////////////////////////////
class    HMapRenderList: public MemObject
{
  public:
                        HMapRenderList  (Pool*,HeightMap&,uint vcount,uint icount);
       virtual          ~HMapRenderList ();

////////////////////////////////////////////////////////////////////////////////
///ДОбавление FAN
////////////////////////////////////////////////////////////////////////////////
       BOOL             AddFAN          (ulong s,ulong c,ulong e,uint x,uint y,uint w,uint h,hmedge_t);  

       void             NextTile        (uint x,uint y);
       void             reset           ();

////////////////////////////////////////////////////////////////////////////////
///Доступ к массивам вершин и индексов
////////////////////////////////////////////////////////////////////////////////
       vertex_t*        vertex          () const        { return mVertex; }
       uint*            index           () const        { return mIndex;  }
       uint             count           () const        { return mIPos;   }

  protected:
////////////////////////////////////////////////////////////////////////////////
///Добавление вершины
////////////////////////////////////////////////////////////////////////////////
       int              AddVertex       (uint x,uint y,uchar* h,uint outside,float dx = 0,float dy = 0);

  private:
       HeightMap&       mMap; 
       int              **mXLine, **mYLine;
       int**            mTile, *mData;
       uint             mXTile, mYTile;
       uint             mXShift, mYShift;

       vertex_t*        mVertex;
       uint*            mIndex, mICount, mVCount, mVPos, mIPos;
};

#endif
