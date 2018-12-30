#include "stdafx.h"
#include "WaveResource.h"
#include "Files.h"

CWave::CWave(char *fn)
{
	m_err=ERR_NO_ERROR;
	memcpy(m_FileName,fn,100); 
	SoundFile file;
	file=NULL;
	OpenFile(m_FileName,&file);
	if (IsValid(file))
	{
		m_DataSize=Size(file);
		Close(&file);
//		memcpy((void*)&m_FormatTag,(BYTE*)m_Data+20,14);
//		memcpy((void*)&m_MaxData,(BYTE*)m_Data+40,4);
		load();
		return;
	}
	m_FileName[0]=0;	
}

CWave::~CWave()
{
//	CSoundResource::~CSoundResource();
	unload();
}

void *CWave::GetData()
{
	return (BYTE*)m_Data+44;
}

DWORD CWave::GetData(DWORD Pos,DWORD Len,void *Data)
{
//	Pos-=3;
	if(44+Pos<m_DataSize)
	{
		if(44+Pos+Len>m_DataSize)
			Len=m_DataSize-Pos;
		if(Len!=0)
			memcpy(Data,(BYTE*)m_Data+44+Pos,Len);
		else
			return 0;
		return Len;
	}
	return 0;
}

bool CWave::load()
{
//	void *dd=m_Data;
/*	if(m_Data==NULL)
		m_Data=new BYTE[m_DataSize];

	
	m_Data=NULL;*/
	if(strlen(m_FileName)>0)
	{
		SoundFile file;
		OpenFile(m_FileName,&file);
		if (!IsValid (file))
		{
			m_err=ERR_CAN_NOT_OPEN_FILE;
			return false;
		}
		m_Data=new BYTE[Size(file)];
		if (m_Data==NULL)
			return false;
		Read(m_Data,1,Size(file),&file);
		Close(&file);
//	for(int i=0;i<;i++)
		memcpy((void*)&m_FormatTag,(BYTE*)m_Data+20,2);
		memcpy((void*)&m_Channels,(BYTE*)m_Data+22,2);
		memcpy((void*)&m_SamplesPerSec,(BYTE*)m_Data+24,4);
		memcpy((void*)&m_AvgBytesPerSec,(BYTE*)m_Data+28,4);
		memcpy((void*)&m_BlockAlign,(BYTE*)m_Data+32,2);
		memcpy((void*)&m_BitsPerSample,(BYTE*)m_Data+34,2);
		memcpy((void*)&m_MaxData,(BYTE*)m_Data+40,4);
		return true;
	}
	return false;
}

/*float CWave::GetPriority()
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
}*/

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

void CWave::unload()
{
	if(m_Data!=NULL)
		delete m_Data;
	m_Data=NULL;
}
