
/* ************************************************************************ */
/* 
   grouped and transformed geometry functions 
   author: Tim Schmidt tisch@uni-paderborn.de
*/

#include <pch.h>

//#define SHARED_GEOM_H_INCLUDED_FROM_DEFINING_FILE 1
//#include <ode/common.h>
//#include <ode/geom.h>
//#include <ode/rotation.h>
//#include <ode/odemath.h>
//#include <ode/memory.h>
//#include <ode/misc.h>
//#include <ode/objects.h>
//#include <ode/matrix.h>
#include <ode/addons/GeomTransformGroup.h>
#include <ode/addons/misc.h>
#include "q:\src\ode\array.h"
#include "q:\src\ode\collision_kernel.h"

// ############################################################################

int dGeomTransformGroupClass=-1;
// ############################################################################

struct dxNodeGeomTransformGroup
{
	// cached final object transform (body tx + relative tx). this is set by
	// computeAABB(), and it is valid while the AABB is valid.
	dVector3	final_pos;
	dMatrix3	final_R;
	dGeomID		geom;	
};

struct dxGeomTransformGroup 
{		
	dArray<dxNodeGeomTransformGroup> parts;
	bool ClearGeoms;			// cleanup mode, 1=destroy geoms on exit
	bool Infomode;			// 1 to put Tx geom in dContactGeom g1
	dVector3 local_Z_pos;
	dMatrix3 local_Z_R;
};
// ############################################################################

static void computeFinalTransformation(dGeomID tg,dxNodeGeomTransformGroup* part)
{
	const dReal* R=dGeomGetRotation(tg);
	dMULTIPLY0_331(part->final_pos,R,dGeomGetPosition(part->geom));
	vector_add3(part->final_pos,dGeomGetPosition(tg));
	dMULTIPLY0_333(part->final_R,R,dGeomGetRotation(part->geom));
}
// ############################################################################

