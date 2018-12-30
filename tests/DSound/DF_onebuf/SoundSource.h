#ifndef __SoundSource_H__
#define __SoundSource_H__

#include <windows.h>
#include <mmsystem.h>
#include <Dsound.h>

class CDSPlayer;

class CSoundSource
{
friend CDSPlayer;
protected:
	CSoundSource(IDirectSound *DS,char *fn);
public:
	~CSoundSource();

    bool Play();

protected:
	void LoadBuffer();

	char m_FileName[256];
	IDirectSoundBuffer *m_Buf;
	IDirectSound3DBuffer *m_Buf3D;
	DWORD m_Len;
	WAVEFORMATEX m_Format;
	DSBUFFERDESC m_BufParams;
	IDirectSound *m_DS;
	IKsPropertySet *m_Ps;
	bool m_Created;

public:
	void SetPos(float p[]);
	void SetOrientation(float p[]);
	void SetVelociti(float p[]);
	void SetMaxDist(float d);
	void SetReferenceDist(float d);
	void SetConeAngles(float in,float out);
	void SetConeOutsideVolume(long v);
	void SetVolume(long v);
};

#endif