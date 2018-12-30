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
#include <drawstuff/drawstuff.h>

#include "PhCTest\BCPhisicalScene.h"

#ifdef MSVC
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

// It's my !!!!!!!!!!!!!!!!!

#pragma comment(lib,"drawstuff.lib")
#pragma comment(lib,"Comctl32.lib")
#pragma comment(lib,"kernel32.lib") 
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"Glu32.lib")


// some constants

#define LENGTH 0.7	// chassis length
#define WIDTH 0.5	// chassis width
#define WHEEL_WIDTH 0.1
#define HEIGHT 0.2	// chassis height
#define RADIUS 0.18	// wheel radius
#define STARTZ 5	// starting height of chassis
#define CMASS 1		// chassis mass
#define WMASS 0.2	// wheel mass


// dynamics and collision objects (chassis, 3 wheels, environment)

// things that the user controls

static dReal speed=0,steer=0;	// user commands

static BCPhisicalScene* Scene=NULL;

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
  }
}


// simulation loop

static int MotorJoint=-1;
static BCPhisComponentObject* Vehicle=NULL;

static void simLoop (int pause)
{
  	if (!pause) 
	{
	    // motor
	    	Vehicle->SetHinge2Param(MotorJoint,dParamVel2,-speed);
	    	Vehicle->SetHinge2Param(MotorJoint,dParamFMax2,0.1);
	    // steering
	    	dReal v = steer - Vehicle->GetHinge2Angle1(MotorJoint);
	    	if (v > 0.1) v = 0.1;
	    	if (v < -0.1) v = -0.1;
	    	v *= 10.0;
	    	Vehicle->SetHinge2Param(MotorJoint,dParamVel,v);
	    	Vehicle->SetHinge2Param(MotorJoint,dParamFMax,0.2);
	    	Vehicle->SetHinge2Param(MotorJoint,dParamLoStop,-0.75);
	    	Vehicle->SetHinge2Param(MotorJoint,dParamHiStop,0.75);
	    	Vehicle->SetHinge2Param(MotorJoint,dParamFudgeFactor,0.1);
		Scene->WorldStep(0.05);
  	}
	Scene->Draw();
}


int main (int argc, char **argv)
{
	dMatrix3 R;
	// setup pointers to drawstuff callback functions
	dsFunctions fn;
	fn.version = DS_VERSION;
	fn.start = &start;
	fn.step = &simLoop;
	fn.command = &command;
	fn.stop = 0;
	fn.path_to_textures = "textures";
  // create world
        Scene=new BCPhisicalScene();
	Scene->SetGravity(0,0,-0.8);
	Scene->AddGeom(dCreatePlane(0,0,0,1,0));
  // environment

	dGeomID geom;
	Scene->AddGeom(geom=dCreateBox(0,2,1.5,1));
	dRFromAxisAndAngle(R,0,1,0,-0.15);
  	dGeomSetPosition(geom,2,0,-0.34);
  	dGeomSetRotation(geom,R);
	Scene->AddGeom(geom=dCreateBox(0,2,1.5,1));
  	dRFromAxisAndAngle(R,0,1,0,0.15);
  	dGeomSetPosition(geom,12,0,-0.34);
  	dGeomSetRotation(geom,R);
#define FACE_ROOT_COUNT	16

  	for(int i=0;i<FACE_ROOT_COUNT;i++)
  	{
		dReal angle=dReal(2*i)*M_PI/dReal(FACE_ROOT_COUNT),
	      	      angle2=dReal(2*(i-1))*M_PI/dReal(FACE_ROOT_COUNT),
	      	      angle1x=dReal(2*i-1)*M_PI/dReal(FACE_ROOT_COUNT),
	      	      angle2x=dReal(2*i+1)*M_PI/dReal(FACE_ROOT_COUNT);
	dVector3 p1={11*sin(angle),-4+11*cos(angle),1},
		 p2={11*sin(angle2),-4+11*cos(angle2),1},
		 p3={0,-4,0.8};
	Scene->AddGeom(dCreateFace(0,p1,p2,p3));
	Scene->AddGeom(dCreateEdge(0,p1,p2));
	Scene->AddGeom(dCreateEdge(0,p1,p3));
	p3[0]=12*sin(angle1x);
	p3[1]=-4+12*cos(angle1x);
	p3[2]=3;
	Scene->AddGeom(dCreateFace(0,p1,p3,p2));
	Scene->AddGeom(dCreateEdge(0,p1,p3));
	Scene->AddGeom(dCreateEdge(0,p2,p3));
	p2[0]=12*sin(angle2x);
	p2[1]=-4+12*cos(angle2x);
	p2[2]=3;
	Scene->AddGeom(dCreateFace(0,p1,p2,p3));
	Scene->AddGeom(dCreateEdge(0,p2,p3));
  	}


  // chassis body
	BCPhisComponentObject* co=new BCPhisComponentObject(Scene);
		BCScenePhisObject* po=new BCScenePhisObject(co);
			po->AddGBox(LENGTH,WIDTH,HEIGHT);
			po->AddGSphere(HEIGHT,0,0,HEIGHT,R);
			po->SetMass(MassSetBox(1.0,LENGTH,WIDTH,HEIGHT));
			po->SetMass(CMASS);
			po->SetPosition(0,0,STARTZ);
		// wheel bodies
  		for(i=0;i<3;i++) 
		{
			po=new BCScenePhisObject(co);
			po->AddGCylinder(RADIUS,WHEEL_WIDTH);
			po->AddGSphere(WHEEL_WIDTH);
			po->SetMass(MassSetCylinder(1,2,RADIUS,WHEEL_WIDTH));
			po->SetMass(WMASS);
			dRFromAxisAndAngle(R,1,0,0,M_PI*0.5);
			po->SetRotation(R);
			switch(i)
			{
				case 0: po->SetPosition(LENGTH,0,STARTZ-HEIGHT*0.5); break;
				case 1: po->SetPosition(-0.5*LENGTH, WIDTH,STARTZ-HEIGHT*0.5); break;
				case 2: po->SetPosition(-0.5*LENGTH,-WIDTH,STARTZ-HEIGHT*0.5); break;
			}
		}

  // front and back wheel hinges
  	for (i=0; i<3; i++) 
  	{
  		dVector3 pos;
  		const dVector3 axis1={0,0,1,0},axis2={0,1,0,0};
		co->GetObjects()[i+1]->GetPosition(pos);
  		int joint=co->CreateHinge2(0,i+1,pos,axis1,axis2);
  		co->SetHinge2Param(joint,dParamSuspensionERP,0.4);
  		co->SetHinge2Param(joint,dParamSuspensionCFM,0.8);
  		if(i)
  		{
  			co->SetHinge2Param(joint,dParamLoStop,0);
    			co->SetHinge2Param(joint,dParamHiStop,0);
  		}
  	}
  	Vehicle=co;
  	MotorJoint=0;
	co=new BCPhisComponentObject(Scene);
		po=new BCScenePhisObject(co);
			po->AddGSphere(RADIUS);
			po->SetMass(MassSetCylinder(1.0,2,RADIUS,WIDTH));
			po->SetMass(CMASS);
			po->SetPosition(0,0,2*STARTZ);
	printf("dHashSpaceClass is %d\n",dHashSpaceClass);
	printf("dSimpleSpaceClass is %d\n",dSimpleSpaceClass);
	printf("duCylinderClass is %d\n",duCylinderClass);
	// run simulation
  	dsSimulationLoop (argc,argv,352,288,&fn);

	delete Scene;
	return 0;
}
