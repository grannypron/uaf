//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxmap.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdxmap.cpp,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.8  2001/02/15 23:30:54  istan
// Fixed calculation of the SCREEN_TW/TH vars
// Fixed ScrollDown and ScrollUp to not break bottom right tile
// Thanks for both fixes go out to javiermd@wanadoo.es
//
// Revision 1.7  2000/09/07 06:27:43  istan
// Fixed some clipping problems when the tilesize wasn't evenly divisible by the height or width
//
// Revision 1.6  2000/08/13 22:15:46  mindcry
// Hot fixed the Drawing of a clipped Map
//
// Revision 1.5  2000/08/13 09:54:20  mindcry
// A few bugfixes and added the NextAnim function
// Fixed by Gorth
//
// Revision 1.4  2000/07/21 16:15:54  mindcry
// Several small fixes, that keeped crashing when called with
// wrong parameters
//
// Revision 1.3  2000/05/22 17:55:03  hebertjo
// Fixed a bug in the Create function as per David M.'s instructions.
//
// Revision 1.2  2000/05/10 23:29:26  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.1.1.1  2000/04/22 16:10:36  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.12  2000/01/30 04:47:47  MICHAELR
// Added accessor methods for some private variables
//
// Revision 2.11  2000/01/29 05:03:31  MICHAELR
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
// Revision 2.8  2000/01/28 17:15:21  MICHAELR
// Modified code to use then new m_ prefixed member vars
// Remarked out the WrapScrollXXX functions
// Added the code body for the new EnableScrollWrapping function
// Added Init() function and called it from both constructors
// Modified the ScrollXXX functions to use the new scrolling var when deciding how to scroll
// In destructor, modified deletion of m_DATA array to use DELETEARRAY
//
// Revision 2.7  2000/01/21 19:09:21  jhebert
// Initial v3.0 checkin
//
// Revision 2.6  1999/08/03 19:58:46  Mindcry
// because i have mixed some code together, can't say exactly what all has changed.
//
// Revision 2.5  1999/08/03 06:43:42  Mindcry
// Changes include:
// -ScrollRight() and ScrollDown() were modified so that the possibility of
// m_PosX and m_PosY being zero is eliminated
// -Adjustable Viewport added through the function SetViewPort(LPRECT)
// -Function RECT GetTileCoord(int, int) added to the get screen
// coordinates of a tile at any map coordinates.
// (thank Mike for this)
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
static char rcsid[] = "@(#) $Id: cdxmap.cpp,v 1.4 2001/10/13 15:19:59 cocoaspud Exp $";
#endif

#include "CDX.h"
#include "cdxmap.h"
#include "cdxsprite.h"

//////////////////////////////////////////////////////////////////////////////////
// CDXMap Default Constructor
//////////////////////////////////////////////////////////////////////////////////
CDXMap::CDXMap(CDXScreen *pScreen)
{
	// initialize the class so we have a fresh start
	Init();

	m_SCREEN_W = pScreen->GetWidth();
	m_SCREEN_H = pScreen->GetHeight();
}

//////////////////////////////////////////////////////////////////////////////////
// CDXMap Constructor
//////////////////////////////////////////////////////////////////////////////////
CDXMap::CDXMap(CDXTile *pTiles, CDXScreen *pScreen)
{
	// initialize the class so we have a fresh start
	Init();

	m_Tiles = pTiles;
	m_Screen = pScreen;

	m_TileWidth = m_Tiles->m_BlockWidth;
	m_TileHeight = m_Tiles->m_BlockHeight;

	m_SCREEN_W = pScreen->GetWidth();
	m_SCREEN_H = pScreen->GetHeight();

//	m_SCREEN_TW = (m_SCREEN_W / m_TileWidth) + 1;
//	m_SCREEN_TH = (m_SCREEN_H / m_TileHeight) + 1;

	// fixed reported by javiermd@wanadoo.es
	m_SCREEN_TW = (m_SCREEN_W / m_TileWidth); 
	m_SCREEN_TH = (m_SCREEN_H / m_TileHeight); 

	if(m_SCREEN_W % m_TileWidth) m_SCREEN_TW++; 
	if(m_SCREEN_H % m_TileHeight) m_SCREEN_TH++; 


	LoadTiles(pTiles);
}

