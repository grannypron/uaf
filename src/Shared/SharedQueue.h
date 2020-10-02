/******************************************************************************
* Filename: SharedQueue.h
* Copyright (c) 2000, UAF Development Team (email CocoaSpud@hotmail.com)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/
#ifndef SharedQueue_HEADER
#define SharedQueue_HEADER

#include <afxtempl.h>

class SPELL_ID;

const long int UNLIMITED_QUEUE_SIZE = -1;

template<class T> void DestructQueueElement(T& element);
template<class T> void DestructSharedQueueElement(T& element);

//*****************************************************************************
//	CLASS Queue
//	This class maintains a producer-consumer queue of nodes of a paramaterized type. 
// The capacity of nodes is determined by Size. Access by multiple threads is 
// synchronized by a Win32 critical section.
//
//*****************************************************************************

template <class T, long Size> class Queue
{
public:
      Queue(void);
      // Initializes the queue

      ~Queue(void);
      // Will remove any existing nodes in the queue

      BOOL Push(T& node);
      // Adds a node to the head of the buffer.

      BOOL PushHighPriority(T& node);
      // Adds a node to the tail, in front of all other nodes.
      // Will add nodes beyond requested node capacity.

      BOOL Pop(T& node);
      // Removes a node from the tail of the buffer

      void PopAll(void);
      // Removes all nodes from the queue.

      BOOL IsEmpty(void) { return m_Msgs.IsEmpty(); }
      // return TRUE if there are nodes in the buffer, else FALSE

      int GetCount(void) { if (IsEmpty()) return 0; 
                           else return m_Msgs.GetCount(); }
      // return count of nodes in the buffer

      HANDLE GetNotEmptyEvent(void) { return m_hNotEmptyEvent; }
      // returns the event that will be signaled as long 
      // as there are nodes in the queue.

protected:
   // nothing

private:
   long m_MaxBufSize;  // the maximum nodes contained in the queue

   HANDLE m_hNotEmptyEvent;
   BOOL CreateEvent(void);

   mCList<T, T&> m_Msgs; // templatized linked list of nodes
};

//*****************************************************************************
//	Queue::Queue(void)
//
// Constructor. Create events and critical section.
//*****************************************************************************
template <class T, long Size> 
Queue< T, Size >::Queue(void) : m_MaxBufSize(Size), m_hNotEmptyEvent(NULL)
{
   CreateEvent();
}

//*****************************************************************************
//	Queue::~Queue(void)
//
// Destructor. Clean up allocated resources.
//*****************************************************************************
template <class T, long Size> 
Queue< T, Size >::~Queue(void)
{
   PopAll();
   CloseHandle(m_hNotEmptyEvent);
}

//*****************************************************************************
//	Queue::PopAll(void)
//
// Remove all elements from the queue.
//*****************************************************************************
template <class T, long Size> 
void Queue< T, Size >::PopAll(void)
{
   ResetEvent(m_hNotEmptyEvent);

   POSITION pos = m_Msgs.GetHeadPosition();

   while (pos != NULL)
      DestructQueueElement(m_Msgs.GetNext(pos));

   m_Msgs.RemoveAll();
}

//*****************************************************************************
//	Queue::Push(T& node)
//
// Add an element to the queue and signal the NotEmpty event.
//*****************************************************************************
template <class T, long Size> 
BOOL Queue< T, Size >::Push(T& node)
{
   BOOL success = TRUE;
   
   if ((m_MaxBufSize == UNLIMITED_QUEUE_SIZE) || (GetCount() < m_MaxBufSize))
   {
      m_Msgs.AddHead(node);

      if (m_hNotEmptyEvent == NULL)
         CreateEvent();

      if (m_hNotEmptyEvent != NULL)
      {
         if (!SetEvent(m_hNotEmptyEvent))
         {
            success = FALSE;
         }
      }
      else
         success = FALSE;
   }
   else 
   {
      success = FALSE;
   }

   return success;
}

//*****************************************************************************
//	Queue::PushHighPriority(T& node)
//
// Add an element to the front of the queue, and set the NotEmpty event.
//*****************************************************************************
template <class T, long Size> 
BOOL Queue< T, Size >::PushHighPriority(T& node)
{
   BOOL success = TRUE;
     
   m_Msgs.AddTail(node);

   if (m_hNotEmptyEvent == NULL)
      CreateEvent();

   if (m_hNotEmptyEvent != NULL)
   {
      if (!SetEvent(m_hNotEmptyEvent))
      {
         success = FALSE;
      }
   }
   else
      success = FALSE;

   return success;
}

//*****************************************************************************
//	Queue::Pop(T& node)
//
// Remove and return the next element. If no elements remain, clear the 
// NotEmpty event.
//*****************************************************************************
template <class T, long Size> 
BOOL Queue< T, Size >::Pop(T& node)
{
   BOOL success = TRUE;

   if (GetCount() > 0)
   {
      node = m_Msgs.RemoveTail();

      if (GetCount() <= 0)
      {
         if (m_hNotEmptyEvent == NULL)
            CreateEvent();

         if (m_hNotEmptyEvent != NULL)
         {
            if (!ResetEvent(m_hNotEmptyEvent))
            {
               success = FALSE;
            }
         }
         else
            success = FALSE;
      }      
   }
   else
      success = FALSE;

   return success;
}

//*****************************************************************************
//	Queue::CreateEvent(void)
//
// Create any needed events.
//*****************************************************************************
template <class T, long Size> 
BOOL Queue< T, Size >::CreateEvent(void)
{
   BOOL success = TRUE;

   if (m_hNotEmptyEvent == NULL)
      m_hNotEmptyEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

   if (m_hNotEmptyEvent == NULL)
   {
      success = FALSE;
   }

   return success;
}

//*****************************************************************************
//	void DestructQueueElement(T& element)
//
// Global template helper function called to destruct an element. This is the
// default destructor, clients can over-ride this for processing specific to
// a particular element type.
//*****************************************************************************
template<class T> void DestructQueueElement(T& element)
{
   memset(&element, 0, sizeof(T));
}


//
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
//


//*****************************************************************************
//	CLASS SharedQueue
//	This class maintains a producer-consumer queue of nodes of a paramaterized type. 
// The capacity of nodes is determined by Size. Access by multiple threads is 
// synchronized by a Win32 critical section.
//
//*****************************************************************************

template <class T, long Size> class SharedQueue
{
public:
      SharedQueue(void);
      // Initializes the queue

      ~SharedQueue(void);
      // Will remove any existing nodes in the queue

      BOOL Push(T& node);
      // Adds a node to the head of the buffer.

      BOOL PushHighPriority(T& node);
      // Adds a node to the tail, in front of all other nodes.
      // Will add nodes beyond requested node capacity.

      BOOL Pop(T& node);
      // Removes a node from the tail of the buffer

      void PopAll(void);
      // Removes all nodes from the queue.

      // the InterlockedExchange routine guarantees an atomic read of
      // the data

      BOOL IsEmpty(void) { return (InterlockedExchange(&m_Count, m_Count) <= 0); }
      // return TRUE if there are nodes in the buffer, else FALSE

      long GetCount(void) { return InterlockedExchange(&m_Count, m_Count); }
      // return count of nodes in the buffer

      HANDLE GetNotEmptyEvent(void) { return m_hNotEmptyEvent; }
      // returns the event that will be signaled as long 
      // as there are nodes in the queue.

protected:
   // nothing

private:
   long m_Count;       // contains the node count
   long m_MaxBufSize;  // the maximum nodes contained in the queue
   CRITICAL_SECTION m_cs; // synchronizes access to buffer data

   HANDLE m_hNotEmptyEvent;
   BOOL CreateEvent(void);

   mCList<T, T&> m_Msgs; // templatized linked list of nodes
};

//*****************************************************************************
//	SharedQueue::SharedQueue(void)
//
// Constructor. Create events and critical section.
//*****************************************************************************
template <class T, long Size> 
SharedQueue< T, Size >::SharedQueue(void) : m_Count(0), m_MaxBufSize(Size), m_hNotEmptyEvent(NULL)
{
   InitializeCriticalSection(&m_cs);
   CreateEvent();
}

//*****************************************************************************
//	SharedQueue::~SharedQueue(void)
//
// Destructor. Clean up allocated resources.
//*****************************************************************************
template <class T, long Size> 
SharedQueue< T, Size >::~SharedQueue(void)
{
   PopAll();
   CloseHandle(m_hNotEmptyEvent);
   DeleteCriticalSection(&m_cs);
}

//*****************************************************************************
//	SharedQueue::PopAll(void)
//
// Remove all elements from the queue.
//*****************************************************************************
template <class T, long Size> 
void SharedQueue< T, Size >::PopAll(void)
{
   EnterCriticalSection(&m_cs);
   m_Count = 0;
   ResetEvent(m_hNotEmptyEvent);

   POSITION pos = m_Msgs.GetHeadPosition();

   while (pos != NULL)
      DestructSharedQueueElement(m_Msgs.GetNext(pos));

   m_Msgs.RemoveAll();
   LeaveCriticalSection(&m_cs);
}

//*****************************************************************************
//	SharedQueue::Push(T& node)
//
// Add an element to the queue and signal the NotEmpty event.
//*****************************************************************************
template <class T, long Size> 
BOOL SharedQueue< T, Size >::Push(T& node)
{
   BOOL success = TRUE;
   
   EnterCriticalSection(&m_cs);

   if ((m_MaxBufSize == UNLIMITED_QUEUE_SIZE) || (GetCount() < m_MaxBufSize))
   {
      m_Count++;
      m_Msgs.AddHead(node);

      if (m_hNotEmptyEvent == NULL)
         CreateEvent();

      if (m_hNotEmptyEvent != NULL)
      {
         if (!SetEvent(m_hNotEmptyEvent))
         {
            success = FALSE;
         }
      }
      else
         success = FALSE;
   }
   else 
   {
      success = FALSE;
   }

   LeaveCriticalSection(&m_cs);

   return success;
}

//*****************************************************************************
//	SharedQueue::PushHighPriority(T& node)
//
// Add an element to the front of the queue, and set the NotEmpty event.
//*****************************************************************************
template <class T, long Size> 
BOOL SharedQueue< T, Size >::PushHighPriority(T& node)
{
   BOOL success = TRUE;

   EnterCriticalSection(&m_cs);
     
   m_Count++;
   m_Msgs.AddTail(node);

   if (m_hNotEmptyEvent == NULL)
      CreateEvent();

   if (m_hNotEmptyEvent != NULL)
   {
      if (!SetEvent(m_hNotEmptyEvent))
      {
         success = FALSE;
      }
   }
   else
      success = FALSE;

   LeaveCriticalSection(&m_cs);

   return success;
}

//*****************************************************************************
//	SharedQueue::Pop(T& node)
//
// Remove and return the next element. If no elements remain, clear the 
// NotEmpty event.
//*****************************************************************************
template <class T, long Size> 
BOOL SharedQueue< T, Size >::Pop(T& node)
{
   BOOL success = TRUE;

   EnterCriticalSection(&m_cs);

   if (m_Msgs.GetCount() > 0)
   {
      m_Count--;
      node = m_Msgs.RemoveTail();

      if (m_Count <= 0)
      {
         if (m_hNotEmptyEvent == NULL)
            CreateEvent();

         if (m_hNotEmptyEvent != NULL)
         {
            if (!ResetEvent(m_hNotEmptyEvent))
            {
               success = FALSE;
            }
         }
         else
            success = FALSE;
      }      
   }
   else
      success = FALSE;

   LeaveCriticalSection(&m_cs);

   return success;
}

//*****************************************************************************
//	SharedQueue::CreateEvent(void)
//
// Create any needed events.
//*****************************************************************************
template <class T, long Size> 
BOOL SharedQueue< T, Size >::CreateEvent(void)
{
   BOOL success = TRUE;

   if (m_hNotEmptyEvent == NULL)
      m_hNotEmptyEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

   if (m_hNotEmptyEvent == NULL)
   {
      success = FALSE;
   }

   return success;
}

//*****************************************************************************
//	void DestructSharedQueueElement(T& element)
//
// Global template helper function called to destruct an element. This is the
// default destructor, clients can over-ride this for processing specific to
// a particular element type.
//*****************************************************************************
template<class T> void DestructSharedQueueElement(T& element)
{
   memset(&element, 0, sizeof(T));
}

//
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
//

template <class T> struct OrderedQueueNode
{
  OrderedQueueNode() { key=0; }
  
  OrderedQueueNode( const OrderedQueueNode &rhs )
  {
    key = rhs.key;
    data = rhs.data;
  }

  ~OrderedQueueNode() { key=0; }


  DWORD key;
  T data;
};

template<class T> void DestructOrderedQueueElement(OrderedQueueNode<T>& element);

//*****************************************************************************
// CLASS OrderedQueue
// This class maintains a producer-consumer queue of nodes of a parameterized type. 
// The capacity of nodes is determined by Size. 
//
//*****************************************************************************

template <class T, long Size> class OrderedQueue
{
    // Key searching functions
            T& GetNode(DWORD key);
      const T& PeekNode(DWORD key);
public:
      OrderedQueue(void);
      // Initializes the queue

      ~OrderedQueue(void);
      // Will remove any existing nodes in the queue

      BOOL Insert(const T& node, DWORD key);
      BOOL Insert(const T& node);

      BOOL Remove(T& node, DWORD key);
      BOOL Remove(DWORD key);
      void RemoveHead();
      void RemoveTail();
      void RemoveAll(void);

      //BOOL GetHead(T& node, DWORD &key) const; 
      //BOOL GetNext(T& node, DWORD &key) const; // relative to curr pos
      //BOOL GetPrev(T& node, DWORD &key) const; // relative to curr pos


      //const T  GetAtConst(POSITION pos) const { return m_Msgs.GetAt(pos).data; }
      //const T  GetNextConst(POSITION &pos) const { return m_Msgs.GetNext(pos).data; }

      POSITION FindKeyPos(DWORD key) const;
      POSITION GetHeadPosition() const { return m_Msgs.GetHeadPosition(); }

      // Functions that return a reference to the data.
            T& GetAtPos  (POSITION pos)       { return m_Msgs.GetAt(pos)->data; }
      const T& PeekAtPos (POSITION pos) const 
      { 
        return const_cast<OrderedQueue< T, Size > *>(this)->m_Msgs.GetAt(pos)->data;
      };
            T& GetAtKey  (DWORD key)          { return m_Msgs.GetNode(key).data; }
      const T& PeekAtKey (DWORD key)    const { return m_Msgs.PeekNode(key).data; }
            T& GetNext(POSITION &pos)         { return m_Msgs.GetNext(pos)->data; }
      const T  *PeekNext(POSITION &pos)  const
      {
        const T *result;
        if (pos == NULL) return NULL;
        result = &PeekAtPos(pos);
        m_Msgs.PeekNext(pos);
        return result;
        //return const_cast<OrderedQueue< T, Size > *>(this)->m_Msgs.GetNext(pos).data;
      };
      


//      T& PeekAt(POSITION pos) { return const_cast<m_Msgs&>(m_Msgs&).GetAt(pos).data; }
      void RemoveAt(POSITION pos) { m_Msgs.RemoveAt(pos); }
      //T& GetPrev(POSITION &pos) { return m_Msgs.GetPrev(pos).data; }
            T& GetHead()  { return m_Msgs.GetHead().data; }
      const T& PeekHead() const 
      { 
        return const_cast<OrderedQueue< T, Size >*>(this)->m_Msgs.GetHead().data; 
      };

      T& GetTail() { return m_Msgs.GetTail()->data; }

      DWORD GetKeyAt(POSITION pos) const 
      { // All this const_cast is to prevent the compiler
        // from using the const version of GetAt().  It does a
        // copy of the entire node.
        return const_cast<OrderedQueue< T, Size >*>(this)->m_Msgs.GetAt(pos)->key; 
      };

      BOOL Find(T& node, DWORD key) const;
      BOOL Update(T& node, DWORD key);
      BOOL HaveKey(DWORD key) const;
      BOOL HaveID(const SPELL_ID& spellID) const;

      BOOL IsEmpty(void) const { return m_Msgs.IsEmpty(); }
      // return TRUE if there are nodes in the buffer, else FALSE

      long GetCount(void) const { if (IsEmpty()) return 0;
                                  else return m_Msgs.GetCount(); }
      // return count of nodes in the buffer

      HANDLE GetNotEmptyEvent(void) const { return m_hNotEmptyEvent; }
      // returns the event that will be signaled as long 
      // as there are nodes in the queue.

     OrderedQueue< T, Size >& operator =(const OrderedQueue< T, Size >& src);

private:
   long m_MaxBufSize;  // the maximum nodes contained in the queue

   //HANDLE m_hNotEmptyEvent;
   //BOOL CreateEvent(void);

   POSITION FindInsertAfterPos(DWORD key);   

   // templatized container of nodes
   mCList<OrderedQueueNode<T>, OrderedQueueNode<T>&> m_Msgs; 
   //CMap<DWORD, DWORD, POSITION, POSITION&> m_Msg; 
};

//*****************************************************************************
//	OrderedQueue::OrderedQueue(void)
//
// Constructor. Create events and critical section.
//*****************************************************************************
template <class T, long Size> 
OrderedQueue< T, Size >::OrderedQueue(void) : m_MaxBufSize(Size)/*, m_hNotEmptyEvent(NULL)*/
{
   //CreateEvent();
}

