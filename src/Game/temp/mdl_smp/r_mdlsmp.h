#ifndef __RENDER_SIMPLE_MODEL__
#define __RENDER_SIMPLE_MODEL__

#include "r_mdl3d.h"
#include "r_meshrender.h"

struct   smpmodeldata_t
{
  int      material;    //���ਠ�
  bound_t  bound;       //��࠭�稢��騥 ⥫�
  int      type;        //MODEL_TRIANGLES_DATA / MODEL_STRIP_DATA

  struct lump_t
  {
    ulong       offs;   //���饭�� ������ (�� data [])
    ulong       num;    //������⢮ ����⮢
    ulong       size;   //������ ������
  }vertex, normal, facet, mat, strip, tcoord;

  char     data [];
};

struct  smpmodel_t: public m3dmodel_t
{
  textmat_t     mat;            //���ਠ�
  int           texture;        //������ ��।��񭭮� ⥪�����
  int           list;           //������ ��।��񭭮�� ᯨ᪠
  ulong         crc;            //CRC ����� ⥪�����
};

////////////////////////////////////////////////////////////////////////////////
///�������� ������
///InitModel: pos: vector3d_t
///ChangeProperties: type:(MODEL_MULT_MATRIX,MODEL_CHANGE_MATRIX), matrix_t
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///�࠭���� ������ ������� (���� ���ਠ�)
////////////////////////////////////////////////////////////////////////////////
class    SimpleModelKeeper: public M3DModelKeeper
{
  public:
                        SimpleModelKeeper   (Pool*,ModelEnv*,int _MeshesNum);
            virtual     ~SimpleModelKeeper  ();

  protected:
     virtual    int        filetype      () const { return SMP_MODEL_ID; }

////////////////////////////////////////////////////////////////////////////////
///����� � �������
////////////////////////////////////////////////////////////////////////////////
    virtual     BOOL       InitModel        (model_t*,MODELDATA,va_list);

////////////////////////////////////////////////////////////////////////////////
///�����ਭ�
////////////////////////////////////////////////////////////////////////////////
    virtual void           render           (const modelnode_t*,const Camera&);
    virtual void           RenderShadow     (const model_t*);
//    virtual void           DrawAlpha        (DWORD param = 0);

  protected:
        MeshRender         mMeshRender;
};

#endif