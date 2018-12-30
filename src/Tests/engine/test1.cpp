#include <engine\stream.h>
#include <conio.h>

void     main ()
{
  FlatMessageStream s (M_GetPool (),231);

  bool flag = false;
  int total = 0;

  while (!kbhit ())
  {
    s.BeginWrite ("Fucked message",NoObject);

    s<<total++;

    s.EndWrite   ();

    if (flag)
      s.BeginRead ();

    int id = -1;

    s>>id; cout<<id<<endl;  

    if (flag)
      s.EndRead ();

    flag = true;
  }
}