    if (num == 1)
    {
      edgenode_t     edge [2];
      static int addt [3] = {1,2,0};
      static int subt [3] = {2,0,1};

      edge [0].vertex [0] = edge [1].vertex [0] = vindex;
      edge [0].vertex [1] = facet.index [addt [j]];
      edge [1].vertex [1] = facet.index [subt [j]];

      mEdgeRing.insert (edge [0]);
      mEdgeRing.insert (edge [1]);
    }

        else
        {
          edgenode_t edge;
          int        find;

          for (int k=0;k<3;k++)
            for (int l=0;l<3;l++)
              if (mFacet [*findex].index [k] == facet.index [l])
                find = facet.index [l];

          edge.vertex [0] = vindex;
          edge.vertex [1] = find;

          mEdgeRing.insert (edge);
        }       

  if (ffrom != -1)
  {
    int       el  [4] = {0};
    int       el1 [4] = {0};
    int       pos     = 0;
    facet_t&  prev    = mFacet [ffrom];

    for (int k=0;k<3;k++)    
      for (int l=0;l<3;l++)
        if (prev.index [l] == facet.index [k])        
        {
          el  [pos]   = l;
          el1 [pos++] = k;
        }  

    for (int  i=0;i<3;i++)
      dout<<facet.index [i]<<endl;

    for (i=0;i<3;i++)
      dout<<prev.index [i]<<endl;

    dout<<endl<<pos<<endl<<endl;

    if (pos == 2)
    {
      int  edge   = el [0] < el [1] ? el [0] : el [1];
      int  edge1  = el1 [0] < el1 [1] ? el1 [0] : el1 [1];

      mFacetEdge [ffrom].edge [edge]   = TRUE;
      mFacetEdge [ffrom].process++;

      mFacetEdge [ffirst].edge [edge1] = TRUE;
      mFacetEdge [ffirst].process++;
    }  
  }

void     PortalKeeper::buildEdges  ()
{
  int count = mCount [PK_FACET];

  for (int i=0;i<count;i++)
    if (mFacetEdge [i].process < 3)
    {  
      for (int j=0;j<3;j++)
        if (!mFacetEdge [i].edge [j])
        {
          static edgenode_t edge2node [3] = {
            {0,1},
            {1,2},
            {2,0}
          }; 

          mEdgeRing.insert (edge2node [j]);
        }            
      dout<<"facet : "<<i<<" "<<(int)mFacetEdge [i].process<<endl;          
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
      mFacetEdge [ffrom].edge [edge]   = TRUE;
      mFacetEdge [ffrom].process++;

      dout<<"facet: "<<ffrom<<" edge: "<<edge<<" num: "<<(int)mFacetEdge [ffrom].process<<endl;
    }

    if (!mFacetEdge [ffirst].edge [edge1])
    {
      mFacetEdge [ffirst].edge [edge1] = TRUE;
      mFacetEdge [ffirst].process++;

      dout<<"facet: "<<ffirst<<" edge: "<<edge1<<" num: "<<(int)mFacetEdge [ffirst].process<<endl;
    }
  }    
}

