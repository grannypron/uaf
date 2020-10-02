//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxspritelist.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdxspritelist.cpp,v $
// Revision 1.5  2013/12/16 01:35:59  cocoaspud
// fixed a few issues pointed out by static analysis tool
//
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.2  2000/05/10 23:29:26  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.1.1.1  2000/04/22 16:12:04  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.5  2000/01/19 23:16:47  jhebert
// Added HRESULT return value to the Draw function.
//
// Revision 2.4  2000/01/13 23:45:05  jhebert
// Fisrt checkin for v3.0.
// I what to remove the AddSprite function that takes all the arguments.  I'm using
// the linked list template to manage the list.
//
// 
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxspritelist.cpp,v 1.5 2013/12/16 01:35:59 cocoaspud Exp $";
#endif

#include "CDX.h"
#include "cdxsprite.h"

//////////////////////////////////////////////////////////////////////////////////
// NAME: Default Constructor
// 
// PURPOSE: Create an instance of the object.            
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
CDXSpriteList::CDXSpriteList(void)
{
	
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Destructor
// 
// PURPOSE: Deletes all sprites in the list then deletes inself          
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
CDXSpriteList::~CDXSpriteList(void)
{
    m_spriteList.Clear(TRUE);
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Draw
// 
// PURPOSE: Draws all the sprites in the list to the surface pointed to by lpDDS.       
//
// INPUT: lpCDXS     - pointer to the destination surface
//        ScrnWorldX - X position of the screen in the world
//        ScrnWorldY - Y position of the screen in the world
//        BltType    - The type of drawing operation to perform.
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSpriteList::Draw(CDXSurface* lpCDXS, DWORD ScrnWorldX, DWORD ScrnWorldY, WORD BltType)
{
    HRESULT rval = NOERROR;
	CDXSprite* Node;

	// Loop the list and draw the objects
	for(Node = m_spriteList.GetFirst(); Node != NULL; Node = Node->GetNext())
	{
		rval = Node->Draw(lpCDXS, ScrnWorldX, ScrnWorldY, BltType);

        if( FAILED(rval) )
            return rval;
	}

    return rval;
}
