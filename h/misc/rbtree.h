#ifndef __MISC_RB_TREE__
#define __MISC_RB_TREE__

#include <types.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///������ - ������ �������� ������
/////////////////////////////////////////////////////////////////////////////////////////////
class RedBlackTree
{
  protected:
    struct  mapnode_t;
    friend  struct RedBlackTree::mapnode_t;

  public:

                   RedBlackTree ();

            enum Colors {
              RED = 1, BLACK = 0
            };        

/////////////////////////////////////////////////////////////////////////////////////////////
///����������
/////////////////////////////////////////////////////////////////////////////////////////////
        int         count  () const { return mCount; }

/////////////////////////////////////////////////////////////////////////////////////////////
///���������� / ��������
/////////////////////////////////////////////////////////////////////////////////////////////
        void        insert (mapnode_t&);
        void        erase  (mapnode_t&);

/////////////////////////////////////////////////////////////////////////////////////////////
///������ / ��������� / root
/////////////////////////////////////////////////////////////////////////////////////////////
        mapnode_t&  GetMin   () const { return mRoot ? GetMin (*mRoot) : mNull; }
        mapnode_t&  GetMax   () const { return mRoot ? GetMax (*mRoot) : mNull; }
        mapnode_t&  GetRoot  () const { return mRoot ? *mRoot : mNull; }

/////////////////////////////////////////////////////////////////////////////////////////////
///���������� - ����������    
/////////////////////////////////////////////////////////////////////////////////////////////
        mapnode_t&  GetMin   (const mapnode_t& x) const;
        mapnode_t&  GetMax   (const mapnode_t& x) const;

  protected:
/////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
/////////////////////////////////////////////////////////////////////////////////////////////
            void        SetRoot  (mapnode_t& node) { mRoot = &node; }

  protected:
            void        reset    () { mRoot = NULL; mCount = 0; }

  private:
/////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� �������
/////////////////////////////////////////////////////////////////////////////////////////////
    void          LeftRotate  (mapnode_t*);
    void          RightRotate (mapnode_t*);
    void          DeleteFixup (mapnode_t*);
    mapnode_t*    successor   (mapnode_t*);

  private:
    static mapnode_t   mNull;
    mapnode_t*         mRoot;
    int                mCount;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///���� ������ - ������� ������
/////////////////////////////////////////////////////////////////////////////////////////////
struct RedBlackTree::mapnode_t
{
  mapnode_t *left, *right, *parent;
  int       color;

  mapnode_t&  IterNext () const;
  mapnode_t&  IterPrev () const;

  mapnode_t&  GetMin   () const;            
  mapnode_t&  GetMax   () const; 

  bool        valid    () const { return this && this != &mNull; }
  operator    bool     () const { return valid (); }

              mapnode_t (): parent (NULL), left (&mNull), right (&mNull), color(BLACK) {}
              mapnode_t (mapnode_t* p,mapnode_t* l,mapnode_t* r,int c): parent (p), left (l), right (r), color(c) {}
};


#endif