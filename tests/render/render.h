#include "defs.h"
#include "material.h"

class Context
{
  public:
    void BeginDraw  (); //target
    void EndDraw    ();

    void SetMaterial    (Material*);

    //create ???
    Texture2D* CreateTexture (Buffer&);
    VB*        CreateVB      (Buffer&);
    IB*        CreateIB      (Buffer&);

  private:
    void render           (Primitive* primitives,uint count);

    void MaterialSort     (Primitive*,uint,bool);
    void RenderPrimitives (Primitive*,uint);

    void NextFilter       (Primitive*,uint,bool);

  private:       
    enum { FIRST = 0, CURRENT, LAST };
    struct Filter;

    Filter*   mFilter [3];
    Material* mMaterial;
};


