#include "stdafx.h"
#include "SoundResource.h"
#include <memory.h>
#include <alc.h>

CSoundResource::CSoundResource()
{
	memset(&m_Channels,0,sizeof(CSoundResource));
}

/*CSoundResource::CSoundResource(BYTE *Data, DWORD Len,WORD Channels,WORD BitsPerSample,DWORD SamplesPerSec)
{
	m_File=false;
	m_DataSize=Len;
	m_Channels=Channels;
	m_BitsPerSample=BitsPerSample;
	m_SamplesPerSec=SamplesPerSec;
	m_Data=new BYTE[Len];
	memcpy(m_Data,Data,m_DataSize);
	m_MaxData=Len;
}

CSoundResource::CSoundResource(char *fn)
{
	m_File=true;
	m_err=ERR_NO_ERROR;
	memcpy(m_FileName,fn,100); 
	BinFile* file = F_OpenFile (m_FileName,BF_OPEN_RO);
	if (IsValid (file))
	{
		m_DataSize=file->size();
		F_CloseFile(file);
		return;
	}
	m_FileName[0]=0;	
}
*/
CSoundResource::~CSoundResource()
{
	unload();
}

/*bool CSoundResource::load()
{
	m_Data=NULL;
	if((m_File)&(strlen(m_FileName)>0))
	{
		BinFile* file = F_OpenFile (m_FileName,BF_OPEN_RO);
		if (!IsValid (file))
		{
			m_err=ERR_CAN_NOT_OPEN_FILE;
			return false;
		}
		m_Data=new BYTE[file->size()];
		if (m_Data==NULL)
			return false;
		file->read(m_Data,file->size());
		F_CloseFile(file);
	}
	return true;
}*/

/*void CSoundResource::unload()
{
	if(m_File)
		if(m_Data!=NULL)
			delete m_Data;
	m_Data=NULL;
}*/

DWORD CSoundResource::ALFormat()
{
	if(m_err==ERR_NO_ERROR)
	{
		if((m_Channels==1)&&(m_BitsPerSample==8))
				return AL_FORMAT_MONO8;
		if((m_Channels==1)&&(m_BitsPerSample==16))
				return AL_FORMAT_MONO16;
		if((m_Channels==2)&&(m_BitsPerSample==8))
				return AL_FORMAT_STEREO8;
		if((m_Channels==2)&&(m_BitsPerSample==16))
				return AL_FORMAT_STEREO16;
	}
	return 0;
}

float CSoundResource::GetPriority()
{
	BYTE *b=(BYTE*)GetData();
	WORD *w=(WORD*)GetData();
	int i;
	DWORD DS;
	if(Priority!=0)
		return Priority;
	if(m_BitsPerSample==8)
	{
		DS=GetMaxDataSize();
		for(i=0;i<DS;i++)
			Priority+=sqr(b[i]);
	}
	else
	{
		DS=GetMaxDataSize()/2;
		for(i=0;i<DS;i++)
			Priority+=sqr(w[i]);
	}
	Priority=sqrt(Priority/DS);
	return Priority;
}

DWORD CSoundResource::GetData(DWORD Pos,DWORD Len,void *Data)
{
	return true;
}

void *CSoundResource::GetData()
{
	return m_Data;
}

float CSoundResource::GetTime()
{
	float res=8*m_MaxData/m_Channels/m_SamplesPerSec/m_BitsPerSample ;
	return 8*m_MaxData/m_Channels/m_SamplesPerSec/m_BitsPerSample ;
}
