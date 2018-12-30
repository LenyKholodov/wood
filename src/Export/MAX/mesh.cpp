#include <pch.h>

#include "mesh.h"
#include "geom.h"

TriObject* MeshExport::GetTriObject (bool& del)
{
  del = false;

  Object* obj = node.EvalWorldState(time).obj;

  if (obj->CanConvertToType (Class_ID (TRIOBJ_CLASS_ID,0))) 
  { 
    TriObject* tri = (TriObject*)obj->ConvertToType (time,Class_ID(TRIOBJ_CLASS_ID, 0));

    if (tri && obj != tri) 
      del = true;

    return tri;
  }
  else return NULL;
}

void  MeshExport::VertexesExport (Mesh& mesh,Matrix3& tm,Matrix3& invmat,AffineParts& ap)
{
  OpenFrame ("vertexes");

  bool res = false;

  if (info.bin.check (BIN_VERTEXES))
  {
    vertex_t* vrtx = ext_new vertex_t [mesh.getNumVerts()];

    if (vrtx)
    {
      vertex_t* v = vrtx;
    
      for (int i=0;i<mesh.getNumVerts();i++,v++) 
      {
        //Point3 p = tm * mesh.verts[i];
        Point3     p  = invmat.PointTransform (tm * mesh.verts [i]);

        v->x = -1.0 * p.x * ap.k.x;
        v->y = p.z * ap.k.z;
        v->z = p.y * ap.k.y;        
      }

      dump.WriteArray ("vertex",vrtx,mesh.getNumVerts ());

      res = true;

      delete [] vrtx;
    }
  }

  if (!res)
  {
    for (int i=0;i<mesh.getNumVerts();i++) 
    {
      //Point3 v = tm * mesh.verts[i];
      Point3 v  = invmat.PointTransform (tm * mesh.verts [i]);

      v.x *= ap.k.x;
      v.y *= ap.k.y;
      v.z *= ap.k.z;

  //    dump<<"vertex "<<i<<"\t"<<-1.0*v.x<<"\t"<<v.z<<"\t"<<v.y<<endlineine;
      dump<<"vertex\t"<<v<<endline;
    }
  }

  CloseFrame ();
}

void  MeshExport::FacesExport  (Mesh& mesh,int* order)
{
  OpenFrame ("faces");

  bool res = false;

  if (info.bin.check (BIN_FACES))
  {
    WXFFace* faces = ext_new WXFFace [mesh.getNumFaces()];

    if (faces)
    {
      WXFFace* f = faces;

      if (mesh.getNumTVerts())
      {
        for (int i=0;i<mesh.getNumFaces();i++,f++) 
        {
          f->index  [0] = mesh.faces [i].v [order [0]];
          f->index  [1] = mesh.faces [i].v [order [1]];
          f->index  [2] = mesh.faces [i].v [order [2]];

          Point3& n     = mesh.getFaceNormal(i);

          f->n.x        = -n.x;
          f->n.y        = n.z;
          f->n.z        = n.y;
        }   
      }
      else
      {
        for (int i=0;i<mesh.getNumFaces();i++,f++) 
        {
          f->index  [0] = mesh.faces [i].v [order [0]];
          f->index  [1] = mesh.faces [i].v [order [1]];
          f->index  [2] = mesh.faces [i].v [order [2]];

          Point3& n     = mesh.getFaceNormal(i);

          f->n.x        = -n.x;
          f->n.y        = n.z;
          f->n.z        = n.y;
        }   
      }

      dump.WriteArray ("face",faces,mesh.getNumFaces());

      delete [] faces;

      res = true;
    }  
  }

  if (!res)
  {
    for (int i=0;i<mesh.getNumFaces();i++) 
    {
      dump<<"face"
          <<"\t"<<mesh.faces [i].v [order [0]]
          <<"\t"<<mesh.faces [i].v [order [1]]
          <<"\t"<<mesh.faces [i].v [order [2]]        
          <<"\t"<<mesh.faces[i].getMatID()
          <<endline;
    }
  }

  CloseFrame ();
}

