#define  DEBUG
#include <memory\memory.h>

#include "iostream.h"
#include "stdlib.h"
#include "string.h"
#include "conio.h"
#include "time.h"

const    clock_t    TEST_TIME           = CLK_TCK*5;
const    clock_t    TEST_POOL_SIZE      = 1024*1024*128;

SystemPool   sys;

int          rndTable [1024];
int          rndPos  ;

void         TestDefrag  ();
void         TestFloat   ();
void         TestFixed   ();
void         TestTemp    ();

int          Random1024  () { return rndTable [rndPos = ++rndPos & 1023]; }
void         InitRandom  ();

void     main ()
{
  cout<<"Pool allocator testing system"<<endl;
  cout<<"Init system...";
  InitRandom ();
  cout<<"Ok"<<endl<<endl;

  cout<<"System info:"<<endl;
  cout<<"\tPool size:\t"<<sys.size()<<endl;
  cout<<"\tFree size:\t"<<sys.memavail()<<endl<<endl;

  cout<<"Begin testing"<<endl;
  cout<<"For all tests:"<<endl; 
  cout<<"\tTEST_TIME:\t"<<TEST_TIME/CLK_TCK<<" sec"<<endl;
  cout<<"\tTEST_POOL_SIZE\t"<<TEST_POOL_SIZE<<endl<<endl;;  

  cout<<"Float pool testing...\t"<<endl; TestFloat  ();  cout<<endl;        
  cout<<"Fixed pool testing...\t"<<endl; TestFixed  ();  cout<<endl;       
  cout<<"Temp pool testing...\t"<<endl;  TestTemp   ();  cout<<endl;
  cout<<"Defrag pool testing...\t"<<endl; TestDefrag  ();  cout<<endl;
}

void         TestFixed   ()
{
  const int ClasterSize = 1024*256;

  ClasterPool* pool  = new ClasterPool (&sys,TEST_POOL_SIZE,ClasterSize);  

  if (!Pool::IsValid (pool))
  {
    cout<<"Erron in allocate pool:\t"<<TEST_POOL_SIZE<<endl;
    cout<<"Test failed"<<endl;
    return;
  }

  void* mem [1024];

  memset (mem,0,sizeof(void*)*1024);
  
  cout<<"Pool size:\t"<<pool->size()<<endl;

  int         total = 0;

  for (clock_t start = clock();clock()-start < TEST_TIME;)           
  {
    int allocPos  = Random1024();
    int freePos   = Random1024();
    
    if (mem [allocPos]) pool->free  (mem [allocPos]);
    mem [allocPos]    = pool->alloc (ClasterSize);   
    
    pool->free (mem [freePos]); mem [freePos] = NULL;
    
    total++;
  }
  
  clock_t end   = clock();
  float   time  = (end-start)/CLK_TCK;
  
  cout<<"Total:\t"<<total<<endl;
  cout<<"Total time:\t"<<int(time)<<endl;
  cout<<"FPS:\t"<<int(total/time)<<endl;
  
  delete pool;    
}

void         TestDefrag ()
{
  void* mem [1024];

  DefragPool* pool = new DefragPool (&sys,TEST_POOL_SIZE);  

  if (!Pool::IsValid (pool))
  {
    cout<<"Erron in allocate pool:\t"<<TEST_POOL_SIZE<<endl;
    cout<<"Test failed"<<endl;
    return;
  }
  
  memset (mem,0,sizeof(void*)*1024);
  
  cout<<"Pool size:\t"<<pool->size()<<endl;
  
  int         total       = 0;
  int         totalDefrag = 0;

  for (clock_t start = clock();clock() - start < TEST_TIME;)
  {
    int allocPos  = Random1024();
    int freePos   = Random1024();
    int allocSize = Random1024()*Random1024()*(Random1024()>>2);       
    
    if (mem [allocPos]) pool->free  (mem [allocPos]);
    mem [allocPos]    = pool->alloc (allocSize,&mem [allocPos]);
    
    if (allocSize > 0 && pool->memavail() > allocSize && mem [allocPos] == NULL)
    {  
       totalDefrag++; 

       pool->defrag ();      

       mem [allocPos]  = pool->alloc (allocSize,&mem [allocPos]);       
    
       if (mem [allocPos] == NULL && pool->memavail() > allocSize) 
       {
          dout<<"No alloc block\t"<<allocSize<<"\tfree\t"<<pool->memavail()
              <<"\tmaxfree\t"<<pool->maxavail()<<endl;
          pool->dump (dout);    
       }       
       else if (mem [allocPos] == NULL)
         pool->reset ();
    }       
    
    pool->free (mem [freePos]); mem [freePos] = NULL;
    
    total++;
  }

  clock_t end   = clock();
  float   time  = (end-start)/CLK_TCK;
  
  cout<<"Total:\t"<<total<<endl;
  cout<<"Total defrag:\t"<<totalDefrag<<endl;
  cout<<"Total time:\t"<<int(time)<<endl;
  cout<<"FPS:\t"<<int(total/time)<<endl; 
  
  delete pool;
}

