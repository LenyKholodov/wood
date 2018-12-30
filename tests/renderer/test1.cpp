#include <iostream>
#include <time.h>

using std::cout;
using std::endl;

typedef unsigned int uint;

template <class type,const size_t stride = sizeof (type)>
class iterator
{
  public:
        operator type* ()           { return (type*)data; }
        type& operator [] (int i)   { return *(type*)((char*)data + stride * i); }
        type* operator -> ()        { return (type*)data; }

        iterator& operator ++ ()    { data = (char*)data + stride; return *this; }
        iterator  operator ++ (int) { return ++iterator (*this); }
        iterator& operator -- ()    { data = (char*)data - stride; return *this;}
        iterator  operator -- (int) { return --iterator (*this);  }

//        iterator (void* _data,size_t _stride):data (_data), stride (_stride) {}
//        iterator (type* _data): data (_data), stride (sizeof (type)) {}
        iterator (void* _data,size_t _stride):data (_data) {}
        iterator (type* _data): data (_data) {}
            
  private:
    void*  data;
//    size_t stride;
};

struct A
{
  char array [25600];
};

template <class iter>
void TestIter (iter i,uint count)
{
  for (int j=count;j;)
    i [j].array [3] = j--;
}

void InitArray (A* a,uint count)
{
  for (int i=0;i<count;i++)
    for (int j=0;j<100;j++)
      a [i].array [j] = i*j;
}

void OutArray (A* a,uint count)
{
  for (int i=0;i<count;i++)
    for (int j=0;j<100;j++)
      a [i].array [j] = a [count-i-1].array [100-j-1];
}

void main ()
{
  const int N = 256;
  const uint total = 10000;

  static A a [N];  

  InitArray (a,N);

  clock_t start1 = clock ();

  for (int i=0;i<total;i++)
    TestIter (&a [0],N);

  clock_t end1 = clock ();

  clock_t start2 = clock ();

  for (int i=0;i<total;i++)
    TestIter (iterator<A> (a),N);

  clock_t end2 = clock ();

  float fps1 = float (total*N)/float (end1-start1)*float (CLK_TCK);
  float fps2 = float (total*N)/float (end2-start2)*float (CLK_TCK);

  cout<<"FPS1:\t"<<fps1<<endl;
  cout<<"FPS2:\t"<<fps2<<endl;
  cout<<"d:\t"<<fps2/fps1<<endl;

  OutArray (a,N);
}
