#ifndef __Listener_h__
#define __Listener_h__

#include "OALListener.h"

class Listener
{
  public:
        virtual void SetPos(float Pos[]) = 0;
        virtual void SetOrientation(float Ori[]) = 0;
        virtual void SetVelociti(float Vel[]) = 0;

        void SetGain(float g);
        void SetDoppler(float factor=1,float velocity=343.3);

        virtual float *GetPos() = 0;
        virtual float *GetOrientation() = 0;
        virtual float *GetVelociti() = 0;
        float GetGain();
        float GetDopplerFactor();
        float GetDopplerVelocity();

//        void SetEnvironment(unsigned long l);
        void SetEnvironment (StdEnv env);
//        void SetEnvironment (CListenerEnvironment *e);
        void SetEnvironment (const CListenerEnvironment&);
        CListenerEnvironment *GetEnvironment();

  protected:
        CListener();
};

#endif