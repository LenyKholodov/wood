#ifndef __WXF_IMPORT_NODE__
#define __WXF_IMPORT_NODE__

#include <import\wxf\wxfbase.h>
#include <import\wxf\wxfmesh.h>
#include <import\wxf\wxfhelper.h>
#include <misc.h>
#include <math3d.h>

class    WXFMaterial;
class    WXFMesh;
class    WXFHelper;
class    FrameReader;

typedef matrix_t   WXFMatrix;

/////////////////////////////////////////////////////////////////////////////////////////////
///Узел
/////////////////////////////////////////////////////////////////////////////////////////////
class   WXFNode: public WXFBase
{
  friend class WXFImport;
  public:

           enum DataType {
             NO_ASSIGN  = 0,
             MESH,
             HELPER
           };

    Str32                 name;         //имя
    WXFNode*              parent;       //предок
    Ring<WXFNode*>        children;     //потомки
    DataType              type;         //тип данных

    WXFMatrix             base;         //базис

    WXFMaterial*          mtl;          //материал
    color3d_t             wireColor;    //либо цвет 

    union {                             //данные
      WXFMesh*       mesh;
      WXFHelper*     helper;
      void*          ptr;
    }data;

  public:
          WXFNode  (WXFImport*);
          ~WXFNode ();  

  private:
    static   WXFNode*     Create          (WXFNode*,FrameReader&,WXFImport*);
    static   bool         ReadNodeName    (istream&,WXFNode*);
    static   bool         ReadNodeMat     (istream&,WXFNode*);
    static   bool         ReadNodeParent  (istream&,WXFNode*);
};

#endif
