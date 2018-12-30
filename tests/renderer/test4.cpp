#pragma inline_depth (255)
#pragma inline_recursion (on)
#pragma auto_inline (on)

struct A
{
  int x,y;
};

template <bool test (const A&,const A&)>
struct cmp {  
  cmp (const A* table,int count,bool* result)
  {
    for (int i=0;i<count;i++,table++)
        *result = test (*table,*table);
  }
};

inline bool test1 (const A& a1,const A& a2) { return 0; }
inline bool test2 (const A& a1,const A& a2) { return 1; }

void main ()
{
  const int N = 45;
  A a [N];
  bool result [N];

  cmp<test2>::cmp (a,N,result);
}