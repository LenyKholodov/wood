#include "stdafx.h"
#include "OALPlayer.h"

CSoundMaterial COALPlayer::DefaultMaterial;
COALListener COALPlayer::DefaultListener;


COALPlayer::COALPlayer()
{
	CSoundSource::Player=this;
	COALListener::Player=this;
	ArraySize=256;
	PlayingSourcesArray=new CSoundSource*[ArraySize];
	Listener=&DefaultListener;
	SourcesCount=-1;
	memset(PlayingSourcesArray,0,sizeof(PlayingSourcesArray));

//	Device = alcOpenDevice((ALubyte*)"DirectSound3D");
	Device = alcOpenDevice(NULL);
	Context=alcCreateContext(Device,NULL);
	alcMakeContextCurrent(Context);

	EAXSet PSetEax;
	EAXGet PGetEax;

	if(alIsExtensionPresent((ALubyte *)"EAX2.0")==AL_TRUE)
	{
			PSetEax = (EAXSet)alGetProcAddress((ALubyte*)"EAXSet");
			PGetEax = (EAXGet)alGetProcAddress((ALubyte*)"EAXGet");
    }
	else
	{
		PSetEax=NULL;
		PGetEax=NULL;
	}
	CSoundSource::ProcGetEax=PGetEax;
	CSoundSource::ProcSetEax=PSetEax;
	COALListener::ProcGetEax=PGetEax;
	COALListener::ProcSetEax=PSetEax;
//----------------------------------
	Extensions=(char *)alGetString(AL_EXTENSIONS);
	Renderer=(char *)alGetString(AL_RENDERER);
	DeviceName=(char *)alcGetString(NULL,ALC_DEFAULT_DEVICE_SPECIFIER);
//----------------------------------
	SetListener(Listener);
}

COALPlayer::~COALPlayer()
{
	int i;
	int n;
	list<CSoundSource*>::iterator liter=SourcesList.end();
	liter--;

	n=SourcesList.size();
//	liter=SourcesList.begin();
	for(i=0;i<n;i++)
	{
		liter=SourcesList.begin();
		if(liter!=NULL)
		{
			delete *liter;
			SourcesList.erase(liter);
		}
	}

	map<DWORD,CSoundResource*>::iterator diter=WaveList.begin();
	n=WaveList.size();
//	diter=WaveList.begin();
	diter=WaveList.end();
	for(i=0;i<n;i++)
	{
//		diter=WaveList.begin();
		diter--;
		if(diter!=NULL)
			{
				CSoundResource* dr=diter->second;
				delete dr;
//				diter++;
//				diter--;
			}
	}
	WaveList.clear();

	delete PlayingSourcesArray;
	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);
}

CSoundSource *COALPlayer::CreateSource(CSoundResource *Sound)
{
	bool b=true;
//	map<DWORD,CSoundResource*>::const_iterator iter=WaveList.find((DWORD)Sound);
	CSoundResource *sr=WaveList[(DWORD)Sound];
	if(sr==NULL)
		return NULL;
	CSoundSource *src=new  CSoundSource(sr);
	SourcesList.push_back(src);
/*	if(!b)
	{
		delete src;
		return NULL;
	}*/
	src->iter=SourcesList.end();
	src->iter--;
	return src;
}
/*
void COALPlayer::LoadWaveTable(char *fn)
{
	char Name[30];
	char File[128];
	BinFile* file = F_OpenFile (fn,BF_OPEN_RO);
	if (!IsValid (file))
		return;

	DWORD size=file->size();
	char *fbuf=new char[size];
	file->read(fbuf,size);

	int i=0;
	int k=0;
	int j=0;
	while(i<size)
	{
		while((isalpha(fbuf[i])==0)&(i<size))
			i++;
		Name[0]=0;
		j=0;
		while(((isalpha(fbuf[i])!=0)|(isdigit(fbuf[i])!=0))&(i<size))
		{
			Name[j]=fbuf[i];
			Name[j+1]=0;
			i++;
			j++;
		}
		while((isalpha(fbuf[i])==0)&(i<size))
			i++;
		j=0;

		while(((isalpha(fbuf[i])!=0)|(isdigit(fbuf[i])!=0)|(fbuf[i]==':')|(fbuf[i]=='\\')|(fbuf[i]=='.'))&(i<size))
		{
			File[j]=fbuf[i];
			File[j+1]=0;
			i++;
			j++;
		}
		k++;
		while((isalpha(fbuf[i])==0)&(i<size))
			i++;
		RegisterWave(Name,File);
	}
	delete fbuf;
}
*/

bool COALPlayer::RegisterSound(CSoundResource *Sound)
{
	if(Sound==NULL)
		return false;
	ResPair p;
	p.first=(DWORD)Sound;
	p.second=Sound;
	WaveList.insert(p);
	return true;
}

