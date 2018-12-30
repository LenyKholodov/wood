#ifndef __CONVERT_UTILS__
#define __CONVERT_UTILS__

#include "defs.h"
#include <graph\grtypes.h>
#include <graph\x3f.h>

////////////////////////////////////////////////////////////////////////////////
///Конвертер Object3D -> facet + vertex + normal
////////////////////////////////////////////////////////////////////////////////
BOOL     ConvertObject3D  (
  const Object3D&         obj,
  vertex_t*               vertex,
  vector3d_t*             normal,
  facet_t*                facet
);

static int ___ConvertObject3DNull = 0;

BOOL     ConvertObject3D  (
  const Object3D&         obj,
  Pool*                   pool,
  int&                    vCount   = ___ConvertObject3DNull,
  vertex_t**              vertex   = NULL,
  int&                    fCount   = ___ConvertObject3DNull,
  facet_t**               facet    = NULL,
  int&                    nCount   = ___ConvertObject3DNull,
  vector3d_t**            normal   = NULL,
  int&                    mCount   = ___ConvertObject3DNull,
  textmat_t**             material = NULL
);

void    ComputeBound    (int,vertex_t*,bound_t&);
void    ComputeBound    (int,facet_t*,vertex_t*,bound_t&);

////////////////////////////////////////////////////////////////////////////////
///Конвертер FacetArray с скрещивающимися текстурными координатами -> 
///FacetArray с неперекрещивающимися координатами. Каждая грань - один VB
////////////////////////////////////////////////////////////////////////////////
uint    ConvertFacetArray (
  facet_t*                _FacetArray,  
  uint                    _FacetCount,
  int*                    _Mirror,
  textcoord_t*            _DestTexCoords,
  uint                    _MirrorSize,
  uint                    _VertexCount
);

////////////////////////////////////////////////////////////////////////////////
///Преобразование вершины/нормали/текстурные координаты с перераспределением памяти
////////////////////////////////////////////////////////////////////////////////
///Перераспределение с учётом разделения текстурных координат
///Формируется три буфера пригодные для VA 
///В FacetArray сохраняются исходные грани с новыми индексами
///В SaveFacetArray можно сохранить исходные грани (default:NULL)
///Result: размер вершинного буфера в вершинах
////////////////////////////////////////////////////////////////////////////////
uint    ConvertArrays (
  uint                    _VertexCount,
  vertex_t**              _VertexArray,
  vector3d_t**            _NormalArray,
  textcoord_t**           _TCoordArray, //dest, *src = NULL
  facet_t*                _FacetArray,
  uint                    _FacetCount,
  Pool*                   _Pool,
  BOOL                    _EnableClear = TRUE,
  facet_t**               _SaveFacetArray = NULL,
  vertex_t**              _SaveVertexArray = NULL
);

////////////////////////////////////////////////////////////////////////////////
///Создание файла материалов
////////////////////////////////////////////////////////////////////////////////
void    CreateMatFile   (Object3D&,const char* mat_file);

////////////////////////////////////////////////////////////////////////////////
///Создание mesh (T&L,strip)
////////////////////////////////////////////////////////////////////////////////
mesh_t*         CreateMesh (
  const RingTmpl<int>&     facet_ring,
  const facet_t*           facet_array,
  uint                     vcache_size = 16,
  BOOL                     strip_enable = FALSE,
  BOOL                     size_pack    = FALSE,
  Pool*                    pool = M_GetPool (),
  Pool*                    temp_pool = M_GetPool ()
);

BOOL            CreateMesh (
  mesh_t*                  mesh,                
  const RingTmpl<int>&     facet_ring,
  const facet_t*           facet_array,
  uint                     vcache_size = 16,
  BOOL                     strip_enable = FALSE,
  BOOL                     size_pack    = FALSE,
  Pool*                    temp_pool = M_GetPool ()
);

BOOL            CreateMesh (
  mesh_t*                  mesh,                
  const RingTmpl<facet_t>& facet_ring,
  uint                     vcache_size = 16,
  BOOL                     strip_enable = FALSE,
  BOOL                     size_pack    = FALSE,
  Pool*                    temp_pool = M_GetPool ()
);

#endif