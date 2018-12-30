#include "ActiveObject.h"

CActiveObject::CActiveObject()
{
}

CActiveObject::CActiveObject(CDomain *D)
{
	CGameObject::CGameObject(D);
	CActiveObject();
}

CActiveObject::~CActiveObject()
{
}