void COALPlayer::UnregisterSound(CSoundResource *Sound)
{
	DWORD ID=(DWORD)Sound;
	map<DWORD,CSoundResource*>::iterator iter=WaveList.find(ID);
	list<CSoundSource*>::iterator iter1=SourcesList.begin();
	list<CSoundSource*>::iterator iter2;
	CSoundSource *src;
	int c=SourcesList.size();
	int i=0;
	int dp=0;
	if(iter->second!=NULL)
	{
		while(i<c)
		{
			//CSoundSource* si1=*iter1;
			if((*iter1)->WaveFile==iter->second)
			{
				src=(*iter1);
//-------------------------------------------------------------
				iter2=src->PList_iter;
				if(*iter2!=NULL)
					PlayList.erase(iter2);
//-------------------------------------------------------------
				SourcesList.erase(iter1);
				delete src;
				c=SourcesList.size();
				dp=i;
				i=0;
				iter1=SourcesList.begin();
			}
			else
			{
				iter1++;
				i++;
			}
		}
	WaveList.erase(iter);
	}
}

void COALPlayer::DeleteSource(CSoundSource *Source)
{
	list<CSoundSource*>::iterator iter1=Source->PList_iter;
	
	if(Source->iter!=NULL)
	{
		SourcesList.erase(Source->iter);
		Source->iter=list<CSoundSource*>::iterator::iterator();
		if(*iter1!=NULL)
		{
			PlayList.erase(iter1);
			Source->PList_iter=list<CSoundSource*>::iterator::iterator();
		}
		delete Source;
	}
}

void COALPlayer::Refresh()
{
	if(IsDirty)
	{
		RefreshArray();
		IsDirty=false;
	}
	RefreshData();
}

void COALPlayer::RefreshArray()
{
	CSoundSource *src;
	bool CanPlay=false;
	int Count=PlayList.size();
	list<CSoundSource*>::iterator iter=PlayList.begin();
	for(int i=0;i<Count;i++,iter++)
	{
		CanPlay=false;
		src=*iter;
		if(src->IsPlaying!=true)
		{
			if(src->Create())
			{
				alSourcePlay(src->Source);
				int err=alGetError();
				SourcesCount++;
				PlayingSourcesArray[SourcesCount]=src;
				src->IsPlaying=true;
			}
			else
			{
				qsort(PlayingSourcesArray,SourcesCount,sizeof(CSoundSource*),CompareSources);
				while(src->GetPriority()>PlayingSourcesArray[SourcesCount]->GetPriority())
				{
					PlayingSourcesArray[SourcesCount]->Destroy();
					memcpy(PlayingSourcesArray[SourcesCount],PlayingSourcesArray[SourcesCount+1],
						ArraySize-SourcesCount);
//					PlayingSourcesArray[SourcesCount]=NULL;
					SourcesCount--;
					if(src->Create())
					{
						alSourcePlay(src->Source);
						SourcesCount++;
						PlayingSourcesArray[SourcesCount]=src;
						src->IsPlaying=true;
						break;
					}
				}
			}
		}
	}
}

void COALPlayer::RefreshData()
{
	CSoundSource *src;
	DWORD DataLen;
	int err;
//	if(SourcesCount!=0)
	for(int i=0;i<=SourcesCount;i++)
	{
		src=PlayingSourcesArray[i];
		bool InPL=(src->PList_iter)==NULL ? false : true;
//		if((clock()>PlayingSourcesArray[i]->EndPlayingTime())|(!InPL))
		if((PlayingSourcesArray[i]->DataPresent())&&(InPL))
			PlayingSourcesArray[i]->NextData();
		else
		{
			PlayingSourcesArray[i]->IsPlaying=false;
			if(InPL)
			{
				PlayList.erase(PlayingSourcesArray[i]->PList_iter);
				PlayingSourcesArray[i]->PList_iter=NULL;
			}
			memcpy(&PlayingSourcesArray[i],&PlayingSourcesArray[i+1],(SourcesCount-i)*4);
			SourcesCount--;
		}
	}
}

void COALPlayer::SetListener(COALListener *l)
{
	if(l!=NULL)
	{
		Listener->Cur=false;
		Listener->SaveListener();
		Listener=l;
		Listener->Apply();
		Listener->Cur=true;
	}
}

int CompareSources( const void *arg1, const void *arg2 )
{
   void **s1=(void**)arg1;
   void **s2=(void**)arg2;
   CSoundSource *src1=(CSoundSource*)*s1;
   CSoundSource *src2=(CSoundSource*)*s2;
   float p1,p2;
/*   if((src1==NULL)&&(src2==NULL))
	   return 0;
   if((src1==NULL)&&(src2!=NULL))
	   return 1;
   if((src1!=NULL)&&(src2==NULL))
	   return -1;*/
   p1=src1->GetPriority();
   p2=src2->GetPriority();
   if(p1>p2)
	   return -1;
   if(p1<p2)
	   return 1;
   return 0;
}

CSoundResource *COALPlayer::CreateSound(char *file,bool Register)
{
	CSoundResource *res;
	int i=strlen(file)-1;
	if(file[i]=='v')
		res=new CWave(file);
	if(file[i]=='g')
		res=new COgg(file);
	if(Register)
		RegisterSound(res);
	return res;
}

void COALPlayer::DeleteSound(CSoundResource *res)
{
	UnregisterSound(res);
	delete res;
}
