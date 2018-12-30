#include "stdafx.h"
#include "SoundSource.h"
#include "WaveFormat.h"
#include <eax.h>
#include <kernel.h>

CSoundSource::CSoundSource(IDirectSound *DS,char *fn)
{
	memcpy(m_FileName,fn,256);
	m_DS=DS;
	m_Created=false;
	m_Ps=NULL;
	HRESULT hr;

	SWaveFormat format;

	BinFile* file = F_OpenFile (m_FileName,BF_OPEN_RO);
	if (!IsValid (file))
		return;
	file->read((void *)&format,sizeof(SWaveFormat));

	if(format.wFormatTag==1)
	{
		m_Format.cbSize=0;
		m_Format.nAvgBytesPerSec=format.dwAvgBytesPerSec;
		m_Format.nBlockAlign=format.wBlockAlign;
		m_Format.nChannels=format.wChannels;
		m_Format.nSamplesPerSec=format.dwSamplesPerSec;
		m_Format.wBitsPerSample=format.wBitsPerSample;
		m_Format.wFormatTag=format.wFormatTag;
		m_Len=format.DataSize;

	m_BufParams.dwSize=sizeof(DSBUFFERDESC);
	m_BufParams.dwFlags=DSBCAPS_CTRL3D|DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLPOSITIONNOTIFY|
		DSBCAPS_CTRLVOLUME|DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_LOCDEFER|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_STATIC;
	m_BufParams.dwBufferBytes=m_Len;
	m_BufParams.dwReserved=0;
	m_BufParams.lpwfxFormat=&m_Format;
	m_BufParams.guid3DAlgorithm=DS3DALG_HRTF_FULL;
	hr=DS->CreateSoundBuffer(&m_BufParams,(LPDIRECTSOUNDBUFFER *)&m_Buf,NULL);
	if(hr==S_OK)
	{
		hr=m_Buf->QueryInterface(IID_IDirectSound3DBuffer,(LPVOID *)&m_Buf3D);
		if(hr==S_OK)
		{
				hr = m_Buf3D->QueryInterface( IID_IKsPropertySet, (void**)&m_Ps);
				if(hr==S_OK)
				{
ULONG ulSupport = 0;
 hr=m_Ps->QuerySupport(DSPROPSETID_EAX20_BufferProperties, 3, &ulSupport);
 if ( (ulSupport&(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)) == (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET) )
					{
						Sleep(200);
					}
				}
				m_Created=true;
		}
		else
			m_Buf->Release();
	}
	}
	F_CloseFile(file);
	LoadBuffer();
}

CSoundSource::~CSoundSource()
{
	if(m_Ps!=NULL)
		m_Ps->Release();
	m_Buf->Release();
	m_Buf3D->Release();
}

void CSoundSource::LoadBuffer()
{
	LPVOID p1;
	DWORD b;
	SWaveFormat format;
	BinFile* file = F_OpenFile (m_FileName,BF_OPEN_RO);
	if (!IsValid (file))
		return;
	file->read((void *)&format,sizeof(SWaveFormat));

	HRESULT hr=m_Buf->Lock(0,0,&p1,&b,0,0,DSBLOCK_ENTIREBUFFER );
	file->read(p1,b);
	hr=m_Buf->Unlock(p1,b,0,0);
	hr=m_Buf->SetCurrentPosition(0);
	F_CloseFile(file);
}

bool CSoundSource::Play()
{
HRESULT hr;
	if(m_Created)
	{
		hr=m_Buf->Play(0,0,0);
		if(hr==DSERR_BUFFERLOST)
		{
			LoadBuffer();
			hr=m_Buf->Play(0,0,0);
		}
	}
	if(hr==S_OK)
		return true;
	else
		return false;
}

void CSoundSource::SetPos(float p[])
{
	HRESULT hr=m_Buf3D->SetPosition(p[0],p[1],p[2],DS3D_IMMEDIATE);
}

void CSoundSource::SetOrientation(float p[])
{
	HRESULT hr=m_Buf3D->SetConeOrientation(p[0],p[1],p[2],DS3D_IMMEDIATE);
}

void CSoundSource::SetVelociti(float p[])
{
	HRESULT hr=m_Buf3D->SetVelocity(p[0],p[1],p[2],DS3D_IMMEDIATE);
}

void CSoundSource::SetMaxDist(float d)
{
	HRESULT hr=m_Buf3D->SetMaxDistance(d,DS3D_IMMEDIATE);
}

void CSoundSource::SetConeAngles(float in,float out)
{
	HRESULT hr=m_Buf3D->SetConeAngles(in,out,DS3D_IMMEDIATE);
}

void CSoundSource::SetConeOutsideVolume(long v)
{
	HRESULT hr=m_Buf3D->SetConeOutsideVolume(v,DS3D_IMMEDIATE);
}

void CSoundSource::SetVolume(long v)
{
	HRESULT hr=m_Buf->SetVolume(v);
}

void CSoundSource::Seek(DWORD Pos)
{
	HRESULT hr=m_Buf->SetCurrentPosition(Pos);
	hr=m_Buf->SetFrequency(50000);
}