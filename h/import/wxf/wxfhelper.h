#ifndef __WXF_IMPORT_HELPERS__
#define __WXF_IMPORT_HELPERS__

#include <import\wxf\wxfbase.h>

class    FrameReader;
class    WXFNode;

/////////////////////////////////////////////////////////////////////////////////////////////
///������
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
///��ᮥ������� �३�஢ ��� 奫��஢
/////////////////////////////////////////////////////////////////////////////////////////////
    static void BindCreaters (FrameReader&,WXFNode&);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///���
/////////////////////////////////////////////////////////////////////////////////////////////
class  WXFHelperSphere: public WXFHelper
{
  friend class WXFHelper;
  public:
    vertex_t     c;      //業��
    float        r;      //ࠤ���

  public:
                  WXFHelperSphere () : WXFHelper (SPHERE) {}

  private:
    static  WXFHelperSphere*  Create (WXFHelperSphere*,FrameReader&,WXFNode&);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///�������
/////////////////////////////////////////////////////////////////////////////////////////////
class WXFHelperCylinder: public WXFHelper
{
  friend class WXFHelper;
  public:
    float      r;          //ࠤ���
    float      height;     //����
    vertex_t   c;          //業��
    vector3d_t dir;        //���ࠢ����� 

                  WXFHelperCylinder () : WXFHelper (CYLINDER) {}

  private:
    static  WXFHelperCylinder*  Create (WXFHelperCylinder*,FrameReader&,WXFNode&);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///�����
/////////////////////////////////////////////////////////////////////////////////////////////
class WXFHelperDummy: public WXFHelper
{
  friend class WXFHelper;
  public:
    vertex_t      c;   //業��
    vector3d_t    dir; //���ࠢ�����

                  WXFHelperDummy () : WXFHelper (DUMMY) {}  

  private:
    static  WXFHelperDummy*  Create (WXFHelperDummy*,FrameReader&,WXFNode&);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///��஡��
/////////////////////////////////////////////////////////////////////////////////////////////
class  WXFHelperBox: public WXFHelper
{
  friend class WXFHelper;
  public:
    vertex_t      c;    //業��
    vector3d_t    dir;  //���ࠢ�����

                  WXFHelperBox () : WXFHelper (BOX) {}  

  private:
    static  WXFHelperBox*  Create (WXFHelperBox*,FrameReader&,WXFNode&);
};

#endif