#define  DEBUG
#include <pch.h>
#include <default.h>
#include <fixed\fixed.h>

#include <math.h>
#include <stdlib.h>

void    FormSin(BinFile* file);

int    main(int argc,char* argv[])
{          
 
  if (argc<2)
  {
     stdBinFile res ("table.dat",BF_OPEN_RW);
     FormSin        (&res);
  }     
  else
  {
     stdBinFile res (argv[1],BF_OPEN_RW);
     FormSin        (&res);
  }   

  return 0;
}

void    FormSin(BinFile* file)
{
  char header[] = "const long __sin[8192] = \r{\r\t";
  file->write(header,strlen(header));
  Angle a;
  double angle;
  Fixed  value;
  char    buffer[100];
  int     count=0;
  for (long ij=0;ij<8192;ij++) //!!!8192
  {
   a = ij<<3;
   angle = angle2float(a);
   value = float2fixed(sin(angle));

   ltoa(value,buffer,10);
   if (count++==8)
   {
      file->write("\r\t",strlen("\r\t"));
      count=0;
   }
   file->write(buffer,strlen(buffer));
   file->write(", ",strlen(", "));
  }
  file->write("\r};\r\r",strlen("\r};\r\r"));
}
