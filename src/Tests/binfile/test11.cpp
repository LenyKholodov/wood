#include <kernel.h>
#include <binfile\strbinfile.h>

struct pack_t
{
  size_t        size;
  char          data [];

  static pack_t*  create (const void* x,size_t s) {
    pack_t* p = (pack_t*)new char [s+sizeof (pack_t)];
    if (!p)
      return NULL;
    p->size = s;  
    memcpy (p->data,x,s);  
    return p;
  }
};

typedef Queue<pack_t*>   PQueue;

class  TestStream: public streamBinFile
{
  public:
        TestStream (): streamBinFile (can_read|can_write),pos (NULL),rsize (0) {}

  virtual size_t _write (const void* x,size_t size,filepos_t) { q.push (pack_t::create (x,size)); return size; }
  virtual size_t _read  (void* x,size_t size,filepos_t) { 
    size_t res = 0;
    uchar* ptr = (uchar*)x;

    while (size)
    {
      if (!rsize)
      {    
        pack_t* p = q.pop (); 

        if (!p)
          break;

        rsize = p->size;
        pos   = (uchar*)p->data;
      }   

      size_t rr = rsize>size?size:rsize;

      memcpy (ptr,pos,rr);

      rsize -= rr;
      size  -= rr;
      pos   += rr;
      ptr   += rr;
      res   += rr;
    }          

    return res;
  }

  private:
    PQueue      q;
    size_t      rsize;
    uchar*      pos;
};

void     main ()
{
  K_FastInit ("test11.map");

  TestStream s;

  char buf [128] = {0};

  cout<<s.puts ("Hello world!")<<endl;;
  cout<<s.gets (buf)<<endl;
  cout<<buf<<endl;
}