#include <kernel.h>
#include <graph\wxfimp.h>

void     Test (WXFNode*);
void     Test (WXFHelper*);
void     Test (WXFMesh*);
void     Test (WXFMaterial*);

void     main ()
{
  K_FastInit (NULL);

  WXFImport mdl ("q:/tests/3dsmax/data/1.wxf");

  dout<<"lock"<<endl;

  float start = clock ();

  cout<<mdl.lock ()<<endl;

  float end = clock ();

  dout<<"time: "<<(end-start)/float (CLK_TCK)<<endl;

  dout<<"Nodes"<<endl;

  Test (mdl.root ());
}

void     Test (WXFNode* node)
{
  dout<<node->name;

  if (node->parent && node->parent->name [0])  dout<<"\t\t"<<node->parent->name<<endl;
  else                                   dout<<(node->parent?"\t\troot":"")<<endl;

  if (node->mtl) 
    Test (node->mtl);

  dout<<node->base<<endl;

/*  switch (node->type)
  {
    case WXFNode::MESH:
         Test (node->data.mesh);
         break;
    case WXFNode::HELPER:
         Test (node->data.helper);
         break;
  }*/

  Ring<WXFNode*>::iter iter  = node->children;
  uint                 count = node->children.count ();

  for (int i=0;i<count;i++,iter++)
    Test (iter);
}

void    Test  (WXFMap* tex)
{
  dout<<"texture:\t\t"<<tex->bmp.name<<endl;

  Ring<WXFMap*>::iter iter  = tex->maps;
  uint                count = tex->maps.count ();

  for (int i=0;i<count;i++,iter++)
    Test (iter);  
}

void     Test (WXFMaterial* mtl)
{
  dout<<"material:\t"<<mtl->name;

  if (mtl->parent && mtl->parent->name [0])  dout<<"\t\t"<<mtl->parent->name<<endl;
  else                                   dout<<"\t\troot"<<endl;

  static char* mapType [WXFMaterial::MAPS_NUM] =  {
    "Diffuse",
    "Opacity",
    "Bump"
  };

  for (int i=0;i<WXFMaterial::MAPS_NUM;i++)
    if (mtl->maps [i])
    {
      dout<<"map_type:\t\t"<<mapType [i]<<endl;
      Test (mtl->maps [i]);
    }  

  Ring<WXFMaterial*>::iter iter  = mtl->children;
  uint                     count = mtl->children.count ();

  for (i=0;i<count;i++,iter++)
    Test (iter);
}

void    Test (WXFHelperBox* hlp)
{
  dout<<"\t\tBox:\t\t"<<hlp->c<<"\t"<<hlp->dir<<endl;   
}

void    Test (WXFHelperDummy* hlp)
{
  dout<<"\t\tDummy:\t\t"<<hlp->c<<"\t"<<hlp->dir<<endl;   
}

void    Test (WXFHelperSphere* hlp)
{
  dout<<"\t\tSphere:\t\t"<<hlp->c<<"\t"<<hlp->r<<endl;   
}

void    Test (WXFHelperCylinder* hlp)
{
  dout<<"\t\tCylinder:\t\t"<<hlp->c<<"\t"<<hlp->dir<<"\t"<<hlp->height<<"\t"<<hlp->r<<endl;   
}

void    Test (WXFHelper* hlp)
{
  switch (hlp->type)
  {
    case WXFHelper::BOX:      Test ((WXFHelperBox*)hlp);
                              break;
    case WXFHelper::DUMMY:    Test ((WXFHelperDummy*)hlp);
                              break;
    case WXFHelper::SPHERE:   Test ((WXFHelperSphere*)hlp);
                              break;
    case WXFHelper::CYLINDER: Test ((WXFHelperCylinder*)hlp);
                              break;
  }
}

void    Test (WXFMesh* mesh)
{
//    dout<<"tcoords_num: "<<mesh->vertexes.tcount<<endl;

  if (mesh->vertexes.tcount)
  {
    tcoord_t* t = mesh->vertexes.tcoords;

    for (int i=0;i<mesh->vertexes.tcount;i++,t++)
    {
//        dout<<t->u<<" "<<t->v<<endl;
    }
  }
}