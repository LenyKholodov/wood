#include "pch.h"
#include "cv_utils.h"

struct  tcnode_t
{   
  uchar         use;
  int           next;
};

inline BOOL CheckTextCoords (const textcoord_t& t1,const textcoord_t& t2)
{
  return fabs (t1.u - t2.u) < EPS  && fabs (t1.v - t2.v) < EPS;
}

uint    ConvertFacetArray (
  facet_t*                farray,  
  uint                    fCount,
  int*                    mirror,
  textcoord_t*            textCoord,
  uint                    mCount,
  uint                    vCount
)
{
  D_Try
  {
    facet_t*            facet  = farray;
    tcnode_t*           tcnode = new tcnode_t [mCount];
    int                 aPos   = vCount;

    for (int i=0;i<vCount;i++)
    {
      mirror [i]      = i;
      tcnode [i].next = -1;
      tcnode [i].use  = FALSE;
    }

    for (;i<mCount;i++)
    {
      mirror [i]      = -1;
      tcnode [i].next = -1;
      tcnode [i].use  = FALSE;
    }    

    for (i=0;i<fCount;i++,facet++)
    {
      if (facet->texture != -1)
      {
        for (int j=0;j<3;j++)
        {
          textcoord_t& coord  = facet->tindex [j];
          int          index  = facet->index [j];
          int          rIndex = index;

          if (!tcnode [index].use)
          {
            tcnode [index].use   = TRUE;
            textCoord [index]    = coord;
          }
          else if (!CheckTextCoords (textCoord [index],coord))
          {
            int next = tcnode [index].next;
            for (;!CheckTextCoords (textCoord [index],coord) && next != -1;
                 index = next,next = tcnode [next].next);

            if (!CheckTextCoords (textCoord [index],coord))
            {
              if (aPos == mCount)
              {
                #ifdef DEBUG
                  dout<<"Error at ConvertFacetArray: No enough mirror references"<<endl;
                #endif
                return 0;
              }

              tcnode [index].next  = index = aPos++;
              tcnode [index].use   = TRUE;

              textCoord [index]    = coord;
              mirror [index]       = rIndex;
            }            
          }     

          facet->index [j] = index;
        }
      }      
    }

    delete [] tcnode;

    return aPos;
  }
  D_ExcRet ("Error at ConverFacetArray",0);
}
