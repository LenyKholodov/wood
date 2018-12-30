#ifndef __ODE_ADVANCED_SHAPES_MESH_H__
#define __ODE_ADVANCED_SHAPES_MESH_H__

#include <ode\ode.h>

///////////////////////////////////////////////////////////////////////////////////////////
// используемые структуры. Описание вершины:

struct _dMeshTriangle;

////////////////////////////
typedef struct
{
	dVector3				p_local;		// координаты точки в локальной системе коррдинат
	dVector3				p;				// пересчитываемые глобальные координаты
	struct _dMeshTriangle*	triangle;		// ссылка на первый треугольник в списке фигур, содержащих эту вершину
	int						triangle_vertex;// индекс вершины в треугольнике
} dMeshVertex;

////////////////////////////
typedef struct _dMeshTriangle
{
	int						vertex[3];				// индексы вершин треугольника
	struct _dMeshTriangle*	next_v_triangle[3];		// ссылка на следущий треугольник (по вершинам)
	int						next_vtr_index[3];		// индекс вершины в следующем треугольнике
	struct _dMeshTriangle*	beside_edge_triangle[3];// ссылки на треугольник, имеющий соотв. смежную сторону:
	bool					WasCollided;		
#if defined(DEBUG) || defined(_DEBUG)	
	bool					DrawCollide;			// флаг для отрисовки столкнувшихся полигонов
#endif
	dReal					aabb[6];			// AABB для треугольника
	struct _dMeshTriangle*	edge_possible_collide_next;		// ссылка на след. в очереди возможных пересечений
	int						edge_possible_collide_index;	// индекс стороны возможного пересечения
	struct _dMeshTriangle*	OctreeNextTriangle;			// список треугольников, принадлежащих заданному узлу дерева
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
	void					ClearInternalStatus(void);	// зачистка внутренних параметров: цепи "подозреваемых" отрезков, флагов коллизий, и т.д.
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
