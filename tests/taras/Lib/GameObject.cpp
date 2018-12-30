#include "GameObject.h"

CGameObject::CGameObject()
{
        Domain=NULL;
}

CGameObject::CGameObject(CDomain *D)
{
        Domain=D; 
}