//*****************************************************************************
//	OrderedQueue::~OrderedQueue(void)
//
// Destructor. Clean up allocated resources.
//*****************************************************************************
template <class T, long Size> 
OrderedQueue< T, Size >::~OrderedQueue(void)
{
   RemoveAll();
   //CloseHandle(m_hNotEmptyEvent);
}

//*****************************************************************************
//	OrderedQueue::operator =(const OrderedQueue& src)
//
// copy all queue elements from source to destination
//*****************************************************************************
template <class T, long Size> 
OrderedQueue< T, Size > &OrderedQueue< T, Size >::operator =(const OrderedQueue< T, Size >& src)
{
  RemoveAll();

  POSITION p = src.m_Msgs.GetHeadPosition();
  while (p != NULL)
  {
    OrderedQueueNode<T> temp = *src.m_Msgs.PeekNext(p);
    Insert(temp.data, temp.key);
  }

  return *this;
}

//*****************************************************************************
//	OrderedQueue::RemoveAll(void)
//
// Remove all elements from the queue.
//*****************************************************************************
template <class T, long Size> 
void OrderedQueue< T, Size >::RemoveAll(void)
{
   //ResetEvent(m_hNotEmptyEvent);

   POSITION pos = m_Msgs.GetHeadPosition();
   while (pos != NULL)
      DestructOrderedQueueElement(*m_Msgs.GetNext(pos));

   m_Msgs.RemoveAll();
}

