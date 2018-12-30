#include <iostream.h>


struct token_t
{
  int           type;   //’¨¯ 

  union {
    const char* str;  
    float       f;
    double      d;  
    int         i;
    short       s;
    char        c;
  }var;
};
