template <class T>
T       Queue<T>::pop   (bool exc) throw (ExcEmpty&)
{
  if (!count ())
  {
    if (exc) throw ExcEmpty ("Queue::pop");

    return iterator ();
  }  

  iterator& i = GetLast ();
  T         x = i;

  erase (i);

  return x;
}

template <class T>
bool    Queue<T>::pop   (T& x)
{
  if (!count ())
    return false;

  x = GetLast ();

  erase (GetLast ());

  return true;
}