#ifndef __CONVERT_UTILS__
#define __CONVERT_UTILS__

#include "defs.h"
#include <graph\grtypes.h>
#include <graph\x3f.h>

////////////////////////////////////////////////////////////////////////////////
///�������� Object3D -> facet + vertex + normal
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
///�������� FacetArray � ��騢��騬��� ⥪����묨 ���न��⠬� -> 
///FacetArray � ����४�騢��騬��� ���न��⠬�. ������ �࠭� - ���� VB
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
///�८�ࠧ������ ���設�/��ଠ��/⥪����� ���न���� � �����।������� �����
////////////////////////////////////////////////////////////////////////////////
///�����।������ � ���⮬ ࠧ������� ⥪������ ���न���
///��ନ����� �� ���� �ਣ���� ��� VA 
///� FacetArray ��࠭����� ��室�� �࠭� � ���묨 �����ᠬ�
///� SaveFacetArray ����� ��࠭��� ��室�� �࠭� (default:NULL)
///Result: ࠧ��� ���設���� ���� � ���設��
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
///�������� 䠩�� ���ਠ���
////////////////////////////////////////////////////////////////////////////////
void    CreateMatFile   (Object3D&,const char* mat_file);

////////////////////////////////////////////////////////////////////////////////
///�������� mesh (T&L,strip)
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