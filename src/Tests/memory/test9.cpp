#include <default.h>
#include <resource.h>

class    TestResource: public Memory::FileResource
{
  public:
                TestResource (Str128 file_name): FileResource (file_name) {}

  protected:
    virtual     size_t    check (BinFile* file) const { return file->size (); }
    virtual     bool      read  (BinFile* file,void* data,size_t size) {
      dout<<"Resource ready: read"<<endl;  

      file->read (data,size);

      return true;
    }
};

void     main ()
{
  R_Load ();

  Cache* cache = R_CreateDefragCache ("cache1","default",5*1024*1024);

  if (!IsValid (cache))
  {  
    dout<<"Error at create cache "<<cache<<endl;
    return;
  }

  R_Push (cache);

  Resource*     res1 = new TestResource ("test.dat");
  Resource*     res2 = new TestResource ("test.dat");  
  Resource*     res3 = new TestResource ("test.dat");  

  char*         data  = (char*)res1->lock (); 
  char*         data1 = (char*)res2->lock ();
  char*         data2 = (char*)res3->lock ();

  res1->unlock (); 

  dout<<(data!=NULL)<<endl; 
  dout<<(data1!=NULL)<<endl;
  dout<<(data2!=NULL)<<endl;

  delete res3;
  delete res2;
  delete res1;  

  R_Done ();
}

