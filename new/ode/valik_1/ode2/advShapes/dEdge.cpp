#include <ode/addons/advShapes.h>
#include <ode/addons/misc.h>
#include <ode/rotation.h>
#include <ode/odemath.h>
#include <math\misc.h>

int duEdgeClass=-1;

const dReal MicroCylinder=1e-6;


void 	dGeomEdgeSetParams(dGeomID g,const dVector3 p0,const dVector3 p1)
{
	dUASSERT(g && dGeomGetClass(g)==duEdgeClass,"argument not a edge");
	dVector3 v;
	vector_sub3(v,p1,p0);
	*((dReal*)dGeomGetClassData(g))=sqrtf(sqr(v[0])+sqr(v[1])+sqr(v[2])); // length of Edge
	dGeomSetPosition(g,p0[0],p0[1],p0[2]);
	dMatrix3 R;
	dRFromZAxis(R,v[0],v[1],v[2]);
	dGeomSetRotation(g,R);
}

void 	dGeomEdgeGetParams(dGeomID g,dVector3 p0,dVector3 p1)
{
	dUASSERT(g && dGeomGetClass(g)==duEdgeClass,"argument not a edge");
	vector_copy3(p0,dGeomGetPosition(g));
	const dReal* R=dGeomGetRotation(g);
	dReal l=*((dReal*)dGeomGetClassData(g));
	p1[0]=p0[0]+R[2]*l;
	p1[1]=p0[1]+R[6]*l;
	p1[2]=p0[2]+R[10]*l;
}


//---------------------------------------------------------------------------------
int dCollideEdgeSphere(dGeomID gEdge,dGeomID gSphere,int flags,dContactGeom *contact, int skip)
{
  	dIASSERT(skip>=(int)sizeof(dContactGeom));
	dUASSERT(gEdge && dGeomGetClass(gEdge)==duEdgeClass,"argument not a edge");
	dUASSERT(gSphere && dGeomGetClass(gSphere)==dSphereClass,"argument not a sphere");
	contact->g1=gEdge;
  	contact->g2=gSphere;
	dVector3 q,p0,p1;
	dGeomEdgeGetParams(gEdge,p0,p1);
	const dReal* sp0=dGeomGetPosition(gSphere);
	vector_sub3(q,sp0,p0);
	const dReal* R=dGeomGetRotation(gEdge);
  	dReal B=dDOT14(q,R+2); // проекция центра сферы на прямую ребра
  	if(B<0)
  		vector_copy3(contact->pos,p0);
  	else
  	{
  		dReal l=*((dReal*)dGeomGetClassData(gEdge));
  		if(B<l) // пересечение где-то посредине
  		{
  			contact->pos[0]=p0[0]+R[2]*B;
			contact->pos[1]=p0[1]+R[6]*B;
			contact->pos[2]=p0[2]+R[10]*B;
  		}
  		else
  			vector_copy3(contact->pos,p1);
	}
	contact->depth=dGeomSpherePointDepth(gSphere,contact->pos[0],contact->pos[1],contact->pos[2]);
  	if(contact->depth<0)
  		return 0;
	vector_sub3(contact->normal,contact->pos,sp0);
	dNormalize3(contact->normal);
	return 1; 
}

