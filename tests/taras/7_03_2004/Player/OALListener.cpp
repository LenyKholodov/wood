#include "stdafx.h"
#include "Environment.h"
#include "SoundSource.h"
#include "listener.h"

EAXSet COALListener::ProcSetEax=NULL;
EAXGet COALListener::ProcGetEax=NULL;
COALPlayer *COALListener::Player=NULL;

COALListener::COALListener()
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

void COALListener::SetEnvironment(unsigned long l)
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

void COALListener::SetEnvironment(CListenerEnvironment *e)
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

void COALListener::SetPos(float Pos[])
{
	memcpy(this->Pos,Pos,sizeof(this->Ori));
	if(Cur)
	{
		Player->IsDirty=true;
		alListenerfv(AL_POSITION,Pos);
	}
}

void COALListener::SetOrientation(float Ori[])
{
	memcpy(this->Ori,Ori,sizeof(this->Ori));
	if(Cur)
	{
		Player->IsDirty=true;
		alListenerfv(AL_ORIENTATION,Ori);
	}
}

void COALListener::SetVelociti(float Vel[])
{
	memcpy(this->Vel,Vel,sizeof(this->Vel));
	if(Cur)
	{
		Player->IsDirty=true;
		alListenerfv(AL_VELOCITY,Vel);
	}
}

void COALListener::SetDoppler(float factor,float velocity)
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

void COALListener::SetGain(float g)
{
	Gain=g;
	if(Cur)
	{
		Player->IsDirty=true;
		alListenerf(AL_GAIN,g);
	}
}

void COALListener::Apply()
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

void COALListener::SaveListener()
{
	if(ProcSetEax)
		ProcGetEax(&DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ALLPARAMETERS,0,&Env,sizeof(CListenerEnvironment));
}

float *COALListener::GetPos()
{
	return Pos;
}

float *COALListener::GetOrientation()
{
	return Ori;
}

float *COALListener::GetVelociti()
{
	return Vel;
}

float COALListener::GetGain()
{
	return Gain;
}

float COALListener::GetDopplerFactor()
{
	return DF;
}

float COALListener::GetDopplerVelocity()
{
	return DV;
}

CListenerEnvironment *COALListener::GetEnvironment()
{
	if(ProcSetEax)
	{
		ProcGetEax(&DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ALLPARAMETERS,0,&Env,sizeof(CListenerEnvironment));
		return &Env;
	}
	else
		return NULL;
}
