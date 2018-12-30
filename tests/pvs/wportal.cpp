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

struct  preleaf_t
{
  int                   pvs;
  RingTmpl<portal_t>    portal;

                        preleaf_t (): pvs (-1), portal (NULL) {}
};

typedef  RingTmpl<int>          PVSRing;
typedef  RingTmpl<edgenode_t>   EdgeRing;
typedef  RingTmpl<portal_t>     PortalRing;
typedef  RingTmpl<preleaf_t>    LeafRing;

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
                edgenode_t*          findEdge    (const EdgeRing&,
                                                  int vertex,
                                                  const edgenode_t&);
                BOOL                 checkPortal (const PVSRing&,const vertex_t*);
                int                  makePortal  (edgenode_t&,
                                                  edgenode_t&,
                                                  vertex_t*,
                                                  edgenode_t&);
                void                 buildPortals(const PVSRing&); 
                void                 buildPreLeaf(int,const PortalRing&);
                int                  findFirst   (int*,int num);
     inline     BOOL                 checkProcess(int findex) {
       return mFacetProcess [findex] == -mCurLeaf || 
              (mFacetProcess [findex] <= mCurLeaf && mFacetProcess [findex] > 0);
     }     

  protected:  
           int*         mFacetProcess;
           facetedge_t* mFacetEdge;
           EdgeRing     mEdgeRing;
           PortalRing   mPortalRing;
           LeafRing     mLeafRing;
           float        mPlaneParam;
           BOOL         mAll;
           int          mCurLeaf;
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

        //sub -1 from each leaf!!!!!!!

    for (mCurLeaf=1;(ffirst=findFirst (mFacetProcess,mCount [PK_FACET]))!=-MAX_LEAFS;mCurLeaf++)
    {
      dout<<"Leaf: "<<mCurLeaf<<endl;
      PVSRing    ring (pool());

      memset       (mFacetEdge,0,sizeof (facetedge_t) * mCount [PK_FACET]);

      buildPVS     (ring,vnode,ffirst,-1);      
      dout<<"pvs process "<<ring.count ()<<endl;
      buildEdges   (ring);      
      dout<<"process edges "<<mEdgeRing.count ()<<endl;
      buildPortals (ring);
      dout<<"portals process"<<endl;
      buildPreLeaf (mCurLeaf,mPortalRing);

      dout<<"Build PVS: Facets: "<<ring.count ()
          <<" Edges: "<<mEdgeRing.count ()
          <<" Portals: "<<mPortalRing.count ()<<endl;

      total += ring.count ();          

      cout<<"%"<<float(total)/float(mCount [PK_FACET])*100.0f<<endl;

      mPortalRing.reset ();
    }

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
//        dout<<"from: "<<*findex<<" "<<mFacetProcess [*findex]<<endl;

        if (!checkProcess (*findex))
        {
          if (checkPVS (ring,mFacet [*findex],facet))
            buildPVS (ring,vnode,*findex,ffirst);
          else
            mFacetProcess [*findex] = -mCurLeaf;  
        }     
//        dout<<"to: "<<*findex<<" "<<mFacetProcess [*findex]<<endl;

        if (mFacetProcess [*findex] == mCurLeaf)
          checkEdges (*findex,ffirst);
//        dout<<"next "<<*findex<<endl;          
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

edgenode_t*    PortalKeeper::findEdge    (const EdgeRing& ring,int vertex,const edgenode_t& edge)
{
  EdgeRing::iterator  iter  = ring;
  int                 count = ring.count ();

  for (int i=0;i<count;i++,iter++)
  {
//    dout<<"Search edge: "<<iter.data ().vertex [0]<<" "<<iter.data ().vertex [1]<<endl;
    if ((!memcmp (mVertex + iter.data ().vertex [0],mVertex + vertex,sizeof (vertex_t))  || 
         !memcmp (mVertex + iter.data ().vertex [1],mVertex + vertex,sizeof (vertex_t))) &&
          memcmp (&edge,&iter.data(),sizeof (edgenode_t)))
      return &iter.data ();
  }

  return NULL;  
}

BOOL  PortalKeeper::checkPortal (const PVSRing& ring,const vertex_t* v)
{
  D_Try
  {
    vector3d_t  n = (v [1] - v [0]) ^ (v [2] - v [0]);

        //recalc plane param

    plane_t   plane;

    plane.a   = n.x;
    plane.b   = n.y;    
    plane.c   = n.z;    
    plane.d   = -(v [0] & n);

    normalize (plane);

       //check for all

    PVSRing::iterator iter  = ring;
    int               num   = ring.count ();
    vertex_t&         v     = mVertex [mFacet [iter.data()].index [0]];
    float             state = (v.x*plane.a + v.y*plane.b + v.z*plane.c + plane.d);

    if (fabs (state) < EPS)
      state = 0.0f;

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
  
          if (fabs (s+state) < fabs (s-state)) 
          {
//            dout<<"S: "<<s<<" State: "<<state<<endl;

            return FALSE;
          }  
        }
      }  
    }

    return TRUE;    
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    dout<<"Error at checkPortal "<<endl;           
    return FALSE;
  }
}

