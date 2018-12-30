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

static dSpaceID space;
static dJointGroupID contactgroup;
static dGeomID geom=NULL,cylinder;

// things that the user controls

// this is called by dSpaceCollide when two objects in space are
// potentially colliding.

void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
	int i,n;
	const int N = 10;
	dContact contact[N];
	n=dCollide (o1,o2,10,&contact[0].geom,sizeof(dContact));  
	for(i=0; i<n; i++)
	{
		dsSetColor(1,0,0);
		dMatrix3 R;
		dSetZero(R,12);
		R[0]=R[5]=R[10]=1.0;
		dsDrawSphere(contact[i].geom.pos,R,0.05);	
		dVector3 p2;
		vector_add3(p2,contact[i].geom.pos,contact[i].geom.normal);
		dsDrawLine(contact[i].geom.pos,p2);
	}
}


// start simulation - set viewpoint

static void start()
{
  static float xyz[3] = {1.8317f,-5.9817f,1.8000f};
  static float hpr[3] = {121.0000f,-27.5000f,0.0000f};
  dsSetViewpoint (xyz,hpr);
  printf ("Press:\t'a' to increase speed.\n"
	  "\t'z' to decrease speed.\n"
	  "\t',' to steer left.\n"
	  "\t'.' to steer right.\n"
	  "\t' ' to reset speed and steering.\n");
}


dGeomID* CurrentGeom=&cylinder;

static void command (int cmd)
{	
	switch (cmd) 
	{
		case '5':		
			CurrentGeom=CurrentGeom==&geom?&cylinder:&geom;
			break;
		case '8':
			if(*CurrentGeom)
			{
				dVector3 pos;
				vector_copy3(pos,dGeomGetPosition(*CurrentGeom));
				dGeomSetPosition(*CurrentGeom,pos[0],pos[1]+0.1,pos[2]);
			}
			break;
		case '2':
			if(*CurrentGeom)
			{
				dVector3 pos;
				vector_copy3(pos,dGeomGetPosition(*CurrentGeom));
				dGeomSetPosition(*CurrentGeom,pos[0],pos[1]-0.1,pos[2]);
			}
			break;
		case '6':
			if(*CurrentGeom)
			{
				dVector3 pos;
				vector_copy3(pos,dGeomGetPosition(*CurrentGeom));
				dGeomSetPosition(*CurrentGeom,pos[0]+0.1,pos[1],pos[2]);
			}
			break;
		case '4':
			if(*CurrentGeom)
			{
				dVector3 pos;
				vector_copy3(pos,dGeomGetPosition(*CurrentGeom));
				dGeomSetPosition(*CurrentGeom,pos[0]-0.1,pos[1],pos[2]);
			}
			break;		
		case '9':
			if(*CurrentGeom)
			{
				dVector3 pos;
				vector_copy3(pos,dGeomGetPosition(*CurrentGeom));
				dGeomSetPosition(*CurrentGeom,pos[0],pos[1],pos[2]+0.1);
			}
			break;
		case '7':
			if(*CurrentGeom)
			{
				dVector3 pos;
				vector_copy3(pos,dGeomGetPosition(*CurrentGeom));
				dGeomSetPosition(*CurrentGeom,pos[0],pos[1],pos[2]-0.1);
			}
			break;
		case '1':
			if(*CurrentGeom)
			{
				dQuaternion q;
				dMatrix3 R,final_R;
				dQFromAxisAndAngle(q,1,0,0,M_PI*0.05);
				dQtoR(q,R);
				dMULTIPLY0_333(final_R,R,dGeomGetRotation(*CurrentGeom));
				dGeomSetRotation(*CurrentGeom,final_R);
			}
			break;
		case '3':
			if(*CurrentGeom)
			{
				dQuaternion q;
				dMatrix3 R,final_R;
				dQFromAxisAndAngle(q,1,0,0,M_PI*0.05);
				dQtoR(q,R);
				dMULTIPLY1_333(final_R,R,dGeomGetRotation(*CurrentGeom));
				dGeomSetRotation(*CurrentGeom,final_R);
			}
			break;
		case '0':
			if(*CurrentGeom)
			{
				dQuaternion q;
				dMatrix3 R,final_R;
				dQFromAxisAndAngle(q,0,1,0,M_PI*0.05);
				dQtoR(q,R);
				dMULTIPLY0_333(final_R,R,dGeomGetRotation(*CurrentGeom));
				dGeomSetRotation(*CurrentGeom,final_R);
			}
			break;
		case '.':
			if(*CurrentGeom)
			{
				dQuaternion q;
				dMatrix3 R,final_R;
				dQFromAxisAndAngle(q,0,1,0,M_PI*0.05);
				dQtoR(q,R);
				dMULTIPLY1_333(final_R,R,dGeomGetRotation(*CurrentGeom));
				dGeomSetRotation(*CurrentGeom,final_R);
			}
			break;
		case ' ':
			{
				int type=geom==NULL?-1:dGeomGetClass(geom);
				if(geom)
					dGeomDestroy(geom);
				switch(type)
				{					
					case dSphereClass: 
							geom=dCreateBox(space,1,0.5,0.7);
							break;
					case dBoxClass:
							geom=dCreateCCylinder(space,0.5,0.7);
							break;
					case dCCylinderClass:
							geom=dCreateCylinder(space,0.5,0.7);
							break;
					default:
						if(type==dCylinderClassUser || type==-1)
							geom=dCreateSphere(space,1);
				}
			}
	}
}

// simulation loop

static void simLoop (int pause)
{  
	dSpaceCollide (space,0,&nearCallback);
	//dJointGroupEmpty (contactgroup);	
	dsSetTexture(DS_WOOD);
	dsSetColor(0,1,0);
	dsDrawGeom(geom);
	dsSetColor(1,1,1);
	dsDrawGeom(cylinder);
}


int main (int argc, char **argv)
{
	// setup pointers to drawstuff callback functions
	dsFunctions fn;
	fn.version = DS_VERSION;
	fn.start = &start;
	fn.step = &simLoop;
	fn.command = &command;
	fn.stop = 0;
	fn.path_to_textures = "textures";

	space=dSimpleSpaceCreate(0);

	cylinder=dCreateCylinder(space,1,3);
	geom=dCreateSphere(space,1);
//	dGeomID plane=dCreatePlane(space,0,0,1,0);	
	
	dsSimulationLoop (argc,argv,800,600,&fn);
	
	dSpaceDestroy (space);
	return 0;
}
