#ifndef __RENDER_MODEL_KEEPER__
#define __RENDER_MODEL_KEEPER__

#include <stdarg.h>
#include <pool.h>
#include "camera.h"

#include "r_render.h"

class    ModelKeeper;
struct   model_t;
struct   modeldata_t;

struct   mdlmapnode_t
{
  mdlmapnode_t*   nextTile;
  mdlmapnode_t*   prev;
  mdlmapnode_t*   next;
  int             index;

  model_t*        model;
};

struct   model_t
{
  MODELDATA     modelData;        //�ਢ離� � �����
  ModelKeeper*  owner;

  struct Position {
    vector3d_t    pos;            //������ ������
    boundbox_t    box;            //��࠭�稢��騩 ��
    int           tile;           //���⠩�  (����)
    mdlmapnode_t* node;           //�����⥫� �� ���ਯ�� ��ࢮ�� ⠩��
  }pos;

  struct Data {
    size_t        size;           //������ ������
    void*         data;           //�����
  }data;
};

struct  modelnode_t
{
  modelnode_t*  next;
  model_t*      model;
  int           detail;
};

////////////////////////////////////////////////////////////////////////////////
///����ࠪ⭮� �࠭���� �������
////////////////////////////////////////////////////////////////////////////////
class   ModelEnv;
class   ModelPool;
class   ModelKeeper: public Render
{
  friend class ModelDataCache;
  friend class ModelEnv;
  public:
                        ModelKeeper  (Pool*,ModelEnv*);
        virtual         ~ModelKeeper ();

        virtual bool    valid        () const { return mModelRing.valid () && mModelEnv != NULL; }        

////////////////////////////////////////////////////////////////////////////////
///��� 
////////////////////////////////////////////////////////////////////////////////
                int        type () const { return mType; }
   virtual      int        filetype () const { return -1; };

////////////////////////////////////////////////////////////////////////////////
///�����ਭ� �������
////////////////////////////////////////////////////////////////////////////////
                void       RenderScene      (const modelnode_t* node,const Camera&);

////////////////////////////////////////////////////////////////////////////////
///���㧪� ��� �������
////////////////////////////////////////////////////////////////////////////////
    virtual     void       Done             ();

  protected:
////////////////////////////////////////////////////////////////////////////////
///����� � ᯨ᪮� �������
////////////////////////////////////////////////////////////////////////////////
                BOOL       InsertModel      (const model_t*);
                void       ExtractModel     (const model_t*);

////////////////////////////////////////////////////////////////////////////////
///����� � �����ﬨ
////////////////////////////////////////////////////////////////////////////////
    virtual     BOOL       InitModel        (model_t*,MODELDATA,va_list) { return TRUE; }
    virtual     BOOL       InitModel        (const model_t*,model_t*);
    virtual     void       DoneModel        (model_t*) {}
    virtual     void       ChangeProperties (model_t*,va_list) {}

    virtual     void       SetPosition      (model_t*,const vector3d_t&);
    virtual     void       SetBoundBox      (model_t*,const boundbox_t&);

    virtual     const bound_t  GetBound     (model_t*);

                void*      LockModelData    (MODELDATA);
                void       UnlockModelData  (MODELDATA);

                ModelPool* GetModelPool     ();
                ModelEnv*  GetModelEnv      ()  { return mModelEnv; }

////////////////////////////////////////////////////////////////////////////////
///����� � ModelDataCache
////////////////////////////////////////////////////////////////////////////////
                //-1: WRONG_PARAMS;>=0 - model data
    virtual int            GetModelDataSize (va_list src,va_list dest) = 0; 
            
    virtual BOOL           LoadModelData    (void*,size_t,va_list) { return TRUE; }
    virtual BOOL           UnloadModelData  (void*,size_t,va_list) { return TRUE; }

////////////////////////////////////////////////////////////////////////////////
///�����ਭ� �������
////////////////////////////////////////////////////////////////////////////////
    virtual void           render           (const modelnode_t*,const Camera&) = 0;
    virtual void           RenderShadow     (const model_t*,const lightsrc_t&) = 0;

    const   frustum_t&     GetFrustum       () const;
    const   Camera&        GetCamera        () const;

    typedef RingTmpl<model_t*> MdlRing;

  private:
    virtual void           Draw             () {}    
    virtual void           SetVertexBuffer  (mesh_t*) {}

  protected:
        ModelEnv*          mModelEnv;
        MdlRing            mModelRing;
        int                mType;
};

////////////////////////////////////////////////////////////////////////////////
///�஢�ઠ ���४⭮�� ������
////////////////////////////////////////////////////////////////////////////////
BOOL    IsValid (model_t*);

#endif