void         TestFloat ()
{
  void* mem [1024];

  FloatPool* pool = new FloatPool (&sys,TEST_POOL_SIZE);

  if (!Pool::IsValid (pool))
  {
    cout<<"Erron in allocate pool:\t"<<TEST_POOL_SIZE<<endl;
    cout<<"Test failed"<<endl;
    return;
  }
  
  memset (mem,0,sizeof(void*)*1024);
  
  cout<<"Pool size:\t"<<pool->size()<<endl;
  
  int         total       = 0;
  int         totalDefrag = 0;

  for (clock_t start = clock();clock() - start < TEST_TIME;)
  {
    int allocPos  = Random1024();
    int freePos   = Random1024();
    int allocSize = Random1024()*Random1024()*(Random1024()>>2);
    
    if (mem [allocPos]) pool->free  (mem [allocPos]);
    mem [allocPos]    = pool->alloc (allocSize);
    
    if (allocSize > 0 && pool->memavail() > allocSize && mem [allocPos] == NULL)
    {
       pool->reset ();
       memset (mem,0,sizeof(void*)*1024);
    }   
    
    pool->free (mem [freePos]); mem [freePos] = NULL;
    
    total++;
  }

  clock_t end   = clock();
  float   time  = (end-start)/CLK_TCK;
  
  cout<<"Total:\t"<<total<<endl;
  cout<<"Total time:\t"<<int(time)<<endl;
  cout<<"FPS:\t"<<int(total/time)<<endl; 
  
  delete pool;
}

void         InitRandom  ()
{
  for (int i=0;i<1024;i++)
    rndTable [i] = rand () % 1024;
    
  rndPos = rand () % 1024;  
}

void         TestTemp    ()
{
  Pool* pool  = new TempPool (&sys,TEST_POOL_SIZE);  

  if (!Pool::IsValid (pool))
  {
    cout<<"Erron in allocate pool:\t"<<TEST_POOL_SIZE<<endl;
    cout<<"Test failed"<<endl;
    return;
  }

  void* mem [1024];

  memset (mem,0,sizeof(void*)*1024);
  
  cout<<"Pool size:\t"<<pool->size()<<endl;

  int         total = 0;

  for (clock_t start = clock();clock()-start < TEST_TIME;)           
  {
    int allocPos  = Random1024();
    int freePos   = Random1024();
    int allocSize = Random1024()*Random1024()*(Random1024()>>2);       
    
    if (mem [allocPos]) pool->free  (mem [allocPos]);
    mem [allocPos]    = pool->alloc (allocSize);   

    if (mem [allocPos] == NULL)
    {
      pool->reset ();
      mem [allocPos] = pool->alloc (allocSize);
    }  
    
    pool->free (mem [freePos]); mem [freePos] = NULL;
    
    total++;
  }
  
  clock_t end   = clock();
  float   time  = (end-start)/CLK_TCK;
  
  cout<<"Total:\t"<<total<<endl;
  cout<<"Total time:\t"<<int(time)<<endl;
  cout<<"FPS:\t"<<int(total/time)<<endl;
  
  delete pool;      
}

