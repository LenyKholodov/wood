#include "pch.h"
#include "cv_utils.h"
#include <fstream.h>

void    ComputeBound    (int count,vertex_t* vertex,bound_t& b)
{
  vector3d_t c (.0f);
  vector3d_t dim (.0f);

  for (int i=0;i<count;i++)
    c += vertex [i];

  c /= float (count);

  for (i=0;i<count;i++)
  {
    vector3d_t d = abs (c - vertex [i]);

    if (d.x > dim.x) dim.x = d.x;
    if (d.y > dim.y) dim.y = d.y;
    if (d.z > dim.z) dim.z = d.z;
  }    

  b.box.min       = c - dim;
  b.box.max       = c + dim;
  b.sphere.center = c;
  b.sphere.r      = length (dim) * 2.0f;
}

void    ComputeBound    (int count,facet_t* facet,vertex_t* vertex,bound_t& b)
{
  vector3d_t c (.0f);
  vector3d_t dim (.0f);

  for (int i=0;i<count;i++)
    for (int j=0;j<3;j++)
      c += vertex [facet [i].index [j]];

  c /= float (count * 3);

  for (i=0;i<count;i++)
    for (int j=0;j<3;j++)
    {
      vector3d_t d = abs (c - vertex [facet [i].index [j]]);

      if (d.x > dim.x) dim.x = d.x;
      if (d.y > dim.y) dim.y = d.y;
      if (d.z > dim.z) dim.z = d.z;
    }

  b.box.min       = c - dim;
  b.box.max       = c + dim;
  b.sphere.center = c;
  b.sphere.r      = length (dim) * 2.0f;  
}

uint    ConvertArrays (
  uint                    vcount,
  vertex_t**              vertex,
  vector3d_t**            normal,
  textcoord_t**           tcoord,
  facet_t*                farray,
  uint                    fcount,
  Pool*                   pool,
  BOOL                    eClear,
  facet_t**               srcfacet,
  vertex_t**              srcvertex
)
{
  D_Try
  {
    if (!*vertex || !farray)
    {
      D_Message ("ConvertArrays: Wrong input data");
      return 0;
    }  

    if (srcfacet)
    {
      *srcfacet         = (facet_t*)pool->alloc (sizeof (facet_t) * fcount);
      if (!*srcfacet) 
        D_NoMemoryRet ("ConvertArrays",sizeof (facet_t) * fcount,0);
      memcpy (*srcfacet,farray,sizeof (facet_t) * fcount);
    }

    if (srcvertex)
      *srcvertex = *vertex;

               //Compile facet arrays

    textcoord_t* temp   = (textcoord_t*)pool->alloc (vcount * 3 * sizeof (textcoord_t));
    int*         mirror = (int*)pool->alloc (vcount * 3 * sizeof (int));

    if (!temp || !mirror)
    {
      if (!temp)   D_NoMemory ("ConvertArrays",vcount * 3 * sizeof (textcoord_t));
      if (!mirror) D_NoMemory ("ConvertArrays",vcount * 3 * sizeof (int));

      if (srcfacet && *srcfacet) 
      {
        pool->free (*srcfacet);
        *srcfacet = NULL;
      }  
      if (srcvertex) 
        *srcvertex = NULL;

      if (temp) pool->free (temp);
      if (mirror) pool->free (mirror);

      return 0;
    }

    int size            = ConvertFacetArray (farray,fcount,mirror,temp,vcount * 3,vcount);

    vertex_t*    new_vertex = (vertex_t*)pool->alloc (size * sizeof (vertex_t));
    vector3d_t*  new_normal = NULL;
    if (normal)
      new_normal = (vector3d_t*)pool->alloc (size * sizeof (vector3d_t));
    textcoord_t* new_tcoord = (textcoord_t*)pool->alloc (size * sizeof (textcoord_t));

    if (!new_vertex || (!new_normal && normal) || !new_tcoord)
    {
      if (!new_vertex)   D_NoMemory ("ConvertArrays",size * sizeof (vertex_t));
      if (!new_normal && normal)   D_NoMemory ("ConvertArrays",size * sizeof (vector3d_t));
      if (!new_tcoord)   D_NoMemory ("ConvertArrays",size * sizeof (textcoord_t));

      if (srcfacet && *srcfacet) 
      {
        pool->free (*srcfacet);
        *srcfacet = NULL;
      }  
      if (srcvertex) 
        *srcvertex = NULL;

      if (temp) pool->free (temp);
      if (mirror) pool->free (mirror);
      if (new_vertex) pool->free (new_vertex);
      if (new_normal) pool->free (new_normal);
      if (new_tcoord) pool->free (new_tcoord);    

      return 0;
    }

    *tcoord = new_tcoord;

    memcpy (new_tcoord,temp,size * sizeof (textcoord_t));

    if (normal)  
      for (int i=0;i<size;i++)
      {   
        new_vertex [i] = (*vertex) [mirror [i]];
        new_normal [i] = (*normal) [mirror [i]];
      }
    else  
      for (int i=0;i<size;i++)
        new_vertex [i] = (*vertex) [mirror [i]];

    pool->free (mirror);
    pool->free (temp);

    if (eClear)
      if (normal) pool->free (*normal);

    *vertex    = new_vertex;
    if (normal) *normal    = new_normal;

    return size;
  }
  D_ExcRet ("::Error at ConvertArrays",0);
}

void    CreateMatFile   (Object3D& obj,const char* mat_file)
{
  ofstream      os    (mat_file,ios::out);

  int mcount = obj.count (Object3D::MATERIAL); 

  for (int i=0;i<mcount;i++)
  {
    textmat_t  mat = obj.GetMaterial (i);

    os<<"material {"<<endl;
    os<<"\tname "<<'"'<<mat.name<<'"'<<endl<<endl;

    if (mat.text [0]) os<<"\tdiffuse_map "<<'"'<<mat.text<<'"'<<endl;
    if (mat.alp  [0]) os<<"\talpha_map "<<'"'<<mat.alp<<'"'<<endl;
    os<<endl;

    mat.m.diffuse  *= 256.0f;
    mat.m.specular *= 256.0f;
    mat.m.ambient  *= 256.0f;
    mat.m.emission *= 256.0f;

    os<<"\tdiffuse "<<(int)mat.m.diffuse.x<<" "<<(int)mat.m.diffuse.y<<" "<<(int)mat.m.diffuse.z<<" "<<endl;
    os<<"\tspecular "<<(int)mat.m.specular.x<<" "<<(int)mat.m.specular.y<<" "<<(int)mat.m.specular.z<<" "<<endl;
    os<<"\tambient "<<(int)mat.m.ambient.x<<" "<<(int)mat.m.ambient.y<<" "<<(int)mat.m.ambient.z<<" "<<endl;
    os<<"\temission "<<(int)mat.m.emission.x<<" "<<(int)mat.m.emission.y<<" "<<(int)mat.m.emission.z<<" "<<endl<<endl;

    os<<"\tshiness "<<mat.m.shiness<<endl;

    os<<"\talpha_koef "<<1<<endl;
    os<<"\talpha_present "<<mat.m.blend<<endl;

    os<<"}"<<endl<<endl;
  }  
}
