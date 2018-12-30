#include "stdafx.h"
#include "Environment.h"

CListenerEnvironment::CListenerEnvironment()
{
    Room=-1000;             
    RoomHF=-100;           
    RoomRolloffFactor=0;
    DecayTime=1.49;       
    DecayHFRatio=0.83;    
    Reflections=-2602;     
    ReflectionsDelay=0.007;
    Reverb=200;        
    ReverbDelay=0.011;   
    Environment=0;   
    EnvironmentSize=7.5;    
    EnvironmentDiffusion=1;
    AirAbsorptionHF=-5;
    Flags=63;
}