//---------------------------------------------------------------------------------
int dCollideEdgeCyl(dGeomID gEdge,dGeomID gCyl,int flags,dContactGeom *contact, int skip)
{

extern int dCollideCylCyl (dGeomID o1,dGeomID o2, int flags,dContactGeom *contact, int skip);

  	dIASSERT(skip>=(int)sizeof(dContactGeom));
	dUASSERT(gEdge && dGeomGetClass(gEdge)==duEdgeClass,"argument not a edge");
	dUASSERT(gCyl && dGeomGetClass(gCyl)==duCylinderClass,"argument not a cylinder");
	// ужжасно пошло:
	// создаем сверхтонкий цилиндр на это фуфло
	// перысикаим, смотрим и восторгаимся результатом
	dReal le=*((dReal*)dGeomGetClassData(gEdge));
	dGeomID geKK=dCreateCylinder(0,MicroCylinder,le);
	dGeomSetRotation(geKK,dGeomGetRotation(gEdge));
	dVector3 p0,p1;
	dGeomEdgeGetParams(gEdge,p0,p1);
	dGeomSetPosition(geKK,(p0[0]+p1[0])*0.5,(p0[1]+p1[1])*0.5,(p0[2]+p1[2])*0.5);
	int num=dCollideCylCyl(geKK,gCyl,flags,contact,skip);
	for(int i=0;i<num;i++)
		CONTACT(contact,i*skip)->g1=gEdge;
	dGeomDestroy(geKK);
	return num;
}

int dCollideEdgeFiX(dGeomID gEdge,dGeomID g2,int flags,dContactGeom *contact, int skip)
{
  	dIASSERT(skip>=(int)sizeof(dContactGeom));
	dUASSERT(gEdge && dGeomGetClass(gEdge)==duEdgeClass,"argument not a edge");

	// ужжасно пошло:
	// создаем сверхтонкий цилиндр на это фуфло
	// перысикаим, смотрим и восторгаимся результатом
	dReal le=*((dReal*)dGeomGetClassData(gEdge));
	dGeomID geKK=dCreateCylinder(0,MicroCylinder,le);
	dGeomSetRotation(geKK,dGeomGetRotation(gEdge));
	dVector3 p0,p1;
	dGeomEdgeGetParams(gEdge,p0,p1);
	dGeomSetPosition(geKK,(p0[0]+p1[0])*0.5,(p0[1]+p1[1])*0.5,(p0[2]+p1[2])*0.5);
	int num=dCollide(geKK,g2,flags,contact,skip);
	for(int i=0;i<num;i++)
		if(CONTACT(contact,i*skip)->g1==geKK)
			CONTACT(contact,i*skip)->g1=gEdge;
		else
			CONTACT(contact,i*skip)->g2=gEdge;
	dGeomDestroy(geKK);
	return num;	
}

static dColliderFn* dEdgeColliderFn(int num)
{
//	if(num==dPlaneClass)		???;
//	if(num==dBoxClass)    		return dCollideFaceBox;
  	if(num==dSphereClass) 		return dCollideEdgeSphere;
  	if(num==duCylinderClass) 	return dCollideEdgeCyl;
//  	if(num==dRayClass) 		???;
//  	if(num==dCCylinderClass)	???;
//  	if(num==duFaceClass) 		???;
	return dCollideEdgeFiX;
}

static void dEdgeAABB(dGeomID g,dReal aabb[6])
{
	dUASSERT (g && dGeomGetClass(g)==duEdgeClass,"argument not a edge");
	dVector3 p0,p1;
	dGeomEdgeGetParams(g,p0,p1);
	for(int i=0;i<3;i++)
		if(p0[i]>p1[i])
		{
			aabb[2*i]=p1[i];
			aabb[2*i+1]=p0[i];
		}
		else
		{
			aabb[2*i]=p0[i];
			aabb[2*i+1]=p1[i];
		}
}


dGeomID dCreateEdge(dSpaceID space,const dVector3 p0,const dVector3 p1)
{
  	if(duEdgeClass == -1) 
	{
    		dGeomClass c;
    		c.bytes=sizeof(dReal);
    		c.collider=&dEdgeColliderFn;
    		c.aabb=&dEdgeAABB;
    		c.aabb_test=NULL;
    		c.dtor=NULL;
    		duEdgeClass=dCreateGeomClass(&c);
  	}
        dGeomID g=dCreateGeom(duEdgeClass);
  	if(space)
		dSpaceAdd(space,g);
	dGeomEdgeSetParams(g,p0,p1);
  	return g;
}
