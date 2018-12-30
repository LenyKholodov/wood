// iostream_buf.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include <iostream.h>

static   char string [] = "Hello world";

class    buffer: public streambuf
{
  public:
	  buffer ():streambuf (NULL,0) {                
    	setbuf (string,strlen (string));
                }

    virtual     int underflow () {
		cout<<"underflow"<<endl;
	  static char* s = string;
	  return *s++;
    }

    virtual int overflow (int c)
    {
      cout<<"overflow"<<endl;
      return c;
    }

    streambuf* setbuf(char * s, int size)
    {
      cout<<"set buf"<<endl;
      return streambuf::setbuf (s,size);
    }

    streampos seekoff(streamoff, ios::seek_dir, int)
    {
      cout<<"seek off"<<endl;
      return 0;      
    }
};

void    main ()
{
  buffer             buf;
  istream_withassign is (&buf);

  char s [128];

  while (is)
  {	
	is>>s;

    cout<<s<<endl;
  } 
}