#ifndef __MAX_GEOMETRY_EXPORT__
#define __MAX_GEOMETRY_EXPORT__

#include "export.h"

class INodeExport;

/////////////////////////////////////////////////////////////////////////////////////////////
///Экспорт геометрии
/////////////////////////////////////////////////////////////////////////////////////////////
class GeomExport: public BaseExport
{
  public:
                GeomExport (INodeExport&);

         INode& GetNode () const { return node; }

  private:
        INode&  node;
};

#endif