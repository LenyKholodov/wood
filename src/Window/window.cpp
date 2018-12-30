#include <pch.h>
#include <debug.h>

Dict<Window*>* Window::mWindDict = NULL;
Window*        Window::mFuck     = NULL;

LONG WINAPI Window::WndProc (HWND wnd,UINT id,WPARAM wp,LPARAM lp)
{
  D_Try
  {
    Window* window = NULL;

    if (mWindDict) 
      mWindDict->search ((DWORD)wnd,window);

    if (window)
    {
      handler_t   hnd;

      if (!window->mHandDict.search (id,hnd))
        return window->handler (id,wp,lp);

      HandlerFn   fn    = hnd.fn;

      if (fn) return (*fn)(window,id,wp,lp,hnd.param);
      else    return window->handler (id,wp,lp);
    }
    else if (mFuck)
    {
      window        = mFuck;
      window->mHWND = wnd;

      handler_t   hnd;

      if (!window->mHandDict.search (id,hnd))
        return window->handler (id,wp,lp);      

      HandlerFn   fn    = hnd.fn;

      if (fn)  return (*fn)(window,id,wp,lp,hnd.param);
      else     return window->handler (id,wp,lp);
    }
    else      
      return DefWindowProc (wnd,id,wp,lp);
  }
  D_ExcRet ("Window: Error at WndProc ()",DefWindowProc (wnd,id,wp,lp));
}

void Window::Destroy (bool destroyWnd)
{
  mDestroy = true;

  if (mHDC)       ReleaseDC    (mHWND,mHDC);
  if (mDoubleBuf) DeleteObject (mDoubleBuf);
  if (mRealDC)    ReleaseDC    (mHWND,mRealDC);

  mHDC       = NULL;
  mDoubleBuf = NULL;
  mRealDC    = NULL;

  Ring<Window*>::iterator iter  = mWindRing;--iter;
  Ring<Window*>::iterator prev  = iter;--prev;
  int                     count = mWindRing.count ();

  for (int i=0;i<count;i++,iter=prev,prev--)
    delete iter;

  if (mParent) mParent->DelWindow (this);

  if (mWindDict)
      mWindDict->erase ((DWORD)mHWND);

  if (destroyWnd)
    DestroyWindow (mHWND);

  mHWND    = NULL;

  if (!mWindDict->count () && !destroyWnd)
  {
    int res = MessageBox (NULL,
                          "У программы отсутствуют активные окна",
                          "Завершить?",
                          MB_YESNO|MB_DEFBUTTON1
                         );

    if (res == IDYES)
      PostQuitMessage (0);                            
  }            
}

int  Window::WndDestroy (Window* wnd,UINT,WPARAM,LPARAM,DWORD) 
{ 
  if (!wnd->mDestroy)
    wnd->Destroy (false);

  return 0; 
}

LRESULT     Window::handler         (UINT id,WPARAM wp,LPARAM lp)
{
  return DefWindowProc (mHWND,id,wp,lp);
}

BOOL   Window::InsertHandler   (UINT id,HandlerFn fn,DWORD param)
{
  D_Try
  {
    handler_t hnd  = { fn,param };

    return mHandDict.insert (id,hnd);
  }
  D_ExcRet ("Window: Error at InsertHandler () "<<id,FALSE);
}

void   Window::DeleteHandler   (UINT id)
{
  D_Try
  {
    mHandDict.erase (id);
  }
  D_Exc ("Window: Error at DeleteHandler () "<<id);
}

void   Window::Show            (int state)
{
  ShowWindow (mHWND,state);
}

void   Window::ShowCursor      (BOOL state)
{
  ::ShowCursor (state);
}

void   Window::Draw            ()
{  }

Window*  Window::GetWindow  (HWND wnd)
{
  if (mWindDict) 
  {
    Window* w = NULL;

    if (mWindDict->search ((DWORD)wnd,w)) return w;
    else                           return NULL;
  }  
  else return NULL;
}

