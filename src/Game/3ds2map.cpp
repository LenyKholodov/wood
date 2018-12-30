#include "pch.h"
#include <graph\grtypes.h>
#include <scripts\us.h>
#include <iostream.h>
#include <kernel.h>

#include "cv_grmap.h"
#include "cv_utils.h"

char     levelName [128] = "Fuck!";
char     srcName   [128] = "default.3ds",
         destName  [128] = "default.map",
         matPrefix [32]  = "",
         matFile   [128] = "materials/static_map.mat";
int      gradation       = 16;
int      cWidth = 0, cHeight = 0;
int      vbWidth = 0, vbHeight = 0; 
uint     stripEnable = FALSE, hwVCSize = 16;

const    int       SB_BACK_ID     = 0;
const    int       SB_FRONT_ID    = 1;
const    int       SB_BOTTOM_ID   = 2;
const    int       SB_TOP_ID      = 3;
const    int       SB_LEFT_ID     = 4;
const    int       SB_RIGHT_ID    = 5;

typedef  char textname_t [128];
textname_t    textures [6];

int      main (int argc,char* argv [])
{
  K_Init ();
  D_Load ("3ds2map.map");

  USStreamReader reader (M_GetPool(),cout);

  if (argc < 2)
  {
    cout<<"3ds2map.exe <info.mk>"<<endl;
    D_Done ();
    K_Done ();
    return 1;
  }

  reader.BindString     ("source",srcName);
  reader.BindString     ("mat_prefix",matPrefix);
  reader.BindString     ("mat_file",matFile);
  reader.BindString     ("dest",destName);
  reader.Bind           ("gradation",gradation);
  reader.Bind           ("name",levelName);
  reader.Bind           ("cache_width",cWidth);
  reader.Bind           ("cache_height",cHeight);
  reader.Bind           ("vb_chunk_width",vbWidth);
  reader.Bind           ("vb_chunk_height",vbHeight);  
  reader.Bind           ("TL_vertex_cache_size",hwVCSize);
  reader.Bind           ("strip_enable",stripEnable);
  reader.BindString     ("sky_box_bottom",textures [SB_BOTTOM_ID]);
  reader.BindString     ("sky_box_top",textures [SB_TOP_ID]);
  reader.BindString     ("sky_box_left",textures [SB_LEFT_ID]);
  reader.BindString     ("sky_box_right",textures [SB_RIGHT_ID]);
  reader.BindString     ("sky_box_back",textures [SB_BACK_ID]);
  reader.BindString     ("sky_box_front",textures [SB_FRONT_ID]);  

  reader.read           (argv [1]);

  FloatPool     pool  (M_GetPool(),64000000);

  M_PushPool          (&pool);

  Object3D      obj   (&pool,srcName,32000000);

  if (!IsValid (&obj))
  {
    cout<<"Error at open file "<<srcName<<endl;
    D_Done ();
    K_Done ();
    return 0;
  }

  if (matPrefix [0]) obj.SetMatPrefix    (matPrefix);

  GrMapConv     conv  (&pool,obj); 

  conv.SetCacheInfo   (cWidth,cHeight);
  conv.SetVBInfo      (vbWidth,vbHeight);
  conv.SetGrad        (gradation);
  conv.EnableStrip    (stripEnable);
  conv.SetTLCacheSize (hwVCSize);

  conv.compile        ();

  for (int i=0;i<6;i++)
    conv.SetSkyTexture (i,textures [i]);

  BinFile*      file  = F_OpenFile (destName,BF_OPEN_CR);
  GameLevel*    level = new (&pool) GameLevel (&pool,file);

  conv.Save           (level);
  level->Save         (levelName);

  delete level;
  delete file;

  M_PopPool           ();

  CreateMatFile       (obj,matFile);

  D_Done ();
  K_Done ();

  return 0;
}

//<scene.3ds> <gamemap.map> [<GRAD:Integer>] [<CacheWidth>] [<CacheHeight>] [<Level Name>]