/*
//*****************************************************************************
//	OrderedQueue::GetHead(T& node)
//
// 
//*****************************************************************************
template <class T, long Size> 
BOOL OrderedQueue< T, Size >::GetHead(T& node, DWORD &key) const
{
  BOOL found = FALSE;  
  pos = m_Msgs.GetHeadPosition();

  if (pos != NULL)
  {
    // get node at pos and inc pos
    // to next node
    OrderedQueueNode<T> temp = m_Msgs.GetAt(pos);
    node = temp.data;
    key  = temp.key;
    found = TRUE;
  }

  return found;
}

//*****************************************************************************
//	OrderedQueue::GetNext(T& node)
//
// 
//*****************************************************************************
template <class T, long Size>
BOOL OrderedQueue< T, Size >::GetNext(T& node, DWORD &key) const
{
  if (pos == NULL)
      return FALSE;

  // get node at pos and inc pos
  // to next node
  m_Msgs.GetNext(pos);

  if (pos != NULL)
  {
    OrderedQueueNode<T> temp = m_Msgs.GetAt(pos);
    node = temp.data;
    key  = temp.key;
    return TRUE;
  }
  
  return FALSE;
}

//*****************************************************************************
//	OrderedQueue::GetPrev(T& node)
//
// 
//*****************************************************************************
template <class T, long Size>
BOOL OrderedQueue< T, Size >::GetPrev(T& node, DWORD &key) const
{
  if (pos == NULL)
      return FALSE;

  // get node at pos and inc pos
  // to next node
  m_Msgs.GetPrev(pos);

  if (pos != NULL)
  {
    OrderedQueueNode<T> temp = m_Msgs.GetAt(pos);
    node = temp.data;
    key  = temp.key;
    return TRUE;
  }
  
  return FALSE;
}
*/


