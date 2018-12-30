#include <pch.h>
#include <render\render.h>
#include <render\screen.h>

///Работа с контекстами

bool  Render::SetContext (RenderContext* context)
{
  static bool init = false;

  if (!context)
  {
    ReleaseContext ();
    return true;
  }  
  else
  {    
    RenderContext* old = mCurrent.context;

    if (!ChangeContext (*context))
    {
      ReleaseContext ();

      if (old) return SetContext (old); //восстанавливаем старый контекст
      else     return false;
    }
    else 
    {
      if (!init)
      {
        extgl_Initialize ();
        init = true;
      }  

      return true;
    }
  }
}

void  Render::ReleaseContext ()
{
  LostContext ();
}

void  Render::LostContext ()
{
  if (mCurrent.context)
  {
    ContextObject* obj = mFirstCO;

    if (obj)
    {
      do
      {
        if (obj->IsBindContext ())
        {
          obj->LostContext ();
          obj->flags.clear (CO_BIND_CONTEXT);
        }  

        obj = obj->next;
      } while (obj != mFirstCO);
    }

    mCurrent.context->UnbindContext ();
    mCurrent.context->mOwner = NULL;
    mCurrent.context         = NULL;
  }

  //обновлять все cuctrl
}

bool  Render::ChangeContext (RenderContext& newContext)
{  
  if (mCurrent.context)
  {
    if (!IsShared (newContext,*mCurrent.context)) //если списки контекстов не расшарены
    {
      LostContext ();
    }
    else
    {
      ContextObject* obj = mFirstCO;

      if (obj)
      {
        do
        {
          if (!obj->IsSharedPossible () && obj->IsBindContext ())
          {
            obj->LostContext ();
            obj->flags.clear (CO_BIND_CONTEXT);
          }  

          obj = obj->next;
        } while (obj != mFirstCO);
      }

      mCurrent.context->UnbindContext ();
      mCurrent.context->mOwner = NULL;
      mCurrent.context         = NULL;
    }
  }

  if (newContext.BindContext (*this))
  {      
    mCurrent.context  = &newContext;
    newContext.mOwner = this;

    if (!mCurrent.context->GetScreen ())
      return false;

      //Обновление на изменение контекста

    ContextObject* obj = mFirstCO;

    if (obj)
    {
      do
      {
        if (!obj->IsBindContext ())
        {
          obj->flags.set     (CO_BIND_CONTEXT);
          obj->ChangeContext ();
        }  

        obj = obj->next;

      } while (obj != mFirstCO);
    }

    return true;    
  } 
  else return false;    
}

bool  Render::IsCurrent (const RenderContext& cntx) const
{
  return mCurrent.context == &cntx; 
}

void  Render::AddContextObject (ContextObject& obj)
{ 
  if (!obj.render)
  {
    if (mFirstCO)
    {
      obj.prev             = mFirstCO->prev;
      obj.next             = mFirstCO;
      mFirstCO->prev->next = &obj;
      mFirstCO->prev       = &obj;      
    }
    else
      mFirstCO = obj.next = obj.prev = &obj;

    obj.render = this;

    if (mCurrent.context)
    {
      obj.ChangeContext ();
      obj.flags.set     (CO_BIND_CONTEXT);
    }  
  }
}

void  Render::DelContextObject (ContextObject& obj)
{
  if (obj.render == this)
  {
    if (mCurrent.context && obj.IsBindContext ())
    {
      obj.LostContext ();
      obj.flags.clear (CO_BIND_CONTEXT);
    }  

    obj.prev->next = obj.next;
    obj.next->prev = obj.prev;    
    obj.render     = NULL;

    if (mFirstCO == &obj)
      mFirstCO = mFirstCO->next;

    if (mFirstCO == &obj)
      mFirstCO = NULL;
  }
}

///Работа с интерфейсами

