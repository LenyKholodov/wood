#include <iostream.h>
#include <render\state.h>
#include <time.h>

class TestState: public RenderState
{
  protected:
    virtual uchar bind () { return 1; }
    virtual void  pass (uchar) {}
    virtual void  unbind () {}
};

void main ()
{
  const uint total = 10000000;

  TestState state1;
  TestState* state = &state1;

  clock_t start = clock ();

  for (int i=total;i--;)
  {
    state->Bind ();

    uchar count = state->GetPassCount ();

    for (int j=0;j<count;j++)
    {
      state->SetPass (j);
    }

    state->Unbind ();
  }

  clock_t end = clock ();

  cout<<"FPS:\t"<<float (total)/float (end-start)*float (CLK_TCK)<<endl;
}