int dCollideTransformGroup(dGeomID o1,dGeomID o2, int flags,dContactGeom *contact, int skip)
{
	dUASSERT(o1 &&  dGeomGetClass(o1)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup = (dxGeomTransformGroup*)dGeomGetClassData(o1);
	if(transformGroup->parts.size()==0)
		return 0;
	int numleft=flags & NUMC_MASK;
	if(numleft==0) 
		numleft=1;
	flags&=~NUMC_MASK;
	int num=0;

	if(o1->gflags & GEOM_AABB_BAD)	// recalc abs coords
	{
		for(int i=0;i<transformGroup->parts.size();i++)
			computeFinalTransformation(o1,&transformGroup->parts[i]);
	}
	for(int i=0;i<transformGroup->parts.size() && numleft>0;i++) 
	{
		dxNodeGeomTransformGroup& node=transformGroup->parts[i];
		dUASSERT(dGeomGetSpace(node.geom)==0,"GeomTransformGroup encapsulated object must not be in a space");
		dUASSERT(dGeomGetBody(node.geom)==0,"GeomTransformGroup encapsulated object must not be attached to a body");
		// backup the relative pos and R pointers of the encapsulated geom object,
		// and the body pointer
		dReal *posbak =node.geom->pos;
		dReal *Rbak	  =node.geom->R;

		node.geom->pos=node.final_pos;
		node.geom->R  =node.final_R;
		node.geom->body=dGeomGetBody(o1);

		// do the collision
		int n=dCollide(node.geom,o2,flags,contact,skip);
		// if required, adjust the 'g1' values in the generated contacts so that
		// thay indicated the GeomTransform object instead of the encapsulated
		// object.
		if(!transformGroup->Infomode)
		{
			for(int i=0; i<n; i++) 
				CONTACT(contact,skip*i)->g1=o1;      
	    }  
		node.geom->body=NULL;
		node.geom->R=Rbak;
		node.geom->pos=posbak;
	    contact=CONTACT (contact,skip*n);
		numleft-=n;
		num+=n;    
	}
	return num;
}
// ############################################################################

static dColliderFn *dGeomTransformGroupColliderFn (int num)
{
	return dCollideTransformGroup;
}
// ############################################################################

static void dGeomTransformGroupAABB(dGeomID g,dReal aabb[6])
{
	dUASSERT(g &&  dGeomGetClass(g)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);
	int pcount=transformGroup->parts.size();
	if(pcount==0) 
	{
		dSetZero(aabb,6);
		return;
	}
	aabb[0]=dInfinity;
	aabb[1]=-dInfinity;
	aabb[2]=dInfinity;
	aabb[3]=-dInfinity;
	aabb[4]=dInfinity;
	aabb[5]=-dInfinity;
	for(int i=0;i<pcount;i++)
	{
		dxNodeGeomTransformGroup* node=&transformGroup->parts[i];
		computeFinalTransformation(g,node);
		// backup the relative pos and R pointers of the encapsulated geom object
		dReal *posbak=node->geom->pos;
		dReal *Rbak  =node->geom->R;
	  // compute temporary pos and R for the encapsulated geom object
		node->geom->pos=node->final_pos;
		node->geom->R  =node->final_R;
	  // compute the AABB
		node->geom->computeAABB();
	  // restore the pos and R
		node->geom->pos=posbak;
		node->geom->R  =Rbak;

		for(int j=0;j<6;j+=2) 
			if(node->geom->aabb[j]<aabb[j]) 
				aabb[j]=node->geom->aabb[j];
		for(j=1;j<6;j+=2) 
			if(node->geom->aabb[j]>aabb[j]) 
				aabb[j]=node->geom->aabb[j];
	}
}

// ############################################################################
static void dGeomTransformGroupDtor(dGeomID g)
{
	dUASSERT(g &&  dGeomGetClass(g)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);
	if(transformGroup->ClearGeoms)
	{
		for(int i=0;i<transformGroup->parts.size();i++)
		{
			dGeomDestroy(transformGroup->parts[i].geom);			
		}
	}
	transformGroup->parts.~dArray();
}

// ############################################################################
dGeomID dCreateGeomTransformGroup(dSpaceID space)
{
	if(dGeomTransformGroupClass == -1) 
	{
		dGeomClass c;
		c.bytes	   =sizeof (dxGeomTransformGroup);
		c.collider =&dGeomTransformGroupColliderFn;
		c.aabb	   =&dGeomTransformGroupAABB;
		c.aabb_test=0;
		c.dtor	   =dGeomTransformGroupDtor;
		dGeomTransformGroupClass=dCreateGeomClass (&c);
	}
	dGeomID g=dCreateGeom(dGeomTransformGroupClass);
	if(space)
		dSpaceAdd (space,g);
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);
	transformGroup->ClearGeoms=true;
	transformGroup->Infomode=false;
	transformGroup->parts.constructor();
	dSetZero(transformGroup->local_Z_pos,4);
	dSetZero(transformGroup->local_Z_R,12);
	transformGroup->local_Z_R[0]=
	transformGroup->local_Z_R[5]=
	transformGroup->local_Z_R[10]=1.0;
	return g;
}


void dGeomTransformGroupSetCleanup(dGeomID g,bool mode)
{
	dUASSERT(g &&  dGeomGetClass(g)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);
	transformGroup->ClearGeoms=mode;
}

bool dGeomTransformGroupGetCleanup(dGeomID g)
{
	dUASSERT(g &&  dGeomGetClass(g)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);
	return transformGroup->ClearGeoms;
}

