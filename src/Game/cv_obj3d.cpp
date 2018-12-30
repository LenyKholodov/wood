#include "pch.h"
#include "cv_utils.h"

void     ConvertObject3D  (Object3D::Node node,int* pos,vertex_t* vertex,vector3d_t* normal,facet_t* _facet)
{
  D_Try
  {  
    if (node.count (Object3D::FACET) > 0 && _facet)
    {
      int      count   = node.count (Object3D::FACET);
      facet_t* facet   = node.facet ();
      facet_t* dest    = &_facet [pos [0]];
      int      fvertex = pos [1];

      for (int i=0;i<count;i++,facet++,dest++)
      {
        *dest = *facet;

        for (int j=0;j<3;j++)
          dest->index [j] = facet->index [j] + fvertex;
      } 

      pos [0]  += count;
    }

    if (node.count (Object3D::NORMAL) > 0 && normal && node.normal ())
      memcpy (normal + pos [1],node.normal(),node.count (Object3D::NORMAL) * sizeof (vector3d_t));

    if (node.count (Object3D::VERTEX) > 0 && vertex  && node.vertex ())
      memcpy (vertex + pos [1],node.vertex(),node.count (Object3D::VERTEX) * sizeof (vertex_t));

    if (node.count (Object3D::VERTEX) > 0)
      pos [1] += node.count (Object3D::VERTEX);

    for (Object3D::Node iter = node.node();iter.valid();iter = iter.next())
      ConvertObject3D (iter,pos,vertex,normal,_facet);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at convert ConvertObject3D (Object3D::Node,int*) "<<endl;
    #endif
  }    
}

BOOL     ConvertObject3D  (
  const Object3D&         obj,
  vertex_t*               vertex,
  vector3d_t*             normal,
  facet_t*                facet
)
{
  D_Try
  {
    if (!IsValid (&obj))
      return FALSE;

    dout<<"Begin convert scene"<<endl;

    int             pos [16]; memset (pos,0,sizeof (int) * 16);
    Object3D::Node  node ((Object3D&)obj,obj.MainIndex ());

    ConvertObject3D (node,pos,vertex,normal,facet);  

    vertex_t*   v = vertex;
    vector3d_t* n = normal;
    facet_t*    f = facet;

    for (int i=0;i<obj.count (Object3D::VERTEX);i++,v++,n++)
    {
      v->x = -v->x; 
//      swap (v->y,v->z);
//      swap (n->y,n->z);
      n->x = -n->x;
    }  

    for (i=0;i<obj.count (Object3D::FACET);i++,f++)
      f->n.x = -f->n.x;
  
    return TRUE;
  }
  D_ExcRet ("Error at ConvertObject3D",FALSE);
}

BOOL     ConvertObject3D  (
  const Object3D&         obj,
  Pool*                   pool,
  int&                    vCount,
  vertex_t**              vertexPtr,
  int&                    fCount,
  facet_t**               facetPtr,
  int&                    nCount,
  vector3d_t**            normalPtr,
  int&                    mCount,
  textmat_t**             matPtr
)
{
  if (!IsValid (pool) || !IsValid (&obj))
    return FALSE;

  D_Try
  {
    vertex_t*     vertex = NULL;
    vector3d_t*   normal = NULL;
    facet_t*      facet  = NULL;
    textmat_t*    mat    = NULL;

    if (vertexPtr) 
    {
      vCount     = 0;
      *vertexPtr = NULL;
    }  

    if (facetPtr)  
    {
      fCount     = 0;
      *facetPtr  = NULL;
    }  

    if (normalPtr) 
    {
      nCount     = 0;
      *normalPtr = NULL;
    }  

    if (matPtr)    
    {
      mCount     = 0;
      *matPtr    = NULL;
    }

    BOOL error = FALSE;  

    if (vertexPtr)  
    {
      vCount     = obj.count (Object3D::VERTEX);
      vertex     = (vertex_t*)pool->alloc (sizeof (vertex_t) * vCount);
      *vertexPtr = vertex;
      if (!vertex && vCount) error = TRUE;
    }  

    if (facetPtr)
    {
      fCount    = obj.count (Object3D::FACET);
      facet     = (facet_t*)pool->alloc (sizeof (facet_t) * fCount);
      *facetPtr = facet;
      if (!facet && fCount) error = TRUE;
    }

    if (normalPtr)
    {
      nCount     = obj.count (Object3D::NORMAL);
      normal     = (vector3d_t*)pool->alloc (sizeof (vector3d_t) * nCount);
      *normalPtr = normal;
      if (!normal && nCount) error = TRUE;
    }

    if (matPtr)
    {
      mCount  = obj.count (Object3D::MATERIAL);
      mat     = (textmat_t*)pool->alloc (sizeof (textmat_t) * mCount);
      *matPtr = mat;
      if (!mat && mCount) error = TRUE;
    }

    if (!ConvertObject3D (obj,vertex,normal,facet))
      error = TRUE;

    for (int i=0;i<mCount;i++)
      mat [i] = obj.GetMaterial (i);

    if (error)
    {
      if (mat)          pool->free (mat);
      if (normal)       pool->free (normal);
      if (facet)        pool->free (facet);
      if (vertex)       pool->free (vertex);

      vCount = mCount = nCount = fCount = 0;

      return FALSE;
    }

    return TRUE;
  }
  D_ExcRet ("Error at ConvertObject3D",FALSE);
}
