#define  DEBUG
#include <engine\pgkeeper.h>
#include <debug.h>
#include <ring.h>

const    int    MAX_FACET_AT_VERTEX = 128;
const    float  EPS                 = 0.1;

const    float  PVS_POSITIVE        = 1.0f;
const    float  PVS_NEGATIVE        = -1.0f;
const    BOOL   PVS_ALL             = TRUE;
const    BOOL   PVS_LAST            = FALSE;
const    int    MAX_LEAFS           = 0x10000;

struct   vertexnode_t
{
  int   num;
  int   facet [MAX_FACET_AT_VERTEX];
};

struct  edgenode_t
{
  int   vertex [2];
};

struct  facetedge_t
{
  char  process;
  char  edge [3];
};

class preleaf_t
{
  public:
    int                   leaf;        
    boundbox_t            box;
    RingTmpl<int>         neighbour;
    RingTmpl<edgenode_t>  edge;

                preleaf_t (Pool* pool): leaf (-1), neighbour (pool), edge (pool) {}

    void*    operator new (size_t size,Pool* pool) { return pool->alloc (size); }
    void     operator delete  (void* x,Pool* pool) { pool->free (x); } 
};

class  boundnode_t
{
  public:
    int                     leaf;        
    boundbox_t              box;        
    boundnode_t*            parent;
    RingTmpl<boundnode_t*>  child;
    RingTmpl<preleaf_t*>    ring;

                            boundnode_t (Pool* _Pool): 
                    child (_Pool), ring (_Pool), parent (NULL), leaf (-1) {}

    void*    operator new (size_t size,Pool* pool) { return pool->alloc (size); }
    void     operator delete  (void* x,Pool* pool) { pool->free (x); } 
};

struct  pvs_t
{
  int   num;
  int   first;
};

struct leaf_t
{
  int   pvs;
  int   num;
  int   first;
};

struct btree_t
{
  int   parent;
  int   num;
  int   child;
  int   leaf;
};

typedef  RingTmpl<int>          PVSRing;
typedef  RingTmpl<edgenode_t>   EdgeRing;
typedef  RingTmpl<preleaf_t*>   LeafRing;
typedef  RingTmpl<boundnode_t*> BoundRing;         
typedef  RingTmpl<int>          NeighbourRing;

class   PortalKeeper: public PolygonKeeper
{
  public:
                        PortalKeeper  (Pool*,Object3D&,float = PVS_POSITIVE,BOOL = TRUE);
           virtual      ~PortalKeeper ();

           virtual      int    type  () const { return PORTAL_KEEPER; };          

  protected:
                void                 convert     (); 
                void                 buildvnode  (vertexnode_t*);
                void                 buildPVS    (PVSRing&,vertexnode_t*,int ffrist,int ffrom);
                BOOL                 checkPVS    (const PVSRing&,const facet_t&,const facet_t&);
                void                 buildEdges  (const PVSRing&);
                void                 checkEdges  (int,int);                
                void                 buildPreLeaf(int,const PVSRing&,const EdgeRing&);
                int                  findFirst   (int*,int num);
                void                 buildBound  (const PVSRing&,boundbox_t&);
                void                 buildBound  (boundnode_t&);
                void                 buildTree   (boundnode_t*);                        
                boundnode_t*         buildTree   (const LeafRing&);
                BOOL                 isIn        (const boundbox_t&,const boundbox_t&);
                void                 insertNeighbour(int,int);
                BOOL                 checkNeighbour (int,int);
                void                 buildNeighbour (int);
                void                 buildNeighbour ();
                void                 buildNeighbour (boundnode_t*);
                BOOL                 checkEdge      (int,const edgenode_t&);
                void                 deleteEdge     (int,const edgenode_t&);
     inline     BOOL                 checkProcess(int findex) {
       return mFacetProcess [findex] == -mCurLeaf || 
              (mFacetProcess [findex] <= mCurLeaf && mFacetProcess [findex] > 0);
     }     