//*****************************************************************************
//	OrderedQueue::FindInsertAfterPos(DWORD key)
//
// 
//*****************************************************************************
template <class T, long Size> 
POSITION OrderedQueue< T, Size >::FindInsertAfterPos(DWORD key)
{
  POSITION InsertPos = m_Msgs.GetHeadPosition();
  POSITION pos = InsertPos;
  POSITION prevPos = pos;

  // empty list, insert at head
  if (InsertPos == NULL)
    return NULL;

  // new key less than first key, insert at head
  if (key < m_Msgs.GetAt(InsertPos)->key)
    return NULL;

  bool found = false;

  while ((!found) && (pos != NULL))
  {
    if (key < m_Msgs.GetAt(pos)->key)
    {
      found = true;
      InsertPos = prevPos;
    }
    else
    {
      prevPos = pos;
      m_Msgs.GetNext(pos);
    }
  }

  // if not found, new key is greater than all
  // keys in list, so insert after last element
  if (!found)
    InsertPos = prevPos;

  return InsertPos;
}


//*****************************************************************************
//	OrderedQueue::Find(T& node, DWORD key)
//
// 
//*****************************************************************************
template <class T, long Size> 
BOOL OrderedQueue< T, Size >::Find(T& node, DWORD key) const
{
  POSITION pos = m_Msgs.GetHeadPosition();

  if (pos == NULL)
    return FALSE;

  BOOL found = FALSE;
  bool cont = true;
  //OrderedQueueNode<T> temp;
  DWORD currKey;

  while ((cont) && (!found) && (pos != NULL))
  {
    //temp = m_Msgs.GetNext(pos);
    currKey = m_Msgs.PeekAt(pos)->key;

    if (key == currKey)
    {
      node = m_Msgs.PeekAt(pos)->data;
      found = true;
    }
    else if (currKey > key)
    {
      cont = false;
    }

    if (!found)
      m_Msgs.PeekNext(pos);
  }  

  return found;
}

