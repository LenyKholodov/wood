class ResourceManager;

class Resource
{      
  public:

  protected:
    Resource (ResourceManager&); 
    virtual ~Resource ();

  private:
    uint mRefCount;
    std::string mName;
    ResourceManager& mManager;
    time_t mLastAccess;
};

class ResourceManager
{
  public:

  private:
    std::map<Resource*,resname_t> mResMap;
};

