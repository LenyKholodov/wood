#ifndef __SoundResource_H__
#define __SoundResource_H__

#include <windows.h>
#include <altypes.h>

//#pragma comment(lib,"resourced.lib")

class COALPlayer;

class CSoundResource//:public PriorResource
{
friend class COALPlayer;
protected:
//	WORD m_FormatTag;
	WORD m_Channels;
	WORD m_BitsPerSample;
	DWORD  m_SamplesPerSec;
//	char m_FileName[100];
	void *m_Data;
	DWORD m_DataSize;
	BYTE m_err;
	DWORD m_MaxData;
	float Priority;
	CSoundResource();
	virtual ~CSoundResource();
public:
//	CSoundResource(char *fn);
//	CSoundResource(BYTE *Data,DWORD Len,WORD Channels,WORD BitsPerSample,DWORD SamplesPerSec);
	BYTE GetError() {return m_err;};
//	bool load();
//	void unload();
	virtual size_t size() {return m_DataSize;};
	virtual float GetPriority();
	virtual DWORD GetData(DWORD Pos,DWORD Len,void *Data);
	virtual DWORD GetMaxDataSize(){return m_MaxData;};
	virtual void *GetData();
	virtual float GetTime();
//	static CSoundResource *Create(char *fn);
//	static CSoundResource *Create(BYTE *Data,DWORD Len,WORD Channels,WORD BitsPerSample,DWORD SamplesPerSec);
	void *Data() {return m_Data;}
	DWORD DataSize() {return m_MaxData;};
	ALenum ALFormat();
	ALsizei GetFrequency() 
	{
		return m_SamplesPerSec;
	};
};

enum
{
	ERR_NO_ERROR,
	ERR_NOT_RIFF,
	ERR_NOT_WAVE,
	ERR_NO_FORMAT_BLOCK,
	ERR_NO_DATA_BLOCK,
	ERR_UNKNOWN_FORMAT,
	ERR_CAN_NOT_OPEN_FILE
};
#endif