#include <windows.h>
#include <iostream.h>
#include <time.h>

const int TOTAL = 512;
const int TOTAL_TEST  = 512*2;

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

char* table [TOTAL] = {NULL};

void     main ()
{
  InitRandom ();

  size_t size = 1024*1024;

  for (int i=0;i<TOTAL;i++)
  {
    table [i] = new char [size];

    if (!table [i])
    {
      cout<<"No memory: "<<i<<endl;
      return;
    }
  }  

  float start = clock ();

  for (i=0;i<TOTAL_TEST;i++)
  {
    int index = Random ();

    memset (table [index],0,size);
  } 

  float end = clock ();

  cout<<"FPS: "<<float (CLK_TCK*TOTAL_TEST)/(end-start)<<endl;    

  for ( i=0;i<TOTAL;i++)
    if (table [i])
      delete [] table [i];
}
