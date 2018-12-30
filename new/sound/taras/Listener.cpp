#include "stdafx.h"
#include "Environment.h"
#include "SoundSource.h"
#include "listener.h"

EAXSet CListener::ProcSetEax=NULL;
EAXGet CListener::ProcGetEax=NULL;
COALPlayer *CListener::Player=NULL;

CListener::CListener()
{
	ALfloat listenerPos[]={0.0,0.0,0.0};
	ALfloat listenerVel[]={0.0,0.0,0.0};
	ALfloat	listenerOri[]={0.0,0.0,-1.0, 0.0,1.0,0.0};
	memcpy(Pos,listenerPos,sizeof(Pos));
	memcpy(Vel,listenerVel,sizeof(Vel));
	memcpy(Ori,listenerOri,sizeof(Ori));
	DF=1;
	DV=343.3;
	Cur=false;
	Gain=1;

/*	if(CSoundSource::Player!=NULL)
	{
		alListenerfv(AL_POSITION,Pos);
		alListenerfv(AL_VELOCITY,Vel);
		alListenerfv(AL_ORIENTATION,Ori);
		alDopplerVelocity(DV);
		alDopplerFactor(DF);
	}*/
}

void CListener::SetEnvironment(unsigned long l)
{
	if(Cur)
	if(ProcSetEax)
	{
		ProcSetEax(&DSPROPSETID_EAX_ListenerProperties,
                   DSPROPERTY_EAXLISTENER_ENVIRONMENT,
                   0,
                   &l,
                   sizeof(unsigned long));
	}
}

void CListener::SetEnvironment(CListenerEnvironment *e)
{
	if(e!=NULL)
	{
		memcpy(&Env,e,sizeof(Env));
		if(Cur&&(ProcGetEax!=NULL))
		{
			ProcSetEax(&DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ALLPARAMETERS,0,&Env,sizeof(CListenerEnvironment));
		}
	}
}

void CListener::SetPos(float Pos[])
{
	memcpy(this->Pos,Pos,sizeof(this->Ori));
	if(Cur)
	{
		Player->IsDirty=true;
		alListenerfv(AL_POSITION,Pos);
	}
}

void CListener::SetOrientation(float Ori[])
{
	memcpy(this->Ori,Ori,sizeof(this->Ori));
	if(Cur)
	{
		Player->IsDirty=true;
		alListenerfv(AL_ORIENTATION,Ori);
	}
}

void CListener::SetVelociti(float Vel[])
{
	memcpy(this->Vel,Vel,sizeof(this->Vel));
	if(Cur)
	{
		Player->IsDirty=true;
		alListenerfv(AL_VELOCITY,Vel);
	}
}

void CListener::SetDoppler(float factor,float velocity)
{
	DV=velocity;
	DF=factor;
	if(Cur)
	{
		Player->IsDirty=true;
		alDopplerVelocity(velocity);
		alDopplerFactor(factor);
	}
}

void CListener::SetGain(float g)
{
	Gain=g;
	if(Cur)
	{
		Player->IsDirty=true;
		alListenerf(AL_GAIN,g);
	}
}

void CListener::Apply()
{
	if(CSoundSource::Player!=NULL)
	{
		Player->IsDirty=true;
		alListenerfv(AL_POSITION,Pos);
		int err=alGetError();
		alListenerfv(AL_VELOCITY,Vel);
		alListenerfv(AL_ORIENTATION,Ori);
		alDopplerVelocity(DV);
		alDopplerFactor(DF);
		alListenerf(AL_GAIN,Gain);
	}
}

void CListener::SaveListener()
{
	if(ProcSetEax)
		ProcGetEax(&DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ALLPARAMETERS,0,&Env,sizeof(CListenerEnvironment));
}