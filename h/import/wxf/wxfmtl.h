#ifndef __WXF_IMPORT_MATERIAL_DEFINES__
#define __WXF_IMPORT_MATERIAL_DEFINES__

#include <import\wxf\wxfbase.h>
#include <misc.h>

class    WXFMaterial;
class    WXFMap;
class    FrameReader;

/////////////////////////////////////////////////////////////////////////////////////////////
///������ ����⥫� ���ਠ���
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
///���� 
/////////////////////////////////////////////////////////////////////////////////////////////
class  WXFMap: public WXFMtlBase
{  
  friend class WXFMaterial;
  public:
    typedef Ring<WXFMap*> ChildRing;

    WXFMap*     parent;   //�।��
    ChildRing   maps;     //��⮬��

    struct      bitmap_t  //����⥫� ⥪�����
    {
      Str256      name;                  //���� � ⥪���ன
      float       u_offs,v_offs;         //ᬥ饭��
      float       u_tiling, v_tiling;    //蠣 ⠩�����
    }bmp;

  public:
                  ~WXFMap ();

  private:
                   WXFMap  (WXFMaterial&);

/////////////////////////////////////////////////////////////////////////////////////////////
///�⥭��
/////////////////////////////////////////////////////////////////////////////////////////////
    static   WXFMap*  Create (WXFMap*,FrameReader&,WXFMtlBase*);

  private:
    WXFMaterial*  mtl; //���ਠ�, ���஬� ���� �७�������
};

/////////////////////////////////////////////////////////////////////////////////////////////
///���ਠ�
/////////////////////////////////////////////////////////////////////////////////////////////
class   WXFMaterial: public WXFMtlBase, protected WXFBase
{
  friend class WXFMap;
  friend class WXFImport;
  public:
    typedef Ring<WXFMaterial*> MatRing;

    String<63>   name;         //���

    WXFMaterial* parent;       //�।��
    MatRing      children;     //��⮬��

    color3d_t    ambient;      //�����饭��
    color3d_t    diffuse;      //�����
    color3d_t    specular;     //��ࠦ��
    float        shininess;    //�����樥�� "��⠫�筮��"

//���ᠭ�� ����

                 enum Maps {
                   DIFFUSE = 0,
                   OPACITY,
                   BUMP,
                   
                   MAPS_NUM
                 };
    
    WXFMap*      maps [MAPS_NUM]; //�����

  public:
                 ~WXFMaterial ();

  private:
                 WXFMaterial  (WXFImport*);

/////////////////////////////////////////////////////////////////////////////////////////////
///�३��
/////////////////////////////////////////////////////////////////////////////////////////////
    static   WXFMaterial* Create         (WXFMaterial*,FrameReader&,WXFImport*);

  private:
    static   bool         ReadMatParent  (istream&,WXFMaterial*);
    static   bool         ReadMatName    (istream&,WXFMaterial*);
};

#endif