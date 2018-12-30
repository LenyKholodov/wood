#pragma once


#include <ode\ode.h>
#include <ode\addons\misc.h>
#include <ode\addons\advShapes.h>
#include <drawstuff/drawstuff.h>
#include <vector>
#include <list>
#include <map>

class BCScenePhisObject;
class BCPhisComponentObject;
class BCPhisicalScene;

class BodyBackInfo:public dBodyPrivateData
{
public:
	BCScenePhisObject* pso; // phisic simple object
};

enum BCDrawGeomType
{
	gNull=0,
	gSphere,
	gBox,
	gCylinder,
	gCappedCylinder,
	gDRTCount	
};

typedef struct
{
	dMatrix4	mtx;
	BCDrawGeomType  gtype;
	union
	{
		struct { float radius,length; };
		struct { float lx,ly,lz; };
		float d[3];
	};
} DrawGeomInfo;

typedef std::vector<dGeomID> Geoms;
typedef std::vector<BCScenePhisObject*> BCScenePhisObjects;
typedef std::vector<BCPhisicalScene*> BCPhisicalScenes;


typedef std::vector<dJointID> dJoints;

// описание коллизии
struct BCCollisionData
{
	void*			obj1;
	void*			obj2; // —сылка на игровые данные столкнувшихс€ объектов
	int			MaterialID1,
	      			MaterialID2;
	dVector3 		position,
				normal;
	float			depth;
};

typedef std::list<BCCollisionData>	CollisionList;
typedef std::vector<dSpaceID>		Spaces;

#ifndef UINT32

	typedef unsigned long UINT32;
#endif