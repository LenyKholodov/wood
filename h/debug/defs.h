#ifndef __DEBUG_DEFINES_FOR_WOOD__
#define __DEBUG_DEFINES_FOR_WOOD__

#include <types.h>
#include <excpt.h>

extern std::ostream* debugstream;

#define dout (*debugstream)

#ifdef DEBUG
  #pragma comment (linker,"/MAPINFO:EXPORTS /MAPINFO:LINES")
#endif

#define D_Try                      __try 
#define D_Exception                __except
#define D_Message(X)               dout<<X<<endl;
#define D_NoMemory(X,SIZE)         dout<<X<<':'<<" No enough memory: "<<SIZE<<std::endl;
#define D_NoMemoryRet(X,SIZE,RET)  {dout<<X<<':'<<" No enough memory: "<<SIZE<<std::endl; return RET;}
#define D_Check(X,MSG,SIZE)        {!(X) ? dout<<MSG<<':'<<" No enough memory "<<SIZE<<endl; : ; }
#define D_CheckRet(X,MSG,SIZE,RET) {if (!(X)) { dout<<MSG<<':'<<" No enough memory "<<SIZE<<std::endl; return RET; } }
#define D_Exc(X)                   __except (EXCEPTION_EXECUTE_HANDLER) { \
                                   dout<<X<<std::endl; \
                                    }
#define D_ExcRet(X,Y)              __except (EXCEPTION_EXECUTE_HANDLER) { \
                                   dout<<X<<std::endl; \
                                   return Y; \
                                    }


#endif