#include <pch.h>
#include <import\wxf\wxfimp.h>
#include <import\wxf\wxfmesh.h>
#include <scripts\framerdr.h>

static istream& operator >> (istream& is,WXFFace& f)
{
  int text;
  return is>>f.index [0]>>f.index [1]>>f.index [2]>>text;
}

static istream& operator >> (istream& is,tcoord_t& t)
{
  return is>>t.u>>t.v;
}

static istream& operator >> (istream& is,tindex_t& t)
{
  return is>>t [0]>>t [1]>>t [2];
}

static istream& operator >> (istream& is,WXFFaceMap& fm)
{
  float z;

  for (int i=0;i<3;i++)
    is>>fm [i].u>>fm [i].v>>z;

  return is;
}

WXFMesh*   WXFMesh::Create (WXFMesh* mesh,FrameReader& rdr,WXFNode& node)
{
  if (!mesh)
  {  
    node.data.mesh = mesh = new (node.GetPool ()) WXFMesh (node.GetPool ());
    node.type      = WXFNode::MESH;

    if (!mesh)
      return NULL;

    rdr.DeclareFrame ("vertexes");
    rdr.DeclareFrame ("faces");
    rdr.DeclareFrame ("map_channel");
    rdr.DeclareFrame ("normals");
    rdr.DeclareFrame ("face_map_list");

    rdr.BindValue    ("vertex_num",mesh->vertexes.count);
    rdr.BindValue    ("faces_num",mesh->faces.count);
    rdr.BindValue    ("vrtx_normals_num",mesh->vertexes.ncount);

    rdr.BindCreater  ("vertexes",ReadVertexes,mesh);
    rdr.BindCreater  ("normals",ReadNormals,mesh);
    rdr.BindCreater  ("map_channel",ReadMapChannel,mesh);
    rdr.BindCreater  ("faces",ReadFaces,mesh);
    rdr.BindCreater  ("face_map_list",ReadFaceMap,mesh);
  }

  return mesh;
}

WXFMesh::vbuf_t*  WXFMesh::ReadVertexes (vbuf_t* vb,FrameReader& rdr,WXFMesh& mesh)
{
  if (!vb)
  {
    vb          = &mesh.vertexes;

    if (!vb->count)
      return NULL;

    vb->verts   = new (mesh.GetPool ()) WXFVertex [vb->count];

    if (!vb->verts)
      return NULL;

    rdr.BindArray ("vertex",vb->verts);
  }

  return vb;
}

WXFMesh::vbuf_t*  WXFMesh::ReadNormals  (vbuf_t* vb,FrameReader& rdr,WXFMesh& mesh)
{
  if (!vb)
  {
    vb          = &mesh.vertexes;

    if (!vb->ncount)
      return NULL;

    vb->normals = new (mesh.GetPool ()) normal_t  [vb->ncount];

    if (!vb->normals)
      return NULL;

    rdr.BindArray ("normal",vb->normals);
  }

  return vb;
}

WXFFaceMap*   WXFMesh::ReadFaceMap  (WXFFaceMap* fm,FrameReader& rdr,WXFMesh& mesh)
{
  if (!fm)
  {
    mesh.faces.faceMap = fm = new (mesh.GetPool ())  WXFFaceMap [mesh.faces.count];

    if (!fm)
      return NULL;

    rdr.BindArray ("face_map",mesh.faces.faceMap);  
  }

  return fm;
}

WXFMesh::mapchannel_t*  WXFMesh::ReadMapChannel (mapchannel_t* chan,FrameReader& rdr,WXFMesh& mesh)
{
  if (!chan)
  {
    chan = new (mesh.GetPool ()) mapchannel_t;

    if (!chan)
      return NULL;

    rdr.DeclareFrame ("tex_coords");
    rdr.DeclareFrame ("texmap_faces");

    rdr.BindCreater  ("tex_coords",ReadTexVerts,&mesh);
    rdr.BindCreater  ("texmap_faces",ReadTexFaces,&mesh);

    rdr.BindValue    ("tex_coords_num",chan->vcount);
    rdr.BindValue    ("texmap_faces_num",chan->fcount);
    rdr.BindValue    ("channel",chan->channel);
  }
  else
  {
    if (chan->channel < WXFMesh::MAX_MAPS && chan->channel > 0)
    {
      if (!mesh.vertexes.channels [chan->channel])
        mesh.vertexes.channels [chan->channel] = chan;
      else
      {
        delete mesh.vertexes.channels [chan->channel];          
        mesh.vertexes.channels [chan->channel] = chan;
      }  
    }  
    else
    {
      delete chan;      
      return NULL;
    }  
  }

  return chan;
}

tcoord_t*     WXFMesh::ReadTexVerts (tcoord_t* verts,FrameReader& rdr,WXFMesh& mesh)
{
  if (!verts)
  {
    mapchannel_t* chan = (mapchannel_t*)rdr.GetParentFrameObj ();
    chan->verts    = new (mesh.GetPool ()) tcoord_t [chan->vcount];

    if (!chan->verts)
      return NULL;

    rdr.BindArray ("tex_coord",chan->verts);
  }

  return verts;
}

tindex_t*     WXFMesh::ReadTexFaces (tindex_t* faces,FrameReader& rdr,WXFMesh& mesh)
{
  if (!faces)
  {
    mapchannel_t* chan = (mapchannel_t*)rdr.GetParentFrameObj ();
    chan->faces    = new (mesh.GetPool ()) tindex_t [chan->fcount];

    if (!chan->faces)
      return NULL;

    rdr.BindArray ("tex_index",chan->faces);
  }

  return faces;
}

WXFMesh::faces_t* WXFMesh::ReadFaces (faces_t* fb,FrameReader& rdr,WXFMesh& mesh)
{
  if (!fb)
  {
    fb          = &mesh.faces;

    fb->faces   = new (mesh.GetPool ()) WXFFace [fb->count];

    if (fb->count && !fb->faces)
      return NULL;

    if (fb->count)  rdr.BindArray ("face",fb->faces);
  }

  return fb;
}

WXFMesh::WXFMesh  (Pool* pool)
        : MemObject (pool)
{   }

WXFMesh::~WXFMesh ()
{
  if (vertexes.verts)    delete [] vertexes.verts;
  if (vertexes.normals)  delete [] vertexes.normals;
  if (faces.faces)       delete [] faces.faces;

  for (int i=0;i<MAX_MAPS;i++)
    if (vertexes.channels [i])
      delete vertexes.channels [i];
}

WXFMesh::vbuf_t::vbuf_t ()
{
  count  = 0;
  ncount = 0;
  verts  = NULL;
  normals = NULL;

  for (int i=0;i<MAX_MAPS;i++)
    channels [i] = NULL;
}

WXFMesh::mapchannel_t::mapchannel_t()
{
  vcount  = 0;
  fcount  = 0;
  verts   = NULL;
  faces   = NULL;
  channel = 0;
}

WXFMesh::mapchannel_t::~mapchannel_t ()
{
  if (verts) delete [] verts;
  if (faces) delete [] faces;
}
