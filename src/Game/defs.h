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

////////////////////////////Заголовки ресурсов//////////////////////////////////

struct   vbheader_t
{
  uint          count;          //Количество VertexBuffer
  size_t        vbSize;         //Размер VertexBuffer
  ulong         offs;           //Смещение первого VertextBuffer
};

struct  grmapheader_t
{
  int           width, height;                  //Размеры карты
  int           vCount, fCount;                 //Кол-во вершин\граней
  boundbox_t    bbox;                           //Ограничивающая коробка
  float         step;                           //Шаг сетки
  int           cacheWidth, cacheHeight;        //Размеры прямоугольника кэширования
};

struct  grcacheheader_t
{
  size_t   minMeshSize,                         //Минимальный размер мэша
           maxMeshSize;                         //Максимиальный размер мэша
  int      meshNum;                             //Количесвто мэшей
  int      mapSize;                             //Размер карты: width * height
  ulong    tablesOffs;                          //Смещение таблиц (конец файла)
  int      vCount;                              //Размер вершинного буфера
};

struct  landmatheader_t
{
  ulong         count;          //Количество материалов
  size_t        strsize;        //Размер строки с именами
};

struct  hmapheader_t
{
  char          mname [128],      //Имя файла (NONAME = map include)
                tname [128];      
  int           width, height;    //Размеры карты
  int           twidth,theight;   //Размеры тайла
  int           text_twidth,      //Размеры тайла текстуры
                text_theight;
  float         kx,ky,kz;         //Коеффициенты масштабирования
  ulong         infoOffs;         //Смещение до таблиц
};

////////////////////////////////////////////////////////////////////////////////
///Основной пакет передачи треугольников
///VERTEX_LIST: normal3f, textcoord2f, vertex3f
////////////////////////////////////////////////////////////////////////////////

#ifndef __RENDER_MESH__
#define __RENDER_MESH__

#pragma pack (push)
#pragma pack (4)

struct  mesh_t
{
  size_t        size;       //Размер мэша
  uint16        type;       //Тип (GL_TRIANGLES, GL_TRIANGLE_STRIP, VERTEX_LIST)
  uint16        vbuffer;    //Индекс VertexBuffer'а
  uint16        elType;     //Тип элемента (uint,uint16,byte,vertex,etc.)
  uint          start, end; //Индекс начальной и конечной вершин
  uint          index;      //Индекс мэша
  uint          count;      //Количество элементов в мэше
  int           material;   //Материал мэша
  char          data [];    //Данные
};

struct   multimesh_t
{
  int           num;        //Количество мэшей
  mesh_t*       mesh () const { return (mesh_t*)((char*)this+sizeof (multimesh_t)); }
};

#pragma pack (pop)

#endif

////////////////////////////////////////////////////////////////////////////////
///Описание источника света
////////////////////////////////////////////////////////////////////////////////
struct  lightsrc_t
{
  light_t      light;
  float        shiness;
};

////////////////////////////////////////////////////////////////////////////////
///Описание вершинного буфера
////////////////////////////////////////////////////////////////////////////////
struct  vertexbuffer_t
{
  int           count,          //Количество
                pos,            //Позиция заполнения
                first;          //Индекс первой вершины

  vertex_t*     vertex;         //Вершины
  textcoord_t*  tcoord;         //Текстурные координаты
  vector3d_t*   normal;         //Нормали
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
  size_t        size;           //Размер node
  ulong         offs;           //Смещение node в файле

  struct coord_t {
    int16 x,y;
  }coord;  
};

struct  nodeinfo_t {
  struct chunk_t {
    size_t        size;         //Размер куска
    ulong         offs;         //Смещение куска (относительно начала node)

    uint          count;        //Количество элементов
    int           first;        //Индекс первого элемента
  }chunk [GRC_NUM];  
};

struct  grtile_t
{
  boundbox_t    box;          //Ограничивающий ящик
  
  tileinfo_t    tile;         //Информация о тайле
  nodeinfo_t    node;  

  int           index;
};

struct  facetpatch_t
{
  int           count;              //Количество треугольников
  int           vbuffer;            //Вершинный буфер
  size_t        size;               //Размер группы            
  facet_t       data  [];           //Треугольники
};

struct  hmtile_t
{
  boundbox_t    box;                //Коробка, около тайла height map'а 
  int           texture;            //Текстура тайла
  int           index;              //Индекс тайла
  int           x,y;                //Координаты начала тайла

  ulong         sOffs, eOffs;       //Смещения
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