/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

/*

buggy with suspension.
this also shows you how to use geom groups.

*/


#include <ode/ode.h>
#include <ode/addons/advShapes.h>
#include <drawstuff/draw2.h>

#ifdef MSVC
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

// It's my !!!!!!!!!!!!!!!!!

#pragma comment(lib,"Comctl32.lib")
#pragma comment(lib,"kernel32.lib") 
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

// select correct drawing functions

// some constants

#define LENGTH 0.7	// chassis length
#define WIDTH 0.5	// chassis width
#define WHEEL_W 0.1
#define HEIGHT 0.2	// chassis height
#define RADIUS 0.18	// wheel radius
#define STARTZ 0.5	// starting height of chassis
#define CMASS 100	// chassis mass
#define WMASS 0.1	// wheel mass


#define GRAVITY 10
#define STEP_IN_COUNT	10

// dynamics and collision objects (chassis, 3 wheels, environment)

static dWorldID world;
static dSpaceID space,geom_group;
static dBodyID body[5],torus;

static dJointID joint[3];	// joint[0] is the front wheel
static dJointGroupID contactgroup;
static dGeomID ground;
static dGeomID box;
static dGeomID sphere[5];
static dGeomID ground_box;


// things that the user controls

static dReal speed=0,steer=0;	// user commands



// this is called by dSpaceCollide when two objects in space are
// potentially colliding.

void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
  int i,n;
  
	int g1=(o1==box || o1==sphere[0] || o1==sphere[1] || o1==sphere[2] || o1==sphere[3]);
	int g2=(o2==box || o2==sphere[0] || o2==sphere[1] || o2==sphere[2] || o2==sphere[3]);
	if(g1 && g2)
		return;

//  int g1 = (o1 == ground || o1 == ground_box);
//  int g2 = (o2 == ground || o2 == ground_box);
//  if (!(g1 ^ g2)) return;  

  const int N = 10;
  dContact contact[N];

  n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));  
  if (n > 0) {
    for (i=0; i<n; i++) {
      contact[i].surface.mode = dContactSlip1 | dContactSlip2 |
	dContactSoftERP | dContactSoftCFM  | dContactApprox1;

	  dMass mass;
	  dReal m=0;
	  if(dGeomGetBody(o1))
	  {
		dBodyGetMass(dGeomGetBody(o1),&mass);
		m=mass.mass;
	  }
	  if(dGeomGetBody(o2))
	  {
		  dBodyGetMass(dGeomGetBody(o2),&mass);
		  if(m<mass.mass)
			  m=mass.mass;
	  }	  
		
      contact[i].surface.mu = dInfinity;
      contact[i].surface.slip1 = 0.001;
      contact[i].surface.slip2 = 0.001;
      contact[i].surface.soft_erp = 0.1;//CMASS;//m>1?m>100?10:sqrt(m):1.0;   //def 0.9
      contact[i].surface.soft_cfm = 0.1/CMASS;//m>100?1/sqr(m):0.01; //def:0.3
      dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
      dJointAttach (c,
		    dGeomGetBody(contact[i].geom.g1),
		    dGeomGetBody(contact[i].geom.g2));
    }
  }
}


// start simulation - set viewpoint

static void start()
{
  static float xyz[3] = {0.8317f,-0.9817f,0.8000f};
  static float hpr[3] = {121.0000f,-27.5000f,0.0000f};
  dsSetViewpoint (xyz,hpr);
  printf ("Press:\t'a' to increase speed.\n"
	  "\t'z' to decrease speed.\n"
	  "\t',' to steer left.\n"
	  "\t'.' to steer right.\n"
	  "\t' ' to reset speed and steering.\n");
}


// called when a key pressed

dGeomID upor=NULL;
dGeomID mesh,MegaMesh;

static void command (int cmd)
{
  switch (cmd) {
  case 'a': case 'A':
    speed += 0.3;
    break;
  case 'z': case 'Z':
    speed -= 0.3;
    break;
  case ',':
    steer -= 0.5;
    break;
  case '.':
    steer += 0.5;
    break;
  case ' ':
    speed = 0;
    steer = 0;
    break;
	case '1':
		dGeomSetPosition(box,0,0,8);
		break;
	case '2':
		if(upor)
		{
			dGeomTransformGroupRemoveGeom(sphere[0],upor);
			upor=NULL;
		}
		else
		{			
			dVector3 pos;
			dMatrix3 R;
			dGeomTransformGroupGetRootPosition(sphere[0],pos,R);
			if(pos[2]<0)	// меняем локальный центр:
			{
				dGeomTransformGroupGetLocalRootPosition(sphere[0],pos,R);
				vector_neg3(pos);
				dGeomTransformGroupSetLocalRootPosition(sphere[0],pos,R);
			}
			dQuaternion q;
			dQFromAxisAndAngle(q,1,0,0,M_PI*0.5);
			dQtoR(q,R);
			upor=dCreateCylinder(NULL,RADIUS/5,WHEEL_W);
			dGeomSetRotation(upor,R);
			dGeomTransformGroupAddGeom(sphere[0],upor,true);
		}
  }
}