  protected:  
           int*         mFacetProcess;
           facetedge_t* mFacetEdge;
           EdgeRing     mEdgeRing;
           LeafRing     mLeafRing;
           float        mPlaneParam;
           BOOL         mAll;
           int          mCurLeaf;
           preleaf_t**  mLeafTable;
};

void     PortalKeeper::convert ()
{
  D_Try
  {
    vertexnode_t* vnode = (vertexnode_t*)alloc (sizeof (vertexnode_t) * mCount [PK_VERTEX]);
    mFacetProcess       = (int*)alloc (sizeof (int) * mCount [PK_FACET]);
    mFacetEdge          = (facetedge_t*)alloc (sizeof (facetedge_t) * mCount [PK_FACET]);

    memsetd      (mFacetProcess,MAX_LEAFS,mCount [PK_FACET]);
    buildvnode   (vnode);    

    int ffirst = -1;
    int total  = 0;

    for (mCurLeaf=1;(ffirst=findFirst (mFacetProcess,mCount [PK_FACET]))!=-MAX_LEAFS;mCurLeaf++)
    {
      PVSRing    ring (pool());

      memset       (mFacetEdge,0,sizeof (facetedge_t) * mCount [PK_FACET]);

      buildPVS     (ring,vnode,ffirst,-1);      
      buildEdges   (ring);      
      buildPreLeaf (mCurLeaf,ring,mEdgeRing);

//      dout<<"Build PVS: Facets: "<<ring.count ()
//          <<" Edges: "<<mEdgeRing.count ()<<endl;

      total += ring.count ();

      mEdgeRing.reset ();

      cout<<"%"<<float(total)/float(mCount [PK_FACET])*100.0f<<endl;
    }

    dout<<"Build bound tree"<<endl;

    boundnode_t* root = buildTree (mLeafRing);    

    dout<<"Build neighbours"<<endl;

    LeafRing::iterator  leaf  = mLeafRing;
    int                 count = mLeafRing.count ();

    mLeafTable = (preleaf_t**)alloc (sizeof (preleaf_t*) * count);
    memset (mLeafTable,0,sizeof (preleaf_t*) * count);

    for (int i=0;i<count;i++,leaf++)
      mLeafTable [leaf.data()->leaf] = leaf.data ();

    buildNeighbour ();  
    buildNeighbour (root);

    dout<<"Total: Facets: "<<mCount [PK_FACET]<<endl;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)       
  {
    dout<<"Error at convert () "<<endl;
  }
}

BOOL     PortalKeeper::checkPVS    (const PVSRing& ring,const facet_t& f2,const facet_t& last)
{
  D_Try
  {
        //recalc plane param

    plane_t   plane;

    plane.a   = f2.n.x;    
    plane.b   = f2.n.y;    
    plane.c   = f2.n.z;    
    plane.d   = -(mVertex [f2.index [0]] & f2.n);

    normalize (plane);

       //check for last                  

    for (int i=0;i<3;i++)
    {
      vertex_t& v    = mVertex [last.index [i]];
      float state    = (v.x*plane.a + v.y*plane.b + v.z*plane.c + plane.d);

      if (fabs (state) < EPS) 
        continue;

      if (fabs (mPlaneParam+state) < fabs (mPlaneParam-state)) 
        return FALSE;
    }

       //check for all

    if (mAll)
    {      
      PVSRing::iterator iter = ring;
      int num                = ring.count ();

      if (num)
      {
        for (int i=0;i<num;i++,iter++)
        {
          facet_t& facet = mFacet [iter.data ()];
          float    s;
   
          for (int j=0;j<3;j++)
          {
            vertex_t& v = mVertex [facet.index [j]];
            s           = (v.x*plane.a + v.y*plane.b + v.z*plane.c + plane.d);

            if (fabs (s) < EPS) 
              continue;
  
            if (fabs (s+mPlaneParam) < fabs (s-mPlaneParam)) 
            {
//              dout<<"S: "<<s<<" State: "<<mPlaneParam<<endl;

              return FALSE;
            }  
          }
        }  
      }
    }    

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"PortalKeeper: Error at ckeckPVS () "<<this<<endl;
    #endif
    return FALSE;
  }
}

