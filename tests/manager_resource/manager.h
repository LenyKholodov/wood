
template <class Name,class Object> class Resource
{
  public:
    friend Object* lock (Resource& resource) {
      if (resource.object)
      {
        //->lock state
        return resource.object
      }
      else
      {
        //load
        return resource.object;
      }
    }

  private:
    Name    name;    
    Object* object;
};

class ResourceManager
{
  public:
        void SetCacheWindow (size_t);
        

  protected:
        void AllocWindow    (size_t);
        void UnloadAll      ();

  private:
    typedef std::list<Object*> ObjList;

    ObjList  mObjects;
};