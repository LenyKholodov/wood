#include "pch.h"
#include "cv_mdlsprite.h"
#include "r_mdlsprite.h"

#include <misc.h>
#include <string.h>
#include <fstream.h>
#include <binfile.h>

/////////////////implementation of class Sprite3DModelConv//////////////////////

void        Sprite3DModelConv::Save          (const char* name)
{
  D_Try
  {
    BinFile* file = F_OpenFile (name,BF_OPEN_RW);

    if (!IsValid (file))
    {
      #ifdef DEBUG
        dout<<"Sprite3DModelConv: Error at create file '"<<name<<"'"<<endl;
      #endif
      if (file) delete file;
      return;
    }

    m3dheader_t hdr;

    hdr.id    = SPRITE_MODEL_ID;
    hdr.size  = sizeof (textmat_t) * mMCount + 
                mSpriteRing.count () * sizeof (sprite3d_t) +
                sizeof (spritemodeldata_t);

    spritemodeldata_t data;  
    double            temp;

    data.koef         = mGrad > 0 ? 
                        1.0f / float (int2angle (mGrad)):
                        -1.0f;
    
    data.text.num     = mMCount;
    data.text.size    = sizeof (textmat_t) * mMCount;
    data.text.offs    = 0;

    data.sprite.num   = mSpriteRing.count ();
    data.sprite.size  = sizeof (sprite3d_t) * mSpriteRing.count ();
    data.sprite.offs  = data.text.size;

    file->write (&hdr,sizeof (hdr));
    file->write (&data,sizeof (data));

    NRing::iterator iter = mNameRing;

    for (int i=0;i<mMCount;i++,iter++)
    {
      textmat_t mat;

      strcpy      (mat.text,iter.data().name);

      file->write (&mat,sizeof (mat));

//      cout<<"...material: "<<i<<" '"<<iter.data().name<<"'"<<endl;
    }

    SRing::iterator iter1 = mSpriteRing;

    for (i=0;i<mSpriteRing.count ();i++,iter1++)
    {
      face_t     face = iter1.data ();
      sprite3d_t spr  = {face.text,int2angle (face.start),int2angle (face.end)};

      if (spr.end < (ANGLE_1>>4)) spr.end = ANGLE_360;

      file->write (&spr,sizeof (spr));
      
//      cout<<"...sprite: "<<i<<" texture: "<<face.text<<" angle: "<<face.angle<<endl;
    }

    delete file;

    cout<<"Process model: '"<<name<<"'"<<endl;
    cout<<"\t\tSprites:\t"<<mSpriteRing.count ()<<endl;
    cout<<"\t\tTextures:\t"<<mMCount<<endl;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Sprite3DModelConv: Error at Save "<<endl;
    #endif
  }
}

void        Sprite3DModelConv::compute       (istream& is)
{
  char  buf [1024];

  for(;is;) 
  {
    face_t      face;
    ulong       crc;
    int         temp = -1;
    char        buf [128]= {0}, test [128] = {0};

    is>>test;

    locase (test,test);

    if (!strcmp (test,"sprite"))
    {
      is>>face.start>>face.end>>buf;      

      crc = CRC32 (buf,strlen (buf));

      if (!mNameDict.search (CRC32 (buf,strlen (buf)),temp))
      {
        name_t name; strcpy (name.name,buf);

        mNameDict.insert (CRC32 (buf,strlen (buf)),mMCount);
        mNameRing.insert (name);      

        temp = mMCount++;
      }

      face.text = temp;
      
      mSpriteRing.insert (face);
    }

    if (!strcmp (test,"step"))
    {
      is>>mGrad;        
    }
  }
}

Sprite3DModelConv::Sprite3DModelConv  (Pool* _Pool,const char* name)
                  : MemObject (_Pool),
                    mNameDict  (_Pool),
                    mNameRing  (_Pool),
                    mSpriteRing(_Pool),
                    mMCount     (0),
                    mGrad       (0)
{
  D_Try
  {    
    compute (ifstream (name,ios::in));
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Sprite3DModelConv: Error at contruct "<<endl;
    #endif
  }
}

Sprite3DModelConv::~Sprite3DModelConv ()
{
}