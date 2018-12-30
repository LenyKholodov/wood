////////////////////////////////////////////////////////////////////////////////////////////
///Inlines
////////////////////////////////////////////////////////////////////////////////////////////

template <class type>
inline void swap (type& a,type& b)
{
  type t = a;
  a      = b;
  b      = t;
}

template <class    Attrib,
          Attrib&  get   (Primitive&),
          bool     cmp   (const Attrib&,const Attrib&)
         >
void SortIter<Attrib,get,cmp>::next ()
{
  uint len    = 0,
       i      = count;

  Attrib& attrib = get (*first);

  for (Primitive* iter1=first;i && cmp (get (*iter1),attrib);iter1++,i--,len++);
  for (Primitive* iter2=iter1;i && !cmp (get (*iter2),attrib);iter2++,i--);

  while (i)
  {
    swap (*iter1,*iter2);

    len++;

    for (;iter1 != iter2 && cmp (get (*iter1),attrib);iter1++);
    for (;i && !cmp (get (*iter2),attrib);iter2++,i--);
  }

  span.first  = first;
  span.count  = len;
  span.attrib = &attrib;

  first += len;
  count -= len;
}

template <class    Attrib,
          Attrib&  get   (Primitive&),
          bool     cmp   (const Attrib&,const Attrib&)
         >
SortIter<Attrib,get,cmp>::SortIter (Primitive* _first,uint _count)
  : first (_first), count (_count)
{
  next ();
}

template <class    Attrib,
          Attrib&  get   (Primitive&),
          bool     cmp   (const Attrib&,const Attrib&)
         >
void SearchIter<Attrib,get,cmp>::next ()
{
  uint len    = 0,
       i      = count;

  Attrib& attrib = get (*first);

  for (Primitive* iter1=first;i && cmp (get (*iter1),attrib);iter1++,i--,len++);

  span.first  = first;
  span.count  = len;
  span.attrib = &attrib;

  first += len;
  count -= len;
}

template <class    Attrib,
          Attrib&  get   (Primitive&),
          bool     cmp   (const Attrib&,const Attrib&)
         >
SearchIter<Attrib,get,cmp>::SearchIter (Primitive* _first,uint _count)
  : first (_first), count (_count)
{
  next ();
}

template <class T,int cmp (const T&,const T&)>
QuickSort<T,cmp>::QuickSort (T* base,unsigned num)
{
  const int cutoff = 8;

  if (num < 2)
      return;

  int stkptr = 0;             
  T*  lo     = base;
  T*  hi     = base + num - 1;
  T   *lostk [40], *histk[40];

recurse: //label

  size_t size = (hi - lo) + 1; 

  if (size <= cutoff)  //short sort
  {
    T *h = hi,
      *l = lo;

    while (h > l) 
    {
      T* max = l;

      for (T* p=l+1;p<=h;++p) 
        if (cmp (*p,*max) > 0)  
          max = p;

      if (max != h)
        swap (*max,*h);

      h--;
    }
  }
  else 
  {
    T* mid = lo + (size >> 1);

    if (mid != lo)
      swap (*mid,*lo);

    T* loguy = lo;
    T* higuy = hi + 1;

    while (1)
    {
      while (++loguy <= hi && cmp (*loguy,*lo) <= 0);
      while (--higuy > lo  && cmp (*higuy,*lo) >= 0);

      if (higuy < loguy)
         break;

      if (loguy != higuy)
        swap (*loguy,*higuy);
    }

    if (lo != higuy)
      swap (*lo,*higuy); 

    if ((char*)higuy - 1 - (char*)lo >= hi - loguy)  //?? -1???
    {
      if (lo + 1 < higuy) 
      {
        lostk [stkptr] = lo;
        histk [stkptr] = higuy - 1;
        ++stkptr;
      }                   

      if (loguy < hi) 
      {
        lo = loguy;
        goto recurse;   
      }
    }
    else 
    {
      if (loguy < hi) 
      {
        lostk[stkptr] = loguy;
        histk[stkptr] = hi;
        ++stkptr;       
      }

      if (lo + 1 < higuy) 
      {
        hi = higuy - 1;
        goto recurse;    
      }
    }
  }

  --stkptr;

  if (stkptr >= 0) 
  {
    lo = lostk[stkptr];
    hi = histk[stkptr];
    goto recurse;
  }
  else return;      
}

