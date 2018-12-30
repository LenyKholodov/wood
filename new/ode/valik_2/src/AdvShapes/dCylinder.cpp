#include <pch.h>

// Не доработан цилиндр-цилиндр пересечение


#include <windows.h>
#include <ode/ode.h>
#include <ode\addons\advShapes.h>

// given a pointer `p' to a dContactGeom, return the dContactGeom at
// p + skip bytes.


struct dxCylinder 
{	// cylinder
  dReal radius,lz;	// radius, length along y axis //  
};

int dCylinderClassUser=-1;

#define NUMC_MASK (0xffff)

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////circleIntersection//////////////////////////////////////////////////
//this does following:
//takes two circles as normals to planes n1,n2, center points cp1,cp2,and radiuses r1,r2
//finds line on which circles' planes intersect
//finds four points O1,O2 - intersection between the line and sphere with center cp1 radius r1
//					O3,O4 - intersection between the line and sphere with center cp2 radius r2
//returns false if there is no intersection
//computes distances O1-O3, O1-O4, O2-O3, O2-O4
//in "point" returns mean point between intersection points with smallest distance
/////////////////////////////////////////////////////////////////////////////////////////////////

inline bool circleIntersection(const dReal* n1,const dReal* cp1,dReal r1,const dReal* n2,const dReal* cp2,dReal r2,dVector3 point)
{
	dReal c1=dDOT14(cp1,n1);
	dReal c2=dDOT14(cp2,n2);
	dReal cos=dDOT44(n1,n2);
	dReal cos_2=cos*cos;
	dReal sin_2=1-cos_2;
	dReal p1=(c1-c2*cos)/sin_2;
	dReal p2=(c2-c1*cos)/sin_2;
	dVector3 lp={p1*n1[0]+p2*n2[0],p1*n1[4]+p2*n2[4],p1*n1[8]+p2*n2[8]};
	dVector3 n;
	dCROSS144(n,=,n1,n2);
	dVector3 LC1={lp[0]-cp1[0],lp[1]-cp1[1],lp[2]-cp1[2]};
	dVector3 LC2={lp[0]-cp2[0],lp[1]-cp2[1],lp[2]-cp2[2]};
	dReal A,B,C,B_A,B_A_2,D;
	dReal t1,t2,t3,t4;
	A=dDOT(n,n);
	B=dDOT(LC1,n);
	C=dDOT(LC1,LC1)-r1*r1;
	B_A=B/A;
	B_A_2=B_A*B_A;
	D=B_A_2-C;
	if(D<0.f)
	{	//somewhat strange solution 
			//- it is needed to set some 
			//axis to sepparate cylinders
			//when their edges approach
		t1=-B_A+sqrtf(-D);
		t2=-B_A-sqrtf(-D);
//		return false;
	}
	else
	{
		t1=-B_A-sqrtf(D);
		t2=-B_A+sqrtf(D);
	}
	B=dDOT(LC2,n);
	C=dDOT(LC2,LC2)-r2*r2;
	B_A=B/A;
	B_A_2=B_A*B_A;
	D=B_A_2-C;
	if(D<0.f) 
	{
		t3=-B_A+sqrtf(-D);
		t4=-B_A-sqrtf(-D);
	//	return false;
	}
	else
	{
		t3=-B_A-sqrtf(D);
		t4=-B_A+sqrtf(D);
	}
	dVector3 O1={lp[0]+n[0]*t1,lp[1]+n[1]*t1,lp[2]+n[2]*t1};
	dVector3 O2={lp[0]+n[0]*t2,lp[1]+n[1]*t2,lp[2]+n[2]*t2};
	dVector3 O3={lp[0]+n[0]*t3,lp[1]+n[1]*t3,lp[2]+n[2]*t3};
	dVector3 O4={lp[0]+n[0]*t4,lp[1]+n[1]*t4,lp[2]+n[2]*t4};
	dVector3 L1_3={O3[0]-O1[0],O3[1]-O1[1],O3[2]-O1[2]};
	dVector3 L1_4={O4[0]-O1[0],O4[1]-O1[1],O4[2]-O1[2]};
	dVector3 L2_3={O3[0]-O2[0],O3[1]-O2[1],O3[2]-O2[2]};
	dVector3 L2_4={O4[0]-O2[0],O4[1]-O2[1],O4[2]-O2[2]};
	dReal l1_3=dDOT(L1_3,L1_3);
	dReal l1_4=dDOT(L1_4,L1_4);
	dReal l2_3=dDOT(L2_3,L2_3);
	dReal l2_4=dDOT(L2_4,L2_4);
	if(l1_3<l1_4)
		if(l2_3<l2_4)
			if(l1_3<l2_3) // l1_3
				vector_add3mul(point,O1,O3,0.5);
			else		//l2_3;
				vector_add3mul(point,O2,O3,0.5);
		else
			if(l1_3<l2_4)	//l1_3;
				vector_add3mul(point,O1,O3,0.5);			
			else //l2_4;
				vector_add3mul(point,O2,O4,0.5);
	else
		if(l2_3<l2_4)
			if(l1_4<l2_3) //l1_4;
				vector_add3mul(point,O1,O4,0.5);				
			else //l2_3;
				vector_add3mul(point,O2,O3,0.5);			
		else
			if(l1_4<l2_4) //l1_4;
				vector_add3mul(point,O1,O4,0.5);			
			else //l2_4;
				vector_add3mul(point,O2,O4,0.5);			
	return true;
}


void lineClosestApproach(const dVector3 pa, const dVector3 ua,
						 const dVector3 pb, const dVector3 ub,
						 dReal *alpha, dReal *beta)
{	
	dReal uaub=dDOT(ua,ub);	
	dReal d=1-sqr(uaub);
	if(d<=0) 
	{
		// @@@ this needs to be made more robust
		*alpha=0;
		*beta =0;
	}
	else 
	{
		dVector3 p;
		vector_sub3(p,pb,pa);
		dReal q1= dDOT(ua,p);
		dReal q2=-dDOT(ub,p);
	    d=dRecip(d);
		*alpha=(q1+uaub*q2)*d;
		*beta =(uaub*q1+q2)*d;
	}
}

