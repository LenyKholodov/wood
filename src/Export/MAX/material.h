#ifndef __MAX_EXPORT_MATERIALS__
#define __MAX_EXPORT_MATERIALS__

#include "export.h"

/////////////////////////////////////////////////////////////////////////////////////////////
///Экспорт материала
/////////////////////////////////////////////////////////////////////////////////////////////
class    MaterialExport: public BaseExport
{
  public:
                MaterialExport (RootExport&,Mtl*,int id);
                MaterialExport (MaterialExport&,Mtl*,int id);

  private:
                void    DoExport      ();
                void    ExportTexture (Texmap*,const char*,float);
                void    ExportUVGen   (StdUVGen*);
                const char* GetMapID  (Class_ID cid,int subNo);

  private:
        Mtl&       mtl, *parent;
        TimeValue  time;
};

#endif