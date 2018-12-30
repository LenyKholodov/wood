#include "pch.h"
#include "cv_treecomp.h"
#include "g_level.h"

//////////////////implementation of class TreeGroup/////////////////////////////

class    TreeCompiler::TreeGroup: public MemObject
{
  friend class TreeCompiler::Tree;
  friend class TreeCompiler;
  public:
                TreeGroup  (Pool* pool,TreeCompiler* comp,int desc)
                   : MemObject (pool), mRing (pool), mComp (comp), mDesc (desc) {}
                TreeGroup  (Pool* pool,TreeCompiler* comp,int desc,BinFile* file,const treemapheader_t::block_t& hdr)
                   : MemObject (pool), mRing (pool), mComp (comp), mDesc (desc)
             {
               file->seek (hdr.offs);

               for (int i=0;i<hdr.count;i++)
               {                  
                 tree_t tree;    
               
                 file->read (&tree,sizeof (tree_t));

                 add (tree.tree,tree.pos.x,tree.pos.z);
                }  
              }  
        virtual ~TreeGroup () {
          TreeGroupRing::iterator   iter  = mComp->mGroupMap;
          int                       count = mComp->mGroupMap.count ();

          for (int i=0;i<count;i++)
            if (iter.data ().group == mDesc)  mComp->mGroupMap.erase (iter);
            else                        iter++;
        }

        void    add        (TREE desc,float x,float y)  {
          tree_t tree;
          
          tree.pos  = vector3d_t (x,0,y);
          tree.tree = desc;

          mRing.insert (tree);
        }

        void    del        (TREE desc,float x,float y) {
          TreeRing::iterator iter  = mRing;
          int                count = mRing.count ();

          for (int i=0;i<count;i++)
            if (iter.data().pos.x == x && iter.data().pos.z == y &&
                iter.data().tree == desc)
                  mRing.erase (iter);
            else
              iter++;      
        }

        void    del        (TREE desc) {
          TreeRing::iterator iter  = mRing;
          int                count = mRing.count ();

          for (int i=0;i<count;i++)
            if (iter.data().tree == desc)     mRing.erase (iter);
            else                              iter++;
        }

        void    save       (BinFile* file,treemapheader_t::block_t& hdr) {
          TreeRing::iterator iter  = mRing;
          int                count = mRing.count ();

          hdr.count = count;
          hdr.offs  = file->getpos ();
          hdr.size  = count * sizeof (tree_t);

          for (int i=0;i<count;i++,iter++)
            file->write (&iter.data (),sizeof (tree_t));
        }

        static void    saveNull   (treemapheader_t::block_t& hdr) {
          hdr.count = 0;
          hdr.offs  = -1;
          hdr.size  = 0;
        }

  private:
        TreeRing        mRing;
        TreeCompiler*   mComp;
        int             mDesc;
};

///////////////////implementation of class Tree/////////////////////////////////

static char treeNullName [MODEL_NAME_LEN] = "Attention: Null tree!";

class    TreeCompiler::Tree: public MemObject
{
  friend class TreeCompiler;
  public:
                Tree  (Pool* pool,TreeCompiler* comp,int desc)
                   : MemObject (pool), mComp (comp), mDesc (desc) { mModelName [0] = 0; }
                Tree  (Pool* pool,TreeCompiler* comp,int desc,const char* name) 
                   : MemObject (pool), mComp (comp), mDesc (desc)
                {
                  set (name);
                }
        virtual ~Tree () {
          for (int j=0;j<mComp->mTreeGrCount;j++)
            if (mComp->mTreeGroup [j])
            {
              TreeRing::iterator   iter  = mComp->mTreeGroup [j]->mRing;
              int                  count = mComp->mTreeGroup [j]->mRing.count ();

              for (int i=0;i<count;i++)
                if (iter.data ().tree == mDesc)  mComp->mTreeGroup [j]->mRing.erase (iter);
                else                             iter++;
            }       
        }

