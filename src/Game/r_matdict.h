#ifndef __RENDER_MATERIAL_DICTIONARY__
#define __RENDER_MATERIAL_DICTIONARY__

#include <pool.h>
#include <binfile.h>
#include <misc.h>

////////////////////////////////////////////////////////////////////////////////
///Словарь материалов
////////////////////////////////////////////////////////////////////////////////
class       MatDictionary: public MemObject
{
  public:
                MatDictionary  (Pool*,int num);
     virtual    ~MatDictionary ();

                int     AddMaterial (const char*);
                int     GetMaterial (const char*) const;
   const char*          GetMaterial (int) const;

                int     maxcount    () const { return mNum; }
                int     count       () const { return mPos; }

                void    save        (BinFile*);

  private:
        typedef  char str_t [128];

        int             mNum, mPos;
        str_t*          mMat;  
        Dict<int> mDict;
};

#endif