#ifndef __CONVERT_SPRITE3D_MODEL__
#define __CONVERT_SPRITE3D_MODEL__

#include "defs.h"
#include <misc.h>

/////////////////////////////////////////////////////////////////////////////////
///Конвертер 3D спрайтов
////////////////////////////////////////////////////////////////////////////////
class  Sprite3DModelConv: public MemObject
{
  public:
                        Sprite3DModelConv  (Pool*,const char*);
    virtual             ~Sprite3DModelConv ();

////////////////////////////////////////////////////////////////////////////////
///Сохранение модели
////////////////////////////////////////////////////////////////////////////////
            void        Save          (const char*);

  protected:                          
            void        compute       (istream&);

  private:
        struct  face_t
        {
          int           text;
          float         start, end;
        };

        struct  name_t  
        {
          char name [128];
        };

        typedef Dict<int>     NDict;
        typedef Ring<name_t>  NRing;
        typedef Ring<face_t>  SRing;

        NRing                           mNameRing;
        SRing                           mSpriteRing;
        NDict                           mNameDict;
        int                             mMCount;
        float                           mGrad;
};

#endif