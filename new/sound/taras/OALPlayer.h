#ifndef __OALPlayer_H__
#define __OALPlayer_H__

#define MAX_BUF_LEN 1024*10 
#define MAX_BUFFERS 6

#include <alc.h>
#include <alut.h>
#include <eax.h>
//#include <Afxtempl.h>
#include <list.h>
#include <time.h>
#include <stdlib.h>
#include <kernel.h>
#include "SoundSource.h"
#include "WaveResource.h"
#include "Environment.h"
#include "SoundMaterial.h"
#include "Listener.h"

//#pragma comment(lib,"kerneld.lib")
#pragma comment(lib,"openal32.lib")
#pragma comment(lib,"alut.lib")
#pragma comment(lib,"eax.lib")
#pragma comment(lib,"eaxguid.lib")
#pragma comment(lib,"resourced.lib")

class CSoundSource;
int CompareSources( const void *arg1, const void *arg2 );

class COALPlayer
{
friend class CSoundSource;
friend class CListener;
protected:
	ALCcontext *Context;
	ALCdevice *Device;

	List<CSoundSource*> SourcesList;
	Dict<CSoundSource*> PlayList;
	WORD ArraySize;
	CSoundSource **PlayingSourcesArray;
	int SourcesCount;
	Dict<CSoundResource*> WaveList;
	static CSoundMaterial DefaultMaterial;
	static CListener DefaultListener;

	CListener *Listener;
	BYTE TempBuf[MAX_BUF_LEN];
	bool IsDirty;

	void RefreshArray();
	void RefreshData();
public:
	void Refresh();
	char *Extensions;
	char *DeviceName;
	char *Renderer;
	COALPlayer();
	~COALPlayer();

//---------------------------------------------------------
	CSoundSource *CreateSource(CSoundResource *Sound);
	void DeleteSource(CSoundSource *Source);
//---------------------------------------------------------
	bool RegisterSound(CSoundResource *Sound);
	void UnregisterSound(CSoundResource *Sound);
//---------------------------------------------------------
	void SetListener(CListener *l);
	CListener *GetListener(){return Listener;};
};
#endif