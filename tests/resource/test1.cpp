#include <kernel.h>
#include <resource\presource.h>
#include <conio.h>
#include <time.h>

const size_t SIZE_OF_SINGLE_RESOURCE = 64;
const size_t RECOMMENDED_SIZE = 6*SIZE_OF_SINGLE_RESOURCE;

class TestPriorResource: public PriorResource
{
  private:
        int     index;

  public:
                TestPriorResource (int i,PriorResourceGroup* group):PriorResource (group),index(i) {}

   virtual size_t size () const { return SIZE_OF_SINGLE_RESOURCE; }
   virtual bool   load ()
   {
     if (!PriorResource::load ())
       return false;

     cout<<"load resource #"<<index<<endl;

     return true;
   }

   virtual void   unload ()
   {
     PriorResource::unload ();

     cout<<"unload resource #"<<index<<endl;
   }
};

void     main ()
{
  K_FastInit ("test1.map");

  PriorResourceGroup group (RECOMMENDED_SIZE);

  PriorResource* res [1024] = {NULL};

  for (int i=0;i<1024;i++)
  {
    res [i] = new (::M_GetPool()) TestPriorResource (i,&group); 

    if (!res [i])
    {  
      cout<<"error at create resource #"<<i<<endl;
    }
  }  

  clock_t last = clock ();

  while (!kbhit ())
  {
    int i = random (1024);

    res [i]->lock ();
    res [i]->unlock ();

    if (clock () - last > CLK_TCK<<2)
    {
      group.process ();
      last = clock ();      
    }

    Sleep (150);
  }  
}