#include <ode/addons/advShapes.h>
#include <ode/addons/misc.h>
#include <ode/odemath.h>

#include <math.h>

//------------------------------------------------------------------------
// примитив грань (треугольник)
// p0, p1, p2 - вершины треугольника в мировой системе координат;

int duFaceClass=-1; //standart (flat) cylinder

struct duFace // координаты face
{
	dVector3 p[3];
};

//---------------------------------------------------------------------------------
void 	dGeomFaceSetParams(dGeomID g,const dVector3 p0,const dVector3 p1,const dVector3 p2)
{
	dUASSERT (g && dGeomGetClass(g)==duFaceClass,"argument not a face");
	duFace *c=(duFace*)dGeomGetClassData(g);
	vector_copy3(c->p[0],p0);
	vector_copy3(c->p[1],p1);
	vector_copy3(c->p[2],p2);
}


void 	dGeomFaceGetParams(dGeomID g,dVector3 p0,dVector3 p1,dVector3 p2)
{
	dUASSERT(g && dGeomGetClass(g)==duFaceClass,"argument not a face");
	const duFace *c=(const duFace*)dGeomGetClassData(g);
	vector_copy3(p0,c->p[0]);
	vector_copy3(p1,c->p[1]);
	vector_copy3(p2,c->p[2]);
}

//---------------------------------------------------------------------------------
#define CONTACT(p,skip) ((dContactGeom*) (((char*)p) + (skip)))
#define NUMC_MASK 0xffff

//---------------------------------------------------------------------------------
// создание объекта

dReal CheckLine(dReal x,dReal y,dReal x1,dReal y1,dReal x2,dReal y2)
{
	return (y2-y1)*(x-x1)+(x1-x2)*(y-y1);
}

int CheckInTriangle(dReal x,dReal y,dReal x1,dReal y1,dReal x2,dReal y2,dReal x3,dReal y3)
{
	dReal sign[3]={CheckLine(x,y,x1,y1,x2,y2),
		       CheckLine(x,y,x2,y2,x3,y3),
	      	       CheckLine(x,y,x3,y3,x1,y1)};
	for(int i=0;i<3;i++)
		if(sign[i]>dReal(0))
			return sign[(i+1)%3]>=dReal(0) && sign[(i+2)%3]>=dReal(0);
		else
			if(sign[i]<dReal(0))
			return sign[(i+1)%3]<=dReal(0) && sign[(i+2)%3]<=dReal(0);
	return false;
}

