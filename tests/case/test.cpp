#include <iostream.h>

void     main ()
{
  int x;

  cin>>x;

  switch (x)
  {
    case 1:  cout<<"1"<<endl; break;
    case 2:  cout<<"1"<<endl; break;
    case 0xAFFF: cout<<"0xAFFF"<<endl; break;
    default: cout<<"default"<<endl;  break;
  }
}