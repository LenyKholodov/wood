//#include <pch.h>
#include <graph\striper\stdafx.h>
#include <graph\striper.h>

stripdest_t*    ComputeStrip    (const stripsrc_t& src,Pool* pool)
{
//  D_Try
//  {
    STRIPERCREATE sc;
    STRIPERRESULT sr;
    Striper       strip;
    stripdest_t*  dest  = NULL;

    sc.DFaces           = src.faces;
    sc.NbFaces          = src.count;
    sc.AskForWords      = false;
    sc.ConnectAllStrips = src.flags & STRIP_CONNECT_ALL_STRIPS;
    sc.OneSided         = src.flags & STRIP_ONE_SIDED_STRIPS; 
    sc.SGIAlgorithm     = src.flags & STRIP_USE_SGI_ALGORITHM;

    if (!strip.Init  (sc))
    {
//      #ifdef DEBUG
//        dout<<"Error at init striper "<<endl;        
//      #endif
      return NULL;
    }

    if (!strip.Compute (sr))
    {
//      #ifdef DEBUG
//        dout<<"Error at compute strips "<<endl;
//      #endif
      return NULL;
    }

    int                 num    = sr.NbStrips;
    int                 count  = 0;
    udword*             len    = sr.StripLengths;
    udword*             refs   = (udword*)sr.StripRuns;

    if (!len)
      return NULL;
    
    for (int i=0;i<num;count+=len [i++]);

    dest  = (stripdest_t*)pool->alloc (sizeof (stripdest_t) + 
                         sizeof (stripdest_t::strip_t) * num +
                         sizeof (uint) * count
                         );

    if (!dest)
      return FALSE;

    dest->count                    = num;  

    uint*                 stripPos = (uint*)((char*)dest + sizeof (stripdest_t) + sizeof (stripdest_t::strip_t) * num);
    stripdest_t::strip_t* desc     = (stripdest_t::strip_t*)((char*)dest + sizeof (stripdest_t));

    for (i=0;i<num;i++,desc++)
    {
      desc->len    = len [i];
      desc->strip  = stripPos;

      memcpy (desc->strip,refs,len [i] * sizeof (udword));

      stripPos    += len [i];
      refs        += len [i];
    }

    return dest;
//  }
//  D_ExcRet ("Error at ComputeStrip",NULL);
}

stripdest_t*    ComputeStrip    
 ( const RingTmpl<facet_t>& ring,
   uint                     flags,
   Pool*                    pool )
{
  D_Try
  {
    if (!ring.count ())
      return NULL;

    M_PushPool (pool);

    stripsrc_t src;

    src.count  = ring.count ();
    src.flags  = flags;
    src.faces  = new uint [src.count * 3];

    if (!src.faces)
      return NULL;

    RingTmpl<facet_t>::iterator iter = ring;
    uint*                       face = src.faces;

    for (int i=0;i<src.count;i++,iter++,face+=3)
    {
      face [0] = iter.data ().index [0];
      face [1] = iter.data ().index [1];
      face [2] = iter.data ().index [2];
    }

    stripdest_t* dest = ComputeStrip (src,pool);

    delete [] src.faces;

    M_PopPool  ();

    return dest;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at ComputeStrip"<<endl;
    #endif
    M_PopPool  ();
    return NULL;
  }
}

stripdest_t*    ComputeStrip    
 ( const RingTmpl<int>&     ring,
   facet_t*                 facet,
   uint                     flags,
   Pool*                    pool )
{
  D_Try
  {
    if (!ring.count ())
      return NULL;

    M_PushPool (pool);

    stripsrc_t src;

    src.count  = ring.count ();
    src.flags  = flags;
    src.faces  = new uint [src.count * 3];

    if (!src.faces)
      return NULL;

    RingTmpl<int>::iterator iter = ring;
    uint*                   face = src.faces;

    for (int i=0;i<src.count;i++,iter++,face+=3)
    {
      face [0] = facet [iter.data ()].index [0];
      face [1] = facet [iter.data ()].index [1];
      face [2] = facet [iter.data ()].index [2];
    }

    stripdest_t* dest = ComputeStrip (src,pool);

    delete [] src.faces;

    M_PopPool  ();

    return dest;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at ComputeStrip"<<endl;
    #endif
    M_PopPool  ();
    return NULL;
  }
}

stripdest_t*    ComputeStrip    
 ( uint                     count,
   facet_t*                 facet,
   uint                     flags,
   Pool*                    pool )
{
  D_Try
  {
    if (!count)
      return NULL;

    M_PushPool (pool);

    stripsrc_t src;

    src.count  = count;
    src.flags  = flags;
    src.faces  = new uint [src.count * 3];

    if (!src.faces)
      return NULL;

    uint*                   face = src.faces;

    for (int i=0;i<src.count;i++,face+=3)
    {
      face [0] = facet [i].index [0];
      face [1] = facet [i].index [1];
      face [2] = facet [i].index [2];
    }

    stripdest_t* dest = ComputeStrip (src,pool);

    delete [] src.faces;

    M_PopPool  ();

    return dest;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at ComputeStrip"<<endl;
    #endif
    M_PopPool  ();
    return NULL;
  }
}
