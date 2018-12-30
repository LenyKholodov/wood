#ifndef __GAME_CACHE_TILE_MAP__
#define __GAME_CACHE_TILE_MAP__

#include "defs.h"
#include <pool.h>

////////////////////////////////////////////////////////////////////////////////
///���⨯ ��� ᮧ����� ���� � ⠩���� ���஢����� �� ��ਧ����
////////////////////////////////////////////////////////////////////////////////
class   CacheMap: public MemObject
{
  public:
                CacheMap   (Pool*);
                CacheMap   (Pool*       pool,
                            float       coord_map_width,
                            float       coord_map_height,
                            float       map_width,
                            float       map_height,
                            float       min_depth = 0
                           );

////////////////////////////////////////////////////////////////////////////////
///���樠������ (�㡫�஢���� ���������)
////////////////////////////////////////////////////////////////////////////////
            void                Init           (float       coord_map_width,
                                                float       coord_map_height,
                                                float       map_width,
                                                float       map_height,
                                                float       coord_min_depth
                                               );

////////////////////////////////////////////////////////////////////////////////
///���஢���� �����
////////////////////////////////////////////////////////////////////////////////             
            void                SetPos         (float x_coord,float y_coord);
            void                SetViewDepth   (float view_depth_in_coord);

  protected:
            struct box_t  {
              struct { float x1, y1, x2, y2; }   fl;   //� ��஢�� ���न����
              struct { int x1, y1, x2, y2;   }   i;    //� �८�ࠧ������� ���न����
            };         

    virtual void                reload        (const box_t&) = 0;
    virtual void                resize        (uint count) = 0;

  private:
    inline  void                clip          (box_t&);
    inline  void                create        (float,float,box_t&,float);

  protected:
     box_t              mBox;
     float              mCViewDepth, mCMaxViewDepth, mCMinViewDepth;
     float              mTViewDepth, mTMaxViewDepth, mTMinViewDepth;     
     float              mCWidth, mCHeight, mTWidth, mTHeight;
     uint               mTSize;
};

#endif