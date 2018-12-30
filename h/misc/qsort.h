#ifndef __MISC_QUICK_SORT__
#define __MISC_QUICK_SORT__

#include <misc\misc.h>

/*
    array  - массив элементов
    num    - количество элементов
*/

template <class T>
void QSort (T* array,unsigned num);

/*
    array  - массив элементов
    num    - количество элементов
    comp   - функция (или функтор) сравнения 
*/

template <class T,typename Compare>
void QSortEx (T* array,unsigned num,Compare comp);

#include <misc\qsort.inl>

#endif