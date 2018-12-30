#include <pch.h>
#include <graph\wxfimp.h>
#include <scripts\framerdr.h>

bool  WXFImport::load   ()
{
  if (!Resource::load ())
    return false;

  mRoot     = new (this) WXFNode (this);
  mMatDict  = ext_new    StringDict<WXFMaterial*> (GetPool ());
  mNodeDict = ext_new    StringDict<WXFNode*> (GetPool ());

  if (!mRoot || !mMatDict || !mNodeDict)
    return false;

  FrameReader rdr;

  rdr.DeclareFrame ("material");
  rdr.DeclareFrame ("node");

  rdr.BindCreater  ("material",WXFMaterial::Create,this);
  rdr.BindCreater  ("node",WXFNode::Create,this);

  rdr.ReadStream   (mFileName);

  return true;
}

void  WXFImport::unload ()
{
  Resource::unload ();
}

WXFMaterial* WXFImport::GetMtl  (const char* name) const
{
  WXFMaterial* mat = NULL;
  
  if (mMatDict)
    mMatDict->find (name,mat);

  return mat;
}

WXFNode*     WXFImport::GetNode (const char* name) const
{
  WXFNode* node = NULL;

  if (mNodeDict)
    mNodeDict->find (name,node);

  return node;
}

void        WXFImport::AddMtl (const char* name,WXFMaterial* mtl)
{
  if (mMatDict) mMatDict->insert (name,mtl);
}

void        WXFImport::DelMtl (const char* name)
{
  if (mMatDict) mMatDict->erase (name);
}

void        WXFImport::AddNode (const char* name,WXFNode* mtl)
{
  if (mNodeDict) mNodeDict->insert (name,mtl);
}

void        WXFImport::DelNode (const char* name)
{
  if (mNodeDict) mNodeDict->erase (name);
}

WXFImport::WXFImport  (const char* fname,ResourceGroup* group)
          : Resource  (group),
            mFileName (fname),
            mRoot     (NULL),
            mNodeDict (NULL), mMatDict (NULL)
            
{   }

WXFImport::~WXFImport ()
{
//???  unload ();
}
