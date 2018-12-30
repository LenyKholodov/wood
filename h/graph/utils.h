#ifndef __ENGINE_UTILS__
#define __ENGINE_UTILS__

#include <graph\grtypes.h>
#include <misc.h>

////////////////////////////////////////////////////////////////////////////////
///Check functions
////////////////////////////////////////////////////////////////////////////////
BOOL    IsInBound       (const vertex_t&,int num,const plane_t*);
BOOL    IsInBound       (int count,const vertex_t*,int num,const plane_t*);
BOOL    IsInBound       (int count,int*,const vertex_t*,int num,const plane_t*);
BOOL    IsInBound       (const vertex_t&,float width,int num,const plane_t*);
BOOL    IsInBound       (const facet_t&,const vertex_t*,int num,const plane_t*);
BOOL    IsInBound       (const portal_t&,int num,const plane_t*);
BOOL    IsInBound       (const boundbox_t&,int num,const plane_t*);
BOOL    IsInBound       (const boundsphere_t&,int num,const plane_t*);
BOOL    IsInBound       (const boundbase_t&,int num,const plane_t*);
BOOL    IsInBound       (const bound_t&,int num,const plane_t*);

//BOOL    IsFront         (const Camera&,const facet_t&,const vertex_t*);
//BOOL    IsFront         (const Camera&,const portal_t&);
//BOOL    IsFront         (const Camera&,const vertex_t&,float);
//BOOL    IsFront         (const Camera&,const boundbox_t&);
//BOOL    IsFront         (const Camera&,const bound_t&);

////////////////////////////////////////////////////////////////////////////////
///Check for z-clipping (only with GL_HP_occlusion_test present)
////////////////////////////////////////////////////////////////////////////////
//BOOL    IsVisible       (const Camera&,const vertex_t&);
//BOOL    IsVisible       (const Camera&,const vertex_t&,float width);
//BOOL    IsVisible       (const Camera&,const facet_t&,const vertex_t*);
//BOOL    IsVisible       (const Camera&,const portal_t&);
//BOOL    IsVisible       (const Camera&,const boundbox_t&);
//BOOL    IsVisible       (const Camera&,const boundsphere_t&);
//BOOL    IsVisible       (const Camera&,const bound_t&);

//BOOL    IsVisible       (const Camera&,const vertex_t&,const portalbound_t&);
//BOOL    IsVisible       (const Camera&,const vertex_t&,float width,const portalbound_t&);
//BOOL    IsVisible       (const Camera&,const facet_t&,const vertex_t*,const portalbound_t&);
//BOOL    IsVisible       (const Camera&,const portal_t&,const portalbound_t&);
//BOOL    IsVisible       (const Camera&,const boundbox_t&,const portalbound_t&);
//BOOL    IsVisible       (const Camera&,const boundsphere_t&,const portalbound_t&);
//BOOL    IsVisible       (const Camera&,const bound_t&,const portalbound_t&);

////////////////////////////////////////////////////////////////////////////////
///Recalc
////////////////////////////////////////////////////////////////////////////////
//portalbound_t&  CalcPortalBound     (const Camera&,const portal_t&,portalbound_t&);
//portal_t&       CalcPortalIntercept (const Camera&,const portal_t&,const portal_t&,portal_t&);
frustum_t&      CalcGLFrustum       (frustum_t&);
frustum_t&      CalcFrustum         (const matrix_t&,frustum_t&);

BOOL            CalcBound           (int,vertex_t*,bound_t&);
BOOL            CalcBound           (int fCount,facet_t*,vertex_t*,bound_t&);
void            CalcBound           (const boundbase_t&,bound_t&);
void            CalcBoundBase       (const matrix_t&,boundbase_t&);

////////////////////////////////////////////////////////////////////////////////
///Settings
////////////////////////////////////////////////////////////////////////////////
void            SetMaterial         (const material_t&);
void            SetLight            (int i,const light_t&);
void            SetLight            (int,BOOL);
light_t         GetLight            (int i);

////////////////////////////////////////////////////////////////////////////////
///Plane work
////////////////////////////////////////////////////////////////////////////////
inline 
float           FindDist           (const vertex_t& v,const plane_t& p) {
  return v.x * p.a + v.y * p.b + v.z * p.c + p.d;
}
//int             SplitFacet         (const vertex_t* srcTriangle,
//                                    const plane_t&  splitPlane,
//                                    vertex_t*       destTriangle1,
//                                    vertex_t*       destTriangle2,
//                                    vertex_t*       destTriangle3
//                                   );
//int             SplitFacet         (const MemListTmpl<vertex_t>& srcFacet,
//                                    const plane_t&               splitPlane,
//                                    MemListTmpl<vertex_t>&       leftFacet,
//                                    MemListTmpl<vertex_t>&       rightFacet
//                                   );

//int             SplitFacet         (const vertex_t* srcTriangle,
//                                    int             planeCount,
//                                    const plane_t*  planes,
//                                    vertex_t*       destTriangles //3*return vertex
//                                   );

////////////////////////////////////////////////////////////////////////////////
///Find intersections
////////////////////////////////////////////////////////////////////////////////
BOOL            IsPlaneIntersect   (vertex_t* line,const plane_t&);
BOOL            IsPlaneIntersect   (vertex_t* line,vertex_t* poly);
BOOL            IsPolygonIntersect (vertex_t* line,vertex_t* v,int count);
BOOL            IntersectionPoint  (vertex_t* line,const plane_t&,vertex_t&);
BOOL            IsInsidePolygon    (const vertex_t&,vertex_t* v,int count);

#endif