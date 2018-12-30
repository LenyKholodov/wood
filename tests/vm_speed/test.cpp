#include <default.h>

#include <default.h>
#include <resource.h>

size_t size = 0;
BinFile* file = NULL;

class    TestResource: public Resource
{
  public:
                TestResource (): Resource () {
                }

  protected:
    virtual     void*     load       () {
      file->reset ();

      void*    data = alloc (file->size ());

      if (!data)
        return NULL;

      file->read (data,file->size ());

      return data;
    }
};

const int TOTAL = 128;
const int TOTAL_TEST = TOTAL;

Resource* res [TOTAL] = {NULL};

int          rndTable [TOTAL];
int          rndPos  ;

int          Random  () { return rndTable [rndPos = ++rndPos & (TOTAL-1)]; }
void         InitRandom  ();

void         InitRandom  ()
{
  for (int i=0;i<TOTAL;i++)
    rndTable [i] = rand () % TOTAL;
    
  rndPos = rand () % TOTAL; 
}

void     main ()
{
  FloatPool pool (M_GetPool(),512*1024*1024);

  R_Load (&pool,0,TOTAL * 64);
  InitRandom ();

  file = F_OpenFile ("test.dat",BF_OPEN_RO);
  size = file->size ();

  if (!IsValid (file))
  {
    dout<<"Error at open file"<<endl;
    return ;
  }

  Cache* cache = R_CreateDefragCache ("cache1","default",128*1024*1024);

  if (!IsValid (cache))
  {  
    dout<<"Error at create cache "<<cache<<endl;
    return;
  }

  for (int i=0;i<TOTAL;i++)
    res [i] = new TestResource;

  float start = clock ();

  for (i=0;i<TOTAL_TEST;i++)
  {    
    uint index = Random();
    void* data = res [index]->lock ();
//    if (data)
//       memset (data,0,size);
    res [index]->unlock ();
  }  

  float end = clock ();

  cout<<"FPS: "<<float (CLK_TCK*TOTAL_TEST)/(end-start)<<endl;

  R_Done ();
}

