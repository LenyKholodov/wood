#ifndef __CONVERT_MEMORY__
#define __CONVERT_MEMORY__

#include <types.h>

class    GameLevel;

struct   memavail_t
{
        size_t   mem_min, mem_max;

        memavail_t (): mem_min (0), mem_max (0) {}        
        memavail_t (size_t Min,size_t Max): mem_min (Min), mem_max (Max) {}        

        memavail_t& operator += (const memavail_t& x) { mem_min+=x.mem_min; mem_max+=x.mem_max; return *this; }
        friend memavail_t  operator +  (const memavail_t& x,const memavail_t& y) { 
          memavail_t a;
          a.mem_min = x.mem_min + y.mem_min;
          a.mem_max = x.mem_max + y.mem_max;
          return a;          
        }
};

////////////////////////////////////////////////////////////////////////////////
///Информация о памяти необходимой для загрузки уровня
////////////////////////////////////////////////////////////////////////////////
struct  levelmemavail_t
{
  memavail_t            total_info;                 //Общий размер

  memavail_t            land_cache;                 //Размер кэша земли
  memavail_t            land_vertex_buffer;         //Размер кэша вершин земли

  memavail_t            trees_vertex_buffer;        //Размер вершинного кэша для деревьев
  memavail_t            trees_data;                 //Размер кэша деревьев
};

void    BuildMemoryInfo         (GameLevel&);
BOOL    LoadMemoryInfo          (GameLevel&,levelmemavail_t&);

#endif