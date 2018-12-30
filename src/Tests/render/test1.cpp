#include <render\node.h>
#include <kernel.h>

void     main ()
{
  K_FastInit (NULL);

  SetRenderPool (M_GetPool ());

  const uint TOTAL = 10;

  RenderNode   root;
  static RenderNode*  nodes [TOTAL]  = {NULL};
  RenderNode*  test = NULL;
  matrix_t     m    = matrix_t::translate (vector3d_t (1,2,3));
  matrix_t     m1   = matrix_t::scale     (vector3d_t (10,20,30));

  dout<<"root\t"<<&root<<endl;

//  float start = clock ();

  for (int i=0;i<TOTAL;i++)
  {
    nodes [i] = new RenderNode (root,m);  

    for (int j=0;j<10;j++)
      test = new RenderNode (*nodes [i],m1);

    dout<<i<<"\t"<<nodes [i]<<endl;
  }  

  dout<<test->GetWorldTM ()<<endl; 

//  float end = clock ();

//  cout<<"FPS:\t"<<float (TOTAL)/(end-start)*float (CLK_TCK)<<endl;
}