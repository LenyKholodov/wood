#include "stdafx.h"
#include "DSPlayer.h"

CDSPlayer::CDSPlayer(HWND hWnd,char *PakName,char *ResTab)
{
	HRESULT hr;
	F_RegisterPackFile (PakName);
	UseEAX=false;
	m_Ps=NULL;
	
	hr=CoInitialize(NULL);
	if(hr!=S_OK)
		return;
	hr = EAXDirectSoundCreate(NULL, (LPDIRECTSOUND *)&m_DS, NULL);
	if(hr==S_OK)
	{
		hr=m_DS->SetCooperativeLevel(hWnd,DSSCL_NORMAL);
		if(hr==S_OK)
			m_DSCreated=true;
		else
			m_DS->Release();
	}
	else
	{
		m_DSCreated=false;
		return;
	}


	DSBUFFERDESC     dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

	hr=m_DS->CreateSoundBuffer(&dsbd, (LPDIRECTSOUNDBUFFER *)&m_PrBuf, NULL);
	if (hr==S_OK)
	{
		m_ListenerCreated=true;
		hr=m_PrBuf->QueryInterface(IID_IDirectSound3DListener8,(LPVOID *)&m_Listener);
		if(hr!=S_OK)
		{
			m_PrBuf->Release();
			m_ListenerCreated=false;
			return;
		}
		hr = m_Listener->QueryInterface( IID_IKsPropertySet, (void**)&m_Ps);
		if(hr==S_OK)
		{
			DWORD support=0;
			hr=m_Ps->QuerySupport(DSPROPSETID_EAX20_ListenerProperties,
			DSPROPERTY_EAXLISTENER_ALLPARAMETERS, &support);
			hr=IKsPropertySet_QuerySupport(m_Ps,DSPROPSETID_EAX_ListenerProperties,DSPROPERTY_EAXLISTENER_ALLPARAMETERS,&support);
			if(support&(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET) == (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET))
			{
				Sleep(200);
			}
		}

	}

	m_DeviceParametrs.dwSize=sizeof(DSCAPS);
	hr=m_DS->GetCaps(&m_DeviceParametrs);
	hr=m_PrBuf->Play(0,0,DSBPLAY_LOOPING);
}

CDSPlayer::~CDSPlayer()
{
	List<CSoundSource*>::iterator iter=Sources;
	int c=Sources.count();
	for(int i=0;i<c;i++,iter++)
	{
		CSoundSource *src=iter.data();
		delete src;
	}
	if(m_Ps!=NULL)
		m_Ps->Release();
	if(m_ListenerCreated)
	{
		m_Listener->Release();
		m_PrBuf->Release();
	}
	if(m_DSCreated)
		m_DS->Release();
	CoUninitialize();
}

CSoundSource *CDSPlayer::CreateSource(char *fn)
{
	HRESULT hr;
	CSoundSource *src=new CSoundSource(m_DS,fn);
	Sources.InsertLast(src);
	return src;
}

void CDSPlayer::SetMaxVol(float m)
{
	HRESULT hr=m_PrBuf->SetVolume(m);
}

void CDSPlayer::SetListenerPos(float p[])
{
	HRESULT hr=m_Listener->SetPosition(p[0],p[1],p[2],DS3D_IMMEDIATE);
}

void CDSPlayer::SetListenerOrientation(float p[])
{
	HRESULT hr=m_Listener->SetOrientation(p[0],p[1],p[2],p[3],p[4],p[5],DS3D_IMMEDIATE);
}

void CDSPlayer::SetListenerVelociti(float p[])
{
	HRESULT hr=m_Listener->SetVelocity(p[0],p[1],p[2],DS3D_IMMEDIATE);	
}

void CDSPlayer::SetDopplerFactor(float factor)
{
	HRESULT hr=m_Listener->SetDopplerFactor(factor,DS3D_IMMEDIATE);
}

void CDSPlayer::SetRolloffFactor(float factor)
{
	HRESULT hr=m_Listener->SetRolloffFactor(factor,DS3D_IMMEDIATE);
}

void CDSPlayer::SetDistanceFactor(float factor)
{
	HRESULT hr=m_Listener->SetDistanceFactor(factor,DS3D_IMMEDIATE);
}
