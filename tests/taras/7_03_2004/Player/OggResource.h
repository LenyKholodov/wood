#ifndef __OggResource_h__
#define __OggResource_h__

#include "SoundResource.h"
#include "Files.h"

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#pragma comment(lib, "ogg.lib")
#pragma comment(lib, "vorbisfile.lib")

class COALPlayer;

class COgg:public CSoundResource
{
friend class COALPlayer;
private:
	static ov_callbacks  m_cb;
protected:
	OggVorbis_File  *m_VF;
	vorbis_comment  *m_Comment;
	vorbis_info    *m_Info;
	SoundFile file;
protected:
	char m_FileName[100];
	WORD m_FormatTag;
	COgg(char *fn);
	virtual ~COgg();
public:
	virtual bool load();
	virtual void unload();
	virtual DWORD GetData(DWORD Pos,DWORD Len,void *Data);
	virtual void *GetData();
};

#endif