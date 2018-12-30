#include <iostream.h>

struct   node_t
{
  node_t  *next, *prev;
};

class Owner
{
  public:
                        Owner ();

           void         SetFirst (node_t&);
           void         AddFirst (node_t&);
           void         Del      (node_t&);

           node_t*      GetLast () const { return last.prev; }
           node_t*      GetFirst () const { return first.next; }

    node_t first, last;        
};

void Owner::AddFirst (node_t& n)
{
  n.prev       = &first;
  n.next       = first.next;
  first.next->prev = &n;
  first.prev   = &n;
}

void Owner::Del      (node_t& n)
{
  n.prev->next = n.next;
  n.next->prev = n.prev;
}

void Owner::SetFirst (node_t& n)
{
  Del      (n);  
  AddFirst (n);
}

Owner::Owner ()
{
  first.prev = last.next = NULL;
  first.next = &last;
  last.prev  = &first;
}
