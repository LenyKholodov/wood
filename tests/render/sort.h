#pragma once

//#include <types.h>

typedef unsigned int uint;

////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� ��������� ���������
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
///������� ����������
////////////////////////////////////////////////////////////////////////////////////////////
template <class T,int cmp (const T&,const T&)=StdCmp<T>::Cmp>
class QuickSort
{
  public:
    QuickSort (T* base,unsigned num);
};

////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ������� ����� (�� ��������)
////////////////////////////////////////////////////////////////////////////////////////////
template <class   Attrib, //��� ��������
          Attrib& get   (Primitive&),  //�������� ��������
          bool    cmp   (const Attrib&,const Attrib&) = StdCmp<Attrib>::IsEqual //��������� ���� ���������
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
///�������� ������ ������� ��������
////////////////////////////////////////////////////////////////////////////////////////////
template <class   Attrib, //��� ��������
          Attrib& get   (Primitive&),  //�������� ��������
          bool    cmp   (const Attrib&,const Attrib&) = StdCmp<Attrib>::IsEqual //��������� ���� ���������
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
