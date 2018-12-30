// OpenALPlayer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>

#include "OALPlayer.h"
#include <conio.h>
#include <iostream>

int main(int argc, char* argv[])
{
//	K_FastInit(NULL);
	int *i1;
	int *i2;
	char *Name;
	CSoundSource *res;
	COALPlayer Player;
	CWave *Wave=new CWave("stereo.wav");
	bool br=Player.RegisterSound(Wave);
//	Player.UnregisterSound("foot");

	res=Player.CreateSource(Wave);
	res->Play(0);
	Sleep(100);

//	Player.DeleteSource(res);

//	Player.DeleteSource(res);
//	res->Play(0);
	for(int i=0;i<40;i++)
	{
		Player.Refresh();
		//Player.Start();
		if(i==2)
			res->Stop();
		if(i==10)
			res->Resume();
		Sleep(170);
	}
//	Player.Play(25,0);
	cout << "Device: " << Player.DeviceName << endl;
	cout << "Extensions: " << Player.Extensions << endl;
	cout << "Renderer: " << Player.Renderer << endl;
	getch();
	return 0;
}

