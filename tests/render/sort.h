#pragma once

//#include <types.h>

typedef unsigned int uint;

////////////////////////////////////////////////////////////////////////////////////////////
///Стандартная функция сравнения атрибутов
////////////////////////////////////////////////////////////////////////////////////////////
template <class Attrib>
class StdCmp
{
  public: 
    inline static bool IsEqual (const Attrib& a1,const Attrib& a2) { return a1 == a2; }
    inline static int  Cmp     (const Attrib& a1,const Attrib& a2) { 
      return a1 == a2 ? 0 : a1 < a2 ? -1 : 1; 
    }
}; 

////////////////////////////////////////////////////////////////////////////////////////////
///Быстрая сортировка
////////////////////////////////////////////////////////////////////////////////////////////
template <class T,int cmp (const T&,const T&)=StdCmp<T>::Cmp>
class QuickSort
{
  public:
    QuickSort (T* base,unsigned num);
};

////////////////////////////////////////////////////////////////////////////////////////////
///Итератор выделения смежных групп (по атрибуту)
////////////////////////////////////////////////////////////////////////////////////////////
template <class   Attrib, //тип атрибута
          Attrib& get   (Primitive&),  //селектор атрибута
          bool    cmp   (const Attrib&,const Attrib&) = StdCmp<Attrib>::IsEqual //сравнение двух атрибутов
         >
class SortIter
{
  public: 
    struct Span
    {
      Attrib&    attrib;
      Primitive* first;
      uint       count;
    };

                 SortIter         (Primitive*,uint);

        Span*    operator ->      () const { return (Span*)&span; }        
                 operator bool    () const { return span.count != 0; }

        void     next             ();

  private:
    struct {
      Attrib*    attrib;
      Primitive* first;
      uint       count;
    }span;

    Primitive* first;
    uint       count;
};

////////////////////////////////////////////////////////////////////////////////////////////
///Итератор поиска смежных подгрупп
////////////////////////////////////////////////////////////////////////////////////////////
template <class   Attrib, //тип атрибута
          Attrib& get   (Primitive&),  //селектор атрибута
          bool    cmp   (const Attrib&,const Attrib&) = StdCmp<Attrib>::IsEqual //сравнение двух атрибутов
         >
class SearchIter
{
  public: 
    struct Span
    {
      Attrib&    attrib;
      Primitive* first;
      uint       count;
    };

                 SearchIter       (Primitive*,uint);

        Span*    operator ->      () const { return (Span*)&span; }        
                 operator bool    () const { return span.count != 0; }

        void     next             ();

  private:
    struct {
      Attrib*    attrib;
      Primitive* first;
      uint       count;
    }span;

    Primitive* first;
    uint       count;
};

#include "sort.inl"
