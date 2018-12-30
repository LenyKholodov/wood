template <class T,class key_t>
Map<T,key_t>::dataiterator Map<T,key_t>::operator [] (key_t i) throw (ExcNoElement&, ExcNoMemory&)
{
  iterator& iter = find (i);

  if (!iter.valid ())
  {
    switch (mMode)
    {
      case EXCEPT: throw ExcNoElement ("Map::operator []");
      case CREATE: insert (i,0);
                   return find (i).toData ();
      case EMPTY:  return dataiterator (*(node_t*)NULL);
    }
  }

  return iter;
}                               