//*****************************************************************************
// NAME: GetAtVar - Assumes key is present in list
//    returns non-constant reference to data.
//
// PURPOSE:  
//*****************************************************************************
template <class T, long Size> 
T& OrderedQueue< T, Size >::GetNode(DWORD key)
{
  

  /*
  POSITION pos = m_Msgs.GetHeadPosition();
  ASSERT(pos != NULL);

  BOOL found = FALSE;
  bool cont = true;
  //OrderedQueueNode<T> temp;
  DWORD currKey;

  while ((cont) && (pos != NULL))
  {
    //temp = m_Msgs.GetAt(pos);
    currKey = m_Msgs.GetAt(pos).key;

    if (key == currKey)
    {
      found = true;
      cont = false;
    }
    else if (currKey > key)
      cont = false;
    else
      m_Msgs.GetNext(pos);
  };

  ASSERT(found);
  return m_Msgs.GetAt(pos).data;
  */
}

//*****************************************************************************
//	OrderedQueue::HaveKey(DWORD key)
//
// 
//*****************************************************************************
template <class T, long Size> 
BOOL OrderedQueue< T, Size >::HaveKey(DWORD key) const
{  
  POSITION pos = m_Msgs.GetHeadPosition();

  if (pos == NULL)
    return FALSE;

  BOOL found = FALSE;
  bool cont = true;
  DWORD currKey;
  //OrderedQueueNode<T> temp;

  while ((cont) && (!found) && (pos != NULL))
  {
    currKey = const_cast<OrderedQueue< T, Size >*>(this)->m_Msgs.GetNext(pos).key;
    if (key == currKey)
      found = true;
    else if (currKey > key)
      cont = false;
  }

  return found;
}

//*****************************************************************************
//	OrderedQueue::FindKeyPos(DWORD key)
//
// 
//*****************************************************************************
template <class T, long Size> 
POSITION OrderedQueue< T, Size >::FindKeyPos(DWORD key) const 
{
  
  POSITION pos = m_Msgs.GetHeadPosition();
  BOOL stop = FALSE;
  BOOL found = FALSE;
  DWORD currKey;

  while ((pos != NULL) && (!stop))
  {
    currKey = this->GetKeyAt(pos);
    //currKey = const_cast<OrderedQueue< T, Size > *>(this)->m_Msgs.GetAt(pos).key;

    if (key == currKey)
    {
      found = TRUE;
      stop = TRUE;
    }
    else if (currKey > key)
    {
      stop = TRUE;
    }
    else
      const_cast<OrderedQueue< T, Size >*>(this)->m_Msgs.GetNext(pos);
  }
  if (!found) 
    pos = NULL;
  return pos;
}

