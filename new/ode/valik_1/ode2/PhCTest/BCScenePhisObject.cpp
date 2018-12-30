#include "BCPhisicalScene.h"

BCScenePhisObject::BCScenePhisObject(BCPhisComponentObject* cobj):
		m_Body(NULL),
		m_PCOwner(cobj),
		m_PSOwner(NULL),
		m_UserData(NULL),
		m_MaterialID(0)
{
	dMassSetZero(&m_Mass);
	// создаем тело
	m_Body=dBodyCreate(GetWorld());
	dBodySetData(m_Body,&m_bbi);
	m_bbi.pso=this;
	m_PCOwner->GetObjects().push_back(this);
}

BCScenePhisObject::BCScenePhisObject(BCPhisicalScene* sobj):
		m_Body(NULL),
		m_PCOwner(NULL),
		m_PSOwner(sobj),
		m_UserData(NULL),
		m_MaterialID(0)
{
	dMassSetZero(&m_Mass);
	// создаем тело
	m_Body=dBodyCreate(GetWorld());
	dBodySetData(m_Body,&m_bbi);
	m_bbi.pso=this;
}

BCScenePhisObject::~BCScenePhisObject()
{
	for(int i=m_Geoms.size();i--;dGeomDestroy(m_Geoms[i]));
	m_Geoms.clear();

	if(m_Body)
	{
		dBodyDestroy(m_Body);
		m_Body=NULL;
	}
	
	dMassSetZero(&m_Mass);	
	m_PCOwner=NULL;
	m_PSOwner=NULL;
}

dWorldID BCScenePhisObject::GetWorld()
{
	BCPhisicalScene* sc=GetScene();
	return sc?sc->GetWorld():m_PCOwner->GetWorld();
}


dSpaceID BCScenePhisObject::GetSpace()
{
	if(m_PCOwner)
		return m_PCOwner->GetSpace();
	else
		if(m_PSOwner)
			return m_PSOwner->GetSpace();
	return NULL;
}

BCPhisicalScene* BCScenePhisObject::GetScene()
{
	return m_PCOwner?m_PCOwner->GetScene():m_PSOwner;
}

bool BCScenePhisObject::SetRotation(const dMatrix4 R)
{
	if(!m_Body)
		return false;
	dBodySetRotation(m_Body,R);
	return true;	
}

bool BCScenePhisObject::GetRotation(dMatrix4 R)	// все матрицы и вектора имеют 4(!) компоненты в строке
{
	if(!m_Body)
		return false;
	memcpy(R,dBodyGetRotation(m_Body),sizeof(dMatrix3));
	return true;	
}

bool BCScenePhisObject::SetPosition(float x,float y,float z)
{
	if(!m_Body)
		return false;
	dBodySetPosition(m_Body,x,y,z);		
	return true;
}

bool BCScenePhisObject::GetPosition(dVector3 p)
{
	if(!m_Body)
		return false;
	memcpy(p,dBodyGetPosition(m_Body),sizeof(dVector3));
	return true;	
}
		
// скоростя: линейныя и угловыя
bool BCScenePhisObject::SetLinearVel(float x, float y, float z)
{
	if(!m_Body)
		return false;
	dBodySetLinearVel(m_Body,x,y,z);
	return true;
}

bool BCScenePhisObject::GetLinearVel(dVector3 v)
{
	if(!m_Body)
		return false;
	memcpy(v,dBodyGetLinearVel(m_Body),sizeof(dVector3));
	return true;
}

bool BCScenePhisObject::SetAngularVel(float x, float y, float z)
{
	if(!m_Body)
		return false;
	dBodySetAngularVel(m_Body,x,y,z);
	return true;
}

bool BCScenePhisObject::GetAngularVel(dVector3 v)
{
	if(!m_Body)
		return false;
	memcpy(v,dBodyGetAngularVel(m_Body),sizeof(dVector3));
	return true;
}

