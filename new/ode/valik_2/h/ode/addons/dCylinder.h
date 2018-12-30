#ifndef dCylinder_h
#define dCylinder_h

#include <ode\mass.h>

#ifdef __cplusplus
extern "C" {
#endif

struct dxCylinder;extern int dCylinderClassUser;

dxGeom *dCreateCylinder(dSpaceID space, dReal r, dReal lz);
void dGeomCylinderSetParams(dGeomID g, dReal radius, dReal length);
void dGeomCylinderGetParams(dGeomID g, dReal *radius, dReal *length);

#if defined(DEBUG) || defined(_DEBUG)
	#ifdef dDOUBLE
		#pragma comment(lib,"OdeAddonsDPd.lib")	
	#else
		#pragma comment(lib,"OdeAddonsSPd.lib")
	#endif
#else
	#ifdef dDOUBLE
		#pragma comment(lib,"OdeAddonsDP.lib")	
	#else
		#pragma comment(lib,"OdeAddonsSP.lib")
	#endif
#endif

#ifdef __cplusplus
}
#endif


#endif //dCylinder_h

