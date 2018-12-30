#include <default.h>
#include <resource.h>

class    TestResource: public Resource
{
  public:
                TestResource (const char* filename): Resource () {
                  strcpy (name,filename);
                }

  protected:
    virtual     void*     load       () {
      dout<<"TestResource: Load file '"<<name<<"'"<<endl;

      BinFile* file = F_OpenFile (name,BF_OPEN_RO);

      if (!IsValid (file))
      {
        dout<<"TestResource: Error at open"<<endl;
        return NULL;
      }

      dout<<"alloc size: "<<file->size ()<<endl;

      void*    data = alloc (file->size ());

      if (!data)
        return NULL;

      dout<<"Resource ready: read"<<endl;  

      file->read (data,file->size ());

      return data;
    }

  private:
        char    name [128];
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

  cache = R_CreateDefragCache ("cache2","cache1",4*1024*1024);

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

