#ifndef __Environment_H__
#define __Environment_H__

#include <memory.h>

enum
{
    SOUND_ENVIRONMENT_GENERIC,
    SOUND_ENVIRONMENT_PADDEDCELL,
    SOUND_ENVIRONMENT_ROOM,
    SOUND_ENVIRONMENT_BATHROOM,
    SOUND_ENVIRONMENT_LIVINGROOM,
    SOUND_ENVIRONMENT_STONEROOM,
    SOUND_ENVIRONMENT_AUDITORIUM,
    SOUND_ENVIRONMENT_CONCERTHALL,
    SOUND_ENVIRONMENT_CAVE,
    SOUND_ENVIRONMENT_ARENA,
    SOUND_ENVIRONMENT_HANGAR,
    SOUND_ENVIRONMENT_CARPETEDHALLWAY,
    SOUND_ENVIRONMENT_HALLWAY,
    SOUND_ENVIRONMENT_STONECORRIDOR,
    SOUND_ENVIRONMENT_ALLEY,
    SOUND_ENVIRONMENT_FOREST,
    SOUND_ENVIRONMENT_CITY,
    SOUND_ENVIRONMENT_MOUNTAINS,
    SOUND_ENVIRONMENT_QUARRY,
    SOUND_ENVIRONMENT_PLAIN,
    SOUND_ENVIRONMENT_PARKINGLOT,
    SOUND_ENVIRONMENT_SEWERPIPE,
    SOUND_ENVIRONMENT_UNDERWATER,
    SOUND_ENVIRONMENT_DRUGGED,
    SOUND_ENVIRONMENT_DIZZY,
    SOUND_ENVIRONMENT_PSYCHOTIC,
};

class CListenerEnvironment
{
public:
    long Room;                    // room effect level at low frequencies
    long RoomHF;                  // room effect high-frequency level re. low frequency level
    float RoomRolloffFactor;     // like DS3D flRolloffFactor but for room effect
    float DecayTime;             // reverberation decay time at low frequencies
    float DecayHFRatio;          // high-frequency to low-frequency decay time ratio
    long Reflections;             // early reflections level relative to room effect
    float ReflectionsDelay;      // initial reflection delay time
    long Reverb;                  // late reverberation level relative to room effect
    float ReverbDelay;           // late reverberation delay time relative to initial reflection
    unsigned long Environment;   // sets all listener properties
    float EnvironmentSize;       // environment size in meters
    float EnvironmentDiffusion;  // environment diffusion
    float AirAbsorptionHF;       // change in level per meter at 5 kHz
    unsigned long Flags;         // modifies the behavior of properties
	CListenerEnvironment();
};
#endif