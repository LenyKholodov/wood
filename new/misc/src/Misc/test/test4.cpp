#include <kernel.h>
#include <conio.h>
#include <time.h>
#include <misc\list.h>

template <class T> class RT: public List<T>
{
  public:
              RT (Pool* pool = Memory::M_GetPool()): List<T> (pool) { reset (); }

   virtual    void      reset ()
   {
     List<T>::reset ();

     mFirst.prev = &mLast;
     mLast.next  = &mFirst;
   }
};

void     main ()
{
  K_FastInit ("test4.map");

  RT<int> r;            

  r += 10;

  RT<int>::iterator iter   = r;

  int  total = 10000000;
  float start = clock ();

  for (int i=0;i<total;iter++,i++);

  float end = clock ();

  cout<<"FPS: "<<float (total*CLK_TCK)/(end-start)<<endl;
}
