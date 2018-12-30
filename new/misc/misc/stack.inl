template <class T>
T       Stack<T>::pop   (bool exc) throw (ExcEmpty&)
{
  if (!count ())
  {
    if (exc) throw ExcEmpty ("Stack::pop");

    return iterator ();
  }  

  iterator& i = GetFirst ();
  T         x = i;

  erase (i);

  return x;
}

template <class T>
bool    Stack<T>::pop   (T& x)
{
  if (!count ())
    return false;

  x = GetFirst ();

  erase (GetFirst ());

  return true;
}

template <class T>
bool    FixedStack<T>::push  (const T& x,bool exc) throw (ExcFull&)
{
  if (mCount == mPos)
  {  
    if (exc) throw ExcFull ("FixedStack::push");
    return false;
  }  

  mStack [mPos++] = x;

  return true;
}

template <class T>
T       FixedStack<T>::pop   () throw (ExcEmpty&)
{
  if (!mPos)
    throw ExcEmpty ("FixedStack::pop");

  return mStack [--mPos];
}

template <class T>
bool    FixedStack<T>::pop   (T& x)
{
  if (!mPos)
    return false;

  x = mStack [--mPos];

  return true;
}

template <class T>
FixedStack<T>::FixedStack  (uint count,Pool* pool) throw (ExcNoMemory&)
              : mStack (new (pool) T [count]),
                mCount (count),
                mPos   (0)
{
  if (!mStack)
    throw ExcNoMemory ("FixedStack::FixedStack",sizeof (T) * count);
}

template <class T>
FixedStack<T>::~FixedStack ()
{
  if (mStack) delete [] mStack;
}
