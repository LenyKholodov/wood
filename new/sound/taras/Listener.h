#ifndef __Listener_h__
#define __Listener_h__

#include <alc.h>
#include <alut.h>
#include <eax.h>

class CListener
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
	CListener();
	void SetPos(float Pos[]);
	void SetOrientation(float Ori[]);
	void SetVelociti(float Vel[]);
	void SetGain(float g);
	void SetDoppler(float factor=1,float velocity=343.3);
	void SetEnvironment(unsigned long l);
	void SetEnvironment(CListenerEnvironment *e);
};

#endif