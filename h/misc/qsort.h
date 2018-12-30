#ifndef __MISC_QUICK_SORT__
#define __MISC_QUICK_SORT__

#include <misc\misc.h>

/*
    array  - ������ ���������
    num    - ���������� ���������
*/

template <class T>
void QSort (T* array,unsigned num);

/*
    array  - ������ ���������
    num    - ���������� ���������
    comp   - ������� (��� �������) ��������� 
*/

template <class T,typename Compare>
void QSortEx (T* array,unsigned num,Compare comp);

#include <misc\qsort.inl>

#endif