// Силы и вращающие моменты. З.Ы.: Силы сбрасываются после итерации обработки
// З.З.Ы Приставка (Rel) означает, что координаты даются относительно с.к. тела

bool BCScenePhisObject::SetForce(float fx,float fy,float fz)
{
	if(!m_Body)
		return false;
	dBodySetForce(m_Body,fx,fy,fz);
	return true;
}

bool BCScenePhisObject::GetForce(dVector3 f)
{
	if(!m_Body)
		return false;
	memcpy(f,dBodyGetForce(m_Body),sizeof(dVector3));
	return true;
}

bool BCScenePhisObject::AddForce(float fx, float fy, float fz,bool Relation)
{
	if(!m_Body)
		return false;
	if(Relation)
		dBodyAddRelForce(m_Body,fx,fy,fz);
	else
		dBodyAddForce(m_Body,fx,fy,fz);
	return true;
}

bool BCScenePhisObject::AddForceAtPos(float fx,float fy, float fz,float px,float py, float pz,bool RelForce,bool RelPosition)
{
	if(!m_Body)
		return false;
	if(RelForce)
		if(RelPosition)
			dBodyAddRelForceAtRelPos(m_Body,fx,fy,fz,px,py,pz);
		else
			dBodyAddRelForceAtPos(m_Body,fx,fy,fz,px,py,pz);
	else
		if(RelPosition)
			dBodyAddForceAtRelPos(m_Body,fx,fy,fz,px,py,pz);
		else
			dBodyAddForceAtPos(m_Body,fx,fy,fz,px,py,pz);
	return true;
}

bool BCScenePhisObject::SetTorque(float tx,float ty,float tz)
{
	if(!m_Body)
		return false;
	dBodySetTorque(m_Body,tx,ty,tz);
	return true;
}

bool BCScenePhisObject::GetTorque(dVector3 t)
{
	if(!m_Body)
		return false;
	memcpy(t,dBodyGetTorque(m_Body),sizeof(dVector3));
	return true;
}

bool BCScenePhisObject::AddTorque(float tx, float ty, float tz,bool Relation)
{
	if(!m_Body)
		return false;
	if(Relation)
		dBodyAddRelTorque(m_Body,tx,ty,tz);
	else
		dBodyAddTorque(m_Body,tx,ty,tz);
	return true;
}

// модификация объекта - массы
bool BCScenePhisObject::SetMass(const dMass* mass)
{
	if(!m_Body)
		return false;
	dBodySetMass(m_Body,mass);
	return true;
}

bool BCScenePhisObject::SetMass(float m)
{
	if(!m_Body)
		return false;
	dMass mass;
	dBodyGetMass(m_Body,&mass);
	dMassAdjust(&mass,m);
	dBodySetMass(m_Body,&mass);
	return true;
}

bool BCScenePhisObject::GetMass(dMass* mass)
{
	if(!m_Body)
		return false;
	dBodyGetMass(m_Body,mass);	
	return true;
}

float BCScenePhisObject::GetMass()
{
	if(!m_Body)
		return 0.0;
	dMass mass;
	dBodyGetMass(m_Body,&mass);
	return mass.mass;
}

// блокировка тела (выброс из расчета физики)
bool BCScenePhisObject::BodyEnable()
{
	if(!m_Body)
		return false;
	dBodyEnable(m_Body);
	return true;
}

bool BCScenePhisObject::BodyDisable()
{
	if(!m_Body)
		return false;
	dBodyDisable(m_Body);
	return true;
}

bool BCScenePhisObject::IsEnabled() const
{
	return m_Body && dBodyIsEnabled(m_Body);	
}

// управление точностью расчета
bool BCScenePhisObject::SetFiniteRotationMode(int mode)
{
	if(!m_Body)
		return false;
	dBodySetFiniteRotationMode(m_Body,mode);
	return true;
}

bool BCScenePhisObject::SetFiniteRotationAxis(float x, float y, float z)
{
	if(!m_Body)
		return false;
	dBodySetFiniteRotationAxis(m_Body,x,y,z);
	return true;
}

