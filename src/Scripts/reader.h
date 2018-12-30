#include <scripts\framerdr.h>

class    BinFile;
class    Pool;

/////////////////////////////////////////////////////////////////////////////////////////////
///����� �����প�
/////////////////////////////////////////////////////////////////////////////////////////////
class    iobuffer: public streambuf
{
  friend class FrameReader::Reader;
  public:
                iobuffer (FrameReader::Reader& rdr);

    virtual     int     underflow ();
    virtual     int     overflow  (int c) { return c; }

/////////////////////////////////////////////////////////////////////////////////////////////
///����                
/////////////////////////////////////////////////////////////////////////////////////////////
                bool    reset     ();

  private:
        FrameReader::Reader& mReader;
        bool                      mNeedClose, mUse;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///���⥫�
/////////////////////////////////////////////////////////////////////////////////////////////
class    FrameReader::Reader: public MemObject
{
  friend class iobuffer;
  public:
                        Reader  (BinFile*    file,
                                 BinFile*    assign,
                                 ostream&    err,
                                 Pool*       pool,
                                 size_t      cache_buf_size = 16*1024
                                );
       virtual          ~Reader ();  

/////////////////////////////////////////////////////////////////////////////////////////////
///������ ���ᥬ� � ��ᮥ���񭭮�� ��⮪�
/////////////////////////////////////////////////////////////////////////////////////////////
          char*         next    (ulong& crc); 
          istream&      stream  (bool noEndLine = false);

/////////////////////////////////////////////////////////////////////////////////////////////
///����⨥ / �����⨥ ��
/////////////////////////////////////////////////////////////////////////////////////////////
          struct state_t
          {
            int   level;
            bool  openTag, closeTag;
          };
                
          void          OpenTag   (state_t&); 
          void          CloseTag  (const state_t&);

/////////////////////////////////////////////////////////////////////////////////////////////
///��⮪ �訡��
/////////////////////////////////////////////////////////////////////////////////////////////
          ostream&      err     () { return mErr; }

/////////////////////////////////////////////////////////////////////////////////////////////
///����騩 �஢��� �������� 
/////////////////////////////////////////////////////////////////////////////////////////////
          uint          level   () const { return mLevel; }      

/////////////////////////////////////////////////////////////////////////////////////////////
///�ய�� ��
/////////////////////////////////////////////////////////////////////////////////////////////
           void         TraceTag (); 

/////////////////////////////////////////////////////////////////////////////////////////////
///�뢮� ᮮ�饭�� �� �訡��
/////////////////////////////////////////////////////////////////////////////////////////////
           void         error    (const char*,...);

/////////////////////////////////////////////////////////////////////////////////////////////
///�⥭�� �� 䠩�� �����প�
/////////////////////////////////////////////////////////////////////////////////////////////
           size_t       read     (void*,size_t,long = -1);

           void         ChangeAssignFile (const char*,bool compress = false);
           void         ChangeAssignFile (BinFile* file,bool autoDel = false);
           void         ChangeAssignDir  (const char* dir);

  private:
           int           read         ();
           void          back         ();
           void          ReadComments (bool);
           size_t        LoadBuffer   ();

  private:
        iobuffer            mIOBuffer;
        istream_withassign  mStream; 
        ostream&            mErr;

        BinFile             *mFile, *mAssign;
        bool                mAssignDel;
        char                *mBuf, *mEnd, *mPos;
        size_t              mBufSize;
        Str128              mTag;       

        int                 mLevel, mCurLevel, mLine;
        bool                mOpenTag, mCloseTag, mReadString1, mReadString2;

        Str128              mAssignDir;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///���� � ��ॢ� ࠧ���
/////////////////////////////////////////////////////////////////////////////////////////////
class    FrameReader::Frame: public MemObject
{
  public:
                Frame  (Frame*);
                Frame  (FrameReader*);
                ~Frame ();

/////////////////////////////////////////////////////////////////////////////////////////////
///��ࠡ�⪠
/////////////////////////////////////////////////////////////////////////////////////////////
    bool    process    (FrameReader::Reader*);

/////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���
/////////////////////////////////////////////////////////////////////////////////////////////
    bool    AddTag     (const char*,FrameReader::ReadValueFn,void*,size_t);    
    bool    AddTag     (const char*,void**,FrameReader::CreateTagFn,void*,size_t); //size only for init
    void    DelTag     (const char*);

/////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ��ࠬ��஢
/////////////////////////////////////////////////////////////////////////////////////////////
    void    SetParams  (void** ptr,FrameReader::CreateTagFn init,void* self = NULL);

/////////////////////////////////////////////////////////////////////////////////////////////
///����� � �����
/////////////////////////////////////////////////////////////////////////////////////////////
    void*    GetObj     () const { return mObj; }
    Frame*   GetParent  () const { return mParent; }

  private:
    struct tag_t
    {
      void*             data;
      size_t            size; //for arrays
      ReadValueFn       fn;           
      Frame*            frame;

                        tag_t (ReadValueFn,void*,size_t);
                        tag_t (Frame*,void* = NULL,size_t = 0);
    };

    struct last_t
    {
      ulong   crc;   //CRC
      tag_t*  tag;   //��

              last_t (): tag (NULL), crc (0xFFFFFFFF) {}
    };

    typedef StringDict<tag_t>   TagDict;
    typedef StringDict<Frame*>  ChildDict;

  private:
    bool    ReadFrame     (tag_t*);

    bool    ReadDirective (const char* dirName);
    void    ReadDirAssign ();
    bool    ReadDirChunk  ();

    void    clear         (ChildDict::iter);

/////////////////////////////////////////////////////////////////////////////////////////////
///�室 / ��ࠡ�⪠ / ��室 �� �३��
/////////////////////////////////////////////////////////////////////////////////////////////
    bool    open       (FrameReader::Reader*);
    void    next       ();
    bool    close      ();

/////////////////////////////////////////////////////////////////////////////////////////////
///���� ��������� �३���
/////////////////////////////////////////////////////////////////////////////////////////////
    Frame*  find       (const char*,const char*&) const;

  private:
    FrameReader::Reader*         mReader;
    FrameReader*                 mOwner;
    FrameReader::Reader::state_t mState;
    bool                         mOpen;
    
    Frame**                      mCurFrame;

    FrameReader::CreateTagFn     mHandler;
    void*                        mSelf;
    void**                       mPtr, *mObj;

    TagDict                      mTags;
    ChildDict                    mChildren;
    Frame*                       mParent;
    last_t                       mLast;    
};
