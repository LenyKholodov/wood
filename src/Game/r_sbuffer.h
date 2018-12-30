#ifndef __SBUFFER_RENDERING_SYSTEM__
#define __SBUFFER_RENDERING_SYSTEM__

#include <graph\grtypes.h>
#include <pool.h>

#include "defs.h"

////////////////////////////////////////////////////////////////////////////////
///SBUffer для отсечения невидимых тайлов
////////////////////////////////////////////////////////////////////////////////
struct  span_t;
class   SBuffer: public MemObject
{
  public:
                SBuffer  (Pool*,int _NodeCount = 0,int _SegmentSize = 4096);
    virtual     ~SBuffer ();

        void            reset   ();
        void            set     (const vector3d_t& pos,float angle,const float*);
        BOOL            insert  (int x,int y,int index,int detail,const boundbox_t&);
        BOOL            insert  (const boundbox_t&);
        BOOL            check   (const boundbox_t&);
        visnode_t*      get     (int& count);

        virtual bool    valid   () const { return mVisNode != NULL; }

  protected:
        BOOL            cross   (const line_t&,float&,float&);
        BOOL            calc    (const boundbox_t&,span_t&);
        void            insert  (span_t&);

  private:
        int             mVisPos, mAllocSize;
        visnode_t*      mVisNode;
        float*          mSBuffer;
        BOOL            mVisible;
        vector3d_t      mPos;
        line_t          mPrjLine;
        float           mCos, mSin;
        int             mSegSize;
};

#endif