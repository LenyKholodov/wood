
                edgenode_t*          findEdge    (const EdgeRing&,
                                                  int vertex,
                                                  const edgenode_t&);
                BOOL                 checkPortal (const PVSRing&,const vertex_t*);
                int                  makePortal  (edgenode_t&,
                                                  edgenode_t&,
                                                  vertex_t*,
                                                  edgenode_t&);
                void                 buildPortals(const PVSRing&); 

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