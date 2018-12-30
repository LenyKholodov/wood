#ifndef __INTERPOLATE_SPLINES__
#define __INTERPOLATE_SPLINES__

#include <types.h>

////////////////////////////////////////////////////////////////////////////////////////////
///Интерполяционный кубический сплайн Кочанека-Бартельса
////////////////////////////////////////////////////////////////////////////////////////////
template <class T> SplineCube 
{
  public:                     
        typedef float   Time;

        struct  key_t {       //ключевое состояние
          Time        time;   //время
          T           state;  //состояние

                      //коэффициенты
          float       easeTo,easeFrom;
          float       bias, continuity, tension;

                      key_t () { //по умолчанию все коэффициенты в ноль
                        easeTo     = 0;
                        easeFrom   = 0;
                        bias       = 0;
                        continuity = 0;
                        tension    = 0;
                      }
        };

                SplineCube (const key_t* keys,uint count);

/////////////////////////////////////////////////////////////////////////////////////////////
///Интерполяция        
/////////////////////////////////////////////////////////////////////////////////////////////
        T&      recalc  (Time time,T& result) const; //ret: result
        T       recalc  (Time time) const;

        T       operator [] (Time time) const { return recalc (time); }

/////////////////////////////////////////////////////////////////////////////////////////////
///Количество ключевых моментов
/////////////////////////////////////////////////////////////////////////////////////////////
        uint    count    () const { return mCount; }

  private:
        Time    ease     (Time t,Time from,Time to);
        void    build    (key_t*,uint);

  private:
        struct seg_t {
          T      a,b,c,d;
          Time   start, end;       
          float  locTime;          
          float  easeTo, easeFrom; 
        };
        
        uint    mCount;
        seg_t   mSegs;
};

#include "splinec.inl"

#endif