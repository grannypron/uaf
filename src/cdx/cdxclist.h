//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxclist.h,v $
// $Author: cocoaspud $
//
// $Log: cdxclist.h,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.5  2001/02/22 12:37:11  mindcry
// Sorry there where a few int's left.. but now it should be fixed *g*
//
// Revision 1.4  2001/02/22 10:53:45  mindcry
// Changed all LONG to DWORD
//
// Revision 1.3  2000/08/20 13:49:23  mindcry
// Removed a small memory leak. Well every CDX App/Game had this leak :(
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.1.1.1  2000/04/22 16:09:02  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 1.2  2000/02/22 02:26:00  Karasu
// Added Prev() and GetLast()
//
// Revision 1.1  2000/01/13 23:32:47  jhebert
// This is a new header file to the library.  It is a linklist template for
// classes.  It is a copy of what was in the addon section and I modified
// a few things.  I do not know if templates play nice with compilers other
// then MS VC++, let me know. -John Hebert
//
//
//
// Notes on usage:
// ---------------------
// In order for this template to work your you must have the following four 
// functions defined in your class:
//
//    <TYPE>* GetNext(void);
//    <TYPE>* GetPrev(void);
//    void SetNext(<TYPE>* T);
//    void SetPrev(<TYPE>* T);
//
// An example of this would look like this:
//
//       class SalesOrder
//       {
//         ....
//
//         public:
//         SalesOrder* GetNext(void)   { return m_Next; }
//         SalesOrder* GetPrev(void)   { return m_Prev; }
//         void SetNext(SalesOrder* T) { m_Next = T; }
//         void SetPrev(SalesOrder* T) { m_Prev = T; }
//
//         private:
//         SalesOrder* m_Next;
//         SalesOrder* m_Prev;
//
//         ....
//       };
//       
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifndef CDXCLIST_H
#define CDXCLIST_H

//////////////////////////////////////////////////////////////////////////////////
// The declaration for the CDXCList template
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
class CDXCList
{	
public:
    CDXCList(void);
    virtual ~CDXCList(void);

    void         Clear( BOOL deleteMe = TRUE );
    void         Add( TYPE* newElement , BOOL Last = TRUE );
    void         Insert( TYPE* newElement, DWORD pos );
    TYPE*        Get( DWORD Pos );
    inline TYPE* GetFirst( void );
	inline TYPE* GetLast( void );
    TYPE*        Next( TYPE* &Search );
	TYPE*        Prev( TYPE* &Search );
    TYPE*        Remove( TYPE* &search, BOOL deleteMe = TRUE );
    TYPE*        Remove( DWORD Pos, BOOL deleteMe = TRUE );
    void         CreateLookup( void );
    DWORD         GetCount( void );

private:
    TYPE* m_First;     // Pointer to the first element in the list
    TYPE* m_Last;      // Pointer to the last element in the list
    TYPE** m_PtrList;  // Pointer to a list of pointer used as a lookup table
    DWORD m_Count;      // Count of number of elements in the list
};

