#include <pch.h>

#include "node.h"
#include "geom.h"
#include "anim.h"
#include "helper.h"

void INodeExport::ExportNodeTM ()
{
  Matrix3 pivot = mNode->GetNodeTM (info.staticFrame);

  OpenFrame ("transform");

  dump<<"name\t'"<<mNode->GetName ()<<"'"<<endline;

  DWORD iFlags = mNode->GetTMController()->GetInheritanceFlags();

  dump<<"inherit_pos"
      <<"\t"<<(INHERIT_POS_X & iFlags ? 1 : 0)
      <<"\t"<<(INHERIT_POS_Y & iFlags ? 1 : 0)
      <<"\t"<<(INHERIT_POS_Z & iFlags ? 1 : 0)
      <<endline;

  dump<<"inherit_rot"
      <<"\t"<<(INHERIT_ROT_X & iFlags ? 1 : 0)
      <<"\t"<<(INHERIT_ROT_Y & iFlags ? 1 : 0)
      <<"\t"<<(INHERIT_ROT_Z & iFlags ? 1 : 0)
      <<endline;

  dump<<"inherit_scale"
      <<"\t"<<(INHERIT_SCL_X & iFlags ? 1 : 0)
      <<"\t"<<(INHERIT_SCL_Y & iFlags ? 1 : 0)
      <<"\t"<<(INHERIT_SCL_Z & iFlags ? 1 : 0)
      <<endline;

  OpenFrame ("matrix");

  dump<<pivot;

  CloseFrame ();

  AffineParts ap;
  float       rotAngle;
  Point3      rotAxis;
  float       scaleAngle;
  Point3      scaleAxis;
  
  decomp_affine (pivot,&ap);

  AngAxisFromQ (ap.q,&rotAngle,rotAxis);
  AngAxisFromQ (ap.u,&scaleAngle,scaleAxis);

  dump<<"pos\t\t"<<ap.t<<endline;
  dump<<"rot_axis\t"<<rotAxis<<endline;
  dump<<"rot_angle\t"<<rotAngle<<endline;;
  dump<<"scale\t\t"<<ap.k<<endline;
  dump<<"scale_axis\t"<<scaleAxis<<endline;
  dump<<"scale_angle\t"<<scaleAngle<<endline;

  CloseFrame ();
}

const char* INodeExport::GetType   (int type) const
{
  static Str128 buf;

  switch (type)
  {
    case GEOMOBJECT_CLASS_ID:  
      if (check (EXPORT_GEOMETRY))
        buf = "geometry"; 
      break;
    case CAMERA_CLASS_ID:      
      if (check (EXPORT_CAMERAS))
        buf = "camera";   
      break;
    case LIGHT_CLASS_ID:       
      if (check (EXPORT_LIGHTS))
        buf = "light";    
      break;
    case SHAPE_CLASS_ID:       
      if (check (EXPORT_SHAPES))
        buf = "shape";    
      break;
    case HELPER_CLASS_ID:      
      if (check (EXPORT_HELPERS))
        buf = "helper";   
      break;
    default:                   
      buf = "";
  }

  return buf [0] ? buf : NULL;
}

void    INodeExport::ExportHeader (bool& openNode)
{
  openNode = false;

//  if (mNode->IsGroupHead()) 
//  {
//    OpenFrame ("group");
//    openNode = true;
//  }

  ObjectState os = mNode->EvalWorldState(0); 

  if (os.obj) 
  {
    const char* nodeName = GetType (os.obj->SuperClassID ());

    if (nodeName)
    {
//      OpenFrame (nodeName);
      OpenFrame ("node");
      openNode = true;
    }
  }  

  if (openNode)
  {  
    dump<<"name\t'"<<mNode->GetName()<<"'"<<endline;
    
    if (mNode->GetParentNode () && !mNode->GetParentNode ()->IsRootNode ())
      dump<<"parent\t'"<<mNode->GetParentNode ()->GetName ()<<"'"<<endline;

    ExportNodeTM ();

    if (info.root->GetMtlID (mNode->GetMtl ()) != -1)
//      dump<<"material "<<info.root->GetMtlID (mNode->GetMtl ())<<endline;
      dump<<"material\t'"<<mNode->GetMtl ()->GetName ()<<"'"<<endline;
    else
    {
      Color col (GetRValue (mNode->GetWireColor()) / 255.0,
                 GetGValue (mNode->GetWireColor()) / 255.0,
                 GetBValue (mNode->GetWireColor()) / 255.0);

      dump<<"wire_color\t"<<col<<endline;
    }
  }  
}

INodeExport::INodeExport  (INodeExport& node,INode* inode)
            : BaseExport (NULL,node),
              mNode      (inode)
{
  DoExport ();
}

INodeExport::INodeExport  (RootExport& root)
            : BaseExport (NULL,root),
              mNode      (NULL)
{
  mNode = info.i->GetRootNode ();

  DoExport ();
}

void  INodeExport::DoExport     ()
{
  bool openNode;

  ExportHeader (openNode);

  if (openNode)
  {
    ObjectState os = mNode->EvalWorldState(0); 

    if (os.obj) 
    {
      switch(os.obj->SuperClassID()) 
      {
        case GEOMOBJECT_CLASS_ID: 
          if (check (EXPORT_GEOMETRY)) GeomExport (*this); 
          break;
  //        case CAMERA_CLASS_ID:     CameraExport (mNode); break;
  //        case LIGHT_CLASS_ID:      LightExport  (mNode); break;
  //        case SHAPE_CLASS_ID:      ShapeExport  (mNode); break;
          case HELPER_CLASS_ID:     
            if (check (EXPORT_HELPERS))
              HelperExport (*this); 
            break;
      }
    }

    if (check (EXPORT_ANIM_KEYS))  
      AnimKeysExport (*this);


//    CloseFrame ();  
  }

  info.curStep++;

  info.i->ProgressUpdate ((int)((float)info.curStep/info.totalSteps*100.0f)); 

  for (int i=0;i<mNode->NumberOfChildren ();i++) 
    if (!check (EXPORT_SELECTED) || mNode->GetChildNode (i)->Selected ())
      INodeExport node (*this,mNode->GetChildNode (i));        

  if (openNode)  
    CloseFrame ();  
}
