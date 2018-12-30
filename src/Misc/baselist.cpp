#include <pch.h>
#include <misc\list.h>

BaseList::listnode_t BaseList::mNull (NULL,NULL);

bool            BaseCollection::InsertAfter  (listnode_t& after,listnode_t& node)
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

bool            BaseCollection::InsertBefore (listnode_t& before,listnode_t& node)
{
  if (!&before || !&node || &before == &node)  
    return false;

  Erase (node);

  before.prev->next = &node; 
  node.next         = &before;
  node.prev         = before.prev;
  before.prev       = &node;

  mCount++;

  return true;
}

void            BaseCollection::Erase        (listnode_t& node)
{
  if (&node && node.prev != &mNull)
  {
    node.prev->next = node.next;
    node.next->prev = node.prev;
    node.prev       = node.next = &node;
    mCount--;    
  }
}

BaseCollection::BaseCollection () : mCount (0) {}

void            BaseList::InsertFirst  (listnode_t& node)
{
  InsertAfter (mFirst,node); 
}

void            BaseList::InsertLast  (listnode_t& node)
{
  InsertBefore (mLast,node);
}

void            BaseList::reset        ()
{
  BaseCollection::reset ();

  mFirst.prev = mLast.next = &mNull;
  mLast.prev  = &mFirst;
  mFirst.next = &mLast;
}

BaseList::BaseList ()
{
  reset ();
}

void  BaseRing::InsertFirst  (listnode_t& node)
{
  if (mFirst)
  {
    InsertBefore (*mFirst,node);
    mFirst    = &node;
  }
  else
  {
    mFirst = node.prev = node.next = &node;
    mCount++;
  }  
}

void  BaseRing::InsertLast   (listnode_t& node)
{
  if (mFirst)
    InsertBefore (*mFirst,node);
  else
  {
    mFirst = node.prev = node.next = &node;
    mCount++;
  }  
}

void  BaseRing::Erase  (listnode_t& node)
{
  if (&node == mFirst)
  {
    if (count () == 1) mFirst = NULL;
    else               mFirst = node.next;
  }  

  BaseCollection::Erase (node);  
}

void  BaseRing::reset  ()
{
  BaseCollection::reset ();

  mFirst = NULL;
}

BaseRing::BaseRing ()
         : mFirst (NULL)
{
  reset ();
}
