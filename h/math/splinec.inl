template <class T>
T&   SplineCube<T>::recalc  (Time t,T& res) const
{
  seg_t* seg = mSegs;

  if      (t < mSegs [0].start)     t = mSegs [0].start;
  else if (t > mSegs [count-1].end) t = mSegs [count-1].end;

  for (int i=0;i<count && seg->end > t && seg->start < t;i++,seg++);

  t        = ease (t,seg->easeFrom,seg->easeTo);
  float t2 = t*t,
        t3 = t2*t;

  return res = seg->a * t3 + seg->b * t2 + seg->c * t + seg->d;
}

template <class T>
T   SplineCube<T>::recalc  (Time t) const
{
  seg_t* seg = mSegs;  

  if      (t < mSegs [0].start)     t = mSegs [0].start;
  else if (t > mSegs [count-1].end) t = mSegs [count-1].end;

  for (int i=0;i<count && seg->end > t && seg->start < t;i++,seg++);

  t        = ease (t,seg->easeFrom,seg->easeTo);
  float t2 = t*t,
        t3 = t2*t;

  return seg->a * t3 + seg->b * t2 + seg->c * t + seg->d;
}

template <class T>
Time   SplineCube<T>::ease (Time t,Time from,Time to) 
{
  float k;
  float s = from + to;

  if (s == 0.0) return t;

  if (s > 1.0) 
  {
    from = from / s;
    to = to / s;
  }

  k = 1.0 / (2.0 - from - to);

  if (t < from)    
    return ((k / from) * t * t);
  else 
  {
    if (t < 1.0 - to)
      return (k * (2 * t - from));
    else 
    {
      t = 1.0 - t;
      return (1.0 - (k / to) * t * t);
    }
  }
}

template <class T>
void    SplineCube<T>::build  (key_t* keys,uint count)
{
}
