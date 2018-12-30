#define  DEBUG
#include <graph\utils.h>
#include <debug.h>

void     main ()
{
  vertex_t v [3] = { vector3d_t (-10,-5,0),
                     vector3d_t (10,-5,0),
                     vector3d_t (0,10,0)
  };

/*  vertex_t v [3] = { vector3d_t (-10,0,0),
                     vector3d_t (10,0,0),
                     vector3d_t (0,10,0)
  };*/

  vertex_t tri   [3][3];
//  plane_t  plane = {0,1,0,0};
  plane_t  plane = {1,0,0,0};
//  plane_t  plane = {0,1,0,10};
  int      count = SplitFacet (v,plane,tri [0],tri [1],tri [2]);

  cout<<"Count: "<<count<<endl;

  switch (count)
  {
    case 1: cout<<"No split"<<endl; break;
    case 3: cout<<"Split "<<count<<" triangles:"<<endl;
            cout<<"\t"<<tri [2][0]<<" "<<tri [2][1]<<" "<<tri [2][2]<<endl;
    case 2:
            cout<<"\t"<<tri [0][0]<<" "<<tri [0][1]<<" "<<tri [0][2]<<endl;
            cout<<"\t"<<tri [1][0]<<" "<<tri [1][1]<<" "<<tri [1][2]<<endl;
            break;
  }    
}