//*****************************************************************************
//	OrderedQueue::Insert(const T& node, DWORD key)
//
// Add an element to the queue and signal the NotEmpty event.
//*****************************************************************************
template <class T, long Size> 
BOOL OrderedQueue< T, Size >::Insert(const T& node, DWORD key)
{
   BOOL success = TRUE;
   POSITION pos = NULL;
   OrderedQueueNode<T> newNode;
   
   if ((m_MaxBufSize == UNLIMITED_QUEUE_SIZE) || (GetCount() < m_MaxBufSize))
   {
      newNode.key = key;
      newNode.data = node;

      pos = FindInsertAfterPos(key);

      if (pos == NULL)
        m_Msgs.AddHead(newNode);
      else
        m_Msgs.InsertAfter(pos, newNode);

      /*
      if (m_hNotEmptyEvent == NULL)
         CreateEvent();

      if (m_hNotEmptyEvent != NULL)
      {
         if (!SetEvent(m_hNotEmptyEvent))
         {
            success = FALSE;
         }
      }
      else
         success = FALSE;
      */
   }
   else 
   {
      success = FALSE;
   }

   return success;
}

//*****************************************************************************
//	OrderedQueue::Remove(T& node)
//
// Remove and return the next element. If no elements remain, clear the 
// NotEmpty event.
//*****************************************************************************
template <class T, long Size> 
BOOL OrderedQueue< T, Size >::Remove(T& node, DWORD key)
{
   BOOL success = TRUE;

   if (GetCount() > 0)
   {
      POSITION pos = FindKeyPos(key);

      if (pos != NULL)
      {
        OrderedQueueNode<T> temp = m_Msgs.RemoveAt(pos);
        node = temp.data;

        /*
        if (GetCount() <= 0)
        {
           if (m_hNotEmptyEvent == NULL)
              CreateEvent();

           if (m_hNotEmptyEvent != NULL)
           {
              if (!ResetEvent(m_hNotEmptyEvent))
              {
                 success = FALSE;
              }
           }
           else
              success = FALSE;
        } 
        */
      }
      else
        success = FALSE;
   }
   else
      success = FALSE;

   return success;
}

//*****************************************************************************
//	OrderedQueue::Remove(T& node)
//
//
//*****************************************************************************
template <class T, long Size> 
BOOL OrderedQueue< T, Size >::Remove(DWORD key)
{
   BOOL success = TRUE;

   if (GetCount() > 0)
   {
      POSITION pos = FindKeyPos(key);

      if (pos != NULL)
      {
        m_Msgs.RemoveAt(pos);

        /*
        if (GetCount() <= 0)
        {
           if (m_hNotEmptyEvent == NULL)
              CreateEvent();

           if (m_hNotEmptyEvent != NULL)
           {
              if (!ResetEvent(m_hNotEmptyEvent))
              {
                 success = FALSE;
              }
           }
           else
              success = FALSE;
        } 
        */
      }
      else
        success = FALSE;
   }
   else
      success = FALSE;

   return success;
}

//*****************************************************************************
//	OrderedQueue::RemoveHead()
//
//
//*****************************************************************************
template <class T, long Size> 
void OrderedQueue< T, Size >::RemoveHead()
{
  if (IsEmpty()) return;
  m_Msgs.RemoveHead();
}

//*****************************************************************************
//	OrderedQueue::RemoveTail()
//
//
//*****************************************************************************
template <class T, long Size> 
void OrderedQueue< T, Size >::RemoveTail()
{
  if (IsEmpty()) return;
  m_Msgs.RemoveTail();
}


//*****************************************************************************
//	OrderedQueue::Update(T& node, DWORD key)
//
//
//*****************************************************************************
template <class T, long Size> 
BOOL OrderedQueue< T, Size >::Update(T& node, DWORD key)
{
   BOOL success = TRUE;

   if (GetCount() > 0)
   {
      POSITION pos = FindKeyPos(key);

      if (pos != NULL)
      {
        OrderedQueueNode<T> temp;
        temp.data = node;
        temp.key = key;

        m_Msgs.SetAt(pos, temp);
      }
      else
        success = FALSE;
   }
   else
      success = FALSE;

   return success;
}

//*****************************************************************************
//	OrderedQueue::CreateEvent(void)
//
// Create any needed events.
//*****************************************************************************
/*
template <class T, long Size> 
BOOL OrderedQueue< T, Size >::CreateEvent(void)
{
   BOOL success = TRUE;

   if (m_hNotEmptyEvent == NULL)
      m_hNotEmptyEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

   if (m_hNotEmptyEvent == NULL)
   {
      success = FALSE;
   }

   return success;
}
*/
//*****************************************************************************
//	void DestructQueueElement(T& element)
//
// Global template helper function called to destruct an element. This is the
// default destructor, clients can over-ride this for processing specific to
// a particular element type.
//*****************************************************************************
template<class T> void DestructOrderedQueueElement(OrderedQueueNode<T>& element)
{
  element.key = 0;
}

