#ifndef __Socket_h__
#define __Socket_h__

#include "Base.h"

class CSocket
{
friend class CDomain;
private:
        DWORD Count; //
public:
        CSocket();
        virtual SendMsg(DWORD ID, DWORD From, DWORD Size, BYTE *msg);
};

#endif