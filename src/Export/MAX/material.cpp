#include <pch.h>

#include "material.h"

void     MaterialExport::DoExport ()
{
  TSTR className;
  mtl.GetClassName (className);

  dump<<"name\t'"<<mtl.GetName ()<<"'"<<endline;;
  if (parent) dump<<"parent\t'"<<parent->GetName ()<<"'"<<endl;
  dump<<"class\t"<<className.data ()<<endline;

  // We know the Standard material, so we can get some extra info
  if (mtl.ClassID() == Class_ID(DMTL_CLASS_ID, 0)) 
  {
    StdMat* std = (StdMat*)&mtl;

    dump<<"ambient\t"<<std->GetAmbient (time)<<endline;
    dump<<"diffuse\t"<<std->GetDiffuse (time)<<endline;
    dump<<"specular\t"<<std->GetSpecular (time)<<endline;
    dump<<"shiness\t"<<std->GetShininess (time)<<endline;
    dump<<"shine_str\t"<<std->GetShinStr (time)<<endline;
    dump<<"transp\t"<<std->GetXParency (time)<<endline;
    dump<<"wire_size\t"<<std->WireSize (time)<<endline;
    dump<<"shading\t";

    switch (std->GetShading()) 
    {
      case SHADE_CONST: dump<<"Constant"<<endline;  break;
      case SHADE_PHONG: dump<<"Phong"<<endline;     break;
      case SHADE_METAL: dump<<"Metal"<<endline;     break;
      case SHADE_BLINN: dump<<"Blinn"<<endline;     break;
      default:          dump<<"Other"<<endline;     break;
    }

    dump<<"xp_falloff\t"<<std->GetOpacFalloff (time)<<endline;
    dump<<"self_illum\t"<<std->GetSelfIllum (time)<<endline;
    dump<<"falloff\t"<<(std->GetFalloffOut() ? "out" : "in")<<endline;   
    
    if (std->GetTwoSided())  dump<<"two_sided"<<endline;    
    if (std->GetWire())      dump<<"wire"<<endline;    
    if (std->GetWireUnits()) dump<<"wire_units"<<endline;
    if (std->GetFaceMap())   dump<<"face_map"<<endline;
    if (std->GetSoften())    dump<<"soften"<<endline;

    dump<<"xp_type\t";
    
    switch (std->GetTransparencyType()) 
    {
      case TRANSP_FILTER:      dump<<"Filter"<<endline;      break;
      case TRANSP_SUBTRACTIVE: dump<<"Subtractive"<<endline; break;
      case TRANSP_ADDITIVE:    dump<<"Additive"<<endline;    break;
      default:                 dump<<"Other"<<endline;       break;
    }
  }
  else 
  {
    dump<<"ambient\t"<<mtl.GetAmbient ()<<endline;
    dump<<"diffuse\t"<<mtl.GetDiffuse ()<<endline;
    dump<<"specular\t"<<mtl.GetSpecular ()<<endline;
    dump<<"shiness\t"<<mtl.GetShininess ()<<endline;
    dump<<"shine_str\t"<<mtl.GetShinStr ()<<endline;
    dump<<"transp\t"<<mtl.GetXParency (time)<<endline;
    dump<<"wire_size\t"<<mtl.WireSize (time)<<endline;
  }

  for (int i=0;i<mtl.NumSubTexmaps();i++) 
  {
    Texmap* subTex = mtl.GetSubTexmap(i);

    float amt = 1.0f;

    if (subTex) 
    {
      if (mtl.ClassID() == Class_ID(DMTL_CLASS_ID,0)) 
      {
        if (!((StdMat*)&mtl)->MapEnabled(i))
                continue;
        amt = ((StdMat*)&mtl)->GetTexmapAmt (i,0);
      }

      ExportTexture (subTex,GetMapID (mtl.ClassID(),i),amt);
    }
  }
  
  if (mtl.NumSubMtls() > 0)  
  {
    dump<<"sub_materials_num\t"<<mtl.NumSubMtls()<<endline;
   
    for (int i=0;i<mtl.NumSubMtls();i++) 
    {
      Mtl* subMtl = mtl.GetSubMtl(i);

      if (subMtl) 
        MaterialExport exp (*this,subMtl,i);
    }
  }
}

