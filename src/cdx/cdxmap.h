//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxmap.h,v $
// $Author: cocoaspud $
//
// $Log: cdxmap.h,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.6  2001/02/17 21:04:27  istan
// Added GetPixelWidth & GetPixelHeight properties
//
// Revision 1.5  2000/08/18 14:28:29  mindcry
// added new function GetMapSize
//
// Revision 1.4  2000/08/13 09:54:49  mindcry
// A few bugfixes and added the NextAnim function
// Fixed by Gorth
//
// Revision 1.3  2000/07/21 16:17:34  mindcry
// Several minor fixes
//
// Revision 1.2  2000/05/10 23:29:26  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.1.1.1  2000/04/22 16:10:39  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.12  2000/01/30 04:47:47  MICHAELR
// Added accessor methods for some private variables
//
// Revision 2.11  2000/01/29 05:03:32  MICHAELR
// Added some ZDRPG enhancements to the class that were missing
//
// Revision 2.10  2000/01/28 17:45:06  MICHAELR
// Added GetPosX/GetPosY functions
// Added GetTileWidth/GetTileHeight functions
// Made all member variables private
// Prefixed remaing member vars with m_
//
// Revision 2.9  2000/01/28 17:24:12  MICHAELR
// Added GetMapWidth/GetMapHeight functions
//
// Revision 2.8  2000/01/28 17:12:02  MICHAELR
// Prefixed some member vars with m_ to help designate them better
// Remarked out the WrapScrollXXX functions
// Added EnableScrollWrapping function
// Added Init() function
// Change section level for member vars to Protected from Public
// Added member var m_bScrollWrapping
//
// Revision 2.7  1999/08/29 22:29:04  MICHAELR
// added CDXDLL support to classes/etc
//
// Revision 2.6  1999/08/15 04:03:16  MICHAELR
// vitualized the CDXMap class so we could make subclasses more easily
//
// Revision 2.5  1999/08/10 18:05:27  MICHAELR
// made destructor, load, and save for cdxmapcell virtual
//
// Revision 2.4  1999/08/03 19:58:20  Mindcry
// because i have mixed some code together, can't say exactly what all has changed.
//
// Revision 2.3  1999/08/03 06:43:42  Mindcry
// Changes include:
// -ScrollRight() and ScrollDown() were modified so that the possibility of
// m_PosX and m_PosY being zero is eliminated
// -Adjustable Viewport added through the function SetViewPort(LPRECT)
// -Function RECT GetTileCoord(int, int) added to the get screen
// coordinates of a tile at any map coordinates.
// (thank Mike for this)
//
// Revision 2.1  1999/05/20 15:29:03  pietro
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
#ifndef CDXMAP_H
#define CDXMAP_H

#include "cdxtile.h"

//////////////////////////////////////////////////////////////////////////////////
// The CDXMapCell is a helper class for all CDXMap classes. It allows you to 
// define override a custom class to define your data for each map tile. This 
// way you can create your own CDXMapCell class and store as much information
// as you want and save it the way you want.
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXMapCell
{
public:
	CDXMapCell();
	virtual ~CDXMapCell();

	virtual void Save(FILE *fp);
	virtual void Load(FILE *fp);
	virtual int GetTile();
	virtual void SetTile(int Tile);
private:
	int TileNum;
};