        void    set (const char* name)  {
          if (strlen (name) < MODEL_NAME_LEN)  strcpy (mModelName,name);
          else                                   D_Message ("TreeCompiler::Tree: Name too long: "<<name);
        }        

        void    save       (BinFile* file) {
          file->write (mModelName,MODEL_NAME_LEN);
        }

        static void    saveNull   (BinFile* file) {
          file->write (treeNullName,MODEL_NAME_LEN);
        }

        static size_t size () { return MODEL_NAME_LEN; }

  private:
        char            mModelName [MODEL_NAME_LEN];
        TreeCompiler*   mComp;
        int             mDesc;
};

//////////////////implementation of class TreeCompiler//////////////////////////

TREEGROUP       TreeCompiler::CreateTreeGroup         ()
{
  for (int i=0;i<mTreeGrCount;i++)
    if (!mTreeGroup [i])
    {
      mTreeGroup [i] = ext_new  TreeGroup (GetPool(),this,i);

      return mTreeGroup [i] ? i : -1;
    }  
  return -1;
}

void            TreeCompiler::DeleteTreeGroup         (TREEGROUP index)
{
  if (index < 0 || index >= mTreeGrCount)
    return;

  if (mTreeGroup [index])
  {
    delete mTreeGroup [index];
    mTreeGroup [index] = NULL;
  }  
}

TREE            TreeCompiler::CreateTree              (const char* modelName)
{
  for (int i=0;i<mTreeDescCount;i++)
    if (!mTreeDesc [i])
    {
      mTreeDesc [i] = ext_new Tree (GetPool(),this,i);

      if (!mTreeDesc [i])
        return -1;

      mTreeDesc [i]->set (modelName);

      return i;
    }    
  return -1;
}

void            TreeCompiler::ChangeTree              (TREE tree,const char* name)
{
  if (tree < 0 || tree >= mTreeDescCount)
    return;

  if (mTreeDesc [tree]) mTreeDesc [tree]->set (name);
}

void            TreeCompiler::DeleteTree              (TREE tree)
{
  if (tree < 0 || tree >= mTreeDescCount)
    return;

  if (mTreeDesc [tree]) 
  {
    delete mTreeDesc [tree];
    mTreeDesc [tree] = NULL;
  } 
}

void            TreeCompiler::AddTree   (TREEGROUP group,TREE tree,float locX,float locY)
{
  if (group < 0 || group >= mTreeGrCount || tree < 0 || tree >= mTreeDescCount)
    return;

  if (mTreeGroup [group] && mTreeDesc [tree])
    mTreeGroup [group]->add (tree,locX,locY);
}

void            TreeCompiler::DelTree   (TREEGROUP group,TREE tree,float locX,float locY)
{
  if (group < 0 || group >= mTreeGrCount || tree < 0 || tree >= mTreeDescCount)
    return;

  if (mTreeGroup [group] && mTreeDesc [tree])
    mTreeGroup [group]->del (tree,locX,locY);
}

void            TreeCompiler::DelTree   (TREEGROUP group,TREE tree)
{
  if (group < 0 || group >= mTreeGrCount || tree < 0 || tree >= mTreeDescCount)
    return;

  if (mTreeGroup [group] && mTreeDesc [tree])
    mTreeGroup [group]->del (tree);
}

void            TreeCompiler::AddTree     (TREE tree,float locX,float locY)
{
  AddTree (mSingleGroup,tree,locX,locY);  
}

void            TreeCompiler::DelTree     (TREE tree,float locX,float locY)
{
  DelTree (mSingleGroup,tree,locX,locY);  
}

void            TreeCompiler::DelTree     (TREE tree)
{
  DelTree (mSingleGroup,tree);
}

void            TreeCompiler::AddTreeGroup  (TREEGROUP group,float worldX,float worldY)
{
  if (group < 0 || group >= mTreeGrCount)
    return;  

  treegroup_t obj;

  obj.pos     = vector3d_t (worldX,0,worldY);
  obj.group   = group;

  mGroupMap.insert (obj);
}

