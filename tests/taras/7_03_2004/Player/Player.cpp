#include "Player.h"

CPlayer::CPlayer()
{
	COALPlayer::COALPlayer();
}

CPlayer::~CPlayer()
{
	COALPlayer::~COALPlayer();
}

CSource *CPlayer::CreateSource(CSoundResource *Sound)
{
	return (CSource*)COALPlayer::CreateSource(Sound);
}

void CPlayer::DeleteSource(CSource *Source)
{
	return COALPlayer::DeleteSource((CSoundSource*)Source);
}

CSoundResource *CPlayer::CreateSound(char *file,bool Register)
{
	return COALPlayer::CreateSound(file,Register);
}

void CPlayer::DeleteSound(CSoundResource *res)
{
	COALPlayer::DeleteSound(res);
}

bool CPlayer::RegisterSound(CSoundResource *Sound)
{
	return COALPlayer::RegisterSound(Sound);
}

void CPlayer::UnregisterSound(CSoundResource *Sound)
{
	COALPlayer::UnregisterSound(Sound);
}

void CPlayer::SetListener(CListener *l)
{
	COALPlayer::SetListener((COALListener*)l);
}

CListener *CPlayer::GetListener()
{
	return (CListener*)COALPlayer::GetListener();
}

void CPlayer::Refresh()
{
	COALPlayer::Refresh();
}
