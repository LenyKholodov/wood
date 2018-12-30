#ifndef __WXF_IMPORT_MESH__
#define __WXF_IMPORT_MESH__

#include <import\wxf\wxfbase.h>
#include <memory\object.h>

typedef vertex_t WXFVertex;
typedef int      tindex_t [3];

struct  WXFFace
{
  int       index  [3];
  normal_t  n;
};

typedef tcoord_t WXFFaceMap [3];

class   WXFMesh: public MemObject
{
  friend class WXFNode;
  public:

                enum {
                  MAX_MAPS = 100 //как в MAX'е
                };

    struct      mapchannel_t  //каналы мапинга
    {
      int         channel;    //Номер канала
      uint        vcount,     //кол-во текстурированых вершин
                  fcount;     //кол-во текстурированиых граней
      tcoord_t*   verts;      //текст. вершины
      tindex_t*   faces;      //индексы для каждой грани

                  mapchannel_t ();
                  ~mapchannel_t ();
    };

    struct vbuf_t //вершинные буфера
    {
      uint        count,        //кол-во вершин
                  ncount;       //кол-во нормалей

      WXFVertex*  verts;        //вершины
      normal_t*   normals;      //нормали

      mapchannel_t* channels [MAX_MAPS]; //каналы мапинга

                    vbuf_t ();
    }vertexes;

    struct faces_t //грани
    {
      uint        count;
      WXFFace*    faces;
      WXFFaceMap* faceMap;
      
                  faces_t (): count (0), faces (NULL), faceMap (NULL) {}
    }faces;

  public:
                  WXFMesh  (Pool*);
                  ~WXFMesh ();

  private:
    static   WXFMesh*   Create          (WXFMesh*,FrameReader&,WXFNode&);
    static   faces_t*   ReadFaces       (faces_t*,FrameReader&,WXFMesh&);
                                        
    static   vbuf_t*    ReadVertexes    (vbuf_t*,FrameReader&,WXFMesh&);
    static   vbuf_t*    ReadNormals     (vbuf_t*,FrameReader&,WXFMesh&);

    static   mapchannel_t* ReadMapChannel (mapchannel_t*,FrameReader&,WXFMesh&);
    static   tcoord_t*     ReadTexVerts (tcoord_t*,FrameReader&,WXFMesh&); 
    static   tindex_t*     ReadTexFaces (tindex_t*,FrameReader&,WXFMesh&);

    static   WXFFaceMap*   ReadFaceMap  (WXFFaceMap*,FrameReader&,WXFMesh&);    
};

#endif