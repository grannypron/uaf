//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/Cdxtile.cpp,v $
// $Author: cocoaspud $
//
// $Log: Cdxtile.cpp,v $
// Revision 1.5  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.9  2000/09/09 08:39:09  istan
// Did fixes to AddAnimSection.  I was not init'ing the anim array
// Also modify the last param to be the num of blocks versus the last block
//
// Revision 1.8  2000/08/21 21:26:17  mindcry
// Removed all fstreams and exchanged them with CDXFile. I had to do this
// cause fstream wasn't working right with MFC apps/games and also in some
// circumstances it crashed or not opened the file. Sorry for breaking
// backwards compatibility in some ways. But it was necessary.
//
// I also fixed another crash problem with CDXTile. When called with a to high tile number.
//
// Revision 1.7  2000/08/13 09:55:25  mindcry
// A few bugfixes by Gorth
//
// Revision 1.6  2000/05/10 23:29:26  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.5  2000/05/09 01:22:06  hebertjo
// Moved the CDXBLT_XXXX #defines to cdxsurface.h.  This is where they belong, thanks
// to who ever pointed that out.
//
// Revision 1.4  2000/05/08 02:30:46  hebertjo
// Got rid of the two warning we were getting.
//
// Revision 1.3  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.2  2000/04/26 18:45:43  hebertjo
// Made many small changes to fix numerous bugs and some enhancements.
//
// Revision 1.1.1.1  2000/04/22 16:13:39  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.14  2000/03/22 21:57:18  wasteland
// Fixed bug where it was still evaluating the return of CDXSurface::Create() incorrectly
//
// Revision 2.13  2000/03/20 21:14:26  wasteland
// Changed DrawBlk to DrawDDBlk.  Added a typecast to remove a MSVC warning.
//
// Revision 2.12  2000/03/06 18:52:51  jhebert
// Updated the CDXSurface::Draw functions so that they accept a pointer to
// a RECT as the srcRect argument.  This allows you to set this to NULL if
// you want to simply use the whole surface as the source.  This affected
// many of the other classes which have all been modified.
//
// Revision 2.11  2000/01/29 07:08:30  MICHAELR
// Updated Draw to called DrawDDTrans for the Trans blt
//
// Revision 2.10  2000/01/29 05:04:19  MICHAELR
// Added AddAnimSection function
//
// Revision 2.9  2000/01/25 05:20:39  jhebert
// It might help if I actually make the change!
//
// Revision 2.8  2000/01/25 05:19:01  jhebert
// Fixed a problem with multiple calls to CDXSurface::Create from one of the constructors.
//
// Revision 2.7  2000/01/21 19:12:19  jhebert
// Initial checkin for v3.0
// I'm checking this in for Michael Rich
//
// Revision 2.6  1999/07/20 11:01:48  janni
// fixed CDXTile::Create always returned TRUE, no matter if
// there was an error or not
//
// Revision 2.5  1999/05/30 18:26:56  janni
// fixed CDXTile::Create bug I have introduced
// when you called Create(Screen,Filename,width,height,num) Create was empty
//
// Revision 2.4  1999/05/30 05:13:30  MICHAELR
// added Init func to init all vars & called from all constructors
//
// Revision 2.3  1999/05/23 05:24:53  janni
// added memoryType to CreateFromTLE
// all constructors now call their father constructors and then init themselves
// the problem with CReateFromTLE crashes is fixed, was invalid ddsd member
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
// Revision 2.1  1999/05/03 20:59:00  MICHAELR
// CreateFromTLE didn't have proper handling of DX3 using #ifdefs for ddsd
//
// Revision 2.0  1999/05/01 13:51:17  bsimser
// Updated revision number to 2.0
//
// Revision 1.1.1.1  1999/05/01 04:10:56  bsimser
// Initial revision to cvs
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: Cdxtile.cpp,v 1.5 2001/10/13 15:19:59 cocoaspud Exp $";
#endif

#include "CDX.h"
#include "cdxmap.h"

typedef struct
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[256];
} BITMAPINFO_256;




//////////////////////////////////////////////////////////////////////////////////
// Default constructor
//////////////////////////////////////////////////////////////////////////////////
CDXTile::CDXTile() : CDXSurface()
{
	Init();
}


