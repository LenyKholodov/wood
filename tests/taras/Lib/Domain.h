#ifndef __Domain_h__
#define __Domain_h__

#include "ActiveObject.h"
#include "GameObject.h"
#include "Base.h"

class CGameObject;
class CSocket;

class CDomain:public CActiveObject
{
protected:
        map <DWORD,CGameObject*> Objects; //??
        map <DWORD,CSocket*> Sockets;

public:
        CDomain();
        CDomain(CDomain *D);
        ~CDomain();

protected:
        AddObject(CGameObject *Obj,DWORD ID);
};

#endif