// @@@ some stuff to optimize here, reuse code in contact point calculations.
int dCylBox(const dVector3 p1,const dMatrix3 R1,const dReal radius,const dReal lz,
			const dVector3 p2,const dMatrix3 R2,const dVector3 side2,
			dVector3 normal, dReal *depth, int *code,int maxc,dContactGeom *contact,int skip)
{
	dVector3 p,pp,normalC;
	const dReal *normalR=NULL;
	dVector3 B;
	dMatrix3 R;
	dReal s,s2,l,sQ21,sQ22,sQ23;	
	bool invert_normal;
	// get vector from centers of box 1 to box 2, relative to box 1
	vector_sub3(p,p2,p1);
	dMULTIPLY1_331 (pp,R1,p);		// get pp = p relative to body 1
	// get side lengths / 2
	//A1 =radius; A2 = lz*REAL(0.5); A3 = radius;
	dReal hlz=lz*dReal(0.5);
	vector_mul3(B,side2,0.5);  
	// Rij is R1'*R2, i.e. the relative rotation between R1 and R2
	dMULTIPLY1_333(R,R1,R2);
  		
	//   * see if the axis separates the box with cylinder. if so, return 0.
	//   * find the depth of the penetration along the separating axis (s2)
	//   * if this is the largest depth so far, record it.
	// the normal vector will be set to the separating axis with the smallest
	// depth. note: normalR is set to point to a column of R1 or R2 if that is
	// the smallest depth normal so far. otherwise normalR is 0 and normalC is
	// set to a vector relative to body 1. invert_normal is 1 if the sign of
	// the normal should be flipped.
#define TEST(expr1,expr2,norm,cc)		\
			s2=dFabs(expr1)-(expr2);	\
			if(s2>0)					\
				return 0;				\
			if(s2>s)					\
			{							\
				s=s2;					\
				normalR=norm;			\
				invert_normal=(expr1)<0;\
				*code=(cc);				\
			}
	s=-dInfinity;
	invert_normal=false;
	*code=0;
	// separating axis = cylinder ax u2
	//used when a box vertex touches a flat face of the cylinder

	dVector3 Q3;
	Q3[0]=dFabs(R[8+0]); Q3[1]=dFabs(R[8+1]); Q3[2]=dFabs(R[8+2]);

	TEST (pp[2],(hlz+dDOT(B,Q3)),R1+2,0);
	// separating axis = box axis v1,v2,v3
	//used when cylinder edge touches box face
	//there is two ways to compute sQ: sQ21=sqrtf(1.f-Q21*Q21); or sQ21=sqrtf(Q23*Q23+Q22*Q22); 
	//if we did not need Q23 and Q22 the first way might be used to quiken the routine but then it need to 
	//check if Q21<=1.f, becouse it may slightly exeed 1.f.
 
	sQ21=sqrtf(sqr(Q3[2])+sqr(Q3[1]));
	TEST(dDOT41(R2+0,p),(radius*sQ21+hlz*Q3[0]+B[0]),R2+0,1);
	sQ22=sqrtf(sqr(Q3[2])+sqr(Q3[0]));
	TEST(dDOT41(R2+1,p),(radius*sQ22+hlz*Q3[1]+B[1]),R2+1,2);
	sQ23=sqrtf(sqr(Q3[1])+sqr(Q3[0]));
	TEST(dDOT41(R2+2,p),(radius*sQ23+hlz*Q3[2]+B[2]),R2+2,3);
#undef TEST
#define TEST(expr1,expr2,n,cc)			\
			s2=dFabs(expr1)-(expr2);	\
			if(s2>0)					\
				return 0;				\
			if(s2>s)					\
			{							\
				s=s2;					\
				normalR=NULL;			\
				vector_copy3(normalC,n);\
				invert_normal=(expr1)<0;\
				*code=(cc);				\
			}
	// separating axis is a normal to the cylinder axis passing across the nearest box vertex
	//used when a box vertex touches the lateral surface of the cylinder
	dReal proj,boxProj,cos,sin,cos1,cos2;
	dVector3 tAx,Ax,pb;
	
	//making Ax which is perpendicular to cyl ax to box position//
	proj=dDOT14(p2,R1+2)-dDOT14(p1,R1+2);
	vector_add3Cmul(Ax,p2,R1+2,-proj);
	vector_sub3(Ax,p1);	
	dNormalize3(Ax);
	//using Ax find box vertex which is nearest to the cylinder axis		
	vector_copy3(pb,p2);		
	vector_add3Cmul(pb,R2  ,dDOT14(Ax,R2  )>0?-B[0]:B[0]);
	vector_add3Cmul(pb,R2+1,dDOT14(Ax,R2+1)>0?-B[1]:B[1]);
	vector_add3Cmul(pb,R2+2,dDOT14(Ax,R2+2)>0?-B[2]:B[2]);
	//building axis which is normal to cylinder ax to the nearest box vertex
	proj=dDOT14(pb,R1+2)-dDOT14(p1,R1+2);
	vector_add3Cmul(Ax,pb,R1+2,-proj);
	vector_sub3(Ax,p1);	
	dNormalize3(Ax);
	
	boxProj=dFabs(dDOT14(Ax,R2+0)*B[0])+dFabs(dDOT14(Ax,R2+1)*B[1])+dFabs(dDOT14(Ax,R2+2)*B[2]);
	TEST(dDOT(p,Ax),(radius+boxProj),Ax,4);
	//next three test used to handle collisions between cylinder circles and box ages
	for(int i=0;i<3;i++)
	{
		proj=dDOT14(p1,R2+i)-dDOT14(p2,R2+i);
		vector_add3Cmul(tAx,p2,R2+i,proj);
		vector_sub3(tAx,p1);	
		dNormalize3(tAx);
		//now tAx is normal to i-ax of the box to cylinder center
		//making perpendicular to tAx lying in the plane which is normal to the cylinder axis
		//it is tangent in the point where projection of tAx on cylinder's ring intersect edge circle
		cos=dDOT14(tAx,R1+0);
		sin=dDOT14(tAx,R1+1);
		tAx[0]=R1[1  ]*cos-R1[0]*sin;
		tAx[1]=R1[4+1]*cos-R1[4+0]*sin;
		tAx[2]=R1[8+1]*cos-R1[8+0]*sin;
		//use cross between tAx and first ax of the box as separating axix 
		dCROSS114(Ax,=,tAx,R2+i);
		dNormalize3(Ax);
		boxProj=dFabs(dDOT14(Ax,R2+0)*B[0])+dFabs(dDOT14(Ax,R2+1)*B[1])+dFabs(dDOT14(Ax,R2+2)*B[2]);
		cos=dFabs(dDOT14(Ax,R1+2));
		cos1=dDOT14(Ax,R1+0);
		cos2=dDOT14(Ax,R1+1);
		sin=sqrtf(sqr(cos1)+sqr(cos2));
		TEST(dDOT(p,Ax),(sin*radius+cos*hlz+boxProj),Ax,5+i);
	}
#undef TEST
	// note: cross product axes need to be scaled when s is computed.
	// normal (n1,n2,n3) is relative to box 1.
#define TEST(expr1,expr2,n1,n2,n3,cc)		\
			s2=dFabs(expr1)-(expr2);		\
			if(s2>0)						\
				return 0;					\
			l=dSqrt(sqr(n1)+sqr(n2)+sqr(n3));\
			if(l>0)							\
			{								\
				s2/=l;						\
				if(s2>s)					\
				{							\
					s=s2;					\
					normalR=NULL;			\
					normalC[0]=(n1)/l; normalC[1] = (n2)/l; normalC[2] = (n3)/l; \
					invert_normal=(expr1)<0;\
					*code = (cc);			\
				}							\
			}
		//crosses between cylinder axis and box axes
		// separating axis = u2 x (v1,v2,v3)
	TEST(pp[0]*R[4+0]-pp[1]*R[0+0],(radius+B[1]*Q3[2]+B[2]*Q3[1]),R[4+0],0,-R[0+0],8);
	TEST(pp[0]*R[4+1]-pp[1]*R[0+1],(radius+B[0]*Q3[2]+B[2]*Q3[0]),R[4+1],0,-R[0+1],9);
	TEST(pp[0]*R[4+2]-pp[1]*R[0+2],(radius+B[0]*Q3[1]+B[1]*Q3[0]),R[4+2],0,-R[0+2],10);
#undef TEST
  // if we get to this point, the boxes interpenetrate. compute the normal
  // in global coordinates.
	if(normalR)
		vector_copy3C(normal,normalR); 
	else 
	{
		if(*code>7) 
			dMULTIPLY0_331(normal,R1,normalC);
		else
			vector_copy3(normal,normalC);		
	}
	if(invert_normal)
		vector_neg3(normal);
	*depth=-s;
	// compute contact point(s)
	if(*code>7)
	{
			//find point on the cylinder pa deepest along normal
		dVector3 pa;
		dReal cos1,cos2,factor;
		vector_copy3(pa,p1);
      	cos1=dDOT14(normal,R1+0);
		cos2=dDOT14(normal,R1+1);
		factor=sqrtf(sqr(cos1)+sqr(cos2));
		cos1/=factor;
		cos2/=factor;
		vector_add3Cmul(pa,R1,cos1*radius);
		vector_add3Cmul(pa,R1+1,cos2*radius);
		vector_add3Cmul(pa,R1+2,dDOT14(normal,R1+2)>0?hlz:-hlz);
		// find vertex of the box  deepest along normal 
		dVector3 pb;
		vector_copy3(pb,p2);
		vector_add3Cmul(pb,R2  ,dDOT14(normal,R2  )>0?-B[0]:B[0]);
		vector_add3Cmul(pb,R2+1,dDOT14(normal,R2+1)>0?-B[1]:B[1]);
		vector_add3Cmul(pb,R2+2,dDOT14(normal,R2+2)>0?-B[2]:B[2]);
		dReal alpha,beta;
		dVector3 ua,ub;
		vector_copy3C(ua,R1+2);
		vector_copy3C(ub,R2+*code-8);
        lineClosestApproach (pa,ua,pb,ub,&alpha,&beta);
		vector_add3mul(pa,ua,alpha);
		vector_add3mul(pb,ub,beta);
		vector_add3mul(contact[0].pos,pa,pb,0.5);  
		contact[0].depth=*depth;
		return 1;
	}
  	if(*code==4)
	{
		vector_copy3(contact[0].pos,pb);
		contact[0].depth=*depth;
		return 1;
	}
    dVector3 vertex;
	if(*code==0) 
	{		
		vector_copy3(vertex,p2);
		vector_add3Cmul(vertex,R2  ,dDOT14(normal,R2  )>0?-B[0]:B[0]);
		vector_add3Cmul(vertex,R2+1,dDOT14(normal,R2+1)>0?-B[1]:B[1]);
		vector_add3Cmul(vertex,R2+2,dDOT14(normal,R2+2)>0?-B[2]:B[2]);
	}
	else 
	{
		dReal cos1,cos2,factor;
		vector_copy3(vertex,p1);
		cos1=dDOT14(normal,R1+0);
		cos2=dDOT14(normal,R1+1);
		factor=sqrtf(sqr(cos1)+sqr(cos2));
		if(factor)
		{
			cos1/=factor;
			cos2/=factor;
		}
		vector_add3Cmul(vertex,R1,cos1*radius);
		vector_add3Cmul(vertex,R1+1,cos2*radius);
		vector_add3Cmul(vertex,R1+2,dDOT14(normal,R1+2)>0?hlz:-hlz);
	}
	vector_copy3(contact[0].pos,vertex);
	contact[0].depth=*depth;
	return 1;
}



