#ifndef __RENDER_CONVERTIONAL_UTILS__
#define __RENDER_CONVERTIONAL_UTILS__

#include <graph\x3f.h>
#include <misc.h>

#include "g_map.h"
#include "g_level.h"

typedef RingTmpl<int>             FacetRing;
typedef RingTmpl<facet_t>         PolyRing;
typedef RingTmpl<vertexbuffer_t>  VBRing;

////////////////////////////////////////////////////////////////////////////////
///Конвертер 3DS - Wood Map
////////////////////////////////////////////////////////////////////////////////
struct   mapnode_t;
class    GrMapConv: public MemObject
{
  public:       
                GrMapConv  (Pool*,Object3D&);
        virtual ~GrMapConv ();

                enum {
                     TEMP_DATA_SIZE = 4 * 1024 * 1024
                };

        void    SetCacheInfo  (int _CacheWidth,int _CacheHeight);
        void    SetVBInfo     (int _CacheWidth,int _CacheHeight);
        void    SetGrad       (int _Grad);
        void    SetSkyTexture (int,const char*);
        void    EnableStrip   (BOOL _State = FALSE) { mStripEnable = _State; }
        void    SetTLCacheSize(uint size) { mHWVertexCacheSize = size; }

////////////////////////////////////////////////////////////////////////////////
///Построение
////////////////////////////////////////////////////////////////////////////////
        void   compile        ();

////////////////////////////////////////////////////////////////////////////////
///Сохранение карты
////////////////////////////////////////////////////////////////////////////////
        void    Save       (GameLevel*);

  protected:
        void    calcBound  (boundbox_t&,float&);
        void    buildBound (int,boundbox_t&,float&);
        size_t  buildNode  (char*,nodeinfo_t&,mapnode_t&,int& meshIndex);
        int     computeFacets (mapnode_t&,char*,nodeinfo_t&,int* mtr);
        void    convert    (Object3D&);
        void    calcSize   (int& fSize,int& matSize);
        void    calcCross  (int,float);
        void    SplitVBLump (int,int,int,int,int*);

        void    saveMap    (GameLevel*);
        void    saveMeshes (GameLevel*,tileinfo_t*,nodeinfo_t*);
        void    saveSkyBox (GameLevel*);
        void    saveVB     (GameLevel*);
        void    saveMat    (GameLevel*);

  protected:
        facet_t*           mFacet;
        vertex_t*          mVertex;
        vector3d_t*        mNormal;
        textcoord_t*       mTextCoord;
        int                mFCount,mVCount,mNCount,mSize;
        int                mGrad;

        mapnode_t*         mNode;
        int                mWidth, mHeight;
        int                mCacheWidth, mCacheHeight;
        int                mVBCacheWidth, mVBCacheHeight;

        VBRing             mVBuffer;

        boundbox_t         mBox;
        float              mStep;
        Object3D*          mObj3d;

        BOOL               mStripEnable;
        uint               mHWVertexCacheSize;

        textmat_t          mTextNames [6];
};

#endif