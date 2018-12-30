#include "Source.h"

float CSource::GetPriority()
{
	return CSoundSource::GetPriority();
}

float CSource::GetTime()
{
	return CSoundSource::GetTime();
}

bool CSource::DataPresent()
{
	return CSoundSource::DataPresent();
}

void CSource::SetPos(float Pos[])
{
	CSource::SetPos(Pos);
}

void CSource::SetOrientation(float Ori[])
{
	CSource::SetOrientation(Ori);
}

void CSource::SetVelociti(float Vel[])
{
	CSource::SetVelociti(Vel);
}

void CSource::SetInConeAngle(float a)
{
	CSource::SetInConeAngle(a);
}

void CSource::SetOutConeAngle(float a)
{
	CSource::SetOutConeAngle(a);
}

void CSource::SetMaxDist(float d)
{
	CSource::SetMaxDist(d);
}

void CSource::SetOutConeGlain(float g)
{
	CSource::SetOutConeGlain(g);
}

void CSource::SetReferenceDist(float d)
{
	CSource::SetReferenceDist(d);
}

void CSource::SetRolloffFactor(float f)
{
	CSource::SetRolloffFactor(f);
}

void CSource::SetGain(float g)
{
	CSource::SetGain(g);
}

void CSource::SetMaterial(CSoundMaterial *mat)
{
	CSource::SetMaterial(mat);
}

float *CSource::GetPos()
{
	return CSoundSource::GetPos();
}

float *CSource::GetOrientation()
{
	return CSoundSource::GetOrientation();
}

float *CSource::GetVelociti()
{
	return CSoundSource::GetVelociti();
}

float CSource::GetInConeAngle()
{
	return CSoundSource::GetInConeAngle();
}

float CSource::GetOutConeAngle()
{
	return CSoundSource::GetOutConeAngle();
}

float CSource::GetMaxDist()
{
	return CSoundSource::GetMaxDist();
}

float CSource::GetOutConeGlain()
{
	return CSoundSource::GetOutConeGlain();
}

float CSource::GetReferenceDist()
{
	return CSoundSource::GetReferenceDist();
}

float CSource::GetRolloffFactor()
{
	return CSoundSource::GetRolloffFactor();
}

float CSource::GetGain()
{
	return CSoundSource::GetGain();
}

CSoundMaterial *CSource::GetMaterial()
{
	return CSoundSource::GetMaterial();
}

void CSource::Play(DWORD Pos)
{
	CSoundSource::Play(Pos);
}

void CSource::Stop()
{
	CSoundSource::Stop();
}

void CSource::Resume()
{
	CSource::Resume();
}
