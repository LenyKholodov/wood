#include "pch.h"
#include "g_level.h"
#include <string.h>

struct  woodheader_t
{
  int   id;
  char  copyright [128];
  char  name      [128];  
  ulong pos;
};

////////////////////implementation of class LumpFile////////////////////////////

class LumpFile: public FrameBinFile
{
  public:
        LumpFile (GameLevel* lev,Pool* pool,uint loader,long offs,int mode,size_t size = 0)
          :FrameBinFile (lev->mFile,offs,size,
                     (mode == GameLevel::OPEN_RW) ? can_read|can_write|can_seek|can_resize|can_reset :
                     (mode == GameLevel::OPEN_RO) ? can_read|can_seek|can_reset : can_seek|can_reset
                    ),
           mLevel (lev), mLoader (loader), mSaveState (mode == BF_OPEN_RW && !size)
             { if (mSaveState) lev->mWait = TRUE; }
     virtual ~LumpFile () {
       if (mLevel && mSaveState) 
       {
         mLevel->mLump [mLoader].pos  = mLevel->mFilePos;
         mLevel->mLump [mLoader].size = size ();

         mLevel->mWait                = FALSE;
         mLevel->mChange              = TRUE;
         mLevel->mFilePos            += size ();
       }  

       if (mLevel && mLevel->mLumpFile [mLoader])
           mLevel->mLumpFile [mLoader] = NULL;
     }

  protected:
        GameLevel* mLevel;
        uint       mLoader;
        BOOL       mSaveState;
};

////////////////////implementation of class GameLevel///////////////////////////

BinFile*  GameLevel::CreateLump      (uint _LoaderId,int mode,size_t _LumpSize)
{
  D_Try
  {
    if (_LoaderId >= LL_NUM && !mWait)
      return NULL;

    if (mWrite && mode == OPEN_RW)
    {  
      if (_LumpSize)
      {
        mLump [_LoaderId].pos  = mFilePos;
        mLump [_LoaderId].size = _LumpSize;

        mFilePos += _LumpSize;  

        return mLumpFile [_LoaderId] = ext_new 
                                       LumpFile (this,
                                                 GetPool(),
                                                 _LoaderId,
                                                 mFilePos-_LumpSize,
                                                 mode,
                                                 _LumpSize
                                                );
      }
      else
        return mLumpFile [_LoaderId] = ext_new
               LumpFile (this,GetPool(),_LoaderId,mFilePos,mode);
    }
    else if (mode == OPEN_RO)
      return mLumpFile [_LoaderId] = ext_new
             LumpFile (this,GetPool(),_LoaderId,mLump [_LoaderId].pos,mode,mLump [_LoaderId].size);
    else 
      return NULL;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG       
      dout<<"GameLevel: Error at CreateLump loader: "<<_LoaderId<<endl;
    #endif
    return NULL;
  }
}

const GameLevel::lumpx_t&   GameLevel::GetLump   (uint _LoaderId) const
{
  if (_LoaderId < LL_NUM)
    return mLump [_LoaderId];
  else
  {
    static lumpx_t lump;
    memset (&lump,0,sizeof (lumpx_t));
    return lump;
  }
}

BOOL      GameLevel::Save (const char* name) const
{
  D_Try
  {
    woodheader_t hdr;
    
    hdr.id  = WOOD_HEADER_ID;
    hdr.pos = mFilePos;

    strcpy (hdr.copyright,"(c) UNT Games Studio, 2002");        

    if (name) strcpy (hdr.name,name);
    else      hdr.name [0] = 0;

    mFile->reset ();
    mFile->write (&hdr,sizeof (hdr));
    mFile->write (&mLump [0],sizeof (lumpx_t) * LL_NUM);

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"GameLevel: Error at Save "<<this<<endl;
    #endif
    return FALSE;
  }
}

GameLevel::GameLevel  (Pool* _Pool,const char* _FileName)
          : MemObject (_Pool),
            mFile      (NULL),
            mWrite     (FALSE),
            mWait      (FALSE),
            mChange    (FALSE)
{
  D_Try
  {
    memset (mLump,0,sizeof (lumpx_t) * LL_NUM);
    memset (mLumpFile,0,sizeof (BinFile*) * LL_NUM);

    mFile       = F_OpenFile (_FileName,BF_OPEN_RO);

    if (!IsValid (mFile))
    {
      #ifdef DEBUG
        dout<<"GameLevel: Error at open file '" <<_FileName<<"'"<<endl;
      #endif
      return;
    }

    woodheader_t hdr;

    mFile->read (&hdr,sizeof (hdr));
    
    if (hdr.id != WOOD_HEADER_ID)
    {
      dout<<"GameLevel: Wrong file format "<<this<<endl;
      return;
    }    
    
    dout<<"GameLevel: Load level '"<<hdr.name<<"'"<<endl;
    dout<<"GameLevel: "<<hdr.copyright<<endl;    

    mFile->read (&mLump [0],sizeof (lumpx_t) * LL_NUM);

    mFilePos = hdr.pos;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"GameLevel: Error at construct "<<this<<endl;
    #endif           
  }
}

GameLevel::GameLevel  (Pool* pool,BinFile* file)
          : MemObject  (pool),
            mFile      (file),
            mWrite     (TRUE),
            mWait      (FALSE),
            mChange    (FALSE),
            mFilePos   (sizeof (woodheader_t) + sizeof (lumpx_t) * LL_NUM)
{
  D_Try
  {
    memset (mLump,0,sizeof (lumpx_t) * LL_NUM);
    memset (mLumpFile,0,sizeof (BinFile*) * LL_NUM);

    if (!IsValid (mFile))
    {
      D_Message ("GameLevel: Wrong file");
      return;
    }

    if (mFile->canread () && mFile->size () >= sizeof (woodheader_t))
    {
      woodheader_t hdr;

      mFile->read (&hdr,sizeof (hdr));
      
      if (hdr.id != WOOD_HEADER_ID)
      {
        dout<<"GameLevel: Wrong file format "<<this<<endl;
        return;
      }    
      
      dout<<"GameLevel: Load level '"<<hdr.name<<"'"<<endl;
      dout<<"GameLevel: "<<hdr.copyright<<endl;    

      mFile->read (&mLump [0],sizeof (lumpx_t) * LL_NUM);

      mFilePos = hdr.pos;
    }  
  }
  D_Exc ("GameLevel: Error at construct");  
}

GameLevel::~GameLevel ()
{
  D_Try
  {
    for (int i=0;i<LL_NUM;i++)
      if (mLumpFile [i])
        delete mLumpFile [i];

    if (mFile && mWrite && mChange) 
      Save ("Any name!");

    if (mFile && !mWrite)  delete mFile;
  }     
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"GameLevel: Error at destruct "<<this<<endl;
    #endif
  }
}