//////////////////////////////////////////////////////////////////////////////////
// This is the main object which controls tile based scrolling in CDX. Screen, 
// tile and map information all come together in this class to produce a scrolling 
// output.  Map information is stored (using the default CDXMapCell class) in 
// binary files in the simplest possible form:
//
// Map width (4 bytes)
// Map height (4 bytes)
// Map data (Map width * Map height * 4 bytes)
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXMap
{
public:
	CDXMap(CDXScreen *pScreen);
	CDXMap(CDXTile *pTiles, CDXScreen *pScreen);
	virtual ~CDXMap();

	// creation functions
	virtual BOOL Create(int Width, int Height, int Fill);

	virtual BOOL Load( FILE *fptr );
	virtual BOOL Load(const char *szFilename);
	virtual BOOL Save( FILE *fptr );
	virtual BOOL Save(const char *szFilename);
	virtual void Clear(void);
	virtual void Fill(int TileNum);

	// drawing functions
	virtual void Draw(CDXSurface* lpDDS, int BlitStyle = 0, BOOL Wrap=TRUE );
	virtual void DrawTrans(CDXSurface* lpDDS);
	virtual void DrawClipped(CDXSurface* lpDDS, LPRECT ClipRect);

	// animation functions
	virtual void NextFrame();

	// location functions
	virtual void MoveTo(int PosX, int PosY);
	virtual int GetPosX();
	virtual int GetPosY();
	virtual void ScrollUp(int Offset);
	virtual void ScrollDown(int Offset);
	virtual void ScrollLeft(int Offset);
	virtual void ScrollRight(int Offset);
	void EnableScrollWrapping(BOOL bEnable);
	virtual void ScreenTileSize(int Width, int Height);
	virtual void SetViewPort(LPRECT View);
	virtual RECT GetTileCoord(int MapX, int MapY);
	virtual int  GetTile(int MapX, int MapY);
	virtual CDXMapCell *GetCell(int MapX, int MapY);
	virtual void SetTile(int MapX, int MapY, int Tile);
	virtual void LoadTiles(CDXTile *pTiles);
	virtual BOOL InitData(int iSIZE, int iFill);
	virtual int GetMapWidth();
	virtual int GetMapHeight();
	virtual int GetMapSize() { return m_SIZE; };
	virtual int GetPixelWidth() { return m_PixelWidth; }
	virtual int GetPixelHeight() { return m_PixelHeight; }
	virtual int GetTileWidth();
	virtual int GetTileHeight();
	virtual int GetScreenWidth();
	virtual int GetScreenHeight();
	virtual int GetScreenBlocksWidth();
	virtual int GetScreenBlocksHeight();

protected:
	void Init();

protected:
	int m_PosX;			// The current map X position in pixels
	int m_PosY;			// The current map Y position in pixels
	int m_Width;		// The map width in map units
	int m_Height;		// The map height in map units
	int m_TileWidth;	// Tile width in pixels, from the CDXTile pointer
	int m_TileHeight;	// Tile height in pixels, from the CDXTile pointer
	int m_PixelWidth;	// The width of the map in pixels
	int m_PixelHeight;	// The height of the map in pixels
	int m_ViewportX;	// The starting X position of the Viewport
	int m_ViewportY;	// The starting Y position of the Viewport
	int m_SCREEN_TW;		// Number of tiles wide on screen
	int m_SCREEN_TH;		// Number of tiles high on screen
	int m_SCREEN_W;		// Screen pixel width, from CDXScreen pointer
	int m_SCREEN_H;		// Screen pixel height, from CDXScreen pointer
	int m_SIZE;			// Map size, width * height

protected:
	CDXMapCell *m_DATA;		// Pointer to the CDXMapCell object for tiles
	CDXScreen *m_Screen;	// Pointer to the CDXScreen object
	CDXTile *m_Tiles;		// Pointer to the CDXTile object

	BOOL m_bScrollWrapping;	// turn this on to wrap while you scroll
};

//////////////////////////////////////////////////////////////////////////////////
// This is the main object which controls tile based ISO scrolling in CDX. Screen, 
// tile and map information all come together in this class to produce a scrolling 
// output.
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXIsoMap : public CDXMap
{
public:
	CDXIsoMap(CDXTile *pTiles, CDXScreen *pScreen):CDXMap(pTiles, pScreen) {};
    virtual void ScrollUp(int);
    virtual void ScrollDown(int);
    virtual void ScrollLeft(int);
    virtual void ScrollRight(int);
    virtual void BltTileTrans(CDXSurface*, int, int, int, int, int, int, int, int);
    virtual void DrawTrans(CDXSurface*);
	virtual POINT ProcessClicks(int, int);
};

#endif