// ############################################################################
void dGeomTransformGroupAddGeom(dGeomID g,dGeomID obj,bool rootZero)
{
	dUASSERT(g &&  dGeomGetClass(g)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);

	if(rootZero)
	{
		dVector3 pos;
		dMatrix3 R;
		dMULTIPLY0_331(pos,transformGroup->local_Z_R,dGeomGetPosition(obj));
		vector_add3(pos,transformGroup->local_Z_pos);
		dMULTIPLY0_333(R,transformGroup->local_Z_R,dGeomGetRotation(obj));
		dGeomSetPosition(obj,pos[0],pos[1],pos[2]);
		dGeomSetRotation(obj,R);
	}

	dxNodeGeomTransformGroup nobj;
	nobj.geom=obj;
	computeFinalTransformation(g,&nobj);
	transformGroup->parts.push(nobj);
}
// ############################################################################

void dGeomTransformGroupRemoveGeom(dxGeom *g, dGeomID obj)
{
	dUASSERT (g && dGeomGetClass(g)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);
	for(int i=0;i<transformGroup->parts.size(); i++) 
	{
		if(transformGroup->parts[i].geom==obj) 
		{
			transformGroup->parts.remove(i);
			if(transformGroup->ClearGeoms)
				dGeomDestroy(obj);
			return;
		}
	}
}
// ############################################################################

dGeomID dGeomTransformGroupGetGeom(dGeomID g, int i)
{
	dUASSERT(g && dGeomGetClass(g)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);
	dAASSERT(i>=0 && i<transformGroup->parts.size());
	return transformGroup->parts[i].geom;
}
// ############################################################################

int dGeomTransformGroupGetNumGeoms(dGeomID g)
{
	dUASSERT (g && dGeomGetClass(g)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);
	return transformGroup->parts.size();
}

void dGeomTransformGroupGetGeomPosition(dGeomID g,int index,dReal* pos,dReal* R)
{
	dGeomID ngeom=dGeomTransformGroupGetGeom(g,index);
	const dReal* Rg=dGeomGetRotation(g);
	dMULTIPLY0_331(pos,Rg,dGeomGetPosition(ngeom));
	vector_add3(pos,dGeomGetPosition(g));
	dMULTIPLY0_333(R,Rg,dGeomGetRotation(ngeom));
}

void dGeomTransformGroupGetRootPosition(dGeomID g,dReal* pos,dReal* R)
{
	dUASSERT (g && dGeomGetClass(g)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);
	const dReal* Rg=dGeomGetRotation(g);
	dMULTIPLY0_331(pos,Rg,transformGroup->local_Z_pos);
	vector_add3(pos,dGeomGetPosition(g));
	dMULTIPLY0_333(R,Rg,transformGroup->local_Z_R);
}


// !! не проверено!!
void dGeomTransformGroupSetRootPosition(dGeomID g,const dReal* pos,const dReal* R)
{
	dUASSERT (g && dGeomGetClass(g)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);
	dVector3 new_pos;
	dMatrix3 new_R;

	dMULTIPLY1_333(new_R,R,transformGroup->local_Z_R);
	dMULTIPLY0_331(new_pos,new_R,transformGroup->local_Z_pos);	

	vector_sub3(new_pos,pos,new_pos);
	dGeomSetPosition(g,new_pos[0],new_pos[1],new_pos[2]);
	dGeomSetRotation(g,new_R);	
}

void dGeomTransformGroupSetLocalRootPosition(dGeomID g,const dReal* pos,const dReal* R)
{
	dUASSERT (g && dGeomGetClass(g)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);
	memcpy(transformGroup->local_Z_pos,pos,sizeof(dVector3));
	memcpy(transformGroup->local_Z_R,R,sizeof(dMatrix3));
}

void dGeomTransformGroupGetLocalRootPosition(dGeomID g,dReal* pos,dReal* R)
{
	dUASSERT (g && dGeomGetClass(g)==dGeomTransformGroupClass,"argument not a geom TransformGroup");
	dxGeomTransformGroup *transformGroup=(dxGeomTransformGroup*)dGeomGetClassData(g);
	memcpy(pos,transformGroup->local_Z_pos,sizeof(dVector3));
	memcpy(R,transformGroup->local_Z_R,sizeof(dMatrix3));
}