void     PortalKeeper::buildEdges  (const PVSRing& ring)
{
  int                   count = ring.count ();
  PVSRing::iterator     iter  = ring;

  for (int i=0;i<count;i++,iter++)
  {
    int index = iter.data ();

    if (mFacetEdge [index].process < 3)
    {  
      for (int j=0;j<3;j++)
        if (!mFacetEdge [index].edge [j])
        {
          static int edge2node [3][2] = {
            {0,1},
            {1,2},
            {2,0}
          }; 

          edgenode_t edge = {mFacet [index].index [edge2node [j][0]],
                             mFacet [index].index [edge2node [j][1]]};

          mEdgeRing.insert (edge);
        }            
    }
  }
}

void     PortalKeeper::checkEdges  (int ffirst,int ffrom)
{
  int       el  [4] = {0};
  int       el1 [4] = {0};
  int       pos     = 0;
  facet_t&  prev    = mFacet [ffrom];
  facet_t&  facet   = mFacet [ffirst];

  for (int k=0;k<3;k++)    
    for (int l=0;l<3;l++)
      if (!memcmp (mVertex + prev.index [l],mVertex + facet.index [k],sizeof (vertex_t)))
      {
        el  [pos]   = l;
        el1 [pos++] = k;
      }  


  if (pos == 2)
  {
    static int edges [3][3] = {
      {-1,    0,   2},
      { 0,   -1,   1},
      { 2,    1,  -1},
    };

    int  edge   = edges [el[0]][el[1]];
    int  edge1  = edges [el1[0]][el1[1]];

    if (!mFacetEdge [ffrom].edge [edge])
    {
//      dout<<"Facet: "<<ffrom<<" "<<edge<<endl;
      mFacetEdge [ffrom].edge [edge]   = TRUE;
      mFacetEdge [ffrom].process++;
    }

    if (!mFacetEdge [ffirst].edge [edge1])
    {
//      dout<<"Facet: "<<ffirst<<" "<<edge1<<endl;
      mFacetEdge [ffirst].edge [edge1] = TRUE;
      mFacetEdge [ffirst].process++;
    }
  }    
}

void     PortalKeeper::buildPVS     (PVSRing& ring,vertexnode_t* vnode,int ffirst,int ffrom)
{
  facet_t& facet = mFacet [ffirst];
        
  if (checkProcess (ffirst))
    return;

  ring.insert   (ffirst);
  mFacetProcess [ffirst] = mCurLeaf;

  for (int j=0;j<3;j++)
  {
    int   vindex = facet.index [j];
    int   num    = vnode [vindex].num;
    int*  findex = vnode [vindex].facet;     

    for (int i=0;i<num;i++,findex++)
    {
      if (*findex != ffirst)
      {
        if (!checkProcess (*findex))
        {
          if (checkPVS (ring,mFacet [*findex],facet))
          {
            if (mFacetProcess [*findex] < 0)
              insertNeighbour (-mFacetProcess [*findex],mCurLeaf);

            buildPVS (ring,vnode,*findex,ffirst);
          }  
          else
            mFacetProcess [*findex] = -mCurLeaf;  
        }     

        if (mFacetProcess [*findex] == mCurLeaf)
          checkEdges (*findex,ffirst);
      }
    }
  }
}

