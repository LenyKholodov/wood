#include <iostream.h>

typedef int  (*start_fn)(int,char* []);

void    start ();

void    main ()
{
  start_fn fn1 = (start_fn)start;
  int      y   = 2;

  cout<<(*fn1)(0,NULL)<<endl;;

  int     x    = 1;

  cout<<x<<" "<<y<<endl;
}

void    start ()
{
  int x = 1;

  cout<<"Hey-ho!"<<endl;
  cout<<x<<endl;
}
