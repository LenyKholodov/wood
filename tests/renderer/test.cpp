typedef unsigned int uint;

////////Base

class BaseRender
{
  public:
    class iterator;
    class Span;

    virtual void process (Span&) = 0;

  protected:
            void next    (const iterator&,const iterator&);

  private:
    BaseRender *mNext, *mPrev;
};

class BaseRender::iterator
{
  public:
            iterator       (void** _ptr,size_t _offset);
     friend int operator - (const iterator&,const iterator&);

  protected:
    const void*  data () const;
    void*        data ();

  protected:
    void** ptr;
    size_t offset;
};

class BaseRender::Span
{
  public:
    iterator begin () const;
    iterator end   () const;
    uint     size  () const;

             Span  (iterator _begin,iterator _end);
             
  private:
    iterator mBegin, mEnd;
};

/////////Extented

template <class Attrib> class Render: public BaseRender
{
  public:
    class iterator;
};

template <class Attrib> class Render<Attrib>::iterator: public BaseRender::iterator
{
  public:
                     operator Attrib*      ();
                     operator const Attrib () const;

    Attrib&          operator -> ();
    const Attrib&    operator -> () const;

    iterator         operator [] (int i);
    const iterator   operator [] (int i) const;

    iterator&        operator  ++ ();
    iterator         operator  ++ (int);
    iterator&        operator  -- ();
    iterator         operator  -- (int);

    iterator&        operator  += (int);
    iterator&        operator  -= (int);

    friend iterator       operator   + (const iterator&,int);
    friend iterator       operator   - (const iterator&,int);        

           iterator (const BaseRender::iterator&);
};


