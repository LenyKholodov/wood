#include <kernel.h>
#include <conio.h>
#include <kernel\ticker.h>

Ticker* tck = NULL;

class TestTicker: public TickClient 
{
  private:
        clock_t last [16];

  public:
    virtual void process (tickdata_t x) {
      cout<<x<<' '<<clock ()-last [x]<<endl;
      last [x] = clock ();
    }
};

class TestTicker1: public TickClient 
{
  private:
    tickevent_t ev [1024];

  public:
                TestTicker1 () { for (int i=0;i<1024;ev [i++] = -1); }
                ~TestTicker1 () {
                  for (int i=0;i<1024;i++)
                    if (ev [i] != -1) tck->DelEvent (ev [i]);                     
                }

    virtual void process (tickdata_t x) {
      cout<<"TT1: "<<x<<endl;
      int i = random (1024);
      ev [i] = tck->AddPeriodEvent (*this,random (1000),i);

      i = random (1024);
      if (ev [i] != -1)
        tck->DelEvent  (ev [i]);      
    }
};


void     main ()
{
  K_FastInit ("test9.map");

  TestTicker client;
  Ticker     ticker; tck = &ticker;
  TestTicker1 client1;

  cout<<clock ()<<endl;;

  ticker.AddPeriodEvent (client,120,0);
  ticker.AddPeriodEvent (client1,120,0);
  ticker.AddPeriodEvent (client,180,1);

  cout<<"wait"<<endl;

  while (!kbhit ());
}

