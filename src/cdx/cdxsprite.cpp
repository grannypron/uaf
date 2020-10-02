//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxsprite.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdxsprite.cpp,v $
// Revision 1.5  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.5  2000/05/12 22:48:02  hebertjo
// I added RotoZoomimg to CDXSurface and CDXSprite.  I ripped this code from NukeDX.
// IT works and as been debuged by them but it is slow.  CDX definitely needed this
// feature and it did not take long to CDX'ize their code so I added it.  I plan
// on changing this to a faster implementation once I finish it.
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
// Revision 1.1.1.1  2000/04/22 16:11:59  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.14  2000/03/06 18:52:50  jhebert
// Updated the CDXSurface::Draw functions so that they accept a pointer to
// a RECT as the srcRect argument.  This allows you to set this to NULL if
// you want to simply use the whole surface as the source.  This affected
// many of the other classes which have all been modified.
//
// Revision 2.13  2000/01/31 22:55:45  jhebert
// BlkBlt and TransBlt now call the DD functions in CDXSurface.
//
// Revision 2.12  2000/01/28 17:39:00  MICHAELR
// Fixed problem with class referencing member vars of CDXMap
//
// Revision 2.11  2000/01/13 23:43:17  jhebert
// First checkin for v3.0.  No major changes, added some safety chacks and all
// the functions have headers.
//
// 
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxsprite.cpp,v 1.5 2001/10/13 15:19:59 cocoaspud Exp $";
#endif

#include "CDX.h"
#include "cdxsprite.h"