void MeshExport::MakeFaceUV (Face& f,Point3 *tv)
{
  static int nextpt[3] = {1,2,0};
  static int prevpt[3] = {2,0,1};
  static Point3 basic_tva[3] = { 
          Point3(0.0,0.0,0.0),Point3(1.0,0.0,0.0),Point3(1.0,1.0,0.0)
  };
  static Point3 basic_tvb[3] = { 
          Point3(1.0,1.0,0.0),Point3(0.0,1.0,0.0),Point3(0.0,0.0,0.0)
  };

  /* make the invisible edge be 2->0 */

  int nhid = 2;

  if (!(f.flags&EDGE_A))      nhid=0;
  else if (!(f.flags&EDGE_B)) nhid = 1;
  else if (!(f.flags&EDGE_C)) nhid = 2;

  int na = 2 - nhid;

  Point3* basetv = (f.v [prevpt [nhid]] < f.v [nhid]) ? basic_tva : basic_tvb; 

  for (int i=0; i<3; i++) 
  {  
    tv[i] = basetv [na];
    na    = nextpt [na];
  }
}

void  MeshExport::TexCoordsExport (Mesh& mesh,Mtl& mtl,int* order)
{
  if (!check (EXPORT_TEXTURE_COORDS))
    return;

  if (!&mtl || !&mesh)
    return;

  ULONG matreq = mtl.Requirements(-1);

  if (matreq & MTLREQ_FACEMAP)
  {
    dump<<"face_map_num "<<mesh.getNumFaces()<<endline;

    OpenFrame ("face_map_list");

    bool res = false;

    if (info.bin.check (BIN_TEXCOORDS))
    {
      WXFFaceMap* verts = ext_new WXFFaceMap [mesh.getNumFaces ()];

      if (verts)
      {
        Point3 tv [3];
            
        for (int i=0;i<mesh.getNumFaces();i++) 
        {
          MakeFaceUV (mesh.faces[i],tv);

          for (int j=0;j<3;j++)
          {
            verts [i][j].u = tv [j].x;
            verts [i][j].v = tv [j].y;
          }
        }   

        dump.WriteArray ("face_map",verts,mesh.getNumFaces ());

        res = true;
      }
    }
    
    if (!res)
    {
      Point3 tv [3];
          
      for (int i=0;i<mesh.getNumFaces();i++) 
      {
        MakeFaceUV (mesh.faces[i],tv);

          //Возможно здесь есть ошибка с order
        
        dump<<"face_map ";

        dump<<"\t"<<tv[0].x<<"\t"<<tv[0].y<<"\t"<<tv[0].z
            <<"\t\t"<<tv[1].x<<"\t"<<tv[1].y<<"\t"<<tv[1].z
            <<"\t\t"<<tv[2].x<<"\t"<<tv[2].y<<"\t"<<tv[2].z
            <<endline;
       
    }

      CloseFrame ();
    }

    CloseFrame ();
  }
  else
  {
    for (int mp=1;mp<MAX_MESHMAPS-1;mp++) 
      if (mesh.mapSupport (mp))
        ExportChannel (mesh,order,mp);
  }
}

void    MeshExport::ExportChannel (Mesh& mesh,int* order,int mp)
{
  int numTVx = mesh.getNumMapVerts(mp);

  if (numTVx)
  {
    OpenFrame ("map_channel");

    dump<<"map_channel\t"<<mp<<endline;

    dump<<"tex_coords_num\t"<<numTVx<<endline;
    dump<<"texmap_faces_num\t"<<numTVx<<endline;

    typedef   uint tpatch_t [3];

    bool      res    = false;
    tcoord_t* tcoord = NULL;
    tpatch_t* index  = NULL;

    if (info.bin.check (BIN_TEXCOORDS))
    {
      tcoord_t* tcoord = ext_new tcoord_t [numTVx];
      tpatch_t* index  = ext_new tpatch_t [mesh.getNumFaces()];

      if (tcoord && index)
      {
        tcoord_t* t      = tcoord;
        UVVert* tv = mesh.mapVerts(mp);

        for (int i=0;i<numTVx;i++,t++,tv++) 
        {
          t->u = tv->x;
          t->v = 1.0-tv->y;
        }            

        tpatch_t* ind = index;

        for (i=0;i<mesh.getNumFaces();i++,ind++) 
        {
          (*ind) [0] = mesh.tvFace[i].t[order [0]];
          (*ind) [1] = mesh.tvFace[i].t[order [1]];
          (*ind) [2] = mesh.tvFace[i].t[order [2]];
        }

        OpenFrame ("tex_coords");    
        dump.WriteArray ("tex_coord",tcoord,numTVx);
        CloseFrame ();
        OpenFrame ("texmap_faces");
        dump.WriteArray ("tex_index",index,mesh.getNumFaces());
        CloseFrame ();

        res = true;
      }

      if (tcoord) delete [] tcoord;
      if (index)  delete [] index;
    }

    if (!res)
    {
      OpenFrame ("tex_coords");    

      UVVert* tv = mesh.mapVerts(mp);

      for (int i=0;i<numTVx;i++,tv++) 
        dump<<"tex_coord "<<tv->x<<"\t"  <<(1-tv->y)<<endline;

      CloseFrame (); 

      OpenFrame ("texmap_faces");

      for (i=0;i<mesh.getNumFaces(); i++) 
      {
        dump<<"tex_index "<<mesh.tvFace[i].t[order [0]]
            <<"\t"<<mesh.tvFace[i].t[order [1]]
            <<"\t"<<mesh.tvFace[i].t[order [2]]
            <<endline;
      }

      CloseFrame ();     
    }        
      
    CloseFrame ();  
  }
}