int BCScenePhisObject::GetFiniteRotationMode() const
{
	if(!m_Body)
		return -1;
	return dBodyGetFiniteRotationMode(m_Body);
}

bool BCScenePhisObject::GetFiniteRotationAxis(dVector3 result)
{
	if(!m_Body)
		return false;
	dBodyGetFiniteRotationAxis(m_Body,result);
	return true;
}

int  BCScenePhisObject::GetNumJoints()
{
	return m_Body?dBodyGetNumJoints(m_Body):0;
}

dJointID BCScenePhisObject::GetJoint(int index)
{
	return m_Body?dBodyGetJoint(m_Body,index):NULL;
}

bool BCScenePhisObject::IsConnectedTo(BCScenePhisObject& obj)
{
	return dAreConnected(m_Body,obj.m_Body)?true:false;
}


#include <d:\studio\src\ode\ode\src\objects.h>

// гравитация

bool BCScenePhisObject::SetGravityMode(GravityMode mode)
{
	// примечание!!! функция dBodySetGravityMode работает несовсем так, как ожидается
	// из-за изменений (кто и нафиг - ?) отмена флагов не работает!! (т.е. отмены откл. гравитации нет)
	// посему вписан следующий кусок кода...
	if(!m_Body)
		return false;
	switch(mode)
	{
		case NoGravity:			
			m_Body->flags|=dxBodyNoGravity; 
			break; 
		case WorldGravity:		
			m_Body->flags&=0xFFFFFFFF^(dxBodyNoGravity|dxBodyPrivateGravity); 
			break;
		case PrivateGravity:	
			m_Body->flags|=dxBodyPrivateGravity;
			if(m_Body->flags&dxBodyNoGravity)
				m_Body->flags^=dxBodyNoGravity;
			break;			
		default:
			return false;
	}
	return true;
}

GravityMode BCScenePhisObject::GetGravityMode()
{
	if(!m_Body || m_Body->flags&dxBodyNoGravity)
		return NoGravity;
	return m_Body->flags&dxBodyPrivateGravity?PrivateGravity:WorldGravity;
}

bool BCScenePhisObject::SetGravity(float x,float y,float z)
{
	if(!m_Body)
		return false;
	m_bbi.gravity_x=x;
	m_bbi.gravity_y=y;
	m_bbi.gravity_z=z;
	return true;
}

bool BCScenePhisObject::GetGravity(dVector3 g)
{
	if(!m_Body)
		return false;
	g[0]=m_bbi.gravity_x;
	g[1]=m_bbi.gravity_y;
	g[2]=m_bbi.gravity_z;
	return true;
}

//--- 	методы доступа к геометрии
//  добавить геометрический примитив, получить примитив
//  вставкой из GetGeom для добавления объекта пользоватся не рекомендуется!
// примечание: при "ручном" указании примитива он не должен принадлежать какому-либо пространству
bool BCScenePhisObject::AddGeom(dGeomID geom,bool Relative)
{
	if(!geom)
		return false;
	if(Relative)
	{
		dGeomID t=dCreateGeomTransform(GetSpace());
		dGeomTransformSetGeom(t,geom);
		dGeomTransformSetCleanup(t,1);
		dGeomTransformSetInfo(t,1);
		geom=t;
	}
	else
		dSpaceAdd(GetSpace(),geom);
	dGeomSetBody(geom,m_Body);
	m_Geoms.push_back(geom);
	return true;
}

bool BCScenePhisObject::AddGSphere(float r)
{
	return AddGeom(dCreateSphere(0,r));
}

bool BCScenePhisObject::AddGCappedCylinder(float r,float l)
{
	return AddGeom(dCreateCCylinder(0,r,l));
}

bool BCScenePhisObject::AddGCylinder(float r,float l)
{
	return AddGeom(dCreateCylinder(0,r,l));
}