// simulation loop

static void simLoop (int pause)
{
  int i;
  if (!pause) {
    // motor
    dJointSetHinge2Param (joint[0],dParamVel2,-speed);
    dJointSetHinge2Param (joint[0],dParamFMax2,CMASS*GRAVITY);

    // steering
    dReal v = steer - dJointGetHinge2Angle1 (joint[0]);
    if (v > 0.1) v = 0.1;
    if (v < -0.1) v = -0.1;
    v *= 10.0;
    dJointSetHinge2Param (joint[0],dParamVel,v);
    dJointSetHinge2Param (joint[0],dParamFMax,CMASS*GRAVITY);
    dJointSetHinge2Param (joint[0],dParamLoStop,-0.75);
    dJointSetHinge2Param (joint[0],dParamHiStop,0.75);
    //dJointSetHinge2Param (joint[0],dParamFudgeFactor,100);

	for(int i=0;i<STEP_IN_COUNT;i++)
	{
		dSpaceCollide (space,0,&nearCallback);
		dWorldStep (world,0.01/STEP_IN_COUNT);

    // remove all contact joints
		dJointGroupEmpty (contactgroup);	
	}
  }

  dsSetColor (0,1,1);
  dsSetTexture (DS_WOOD);  
  dsDrawBody(body[0]);
  for(i=1;i<5; i++)
  {
	  dsSetColor(1,1,1);
	  dsDrawBody(body[i]);	    
  }
  dsSetColor(1,1,1);
  dsDrawGeom(ground_box);
	dsSetColor(0,1,0);
	/*dVector3 pos;
	dMatrix3 R;
	dGeomTransformGroupGetRootPosition(sphere[0],pos,R);
	dGeomSetPosition(mesh,pos[0],pos[1],pos[2]);
	dGeomSetRotation(mesh,R);*/
	dsDrawGeom(mesh);
	dsSetColor(0,1,1);
	dsDrawGeom(MegaMesh);

  /*
  printf ("%.10f %.10f %.10f %.10f\n",
	  dJointGetHingeAngle (joint[1]),
	  dJointGetHingeAngle (joint[2]),
	  dJointGetHingeAngleRate (joint[1]),
	  dJointGetHingeAngleRate (joint[2]));
  */
}


dGeomID CreateTorus(dSpaceID space,dReal r1,dReal r2,int N1,int N2,bool backMode=false)
{
	// Проба меша
	dGeomID geom=dCreateMesh(space);
	// 1. Create vertices
	dVector3* vertex=new dVector3[N1*N2];
	for(int i=0,k=0;i<N1;i++)
	{
		float phi=i*2*M_PI/N1;
		for(int j = 0; j < N2; j++, k++ )
		{
			float	psi = j * 2.0 * M_PI / N2;

			vertex[k][0]=(r1+r2*cos(psi))*cos(phi);
			vertex[k][1]=(r1+r2*cos(psi))*sin(phi);
			vertex[k][2]=r2*sin(psi);
		}
	}	
	//dGeomMeshLoad(mesh,m_vertex,4,triangles,4);	
	// 2. Create facets
	int* triangles=new int[N1*N2*6];
	for(i=k=0;i<N1;i++)
		for(int j=0;j<N2;j++,k+=2)
			if(backMode)
			{
				triangles[k*3+1]=i*N2 + j;
				triangles[k*3+0] = ((i+1)%N1)*N2 + j;
				triangles[k*3+2] = ((i+1)%N1)*N2 + (j + 1) % N2;
	
				triangles[k*3+3+1]=i*N2 + j;
				triangles[k*3+3+0]=((i+1)%N1)*N2 + (j + 1)%N2;
				triangles[k*3+3+2]=i*N2 + (j + 1)%N2;
			}
			else
			{
				triangles[k*3+0]=i*N2 + j;
				triangles[k*3+1] = ((i+1)%N1)*N2 + j;
				triangles[k*3+2] = ((i+1)%N1)*N2 + (j + 1) % N2;
	
				triangles[k*3+3+0]=i*N2 + j;
				triangles[k*3+3+1]=((i+1)%N1)*N2 + (j + 1)%N2;
				triangles[k*3+3+2]=i*N2 + (j + 1)%N2;
			}	 
	dGeomMeshLoad(geom,vertex,N1*N2,triangles,N1*N2*2);
	delete triangles;
	delete vertex;
	return geom;
}

