#ifndef __WXF_IMPORT_MATERIAL_DEFINES__
#define __WXF_IMPORT_MATERIAL_DEFINES__

#include <import\wxf\wxfbase.h>
#include <misc.h>

class    WXFMaterial;
class    WXFMap;
class    FrameReader;

/////////////////////////////////////////////////////////////////////////////////////////////
///Базовый описатель материалов
/////////////////////////////////////////////////////////////////////////////////////////////
class  WXFMtlBase
{
  public:
    enum {
      MATERIAL,
      MAP
    };

    int   type;

          WXFMtlBase (int t): type (t) {}
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Карта 
/////////////////////////////////////////////////////////////////////////////////////////////
class  WXFMap: public WXFMtlBase
{  
  friend class WXFMaterial;
  public:
    typedef Ring<WXFMap*> ChildRing;

    WXFMap*     parent;   //предок
    ChildRing   maps;     //потомки

    struct      bitmap_t  //описатель текстуры
    {
      Str256      name;                  //Файл с текстурой
      float       u_offs,v_offs;         //смещения
      float       u_tiling, v_tiling;    //шаг тайлинга
    }bmp;

  public:
                  ~WXFMap ();

  private:
                   WXFMap  (WXFMaterial&);

/////////////////////////////////////////////////////////////////////////////////////////////
///Чтение
/////////////////////////////////////////////////////////////////////////////////////////////
    static   WXFMap*  Create (WXFMap*,FrameReader&,WXFMtlBase*);

  private:
    WXFMaterial*  mtl; //материал, которому карта пренадлежит
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Материал
/////////////////////////////////////////////////////////////////////////////////////////////
class   WXFMaterial: public WXFMtlBase, protected WXFBase
{
  friend class WXFMap;
  friend class WXFImport;
  public:
    typedef Ring<WXFMaterial*> MatRing;

    String<63>   name;         //имя

    WXFMaterial* parent;       //предок
    MatRing      children;     //потомки

    color3d_t    ambient;      //поглощенный
    color3d_t    diffuse;      //рассеяный
    color3d_t    specular;     //отражённый
    float        shininess;    //коэффициент "металичности"

//Описание карт

                 enum Maps {
                   DIFFUSE = 0,
                   OPACITY,
                   BUMP,
                   
                   MAPS_NUM
                 };
    
    WXFMap*      maps [MAPS_NUM]; //карты

  public:
                 ~WXFMaterial ();

  private:
                 WXFMaterial  (WXFImport*);

/////////////////////////////////////////////////////////////////////////////////////////////
///Крейтер
/////////////////////////////////////////////////////////////////////////////////////////////
    static   WXFMaterial* Create         (WXFMaterial*,FrameReader&,WXFImport*);

  private:
    static   bool         ReadMatParent  (istream&,WXFMaterial*);
    static   bool         ReadMatName    (istream&,WXFMaterial*);
};

#endif