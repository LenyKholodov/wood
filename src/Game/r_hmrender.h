#ifndef __RENDER_HEIGHT_MAP_RENDER__
#define __RENDER_HEIGHT_MAP_RENDER__

#include "r_hmap.h"
#include "r_hmlist.h"
#include "r_render.h"

////////////////////////////////////////////////////////////////////////////////
///Рендеринг по карте высот
////////////////////////////////////////////////////////////////////////////////
class    HMapRender: public Render
{
  friend class HMapRenderList;
  public:
                HMapRender (Pool*,MainRender*,int draw_depth = RENDER_HM_DRAW_DEPTH);
      virtual   ~HMapRender();

////////////////////////////////////////////////////////////////////////////////
///Отрисовка
////////////////////////////////////////////////////////////////////////////////
      virtual    void     Draw      ();

////////////////////////////////////////////////////////////////////////////////
///Установка глубины отрисовки
////////////////////////////////////////////////////////////////////////////////
                 void     SetDrawDepth (int);

  private:
                 void     DrawTile  (int x,int y,ulong,ulong,int w,int h,int levels);
                 uchar    height    (uchar*,int,int);
                 void     CalcList  (const Camera&);
                 void     BuildMat  ();  

  protected:
                 void     DrawList  (uint count,uint*,vertex_t*);

  private:
        HeightMap           mMap;        
//        HMapRenderList      mList;
        LandscapeMap&       mLandMap;

               struct       tilenode_t;
               struct       textnode_t;

        textnode_t*         mText;
        tilenode_t*         mTileNode;
        int                 mTilePos, mTileNum, mTextFirst;

        MATERIAL*           mMatReindex;
        MatEnvX*            mMatEnv;

        int*                mLevel, mLevelCount;
};

#endif