#include "OggResource.h"

ov_callbacks COgg::m_cb;

COgg::COgg(char *fn)
{
	m_err=ERR_NO_ERROR;
	memcpy(m_FileName,fn,100); 
	file=NULL;
	m_cb.close_func = Close;
	m_cb.read_func  = Read;
	m_cb.seek_func  = Seek;
	m_cb.tell_func  = Tell;

	m_VF = new OggVorbis_File;

	OpenFile(m_FileName,&file);
	
	if (IsValid(file))
	{
		ov_open_callbacks(&file, m_VF, NULL, -1, m_cb);
		m_Comment=ov_comment(m_VF, -1);
		m_Info=ov_info(m_VF, -1);
		m_DataSize=Size(file);
		m_Channels=m_Info->channels;
		m_SamplesPerSec=m_Info->rate;
		m_BitsPerSample=16;
		m_Data=NULL;
		m_MaxData=ov_pcm_total(m_VF,-1);

		load();
		return;
	}
	m_FileName[0]=0;	
}

COgg::~COgg()
{
//	CSoundResource::~CSoundResource();
	if(m_VF!=NULL)
		delete m_VF;
	unload();
}

void *COgg::GetData()
{
	return (BYTE*)m_Data+44;
}

DWORD COgg::GetData(DWORD Pos,DWORD Len,void *Data)
{
	int current_section;
	if(m_Data!=NULL)
		delete m_Data;
	m_Data=new BYTE[Len];
	__int64 p=Pos;
	ov_pcm_seek(m_VF,p);
	int res=ov_read(m_VF,(char*)m_Data,Len,
		0,2,1,&current_section);
	if(res>0)
	{
		memcpy(Data,m_Data,res);
		return res;
	}
	return 0;
}

bool COgg::load()
{
	return true;
}
void COgg::unload()
{
	if(m_Data!=NULL)
		delete m_Data;
	if(file!=0)
		close(file);
	m_Data=NULL;
}