dGeomID CreateMegaMesh(dSpaceID space,dReal step1,dReal step2,int N1,int N2)
{
	// Проба меша
	dGeomID geom=dCreateMesh(space);
	// 1. Create vertices
	dVector3* vertex=new dVector3[N1*N2];	
	for(int i=0,k=0;i<N1;i++)
	{		
		for(int j=0;j<N2;j++,k++)
		{			
			vertex[k][0]=step1*i;
			vertex[k][1]=step2*j;
			vertex[k][2]=i?vertex[k-N2][2]+dReal(rand()%100)*0.001-0.05:dReal(rand()%100)*0.001;
		}
	}		
	// 2. Create facets
	int* triangles=new int[(N1-1)*(N2-1)*6];
	for(i=k=0;i<N1-1;i++)
		for(int j=0;j<N2-1;j++,k+=2)
		{
			triangles[k*3+0]=i*N2+j;
			triangles[k*3+1]=(i+1)*N2+j+1;
			triangles[k*3+2]=i*N2+j+1;

			triangles[k*3+3+0]=i*N2+j;
			triangles[k*3+3+1]=(i+1)*N2+j;
			triangles[k*3+3+2]=(i+1)*N2+j+1;
		}	
	dGeomMeshLoad(geom,vertex,N1*N2,triangles,(N1-1)*(N2-1)*2);
	delete triangles;
	delete vertex;
	return geom;
}

