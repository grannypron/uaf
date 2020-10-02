//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxlayer.h,v $
// $Author: cocoaspud $
//
// $Log: cdxlayer.h,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.3  2000/05/13 20:43:59  hebertjo
// Added another Create() function that takes a single parameter of a CDXSurface*.
// This way you can create a Layer out of any CDXSurface (Which could have been
// loaded in a variety of ways).
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.1.1.1  2000/04/22 16:10:17  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.3  1999/08/29 22:29:04  MICHAELR
// added CDXDLL support to classes/etc
//
// Revision 2.2  1999/05/20 15:29:02  pietro
// Multiple changes:
// * fixed #include bugs in all .cpp and various .h files
// * fixed all rcsid[] bugs
// * added conditional compile variable CDXINCLUDEALL - when defined,
//   all #include files are included in cdx.h to keep backward compatibility
// * All the libraries are created in ..\..\lib\vc\ directory, library names are
//   cdx.lib/cdxd.lib/cdxdx3.lib/cdxdx3d.lib/cdxadx3.lib/cdxadx3d.lib
//
// Revision 2.1  1999/05/07 20:24:30  janni
// fixed constructor, changed parameter memoryType to have as
// default value CDXMEM_VIDTHENSYS
//
// Revision 2.0  1999/05/01 13:51:16  bsimser
// Updated revision number to 2.0
//
// Revision 1.1.1.1  1999/05/01 04:10:56  bsimser
// Initial revision to cvs
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifndef CDXLAYER_H
#define CDXLAYER_H

#include "cdxsurface.h"

//////////////////////////////////////////////////////////////////////////////////
// CDXLayer is derived from CDXSurface and is very similar in most respects. 
// However, a CDXLayer has the added ability to smoothly scroll the surface in 
// all directions. This effect is often seen in platform games where the background 
// is a scrolling bitmap and the foreground a scrolling tile map, giving a good 
// feeling of depth. A CDXLayer would also be perfect for a pinball game since 
// DirectX places no restrictions on the height of bitmaps in video memory.
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXLayer
{
public:
    CDXLayer(void);
    virtual ~CDXLayer(void);

	HRESULT Create(CDXScreen *pScreen, char *szFilename, BOOL memoryType = CDXMEM_VIDTHENSYS );
	HRESULT Create(CDXSurface* pSurface);

	void ScrollUp(int Offset);
	void ScrollDown(int Offset);
	void ScrollLeft(int Offset);
	void ScrollRight(int Offset);
	void MoveTo(int XOffset, int YOffset);

	virtual HRESULT Draw(CDXSurface* lpDDS);
	virtual HRESULT Draw(int X, int Y, CDXSurface* lpDDS);
	virtual HRESULT DrawTrans(int X, int Y, CDXSurface* lpDDS);

public:
	int         m_XOffset;   // The current X position in pixels
	int         m_YOffset;   // The current Y position in pixels
	CDXSurface* m_pSurface;
	BOOL        m_bSurfaceCreated;
};

#endif
