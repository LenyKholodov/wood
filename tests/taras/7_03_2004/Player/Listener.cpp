#include "Listener.h"

CListener::CListener()
{
	COALListener::COALListener();
}

void CListener::SetPos(float Pos[])
{
	COALListener::SetPos(Pos);
}

void CListener::SetOrientation(float Ori[])
{
	COALListener::SetOrientation(Ori);
}

void CListener::SetVelociti(float Vel[])
{
	COALListener::SetVelociti(Vel);
}

void CListener::SetGain(float g)
{
	COALListener::SetGain(g);
}

void CListener::SetDoppler(float factor,float velocity)
{
	COALListener::SetDoppler(factor,velocity);
}

void CListener::SetEnvironment(unsigned long l)
{
	COALListener::SetEnvironment(l);
}

void CListener::SetEnvironment(CListenerEnvironment *e)
{
	COALListener::SetEnvironment(e);
}

float *CListener::GetPos()
{
	return COALListener::GetPos();
}

float *CListener::GetOrientation()
{
	return COALListener::GetOrientation();
}

float *CListener::GetVelociti()
{
	return COALListener::GetVelociti();
}

float CListener::GetGain()
{
	return COALListener::GetGain();
}

float CListener::GetDopplerFactor()
{
	return COALListener::GetDopplerFactor();
}

float CListener::GetDopplerVelocity()
{
	return COALListener::GetDopplerVelocity();
}

CListenerEnvironment *CListener::GetEnvironment()
{
	return COALListener::GetEnvironment();
}
