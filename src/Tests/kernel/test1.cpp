#include <kernel.h>
#include <time.h>

const    clock_t    TEST_TIME  = CLK_TCK*5;

int          rndTable [1024];
int          rndPos  ;

int          Random1024  () { return rndTable [rndPos = ++rndPos & 1023]; }
void         InitRandom  ();

void     main ()
{
  K_FastInit (NULL);

  void* mem [1024];
  
  memset (mem,0,sizeof(void*)*1024);

  FloatPool pool (M_GetPool(),160*1024*1024);

  M_PushPool (&pool);
  
  int         total       = 0;

  for (clock_t start = clock();clock() - start < TEST_TIME;)
  {
    int allocPos  = Random1024();
    int freePos   = Random1024();
    
    if (mem [allocPos]) delete [] mem [allocPos];
    mem [allocPos]    = new char [128];   
    
    delete [] mem [freePos]; mem [freePos] = NULL;
    
    total++;
  }

  clock_t end   = clock();
  float   time  = (end-start)/CLK_TCK;
  
  cout<<"Total:\t"<<total<<endl;
  cout<<"Total time:\t"<<int(time)<<endl;
  cout<<"FPS:\t"<<int((float)total/time)<<endl; 
}

void         InitRandom  ()
{
  for (int i=0;i<1024;i++)
    rndTable [i] = rand () % 1024;
    
  rndPos = rand () % 1024;  
}
