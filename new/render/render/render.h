#ifndef __RENDER_RENDER__
#define __RENDER_RENDER__

#include <render\camera.h>
#include <render\context.h>
#include <render\node.h>
#include <render\model.h>

#include <render\interface\passes.h>
#include <render\interface\matenv.h>
#include <render\interface\helpers.h>
#include <render\interface\text.h>
#include <render\interface\primitives.h>
//#include <render\interface\buffers.h>

class RenderContextCtrl
{
  public:

  private:
};

/*
        1) ��� �������� ����������� � ��������� ��������� ������ ����� ��������
             Render::BeginDraw ()
             Render::EndDraw ()
           ����� �� ��������� �� ����������.
         
        2) Render ����� ������������ ������ ����� ����,
           ������ ���� ������������ �� ����� ����� ����� ������ �������
*/

////////////////////////////////////////////////////////////////////////////////////////////
///����� �������
////////////////////////////////////////////////////////////////////////////////////////////
class Render;

////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� ��� ���� �������� ������ ����
////////////////////////////////////////////////////////////////////////////////////////////
extern void    SetRenderPool (Pool*);   

////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ������� ��� ������ ����
////////////////////////////////////////////////////////////////////////////////////////////
extern bool    SetRender     (Render*);
extern Render* GetRender     ();
extern bool    IsCurrent     (const Render*);

/////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������
/////////////////////////////////////////////////////////////////////////////////////////////
enum RenderLimits {
  RENDER_FAST_IF_GROUP          = 2048          //���������� ����������� ���������� (�� ������) � ������ �������� �������
};

/////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
/////////////////////////////////////////////////////////////////////////////////////////////
class    Render: public RenderBaseObject
{
  friend class Model;
  friend class RenderContext;

  public:
                                Render  ();
     virtual                    ~Render ();

////////////////////////////////////////////////////////////////////////////////////////////

                MatEnv            mtls;           //��������� ����������
                RenderHelpers     helpers;        //��������� ��������
                RenderPrimitives  primitives;     //��������� ����������
                PassesScheduler   passes;         //������ � ���������
                RenderText        text;           //����� ������

/////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� ����������
/////////////////////////////////////////////////////////////////////////////////////////////
                bool            SetContext     (RenderContext* = NULL);
                void            ReleaseContext ();

                RenderContext*  GetContext     () const;
                RenderScreen*   GetScreen      () const;

                bool            IsCurrent      (const RenderContext&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� - ��������� ��������
////////////////////////////////////////////////////////////////////////////////////////////
                void            AddContextObject (ContextObject&);
                void            DelContextObject (ContextObject&);

////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������������
////////////////////////////////////////////////////////////////////////////////////////////
                bool            RegisterInterface   (RenderInterface&); //���� ��� ���������� - ������
                void            UnregisterInterface (RenderInterface&);

                RenderInterface* GetInterface       (int)         const;
                RenderInterface* GetInterface       (const char*) const;

////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
////////////////////////////////////////////////////////////////////////////////////////////
                void            SetCamera       (Camera*);
                void            ResetCamera     ();

                Camera*         GetCamera       () const;

/////////////////////////////////////////////////////////////////////////////////////////////
///���������
/////////////////////////////////////////////////////////////////////////////////////////////
                bool            BeginDraw       ();
                void            EndDraw         ();

                void            DrawPasses      (); //��������� ��������

                bool            Draw            (); //BeginDraw|DrawPasses|EndDraw

  private:
////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������
////////////////////////////////////////////////////////////////////////////////////////////
                void            LostContext     ();
                bool            ChangeContext   (RenderContext&);

  private:
    struct      current_t
    {
      RenderContext*  context;                //������� ��������
      CurCtrl         camera;                 //������� ������
    };

    typedef Dict<RenderInterface*>       IFDict;
    typedef StringDict<RenderInterface*> IFStringDict;

  private:
    current_t         mCurrent;               //������� ���������

    ContextObject*    mFirstCO;               //������ ����������� ����������� ��������

    IFDict            mIDict;                 //������� ����������� ����������
    IFStringDict      mNamedIDict;            //������� ���������� ����������� ����������
    RenderInterface*  mFastITable [RENDER_FAST_IF_GROUP];      //������� ������ � ������ �����������
    uint              mIFChangeCounter;       //������� ���������� ������� �����������
};

//////////////////////////INLINES///////////////////////////////////////////////////////////

inline RenderContext*  Render::GetContext () const
{
  return mCurrent.context;
}

inline RenderScreen* Render::GetScreen () const
{
  return mCurrent.context ? mCurrent.context->GetScreen () : NULL;
}

#endif