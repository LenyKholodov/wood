#define  DEBUG
#include <console\conbase.h>

class    ConTest: public ConsoleBase
{
  public:
                ConTest (Pool* pool,const char* str,int x,int y)
                  :ConsoleBase (pool,str,x,y) {}

  protected:
       virtual void  writeln   (int,const char* str)
       {
         cout<<str<<endl;
       }
};

void    main ()
{
  FloatPool pool (M_GetPool(),1024*1024);

  M_PushPool (&pool);

  ConTest con (M_GetPool(),"log",128,128);

  dout<<"init ok"<<endl;

  con.width   (12);
  con.message ("Hello big fucker and stupid world with no beer and nuts ******************!");

  cout<<"************"<<endl;
  con.refresh ();

  cout<<"************"<<endl;
}