// Поверхность, набитая из трианглов

#include <pch.h>

#include <ode\addons\advShapes.h>
//#include <ode\addons\misc.h>

#include "q:\src\ode\collision_kernel.h"

struct CallingAABBStruct
{
	dReal			aabb[6];
	dGeomID			o1,o2;
	int				flags;
	int				numleft,num;
	dContactGeom	*contact;
	int				skip;
	dxMesh			*mesh;	
};


typedef void (*CollideAABBTreeFn)(CallingAABBStruct*,dMeshTriangle*);

class OctTreeAABB
{
	struct OctTreeNode
	{		
		OctTreeNode*	subnodes[8];	// разбиение по 8м частям
		OctTreeNode*	subsnodes[6];	// разбиение по подпространствам
		dReal			aabb[6];
		OctTreeNode*	FreeNodeNext;	// следующий свободный блок (только для цепи св. блоков)
		dMeshTriangle*	FirstTriangleInNode;
		int				t_count;		// кол-во треугольников в этом узле
		int				count;			// кол-во треугольников в поддереве
		OctTreeNode*	LevelNodeNext;	// принадлежность списку текущего уровня дробления
		void			GetCollide(CallingAABBStruct*,CollideAABBTreeFn);
	};
	OctTreeNode*		nodes;
	int					n_count;
	OctTreeNode*		FirstFreeNode;
	int					FreeNodeCount;

	void	Clear(void);	
	OctTreeNode* AllocNode(void);
public:
	OctTreeAABB(int nc=5000);
	~OctTreeAABB();

	void	Build(dxMesh* mesh,const dReal* aabb);
	void	collide(CallingAABBStruct* caabbs,CollideAABBTreeFn fn)
				{ if(nodes) nodes[0].GetCollide(caabbs,fn);	}
};


void OctTreeAABB::OctTreeNode::GetCollide(CallingAABBStruct* caabbs,CollideAABBTreeFn fn)
{
	const dReal* boxx=caabbs->aabb;
	if(boxx[1]<aabb[0] || boxx[0]>aabb[1] ||	// Если бокс за границами по X...
		boxx[3]<aabb[2] || boxx[2]>aabb[3] ||	// или по Y...
		boxx[5]<aabb[4] || boxx[4]>aabb[5])		// или по Z...
		return;									// Делать нам нечего
	// Пробежка по треугольникам текущго уровня
	for(dMeshTriangle* t=FirstTriangleInNode;t;t=t->OctreeNextTriangle)
		if(!t->WasCollided)
			fn(caabbs,t);
	for(int i=0;i<8;i++)
		if(subnodes[i])
			subnodes[i]->GetCollide(caabbs,fn);
	for(i=0;i<6;i++)
		if(subsnodes[i])
			subsnodes[i]->GetCollide(caabbs,fn);
}

void OctTreeAABB::Clear(void)
{
	FirstFreeNode=nodes;
	FreeNodeCount=n_count;
	for(int i=0;i<n_count-1;i++)	
		nodes[i].FreeNodeNext=nodes+i+1;
	nodes[i].FreeNodeNext=NULL;
}

OctTreeAABB::OctTreeNode* OctTreeAABB::AllocNode(void)
{
	OctTreeNode* node=FirstFreeNode;
	if(node)
	{
		FirstFreeNode=node->FreeNodeNext;
		FreeNodeCount--;
		for(int i=0;i<8;i++)
			node->subnodes[i]=NULL;
		for(i=0;i<6;i++)
			node->subsnodes[i]=NULL;		
		node->FreeNodeNext=NULL;
		node->FirstTriangleInNode=NULL;
		node->LevelNodeNext=NULL;
		node->t_count=0;
		node->count=0;
	}
	return node;
}

OctTreeAABB::OctTreeAABB(int nc)
{
	dUASSERT(nc>0,"bad count of OctTree nodes");
	nodes=new OctTreeNode[nc];
	dUASSERT(nodes,"No memory for nodes");
	n_count=nc;
	Clear();
}

