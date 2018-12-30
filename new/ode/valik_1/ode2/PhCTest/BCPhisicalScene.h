#pragma once

#include "BCPhisComponentObject.h"

typedef std::map<UINT32,dSurfaceParameters> MaterialMap;

typedef std::vector<BCPhisComponentObject*> BCPhisComponentObjects;

class BCPhisMaterialCollisionCharacter
{
private:
	MaterialMap m_MaterialMap;
	dSurfaceParameters m_Default;
public:
	BCPhisMaterialCollisionCharacter();
	~BCPhisMaterialCollisionCharacter();

	static UINT32 GetSurfaceCode(int m1,int m2) 
		{  return m1>m2?GetSurfaceCode(m2,m1):(m1<<16)|(m2&0xFFFF); }
	const dSurfaceParameters* GetSurface(UINT32 code) const
		{  MaterialMap::const_iterator i=m_MaterialMap.find(code); return i==m_MaterialMap.end()?&m_Default:&(i->second); }
	const dSurfaceParameters* GetSurface(int m1,int m2) const 
		{ return GetSurface(GetSurfaceCode(m1,m2)); }
};

class BCPhisicalScene
{
private:
		dWorldID m_World;
		dSpaceID m_Space;
		dJointGroupID m_ContactGroup;

		BCPhisComponentObjects  m_Objects;
		dSpaceID		m_GeomSpace;
      		Geoms	 		m_Geoms;
		
		BCPhisMaterialCollisionCharacter m_MaterialCC;
		CollisionList		m_Collisions;	// список коллизий последнего шага

private:
		static BCScenePhisObject* 	GetObject(dGeomID o);
		static BCPhisComponentObject* 	GetObject(BCScenePhisObject* o);
		static int 			GetGeomMaterial(dGeomID o);
		static void nearCallback(void *data,dGeomID o1,dGeomID o2);
public:
		BCPhisicalScene();
		~BCPhisicalScene();

// "мировые" методы и характеристики
		dWorldID GetWorld()				{ return m_World; }
		dSpaceID GetSpace()				{ return m_Space; }
		void SetGravity(float x,float y,float z);
		void GetGravity(dVector3 g);
		
		void SetERP(float erp);
		float GetERP();
		void SetCFM(float cfm);
		float GetCFM();

		void WorldStep(float stepsize);
// доступ к объектам
		BCPhisComponentObjects& GetObjects() 	{ return m_Objects; }

		Geoms& GetGeoms()	{ return m_Geoms; }
		void AddGeom(dGeomID g)	{ dSpaceAdd(m_GeomSpace,g); m_Geoms.push_back(g); }
		void Draw();
};

