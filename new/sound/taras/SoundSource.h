#ifndef __SoundSource_H__
#define __SoundSource_H__

#include "WaveResource.h"
#include "OALPlayer.h"
#include "SoundMaterial.h"
#include <alc.h>
#include <eax.h>

class COALPlayer;

class CSoundSource
{
friend COALPlayer;
friend class CListener;
protected:
        BOOL Created;
        ALuint  Buffer[MAX_BUFFERS];
        ALuint  Source;
        BYTE CurBuf;
        List<CSoundSource*>::iterator iter; //source list
        List<CSoundSource*>::iterator iter; //play list

        CSoundSource(CSoundResource *wf);
        ~CSoundSource();
public:
        void Play(DWORD Pos);
        void Stop();
        void Resume();
protected:
        ALfloat Position[3];
        ALfloat Orientation[3];
        ALfloat Velociti[3];
        ALfloat InConeAngle;
        ALfloat OutConeAngle;
        ALfloat MaxDistance;
        ALfloat OutConeGlain;
        ALfloat ReferenceDist;
        ALfloat RolloffFactor;
        ALfloat Gain;
        ALfloat Pr;
        clock_t BeginPlayingTime;
        clock_t EndPlayingTime();
        DWORD BeginPlayingPos;

//      void SaveSourse();
        void LoadSourse();

        CSoundResource *WaveFile;
        CSoundMaterial *Material;
        static EAXSet ProcSetEax;
        static EAXGet ProcGetEax;
//      bool Create(DWORD Pos);
        bool Create();
        void Destroy();
        void NextData();
        static COALPlayer *Player;
        bool IsPlaying;
public:
        float GetPriority();
        float GetTime(){return WaveFile->GetTime();};
        bool DataPresent();

        void SetPos(float Pos[]);
        void SetOrientation(float Ori[]);
        void SetVelociti(float Vel[]);
        void SetInConeAngle(float a);
        void SetOutConeAngle(float a);
        void SetMaxDist(float d);
        void SetOutConeGlain(float g);
        void SetReferenceDist(float d);
        void SetRolloffFactor(float f);
        void SetGain(float g);
        void SetMaterial(CSoundMaterial *mat);
};

#endif