//////////////////////////////////////////////////////////////////////////////////
// NAME: Constructor
// 
// PURPOSE: Create an instance of the object and initize it's member data.            
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
CDXCList<TYPE>::CDXCList(void)
{	
    m_Last    = NULL;
    m_First   = NULL;
	m_Count   = 0;
	m_PtrList = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Destructor
// 
// PURPOSE: Destroy all dynamicly created data and the object itself            
//
// INPUT: none
//
// RETURNS: nothing
//
// NOTE: By default the the destructor does NOT delete the members of the linked
//       list.  The Clear() function can either delete the members or simply unlink
//       them depending on the value passed in.  If your desire is to have the list
//       members deleted call Clear(TRUE) before deleting your list object.
//
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
CDXCList<TYPE>::~CDXCList(void)
{	
    Clear(FALSE);
	delete [] m_PtrList;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Add
// 
// PURPOSE: Add a new Element to list           
//
// INPUT: newElement - pointer to the object to add
//        Last       - if TRUE put the new node at the end of the list else put it
//                     at the beginning.
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
void CDXCList<TYPE>::Add( TYPE* newElement , BOOL Last)
{	
    //Cannot add a NULL to the list.
	if( newElement == NULL ) 
        return; 

	if( m_First == NULL )
	{	
        // If no elements in list, make this the first
		newElement->SetNext(NULL);
		newElement->SetPrev(NULL);
		m_Last  = newElement;
		m_First = newElement;
	}
	else
	{
		if( Last )
		{
            // Insert at end of the list
			newElement->SetNext(NULL);	
			newElement->SetPrev(m_Last);
			m_Last->SetNext(newElement);
			m_Last = newElement;
		} 
        else
		{
            // Insert at the beginning of the list
			newElement->SetNext(m_First);
			newElement->SetPrev(NULL);
			m_First->SetPrev(newElement);
			m_First = newElement;
		}
	}

	m_Count++;

	// If a lookup table was created delete it
	if( m_PtrList != NULL)
    {
	    delete [] m_PtrList;
        m_PtrList = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Insert
// 
// PURPOSE: Inserts a new Element to list at the location specified by 'pos'.          
//
// INPUT: newElement - pointer to the object to add
//        pos        - The position in the list where position 0 (zero) is the 
//                     first position in the list.
//
// RETURNS: nothing
//
////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
void CDXCList<TYPE>::Insert( TYPE* newElement, DWORD pos )
{
    //Cannot add a NULL to the list.
    if( newElement == NULL ) 
        return; 

    if( pos < 0 ) 
        pos=0;

    if( pos >= m_Count )
    {
        // Add to end of the list
        Add( newElement );
    }
    else
    {
        TYPE *Search = Get( pos );

        if( Search->GetPrev() != NULL )
            Search->GetPrev()->SetNext(newElement);
        else
            m_First = newElement;

        newElement->SetNext(Search);
        newElement->SetPrev(Search->GetPrev());
        Search->SetPrev(newElement);
        m_Count++;

	    // If a lookup table was created delete it
		if( m_PtrList != NULL)
        {
	        delete [] m_PtrList;
            m_PtrList = NULL;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: Clear
// 
// PURPOSE: Removes all nodes from the list. If 'deleteMe' is set to TRUE it will
//          also delete each of the members.          
//
// INPUT: deleteMe - determines if the members in the list should be deleted
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
void CDXCList<TYPE>::Clear( BOOL deleteMe )
{	
    if( m_First != NULL )
	{	
        TYPE* Search = m_First;
	
		if( deleteMe==TRUE )
		{
			do
			{	
			    if( (Search->GetPrev() != NULL) )
				    delete Search->GetPrev();
			}while( (Search = Search->GetNext()) != NULL );

			delete m_Last;
		}

		m_First = m_Last = NULL;
		m_Count=0;

        // If a lookup table was created delete it
		if( m_PtrList != NULL)
        {
	        delete [] m_PtrList;
            m_PtrList = NULL;
        }
	}
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Next
// 
// PURPOSE: gets the Next element from Search          
//
// INPUT: Search - address of the pointer to a object
//
// RETURNS: a pointer to the next node
//
// NOTE: 'search' is modified. Retain a copy if you intend to use this pointer.
//       
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
TYPE* CDXCList<TYPE>::Next( TYPE* &Search )
{	
    TYPE* tmp = NULL;

	if( Search != NULL ) 
        tmp = Search->GetNext();

	Search = tmp;

	return tmp;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Prev
// 
// PURPOSE: gets the Previous element from Search          
//
// INPUT: Search - address of the pointer to a object
//
// RETURNS: a pointer to the previous node
//
// NOTE: 'search' is modified. Retain a copy if you intend to use this pointer.
//       
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
TYPE* CDXCList<TYPE>::Prev( TYPE* &Search )
{	
    TYPE* tmp = NULL;

	if( Search != NULL ) 
        tmp = Search->GetPrev();

	Search = tmp;

	return tmp;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetFirst
// 
// PURPOSE: Returns the first node in the list.          
//
// INPUT: none
//
// RETURNS: A pointer to the first node
//
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
inline TYPE* CDXCList<TYPE>::GetFirst(void)
{	
    return m_First;	
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetLast
// 
// PURPOSE: Returns the last node in the list.          
//
// INPUT: none
//
// RETURNS: A pointer to the last node
//
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
inline TYPE* CDXCList<TYPE>::GetLast(void)
{	
    return m_Last;	
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetCount
// 
// PURPOSE: Returns the number of nodes in the list.         
//
// INPUT: none
//
// RETURNS: number of nodes in the list
//
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
inline DWORD CDXCList<TYPE>::GetCount(void)
{	
    return m_Count;	
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Get
// 
// PURPOSE: Returns the node that 'pos' position in the list.         
//
// INPUT: Pos - the index of the node that you want.
//
// RETURNS: A pointer to the node indexed by 'Pos'.
//
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
TYPE* CDXCList<TYPE>::Get( DWORD Pos )
{	
    TYPE* tmp = m_First;

	if( tmp != NULL )
	{	
        if( m_PtrList != NULL )
		{
			return m_PtrList[Pos];
		}
		else
		{
			if (Pos == 0) 
                return m_First;

			if (Pos == m_Count - 1) 
                return m_Last;

            // Start at the first element and GetNext 'pos' times
			for( DWORD i=0; i<Pos && tmp; i++ )
                tmp = tmp->GetNext();

			return tmp;
		}

	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Remove
// 
// PURPOSE: Removes a link from the list and deletes the node if requested. It
//          returns a pointer to the previous node in the list.        
//
// INPUT: Search   - a pointer to the node you want removed
//        deleteMe - determines if the node should be deleted or not.
//
// RETURNS: A pointer to the previous node
//
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
TYPE* CDXCList<TYPE>::Remove( TYPE* &Search, BOOL deleteMe )
{	
    TYPE* tmp;

    // If value passed in is NULL return NULL
	if (Search == NULL) 
        return NULL; 

	if( Search->GetPrev() != NULL )
		Search->GetPrev()->SetNext(Search->GetNext());
	else 
		m_First = Search->GetNext();


	if( Search->GetNext() != NULL )
		Search->GetNext()->SetPrev(Search->GetPrev());
	else
		m_Last = Search->GetPrev();

	tmp = Search->GetPrev();

    if( deleteMe == TRUE )
	    delete Search;

	m_Count--;

	if( m_PtrList != NULL)
    {
	    delete [] m_PtrList;
        m_PtrList = NULL;
    }

	Search=tmp; 

	return tmp;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Remove
// 
// PURPOSE: Removes a link from the list at position 'Pos' and deletes the node 
//          if requested. It returns a pointer to the previous node in the list.        
//
// INPUT: Pos      - index to a node in the list
//        deleteMe - determines if the node should be deleted or not.
//
// RETURNS: A pointer to the previous node 
//
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
TYPE* CDXCList<TYPE>::Remove( DWORD Pos, BOOL deleteMe )
{	
    TYPE* T = Get(Pos);
	T = Remove(T, deleteMe);

	return T;
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: CreateLookup
// 
// PURPOSE: Creates a lookup table for faster Gets, but it will be released if 
//          you call Add/Remove       
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
void CDXCList<TYPE>::CreateLookup( void )
{	
    // Delete the lookup table
    if( m_PtrList != NULL)
    {
	    delete [] m_PtrList;
        m_PtrList = NULL;
    }

	m_PtrList = new TYPE*[m_Count];

	if( m_First != NULL )
	{	
        TYPE* Search = m_First;
		DWORD i=0;

		do
		{	
            m_PtrList[i] = Search;
			i++;

		}while( (Search = Search->GetNext()) != NULL );
	}
}
#endif CDXCLIST_H