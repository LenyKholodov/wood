#ifndef __WXF_IMPORT_HELPERS__
#define __WXF_IMPORT_HELPERS__

#include <import\wxf\wxfbase.h>

class    FrameReader;
class    WXFNode;

/////////////////////////////////////////////////////////////////////////////////////////////
///Хелпер
/////////////////////////////////////////////////////////////////////////////////////////////
class  WXFHelper
{
  public:
    enum Helpers {
      BOX,
      SPHERE,
      CYLINDER,
      DUMMY
    };  

    Helpers       type;

  public:
                  WXFHelper (Helpers t) : type (t) { }

/////////////////////////////////////////////////////////////////////////////////////////////
///Присоединение крейтеров всех хелперов
/////////////////////////////////////////////////////////////////////////////////////////////
    static void BindCreaters (FrameReader&,WXFNode&);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Шар
/////////////////////////////////////////////////////////////////////////////////////////////
class  WXFHelperSphere: public WXFHelper
{
  friend class WXFHelper;
  public:
    vertex_t     c;      //центр
    float        r;      //радиус

  public:
                  WXFHelperSphere () : WXFHelper (SPHERE) {}

  private:
    static  WXFHelperSphere*  Create (WXFHelperSphere*,FrameReader&,WXFNode&);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Цилиндр
/////////////////////////////////////////////////////////////////////////////////////////////
class WXFHelperCylinder: public WXFHelper
{
  friend class WXFHelper;
  public:
    float      r;          //радиус
    float      height;     //высота
    vertex_t   c;          //центр
    vector3d_t dir;        //направление 

                  WXFHelperCylinder () : WXFHelper (CYLINDER) {}

  private:
    static  WXFHelperCylinder*  Create (WXFHelperCylinder*,FrameReader&,WXFNode&);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Дамик
/////////////////////////////////////////////////////////////////////////////////////////////
class WXFHelperDummy: public WXFHelper
{
  friend class WXFHelper;
  public:
    vertex_t      c;   //центр
    vector3d_t    dir; //направление

                  WXFHelperDummy () : WXFHelper (DUMMY) {}  

  private:
    static  WXFHelperDummy*  Create (WXFHelperDummy*,FrameReader&,WXFNode&);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Коробка
/////////////////////////////////////////////////////////////////////////////////////////////
class  WXFHelperBox: public WXFHelper
{
  friend class WXFHelper;
  public:
    vertex_t      c;    //центр
    vector3d_t    dir;  //направление

                  WXFHelperBox () : WXFHelper (BOX) {}  

  private:
    static  WXFHelperBox*  Create (WXFHelperBox*,FrameReader&,WXFNode&);
};

#endif