//
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
//

template <class T> struct UnorderedQueueNode
{
  UnorderedQueueNode() { key=0; }
  ~UnorderedQueueNode() { key=0; }

  DWORD key;
  T data;
};

template<class T> void DestructUnorderedQueueElement(UnorderedQueueNode<T>& element);

//*****************************************************************************
// CLASS UnorderedQueue
// This class maintains a producer-consumer queue of nodes of a parameterized type. 
// The capacity of nodes is determined by Size. 
//
//*****************************************************************************

template <class T, long Size> class UnorderedQueue
{
public:
      UnorderedQueue(void);
      // Initializes the queue

      ~UnorderedQueue(void);
      // Will remove any existing nodes in the queue

      BOOL Insert(const T& node, DWORD key);

      BOOL Remove(T& node, DWORD key);
      BOOL Remove(DWORD key);
      void RemoveHead();
      void RemoveTail();
      void RemoveAll(void);

      //BOOL GetHead(T& node, DWORD &key) const; 
      //BOOL GetNext(T& node, DWORD &key) const; // relative to curr pos
      //BOOL GetPrev(T& node, DWORD &key) const; // relative to curr pos

      T& GetAt(DWORD key);

      POSITION FindKeyPos(DWORD key);
      POSITION GetHeadPosition() const { return m_Msgs.GetHeadPosition(); }
      T& GetAt(POSITION pos) { return m_Msgs.GetAt(pos)->data; }
      void RemoveAt(POSITION pos) { m_Msgs.RemoveAt(pos); }
      T& GetNext(POSITION &pos) { return m_Msgs.GetNext(pos)->data; }
      //T& GetPrev(POSITION &pos) { return m_Msgs.GetPrev(pos).data; }
      T& GetHead() { return m_Msgs.GetHead()->data; }
      T& GetTail() { return m_Msgs.GetTail()->data; }

      DWORD GetKeyAt(POSITION pos) { return m_Msgs.GetAt(pos).key; }

      BOOL Find(T& node, DWORD key) const;
      BOOL Update(T& node, DWORD key);
      BOOL HaveKey(DWORD key) const;

      BOOL IsEmpty(void) const { return (m_Msgs.IsEmpty()); }
      // return TRUE if there are nodes in the buffer, else FALSE

      long GetCount(void) const { return (m_Msgs.GetCount()); }
      // return count of nodes in the buffer

     UnorderedQueue< T, Size >& operator =(const UnorderedQueue< T, Size >& src);

private:
   long m_MaxBufSize;  // the maximum nodes contained in the queue

   // templatized container of nodes
   mCList<UnorderedQueueNode<T>, UnorderedQueueNode<T>&> m_Msgs; 
   CMap<DWORD, DWORD, POSITION, POSITION&> m_MsgPositions; 
};

//*****************************************************************************
//	UnorderedQueue::UnorderedQueue(void)
//
// Constructor. Create events and critical section.
//*****************************************************************************
template <class T, long Size> 
UnorderedQueue< T, Size >::UnorderedQueue(void) : m_MaxBufSize(Size)
{
}

//*****************************************************************************
//	UnorderedQueue::~UnorderedQueue(void)
//
// Destructor. Clean up allocated resources.
//*****************************************************************************
template <class T, long Size> 
UnorderedQueue< T, Size >::~UnorderedQueue(void)
{
   RemoveAll();
}

//*****************************************************************************
//	UnorderedQueue::operator =(const UnorderedQueue& src)
//
// Remove all elements from the queue.
//*****************************************************************************
template <class T, long Size> 
UnorderedQueue< T, Size > &UnorderedQueue< T, Size >::operator =(const UnorderedQueue< T, Size >& src)
{
  RemoveAll();

  POSITION p = src.m_Msgs.GetHeadPosition();
  while (p != NULL)
  {
    UnorderedQueueNode<T> temp = src.m_Msgs.GetNext(p);
    Insert(temp.data, temp.key);
  }

  return *this;
}

//*****************************************************************************
//	UnorderedQueue::RemoveAll(void)
//
// Remove all elements from the queue.
//*****************************************************************************
template <class T, long Size> 
void UnorderedQueue< T, Size >::RemoveAll(void)
{
   POSITION pos = m_Msgs.GetHeadPosition();
   while (pos != NULL)
   {
      DestructUnorderedQueueElement(*m_Msgs.GetNext(pos));
   };

   m_MsgPositions.RemoveAll();
   m_Msgs.RemoveAll();
}

