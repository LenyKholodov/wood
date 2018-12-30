#ifndef __INTERPOLATE_SPLINES__
#define __INTERPOLATE_SPLINES__

#include <types.h>

////////////////////////////////////////////////////////////////////////////////////////////
///���௮��樮��� �㡨�᪨� ᯫ��� ��砭���-���⥫��
////////////////////////////////////////////////////////////////////////////////////////////
template <class T> SplineCube 
{
  public:                     
        typedef float   Time;

        struct  key_t {       //���祢�� ���ﭨ�
          Time        time;   //�६�
          T           state;  //���ﭨ�

                      //�����樥���
          float       easeTo,easeFrom;
          float       bias, continuity, tension;

                      key_t () { //�� 㬮�砭�� �� �����樥��� � ����
                        easeTo     = 0;
                        easeFrom   = 0;
                        bias       = 0;
                        continuity = 0;
                        tension    = 0;
                      }
        };

                SplineCube (const key_t* keys,uint count);

/////////////////////////////////////////////////////////////////////////////////////////////
///���௮����        
/////////////////////////////////////////////////////////////////////////////////////////////
        T&      recalc  (Time time,T& result) const; //ret: result
        T       recalc  (Time time) const;

        T       operator [] (Time time) const { return recalc (time); }

/////////////////////////////////////////////////////////////////////////////////////////////
///������⢮ ���祢�� �����⮢
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