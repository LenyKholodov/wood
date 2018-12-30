#include <misc.h>
#include <iostream.h>
#include <time.h>

void    main ()
{
  const int N = 10; 
  static int array [N] = {4,7,2,9,6,8,1,5,3,0};
  
//  for (int i=0;i<N;i++)
//    array [i] = random ((uint)-1);

  float start = clock ();

  qsort (array,N,array);

  float end   = clock ();

  cout<<"Time:\t"<<(end-start)/float (CLK_TCK)<<endl;

  for (int i=0;i<N;i++)
    cout<<array [i]<<endl;
}
