//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxlayer.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdxlayer.cpp,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.5  2000/05/13 20:43:59  hebertjo
// Added another Create() function that takes a single parameter of a CDXSurface*.
// This way you can create a Layer out of any CDXSurface (Which could have been
// loaded in a variety of ways).
//
// Revision 1.4  2000/05/10 23:29:26  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.3  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.2  2000/04/26 18:45:43  hebertjo
// Made many small changes to fix numerous bugs and some enhancements.
//
// Revision 1.1.1.1  2000/04/22 16:10:17  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.5  2000/03/06 18:52:49  jhebert
// Updated the CDXSurface::Draw functions so that they accept a pointer to
// a RECT as the srcRect argument.  This allows you to set this to NULL if
// you want to simply use the whole surface as the source.  This affected
// many of the other classes which have all been modified.
//
// Revision 2.4  2000/01/30 03:53:42  MICHAELR
// Modifed to use CDXSurface blit methods, and not the direct DX blit functions
//
// Revision 2.3  2000/01/21 19:42:55  jhebert
// Initial checkin for v3.0.
//
// Revision 2.2  1999/05/23 05:23:05  janni
// changed constructor, it now calls its father constructor and then inits itself
//
// Revision 2.1  1999/05/20 15:29:02  pietro
// Multiple changes:
// * fixed #include bugs in all .cpp and various .h files
// * fixed all rcsid[] bugs
// * added conditional compile variable CDXINCLUDEALL - when defined,
//   all #include files are included in cdx.h to keep backward compatibility
// * All the libraries are created in ..\..\lib\vc\ directory, library names are
//   cdx.lib/cdxd.lib/cdxdx3.lib/cdxdx3d.lib/cdxadx3.lib/cdxadx3d.lib
//
// Revision 2.0  1999/05/01 13:51:16  bsimser
// Updated revision number to 2.0
//
// Revision 1.1.1.1  1999/05/01 04:10:56  bsimser
// Initial revision to cvs
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxlayer.cpp,v 1.4 2001/10/13 15:19:59 cocoaspud Exp $";
#endif

#include "CDX.h"
#include "cdxlayer.h"

