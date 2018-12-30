#ifndef __DRAWSTUFF_ADDONS_H__
#define __DRAWSTUFF_ADDONS_H__

#include <drawstuff\drawstuff.h>
#include <ode\ode.h>

#ifdef dDOUBLE
	#define dsDrawBox				dsDrawBoxD
	#define dsDrawSphere			dsDrawSphereD
	#define dsDrawCylinder			dsDrawCylinderD
	#define dsDrawCappedCylinder	dsDrawCappedCylinderD
#endif

void dsDrawGeom(dGeomID geom,const dReal* pos=NULL,const dReal* R=NULL);
void dsDrawBody(dBodyID body);

#if defined(DEBUG) || defined(_DEBUG)
	#pragma comment(lib,"drawstuffD.lib")
#else
	#pragma comment(lib,"drawstuff.lib")
#endif

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"Glu32.lib")

#endif