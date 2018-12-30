#ifndef __RENDER_SIMPLE_ANIM_MODEL__
#define __RENDER_SIMPLE_ANIM_MODEL__

#include "r_mdl3d.h"
#include "r_meshrender.h"

#include <graph\x3f.h>

struct  animnode_t
{        
  bound_t       bound;          //��࠭�稢��饥 ⥫� ��� ��ꥪ�
  int           parent;         //����⥫�
  int           node;           //���� 㧥�
  int           next;           //������騩 ��ꥪ� �� ������ �஢�� ����娨

  struct  group3d_t
  {
    nodelist_t    vertex;
    nodelist_t    facet;
    nodelist_t    normal;
  };

  struct 
  {
    int    flags;      //Rotate|Translate|Scale;
    int    node;       //������ ������ � �����樥� ��� ��ꥪ�
  }anim;  
};

struct   smpmodeldata_t
{
  int      material;    //���ਠ�
  int      root;        //��୥��� 㧥�
  bound_t  bound;       //��࠭�稢��騥 ⥫�

  struct lump_t
  {
    ulong       offs;   //���饭�� ������ (�� data [])
    ulong       num;    //������⢮ ����⮢
    ulong       size;   //������ ������
  }vertex, normal, facet, mat;

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
     virtual    uint       filetype      () const { return SMP_MODEL_ID; }

////////////////////////////////////////////////////////////////////////////////
///����� � �������
////////////////////////////////////////////////////////////////////////////////
    virtual     BOOL       InitModel        (model_t*,MODELDATA,va_list);

////////////////////////////////////////////////////////////////////////////////
///�����ਭ�
////////////////////////////////////////////////////////////////////////////////
    virtual void           render           (const modelnode_t*,const Camera&);

  protected:
        MeshRender         mMeshRender;
};

#endif