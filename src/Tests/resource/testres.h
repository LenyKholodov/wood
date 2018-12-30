#include <resource\resource.h>
#include <stdio.h>

class TestResource: public Resource
{
  public:
        TestResource (size_t s):size (s), buf (NULL) {  }
        ~TestResource () { if (buf) delete [] buf;  }

        bool load () {
          if (!Resource::load ())
            return false;

          buf = ext_new char [size];

          if (!buf)
          {
            dout<<"TestResource: resource not create "<<this<<endl;
            return false;
          }  

          sprintf ((char*)buf,"Hello world form TestResource -%p",this);

          return true;
        }
         
        bool check ()        {
          bool res = false;
          const char* tbl [2] = {"false","true"};
          
          if (buf)
          {          
            static char test [1024*60] ;

            sprintf (test,"Hello world form TestResource -%p",this);

            res =  !strcmp (test,(char*)buf);
          } 

          dout<<"TestResource: check: "<<tbl [res]
              <<"\t - lock: "<<tbl [IsLock ()]
              <<"\t - load: "<<tbl [IsLoad ()]
              <<"\t - dirty: "<<tbl [IsDirty ()]                 
              <<"\t"<<this<<endl;

          return res;    
        }

        void update (void* from,void* to) { 
          dout<<"TestResource: update: "<<this<<endl;
          if (from == buf) 
            buf = to; 
        } 

        void unload () {
          Resource::unload ();

          if (buf) delete [] buf;

          buf = NULL;
        }

    void* buf;
    size_t size;
};
