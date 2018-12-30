/* ************************************************************************ */
/* 
   grouped and transformed geometry functions 
   author: Tim Schmidt tisch@uni-paderborn.de
*/

#ifndef __TRANSFORM_GROUP_H__
#define __TRANSFORM_GROUP_H__

#include <ode\ode.h>

#ifdef __cplusplus
extern "C" {
#endif


extern int dGeomTransformGroupClass;

dGeomID dCreateGeomTransformGroup (dSpaceID space);
void dGeomTransformGroupAddGeom    (dGeomID tg, dGeomID obj,bool rootZero=false);
void dGeomTransformGroupRemoveGeom (dGeomID tg, dGeomID obj);
dGeomID dGeomTransformGroupGetGeom (dGeomID tg, int i);
int dGeomTransformGroupGetNumGeoms (dGeomID tg);

void dGeomTransformGroupSetCleanup(dGeomID tg,bool mode);
bool dGeomTransformGroupGetCleanup(dGeomID tg);

void dGeomTransformGroupGetGeomPosition(dGeomID g,int index,dReal* pos,dReal* R);

void dGeomTransformGroupGetRootPosition(dGeomID g,dReal* pos,dReal* R);
void dGeomTransformGroupSetRootPosition(dGeomID g,const dReal* pos,const dReal* R);

void dGeomTransformGroupGetLocalRootPosition(dGeomID g,dReal* pos,dReal* R);
void dGeomTransformGroupSetLocalRootPosition(dGeomID g,const dReal* pos,const dReal* R);

#ifdef __cplusplus
}
#endif

#endif