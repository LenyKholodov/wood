#ifndef __ODE_ADVANCED_SHAPES_MESH_H__
#define __ODE_ADVANCED_SHAPES_MESH_H__

#include <ode\ode.h>

///////////////////////////////////////////////////////////////////////////////////////////
// ������������ ���������. �������� �������:

struct _dMeshTriangle;

////////////////////////////
typedef struct
{
	dVector3				p_local;		// ���������� ����� � ��������� ������� ���������
	dVector3				p;				// ��������������� ���������� ����������
	struct _dMeshTriangle*	triangle;		// ������ �� ������ ����������� � ������ �����, ���������� ��� �������
	int						triangle_vertex;// ������ ������� � ������������
} dMeshVertex;

////////////////////////////
typedef struct _dMeshTriangle
{
	int						vertex[3];				// ������� ������ ������������
	struct _dMeshTriangle*	next_v_triangle[3];		// ������ �� �������� ����������� (�� ��������)
	int						next_vtr_index[3];		// ������ ������� � ��������� ������������
	struct _dMeshTriangle*	beside_edge_triangle[3];// ������ �� �����������, ������� �����. ������� �������:
	bool					WasCollided;		
#if defined(DEBUG) || defined(_DEBUG)	
	bool					DrawCollide;			// ���� ��� ��������� ������������� ���������
#endif
	dReal					aabb[6];			// AABB ��� ������������
	struct _dMeshTriangle*	edge_possible_collide_next;		// ������ �� ����. � ������� ��������� �����������
	int						edge_possible_collide_index;	// ������ ������� ���������� �����������
	struct _dMeshTriangle*	OctreeNextTriangle;			// ������ �������������, ������������� ��������� ���� ������
} dMeshTriangle;

class OctTreeAABB;

///////////////////////////
struct dxMesh 
{	
	dMeshVertex*			vertexes;
	int						v_count;
	dMeshTriangle*			triangles;
	int						t_count;
	dMeshTriangle*			first_edge_possible_triangle;
	
	OctTreeAABB*			OctTree;

	void					Destroy(void);
	bool					Load(const dVector3* vertexes,int vcount,const int* triangles,int tcount);
	void					ClearInternalStatus(void);	// �������� ���������� ����������: ���� "�������������" ��������, ������ ��������, � �.�.
#if defined(DEBUG) || defined(_DEBUG)
	void					ClearDrawStatus(void);
#endif
	void					CalcAABBs();
};

///////////////////////////////////////////////////////////////////////////////////////////

extern int dGeomMeshClassUser;

dGeomID dCreateMesh(dSpaceID space);
bool dGeomMeshLoad(dGeomID g,const dVector3* vertexes,int vcount,const int* triangles,int tcount);

#endif
