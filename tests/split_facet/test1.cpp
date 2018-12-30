#define  DEBUG
#include <graph\utils.h>
#include <debug.h>

typedef MemListTmpl<vertex_t> Facet;

void     main ()
{
//  vertex_t v [4] = { vector3d_t (-10,-5,0),
//                     vector3d_t (10,-5,0),
//                     vector3d_t (10,10,0),
//                     vector3d_t (-10,10,0)
//  };


  vertex_t v [4] = { vector3d_t (-10,-5,0),
                     vector3d_t (10,-5,0),
                     vector3d_t (10,0,0),
                     vector3d_t (-10,0,0)
  };

  vertex_t tri   [2][16];
  Facet    src   (4,v,4);
  Facet    left  (16,tri [0]),
           right (16,tri [1]);

//  plane_t  plane = {0,1,0,0};
  plane_t  plane = {0,0,1,0};
//  plane_t  plane = {0,1,0,10};

  int      count = SplitFacet (src,plane,left,right);

  cout<<"Count: "<<count<<endl;


  if (left.count ())
  {
    cout<<"Left: "<<left.count ()<<endl;

    for (int i=0;i<left.count ();i++)
      cout<<"\t\t"<<left [i]<<endl;
  }    

  if (right.count ())
  {
    cout<<endl<<"Right: "<<right.count ()<<endl;

    for (int i=0;i<right.count ();i++)
      cout<<"\t\t"<<right [i]<<endl;
  }    
}