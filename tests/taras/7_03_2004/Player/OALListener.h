#ifndef __OALListener_h__
#define __OALListener_h__

#include <windows.h>
#include <commctrl.h>

//#include <stdlib.h>
#include <alc.h>
#include <alut.h>
#include <eax.h>
#include "environment.h"

class COALListener
{
friend class CSoundSource;
friend class COALPlayer;
protected:
	bool Cur;
	ALfloat Pos[3];
	ALfloat Vel[3];
	ALfloat Ori[6];
	ALfloat DV;
	ALfloat DF;
	ALfloat Gain;
	static EAXSet ProcSetEax;
	static EAXGet ProcGetEax;
	static COALPlayer *Player;
	CListenerEnvironment Env;
	void Apply();
	void SaveListener();
public:
	COALListener();
	void SetPos(float Pos[]);
	void SetOrientation(float Ori[]);
	void SetVelociti(float Vel[]);
	void SetGain(float g);
	void SetDoppler(float factor=1,float velocity=343.3);
	void SetEnvironment(unsigned long l);
	void SetEnvironment(CListenerEnvironment *e);

	float *GetPos();
	float *GetOrientation();
	float *GetVelociti();
	float GetGain();
	float GetDopplerFactor();
	float GetDopplerVelocity();
	CListenerEnvironment *GetEnvironment();
};

#endif