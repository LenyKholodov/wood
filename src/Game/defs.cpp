#include "pch.h"
#include "defs.h"

#include <time.h>

extern float  RENDER_VIEW_ANGLE                   = 45.0f / 57.3f;
extern float  RENDER_VIEW_DEPTH                   = 2500.0f;
extern int    RENDER_LR_CACHE_SIZE                = 90;
extern int    RENDER_STATIC_TEXTURES_CACHE_SIZE   = 16;
extern int    RENDER_DINAMIC_TEXTURES_CACHE_SIZE  = 16;
extern int    RENDER_GLOBAL_TEXTURES_CACHE_NUM    = 16;
extern int    RENDER_LAND_TEXT_CACHE              = 16;
extern int    RENDER_TREES_TEXT_CACHE             = 16;
extern int    RENDER_MESH_NUM                     = 2048;
extern int    RENDER_MESH_BUFFER_SIZE             = 32*1024;

extern int    RENDER_MODEL_NUM                  = 16384;
extern int    RENDER_MODEL_DATA_NUM             = 512;
extern size_t RENDER_MODEL_DATA_CACHE_SIZE      = 512*1024;
extern size_t RENDER_MODEL_POOL_SIZE            = 512*1024;
extern float  RENDER_MODEL_SHADOW_ALPHA         = 0.1;
extern int    RENDER_VIS_MODEL_NUM              = 1024;
extern int    RENDER_MODEL_MAP_SHIFT            = 2;
extern int    RENDER_SMPMODEL_MESH_NUM          = 64;
extern int    RENDER_MULTIMODEL_MESH_NUM        = 128;

extern BOOL   RENDER_USE_MESH_RENDER            = TRUE;

extern float  RENDER_SPRITE_MIN_DIST            = EPS;

extern uint   RENDER_VERTEX_CHUNK_NUM           = 2;
extern uint   RENDER_VERTEX_BUFFER_SIZE         = 512*1024; //equal
//extern uint   RENDER_VERTEX_BUFFER_SIZE         = 256*1024; //equal

extern BOOL   RENDER_GL_FINISH                  = FALSE;
extern BOOL   RENDER_CLEAR_COLOR_BUFFER         = FALSE;
extern BOOL   RENDER_CLEAR_DEPTH_BUFFER         = FALSE;
extern BOOL   RENDER_USE_CVA                    = FALSE;
extern BOOL   RENDER_DRAW_SKY                   = TRUE;
extern uint   RENDER_SHADOW_NUM                 = 32;

extern uint   RENDER_WAVE_WATER_REFRESH_TIME    = CLK_TCK >> 4;

extern int    RENDER_TREE_MAX_MATERIALS_COUNT   = 2048;
extern char*  RENDER_TREE_PRECOMP_NAME          = "treeprecomp.wd";
extern int    RENDER_TREE_MAP_SHIFT             = 2;
extern int    RENDER_TREES_IN_FRAME             = 4096; 

extern int    RENDER_TREES_VERTEX_CACHE_SIZE    = 64*1024;  //!!!!!
extern int    RENDER_TREES_MESH_CACHE_SIZE      = 1024 * 1024;
extern int    RENDER_TREE_CACHE_TEXTURES_COUNT  = 16;

extern uint   RENDER_ALPHA_MESH_NUM             = 128;
extern uint   RENDER_HM_TEXT_CACHE_COUNT        = 16;
extern uint   RENDER_HM_DRAW_DEPTH              = 4;

//extern const char* OGL_CONSOLE_TEXTURE0         = "console.bmp";
//extern const char* OGL_CONSOLE_TEXTURE1         = "console.jpg";

extern const char* OGL_CONSOLE_TEXTURE0         = "console.bmp";
extern const char* OGL_CONSOLE_TEXTURE1         = "console.jpg";