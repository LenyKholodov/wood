#ifndef __RENDER_MODEL_KEEPER__
#define __RENDER_MODEL_KEEPER__

#include <stdarg.h>
#include <pool.h>

#include "defs.h"

class    ModelKeeper;

struct   model_t
{
  int           type;           //��� 
  vector3d_t    pos;            //������ ������
  matrix_t      m;              //����� �८�ࠧ������ ��� ������
  boundbox_t    box;            //��࠭�稢��騩 ��
  ModelKeeper*  owner;          //��������
  int           tile;           //���� ������
  model_t       *nextModel, 
                *prevModel;     //���᮪ ������� � �祩�� �����
  void*         memNode;        //������� � �㫥
  size_t        propSize,
                dataSize;       //�������
  void*         prop;          //�����⥫� �� ᢮��⢠
  void*         data;          //�����⥫� �� �����
};

typedef model_t*        MODEL;

struct  modelnode_t
{
  modelnode_t*  next;
  MODEL         model;
  int           detail;
};

////////////////////////////////////////////////////////////////////////////////
///����ࠪ⭮� �࠭���� �������
////////////////////////////////////////////////////////////////////////////////
class   ModelEnv;
class   ModelKeeper: public MemObject 
{
  public:
                        ModelKeeper  (Pool*,ModelEnv*);
        virtual         ~ModelKeeper ();

////////////////////////////////////////////////////////////////////////////////
///����� � �����ﬨ
////////////////////////////////////////////////////////////////////////////////
                BOOL       InitModel        (MODEL,va_list);
                BOOL       InitModel        (const MODEL,MODEL);
                void       DoneModel        (MODEL);
    virtual     void       ChangeProperties (MODEL,va_list) {}

    virtual     void       SetPosition      (MODEL,const vector3d_t&);
    virtual     void       SetTransMatrix   (MODEL,const matrix_t&);        
    virtual     void       SetBoundBox      (MODEL,const boundbox_t&);

////////////////////////////////////////////////////////////////////////////////
///�����ਭ�
////////////////////////////////////////////////////////////////////////////////
                void       RenderScene      (modelnode_t* node);

////////////////////////////////////////////////////////////////////////////////
///�����
////////////////////////////////////////////////////////////////////////////////
                void       Done             ();

  protected:
            BOOL           AllocModelData   (MODEL,size_t _PropSize,size_t _DataSize);
            void           FreeModelData    (MODEL model);

    virtual BOOL           _InitModel       (MODEL,va_list) { return TRUE; }
    virtual BOOL           _InitModel       (const MODEL,MODEL);
    virtual void           _DoneModel       (MODEL) {}

    virtual void           render           (modelnode_t*) = 0;

    typedef RingTmpl<MODEL> MdlRing;

  protected:
        ModelEnv*          mModelEnv;
        MdlRing            mModelRing;
};

////////////////////////////////////////////////////////////////////////////////
///Test keeper
////////////////////////////////////////////////////////////////////////////////
class   TestKeeper: public ModelKeeper
{
  public:
                        TestKeeper  (Pool*,ModelEnv*);
        virtual         ~TestKeeper ();

////////////////////////////////////////////////////////////////////////////////
///����� � �����ﬨ
////////////////////////////////////////////////////////////////////////////////
    virtual     void       SetPosition      (MODEL,const vector3d_t&);

  protected:
    virtual BOOL           _InitModel       (MODEL,va_list);

    virtual void           render           (modelnode_t*);
};

#endif