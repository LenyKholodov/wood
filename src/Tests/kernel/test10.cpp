#include <malloc.h>
#include <iostream.h>
#include <time.h>

const    clock_t    TEST_TIME  = CLK_TCK*5;

int          rndTable [1024];
int          rndPos  ;

int          Random1024  () { return rndTable [rndPos = ++rndPos & 1023]; }
void         InitRandom  ();

void   __stdcall  go          ()
{
  alloca (1024*100);
}

void     main ()
{
 
  int         total       = 0;

  for (clock_t start = clock();clock() - start < TEST_TIME;)
  {
    go ();
    total ++;
  }

  clock_t end   = clock();
  float   time  = (end-start)/CLK_TCK;
  
  cout<<"Total:\t"<<total<<endl;
  cout<<"Total time:\t"<<int(time)<<endl;
  cout<<"FPS:\t"<<int((float)total/time)<<endl; 
}

/*void         InitRandom  ()
{
  for (int i=0;i<1024;i++)
    rndTable [i] = rand () % 1024;
    
  rndPos = rand () % 1024;  
}
  */