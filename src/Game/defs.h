#ifndef __WOOD_ID_DIN_MAN__
#define __WOOD_ID_DIN_MAN__

#include <types.h>
#include <graph\grtypes.h>

////////////////////////////////////////////////////////////////////////////////
///Handle types
////////////////////////////////////////////////////////////////////////////////

typedef  int        MODEL;
typedef  int        MODELDATA;
typedef  int        MODELKEEPER;

typedef  int        LIGHTSRC;

typedef  int        SHADOW;

typedef  int        TREEGROUP;
typedef  int        TREE;

typedef  int        MATERIAL;
typedef  int        MATGROUP;

////////////////////////////////////////////////////////////////////////////////
///Type defines
////////////////////////////////////////////////////////////////////////////////

////////////////////////////��������� ����ᮢ//////////////////////////////////

struct   vbheader_t
{
  uint          count;          //������⢮ VertexBuffer
  size_t        vbSize;         //������ VertexBuffer
  ulong         offs;           //���饭�� ��ࢮ�� VertextBuffer
};

struct  grmapheader_t
{
  int           width, height;                  //������� �����
  int           vCount, fCount;                 //���-�� ���設\�࠭��
  boundbox_t    bbox;                           //��࠭�稢���� ��஡��
  float         step;                           //��� �⪨
  int           cacheWidth, cacheHeight;        //������� ��אַ㣮�쭨�� ���஢����
};

struct  grcacheheader_t
{
  size_t   minMeshSize,                         //��������� ࠧ��� ���
           maxMeshSize;                         //���ᨬ����� ࠧ��� ���
  int      meshNum;                             //������� ��襩
  int      mapSize;                             //������ �����: width * height
  ulong    tablesOffs;                          //���饭�� ⠡��� (����� 䠩��)
  int      vCount;                              //������ ���設���� ����
};

struct  landmatheader_t
{
  ulong         count;          //������⢮ ���ਠ���
  size_t        strsize;        //������ ��ப� � �������
};

struct  hmapheader_t
{
  char          mname [128],      //��� 䠩�� (NONAME = map include)
                tname [128];      
  int           width, height;    //������� �����
  int           twidth,theight;   //������� ⠩��
  int           text_twidth,      //������� ⠩�� ⥪�����
                text_theight;
  float         kx,ky,kz;         //�����樥��� ����⠡�஢����
  ulong         infoOffs;         //���饭�� �� ⠡���
};

////////////////////////////////////////////////////////////////////////////////
///�᭮���� ����� ��।�� ��㣮�쭨���
///VERTEX_LIST: normal3f, textcoord2f, vertex3f
////////////////////////////////////////////////////////////////////////////////

#ifndef __RENDER_MESH__
#define __RENDER_MESH__

#pragma pack (push)
#pragma pack (4)

struct  mesh_t
{
  size_t        size;       //������ ���
  uint16        type;       //��� (GL_TRIANGLES, GL_TRIANGLE_STRIP, VERTEX_LIST)
  uint16        vbuffer;    //������ VertexBuffer'�
  uint16        elType;     //��� ����� (uint,uint16,byte,vertex,etc.)
  uint          start, end; //������ ��砫쭮� � ����筮� ���設
  uint          index;      //������ ���
  uint          count;      //������⢮ ����⮢ � ���
  int           material;   //���ਠ� ���
  char          data [];    //�����
};

struct   multimesh_t
{
  int           num;        //������⢮ ��襩
  mesh_t*       mesh () const { return (mesh_t*)((char*)this+sizeof (multimesh_t)); }
};

#pragma pack (pop)

#endif

////////////////////////////////////////////////////////////////////////////////
///���ᠭ�� ���筨�� ᢥ�
////////////////////////////////////////////////////////////////////////////////
struct  lightsrc_t
{
  light_t      light;
  float        shiness;
};

////////////////////////////////////////////////////////////////////////////////
///���ᠭ�� ���設���� ����
////////////////////////////////////////////////////////////////////////////////
struct  vertexbuffer_t
{
  int           count,          //������⢮
                pos,            //������ ����������
                first;          //������ ��ࢮ� ���設�

  vertex_t*     vertex;         //���設�
  textcoord_t*  tcoord;         //�������� ���न����
  vector3d_t*   normal;         //��ଠ��
};

struct  visnode_t
{
  int   index, detail;
  int   x,y;
};

struct  sprite_t
{
  int   width, height;
  int   texture;  
};

enum NodeChunkId {
  GRC_FIRST       = 0,
  GRC_FACET_PATCH = 0,
  GRC_STRIP,

  GRC_NUM
};

struct  tileinfo_t {
  size_t        size;           //������ node
  ulong         offs;           //���饭�� node � 䠩��

  struct coord_t {
    int16 x,y;
  }coord;  
};

