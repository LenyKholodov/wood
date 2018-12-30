#include <pch.h>
#include <import\wxf\wxfimp.h>
#include <scripts\framerdr.h>

bool  WXFMaterial::ReadMatParent  (istream& is,WXFMaterial* mtl)
{
  char temp [64];

  if (!ReadString (is,temp))  
    return false;
 
  mtl->parent = mtl->owner->GetMtl (temp);

  if (mtl->parent)
    mtl->parent->children.insert (mtl);

  return true;
}

bool  WXFMaterial::ReadMatName (istream& is,WXFMaterial* mtl)
{
  if (!ReadString (is,mtl->name))  
    return false;
 
  mtl->owner->AddMtl (mtl->name,mtl);

  return true;
}

WXFMaterial* WXFMaterial::Create (WXFMaterial* mtl,FrameReader& rdr,WXFImport* imp)
{
  if (!mtl)
  {
    mtl = new (imp) WXFMaterial (imp);

    if (!mtl)
      return NULL;

    rdr.BindValue    ("diffuse",  mtl->diffuse);
    rdr.BindValue    ("ambient",  mtl->ambient);
    rdr.BindValue    ("specular", mtl->specular);
    rdr.BindValue    ("shiness",  mtl->shininess);
    rdr.BindReader   ("name",     ReadMatName,   mtl);
    rdr.BindReader   ("parent",   ReadMatParent, mtl);
    rdr.DeclareFrame ("DiffuseMap");
    rdr.DeclareFrame ("OpacityMap");
    rdr.DeclareFrame ("BumpMap");
    rdr.BindCreater  ("DiffuseMap",mtl->maps [WXFMaterial::DIFFUSE],WXFMap::Create,mtl);
    rdr.BindCreater  ("OpacityMap",mtl->maps [WXFMaterial::OPACITY],WXFMap::Create,mtl);
    rdr.BindCreater  ("BumpMap",mtl->maps [WXFMaterial::BUMP],WXFMap::Create,mtl);
    rdr.DeclareFrame ("material");   
    rdr.BindCreater  ("material",Create,imp);
  }

  return mtl;  
}

WXFMap* WXFMap::Create (WXFMap* map,FrameReader& rdr,WXFMtlBase* m)
{
  if (!map)
  {
    WXFMap*      p   = NULL;
    WXFMaterial* mtl = NULL;

    switch (m->type)
    {
      case WXFMtlBase::MATERIAL: mtl = (WXFMaterial*)m;        
                                 break;
      case WXFMtlBase::MAP:      mtl  = ((WXFMap*)m)->mtl;  
                                 p    = (WXFMap*)m;
                                 break;
      default:                   return NULL;
    }

    map = new (mtl->GetPool ()) WXFMap (*mtl);

    if (!map)
      return NULL;

    map->parent = p;

    if (map->parent)
      map->parent->maps.insert (map);

    rdr.DeclareFrame ("Generic");
    rdr.BindCreater  ("Generic",Create,map);
    rdr.DeclareFrame ("bitmap");
    rdr.BindString   ("bitmap.bitmap",map->bmp.name);
    rdr.BindValue    ("birmap.u_offset",map->bmp.u_offs);
    rdr.BindValue    ("birmap.v_offset",map->bmp.v_offs);
    rdr.BindValue    ("birmap.u_tiling",map->bmp.u_tiling);
    rdr.BindValue    ("birmap.v_tiling",map->bmp.v_tiling);
  }

  return map; 
}

WXFMaterial::WXFMaterial (WXFImport* imp)
            : WXFMtlBase (MATERIAL),
              WXFBase    (imp),
              children   (GetPool ()), 
              parent     (NULL)
{   
  for (int i=0;i<MAPS_NUM;i++)
    maps [i] = NULL;  
}

WXFMaterial::~WXFMaterial ()
{
  owner->DelMtl (name);

  if (parent)  
    parent->children.erase (this);
}

WXFMap::WXFMap (WXFMaterial& m)
            : WXFMtlBase (MAP),
              parent     (NULL), 
              mtl        (&m), 
              maps       (m.GetPool ())
{   }


WXFMap::~WXFMap ()
{
  if (parent)
  {
    parent->maps.erase (this);
  }  
  else
  {
    for (int i=0;i<WXFMaterial::MAPS_NUM;i++)
      if (mtl->maps [i] == this)
        mtl->maps [i] = NULL;
  }  
}

