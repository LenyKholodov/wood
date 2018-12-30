#include "stdafx.h"
#include "SoundSource.h"

EAXSet CSoundSource::ProcSetEax=NULL;
EAXGet CSoundSource::ProcGetEax=NULL;
COALPlayer *CSoundSource::Player=NULL;

void CSoundSource::Play(DWORD Pos)
{
	DWORD ID=(DWORD)this;
	if(iter.valid())
	{
		BeginPlayingPos=Pos;
		Player->PlayList.insert(ID,this);
		BeginPlayingTime=clock();
		Player->IsDirty=true;
	}

}

bool CSoundSource::Create()
{
	WaveFile->lock();
	LoadSourse();
	int err=alGetError();
	alGenSources(1,&Source);
	if(Source==NULL)
		return false;
	alGenBuffers(MAX_BUFFERS,Buffer);
	alSourcei(Source,AL_LOOPING,AL_FALSE);
	Created=true;
//----------------------------------------------------------
	for(int j=0;j<MAX_BUFFERS;j++)
	{
		DWORD DataLen=WaveFile->GetData(BeginPlayingPos,MAX_BUF_LEN,Player->TempBuf);
		BeginPlayingPos+=DataLen;
		alBufferData(Buffer[j],WaveFile->ALFormat(),
			Player->TempBuf,DataLen,WaveFile->GetFrequency());
	}
	alSourceQueueBuffers(Source, MAX_BUFFERS,Buffer ); 
//-----------------------------------------------------------------------------
	return true;
}

void CSoundSource::NextData()
{
int processed;
ALuint BufferID;
//BYTE *TempBuf;
	alGetSourcei(Source,AL_BUFFERS_PROCESSED,&processed);
	while(processed--)
	{
		alSourceUnqueueBuffers(Source, 1, &BufferID);
		//Добавление Даты
		DWORD DataLen=WaveFile->GetData(BeginPlayingPos,MAX_BUF_LEN,Player->TempBuf);
		BeginPlayingPos+=DataLen;
		if(DataLen>0)
		{
			alBufferData(BufferID, 
			WaveFile->ALFormat(),
			Player->TempBuf,DataLen,
			WaveFile->GetFrequency());
			alSourceQueueBuffers(Source, 1, &BufferID);
			int st;
			alGetSourcei(Source,AL_SOURCE_STATE,&st);
			if((DataPresent())&&(st!=AL_PLAYING))
				alSourcePlay(Source);
//			alGetSourcei(Source,AL_BUFFERS_PROCESSED,&processed);
		}
	}
}

void CSoundSource::Stop()
{
	Player->PlayList.erase((DWORD)this);
	Player->IsDirty=true;
//	alSourceStop(Source);
}

void CSoundSource::Resume()
{
	Play(BeginPlayingPos);
}

CSoundSource::CSoundSource(CSoundResource *wf)
{
	IsPlaying=false;
	CurBuf=0;
	WaveFile=wf;
	memset(&Position,0,sizeof(float)*16);
	InConeAngle=360;                                                                                                                     
	OutConeAngle=360;
	OutConeGlain=1;
	ReferenceDist=1000000;
	RolloffFactor=1;
	Gain=1;
	Material=&Player->DefaultMaterial;
}

CSoundSource::~CSoundSource()
{
	if(Created!=FALSE)
	{
		alDeleteSources(1, &Source);
		alDeleteBuffers(MAX_BUFFERS, Buffer);
		WaveFile->unlock();
	}
}

void CSoundSource::SetPos(float Pos[])
{
	memcpy(Position,Pos,sizeof(Position));
	if(Created)
		alSourcefv(Source,AL_POSITION,Pos);
	if(IsPlaying)
		Player->IsDirty=true;
}

void CSoundSource::SetOrientation(float Ori[])
{
	memcpy(Orientation,Ori,sizeof(Orientation));
	if(Created)
		alSourcefv(Source,AL_ORIENTATION,Ori);
	if(IsPlaying)
		Player->IsDirty=true;
}

void CSoundSource::SetVelociti(float Vel[])
{
	memcpy(Velociti,Vel,sizeof(Velociti));
	if(Created)
		alSourcefv(Source,AL_VELOCITY,Vel);
	if(IsPlaying)
		Player->IsDirty=true;
}

void CSoundSource::SetInConeAngle(float a)
{
	InConeAngle=a;
	if(Created)
		alSourcef(Source,AL_CONE_INNER_ANGLE,a);
	if(IsPlaying)
		Player->IsDirty=true;
}

void CSoundSource::SetOutConeAngle(float a)
{
	OutConeAngle=a;
	if(Created)
		alSourcef(Source,AL_CONE_OUTER_ANGLE,a);
	if(IsPlaying)
		Player->IsDirty=true;
}

void CSoundSource::SetMaxDist(float d)
{
	MaxDistance=d;
	if(Created)
		alSourcef(Source,AL_MAX_DISTANCE,d);
	if(IsPlaying)
		Player->IsDirty=true;
}

