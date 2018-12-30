#ifndef __ANIMATION__
#define __ANIMATION__

#include <graph\x3f.h>

/*
  Вычисление отдельных параметров анимации
*/

BOOL     AM_GetTranslate        (int frame,const Object3D::Node&,vector3d_t&);
BOOL     AM_GetScale            (int frame,const Object3D::Node&,vector3d_t&);
BOOL     AM_GetRotate           (int frame,const Object3D::Node&,matrix_t&);
BOOL     AM_GetRotate           (int frame,const Object3D::Node&,vector3d_t&,float&);

/*
  Взятие матрицы перевода в начальное состояние
*/

BOOL     AM_GetNullPosMatrix    (const Object3D::Node&,matrix_t&);

/*
  Матрица преобразование объекта на кадр анимации с учётом начального состояния
*/

BOOL     AM_GetPosMatrix        (int       frame,
                                 Object3D::Node node,
                                 matrix_t* _Transform,
                                 matrix_t* _Parent = NULL,
                                 matrix_t* _ForChild = NULL
                                );

/*
  Матрица для склетной анимации (родительская матрица)
*/

BOOL    AM_GetSceletonMatrix    (int frame,Object3D::Node node,matrix_t& _SceletonMatrix);

#endif