//*****************************************************************************
//	UnorderedQueue::Find(T& node, DWORD key)
//
// 
//*****************************************************************************
template <class T, long Size> 
BOOL UnorderedQueue< T, Size >::Find(T& node, DWORD key) const
{
  POSITION pos = NULL;
  if (m_MsgPositions.Lookup(key, pos))
  {
    ASSERT( pos != NULL );
    node = m_Msgs.GetAt(pos);
    return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
// NAME: GetAt - Assumes key is present in list
//
// PURPOSE:  
//*****************************************************************************
template <class T, long Size> 
T& UnorderedQueue< T, Size >::GetAt(DWORD key)
{  
  POSITION pos = NULL;
  if (m_MsgPositions.Lookup(key, pos))
  {
    ASSERT( pos != NULL );
    return (m_Msgs.GetAt(pos).data);
  }

  die(0xab536);
  UnorderedQueueNode<T> temp;
  return (temp.data);
}

//*****************************************************************************
//	UnorderedQueue::HaveKey(DWORD key)
//
// 
//*****************************************************************************
template <class T, long Size> 
BOOL UnorderedQueue< T, Size >::HaveKey(DWORD key) const
{  
  POSITION pos = NULL;
  if (m_MsgPositions.Lookup(key, pos))
  {
    ASSERT( pos != NULL );
    return (pos != NULL);  
  }
  return FALSE;
}

//*****************************************************************************
//	UnorderedQueue::FindKeyPos(DWORD key)
//
// 
//*****************************************************************************
template <class T, long Size> 
POSITION UnorderedQueue< T, Size >::FindKeyPos(DWORD key)
{
  POSITION pos = NULL;
  if (m_MsgPositions.Lookup(key, pos))
  {
    ASSERT( pos != NULL );
    return pos;  
  }
  return NULL;
}

//*****************************************************************************
//	UnorderedQueue::Insert(const T& node, DWORD key)
//
// Add an element to the queue and signal the NotEmpty event.
//*****************************************************************************
template <class T, long Size> 
BOOL UnorderedQueue< T, Size >::Insert(const T& node, DWORD key)
{
  if ((m_MaxBufSize != UNLIMITED_QUEUE_SIZE) && (GetCount() >= m_MaxBufSize))
    return FALSE; // no more room

  POSITION pos = NULL;
  if (m_MsgPositions.Lookup(key, pos))
    return FALSE;  // key already being used
  
  UnorderedQueueNode<T> newNode;
  newNode.key = key;
  newNode.data = node;
  newNode.data.id = key;
   
  m_Msgs.AddTail(newNode);
  m_MsgPositions[key] = m_Msgs.GetTailPosition();

  return TRUE;
}

//*****************************************************************************
//	UnorderedQueue::Remove(T& node)
//
// Remove and return the element. If no elements remain, clear the 
// NotEmpty event.
//*****************************************************************************
template <class T, long Size> 
BOOL UnorderedQueue< T, Size >::Remove(T& node, DWORD key)
{
  if (IsEmpty())
    return FALSE;

  POSITION pos = FindKeyPos(key);

  if (pos == NULL)
    return FALSE;

  UnorderedQueueNode<T> temp = m_Msgs.GetAt(pos);
  m_Msgs.RemoveAt(pos);
  m_MsgPositions.RemoveKey(temp.key);

  node = temp.data;

  return TRUE;
}

//*****************************************************************************
//	UnorderedQueue::Remove(T& node)
//
//
//*****************************************************************************
template <class T, long Size> 
BOOL UnorderedQueue< T, Size >::Remove(DWORD key)
{
  if (IsEmpty())
    return FALSE;
  
  POSITION pos = FindKeyPos(key);
  
  if (pos == NULL)
    return FALSE;
  
  UnorderedQueueNode<T> temp = *m_Msgs.GetAt(pos);
  m_Msgs.RemoveAt(pos);
  m_MsgPositions.RemoveKey(temp.key);
    
  return TRUE;
}

//*****************************************************************************
//	UnorderedQueue::RemoveHead()
//
//
//*****************************************************************************
template <class T, long Size> 
void UnorderedQueue< T, Size >::RemoveHead()
{
  if (IsEmpty()) return;  
  UnorderedQueueNode<T> temp = m_Msgs.RemoveHead();
  m_MsgPositions.RemoveKey(temp.key);  
}

//*****************************************************************************
//	UnorderedQueue::RemoveTail()
//
//
//*****************************************************************************
template <class T, long Size> 
void UnorderedQueue< T, Size >::RemoveTail()
{
  if (IsEmpty()) return;
  UnorderedQueueNode<T> temp = m_Msgs.RemoveTail();
  m_MsgPositions.RemoveKey(temp.key);  
}


//*****************************************************************************
//	UnorderedQueue::Update(T& node, DWORD key)
//
//
//*****************************************************************************
template <class T, long Size> 
BOOL UnorderedQueue< T, Size >::Update(T& node, DWORD key)
{
  if (IsEmpty()) 
    return FALSE;

  POSITION pos = FindKeyPos(key);
 
  if (pos == NULL)
    return FALSE;

  UnorderedQueueNode<T> temp;
  temp.data = node;
  temp.key = key;

  m_Msgs.SetAt(pos, temp);

  return TRUE;
}

//*****************************************************************************
//	void DestructQueueElement(T& element)
//
// Global template helper function called to destruct an element. This is the
// default destructor, clients can over-ride this for processing specific to
// a particular element type.
//*****************************************************************************
template<class T> void DestructUnorderedQueueElement(UnorderedQueueNode<T>& element)
{
  element.key = 0;
}

//
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
//




#endif // ifndef SharedQueue_HEADER