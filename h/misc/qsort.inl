template <class T>
inline void shortsort (T* lo,T* hi) 
{
  while (hi > lo) 
  {
    T* max = lo;

    for (T* p=lo+1;p<=hi;++p) 
      if (*p > *max)  
        max = p;

    if (max != hi)
      swap (*max,*hi);

    hi--;
  }
}

template <class T>
void QSort (T* base,unsigned num)
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

  if (size <= cutoff) 
  {
    shortsort (lo,hi);
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
      while (++loguy <= hi && *loguy <= *lo);
      while (--higuy > lo  && *higuy >= *lo);

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

template <class T,class Compare>
class QuickSort
{
  public:
    inline static void sort (T* base,unsigned num,Compare comp)
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

      if (size <= cutoff) 
      {
        shortsort (lo,hi,comp);
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
          while (++loguy <= hi && comp (*loguy,*lo) <= 0);
          while (--higuy > lo  && comp (*higuy,*lo) >= 0);

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

  private: 
    template <class T>
    inline static void shortsort (T* lo,T* hi,Compare& comp) 
    {
      while (hi > lo) 
      {
        T* max = lo;

        for (T* p=lo+1;p<=hi;++p) 
          if (comp (*p,*max) > 0)  
            max = p;

        if (max != hi)
          swap (*max,*hi);

        hi--;
      }
    } 
};

template <class T,class Compare>
void QSortEx (T* array,unsigned num,Compare comp)
{
  QuickSort<T,Compare>::sort (array,num,comp);
}
