#include <pch.h>
#include <gizmo.h>
#include <gizmoimp.h>
#include <dummy.h>

#include "helper.h"
#include "node.h"

void    HelperExport::ExportCylinder ()
{
  CylGizmoObject& cyl = (CylGizmoObject&)obj;
  Box3            box;
  Matrix3         x   (1);

  cyl.GetBoundBox (x,info.staticFrame,box);

  ostream& os = dump;

  static Point3 ort (0,0,1);

  os<<"radius\t"<<(box.pmax.x-box.pmin.x) / 2.0<<endline;
  os<<"height\t"<<(box.pmax.z-box.pmin.z)<<endline;

  x = node.GetNodeTM (info.staticFrame);

  cyl.GetBoundBox (x,info.staticFrame,box);  

  os<<"center\t"<<(box.pmax+box.pmin)/2.0f<<endline;
  os<<"dir\t"<<x.VectorTransform (ort)<<endline;
}

void    HelperExport::ExportSphere ()
{
  SphereGizmoObject& s = (SphereGizmoObject&)obj;
  Box3               box;
  Matrix3            x   (1);

  s.GetBoundBox (x,info.staticFrame,box);

  ostream& os = dump;

  os<<"radius\t"<<(box.pmax.z-box.pmin.z)/2.0f<<endline;

  x = node.GetNodeTM (info.staticFrame);

  s.GetBoundBox (x,info.staticFrame,box);  

  os<<"center\t"<<(box.pmax+box.pmin)/2.0f<<endline;
}

void    HelperExport::ExportBox ()
{
  Box3            box;
  BoxGizmoObject& cyl = (BoxGizmoObject&)obj;
  Matrix3         x (1);

  cyl.GetBoundBox (x,info.staticFrame,box);  

  x  = node.GetNodeTM (info.staticFrame);

  Point3   dir = (x*box.pmax-x*box.pmin)/2.0f;  

  cyl.GetBoundBox (x,info.staticFrame,box);  

  x.VectorTransform (dir);

  ostream& os  = dump;

  os<<"center\t"<<(box.pmax+box.pmin)/2.0f<<endline;
  os<<"dir\t"<<dir<<endline;
}

void    HelperExport::ExportDummy ()
{
  Box3            box;
  DummyObject& dummy = (DummyObject&)obj;
  Matrix3         x (1);

  dummy.GetDeformBBox (info.staticFrame,box,&x);

  x  = node.GetNodeTM (info.staticFrame);

  Point3   dir = (x*box.pmax-x*box.pmin)/2.0f;

  dummy.GetDeformBBox (info.staticFrame,box,&x);

  x.VectorTransform (dir);

  dump<<"center\t"<<(box.pmax+box.pmin)/2.0f<<endline;
  dump<<"dir\t"<<dir<<endline;
}

HelperExport::HelperExport (INodeExport& inode)
             : BaseExport ("helper",inode),
               node (inode.GetNode ()),
               obj  ((HelperObject&)*inode.GetNode ().EvalWorldState (0).obj)
{
  TSTR className;

  obj.GetClassName (className);

  if (className)
    OpenFrame (className);

  if (obj.ClassID () == CYLGIZMO_CLASSID)    ExportCylinder ();
  if (obj.ClassID () == SPHEREGIZMO_CLASSID) ExportSphere   ();
  if (obj.ClassID () == BOXGIZMO_CLASSID)    ExportBox      ();
  if (obj.ClassID () == dummyClassID)        ExportDummy    ();

  if (className)
    CloseFrame ();
}