void     PortalKeeper::buildvnode  (vertexnode_t* list)
{
  D_Try
  {
    for (int i=0;i<mCount [PK_VERTEX];i++)
      list [i].num = 0;

    for (i=0;i<mCount [PK_FACET];i++)
      for (int j=0;j<3;j++)
      {
        int vindex = mFacet [i].index [j];
        list [vindex].facet [list [vindex].num++] = i;
      }           

    for (i=0;i<mCount [PK_VERTEX];i++)
      for (int j=0;j<mCount [PK_VERTEX];j++)
        if (!memcmp (mVertex + i,mVertex + j,sizeof (vertex_t)) && j!=i)
        {
          for (int k=0;k<list [i].num;k++)
          {
            int facet = list [i].facet [k];          

            for (int l=0;l<list [j].num;l++)
              if (facet == list [j].facet [l])  
                break;

            if (l == list [j].num)
              list [j].facet [list [j].num++] = facet;    
          }    
        }  
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    dout<<"Error at buildvnode "<<endl;
  }        
}

void    PortalKeeper::buildPreLeaf (int _leaf,const PVSRing& pvs,const EdgeRing& _edge)
{
  preleaf_t*         leaf  = new (pool()) preleaf_t (pool());

  leaf->leaf    = _leaf;
  leaf->edge    = _edge;

  buildBound (pvs,leaf->box);

  mLeafRing.insert (leaf);
}

int   PortalKeeper::findFirst   (int* table,int num)
{
  for (int i=0;i<num;i++,table++)
    if (*table > mCurLeaf || *table < 0)
      return i;

  return -MAX_LEAFS;
}

void   PortalKeeper::buildBound  (const PVSRing& ring,boundbox_t& box)
{
  if (ring.count () == 0)       
  {
    memset (&box,0,sizeof (boundbox_t));
    return;
  }

  vertex_t              c   (0);
  vector3d_t            dim (0);
  PVSRing::iterator     iter  = ring;
  int                   count = ring.count ();
    
  for (int i=0;i<count;i++,iter++)
  {
    facet_t& facet = mFacet [iter.data ()];

    for (int j=0;j<3;j++)
      c += mVertex [facet.index [j]];
  }

  c /= float (count*3);

  for (i=0;i<count;i++,iter++)
  {
    facet_t& facet = mFacet [iter.data ()];

    for (int j=0;j<3;j++)
    {
      vector3d_t delta = abs (mVertex [facet.index [j]] - c);
      
      if (delta.x > dim.x) dim.x = delta.x;
      if (delta.y > dim.y) dim.y = delta.y;
      if (delta.z > dim.z) dim.z = delta.z;
    }  
  }

  box.min = c - dim;
  box.max = c + dim;
}

void  PortalKeeper::buildBound  (boundnode_t& node)
{
  if (node.ring.count () == 0)       
  {
    memset (&node.box,0,sizeof (boundbox_t));
    return;
  }

  vertex_t              c   (0);
  vector3d_t            dim (0);
  LeafRing::iterator    iter  = node.ring;
  int                   count = node.ring.count ();
    
  for (int i=0;i<count;i++,iter++)
  {
    boundbox_t& box = iter.data ()->box;
    c              += box.min;
    c              += box.max;
  }

  c /= float (count*2);

  for (i=0;i<count;i++,iter++)
  {
    boundbox_t& box   = iter.data ()->box;    
    vector3d_t  d [2] = {
      abs (c - box.min),
      abs (c - box.max)
    };

    for (int j=0;j<2;j++)
    {
      vector3d_t& delta = d [j];
      
      if (delta.x > dim.x) dim.x = delta.x;
      if (delta.y > dim.y) dim.y = delta.y;
      if (delta.z > dim.z) dim.z = delta.z;
    }  
  }

  node.box.min = c - dim;
  node.box.max = c + dim;
}

