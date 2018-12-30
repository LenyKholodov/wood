#include "stdafx.h"
#include "SoundMaterial.h"

CSoundMaterial::CSoundMaterial()
{
	memset(this,0,sizeof(CSoundMaterial));
	ObstructionLFRatio=0.25;
	OcclusionRoomRatio=0.5;
	AirAbsorptionFactor=1;
	Flags=0x7;
}