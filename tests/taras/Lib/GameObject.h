#ifndef __GameObject_h__
#define __GameObject_h__

#include "Base.h"
//#include "Domain.h"

class CDomain;

class CGameObject
{
protected:
        CDomain *Domain;
public:
        CGameObject();
        CGameObject(CDomain *D); //protected

        virtual void Handler(CStream *Stream, MsgHeader *Header,byte *Msg);
};

#endif