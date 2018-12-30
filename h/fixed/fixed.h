#ifndef __FIXED_H__
#define __FIXED_H__

#include <types.h>

#pragma warning (disable : 4350)

#define GRADUS (0x10000/360)
#define RAD    (0x10000/FL_ANGLE_180)
#define RAD_2  (RAD/2)

typedef long    Fixed;
typedef uint16  Angle;

const float     FL_PI        =   3.14159265358979323846f;
const float     FL_EPS       =   0.1f;
const float     FL_RAD       =   180.0f/FL_PI;
const float     FL_GRADUS    =   FL_PI/180.0f;
const float     FL_ANGLE_90  =   FL_PI/2.0f;
const float     FL_ANGLE_180 =   FL_PI;
const float     FL_ANGLE_270 =   FL_ANGLE_90*1.5f;
const float     FL_ANGLE_360 =   2.0f*FL_PI;

#define fixed2int(x) ((x)>>16)
#define int2fixed(x) ((x)<<16)
#define fixed2float(x) ((float)((x)/65536.0))
#define float2fixed(x) ((Fixed)((x)*65536.0))
#define float2angle(x) ((Angle)((x)*RAD_2))
#define int2angle(x)   ((Angle)(GRADUS*(x)))
#define angle2int(x)   ((int)((x)/GRADUS))
#define angle2float(x) ((float)((x)/RAD_2))

const Fixed     FX_NULL      =    0;
const Fixed     FX_ONE       =    0x10000;
const Fixed     FX_HALF      =    float2fixed(0.5);
const Fixed     FX_EPS       =    257;

const Fixed     FX_PI        =   float2fixed(FL_PI);
const Fixed     FX_RAD       =   float2fixed(180.0/FL_PI);
const Fixed     FX_GRADUS    =   float2fixed(FL_PI/180.0);

const Angle     ANGLE_0      =   0;
const Angle     ANGLE_1      =   float2angle(FL_PI/180.0);
const Angle     ANGLE_10     =   float2angle(FL_PI/18.0);
const Angle     ANGLE_30     =   float2angle(FL_PI/6.0);
const Angle     ANGLE_45     =   float2angle(FL_PI/4.0);
const Angle     ANGLE_60     =   float2angle(FL_PI/3.0);
const Angle     ANGLE_90     =   0x4000;
const Angle     ANGLE_120    =   ANGLE_60*2;
const Angle     ANGLE_180    =   0x8000;
const Angle     ANGLE_270    =   0xC000;
const Angle     ANGLE_360    =   0xFFFF; //0x10000

inline Fixed   mul     (const Fixed a,const Fixed b)
{
  return ((__int64)a * (__int64)b) >> 16;
}

inline Fixed   div     (const Fixed a,const Fixed b)
{
  return  ((__int64(a)<<16)/__int64(b));
}

inline Fixed   frac    (const Fixed x)
{
  return x & 0xFFFF;
}

#ifdef DEBUG
  #pragma comment (lib,"math3dd.lib")
#else
  #pragma comment (lib,"math3d.lib")
#endif

#endif