OctTreeAABB::~OctTreeAABB()
{
	if(nodes)
	{
		delete nodes;
		FirstFreeNode=nodes=NULL;
		n_count=0;		
	}
}

void OctTreeAABB::Build(dxMesh* mesh,const dReal* aabb)
{
	Clear();
	// Идея: 
	//		берем первый узел.
	//		пихаем в него все треугольники
	//		Заводим список уровня
	//		"пробегаем" текущий список узлов.
	//		"скидываем" в него треугольники по соотв. боксам	
	OctTreeNode* node=AllocNode();
	dUASSERT(node,"No memory for first node");
	for(int i=0;i<6;i++)
		node->aabb[i]=aabb[i];
	node->FirstTriangleInNode=mesh->triangles;
	for(i=0;i<mesh->t_count-1;i++)
	{		
		mesh->triangles[i].OctreeNextTriangle=&mesh->triangles[i+1];
	}
	node->count=node->t_count=mesh->t_count;
	mesh->triangles[i].OctreeNextTriangle=NULL;
	//		переходим на уровень ниже.
	//		так пока не кончится память или треугольники
	for(OctTreeNode* NextLevelNode;node;node=NextLevelNode)
	for(NextLevelNode=NULL;node;node=node->LevelNodeNext)
	{	// Перебираем коллекцию треугольников в поисках разбиения по боксам
		// во избежание путаницы следующая фигня со знаками боксов:
		// биты: (Z:2)(Y:1)(X:0). bit is set(+) bit not set(-)
		// ...
		dMeshTriangle** prevRelationOwner=&node->FirstTriangleInNode;
		dVector3 NodeMed={(node->aabb[0]+node->aabb[1])*0.5,
						  (node->aabb[2]+node->aabb[3])*0.5,
						  (node->aabb[4]+node->aabb[5])*0.5};
		for(dMeshTriangle* triangle=node->FirstTriangleInNode;triangle;triangle=*prevRelationOwner)
		{			
			int SubNodeIndex=0;
			int SubSideFlag=0;	// Доступно размещение по всем сторонам
			// находим индекс куба...
			if(triangle->aabb[0]>=NodeMed[0])	// minX
				SubNodeIndex|=0x01;
			else
				if(triangle->aabb[1]>NodeMed[0])	// maxX
					SubSideFlag|=0x01;
			if(triangle->aabb[2]>=NodeMed[1])	// minY
				SubNodeIndex|=0x02;
			else
				if(triangle->aabb[3]>NodeMed[1])	// maxY
					SubSideFlag|=0x02;				
			if(triangle->aabb[4]>=NodeMed[2])	// minZ
				SubNodeIndex|=0x04;
			else
				if(triangle->aabb[5]>NodeMed[2])	// maxZ
					SubSideFlag|=0x04;
			// Определяем подузел
			OctTreeNode** SubNodePtr=NULL;
			switch(SubSideFlag)
			{
				case 0:	// Это один из восьми "чистых" подузлов
					SubNodePtr=&node->subnodes[SubNodeIndex];
					break;
				case 2:	// Свободны XZ:
				case 4: // Свободно XY:
				case 6: // Свободно X:
					SubNodePtr=&node->subsnodes[SubNodeIndex&1?1:0];
					break;
				case 1:	// Свободны YZ:
				case 5: // Свободно Y:
					SubNodePtr=&node->subsnodes[SubNodeIndex&2?3:2];
					break;					
				case 3: // Свободно Z:
					SubNodePtr=&node->subsnodes[SubNodeIndex&4?5:4];
					break;				
				case 7: // все стороны заняты: объект принадлежит узлу
					prevRelationOwner=&triangle->OctreeNextTriangle;
					continue;
			}
			// куб найден. Проверяем его наличие...
			if((*SubNodePtr)==NULL)
			{
				// Заказываем память под куб;
				if(((*SubNodePtr)=AllocNode())==NULL)
					return;	// память кончилась - выход
				// добавляем куб к списку кубов следующего уровня...
				(*SubNodePtr)->LevelNodeNext=NextLevelNode;
				NextLevelNode=(*SubNodePtr);
				// расчет aabb нового узла
				dReal* saabb=(*SubNodePtr)->aabb;
				switch(SubSideFlag)
				{
					case 0:	// Это один из восьми "чистых" подузлов						
						for(i=0;i<3;i++)
						{
							if(SubNodeIndex&0x01)
							{
								saabb[2*i]  =NodeMed[i];
								saabb[2*i+1]=node->aabb[2*i+1];
							}
							else
							{
								saabb[2*i]  =node->aabb[2*i];
								saabb[2*i+1]=NodeMed[i];
							}
							SubNodeIndex>>=1;
						}
						break;
					case 2:	// Свободны XZ:
					case 4: // Свободно XY:
					case 6: // Свободно X:
						saabb[2]=node->aabb[2];
						saabb[3]=node->aabb[3];
						saabb[4]=node->aabb[4];
						saabb[5]=node->aabb[5];
						if(SubNodeIndex&1)
						{
							saabb[0]=NodeMed[0];
							saabb[1]=node->aabb[1];
						}
						else
						{
							saabb[0]=node->aabb[0];
							saabb[1]=NodeMed[0];
						}
						break;
					case 1:	// Свободны YZ:
					case 5: // Свободно Y:
						saabb[0]=node->aabb[0];
						saabb[1]=node->aabb[1];
						saabb[4]=node->aabb[4];
						saabb[5]=node->aabb[5];
						if(SubNodeIndex&2)
						{
							saabb[2]=NodeMed[1];
							saabb[3]=node->aabb[3];
						}
						else
						{
							saabb[2]=node->aabb[2];
							saabb[3]=NodeMed[1];
						}
						break;					
					case 3: // Свободно Z:
						saabb[0]=node->aabb[0];
						saabb[1]=node->aabb[1];
						saabb[2]=node->aabb[2];
						saabb[3]=node->aabb[3];
						if(SubNodeIndex&4)
						{
							saabb[4]=NodeMed[2];
							saabb[5]=node->aabb[5];
						}
						else
						{
							saabb[4]=node->aabb[4];
							saabb[5]=NodeMed[2];
						}
						break;								
				}	
			}			
			// переносим треугольник в под-куб
			node->t_count--;
			(*SubNodePtr)->count++;
			(*SubNodePtr)->t_count++;
			// "выкусываем" из текущего списка
			*prevRelationOwner=triangle->OctreeNextTriangle;
			triangle->OctreeNextTriangle=(*SubNodePtr)->FirstTriangleInNode;
			(*SubNodePtr)->FirstTriangleInNode=triangle;			
		}
	}					
}


