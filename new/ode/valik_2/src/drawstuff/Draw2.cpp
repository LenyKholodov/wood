#include <pch.h>

#include <drawstuff\draw2.h>
#include <ode\addons\advShapes.h>

#include <windows.h>
#include <GL/gl.h>

// Для работы со списком геометрии ассоциированой с телом
#include "q:\src\ode\objects.h"

void dsDrawGeom(dGeomID geom,const dReal* pos,const dReal* R)
{
	if(geom==NULL)
		return;
	int type=dGeomGetClass(geom);
	
	switch(type)
	{
		case dPlaneClass:				// no placeable geom
			return;
		case dSimpleSpaceClass:			// no placeable geom
			return;
		case dHashSpaceClass:			// no placeable geom
			return;
		case dTriMeshClass:
			return;
		case dGeomTransformClass:
			return;
	}
	if(pos==NULL)
		pos=dGeomGetPosition(geom);
	if(R==NULL)
		R=dGeomGetRotation(geom);

	switch(type)
	{
		case dSphereClass:
			dsDrawSphere(pos,R,dGeomSphereGetRadius(geom)); 
			return;
		case dBoxClass:
			{
				dVector3 sides;
				dGeomBoxGetLengths(geom,sides);
				dsDrawBox(pos,R,sides);
			}
			return;
		case dCCylinderClass:
			{
				dReal r,l;
				dGeomCCylinderGetParams(geom,&r,&l);
				dsDrawCappedCylinder(pos,R,l,r);
			}
			return;
		case dCylinderClass:
			return;
		case dRayClass:
			return;
	}
	if(type==dCylinderClassUser)
	{
		dReal r,l;
		dGeomCylinderGetParams(geom,&r,&l);
		dsDrawCylinder(pos,R,l,r);
		return;
	}
	if(type==dGeomTransformGroupClass)
	{
		int count=dGeomTransformGroupGetNumGeoms(geom);
		for(int i=0;i<count;i++)
		{						
			dGeomID ngeom=dGeomTransformGroupGetGeom(geom,i);			
			dVector3 final_pos;
			dMatrix3 final_R;
			dGeomTransformGroupGetGeomPosition(geom,i,final_pos,final_R);			
			dsDrawGeom(ngeom,final_pos,final_R);
		}
		dVector3 r_pos;
		dMatrix3 r_R;
		dGeomTransformGroupGetRootPosition(geom,r_pos,r_R);
		float color[4];
		dsGetColor(color);
		dsSetColor(1,0,0);
		dsDrawCylinder(r_pos,r_R,0.1,0.005);
		dsSetColor(color[0],color[1],color[2]);
		return;
	}
	if(type==dGeomMeshClassUser)
	{		
		extern void setupDrawingMode();
		extern void setTransform(const float pos[3], const float R[12]);
		extern void setShadowDrawingMode();
		extern void setShadowTransform();
		extern void drawTriangle(const float *v0, const float *v1, const float *v2);
		extern int use_shadows;

		setupDrawingMode();
		glShadeModel(GL_FLAT);
#ifdef dSINGLE
		setTransform(pos,R);
#else
		float pos2[3],R2[12];
		for(int i=0;i<3;i++) pos2[i]=(float)pos[i];
		for(i=0;i<12;i++)	 R2[i]=(float)R[i];  
		setTransform(pos2,R2);
#endif
		dxMesh *mesh=(dxMesh*)dGeomGetClassData(geom);
		dMeshVertex* vtrx=mesh->vertexes;
		dMeshTriangle* tr=mesh->triangles;

		for(int i=0;i<mesh->t_count;i++)
		{
			float v[3][3];
			for(int j=0;j<3;j++)
			{
				v[j][0]=mesh->vertexes[mesh->triangles[i].vertex[j]].p_local[0];
				v[j][1]=mesh->vertexes[mesh->triangles[i].vertex[j]].p_local[1];
				v[j][2]=vtrx[tr[i].vertex[j]].p_local[2];				
			}
#if defined(DEBUG) || defined(_DEBUG)
			if(tr[i].DrawCollide)
			{
				float color[4];
				dsGetColor(color);
				dsSetColor(0,0,1);
				setupDrawingMode();
				drawTriangle(v[0],v[1],v[2]);
				dsSetColor(color[0],color[1],color[2]);
				setupDrawingMode();
			}
			else				
#endif			
				drawTriangle(v[0],v[1],v[2]);
		}
		
		glPopMatrix();
		if(use_shadows) 
		{
			setShadowDrawingMode();
			setShadowTransform();
			setTransform(pos,R);
						
			for(int i=0;i<mesh->t_count;i++)
			{
				float v[3][3];
				for(int j=0;j<3;j++)
				{
					v[j][0]=vtrx[tr[i].vertex[j]].p_local[0];
					v[j][1]=vtrx[tr[i].vertex[j]].p_local[1];
					v[j][2]=vtrx[tr[i].vertex[j]].p_local[2];					
				}				
				drawTriangle(v[0],v[1],v[2]);
			}
			glPopMatrix();			
			glPopMatrix();
			glDepthRange (0,1);
		}
#if defined(DEBUG) || defined(_DEBUG)
		mesh->ClearDrawStatus();
#endif
		return;
	}
}

void dsDrawBody(dBodyID body)
{
	if(body)		
		for(dGeomID geom=((dxBody*)body)->geom;geom;geom=dGeomGetBodyNext(geom))
			dsDrawGeom(geom);
}