//------------------------------------------------------------------------------------------------------
int dCollideCylS (dxGeom *o1, dxGeom *o2, int flags,dContactGeom *contact, int skip)
{
	dIASSERT(skip>=(int)sizeof(dContactGeom));
	dIASSERT(dGeomGetClass(o2)==dSphereClass);
	dIASSERT(dGeomGetClass(o1)==dCylinderClassUser);
	
	const dReal* p1=dGeomGetPosition(o1);
	const dReal* p2=dGeomGetPosition(o2);
	const dReal* R=dGeomGetRotation(o1);

	dVector3 p;
	//Смещение центра сферы относительно цилиндра
	vector_sub3(p,p2,p1);

	dReal cylRadius;
	dReal hl;
	dGeomCylinderGetParams(o1,&cylRadius,&hl);	
	hl*=0.5;

	dReal sphereRadius=dGeomSphereGetRadius(o2);
	// Находим расстояние от центра цилиндра до проекции центра сферы
	dReal dAZ=dDOT14(p,R+2);
	bool SubZeroDAZ=dAZ<0;
	dReal fdAZ=SubZeroDAZ?-dAZ:dAZ;
	if(fdAZ>sphereRadius+hl) // Центр сферы выше (ниже) верхней (нижней) плоскости цилиндра ?
		return 0;	// Да. Считать нечего
	// Находим базис, в котором в плоскости OYZ будут лежать центр сферы и ось цилиндра
	dVector3 Ry;// Вектор Z не меняем, поэтому вместо Rz <=> R[+2]
	// Находим вектор OY
	// OY=Normalize(p-R(z)*dAZ
	vector_sub3Cmul(Ry,p,R+2,dAZ);	
	dReal dAY=dSqrt(sqr(Ry[0])+sqr(Ry[1])+sqr(Ry[2]));
	if(dAY>sphereRadius+cylRadius)	// Расстояние от цилиндра слишком велико - столкновения нет
		return 0;	
	if(dAY<1e-10)	// Можно считать, что сфера на оси цилиндра
	{
		contact->g1=o1;
		contact->g2=o2;	
		// Нормаль совпадает с осью цилиндра
		if(SubZeroDAZ)
		{	
			vector_copy3C(contact->normal,R+2);
			vector_add3Cmul(contact->pos,p1,R+2,dAZ+sphereRadius);			
		}
		else
		{
			contact->normal[0]=-R[2];
			contact->normal[1]=-R[6];
			contact->normal[2]=-R[10];
			vector_add3Cmul(contact->pos,p1,R+2,dAZ-sphereRadius);
		}
		contact->depth=sphereRadius+hl-fdAZ;
		return 1;
	}
	else // Нормируем вектор OY
		vector_mul3(Ry,dRecip(dAY));
	// 1й вариант: боковая поверхность цилиндра:
	//						abs(dAY)<Rcyl+Rsphere (выполнено)
	//						abs(dAZ)<hl
	if(fdAZ<=hl)
	{	// Расклад точки пересечения по векторам:
		// По OZ: dAZ; По OY: dAY-Rsphere
		// Глубина: RCyl+Rsphere-dAY
		// Нормаль: OZ=0;OY=-1;
		contact->g1=o1;
		contact->g2=o2;		
		vector_neg3(contact->normal,Ry);
		contact->depth=cylRadius+sphereRadius-dAY;
		vector_add3mul2(contact->pos,p2,contact->normal,sphereRadius);
		return 1;
	}
	// 2й вариант: сфера над (под) крышкой:
	//									abs(dAY)<Rcyl
	//									abs(dAZ)>hl (выполнено)
	if(dAY<=cylRadius)
	{	// Расклад точки пересечения по векторам:
		// По OZ: dAZ>0?dAZ-Rsphere:dAZ+Rsphere; По OY: dAY
		// Глубина: dAZ>0?Rsphere+hl-dAZ:Rsphere+hl+dAZ
		// Нормаль: OZ=dAZ>0?-1:1;OY=0;
		contact->g1=o1;
		contact->g2=o2;
		contact->depth=sphereRadius+hl-fdAZ;
		if(SubZeroDAZ)
		{
			vector_copy3C(contact->normal,R+2);
		}
		else
		{
			contact->normal[0]=-R[2];
			contact->normal[1]=-R[6];
			contact->normal[2]=-R[10];
		}		
		vector_add3mul2(contact->pos,p2,contact->normal,sphereRadius);
		return 1;
	}
	// 3й вариант: происходит пересечение обода цилиндра
	// возможны 2 варианта: dAZ>0? точка(OZ:hl;OY:Rcyl):точка(OZ:-hl;OY:Rcyl)
	//					проверка: принадлежность соответствующей точки
	dReal depth=sqr(hl-fdAZ)+sqr(cylRadius-dAY);
	dReal sR2=sqr(sphereRadius);
	if(depth>sR2)	// растояие до точки больше радиуса - ничего не делаем
		return 0;
	// Все Ок!
	contact->g1=o1;
	contact->g2=o2;
	// Находим точку контакта
	vector_add3Cmul(contact->pos,p1,R+2,SubZeroDAZ?-hl:hl);
	vector_add3mul(contact->pos,Ry,cylRadius);
	// Находим нормаль...
	vector_sub3(contact->normal,contact->pos,p2);
	// Находим глубину и маштабируем нормаль
	depth=dSqrt(depth);
	vector_mul3(contact->normal,depth);
	contact->depth=sphereRadius-depth;
	return 1;
}

