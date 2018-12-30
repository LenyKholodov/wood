#include <debug.h>

#undef D_Try
#undef D_Exception
#undef D_Exc
#undef D_ExcRet

#define D_Try   try
#define D_Exception(X) catch (...)
#define D_Exc(X)       catch (...) { dout<<X<<endl; }
#define D_ExcRet(X,Y)  catch (...) { dout<<X<<endl; return Y; }
