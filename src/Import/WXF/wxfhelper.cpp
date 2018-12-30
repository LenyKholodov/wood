#include <pch.h>
#include <import\wxf\wxfhelper.h>
#include <import\wxf\wxfnode.h>
#include <scripts\framerdr.h>

void  WXFHelper::BindCreaters (FrameReader& rdr,WXFNode& node)
{
  rdr.DeclareFrame ("helper");
  rdr.DeclareFrame ("helper.BoxGizmo");
  rdr.DeclareFrame ("helper.SphereGizmo");
  rdr.DeclareFrame ("helper.CylGizmo");
  rdr.DeclareFrame ("helper.Dummy");

  rdr.BindCreater  ("helper.BoxGizmo",WXFHelperBox::Create,&node);
  rdr.BindCreater  ("helper.SphereGizmo",WXFHelperSphere::Create,&node);
  rdr.BindCreater  ("helper.CylGizmo",WXFHelperCylinder::Create,&node);
  rdr.BindCreater  ("helper.Dummy",WXFHelperDummy::Create,&node);
}

WXFHelperSphere*  WXFHelperSphere::Create (WXFHelperSphere* sphere,FrameReader& rdr,WXFNode& node)
{
  if (!sphere)
  {
    node.data.helper = sphere = new (node.GetPool ()) WXFHelperSphere;
    node.type        = WXFNode::HELPER;

    if (!sphere)
      return NULL;

    rdr.BindValue ("radius",sphere->r);
    rdr.BindValue ("center",sphere->c);
  }

  return sphere;
}

WXFHelperBox*  WXFHelperBox::Create (WXFHelperBox* box,FrameReader& rdr,WXFNode& node)
{
  if (!box)
  {
    node.data.helper = box = new (node.GetPool ()) WXFHelperBox;
    node.type        = WXFNode::HELPER;

    if (!box)
      return NULL;

    rdr.BindValue ("dir",box->dir);
    rdr.BindValue ("center",box->c);
  }

  return box;
}

WXFHelperCylinder*  WXFHelperCylinder::Create (WXFHelperCylinder* cyl,FrameReader& rdr,WXFNode& node)
{
  if (!cyl)
  {
    node.data.helper = cyl = new (node.GetPool ()) WXFHelperCylinder;
    node.type        = WXFNode::HELPER;

    if (!cyl)
      return NULL;

    rdr.BindValue ("dir",cyl->dir);
    rdr.BindValue ("center",cyl->c);
    rdr.BindValue ("radius",cyl->r);
    rdr.BindValue ("height",cyl->height);
  }

  return cyl;
}

WXFHelperDummy*  WXFHelperDummy::Create (WXFHelperDummy* dummy,FrameReader& rdr,WXFNode& node)
{
  if (!dummy)
  {
    node.data.helper = dummy = new (node.GetPool ()) WXFHelperDummy;
    node.type        = WXFNode::HELPER;

    if (!dummy)
      return NULL;

    rdr.BindValue ("dir",dummy->dir);
    rdr.BindValue ("center",dummy->c);
  }

  return dummy;
}