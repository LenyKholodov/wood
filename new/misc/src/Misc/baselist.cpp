#include <pch.h>
#include <misc\list.h>

BaseList::listnode_t BaseList::mNull (NULL,NULL);

void            BaseList::InsertFirst  (listnode_t& node)
{
  Erase (node);

  node.prev         = &mFirst;
  node.next         = mFirst.next;
  mFirst.next->prev = &node;
  mFirst.next       = &node;

  mCount++;
}

void            BaseList::InsertLast  (listnode_t& node)
{
  Erase (node);

  node.next         = &mLast;
  node.prev         = mFirst.prev;
  mLast.prev->next  = &node;
  mLast.prev        = &node;  

  mCount++;
}

bool            BaseList::InsertAfter  (listnode_t& after,listnode_t& node)
{
  if (!&after || !&node || &after == &node)  
    return false;

  Erase (node);

  after.next->prev = &node;
  node.next        = after.next;
  node.prev        = &after;
  after.next       = &node;

  mCount++;

  return true;
}

bool            BaseList::InsertBefore (listnode_t& before,listnode_t& node)
{
  if (!&before || !&node || &before == &node)  
    return false;

  Erase (node);

  before.next->prev = &node;
  node.next         = before.next;
  node.prev         = &before;
  before.next       = &node;

  mCount++;

  return true;
}

void            BaseList::Erase        (listnode_t& node)
{
  if (&node && node.prev != &mNull)
  {
    node.prev->next = node.next;
    node.next->prev = node.prev;
    node.prev       = node.next = &node;
    mCount--;    
  }
}

void            BaseList::SetFirst   (listnode_t& node)
{
  InsertFirst (node);
}

void            BaseList::SetLast    (listnode_t& node)
{
  InsertLast (node);
}

void            BaseList::reset        ()
{
  mCount      = 0;
  mFirst.prev = mLast.next = &mNull;
  mLast.prev  = &mFirst;
  mFirst.next = &mLast;
}

BaseList::BaseList ()
{
  reset ();
}
