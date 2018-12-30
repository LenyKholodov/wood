#ifndef __WaveResource_H__
#define __WaveResource_H__

#include "SoundResource.h"

class COALPlayer;

class CWave:public CSoundResource
{
friend class COALPlayer;
protected:
	char m_FileName[100];
	WORD m_FormatTag;
	DWORD  m_AvgBytesPerSec;
	WORD m_BlockAlign;
	CWave(char *fn);
	virtual ~CWave();
public:
	virtual bool load();
	virtual void unload();
	virtual DWORD GetData(DWORD Pos,DWORD Len,void *Data);
	virtual void *GetData();
//	virtual float GetPriority();
};
#endif