int     PortalKeeper::makePortal  
 ( edgenode_t&   edge1,
   edgenode_t&   edge2,
   vertex_t*     v,
   edgenode_t&   edge  )
{
  int  state = 0;
  int  cross = 0;
   
  for (int i=0;i<2;i++)
    for (int j=0;j<2;j++)
      if (edge1.vertex [i] == edge2.vertex [j])
      {
        state++;
        cross = edge1.vertex [i];
      }  

  if (state != 1)
    return state;

  if (edge1.vertex [1] != cross)  swap (edge1.vertex [0],edge1.vertex [1]);
  if (edge2.vertex [0] != cross)  swap (edge2.vertex [0],edge2.vertex [1]);

  v [0] = mVertex [edge1.vertex [0]];
  v [1] = mVertex [edge2.vertex [1]];
  v [2] = mVertex [edge2.vertex [0]];

  edge.vertex [0] = edge1.vertex [0];
  edge.vertex [1] = edge2.vertex [1];  

  return state;    
}

void   PortalKeeper::buildPortals(const PVSRing& ring)
{
  if (mEdgeRing.count () == 0)
    return;

//  dout<<"Build Portals: Facets: "<<ring.count ()
//      <<" Edges: "<<mEdgeRing.count ()<<endl;

  vertex_t      v [3];
  edgenode_t    edge;  
  EdgeRing      ring1   (pool());    
  EdgeRing      ring2   (pool());
  EdgeRing*     process = &mEdgeRing;
  EdgeRing*     stack   = &ring1;
    
  do
  {
    for (EdgeRing::iterator iter = *process;process->count ();)
    {
      int         state = 0;
      BOOL        flag  = FALSE;
      edgenode_t  edge1 = iter.data ();

      for (int i=0;i<2;i++)
      {
//      dout<<endl<<"Look for edge: "<<edge1.vertex [0]<<" "<<edge1.vertex [1]<<endl;
        edgenode_t* edge2 = findEdge (*process,edge1.vertex [i],edge1);
   
//        dout<<"end look ";
//        if (edge2) dout<<edge2->vertex [0]<<" "<<edge2->vertex [1]<<endl;
//        else       dout<<endl;

        if (edge2 == NULL)
          continue;  

        if ((state = makePortal (edge1,*edge2,v,edge)) != 1)
        {
//          dout<<"Error makePortal while buildPortals ()"<<endl;
          if (state == 2) 
          {
            process->erase (*edge2);
            i--;
          }  
          if (state == 0) 
          {
            dout<<"Compile halt!"<<endl;
            return;
          }  
          continue;
        }
    
        if (checkPortal (ring,v))
        {
//        dout<<"Create portal:"<<endl;
//        dout<<"\tEdge1: "<<edge1.vertex [0]<<" "<<edge1.vertex [1]<<endl;
//        dout<<"\tEdge2: "<<edge2->vertex [0]<<" "<<edge2->vertex [1]<<endl;

          portal_t portal;

          portal.n      = (v [1] - v [0]) ^ (v [2] - v [0]);
          portal.flags  = 0;
          portal.index  = -1;

          memcpy (portal.v,v,3 * sizeof (vertex_t));
   
          process->erase     (*edge2);
          process->erase     (iter);
          stack->insert      (edge);
          mPortalRing.insert (portal);

//        dout<<"Insert edge: "<<edge.vertex [0]<<" "<<edge.vertex [1]<<endl;

          flag = TRUE;
          break;  
        }
      }

      if (!flag && stack->count () == 0)
      {
        process->erase (edge1);//iter); //?????!!!!!
        iter = *process;
      }    
      else if (!flag)
      {
        stack->insert  (edge1);//iter.data());
        process->erase (edge1);//iter);          
  
        iter = *process;
      }
    }

  //  dout<<endl<<"rechange "<<process->count()<<" "<<stack->count()<<endl;

    process = stack;
    stack   = (stack == &ring1) ? &ring2 : &ring1;
  } while (process->count ());
}

void    PortalKeeper::buildPreLeaf (int pvs,const PortalRing& portal)
{
  preleaf_t leaf;

  leaf.pvs     = pvs;
  leaf.portal  = portal;

  mLeafRing.insert (leaf);
}

int   PortalKeeper::findFirst   (int* table,int num)
{
  for (int i=0;i<num;i++,table++)
    if (*table > mCurLeaf || *table < 0)
      return i;

  return -MAX_LEAFS;
}

PortalKeeper::PortalKeeper  (Pool* _Pool,Object3D& obj,float _PlaneParam,BOOL _All)
             : PolygonKeeper (_Pool,obj),
               mFacetProcess (NULL),
               mEdgeRing     (_Pool),
               mPortalRing   (_Pool),                             
               mLeafRing     (_Pool),
               mAll          (_All),
               mPlaneParam   (_PlaneParam),
               mCurLeaf      (0)
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
