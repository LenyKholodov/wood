#ifndef __STREAM_BIN_FILE__
#define __STREAM_BIN_FILE__

#include <binfile\binfile.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Поток (нет возможности произвольного доступа, только последовательный)
///Данные только бинарные
/////////////////////////////////////////////////////////////////////////////////////////////
class  StreamBinFile: public BinFile
{
  public:
                        StreamBinFile  (int mode=0);

/////////////////////////////////////////////////////////////////////////////////////////////
///Дополнительные функции
/////////////////////////////////////////////////////////////////////////////////////////////
        size_t  printf (const char* format,va_list);
        size_t  scanf  (const char* format,va_list);

        size_t  printf (const char* format,...);
        size_t  scanf  (const char* format,...);

        size_t  gets   (char*); //ret string len
        size_t  puts   (const char*);

        char    GetChar  ();
        int     GetInt   ();
        int16   GetInt16 ();

        template <class T>
        bool     put (const T& x) { return write (&x,sizeof (T)) == sizeof (T); }

        template <class T>
        bool     get (T& x) { return read (&x,sizeof (T)) == sizeof (T); }

  protected:
                BinFile::seek;
                BinFile::reset;
};

template <typename T>
inline  StreamBinFile& operator << (StreamBinFile& os,const T& x)
{
  os.put (&x,sizeof (T));

  return os;
}

inline StreamBinFile& operator << (StreamBinFile& os,const char* x)
{
  os.puts (x);

  return os;
}

inline StreamBinFile& operator << (StreamBinFile& os,const uchar* x)
{
  os.puts ((const char*)x);

  return os;
}

template <typename T>
inline  StreamBinFile& operator >> (StreamBinFile& is,T& x)
{
  is.get (&x,sizeof (T));

  return is;
}

inline StreamBinFile& operator >> (StreamBinFile& is,char* x)
{
  is.gets (x);
  return is;
}

inline StreamBinFile& operator >> (StreamBinFile& is,uchar* x)
{
  is.gets ((char*)x);
  return is;
}

#endif