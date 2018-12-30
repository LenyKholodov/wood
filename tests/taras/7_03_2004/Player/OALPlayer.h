#ifndef __OALPlayer_H__
#define __OALPlayer_H__

#define MAX_BUF_LEN 1024*4 
#define MAX_BUFFERS 60

#include "use.h"

#include <windows.h>
#include <commctrl.h>

//#include <stdlib.h>
#include <alc.h>
#include <alut.h>
#include <eax.h>
//#include <Afxtempl.h>
#include <list>
#include <map>
#include <time.h>
//#include <stdlib.h>
#include "SoundSource.h"
#include "WaveResource.h"
#include "OggResource.h"
#include "Environment.h"
#include "SoundMaterial.h"
#include "OALListener.h"
//#include "ValidateIterator.h"

//#pragma comment(lib,"kerneld.lib")
#pragma comment(lib,"openal32.lib")
#pragma comment(lib,"alut.lib")
#pragma comment(lib,"eax.lib")
#pragma comment(lib,"eaxguid.lib")
//#pragma comment(lib,"resourced.lib")

class CSoundSource;
int CompareSources( const void *arg1, const void *arg2 );

typedef pair <DWORD, CSoundResource*> ResPair;

class COALPlayer
{
friend class CSoundSource;
friend class COALListener;
protected:
	ALCcontext *Context;
	ALCdevice *Device;

	list<CSoundSource*> SourcesList;
	list<CSoundSource*> PlayList;
	WORD ArraySize;
	CSoundSource **PlayingSourcesArray;
	int SourcesCount;
	map<DWORD,CSoundResource*> WaveList;
	static CSoundMaterial DefaultMaterial;
	static COALListener DefaultListener;

	COALListener *Listener;
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
	CSoundResource *CreateSound(char *file,bool Register=false);
	void DeleteSound(CSoundResource *res);
	bool RegisterSound(CSoundResource *Sound);
	void UnregisterSound(CSoundResource *Sound);
//---------------------------------------------------------
	void SetListener(COALListener *l);
	COALListener *GetListener(){return Listener;};
};


#endif