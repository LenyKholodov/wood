#include <misc\rbtree.h>
#include <kernel.h>

class TestTree: public RedBlackTree
{
  public:

        struct node_t: public RedBlackTree::mapnode_t
        {
          int   key;
          int   data;
        };

    void insert (int key,int data = 0) {
      node_t* node = NULL;

      if (GetRoot ().valid ())
      {
//        dout<<"start search: "<<key<<endl;
        for (node = (node_t*)&GetRoot ();node->valid ();)
        {
//          dout<<"..."<<node->key<<endl;
          if (key == node->key)
          {
            node->data = data;
            dout<<"already exist "<<key<<endl;
            return ;
          }  
          else 
            if (key < node->key)  
               if (node->left->valid ())   node = (node_t*)node->left;
               else                        break;
            else if (key > node->key) 
                   if (node->right->valid ())  node = (node_t*)node->right;
                   else                        break;               
        }           
//        dout<<"end search: "<<key<<endl;
      }

//      if (node->valid ())
//        dout<<"Find parent: "<<node->key<<endl;

      node_t* ins = new node_t;

      if (!ins)
      {
        cout<<"no enough memory"<<endl;
        return ;
      }  

      ins->parent = node;
      ins->key    = key;
      ins->data   = data;

      if (node->valid ())
      {
//        dout<<"valid parent: "<<node->key<<" for child "<<key<<endl;
        if (key > node->key) node->right = ins;
        else                 node->left  = ins;
      }  
      else
      {
//        dout<<"eqe: "<<key<<endl;;
        SetFirst (*ins);        
      }  

      RedBlackTree::insert (*ins);
    }
    
    node_t* find (int key) const
    {
      for (node_t* node = (node_t*)&GetRoot ();node->valid ();)
        if      (key == node->key)  return node;    
        else 
          if (key >  node->key)  node = (node_t*)node->right;
          else                   node = (node_t*)node->left;         

      return NULL;
    }

    void erase (int key)
    {    
      node_t* node = find (key);

      if (!node)
        return;  

      RedBlackTree::erase (*node);

      delete node;  
    }

    void        dump (ostream& os) const
    {
      int h = dump (os,(node_t*)&GetRoot(),0);

      dout<<"Height: "<<h<<endl;
      dout<<"Left: "<<LeftCount ((node_t*)&GetRoot ())<<" right: "<<RightCount ((node_t*)&GetRoot ())<<endl;
      dout<<"Check: "<<check ()<<endl;
    }

    int  dump (ostream& os,node_t* node,int level) const
    {
      for (int i=0;i<level;i++) os<<"  ";
      int l = 0,r = 0;
      os<<"Node: "<<node->key<<" color: "<<node->color<<endl;
      for (i=0;i<level;i++) os<<"  ";
      os<<" left: "<<node->key<<endl;
      if (node->left->valid()) l=dump (os,(node_t*)node->left,level+1);
      for (i=0;i<level;i++) os<<"  ";
      os<<" right: "<<node->key<<endl;
      if (node->right->valid ()) r=dump (os,(node_t*)node->right,level+1);
      for (i=0;i<level;i++) os<<"  ";
      os<<"End node: "<<node->key<<endl;
      return !l&&!r ? level+1 : l>r?l:r;
    }

    int  RightCount (node_t* node) const
    {
      if (node->right->valid ())
        return RightCount ((node_t*)node->right) + (node->color == BLACK ? 1 : 0);
      else return 0;  
    }

    int  LeftCount (node_t* node) const
    {
      if (node->left->valid ())
        return LeftCount ((node_t*)node->left) + (node->color == BLACK ? 1 : 0);
      else return 0;  
    }

    bool check () const
    {     
      return check ((node_t*)&GetRoot ());
    }

    bool check (node_t* node) const
    {
      if (node->color == RED)
      {
        if (node->left->color != BLACK || node->right->color != BLACK)
          return false;
      }

      bool l = true, r = true;

      if (node->left->valid ()) l = check ((node_t*)node->left);
      if (node->right->valid ()) l = check ((node_t*)node->right);

      return l && r;
    }

    void swap (mapnode_t& t1,mapnode_t& t2)
    {
      node_t& x1 = (node_t&)t1;
      node_t& x2 = (node_t&)t2;

      ::swap (x1.data,x2.data);      
      ::swap (x1.key,x2.key);      

      dout<<"swap "<<x1.key<<" "<<x2.key<<endl;
      dout<<"--swap "<<x1.data<<" "<<x1.key<<endl;
      dout<<"--swap "<<x2.data<<" "<<x2.key<<endl;
    }
};

void    main ()
{
  K_FastInit ("test9.map");

  TestTree tree;

/*  tree.insert (1);
  tree.insert (2);
  tree.insert (4);
//  tree.dump  (dout); dout<<endl;
  tree.insert (5);
  tree.insert (7);
  tree.insert (8);
  tree.insert (11);
  tree.dump  (dout); dout<<endl;
  tree.erase (2);
  tree.dump  (dout); dout<<endl;*/
  
//  tree.insert (14);
//  tree.insert (15);
//  tree.insert (13);


  const int num = 0x10000;

  for (int i=0;i<num;i++)
    tree.insert (num-i,num-i);
//    tree.insert (i);

  tree.erase (64);
  tree.erase (32);
  tree.erase (75);
  tree.erase (7);
  tree.erase (5);

  tree.dump (dout);
}