void dxMesh::CalcAABBs()
{
	dMeshTriangle*		triangle=triangles;	
	for(int i=0;i<t_count;i++,triangle++)
	{
		dReal* aabb=triangle->aabb;
		for(int j=0;j<3;j++)
		{
			aabb[0+j*2]=__min(vertexes[triangle->vertex[0]].p[j],
					  __min(vertexes[triangle->vertex[1]].p[j],
						  vertexes[triangle->vertex[2]].p[j]));
			aabb[1+j*2]=__max(vertexes[triangle->vertex[0]].p[j],
					  __max(vertexes[triangle->vertex[1]].p[j],
						  vertexes[triangle->vertex[2]].p[j]));
		}
	}
}

void dxMesh::Destroy(void)
{
	if(vertexes)
	{
		delete vertexes;
		vertexes=NULL;
		v_count=0;
	}
	if(triangles)
	{
		delete triangles;
		triangles=NULL;
		t_count=0;
	}
	if(OctTree)
	{
		delete OctTree;
		OctTree=NULL;
	}
	first_edge_possible_triangle=NULL;
}

void dxMesh::ClearInternalStatus(void)
{
	first_edge_possible_triangle=NULL;
	for(int i=0;i<t_count;i++)
	{
		triangles[i].edge_possible_collide_next=NULL;
		triangles[i].edge_possible_collide_index=-1;
		triangles[i].WasCollided=false;
	}
}

