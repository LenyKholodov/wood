#pragma once

#include "defs.h"

enum SortMode {
  NO_DIST_SORT = 0,
  BACK_TO_FRONT,
  FRONT_TO_BACK
};

struct AlphaTest  
{
  float    ref;            //��������
  GLEnum   func;           //����-������� 
};

struct Blend
{
  GLenum   src, dest;
  GLenum   mode;
  Color4   constant;
};

////////////////////////////////////////////////////////////////////////////////////////////
///��������
////////////////////////////////////////////////////////////////////////////////////////////
class Material
{
  public:
    bool       lighting;       //������������ �� ���������
//    bool       color_material; //

    union {
      struct {                 //��������� ��������� ��� ���������� ���������
        col4f  ambient;       //����������
        col4f  diffuse;       //�����������
        col4f  specular;      //���������
        col4f  emission;      //���������

        float   power;         //������������� �������������
      };  

      col4f     wire_color;    //���� ��� ����������� ���������
    };

    GLenum     shade_model;         //������ ������������
    GLenum     front_face;          //������� ������ �������� �����
    SortMode   face_sort;           //���������� �� ����������
    bool       render_front_face,   //������������ �������� �������
               render_back_face;    //������������ ������ �������

    AlphaTest* alpha_test;
    Blend*     blend; 

               bool apply ();
};

