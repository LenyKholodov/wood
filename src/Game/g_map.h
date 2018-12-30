#ifndef __G_MAP__
#define __G_MAP__

#include <binfile.h>
#include "g_cachemap.h"

////////////////////////////////////////////////////////////////////////////////
///��� ���� (�� ����襩 index = 0 � ����让 index = 64)
////////////////////////////////////////////////////////////////////////////////
struct heightstage_t {
  float         height [64];
};

////////////////////////////////////////////////////////////////////////////////
///���� ������
////////////////////////////////////////////////////////////////////////////////
class   LevelLoader;
class   LandMeshRender;
class   LandscapeCache;
class   LandscapeMap: public CacheMap
{
  protected:
       grtile_t*           map;                         //�����

  public:
       int                 cacheWidth, cacheHeight;     //������� ���-��אַ㣮�쭨��
       int                 vCount, fCount;              //������⢮ ����⮢
      
  public:
       int                 width, height;               //������� �����
       boundbox_t          bbox;                        //��࠭�稢��騩 ���-���
       float               step;                        //��� ����⭮� �⪨

  public:
                LandscapeMap   (Pool*,LevelLoader&,size_t land_cache_size);
       virtual  ~LandscapeMap  ();

////////////////////////////////////////////////////////////////////////////////
///���४⭮���
////////////////////////////////////////////////////////////////////////////////
       virtual bool valid      () const { return CacheMap::valid () && map && IsValid ((MemObject*)mCache); }

////////////////////////////////////////////////////////////////////////////////
///���஢���� �����
////////////////////////////////////////////////////////////////////////////////             
            void                SetPos         (const vector3d_t& p) { CacheMap::SetPos (p.x,p.z); }

            grtile_t&           tile  (int,int);
    inline  int                 index (int x,int y) { return (y-mBox.i.y1) * (mBox.i.x2-mBox.i.x1+1) + x - mBox.i.x1; }
    inline  grtile_t&           tile  (int index)   { return map [index]; }

            LandscapeCache*     cache ()  { return mCache; }

////////////////////////////////////////////////////////////////////////////////
///��।������ �����
////////////////////////////////////////////////////////////////////////////////
            uint                CalcHeight    (const vertex_t&,heightstage_t&);
            float               CalcMaxHeight (const vertex_t&);
            float               CalcMinHeight (const vertex_t&);

  protected:
    virtual void                reload        (const box_t&);
    virtual void                resize        (uint);

  private:
     BinFile*           mFile;
     LandscapeCache*    mCache;
};

#endif