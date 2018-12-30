#define  DEBUG
#include <windows.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <gl\gl.h>

#include <graph\jpeglib.h>
#include <graph\vector3d.h>
#include <debug\debug.h>
#include <binfile.h>

const    int MAX_TEXTURES = 1024;

SystemPool              sys;
Debug                   debug (&sys);

void            Init            ();
void CALLBACK   Reshape         (int,int);
void CALLBACK   Draw            ();
void            LoadTexture     (int,tImageJPG*);
tImageJPG*      LoadJPEG        (const char* name);
void            DecodeJPG       (jpeg_decompress_struct*,tImageJPG*);

GLuint          texture[MAX_TEXTURES];

void     main ()
{
  auxInitPosition       (0,0,1024,768);
  auxInitDisplayMode    (AUX_RGB | AUX_DOUBLE | AUX_DEPTH);
  auxInitWindow         ("Load JPEG texture test");  

  Init                  ();

  auxReshapeFunc        (Reshape);
  auxMainLoop           (Draw);
}

void    Init ()
{
  glGenTextures    (MAX_TEXTURES,texture);
  glEnable         (GL_TEXTURE_2D);

  LoadTexture      (0,LoadJPEG ("image.jpg"));
}

void CALLBACK   Reshape         (int width,int height)
{
  glViewport       (0,0,width,height);
  glMatrixMode     (GL_PROJECTION);
  glLoadIdentity   ();
  glOrtho          (-10,10,-20,20,-100.0,100.0);

  glClearColor     (0.5,0.5,0.5,1.0);
  glClear          (GL_COLOR_BUFFER_BIT);

  glEnable         (GL_DEPTH_TEST);  
  glEnable         (GL_NORMALIZE);

}

void CALLBACK   Draw            ()
{
  glClear   (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindTexture (GL_TEXTURE_2D,texture [0]);

  glBegin   (GL_TRIANGLES);
    glTexCoord2f (0.5,0); glVertex3f (0,15,-5);
    glTexCoord2f (0,1);   glVertex3f (-15,0,-5);
    glTexCoord2f (1,1);   glVertex3f (15,0,-5);
  glEnd     ();

  glFinish  ();

  auxSwapBuffers();
}

void        LoadTexture  (int i,tImageJPG* img)
{
  glBindTexture(GL_TEXTURE_2D,texture[i]);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D,0,3,img->sizeX,img->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,img->data);
}

tImageJPG*      LoadJPEG        (const char* name)
{
  struct jpeg_decompress_struct cinfo;

  tImageJPG*      pImageData = NULL;
  FILE            pFile;
  stdBinFile      file (&sys,name,BinFile::OPEN_RO); 
  jpeg_error_mgr  jerr;
  static char     buf [1024*1024];

  file.read (buf,file.size ());

  dout<<"!!!: "<<(int)buf [0]<<endl;

  cinfo.err       = jpeg_std_error(&jerr);
  pFile._base     = buf;
  pFile._ptr      = buf;
  pFile._bufsiz   = file.size ();
  pFile._cnt      = file.size ();
  pFile._file     = -1;
  pFile._charbuf  = 0;
  pFile._flag     = _IOMYBUF|_IORW;
  pFile._tmpfname = NULL;
        
  jpeg_create_decompress(&cinfo);        
  jpeg_stdio_src(&cinfo, &pFile);
        
  pImageData = (tImageJPG*)malloc(sizeof(tImageJPG));

  DecodeJPG(&cinfo, pImageData);

  dout<<"here!"<<endl;
        
  jpeg_destroy_decompress(&cinfo);
        
  return pImageData;
}

void DecodeJPG(jpeg_decompress_struct* cinfo, tImageJPG *pImageData)
{
  jpeg_read_header(cinfo, TRUE);
  jpeg_start_decompress(cinfo);

  pImageData->rowSpan = cinfo->image_width * cinfo->num_components;
  pImageData->sizeX   = cinfo->image_width;
  pImageData->sizeY   = cinfo->image_height;
        
  pImageData->data    = new unsigned char[pImageData->rowSpan * pImageData->sizeY];
                
  unsigned char** rowPtr = new unsigned char*[pImageData->sizeY];
  for (int i = 0; i < pImageData->sizeY; i++)
      rowPtr[i] = &(pImageData->data[i*pImageData->rowSpan]);

  int rowsRead = 0;
  while (cinfo->output_scanline < cinfo->output_height) 
    rowsRead += jpeg_read_scanlines(cinfo, &rowPtr[rowsRead], cinfo->output_height - rowsRead);
       
  delete [] rowPtr;

  jpeg_finish_decompress(cinfo);
}

