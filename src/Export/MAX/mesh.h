#ifndef __MAX_MESH_EXPORT__
#define __MAX_MESH_EXPORT__

#include "export.h"

class GeomExport;

/////////////////////////////////////////////////////////////////////////////////////////////
///ªα―®ΰβ ¬νθ 
/////////////////////////////////////////////////////////////////////////////////////////////
class MeshExport: public BaseExport
{
  public:
                        MeshExport  (BaseExport&,INode&,TimeValue);

  private:
         void           DoExport     ();
         void           BuildNormals ();

         void           VertexesExport  (Mesh&,Matrix3&,Matrix3&,AffineParts&);
         void           FacesExport     (Mesh&,int* order);
         void           VertexColorsExport (Mesh&,int* order);
         void           NormalsExport   (Mesh&,int* order);
         void           ExportExtChannels ();

         void           TexCoordsExport (Mesh&,Mtl&,int* order);
         void           ExportChannel   (Mesh& mesh,int* order,int mp);

         void           MakeFaceUV   (Face&,Point3*);
         Point3         GetVertexNormal (Mesh* mesh,int faceNo,RVertex* rv);

         TriObject*     GetTriObject (bool& del);
         bool           IsNeg        (Matrix3&);

  private:
        INode&      node;
        TimeValue   time;
};

#endif