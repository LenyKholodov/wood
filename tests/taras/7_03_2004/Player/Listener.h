#ifndef __Listener_h__
#define __Listener_h__

#include "OALListener.h"

class CListener:private COALListener
{
public:
	CListener();
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