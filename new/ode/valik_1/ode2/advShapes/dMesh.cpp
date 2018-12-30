#include <ode/addons/advShapes.h>
#include <ode/addons/misc.h>

// пока изделано крайне тупо:
// создается группа, в группу пихаются треугольники, 
// создается массив pCount*pCount (индикация пересечений), в geom пихаются ребра
dGeomID dCreateMesh(dSpaceID space,const dVector3 p[],int pCount,const int tripple[][3],int tCount)
{
	dGeomID group=dCreateGeomGroup(space);
	char* lFlag=new char[pCount*pCount];
	memset(lFlag,0,pCount*pCount);
#define EDGE(i,j)	lFlag[i*pCount+j]
	for(int i=0;i<tCount;i++)
	{	// добавляем треугольник
		dGeomGroupAdd(group,dCreateFace(space,p[tripple[i][0]],p[tripple[i][1]],p[tripple[i][2]]));
		// Добавляем ребра
		for(int j=0;j<3;j++)
		{
			int i1=tripple[i][j],i2=tripple[i][(j+1)%3];
			if(!EDGE(i1,i2))
			{
				EDGE(i1,i2)=EDGE(i2,i1)=1;
				dGeomGroupAdd(group,dCreateEdge(space,p[i1],p[i2]));
			}
		}
	}
#undef  EDGE
	delete lFlag;
	return group;
}