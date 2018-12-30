#include "pch.h"
#include "r_matdict.h"

#include <graph\grtypes.h>
#include <graph\x3f.h>

int     MatDictionary::AddMaterial (const char* name)
{
  D_Try
  {
    ulong crc   = CRC32 (name,strlen (name));
    int   index = -1;

    if (mDict.search (crc,index))
      return index;
    else 
    {
      strcpy (mMat [index=mPos++],name);

      mDict.insert (crc,index);

      return index;
    }    
  }
  D_ExcRet ("MatDictionary: Error at AddMaterial",-1);
}

const char*    MatDictionary::GetMaterial (int index) const
{
  D_Try
  {
    return mMat [index];
  }
  D_ExcRet ("MatDictionary: Error at GetMaterial: "<<index,NULL);
}               

int     MatDictionary::GetMaterial (const char* name) const
{
  D_Try
  {
    ulong crc   = CRC32 (name,strlen (name));    
    int   index = -1;

    if (!mDict.search (crc,index))
      return -1;
    else 
      return index;    
  }
  D_ExcRet ("MatDictionary: Error at GetMaterial",-1);
}

void    MatDictionary::save        (BinFile* file)
{
  file->write (mMat,sizeof (str_t) * mPos);
}

MatDictionary::MatDictionary  (Pool* _Pool,int num)
        : MemObject (_Pool), mNum (num), mPos (0),
          mDict (_Pool)
{
  mMat = (str_t*)alloc (sizeof (str_t) * mNum);

  if (!mMat || !IsValid (&mDict))
  {
    D_Message ("MatDict: No enough memory for sys data");
    return;
  }
}

MatDictionary::~MatDictionary ()
{
  if (mMat) free (mMat);
}
