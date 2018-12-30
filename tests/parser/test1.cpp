#include <iostream.h>

enum Types {
  NUMBER  = 0,     
  INTEGER,
  FLOAT
};

class NumExc
{
  public:
        NumExc (const char* msg): message (msg) {}

        friend ostream& operator << (ostream& os,const NumExc& e) { e.print (os); return os;}

  protected:
        virtual void print (ostream& os) const { os<<message<<endl; }

  private:
        const char* message;  
};

class    Integer;
class    Float;

class    Number
{
  public:
/////////////////////////////////////////////////////////////////////////////////////////////
///RTTI
/////////////////////////////////////////////////////////////////////////////////////////////
        virtual int type  () const { return NUMBER; }
        virtual int prior () const { return 0; }

        virtual Number&  convert (int type) const { throw NumExc ("Convert to abstract number"); }

        virtual Number&  operator + (const Number&) const;

        friend  ostream& operator << (ostream&,const Number&);

  protected:
        virtual void     print     (ostream&) const {}
};

class   Integer: public Number
{
  public:
                 Integer (int i): x(i) {}

    virtual int  type       () const { return INTEGER; }
    virtual int  prior      () const { return 1; }

    virtual Number&  convert (int type) const;

    virtual Number&  operator + (const Number& n) const {
      if (n.type () != type ())
        return Number::operator + (n);
      else
        return *new Integer (((Integer&)n).x + x);
    }

  protected:
    virtual      void    print      (ostream& os) const { os<<x; }

  private:
        int     x;
};

class   Float: public Number
{
  public:
                 Float (float i): x(i) {}

    virtual int  type       () const { return FLOAT; }
    virtual int  prior      () const { return 2; }

    virtual Number&  convert (int type) const;

    virtual Number&  operator + (const Number& n) const {
      if (n.type () != type ())
        return Number::operator + (n);
      else
        return *new Float (((Float&)n).x + x);
    }

  protected:
    virtual      void    print      (ostream& os) const { os<<x; }

  private:
    float x;
};

Number&  Number::operator + (const Number& n) const
{
  if (type () != n.type ())
  {
    const Number *a = this,
                 *b = &n; 

    if (a->prior () > b->prior ())   b = &b->convert (a->type ());
    else                             a = &a->convert (b->type ());

    return *a + *b;
  }
  else throw NumExc ("operator + at abstract Number");
}

Number& Integer::convert (int type) const
{
  switch (type)
  {
    case INTEGER: return (Integer&)*this;
    case FLOAT:   return *new Float (x);
    default: return Number::convert (type);
  }
}

Number& Float::convert (int type) const 
{
  switch (type)
  {
    case INTEGER: return *new Integer (x); 
    case FLOAT:   return (Float&)*this;
    default: return Number::convert (type);
  }
}

ostream& operator <<  (ostream& os,const Number& n)
{
  n.print (os);
  return os;
}

void    main ()
{
  try
  {
    Integer i1 = 5, i2 = 10;
    Float   f1 = 2.3;
    Number& a  = i1, &b = f1;
    Number& c = a + b;

    cout<<c<<endl;
  }
  catch (NumExc& e)
  {
    cout<<e<<endl;
  }  
}