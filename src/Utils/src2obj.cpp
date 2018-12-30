#include <string.h>
#include <iostream.h>

int      main (int argc,char* argv [])
{
  if (argc < 2)
    return 1;

  const char* dir = argv [1];

  for (int i=2;i<argc-1;i++)
  {
    char* dot = strchr (argv [i],'.');

    if (!dot)
      continue;

    *dot = 0;

    cout<<dir<<"/"<<argv [i]<<".obj ";
  }    

  return 0;
}