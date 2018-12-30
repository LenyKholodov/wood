#include "domain.h"

CDomain::CDomain()
{
}

CDomain::CDomain(CDomain *D)
{
	CActiveObject::CActiveObject(D);
	CDomain();
}

CDomain::~CDomain()
{
}

CDomain::AddObject(CGameObject *Obj,DWORD ID)
{
//	Objects[ID]=Obj;
}