#if defined(DEBUG) || defined(_DEBUG)
void dxMesh::ClearDrawStatus(void)
{
	for(int i=0;i<t_count;i++)
		triangles[i].DrawCollide=false;
}
#endif

bool dxMesh::Load(const dVector3* v,int vcount,const int* t,int tcount)
{		
	Destroy();
	vertexes=new dMeshVertex[vcount];
	if(vertexes==NULL)
		return false;
	// заполняем вершины...
	v_count=vcount;
	for(int i=0;i<vcount;i++)
	{
		vector_copy3(vertexes[i].p_local,v[i]); // координаты точки в локальной системе коррдинат
		vertexes[i].triangle=NULL;				// пока нет треугольников в списке
		vertexes[i].triangle_vertex=0;			// индекс вершины в треугольнике
	}
	triangles=new dMeshTriangle[tcount];
	if(triangles==NULL)
	{
		Destroy();
		return false;
	}
	t_count=tcount;
	OctTree=new OctTreeAABB;
	if(OctTree==NULL)
	{
		Destroy();
		return false;
	}
	// заполняем треугольники
	for(i=0;i<tcount;i++)
	{
		dMeshTriangle* ctr=triangles+i;
				
		for(int j=0;j<3;j++)
		{
			ctr->vertex[j]=t[i*3+j];		// индексы вершин треугольника
			
			dMeshVertex* vtx=vertexes+ctr->vertex[j];

			ctr->next_v_triangle[j]=vtx->triangle;		// ссылка на следущий треугольник (по вершинам)
			ctr->next_vtr_index[j]=vtx->triangle_vertex;// индекс вершины в следующем треугольнике
			vtx->triangle=ctr;							// теперь и наш треугольник в списке
			vtx->triangle_vertex=j;
			ctr->beside_edge_triangle[j]=NULL;
		}

		// Анализ сторон треугольника
		// цель: поиск треугольника с такой же стороной
		for(j=0;j<3;j++)
		{			
			// 0:1-2; 1:2-0;2:0-1
			int v1=ctr->vertex[j];
			// поиск по вершине
			dMeshTriangle*	tr	=ctr->next_v_triangle[j];
			int				v_tr=ctr->next_vtr_index[j];
			while(tr)
			{
				if(tr->vertex[(v_tr+1)%3]==ctr->vertex[(j+1)%3]) // "наш" случай 1 (вперед)
				{
					dUASSERT(tr->beside_edge_triangle[v_tr],"Сбой: 2е пересечение по иджу");
					tr->beside_edge_triangle[v_tr]=ctr;
					ctr->beside_edge_triangle[j]=tr;
					break;
				}
				if(tr->vertex[(v_tr+2)%3]==ctr->vertex[(j+1)%3]) // "наш" случай 2
				{
					dUASSERT(tr->beside_edge_triangle[(v_tr+2)%3]==NULL,"Сбой: 2е пересечение по иджу");
					tr->beside_edge_triangle[(v_tr+2)%3]=ctr;
					ctr->beside_edge_triangle[j]=tr;
					break;
				}
				int next_v_tr=tr->next_vtr_index[v_tr];
				tr=tr->next_v_triangle[v_tr];
				v_tr=next_v_tr;
			}			
		}
	}
	ClearInternalStatus();
#if defined(DEBUG) || defined(_DEBUG)
	ClearDrawStatus();
#endif
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

int dGeomMeshClassUser=-1;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void dGeomMeshDtor(dGeomID g)
{
	dUASSERT(g &&  dGeomGetClass(g)==dGeomMeshClassUser,"argument not a geom mesh");
	dxMesh *mesh=(dxMesh*)dGeomGetClassData(g);
	mesh->Destroy();
}

static void dGeomMeshAABB(dGeomID g,dReal aabb[6])
{
	dUASSERT(g &&  dGeomGetClass(g)==dGeomMeshClassUser,"argument not a geom mesh");	
	dxMesh *mesh=(dxMesh*)dGeomGetClassData(g);		
	if(mesh->v_count==0) 
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
	
	const dReal* R=dGeomGetRotation(g);
	const dReal* pos=dGeomGetPosition(g);
	
	for(int i=0;i<mesh->v_count;i++)
	{		
		dMeshVertex* cv=mesh->vertexes+i;
		dMULTIPLY0_331(cv->p,R,cv->p_local);
		vector_add3(cv->p,pos);
		for(int j=0,k=0;j<3;j++)
		{
			if(aabb[k]>cv->p[j])
				aabb[k]=cv->p[j];
			k++;
			if(aabb[k]<cv->p[j])
				aabb[k]=cv->p[j];
			k++;
		}
	}
	// Пересчет aabb для треугольников
	mesh->CalcAABBs();
	mesh->OctTree->Build(mesh,aabb);
}


class dcPlane
{
public:
	dVector3 Normal;
	dReal	 Distance;

	Init(const dVector3 v0,const dVector3 v1,const dVector3 v2)
	{
		dVector3 u,v;
		vector_sub3(u,v1,v0);
		vector_sub3(v,v2,v0);		
		dCROSS(Normal,=,u,v);
		Distance=dDOT(v0,Normal);

		dReal l=dDOT(Normal,Normal);
		if(l>0) 
		{
			l=dRecipSqrt(l);
			vector_mul3(Normal,l);
			Distance*=l;
		}
		else 
		{
			Normal[0]=1;
			Normal[1]=0;
			Normal[2]=0;
			Distance=0;
		}
	}

	dcPlane()	{}

	dcPlane(const dVector3 v0,const dVector3 v1,const dVector3 v2)	{ Init(v0,v1,v2); }	

	bool Contains(const dVector3 RefObject,dReal Epsilon = 0.0f) const
	{
		return dDOT(Normal,RefObject)-Distance>-Epsilon;
	}
};


static void CollideAABBTreeMeshFn(CallingAABBStruct* caabbs,dMeshTriangle* triangle)
{
	dxMesh* mesh=caabbs->mesh;
	dGeomID	o2=caabbs->o2;
	dGeomID	o1=caabbs->o1;
	int& numleft=caabbs->numleft;
	int& num	=caabbs->num;
	if(numleft<=0)
		return;

	dVector3 v[3];
	for(int j=0;j<3;j++)
		vector_copy3(v[j],mesh->vertexes[triangle->vertex[j]].p);
	dcPlane triangle_plane(v[0],v[1],v[2]);
	// проверка того, что центр фигуры лежит над поверхностью (а надо ли?)
	if(!triangle_plane.Contains(dGeomGetPosition(o2)))
		return;
		// Вычисление пересечений с треугольником
	dGeomID plane=dCreatePlane(NULL,triangle_plane.Normal[0],
									triangle_plane.Normal[1],
									triangle_plane.Normal[2],
									triangle_plane.Distance);
	const int N=10;
	dContact lcontact[N];

	int n=dCollide(plane,o2,N,&lcontact[0].geom,sizeof(dContact));
	if(n)
	{
		// Проверка того, что точка пересечения находится в пределах призмы треугольника
		// Для каждой из точек пересечения проверяем поочередно стороны.
		// если все ок- вычеркиваем ребро из списка ребер (см. текущие ссылки и ссылки соседа)
		// нет - пропускаем точку контакта и записываем соотв. ребро в список потенциальных "пересекальцев"
		dcPlane edge[3];
		for(j=0;j<3;j++)
		{
			dVector3 tpoint;
			vector_sub3(tpoint,v[(j+1)%3],triangle_plane.Normal);
			edge[j].Init(v[j],v[(j+1)%3],tpoint);
		}
		for(int k=0;k<n && numleft>0;k++)
		{
			for(j=0;j<3;j++)
				if(!edge[j].Contains(lcontact[k].geom.pos))					
				{	// Занести соотв. edge в список (если его еще там нет...)						
					// проверяем соседа:
					dMeshTriangle* c_tr=triangle;
					if(c_tr->edge_possible_collide_index>=0) // чего-то уже занесено.
							break;
					if(c_tr->beside_edge_triangle[j]==NULL) // можно заносить, соседа нет.
					{
						c_tr->edge_possible_collide_index=j;
						c_tr->edge_possible_collide_next=mesh->first_edge_possible_triangle;
						mesh->first_edge_possible_triangle=c_tr;
							break;
					}
					// сосед есть... проверяем, а не занес ли он эту же сторону в список?
					dMeshTriangle* be_tr=c_tr->beside_edge_triangle[j];
					if(be_tr->edge_possible_collide_index>=0 &&
					    be_tr->beside_edge_triangle[be_tr->edge_possible_collide_index]==c_tr)
					{
						// Да, занес. Ничего не делаем
						break;
					}
					// Нет, не занес. заносим :)
					c_tr->edge_possible_collide_index=j;
					c_tr->edge_possible_collide_next=mesh->first_edge_possible_triangle;
					mesh->first_edge_possible_triangle=c_tr;
					break;
				}
			if(j==3)	// точка контакта подходит
			{	// копируем данные в список				
				if(numleft>0)
				{
					memcpy(caabbs->contact,&lcontact[k].geom,sizeof(dContactGeom));
					caabbs->contact->g1=o1;
					caabbs->contact=CONTACT(caabbs->contact,caabbs->skip);
					numleft--;
					num++;
					triangle->WasCollided=true;
#if defined(DEBUG) || defined(_DEBUG)
					triangle->DrawCollide=true;
#endif
				}
			}
		}		
	}
	dGeomDestroy(plane);
}

int dCollideMesh(dGeomID o1,dGeomID o2, int flags,dContactGeom *contact, int skip)
{
	dUASSERT(o1 &&  dGeomGetClass(o1)==dGeomMeshClassUser,"argument not a geom mesh");	
	dxMesh *mesh=(dxMesh*)dGeomGetClassData(o1);
	if(mesh->v_count==0) 
		return 0;
	
	int numleft=flags & NUMC_MASK;
	if(numleft==0) 
		numleft=1;
	flags&=~NUMC_MASK;
	
	int num=0;

	if(o1->gflags & GEOM_AABB_BAD)	// recalc abs coords
	{								// временно и очень криво. При введении OctTree переписать
		dReal aabb[6];
		dGeomMeshAABB(o1,aabb);
	}

	mesh->ClearInternalStatus();

	const int N=10;
	dContact lcontact[N];

	CallingAABBStruct caabbs;
	dGeomGetAABB(o2,caabbs.aabb);
	caabbs.o1		=o1;
	caabbs.o2		=o2;
	caabbs.flags	=flags;
	caabbs.numleft	=numleft;
	caabbs.num		=num;
	caabbs.contact	=contact;
	caabbs.skip		=skip;
	caabbs.mesh		=mesh;

	mesh->OctTree->collide(&caabbs,CollideAABBTreeMeshFn);

	flags=caabbs.flags;
	numleft=caabbs.numleft;
	num=caabbs.num;
	contact=caabbs.contact;	
	
	// Дорабатываем ребра...
	dGeomID geKK=dCreateCylinder(NULL,1,1);

	const dReal MicroCylinder=1e-4;

	for(dMeshTriangle* tr=mesh->first_edge_possible_triangle;
			tr && tr->edge_possible_collide_index>=0 && numleft>0;
			tr=tr->edge_possible_collide_next)
	{
		// Проверка списка сторон, попавших в потенциальные пересечения: 
		// список не должен дублироватся
		{
			int itr1=tr->vertex[tr->edge_possible_collide_index],
				itr2=tr->vertex[(tr->edge_possible_collide_index+1)%3];
			
			for(dMeshTriangle* test_tr=tr->edge_possible_collide_next;
				test_tr && test_tr->edge_possible_collide_index>=0;
				test_tr=test_tr->edge_possible_collide_next)
			{					
				int test_itr1=test_tr->vertex[test_tr->edge_possible_collide_index],
					test_itr2=test_tr->vertex[(test_tr->edge_possible_collide_index+1)%3];
				dUASSERT(!((test_itr1==itr1 && test_itr2==itr2) || (test_itr1==itr2 && test_itr2==itr1)),
					"Bad edge list");

			}
		}
		// Проверяем соседей и пересечения:
		if(tr->WasCollided || 
			(tr->beside_edge_triangle[tr->edge_possible_collide_index] && tr->beside_edge_triangle[tr->edge_possible_collide_index]->WasCollided))
			continue;
		// Корректируем параметры цилиндра...
		dVector3 p0,p1,zv;
		int index=tr->edge_possible_collide_index;
		vector_copy3(p0,mesh->vertexes[tr->vertex[index]].p);
		vector_copy3(p1,mesh->vertexes[tr->vertex[(index+1)%3]].p);
		vector_sub3(zv,p0,p1);
		dReal le=sqrt(sqr(zv[0])+sqr(zv[1])+sqr(zv[2]));
		dGeomCylinderSetParams(geKK,MicroCylinder,le);
		// корректируем положение...
		dGeomSetPosition(geKK,(p0[0]+p1[0])*0.5,(p0[1]+p1[1])*0.5,(p0[2]+p1[2])*0.5);
		dMatrix3 R;
		dRFromZAxis(R,zv[0],zv[1],zv[2]);
		dGeomSetRotation(geKK,R);
		//, пересекаем, смотрим...		
		int n=dCollide(geKK,o2,N,&lcontact[0].geom,sizeof(dContact));
		for(int k=0;numleft>0 && k<n;k++)
		{
			memcpy(contact,&lcontact[k].geom,sizeof(dContactGeom));
			contact->g1=o1;
			//contact->depth/=2;
			contact=CONTACT(contact,skip);
			numleft--;
			num++;
		}			
	}
	dGeomDestroy(geKK);
	return num;
}

static dColliderFn *dGeomMeshColliderFn(int num)
{
	switch(num)
	{
		case dPlaneClass: return NULL;
	}
	return dCollideMesh;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
dGeomID dCreateMesh(dSpaceID space)
{
	if(dGeomMeshClassUser==-1) 
	{
		dGeomClass c;
		c.bytes	   =sizeof(dxMesh);
		c.collider =&dGeomMeshColliderFn;
		c.aabb	   =&dGeomMeshAABB;
		c.aabb_test=0;
		c.dtor	   =dGeomMeshDtor;
		dGeomMeshClassUser=dCreateGeomClass(&c);
	}
	dGeomID g=dCreateGeom(dGeomMeshClassUser);
	if(space)
		dSpaceAdd (space,g);
	dxMesh *mesh=(dxMesh*)dGeomGetClassData(g);
	mesh->vertexes	=NULL;
	mesh->v_count	=0;
	mesh->triangles	=NULL;
	mesh->t_count	=0;
	mesh->first_edge_possible_triangle=NULL;
	mesh->OctTree	=NULL;
	return g;
}

bool dGeomMeshLoad(dGeomID g,const dVector3* vertexes,int vcount,const int* triangles,int tcount)
{
	dUASSERT(g &&  dGeomGetClass(g)==dGeomMeshClassUser,"argument not a geom mesh");
	dUASSERT(vertexes && vcount>2,"Bad vertex list");
	dUASSERT(triangles && tcount>0,"Bad triangle list");
	dxMesh *mesh=(dxMesh*)dGeomGetClassData(g);
	return mesh->Load(vertexes,vcount,triangles,tcount);
}
