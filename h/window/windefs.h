#ifndef __CLASS_WINDOW_DEFINES__
#define __CLASS_WINDOW_DEFINES__

#include <types.h>

#define PS_SOLID            0
#define PS_DASH             1       /* -------  */
#define PS_DOT              2       /* .......  */
#define PS_DASHDOT          3       /* _._._._  */
#define PS_DASHDOTDOT       4       /* _.._.._  */
#define PS_NULL             5
#define PS_INSIDEFRAME      6
#define PS_USERSTYLE        7
#define PS_ALTERNATE        8
#define PS_STYLE_MASK       0x0000000F

struct   pen_t
{
  ulong         color;  //Цвет пера
  int           width;  //Толщина пера
  uint          style;  //Стиль пера                  

                pen_t (ulong _color,int _width,uint _style):
                  color (_color), width (_width), style (_style) {}                  
};

#define BS_SOLID            0
#define BS_NULL             1
#define BS_HOLLOW           BS_NULL
#define BS_HATCHED          2
#define BS_PATTERN          3
#define BS_INDEXED          4
#define BS_DIBPATTERN       5
#define BS_DIBPATTERNPT     6
#define BS_PATTERN8X8       7
#define BS_DIBPATTERN8X8    8
#define BS_MONOPATTERN      9

#define HS_HORIZONTAL       0       /* ----- */
#define HS_VERTICAL         1       /* ||||| */
#define HS_FDIAGONAL        2       /* \\\\\ */
#define HS_BDIAGONAL        3       /* ///// */
#define HS_CROSS            4       /* +++++ */
#define HS_DIAGCROSS        5       /* xxxxx */

struct  brush_t
{
  ulong         color;  //Цвет кисти
  uint          style;  //Стиль кисти
  long          hatch;  //Стиль штриховки

                brush_t (ulong _color,uint _style,long _hatch):
                  color (_color), style (_style), hatch (_hatch) {}
};

typedef DWORD COLORREF;

#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define PALETTERGB(r,g,b)   (0x02000000 | RGB(r,g,b))
#define PALETTEINDEX(i)     ((COLORREF)(0x01000000 | (DWORD)(WORD)(i)))

#define FW_DONTCARE         0
#define FW_THIN             100
#define FW_EXTRALIGHT       200
#define FW_LIGHT            300
#define FW_NORMAL           400
#define FW_MEDIUM           500
#define FW_SEMIBOLD         600
#define FW_BOLD             700
#define FW_EXTRABOLD        800
#define FW_HEAVY            900

#define FW_ULTRALIGHT       FW_EXTRALIGHT
#define FW_REGULAR          FW_NORMAL
#define FW_DEMIBOLD         FW_SEMIBOLD
#define FW_ULTRABOLD        FW_EXTRABOLD
#define FW_BLACK            FW_HEAVY

#define ANSI_CHARSET            0
#define DEFAULT_CHARSET         1
#define SYMBOL_CHARSET          2
#define SHIFTJIS_CHARSET        128
#define HANGEUL_CHARSET         129
#define HANGUL_CHARSET          129
#define GB2312_CHARSET          134
#define CHINESEBIG5_CHARSET     136
#define OEM_CHARSET             255

#define JOHAB_CHARSET           130
#define HEBREW_CHARSET          177
#define ARABIC_CHARSET          178
#define GREEK_CHARSET           161
#define TURKISH_CHARSET         162
#define VIETNAMESE_CHARSET      163
#define THAI_CHARSET            222
#define EASTEUROPE_CHARSET      238
#define RUSSIAN_CHARSET         204

#define MAC_CHARSET             77
#define BALTIC_CHARSET          186

struct  gdifont_t
{
  int           width;          //Ширина
  int           height;         //Высота
  int           weight;         //Вес
  uint          flags;          //Флаги
  uint          charSet;        //Кодировка
  char*         name;           //Имя гарнитуры

                enum {
                  ITALIC,
                  UNDERLINE,
                  STRIKE_OUT
                };

                gdifont_t (int _width,
                           int _height,
                           int _weight = FW_NORMAL,
                           int _flags  = 0,
                           int _set    = ANSI_CHARSET,
                           char* _name  = NULL):
                width (_width), height (_height), weight (_weight),
                flags (_flags), charSet (_set), name (_name) {}
};

#endif