//////////////////////////////////////////////////////////////////////////////////
// NAME: Default Constructor
// 
// PURPOSE: Creates a sprite object             
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
CDXSprite::CDXSprite(void)
{
	Init();
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Init
// 
// PURPOSE: Initializes the class           
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXSprite::Init()
{
	m_PosX			  = 0;
	m_PosY			  = 0;
	m_PosZ			  = 0;
	m_VelX			  = 0;
	m_VelY			  = 0;
	m_Frame			  = 0;
	m_Delay		  	  = 0;
	m_State			  = 0;
	m_Type			  = 0;
	m_Angle			  = 0;
    m_ScaleFactor     = 1;
	m_Flipped		  = FALSE;
	m_ShadowOffsetX   = 0;
	m_ShadowOffsetY   = 0;
	m_ShadowValue	  = 0;
	m_AlphaValue	  = 0;
	m_TileCreated	  = FALSE;
    m_StretchedHeight = 0;
    m_StretchedWidth  = 0;
	m_Tile			  = NULL;
	m_Next			  = NULL;
	m_Prev			  = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Destructor
// 
// PURPOSE: Destroys the surface and frees the memory.          
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
CDXSprite::~CDXSprite(void)
{
	if(m_TileCreated == TRUE)
    {
		delete m_Tile;
        m_Tile = NULL;
    }

	m_Tile = NULL;
	m_Next = NULL;
	m_Prev = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Create
// 
// PURPOSE: Creates a sprite object from a pointer to a CDXTile object holding the 
//          sprite's bitmap data.          
//
// INPUT: pTile - a valid pointer to a CDXTile object
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::Create(CDXTile* pTile)
{
    // If NULL is passed in return error
	if(pTile == NULL) 
		return -1;

    // If the m_Tile is already set return error
    if(GetTile() != NULL)
        return -2;

    // Specify that the class did NOT create the object pointed to by m_Tile
    m_Tile = pTile;
	m_TileCreated = FALSE;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Create
// 
// PURPOSE: Creates the sprite object. Requires a pointer to your CDXScreen object
//          and the name of the bitmap in which the sprite is stored. The w and h
//          parameters refer to the width and height of a single frame of the 
//          sprite and the num parameter refers to the total number of frames in 
//          the bitmap file.        
//
// INPUT: pTile      - valid pointer to a CDXTile object
//        Filename   - name of file or resource with bitmap data
//        w          - the width of one frame of animation
//        h          - the height of one frame of animation
//        num        - number of frames of animation
//        memoryType - determines where the surface will be created sys or vid memory
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::Create(CDXScreen *pScreen, const char* Filename, int w, int h, int num, BYTE memoryType)
{
    HRESULT rval;

    // If the filename is NULL return an error
	if(Filename == NULL) 
        return -1;

    // If the m_Tile is NOT NULL return an error
    if(GetTile() != NULL)
        return -2;

    // Create the m_Tile object
	m_Tile = new CDXTile();
    rval = m_Tile->Create(pScreen, Filename, w, h, num, memoryType);
    if( rval == FALSE )
        return -3;

    m_TileCreated = TRUE;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Draw
// 
// PURPOSE: Staging area for all blit types.  This is the only exposed drawing 
//          function.       
//
// INPUT: lpCDXS     - pointer to destination surface
//        ScrnWorldX - X location of the upper-left corner of the view within 
//                     your world.
//        ScrnWorldY - Y location of the upper-left corner of the view within 
//                     your world.
//        BltType    - what kind of drawing operation you want see header for values
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
// NOTE: If your app is like PacMan, where your entire world is simply the screen
//       then you would set ScrnWorldX and ScrnWorldY both to zero.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::Draw(CDXSurface* lpCDXS, DWORD ScrnWorldX, DWORD ScrnWorldY, WORD BltType)
{
	RECT SpriteDestRect;	// Sprite RECT is defined in world space
	RECT SpriteSrcRect;		// RECT defining the source area for the BLT
	DWORD TilesInWidth;		// Number of tiles in width of sprite tile surface
	DWORD SpriteSrcX;
	DWORD SpriteSrcY;
    int blockWidth, blockHeight;
	HRESULT rval;

    // Get block width and height
    blockWidth  = GetTile()->m_BlockWidth;
    blockHeight = GetTile()->m_BlockHeight;

	// Validate the screen world space coordinates.
	if ( (ScrnWorldX == NULL) || (ScrnWorldX < 0) ) 
		ScrnWorldX = 0;
	if ( (ScrnWorldY == NULL) || (ScrnWorldY < 0) ) 
		ScrnWorldY = 0;

	// RECT defining the sprite in world space.
	SpriteDestRect.top    = m_PosY;
	SpriteDestRect.left   = m_PosX;
	SpriteDestRect.bottom = m_PosY + blockHeight;
	SpriteDestRect.right  = m_PosX + blockWidth;

	// Adjust the sprite RECT to screen space. Destination RECT for the BLT.
	SpriteDestRect.top    -= ScrnWorldY;
	SpriteDestRect.bottom -= ScrnWorldY;
	SpriteDestRect.left   -= ScrnWorldX;
	SpriteDestRect.right  -= ScrnWorldX;

	// Get the number of tiles in the sprite tile surface width
	TilesInWidth = GetTile()->GetWidth() / blockWidth;

	// Calc the upper left corner of the current frame of animation
	SpriteSrcX = (m_Frame % TilesInWidth) * blockWidth;
	SpriteSrcY = (m_Frame / TilesInWidth) * blockHeight;
	
	// Define the source RECT for the BLT.
	SpriteSrcRect.top    = SpriteSrcY;
	SpriteSrcRect.left   = SpriteSrcX;
	SpriteSrcRect.bottom = SpriteSrcY + blockHeight;
	SpriteSrcRect.right  = SpriteSrcX + blockWidth;


	switch(BltType)
	{
	case CDXBLT_BLK:
		rval = BlkBlt(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKALPHA:
		rval = BlkAlpha(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKALPHAFAST:
		rval = BlkAlphaFast(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKSCALED:
		rval = BlkScaled(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKSHADOW:
		rval = BlkShadow(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKSHADOWFAST:
		rval = BlkShadowFast(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKROTATED:
		//rval = BlkRotated(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKROTOZOOM:
		rval = BlkRotoZoom(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKHFLIP:
		rval = BlkHFlip(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKVFLIP:
		rval = BlkVFlip(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKSTRETCHED:
		rval = BlkStretched(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANS:
		rval = TransBlt(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSALPHA:  
		rval = TransAlpha(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSALPHAFAST:
		rval = TransAlphaFast(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSSCALED:
		rval = TransScaled(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSSHADOW:
		rval = TransShadow(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSSHADOWFAST:
		rval = TransShadowFast(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSROTATED:
		//rval = TransRotated(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSROTOZOOM:
		rval = TransRotoZoom(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSHFLIP:
		rval = TransHFlip(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSVFLIP:
		rval = TransVFlip(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSSTRETCHED:
		rval = TransStretched(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;


	case CDXBLT_TRANSALPHAMASK:
		rval = TransAlphaMask(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	default:
		break;
	}

    return rval;
}

HRESULT CDXSprite::Blit(CDXSurface* lpCDXS, const RECT *srcRect, const RECT *dstRect, WORD BltType)
{
  if ((srcRect==NULL)||(dstRect==NULL)) return -1;

	RECT SpriteDestRect = *dstRect;
	RECT SpriteSrcRect = *srcRect;
	HRESULT rval=-1;

	switch(BltType)
	{
	case CDXBLT_BLK:
		rval = BlkBlt(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKALPHA:
		rval = BlkAlpha(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKALPHAFAST:
		rval = BlkAlphaFast(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKSCALED:
		rval = BlkScaled(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKSHADOW:
		rval = BlkShadow(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKSHADOWFAST:
		rval = BlkShadowFast(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKROTATED:
		//rval = BlkRotated(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKROTOZOOM:
		//rval = BlkRotoZoom(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKHFLIP:
		rval = BlkHFlip(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKVFLIP:
		rval = BlkVFlip(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_BLKSTRETCHED:
		rval = BlkStretched(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANS:
		rval = TransBlt(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSALPHA:  
		rval = TransAlpha(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSALPHAFAST:
		rval = TransAlphaFast(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSSCALED:
		rval = TransScaled(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSSHADOW:
		rval = TransShadow(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSSHADOWFAST:
		rval = TransShadowFast(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSROTATED:
		//rval = TransRotated(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSROTOZOOM:
		//rval = TransRotoZoom(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSHFLIP:
		rval = TransHFlip(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSVFLIP:
		rval = TransVFlip(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSSTRETCHED:
		rval = TransStretched(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	case CDXBLT_TRANSALPHAMASK:
		rval = TransAlphaMask(lpCDXS, SpriteSrcRect, SpriteDestRect);
		break;
	default:
		break;
	}
  return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: BlkRotoZoom
// 
// PURPOSE:    
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::BlkRotoZoom(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;
    DWORD w,h;

    w = SrcRect.right - SrcRect.left;
    h = SrcRect.bottom - SrcRect.top;

	rval = m_Tile->CDXSurface::DrawBlkRotoZoom(lpCDXSDest, DestRect.left + w/2, DestRect.top + h/2, &SrcRect, m_Angle, m_ScaleFactor);

	return rval;
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: TransRotoZoom
// 
// PURPOSE:    
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::TransRotoZoom(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;
    DWORD w,h;

    w = SrcRect.right - SrcRect.left;
    h = SrcRect.bottom - SrcRect.top;

	rval = m_Tile->CDXSurface::DrawTransRotoZoom(lpCDXSDest, DestRect.left + w/2, DestRect.top + h/2, &SrcRect, m_Angle, m_ScaleFactor);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetTile
// 
// PURPOSE: To reset the m_Tile pointer.  This allows you to retain the sprite but
//          change it's looks by pointing to a different tileset.  If this class
//          created the original CDXTile it will be delete and lost forever.
//
// INPUT: lpTile - pointer to a valid CDXTile object.
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXSprite::SetTile(CDXTile* lpTile)
{
    // If this class created the CDXTile then delete it.
    if(m_TileCreated)
    {
		delete m_Tile;
        m_TileCreated = FALSE;
    }

    // Set the m_Tile pointer
    m_Tile = lpTile;

}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetShadowOffset
// 
// PURPOSE: Sets the offset of the shadow to use when bltting.       
//
// INPUT: dx - offset in the X direction can be negitive
//        dy - offset in the Y direction can be negitive
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXSprite::SetShadowOffset(LONG dx, LONG dy)
{
	m_ShadowOffsetX = dx;
	m_ShadowOffsetY = dy;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: TransStretched
// 
// PURPOSE: Draws a stretched transparent sprite.       
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::TransStretched(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawTransStretched(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect,m_StretchedWidth ,m_StretchedHeight);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: BlkStretched
// 
// PURPOSE: Draws a stretched transparent sprite.       
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::BlkStretched(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawBlkStretched(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect,m_StretchedWidth ,m_StretchedHeight);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: TransScaled
// 
// PURPOSE: Draws a scaled transparent sprite.       
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::TransScaled(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawTransScaled(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect, m_ScaleFactor);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: BlkScaled
// 
// PURPOSE: Draws a scaled sprite, no transparency.      
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::BlkScaled(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawBlkScaled(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect, m_ScaleFactor);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: TransBlt
// 
// PURPOSE: Draws a transparent sprite.    
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::TransBlt(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawTrans(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: BlkBlt
// 
// PURPOSE: Draws a Block sprite, no transparency.   
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::BlkBlt(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawBlk(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: TransHFlip
// 
// PURPOSE: Transparently horizontally flips the sprite.   
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::TransHFlip(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawTransHFlip(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: BlkHFlip
// 
// PURPOSE: Flips the sprite horizontally.   
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::BlkHFlip(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawBlkHFlip(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: TransVFlip
// 
// PURPOSE: Transparently vertically flips the sprite.   
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::TransVFlip(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawTransVFlip(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: BlkVFlip
// 
// PURPOSE: Vertically flips the sprite.   
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::BlkVFlip(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawBlkVFlip(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: BlkAlphaFast
// 
// PURPOSE:    
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::BlkAlphaFast(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawBlkAlphaFast(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect);

	return rval;
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: TransAlphaFast
// 
// PURPOSE:    
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::TransAlphaFast(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawTransAlphaFast(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect);

	return rval;
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: TransAlpha
// 
// PURPOSE:    
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::TransAlpha(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawTransAlpha(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect, m_AlphaValue);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: BlkAlpha
// 
// PURPOSE:    
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::BlkAlpha(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawBlkAlpha(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect, m_AlphaValue);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: BlkShadow
// 
// PURPOSE:    
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::BlkShadow(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawBlkShadow(lpCDXSDest, DestRect.left - m_ShadowOffsetX, DestRect.top - m_ShadowOffsetY, &SrcRect, m_ShadowValue);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: TransShadow
// 
// PURPOSE:    
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::TransShadow(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawTransShadow(lpCDXSDest, DestRect.left - m_ShadowOffsetX, DestRect.top - m_ShadowOffsetY, &SrcRect, m_ShadowValue);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: BlkShadowFast
// 
// PURPOSE:    
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::BlkShadowFast(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawBlkShadowFast(lpCDXSDest, DestRect.left - m_ShadowOffsetX, DestRect.top - m_ShadowOffsetY, &SrcRect);

	return rval;
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: TransShadowFast
// 
// PURPOSE:    
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::TransShadowFast(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawTransShadowFast(lpCDXSDest, DestRect.left - m_ShadowOffsetX, DestRect.top - m_ShadowOffsetY, &SrcRect);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: TransAlphaMask
// 
// PURPOSE:    
//
// INPUT: lpCDXSDest - destination surface pointer
//        SrcRect    - rectangle defining source bitmap to copy
//        DestRect   - destination rectangle definition
//
// RETURNS: 0 or > - succeeded
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSprite::TransAlphaMask(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect)
{
	HRESULT rval;

	rval = m_Tile->CDXSurface::DrawTransAlphaMask(lpCDXSDest, DestRect.left, DestRect.top, &SrcRect);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SpriteHit
// 
// PURPOSE: Checks for collisions between sprites. Returns TRUE if any part of 
//          pSprite overlaps with the source sprite.   
//
// INPUT: pSprite - pointer of sprite to check collision against
//
// RETURNS: TRUE  - succeeded
//          FALSE - failed
//
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXSprite::SpriteHit(CDXSprite* pSprite)
{
	if(pSprite == this) 
        return FALSE;

	RECT Rect1 = { m_PosX, m_PosY,
                   m_PosX + GetTile()->m_BlockWidth,
                   m_PosY + GetTile()->m_BlockHeight };

	RECT Rect2 = { pSprite->GetPosX(), pSprite->GetPosY(),
                   pSprite->GetPosX() + pSprite->GetTile()->m_BlockWidth,
                   pSprite->GetPosY() + pSprite->GetTile()->m_BlockHeight };

	if(Rect1.top > Rect2.bottom) return FALSE;
	else if(Rect1.bottom < Rect2.top) return FALSE;
	else if(Rect1.right < Rect2.left) return FALSE;
	else if(Rect1.left > Rect2.right) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: TileHit
// 
// PURPOSE:  Checks for collisions between the source sprite and tiles in a map.
//           Returns TRUE if any part of the sprite overlaps with a tile of value
//           Tile in the specified map. In mMap->GetTile(x,y) it's necessary to 
//           keep within the X and Y boundaries! So min(X,MaxX-1) ensures that 
//           you can't exceed the max X or Y position 
//
// INPUT: pMap - current map
//        Tile - tile id
//
// RETURNS: TRUE  - succeeded
//          FALSE - failed
//
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXSprite::TileHit(CDXMap* pMap, int Tile)
{
	int MapX, MapY, TileType;

    // Check top left
    MapX = (pMap->GetPosX() + GetPosX() ) / pMap->GetTileWidth();
    MapY = (pMap->GetPosY() + GetPosY() ) / pMap->GetTileHeight();
    TileType = pMap->GetTile(min(MapX, pMap->GetMapWidth()-1), min(MapY, pMap->GetMapHeight()-1));
    if(TileType == Tile) return TRUE;

    // Check top right
    MapX = (pMap->GetPosX() + GetPosX() + GetTile()->m_BlockWidth) / pMap->GetTileWidth();
    MapY = (pMap->GetPosY() + GetPosY() ) / pMap->GetTileHeight();
    TileType = pMap->GetTile(min(MapX, pMap->GetMapWidth()-1), min(MapY, pMap->GetMapHeight()-1));
    if(TileType == Tile) return TRUE;

    // Check bottom left
    MapX = (pMap->GetPosX() + GetPosX() ) / pMap->GetTileWidth();
    MapY = (pMap->GetPosY() + GetPosY() + GetTile()->m_BlockHeight) / pMap->GetTileHeight();
    TileType = pMap->GetTile(min(MapX, pMap->GetMapWidth()-1), min(MapY, pMap->GetMapHeight()-1));
    if(TileType == Tile) return TRUE;

    // Check bottom right
    MapX = (pMap->GetPosX() + GetPosX() + GetTile()->m_BlockWidth) / pMap->GetTileWidth();
    MapY = (pMap->GetPosY() + GetPosY() + GetTile()->m_BlockHeight) / pMap->GetTileHeight();
    TileType = pMap->GetTile(min(MapX, pMap->GetMapWidth()-1), min(MapY, pMap->GetMapHeight()-1));
    if(TileType == Tile) return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SpriteHitPixel
// 
// PURPOSE:  This function first performs a bounding box collision detection 
//           check and if the 2 bounding boxes are coliding it then performs
//           a pixel level collision detection check.
//
// INPUT: pSprite - pointer to the sprite object that you want to check collision with
//
// RETURNS: TRUE  - succeeded
//          FALSE - failed
//
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXSprite::SpriteHitPixel(CDXSprite* pSprite)
{	
	CDXSprite *rightSprite;
	CDXSprite *leftSprite;
	UINT  intersectWidth;
	UINT  intersectHeight;
	POINT LSPoint;
	POINT RSPoint;
	UINT  LSFrame;
	UINT  RSFrame;
	LONG  LSPitch;
	LONG  RSPitch;
	UINT  LSWidth;
	UINT  LSHeight;
	UINT  RSWidth;
	UINT  RSHeight;
	UINT  LSFramesInWidth;
	UINT  RSFramesInWidth;
	UINT  LSFrameX;
	UINT  LSFrameY;
	UINT  RSFrameX;
	UINT  RSFrameY;
	DWORD LSColorKey;
	DWORD RSColorKey;
	BYTE* LSptr;
	BYTE* RSptr;
	UINT  x,y;
    RGBFORMAT pixelFormat;

	// If we are checking against ourseves, no collision.
	if(pSprite == this) 
		return FALSE;

	// Rectangle describing sprites current world location.
	RECT Rect1 = { GetPosX(), 
				   GetPosY(),
				   GetPosX() + GetTile()->m_BlockWidth,
				   GetPosY() + GetTile()->m_BlockHeight };

	// Rectangle describing the current sprite we are checking colisions against.
	RECT Rect2 = { pSprite->GetPosX(), 
				   pSprite->GetPosY(),
				   pSprite->GetPosX() + pSprite->GetTile()->m_BlockWidth,
				   pSprite->GetPosY() + pSprite->GetTile()->m_BlockHeight };

	// We must perform a bounding box collision detection first
	// otherwise we might get unexpected results.
	if(Rect1.top > Rect2.bottom) 
		return FALSE;
	else if(Rect1.bottom < Rect2.top) 
		return FALSE;
	else if(Rect1.right < Rect2.left) 
		return FALSE;
	else if(Rect1.left > Rect2.right) 
		return FALSE;

	// Determine which sprite is to the left and which is to the right.
	if (pSprite->GetPosX() > GetPosX() )
	{
		rightSprite = pSprite;
		leftSprite  = this;
	}
	else
	{
		rightSprite = this;
		leftSprite  = pSprite;
	}
	
	// Get the width and height of each sprite
	LSWidth  = leftSprite->GetTile()->m_BlockWidth;
	LSHeight = leftSprite->GetTile()->m_BlockHeight;
	RSWidth  = rightSprite->GetTile()->m_BlockWidth;
	RSHeight = rightSprite->GetTile()->m_BlockHeight;
	
	// Get the current frame for each sprite
	LSFrame = leftSprite->GetFrame();
	RSFrame = rightSprite->GetFrame();
	
	// Get the amount the two sprites overlap in the X direction
	intersectWidth = LSWidth - (rightSprite->GetPosX() - leftSprite->GetPosX());

	// The intersecting width cannot be greater then the sprite width.
	if (intersectWidth > RSWidth)
		intersectWidth = RSWidth;

	// Determine which sprite is lower on the screen.
	if (leftSprite->GetPosY() > rightSprite->GetPosY()) 
	{
		LSPoint.y = 0;
		RSPoint.y = leftSprite->GetPosY() - rightSprite->GetPosY();
		intersectHeight = RSHeight - (leftSprite->GetPosY() - rightSprite->GetPosY());
		if (intersectHeight > LSWidth)
			intersectHeight = LSWidth;
	}
	else // left sprite is higher on the screen.
	{
		RSPoint.y = 0;
		LSPoint.y = rightSprite->GetPosY() - leftSprite->GetPosY();
		intersectHeight = LSHeight - (rightSprite->GetPosY() - leftSprite->GetPosY());
		if (intersectHeight > LSWidth)
			intersectHeight = LSWidth;
	}

	// Calculate the starting X position of collision.
	RSPoint.x = 0;
	LSPoint.x = rightSprite->GetPosX() - leftSprite->GetPosX();

	// Determine the number of frames that fit across the surface.
	LSFramesInWidth = leftSprite->GetTile()->GetWidth() / LSWidth;
	RSFramesInWidth = rightSprite->GetTile()->GetWidth() / RSWidth;

	// Find x & y location on the surface(s) of first pixel in question
	LSFrameX = ((LSFrame % LSFramesInWidth) * LSWidth) + (UINT)LSPoint.x;
	RSFrameX = ((RSFrame % RSFramesInWidth) * RSWidth) + (UINT)RSPoint.x;
	LSFrameY = ((LSFrame / LSFramesInWidth) * LSHeight) + (UINT)LSPoint.y;
	RSFrameY = ((RSFrame / RSFramesInWidth) * RSHeight) + (UINT)RSPoint.y;

	// Get the color key for each surface.
	LSColorKey = leftSprite->GetTile()->GetColorKey();
	RSColorKey = rightSprite->GetTile()->GetColorKey();


	// We are assumming that both surfaces have the same pixel bit depth.
    leftSprite->GetTile()->GetPixelFormat(&pixelFormat);
	switch(pixelFormat.bpp)
	{
	case 8:
		// The potential exists that the two sprites we are collition detecting
		// exist on the same surface.  In that case we only need to lock one surface.
		if (leftSprite->GetTile() == rightSprite->GetTile())
		{
			if ( FAILED(leftSprite->GetTile()->Lock() ) )
				return FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->GetTile()->GetPitch();
			RSPitch = LSPitch;

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr  = (BYTE*)leftSprite->GetTile()->GetSurfacePointer();
			LSptr += LSFrameX + (LSFrameY * (UINT)LSPitch);

			RSptr  = (BYTE*)rightSprite->GetTile()->GetSurfacePointer();
			RSptr += RSFrameX + (RSFrameY * (UINT)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if ((*LSptr != (BYTE)LSColorKey) && (*RSptr != (BYTE)RSColorKey))
					{
						leftSprite->GetTile()->UnLock();
						return TRUE;
					}
					LSptr ++;
					RSptr ++;
				}
				LSptr += LSPitch - intersectWidth;
				RSptr += RSPitch - intersectWidth;
			}
			leftSprite->GetTile()->UnLock();
		}
		else // Sprites are on different surfaces.
		{
			if ( FAILED(leftSprite->GetTile()->Lock() ) )
				return FALSE;
			if ( FAILED(rightSprite->GetTile()->Lock() ) )
				return FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->GetTile()->GetPitch();
			RSPitch = rightSprite->GetTile()->GetPitch();

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr  = (BYTE*)leftSprite->GetTile()->GetSurfacePointer();
			LSptr += LSFrameX + (LSFrameY * (UINT)LSPitch);

			RSptr  = (BYTE*)rightSprite->GetTile()->GetSurfacePointer();
			RSptr += RSFrameX + (RSFrameY * (UINT)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if ((*LSptr != (BYTE)LSColorKey) && (*RSptr != (BYTE)RSColorKey))
					{
						leftSprite->GetTile()->UnLock();
						rightSprite->GetTile()->UnLock();
						return TRUE;
					}
					LSptr ++;
					RSptr ++;
				}
				LSptr += LSPitch - intersectWidth;
				RSptr += RSPitch - intersectWidth;
			}
			leftSprite->GetTile()->UnLock();
			rightSprite->GetTile()->UnLock();
		}
		break;

	case 15:
	case 16:
		// The potential exists that the two sprites we are collition detecting
		// exist on the same surface.  In that case we only need to lock one surface.
		if (leftSprite->GetTile() == rightSprite->GetTile())
		{
			if ( FAILED(leftSprite->GetTile()->Lock() ) )
				return FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->GetTile()->GetPitch();
			RSPitch = LSPitch;

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr  = (BYTE*)leftSprite->GetTile()->GetSurfacePointer();
			LSptr += (LSFrameX * 2) + (LSFrameY * (UINT)LSPitch);

			RSptr  = (BYTE*)rightSprite->GetTile()->GetSurfacePointer();
			RSptr += (RSFrameX * 2) + (RSFrameY * (UINT)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if ((*((WORD*)LSptr) != (WORD)LSColorKey) && (*((WORD*)RSptr) != (WORD)RSColorKey))
					{
						leftSprite->GetTile()->UnLock();
						return TRUE;
					}
					LSptr += 2;
					RSptr += 2;
				}
				LSptr += LSPitch - (intersectWidth * 2);
				RSptr += RSPitch - (intersectWidth * 2);
			}
			leftSprite->GetTile()->UnLock();
		}
		else // Sprites are on different surfaces.
		{
			if ( FAILED(leftSprite->GetTile()->Lock() ) )
				return FALSE;
			if ( FAILED(rightSprite->GetTile()->Lock() ) )
				return FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->GetTile()->GetPitch();
			RSPitch = rightSprite->GetTile()->GetPitch();

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr  = (BYTE*)leftSprite->GetTile()->GetSurfacePointer();
			LSptr += (LSFrameX * 2) + (LSFrameY * (UINT)LSPitch);

			RSptr  = (BYTE*)rightSprite->GetTile()->GetSurfacePointer();
			RSptr += (RSFrameX * 2) + (RSFrameY * (UINT)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if ((*((WORD*)LSptr) != (WORD)LSColorKey) && (*((WORD*)RSptr) != (WORD)RSColorKey))
					{
						leftSprite->GetTile()->UnLock();
						rightSprite->GetTile()->UnLock();
						return TRUE;
					}
					LSptr += 2;
					RSptr += 2;
				}
				LSptr += LSPitch - (intersectWidth * 2);
				RSptr += RSPitch - (intersectWidth * 2);
			}
			leftSprite->GetTile()->UnLock();
			rightSprite->GetTile()->UnLock();
		}
		break;

	case 24:
		// The potential exists that the two sprites we are collition detecting
		// exist on the same surface.  In that case we only need to lock one surface.
		if (leftSprite->GetTile() == rightSprite->GetTile())
		{
			if ( FAILED(leftSprite->GetTile()->Lock() ) )
				return FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->GetTile()->GetPitch();
			RSPitch = LSPitch;

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr  = (BYTE*)leftSprite->GetTile()->GetSurfacePointer();
			LSptr += (LSFrameX * 3) + (LSFrameY * (UINT)LSPitch);

			RSptr  = (BYTE*)rightSprite->GetTile()->GetSurfacePointer();
			RSptr += (RSFrameX * 3) + (RSFrameY * (UINT)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if (( (*((DWORD*)LSptr) & 0xFFFFFF) != (DWORD)LSColorKey) && ( (*((DWORD*)RSptr) & 0xFFFFFF) != (DWORD)RSColorKey))
					{
						leftSprite->GetTile()->UnLock();
						return TRUE;
					}
					LSptr += 3;
					RSptr += 3;
				}
				LSptr += LSPitch - (intersectWidth * 3);
				RSptr += RSPitch - (intersectWidth * 3);
			}
			leftSprite->GetTile()->UnLock();
		}
		else // Sprites are on different surfaces.
		{
			if ( FAILED(leftSprite->GetTile()->Lock() ) )
				return FALSE;
			if ( FAILED(rightSprite->GetTile()->Lock() ) )
				return FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->GetTile()->GetPitch();
			RSPitch = rightSprite->GetTile()->GetPitch();

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr  = (BYTE*)leftSprite->GetTile()->GetSurfacePointer();
			LSptr += (LSFrameX * 3) + (LSFrameY * (UINT)LSPitch);

			RSptr  = (BYTE*)rightSprite->GetTile()->GetSurfacePointer();
			RSptr += (RSFrameX * 3) + (RSFrameY * (UINT)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if (( (*((DWORD*)LSptr) & 0xFFFFFF) != (DWORD)LSColorKey) && ( (*((DWORD*)RSptr) & 0xFFFFFF) != (DWORD)RSColorKey))
					{
						leftSprite->GetTile()->UnLock();
						rightSprite->GetTile()->UnLock();
						return TRUE;
					}
					LSptr += 3;
					RSptr += 3;
				}
				LSptr += LSPitch - (intersectWidth * 3);
				RSptr += RSPitch - (intersectWidth * 3);
			}
			leftSprite->GetTile()->UnLock();
			rightSprite->GetTile()->UnLock();
		}
		break;

	case 32:
		// The potential exists that the two sprites we are collition detecting
		// exist on the same surface.  In that case we only need to lock one surface.
		if (leftSprite->GetTile() == rightSprite->GetTile())
		{
			if ( FAILED(leftSprite->GetTile()->Lock() ) )
				return FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->GetTile()->GetPitch();
			RSPitch = LSPitch;

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr  = (BYTE*)leftSprite->GetTile()->GetSurfacePointer();
			LSptr += (LSFrameX * 4) + (LSFrameY * (UINT)LSPitch);

			RSptr  = (BYTE*)rightSprite->GetTile()->GetSurfacePointer();
			RSptr += (RSFrameX * 4) + (RSFrameY * (UINT)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if ((*((DWORD*)LSptr) != (DWORD)LSColorKey) && (*((DWORD*)RSptr) != (DWORD)RSColorKey))
					{
						leftSprite->GetTile()->UnLock();
						return TRUE;
					}
					LSptr += 4;
					RSptr += 4;
				}
				LSptr += LSPitch - (intersectWidth * 4);
				RSptr += RSPitch - (intersectWidth * 4);
			}
			leftSprite->GetTile()->UnLock();
		}
		else // Sprites are on different surfaces.
		{
			if ( FAILED(leftSprite->GetTile()->Lock() ) )
				return FALSE;
			if ( FAILED(rightSprite->GetTile()->Lock() ) )
				return FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->GetTile()->GetPitch();
			RSPitch = rightSprite->GetTile()->GetPitch();

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr  = (BYTE*)leftSprite->GetTile()->GetSurfacePointer();
			LSptr += (LSFrameX * 4) + (LSFrameY * (UINT)LSPitch);

			RSptr  = (BYTE*)rightSprite->GetTile()->GetSurfacePointer();
			RSptr += (RSFrameX * 4) + (RSFrameY * (UINT)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if (( (*((DWORD*)LSptr) & 0xFFFFFF) != (DWORD)LSColorKey) && ( (*((DWORD*)RSptr) & 0xFFFFFF) != (DWORD)RSColorKey))
					{
						leftSprite->GetTile()->UnLock();
						rightSprite->GetTile()->UnLock();
						return TRUE;
					}
					LSptr += 4;
					RSptr += 4;
				}
				LSptr += LSPitch - (intersectWidth * 4);
				RSptr += RSPitch - (intersectWidth * 4);
			}
			leftSprite->GetTile()->UnLock();
			rightSprite->GetTile()->UnLock();
		}
		break;

	default:
		break;
	}
	return FALSE;
}