void     PortalKeeper::buildPVS     (PVSRing& ring,vertexnode_t* vnode,int ffirst,int ffrom)
{
  facet_t& facet = mFacet [ffirst];
        
  if (mFacetProcess [ffirst])
    return;

  ring.insert   (&facet);
  mFacetProcess [ffirst] = TRUE;

  for (int j=0;j<3;j++)
  {
    int   vindex = facet.index [j];
    int   num    = vnode [vindex].num;
    int*  findex = vnode [vindex].facet;     

    dout<<"Compile: "<<ffirst<<": ";
    for (int k=0;k<num;k++)
      dout<<findex [k]<<" ";
    dout<<endl;       

    for (int i=0;i<num;i++,findex++)
    {
      if (*findex != ffirst)
      {
        if (!mFacetProcess [*findex])
        {
          if (checkPVS (ring,mFacet [*findex],facet))
            buildPVS (ring,vnode,*findex,ffirst);
          else
            mFacetProcess [*findex] = -1;  
        }     

        if (mFacetProcess [*findex])
          checkEdges (ffirst,*findex);
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

void     PortalKeeper::buildEdges  (const PVSRing& ring)
{
  int                   count = ring.count ();
  PVSRing::iterator     iter  = ring;

  for (int i=0;i<count;i++,iter++)
  {
    int index = long(iter-mFacet);

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

  if (mEdgeRing.count () == 0)
    return;

  dout<<"Build Portals: Facets: "<<ring.count ()
      <<" Edges: "<<mEdgeRing.count ()<<endl;

  vertex_t      v [3];
  edgenode_t    edge;  

  for (EdgeRing::iterator iter = mEdgeRing;mEdgeRing.count ();)
  {
    dout<<"Count: "<<mEdgeRing.count ()<<endl;

    int         state = 0;
    edgenode_t  edge1 = iter.data ();
    dout<<"Look for edge: "<<edge1.vertex [0]<<" "<<edge1.vertex [1]<<endl;
    edgenode_t* edge2 = findEdge (edge1.vertex [1],edge1);

    if (edge2 == NULL)
      edge2 = findEdge  (edge1.vertex [0],edge1);

    dout<<"end look"<<endl;

    if (edge2 == NULL && mEdgeRing.count () > 1)
    {
      dout<<"Error at find edge while process 2 vertex"<<endl;
      dout<<"Edge: "<<edge1.vertex [0]<<" "<<edge1.vertex [1]<<endl;
      mEdgeRing.erase (iter);
      continue;
    }      
    else if (edge2 == NULL && mEdgeRing.count () == 1)
    {
      mEdgeRing.erase (iter);
      iter = mEdgeRing;
      continue;
    }  

    if ((state = makePortal (edge1,*edge2,v,edge)) != 1)
    {
      dout<<"Error makePortal while buildPortals ()"<<endl;
      if (state == 2) 
        mEdgeRing.erase (*edge2);
      if (state == 0) 
      {
        dout<<"Compile halt!"<<endl;
        return;
      }  
      continue;
    }
    
    if (checkPortal (ring,v))
    {
      dout<<"Create portal:"<<endl;
      dout<<"\tEdge1: "<<edge1.vertex [0]<<" "<<edge1.vertex [1]<<endl;
      dout<<"\tEdge2: "<<edge2->vertex [0]<<" "<<edge2->vertex [1]<<endl;

      portal_t portal;

      portal.n      = (v [1] - v [0]) ^ (v [2] - v [0]);
      portal.flags  = 0;
      portal.index  = -1;

      memcpy (portal.v,v,3 * sizeof (vertex_t));

      mEdgeRing.erase    (*edge2);
      mEdgeRing.erase    (iter);
      mEdgeRing.insert   (edge);
      mPortalRing.insert (portal);

      dout<<"Insert edge: "<<edge.vertex [0]<<" "<<edge.vertex [1]<<endl;
    }
    else
      iter++;
  }

      if (mEdgeRing.count () == 2)
      {
        PVSRing::iterator  iter = ring;
        facet_t&           f1   = mFacet [iter.data ()]; iter++;
        facet_t&           f2   = mFacet [iter.data ()]; iter++;

        dout<<"Facet1: " <<f1.index [0]<<" "<<f1.index [1]<<" "<<f1.index[2]<<endl;
        dout<<"Facet2: " <<f2.index [0]<<" "<<f2.index [1]<<" "<<f2.index[2]<<endl;
      }

  if (mEdgeRing.count () == 0)
    return;

  dout<<"Build Portals: Facets: "<<ring.count ()
      <<" Edges: "<<mEdgeRing.count ()<<endl;

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
      edgenode_t  edge1 = iter.data ();
//      dout<<endl<<"Look for edge: "<<edge1.vertex [0]<<" "<<edge1.vertex [1]<<endl;
      edgenode_t* edge2 = findEdge (*process,edge1.vertex [1],edge1);

      if (edge2 == NULL)
        edge2 = findEdge  (*process,edge1.vertex [0],edge1);

      dout<<"end look ";
      if (edge2) dout<<edge2->vertex [0]<<" "<<edge2->vertex [1]<<endl;
      else       dout<<endl;

      if (edge2 == NULL && process->count () <= 2 && stack->count () == 0)
      {
//        dout<<"Error at find edge while process 2 vertex"<<endl;
//        dout<<"Edge: "<<edge1.vertex [0]<<" "<<edge1.vertex [1]<<endl;
        process->erase (iter);
        process->erase (iter);
        iter = *process;
        continue;      
      }    
      else if (edge2 == NULL)
      {
        stack->insert  (iter.data());
        process->erase (iter);          

        if (process->count () == 1) iter = *process;
        continue;
      }      

      if ((state = makePortal (edge1,*edge2,v,edge)) != 1)
      {
//        dout<<"Error makePortal while buildPortals ()"<<endl;
        if (state == 2) 
          process->erase (*edge2);
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
      }
      else
        iter++;
    }

    dout<<endl<<"rechange "<<process->count()<<" "<<stack->count()<<endl;

    process = stack;
    stack   = (stack == &ring1) ? &ring2 : &ring1;
  } while (process->count ());