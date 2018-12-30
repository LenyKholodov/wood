#include <pch.h>
#include <binfile\StreamFile.h>

size_t  StreamBinFile::printf (const char* format,va_list list)
{
  D_Try
  {
    const  size_t CACHE_SIZE = 256;
    char   dest [CACHE_SIZE];
    int    size = 0;
    uchar* ptr  = NULL;
    size_t total = 0,
           res = 0;

    for (;*format;)
    {
      for (int i=0;i<CACHE_SIZE && *format;)
      {
        switch (*format)
        {
          case '%': switch (*++format)
                    {
                      case  0 : break;
                      case 'i':
                      case 'u':
                      case 'd':
                      case 'f':
                      case 'p': 
                                if (i + 4 > CACHE_SIZE)
                                {
                                  res = write (dest,i);
                                  if (res < i)
                                    return total += res;
                                  i = 0;
                                }
                                *((ulong*)(dest + i)) = va_arg (list,ulong);
                                i      += 4;
                                format++;
                                break;
                      case 'e': 
                                if (i + 8 > CACHE_SIZE)
                                {
                                  res = write (dest,i);
                                  if (res < i)
                                    return total += res;
                                  i = 0;
                                }
                                *((double*)(dest + i)) = va_arg (list,double);
                                i      += 8;
                                format++;
                                break;
                      case 'c': 
                                if (i + 1 > CACHE_SIZE)
                                {
                                  write (dest,i);
                                  if (res < i)
                                    return total += res;
                                  i = 0;
                                }
                                *(dest + i) = va_arg (list,uchar);
                                i++;
                                format++;
                                break;
                      case 'w': 
                                if (i + 2 > CACHE_SIZE)
                                {
                                  write (dest,i);
                                  if (res < i)
                                    return total += res;
                                  i = 0;
                                }
                                *(uint16*)(dest + i) = va_arg (list,uint16);
                                i          += 2;
                                format++;
                                break;
                      case 'm': size = va_arg (list,size_t); 
                      case 's': 
                      {
                        ptr = va_arg (list,uchar*);

                        if (*format == 's')
                          size = strlen ((char*)ptr) + 1;

                        if (i + size > CACHE_SIZE)
                        {
                          res  = write (dest,i); 
                          res += write (ptr,size);                          

                          i=0;

                          if (res < i + size)
                             return total += res;
                        }
                        else
                        {
                          memcpy (dest+i,ptr,size);
                          i+=size;
                        }  
                        format++;
                        size = 0;
                        ptr  = NULL;
                        break;
                      }
                      default : dest [i++] = *format++;
                    }
                    break;
          default:  dest [i++] = *format++;
                    break;
        }

        total += res;
        res    = 0;
      }

      res = write (dest,i);

      if (res < i)
        return total += res;
    }

    return total;
  }
  D_ExcRet ("StreamBinFile: Error at printf",0);
}

size_t  StreamBinFile::scanf  (const char* format,va_list list)
{
  size_t total = 0,
         res   = 0;

  D_Try
  {
    int    size = 0;
    void*  ptr  = NULL;

    for (;*format;)
    {
      switch (*format)
      {
        case '%': switch (*++format)
                  {
                    case  0 : break;
                    case 'i':  
                    case 'u':
                    case 'd':
                    case 'f':
                    case 'p': 
                              if ((res = read (va_arg (list,ulong*),4)) < 4)
                                return total += res;
                              format++;
                              break;
                    case 'e': 
                              if ((res = read (va_arg (list,double*),8)) < 8)
                                return total += res;
                              format++;
                              break;
                    case 'c': 
                              if ((res = read (va_arg (list,uchar*),1)) < 1)
                                return total += res;
                              format++;
                              break;
                    case 'w': 
                              if ((res = read (va_arg (list,uint16*),2)) < 2)
                                return total += res;
                              format++;
                              break;
                    case 'm': size = va_arg (list,size_t);
                              if ((res = read (va_arg (list,void*),size)) < size)
                                return total += res;
                              format++;
                              break;
                    case 's': 
                    {
                      ptr  = va_arg (list,uchar*);
                      res  = gets ((char*)ptr);
                      format++;
                      break;
                    }
                    default : format++;
                  }
                  break;
        default:  format++;
                  break;
      }
      total += res;
      res    = 0;
    }
  }
  D_ExcRet ("StreamBinFile: Error at scanf",total+res);

  return total;
}

size_t  StreamBinFile::printf (const char* format,...)
{
  size_t res = 0;

  if (format)
  {
    va_list       list;
    va_start      (list,format);
    res = printf  (format,list);
    va_end        (list);    
  }

  return res;

}

size_t  StreamBinFile::scanf  (const char* format,...)
{ 
  size_t res = 0;

  if (format)
  {
    va_list       list;
    va_start     (list,format);
    res = scanf  (format,list);
    va_end       (list);    
  }

  return res;
}

char    StreamBinFile::GetChar   ()
{
  char x;

  if (!get (x)) return 0;
  else          return x;
}

int     StreamBinFile::GetInt   ()
{
  int x;

  if (!get (x)) return 0;
  else          return x;
}

int16   StreamBinFile::GetInt16 ()
{
  int16 x;

  if (!get (x)) return 0;
  else          return x;
}

size_t  StreamBinFile::gets   (char* str)
{
  D_Try
  {
    size_t size = 0;
    do 
    {
      get (*str); 
      size++;
      str++;
    } while (str [-1]);

    return size - 1;
  }
  D_ExcRet ("StreamBinFile: Error at gets",0);
}

size_t  StreamBinFile::puts   (const char* str)
{
  D_Try
  {
    return write (str,strlen(str)+1);
  }
  D_ExcRet ("stremBinFile: Error at puts",0);
}

StreamBinFile::StreamBinFile  (int mode)              
              : BinFile (mode)
{
  mMode |= can_resize;
  mMode &= ~(long)(can_seek|can_reset);
}