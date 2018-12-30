#include <pch.h>
#include <misc\rbtree.h>

#define NIL &mNull

RedBlackTree::mapnode_t RedBlackTree::mNull (NULL,NIL,NIL,RedBlackTree::BLACK);

void    RedBlackTree::LeftRotate  (mapnode_t* x)
{
  mapnode_t* y = x->right;
  x->right     = y->left;

  if (y->left != NIL)
    y->left->parent = x;

//  if (y != NIL)
    y->parent = x->parent;

  if (!x->parent)  mRoot = y;
  else if (x == x->parent->left) x->parent->left  = y;
       else                      x->parent->right = y;
    
  y->left   = x;
  if (x!=NIL) x->parent = y;
}

void    RedBlackTree::RightRotate (mapnode_t* x)
{
  mapnode_t* y = x->left;
  x->left      = y->right;

  if (y->right != NIL)
    y->right->parent = x;

//  if (y!=NIL)
    y->parent = x->parent;

  if (!x->parent)  mRoot = y;
  else if (x == x->parent->right) x->parent->right = y;
       else                       x->parent->left  = y;

  y->right  = x;
  if (x!=NIL) x->parent = y;
}

void    RedBlackTree::insert (mapnode_t& _x)
{
  mapnode_t* x = &_x;

  x->color = RED;

  while (x != mRoot && x->parent->color == RED) 
  {
    if (x->parent == x->parent->parent->left) 
    {
      mapnode_t *y = x->parent->parent->right;

      if (y->color == RED) 
      {
        x->parent->color = BLACK;
        y->color = BLACK;
        x->parent->parent->color = RED;
        x = x->parent->parent;
      } 
      else 
      {
        if (x == x->parent->right) 
        {
          x = x->parent;
          LeftRotate(x);
        }

        x->parent->color = BLACK;
        x->parent->parent->color = RED;
        RightRotate(x->parent->parent);
      }
    } 
    else 
    {
      mapnode_t *y = x->parent->parent->left;
      if (y->color == RED) 
      {
        x->parent->color = BLACK;
        y->color = BLACK;
        x->parent->parent->color = RED;
        x = x->parent->parent;
      } 
      else 
      {
        if (x == x->parent->left) 
        {
          x = x->parent;
          RightRotate(x);
        }
        x->parent->color = BLACK;
        x->parent->parent->color = RED;
        LeftRotate(x->parent->parent);
      }
    }
  }

  mRoot->color = BLACK;
  mCount++;
}

void    RedBlackTree::erase  (mapnode_t& _z)
{
  mapnode_t* z = &_z;
  mapnode_t *y = NULL, *x = NULL;

  if (z->left == NIL && z->right == NIL) y = z;
  else                                   y = successor (z);

  if (y->left != NIL)  x = y->left;
  else                 x = y->right;

  x->parent = y->parent;

  if (!y->parent)  mRoot = x;
  else if (y == y->parent->left)  y->parent->left  = x;
       else                       y->parent->right = x;

  if (y->color == BLACK)
    DeleteFixup (x);

  if (y != z) 
  {
    this->swap (*y,*z);

    y->color  = z->color;
    y->left   = z->left;
    y->right  = z->right;
    y->parent = z->parent;

    if (!z->parent) mRoot = y;
    else if (z == z->parent->left) z->parent->left  = y;
         else                      z->parent->right = y;
    
    z->left->parent  = y;
    z->right->parent = y;         
  }  

  mCount--;
}

void    RedBlackTree::DeleteFixup (mapnode_t* x)
{
  while (x != mRoot && x->color == BLACK) 
  {
    if (x == x->parent->left) 
    {
      mapnode_t *w = x->parent->right;

      if (w->color == RED) 
      {
        w->color = BLACK;
        x->parent->color = RED;
        LeftRotate (x->parent);
        w = x->parent->right;
      }
      if (w->left->color == BLACK && w->right->color == BLACK) 
      {
        w->color = RED;
        x = x->parent;
      } 
      else 
      {
        if (w->right->color == BLACK) 
        {
          w->left->color = BLACK;
          w->color = RED;
          RightRotate (w);
          w = x->parent->right;
        }

        w->color = x->parent->color;
        x->parent->color = BLACK;
        w->right->color = BLACK;
        LeftRotate (x->parent);
        x = mRoot;
      }
    } 
    else 
    {
      mapnode_t *w = x->parent->left;

      if (w->color == RED) 
      {
        w->color = BLACK;
        x->parent->color = RED;
        RightRotate (x->parent);
        w = x->parent->left;
      }
      if (w->right->color == BLACK && w->left->color == BLACK) 
      {
        w->color = RED;
        x = x->parent;
      } 
      else 
      {
        if (w->left->color == BLACK) 
        {
          w->right->color = BLACK;
          w->color = RED;
          LeftRotate (w);
          w = x->parent->left;
        }

        w->color = x->parent->color;
        x->parent->color = BLACK;
        w->left->color = BLACK;
        RightRotate (x->parent);
        x = mRoot;
      }
    }
  }

  x->color = BLACK;
}

RedBlackTree::mapnode_t*  RedBlackTree::successor  (mapnode_t* node)
{
  if (node->right->valid ()) return &GetMin (*node->right);

  for (mapnode_t* parent = node->parent;parent && node == parent->right;
       node = parent,parent = parent->parent);

  return parent;
}

RedBlackTree::mapnode_t&  RedBlackTree::GetMin   (const mapnode_t& x) const
{
  return x.GetMin ();
}

RedBlackTree::mapnode_t&  RedBlackTree::GetMax   (const mapnode_t& x) const
{
  return x.GetMax ();
}

RedBlackTree::RedBlackTree ()
             : mRoot (NULL), mCount (0)
{  }

RedBlackTree::mapnode_t& RedBlackTree::mapnode_t::IterNext () const
{
  if (right->valid ()) return right->GetMin ();
  else
  {
    for (const mapnode_t* ptr = this,*p=ptr->parent;p && ptr == p->right;ptr = p,p=ptr->parent);
    if  (!ptr->valid ()) return *(mapnode_t*)NULL; 
    return (mapnode_t&)*(ptr=p);
  }
}

RedBlackTree::mapnode_t& RedBlackTree::mapnode_t::IterPrev () const
{
  if (left->valid ()) return left->GetMax ();
  else
  {
    for (const mapnode_t* ptr = this,*p=ptr->parent;p && ptr == p->left;ptr = p,p=ptr->parent);
    if  (!ptr->valid ()) return *(mapnode_t*)NULL;
    return (mapnode_t&)*(ptr = p);
  }  
}

RedBlackTree::mapnode_t&  RedBlackTree::mapnode_t::GetMin () const
{
  for (const mapnode_t* tree=this;tree->left->valid ();tree = tree->left);

  return (mapnode_t&)*tree;
}

RedBlackTree::mapnode_t&  RedBlackTree::mapnode_t::GetMax () const
{
  for (const mapnode_t* tree=this;tree->right->valid ();tree = tree->right);

  return (mapnode_t&)*tree;
}