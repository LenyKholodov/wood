template <class Type> class Resource
{
  private:
    Type*  resource;
    string name;
    time_t lastAccess;
    size_t size;
};

class Manager
{
  public:
    template <class Type>
    Type*    LockResource (Resource<Type>&);
};

class Texture: public Resource<TextureBuffer>
{
  public:
};

class TextureManager: public Manager
{
  public:
};