//////////////////////////////////////////////////////////////////////////////////
// Default destructor. Destroys the map and frees the memory.
//////////////////////////////////////////////////////////////////////////////////
CDXMap::~CDXMap()
{
	// delete the map data
	DELETEARRAY(m_DATA);


	// get rid of our reference to the tiles
	m_Tiles = NULL;

	// do the same with the screen reference
	m_Screen = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// Initializes the class
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::Init()
{
	m_bScrollWrapping = FALSE;

	m_DATA = NULL;
	m_Tiles = NULL;
	m_Screen = NULL;

	m_TileWidth = 0;
	m_TileHeight = 0;

	m_PosX = 0;
	m_PosY = 0;
	
	m_ViewportX = 0;
	m_ViewportY = 0;

	m_SCREEN_W = 0;
	m_SCREEN_H = 0;

	m_SCREEN_TW = 0;
	m_SCREEN_TH = 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Creates a new map. Fills in the map with the specified Fill value.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXMap::Create(int Width, int Height, int Fill)
{
	m_Width = Width;
	m_Height = Height;

	m_PixelWidth = m_Width * m_TileWidth;
	m_PixelHeight = m_Height * m_TileHeight;

//  CHANGE:  Per David Maziarka 05-22-2000
	//m_SCREEN_TW = min(m_SCREEN_W, m_Width);
	//m_SCREEN_TH = min(m_SCREEN_H, m_Height);
//    if( m_TileWidth ) m_SCREEN_TW = (m_SCREEN_W / m_TileWidth) + 1;
//    if( m_TileHeight ) m_SCREEN_TH = (m_SCREEN_H / m_TileHeight) + 1;
//  CHANGE:  Per David Maziarka 05-22-2000
	// fixed reported by javiermd@wanadoo.es
	m_SCREEN_TW = (m_SCREEN_W / m_TileWidth); 
	m_SCREEN_TH = (m_SCREEN_H / m_TileHeight); 

	if(m_SCREEN_W % m_TileWidth) m_SCREEN_TW++; 
	if(m_SCREEN_H % m_TileHeight) m_SCREEN_TH++; 

	m_SIZE = m_Width * m_Height;
	return InitData(m_SIZE, Fill);
}

//////////////////////////////////////////////////////////////////////////////////
// Initializes the array of data with the specified fill value.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXMap::InitData(int nSIZE, int iFill)
{
	m_DATA = new CDXMapCell[nSIZE];
	if( m_DATA==NULL ) return FALSE;

	for(int i=0; i<m_SIZE; i++)
		m_DATA[i].SetTile(iFill);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Loads a map from a filepointer.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXMap::Load( FILE *fptr )
{
	if( fptr==NULL ) return FALSE;

	fread(&m_Width, sizeof(int), 1, fptr);
	fread(&m_Height, sizeof(int), 1, fptr);

	if( Create( m_Width, m_Height, 0 ) == FALSE )
		return FALSE;

    for(int x=0; x<m_SIZE; x++)
		m_DATA[x].Load(fptr);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Loads a map from a file.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXMap::Load(const char *szFilename)
{
	FILE *fp;
	BOOL rval;

	if(szFilename == NULL) return FALSE;

	fp = fopen(szFilename, "rb");
	if(fp == NULL) return FALSE;

	rval = Load(fp);
	if( rval==TRUE && m_Tiles )
		rval = m_Tiles->LoadAnim( fp );

	fclose(fp);

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// Saves a map to a filepointer.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXMap::Save( FILE *fptr )
{
	if(fptr == NULL) return FALSE;

	fwrite(&m_Width, sizeof(int), 1, fptr);
	fwrite(&m_Height, sizeof(int), 1, fptr);

	m_SIZE = m_Width * m_Height;

	for(int x=0; x<m_SIZE; x++)
		m_DATA[x].Save(fptr);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Saves a map to a file.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXMap::Save(const char *szFilename)
{
	FILE *fp;
	BOOL retval;

	fp = fopen(szFilename, "wb");
	retval = Save(fp);
	if(retval) retval = m_Tiles->SaveAnim( fp );

	fclose(fp);

	return retval;
}

//////////////////////////////////////////////////////////////////////////////////
// Clears out the map and sets all the map tiles to 0.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::Clear()
{
	for(int i=0; i<m_SIZE; i++)
		m_DATA[i].SetTile(0);
}

//////////////////////////////////////////////////////////////////////////////////
// Fills the map with a tile specified by TileNum.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::Fill(int TileNum)
{
  for(int i=0; i<m_SIZE; i++)
		m_DATA[i].SetTile(TileNum);
}

//////////////////////////////////////////////////////////////////////////////////
// Set the position in the map. All values are in pixels.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::MoveTo(int PosX, int PosY)
{
	m_PosX = PosX;
	m_PosY = PosY;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the current X position of the map in pixels
//////////////////////////////////////////////////////////////////////////////////
int CDXMap::GetPosX()
{
	return m_PosX;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the current Y position of the map in pixels
//////////////////////////////////////////////////////////////////////////////////
int CDXMap::GetPosY()
{
	return m_PosY;
}

//////////////////////////////////////////////////////////////////////////////////
// Scroll the map upwards by the offset passed in.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::ScrollUp(int Offset)
{
	if (m_bScrollWrapping)
	{
		if(m_PosY - Offset >= 0) m_PosY -= Offset;
		else m_PosY = (m_Height * m_TileHeight) - Offset + m_PosY;
	}
	else
	{
		m_PosY -= Offset;
		if(m_PosY < 0) m_PosY = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Scroll the map down by the offset passed in.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::ScrollDown(int Offset)
{
	if (m_bScrollWrapping)
	{
		if (m_PosY + Offset < (m_Height * m_TileHeight)) m_PosY += Offset;
		else m_PosY = Offset - (m_Height * m_TileHeight) + m_PosY;
	}
	else
	{
		m_PosY += Offset;

//		if(m_PosY > (m_Height - m_SCREEN_TH) * m_TileHeight)
//			m_PosY = max(0,(m_Height - m_SCREEN_TH) * m_TileHeight);

		// fixed reported by javiermd@wanadoo.es
		if(m_PosY > (m_Height * m_TileHeight - m_SCREEN_H)) 
			m_PosY = max(0,m_Height * m_TileHeight - m_SCREEN_H); 
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Scroll the map left by the offset passed in.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::ScrollLeft(int Offset)
{
	if (m_bScrollWrapping)
	{
		if (m_PosX - Offset >= 0) m_PosX -= Offset;
		else m_PosX = (m_Width * m_TileWidth) - Offset + m_PosX;
	}
	else
	{
		m_PosX -= Offset;
		if(m_PosX < 0) m_PosX = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Scroll the map right by the offset passed in.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::ScrollRight(int Offset)
{
	if (m_bScrollWrapping)
	{
		if (m_PosX + Offset < (m_Width * m_TileWidth)) m_PosX += Offset;
		else m_PosX = Offset - (m_Width * m_TileWidth) + m_PosX;
	}
	else
	{
		m_PosX += Offset;

//		if(m_PosX > (m_Width - m_SCREEN_TW) * m_TileWidth)
//			m_PosX = max(0,(m_Width - m_SCREEN_TW) * m_TileWidth);

		// fixed reported by javiermd@wanadoo.es
		if(m_PosX > (m_Width * m_TileWidth - m_SCREEN_W)) 
			m_PosX = max(0,m_Width * m_TileWidth - m_SCREEN_W); 
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Enables / Disables the wrapping function of the scroll methods.  If this is
// enabled, the map will wrap around instead of simply stopping.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::EnableScrollWrapping(BOOL bEnable)
{
	m_bScrollWrapping = bEnable;
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the screen tile width and screen tile height. This is the number of 
// tiles drawn vertically and horizontally on screen.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::ScreenTileSize(int Width, int Height)
{
	m_SCREEN_TW = Width;
	m_SCREEN_TH = Height;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the width of the map in map units
//////////////////////////////////////////////////////////////////////////////////
int CDXMap::GetMapWidth()
{
	return m_Width;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the height of the map in map units
//////////////////////////////////////////////////////////////////////////////////
int CDXMap::GetMapHeight()
{
	return m_Height;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the width of the screen as the map see it
//////////////////////////////////////////////////////////////////////////////////
int CDXMap::GetScreenWidth()
{
	return m_SCREEN_W;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the height of the screen as the map see it
//////////////////////////////////////////////////////////////////////////////////
int CDXMap::GetScreenHeight()
{
	return m_SCREEN_H;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the number of blocks that will be drawn horizontally on the map
//////////////////////////////////////////////////////////////////////////////////
int CDXMap::GetScreenBlocksWidth()
{
	return m_SCREEN_TW;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the number of blocks that will be drawn vertically on the map
//////////////////////////////////////////////////////////////////////////////////
int CDXMap::GetScreenBlocksHeight()
{
	return m_SCREEN_TH;
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the top left coordinates of the viewport and sets the screen width and
// height in map units.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::SetViewPort(LPRECT View)
{
	m_ViewportX = View->left;
	m_ViewportY = View->top;
	m_SCREEN_TW = ((View->right - View->left) / m_TileWidth) + 1;
	m_SCREEN_TH = ((View->bottom - View->top) / m_TileHeight) + 1;
}

//////////////////////////////////////////////////////////////////////////////////
// This returns a RECT struct containing the coordinates of any tile in the map
// relative to screen coordinates.  (Useful for positioning sprites for the
// first time)
//////////////////////////////////////////////////////////////////////////////////
RECT CDXMap::GetTileCoord(int MapX, int MapY)
{
	RECT TileCoord;
	TileCoord.left = (m_PosX - (m_TileWidth * MapX)) * -1;
	TileCoord.top = (m_PosY - (m_TileHeight * MapY)) * -1;
	TileCoord.right = TileCoord.left + m_TileWidth;
	TileCoord.bottom = TileCoord.top + m_TileHeight;
	return TileCoord;
}

//////////////////////////////////////////////////////////////////////////////////
// This gets the CDXMapCell object from the map at the given MapX and MapY coordinates.
//////////////////////////////////////////////////////////////////////////////////
CDXMapCell *CDXMap::GetCell(int MapX, int MapY)
{
	if( MapX<0 || MapY<0 || MapX>=m_Width || MapY>=m_Height ) return 0;
	return &m_DATA[MapX + (MapY * m_Width)];
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the value of the tile at MapX, MapY. Could be used for tile collision detection.
//////////////////////////////////////////////////////////////////////////////////
int CDXMap::GetTile(int MapX, int MapY)
{
	if( MapX<0 || MapY<0 || MapX>=m_Width || MapY>=m_Height ) return 0;
	return m_DATA[MapX + (MapY * m_Width)].GetTile();
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the value of the tile at MapX, MapY. Could be used for tile collision detection.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::SetTile(int MapX, int MapY, int Tile)
{
	if( MapX>=0 && MapY>=0 && MapX<m_Width && MapY<m_Height )
		m_DATA[MapX + (MapY * m_Width)].SetTile(Tile);
}

//////////////////////////////////////////////////////////////////////////////////
// Loads a different set of tiles to the map.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::LoadTiles(CDXTile *pTiles)
{
	m_Tiles = pTiles;

	m_TileWidth = m_Tiles->GetBlockWidth();
	m_TileHeight = m_Tiles->GetBlockHeight();

//	m_SCREEN_TW = (m_SCREEN_W / m_TileWidth) + 1;
//	m_SCREEN_TH = (m_SCREEN_H / m_TileHeight) + 1;
	
	// fixed reported by javiermd@wanadoo.es
	m_SCREEN_TW = (m_SCREEN_W / m_TileWidth); 
	m_SCREEN_TH = (m_SCREEN_H / m_TileHeight); 

	if(m_SCREEN_W % m_TileWidth) m_SCREEN_TW++; 
	if(m_SCREEN_H % m_TileHeight) m_SCREEN_TH++; 
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the width of the tile blocks being used in pixels
//////////////////////////////////////////////////////////////////////////////////
int CDXMap::GetTileWidth()
{
	return m_TileWidth;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the height of the tile blocks being used in pixels
//////////////////////////////////////////////////////////////////////////////////
int CDXMap::GetTileHeight()
{
	return m_TileHeight;
}

//////////////////////////////////////////////////////////////////////////////////
// Draws the map to the surface pointed to by lpDDS.
// Additional: With any style you want via BlitStyle. 
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::Draw(CDXSurface* lpDDS, int BlitStyle, BOOL Wrap )
{
	int i,j;
	int xoffset, yoffset;
	int xcoord = 0, ycoord = 0;
	int yidx = m_PosY/m_TileHeight;
	int xidx;

	xoffset = m_PosX % m_TileWidth;
	yoffset = m_PosY % m_TileHeight;

	// NEXT X NUMBER OF ROWS
	for(j=0, ycoord=m_ViewportY; j<m_SCREEN_TH+1; j++)
	{
		xcoord = m_ViewportX;
		xidx = m_PosX/m_TileWidth;

		for(i=0; i<m_SCREEN_TW+1; i++)
		{
			int tile = m_DATA[yidx*m_Width+xidx].GetTile( );
			m_Tiles->Draw(xcoord-xoffset, ycoord-yoffset, lpDDS, BlitStyle, tile );
			xcoord += m_TileWidth;
			xidx++;
			if( xidx>=m_Width )
			{	
				if( Wrap ) xidx=0;
				else break;
			}
		}

		yidx++;
		if( yidx>=m_Height )
		{
			if( Wrap ) yidx=0;
			else break;
		}
		ycoord += m_TileHeight;
	}

}

//////////////////////////////////////////////////////////////////////////////////
// Draws the map to the surface pointed to by lpDDS with transparent tiles. If 
// you haven't set the colour key for the tiles nothing will be drawn.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::DrawTrans(CDXSurface* lpDDS )
{
	int i,j;
	int xoffset, yoffset;
	int xcoord = 0, ycoord = 0;
	int yidx = m_PosY/m_TileHeight;
	int xidx;

	xoffset = m_PosX % m_TileWidth;
	yoffset = m_PosY % m_TileHeight;

	// NEXT X NUMBER OF ROWS
	for(j=0, ycoord=m_ViewportY; j<m_SCREEN_TH+1; j++)
	{
		xcoord = m_ViewportX;
		xidx = m_PosX/m_TileWidth;

		for(i=0; i<m_SCREEN_TW+1; i++)
		{
			int tile = m_DATA[yidx*m_Width+xidx].GetTile( );
			m_Tiles->Draw(xcoord-xoffset, ycoord-yoffset, lpDDS, CDXBLT_TRANS, tile );
			xcoord += m_TileWidth;
			xidx++;
			if( xidx>=m_Width ) xidx=0;
		}

		yidx++;
		if( yidx>=m_Height ) yidx=0;
		ycoord += m_TileHeight;
	}

}

//////////////////////////////////////////////////////////////////////////////////
// Draws the map to the surface pointed to by lpDDS. The map is clipped to 
// the rectangle ClipDestRect.
//////////////////////////////////////////////////////////////////////////////////
void CDXMap::DrawClipped(CDXSurface* lpDDS, LPRECT ClipRect)
{

	// clipping Hot fix [MINDCRY]
	RECT sRect;
	CopyRect( &sRect, lpDDS->GetClipRect() );
	lpDDS->SetClipRect( ClipRect );

	int i,j;
	int xoffset, yoffset;
	int xcoord = 0, ycoord = 0;
	int yidx = m_PosY/m_TileHeight;
	int xidx;

	xoffset = m_PosX % m_TileWidth;
	yoffset = m_PosY % m_TileHeight;

	// NEXT X NUMBER OF ROWS
	for(j=0, ycoord=m_ViewportY; j<m_SCREEN_TH+1; j++)
	{
		xcoord = m_ViewportX;
		xidx = m_PosX/m_TileWidth;

		for(i=0; i<m_SCREEN_TW+1; i++)
		{
			int tile = m_DATA[yidx*m_Width+xidx].GetTile( );
			m_Tiles->Draw(xcoord-xoffset, ycoord-yoffset, lpDDS, 0, tile );
			xcoord += m_TileWidth;
			xidx++;
			if( xidx>=m_Width ) xidx=0;
		}

		yidx++;
		if( yidx>=m_Height ) yidx=0;
		ycoord += m_TileHeight;
	}

	lpDDS->SetClipRect( &sRect );
}

//////////////////////////////////////////////////////////////////////////////////
// Advances the Tile's animation to the next frame
//////////////////////////////////////////////////////////////////////////////////

void CDXMap::NextFrame()
{
	if(m_Tiles)
		m_Tiles->NextFrame();
}