#pragma once

#include "math\vecmath.h"

typedef unsigned int   uint, uint32;
typedef unsigned short ushort, uint16;
typedef signed   int   int32;
typedef signed   short int16;
typedef unsigned char  uchar, ubyte;
typedef char           byte;

template <class type> struct GLType { };

template <> struct GLType<float>  { enum { type = GL_FLOAT }; };
template <> struct GLType<double> { enum { type = GL_DOUBLE }; };
template <> struct GLType<int>    { enum { type = GL_INT }; };
template <> struct GLType<uint>   { enum { type = GL_UNSIGNED_INT }; };
template <> struct GLType<short>  { enum { type = GL_SHORT }; };
template <> struct GLType<ushort> { enum { type = GL_UNSIGNED_SHORT }; };
template <> struct GLType<char>   { enum { type = GL_BYTE }; };
template <> struct GLType<uchar>  { enum { type = GL_UNSIGNED_SHORT }; };