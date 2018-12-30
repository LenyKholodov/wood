#ifndef __Player_h__
#define __Player_h__

#include "OALPlayer.h"
#include "Source.h"
#include "Listener.h"

class CPlayer:private COALPlayer
{
public:
	CPlayer();
	~CPlayer();
//---------------------------------------------------------
	CSource *CreateSource(CSoundResource *Sound);
	void DeleteSource(CSource *Source);
//---------------------------------------------------------
	CSoundResource *CreateSound(char *file,bool Register=false);
	void DeleteSound(CSoundResource *res);
	bool RegisterSound(CSoundResource *Sound);
	void UnregisterSound(CSoundResource *Sound);
//---------------------------------------------------------
	void SetListener(CListener *l);
	CListener *GetListener();

	void Refresh();
};

#endif