//////////////////////////////////////////////////////////////////////////////////
// Creates a DirectDrawSurface object holding the bitmap pointed to by szFilename.
//////////////////////////////////////////////////////////////////////////////////
CDXLayer::CDXLayer(void)
{	
	m_XOffset         = 0;
	m_YOffset         = 0;
	m_pSurface        = NULL;
	m_bSurfaceCreated = FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////////////////
CDXLayer::~CDXLayer(void) 
{	

	// Delete the surface if we created it and it is not null;
	if( (m_bSurfaceCreated == TRUE) && (m_pSurface != NULL) )
	{
		delete m_pSurface;
		m_pSurface = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Creates a DirectDrawSurface object holding the bitmap pointed to by szFilename.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXLayer::Create(CDXScreen *pScreen, char *szFilename, BOOL memoryType) 
{	
    HRESULT rval;

	//Validate pScreen
    if(pScreen == NULL)
        return -1;

    //Validate szFilename
    if(szFilename == NULL)
        return -2;

	// Create the CDXSurface.
	m_pSurface = new CDXSurface();
	if(m_pSurface == NULL)
		return -3;
    rval = m_pSurface->Create(pScreen , szFilename , memoryType);

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// Creates a DirectDrawSurface object holding the bitmap pointed to by szFilename.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXLayer::Create(CDXSurface* pSurface) 
{	
    m_pSurface = pSurface;

    return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// Scroll the bitmap up by the number of pixels indicated by offset.
//////////////////////////////////////////////////////////////////////////////////
void CDXLayer::ScrollUp(int Offset)
{
    m_YOffset += Offset;

    if( m_YOffset >= m_pSurface->GetHeight() ) 
        m_YOffset -= m_pSurface->GetHeight();
}

//////////////////////////////////////////////////////////////////////////////////
// Scroll the bitmap down by the number of pixels indicated by offset.
//////////////////////////////////////////////////////////////////////////////////
void CDXLayer::ScrollDown(int Offset)
{
    m_YOffset -= Offset;

    if( m_YOffset < 0 ) 
        m_YOffset = m_pSurface->GetHeight() + m_YOffset;
}

//////////////////////////////////////////////////////////////////////////////////
// Scroll the bitmap left by the number of pixels indicated by offset.
//////////////////////////////////////////////////////////////////////////////////
void CDXLayer::ScrollLeft(int Offset)
{
    m_XOffset += Offset;

    if( m_XOffset >= m_pSurface->GetWidth() ) 
        m_XOffset -= m_pSurface->GetWidth();
}

//////////////////////////////////////////////////////////////////////////////////
// Scroll the bitmap right by the number of pixels indicated by offset.
//////////////////////////////////////////////////////////////////////////////////
void CDXLayer::ScrollRight(int Offset)
{
    m_XOffset -= Offset;

    if( m_XOffset < 0 ) 
        m_XOffset = m_pSurface->GetWidth() + m_XOffset;
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the position in the bitmap.
//////////////////////////////////////////////////////////////////////////////////
void CDXLayer::MoveTo(int XOffset, int YOffset)
{
    m_XOffset = XOffset;
    m_YOffset = YOffset;
}

//////////////////////////////////////////////////////////////////////////////////
// Copies the bitmap from the source surface to the surface pointed to by lpDDS.  
// Uses the DestRect structure to position the bitmap. The bitmap automatically 
// wraps around the screen.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXLayer::Draw(CDXSurface* s)
{
	HRESULT rval;
    RECT rect;
    int h, w;

    h = m_pSurface->GetHeight();
    w = m_pSurface->GetWidth();

    rect.top    = h - m_YOffset;
    rect.left   = w - m_XOffset;
    rect.bottom = h;
    rect.right  = w;
    m_pSurface->DrawBlk(s, 0, 0, &rect);

    rect.top    = h - m_YOffset;
    rect.left   = 0;
    rect.bottom = h;
    rect.right  = w - m_XOffset;
	m_pSurface->DrawBlk(s, m_XOffset, 0, &rect);

    rect.top    = 0;
    rect.left   = w - m_XOffset;
    rect.bottom = h - m_YOffset;
    rect.right  = w;
	m_pSurface->DrawBlk(s, 0, m_YOffset, &rect);

    rect.top    = 0;
    rect.left   = 0;
    rect.bottom = h - m_YOffset;
    rect.right  = w - m_XOffset;
	rval = m_pSurface->DrawBlk(s, m_XOffset, m_YOffset, &rect);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// Draw the layer at the given X, Y location to the surface lpDDS.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXLayer::Draw(int X,int Y,CDXSurface* s)
{
	HRESULT rval;
    RECT rect;
    int h, w;

    h = m_pSurface->GetHeight();
    w = m_pSurface->GetWidth();

    rect.top    = h - m_YOffset;
    rect.left   = w - m_XOffset;
    rect.bottom = h;
    rect.right  = w;
    m_pSurface->DrawBlk(s, 0, 0, &rect);

    rect.top    = h - m_YOffset;
    rect.left   = 0;
    rect.bottom = h;
    rect.right  = w - m_XOffset;
	m_pSurface->DrawBlk(s, m_XOffset+X, 0, &rect);

    rect.top    = 0;
    rect.left   = w - m_XOffset;
    rect.bottom = h - m_YOffset;
    rect.right  = w;
	m_pSurface->DrawBlk(s, 0, m_YOffset+Y, &rect);

    rect.top    = 0;
    rect.left   = 0;
    rect.bottom = h - m_YOffset;
    rect.right  = w - m_XOffset;
	rval = m_pSurface->DrawBlk(s, m_XOffset, m_YOffset, &rect);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// Draw the layer using the transparent color at the given X, Y location to 
// the surface lpDDS.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXLayer::DrawTrans(int X,int Y,CDXSurface* s)
{
	HRESULT rval;
    RECT rect;
    int h, w;

    h = m_pSurface->GetHeight();
    w = m_pSurface->GetWidth();

    rect.top    = h - m_YOffset;
    rect.left   = w - m_XOffset;
    rect.bottom = h;
    rect.right  = w;
    m_pSurface->DrawTrans(s, 0, 0, &rect);

    rect.top    = h - m_YOffset;
    rect.left   = 0;
    rect.bottom = h;
    rect.right  = w - m_XOffset;
    m_pSurface->DrawTrans(s, m_XOffset+X, 0, &rect);

    rect.top    = 0;
    rect.left   = w - m_XOffset;
    rect.bottom = h - m_YOffset;
    rect.right  = w;
    m_pSurface->DrawTrans(s, 0, m_YOffset+Y, &rect);

    rect.top    = 0;
    rect.left   = 0;
    rect.bottom = h - m_YOffset;
    rect.right  = w - m_XOffset;
    rval = m_pSurface->CDXSurface::DrawTrans(s, m_XOffset, m_YOffset, &rect);

	return rval;
}