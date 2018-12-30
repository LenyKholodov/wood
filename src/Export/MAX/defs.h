#ifndef __MAX_EXPORT_DEFINES_WOOD_EXPORTER__
#define __MAX_EXPORT_DEFINES_WOOD_EXPORTER__

#include <pch.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Флаги экспорта
/////////////////////////////////////////////////////////////////////////////////////////////
enum ExportFlags {
  EXPORT_SELECTED    = 1,

  EXPORT_TEXTURE_COORDS = 2,
  EXPORT_VERTEX_COLORS  = 4,
  EXPORT_NORMALS        = 8,

  EXPORT_GEOMETRY    = 16,
  EXPORT_MESH        = 32,
  EXPORT_ANIM_KEYS   = 64,
  EXPORT_MESH_ANIM   = 128,
  EXPORT_IK_JOINTS   = 356,
  EXPORT_MATERIALS   = 512,
  EXPORT_CAMERAS     = 1024,
  EXPORT_HELPERS     = 2048,
  EXPORT_LIGHTS      = 4096,
  EXPORT_SHAPES      = 8129
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Экспорт в бинарном виде
/////////////////////////////////////////////////////////////////////////////////////////////
enum BinExport {
  BIN_FACES      = 1,
  BIN_VERTEXES   = 2,
  BIN_NORMALS    = 4,
  BIN_TEXCOORDS  = 8,
  BIN_COLORS     = 16,
  BIN_ANIM_KEYS  = 32
};

const   int     EXPORT_FLOAT_PRECISION = 4;

#define PREC setprecision (EXPORT_FLOAT_PRECISION)

#endif