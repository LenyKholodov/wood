#ifndef __ENUM_MAX_NODE__
#define __ENUM_MAX_NODE__

#include "export.h"

class    MeshExport;
class    NodeExport;

/////////////////////////////////////////////////////////////////////////////////////////////
///Узел в дереве сцены
/////////////////////////////////////////////////////////////////////////////////////////////
class INodeExport: public BaseExport
{
  public:
                        INodeExport  (RootExport&);  //for root
                        INodeExport  (INodeExport&,INode*); //for children

                INode&  GetNode () { return *mNode; }        

  private:
                void    DoExport      ();
                void    ExportHeader  ();
                void    ExportNodeTM  ();
                void    ExportHeader  (bool&);

                const char* GetType   (int) const;

  private:
        INode*          mNode;
};

#endif