int dCollideCylB(dxGeom *o1,dxGeom *o2,int flags,dContactGeom *contact,int skip)
{
	dVector3 normal;
	dReal depth;
	int code;
	dReal cylRadius,cylLength;
	dVector3 boxSides;
	dGeomCylinderGetParams(o1,&cylRadius,&cylLength);
	dGeomBoxGetLengths(o2,boxSides);
	int num=dCylBox(dGeomGetPosition(o1),dGeomGetRotation(o1),cylRadius,cylLength, 
					dGeomGetPosition(o2),dGeomGetRotation(o2),boxSides,
					normal,&depth,&code,flags & NUMC_MASK,contact,skip);
	vector_neg3(normal);
	for(int i=0;i<num;i++)
	{
		dContactGeom *c=CONTACT(contact,i*skip);
		vector_copy3(c->normal,normal);
		c->g1=o1;
		c->g2=o2;		
	}
    return num;
}

int dCylCyl (const dVector3 p1, const dMatrix3 Ro1,
			const dReal radius1,const dReal lz1, const dVector3 p2,
			const dMatrix3 Ro2, const dReal radius2,const dReal lz2,
			dVector3 normal, dReal *depth, int *code,
			int maxc, dContactGeom *contact, int skip)
{
	// ъюЁЁхъЄшЁютър. Tёх ЁрёёўхЄv фхырэv фы  ЎшышэфЁр ё юё№¦ яю y. є эрё ЎшышэфЁ яю z 
	dMatrix3 R1,R2;
  	const dMatrix3 rotZ2Y={1,0,0,0,0,0,1,0,0,-1,0,0};
  	dMULTIPLY0_333(R1,Ro1,rotZ2Y);
  	dMULTIPLY0_333(R2,Ro2,rotZ2Y);

	dVector3 p,pp1,pp2,normalC;
	const dReal *normalR=NULL;
	dReal hlz1,hlz2,s,s2;
	int i;

	bool invert_normal;

  	// get vector from centers of box 1 to box 2, relative to box 1
  	vector_sub3(p,p2,p1);  	
  	dMULTIPLY1_331 (pp1,R1,p);		// get pp1 = p relative to body 1
  	dMULTIPLY1_331 (pp2,R2,p);
  	// get side lengths / 2
  	hlz1=lz1*REAL(0.5);
  	hlz2=lz2*REAL(0.5); 

 	dReal proj,cos,sin,cos1,cos3;

#define TEST(expr1,expr2,norm,cc) \
  		s2 = dFabs(expr1) - (expr2); \
		if (s2 > 0) return 0; \
	        if (s2 > s) { \
    			s = s2; \
    			normalR = norm; \
    			invert_normal =(expr1)<0; \
    			*code = (cc); \
  		}

	s = -dInfinity;
	invert_normal=false;
  	*code = 0;

	cos=dFabs(dDOT44(R1+1,R2+1));
  	sin=sqrtf(1.f-(cos>1.f ? 1.f : cos));

  	TEST(pp1[1],(hlz1+radius2*sin+hlz2*cos),R1+1,0);//pp
	TEST(pp2[1],(radius1*sin+hlz1*cos+hlz2),R2+1,1);

  	// note: cross product axes need to be scaled when s is computed.
#undef TEST
#define TEST(expr1,expr2,n1,n2,n3,cc) \
			s2 = dFabs(expr1) - (expr2); \
			if (s2 > 0) return 0; \
			if (s2 > s) { \
				s = s2; \
				normalR=NULL; \
				normalC[0] = (n1); normalC[1] = (n2); normalC[2] = (n3); \
				invert_normal = (expr1)<0; \
				*code = (cc); \
			} 
 

	dVector3 tAx,Ax,pa,pb;

	//cross between cylinders' axes
	dCROSS144(Ax,=,R1+1,R2+1);
	dNormalize3(Ax);
	TEST(dDOT(p,Ax),radius1+radius2,Ax[0],Ax[1],Ax[2],6);
	{
		dReal sign, factor;
		//making ax which is perpendicular to cyl1 ax passing across cyl2 position//
		//(project p on cyl1 flat surface )    
		vector_copy3(pb,p2);
 		//cos1 = dDOT14(p,R1+0);
		//cos3 = dDOT14(p,R1+2) ;
		tAx[0]=pp1[0]*R1[0]+pp1[2]*R1[2];
		tAx[1]=pp1[0]*R1[4]+pp1[2]*R1[6];
		tAx[2]=pp1[0]*R1[8]+pp1[2]*R1[10];
		dNormalize3(tAx);

		//find deepest point pb of cyl2 on opposite direction of tAx
 		cos1 = dDOT14(tAx,R2+0);
		cos3 = dDOT14(tAx,R2+2) ;
		factor=sqrtf(sqr(cos1)+sqr(cos3));
		cos1/=factor;
		cos3/=factor;
		for (i=0; i<3; i++) pb[i] -= cos1 * radius2 * R2[i*4];

		sign = (dDOT14(tAx,R2+1) > 0) ? REAL(1.0) : REAL(-1.0);
		for (i=0; i<3; i++) pb[i] -= sign * hlz2 * R2[i*4+1];

	    for (i=0; i<3; i++) pb[i] -= cos3 * radius2 * R2[i*4+2];

		//making perpendicular to cyl1 ax passing across pb
		proj=dDOT14(pb,R1+1)-dDOT14(p1,R1+1);

		Ax[0]=pb[0]-p1[0]-R1[1]*proj;
		Ax[1]=pb[1]-p1[1]-R1[5]*proj;
		Ax[2]=pb[2]-p1[2]-R1[9]*proj;	
	}

	dNormalize3(Ax);
	
	cos=dFabs(dDOT14(Ax,R2+1));
	cos1=dDOT14(Ax,R2+0);
	cos3=dDOT14(Ax,R2+2);
	sin=sqrtf(sqr(cos1)+sqr(cos3));

	TEST(dDOT(p,Ax),radius1+cos*hlz2+sin*radius2,Ax[0],Ax[1],Ax[2],3);

	{
		dReal sign, factor;
		vector_copy3(pa,p1);
   	
 		//making ax which is perpendicular to cyl2 ax passing across cyl1 position//
		//(project p on cyl2 flat surface )
 		//cos1 = dDOT14(p,R2+0);
		//cos3 = dDOT14(p,R2+2) ;
		tAx[0]=pp2[0]*R2[0]+pp2[2]*R2[2];
		tAx[1]=pp2[0]*R2[4]+pp2[2]*R2[6];
		tAx[2]=pp2[0]*R2[8]+pp2[2]*R2[10];
		dNormalize3(tAx);

	 	cos1 = dDOT14(tAx,R1+0);
		cos3 = dDOT14(tAx,R1+2) ;
		factor=sqrtf(sqr(cos1)+sqr(cos3));
		cos1/=factor;
		cos3/=factor;

		//find deepest point pa of cyl2 on direction of tAx
		for (i=0; i<3; i++) pa[i] += cos1 * radius1 * R1[i*4];

		sign = (dDOT14(tAx,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
		for (i=0; i<3; i++) pa[i] += sign * hlz1 * R1[i*4+1];

		for (i=0; i<3; i++) pa[i] += cos3 * radius1 * R1[i*4+2];

		proj=dDOT14(pa,R2+1)-dDOT14(p2,R2+1);

		Ax[0]=pa[0]-p2[0]-R2[1]*proj;
		Ax[1]=pa[1]-p2[1]-R2[5]*proj;
		Ax[2]=pa[2]-p2[2]-R2[9]*proj;
	}
	
	dNormalize3(Ax);
	cos=dFabs(dDOT14(Ax,R1+1));
	cos1=dDOT14(Ax,R1+0);
	cos3=dDOT14(Ax,R1+2);
	sin=sqrtf(sqr(cos1)+sqr(cos3));

	TEST(dDOT(p,Ax),radius2+cos*hlz1+sin*radius1,Ax[0],Ax[1],Ax[2],4);


////test circl

//@ this needed to set right normal when cylinders edges intersect
//@ the most precise axis for this test may be found as a line between nearest points of two
//@ circles. But it needs comparatively a lot of computation.
//@ I use a trick which lets not to solve quadric equation. 
//@ In the case when cylinder eidges touches the test below rather accurate.
//@ I still not sure about problems with sepparation but they have not been revealed during testing.
	dVector3 point;
	{
		dVector3 ca,cb; 
		dReal sign;
		for (i=0; i<3; i++) ca[i] = p1[i];
		for (i=0; i<3; i++) cb[i] = p2[i];
		//find to nearest flat rings
		sign = (pp1[1] > 0) ? REAL(1.0) : REAL(-1.0);
		for (i=0; i<3; i++) ca[i] += sign * hlz1 * R1[i*4+1];
		 sign = (pp2[1] > 0) ? REAL(1.0) : REAL(-1.0);
		for (i=0; i<3; i++) cb[i] -= sign * hlz2 * R2[i*4+1];

		dVector3 tAx,tAx1;
		circleIntersection(R1+1,ca,radius1,R2+1,cb,radius2,point);

		vector_sub3(Ax,point,ca);
		
		cos1 = dDOT14(Ax,R1+0);
		cos3 = dDOT14(Ax,R1+2) ;
	
		tAx[0]=cos3*R1[0]-cos1*R1[2];
		tAx[1]=cos3*R1[4]-cos1*R1[6];
		tAx[2]=cos3*R1[8]-cos1*R1[10];

		vector_sub3(Ax,point,cb);
		
 		cos1 = dDOT14(Ax,R2+0);
		cos3 = dDOT14(Ax,R2+2) ;

		tAx1[0]=cos3*R2[0]-cos1*R2[2];
		tAx1[1]=cos3*R2[4]-cos1*R2[6];
		tAx1[2]=cos3*R2[8]-cos1*R2[10];
		dCROSS(Ax,=,tAx,tAx1);
	

		dNormalize3(Ax);
		dReal cyl1Pr,cyl2Pr;

		cos=dFabs(dDOT14(Ax,R1+1));
		cos1=dDOT14(Ax,R1+0);
		cos3=dDOT14(Ax,R1+2);
		sin=sqrtf(sqr(cos1)+sqr(cos3));
		cyl1Pr=cos*hlz1+sin*radius1;

		cos=dFabs(dDOT14(Ax,R2+1));
		cos1=dDOT14(Ax,R2+0);
		cos3=dDOT14(Ax,R2+2);
		sin=sqrtf(sqr(cos1)+sqr(cos3));
		cyl2Pr=cos*hlz2+sin*radius2;
		TEST(dDOT(p,Ax),cyl1Pr+cyl2Pr,Ax[0],Ax[1],Ax[2],5);

	}

#undef TEST

	// if we get to this point, the cylinders interpenetrate. compute the normal
	// in global coordinates.
	if(normalR) 
	{
		normal[0] = normalR[0];
		normal[1] = normalR[4];
		normal[2] = normalR[8];
	}
	else
		vector_copy3(normal,normalC);	
	if(invert_normal)
		vector_neg3(normal);
	
	*depth = -s;

	// compute contact point(s)

  	if(*code==3){
		for (i=0; i<3; i++) contact[0].pos[i] = pb[i];
		contact[0].depth = *depth;
		return 1;
				}

	if(*code==4){
		for (i=0; i<3; i++) contact[0].pos[i] = pa[i];
		contact[0].depth = *depth;
		return 1;
				}

	if(*code==5){
		for (i=0; i<3; i++) contact[0].pos[i] = point[i];
		contact[0].depth = *depth;
		return 1;
				}

	if (*code == 6) 
	{
	    dVector3 pa;
		dReal sign, cos1,cos3,factor;
		vector_copy3(pa,p1);

      	cos1 = dDOT14(normal,R1+0);
		cos3 = dDOT14(normal,R1+2) ;
		factor=sqrtf(sqr(cos1)+sqr(cos3));

		cos1/=factor;
		cos3/=factor;
	
		for (i=0; i<3; i++) pa[i] += cos1 * radius1 * R1[i*4];

		sign = (dDOT14(normal,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
		for (i=0; i<3; i++) pa[i] += sign * hlz1 * R1[i*4+1];

	    for (i=0; i<3; i++) pa[i] += cos3 * radius1 * R1[i*4+2];

		// find a point pb on the intersecting edge of box 2
		dVector3 pb;
		vector_copy3(pb,p2);
		
 		cos1 = dDOT14(normal,R2+0);
		cos3 = dDOT14(normal,R2+2) ;
		factor=sqrtf(sqr(cos1)+sqr(cos3));

		cos1/=factor;
		cos3/=factor;
		
		for (i=0; i<3; i++) pb[i] -= cos1 * radius2 * R2[i*4];

		sign = (dDOT14(normal,R2+1) > 0) ? REAL(1.0) : REAL(-1.0);
		for (i=0; i<3; i++) pb[i] -= sign * hlz2 * R2[i*4+1];

  
	    for (i=0; i<3; i++) pb[i] -= cos3 * radius2 * R2[i*4+2];

		dReal alpha,beta;
		dVector3 ua,ub;
		for (i=0; i<3; i++) ua[i] = R1[1 + i*4];
		for (i=0; i<3; i++) ub[i] = R2[1 + i*4];
		lineClosestApproach (pa,ua,pb,ub,&alpha,&beta);
		for (i=0; i<3; i++) pa[i] += ua[i]*alpha;
		for (i=0; i<3; i++) pb[i] += ub[i]*beta;

	    for (i=0; i<3; i++) contact[0].pos[i] = REAL(0.5)*(pa[i]+pb[i]);
		contact[0].depth = *depth;
		return 1;
	}

	// okay, we have a face-something intersection (because the separating
	// axis is perpendicular to a face).

	// @@@ temporary: make deepest point on the "other" cylinder the contact point.
  // @@@ this kind of works, but we need multiple contact points for stability,
  // @@@ especially for face-face contact.

	dVector3 vertex;
	if (*code == 0) 
	{
		// flat face from cylinder 1 touches a edge/face from cylinder 2.
		dReal sign,cos1,cos3,factor;
		vector_copy3(vertex,p2);
        cos1 = dDOT14(normal,R2+0) ;
		cos3 = dDOT14(normal,R2+2);
		factor=sqrtf(sqr(cos1)+sqr(cos3));

		cos1/=factor;
		cos3/=factor;
		for (i=0; i<3; i++) vertex[i] -= cos1 * radius2 * R2[i*4];

		sign = (dDOT14(normal,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
		for (i=0; i<3; i++) vertex[i] -= sign * hlz2 * R2[i*4+1];
   
	    for (i=0; i<3; i++) vertex[i] -= cos3 * radius2 * R2[i*4+2];
	}
	else 
	{
		 // flat face from cylinder 2 touches a edge/face from cylinder 1.
		dReal sign,cos1,cos3,factor;
		vector_copy3(vertex,p1);
		cos1 = dDOT14(normal,R1+0) ;
		cos3 = dDOT14(normal,R1+2);
		factor=sqrtf(sqr(cos1)+sqr(cos3));

		cos1/=factor;
		cos3/=factor;
		for (i=0; i<3; i++) vertex[i] += cos1 * radius1 * R1[i*4];

	    sign = (dDOT14(normal,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
		for (i=0; i<3; i++) vertex[i] += sign * hlz1 * R1[i*4+1];
   
		for (i=0; i<3; i++) vertex[i] += cos3 * radius1 * R1[i*4+2];
	}
	vector_copy3(contact[0].pos,vertex);
	contact[0].depth=*depth;
	return 1;
}

//****************************************************************************

int dCollideCylCyl (dGeomID o1,dGeomID o2, int flags,dContactGeom *contact, int skip)
{
	dIASSERT(skip>=(int)sizeof(dContactGeom));
  	dIASSERT(dGeomGetClass(o1)==dCylinderClassUser);
  	dIASSERT(dGeomGetClass(o2)==dCylinderClassUser);
	
	dVector3 normal;
	dReal depth;
	int code;

	dReal c1r,c2r,c1l,c2l;
	dGeomCylinderGetParams(o1,&c1r,&c1l);
	dGeomCylinderGetParams(o2,&c2r,&c2l);

	int num=dCylCyl(dGeomGetPosition(o1),dGeomGetRotation(o1),c1r,c1l,
			        dGeomGetPosition(o2),dGeomGetRotation(o2),c2r,c2l,
							 normal,&depth,&code,flags & NUMC_MASK,contact,skip);
	vector_neg3(normal);
	for (int i=0; i<num; i++) 
	{
		dContactGeom *c=CONTACT(contact,i*skip);
		vector_copy3(c->normal,normal);		
		c->g1=o1;
		c->g2=o2;		
	}
	return num;
}


int dCollideCylPlane(dxGeom *o1,dxGeom *o2,int flags,dContactGeom *contact,int skip)
{	
	dIASSERT(skip>=(int)sizeof(dContactGeom));
	dIASSERT(dGeomGetClass(o1)==dCylinderClassUser);
	dIASSERT(dGeomGetClass(o2)==dPlaneClass);	

	dReal radius;
	dReal hlz;
	dGeomCylinderGetParams(o1,&radius,&hlz);
	hlz*=0.5;
 
	const dReal* R=dGeomGetRotation(o1);	
	const dReal* p=dGeomGetPosition(o1);
	
	dVector4 n;		// normal vector
	dGeomPlaneGetParams(o2,n);	
	dReal cos1,sidePr;
	cos1=dFabs(dDOT14(n,R+2));
	if(cos1<dReal(1.0))
	{
		dReal sin1=sqrtf(REAL(1.)-sqr(cos1));
		sidePr=cos1*hlz+sin1*radius;
	}
	else
		sidePr=hlz;
//////////////////////////////	
	dReal dist=-n[3]+dDOT(n,p);
	dReal outDepth=sidePr-dist;
	if(outDepth<0.f) 
		return 0;
/////////////////////////////////////////// from geom.cpp dCollideBP
	dVector3 Q;
	dMULTIPLY0_133(Q,n,R);
	
	dReal factor=sqrtf(sqr(Q[0])+sqr(Q[1]));
	factor=factor?factor:1.f;
	dVector3 A={radius*Q[0]/factor,
				radius*Q[1]/factor,
				Q[2]>0?hlz:-hlz};

	dVector3 pos;
		
	dMULTIPLY0_331(pos,R,A);
	vector_sub3(pos,p,pos);

	vector_copy3(contact->pos,pos);	
	contact->depth = outDepth;
		
	int ret=1;
	int count=flags&NUMC_MASK;	
 	
	if(count>1)
	{
		dContactGeom* cptr=CONTACT(contact,skip);
		if(dFabs(Q[2])>M_SQRT1_2)
		{
			cptr->depth=outDepth-dFabs(Q[0]*(A[0]*=2.f));
			if(cptr->depth>0.f)
			{
				vector_add3Cmul(cptr->pos,pos,R,A[0]);
				ret++;
				cptr=CONTACT(cptr,skip);
			}
			
			if(count>2)
			{
				cptr->depth=outDepth-dFabs(Q[1]*(A[1]*=2.f));
				if(cptr->depth>0.f)
				{
					vector_add3Cmul(cptr->pos,pos,R+1,A[1]);
					ret++;			
				}		
			}
		}	
		else 
		{				
			cptr->depth=outDepth-dFabs(Q[2]*(A[2]*=2.f));
			if(cptr->depth>0.f) 
			{
				vector_add3Cmul(cptr->pos,pos,R+2,A[2]);
				ret++;
			}		
		}
	}
	
	for(int i=0;i<ret;i++) 
	{
		contact->g1=o1;
		contact->g2=o2;
		vector_copy3(contact->normal,n);
		contact=CONTACT(contact,skip);
	}
	return ret;
}


// Переписана, но не проверена!!!!
int dCollideCylRay(dxGeom *o1,dxGeom *o2,int flags,dContactGeom *contact,int skip) 
{
	dIASSERT(skip >= (int)sizeof(dContactGeom));
	dIASSERT(dGeomGetClass(o1)==dCylinderClassUser);
	dIASSERT(dGeomGetClass(o2)==dRayClass);
	contact->g1=o1;
	contact->g2=o2;
	
	dReal radius;
	dReal lz;
	dGeomCylinderGetParams(o1,&radius,&lz);
	dReal lz2=lz*REAL(0.5);
	const dReal *R=dGeomGetRotation(o1);
	const dReal *p=dGeomGetPosition(o1);

	dVector3 start,dir;
	dGeomRayGet(o2,start,dir); // position and orientation of the ray
	dReal length=dGeomRayGetLength(o2);
	// compute some useful info
	dVector3 cs,q,r;
	dReal C,k;
	vector_sub3(cs,start,p);
	k = dDOT41(R+2,cs);	// position of ray start along cyl axis (Y)	
	vector_mul3Csub(q,k,R+2,cs);	
	C=dDOT(q,q)-sqr(radius);
	// if C < 0 then ray start position within infinite extension of cylinder
	// if ray start position is inside the cylinder
	bool inside_cyl=C<0 && !(k<-lz2 || k>lz2);
	// compute ray collision with infinite cylinder, except for the case where
	// the ray is outside the cylinder but within the infinite cylinder
	// (it that case the ray can only hit endcaps)
	if(!inside_cyl && C<0)
	{
		// set k to cap position to check
		k=k<0?-lz2:lz2;
	}
	else 
	{
		dReal uv=dDOT41(R+2,dir);
		vector_mul3Csub(r,uv,R+2,dir);		
		dReal A=dDOT(r,r);
		dReal B=2*dDOT(q,r);
		k=sqr(B)-4*A*C;
		if(k<0) 
		{
			// the ray does not intersect the infinite cylinder, but if the ray is
			// inside and parallel to the cylinder axis it may intersect the end
			// caps. set k to cap position to check.
			if(!inside_cyl) 
				return 0;
			k=uv<0?-lz2:lz2;
		}
		else 
		{
			k=dSqrt(k);
			A=dRecip(2*A);
			dReal alpha=(-B-k)*A;
			if(alpha<0) 
			{
				alpha=(-B+k)*A;
				if(alpha<0) 
					return 0;
			}
			if(alpha>length)
				return 0;
			// the ray intersects the infinite cylinder. check to see if the
			// intersection point is between the caps
			vector_copy3(contact->pos,start);
			vector_add3mul(contact->pos,dir,alpha);
			vector_sub3(q,contact->pos,p);
			k=dDOT14(q,R+2);
			if(k>=-lz2 && k<=lz2) 
			{
				if(inside_cyl)
				{
					vector_mul3Csub(contact->normal,k,R+2,contact->pos);
					vector_add3(contact->normal,p);
				}
				else
				{
					vector_mul3Csub(contact->normal,-k,R+2,p);
					vector_add3(contact->normal,contact->pos);					
				}
				dNormalize3(contact->normal);
				contact->depth=alpha;
				return 1;
			}
			// the infinite cylinder intersection point is not between the caps.
			// set k to cap position to check.
			k=k<0?-lz2:lz2;
		}
	}
	// check for ray intersection with the caps. k must indicate the cap
	// position to check
	// perform a ray plan interesection
	// R+1 is the plan normal
	vector_mul3Csub(q,-k,R+2,p);
	vector_sub3(q,start,q);	
	dReal alpha=-dDOT14(q,R+2);
	dReal k2=dDOT14(dir,R+2);
	if (k2==0) 
		return 0; // ray parallel to the plane
	alpha/=k2;
	if (alpha<0 || alpha>length) 
		return 0; // too short
	vector_copy3(contact->pos,start);
	vector_add3mul(contact->pos,dir,alpha);		
	if(k<0)
	{
		contact->normal[0]=-R[0*4+2];
		contact->normal[1]=-R[1*4+2];
		contact->normal[2]=-R[2*4+2];
	}
	else
		vector_copy3C(contact->normal,R+2);
	contact->depth=alpha;
	return 1;
}


int dCollideCylCCyl(dGeomID o1,dGeomID o2, int flags,dContactGeom *contact, int skip)
{
	dIASSERT(skip>=sizeof(dContactGeom));
  	dIASSERT(dGeomGetClass(o1)==dCylinderClassUser);
  	dIASSERT(dGeomGetClass(o2)==dCCylinderClass);
	// присобачиваем к цилиндру пару сфер, полученную группу пересекаем (?)
	dGeomID gc;
	dReal ccl,ccr;
	dGeomCCylinderGetParams(o2,&ccr,&ccl);
	gc=dCreateCylinder(0,ccr,ccl);
	ccl*=dReal(0.5);
	const dReal* Pcc=dGeomGetPosition(o2);
	const dReal* Rcc=dGeomGetRotation(o2);
	dGeomSetPosition(gc,Pcc[0],Pcc[1],Pcc[2]);
	dGeomSetRotation(gc,Rcc);
	int num=dCollideCylCyl(o1,gc,flags,contact,skip);
	for(int i=0;i<num;i++)
		CONTACT(contact,i*skip)->g2=o2;
	dGeomDestroy(gc);
	gc=dCreateSphere(0,ccr);
	dGeomSetPosition(gc,Pcc[0]+Rcc[2]*ccl,Pcc[1]+Rcc[6]*ccl,Pcc[2]+Rcc[10]*ccl);
	num+=dCollideCylS(o1,gc,flags-num,CONTACT(contact,i*skip),skip);
	for(;i<num;i++)
		CONTACT(contact,i*skip)->g2=o2;
	dGeomSetPosition(gc,Pcc[0]-Rcc[2]*ccl,Pcc[1]-Rcc[6]*ccl,Pcc[2]-Rcc[10]*ccl);
	num+=dCollideCylS(o1,gc,flags-num,CONTACT(contact,i*skip),skip);
	for(;i<num;i++)
		CONTACT(contact,i*skip)->g2=o2;
	dGeomDestroy(gc);
	return num;	
}

static dColliderFn* dCylinderColliderFn (int num)
{
	// internal geom types
	switch(num)
	{
			case dBoxClass:			return dCollideCylB;	//!!
		case dSphereClass:		return dCollideCylS; 
		case dPlaneClass:		return dCollideCylPlane;
			case dRayClass:			return dCollideCylRay;	//!!
			case dCCylinderClass:	return dCollideCylCCyl;	//!!
	}
	// external types
	if(num==dCylinderClassUser)		//!!
		return dCollideCylCyl;
    return NULL;
}

static void dCylinderAABB(dxGeom *geom, dReal aabb[6])
{
	dReal radius,lz;

	const dReal* R	=dGeomGetRotation(geom);
	const dReal* pos=dGeomGetPosition(geom);

	dGeomCylinderGetParams(geom,&radius,&lz);

	lz*=0.5;	
	dReal xrange=dFabs(R[0]*radius)+dFabs(R[1]*radius)+dFabs(R[2]*lz);
	dReal yrange=dFabs(R[4]*radius)+dFabs(R[5]*radius)+dFabs(R[6]*lz);
	dReal zrange=dFabs(R[8]*radius)+dFabs(R[9]*radius)+dFabs(R[10]*lz);

	aabb[0]=pos[0]-xrange;
	aabb[1]=pos[0]+xrange;
	aabb[2]=pos[1]-yrange;
	aabb[3]=pos[1]+yrange;
	aabb[4]=pos[2]-zrange;
	aabb[5]=pos[2]+zrange;
}

dxGeom *dCreateCylinder(dSpaceID space,dReal r,dReal lz)
{	
	if(dCylinderClassUser == -1)
	{
		dGeomClass c;
		c.bytes		=sizeof(dxCylinder);
		c.collider	=&dCylinderColliderFn;
		c.aabb		=&dCylinderAABB;
		c.aabb_test	=0;
		c.dtor		=0;

		dCylinderClassUser=dCreateGeomClass (&c);
	}
	dGeomID g = dCreateGeom (dCylinderClassUser);
	if(space) 
		dSpaceAdd (space,g);
	dGeomCylinderSetParams(g,r,lz);	
	return g;
}

void dGeomCylinderSetParams(dGeomID g,dReal radius,dReal length)
{
	dUASSERT (g && dGeomGetClass(g) == dCylinderClassUser,"argument not a cylinder");
	dAASSERT (radius>0 && length>0);
	dxCylinder *c = (dxCylinder*) dGeomGetClassData(g);
	c->radius=radius;
	c->lz=length;
}

void dGeomCylinderGetParams(dGeomID g, dReal *radius, dReal *length)
{
	dUASSERT (g && dGeomGetClass(g) == dCylinderClassUser ,"argument not a cylinder");
	dxCylinder *c = (dxCylinder*) dGeomGetClassData(g);
	*radius=c->radius;
	*length=c->lz;
}