void   PortalKeeper::buildTree   (boundnode_t* node)
{
  if (node->ring.count () == 0)
    return;

  buildBound (*node);

  while (node->ring.count ())
  {
    float      V    = 0;
    preleaf_t* leaf = NULL;

    LeafRing::iterator iter  = node->ring;
    int                count = node->ring.count ();

    for (int i=0;i<count;i++,iter++)
    {
      boundbox_t  box   = iter.data ()->box;
      vector3d_t  delta = box.max - box.min;
      float       v     = delta.x * delta.y * delta.z;

      if (v >= V) 
      {
        V     = v;
        leaf  = iter.data ();
      }  
    }

    if (leaf == NULL)
    {
      dout<<"Error found leaf "<<endl;
      return;
    }

    boundnode_t* child = new (pool()) boundnode_t (pool());
    
    child->box    = leaf->box;
    child->leaf   = leaf->leaf;
    child->parent = node;

    node->ring.erase   (leaf);    
    node->child.insert (child);

    iter  = node->ring;
    count = node->ring.count ();

    for (i=0;i<count;i++,iter++)
      if (isIn (child->box,iter.data()->box))
      {
        child->ring.insert (iter.data());
        node->ring.erase   (iter);
        iter--;
      }
  }
  
  BoundRing::iterator iter  = node->child;
  int                 count = node->child.count ();

  for (int i=0;i<count;i++,iter++)
  {
    boundnode_t* child = iter.data ();

    if (child->ring.count ())
      buildTree (child);
  }
}

BOOL   PortalKeeper::isIn        (const boundbox_t& big,const boundbox_t& lit)
{
  return lit.min.x >= big.min.x && lit.min.y >= big.min.y && lit.min.z >= big.min.z &&
         lit.max.x <= big.max.x && lit.max.y <= big.max.y && lit.max.z <= big.max.z;
}

boundnode_t*   PortalKeeper::buildTree   (const LeafRing& leafs)
{
  boundnode_t* root = new (pool()) boundnode_t (pool());
  preleaf_t*   leaf = new (pool()) preleaf_t   (pool());

  root->leaf    = 0;
  root->parent  = NULL;
  leaf->leaf    = 0;

  LeafRing::iterator   iter  = leafs;
  int                  count = leafs.count ();

  for (int i=0;i<count;i++,iter++)
    root->ring.insert (iter.data());

  buildTree (root);

  leaf->box = root->box;

  mLeafRing.insert (leaf);

  return root;
}

void  PortalKeeper::insertNeighbour(int leaf,int neighbour)
{
  if (checkNeighbour (leaf,neighbour))
    return;

  LeafRing::iterator  iter  = mLeafRing;
  int                 count = mLeafRing.count ();

  for (int i=0;i<count;i++,iter++)
    if (iter.data()->leaf == leaf)
    {
      iter.data()->neighbour.insert (neighbour);
      break;
    }  
}

BOOL  PortalKeeper::checkNeighbour(int leaf,int neighbour)
{
  LeafRing::iterator  iter  = mLeafRing;
  int                 count = mLeafRing.count ();

  for (int i=0;i<count;i++,iter++)
    if (iter.data()->leaf == leaf)
    {
      NeighbourRing::iterator   iter1  = iter.data()->neighbour;
      int                       count1 = iter.data()->neighbour.count ();

      for (int j=0;j<count1;j++,iter1++)
        if (iter1.data () == neighbour)
          return TRUE;

      return FALSE;
    }  

  return FALSE;
}

void   PortalKeeper::buildNeighbour ()
{
  LeafRing::iterator   iter  = mLeafRing;
  int                  count = mLeafRing.count ();

  for (int i=0;i<count;i++,iter++)
  {
    preleaf_t&               leaf   = *mLeafTable [iter.data ()->leaf];
    NeighbourRing::iterator  neigh  = leaf.neighbour;
    
    for (int j=0;j<leaf.neighbour.count ();j++,neigh++)
      if (neigh.data () != leaf.leaf)
         mLeafTable [neigh.data ()]->neighbour.insert (leaf.leaf);
  }

  for (i=0;i<count;i++,iter++)
    buildNeighbour (iter.data ()->leaf);  
}

void  PortalKeeper::buildNeighbour (boundnode_t* tree)
{
  if (tree->leaf > 0 && tree->parent && mLeafTable [tree->leaf]->edge.count ())
    insertNeighbour (tree->leaf,tree->parent->leaf);

  if (tree->leaf < 0)
    return;

  BoundRing::iterator  iter  = tree->child;
  int                  count = tree->child.count ();

  for (int i=0;i<count;i++,iter++)
  {
    insertNeighbour (tree->leaf,iter.data()->leaf);
    buildNeighbour  (iter.data());
  }
}

