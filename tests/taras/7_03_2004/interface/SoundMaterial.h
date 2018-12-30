#ifndef __SoundMaterial_H__
#define __SoundMaterial_H__

#include <memory.h>

class  CSoundMaterial
{
public:
    long Direct;                // direct path level
    long DirectHF;              // direct path level at high frequencies
    long Room;                  // room effect level
    long RoomHF;                // room effect level at high frequencies
    float RoomRolloffFactor;   // like DS3D flRolloffFactor but for room effect
    long Obstruction;           // main obstruction control (attenuation at high frequencies) 
    float ObstructionLFRatio;  // obstruction low-frequency level re. main control
    long Occlusion;             // main occlusion control (attenuation at high frequencies)
    float OcclusionLFRatio;    // occlusion low-frequency level re. main control
    float OcclusionRoomRatio;  // occlusion room effect level re. main control
    long OutsideVolumeHF;       // outside sound cone level at high frequencies
    float AirAbsorptionFactor; // multiplies DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF
    unsigned long Flags;       // modifies the behavior of properties

	CSoundMaterial();
};
#endif