const char* MaterialExport::GetMapID (Class_ID cid,int subNo)
{
  static Str128 buf;

  if (cid == Class_ID(0,0))
     buf = "Envmap"; 
  else if (cid == Class_ID(DMTL_CLASS_ID, 0)) 
  {
    switch (subNo) 
    {
       case ID_AM: buf = "AmbientMap";       break;
       case ID_DI: buf = "DiffuseMap";       break;
       case ID_SP: buf = "SpecularMap";      break;
       case ID_SH: buf = "ShineMap";         break;
       case ID_SS: buf = "ShineStrengthMap"; break;
       case ID_SI: buf = "SelfIllumMap";     break; 
       case ID_OP: buf = "OpacityMap";       break; 
       case ID_FI: buf = "FilterColorMap";   break;
       case ID_BU: buf = "BumpMap";          break;
       case ID_RL: buf = "ReflectMap";       break;
       case ID_RR: buf = "RefractMap";       break;
     }
  }
  else buf = "Generic";

  return buf;
}

void MaterialExport::ExportUVGen (StdUVGen* uvGen)
{
  int mapType = uvGen->GetCoordMapping (0);
  TimeValue t = info.staticFrame;

  dump<<"map_type\t";
 
  switch (mapType) 
  {
    case UVMAP_EXPLICIT:    dump<<"Explicit"<<endline;    break;
    case UVMAP_SPHERE_ENV:  dump<<"Spherical"<<endline;   break;
    case UVMAP_CYL_ENV:     dump<<"Cylindrical"<<endline; break;
    case UVMAP_SHRINK_ENV:  dump<<"Shrinkwrap"<<endline;  break;
    case UVMAP_SCREEN_ENV:  dump<<"Screen"<<endline;      break;
  }

  dump<<"u_offset\t"<<uvGen->GetUOffs (t)<<endline;
  dump<<"v_offset\t"<<uvGen->GetVOffs (t)<<endline;
  dump<<"u_tiling\t"<<uvGen->GetUScl (t)<<endline;
  dump<<"v_tiling\t"<<uvGen->GetVScl (t)<<endline;  
  dump<<"angle\t"<<uvGen->GetAng (t)<<endline;
  dump<<"blur\t"<<uvGen->GetBlur(t)<<endline;
  dump<<"blur_offset\t"<<uvGen->GetBlurOffs (t)<<endline;
  dump<<"noise_amount\t"<<uvGen->GetNoiseAmt(t)<<endline;
  dump<<"noise_size\t"<<uvGen->GetNoiseSize(t)<<endline;
  dump<<"noise_level\t"<<uvGen->GetNoiseLev(t)<<endline;
  dump<<"noise_phase\t"<<uvGen->GetNoisePhs(t)<<endline; 
}


void MaterialExport::ExportTexture (Texmap* tex,const char* mapId,float amt)
{
  if (!tex) 
    return;

  TSTR className;
  tex->GetClassName(className);

  OpenFrame (mapId);

  dump<<"name\t'"<<tex->GetName ()<<"'"<<endline;
  dump<<"class\t"<<className.data ()<<endline;
  dump<<"amount\t"<<amt<<endline;
  dump<<"map_channel\t"<<tex->GetMapChannel ()<<endline;

  // Is this a bitmap texture?
  // We know some extra bits 'n pieces about the bitmap texture
  if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)) 
  {
    OpenFrame ("bitmap");

    dump<<"bitmap\t'"<<((BitmapTex *)tex)->GetMapName()<<"'"<<endline;

    StdUVGen* uvGen = ((BitmapTex *)tex)->GetUVGen();

    if (uvGen) 
      ExportUVGen (uvGen);

    TextureOutput* texout = ((BitmapTex*)tex)->GetTexout();

    if (texout->GetInvert()) 
      dump<<"invert"<<endline;

    dump<<"filter\t";
    
    switch (((BitmapTex*)tex)->GetFilterType()) 
    {
      case FILTER_PYR: dump<<"Pyramidal"<<endline; break;
      case FILTER_SAT: dump<<"SAT"<<endline;       break;
      default:         dump<<"None"<<endline;      break;
    }

    CloseFrame ();
  }

  for (int i=0;i<tex->NumSubTexmaps();i++) 
    ExportTexture (tex->GetSubTexmap(i),GetMapID (tex->ClassID(),i),1.0f);

  CloseFrame ();
}

MaterialExport::MaterialExport (MaterialExport& exp,Mtl* m,int id)
               : BaseExport ("material",exp),
                 mtl        (*m), parent (&exp.mtl)
{
  time = info.staticFrame;

  dump<<"id\t"<<id<<endline;

  DoExport ();
}

MaterialExport::MaterialExport (RootExport& root,Mtl* m,int id)
               : BaseExport ("material",root),
                 mtl        (*m), parent (NULL)
{
  time = info.staticFrame;

  dump<<"id\t\t"<<id<<endline;

  DoExport ();
}
