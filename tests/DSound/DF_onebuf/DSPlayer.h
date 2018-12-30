#ifndef __DSPlayer_H__
#define __DSPlayer_H__

#include <windows.h>
#include <mmsystem.h>
#include <Dsound.h>
#include <kernel.h>
#include <eax.h>
//#include "eaxman.h"
#include "SoundSource.h"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "eax.lib")
#pragma comment(lib, "eaxguid.lib")
//#pragma comment(lib, "eaxman.lib")

class CDSPlayer
{
protected:
	IDirectSound *m_DS;
	bool m_DSCreated;
	IDirectSound3DListener* m_Listener;
	IDirectSoundBuffer *m_PrBuf;
	IKsPropertySet *m_Ps;
	bool m_ListenerCreated;
	DSCAPS m_DeviceParametrs;
	List <CSoundSource*> Sources;
	bool UseEAX;
public:
	CDSPlayer(HWND hWnd,char *PakName,char *ResTab);
	~CDSPlayer();
	CSoundSource *CreateSource(char *fn);

	void SetMaxVol(float m);
	void SetListenerPos(float p[]);
	void SetListenerOrientation(float p[]);
	void SetListenerVelociti(float p[]);
	void SetDopplerFactor(float factor=1);
	void SetRolloffFactor(float factor);
	void SetDistanceFactor(float factor);
};
#endif