#ifndef __NUMBERS__
#define __NUMBERS__

#include <pool.h>

class    Float;
class    Double;
class    Integer;

#define EnumClassOper(CMD,OP) \
  CMD (Integer,OP) \
  CMD (Float,OP)  \
  CMD (Double,OP) 

#define DeclareClassOperAbstract(CLASS,OP)  \
    virtual Number& operator OP (const CLASS&) = 0;
#define DeclareOperatorAbstract(OP) \
    DeclareClassOperAbstract (Number,OP) \
    EnumClassOper (DeclareClassOperAbstract,OP)

#define DeclareClassOper(CLASS,OP)  \
    virtual Number& operator OP (const CLASS&);
#define DeclareOperator(OP) \
    DeclareClassOper (Number,OP) \
    EnumClassOper (DeclareClassOper,OP)

#define DeclareNumber(NAME,TYPE,PRIOR) \
class NAME: public Value<TYPE,PRIOR> \
{ \
  public: \
        NAME (TYPE i): Value<TYPE,PRIOR> (i) {} \
};

class   Number
{
  public:

        void* operator new     (size_t size) { return mNumbersPool->alloc (size); }
        void  operator delete  (void*) {}

        DeclareOperatorAbstract (+);
        DeclareOperatorAbstract (-);
        DeclareOperatorAbstract (*);
        DeclareOperatorAbstract (/);


        friend ostream& operator << (ostream& os,const Number& n) { n.print (os); return os; }

  protected:
        virtual void print (ostream&) const = 0;

//  private:
  public:
      static Pool*  mNumbersPool; //for temp evalutes
};

template <class T,const int P>
class Value: public Number
{
  public:  
                Value (T i): x (i) {}

        DeclareOperator (+);
        DeclareOperator (-);
        DeclareOperator (*);
        DeclareOperator (/);

            T  data () const { return x; }            

    virtual int     prior      () const { return P; }

  protected:
        virtual void print (ostream& os) const { os<<x; }

  protected:
        T       x;
};

DeclareNumber (Integer,int,1);
DeclareNumber (Float,float,2);
DeclareNumber (Double,double,3);

#endif