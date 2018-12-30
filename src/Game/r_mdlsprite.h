#ifndef __RENDER_SPRITE_KEEPER__
#define __RENDER_SPRITE_KEEPER__

#include <math3d.h>
#include <misc.h>
#include "r_mdl3d.h"
#include <fixed\tables.h>

struct   sprite3d_t 
{
  int           texture;
  Angle         start, end;          //after
};

struct   spritemodeldata_t
{
  float         koef;   //�����樥�� ���������� 㣫� (���. ������ �ࠩ�) if (koef<0) then NO_KOEF

  struct lump_t
  {
    ulong       offs;   //���饭�� ������ (�� data [])
    ulong       num;    //������⢮ ����⮢
    ulong       size;   //������ ������
  }text, sprite;

  char     data [];  
};

struct  spritemodel_t: public m3dmodel_t
{
  Angle             angle;          //���� "�७��" �ࠩ� (�⭮�⥫쭮 NORTH z = 0, x = 1)
  float             width, height;  //������� �ࠩ� � ��஢�� ���न����

  ptroffs_t<int>    reindex;

  char              data [];
};

////////////////////////////////////////////////////////////////////////////////
///�࠭���� 3D �ࠩ⮢
////////////////////////////////////////////////////////////////////////////////
class    Sprite3DModelKeeper: public M3DModelKeeper
{
  public:
                Sprite3DModelKeeper  (Pool*,ModelEnv*);
       virtual  ~Sprite3DModelKeeper ();

  protected:
     virtual    int        filetype      () const { return SPRITE_MODEL_ID; }

////////////////////////////////////////////////////////////////////////////////
///����� � �������
////////////////////////////////////////////////////////////////////////////////
    virtual     BOOL       InitModel        (model_t*,MODELDATA,va_list);

////////////////////////////////////////////////////////////////////////////////
///�����ਭ�
////////////////////////////////////////////////////////////////////////////////
    virtual void           render           (const modelnode_t*,const Camera&);  
    virtual void           DrawAlpha        (DWORD param,const Camera&);
    virtual void           RenderShadow     (const model_t*,const lightsrc_t&);

  private:
        MATGROUP        mMatGroup;
};

#endif