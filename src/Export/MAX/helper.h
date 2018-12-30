#ifndef __MAX_EXPORT_HELPERS__
#define __MAX_EXPORT_HELPERS__

#include "export.h"

class    INodeExport;

/////////////////////////////////////////////////////////////////////////////////////////////
///��ᯮ�� helper'��
/////////////////////////////////////////////////////////////////////////////////////////////
class   HelperExport: public BaseExport
{
  public:
                HelperExport (INodeExport&);

  private:
        void    ExportCylinder ();
        void    ExportSphere   ();
        void    ExportBox      ();
        void    ExportDummy    ();

  private:
        INode&   node;
        HelperObject&  obj;
};

#endif