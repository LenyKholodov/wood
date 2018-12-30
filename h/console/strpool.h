#ifndef __CONSOLE_STRING_POOL__
#define __CONSOLE_STRING_POOL__

#include <pool.h>

////////////////////////////////////////////////////////////////////////////////
///Пул строк
////////////////////////////////////////////////////////////////////////////////
class    StringPool: public DefragPool
{
  public:
                        StringPool  (Pool*,size_t);

          char*         alloc   (size_t,char**);
          char*         realloc (size_t,char**);

  protected:
                        DefragPool::defrag;
                        DefragPool::alloc;
};

#endif