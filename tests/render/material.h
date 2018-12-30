#pragma once

#include "defs.h"

enum SortMode {
  NO_DIST_SORT = 0,
  BACK_TO_FRONT,
  FRONT_TO_BACK
};

struct AlphaTest  
{
  float    ref;            //параметр
  GLEnum   func;           //тест-функция 
};

struct Blend
{
  GLenum   src, dest;
  GLenum   mode;
  Color4   constant;
};

////////////////////////////////////////////////////////////////////////////////////////////
///Материал
////////////////////////////////////////////////////////////////////////////////////////////
class Material
{
  public:
    bool       lighting;       //присутствует ли освещение
//    bool       color_material; //

    union {
      struct {                 //параметры материала при включенном освещении
        col4f  ambient;       //поглощение
        col4f  diffuse;       //рассеивание
        col4f  specular;      //отражение
        col4f  emission;      //излучение

        float   power;         //агрессивность металличности
      };  

      col4f     wire_color;    //цвет при выключенном освещении
    };

    GLenum     shade_model;         //модель закрашивания
    GLenum     front_face;          //порядок обхода передней грани
    SortMode   face_sort;           //сортировка по расстоянию
    bool       render_front_face,   //отрисовывать переднюю сторону
               render_back_face;    //отрисовывать заднюю сторону

    AlphaTest* alpha_test;
    Blend*     blend; 

               bool apply ();
};

