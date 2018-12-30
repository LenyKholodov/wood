#include "BCScenePhisObject.h"

static dMass Mass;

const dMass* MassSetSphere(float density,float r)
{
	dMassSetSphere(&Mass,density,r);
	return &Mass;
}

const dMass* MassSetCappedCylinder(dReal density,int direction,float r,float l)
{
	dMassSetCappedCylinder(&Mass,density,direction,r,l);
	return &Mass;
}

const dMass* MassSetCylinder(dReal density,int direction,float r,float l)
{
	dMassSetCylinder(&Mass,density,direction,r,l);
	return &Mass;
}

const dMass* MassSetBox(float density,float lx,float ly,float lz)
{
	dMassSetBox(&Mass,density,lx,ly,lz);
	return &Mass;
}

