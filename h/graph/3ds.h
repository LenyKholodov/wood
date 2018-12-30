#ifndef __3DS_LOADER__
#define __3DS_LOADER__

#include <graph\x3f.h>

////////////////////////////////////////////////////////////////////////////////
///Вспомагательные структуры при загрузке 3DS
////////////////////////////////////////////////////////////////////////////////

extern BOOL     CONV3DS_CONVERT_ALPHA_TEXTURES; 

enum    ChunkId
{
  CHUNK_MAIN         = 0x4D4D, 
  CHUNK_OBJMESH      = 0x3D3D, 
  CHUNK_OBJBLOCK     = 0x4000, 
  CHUNK_TRIMESH      = 0x4100, 
  CHUNK_VERTLIST     = 0x4110, 
  CHUNK_FACELIST     = 0x4120, 
  CHUNK_FACEMAT      = 0x4130, 
  CHUNK_MAPLIST      = 0x4140, 
  CHUNK_TRMATRIX     = 0x4160, 
  CHUNK_CAMERA       = 0x4700, 
  CHUNK_MAT_ENTRY    = 0xAFFF, 
  CHUNK_MAT_NAME     = 0xA000, 
  CHUNK_TEXTURE      = 0xA200, 
  CHUNK_ALPHA_TEXTURE = 0xA210, 
  CHUNK_MAPFILE      = 0xA300,                               

  CHUNK_COLOR_F      = 0x0010,
  CHUNK_COLOR_24     = 0x0011,
  CHUNK_LIN_COLOR_24 = 0x0012,
  CHUNK_LIN_COLOR_F  = 0x0013,

  CHUNK_MAT_AMBIENT  = 0xA010,
  CHUNK_MAT_DIFFUSE  = 0xA020,
  CHUNK_MAT_SPECULAR = 0xA030,
  CHUNK_MAT_SHINESS  = 0xA040,

  CHUNK_INT_PERCENTAGE = 0x030,

  CHUNK_N_DIRECT_LIGHT = 0x4600,
  CHUNK_DL_SPOT_LIGHT  = 0x4610,
  CHUNK_DL_MULTIPLIER  = 0x465b,

  CHUNK_KF_DATA         = 0xB000,
  CHUNK_OBJECT_NODE_TAG = 0xB002,
  CHUNK_NODE_HDR        = 0xB010,
  CHUNK_NODE_ID         = 0xB030,
  CHUNK_PIVOT           = 0xB013,
  CHUNK_POS_TRACK_TAG   = 0xB020,
  CHUNK_ROT_TRACK_TAG   = 0xB021,
  CHUNK_SCL_TRACK_TAG   = 0xB022
};

enum SplineFlags
{
  TENSION  = 0,
  CONTINUITY,
  BIAS,
  EASE_TO,
  EASE_FROM
};

#pragma pack(push,1)
struct  chunk_t
{
  uint16        id;
  DWORD         len;  
};

struct  vertex3ds_t
{
  float x,z,y;
};

typedef vertex3ds_t vector3ds_t;

struct vertexlist3ds_t
{
  uint16        count;
  vertex3ds_t   data [];
};

struct  facet3ds_t
{          
  int16   v [3];
  int16   flags;
};

struct facetlist3ds_t
{
  uint16        count;
  facet3ds_t    data [];
};

struct textlist3ds_t
{
  uint16        count;
  textcoord_t   data [];            
};

struct matlist3ds_t
{
  char          name [MAT_NAME_LEN];
  uint16        count;
  int16         data [];
};

struct matrix3ds_t
{
  float         m [3][3];
  vertex3ds_t   offs;
};

typedef vecseg_t  vecseg3ds_t;
typedef quatseg_t quatseg3ds_t;

struct position3ds_t
{
  uint          frame;
  uint16        flags;
  float         spline [5];
  vector3ds_t   pos;
};

struct trackpos3ds_t
{
  struct
  {        
    uint16 flags;
    uchar  unknown[8];
    uint   num;
  } main;
  position3ds_t*  pos;
};