void  MeshExport::VertexColorsExport (Mesh& mesh,int* order)
{
  if (!check (EXPORT_VERTEX_COLORS) ||  !mesh.numCVerts )  
    return;

  dump<<"color_faces_num\t"<<mesh.getNumFaces()<<endline;
  dump<<"color_vertexes_num\t"<<mesh.numCVerts<<endline;

  bool res = false;

  if (info.bin.check (BIN_COLORS))
  {
    typedef uint cpatch_t [3];
    color3d_t* color = ext_new color3d_t [mesh.numCVerts];
    cpatch_t*  index = ext_new cpatch_t [mesh.getNumFaces()];

    if (color && index)
    {
      color3d_t* c = color;

      for (int i=0;i<mesh.numCVerts;i++,c++) 
      {
        c->x = mesh.vertCol [i].x;
        c->y = mesh.vertCol [i].y;
        c->z = mesh.vertCol [i].z;
      }

      cpatch_t* ind = index;

      for (i=0;i<mesh.getNumFaces();i++,ind++) 
      {
        (*ind) [0] = mesh.vcFace [i].t [order [0]];
        (*ind) [1] = mesh.vcFace [i].t [order [1]];
        (*ind) [2] = mesh.vcFace [i].t [order [2]];
      }
      
      OpenFrame ("color_vertexes");            
      dump.WriteArray ("color_vertex",color,mesh.numCVerts);
      CloseFrame ();

      OpenFrame ("color_faces");
      dump.WriteArray ("color_face",index,mesh.getNumFaces());
      CloseFrame ();

      res = true;

      delete [] color;
      delete [] index;
    }
    else
    {
      if (color) delete [] color;
      if (index) delete [] index;
    }
  }

  if (!res)
  {
    OpenFrame ("color_vertexes");

    for (int i=0;i<mesh.numCVerts;i++) 
    {
      dump<<"color_vertex "<<i
          <<"\t"<<mesh.vertCol [i].x
          <<"\t"<<mesh.vertCol [i].y
          <<"\t"<<mesh.vertCol [i].z
          <<endline;
    }

    CloseFrame ();

    OpenFrame ("color_faces");

    for (i=0;i<mesh.getNumFaces();i++) 
    {
      dump<<"color_face "<<i
          <<"\t"<<mesh.vcFace [i].t [order [0]]
          <<"\t"<<mesh.vcFace [i].t [order [1]] 
          <<"\t"<<mesh.vcFace [i].t [order [2]]
          <<endline;
    }

    CloseFrame ();
  }
}

Point3 MeshExport::GetVertexNormal (Mesh* mesh,int faceNo,RVertex* rv)
{
  Face& f       = mesh->faces[faceNo];
  DWORD smGroup = f.smGroup;
  int    numNormals;

  // Is normal specified
  // SPCIFIED is not currently used, but may be used in future versions.
  if (rv->rFlags & SPECIFIED_NORMAL)  return rv->rn.getNormal();
  // If normal is not specified it's only available if the face belongs
  // to a smoothing group
  else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) 
  {
    // If there is only one vertex is found in the rn member.
    if (numNormals == 1) return rv->rn.getNormal();
    else 
    {
      // If two or more vertices are there you need to step through them
      // and find the vertex with the same smoothing group as the current face.
      // You will find multiple normals in the ern member.

      Point3 vertexNormal;

      for (int i=0;i<numNormals;i++) 
          if (rv->ern [i].getSmGroup() & smGroup) 
            vertexNormal = rv->ern[i].getNormal(); 

      return vertexNormal;
    }
  }   
  else 
    return mesh->getFaceNormal(faceNo); // Get the normal from the Face if no smoothing groups are there
}

