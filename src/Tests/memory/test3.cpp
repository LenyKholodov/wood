#define  DEBUG
#include <pool.h>
#include <misc.h>

const    clock_t    TEST_TIME           = CLK_TCK*5;

int          rndTable [1024];
int          rndPos  ;

int          Random1024  () { return rndTable [rndPos = ++rndPos & 1023]; }
void         InitRandom  ();

void     main ()
{
  void* mem [1024] = {NULL}; 
  
  FloatPool pool (M_GetPool (),1024*1024*5);

  M_PushPool (&pool);

  clock_t start = clock ();

  for (int total=0;total<1000000;total++)
  {
    int allocPos  = Random1024();
    int freePos   = Random1024();
    
    if (mem [allocPos]) delete [] mem [allocPos];
    mem [allocPos]    = new char [128];   
    
    delete [] mem [freePos]; mem [freePos] = NULL; 
  }

  clock_t end   = clock();
  float   time  = float (end-start)/ float (CLK_TCK);
  
  dout<<"Total:\t"<<total<<endl;
  dout<<"Total time:\t"<<int(time)<<endl;
  dout<<"FPS:\t"<<int((float)total/time)<<endl; 
}

void         InitRandom  ()
{
  for (int i=0;i<1024;i++)
    rndTable [i] = random (1024);
    
  rndPos = random (1024);
}
