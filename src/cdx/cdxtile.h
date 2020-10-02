//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxtile.h,v $
// $Author: cocoaspud $
//
// $Log: cdxtile.h,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.6  2000/09/09 08:39:09  istan
// Did fixes to AddAnimSection.  I was not init'ing the anim array
// Also modify the last param to be the num of blocks versus the last block
//
// Revision 1.5  2000/08/21 21:26:55  mindcry
// Removed all fstreams and exchanged them with CDXFile. I had to do this
// cause fstream wasn't working right with MFC apps/games and also in some
// circumstances it crashed or not opened the file. Sorry for breaking
// backwards compatibility in some ways. But it was necessary.
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
// Revision 1.1.1.1  2000/04/22 16:13:41  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.7  2000/01/29 05:04:20  MICHAELR
// Added AddAnimSection function
//
// Revision 2.6  2000/01/21 19:12:19  jhebert
// Initial checkin for v3.0
// I'm checking this in for Michael Rich
//
// Revision 2.5  1999/08/29 22:29:05  MICHAELR
// added CDXDLL support to classes/etc
//
// Revision 2.4  1999/05/30 05:13:30  MICHAELR
// added Init func to init all vars & called from all constructors
//
// Revision 2.3  1999/05/23 05:23:40  janni
// added memoryType to CreateFromTLE
//
// Revision 2.2  1999/05/20 15:29:03  pietro
// Multiple changes:
// * fixed #include bugs in all .cpp and various .h files
// * fixed all rcsid[] bugs
// * added conditional compile variable CDXINCLUDEALL - when defined,
//   all #include files are included in cdx.h to keep backward compatibility
// * All the libraries are created in ..\..\lib\vc\ directory, library names are
//   cdx.lib/cdxd.lib/cdxdx3.lib/cdxdx3d.lib/cdxadx3.lib/cdxadx3d.lib
//
// Revision 2.1  1999/05/05 04:47:01  MICHAELR
// Added default value for memoryType in constructors to be consistent with CDXSurface
//
// Revision 2.0  1999/05/01 13:51:17  bsimser
// Updated revision number to 2.0
//
// Revision 1.1.1.1  1999/05/01 04:10:56  bsimser
// Initial revision to cvs
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifndef CDXTILE_H
#define CDXTILE_H

#include "cdxsurface.h"

#define CDXBLT_BLKALPHA25			11
#define CDXBLT_TRANSALPHA25 		111

//////////////////////////////////////////////////////////////////////////////////
// This class name is somewhat deceptive given that a CDXTile object holds the 
// bitmap information for all the tiles of a given map or sprite, rather than just 
// one tile. CDXTile is derived from CDXSurface.
//
// IMPORTANT NOTES: The bitmap file containing the tiles should have an empty tile 
// at the start, this is because CDX treats the first tile to be transparent. Tiles 
// should be stored from left to right down the bitmap and should fit exactly into 
// the width of the bitmap, have a look at the tiles from one of the examples if 
// this is unclear. Remember to set the colour key for the surface if you intend to 
// draw the tiles transparently.
//////////////////////////////////////////////////////////////////////////////////
struct CDX_ANIMSTRUCT
{
	INT32 framerate, pause, rnd;
    CDXBOOL pingpong;
	
	INT32 *anim, num;
};

class CDXAPI CDXTile : public CDXSurface
{
public:
	CDXTile();
	virtual ~CDXTile();

	CDXBOOL Create(CDXScreen *pScreen, const char* szFilename, int w, int h, int num, BYTE memoryType = CDXMEM_VIDTHENSYS);
    CDXBOOL Create(CDXScreen *pScreen, LONG lSize, CDXFile* fs, int w, int h, int num, BYTE memoryType = CDXMEM_VIDTHENSYS);
    CDXBOOL Create(CDXScreen *pScreen, LONG lSize, CHAR* lpCache, int w, int h, int num, BYTE memoryType = CDXMEM_VIDTHENSYS);
	CDXBOOL CreateFromTLE(CDXScreen *pScreen, const char* szFilename , BYTE memoryType = CDXMEM_VIDTHENSYS );

	const RECT *GetBlockRect(int block);

 // Anim stuff

	// adds animation frames to a specific index
//	virtual CDXBOOL AddAnim( int framerate, int pause, int rnd, BOOL pingpong, INT32 *anim, int num );
	virtual INT32 AddAnim( int framerate, int pause, int rnd, BOOL pingpong, INT32 *anim, int num );
//	virtual CDXBOOL AddAnimSection( int framerate, int pause, int rnd, BOOL pingpong, int blockstart, int blockend );
	virtual INT32 AddAnimSection( int framerate, int pause, int rnd, BOOL pingpong, int blockstart, int numblocks );
	virtual CDXBOOL InsertTile( int anim, int pos, int tile );
	virtual CDXBOOL DelAnim( int pos );
	virtual CDXBOOL RemoveTile( int anim, int pos );
	virtual INT32 GetAnimTile( int tile );
	virtual void Draw( int x, int y, CDXSurface *s, int BlitType, int tile );
	virtual void DrawFirst( int x, int y, CDXSurface *s, int BlitType, int tile );
	virtual void NextFrame( );
	virtual CDXBOOL LoadAnim( FILE *fptr );
	virtual CDXBOOL SaveAnim( FILE *fptr );
	virtual inline INT32 GetAnimCount( void )	{	return m_Count;	};
	virtual CDX_ANIMSTRUCT * GetAnimData( int nr );

private:
	CDX_ANIMSTRUCT *m_Data;
	INT32 m_Count, m_FrameCtr;
 // Anim stuff end 

public:
	INT32 GetBlockWidth() { return m_BlockWidth; }
	INT32 GetBlockHeight() { return m_BlockHeight; }
	INT32 GetBlockCount() { return m_BlockNum; }

protected:
	void Init();
	void SetSrc(INT32 block) { CDXSurface::SetClipRect(&m_BlockRects[block]); }

public:
	INT32 m_BlockWidth;		// The width of one tile, in pixels
	INT32 m_BlockHeight;	// The height of one tile, in pixels
	INT32 m_BlockNum;		// The number of tiles in the bitmap file
	INT32 m_BlockX;			// The number of tiles (x)
	INT32 m_BlockY;			// The number of tiles (y)

private:
	RECT *m_BlockRects;    // Pointer to thelist of block rects
};

#endif