#ifndef __RENDER_MULTI_MODEL__
#define __RENDER_MULTI_MODEL__

#include "r_mdl3d.h"
#include "r_meshrender.h"
#include <misc.h>

struct   multimodeldata_t
{
  bound_t  bound;       //��࠭�稢��騥 ⥫�

  struct lump_t
  {
    ulong       offs;   //���饭�� ������ (�� data [])
    ulong       num;    //������⢮ ����⮢
    ulong       size;   //������ ������
  }vertex, normal, facet, mesh, matname;

  char     data [];
};

struct  multimodel_t: public m3dmodel_t
{
  int              num;        

  ptroffs_t<int>   listTable;
  ptroffs_t<int>   reindex;

  char             data [];
};

////////////////////////////////////////////////////////////////////////////////
///�������� ������ (�������筮 SmpModel)
///InitModel: pos: vector3d_t
///ChangeProperties: type:(MODEL_MULT_MATRIX,MODEL_CHANGE_MATRIX), matrix_t
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///�࠭���� ������ ������� (���� ���ਠ�)
////////////////////////////////////////////////////////////////////////////////
class    MultiModelKeeper: public M3DModelKeeper
{
  public:
                        MultiModelKeeper   (Pool*,ModelEnv*,int _MeshesNum);
            virtual     ~MultiModelKeeper  ();

  protected:
     virtual    int        filetype      () const { return MULTI_MODEL_ID; }

////////////////////////////////////////////////////////////////////////////////
///����� � �������
////////////////////////////////////////////////////////////////////////////////
    virtual     BOOL       InitModel        (model_t*,MODELDATA,va_list);

////////////////////////////////////////////////////////////////////////////////
///�����ਭ�
////////////////////////////////////////////////////////////////////////////////
    virtual void           render           (const modelnode_t*,const Camera&);
    virtual void           RenderShadow     (const model_t*,const lightsrc_t&);
    virtual void           DrawAlpha        (DWORD,const Camera&);

  protected:
        MeshRender         mMeshRender;
        MATGROUP           mMatGroup;
};

#endif