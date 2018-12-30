#ifndef __ActiveObject_h__
#define __ActiveObject_h__

#include "GameObject.h"
#include "Base.h"

class CActiveObject:public CGameObject // занаследовать от нити
{
public:
	CActiveObject();
	CActiveObject(CDomain *D);
	~CActiveObject();

private:
	virtual void Process();
	virtual void Idle();
};

#endif