void  MeshExport::NormalsExport   (Mesh& mesh,int* order)
{
  if (!check (EXPORT_NORMALS))    
    return;

  dump<<"vrtx_normals_num\t"<<mesh.normalCount<<endline;
  dump<<"face_normals_num "<<mesh.getNumFaces()<<endline;

  OpenFrame ("face_normals");

  bool res = false;

  if (info.bin.check (BIN_NORMALS))
  {
    vector3d_t* norm = ext_new vector3d_t [mesh.getNumFaces()];

    if (norm)
    {
      vector3d_t* n = norm;      

      for (int i=0;i<mesh.getNumFaces();i++,n++) 
      {
        Point3  fn = mesh.getFaceNormal(i);

        n->x       = -1.0*fn.x;
        n->y       = fn.z;
        n->z       = fn.y;
      }

      dump.WriteArray ("normal",norm,mesh.getNumFaces());

      delete [] norm;
      res = true;
    }    
  }

  if (!res)
  {
    Point3 fn;  // Face normal
    Point3 vn;  // Vertex normal
    int    vert;
    Face*  f;

    for (int i=0;i<mesh.getNumFaces();i++) 
    {
      Face&   f  = mesh.faces [i];
      Point3  fn = mesh.getFaceNormal(i);

      dump<<"normal\t"<<fn<<endline;
    }

  }
    
  CloseFrame ();

  normal_t* norm = ext_new normal_t [mesh.getNumVerts ()];

  if (!norm)
    return;

  uint fcount = mesh.getNumFaces (),
       vcount = mesh.getNumVerts ();        

  for (int i=0;i<vcount;i++)
    norm [i] = vector3d_t (.0f);

  Face* f = mesh.faces;

  for (i=0;i<fcount;i++,f++)
  {
    for (int j=0;j<3;j++)
    {
      int ind   = f->v [order [j]];
      Point3 n  = GetVertexNormal (&mesh,i,mesh.getRVertPtr (f->getVert (order [j])));
  
      norm [ind].x += -n.x; 
      norm [ind].y += n.z;
      norm [ind].z += n.y;
    }
  }    

  OpenFrame  ("normals");

  if (info.bin.check (BIN_NORMALS))
  {
    normal_t* n = norm;

    for (i=0;i<vcount;i++,n++)
      n->normalize ();

    dump.WriteArray ("normal",norm,vcount);
  }  
  else
  {
    normal_t* n = norm;

    for (int i=0;i<vcount;i++,n++)
    {
      n->normalize ();
      dump<<"normal\t"<<n->x<<"\t"<<n->y<<"\t"<<n->z<<endline;
    }  
  }   

  CloseFrame ();
}

bool  MeshExport::IsNeg (Matrix3 &m)
{
  return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?1:0;
}


void  MeshExport::DoExport ()
{
  int     order [3];
  bool    delTriObj = false;

  Mtl*        mtl    = node.GetMtl ();
  Matrix3     tm     = node.GetObjTMAfterWSM (time); 
  ObjectState os     = node.EvalWorldState (time);
  TriObject* tri     = GetTriObject (delTriObj);

  if (IsNeg (tm)) 
  {
    order [0] = 2;
    order [1] = 1;
    order [2] = 0;
  }
  else 
  {
    order [0] = 0;
    order [1] = 1;
    order [2] = 2;
  }

  if (tri)
  {
    AffineParts ap;
    Mesh&       mesh   = tri->GetMesh ();
    Matrix3     pivot  = node.GetNodeTM (info.staticFrame);
    Matrix3     invmat = Inverse(pivot);

    mesh.buildNormals();

    decomp_affine (pivot, &ap);

    dump<<"time\t"<<time<<endline;
    dump<<"vertex_num\t"<<mesh.getNumVerts()<<endline;
    dump<<"faces_num\t"<<mesh.getNumFaces()<<endline;    

    VertexesExport  (mesh,tm,pivot,ap);
    FacesExport     (mesh,order);
    TexCoordsExport (mesh,*node.GetMtl(),order);  
    VertexColorsExport (mesh,order);
    NormalsExport   (mesh,order);

    if (delTriObj)
     delete tri;
  }
}

MeshExport::MeshExport  (BaseExport& exp,INode& n,TimeValue t)
           : BaseExport ("mesh",exp),
             node (n), time (t)
{
  DoExport ();
}