BOOL   Window::create          (const char* _WndName,const char* _ClassName,DWORD _Style,const rect_t& rect)
{
  D_Try
  {
    if (mHWND) 
    {
      D_Try
      {
        mWindDict->erase ((DWORD)mHWND);
      }
      D_Exception (EXCEPTION_EXECUTE_HANDLER)
      {
      }

      DestroyWindow (mHWND);
    }

    while (mFuck);
    
    mFuck = this;
    mHWND = CreateWindow ( _ClassName, _WndName,
                           _Style,
                           rect.x, rect.y, rect.width, rect.height, 
                           mParent ? mParent->wnd () : NULL, 
                           NULL, instance (), NULL
                         );
    mFuck = NULL;
    GetDC ();

    D_Try
    {
      if (mHWND) mWindDict->insert ((DWORD)mHWND,this);
      else       dout<<"Window: Error CreateWindow () "<<this<<endl;
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
    }

    InsertHandler (WM_DESTROY,WndDestroy);

    ShowWindow ( mHWND, SW_SHOW); //!!
    ShowCursor (FALSE);    //!!
    UpdateWindow ( mHWND ); //!! 

    return mHWND != 0;
  }
  D_ExcRet ("Window: Error at create () "<<this,FALSE);
}

BOOL    Window::AddWindow       (const Window* wnd)
{
  D_Try
  {
    return mWindRing.insert ((Window*)wnd);
  }
  D_ExcRet ("Window: Error at AddWindow () ",FALSE);
}

void    Window::DelWindow       (const Window* wnd)
{
  D_Try
  {
    Ring<Window*>::iterator  iter  = mWindRing;
    int                      count = mWindRing.count ();

    for (int i=0;i<count;i++,iter++)
      if (iter.data () == wnd)
      {
        mWindRing.erase (iter);
        return;
      }
  }
  D_Exc ("Window: Error at DelWindow () ");
}

void        Window::PutPixel       (int x,int y,long color) 
{
  ::SetPixel (GetDC(),x,y,(DWORD)color);
}

long        Window::GetPixel       (int x,int y)
{
  return ::GetPixel (GetDC(),x,y);
}

void        Window::LineTo         (int x,int y)
{
  ::LineTo (GetDC(),x,y);
}

void        Window::Line           (int x1,int y1,int x2,int y2)
{
  MoveTo (x1,y1);
  LineTo (x2,y2);
}

void        Window::MoveTo         (int x,int y)
{
  POINT p;
  ::MoveToEx (GetDC(),x,y,&p);
}

void        Window::GetPos         (int& x,int& y)
{
  POINT p;
  ::GetCurrentPositionEx (GetDC(),&p);
  x = p.x;
  y = p.y;
}

void        Window::FillRect       (int x,int y,int width,int height,HBRUSH brush)
{
  RECT dest;

  dest.left    = x;
  dest.right   = x + width;
  dest.top     = y;
  dest.bottom  = y + height;

  ::FillRect (GetDC(),&dest,brush);
}

void        Window::Cls            (HBRUSH brush)
{
  RECT rect;

  ::GetClientRect (mHWND,&rect);

  FillRect (rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,brush);
}

int         Window::width          () const 
{
  RECT rect;

  ::GetClientRect (mHWND,&rect);

  return rect.right-rect.left;
}

int         Window::height         () const
{
  RECT rect;

  ::GetClientRect (mHWND,&rect);

  return rect.bottom-rect.top;
}

void        Window::Rectangle      (int x,int y,int width,int height)
{
  ::Rectangle (GetDC(),x,y,x+width,y+height);
}

void        Window::OutText        (int x,int y,const char* string)
{
  ::TextOut (GetDC(),x,y,string,strlen (string));
}

HPEN        Window::CreatePen      (const pen_t& pen)
{
  return ::CreatePen (pen.style,pen.width,pen.color);
}

HBRUSH      Window::CreateBrush       (const brush_t& brush)
{
  LOGBRUSH b;

  b.lbStyle   = brush.style;
  b.lbColor   = brush.color;
  b.lbHatch   = brush.hatch;

  return ::CreateBrushIndirect(&b);
}

