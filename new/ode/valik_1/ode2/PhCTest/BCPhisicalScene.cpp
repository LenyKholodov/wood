#include "BCPhisicalScene.h"

#define CONTACT_POINTS_COUNT	10

BCScenePhisObject* BCPhisicalScene::GetObject(dGeomID o)
{
	dBodyID body=dGeomGetBody(o);
	return body?((BodyBackInfo*)dBodyGetData(body))->pso:NULL;
}

BCPhisComponentObject* BCPhisicalScene::GetObject(BCScenePhisObject* o)
{
	return o?o->GetComponentObject():NULL;
}

int BCPhisicalScene::GetGeomMaterial(dGeomID o)
{
	BCScenePhisObject* spo=GetObject(o);
	return spo?spo->GetMaterialID():(int)dGeomGetData(o);
}

void BCPhisicalScene::nearCallback(void *data, dGeomID o1, dGeomID o2)
{
	BCPhisicalScene* scene=(BCPhisicalScene*)data;
	// столкновения статических объектов не выполняем
	if(dGeomGetBody(o1)==NULL && dGeomGetBody(o2)==NULL)
		return;
	// столкновения одного компонентного объекта не выполняем
	BCScenePhisObject* po1=GetObject(o1);
	BCScenePhisObject* po2=GetObject(o2);
	if(GetObject(po1)==GetObject(po2) && GetObject(po1)!=NULL)
		return;
	// все в порядке, можно перепихнутся
	dContact contact[CONTACT_POINTS_COUNT];
	int n=dCollide(o1,o2,CONTACT_POINTS_COUNT,&contact[0].geom,sizeof(dContact));
    for(int i=0;i<n;i++) 
	{
		int m1=GetGeomMaterial(o1);
		int m2=GetGeomMaterial(o2);
		memcpy(&contact[i].surface,scene->m_MaterialCC.GetSurface(m1,m2),sizeof(dSurfaceParameters));
		dJointID c=dJointCreateContact(scene->m_World,scene->m_ContactGroup,&contact[i]);
      		dJointAttach (c,dGeomGetBody(contact[i].geom.g1),dGeomGetBody(contact[i].geom.g2));
		// добавить найденую толкучку в список перепихнушек
		BCCollisionData cdata;
		cdata.obj1=po1?po1->GetUserData():NULL;	
		cdata.obj2=po2?po2->GetUserData():NULL;
		cdata.MaterialID1=m1;
		cdata.MaterialID2=m2;
		memcpy(cdata.position,contact[i].geom.pos,sizeof(dVector3));
		memcpy(cdata.normal,contact[i].geom.normal,sizeof(dVector3));
		cdata.depth=contact[i].geom.depth;
		scene->m_Collisions.push_back(cdata);
	}

}

BCPhisicalScene::BCPhisicalScene()
{
	m_World=dWorldCreate();
	m_Space=dSimpleSpaceCreate(0);
	m_ContactGroup=dJointGroupCreate(0);
	m_GeomSpace=dSimpleSpaceCreate(m_Space);
}

BCPhisicalScene::~BCPhisicalScene()
{
	if(m_ContactGroup)
	{
		dJointGroupDestroy(m_ContactGroup);
		m_ContactGroup=NULL;
	}
	for(int i=0;i<m_Objects.size();i++)
		delete m_Objects[i];
	if(m_GeomSpace)
	{
		dSpaceDestroy(m_GeomSpace);
		m_GeomSpace=NULL;
	}
	if(m_Space)
	{
		dSpaceDestroy(m_Space);
		m_Space=NULL;
	}
	if(m_World)
	{
		dWorldDestroy(m_World);
		m_World=NULL;
	}
}

void BCPhisicalScene::SetGravity(float x,float y,float z)
{
	if(m_World)
		dWorldSetGravity(m_World,x,y,z);
}

void BCPhisicalScene::GetGravity(dVector3 g)
{
	if(m_World)
		dWorldGetGravity(m_World,g);
}
		
void BCPhisicalScene::SetERP(float erp)
{
	if(m_World)
		dWorldSetERP(m_World,erp);
}

float BCPhisicalScene::GetERP()
{
	return m_World?dWorldGetERP(m_World):0.0;
}

void BCPhisicalScene::SetCFM(float cfm)
{
	if(m_World)
		dWorldSetCFM(m_World,cfm);
}

float BCPhisicalScene::GetCFM()
{
	return m_World?dWorldGetCFM(m_World):0.0;
}


void BCPhisicalScene::WorldStep(float stepsize)
{       // Подготовка: зачистка контактных точек и списка коллизий
	m_Collisions.clear();
	// Просчет геометрии
//	dSpaceCollide(GetSpace(),this,nearCallback);
	for(int i=0;i<m_Objects.size();i++)
	{
		dSpaceCollide2((dGeomID)m_Objects[i]->GetSpace(),(dGeomID)m_GeomSpace,this,nearCallback);
		for(int j=i+1;j<m_Objects.size();j++)
			dSpaceCollide2((dGeomID)m_Objects[i]->GetSpace(),(dGeomID)m_Objects[j]->GetSpace(),this,nearCallback);
	}
	// Просчет физики
	if(m_World)
    		dWorldStep(m_World,stepsize);
	if(m_ContactGroup)
    		dJointGroupEmpty(m_ContactGroup);
}


void BCPhisicalScene::Draw()
{
	for(int i=m_Geoms.size();i--;)
	{
		dGeomID g=m_Geoms[i];
		if(dGeomGetClass(g)==duCylinderClass)
		{
			dReal r,l;
			dGeomCylinderGetParams(g,&r,&l);
			dsDrawCylinder(dGeomGetPosition(g),dGeomGetRotation(g),l,r);		
		}
		else
		if(dGeomGetClass(g)==dBoxClass)
		{
			dVector3 ss;
  			dGeomBoxGetLengths(g,ss);
			dsDrawBox(dGeomGetPosition(g),dGeomGetRotation(g),ss);
		}
		else
		if(dGeomGetClass(g)==dCCylinderClass)
		{
			dReal r,l;
			dGeomCCylinderGetParams(g,&r,&l);
			dsDrawCappedCylinder(dGeomGetPosition(g),dGeomGetRotation(g),l,r);
		}
		else
		if(dGeomGetClass(g)==dSphereClass)
			dsDrawSphere(dGeomGetPosition(g),dGeomGetRotation(g),dGeomSphereGetRadius(g));
		else
		if(dGeomGetClass(g)==dRayClass)
		{
			dVector3 a,b;
			dGeomRayGet(g,a,b);
			vector_add3(b,a);
			dsDrawLine(a,b);
		}
		else
		if(dGeomGetClass(g)==duFaceClass)
		{
			const dVector3* p=(const dVector3*)dGeomGetClassData(g);
			float pf[3][4];
			for(int i=0;i<3;i++)
			{
				pf[i][0]=p[i][0];
				pf[i][1]=p[i][1];
				pf[i][2]=p[i][2];
				pf[i][3]=1.0;
			}
			dsDrawTriangle(dGeomGetPosition(g),dGeomGetRotation(g),pf[0],pf[1],pf[2]);
		}
		else
		if(dGeomGetClass(g)==duEdgeClass)
		{
			dVector3 p0,p1,p;
			dGeomEdgeGetParams(g,p0,p1);
			vector_add3mul(p,p0,p1,0.5);
			dsDrawCappedCylinder(p,dGeomGetRotation(g),*((dReal*)dGeomGetClassData(g)),0.01);
		}
	}
	for(i=m_Objects.size();i--;m_Objects[i]->Draw());
	
}