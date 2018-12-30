#ifndef __GR_TYPES__
#define __GR_TYPES__

#include <math3d.h>
#include <types.h>

const    int            FACET_MAX_VERTEX_COUNT = 3;
const    int            TEXT_NAME_LEN          = 64;

typedef  vector3d_t     vertex_t;
typedef  vector3d_t     color3d_t;
typedef  vector3d_t     color3_t;
typedef  vector3d_t     point3d_t;
typedef  vector3d_t     normal_t;

struct   point2d_t
{
  int    x, y;
};

struct   rect_t
{
  int   x,y,width,height;

        rect_t (): x(0), y(0), width (0), height (0) {}
        rect_t (int _x,int _y,int _w,int _h): x(_x), y(_y), width (_w), height (_h) {}
};

struct   color4d_t
{
  float x,y,z,w;
};

typedef struct   textcoord_t
{
  float u,v;
}texcoord_t, tex2f_t, tcoord_t;

enum    FacetFlags
{
  ALREADY_DRAW          = 1,
  PORTAL_FACET          = 2,
  MIRROR_FACET          = 4,
  BLEND_FACET           = 8
};

struct   facet_t
{
  vector3d_t    n;
  int           index  [FACET_MAX_VERTEX_COUNT];
  textcoord_t   tindex [FACET_MAX_VERTEX_COUNT];
  int16         flags;
  int16         texture;                        //����� ⥪����� / ���ਠ��
};

struct   boundbox_t
{
  vector3d_t    min, max;        
};

#ifndef __BOUND_BASE__
#define __BOUND_BASE__

struct   boundbase_t
{
  vector3d_t    pos;
  vector3d_t    axis [3];
};

#endif

struct  boundsphere_t
{
  vector3d_t    center;
  float         r;
};

struct  bound_t
{
  boundbox_t     box;
  boundsphere_t  sphere;
};

struct  material_t
{
  vector3d_t    ambient;        //������
  vector3d_t    diffuse;        //����㧭�
  vector3d_t    specular;       //��ઠ���
  vector3d_t    emission;       //�����⥫��
  float         shiness;        //�⥯��� ��ࠦ����
  BOOL          blend;          //������⢨� �஧�筮��
  
                material_t (): ambient (.0f), diffuse (.8f), specular (.0f),
                               emission (.0f),shiness (1.0f), blend (FALSE) {}
}; 

struct textmat_t
{
  char          text [TEXT_NAME_LEN];   //��� ⥪�����
//  char          bump [TEXT_NAME_LEN];   //��� ⥪�����
//  char         spec  [TEXT_NAME_LEN];   //��� ⥪�����
  char          name [TEXT_NAME_LEN];   //��� ���ਠ��
  char          alp  [TEXT_NAME_LEN];   //��� �����
  material_t    m;                      //��।������ ���ਠ��

                textmat_t () { text [0] = 0; name [0] = 0; alp [0] = 0; }
                textmat_t (int);
};

struct  light_t
{
  vector3d_t    pos;            //���न����
  vector3d_t    dir;            //���ࠢ�����
  vector3d_t    diffuse;        //����㧭�� �ᢥ饭��
  vector3d_t    ambient;        //����ﭮ� �ᢥ饭��
  vector3d_t    specular;       //��ઠ�쭮� �ᢥ饭��
  float         shiness;        //�મ���
  float         angle;          //���� (ࠤ����) 
  BOOL          farlight;       //������ �� ���筨�
};

struct  portal_t
{
   vector3d_t   v [FACET_MAX_VERTEX_COUNT];
   vector3d_t   n;
   int          flags;
   int          index;
};

struct  plane_t
{
  float a, b, c, d;
};

struct  line_t
{
  float a, b, c;
};

void         normalize      (plane_t&);
void         normalize      (line_t&);

struct  frustum_t
{
  plane_t clip [6];
};

struct  portalbound_t
{
  plane_t    clip [3];
  portal_t   portal;
};

#ifndef __RGB_STRUCT_DEFINE__
#define __RGB_STRUCT_DEFINE__

struct  rgb_t
{
  uchar  red;
  uchar  green;
  uchar  blue;
};

struct  rgba_t
{
  uchar  red;
  uchar  green;
  uchar  blue;
  uchar  alpha;        
};

#endif

struct  bitmap_t
{
  int   width;
  int   height;
  int   bpp;
  char  data [];
};

#ifndef __RENDER_MESH__
#define __RENDER_MESH__

#pragma pack (push)
#pragma pack (4)

struct  mesh_t
{
  size_t        size;       //������ ���
  uint16        type;       //��� (GL_TRIANGLES, GL_TRIANGLE_STRIP)
  uint16        vbuffer;    //������ VertexBuffer'�
  uint16        elType;     //��� ����� (uint,uint16,byte,vertex,etc.)
  uint          start, end; //������ ��砫쭮� � ����筮� ���設
  uint          index;      //������ ���
  uint          count;      //������⢮ ����⮢ � ���
  int           material;   //���ਠ� ���
  char          data [];    //�����
};

struct   multimesh_t
{
  int           num;        //������⢮ ��襩
  mesh_t*       mesh () const { return (mesh_t*)((char*)this+sizeof (multimesh_t)); }
};

#pragma pack (pop)

#endif


#ifdef DEBUG
  #pragma comment (lib,"3dgld.lib")
#else
  #pragma comment (lib,"3dgl.lib")
#endif

#endif