#include "stdafx.h"
#include "OALPlayer.h"

CSoundMaterial COALPlayer::DefaultMaterial;
CListener COALPlayer::DefaultListener;


COALPlayer::COALPlayer()
{
	CSoundSource::Player=this;
	CListener::Player=this;
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
	CListener::ProcGetEax=PGetEax;
	CListener::ProcSetEax=PSetEax;
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
	List<CSoundSource*>::iterator liter=SourcesList;
	n=SourcesList.count();
	liter=SourcesList.GetFirst();
	for(i=0;i<n;i++,liter++)
		if(liter.valid())
			delete liter.data();

	Dict<CSoundResource*>::iterator diter=WaveList;
	n=WaveList.count();
	diter=WaveList.GetFirst();
	for(i=0;i<n;i++,diter++)
			if(diter.valid())
				delete diter.data();

	delete PlayingSourcesArray;
	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);
}

CSoundSource *COALPlayer::CreateSource(CSoundResource *Sound)
{
	bool b;
	Dict<CSoundResource*>::iterator iter=WaveList.find((DWORD)Sound);
	if(!iter.valid())
		return NULL;
	CSoundSource *src=new  CSoundSource(iter.data());
	b=SourcesList.InsertLast(src);
	if(!b)
	{
		delete src;
		return NULL;
	}
	src->iter=SourcesList.GetLast();
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
	return WaveList.insert((DWORD)Sound,Sound);
}

void COALPlayer::UnregisterSound(CSoundResource *Sound)
{
	DWORD ID=(DWORD)Sound;
	Dict<CSoundResource*>::iterator iter=WaveList.find(ID);
	List<CSoundSource*>::iterator iter1=SourcesList.GetFirst();
	Dict<CSoundSource*>::iterator iter2;
	CSoundSource *src;
	int c=SourcesList.count();
	int i=0;
	int dp=0;
	if(iter.valid())
	{
		while(i<c)
			if(iter1.data()->WaveFile==iter.data())
			{
				src=iter1.data();
//-------------------------------------------------------------
				iter2=PlayList.find((DWORD)iter1.data());
				if(iter2.valid())
					PlayList.erase(iter2);
//-------------------------------------------------------------
				SourcesList.erase(iter1);
				delete src;
				c=SourcesList.count();
				dp=i;
				i=0;
				iter1=SourcesList.GetFirst();
			}
			else
			{
				iter1++;
				i++;
			}
	WaveList.erase(iter);
	}
}

void COALPlayer::DeleteSource(CSoundSource *Source)
{
	Dict<CSoundSource*>::iterator iter1=PlayList.find((DWORD)Source);
	
	if(Source->iter.valid())
	{
		SourcesList.erase(Source->iter);
		Source->iter=List<CSoundSource*>::iterator::iterator();
		if(iter1.valid())
			PlayList.erase(iter1);
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
	int Count=PlayList.count();
	Dict<CSoundSource*>::iterator iter=PlayList.GetFirst();
	for(int i=0;i<Count;i++,iter++)
	{
		CanPlay=false;
		src=iter.data();
		if(src->IsPlaying!=true)
		{
			if(src->Create())
			{
				alSourcePlay(src->Source);
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
	for(int i=0;i<=SourcesCount;i++)
	{
		src=PlayingSourcesArray[i];
		bool InPL=PlayList.find((DWORD)src).valid();
//		if((clock()>PlayingSourcesArray[i]->EndPlayingTime())|(!InPL))
		if((PlayingSourcesArray[i]->DataPresent())&&(InPL))
			PlayingSourcesArray[i]->NextData();
		else
		{
			PlayingSourcesArray[i]->IsPlaying=false;
			PlayList.erase((DWORD)PlayingSourcesArray[i]);
			memcpy(&PlayingSourcesArray[i],&PlayingSourcesArray[i+1],(SourcesCount-i)*4);
			SourcesCount--;
		}
	}
}

void COALPlayer::SetListener(CListener *l)
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