typedef  trackpos3ds_t trackscale3ds_t;

struct rotate3ds_t
{
  uint          frame;
  uint16        flags;
  float         spline [5];
  float         angle;
  vector3ds_t   ort;
};

struct trackrot3ds_t
{
  struct 
  {
    uint16 flags;
    uchar  unknown[8]; 
    uint   num;       
  }main;
  rotate3ds_t*   rot;
};

class   Trimesh3DS: public MemObject
{
  public:
    vertexlist3ds_t*        vertex;
    facetlist3ds_t*         facet;
    textlist3ds_t*          text;
    matrix_t*               trans;
    vector3d_t              offs;
    RingTmpl<matlist3ds_t*>  mat;

                          Trimesh3DS  (Pool*,BinFile*,size_t);
              virtual     ~Trimesh3DS ();

  protected:
    vertexlist3ds_t*      createVertexList  (BinFile*);
    facetlist3ds_t*       createFacetList   (BinFile*);
    textlist3ds_t*        createTextList    (BinFile*);
    matlist3ds_t*         createMatList     (BinFile*);
    matrix_t*             createTransMatrix (BinFile*);
};

class  DirectLight3DS: public MemObject
{
  public:
    vertex3ds_t         pos;
    vertex3ds_t         target;
    vector3d_t          color;
    float               exp;
    float               angle;

                        DirectLight3DS  (Pool*,BinFile*,size_t);
          virtual       ~DirectLight3DS ();
};

class  Anim3DS: public MemObject
{
  public:
        vecseg3ds_t*       trans;
        vecseg3ds_t*       scale;
        quatseg3ds_t*      rot;
        vector3d_t         pivot;

        int                rotCount, scaleCount, transCount;

                           Anim3DS  (Pool*);
          virtual          ~Anim3DS ();
};

class  Object3DS: public MemObject
{
  public:
    char                  name [128];
    int                   node;
    int                   parent;
    Trimesh3DS*           block;
    Anim3DS*              anim;
//    DirectLight3DS*       light;

                          Object3DS  (Pool*,int node,int parent);
                          Object3DS  (Pool*,BinFile*,size_t,Mesh3DS&);
             virtual      ~Object3DS ();
};

class Material3DS: public MemObject, public textmat_t
{
  public:
//        char    name [MAT_NAME_LEN];

                          Material3DS  (Pool*,BinFile*,size_t);
        virtual           ~Material3DS ();
};

class Node3DS: public MemObject
{
  public:
                        Node3DS  (Pool*,BinFile*,size_t,Mesh3DS&);
             virtual    ~Node3DS ();
};

class Mesh3DS: public MemObject
{
  public:
    RingTmpl<Object3DS*>       ring;            //Кольцо объектов
    RingTmpl<DirectLight3DS*>  light;           //Список источников света
    MatIndexDict               idict;           //Дерево имя материала-индекс
    MatDict                    dict;            //Дерево индекс материала-материал
    Dict<Object3DS*>           node;            //Дерево узлов
    int                        mcount;          //Количество материалов
    int                        acount;          //Количество кадров анимации

                               Mesh3DS  (Pool*,BinFile*,size_t);
                   virtual     ~Mesh3DS ();
};

class KFData3DS: public MemObject
{
  public:
                               KFData3DS  (Pool*,BinFile*,size_t,Mesh3DS*);
                   virtual     ~KFData3DS ();
};

#pragma pack (pop)

void   LoadASCIIZ     (BinFile*,char*);
void   LoadColorF     (BinFile*,vector3d_t&);
void   LoadColor24    (BinFile*,vector3d_t&);
void   LoadPercentInt (BinFile*,int16&);
void   ConvertAlpText (char*,const char*);

vecseg_t*   LoadVecAnim    (BinFile*,Pool*,int& count);
quatseg_t*  LoadQuatAnim   (BinFile*,Pool*,int& count);

#endif