#ifndef __MISC_INCLUDE__
#define __MISC_INCLUDE__

#include <types.h>
#include <math.h>
#include <iosfwd>

////////////////////////////////////////////////////////////////////////////////
///Swap
////////////////////////////////////////////////////////////////////////////////
template <class Type>
void     swap (Type& v1,Type& v2)
{
  Type temp = v1;
  v1        = v2;
  v2        = temp;
}

////////////////////////////////////////////////////////////////////////////////
///CRC32
////////////////////////////////////////////////////////////////////////////////
extern ulong crc_32_tab [];

ulong           CRC32 (const void* data,int size,ulong crc=0xFFFFFFFF);
inline ulong    CRC32 (uchar data,ulong crc)
       { return crc_32_tab[(crc^data)&0xff]^(crc>>8); }

////////////////////////////////////////////////////////////////////////////////
///����� � ��ப��� (������ ���᪨� ��䠢��)
////////////////////////////////////////////////////////////////////////////////

extern  char    toUpper [256];
extern  char    toLower [256];

char*           upcase   (const char*,char*);
char*           locase   (const char*,char*);

inline char            ToUpperEx(char x) { return toUpper [(uchar)x]; }
inline char            ToLowerEx(char x) { return toLower [(uchar)x]; }

////////////////////////////////////////////////////////////////////////////////
///Memory
////////////////////////////////////////////////////////////////////////////////
void            memsetw  (void*,uint16,int num); //num!!!
void            memsetd  (void*,uint,int);

////////////////////////////////////////////////////////////////////////////////
///Random
////////////////////////////////////////////////////////////////////////////////
uint            random     (uint max); 
inline float    randomfl   ()          { return float (random (20000)) / 20000.0f; }

////////////////////////////////////////////////////////////////////////////////
///Math
////////////////////////////////////////////////////////////////////////////////
template <class Type> inline Type sqr (Type x) { return x * x; }

inline float    GetAngle   (float dx,float dy)
{
  if (dx <= 0)   return (float)acos (dy);
  else           return -(float)acos (dy);
}

////////////////////////////////////////////////////////////////////////////////
///Utilites
////////////////////////////////////////////////////////////////////////////////
class    Window;
int      WND_Done     (Window*,UINT,WPARAM,LPARAM,DWORD);

void     W32_PostQuitMessage (int);
BOOL     W32_SetCursorPos    (int,int);

///////////////////////////////////////////////////////////////////////////////
///iostreams
///////////////////////////////////////////////////////////////////////////////
class tab
{
  public:
        tab (int x = 8): num (x) {}

        friend std::ostream& operator << (std::ostream&,const tab&);

  private:
        int num;
};

///////////////////////////////////////////////////////////////////////////////
///����� ��� ����� 䠩��
///////////////////////////////////////////////////////////////////////////////
void    ParseFileName (const char* full_name,
                       char*       dir_name,
                       char*       file_name,
                       char*       ext_name
                      );


#endif