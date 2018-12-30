#include "render.h"
#include "sort.h"

#pragma pack (push,1)

struct Render::Filter
{
  typedef void (*FilterFn)(Primitive*,uint,bool);

  FilterFn filter;
};

#pragma pack (pop)

////////////////////////////////////////////////////////////////////////////////////////////
///Render functions
////////////////////////////////////////////////////////////////////////////////////////////

inline void Render::NextFilter (Primitive* first,uint count,bool already_sorted)
{
  if (mFilter [CURRENT] != mFilter [LAST])
  {
    mFilter [CURRENT]++;
    mFilter [CURRENT][-1].filter (first,count,already_sorted);
    mFilter [CURRENT]--;
  }  
}

////////////////////////////////////////////////////////////////////////////////////////////
///Material sort
////////////////////////////////////////////////////////////////////////////////////////////

inline int FrontToBack (const Primitive& p1,const Primitive& p2)
{
  return p1.depth - p2.depth;
}

inline int BackToFront (const Primitive& p1,const Primitive& p2)
{
  return p2.depth - p1.depth;
}

void Render::MaterialSort (Primitive* first,uint count,bool already_sorted)
{
  struct Selectors {
    inline static SortMode&  GetSortMode (Primitive& p) { return p.state->mtl->sort_mode; }    
    inline static Material*& GetMaterial (Primitive& p) { return p.state->mtl; }
  };
        
  if (!already_sorted)
  {
    SortIter<SortMode,Selectors::GetSortMode> iter (first,count);

    for (;iter;iter.next ())
    {
      switch (iter->attrib)
      {
        case BACK_TO_FRONT:
          QuickSort<Primitive,BackToFront> (first,count);
          break;      
        case FRONT_TO_BACK:
          QuickSort<Primitive,FrontToBack> (first,count);
          break;
      }

      SortIter<Material*,Selectors::GetMaterial> iter1 (iter->first,iter->count);

      for (;iter1;iter1.next ())
      {
        //применить материал
        mMaterial = iter1->attrib;

        mMaterial->apply ();
        
        NextFilter (iter1->first,iter1->count,false);
      }
    }
  }
}

void Render::RenderPrimitives (Primitive*,uint)
{
}

void Render::render (Primitive* primitives,uint count)
{
  NextFilter (primitives,count,false);
}
