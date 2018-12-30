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
                  MAX_MAPS = 100 //��� � MAX'�
                };

    struct      mapchannel_t  //������ �������
    {
      int         channel;    //����� ������
      uint        vcount,     //���-�� ⥪����஢���� ���設
                  fcount;     //���-�� ⥪����஢����� �࠭��
      tcoord_t*   verts;      //⥪��. ���設�
      tindex_t*   faces;      //������� ��� ������ �࠭�

                  mapchannel_t ();
                  ~mapchannel_t ();
    };

    struct vbuf_t //���設�� ����
    {
      uint        count,        //���-�� ���設
                  ncount;       //���-�� ��ଠ���

      WXFVertex*  verts;        //���設�
      normal_t*   normals;      //��ଠ��

      mapchannel_t* channels [MAX_MAPS]; //������ �������

                    vbuf_t ();
    }vertexes;

    struct faces_t //�࠭�
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