#include <engine\stream.h>

void     main ()
{
  FlatMessageStream s (M_GetPool (),231);

  int    a;
  char   b;
  int16  c;
  double d;  
  char   e [128] = {0};
  char   f [128] = "hello";
  char   g [128] = {0};

  s.printf ("Fucked message",NoObject,NoObject,
            "%i%c%w%e%s%m",60000,'a',-1000,0.000000000001,"Hello world!",strlen (f)+1,f
           );
  s.scanf  (NULL,NULL,NULL,
            "%i%c%w%e%s%m",&a,&b,&c,&d,e,strlen (f)+1,g
           );

  cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<g<<endl;
}