//////////////////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////////////////
CDXTile::~CDXTile()
{
	if (m_BlockRects!=NULL) { delete [] m_BlockRects; m_BlockRects = 0; }
	for( int i=0; i<m_Count; i++ )
		DELETEARRAY(m_Data[i].anim);
	
	DELETEARRAY(m_Data);
}

//////////////////////////////////////////////////////////////////////////////////
// Initializes the class
//////////////////////////////////////////////////////////////////////////////////
void CDXTile::Init()
{
	// reset out block vars
	m_BlockWidth = m_BlockHeight = m_BlockNum = 0;
	m_BlockX = m_BlockY = 0;

	// clear the pointer to our RECT structures
	m_BlockRects = NULL;

	// Reset our animation data
	m_Count = 0;
	m_FrameCtr = 0;
	m_Data = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// Creates a DirectDrawSurface and loads the tiles from a .BMP file onto it. You 
// must also specify the width and height of a single tile and the number of tiles 
// in the bitmap.
//////////////////////////////////////////////////////////////////////////////////
CDXBOOL CDXTile::Create(CDXScreen *pScreen, const char* szFilename, int w, int h, int num, BYTE memoryType)
{
	int i;
	DWORD x, y;

	if(szFilename == NULL) 
		return FALSE;

	// Moved it here so it will load the BMP first [MND]
	if(FAILED(CDXSurface::Create( pScreen , szFilename , memoryType )))
		return FALSE;

	m_BlockWidth = w;
	m_BlockHeight = h;

	m_BlockX = (GetWidth()/w);
	m_BlockY = (GetHeight()/h);

	// if num==0 count the tiles [MND]
	if( num == 0 ) num = m_BlockX*m_BlockY;
	m_BlockNum = num;

	// allocate space for our rects
	m_BlockRects = new RECT[num];
	if (m_BlockRects == NULL) return FALSE;
	
	// clear out the block rect memory
	ZeroMemory(m_BlockRects, sizeof(RECT) * num);
	for (i=0, x=0, y=0;i<num;i++, x+=m_BlockWidth)
	{
		if (x>(DWORD)(GetWidth()-m_BlockWidth))
		{
			x = 0;
			y+=m_BlockHeight;
		}

		m_BlockRects[i].left = x;
		m_BlockRects[i].top = y;
		m_BlockRects[i].right = x + m_BlockWidth;
		m_BlockRects[i].bottom = y + m_BlockHeight;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Creates a DirectDrawSurface and loads the tiles from a .BMP file onto it. You 
// must also specify the width and height of a single tile and the number of tiles 
// in the bitmap.
//////////////////////////////////////////////////////////////////////////////////
CDXBOOL CDXTile::Create(CDXScreen *pScreen, LONG lSize, CDXFile* fs, int w, int h, int num, BYTE memoryType)
{
	int i;
	DWORD x, y;

	if(fs == NULL) 
		return FALSE;

	// Moved it here so it will load the BMP first [MND]
	if( CDXSurface::Create( pScreen, lSize, fs, memoryType ) == FALSE )
		return FALSE;

	m_BlockWidth = w;
	m_BlockHeight = h;

	m_BlockX = (GetWidth()/w);
	m_BlockY = (GetHeight()/h);

	// if num==0 count the tiles [MND]
	if( num == 0 ) num = m_BlockX*m_BlockY;
	m_BlockNum = num;

	// allocate space for our rects
	m_BlockRects = new RECT[num];
	if (m_BlockRects == NULL) return FALSE;
	
	// clear out the block rect memory
	ZeroMemory(m_BlockRects, sizeof(RECT) * num);
	for (i=0, x=0, y=0;i<num;i++, x+=m_BlockWidth)
	{
		if (x > (DWORD)GetWidth() - m_BlockWidth)
		{
			x = 0;
			y+=m_BlockHeight;
		}

		m_BlockRects[i].left = x;
		m_BlockRects[i].top = y;
		m_BlockRects[i].right = x + m_BlockWidth;
		m_BlockRects[i].bottom = y + m_BlockHeight;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Creates a DirectDrawSurface and loads the tiles from a .BMP file onto it. You 
// must also specify the width and height of a single tile and the number of tiles 
// in the bitmap.
//////////////////////////////////////////////////////////////////////////////////
CDXBOOL CDXTile::Create(CDXScreen *pScreen, LONG lSize, CHAR* lpCache, int w, int h, int num, BYTE memoryType)
{
	int i;
    CHAR* cache;
	DWORD x, y;

	if(lpCache == NULL) 
		return FALSE;

    cache = lpCache;

	// Moved it here so it will load the BMP first [MND]
	if( CDXSurface::Create( pScreen, lSize, cache, memoryType ) == FALSE )
		return FALSE;

	m_BlockWidth = w;
	m_BlockHeight = h;

	m_BlockX = (GetWidth()/w);
	m_BlockY = (GetHeight()/h);

	// if num==0 count the tiles [MND]
	if( num == 0 ) num = m_BlockX*m_BlockY;
	m_BlockNum = num;

	// allocate space for our rects
	m_BlockRects = new RECT[num];
	if (m_BlockRects == NULL) return FALSE;
	
	// clear out the block rect memory
	ZeroMemory(m_BlockRects, sizeof(RECT) * num);
	for (i=0, x=0, y=0;i<num;i++, x+=m_BlockWidth)
	{
		if (x > (DWORD)GetWidth() - m_BlockWidth)
		{
			x = 0;
			y+=m_BlockHeight;
		}

		m_BlockRects[i].left = x;
		m_BlockRects[i].top = y;
		m_BlockRects[i].right = x + m_BlockWidth;
		m_BlockRects[i].bottom = y + m_BlockHeight;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////
// Sets the bitmap info and creates a BITMAPINFOHEADER. Used internally.
//////////////////////////////////////////////////////////////////////////////////
static void SetBitmapInfo(long width, long height, BITMAPINFO_256 &info, PBYTE pal)
{
	BITMAPINFOHEADER &h = info.bmiHeader;
	
	h.biSize			= sizeof(info.bmiHeader);
	h.biWidth			= width;
	h.biHeight			= height;
	h.biPlanes			= 1;
	h.biBitCount		= 8;
	h.biCompression		= BI_RGB;
	h.biSizeImage		= ((width +3) & ~3) * height;
	h.biXPelsPerMeter	= 0;
	h.biYPelsPerMeter	= 0;
	h.biClrUsed			= 256; // Max. amount of colors
	h.biClrImportant	= 256; // All colors are important

	if (pal)
	{
		for (int col=0; col<256; col++)
		{
			
			info.bmiColors[col].rgbRed		= pal[0];
			info.bmiColors[col].rgbGreen	= pal[1];
			info.bmiColors[col].rgbBlue		= pal[2];
			info.bmiColors[col].rgbReserved	= 0;

			pal+=3;
		}
	}
	else
	{
		::ZeroMemory(info.bmiColors, sizeof(info.bmiColors));
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Creates a CDX tile file from an autograb/MapMaker tle file
// pScreen points to the CDX main screen, szFilename contains the name
// of the TLE file to load.
//////////////////////////////////////////////////////////////////////////////////
#if 0
CDXBOOL CDXTile::CreateFromTLE(CDXScreen *pScreen, const char* szFilename , BYTE memoryType )
{
	if(szFilename == NULL) 
		return FALSE;

	// So, let's try to load the tle file.
	FILE *tleFile = fopen(szFilename, "rb"); // open for binary read
	WORD count;    // number of bytes stored in the tle file
	WORD width;    // width of each tile
	WORD height;   // height of each tile
	BYTE pal[768]; // the palette used by the tiles

	if (!tleFile)
	{
		return FALSE;
	}
	
	// read the number of tiles and their dimensions
	fread(&count , sizeof(WORD), 1, tleFile);
	fread(&width , sizeof(WORD), 1, tleFile);
	fread(&height, sizeof(WORD), 1, tleFile);

	// read the (dos) palette
	fread(pal, 768, 1, tleFile);

	// read all of the tile data at once
	PBYTE buffer;
	DWORD size = count * width * height;
	DWORD count_read;

	// create a buffer which can handle "size" bytes
	buffer = (PBYTE) malloc(size);
	if (!buffer)
	{
		// uups, no memory
		fclose(tleFile);
		return FALSE;
	}

	// Read "size" bytes
	count_read = fread(buffer, 1, size, tleFile);

	if (count_read != size)
	{
		// Something went wrong. Either a read error,
		// or a wrong file format
		free(buffer);
		fclose(tleFile);
		return FALSE;
	}

	fclose(tleFile); // close TLE file
    
	DWORD image_width    = count * width;
    DWORD image_height   = height;
    LONG  count_per_line = count;
    LONG  count_per_col  = 1;

    DWORD maxWidth = pScreen->GetWidth();
    if (image_width > maxWidth)
    {
        count_per_line = maxWidth / width;
        count_per_col  = (count / count_per_line) +1;
        
        image_width    = count_per_line * width;
        image_height   = count_per_col  * height;
    }

	// Since CDX awaits tile data in a DD surface, let's create one
	// with a height of "height" and a size of ("width" * "count")
	if (FAILED(CDXSurface::Create( pScreen , image_width , image_height , memoryType ))
		return FALSE;
    //m_lpDDS = DDCreateSurface(pScreen->m_lpDD, image_width, image_height, &ddsd);
	if (GetDDS() == NULL)
	{
		free(buffer);
		MessageBox(NULL, "CreateSurface failed", "Dbg", MB_OK);
		return FALSE;
	}
    
	// Now, we create a DIBSection to hold the tiles
	// First step is to set-up the information
	HDC				hDC = ::CreateDC("DISPLAY", NULL, NULL, NULL);
	BITMAPINFO_256	BitmapInfo;	
	UINT			iUsage= DIB_RGB_COLORS;

	SetBitmapInfo(image_width, image_height, BitmapInfo, pal);
	
	// Then to create the DIBSection
	BYTE *bmp_data; // Going to get the buffer
	HBITMAP hBitmap;
	hBitmap = CreateDIBSection(
					hDC,
					(BITMAPINFO*)&BitmapInfo, 
					iUsage, 
					(void **) &bmp_data, 
					NULL, 0);
	
	DeleteDC(hDC);

	// Fill with 0
	FillMemory(bmp_data, ((width +3) & ~3) * image_height, 0);
	
	// Since TLE files store 256 color tiles, we need to check
	// the format of the CDX- screen.
	// If we have less than 8 bpp, we return with an error.
	// If we have 8 bpp, we copy the data
	// If we have more than 8bpp, we use  the palette data to
	//    fill the surface with rgbx values.

	LONG dwLineWidth = (image_width +3) & ~3;
	LONG offset=0;
    LONG btm = (image_height-1) * dwLineWidth;
    LONG line_height = dwLineWidth *height;
    LONG x=0, y=0, foo = count_per_col-1;
	PBYTE surface; 
	PBYTE source = buffer;
    LONG  transfered =count;

	int line;

    for (y=0; y < (count_per_col-1); y++)
    {
        for (x=0, offset=0; x < count_per_line; x++, offset += width)
        {
            surface = bmp_data + btm + offset;
        
            for (line=0; line < height; line++)
		    {
			    memcpy(surface, source, width);
			    source  += width;
                surface -= dwLineWidth;
		    }
            transfered--;
        }
        btm -= line_height;
    }

    for (x=0, offset=0; transfered >0; x++, offset += width, transfered--)
    {
        surface = bmp_data + btm + offset;
    
        for (line=0; line < height; line++)
		{
			memcpy(surface, source, width);
			source  += width;
            surface -= dwLineWidth;
		}
    }
	free(buffer);

	// At this point, we have a DIB holding the
	// tile data, and a surface holding nothing, so
	// let's copy the DIB to the surface
	DDCopyBitmap(GetDDS(), hBitmap, 0, 0, 0, 0);

	// Now, we can free the DIB
	::DeleteObject(hBitmap);

	m_pFilename = NULL;//szFilename;

	Screen = pScreen; 

	m_BlockWidth  = width;
	m_BlockHeight = height;
	m_BlockNum    = count;

	return TRUE;
}
#endif

//////////////////////////////////////////////////////////////////////////////////
// returns the RECT for an individual block
//////////////////////////////////////////////////////////////////////////////////
const RECT *CDXTile::GetBlockRect(int block)
{
 return &m_BlockRects[block];
}

//////////////////////////////////////////////////////////////////////////////////
// Loads the block animation data from the specified file
//////////////////////////////////////////////////////////////////////////////////
CDXBOOL CDXTile::LoadAnim( FILE *fptr )
{
	int i;
	for( i=0; i<m_Count; i++ )
		DELETEARRAY(m_Data[i].anim);

	DELETEARRAY(m_Data);
	m_Count=0;

	if( !feof( fptr ) )
	{	int i;

		fread(&m_Count, sizeof(int), 1, fptr);
		m_Data = new CDX_ANIMSTRUCT[m_Count];

		for( i=0; i<m_Count; i++ )
		{
			fread( &m_Data[i], sizeof( CDX_ANIMSTRUCT ), 1, fptr );
			m_Data[i].anim = new INT32[m_Data[i].num];
			fread( m_Data[i].anim, sizeof(INT32), m_Data[i].num, fptr );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Save the block animation data to the specified file
//////////////////////////////////////////////////////////////////////////////////
CDXBOOL CDXTile::SaveAnim( FILE *fptr )
{
	fwrite(&m_Count, sizeof(int), 1, fptr );

	for( int i=0; i<m_Count; i++ )
	{
		fwrite( &m_Data[i], sizeof( CDX_ANIMSTRUCT ), 1, fptr );
		fwrite( m_Data[i].anim, sizeof(int), m_Data[i].num, fptr );
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//
//	Returns: Tile index for the block or 0 if it fails
//////////////////////////////////////////////////////////////////////////////////
INT32 CDXTile::AddAnim( int framerate, int pause, int rnd, BOOL pingpong, INT32 *anim, int num )
{
	CDX_ANIMSTRUCT *ptr = new CDX_ANIMSTRUCT[m_Count+1];
	CopyMemory( ptr, m_Data, sizeof( CDX_ANIMSTRUCT ) * m_Count );

	ptr[m_Count].framerate = framerate;
	ptr[m_Count].pause = pause;
	ptr[m_Count].rnd = rnd;
	ptr[m_Count].pingpong = pingpong;
	ptr[m_Count].anim = new INT32[num];
	ptr[m_Count].num = num;
	CopyMemory( ptr[m_Count].anim, anim, num*sizeof( INT32 ) );

	DELETEARRAY( m_Data );
	m_Data = ptr;
	m_Count++;

	return m_Count;
}

//////////////////////////////////////////////////////////////////////////////////
//	Func: AddAnimSection
//
//	Purpose: Adds a set of tiles to make an animated tile.  Specify the first
//			 block and then give it the number of total tiles.  All tiles must
//			 be sequential in the file
//
//	Returns: Tile index for the block or 0 if it fails
//////////////////////////////////////////////////////////////////////////////////
INT32 CDXTile::AddAnimSection( int framerate, int pause, int rnd, BOOL pingpong, int blockstart, int numblocks )
{
	INT32 *anim = NULL;
	int i;

	// allocate enough space for the blocks
	anim = new INT32[numblocks];
	if (anim==NULL) return 0;

	// now put the block values in the array
	for (i=0;i<numblocks;i++)
		anim[i] = blockstart + i;

	if (!AddAnim(framerate, pause, rnd, pingpong, anim, numblocks))
	{
		delete [] anim;
		return 0;
	}

	return m_Count;
}

//////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////
CDXBOOL CDXTile::InsertTile( int anim, int pos, int tile )
{
	if( anim>=m_Count || anim<0 )
		return FALSE;

	if( pos>m_Data[anim].num || pos < 0 )
		return FALSE;

	INT32 *ptr = new INT32 [m_Data[anim].num+1];

	if( pos>0 ) CopyMemory( ptr, m_Data[anim].anim, pos*sizeof(INT32) );
	if( pos<m_Data[anim].num ) CopyMemory( &ptr[pos+1], &m_Data[anim].anim[pos], (m_Data[anim].num-pos)*sizeof(INT32) );
	ptr[pos]=tile;
	m_Data[anim].num++;

	SAFEDELETE( m_Data[anim].anim );
	m_Data[anim].anim = ptr;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////
CDXBOOL CDXTile::DelAnim( int pos )
{
	if( pos >= m_Count || pos<0 )
		return FALSE;

	CDX_ANIMSTRUCT *ptr = new CDX_ANIMSTRUCT[m_Count-1];

	if( pos>0 )	CopyMemory( ptr, m_Data, pos*sizeof( CDX_ANIMSTRUCT) );
	if( pos<m_Count-1 ) CopyMemory( &ptr[pos], &m_Data[pos+1], (m_Count-pos)*sizeof(CDX_ANIMSTRUCT) );
	DELETEARRAY(m_Data[pos].anim );
	DELETEARRAY(m_Data);

	m_Count--;
	m_Data=ptr;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////
CDXBOOL CDXTile::RemoveTile( int anim, int pos )
{
	if( anim>=m_Count || anim<0 )
		return FALSE;

	if( pos>=m_Data[anim].num || pos < 0 || m_Data[anim].num==1 )
		return FALSE;

	INT32 *ptr = new INT32 [m_Data[anim].num-1];

	if( pos>0 ) CopyMemory( ptr, m_Data[anim].anim, (pos)*sizeof(INT32) );
	if( pos<m_Data[anim].num ) CopyMemory( &ptr[pos], &m_Data[anim].anim[pos+1], (m_Data[anim].num-pos)*sizeof( INT32 ) );
	
	m_Data[anim].num--;
	m_Data[anim].anim = ptr;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////
INT32 CDXTile::GetAnimTile( int tile )
{
	if( tile>=0 ) return tile;
	else
	{
		// Get anim tile
		tile = (-tile)-1;
		if( tile >= m_Count )
			return 0;

		CDX_ANIMSTRUCT *ptr = &m_Data[tile];

		int pos;
		if( ptr->pingpong )
		{
			int i = (m_FrameCtr/ptr->framerate)%(ptr->num*2-2+ptr->pause);
			if( i<(ptr->num) )	pos = i;
			else
			{	pos = ptr->num-(i-(ptr->num))-1-1;
				if( pos < 0 ) pos=0;
			}
		}
		else
		{	pos = (m_FrameCtr/ptr->framerate)%(ptr->num+ptr->pause);
			if( pos>=ptr->num ) pos=0;
		}

		tile = ptr->anim[pos];
	}

	return tile;
}

//////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////
void CDXTile::Draw( int x, int y, CDXSurface *s, int BlitType, int tile )
{
	// If no tile then just return
	if( tile==0 || tile>=m_BlockNum ) return;

	tile = GetAnimTile( tile );

	switch( BlitType )
	{
		case CDXBLT_BLK:	CDXSurface::DrawBlk(s, x, y, &m_BlockRects[tile]);
							break;
		case CDXBLT_BLKALPHAFAST:
							CDXSurface::DrawBlkAlphaFast(s, x, y, &m_BlockRects[tile]);
							break;
		case CDXBLT_BLKALPHA25:
							CDXSurface::DrawBlkAlpha(s, x, y, &m_BlockRects[tile], 25);
							break;
		case CDXBLT_TRANS:	CDXSurface::DrawTrans(s, x, y, &m_BlockRects[tile]);
							break;
		case CDXBLT_TRANSALPHAFAST:
							CDXSurface::DrawTransAlphaFast(s, x, y, &m_BlockRects[tile]);
							break;
		case CDXBLT_TRANSALPHA25:
							CDXSurface::DrawTransAlpha(s, x, y, &m_BlockRects[tile], 25);
							break;
	}
}

//////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////
void CDXTile::DrawFirst( int x, int y, CDXSurface *s, int BlitType, int tile )
{
	// If no tile then just return
	if( tile == 0 ) return;
	
	if( tile < 0 )
	{
		// Get anim tile
		tile = (-tile)-1;
		if( tile >= m_Count ) return;

		tile = m_Data[tile].anim[0];
	}

	Draw( x, y, s, BlitType, tile );
}

//////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////
void CDXTile::NextFrame( void )
{
	m_FrameCtr++;
}

CDX_ANIMSTRUCT *CDXTile::GetAnimData( int nr )
{
	if( nr>=0 && nr<m_Count ) return &m_Data[nr];
	return NULL;
}
