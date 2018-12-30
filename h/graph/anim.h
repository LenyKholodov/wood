#ifndef __ANIMATION__
#define __ANIMATION__

#include <graph\x3f.h>

/*
  ���᫥��� �⤥���� ��ࠬ��஢ �����樨
*/

BOOL     AM_GetTranslate        (int frame,const Object3D::Node&,vector3d_t&);
BOOL     AM_GetScale            (int frame,const Object3D::Node&,vector3d_t&);
BOOL     AM_GetRotate           (int frame,const Object3D::Node&,matrix_t&);
BOOL     AM_GetRotate           (int frame,const Object3D::Node&,vector3d_t&,float&);

/*
  ���⨥ ������ ��ॢ��� � ��砫쭮� ���ﭨ�
*/

BOOL     AM_GetNullPosMatrix    (const Object3D::Node&,matrix_t&);

/*
  ����� �८�ࠧ������ ��ꥪ� �� ���� �����樨 � ���⮬ ��砫쭮�� ���ﭨ�
*/

BOOL     AM_GetPosMatrix        (int       frame,
                                 Object3D::Node node,
                                 matrix_t* _Transform,
                                 matrix_t* _Parent = NULL,
                                 matrix_t* _ForChild = NULL
                                );

/*
  ����� ��� ᪫�⭮� �����樨 (த�⥫�᪠� �����)
*/

BOOL    AM_GetSceletonMatrix    (int frame,Object3D::Node node,matrix_t& _SceletonMatrix);

#endif