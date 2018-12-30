#define  DEBUG
#include <graph\alp.h>

char*     imgFile  = NULL;
char*     destFile = NULL;
char      temp [256];
char      alpText [256];
rgb_t     first    = {0,0,0};
rgb_t     last     = {0,0,0};
uint      ktrue    = 0;
uint      kfalse   = 255;          
int       format   = FMT_UNKNOWN;
BOOL      alpPresent = FALSE;

void      LoadRGB  (rgb_t&,char*);
void      LoadKoef (uint&,char*);

void     main (int argc,char* argv [])
{
  if (argc < 2)
  {
    cout<<"img2alp <img.*> -key"<<endl;
    cout<<"\t"<<"-alpha\t\tTexture with alpha channel"<<endl;
    cout<<"\t"<<"-first\t\tFirst alpha blending color"<<endl;
    cout<<"\t"<<"-last\t\tLast alpha blending color"<<endl;
    cout<<"\t"<<"-ktrue\t\tKoef alpha for TRUE interval"<<endl;
    cout<<"\t"<<"-kfalse\t\tKoef alpha for FALSE interval"<<endl;
    return;
  }

  imgFile  = argv [1];

  destFile = temp;

  strcpy (destFile,imgFile);

  char* str = strchr (destFile,'.');

  strcpy (str,".alp");

  int cnt = argc - 2;

  for (int i=2;i<cnt+2;i++)
  {
    if (!strcmp (argv [i],"-first"))  LoadRGB  (first,argv [++i]);
    if (!strcmp (argv [i],"-last"))   LoadRGB  (last,argv [++i]);
    if (!strcmp (argv [i],"-ktrue"))  LoadKoef (ktrue,argv [++i]);
    if (!strcmp (argv [i],"-kfalse")) LoadKoef (kfalse,argv [++i]);
    if (!strcmp (argv [i],"-alpha"))  
    {
      strcpy (alpText,argv [++i]);
      alpPresent = TRUE;
    }
  }

  char*         ext     = strchr (argv [1],'.');

  if      (!strcmp (ext,".bmp"))  format = FMT_BMP;
  else if (!strcmp (ext,".jpg"))  format = FMT_JPEG;  
  else if (!strcmp (ext,".tga"))  format = FMT_TGA;

  if (alpPresent)
  {
    stdBinFile src  (M_GetPool(),imgFile,BinFile::OPEN_RO);
    stdBinFile alp  (M_GetPool(),alpText,BinFile::OPEN_RO);
    stdBinFile dest (M_GetPool(),destFile,BinFile::OPEN_RW);

    SaveALP         (&dest,&src,&alp,M_GetPool(),format);
  }
  else
  {
    stdBinFile src  (M_GetPool(),imgFile,BinFile::OPEN_RO);
    stdBinFile dest (M_GetPool(),destFile,BinFile::OPEN_RW);

    SaveALP         (&dest,&src,M_GetPool(),first,last,ktrue,kfalse,format);
  }
}

void      LoadRGB  (rgb_t& rgb,char* str)
{
  char   dig [16];
  char*  pos = dig;  

  int    value [30] = {0};
  int*   ipos  = value;
  int    state = 0;

  while (*str++)
    if      (isdigit (*str) && !state) 
    {
      state  = TRUE;
      pos    = dig;
      *pos++ = *str;
    }  
    else if (isdigit (*str) && state)
      *pos++ = *str;
    else if (!isdigit (*str) && state) 
    {
      state   = FALSE;
      *pos    = 0;
      *ipos++ = atoi (dig);
    }
  
  rgb.red   = value [0];
  rgb.green = value [1];
  rgb.blue  = value [2];
}

void      LoadKoef (uint& koef,char* str)
{
  koef = (1.0f - atof (str)) * 255.0f;
}