struct  nodeinfo_t {
  struct chunk_t {
    size_t        size;         //������ ��᪠
    ulong         offs;         //���饭�� ��᪠ (�⭮�⥫쭮 ��砫� node)

    uint          count;        //������⢮ ����⮢
    int           first;        //������ ��ࢮ�� �����
  }chunk [GRC_NUM];  
};

struct  grtile_t
{
  boundbox_t    box;          //��࠭�稢��騩 �騪
  
  tileinfo_t    tile;         //���ଠ�� � ⠩��
  nodeinfo_t    node;  

  int           index;
};

struct  facetpatch_t
{
  int           count;              //������⢮ ��㣮�쭨���
  int           vbuffer;            //���設�� ����
  size_t        size;               //������ ��㯯�            
  facet_t       data  [];           //��㣮�쭨��
};

struct  hmtile_t
{
  boundbox_t    box;                //��஡��, ����� ⠩�� height map'� 
  int           texture;            //������� ⠩��
  int           index;              //������ ⠩��
  int           x,y;                //���न���� ��砫� ⠩��

  ulong         sOffs, eOffs;       //���饭��
};

////////////////////////////////////////////////////////////////////////////////
///Function defines
////////////////////////////////////////////////////////////////////////////////

#define  RENDER_TEXT_SET 1
#define  RENDER_MAT_SET  2

typedef  BOOL (*SetMaterialFn)(DWORD,int _material,int mode = RENDER_TEXT_SET | RENDER_MAT_SET);
typedef  BOOL (*ChangeEnvFn)(DWORD,uint _tile,uint _InterrupDrawing = 0);

////////////////////////////////////////////////////////////////////////////////
///Var defines
////////////////////////////////////////////////////////////////////////////////

extern float  RENDER_VIEW_ANGLE;
extern float  RENDER_VIEW_DEPTH;
extern int    RENDER_LR_CACHE_SIZE;

extern int    RENDER_STATIC_TEXTURES_CACHE_SIZE;
extern int    RENDER_DINAMIC_TEXTURES_CACHE_SIZE;
extern int    RENDER_LAND_TEXT_CACHE;
extern int    RENDER_TREES_TEXT_CACHE;
extern int    RENDER_GLOBAL_TEXTURES_CACHE_NUM;

extern int    RENDER_MESH_NUM;
extern int    RENDER_MESH_BUFFER_SIZE;

extern BOOL   RENDER_GL_FINISH;
extern BOOL   RENDER_CLEAR_COLOR_BUFFER;
extern BOOL   RENDER_CLEAR_DEPTH_BUFFER;
extern BOOL   RENDER_DRAW_SKY;
extern BOOL   RENDER_USE_CVA;

extern int    RENDER_MODEL_NUM;
extern int    RENDER_MODEL_DATA_NUM;
extern int    RENDER_MODEL_MAP_SHIFT;
extern size_t RENDER_MODEL_DATA_CACHE_SIZE;
extern size_t RENDER_MODEL_POOL_SIZE;
extern float  RENDER_MODEL_SHADOW_ALPHA;
extern int    RENDER_VIS_MODEL_NUM;
extern int    RENDER_SMPMODEL_MESH_NUM;
extern int    RENDER_MULTIMODEL_MESH_NUM;

extern int    RENDER_TREE_MAX_MATERIALS_COUNT;
extern int    RENDER_TREE_CACHE_TEXTURES_COUNT;
extern char*  RENDER_TREE_PRECOMP_NAME;
extern int    RENDER_TREE_MAP_SHIFT;
extern int    RENDER_TREES_IN_FRAME;
extern int    RENDER_TREES_VERTEX_CACHE_SIZE;
extern int    RENDER_TREES_MESH_CACHE_SIZE;

extern BOOL   RENDER_USE_MESH_RENDER;

extern float  RENDER_SPRITE_MIN_DIST;

extern uint   RENDER_VERTEX_CHUNK_NUM;
extern uint   RENDER_VERTEX_BUFFER_SIZE;
extern uint   RENDER_SHADOW_NUM;
extern uint   RENDER_WAVE_WATER_REFRESH_TIME;

extern uint   RENDER_ALPHA_MESH_NUM;

extern uint   RENDER_HM_TEXT_CACHE_COUNT;
extern uint   RENDER_HM_DRAW_DEPTH;

extern const char* OGL_CONSOLE_TEXTURE0;
extern const char* OGL_CONSOLE_TEXTURE1;

////////////////////////////////////////////////////////////////////////////////
///Const defines
////////////////////////////////////////////////////////////////////////////////

const   float   EPS                             = 0.1f;
const   int     MODEL_NAME_LEN                  = 64;
const   uint    LOADER_NUM_OF_OBJECTS           = 32;
const   uint    MESH_VERTEX_LIST                = 0x10000;
const   uint    MESH_VERTEX_LIST_MASK           = 0xFFFF;

#endif