void            TreeCompiler::DelTreeGroup  (TREEGROUP group,float worldX,float worldY)
{
  if (group < 0 || group >= mTreeGrCount)
    return;  

  TreeGroupRing::iterator   iter  = mGroupMap;
  int                       count = mGroupMap.count ();

  for (int i=0;i<count;i++)
    if (iter.data ().group == group  && 
        iter.data ().pos.x == worldX &&
        iter.data ().pos.z == worldY
       )  
         mGroupMap.erase (iter);
    else 
         iter++;    
}

void            TreeCompiler::DelTreeGroup            (TREEGROUP group)
{
  if (group < 0 || group >= mTreeGrCount)
    return;  

  TreeGroupRing::iterator   iter  = mGroupMap;
  int                       count = mGroupMap.count ();

  for (int i=0;i<count;i++)
    if (iter.data ().group == group)  
         mGroupMap.erase (iter);
    else 
         iter++;    
}

BOOL            TreeCompiler::GetTree          (TREE tree,char* model)
{
  D_Try
  {
    model [0] = 0;

    if (tree < 0 || tree >= mTreeDescCount)
      return FALSE;

    if (!mTreeDesc [tree]) return FALSE;

    strcpy (model,mTreeDesc [tree]->mModelName);

    return TRUE;
  }
  D_ExcRet ("TreeCompiler: Error at GetTree",FALSE);
}

BOOL            TreeCompiler::GetTreeGroup     (TREEGROUP group,RingTmpl<tree_t>& ring)
{
  D_Try
  {
    if (group < 0 || group >= mTreeDescCount)
      return FALSE;

    if (!mTreeGroup [group]) return FALSE;

    TreeRing::iterator iter  = mTreeGroup [group]->mRing;   
    int                count = mTreeGroup [group]->mRing.count ();

    for (int i=0;i<count;i++,iter++)
      ring.insert (iter.data ());

    return TRUE;
  }
  D_ExcRet ("TreeCompiler: Error at GetTreeGroup",FALSE);  
}

BOOL            TreeCompiler::GetMap           (RingTmpl<treegroup_t>& ring)
{
  D_Try
  {
    TreeGroupRing::iterator iter  = mGroupMap;   
    int                     count = mGroupMap.count ();

    for (int i=0;i<count;i++,iter++)
      ring.insert (iter.data ());

    return TRUE;
  }
  D_ExcRet ("TreeCompiler: Error at GetMap",FALSE);  
}

void            TreeCompiler::reindex   ()
{
  D_Try
  {        
    ////make reindex    !!!
  }
  D_Exc ("TreeCompiler: Error at reindex");
}

