#ifndef __WaveResource_H__
#define __WaveResource_H__

#include "SoundResource.h"

class CWave:public CSoundResource
{
protected:
	char m_FileName[100];
	WORD m_FormatTag;
public:
	CWave(char *fn);
	virtual bool load();
	virtual void unload();
	virtual DWORD GetData(DWORD Pos,DWORD Len,void *Data);
	virtual void *GetData();
//	virtual float GetPriority();
};
#endif
