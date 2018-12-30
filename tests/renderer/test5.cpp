#pragma inline_depth (255)
#pragma inline_recursion (on)
#pragma auto_inline (on)

struct A
{
  int x,y;

  bool operator == (const A& a) const { return 1; }
};

template <class T,const T& get (const A& a)>
struct cmp {  
  cmp (const A* table,int count,bool* result)
  {
    for (int i=0;i<count;i++,table++)
      *result = get (*table) == get (*table);
  }
};

inline const A& get (const A& a) { return a; }

void main ()
{
  const int N = 45;
  A a [N];
  bool result [N];

  cmp<A,get>::cmp (a,N,result);
}