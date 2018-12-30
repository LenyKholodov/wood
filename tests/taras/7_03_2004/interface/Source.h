#ifndef __Source_h__
#define __Source_h__

#include "SoundSource.h"

/////////////////////////////////////////////////////////////////////////////////////////////
///Источник звука
/////////////////////////////////////////////////////////////////////////////////////////////
class Source
{
  public:
        float GetPriority();
        float GetTime();
        bool DataPresent();

//////////////////
    virtual void SetPos           (float []);
    virtual void SetOrientation   (float []); 
    virtual void SetVelociti      (float []);

    void SetInConeAngle   (float a);
    void SetOutConeAngle  (float a);
    void SetMaxDist       (float d);
    void SetOutConeGlain  (float g);
    void SetReferenceDist (float d);
    void SetRolloffFactor (float f);
    void SetGain          (float g);

    virtual void GetPos         (float*) = 0; //!!!
    virtual void GetOrientation (float*);
    virtual void GetVelociti    (float*);

    virtual float GetInConeAngle();
    virtual float GetOutConeAngle();
    virtual float GetMaxDist();
    virtual float GetOutConeGlain();
    virtual float GetReferenceDist();
    virtual float GetRolloffFactor();
    virtual float GetGain();

////////////////
    void SetMaterial(CSoundMaterial *mat);
    CSoundMaterial *GetMaterial();

////////////////
    virtual void Play   (DWORD Pos) = 0;

    virtual void Pause  () = 0;
    virtual void Resume () = 0;

    virtual void Stop   () = 0;

  protected:
//    Source (const SourceParams&);
};

#endif