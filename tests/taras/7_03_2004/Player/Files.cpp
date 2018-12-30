#include "Files.h"

void OpenFile(char *Name,SoundFile *f)
{
	int file=open(Name,_O_BINARY );
	*f=file;
}

DWORD Size(SoundFile f)
{
	long pos;
	long s;
	s=0;
	char c;
	pos=tell(f);
	lseek(f,0,SEEK_END);
	s=tell(f);
	lseek(f,pos,SEEK_SET);
	return s;
}

bool IsValid(SoundFile f)
{
	if(f!=NULL)
		return true;
	return false;
}

size_t Read(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	int f=*((int*)datasource);
	return read(f,ptr,size*nmemb);;
}

int Seek(void *datasource, __int64 offset, int whence)
{
	int f=*((int*)datasource);
	return lseek(f,offset,whence);
//	return ferror((FILE*)datasource);
}

long Tell(void *datasource)
{
	int f=*((int*)datasource);
	return tell(f);
}

int Close(void *datasource)
{
	int f=*((int*)datasource);
	close(f);
	datasource=0;
	return 0;
}