bool Render::RegisterInterface   (RenderInterface& i)
{
  if (i.mRender != this)
  {
    if (i.mRender)
      i.mRender->UnregisterInterface (i);
    
    if (!i.BindRender (*this))
      return false;

    int         num  = i.GetType ();
    const char* name = i.GetName ();

    if (!mNamedIDict.insert (name,&i))
    {
      i.UnbindRender ();
      return false;
    }  

    if (num >= 0)
    {
      RenderInterface* old = GetInterface (num);

      if (old)  //если интерфейс уже зарегистрирован - update
        UnregisterInterface (*old);

      if (num >= 0 && num < RENDER_FAST_IF_GROUP)
        mFastITable [num] = &i;
      else
      {
        if (!mIDict.insert (num,&i))
        {
          mNamedIDict.erase (name);
          i.UnbindRender    ();

          if (old)
            RegisterInterface (*old);

          return false;  //восстановление при неудаче
        }
      }  
    }

    mIFChangeCounter++;

    i.mRender = this;
    i.mName   = i.GetName ();
    i.mIndex  = i.GetType ();

    AddContextObject (i);
  }

  return true;
}

void Render::UnregisterInterface (RenderInterface& i)
{
  if (i.mRender == this)
  {
    int         num  = i.mIndex;
    const char* name = i.mName;

    DelContextObject (i);
    i.UnbindRender   ();

    mNamedIDict.erase (name);

    if (num >= 0)
    {
      if (num >= 0 && num < RENDER_FAST_IF_GROUP)
        mFastITable [num] = NULL;
      else
        mIDict.erase (num);  
    } 

    mIFChangeCounter++;

    i.mRender = NULL;
    i.mName   = NULL;
    i.mIndex  = -1;
  }
}

RenderInterface* Render::GetInterface (int num) const
{
  if (num >=0 && num < RENDER_FAST_IF_GROUP)
    return mFastITable [num];
  else
  {
    Dict<RenderInterface*>::iter iter = mIDict.find (num);

    return iter.valid () ? iter.data () : NULL;    
  }
}

RenderInterface* Render::GetInterface (const char* name) const
{
  StringDict<RenderInterface*>::iter iter = mNamedIDict.find (name);

  return iter.valid () ? iter.data () : NULL;
}

///Камера

void Render::SetCamera (Camera* cam)
{
  if (cam) mCurrent.camera.SetCurrent   (*cam);
  else     mCurrent.camera.ResetCurrent ();
}

void  Render::ResetCamera ()
{
  mCurrent.camera.ResetCurrent ();
}

Camera* Render::GetCamera () const
{
  return (Camera*)mCurrent.camera.GetCurrent ();
}

///Отрисовка

bool Render::BeginDraw ()
{
  if (!mCurrent.context)
    return false;  

  mCurrent.context->GetScreen ()->BeginDraw ();

  passes.ProcessBefore ();

  return true;  
}

void Render::EndDraw ()
{
  mCurrent.context->GetScreen ()->EndDraw ();

  passes.ProcessAfter ();    
}

void Render::DrawPasses ()
{
  passes.ProcessDrawing ();
}

bool Render::Draw ()
{
  if (BeginDraw ())
  {
    DrawPasses ();
    EndDraw    ();

    return true;
  }
  else return false;
}

///Инициализация / декструкция

Render::Render  ()
       : RenderBaseObject (this),
         mIDict (GetPool ()),
         mNamedIDict (GetPool ())
{
  memset (mFastITable,0,RENDER_FAST_IF_GROUP * sizeof (RenderInterface*));

  mCurrent.context  = NULL;

  mFirstCO          = NULL;
  mIFChangeCounter  = 0;

  RegisterInterface (passes);
  RegisterInterface (mtls);
  RegisterInterface (helpers);
  RegisterInterface (primitives);
  RegisterInterface (text);
}

Render::~Render ()
{
  UnregisterInterface (mtls);
  UnregisterInterface (passes);
  UnregisterInterface (helpers);
  UnregisterInterface (primitives);
  UnregisterInterface (text);

///  SetContext          (NULL); //???
}

