#include <graph\grtypes.h>
#include <memory\memory.h>

const float EPS = 0.1;

struct  span_t;
struct  visnode_t
{
  int   index, detail;
};

////////////////////////////////////////////////////////////////////////////////
///Дерево высот для отсечение невидимых тайлов
////////////////////////////////////////////////////////////////////////////////
class   SBuffer: public MemObject
{
  public:
                SBuffer  (Pool*,int _NodeCount,int _SegmentSize = 4096);
    virtual     ~SBuffer ();

        void            reset   ();
        void            set     (const vector3d_t& pos,float angle,const float*);
        BOOL            insert  (int index,int detail,const boundbox_t&);
        BOOL            check   (const boundbox_t&);
        visnode_t*      get     (int& count);

  protected:
        void            cross   (const line_t&,float&,float&);
        void            calc    (const boundbox_t&,span_t&);
        BOOL            insert  (const span_t&);

  private:
        int             mSpanPos, mVisPos, mAllocSize;
        visnode_t*      mVisNode;
        span_t*         mSpan;
        BOOL            mVisible;
        int             mDetail;
        vector3d_t      mPos;
        line_t          mPrjLine;
        float           mCos, mSin, mScale;
        int             mSegSize;
};
