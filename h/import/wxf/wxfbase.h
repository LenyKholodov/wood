#ifndef __WXF_IMPORT_BASE__
#define __WXF_IMPORT_BASE__

#include <graph\grtypes.h>
#include <memory\memory.h>

class    WXFImport;

/////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс импорта
/////////////////////////////////////////////////////////////////////////////////////////////
class    WXFBase
{
  public:
        void*   operator new     (size_t,WXFImport*);
        void    operator delete  (void*);

                WXFBase (WXFImport* imp);

        Pool*   GetPool () const;

  protected:
        WXFImport*      owner;
};

#endif