int main (int argc, char **argv)
{
  int i;
  dMass m;

  // setup pointers to drawstuff callback functions
  dsFunctions fn;
  fn.version = DS_VERSION;
  fn.start = &start;
  fn.step = &simLoop;
  fn.command = &command;
  fn.stop = 0;
  fn.path_to_textures = "textures";

  // create world

  world=dWorldCreate();
  space=dSimpleSpaceCreate (0);

  contactgroup=dJointGroupCreate (0);
  dWorldSetGravity (world,0,0,-GRAVITY);
  ground=dCreatePlane (space,0,0,1,0);

	//mesh=CreateTorus(space,8,1.8,160,10);
	//dGeomSetPosition(mesh,0,-8,2.1+8);	
	MegaMesh=CreateMegaMesh(space,0.2,0.2,16,16);
	dGeomSetPosition(MegaMesh,2,0,0.1);	

  // chassis body
  body[0]=dBodyCreate(world);
  dBodySetPosition(body[0],0,0,STARTZ);
  dMassSetBox(&m,1,LENGTH,WIDTH,HEIGHT);
  dMassAdjust(&m,CMASS);
  torus=dBodyCreate(world);
  dBodySetMass(body[0],&m); 
  box=dCreateBox(0,LENGTH,WIDTH,HEIGHT);
  dGeomSetBody(box,body[0]);
  	//dMassAdjust(&m,WMASS);
	//dBodySetMass(torus,&m);
	//dGeomSetBody(mesh,torus);
	//dBodySetPosition(torus,1,1,5);
		

  // wheel bodies
  for (i=1; i<5; i++) {
    body[i] = dBodyCreate (world);
    dQuaternion q;
    dQFromAxisAndAngle (q,1,0,0,M_PI*0.5);
    dBodySetQuaternion (body[i],q);
    dMassSetSphere (&m,1,RADIUS);
    dMassAdjust (&m,WMASS);
    dBodySetMass (body[i],&m);             
	if(i>3)
	{
		/*sphere[i-1]=dCreateGeomTransformGroup(space);
		dGeomID t=dCreateCylinder(NULL,RADIUS,WHEEL_W);
		dGeomSetPosition(t,0,0,-WHEEL_W);
		dGeomTransformGroupAddGeom(sphere[i-1],t);
		t=dCreateCylinder(NULL,RADIUS,WHEEL_W);
		dGeomSetPosition(t,0,0,WHEEL_W);
		dGeomTransformGroupAddGeom(sphere[i-1],t);*/
		/*t=dCreateCylinder(NULL,WHEEL_W,WHEEL_W);
		dGeomSetPosition(t,0,0,WHEEL_W);
		dGeomTransformGroupAddGeom(sphere[i-1],t);*/		
		sphere[i-1]=dCreateSphere(0,RADIUS);
	}
	else
		if(i==1) // forward wheel
		{
			sphere[i-1]=dCreateGeomTransformGroup(NULL);
			// add 2 wheel
			dGeomID t=dCreateCylinder(NULL,RADIUS,WHEEL_W);
			dGeomSetPosition(t,0,0,-WHEEL_W);
			dGeomTransformGroupAddGeom(sphere[i-1],t);
			t=dCreateCylinder(NULL,RADIUS,WHEEL_W);
			dGeomSetPosition(t,0,0,WHEEL_W);
			dGeomTransformGroupAddGeom(sphere[i-1],t);
			t=dCreateCCylinder(NULL,RADIUS/2,4*WHEEL_W);
			dGeomSetPosition(t,0,0,0);
			dGeomTransformGroupAddGeom(sphere[i-1],t);
			dVector3 pos={-1,-1,0,0};
			dMatrix3 R={1,0,0,0,0,1,0,0,0,0,1,0};
			dGeomTransformGroupSetLocalRootPosition(sphere[i-1],pos,R);			
		}
		else		
			sphere[i-1]=dCreateCylinder(NULL,RADIUS,WHEEL_W);
    dGeomSetBody (sphere[i-1],body[i]);
  }
  dBodySetPosition (body[1],0.5*LENGTH+RADIUS*2,0,STARTZ-HEIGHT*0.5);
  dBodySetPosition (body[2],-0.5*LENGTH, WIDTH*0.5+WHEEL_W,STARTZ-HEIGHT*0.5);
  dBodySetPosition (body[3],-0.5*LENGTH,-WIDTH*0.5-WHEEL_W,STARTZ-HEIGHT*0.5);

  dBodySetPosition (body[4],0,0,2);
  /*dBodySetPosition (body[5],1,0,5);
  dBodySetPosition (body[6],0,1,5);
  dBodySetPosition (body[7],2,0,5);*/

  // front wheel hinge
  /*
  joint[0] = dJointCreateHinge2 (world,0);
  dJointAttach (joint[0],body[0],body[1]);
  const dReal *a = dBodyGetPosition (body[1]);
  dJointSetHinge2Anchor (joint[0],a[0],a[1],a[2]);
  dJointSetHinge2Axis1 (joint[0],0,0,1);
  dJointSetHinge2Axis2 (joint[0],0,1,0);
  */

  // front and back wheel hinges
  for (i=0; i<3; i++) {
    joint[i] = dJointCreateHinge2 (world,0);
    dJointAttach (joint[i],body[0],body[i+1]);
    const dReal *a = dBodyGetPosition (body[i+1]);
    dJointSetHinge2Anchor (joint[i],a[0],a[1],a[2]);
    dJointSetHinge2Axis1 (joint[i],0,0,1);
    dJointSetHinge2Axis2 (joint[i],0,1,0);
  }

  // set joint suspension
  for (i=0; i<3; i++) {
    dJointSetHinge2Param (joint[i],dParamSuspensionERP,0.5);
    dJointSetHinge2Param (joint[i],dParamSuspensionCFM,0.0001);
    //dJointSetHinge2Param (joint[i],dParamFudgeFactor,0.1);
  }

  // lock back wheels along the steering axis
  for (i=1; i<3; i++) {
    // set stops to make sure wheels always stay in alignment
    dJointSetHinge2Param (joint[i],dParamLoStop,0);
    dJointSetHinge2Param (joint[i],dParamHiStop,0);
    // the following alternative method is no good as the wheels may get out
    // of alignment:
    //   dJointSetHinge2Param (joint[i],dParamVel,0);
    //   dJointSetHinge2Param (joint[i],dParamFMax,dInfinity);
  }

  // create geometry group and add it to the space
  geom_group = dSimpleSpaceCreate(space);
  dSpaceAdd(geom_group,sphere[0]);
  dSpaceAdd(geom_group,sphere[1]);
  dSpaceAdd(geom_group,sphere[2]);
  dSpaceAdd(geom_group,box);

  dSpaceAdd(space,sphere[3]);

  // environment
  ground_box = dCreateBox (space,2,1.5,1);
  dMatrix3 R;
  dRFromAxisAndAngle (R,0,1,0,-0.15);
  dGeomSetPosition (ground_box,2,0,-0.34);
  dGeomSetRotation (ground_box,R);
	  

  // run simulation
  dsSimulationLoop (argc,argv,352,288,&fn);

  dJointGroupDestroy (contactgroup);
  dSpaceDestroy (space);
  dWorldDestroy (world);

  return 0;
}
