#include <pch.h>

#include "geom.h"
#include "mesh.h"
#include "node.h"

GeomExport::GeomExport (INodeExport& own)
           : BaseExport ("geometry",own),
             node       (own.GetNode ())
{
  ObjectState os = node.EvalWorldState (info.staticFrame); 

  // Targets are actually geomobjects, but we will export them
  // from the camera and light objects, so we skip them here.
  if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
     return;
 
//  ExportNodeHeader(node, ID_GEOMETRY, indentLevel);  
//  ExportNodeTM(node, indentLevel);

  if (check (EXPORT_MESH))       
    MeshExport (*this,node,info.staticFrame);  

  // Node properties (only for geomobjects)
//  fprintf(pStream, "%s\t%s %d\n", indent.data(), ID_PROP_MOTIONBLUR, node.MotBlur());
//  fprintf(pStream, "%s\t%s %d\n", indent.data(), ID_PROP_CASTSHADOW, node.CastShadows());
//  fprintf(pStream, "%s\t%s %d\n", indent.data(), ID_PROP_RECVSHADOW, node.RcvShadows());

//  gunWriteToFile(pBinFile, GUN_PROP_MOTIONBLUR, node.MotBlur());
//  gunWriteToFile(pBinFile, GUN_PROP_CASTSHADOW, node.CastShadows());
//  gunWriteToFile(pBinFile, GUN_PROP_RECVSHADOW, node.RcvShadows());

//  if (check (EXPORT_ANIM))
//     ExportAnimKeys(node, indentLevel);

/*
  // Export the visibility track
  Control* visCont = node.GetVisController();
  if (visCont) {
          fprintf(pStream, "%s\t%s {\n", indent.data(), ID_VISIBILITY_TRACK);
          DumpFloatKeys(visCont, indentLevel);
          fprintf(pStream, "\t}\n");
  }

  if (GetIncludeMtl()) {
          ExportMaterial(node, indentLevel);
  }

  if (GetIncludeMeshAnim()) {
          ExportAnimMesh(node, indentLevel);
  }
  
  if (GetIncludeIKJoints()) {
          ExportIKJoints(node, indentLevel);
  }
  
  fprintf(pStream,"%s}\n", indent.data());
  */      
}
