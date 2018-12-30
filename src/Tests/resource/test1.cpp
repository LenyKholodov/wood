#include <default.h>
#include <resource\caches.h>

class TestCache: public FlatCache<FloatPool>
{
  public:
        TestCache (Pool* pool,size_t size):FlatCache<FloatPool> (pool,size) {}
};

class TestResource: public Resource
{
  public:
        TestResource ():buf (NULL) {}
        ~TestResource () { if (buf) delete [] buf; if (buf1) delete [] buf1; }

        bool load () {
          if (!Resource::load ())
            return false;

          buf = ext_new char [1024*511];
          buf1 = ext_new char [1024*511];

          if (!buf || !buf1)
          {
            cout<<"Error!"<<endl;
            return false;
          }  

          delete [] buf1;
          delete [] buf;

          buf = ext_new char [1024*511];
          buf1 = ext_new char [1024*511];

          if (!buf || !buf1)
          {
            cout<<"Error!"<<endl;
            return false;
          }  

          return true;
        }

        void unload () {
          Resource::unload ();

          if (buf) delete [] buf;
          if (buf1) delete [] buf1;

          buf = NULL;
          buf1 = NULL;
        }

    void* buf,*buf1;
};

void    main ()
{
  D_BindMapFile ("test1.map");

  TestCache cache (M_GetPool(),1024*1024);
  TestResource res;

  cout<<res.lock ()<<endl;
  cout<<res.lock ()<<endl;
  cout<<res.buf<<endl;  
  cout<<res.buf1<<endl;  

  res.SetOwner (&cache);

  cout<<res.lock ()<<endl;
  cout<<res.lock ()<<endl;
  cout<<res.buf<<endl;  
  cout<<res.buf1<<endl;  
}