void CSoundSource::SetReferenceDist(float d)
{
	ReferenceDist=d;
	if(Created)
		alSourcef(Source,AL_REFERENCE_DISTANCE,d);
	if(IsPlaying)
		Player->IsDirty=true;
}

void CSoundSource::SetOutConeGlain(float g)
{
	OutConeGlain=g;
	if(Created)
		alSourcef(Source,AL_CONE_OUTER_GAIN,g);
	if(IsPlaying)
		Player->IsDirty=true;
}

/*void CSoundSource::SaveSourse()
{
	if(Created)
	{
		alGetSourcef(Source,AL_CONE_INNER_ANGLE,&InConeAngle);
		alGetSourcef(Source,AL_CONE_OUTER_GAIN,&OutConeGlain);
		alGetSourcef(Source,AL_MAX_DISTANCE,&MaxDistance);
		alGetSourcefv(Source,AL_ORIENTATION,Orientation);
		alGetSourcef(Source,AL_CONE_OUTER_ANGLE,&OutConeAngle);
		alGetSourcefv(Source,AL_POSITION,Position);
		alGetSourcefv(Source,AL_VELOCITY,Velociti);
		alGetSourcefv(Source,AL_REFERENCE_DISTANCE,&ReferenceDist);
		alGetSourcefv(Source,AL_ROLLOFF_FACTOR,&RolloffFactor);
		if(ProcGetEax)
			ProcGetEax(&DSPROPSETID_EAX_SourceProperties,DSPROPERTY_EAXBUFFER_ALLPARAMETERS,Source,&Material,sizeof(CSoundMaterial));
	}
}*/

void CSoundSource::LoadSourse()
{
	if(Created)
	{
		alSourcef(Source,AL_CONE_INNER_ANGLE,InConeAngle);
		alSourcef(Source,AL_CONE_OUTER_GAIN,OutConeGlain);
		alSourcef(Source,AL_MAX_DISTANCE,MaxDistance);
		alSourcefv(Source,AL_ORIENTATION,Orientation);
		alSourcef(Source,AL_CONE_OUTER_ANGLE,OutConeAngle);
		alSourcefv(Source,AL_POSITION,Position);
		alSourcefv(Source,AL_VELOCITY,Velociti);
		alSourcefv(Source,AL_REFERENCE_DISTANCE,&ReferenceDist);
		alSourcefv(Source,AL_ROLLOFF_FACTOR,&RolloffFactor);
		if(Material!=NULL)
			if(ProcSetEax)
				ProcSetEax(&DSPROPSETID_EAX_SourceProperties,DSPROPERTY_EAXBUFFER_ALLPARAMETERS,Source,Material,sizeof(CSoundMaterial));
	}
}

void CSoundSource::SetMaterial(CSoundMaterial *mat)
{
	if(mat!=NULL)
		if(ProcSetEax)
		{
			Material=mat;
			if(Created)
				ProcSetEax(&DSPROPSETID_EAX_SourceProperties,DSPROPERTY_EAXBUFFER_ALLPARAMETERS,Source,&Material,sizeof(CSoundMaterial));
		}
}

void CSoundSource::SetRolloffFactor(float f)
{
	RolloffFactor=f;
	if(Created)
		alSourcefv(Source,AL_ROLLOFF_FACTOR,&RolloffFactor);
	if(IsPlaying)
		Player->IsDirty=true;
}

void CSoundSource::Destroy()
{
//	SaveSourse();
	if(Created!=FALSE)
	{
		alDeleteSources(1, &Source);
		alDeleteBuffers(MAX_BUFFERS, Buffer);
		WaveFile->unlock();
		IsPlaying=false;
	}
}

void CSoundSource::SetGain(float g)
{
	Gain=g;
	if(Created)
		alSourcefv(Source,AL_GAIN,&g);
	if(IsPlaying)
		Player->IsDirty=true;
}

float CSoundSource::GetPriority()
{
	float dist=sqr(Player->Listener->Pos[0]-Position[0])+sqr(Player->Listener->Pos[1]-Position[1])+sqr(Player->Listener->Pos[2]-Position[2]);
	if(MaxDistance>dist)
		return 0;
//	float p=WaveFile->GetPriority()   ;
//		/(1+RolloffFactor*(dist-ReferenceDist)/ReferenceDist);
	return Gain*WaveFile->GetPriority()/
		(1+RolloffFactor*(dist-ReferenceDist)/ReferenceDist);
}

clock_t CSoundSource::EndPlayingTime()
{
	return BeginPlayingTime+WaveFile->GetTime()*CLOCKS_PER_SEC;
}

bool CSoundSource::DataPresent()
{
	int InQ;
	BYTE TempBuf;
	DWORD DataLen=WaveFile->GetData(BeginPlayingPos,1,&TempBuf);
	alGetSourcei(Source,AL_BUFFERS_QUEUED,&InQ);
	
	if((InQ!=0)&&(DataLen!=0))
		return true;
	else
		return false;
}