/****************************************************************************
*    ���������� �����.                                                      *
*    ������ 2.00 ��� Visual C/C++ 6.0                                       *
*    ����������� ����� ���� ����� � ����������.                             *
****************************************************************************/
#ifndef __BIG_FUCK_TYPES_H__
#define __BIG_FUCK_TYPES_H__

////////////////////////////////////////////////////////////////////////////////
///����������� �������� ����� ��� ������ � Windows
////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stddef.h>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///Includes for VS.NET
////////////////////////////////////////////////////////////////////////////////
/*#include <iostream>
#include <fstream>

using std::iostream;
using std::istream;
using std::ostream;
using std::ios;
using std::endl;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::streambuf;
*/

////////////////////////////////////////////////////////////////////////////////
///���������� ������ � realese ������!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
////////////////////////////////////////////////////////////////////////////////
#ifndef DEBUG 
  #ifndef WOOD_LIB_COMPILE
    #undef   DEBUG
    #define  DEBUG
  #endif
#endif

////////////////////////////////////////////////////////////////////////////////
///�������� ���������������� �����
////////////////////////////////////////////////////////////////////////////////

typedef __int64           int64;
typedef unsigned __int64  uint64;

typedef unsigned char   uchar;
typedef unsigned long   ulong;

typedef long            int32;
typedef unsigned long   uint32;

typedef unsigned short  uint16;
typedef signed short    int16;

typedef unsigned char   ubyte;
typedef unsigned char   byte;
typedef signed   char   ibyte;

typedef unsigned int    uint;
typedef unsigned short  ushort;

typedef int             INT32;
typedef unsigned int    UINT32;

typedef unsigned char   BYTE;

typedef char*           CHPTR;
typedef const char*     CHCPTR;
typedef char*           chptr;
typedef unsigned int*   uintptr;
typedef int*            intptr;
typedef ulong*          ulongptr;
typedef long*           longptr;

typedef wchar_t         WCHAR;

typedef char            *LPSTR, *PSTR;
typedef const char      *LPCSTR, *PCSTR;

////////////////////////////////////////////////////////////////////////////////
///��� ������������ ����������
////////////////////////////////////////////////////////////////////////////////
#define    ext_new  new (M_GetPool ())

///////////////////////////////////////////////////////////////////////////////
///���������� ��������� ��������������
///////////////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4291)
#pragma warning (disable : 4280)
#pragma warning (disable : 4284)
#pragma warning (disable : 4200)
#pragma warning (disable : 4651)
#pragma warning (disable : 4800)
#pragma warning (disable : 4018)
#pragma warning (disable : 4350)
#pragma warning (disable : 4305)

////////////////////////////////////////////////////////////////////////////////
///������������� ����
////////////////////////////////////////////////////////////////////////////////
#pragma comment (linker,"/libpath:q:/lib")
#pragma comment (linker,"/libpath:q:/lib/api") 

////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ���������
////////////////////////////////////////////////////////////////////////////////

#ifndef WOOD_LIB_COMPILE
  #ifdef DEBUG
    #pragma comment (lib,"kerneld.lib")
  #else
    #pragma comment (lib,"kernel.lib")
  #endif
#endif

#endif