void   PortalKeeper::buildNeighbour (int lindex)
{  
  preleaf_t&          leaf  = *mLeafTable [lindex];
  EdgeRing::iterator  iter  = leaf.edge;
  int                 count = leaf.edge.count ();

  for (int i=0;i<count;i++,iter++)
  {
    NeighbourRing           stack (pool());
    NeighbourRing::iterator neigh = leaf.neighbour;
    edgenode_t&             edge  = iter.data ();

    for (int j=0;j<leaf.neighbour.count ();j++,neigh++)
      if (checkEdge (neigh.data (),edge))
        stack.insert (neigh.data ());

    if (stack.count ())
    {
      neigh  = stack;
          
      for (int j=0;j<stack.count ();j++,neigh++)
        deleteEdge (neigh.data (),edge);                

      leaf.edge.erase (iter);
      iter--;
    }  
  }
}

BOOL  PortalKeeper::checkEdge      (int lindex,const edgenode_t& edge)
{
  preleaf_t&            leaf  = *mLeafTable [lindex];
  EdgeRing::iterator    iter  = leaf.edge;
  int                   count = leaf.edge.count ();

  for (int i=0;i<count;i++,iter++)
  {
    edgenode_t& edge1 = iter.data ();

    if (!memcmp (mVertex + edge.vertex [0],mVertex + edge1.vertex [0],sizeof (vertex_t)) &&
        !memcmp (mVertex + edge.vertex [1],mVertex + edge1.vertex [1],sizeof (vertex_t)))
          return TRUE;

    if (!memcmp (mVertex + edge.vertex [0],mVertex + edge1.vertex [1],sizeof (vertex_t)) &&
        !memcmp (mVertex + edge.vertex [1],mVertex + edge1.vertex [0],sizeof (vertex_t)))
          return TRUE;
  }         

  return FALSE;
}

void  PortalKeeper::deleteEdge     (int lindex,const edgenode_t& edge)
{
  preleaf_t&            leaf  = *mLeafTable [lindex];
  EdgeRing::iterator    iter  = leaf.edge;
  int                   count = leaf.edge.count ();

  for (int i=0;i<count;i++)
  {
    edgenode_t& edge1 = iter.data ();

    if ((!memcmp (mVertex + edge.vertex [0],mVertex + edge1.vertex [0],sizeof (vertex_t)) &&
        !memcmp (mVertex + edge.vertex [1],mVertex + edge1.vertex [1],sizeof (vertex_t))) ||
        (!memcmp (mVertex + edge.vertex [0],mVertex + edge1.vertex [1],sizeof (vertex_t)) &&
        !memcmp (mVertex + edge.vertex [1],mVertex + edge1.vertex [0],sizeof (vertex_t))))
    {
      leaf.edge.erase (iter);
    }    
    else 
      iter++;
  }         
}

PortalKeeper::PortalKeeper  (Pool* _Pool,Object3D& obj,float _PlaneParam,BOOL _All)
             : PolygonKeeper (_Pool,obj),
               mFacetProcess (NULL),
               mEdgeRing     (_Pool),
               mLeafRing     (_Pool),
               mAll          (_All),
               mPlaneParam   (_PlaneParam),
               mCurLeaf      (0),
               mLeafTable    (NULL)
{
  convert ();
}

PortalKeeper::~PortalKeeper ()
{
  D_Try
  {
    if (mFacetProcess) free (mFacetProcess);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    dout<<"Error at destruct "<<endl;    
  }
}

void    main ()
{
  SystemPool            sys;
  FloatPool             pool (&sys,40000000);
  Object3D              obj  (&pool,"garag.3ds");
  PortalKeeper          kpr  (&pool,obj,PVS_NEGATIVE,TRUE);
}
