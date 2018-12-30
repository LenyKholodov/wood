#ifndef __RENDER_MESH_RENDER__
#define __RENDER_MESH_RENDER__

#include "defs.h"
#include <misc.h>

////////////////////////////////////////////////////////////////////////////////
///Рендер мэшей
////////////////////////////////////////////////////////////////////////////////
class    MeshRender: public MemObject
{
  public:
                        MeshRender  (Pool*,int _MeshesCacheNum);
           virtual      ~MeshRender ();

           BOOL         DrawMesh    (int,ulong _Id);

           BOOL         BeginMesh   (ulong _Id,int& list);
           void         EndMesh     ();

  private: 
           struct       meshid_t
           {            
             uint       list;
             long       id;
           };

           typedef Ring<meshid_t>    IRing;
           typedef Dict<int>   IDict;
       
           IRing                         mIndexRing;
           IDict                         mIndexDict;
           uint                          mFirstList, mListNum;
           int                           mCurList;
           IRing::iterator*              mIter;
};

#endif