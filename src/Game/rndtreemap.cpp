#include "pch.h"
#include "cv_treecomp.h"
#include "r_treeprecomp.h"
#include "g_level.h"
#include "g_map.h"

#include <kernel.h>

float    fuck_rnd () { return (float)random (65355)/65355.0f; }

void     SaveLoad       ();
void     OnlyLoad       ();
void     LoadAndPrecomp ();
void     LoadBox        (GameLevel&);

void     main ()
{
//  LoadAndPrecomp ();
  SaveLoad ();
}

boundbox_t      box;

void     SaveLoad ()
{
  K_Init  ();
  D_Load  ("rndtreemap.map");

  FloatPool    pool (M_GetPool(),16*1024*1024);
  BinFile*     file  = F_OpenFile ("maps/park.map",BF_OPEN_RW);
  GameLevel*   level = new (&pool) GameLevel (&pool,file);

  M_PushPool (&pool);

  TreeCompiler  comp (M_GetPool(),16,16);  

  TREEGROUP group = comp.CreateTreeGroup ();
  TREE      tree  = comp.CreateTree      ("models/tree2.wmd");
  TREE      tree1 = comp.CreateTree      ("models/palma.wmd");
//  TREE      tree  = comp.CreateTree      ("models/baba.wmd");
  TREE      tree2 = comp.CreateTree      ("models/tree3.wmd");
  TREE      tree3 = comp.CreateTree      ("models/tree4.wmd");
  TREE      tree4 = comp.CreateTree      ("models/tree5.wmd");

  LoadBox (*level);

  for (int i=0;i<16;i++)
  {
    float x = fuck_rnd () * box.max.x * 2,
          y = fuck_rnd () * box.max.z * 2;
    comp.AddTree (group,tree4,x,y);
  }  

/*  comp.AddTree      (group,tree4,0,0);*/

//  comp.AddTree      (group,tree1,0,0);
/*  comp.AddTree      (group,tree,0,0);
  comp.AddTree      (group,tree1,100,100);
  comp.AddTree      (group,tree,-100,100);
  comp.AddTree      (group,tree1,45,25);
  comp.AddTree      (group,tree2,150,25);
  comp.AddTree      (group,tree2,25,150);*/

//  comp.AddTree      (group,tree,0,0);
//  comp.AddTree      (group,tree,-10,10);
//  comp.AddTree      (group,tree1,45,25);
//  comp.AddTree      (group,tree2,15.0,25);
//  comp.AddTree      (group,tree2,25,15.0);

//  comp.AddTree      (group,tree,10,12);
//  comp.AddTree      (group,tree1,28,26);
//  comp.AddTree      (group,tree,14,20);

   comp.AddTreeGroup (group,0,0);  //!!
////  comp.AddTreeGroup (group,60,75);  //!!

//  comp.AddTreeGroup (group,89,45);
//  comp.AddTreeGroup (group,55,105);

//  comp.AddTreeGroup (group,15.12*5,30.25*5);  //Hey!
//  comp.AddTreeGroup (group,17.12*5,30.25*5);  //Hey!
//  comp.AddTreeGroup (group,15.12*5,32.25*5);   //Hey!
//  comp.AddTreeGroup (group,13.12*5,28.00*5);  //Hey!

/*  comp.AddTreeGroup (group,1512,3025);
  comp.AddTreeGroup (group,1712,3025);
  comp.AddTreeGroup (group,1512,3225);
  comp.AddTreeGroup (group,1312,2800);*/

  comp.Save (level);  

  delete level;
  delete file;

/*  level = new (&pool) GameLevel (&pool,"maps/park.map");

  comp.Load (level);

  char buf [126];
  RingTmpl<tree_t> ring (M_GetPool());
  RingTmpl<treegroup_t> ring1 (M_GetPool());

  comp.GetTree (tree,buf); 
  comp.GetTreeGroup (group,ring); 
  comp.GetMap       (ring1); 
  
  cout<<buf<<endl;

  RingTmpl<tree_t>::iterator iter = ring;
  int                count = ring.count ();

  for (int i=0;i<count;i++,iter++)
    cout<<"Tree: "<<iter.data().tree
        <<" X: "<<iter.data().pos.x
        <<" Y: "<<iter.data().pos.y
        <<endl;

  RingTmpl<treegroup_t>::iterator iter1 = ring1;
  count                                 = ring1.count ();

  for (i=0;i<count;i++,iter1++)
    cout<<"Group: "<<iter1.data().group
        <<" X: "<<iter1.data().pos.x
        <<" Y: "<<iter1.data().pos.y
        <<endl;


  M_PopPool ();

  delete level;
//  delete file;*/

  D_Done ();
  K_Done ();
}

void     OnlyLoad ()
{
  FloatPool    pool (M_GetPool(),16*1024*1024);
  GameLevel*   level = new (&pool) GameLevel (&pool,"maps/park.map");

  M_PushPool (&pool);

  TreeCompiler  comp (M_GetPool(),level);  

  char buf [126];
  RingTmpl<tree_t> ring (M_GetPool());
  RingTmpl<treegroup_t> ring1 (M_GetPool());

  TREE tree = 0;
  TREEGROUP group = 1;

  comp.GetTree      (tree,buf); 
  comp.GetTreeGroup (group,ring); 
  comp.GetMap       (ring1); 
  
  cout<<buf<<endl;

  RingTmpl<tree_t>::iterator iter = ring;
  int                count = ring.count ();

  for (int i=0;i<count;i++,iter++)
    cout<<"Tree: "<<iter.data().tree
        <<" X: "<<iter.data().pos.x
        <<" Y: "<<iter.data().pos.y
        <<endl;

  RingTmpl<treegroup_t>::iterator iter1 = ring1;
  count                                 = ring1.count ();

  for (i=0;i<count;i++,iter1++)
    cout<<"Group: "<<iter1.data().group
        <<" X: "<<iter1.data().pos.x
        <<" Y: "<<iter1.data().pos.y
        <<endl;


  M_PopPool ();

  delete level;
}

void     LoadAndPrecomp ()
{
/*  FloatPool    pool (M_GetPool(),16*1024*1024);
  GameLevel*   level = new (&pool) GameLevel (&pool,"maps/park.map");

  M_PushPool (&pool);

  cout<<BuildTreePrecompile (level,&pool)<<endl;

  M_PopPool ();

  delete level;*/
}

void     LoadBox        (GameLevel& level)
{
  BinFile* file = level.CreateLump (LL_LANDSCAPE,GameLevel::OPEN_RO);

  grmapheader_t    map;  

  file->read (&map,sizeof (map));

  box = map.bbox;

  delete file;
}