int dCollideFaceX(dGeomID gFace,dGeomID g2, int flags,dContactGeom *contact, int skip)
{
	dUASSERT(gFace && dGeomGetClass(gFace)==duFaceClass,"argument not a face");	
	dIASSERT(skip>=(int)sizeof(dContactGeom));
	
	// 1: строим плоскость на наш треугольник
	const dVector3* p=(const dVector3*)dGeomGetClassData(gFace);
	dVector3 n1,n2,n;
	vector_sub3(n1,p[1],p[0]);
	vector_sub3(n2,p[2],p[0]);
	dCROSS(n,=,n1,n2);
	dNormalize3(n);
	dGeomID gfPlane=dCreatePlane(NULL,n[0],n[1],n[2],dDOT(n,p[0]));
	// 2: находим пересечение
	int num=dCollide(gfPlane,g2,flags,contact,skip);
	// Re: пункты 3 и 4 делать для каждого пересечения!
	for(int i=0;i<num;i++)
	{
		dContactGeom* cg=CONTACT(contact,i*skip);
		int SkipFlag=0;
	// 3: проверяем на паралельность, выбираем плоскость наиболее "лежачую"
	// 4: проверяем на вхождение
		if(n[2]>=n[0] && n[2]>=n[1]) // Плоскость XY подходит
		{
			if(!CheckInTriangle(cg->pos[0],cg->pos[1],p[0][0],p[0][1],p[1][0],p[1][1],p[2][0],p[2][1]))
				SkipFlag=1;
		}
		else
		if(n[1]>=n[0] && n[1]>=n[2]) // Плоскость XZ подходит
		{
			if(!CheckInTriangle(cg->pos[0],cg->pos[2],p[0][0],p[0][2],p[1][0],p[1][2],p[2][0],p[2][2]))
				SkipFlag=1;
		}
		else // Плоскость YZ подходит
		{
			if(!CheckInTriangle(cg->pos[1],cg->pos[2],p[0][1],p[0][2],p[1][1],p[1][2],p[2][1],p[2][2]))
				SkipFlag=1;
		}
		if(SkipFlag) // рекомендуется отброс поверхности
		{
/*			// проверяем пересечение с ребрами. Если пересечение есть, отброс откладывается
			// считаем, что пересечение может быть только одно
			for(int j=0;j<3;j++)
			{
				dGeomID ray=dCreateEdge(NULL,p[j],p[(j+1)%3]);
				dContactGeom ContactGm;
				int xn=dCollide(ray,g2,1,&ContactGm,skip);
				dGeomDestroy(ray);
				if(xn)
				{
					vector_copy3(cg->pos,ContactGm.pos); // рекомендуется уточнение позиции.
//					vector_copy3(cg->normal,n);
					cg->depth/=2;
					break;
				}
			}
			if(j==3)*/
			{
				if(i==--num)
					break;
				memcpy(cg,CONTACT(contact,num*skip),sizeof(dContactGeom));
				i--;
				continue;
			}
		}
		if(cg->g1==gfPlane)
		{
			cg->g1=gFace;
		}
		else
		{
			cg->g2=gFace;
//			vector_mul3(CONTACT(contact,i*skip)->normal,-1);
		}
//    		CONTACT(contact,i*skip)->g2=gCyl;
//    		vector_mul3(CONTACT(contact,i*skip)->normal,-1);
	}
	dGeomDestroy(gfPlane);
	return num;
	//return 0;
}

static dColliderFn* dFaceColliderFn(int num)
{
//	if(num==dPlaneClass)		???;
//	if(num==dBoxClass)    		return dCollideFaceBox;
//  	if(num==dSphereClass) 		???;
//  	if(num==duCylinderClass) 	return dCollideFaceCyl;
//  	if(num==dRayClass) 		???;
//  	if(num==dCCylinderClass)	???;
//  	if(num==duFaceClass) 		???;
//  	return NULL;
	return dCollideFaceX;
}

static void dFaceAABB(dGeomID g,dReal aabb[6])
{
	dUASSERT (g && dGeomGetClass(g)==duFaceClass,"argument not a face");
	// !!! все эты выполняется в предположении, что грань неподвижна!!!
	const duFace* p=(const duFace*)dGeomGetClassData(g);

	aabb[0]=dInfinity;
	aabb[1]=-dInfinity;
	aabb[2]=dInfinity;
	aabb[3]=-dInfinity;
	aabb[4]=dInfinity;
	aabb[5]=-dInfinity;

	for(int i=0;i<3;i++)
	{
		if(aabb[0]>p->p[i][0])
			aabb[0]=p->p[i][0];
		if(aabb[1]<p->p[i][0])
			aabb[1]=p->p[i][0];
		if(aabb[2]>p->p[i][1])
			aabb[2]=p->p[i][1];
		if(aabb[3]<p->p[i][1])
			aabb[3]=p->p[i][1];
		if(aabb[4]>p->p[i][2])
			aabb[4]=p->p[i][2];
		if(aabb[5]<p->p[i][2])
			aabb[5]=p->p[i][2];
	}
}

dGeomID dCreateFace(dSpaceID space,const dVector3 p0,const dVector3 p1,const dVector3 p2)
{
  	if(duFaceClass == -1) 
	{
    		dGeomClass c;
    		c.bytes=sizeof(duFace);
    		c.collider=&dFaceColliderFn;
    		c.aabb=&dFaceAABB;
    		c.aabb_test=NULL;
    		c.dtor=NULL;
    		duFaceClass=dCreateGeomClass(&c);
  	}
        dGeomID g=dCreateGeom(duFaceClass);
  	if(space)
		dSpaceAdd(space,g);
	dGeomFaceSetParams(g,p0,p1,p2);
  	return g;
}
