

inline CurrentContextPtr::operator void* () const
{ 
  return data (); 
}

inline CurrentContextPtr&  CurrentContextPtr::operator = (void* data) 
{ 
  return set (data); 
}

inline CurrentContextPtr&  CurrentContextPtr::operator = (const CurrentContextPtr& ptr)
{
  set (ptr.data ());  
  return *this;
}

inline void*   CurrentContextPtr::data () const 
{ 
  return mCurContext == NO_LINK ? NULL : (void*)TlsGetValue (mCurContext);
}

template <class Type>
inline threadptr_t<Type>&  threadptr_t<Type>::operator = (const threadptr_t<Type>& ptr)
{
  set (ptr.data ());
  return *this;
}

template <class Type>
inline threadptr_t<Type>&  threadptr_t<Type>::operator = (const Type* data)
{
  set ((void*)data);
  return *this;
}

template <class Type>
inline Type* threadptr_t<Type>::operator -> () const
{
  return (Type*)data ();
}

template <class Type>
inline  threadptr_t<Type>::operator Type* () const
{
  return (Type*)data ();
}
       
template <class Type>
inline Type&  threadptr_t<Type>::operator * () const
{
  return *(Type*)data ();
}

template <class Type>
bool threadptr_t<Type>::operator == (const Type* obj)
{
  return (Type*)data () == obj;
}

template <class Type>
bool threadptr_t<Type>::operator != (const Type*)
{
  return (Type*)data () != obj;
}

template <class Type>
bool threadptr_t<Type>::operator ! () const
{
  return (Type*)data () == NULL;
}

template <class Type>
threadptr_t<Type>::threadptr_t  (const Type* data)
                  : CurrentContextPtr ((void*)data)
{   }

template <class Type>
threadptr_t<Type>::threadptr_t  ()
{   }