HFONT      Window::CreateGDIFont        (const gdifont_t& font)
{
  return ::CreateFont (font.height,
                              font.width,
                              0, 0,
                              font.weight,
                              font.flags & gdifont_t::ITALIC,
                              font.flags & gdifont_t::UNDERLINE,
                              font.flags & gdifont_t::STRIKE_OUT,
                              font.charSet,
                              OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS,
                              DEFAULT_QUALITY,
                              DEFAULT_PITCH|FF_DONTCARE,
                              font.name
                             );
}

void    Window::SelectObject (void* obj)
{
  ::SelectObject (GetDC(),obj);
}

void    Window::DeleteObject   (void* obj)
{
  ::DeleteObject (obj);
}

void    Window::SetTextColor (long color)
{
  ::SetTextColor (GetDC(),color);
}

void    Window::SetBkColor (long color)
{
  ::SetBkColor (GetDC(),color);
}

long    Window::GetTextColor ()
{
  return ::GetTextColor (GetDC());
}

long    Window::GetBkColor () 
{
  return ::GetBkColor (GetDC());
}

void    Window::BeginDraw      ()
{
  if (!mDoubleBuf)
  {
    mHDC         = CreateCompatibleDC (mRealDC = mHDC);
    mDoubleBuf   = CreateCompatibleBitmap (mRealDC,width(),height());
    mOldBuf      = (HBITMAP)::SelectObject (mHDC,mDoubleBuf);
  }  
  else
    mOldBuf      = (HBITMAP)::SelectObject (mHDC,mDoubleBuf);    
}

void    Window::EndDraw        ()
{
  if (mDoubleBuf)
  {  
    BitBlt (mRealDC,0,0,width(),height(),mHDC,0,0,SRCCOPY);
    ::SelectObject (mHDC,mOldBuf);
  }
}

HDC Window::GetDC ()
{ 
  return mHDC ? mHDC : mHDC = ::GetDC (mHWND);  
}

void* Window::operator new    (size_t size,Pool* pool)
{
  return MemObject::operator new (size,pool);
}

void  Window::operator delete (void* obj)
{
  Window* wnd = (Window*)obj;

  if (wnd && !wnd->mDestroy)
    MemObject::operator delete (obj);
}

Window::Window  (Pool* pool)
       : MemObject (pool),
         mHWND     (0),
         mHDC      (0),
         mRealDC   (NULL), mDoubleBuf (NULL), mOldBuf (NULL),
         mParent   (NULL),
         mHandDict (pool),
         mInst     (A_GetInstance ()),
         mWindRing (pool), mDestroy (FALSE)
{   }

Window::Window  (Pool* pool,const char* _WndName,const char* _ClassName,DWORD _Style,const rect_t& rect) 
       : MemObject (pool),
         mHWND     (0),
         mHDC      (0),
         mRealDC   (NULL), mDoubleBuf (NULL), mOldBuf (NULL),
         mParent   (NULL),
         mHandDict (pool),
         mInst     (A_GetInstance ()),
         mWindRing (pool), mDestroy (FALSE)
{  
  create (_WndName,_ClassName,_Style,rect);
}

Window::Window  (Window* wnd,const char* _WndName,const char* _ClassName,DWORD _Style,const rect_t& rect) 
       : MemObject (wnd->GetPool()),
         mHWND     (0),
         mHDC      (0),
         mRealDC   (NULL), mDoubleBuf (NULL), mOldBuf (NULL),
         mParent   (wnd),
         mHandDict (wnd->GetPool()),
         mInst     (wnd->instance ()),
         mWindRing (wnd->GetPool()), mDestroy (FALSE)
{  
//  D_Try
//  {        
    create         (_WndName,_ClassName,_Style,rect); 
    wnd->AddWindow (this);
//  }
//  D_Exc ("Window: Error at construct");
}

Window::Window  (Window* window)
       : MemObject (window->GetPool ()),
         mHWND     (0),
         mHDC      (0),
         mRealDC   (NULL), mDoubleBuf (NULL), mOldBuf (NULL),
         mParent   (window),
         mHandDict (window->GetPool ()),
         mInst     (window->instance ()),
         mWindRing (window->GetPool()), mDestroy (FALSE)
{   
  window->AddWindow (this);
}

Window::~Window ()
{   
  if (!mDestroy) 
    Destroy (true);
}
