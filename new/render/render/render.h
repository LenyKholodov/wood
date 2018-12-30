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
        1) Все операции относящиеся к отрисовке корректны только между вызовами
             Render::BeginDraw ()
             Render::EndDraw ()
           иначе их поведение не определено.
         
        2) Render может принадлежать только ОДНОЙ нити,
           данная нить одновременно НЕ может иметь более одного рендера
*/

////////////////////////////////////////////////////////////////////////////////////////////
///Общие функции
////////////////////////////////////////////////////////////////////////////////////////////
class Render;

////////////////////////////////////////////////////////////////////////////////////////////
///Установка пула для ВСЕХ рендеров данной нити
////////////////////////////////////////////////////////////////////////////////////////////
extern void    SetRenderPool (Pool*);   

////////////////////////////////////////////////////////////////////////////////////////////
///Установка текущего рендера для данной нити
////////////////////////////////////////////////////////////////////////////////////////////
extern bool    SetRender     (Render*);
extern Render* GetRender     ();
extern bool    IsCurrent     (const Render*);

/////////////////////////////////////////////////////////////////////////////////////////////
///Ограничения рендера
/////////////////////////////////////////////////////////////////////////////////////////////
enum RenderLimits {
  RENDER_FAST_IF_GROUP          = 2048          //количество интерфейсов попадающих (по номеру) в группу быстрого доступа
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Главный рендер
/////////////////////////////////////////////////////////////////////////////////////////////
class    Render: public RenderBaseObject
{
  friend class Model;
  friend class RenderContext;

  public:
                                Render  ();
     virtual                    ~Render ();

////////////////////////////////////////////////////////////////////////////////////////////

                MatEnv            mtls;           //окружение материалов
                RenderHelpers     helpers;        //рисование хелперов
                RenderPrimitives  primitives;     //рисование примитивов
                PassesScheduler   passes;         //работа с проходами
                RenderText        text;           //вывод текста

/////////////////////////////////////////////////////////////////////////////////////////////
///Работа с текущим контекстом
/////////////////////////////////////////////////////////////////////////////////////////////
                bool            SetContext     (RenderContext* = NULL);
                void            ReleaseContext ();

                RenderContext*  GetContext     () const;
                RenderScreen*   GetScreen      () const;

                bool            IsCurrent      (const RenderContext&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///Добавление контекстно - зависимых объектов
////////////////////////////////////////////////////////////////////////////////////////////
                void            AddContextObject (ContextObject&);
                void            DelContextObject (ContextObject&);

////////////////////////////////////////////////////////////////////////////////////////////
///Работа с интерфейсами
////////////////////////////////////////////////////////////////////////////////////////////
                bool            RegisterInterface   (RenderInterface&); //если уже существует - замена
                void            UnregisterInterface (RenderInterface&);

                RenderInterface* GetInterface       (int)         const;
                RenderInterface* GetInterface       (const char*) const;

////////////////////////////////////////////////////////////////////////////////////////////
///Работа с камерой
////////////////////////////////////////////////////////////////////////////////////////////
                void            SetCamera       (Camera*);
                void            ResetCamera     ();

                Camera*         GetCamera       () const;

/////////////////////////////////////////////////////////////////////////////////////////////
///Отрисовка
/////////////////////////////////////////////////////////////////////////////////////////////
                bool            BeginDraw       ();
                void            EndDraw         ();

                void            DrawPasses      (); //отрисовка проходов

                bool            Draw            (); //BeginDraw|DrawPasses|EndDraw

  private:
////////////////////////////////////////////////////////////////////////////////////////////
///Потеря контекста
////////////////////////////////////////////////////////////////////////////////////////////
                void            LostContext     ();
                bool            ChangeContext   (RenderContext&);

  private:
    struct      current_t
    {
      RenderContext*  context;                //текущий контекст
      CurCtrl         camera;                 //текущая камера
    };

    typedef Dict<RenderInterface*>       IFDict;
    typedef StringDict<RenderInterface*> IFStringDict;

  private:
    current_t         mCurrent;               //текущие установки

    ContextObject*    mFirstCO;               //группа обновляемых контекстных объектов

    IFDict            mIDict;                 //словарь интерфейсов рендеринга
    IFStringDict      mNamedIDict;            //словарь именованых интерфейсов рендеринга
    RenderInterface*  mFastITable [RENDER_FAST_IF_GROUP];      //быстрый доступ к первым интерфейсам
    uint              mIFChangeCounter;       //счётчик обновлений таблицы интерфейсов
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