BOOL            TreeCompiler::Save  (GameLevel* level)
{
  D_Try
  {
    reindex ();

    BinFile*         file = level->CreateLump (LL_TREE_MAP,GameLevel::OPEN_RW);
    treemapheader_t  hdr;
    int              pos = 0, i = 0;

    treemapheader_t::block_t* groupDesc = (treemapheader_t::block_t*)
           GetPool()->alloc (sizeof (treemapheader_t::block_t) * mTreeGrCount);

    if (!groupDesc)
    {
      D_Message ("TreeCompiler: No enough memory for save tree map");
      return FALSE;
    }  

    file->seek (sizeof (treemapheader_t));    

               //calc sizes

/*    int        groupCount = 0,
               modelCount = 0;

    for (int i=0;i<mTreeDescCount;i++)
      if (mTreeDesc [i])
        modelCount = i + 1;

    for (i=0;i<mTreeGrCount;i++)
      if (mTreeGroup [i])
        groupCount = i + 1;        
*/
                //record header

    hdr.trees.offs   = file->getpos ();
    hdr.trees.count  = mTreeDescCount;
    hdr.trees.size   = mTreeDescCount * Tree::size ();
//    hdr.trees.count  = modelCount;
//    hdr.trees.size   = modelCount * Tree::size ();

    hdr.groups.offs  = hdr.trees.offs + hdr.trees.size;
    hdr.groups.count = mTreeGrCount;
    hdr.groups.size  = mTreeGrCount * sizeof (treemapheader_t::block_t);
//    hdr.groups.count = groupCount;
//    hdr.groups.size  = groupCount * sizeof (treemapheader_t::block_t);

    hdr.map.offs     = hdr.groups.offs + hdr.groups.size;
    hdr.map.count    = mGroupMap.count ();
    hdr.map.size     = mGroupMap.count () * sizeof (treegroup_t);

               //write  trees                

    file->seek (hdr.trees.offs);

    for (i=0,pos = 0;i<mTreeDescCount;i++)
      if (mTreeDesc [i])
      {
        mTreeDesc [i]->save (file);
        pos++;
      }  
      else
        Tree::saveNull (file);

               //write  groups 

    file->seek (hdr.map.offs + hdr.map.size);

    for (i=0,pos=0;i<mTreeGrCount;i++)
      if (mTreeGroup [i])
        mTreeGroup [i]->save (file,groupDesc [pos++]);
      else
        TreeGroup::saveNull (groupDesc [pos++]);

               //write  map 

    file->seek (hdr.map.offs);

    TreeGroupRing::iterator iter  = mGroupMap;
    int                     count = mGroupMap.count ();

    for (i=0;i<count;i++,iter++)
      file->write (&iter.data(),sizeof (treegroup_t));

              //write  headers

    file->seek  (hdr.groups.offs);
//    file->write (groupDesc,sizeof (treemapheader_t::block_t) * groupCount);
    file->write (groupDesc,sizeof (treemapheader_t::block_t) * mTreeGrCount);
    file->reset ();
    file->write (&hdr,sizeof (treemapheader_t));

    delete file;

    return TRUE;
  }  
  D_ExcRet ("TreeCompiler: Error at save",FALSE);
}

BOOL      TreeCompiler::load  (BinFile* file)
{
  memset (mTreeDesc,0,sizeof (Tree*) * mTreeDescCount);
  memset (mTreeGroup,0,sizeof (TreeGroup*) * mTreeGrCount);

  mSingleGroup = 0;

              //load trees
  treemapheader_t  hdr;

  file->reset ();
  file->read (&hdr,sizeof (hdr));
  file->seek (hdr.trees.offs);

  for (int i=0;i<hdr.trees.count;i++)
  {
    static char name [256] = {0};
    file->read (name,MODEL_NAME_LEN);
    
    if (strcmp (name,treeNullName))  mTreeDesc [i] = ext_new Tree (GetPool(),this,i,name);
    else                             mTreeDesc [i] = NULL;
  }

              //load groups

  treemapheader_t::block_t* groupDesc = (treemapheader_t::block_t*)
         GetPool()->alloc (sizeof (treemapheader_t::block_t) * hdr.groups.count);

  if (!groupDesc)
  {
    D_Message ("TreeCompiler: No enough memory for load tree map");
    return FALSE;
  }  

  file->seek (hdr.groups.offs);
  file->read (groupDesc,hdr.groups.size);
  
  for (i=0;i<hdr.groups.count;i++)
    if (groupDesc [i].offs != -1)
      mTreeGroup [i] = ext_new TreeGroup (GetPool(),this,i,file,groupDesc [i]);
    else  
      mTreeGroup [i] = NULL;

  GetPool()->free (groupDesc);    

               //load map

  file->seek (hdr.map.offs);
               
  for (i=0;i<hdr.map.count;i++)
  {
    treegroup_t group;

    file->read   (&group,sizeof (treegroup_t));

    AddTreeGroup (group.group,group.pos.x,group.pos.z);
  }             

  return TRUE;
}

