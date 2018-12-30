#include "pch.h"
#include "cv_rawmap.h"
#include <scripts\us.h>
#include <kernel.h>

char     srcName   [128] = "default.raw",
         destName  [128] = "default.map",
         srcText   [128] = "default.tga";
int      width           = 0,
         height          = 0,
         twidth          = 16,
         theight         = 16,  
         ttwidth         = twidth,
         ttheight        = theight;
BOOL     inc             = FALSE;
float    max_height      = 255.0f;

int      main (int argc,char* argv [])
{
  K_Init ();
  D_Load ("raw2map.map");
  F_AddFastPath ("textures","textures\\");

  USStreamReader reader (M_GetPool(),cout);

  if (argc < 2)
  {
    cout<<"raw2map.exe <info.mk>"<<endl;
    D_Done ();
    K_Done ();
    return 1;
  }  

  reader.BindString     ("raw_file",srcName);
  reader.BindString     ("raw_texture",srcText);
  reader.BindString     ("map",destName);
  reader.Bind           ("raw_width",width);
  reader.Bind           ("raw_height",height);
  reader.Bind           ("raw_tile_width",twidth);
  reader.Bind           ("raw_tile_height",theight);
  reader.Bind           ("raw_texture_tile_width",ttwidth);
  reader.Bind           ("raw_texture_tile_height",ttheight);
  reader.Bind           ("map_include",inc);
  reader.Bind           ("max_height",max_height);

  reader.read           (argv [1]);  

  FloatPool     pool (M_GetPool(),64*1024*1024);
  BinFile*              file  = F_OpenFile (destName,BF_OPEN_RW);
  GameLevel*            level = new (&pool) GameLevel (&pool,file);    
  RawConvert            conv (&pool,srcName,srcText,*level,inc);

  conv.SetSize          (width,height);
  conv.SetTileSize      (twidth,theight);
  conv.SetTextTileSize  (ttwidth,ttheight);
  conv.prepare          ();
  conv.SetMaxHeight     (max_height);
  conv.convert          ();
  conv.Save             ();
  
  delete level;
  delete file;

  D_Done ();
  K_Done ();

  return 0;
}