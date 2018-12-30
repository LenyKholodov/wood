#ifndef __RENDER_LIGHT_ENVIRONMENT__
#define __RENDER_LIGHT_ENVIRONMENT__

#include <graph\grtypes.h>
#include <pool.h>
#include <misc.h>

#include "g_map.h"
#include "r_render.h"

const   int     TLE_LOCAL_LIGHT    = 1;
const   int     TLE_GLOBAL_LIGHT   = 0;
const   int     TLE_ENABLE_LIGHT   = 2;
const   int     TLE_DISABLE_LIGHT  = 0;

const   float   TLE_MAX_DIST       = 0.3f; //in percent form maxlenght

////////////////////////////////////////////////////////////////////////////////
///���㦥��� ��� �ᢥ饭��
////////////////////////////////////////////////////////////////////////////////
class TileLightEnv: public Render
{
  public:
                        TileLightEnv  (Pool*,LandscapeMap*,int _LightCount);
       virtual          ~TileLightEnv ();

       virtual bool     valid         () const;

////////////////////////////////////////////////////////////////////////////////
///����������\㤠�����\��������� ���筨�� ᢥ�
///��᫥ ���������� ���筨� �� 㬮�筨� ����祭
////////////////////////////////////////////////////////////////////////////////
         LIGHTSRC       NewLight      (const lightsrc_t&,int = TLE_LOCAL_LIGHT|TLE_ENABLE_LIGHT);
         LIGHTSRC       CreateLight   (const lightsrc_t& l,int flags = TLE_LOCAL_LIGHT|TLE_ENABLE_LIGHT) {
           return NewLight (l,flags);
         }
         void           DeleteLight   (LIGHTSRC);
         
         void           ChangeLight   (LIGHTSRC,const lightsrc_t&);

////////////////////////////////////////////////////////////////////////////////
///��ࠢ����� ��ࠬ��ࠬ� ���筨��
////////////////////////////////////////////////////////////////////////////////
         void           EnableLight   (LIGHTSRC); //if light present
         void           DisableLight  (LIGHTSRC);

         int            GetLight      (int _Tile,lightsrc_t*);
         BOOL           GetLight      (LIGHTSRC,lightsrc_t&);
         BOOL           IsEnabled     (LIGHTSRC li) { return mLightEnable  [li]; } 
         BOOL           IsPresent     (LIGHTSRC li) { return mLightPresent [li]; }

////////////////////////////////////////////////////////////////////////////////
///��।�񭭮� ���祭�� 梥� � ⠩��
////////////////////////////////////////////////////////////////////////////////
        vector3d_t      GetMidColor   (int _Tile);

////////////////////////////////////////////////////////////////////////////////
///���樠������ ��६����� ���ﭨ� OpenGL
////////////////////////////////////////////////////////////////////////////////
        void            Reset         ();
        void            Refresh       ();

////////////////////////////////////////////////////////////////////////////////
///��⠭������� ��ࠬ��஢ ���㦥���
///Result:
///   True:  ���ﭨ� ����������
///   False: ���ﭨ� ��⠫��� �०���
////////////////////////////////////////////////////////////////////////////////
            BOOL        SetLightEnv   (int _Tile,uint _InterruptDrawing = 0);

////////////////////////////////////////////////////////////////////////////////
///���ᮢ�� flares
////////////////////////////////////////////////////////////////////////////////
   virtual  void        Draw          ();

  protected:
   virtual void         DrawAlpha     (DWORD param,const Camera&);

  protected:
    struct tiledesc_t
    {
      int16        light;
      float        prior;
      tiledesc_t   *next, *prev;
      tiledesc_t*  nextDesc;  //free for free desc or next light desc for light list
      int          tile;
    };

    struct lightdesc_t
    {
      uchar        mode;
      uchar        hwlight;
      light_t      visLight;
      lightsrc_t   light;
      tiledesc_t*  firstTile;
      int          nextDesc; //next free light desc
    };    

    struct descindex_t
    {
      tiledesc_t  *first, *last;
      uint16      count;
    };

////////////////////////////////////////////////////////////////////////////////
///�㭪�� ���᫥��� �ਮ���
////////////////////////////////////////////////////////////////////////////////
            float       GetPrior      (const boundbox_t&,const lightsrc_t&);
            float       GetPrior      (float dist,float shiness) { 
                 return shiness / (mK [0] + mK [1] * dist + mK [2] * dist * dist);
            }

////////////////////////////////////////////////////////////////////////////////
///������� ����� �ᢥ饭��
////////////////////////////////////////////////////////////////////////////////
            void        ResetLight     (int lindex);
            void        RecalcLightMap (int lindex);
            tiledesc_t* ChangeTile     (int tile,int light,float prior);

            tiledesc_t* InsertLight    (tiledesc_t*,descindex_t&,int,float);
            void        DeleteLight    (tiledesc_t*);
   
////////////////////////////////////////////////////////////////////////////////
///���।������ ����ᮢ
////////////////////////////////////////////////////////////////////////////////
    int                 AllocLightSrc   ();
    void                FreeLightSrc    (int);
    tiledesc_t*         AllocTileDesc   ();    
    void                FreeTileDesc    (tiledesc_t*);

    typedef  Ring<int> LRing;

  protected:
    LandscapeMap*               mMap;
    FlagBit                     mFlags, mLightEnable, mLightPresent, mLightOn;
    lightdesc_t*                mLightDesc; 
    tiledesc_t*                 mTileDesc, *mFirstTileDesc;
    descindex_t*                mIndex;
    int*                        mOn;
    float                       mK [3], mMaxDist, mMinPrior;
    int                         mLocalCount, mLightNum, mVisNum;
    int                         mFirstLightDesc;
    LRing                       mGlobalLights;
};

#endif