BOOL      TreeCompiler::Load     (GameLevel* level)
{
  D_Try
  {
    BinFile*         file = level->CreateLump (LL_TREE_MAP,GameLevel::OPEN_RO);

    if (!file)
      return FALSE;

    if (!mTreeDesc && !mTreeGroup)
    {
      treemapheader_t  hdr;

      file->reset ();
      file->read  (&hdr,sizeof (hdr));

      M_PushPool (GetPool());

      mTreeDesc   = new Tree*      [mTreeDescCount=hdr.trees.count];
      mTreeGroup  = new TreeGroup* [mTreeGrCount=hdr.groups.count];

      M_PopPool  ();

      if (!mTreeDesc || !mTreeGroup)
      {
        D_Message ("TreeCompiler: No enough memory for sys data");
        if (file) delete file;
        return ;
      }

      memset (mTreeDesc,0,sizeof (Tree*) * mTreeDescCount);
      memset (mTreeGroup,0,sizeof (TreeGroup*) * mTreeGrCount);
    }
    else if (mTreeDesc && mTreeGroup)
    {
      treemapheader_t  hdr;

      file->read       (&hdr,sizeof (hdr));
      
      if (hdr.trees.count > mTreeDescCount || hdr.groups.count > mTreeGrCount)
      {
        D_Message ("TreeCompiler: Load data is equal limit");
        return FALSE;
      }

      if (mTreeDesc)  for (int i=0;i<mTreeDescCount;DeleteTree (i++));
      if (mTreeGroup) for (int i=0;i<mTreeGrCount;DeleteTreeGroup (i++));
      
      TreeGroupRing::iterator iter  = mGroupMap;
      int                     count = mGroupMap.count ();    

      for (int i=0;i<count;i++)  mGroupMap.erase (iter);
    }  
    else
    {
      if (file) delete file;
      return FALSE;
    }

    if (!load (file))
    {
      if (file) delete file;
      return FALSE;
    }  

    delete file;

    return TRUE;
  }
  D_ExcRet ("TreeCompiler: Error at load",FALSE);
}

TreeCompiler::TreeCompiler  (Pool* _Pool,int _TreeGroupNum,int _TreeTypeNum)
             : MemObject (_Pool),
               mGroupMap (_Pool),
               mTreeGrCount (_TreeGroupNum), mTreeDescCount (_TreeTypeNum),
               mTreeDesc    (NULL), mTreeGroup (NULL),
               mSingleGroup (-1)               
{
  D_Try
  {
    if (!IsValid (&mGroupMap))
    {
      D_Message ("TreeCompiler: Error at initialize sys data");
      return;
    }

    M_PushPool (GetPool());

    mTreeDesc   = new Tree*      [mTreeDescCount];
    mTreeGroup  = new TreeGroup* [mTreeGrCount];

    M_PopPool  ();

    if (!mTreeDesc || !mTreeGroup)
    {
      D_Message ("TreeCompiler: No enough memory for sys data");
      return ;
    }

    memset (mTreeDesc,0,sizeof (Tree*) * mTreeDescCount);
    memset (mTreeGroup,0,sizeof (TreeGroup*) * mTreeGrCount);

    mSingleGroup = CreateTreeGroup ();
  }
  D_Exc ("TreeCompiler: Error at construct");
}

TreeCompiler::TreeCompiler  (Pool* _Pool,GameLevel* level)
             : MemObject (_Pool),
               mGroupMap (_Pool),
               mTreeGrCount (0), mTreeDescCount (0),
               mTreeDesc    (NULL), mTreeGroup (NULL),
               mSingleGroup (-1)               
{
  D_Try
  {
    if (!IsValid (&mGroupMap))
    {
      D_Message ("TreeCompiler: Error at initialize sys data");
      return;
    }

    Load (level);
  }
  D_Exc ("TreeCompiler: Error at construct");
}

TreeCompiler::~TreeCompiler ()
{
  D_Try
  {
    if (mTreeDesc)  for (int i=0;i<mTreeDescCount;DeleteTree (i++));
    if (mTreeGroup) for (int i=0;i<mTreeGrCount;DeleteTreeGroup (i++));
    
    if (mTreeDesc)  delete [] mTreeDesc;
    if (mTreeGroup) delete [] mTreeGroup;
  }
  D_Exc ("TreeCompiler: Error at destruct");
}
