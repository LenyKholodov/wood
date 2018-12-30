// Player.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "SoundLib.h"
#include "Player.h"
#include <conio.h>
#include <iostream>

using std::cout;
using std::endl;

//class CPlayer;

int main(int argc, char* argv[])
{
//	K_FastInit(NULL);
	int *i1;
	int *i2;
	char *Name;
	CSource *res;
	CSource *res1;
//	COALPlayer *Player=new COALPlayer;
	CPlayer *Player=new CPlayer;

//	cout << "Device: " << Player->DeviceName << endl;
//	cout << "Extensions: " << Player->Extensions << endl;
//	cout << "Renderer: " << Player->Renderer << endl;

	CSoundResource *Wave=Player->CreateSound("stereo.wav",true);
	CSoundResource *Ogg=Player->CreateSound("1.ogg",true);

	res=Player->CreateSource(Ogg);
	res1=Player->CreateSource(Wave);
	res->Play(0);
	Sleep(100);

//	Player.DeleteSource(res);

//	Player.DeleteSource(res);
	res->Play(0);
	for(int i=0;i<4e4;i++)
	{
/*		if(i==3)
			res->Stop();
		if(i==10)
			res->Resume();*/
		Player->Refresh();
		cout << i << endl;
//		Sleep(1);
	}
//	Player.Play(25,0);
	int k=getch();
	delete Player;
	return -1;
}