bool BCScenePhisObject::AddGBox(float lx,float ly,float lz)
{
	return AddGeom(dCreateBox(0,lx,ly,lz));
}

bool BCScenePhisObject::AddGSphere(float r,float x,float y,float z,dMatrix4 R)
{
	dGeomID g=dCreateSphere(0,r);
	if(!g)
		return false;
	dGeomSetPosition(g,x,y,z);
	dGeomSetRotation(g,R);
	return AddGeom(g,true);
}
bool BCScenePhisObject::AddGCappedCylinder(float r,float l,float x,float y,float z,dMatrix4 R)
{
	dGeomID g=dCreateCCylinder(0,r,l);
	if(!g)
		return false;
	dGeomSetPosition(g,x,y,z);
	dGeomSetRotation(g,R);
	return AddGeom(g,true);
}

bool BCScenePhisObject::AddGCylinder(float r,float l,float x,float y,float z,dMatrix4 R)
{
	dGeomID g=dCreateCylinder(0,r,l);
	if(!g)
		return false;
	dGeomSetPosition(g,x,y,z);
	dGeomSetRotation(g,R);
	return AddGeom(g,true);
}

bool BCScenePhisObject::AddGBox(float lx,float ly,float lz,float x,float y,float z,dMatrix4 R)
{
	dGeomID g=dCreateBox(0,lx,ly,lz);
	if(!g)
		return false;
	dGeomSetPosition(g,x,y,z);
	dGeomSetRotation(g,R);
	return AddGeom(g,true);
}


bool BCScenePhisObject::GetDrawData(int index,DrawGeomInfo* dgi)
{
	if(index<0 || index>=m_Geoms.size())
		return false;
	dGeomID geom=m_Geoms[index];
	int gc=dGeomGetClass(geom);
	const dReal* R=dGeomGetRotation(geom);
	const dReal* pos=dGeomGetPosition(geom);
	if(gc==dGeomTransformClass)
	{
		dGeomID obj=dGeomTransformGetGeom(geom);
		dMULTIPLY0_331(dgi->mtx+12,R,dGeomGetPosition(obj));
		vector_add3(dgi->mtx+12,pos);		
		dMULTIPLY0_333 (dgi->mtx,R,dGeomGetRotation(obj));
		gc=dGeomGetClass(obj);
		geom=obj;
	}
	else
	{
		memcpy(dgi->mtx,R,sizeof(dMatrix3));
		memcpy(dgi->mtx+12,pos,sizeof(dVector3));
	}
	switch(gc)
	{
		case dSphereClass:
			dgi->gtype=gSphere;
			dgi->radius=dGeomSphereGetRadius(geom);
			return true;
		case dBoxClass:
			dgi->gtype=gBox;
			dGeomBoxGetLengths(geom,dgi->d);
			return true;
		case dCCylinderClass:
			dgi->gtype=gCappedCylinder;
			dGeomCCylinderGetParams(geom,&dgi->radius,&dgi->length);
			return true;
	}
	if(gc==duCylinderClass)
	{
		dgi->gtype=gCylinder;
		dGeomCylinderGetParams(geom,&dgi->radius,&dgi->length);
		return true;
	}
	return false;
}

void BCScenePhisObject::Draw()
{
	for(int i=m_Geoms.size();i--;)
	{
		DrawGeomInfo dgi;
		GetDrawData(i,&dgi);
		switch(dgi.gtype)
		{
			case gSphere: 		dsDrawSphere(dgi.mtx+12,dgi.mtx,dgi.radius); break;
			case gBox:    		dsDrawBox(dgi.mtx+12,dgi.mtx,dgi.d); break;
			case gCylinder:		dsDrawCylinder(dgi.mtx+12,dgi.mtx,dgi.length,dgi.radius); break;
			case gCappedCylinder: 	dsDrawCappedCylinder(dgi.mtx+12,dgi.mtx,dgi.length,dgi.radius); break;
		}
	}
}

