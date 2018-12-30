#ifndef __Base_h__
#define __Base_h__

#include <windows.h>
#include <map>
using namespace std;

struct MsgHeader
{
	DWORD Dest;
	DWORD Len;
	DWORD ID;
	DWORD From;
};

class CStream
{
private:
	virtual DWORD Read(void *Dest,DWORD Len);
	virtual DWORD Write(void *Dest,DWORD Len);
};
#endif