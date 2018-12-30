#include <pch.h>
#include <import\wxf\wxfmtl.h>

bool  WXFMaterial::ReadMatParent  (istream& is,WXFMaterial* mtl)
{
  char temp [64];

  if (!ReadString (is,temp))  
    return false;
 
  if (mtl->parent = mtl->owner->GetMtl (temp))
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

WXFMaterial* WXFMaterial::create (WXFMaterial* mtl,FrameReader& rdr,WXFImport* imp)
{
  if (!mtl)
  {
    mtl = new (imp->GetPool ()) WXFMaterial (imp);

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
    rdr.BindCreater  ("DiffuseMap",mtl->maps [WXFMaterial::DIFFUSE],CreateMap,mtl);
    rdr.BindCreater  ("OpacityMap",mtl->maps [WXFMaterial::OPACITY],CreateMap,mtl);
    rdr.BindCreater  ("BumpMap",mtl->maps [WXFMaterial::BUMP],CreateMap,mtl);
    rdr.DeclareFrame ("material");   
    rdr.BindCreater  ("material",CreateMaterial,imp);
  }

  return mtl;  
}

WXFMap* WXFMap::create (WXFMap* map,FrameReader& rdr,WXFMtlBase* m)
{
  if (!map)
  {
    WXFMap*      p   = NULL;
    WXFMaterial* mtl = NULL;

    switch (m->type)
    {
      case WXFMtlBase::MATERIAL: mtl = (WXFMaterial*)m;        
                                 break;
      case WXFMtlBase::TEXTURE:  mtl  = ((WXFTexture*)m)->mtl;  
                                 p    = (WXFMap*)m;
                                 break;
      default:                   return NULL;
    }

    map = new (mtl->owner->GetPool ()) WXFMap (*mtl);

    if (!map)
      return NULL;

    map->parent = p;

    if (map->parent)
      map->parent->maps.insert (map);

    rdr.DeclareFrame ("Generic");
    rdr.BindCreater  ("Generic",create,map);
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
              children   (imp->GetPool ()), 
              parent     (NULL), 
              owner      (imp)
{   
  for (int i=0;i<MAPS_NUM;i++)
    maps [i] = NULL;  
}

WXFMaterial::~WXFmaterial ()
{
  owner->mMtlDict->erase (name);

  if (parent)  
    parent->children.erase (this);
}

WXFMap::WXFMap (WXFMaterial& m)
            : WXFMtlBase (MAP),
              parent     (NULL), 
              mtl        (&m), 
              maps       (m.owner->GetPool ())
{   }

WXFMap::~WXFMap ()
{
  if (parent)
  {
    parent->maps.erase (this);
  }  
  else
  {
    for (int i=0;i<MAPS_NUM;i++)
      if (maps [i] == this)
        maps [i] = NULL;
  }  
}

