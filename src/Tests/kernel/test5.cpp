#include <default.h>
#include <kernel\timer.h>
#include <conio.h>

void     Idle (DWORD,clock_t);

void     main ()
{
  Timer  timer (CLK_TCK>>5,Idle);

  getch ();
}

void     Idle (DWORD id,clock_t delta)
{
  Timer* timer = (Timer*)id;

  cout<<"Delta: "<<delta<<" Range: "<<timer->GetInterval()<<endl;
}