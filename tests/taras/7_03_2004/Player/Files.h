#ifndef __Files_h__
#define __Files_h__

#include <stdio.h>
#include <windows.h>
#include <fcntl.h>
#include <io.h>

//typedef FILE* SoundFile;
#define SoundFile int

void OpenFile(char *Name,SoundFile *f);
int Close(void *datasource);
size_t Read(void *ptr, size_t size, size_t nmemb, void *datasource);
int Seek(void *datasource, __int64 offset, int whence);
DWORD Size(SoundFile f);
bool IsValid(SoundFile f);
long Tell(void *datasource);

#endif