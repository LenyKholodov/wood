#ifndef __STRIPER__
#define __STRIPER__

#include <graph\grtypes.h>
#include <misc.h>
#include <pool.h>

const    int    STRIP_ONE_SIDED_STRIPS    = 1;
const    int    STRIP_USE_SGI_ALGORITHM   = 2;
const    int    STRIP_CONNECT_ALL_STRIPS  = 4;

struct stripsrc_t 
{
  uint      count;
  uint*     faces;
  uint      flags;
};

struct stripdest_t
{
  uint     count;

  struct   strip_t
  {
    uint     len;
    uint*    strip;
  }strip [];
};

////////////////////////////////////////////////////////////////////////////////
///Вычисляет strip для набора треугольников
////////////////////////////////////////////////////////////////////////////////
stripdest_t*   ComputeStrip    (const RingTmpl<facet_t>&,uint flags = 0,Pool* = ::M_GetPool());
stripdest_t*   ComputeStrip    (const RingTmpl<int>&,facet_t*,uint flags = 0,Pool* = ::M_GetPool());
stripdest_t*   ComputeStrip    (uint count,facet_t* list,uint flags = 0,Pool* = ::M_GetPool());
stripdest_t*   ComputeStrip    (const stripsrc_t&,Pool* = ::M_GetPool());

#endif