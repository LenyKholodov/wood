#include <misc\excpt.h>

ostream& operator << (ostream& os,Exception& exc)
{
  return exc.print (os);
}