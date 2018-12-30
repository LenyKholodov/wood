#include <pch.h>
#include <import\wxf\wxfimp.h>
#include <import\wxf\wxfmesh.h>
#include <import\wxf\wxfhelper.h>
#include <scripts\framerdr.h>

bool  WXFNode::ReadNodeName (istream& is,WXFNode* node)
{
  if (!ReadString (is,node->name))  
    return false;
 
  node->owner->AddNode (node->name,node);

  return true;
}

bool    WXFNode::ReadNodeMat (istream& is,WXFNode* node)
{
  char temp [64];

  if (!ReadString (is,temp))  
    return false;

  node->mtl = node->owner->GetMtl (temp);

  return true;
}

bool    WXFNode::ReadNodeParent (istream& is,WXFNode* node)
{
  char temp [64];

  if (!ReadString (is,temp))  
    return false;
 
  node->parent = node->owner->GetNode (temp);

  if (!node->parent)
    node->parent = node->owner->root ();

  node->parent->children.insert (node);

  return true;
}

WXFNode*     WXFNode::Create (WXFNode* node,FrameReader& rdr,WXFImport* imp)
{
  if (!node)
  {
    node = new (imp) WXFNode (imp);

    if (!node)
       return NULL;

    rdr.BindReader   ("name",ReadNodeName,node);
    rdr.BindReader   ("parent",ReadNodeParent,node);
    rdr.BindReader   ("material",ReadNodeMat,node); 
    rdr.BindValue    ("wire_color",node->wireColor);     
    rdr.DeclareFrame ("geometry");
    rdr.DeclareFrame ("geometry.mesh");
    rdr.BindCreater  ("geometry.mesh",WXFMesh::Create,node);

    rdr.DeclareFrame ("transform");
    rdr.DeclareFrame ("transform.matrix");
    rdr.BindArray    ("transform.matrix.row",&node->base [0]);
    
    WXFHelper::BindCreaters (rdr,*node);

    rdr.DeclareFrame ("node");
    rdr.BindCreater  ("node",Create,imp);
    
    return node;
  }
  else
  {
    if (!node->parent)
    {
      node->parent = node->owner->root ();

      node->parent->children.insert (node);
    }
  }

  return node;
}

WXFNode::WXFNode (WXFImport* imp)
       : WXFBase  (imp),
         children (GetPool ())
{
  parent    = NULL;
  type      = NO_ASSIGN;
  mtl       = NULL;
  wireColor = vec3_t (.0f);  
  data.ptr  = NULL;
  base      = matrix_t (1);
}

WXFNode::~WXFNode ()
{
  uint count = children.count ();

  for (int i=0;i<count;i++)
    delete children.GetFirst ();
 
  if (parent)
    parent->children.erase (this);

  if (owner)
    owner->DelNode (name);
}

