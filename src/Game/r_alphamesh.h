#ifndef __RENDER_ALPHA_MESH__
#define __RENDER_ALPHA_MESH__

#include "r_render.h"

////////////////////////////////////////////////////////////////////////////////
///Рендеринг полупрозрачности
////////////////////////////////////////////////////////////////////////////////
class    AlphaMeshRender: public MemObject
{
  public:
                        AlphaMeshRender  (Pool*,int _Num);
       virtual          ~AlphaMeshRender ();

////////////////////////////////////////////////////////////////////////////////
///Добавление полупрозрачности
////////////////////////////////////////////////////////////////////////////////
        void            AddAlpha         (float         dist,
                                          DWORD         param,
                                          Render*       render
                                         );
        
////////////////////////////////////////////////////////////////////////////////
///Рисование
////////////////////////////////////////////////////////////////////////////////
        virtual void    Draw             ();
        void            SetCamera        (const Camera& cam) { mCamera = cam; }

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
        int             count            () const { return mCount; }
        int             pos              () const { return mPos;   }

  private:
    struct alphanode_t;

  private:
        struct alphanode_t
        {
          Render*       render;
          DWORD         param;
          int           next, prev;
          float         dist;
        };

  private:
        alphanode_t*    mTable;
        int             mCount, mPos, mFirst;
        Camera          mCamera;
};

#endif