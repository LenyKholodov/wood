#ifndef __Source_h__
#define __Source_h__

#include "SoundSource.h"

class CSource:private CSoundSource
{
public:
	float GetPriority();
	float GetTime();
	bool DataPresent();

	void SetPos(float Pos[]);
	void SetOrientation(float Ori[]);
	void SetVelociti(float Vel[]);
	void SetInConeAngle(float a);
	void SetOutConeAngle(float a);
	void SetMaxDist(float d);
	void SetOutConeGlain(float g);
	void SetReferenceDist(float d);
	void SetRolloffFactor(float f);
	void SetGain(float g);
	void SetMaterial(CSoundMaterial *mat);

	float *GetPos();
	float *GetOrientation();
	float *GetVelociti();
	float GetInConeAngle();
	float GetOutConeAngle();
	float GetMaxDist();
	float GetOutConeGlain();
	float GetReferenceDist();
	float GetRolloffFactor();
	float GetGain();
	CSoundMaterial *GetMaterial();

	void Play(DWORD Pos);
	void Stop();
	void Resume();
};

#endif