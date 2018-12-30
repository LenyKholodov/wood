class Resource
{
  public:
  private:
    int count;
};

template <class _Resource>
class ResType
{
  public:
    typedef _Resource Resource;

  private:
    ResType (const ResType&);
    ResType& operator = (const ResType&);
};

class Manager
{
  public:
        template <class ResType>
        ResType::Resource* create (const string&,const ResType&);
};