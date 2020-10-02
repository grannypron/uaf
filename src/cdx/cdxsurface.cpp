//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxsurface.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdxsurface.cpp,v $
// Revision 1.13  2013/12/16 01:35:59  cocoaspud
// fixed a few issues pointed out by static analysis tool
//
// Revision 1.12  2013/04/01 14:55:01  stevens-madison
// Working towards Version 1.0.....
// First 'commit' since sourceforge changed URLs.
// Fingers crossed.
//
// Revision 1.11  2011/02/14 21:32:41  stevens-madison
// Advance towards version 1.0
//
// Revision 1.10  2009/06/13 00:44:00  stevens-madison
// Temple Caster fix.  Revised method of initiating OnDraw.
//
// Revision 1.9  2009/04/17 23:14:07  stevens-madison
// Turn Undead bug and a couple of changes for GameRules.h and cn_Icon11.png
//
// Revision 1.8  2002/02/25 22:48:18  cocoaspud
// merge Richard and Paul changes
//
// Revision 1.6  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.14  2000/09/08 18:29:49  istan
// Fixed some BCB compile problem in the TextureMap function
//
// Revision 1.13  2000/08/21 21:22:02  mindcry
// Removed all fstreams and exchanged them with CDXFile. I had to do this
// cause fstream wasn't working right with MFC apps/games and also in some
// circumstances it crashed or not opened the file. Sorry for breaking
// backwards compatibility in some ways. But it was necessary.
//
// Revision 1.12  2000/08/20 15:45:24  mindcry
// FillCircle has been fixed, thanks to Mark Trueman
//
// Revision 1.11  2000/08/20 13:50:08  mindcry
// Removed a damn bug that keept CDX crashing in some circumstances
//
// Revision 1.10  2000/08/18 14:53:28  mindcry
// Fixed drawing with VLine32 and HLine32
//
// Revision 1.9  2000/08/03 21:06:24  mindcry
// Minor fix in TextXY, it crashed when a NULL Pointer was used
//
// Revision 1.8  2000/06/03 16:21:28  hebertjo
// Added clipping to the FillRect() function.
//
// Revision 1.7  2000/06/02 21:55:00  hebertjo
// Fixed the loading of BITMAP resources from within an EXE.
//
// Revision 1.6  2000/05/12 22:48:02  hebertjo
// I added RotoZoomimg to CDXSurface and CDXSprite.  I ripped this code from NukeDX.
// IT works and as been debuged by them but it is slow.  CDX definitely needed this
// feature and it did not take long to CDX'ize their code so I added it.  I plan
// on changing this to a faster implementation once I finish it.
//
// Revision 1.5  2000/05/10 23:29:26  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.4  2000/05/07 20:03:07  hebertjo
// Fixed a bug in SaveAsBMP().
//
// Revision 1.3  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.2  2000/04/26 18:45:43  hebertjo
// Made many small changes to fix numerous bugs and some enhancements.
//
// Revision 1.1.1.1  2000/04/22 16:13:25  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.56  2000/04/07 22:55:29  jhebert
// Moved the inline functions from the .cpp file to the .h file.  Release builds
// of Apps were having Linker "LNK2001" Errors because of this.
//
// Revision 2.55  2000/03/24 21:08:38  jhebert
// Changed the DrawStretched() functions to blt at destination lDestY, lDestX.
// These functions use to stretch around the center point.
//
// Revision 2.54  2000/03/24 17:22:58  jhebert
// Fixed a bug in AALine().  It was locking without unlocking
//
// Revision 2.53  2000/03/16 11:23:38  jrosberg
// Fixed DrawBlk to work with all bit depths
//
// Revision 2.52  2000/03/15 00:44:08  jhebert
// Fixed the HLine and VLine functions.  There were cases were they would cause
// drawing outside of the surface.
//
// Revision 2.50  2000/03/06 18:52:50  jhebert
// Updated the CDXSurface::Draw functions so that they accept a pointer to
// a RECT as the srcRect argument.  This allows you to set this to NULL if
// you want to simply use the whole surface as the source.  This affected
// many of the other classes which have all been modified.
//
// Revision 2.49  2000/02/14 16:35:11  jhebert
// Changed the ReLoadBitmap function so that it uses the CDXImage class.  Also,
// added m_MemoryLocation data member to track what type of memory the surface
// was created in.
//
// Revision 2.48  2000/01/26 22:53:51  jhebert
// Fixed the issue with apps crashing on exit due to the surfacelist deleting
// the surfaces.  ReleaseAllSurfaces does NOT destroy CDXSurfaces anymore. This
// will be handled by the user or by DirectDraw.
//
// Revision 2.47  2000/01/25 21:54:02  jhebert
// GetClipRect now returns a RECT*.
//
// Revision 2.46  2000/01/24 02:31:35  jhebert
// Added some safety checks to some of the functions. They test to be sure that
// m_lpDDS (DX Surface is not NULL. If it is it returns a negative value.  So,
// be sure to check those return codes.
//
// Revision 2.45  2000/01/20 23:32:18  jhebert
// Made the FunctionMapper() function public.
//
// Revision 2.44  2000/01/20 20:41:45  jhebert
// Moved the placement of the FunctionMapper() Function.
//
// Revision 2.43  2000/01/20 03:00:13  jhebert
// Removed locking from the primitives PutPixel, PutAAPixel, VLine, and Hline
// as suggested by Michael.
//
// Revision 2.42  2000/01/19 23:12:42  jhebert
// Made changes to use CDXImage.
//
// Revision 2.41  2000/01/12 19:29:55  jhebert
// Added function pointer support for the primitives function pixel, VLine, HLine
// This class it a monster!
//
// Revision 2.40  2000/01/10 03:30:17  jhebert
// Initial checkin.
//
// Changes:
// 	- Privitization of member data
// 	- All functions have a function header
// 	- Added additional Get/Set functions
// 	- Added a reference couter to the Lock/Unlock
// 	- All Draw functions now check for lost surfaces
//
// 	NOTE: The library does NOT compile!
//
//
// $Revision: 1.13 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxsurface.cpp,v 1.13 2013/12/16 01:35:59 cocoaspud Exp $";
#endif

#include "CDX.h"
#include "cdxsurface.h"
#include "cdximage.h"
#include <math.h>
#include <assert.h>

void WriteDebugString(const char *ptext, ... );


#define MAX_PSD_CHANNELS	24

#ifdef __BORLANDC__
#pragma warn -amb
#endif


#ifdef TRACE_TIMER_DETAILS
class DETAIL_TRACE
{
  FILE *f;
public:
  DETAIL_TRACE(void);
  ~DETAIL_TRACE(void);
//  DWORD    m_id[1000];
//  DWORD    m_data[1000];
//  DWORD    m_time[1000];
//  DWORD    m_virtualTime[1000];
//  int      m_nextIndex;
  void     trace (DWORD id, DWORD data);
};
extern DETAIL_TRACE detailTrace;
#endif


#ifdef TRACE_TIMER_DETAILS
#define DETAILTRACE(a,b) detailTrace.trace((a),(b));
#else
#define DETAILTRACE(a,b)
#endif




//////////////////////////////////////////////////////////////////////////////////
// NAME: InitCDXSurface
// 
// PURPOSE: Is called from all constructors to init member variables             
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXSurface::InitCDXSurface( void )
{
	ZeroMemory(&m_DDSD, sizeof(m_DDSD));
	m_lpDDS				 = NULL;
    m_PixelWidth		 = 0;
	m_PixelHeight		 = 0;
	ZeroMemory(&m_FileName, sizeof(m_FileName));
	m_DC				 = NULL;
    m_ColorKey           = 0;
	m_Font				 = NULL;
	m_Screen			 = NULL;
    m_SIMDInstrctionSet	 = IsMMX();
	ZeroMemory(&m_ClipRect, sizeof(m_ClipRect));
    ZeroMemory(&m_PixelFormat, sizeof(m_PixelFormat));
    ZeroMemory(&m_FontName, sizeof(m_FontName));
    m_TextureEnabled	 = FALSE;
    m_AlphaMask			 = NULL;
    m_Next               = NULL;
    m_Prev               = NULL;
    m_LockCount          = 0;
    m_DeviceContextCount = 0;
    m_MemoryLocation     = CDXMEM_SYSTEMONLY;
    m_SurfaceId          = -1;

    m_leftTable = NULL;
    m_rightTable = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: FunctionMapper
// 
// PURPOSE: Based on the number of bits per pixel this function maps the correct
//          Drawing/Plotting protected function to it's private function pointer.
//          All constructors MUST call this function, otherwise you will have 
//          function pointer that point to nothing!           
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXSurface::FunctionMapper(void)
{
    RGBFORMAT pixelFormat;

    GetPixelFormat(&pixelFormat);

    switch(pixelFormat.bpp)
    {
    case 8:
        m_fpPutPixel = &CDXSurface::PutPixel8;
	    m_fpPutAAPixel = &CDXSurface::PutAAPixel8;
	    m_fpGetPixel   = &CDXSurface::GetPixel8;
	    m_fpVLine      = &CDXSurface::VLine8;
	    m_fpHLine      = &CDXSurface::HLine8;
        break;

    case 15:
        m_fpPutPixel   = &CDXSurface::PutPixel16;
	    m_fpPutAAPixel = &CDXSurface::PutAAPixel15;
	    m_fpGetPixel   = &CDXSurface::GetPixel16;
	    m_fpVLine      = &CDXSurface::VLine16;
	    m_fpHLine      = &CDXSurface::HLine16;
        break;

    case 16:
        m_fpPutPixel   = &CDXSurface::PutPixel16;
	    m_fpPutAAPixel = &CDXSurface::PutAAPixel16;
	    m_fpGetPixel   = &CDXSurface::GetPixel16;
	    m_fpVLine      = &CDXSurface::VLine16;
	    m_fpHLine      = &CDXSurface::HLine16;
        break;

    case 24:
        m_fpPutPixel   = &CDXSurface::PutPixel24;
	    m_fpPutAAPixel = &CDXSurface::PutAAPixel24;
	    m_fpGetPixel   = &CDXSurface::GetPixel24;
	    m_fpVLine      = &CDXSurface::VLine24;
	    m_fpHLine      = &CDXSurface::HLine24;
        break;

    case 32:
        m_fpPutPixel   = &CDXSurface::PutPixel32;
	    m_fpPutAAPixel = &CDXSurface::PutAAPixel32;
	    m_fpGetPixel   = &CDXSurface::GetPixel32;
	    m_fpVLine      = &CDXSurface::VLine32;
	    m_fpHLine      = &CDXSurface::HLine32;
        break;

    default:
        m_fpPutPixel   = NULL;
	    m_fpPutAAPixel = NULL;
	    m_fpGetPixel   = NULL;
	    m_fpVLine      = NULL;
	    m_fpHLine      = NULL;
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Default Constructor
// 
// PURPOSE: Create Object            
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
CDXSurface::CDXSurface(void)
{
    // Initilize the data
	InitCDXSurface();

    // Map the function pointers
    FunctionMapper();
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Copy Constructor
// 
// PURPOSE: Used to make a copy of a CDXSurface object           
//
// INPUT: aSurface - reference to a CDXScreen Object
//
// RETURNS: nothing
//
// Usage: (assuming that pOriginal is a pointer to a CDXSurface)
//	   CDXSurface copy(*pOriginal);
//	   CDXSurface* pCopy = new CDXSurface(*pOriginal);
//////////////////////////////////////////////////////////////////////////////////
CDXSurface::CDXSurface(CDXSurface& aSurface)					
{
	HRESULT rval;
    RECT srcRect;

	InitCDXSurface();

	// Copy all member variables
	m_ClipRect		     = aSurface.m_ClipRect;
	m_ColorKey		     = aSurface.m_ColorKey;
	m_DC			     = aSurface.m_DC;		//!!??!! How do we handle copying of handles !!??!!
	m_DDSD			     = aSurface.m_DDSD;		 // All settings including if sysmem or vidmem
	m_Screen		     = aSurface.m_Screen;
	m_SIMDInstrctionSet	 = aSurface.m_SIMDInstrctionSet;
    strcpy_s(m_FileName, 256, aSurface.m_FileName);
	m_PixelHeight	     = aSurface.m_PixelHeight;
	m_PixelWidth	     = aSurface.m_PixelWidth;
	m_PixelFormat	     = aSurface.m_PixelFormat;
	m_TextureEnabled	 = aSurface.m_TextureEnabled;
    m_MemoryLocation     = aSurface.m_MemoryLocation;

	if(aSurface.m_AlphaMask != NULL)
	{
		m_AlphaMask = new BYTE[m_PixelWidth * m_PixelHeight];

		for(int i=0; i<m_PixelWidth * m_PixelHeight; i++)
			m_AlphaMask[i] = aSurface.m_AlphaMask[i];
	}

	// Copy the font
	LOGFONT FontInfo;
	GetObject( aSurface.m_Font , sizeof( FontInfo ) , &FontInfo );
	m_Font = CreateFontIndirect( &FontInfo );

	// Create a dd Surface
	rval = GetScreen()->GetDD()->CreateSurface(&m_DDSD, &m_lpDDS, NULL);
	if(rval != DD_OK)
	{
		return;
	}
	else
	{
		// Copy surface content to us by a simple call to Draw
        srcRect.left   = 0;
        srcRect.top    = 0;
        srcRect.right  = aSurface.GetWidth();
        srcRect.bottom = aSurface.GetHeight();

        aSurface.DrawBlk(this, 0, 0, &srcRect);

		// add surface to Screen SurfaceList
		GetScreen()->AddSurfaceToList( this );
        m_SurfaceId = GetScreen()->GetNextId();
	}
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: Destrctor
// 
// PURPOSE: Destroys the surface and frees the memory.           
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
CDXSurface::~CDXSurface()
{
	CDXLOG( "CDXSurface::~CDXSurface" );

	if(m_Font != NULL) 
		DeleteObject(m_Font);

	if(m_DC != NULL) 
		ReleaseDC();

	if (m_AlphaMask)
		delete [] m_AlphaMask;

	RELEASE(m_lpDDS);

	// release surface from Screen SurfaceList
	if( m_Screen )
		m_Screen->ReleaseSurfaceFromList( this );
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Create
// 
// PURPOSE: Creates a empty surface with a size set by Width and Height. 
//          MemoryType is where you want the surface to reside (video or system memory)           
//
// INPUT: pScreen    - pointer to a CDXScreen Object
//        Width      - width of new surface in pixels
//        Height     - height of new surface in pixels
//        memoryType - location of surface. Use defines CDXMEM_XXXXX.
//
// RETURNS: TRUE  - function succeeded
//          FALSE - function failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::Create(CDXScreen* pScreen, int Width, int Height, BYTE memoryType)
{
	HRESULT rval;

	ZeroMemory(&m_DDSD, sizeof(m_DDSD));
	m_DDSD.dwSize  = sizeof(m_DDSD);
	m_DDSD.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;

	switch(memoryType)  
    {
	    case CDXMEM_SYSTEMONLY:
	        m_DDSD.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			break;

		case CDXMEM_VIDEOONLY:
			m_DDSD.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
			break;
		
        case CDXMEM_VIDTHENSYS:
		default:
	        m_DDSD.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
			break;
	}

    if( m_TextureEnabled == TRUE ) 
        m_DDSD.ddsCaps.dwCaps |= DDSCAPS_TEXTURE;

	m_DDSD.dwWidth  = Width;
	m_DDSD.dwHeight = Height;

	rval = pScreen->GetDD()->CreateSurface(&m_DDSD, &m_lpDDS, NULL);
	if( FAILED(rval) )
		return rval;

    m_Screen          = pScreen;
	m_PixelWidth      = Width;
	m_PixelHeight     = Height;
    m_MemoryLocation  = (DWORD)memoryType;
	m_ClipRect.top    = 0;
	m_ClipRect.left   = 0;
	m_ClipRect.bottom = m_PixelHeight;
	m_ClipRect.right  = m_PixelWidth;

	GetRGBFormat(GetDDS(), &m_PixelFormat);
	m_SIMDInstrctionSet = IsMMX();

	// add surface to Screen SurfaceList
	GetScreen()->AddSurfaceToList( this );
    m_SurfaceId = GetScreen()->GetNextId();
    
    // Map the function pointers
    FunctionMapper();

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Create
// 
// PURPOSE: Creates a surface holding the bitmap pointed to by szFilename.  
//          MemoryType is where you want the surface to reside (video or system memory)           
//
// INPUT: pScreen    - pointer to a CDXScreen Object
//        szFilename - string containing the name of the file or resource to load
//        memoryType - location of surface. Use defines CDXMEM_XXXXX.
//
// RETURNS: TRUE  - function succeeded
//          FALSE - function failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::Create(CDXScreen* pScreen, const char* szFilename, BYTE memoryType)
{
    CDXImage* image;
    HRSRC hResInfo;
	HGLOBAL hResData;
	void *pvRes;

    // If the filename was not specified return FALSE
	if(szFilename == NULL) 
		return -1;

    // Create the CDXImage object
    image = new CDXImage();

    // Load the file to a DX surface
    m_lpDDS = image->GetImage(pScreen->GetDD(), szFilename, memoryType, CDXIMAGE_UNKNOWN);

    // Delete the CDXImage object
    delete image;

    // Check if the creation was successful
	if(m_lpDDS == NULL) 
    {
        // Check if this is a resource.
	    if((( hResInfo  = FindResource(NULL, szFilename, RT_BITMAP)) == NULL ) ) return -2;
	    if((  hResData  = LoadResource(NULL, hResInfo)) == NULL) return -3;
	    if((  pvRes     = LockResource(hResData)) == NULL) return -4;
	    
        // Create the CDXImage object
        //image = new CDXImage();

	    //m_lpDDS = image->GetImage(pScreen->GetDD(),0 ,(CHAR*)pvRes, memoryType, CDXIMAGE_UNKNOWN);

        // Delete the CDXImage object
        //delete image;

        // See if this is a resource in the executable
        m_lpDDS = DDLoadSizeBitmap(pScreen->GetDD(), szFilename, &m_PixelWidth, &m_PixelHeight, memoryType);

        // If still NULL return error.
        if(m_lpDDS == NULL) 
		    return -1;
    }

    // Locking the surface fills out the DX surface descriptor
    Lock();
    UnLock();

    // Set some internal data members
    m_PixelWidth      = m_DDSD.dwWidth;
    m_PixelHeight     = m_DDSD.dwHeight;
	m_Screen          = pScreen;
	m_ClipRect.top    = 0;
	m_ClipRect.left   = 0;
	m_ClipRect.bottom = m_PixelHeight;
	m_ClipRect.right  = m_PixelWidth;
    m_MemoryLocation  = (DWORD)memoryType;
    strcpy_s(m_FileName, 256, szFilename);
	GetRGBFormat(m_lpDDS,&m_PixelFormat);
	m_SIMDInstrctionSet = IsMMX();

	// add surface to Screen SurfaceList
    GetScreen()->AddSurfaceToList( this );
    m_SurfaceId = GetScreen()->GetNextId();
    
    // Map the function pointers
    FunctionMapper();

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Create
// 
// PURPOSE: Creates a surface holding the bitmap pointed to by the file stream pointer fs.  
//          MemoryType is where you want the surface to reside (video or system memory)           
//
// INPUT: pScreen    - pointer to a CDXScreen Object
//        fs         - file stream pointer.
//        lSize      - length of the file in bytes.
//        memoryType - location of surface. Use defines CDXMEM_XXXXX.
//
// RETURNS: TRUE  - function succeeded
//          FALSE - function failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::Create(CDXScreen* pScreen, LONG lSize, CDXFile* fs, BYTE memoryType)
{
    CDXImage* image;

    // If the fstream was not specified return FALSE
	if(fs == NULL) 
		return -1;

    // Create the CDXImage object
    image = new CDXImage();

    // Load the file to a DX surface
    m_lpDDS = image->GetImage(pScreen->GetDD(),lSize, fs, memoryType, CDXIMAGE_UNKNOWN);

    // Delete the CDXImage object
    delete image;

    // Check if the creation was successful
	if(m_lpDDS == NULL) 
		return -1;

    // Locking the surface fills out the DX surface descriptor
    Lock();
    UnLock();

    // Set some internal data members
    m_PixelWidth      = m_DDSD.dwWidth;
    m_PixelHeight     = m_DDSD.dwHeight;
	m_Screen          = pScreen;
	m_ClipRect.top    = 0;
	m_ClipRect.left   = 0;
	m_ClipRect.bottom = m_PixelHeight;
	m_ClipRect.right  = m_PixelWidth;
    m_MemoryLocation  = (DWORD)memoryType;
	GetRGBFormat(m_lpDDS,&m_PixelFormat);
	m_SIMDInstrctionSet = IsMMX();

	// add surface to Screen SurfaceList
    GetScreen()->AddSurfaceToList( this );
    m_SurfaceId = GetScreen()->GetNextId();
    
    // Map the function pointers
    FunctionMapper();

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Create
// 
// PURPOSE: Creates a surface holding the bitmap pointed to by the memory pointer lpCache.  
//          MemoryType is where you want the surface to reside (video or system memory)           
//
// INPUT: pScreen    - pointer to a CDXScreen Object
//        lpCache    - pointer to memory containing the file.
//        lSize      - length of the file in bytes.
//        memoryType - location of surface. Use defines CDXMEM_XXXXX.
//
// RETURNS: TRUE  - function succeeded
//          FALSE - function failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::Create(CDXScreen* pScreen, LONG lSize, CHAR* lpCache, BYTE memoryType)
{
    CDXImage* image;
    CHAR* cache;

    // If the pointer was not specified return FALSE
	if(lpCache == NULL) 
		return -1;

    // Set the data pointer to a local variable.
    cache = lpCache;

    // Create the CDXImage object
    image = new CDXImage();

    // Load the file to a DX surface
    m_lpDDS = image->GetImage(pScreen->GetDD(), lSize, cache, memoryType, CDXIMAGE_UNKNOWN);

    // Delete the CDXImage object
    delete image;

    // Check if the creation was successful
	if(m_lpDDS == NULL) 
		return -1;

    // Locking the surface fills out the DX surface descriptor
    Lock();
    UnLock();

    // Set some internal data members
    m_PixelWidth      = m_DDSD.dwWidth;
    m_PixelHeight     = m_DDSD.dwHeight;
	m_Screen          = pScreen;
	m_ClipRect.top    = 0;
	m_ClipRect.left   = 0;
	m_ClipRect.bottom = m_PixelHeight;
	m_ClipRect.right  = m_PixelWidth;
    m_MemoryLocation  = (DWORD)memoryType;
	GetRGBFormat(m_lpDDS,&m_PixelFormat);
	m_SIMDInstrctionSet = IsMMX();

	// add surface to Screen SurfaceList
    GetScreen()->AddSurfaceToList( this );
    m_SurfaceId = GetScreen()->GetNextId();
    
    // Map the function pointers
    FunctionMapper();

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Create
// 
// PURPOSE: Creates a CDXSurface from a 24BPP BMP file. The file must be already 
//          opened and the position set. (fpos). The file will be left opened, 
//          but the position will change. With this function, you can have many 
//          bmp files in one large file.           
//
// INPUT: pScreen    - pointer to a CDXScreen Object
//        fh         - pointer to a FILE
//        size       - size of the image in bytes
//        memoryType - location of surface. Use defines CDXMEM_XXXXX.
//
// RETURNS: TRUE  - function succeeded
//          FALSE - function failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::Create(CDXScreen* pScreen, LONG lSize, FILE* fh, BYTE memoryType)
{
    CDXImage* image;

    // If the pointer was not specified return FALSE
	if(fh == NULL) 
		return -1;

    // Create the CDXImage object
    image = new CDXImage();

    // Load the file to a DX surface
    m_lpDDS = image->GetImage(pScreen->GetDD(), lSize, fh, memoryType, CDXIMAGE_UNKNOWN);

    // Delete the CDXImage object
    delete image;

    // Check if the creation was successful
	if(m_lpDDS == NULL) 
		return -1;

    // Locking the surface fills out the DX surface descriptor
    Lock();
    UnLock();

    // Set some internal data members
    m_PixelWidth      = m_DDSD.dwWidth;
    m_PixelHeight     = m_DDSD.dwHeight;
	m_Screen          = pScreen;
	m_ClipRect.top    = 0;
	m_ClipRect.left   = 0;
	m_ClipRect.bottom = m_PixelHeight;
	m_ClipRect.right  = m_PixelWidth;
    m_MemoryLocation  = (DWORD)memoryType;
	GetRGBFormat(m_lpDDS,&m_PixelFormat);
	m_SIMDInstrctionSet = IsMMX();

	// add surface to Screen SurfaceList
    GetScreen()->AddSurfaceToList( this );
    m_SurfaceId = GetScreen()->GetNextId();
    
    // Map the function pointers
    FunctionMapper();

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: LoadAlphaMask
// 
// PURPOSE: This function can be used to load an alpha mask for your surface.
//          The file format that this function expects is:
//
//          <width><height><alpha values>
//
//          width        : must match the width of the surface, 4 bytes long (DWORD)
//          height       : must match the height of the surface, 4 bytes long (DWORD)
//          alpha values : must be width*height bytes long. One byte value (0..255)
//                         for each pixel on the surface.        
//
// INPUT: szFilename - string that holds the name of the file to load
//
// RETURNS: TRUE  - function succeeded
//          FALSE - function failed
//
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXSurface::LoadAlphaMask(const char* szFilename)
{
	FILE* fhMask;
	LONG width, height;
	DWORD byteCount;
	DWORD i;

	// check if a mask is already loaded
	if (m_AlphaMask != NULL)
		return FALSE;
	
	// Open the alpha mask file for reading
	//if ( (fhMask = fopen (szFilename, "rb")) == NULL)
	fhMask = NULL;
	fopen_s(&fhMask, szFilename, "rb");
			if (fhMask == NULL) return FALSE;

	// Read in the width and height
	fread(&width,sizeof(LONG),1,fhMask);
	fread(&height,sizeof(LONG),1,fhMask);

	if ( (width != m_PixelWidth) || (height != m_PixelHeight) )
	{
		fclose(fhMask);
		return FALSE;
	}

	byteCount = width * height;

	// allocate the space for the mask
	if ( (m_AlphaMask = new BYTE[byteCount]) == NULL)
	{
		fclose(fhMask);
		return FALSE;
	}

	// read in alpha mask
	for (i=0; i<byteCount; i++)
		m_AlphaMask[i] = fgetc(fhMask);

	fclose(fhMask);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetAlphaMaskPointer
// 
// PURPOSE:  Set the pointer for the AlphaMask.  If the pointer is not NULL 
//           the data is first deleted.
//
// INPUT: lpAlpha - pointer to new alpha mask
//
// RETURNS:  0 - successful
//          -1 - color bpp is greater then 8 bits
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::SetAlphaMaskPointer(BYTE* lpAlpha)
{
    if(m_AlphaMask != NULL)
        delete [] m_AlphaMask;

    m_AlphaMask = lpAlpha;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: InvertColors
// 
// PURPOSE: Inverts the colors of the passed in area       
//
// INPUT: x1 - first x location
//        y1 - first y location
//        x2 - second x location
//        y2 - second y location
//
// RETURNS:  0 - successful
//          -1 - color bpp is greater then 8 bits
//
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::InvertColors( int x1, int y1, int x2, int y2 )
{
    int x, y, tmp;
    int bpp;
    int pitch;
    BYTE* ptr;

    // If the color bit depth is > 8 return
    if(m_PixelFormat.bpp > 8)
        return -1;

    // Orient the values
    if(x1 > x2)
    {
        tmp = x2;
        x2 = x1;
        x1 = tmp;
    }

    if(y1 > y2)
    {
        tmp = y2;
        y2 = y1;
        y1 = tmp;
    }

	// Clip if bigger then the m_ClipRect
	if( x1 < m_ClipRect.left ) x1 = m_ClipRect.left;
	if( y1 < m_ClipRect.top ) y1 = m_ClipRect.top;
	if( x2 >= m_ClipRect.right ) x2 = m_ClipRect.right - 1;
	if( y2 >= m_ClipRect.bottom ) y2 = m_ClipRect.bottom - 1;

    // Lock surface for read and write and setup the variables
    Lock();

	bpp   = (m_PixelFormat.bpp+1) / 8;
	ptr   = (BYTE*)GetSurfacePointer() + (y1 * GetPitch()) + x1 * bpp;
	pitch = m_DDSD.lPitch - ((x2 - x1) * bpp);

	// Main loop
	for( y=y1; y<y2; y++ )
	{
		for( x=x1; x<x2; x++ )
		{
			for( BYTE bit=0; bit<bpp; bit++ )
			{
				*ptr = 255-*ptr;
				ptr++;
			}
		}
		ptr+=pitch;
	}
	UnLock( );

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawTransAlphaMask
// 
// PURPOSE: Alpha blends a block of pixels to the destination surface pointed to by 
//          'dest'. Pixels that are set to the transparent color of the source 
//          surface are not drawn. In addition this fuction will use the m_AlphaMask
//          for the ALPHA values.       
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: m_AlphaMask is a pointer to an array of unsinged char values that
//       represents the indiviual alpha values for the bitmap loaded into this
//       CDXSurface.  The m_alphaMask array covers the whole surface not just one
//       tile or sprite contained on the surface but for all tiles or sprites.
//       If the m_alphaMask is equal to NULL then the function will call DrawTrans()
//       and draw a simple transparent image.
//
//////////////////////////////////////////////////////////////////////////////////

HRESULT CDXSurface::DrawTransAlphaMask(CDXSurface* dest, LONG lDestX, LONG lDestY, RECT* srcRect)
{
	int register i,j;
	int height,width;
	BYTE* lpSprite;
	BYTE* lpDest;
	unsigned long dPitch, SpritePitch;
	DWORD sColorKey, doubleColorKey;
	DWORD sTemp,dTemp;
	long sr,sg,sb,dr,dg,db;
	WORD sbuf,dbuf,abuf;
	DWORD ALPHA;
	DWORD Result;
	BOOL oddWidth = FALSE;
	BYTE* alphaPtr;
	HRESULT rval;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Point to the alpha channel
	alphaPtr = m_AlphaMask;

	// If m_AlphaMask = NULL then DrawTRANS
	if (alphaPtr == NULL)
	{
		rval = DrawTrans(dest,lDestX, lDestY, &tRect);
		return rval;
	}

	// Validate the inputs and clip them to m_clipRect
	if (ValidateBlt(dest, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

	// Set height and width of SPRITE
    height = tRect.bottom - tRect.top;
    width  = tRect.right - tRect.left; 

    // Get the color key for sprite surface
    sColorKey = (DWORD)m_ColorKey;

    // Lock down both surfaces for read and write
    Lock();
    dest->Lock();

	// Set the pitch for both surfaces
    SpritePitch = GetPitch();
    dPitch      = dest->GetPitch();

    // Initialize the pointers to the upper left hand corner of surface
    lpSprite = (BYTE*)GetSurfacePointer();
    lpDest   = (BYTE*)dest->GetSurfacePointer();


	switch(dest->m_PixelFormat.bpp)
	{
	case 15:
		
		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);
		alphaPtr += (tRect.top * m_PixelWidth) + tRect.left;

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));
		abuf = (WORD)(m_PixelWidth - width);

		doubleColorKey = sColorKey | (sColorKey << 16);

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.

		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);

				if (sTemp != sColorKey)
				{
					dTemp = *((WORD*)lpDest);
					ALPHA = *alphaPtr;

					sb = sTemp & 0x1f;
					db = dTemp & 0x1f;
					sg = (sTemp >> 5) & 0x1f;
					dg = (dTemp >> 5) & 0x1f;
					sr = (sTemp >> 10) & 0x1f;
					dr = (dTemp >> 10) & 0x1f;

					*((WORD*)lpDest) = (WORD)((ALPHA * (sb - db) >> 8) + db |
						((ALPHA * (sg - dg) >> 8) + dg) << 5 |
						((ALPHA * (sr - dr) >> 8) + dr) << 10);


				}

				lpDest   += 2;
				lpSprite += 2;
				alphaPtr++;
			}
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);

				if (sTemp != doubleColorKey )
				{
					dTemp = *((DWORD*)lpDest);
					ALPHA = *((WORD*)alphaPtr);
					Result = dTemp;

					if ((sTemp & 0xffff) != sColorKey)
					{
						sb = sTemp & 0x1f;
						db = dTemp & 0x1f;
						sg = (sTemp >> 5) & 0x1f;
						dg = (dTemp >> 5) & 0x1f;
						sr = (sTemp >> 10) & 0x1f;
						dr = (dTemp >> 10) & 0x1f;

						Result = Result & 0xffff0000;
						Result |= (DWORD)(((ALPHA & 0xFF) * (sb - db) >> 8) + db |
							(((ALPHA & 0xFF) * (sg - dg) >> 8) + dg) << 5 |
							(((ALPHA & 0xFF) * (sr - dr) >> 8) + dr) << 10);
											
					
					}
					if (((sTemp >> 16) & 0xffff) != sColorKey)
					{
						sb = (sTemp >> 16) & 0x1f;
						db = (dTemp >> 16) & 0x1f;
						sg = (sTemp >> 21) & 0x1f;
						dg = (dTemp >> 21) & 0x1f;
						sr = (sTemp >> 26) & 0x1f;
						dr = (dTemp >> 26) & 0x1f;

						Result = Result & 0xffff;
						Result |= (DWORD)( ((ALPHA >> 8) * (sb - db) >> 8) + db |
							(((ALPHA >> 8) * (sg - dg) >> 8) + dg) << 5 |
							(((ALPHA >> 8) * (sr - dr) >> 8) + dr) << 10 ) << 16;
					}

					*((DWORD*)lpDest) = Result;
				}
				lpDest   += 4;
				lpSprite += 4;
				alphaPtr += 2;
			}while (--j > 0);

			lpDest   += dbuf;
			lpSprite += sbuf;
			alphaPtr += abuf;
		}while (--i > 0);

		break;


case 16:

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);
		alphaPtr += (tRect.top * m_PixelWidth) + tRect.left;

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));
		abuf = (WORD)(m_PixelWidth - width);

		doubleColorKey = sColorKey | (sColorKey << 16);

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.


		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);

				if (sTemp != sColorKey)
				{
					dTemp = *((WORD*)lpDest);
					ALPHA = *alphaPtr;

					sb = sTemp & 0x1f;
					db = dTemp & 0x1f;
					sg = (sTemp >> 5) & 0x3f;
					dg = (dTemp >> 5) & 0x3f;
					sr = (sTemp >> 11) & 0x1f;
					dr = (dTemp >> 11) & 0x1f;

					*((WORD*)lpDest) = (WORD)((ALPHA * (sb - db) >> 8) + db |
						((ALPHA * (sg - dg) >> 8) + dg) << 5 |
						((ALPHA * (sr - dr) >> 8) + dr) << 11);
				}

				lpDest   += 2;
				lpSprite += 2;
				alphaPtr++;
			}
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);

				if (sTemp != doubleColorKey )
				{
					dTemp = *((DWORD*)lpDest);
					ALPHA = *((WORD*)alphaPtr);
					Result = dTemp;

					if ((sTemp & 0xffff) != sColorKey)
					{
						sb = sTemp & 0x1f;
						db = dTemp & 0x1f;
						sg = (sTemp >> 5) & 0x3f;
						dg = (dTemp >> 5) & 0x3f;
						sr = (sTemp >> 11) & 0x1f;
						dr = (dTemp >> 11) & 0x1f;

						Result = Result & 0xffff0000;
						Result |= (DWORD)(((ALPHA & 0xFF) * (sb - db) >> 8) + db |
							(((ALPHA & 0xFF) * (sg - dg) >> 8) + dg) << 5 |
							(((ALPHA & 0xFF) * (sr - dr) >> 8) + dr) << 11);

					}
					if (((sTemp >> 16) & 0xffff) != sColorKey)
					{
						sb = (sTemp >> 16) & 0x1f;
						db = (dTemp >> 16) & 0x1f;
						sg = (sTemp >> 21) & 0x3f;
						dg = (dTemp >> 21) & 0x3f;
						sr = (sTemp >> 27) & 0x1f;
						dr = (dTemp >> 27) & 0x1f;

						Result = Result & 0xffff;
						Result |= (DWORD)( ((ALPHA >> 8) * (sb - db) >> 8) + db |
							(((ALPHA >> 8) * (sg - dg) >> 8) + dg) << 5 |
							(((ALPHA >> 8) * (sr - dr) >> 8) + dr) << 11 ) << 16;
					}

					*((DWORD*)lpDest) = Result;
				}
				lpDest   += 4;
				lpSprite += 4;
				alphaPtr += 2;
			}while (--j > 0);

			lpDest   += dbuf;
			lpSprite += sbuf;
			alphaPtr += abuf;
		}while (--i > 0);

		break;
		
	case 24:

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite  += (tRect.top * SpritePitch) + (tRect.left * 3);
		lpDest    += (lDestY * dPitch) + (lDestX * 3);
		alphaPtr  += (tRect.top * m_PixelWidth) + tRect.left;

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (3 * width));
		dbuf = (WORD)(dPitch - (3 * width));
		abuf = (WORD)(m_PixelWidth - width);

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);

				if ((sTemp & 0xFFFFFF) != sColorKey)
				{
					dTemp = *((DWORD*)lpDest);
					ALPHA = *alphaPtr;
					sb = sTemp & 0xFF;
					db = dTemp & 0xFF;
					sg = (sTemp >> 8) & 0xFF;
					dg = (dTemp >> 8) & 0xFF;
					sr = (sTemp >> 16) & 0xFF;
					dr = (dTemp >> 16) & 0xFF;

					Result = (DWORD)((ALPHA * (sb - db) >> 8) + db |
						((ALPHA * (sg - dg) >> 8) + dg) << 8 |
						((ALPHA * (sr - dr) >> 8) + dr) << 16);

					*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
					lpDest += 2;
					*lpDest = (BYTE)(Result >> 16);
					lpDest++;
				}
				else
				{
					lpDest += 3;
				}

				lpSprite += 3;
				alphaPtr++;
			}while (--j > 0);

			lpDest   += dbuf;
			lpSprite += sbuf;
			alphaPtr += abuf;
		}while (--i > 0);

		break;

	case 32:

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite  += (tRect.top * SpritePitch) + (tRect.left * 4);
		lpDest    += (lDestY * dPitch) + (lDestX * 4);
		alphaPtr  += (tRect.top * m_PixelWidth) + tRect.left;

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (4 * width));
		dbuf = (WORD)(dPitch - (4 * width));
		abuf = (WORD)(m_PixelWidth - width);

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);

				if ((sTemp & 0xFFFFFF) != sColorKey)
				{
					dTemp = *((DWORD*)lpDest);
					ALPHA = *alphaPtr;

					sb = sTemp & 0xFF;
					db = dTemp & 0xFF;
					sg = (sTemp >> 8) & 0xFF;
					dg = (dTemp >> 8) & 0xFF;
					sr = (sTemp >> 16) & 0xFF;
					dr = (dTemp >> 16) & 0xFF;

					Result = (DWORD)((ALPHA * (sb - db) >> 8) + db |
						((ALPHA * (sg - dg) >> 8) + dg) << 8 |
						((ALPHA * (sr - dr) >> 8) + dr) << 16);

					*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
					lpDest += 2;
					*lpDest = (BYTE)(Result >> 16);
					lpDest += 2;
				}
				else
				{
					lpDest += 4;
				}

				lpSprite += 4;
				alphaPtr++;
			}while (--j > 0);

			lpDest   += dbuf;
			lpSprite += sbuf;
			alphaPtr += abuf;
		}while (--i > 0);

		break;

        case 8:
        default:
		// IMHO paletized modes are a thing of the past please feel free to 
		// implement this if you so desire.
		break;
	} // End RGB Format switch statement


	UnLock();
	dest->UnLock();

	return 0;
}



//////////////////////////////////////////////////////////////////////////////////
// NAME: ValidateBlt
// 
// PURPOSE: Used to clip both the source and destination rectangles against their
//          associated surface's m_ClipRect.  The source or destination values
//          will be adjusted to comply with the clipping rectangles.      
//
// INPUT: lpCDXS  - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: TRUE  - proceed with the draw operation
//          FALSE - after clipping there is nothing to draw, exit draw operation.
//
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXSurface::ValidateBlt(CDXSurface* lpCDXS, LONG *lDestX, LONG *lDestY, RECT *srcRect)
{
	RECT destRect;
	RECT tmpRect;

	// Test if srcRect lies somewhere on this surface's clipRect
	if (!(ClipRect(srcRect)))
		return FALSE;

	// From srcRect build the destRect
	destRect.top    = *lDestY;
	destRect.left   = *lDestX;
	destRect.bottom = *lDestY + (srcRect->bottom - srcRect->top);
	destRect.right  = *lDestX + (srcRect->right - srcRect->left);

	// Save off the destRect
	tmpRect.top    = destRect.top;
	tmpRect.left   = destRect.left;
	tmpRect.bottom = destRect.bottom;
	tmpRect.right  = destRect.right;

	// Test if destRect lies somewhere on lpCDXS surface's clipRect
	if (!(lpCDXS->ClipRect(&destRect)))
		return FALSE;

	// Compare the returned destRect to the saved tmpRect
	if (destRect.top != tmpRect.top)
		srcRect->top += destRect.top - tmpRect.top;

	if (destRect.left != tmpRect.left)
		srcRect->left += destRect.left - tmpRect.left;

	if (destRect.bottom != tmpRect.bottom)
		srcRect->bottom += destRect.bottom - tmpRect.bottom;

	if (destRect.right != tmpRect.right)
		srcRect->right += destRect.right - tmpRect.right;

	*lDestX = destRect.left;
	*lDestY = destRect.top;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: ClipRect
// 
// PURPOSE: Clips a rectangle specified by Rect to the member variable m_clipRect      
//
// INPUT: Rect - rectangle descirbing the block of pixels to be drawn
//
// RETURNS: TRUE  - proceed with the draw operation
//          FALSE - after clipping there is nothing to draw, exit draw operation.
//
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXSurface::ClipRect(RECT *Rect)
{
	// Check if Rect is completely outside of the m_clipRect (Screen Space)
	if (Rect->top >= m_ClipRect.bottom)
		return FALSE;

	if (Rect->left >= m_ClipRect.right)
		return FALSE;

	if (Rect->bottom <= m_ClipRect.top)
		return FALSE;

	if (Rect->right <= m_ClipRect.left)
		return FALSE;

	// Clip rect to the surface's clipRect
	if (Rect->top < m_ClipRect.top)
		Rect->top = m_ClipRect.top;

	if (Rect->left < m_ClipRect.left)
		Rect->left = m_ClipRect.left;

	if (Rect->bottom > m_ClipRect.bottom)
		Rect->bottom = m_ClipRect.bottom;

	if (Rect->right > m_ClipRect.right)
		Rect->right = m_ClipRect.right;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: SetClipRect
// 
// PURPOSE: Set m_clipRect to the passed in RECT values    
//
// INPUT: clipRect - RECT strcture that contains the new clipping rect values
//
// RETURNS: nothing
//
// NOTE: The clipping rect cannot be large then the current screen size
//
//////////////////////////////////////////////////////////////////////////////////
void CDXSurface::SetClipRect(RECT *clipRect)
{
	if ((m_ClipRect.top = clipRect->top) < 0)
		m_ClipRect.top = 0;

	if ((m_ClipRect.left = clipRect->left) < 0)
		m_ClipRect.left = 0;

	if ((m_ClipRect.bottom = clipRect->bottom) > m_PixelHeight)
		m_ClipRect.bottom = m_PixelHeight;

	if ((m_ClipRect.right = clipRect->right) > m_PixelWidth)
		m_ClipRect.right = m_PixelWidth;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawBlk
// 
// PURPOSE: Draws a block of pixels from this surface to the surface pointed to by
//          lpCDXS.  The transparency color key is ignored.     
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: DirectDraw function BltFast() performs an Lock/Unlock internally.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawBlk(CDXSurface* lpCDXS, LONG lDestX, LONG lDestY, RECT* srcRect)
{
	HRESULT rval;
  RECT tRect, dRect;
	if(m_lpDDS == NULL)	return -1;
    
  if(srcRect == NULL)
  {
      tRect.top    = 0;
      tRect.left   = 0;
      tRect.right  = m_PixelWidth;
      tRect.bottom = m_PixelHeight;
  }
  else
  {
      tRect = *srcRect;
  };
	if (ValidateBlt(lpCDXS, &lDestX, &lDestY, &tRect) == FALSE)
  {
#ifdef DebugFull
    WriteDebugString("!!ValidateBlt() failed\n");
#endif
		return 0;
  };
	
  dRect.left   = lDestX;
  dRect.top    = lDestY;
  dRect.right  = lDestX + tRect.right- tRect.left;
  dRect.bottom = lDestY + tRect.bottom - tRect.top;

	//rval = lpCDXS->m_lpDDS->BltFast(lDestX, lDestY, m_lpDDS, &tRect, DDBLTFAST_WAIT);
	rval = lpCDXS->m_lpDDS->Blt(&dRect, m_lpDDS, &tRect, DDBLT_WAIT, NULL);
#ifdef DebugFull
  {
    DDSURFACEDESC2 sDsrc, sDdst;
    IDirectDrawSurface7 *pDsrcDDS;
    IDirectDrawSurface7 *pDdstDDS;
    sDdst.dwSize = sizeof(sDdst);
    sDsrc.dwSize = sizeof(sDsrc);
    pDsrcDDS = m_lpDDS;
    pDdstDDS = lpCDXS->m_lpDDS;
    pDsrcDDS->GetSurfaceDesc(&sDsrc);
    pDdstDDS->GetSurfaceDesc(&sDdst);
    WriteDebugString("!!dstDDS=0x%08x  width=%d  height=%d  BPP=%d\n",
                       pDdstDDS, sDdst.dwWidth, sDdst.dwHeight, sDdst.ddpfPixelFormat.dwRGBBitCount);
    WriteDebugString("!!srcDDS=0x%08x  width=%d  height=%d  BPP=%d\n",
                       pDsrcDDS, sDsrc.dwWidth, sDsrc.dwHeight, sDsrc.ddpfPixelFormat.dwRGBBitCount);
    WriteDebugString("!!lpCDXS = 0x%08x   m_lpDDS = 0x%08x\n",lpCDXS, lpCDXS->m_lpDDS);
    WriteDebugString("!!dRect = %d %d %d %d\n", dRect.left, dRect.top, dRect.right, dRect.bottom);
    WriteDebugString("!!tRect = %d %d %d %d\n", tRect.left, tRect.top, tRect.right, tRect.bottom);

    WriteDebugString("!!rval = %d = lpCDXS->m_lpDDS->Blt(&dRect, m_lpDDS, &tRect, DDBLT_WAIT, NULL)\n", rval);
  };
#endif
  if (FAILED(rval))

	if(rval == DDERR_SURFACELOST) 
		rval = Restore();

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawBlkSW
// 
// PURPOSE: Draws a block of pixels from this surface to the surface pointed to by
//          lpCDXS.  The transparency color key is ignored.  This function should
//          be used when you are drawing from system memory surfaces to system or
//          video memory surfaces.  The advantage is that you can call Lock() once
//          then make multiple blits.    
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawBlkSW(CDXSurface* lpCDXS, LONG lDestX, LONG lDestY, RECT* srcRect)
{
	HRESULT rval = 0;
    LONG y;
    LONG w, h;
    BYTE* src;
    BYTE* dest;
    DWORD sPitch, dPitch;
    DWORD bytesPerPixel;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	if (ValidateBlt(lpCDXS, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;
	
    // calc width and height 
    w = tRect.right - tRect.left;
    h = tRect.bottom - tRect.top;

	// Lock down both surfaces for read and write
    Lock();
    lpCDXS->Lock();

    // Set the pitch for both surfaces
    sPitch = m_DDSD.lPitch;
    dPitch = lpCDXS->m_DDSD.lPitch;

    // Initialize the pointers to the upper left hand corner of the surface
    src  = (BYTE*)m_DDSD.lpSurface;
    dest = (BYTE*)lpCDXS->m_DDSD.lpSurface;

    // Start RGB Format switch statement
	switch(lpCDXS->m_PixelFormat.bpp)
	{
	case 8:
		bytesPerPixel = 1;
		break;
	case 15:
    case 16:
        bytesPerPixel = 2;
        break;
    case 24:
        bytesPerPixel = 3;
        break;
    case 32:
        bytesPerPixel = 4;
        break;
    }

    // Adjust to beginning of the bitmap
    src  += (tRect.top * sPitch) + tRect.left * bytesPerPixel;
    dest += (lDestY * dPitch) + lDestX * bytesPerPixel;

	 // Use memcpy to transfer the rows of data
    for(y=0; y<h; y++)
    {
        memcpy(dest,src,w*bytesPerPixel);
        src  += sPitch;
        dest += dPitch;
    }

	// UnLock both surfaces 
    UnLock();
    lpCDXS->UnLock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawTrans
// 
// PURPOSE: Draws a block of pixels from this surface to the surface pointed to by
//          lpCDXS.  Pixels matching the transparency color key are not drawn.     
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: DirectDraw function BltFast() performs an Lock/Unlock internally.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawTrans(CDXSurface* lpCDXS, LONG lDestX, LONG lDestY, RECT* srcRect)
{
	HRESULT rval;
    RECT tRect, dRect;

	if(m_lpDDS == NULL)
		return -1;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	if (ValidateBlt(lpCDXS, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;
	dRect.left = lDestX;
  dRect.top  = lDestY;
  dRect.right = dRect.left + tRect.right - tRect.left;
  dRect.bottom = dRect.top + tRect.bottom - tRect.top;
	//rval = lpCDXS->m_lpDDS->BltFast(lDestX, lDestY, m_lpDDS, &tRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
 	rval = lpCDXS->m_lpDDS->Blt(&dRect, m_lpDDS, &tRect, DDBLT_WAIT | DDBLT_KEYSRC  , NULL);
	if(rval == DDERR_SURFACELOST) 
		rval = Restore();


  // I have no idea why we get the INVALIDPARAMS status occasionally.
  // Everything works OK.  The BltFast got no such error, as I recall.
  // Oh, well, let's just ignore it.  PRS 19 May 2009
  if (rval == DDERR_INVALIDPARAMS)
  {
    rval = 0;
  };
	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawTransSW
// 
// PURPOSE: Draws a block of pixels from this surface to the surface pointed to by
//          lpCDXS.  Pixels matching the transparency color key are not drawn.This 
//          function should be used when you are drawing from system memory surfaces
//          to system or video memory surfaces.  The advantage is that you can call 
//          Lock() once then make multiple blits.         
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: DirectDraw function BltFast() performs an Lock/Unlock internally.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawTransSW(CDXSurface* lpCDXS, LONG lDestX, LONG lDestY, RECT* srcRect)
{
	HRESULT rval = 0;
	LONG i,j;
    LONG width, height;
    BYTE* src;
    BYTE* dest;
	DWORD sTemp;
    DWORD sPitch, dPitch;
    DWORD sbuf,dbuf;
    DWORD sColorKey;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	if (ValidateBlt(lpCDXS, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;
	
    // Blt using memcpy
    width  = tRect.right - tRect.left;
    height = tRect.bottom - tRect.top;

	// Lock down both surfaces for read and write
    Lock();
    lpCDXS->Lock();

    // Set the pitch for both surfaces
    sPitch = m_DDSD.lPitch;
    dPitch = lpCDXS->m_DDSD.lPitch;

    // Initialize the pointers to the upper left hand corner of the surface
    src  = (BYTE*)m_DDSD.lpSurface;
    dest = (BYTE*)lpCDXS->m_DDSD.lpSurface;

    // ColorKey
    sColorKey = m_ColorKey;

	// Start RGB Format switch statement
	switch(m_PixelFormat.bpp)
	{
	case 8:
		// Initialize the pointers to the first pixel in the rectangle
		src  += (tRect.top * sPitch) + tRect.left;
		dest += (lDestY * dPitch) + lDestX;

		// Set the horizontal padding
		sbuf = (WORD)(sPitch - width);
		dbuf = (WORD)(dPitch - width);

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *src;

				if (sTemp != sColorKey)
				    *dest = (BYTE)sTemp;

				src++;
				dest++;

			}while (--j > 0);

			src  += sbuf;
			dest += dbuf;

		}while (--i > 0);
		break;

	case 15:

		// Initialize the pointers to the first pixel in the rectangle
		src += (tRect.top * sPitch) + (tRect.left * 2);
		dest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(sPitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((WORD*)src);

				if (sTemp != sColorKey)
				    *((WORD*)dest) = (WORD)sTemp;

				dest   += 2;
				src += 2;

			}while (--j > 0);

			dest   += dbuf;
			src += sbuf;

		}while (--i > 0);

		break;

	case 16:

		// Initialize the pointers to the first pixel in the rectangle
		src  += (tRect.top * sPitch) + (tRect.left * 2);
		dest += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(sPitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((WORD*)src);

				if (sTemp != sColorKey)
					*((WORD*)dest) = (WORD)sTemp;

				dest += 2;
				src  += 2;

			}while (--j > 0);

			dest += dbuf;
			src  += sbuf;

		}while (--i > 0);

		break;

	case 24:
		// Initialize the pointers to the first pixel in the rectangle
		src  += (tRect.top * sPitch) + (tRect.left * 3);
		dest += (lDestY * dPitch) + (lDestX * 3);

		// Set the horizontal padding
		sbuf = (WORD)(sPitch - (3 * width));
		dbuf = (WORD)(dPitch - (3 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)src);

				if ((sTemp &= 0xFFFFFF) != sColorKey)
				{
					*((WORD*)dest) = (WORD)(sTemp & 0xFFFF);
					dest += 2;
					*dest = (BYTE)((sTemp >> 16) & 0xFF);
					dest++;
				}
				else
				{
					dest += 3;
				}

				src += 3;
			}while (--j > 0);

			dest += dbuf;
			src  += sbuf;

		}while (--i > 0);
		break;

	case 32:
		// Initialize the pointers to the first pixel in the rectangle
		src  += (tRect.top * sPitch) + (tRect.left * 4);
		dest += (lDestY * dPitch) + (lDestX * 4);

		// Set the horizontal padding
		sbuf = (WORD)(sPitch - (4 * width));
		dbuf = (WORD)(dPitch - (4 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)src);

				if ((sTemp &= 0xFFFFFF) != sColorKey)
				{
					*((DWORD*)dest) = sTemp;
				}

				dest += 4;
				src  += 4;

			}while (--j > 0);

			dest += dbuf;
			src  += sbuf;

		}while (--i > 0);
		break;
	} // End RGB Format switch statement


	// UnLock both surfaces 
    UnLock();
    lpCDXS->UnLock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawBlkHFlip
// 
// PURPOSE: Draws a block of pixels flipped horizontally from this surface to the 
//          surface pointed to by lpCDXS.  The transparency color key is ignored.     
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: DirectDraw function BltFast() performs an Lock/Unlock internally.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawBlkHFlip(CDXSurface* lpCDXS, LONG lDestX, LONG lDestY, RECT* srcRect)
{
	HRESULT rval;
	DDBLTFX ddBltFx;
	RECT destRect;
	DWORD Pixel;
	LONG width, height;
	LONG tmpLeft, tmpRight;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	tmpLeft  = lDestX;  // equal to or smaller then lDestX
	tmpRight = tRect.right;

	if(m_lpDDS == NULL)
		return -1;

	if (ValidateBlt(lpCDXS, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

	if (tmpLeft != lDestX)
	{
		tRect.left  -= lDestX - tmpLeft;
		tRect.right -= lDestX - tmpLeft;
		tmpRight    -= lDestX - tmpLeft;
	}

	if (tmpRight != tRect.right)
	{
		tRect.left += tmpRight - tRect.right;
		tRect.right += tmpRight - tRect.right;
	}

	ddBltFx.dwSize = sizeof(DDBLTFX);
	ddBltFx.dwDDFX = DDBLTFX_MIRRORLEFTRIGHT;

	width           = tRect.right - tRect.left;
	height          = tRect.bottom - tRect.top;
	destRect.top    = lDestY;
	destRect.left   = lDestX;
	destRect.bottom = lDestY + height;
	destRect.right  = lDestX + width;

	rval = lpCDXS->m_lpDDS->Blt(&destRect, m_lpDDS, &tRect, DDBLT_DDFX | DDBLT_WAIT, &ddBltFx);

	if(rval == DDERR_SURFACELOST)
	{
		rval = Restore();
		return rval;
	}
	else if ( SUCCEEDED(rval) )
	{
		return rval;
	}
	else
	{
		Lock();
		lpCDXS->Lock();

		for(int j = 0; j < height; j++)
		{
			for(int i = 0; i < width; i++)
			{
				Pixel = GetPixel(tRect.left + i, tRect.top + j);
				lpCDXS->PutPixel((destRect.left + width) - i, destRect.top + j, Pixel);
			}
		}

		UnLock();
		lpCDXS->UnLock();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawTransHFlip
// 
// PURPOSE: Draws a block of pixels flipped horizontally from this surface to the 
//          surface pointed to by lpCDXS.  Pixels matching the transparency color 
//          key are not drawn.      
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: DirectDraw function BltFast() performs an Lock/Unlock internally.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawTransHFlip(CDXSurface* lpCDXS, LONG lDestX, LONG lDestY, RECT* srcRect)
{
	HRESULT rval;
	DDBLTFX ddBltFx;
	RECT destRect;
	DWORD Pixel;
	LONG width, height;
	LONG tmpLeft, tmpRight;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	tmpLeft  = lDestX;  // equal to or smaller then lDestX
	tmpRight = tRect.right;

	if(m_lpDDS == NULL)
		return -1;

	if (ValidateBlt(lpCDXS, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

	if (tmpLeft != lDestX)
	{
		tRect.left  -= lDestX - tmpLeft;
		tRect.right -= lDestX - tmpLeft;
		tmpRight    -= lDestX - tmpLeft;
	}

	if (tmpRight != tRect.right)
	{
		tRect.left  += tmpRight - tRect.right;
		tRect.right += tmpRight - tRect.right;
	}

	ddBltFx.dwSize = sizeof(DDBLTFX);
	ddBltFx.dwDDFX = DDBLTFX_MIRRORLEFTRIGHT;

	width           = tRect.right - tRect.left;
	height          = tRect.bottom - tRect.top;
	destRect.top    = lDestY;
	destRect.left   = lDestX;
	destRect.bottom = lDestY + height;
	destRect.right  = lDestX + width;

	rval = lpCDXS->m_lpDDS->Blt(&destRect, m_lpDDS, &tRect, DDBLT_DDFX | DDBLT_WAIT | DDBLT_KEYSRC, &ddBltFx);
	
	if(rval == DDERR_SURFACELOST)
	{
		rval = Restore();
		return rval;
	}
	else if ( SUCCEEDED(rval) )
	{
		return rval;
	}
	else
	{
		Lock();
		lpCDXS->Lock();

		for(int j = 0; j < height; j++)
		{
			for(int i = 0; i < width; i++)
			{
				Pixel = GetPixel(tRect.left + i, tRect.top + j);
				if (Pixel != m_ColorKey)
					lpCDXS->PutPixel((destRect.left + width) - i, destRect.top + j, Pixel);
			}
		}

		UnLock();
		lpCDXS->UnLock();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawBlkHFlip
// 
// PURPOSE: Draws a block of pixels flipped vertically from this surface to the 
//          surface pointed to by lpCDXS.  The transparency color key is ignored.     
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: DirectDraw function BltFast() performs an Lock/Unlock internally.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawBlkVFlip(CDXSurface* lpCDXS, LONG lDestX, LONG lDestY, RECT* srcRect)
{
	HRESULT rval;
	DDBLTFX ddBltFx;
	RECT destRect;
	DWORD Pixel;
	LONG width, height;
	LONG tmpTop, tmpBottom;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	tmpTop    = lDestY;  
	tmpBottom = tRect.bottom;

	if(m_lpDDS == NULL)
		return -1;

	if (ValidateBlt(lpCDXS, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

	if (tmpTop != lDestY)
	{
		tRect.top    -= lDestY - tmpTop;
		tRect.bottom -= lDestY - tmpTop;
		tmpBottom    -= lDestY - tmpTop;
	}

	if (tmpBottom != tRect.bottom)
	{
		tRect.top    += tmpBottom - tRect.bottom;
		tRect.bottom += tmpBottom - tRect.bottom;
	}

	ddBltFx.dwSize = sizeof(DDBLTFX);
	ddBltFx.dwDDFX = DDBLTFX_MIRRORUPDOWN;

	width           = tRect.right - tRect.left;
	height          = tRect.bottom - tRect.top;
	destRect.top    = lDestY;
	destRect.left   = lDestX;
	destRect.bottom = lDestY + height;
	destRect.right  = lDestX + width;

	rval = lpCDXS->m_lpDDS->Blt(&destRect, m_lpDDS, &tRect, DDBLT_DDFX | DDBLT_WAIT, &ddBltFx);
	
	if(rval == DDERR_SURFACELOST)
	{
		rval = Restore();
		return rval;
	}
	else if ( SUCCEEDED(rval) )
	{
		return rval;
	}
	else
	{
		Lock();
		lpCDXS->Lock();

		for(int j = 0; j < height; j++)
		{
			for(int i = 0; i < width; i++)
			{
				Pixel = GetPixel(tRect.left + i, tRect.top + j);
				lpCDXS->PutPixel(destRect.left + i, (destRect.top + height) - j, Pixel);
			}
		}

		UnLock();
		lpCDXS->UnLock();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawTransVFlip
// 
// PURPOSE: Draws a block of pixels flipped vertically from this surface to the 
//          surface pointed to by lpCDXS.  Pixels matching the transparency color 
//          key are not drawn.      
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: DirectDraw function BltFast() performs an Lock/Unlock internally.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawTransVFlip(CDXSurface* lpCDXS, LONG lDestX, LONG lDestY, RECT* srcRect)
{
	HRESULT rval;
	DDBLTFX ddBltFx;
	RECT destRect;
	DWORD Pixel;
	LONG width, height;
	LONG tmpTop, tmpBottom;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	tmpTop    = lDestY;  
	tmpBottom = tRect.bottom;

	if(m_lpDDS == NULL)
		return -1;

	if (ValidateBlt(lpCDXS, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

	if (tmpTop != lDestY)
	{
		tRect.top    -= lDestY - tmpTop;
		tRect.bottom -= lDestY - tmpTop;
		tmpBottom    -= lDestY - tmpTop;
	}

	if (tmpBottom != tRect.bottom)
	{
		tRect.top    += tmpBottom - tRect.bottom;
		tRect.bottom += tmpBottom - tRect.bottom;
	}

	ddBltFx.dwSize = sizeof(DDBLTFX);
	ddBltFx.dwDDFX = DDBLTFX_MIRRORUPDOWN;

	width           = tRect.right - tRect.left;
	height          = tRect.bottom - tRect.top;
	destRect.top    = lDestY;
	destRect.left   = lDestX;
	destRect.bottom = lDestY + height;
	destRect.right  = lDestX + width;

	rval = lpCDXS->m_lpDDS->Blt(&destRect, m_lpDDS, &tRect, DDBLT_DDFX | DDBLT_WAIT | DDBLT_KEYSRC, &ddBltFx);
	
	if(rval == DDERR_SURFACELOST)
	{
		rval = Restore();
		return rval;
	}
	else if ( SUCCEEDED(rval) )
	{
		return rval;
	}
	else
	{
		Lock();
		lpCDXS->Lock();

		for(int j = 0; j < height; j++)
		{
			for(int i = 0; i < width; i++)
			{
				Pixel = GetPixel(tRect.left + i, tRect.top + j);
				if (Pixel != m_ColorKey)
					lpCDXS->PutPixel(destRect.left + i, (destRect.top + height) - j, Pixel);
			}
		}

		UnLock();
		lpCDXS->UnLock();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawBlkAlphaFast
// 
// PURPOSE: Performs a 50% alpha blend of a block of pixels from this surface to 
//          the surface pointed to by dest.  The transparency color key is ignored.     
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawBlkAlphaFast(CDXSurface* dest, LONG lDestX, LONG lDestY, RECT* srcRect)
{
    int register i,j;
	int height,width;
    BYTE* lpSprite;
    BYTE* lpDest;
    unsigned long dPitch, SpritePitch;
    DWORD sTemp,dTemp;
    WORD sbuf,dbuf;
    DWORD Result;
	BOOL oddWidth = FALSE;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Validate the inputs and clip them to m_clipRect
	if (ValidateBlt(dest, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

    // Set height and width of SPRITE
    height = tRect.bottom - tRect.top;
    width  = tRect.right - tRect.left;

    // Lock down both surfaces for read and write
    Lock();
    dest->Lock();

    // Set the pitch for both surfaces
    SpritePitch = m_DDSD.lPitch;
    dPitch      = dest->m_DDSD.lPitch;

    // Initialize the pointers to the upper left hand corner of the surface
    lpSprite = (BYTE*)m_DDSD.lpSurface;
    lpDest   = (BYTE*)dest->m_DDSD.lpSurface;

	// Start RGB Format switch statement
	switch(dest->m_PixelFormat.bpp)
	{
	case 8:
		// IMHO paletized modes are a thing of the past please feel free to 
		// implement this if you so desire.
		break;

	case 15:

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.


		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);
				dTemp = *((WORD*)lpDest);

				*((WORD*)lpDest) = (WORD)( ((sTemp & 0x7BDE) >> 1) + ((dTemp & 0x7BDE) >> 1) );

				lpDest += 2;
				lpSprite += 2;
			}

			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				dTemp = *((DWORD*)lpDest);

				Result = ((sTemp & 0x7BDE) >> 1) + ((dTemp & 0x7BDE) >> 1);
				Result |= ((sTemp & 0x7BDE0000) >> 1) + ((dTemp & 0x7BDE0000) >> 1);

				*((DWORD*)lpDest) = Result;

				lpDest += 4;
				lpSprite += 4;

			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);

		break;

	case 16:

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.

		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);
				dTemp = *((WORD*)lpDest);

				*((WORD*)lpDest) = (WORD)( ((sTemp & 0xF7DE) >> 1) + ((dTemp & 0xF7DE) >> 1) );

				lpDest += 2;
				lpSprite += 2;
			}

			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				dTemp = *((DWORD*)lpDest);

				Result = ((sTemp & 0xF7DE) >> 1) + ((dTemp & 0xF7DE) >> 1);
				Result |= ((sTemp & 0xF7DE0000) >> 1) + ((dTemp & 0xF7DE0000) >> 1);

				*((DWORD*)lpDest) = Result;

				lpDest += 4;
				lpSprite += 4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;

	case 24:

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 3);
		lpDest   += (lDestY * dPitch) + (lDestX * 3);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (3 * width));
		dbuf = (WORD)(dPitch - (3 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				dTemp = *((DWORD*)lpDest);

				Result = ((sTemp & 0xFEFEFE) >> 1) + ((dTemp & 0xFEFEFE) >> 1);

				*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
				lpDest += 2;
				*lpDest = (BYTE)(Result >> 16);

				lpDest++;
				lpSprite += 3;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		
		break;

	case 32:
		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 4);
		lpDest   += (lDestY * dPitch) + (lDestX * 4);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (4 * width));
		dbuf = (WORD)(dPitch - (4 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				dTemp = *((DWORD*)lpDest);

				Result = ((sTemp & 0xFEFEFE) >> 1) + ((dTemp & 0xFEFEFE) >> 1);

				*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
				lpDest += 2;
				*lpDest = (BYTE)(Result >> 16);

				lpDest += 2;
				lpSprite += 4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;
	} // End RGB Format switch statement


    UnLock();
    dest->UnLock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawTransAlphaFast
// 
// PURPOSE: Performs a 50% alpha blend of a block of pixels from this surface to 
//          the surface pointed to by dest.  Pixels that match the transparency 
//          color key are not drawn.     
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawTransAlphaFast(CDXSurface* dest, LONG lDestX, LONG lDestY, RECT* srcRect)
{
    int register i,j;
	int height,width;
    BYTE* lpSprite;
    BYTE* lpDest;
    unsigned long dPitch, SpritePitch;
    DWORD sColorKey;
    DWORD sTemp,dTemp;
    WORD sbuf,dbuf;
    DWORD Result;
	BOOL oddWidth = FALSE;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Validate the inputs and clip them to m_clipRect
	if (ValidateBlt(dest, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

    // Set height and width of SPRITE
    height = tRect.bottom - tRect.top;
    width  = tRect.right - tRect.left;

    // Lock down both surfaces for read and write
    Lock();
    dest->Lock();

    // Set the pitch for both surfaces
    SpritePitch = m_DDSD.lPitch;
    dPitch      = dest->m_DDSD.lPitch;

    // Initialize the pointers to the upper left hand corner of the surface
    lpSprite = (BYTE*)m_DDSD.lpSurface;
    lpDest   = (BYTE*)dest->m_DDSD.lpSurface;

	// Get the color key for sprite surface
	sColorKey = (DWORD)m_ColorKey;

	// Start RGB Format switch statement
	switch(dest->m_PixelFormat.bpp)
	{
	case 8:
		// IMHO paletized modes are a thing of the past please feel free to 
		// implement this if you so desire.
		break;

	case 15:

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.


		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);

				if (sTemp != sColorKey)
				{
					dTemp = *((WORD*)lpDest);
					*((WORD*)lpDest) = (WORD)( ((sTemp & 0x7BDE) >> 1) + ((dTemp & 0x7BDE) >> 1) );
				}

				lpDest += 2;
				lpSprite += 2;
			}

			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				dTemp = *((DWORD*)lpDest);
				Result = dTemp;

				if ((sTemp & 0xFFFF) != sColorKey)
				{
					Result &= 0xFFFF0000;
					Result |= ((sTemp & 0x7BDE) >> 1) + ((dTemp & 0x7BDE) >> 1);
				}
				if ((sTemp >> 16) != sColorKey)
				{
					Result &= 0xFFFF;
					Result |= ((sTemp & 0x7BDE0000) >> 1) + ((dTemp & 0x7BDE0000) >> 1);
				}

				*((DWORD*)lpDest) = Result;
				lpDest += 4;
				lpSprite += 4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);

		break;

	case 16:

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.

		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);

				if (sTemp != sColorKey)
				{
					dTemp = *((WORD*)lpDest);
					*((WORD*)lpDest) = (WORD)( ((sTemp & 0xF7DE) >> 1) + ((dTemp & 0xF7DE) >> 1) );
				}

				lpDest += 2;
				lpSprite += 2;
			}

			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				dTemp = *((DWORD*)lpDest);
				Result = dTemp;

				if ((sTemp & 0xFFFF) != sColorKey)
				{
					Result &= 0xFFFF0000;
					Result |= ((sTemp & 0xF7DE) >> 1) + ((dTemp & 0xF7DE) >> 1);
				}
				if ((sTemp >> 16) != sColorKey)
				{
					Result &= 0xFFFF;
					Result |= ((sTemp & 0xF7DE0000) >> 1) + ((dTemp & 0xF7DE0000) >> 1);
				}

				*((DWORD*)lpDest) = Result;
				lpDest += 4;
				lpSprite += 4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;

	case 24:
		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 3);
		lpDest   += (lDestY * dPitch) + (lDestX * 3);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (3 * width));
		dbuf = (WORD)(dPitch - (3 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);

				if ((sTemp &= 0xFFFFFF) != sColorKey)
				{
					dTemp = *((DWORD*)lpDest);
					Result = ((sTemp & 0xFEFEFE) >> 1) + ((dTemp & 0xFEFEFE) >> 1);

					*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
					lpDest += 2;
					*lpDest = (BYTE)(Result >> 16);
					lpDest++;
				}
				else
				{
					lpDest += 3;
				}

				lpSprite += 3;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;

	case 32:
		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 4);
		lpDest   += (lDestY * dPitch) + (lDestX * 4);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (4 * width));
		dbuf = (WORD)(dPitch - (4 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);

				if ((sTemp &= 0xFFFFFF) != sColorKey)
				{
					dTemp = *((DWORD*)lpDest);
					Result = ((sTemp & 0xFEFEFE) >> 1) + ((dTemp & 0xFEFEFE) >> 1);

					*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
					lpDest += 2;
					*lpDest = (BYTE)(Result >> 16);
					lpDest += 2;
				}
				else
				{
					lpDest += 4;
				}

				lpSprite += 4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;
	} // End RGB Format switch statement


    UnLock();
    dest->UnLock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawBlkAlpha
// 
// PURPOSE: Performs an alpha blend on a block of pixels from this surface to 
//          the surface pointed to by dest.  The transparency color key is ignored. 
//          The amount of alpha blending is determined by the passed in value ALPHA.    
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//        ALPHA   - determines the amount of alpha blending between 0 and 256.
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawBlkAlpha(CDXSurface* dest, LONG lDestX, LONG lDestY, RECT* srcRect, WORD ALPHA)
{
	int register i,j;
	int height,width;
	BYTE* lpSprite;
	BYTE* lpDest;
	unsigned long dPitch, SpritePitch;
	DWORD sTemp,dTemp;
	long sr,sg,sb,dr,dg,db;
	WORD sbuf,dbuf;
	DWORD Result;
	BOOL oddWidth = FALSE;
    RECT tRect;

    DETAILTRACE(0x2ac001,0);
    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Validate the inputs and clip them to m_clipRect
  DETAILTRACE(0x2ac002,0);
	if (ValidateBlt(dest, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

  DETAILTRACE(0x2ac004,0);
	// Set height and width of SPRITE
    height = tRect.bottom - tRect.top;
    width  = tRect.right - tRect.left; 

    // Lock down both surfaces for read and write
    Lock();
    DETAILTRACE(0x2ac006,0);
    dest->Lock();

    DETAILTRACE(0x2ac005,0);
	// Set the pitch for both surfaces
    SpritePitch = m_DDSD.lPitch;
    dPitch      = dest->m_DDSD.lPitch;

    // Initialize the pointers to the upper left hand corner of surface
    lpSprite = (BYTE*)m_DDSD.lpSurface;
    lpDest = (BYTE*)dest->m_DDSD.lpSurface;

  DETAILTRACE(0x2ac003,0);

	switch(dest->m_PixelFormat.bpp)
	{
	case 8:
		// IMHO paletized modes are a thing of the past please feel free to 
		// implement this if you so desire.
    DETAILTRACE(0x2ac008,0);
		break;

	case 15:

		// Check the ALPHA value
    DETAILTRACE(0x2ac015,0);
		if (ALPHA < 0)
			ALPHA = 0;
		else if (ALPHA > 256)
			ALPHA = 256;
		
		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.

		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);
				dTemp = *((WORD*)lpDest);
				sb = sTemp & 0x1f;
				db = dTemp & 0x1f;
				sg = (sTemp >> 5) & 0x1f;
				dg = (dTemp >> 5) & 0x1f;
				sr = (sTemp >> 10) & 0x1f;
				dr = (dTemp >> 10) & 0x1f;

				*((WORD*)lpDest) = (WORD)((ALPHA * (db - sb) >> 8) + sb |
					((ALPHA * (dg - sg) >> 8) + sg) << 5 |
					((ALPHA * (dr - sr) >> 8) + sr) << 10);

				lpDest += 2;
				lpSprite += 2;
			}
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				dTemp = *((DWORD*)lpDest);

				sb = sTemp & 0x1f;
				db = dTemp & 0x1f;
				sg = (sTemp >> 5) & 0x1f;
				dg = (dTemp >> 5) & 0x1f;
				sr = (sTemp >> 10) & 0x1f;
				dr = (dTemp >> 10) & 0x1f;

				Result = (DWORD)((ALPHA * (db - sb) >> 8) + sb |
					((ALPHA * (dg - sg) >> 8) + sg) << 5 |
					((ALPHA * (dr - sr) >> 8) + sr) << 10);

				sb = (sTemp >> 16) & 0x1f;
				db = (dTemp >> 16) & 0x1f;
				sg = (sTemp >> 21) & 0x1f;
				dg = (dTemp >> 21) & 0x1f;
				sr = (sTemp >> 26) & 0x1f;
				dr = (dTemp >> 26) & 0x1f;

				Result |= (DWORD)( (ALPHA * (db - sb) >> 8) + sb |
					((ALPHA * (dg - sg) >> 8) + sg) << 5 |
					((ALPHA * (dr - sr) >> 8) + sr) << 10 ) << 16;

				*((DWORD*)lpDest) = Result;

				lpDest += 4;
				lpSprite += 4;

			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);

		break;

	case 16:

		// Check the ALPHA value
    DETAILTRACE(0x2ac016,0);
		if (ALPHA < 0)
			ALPHA = 0;
		else if (ALPHA > 256)
			ALPHA = 256;

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.


		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);
				dTemp = *((WORD*)lpDest);

				sb = sTemp & 0x1f;
				db = dTemp & 0x1f;
				sg = (sTemp >> 5) & 0x3f;
				dg = (dTemp >> 5) & 0x3f;
				sr = (sTemp >> 11) & 0x1f;
				dr = (dTemp >> 11) & 0x1f;

				*((WORD*)lpDest) = (WORD)((ALPHA * (db - sb) >> 8) + sb |
					((ALPHA * (dg - sg) >> 8) + sg) << 5 |
					((ALPHA * (dr - sr) >> 8) + sr) << 11);

				lpDest += 2;
				lpSprite += 2;
			}
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				dTemp = *((DWORD*)lpDest);

				sb = sTemp & 0x1f;
				db = dTemp & 0x1f;
				sg = (sTemp >> 5) & 0x3f;
				dg = (dTemp >> 5) & 0x3f;
				sr = (sTemp >> 11) & 0x1f;
				dr = (dTemp >> 11) & 0x1f;

				Result = (DWORD)((ALPHA * (db - sb) >> 8) + sb |
					((ALPHA * (dg - sg) >> 8) + sg) << 5 |
					((ALPHA * (dr - sr) >> 8) + sr) << 11);

				sb = (sTemp >> 16) & 0x1f;
				db = (dTemp >> 16) & 0x1f;
				sg = (sTemp >> 21) & 0x3f;
				dg = (dTemp >> 21) & 0x3f;
				sr = (sTemp >> 27) & 0x1f;
				dr = (dTemp >> 27) & 0x1f;

				Result |= (DWORD)( (ALPHA * (db - sb) >> 8) + sb |
					((ALPHA * (dg - sg) >> 8) + sg) << 5 |
					((ALPHA * (dr - sr) >> 8) + sr) << 11 ) << 16;

				*((DWORD*)lpDest) = Result;

				lpDest += 4;
				lpSprite +=4;

			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);

		break;

	case 24:
		// Check the ALPHA value
    DETAILTRACE(0x2ac024,0);
		if (ALPHA < 0)
			ALPHA = 0;
		else if (ALPHA > 256)
			ALPHA = 256;

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 3);
		lpDest   += (lDestY * dPitch) + (lDestX * 3);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (3 * width));
		dbuf = (WORD)(dPitch - (3 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				dTemp = *((DWORD*)lpDest);

				sb = sTemp & 0xFF;
				db = dTemp & 0xFF;
				sg = (sTemp >> 8) & 0xFF;
				dg = (dTemp >> 8) & 0xFF;
				sr = (sTemp >> 16) & 0xFF;
				dr = (dTemp >> 16) & 0xFF;

				Result = (DWORD)(ALPHA * (db - sb) >> 8) + sb |
					(DWORD)((ALPHA * (dg - sg) >> 8) + sg) << 8 |
					(DWORD)((ALPHA * (dr - sr) >> 8) + sr) << 16;

				*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
				lpDest += 2;
				*lpDest = (BYTE)(Result >> 16);

				lpDest++;
				lpSprite += 3;

			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);

		break;

	case 32:
		// Check the ALPHA value
    DETAILTRACE(0x2ac032,0);
		if (ALPHA < 0)
			ALPHA = 0;
		else if (ALPHA > 256)
			ALPHA = 256;

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 4);
		lpDest   += (lDestY * dPitch) + (lDestX * 4);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (4 * width));
		dbuf = (WORD)(dPitch - (4 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				dTemp = *((DWORD*)lpDest);

				sb = sTemp & 0xFF;
				db = dTemp & 0xFF;
				sg = (sTemp >> 8) & 0xFF;
				dg = (dTemp >> 8) & 0xFF;
				sr = (sTemp >> 16) & 0xFF;
				dr = (dTemp >> 16) & 0xFF;

				Result = (DWORD)((ALPHA * (db - sb) >> 8) + sb) |
					(DWORD)((ALPHA * (dg - sg) >> 8) + sg) << 8 |
					(DWORD)((ALPHA * (dr - sr) >> 8) + sr) << 16;

				*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
				lpDest += 2;
				*lpDest = (BYTE)(Result >> 16);

				lpDest += 2;
				lpSprite += 4;

			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;
	} // End RGB Format switch statement

  DETAILTRACE(0x2ac033,0);

	UnLock();
	dest->UnLock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawTransAlpha
// 
// PURPOSE: Performs an alpha blend on a block of pixels from this surface to 
//          the surface pointed to by dest.  Pixels that match the transparency 
//          color key are not drawn. The amount of alpha blending is determined 
//          by the passed in value ALPHA.    
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//        ALPHA   - determines the amount of alpha blending between 0 and 256.
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawTransAlpha(CDXSurface* dest, LONG lDestX, LONG lDestY, RECT* srcRect, WORD ALPHA)
{
	int register i,j;
	int height,width;
	BYTE* lpSprite;
	BYTE* lpDest;
	unsigned long dPitch, SpritePitch;
	DWORD sColorKey;
	DWORD sTemp,dTemp;
	long sr,sg,sb,dr,dg,db;
	WORD sbuf,dbuf;
	DWORD Result;
	BOOL oddWidth = FALSE;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Validate the inputs and clip them to m_clipRect
	if (ValidateBlt(dest, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

	// Set height and width of SPRITE
    height = tRect.bottom - tRect.top;
    width  = tRect.right - tRect.left; 

    // Get the color key for sprite surface
    sColorKey = (DWORD)m_ColorKey;

    // Lock down both surfaces for read and write
    Lock();
    dest->Lock();

	// Set the pitch for both surfaces
    SpritePitch = m_DDSD.lPitch;
    dPitch      = dest->m_DDSD.lPitch;

    // Initialize the pointers to the upper left hand corner of surface
    lpSprite = (BYTE*)m_DDSD.lpSurface;
    lpDest   = (BYTE*)dest->m_DDSD.lpSurface;


	switch(dest->m_PixelFormat.bpp)
	{
	case 8:
		// IMHO paletized modes are a thing of the past please feel free to 
		// implement this if you so desire.
		break;

	case 15:

		// Check the ALPHA value
		if (ALPHA < 0)
			ALPHA = 0;
		else if (ALPHA > 256)
			ALPHA = 256;
		
		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.

		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);

				if (sTemp != sColorKey)
				{
					dTemp = *((WORD*)lpDest);
					sb = sTemp & 0x1f;
					db = dTemp & 0x1f;
					sg = (sTemp >> 5) & 0x1f;
					dg = (dTemp >> 5) & 0x1f;
					sr = (sTemp >> 10) & 0x1f;
					dr = (dTemp >> 10) & 0x1f;

					*((WORD*)lpDest) = (WORD)((ALPHA * (db - sb) >> 8) + sb |
						((ALPHA * (dg - sg) >> 8) + sg) << 5 |
						((ALPHA * (dr - sr) >> 8) + sr) << 10);
				}

				lpDest += 2;
				lpSprite += 2;
			}
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);

				if (sTemp != ( (DWORD)sColorKey | ((DWORD)sColorKey << 16)) )
				{
					dTemp = *((DWORD*)lpDest);
					Result = dTemp;

					if ((sTemp & 0xffff) != sColorKey)
					{
						sb = sTemp & 0x1f;
						db = dTemp & 0x1f;
						sg = (sTemp >> 5) & 0x1f;
						dg = (dTemp >> 5) & 0x1f;
						sr = (sTemp >> 10) & 0x1f;
						dr = (dTemp >> 10) & 0x1f;

						Result = Result & 0xffff0000;
						Result |= (DWORD)((ALPHA * (db - sb) >> 8) + sb |
							((ALPHA * (dg - sg) >> 8) + sg) << 5 |
							((ALPHA * (dr - sr) >> 8) + sr) << 10);
					}
					if (((sTemp >> 16) & 0xffff) != sColorKey)
					{
						sb = (sTemp >> 16) & 0x1f;
						db = (dTemp >> 16) & 0x1f;
						sg = (sTemp >> 21) & 0x1f;
						dg = (dTemp >> 21) & 0x1f;
						sr = (sTemp >> 26) & 0x1f;
						dr = (dTemp >> 26) & 0x1f;

						Result = Result & 0xffff;
						Result |= (DWORD)( (ALPHA * (db - sb) >> 8) + sb |
							((ALPHA * (dg - sg) >> 8) + sg) << 5 |
							((ALPHA * (dr - sr) >> 8) + sr) << 10 ) << 16;
					}

					*((DWORD*)lpDest) = Result;
				}
				lpDest += 4;
				lpSprite += 4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;
		}while (--i > 0);

		break;

	case 16:

		// Check the ALPHA value
		if (ALPHA < 0)
			ALPHA = 0;
		else if (ALPHA > 256)
			ALPHA = 256;

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.


		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);

				if (sTemp != sColorKey)
				{
					dTemp = *((WORD*)lpDest);
					sb = sTemp & 0x1f;
					db = dTemp & 0x1f;
					sg = (sTemp >> 5) & 0x3f;
					dg = (dTemp >> 5) & 0x3f;
					sr = (sTemp >> 11) & 0x1f;
					dr = (dTemp >> 11) & 0x1f;

					*((WORD*)lpDest) = (WORD)((ALPHA * (db - sb) >> 8) + sb |
						((ALPHA * (dg - sg) >> 8) + sg) << 5 |
						((ALPHA * (dr - sr) >> 8) + sr) << 11);
				}

				lpDest += 2;
				lpSprite += 2;
			}
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);

				if (sTemp != ( (DWORD)sColorKey | ((DWORD)sColorKey << 16)) )
				{
					dTemp = *((DWORD*)lpDest);
					Result = dTemp;

					if ((sTemp & 0xffff) != sColorKey)
					{
						sb = sTemp & 0x1f;
						db = dTemp & 0x1f;
						sg = (sTemp >> 5) & 0x3f;
						dg = (dTemp >> 5) & 0x3f;
						sr = (sTemp >> 11) & 0x1f;
						dr = (dTemp >> 11) & 0x1f;

						Result = Result & 0xffff0000;
						Result |= (DWORD)((ALPHA * (db - sb) >> 8) + sb |
							((ALPHA * (dg - sg) >> 8) + sg) << 5 |
							((ALPHA * (dr - sr) >> 8) + sr) << 11);
					}
					if (((sTemp >> 16) & 0xffff) != sColorKey)
					{
						sb = (sTemp >> 16) & 0x1f;
						db = (dTemp >> 16) & 0x1f;
						sg = (sTemp >> 21) & 0x3f;
						dg = (dTemp >> 21) & 0x3f;
						sr = (sTemp >> 27) & 0x1f;
						dr = (dTemp >> 27) & 0x1f;

						Result = Result & 0xffff;
						Result |= (DWORD)( (ALPHA * (db - sb) >> 8) + sb |
							((ALPHA * (dg - sg) >> 8) + sg) << 5 |
							((ALPHA * (dr - sr) >> 8) + sr) << 11 ) << 16;
					}

					*((DWORD*)lpDest) = Result;
				}
				lpDest += 4;
				lpSprite +=4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;
		}while (--i > 0);

		break;

	case 24:

		// Check the ALPHA value
		if (ALPHA < 0)
			ALPHA = 0;
		else if (ALPHA > 256)
			ALPHA = 256;

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 3);
		lpDest   += (lDestY * dPitch) + (lDestX * 3);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (3 * width));
		dbuf = (WORD)(dPitch - (3 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);

				if ((sTemp & 0xFFFFFF) != sColorKey)
				{
					dTemp = *((DWORD*)lpDest);
					sb = sTemp & 0xFF;
					db = dTemp & 0xFF;
					sg = (sTemp >> 8) & 0xFF;
					dg = (dTemp >> 8) & 0xFF;
					sr = (sTemp >> 16) & 0xFF;
					dr = (dTemp >> 16) & 0xFF;

					Result = (DWORD)((ALPHA * (db - sb) >> 8) + sb |
						((ALPHA * (dg - sg) >> 8) + sg) << 8 |
						((ALPHA * (dr - sr) >> 8) + sr) << 16);

					*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
					lpDest += 2;
					*lpDest = (BYTE)(Result >> 16);
					lpDest++;
				}
				else
				{
					lpDest += 3;
				}

				lpSprite += 3;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;
		}while (--i > 0);
		break;

	case 32:
		// Check the ALPHA value
		if (ALPHA < 0)
			ALPHA = 0;
		else if (ALPHA > 256)
			ALPHA = 256;

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 4);
		lpDest   += (lDestY * dPitch) + (lDestX * 4);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (4 * width));
		dbuf = (WORD)(dPitch - (4 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);

				if ((sTemp & 0xFFFFFF) != sColorKey)
				{
					dTemp = *((DWORD*)lpDest);
					sb = sTemp & 0xFF;
					db = dTemp & 0xFF;
					sg = (sTemp >> 8) & 0xFF;
					dg = (dTemp >> 8) & 0xFF;
					sr = (sTemp >> 16) & 0xFF;
					dr = (dTemp >> 16) & 0xFF;

					Result = (DWORD)((ALPHA * (db - sb) >> 8) + sb |
						((ALPHA * (dg - sg) >> 8) + sg) << 8 |
						((ALPHA * (dr - sr) >> 8) + sr) << 16);

					*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
					lpDest += 2;
					*lpDest = (BYTE)(Result >> 16);
					lpDest += 2;
				}
				else
				{
					lpDest += 4;
				}

				lpSprite += 4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;
		}while (--i > 0);
		break;
	} // End RGB Format switch statement


	UnLock();
	dest->UnLock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawBlkShadow
// 
// PURPOSE: Performs an alpha blend with the color black and the surface pointed to 
//          by dest.  The transparency color key is ignored. The amount of 
//          shadowing is determined by the passed in value SHADOW.    
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//        SHADOW  - determines the amount of shadowing, between 0 and 256.
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawBlkShadow(CDXSurface* dest, LONG lDestX, LONG lDestY, RECT* srcRect, WORD SHADOW)
{
	int register i,j;
	int height,width;
	BYTE* lpSprite;
	BYTE* lpDest;
	unsigned long dPitch, SpritePitch;
	DWORD dTemp;
	DWORD dr,dg,db;
	WORD sbuf,dbuf;
	DWORD Result;
	BOOL oddWidth = FALSE;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Validate the inputs and clip them to m_clipRect
	if (ValidateBlt(dest, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

	// Set height and width of SPRITE
    height = tRect.bottom - tRect.top;
    width  = tRect.right - tRect.left; 

    // Lock down both surfaces for read and write
    Lock();
    dest->Lock();

	// Set the pitch for both surfaces
    SpritePitch = m_DDSD.lPitch;
    dPitch      = dest->m_DDSD.lPitch;

    // Initialize the pointers to the upper left hand corner of surface
    lpSprite = (BYTE*)m_DDSD.lpSurface;
    lpDest   = (BYTE*)dest->m_DDSD.lpSurface;


	switch(dest->m_PixelFormat.bpp)
	{
	case 8:
		// IMHO paletized modes are a thing of the past please feel free to 
		// implement this if you so desire.
		break;

	case 15:

		// Check the ALPHA value
		if (SHADOW < 0)
			SHADOW = 0;
		else if (SHADOW > 256)
			SHADOW = 256;
		
		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.

		i = height;
		do
		{
			if (oddWidth)
			{
				dTemp = *((WORD*)lpDest);
				db = dTemp & 0x1f;
				dg = (dTemp >> 5) & 0x1f;
				dr = (dTemp >> 10) & 0x1f;

				*((WORD*)lpDest) = (WORD)((SHADOW * db) >> 8 |
					((SHADOW * dg) >> 8) << 5 |
					((SHADOW * dr) >> 8) << 10);

				lpDest += 2;
				lpSprite += 2;
			}
			j = width;
			do
			{
				dTemp = *((DWORD*)lpDest);

				db = dTemp & 0x1f;
				dg = (dTemp >> 5) & 0x1f;
				dr = (dTemp >> 10) & 0x1f;

				Result = (DWORD)((SHADOW * db) >> 8 |
					((SHADOW * dg) >> 8) << 5 |
					((SHADOW * dr) >> 8) << 10);

				db = (dTemp >> 16) & 0x1f;
				dg = (dTemp >> 21) & 0x1f;
				dr = (dTemp >> 26) & 0x1f;

				Result |= (DWORD)( (SHADOW * db) >> 8 |
					((SHADOW * dg) >> 8) << 5 |
					((SHADOW * dr) >> 8) << 10 ) << 16;

				*((DWORD*)lpDest) = Result;

				lpDest += 4;
				lpSprite += 4;

			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);

		break;

	case 16:

		// Check the ALPHA value
		if (SHADOW < 0)
			SHADOW = 0;
		else if (SHADOW > 256)
			SHADOW = 256;

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.


		i = height;
		do
		{
			if (oddWidth)
			{
				dTemp = *((WORD*)lpDest);

				db = dTemp & 0x1f;
				dg = (dTemp >> 5)  & 0x3f;
				dr = (dTemp >> 11) & 0x1f;

				*((WORD*)lpDest) = (WORD)((SHADOW * db) >> 8 |
					                     ((SHADOW * dg) >> 8) << 5 |
					                     ((SHADOW * dr) >> 8) << 11);

				lpDest   += 2;
				lpSprite += 2;
			}
			j = width;
			do
			{
				dTemp = *((DWORD*)lpDest);

				db = dTemp & 0x1f;
				dg = (dTemp >> 5)  & 0x3f;
				dr = (dTemp >> 11) & 0x1f;

				Result = (DWORD)( ((SHADOW * db) >> 8) |
					             (((SHADOW * dg) >> 8) << 5) |
					             (((SHADOW * dr) >> 8) << 11) );

				db = (dTemp >> 16) & 0x1f;
				dg = (dTemp >> 21) & 0x3f;
				dr = (dTemp >> 27) & 0x1f;

				Result |= (DWORD)( ((SHADOW * db) >> 8) |
					              (((SHADOW * dg) >> 8) << 5) |
					              (((SHADOW * dr) >> 8) << 11) ) << 16;

				*((DWORD*)lpDest) = Result;

				lpDest   += 4;
				lpSprite += 4;

			}while (--j > 0);
			lpDest   += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);

		break;

	case 24:

		// Check the ALPHA value
		if (SHADOW < 0)
			SHADOW = 0;
		else if (SHADOW > 256)
			SHADOW = 256;

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 3);
		lpDest   += (lDestY * dPitch) + (lDestX * 3);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (3 * width));
		dbuf = (WORD)(dPitch - (3 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				dTemp = *((DWORD*)lpDest);

				db = dTemp & 0xFF;
				dg = (dTemp >> 8) & 0xFF;
				dr = (dTemp >> 16) & 0xFF;

				Result = (DWORD)((SHADOW * db) >> 8 |
					((SHADOW * dg) >> 8) << 8 |
					((SHADOW * dr) >> 8) << 16);

				*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
				lpDest += 2;
				*lpDest = (BYTE)(Result >> 16);
				lpDest++;

				lpSprite += 3;

			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;

	case 32:

		// Check the ALPHA value
		if (SHADOW < 0)
			SHADOW = 0;
		else if (SHADOW > 256)
			SHADOW = 256;

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 4);
		lpDest   += (lDestY * dPitch) + (lDestX * 4);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (4 * width));
		dbuf = (WORD)(dPitch - (4 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				dTemp = *((DWORD*)lpDest);

				db = dTemp & 0xFF;
				dg = (dTemp >> 8) & 0xFF;
				dr = (dTemp >> 16) & 0xFF;

				Result = (DWORD)((SHADOW * db) >> 8 |
					((SHADOW * dg) >> 8) << 8 |
					((SHADOW * dr) >> 8) << 16);

				*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
				lpDest += 2;
				*lpDest = (BYTE)(Result >> 16);
				lpDest += 2;

				lpSprite += 4;

			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;
	} // End RGB Format switch statement


	UnLock();
	dest->UnLock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawTransShadow
// 
// PURPOSE: Performs an alpha blend with the color black and the surface pointed to 
//          by dest.  Pixels that match the transparency color are not drawn. The 
//          amount of shadowing is determined by the passed in value SHADOW.    
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//        SHADOW  - determines the amount of shadowing, between 0 and 256.
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawTransShadow(CDXSurface* dest, LONG lDestX, LONG lDestY, RECT* srcRect, WORD SHADOW)
{
	int register i,j;
	int height,width;
	BYTE* lpSprite;
	BYTE* lpDest;
	unsigned long dPitch, SpritePitch;
	DWORD sColorKey;
	DWORD sTemp,dTemp;
	DWORD dr,dg,db;
	WORD sbuf,dbuf;
	DWORD Result;
	BOOL oddWidth = FALSE;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Validate the inputs and clip them to m_clipRect
	if (ValidateBlt(dest, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

	// Set height and width of SPRITE
    height = tRect.bottom - tRect.top;
    width  = tRect.right - tRect.left; 

    // Get the color key for sprite surface
    sColorKey = (DWORD)m_ColorKey;

    // Lock down both surfaces for read and write
    Lock();
    dest->Lock();

	// Set the pitch for both surfaces
    SpritePitch = m_DDSD.lPitch;
    dPitch      = dest->m_DDSD.lPitch;

    // Initialize the pointers to the upper left hand corner of surface
    lpSprite = (BYTE*)m_DDSD.lpSurface;
    lpDest   = (BYTE*)dest->m_DDSD.lpSurface;


	switch(dest->m_PixelFormat.bpp)
	{
	case 8:
		// IMHO paletized modes are a thing of the past please feel free to 
		// implement this if you so desire.
		break;

	case 15:

		// Check the ALPHA value
		if (SHADOW < 0)
			SHADOW = 0;
		else if (SHADOW > 256)
			SHADOW = 256;
		
		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.

		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);

				if (sTemp != sColorKey)
				{
					dTemp = *((WORD*)lpDest);
					db = dTemp & 0x1f;
					dg = (dTemp >> 5) & 0x1f;
					dr = (dTemp >> 10) & 0x1f;

					*((WORD*)lpDest) = (WORD)( (SHADOW * db) >> 8 |
						((SHADOW * dg) >> 8) << 5 |
						((SHADOW * dr) >> 8) << 10 );
				}

				lpDest += 2;
				lpSprite += 2;
			}
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);

				if (sTemp != ( (DWORD)sColorKey | ((DWORD)sColorKey << 16)) )
				{
					dTemp = *((DWORD*)lpDest);
					Result = dTemp;

					if ((sTemp & 0xffff) != sColorKey)
					{
						db = dTemp & 0x1f;
						dg = (dTemp >> 5) & 0x1f;
						dr = (dTemp >> 10) & 0x1f;

						Result = Result & 0xffff0000;
						Result |= (WORD)( (SHADOW * db) >> 8 |
							((SHADOW * dg) >> 8) << 5 |
							((SHADOW * dr) >> 8) << 10 );
					}
					if (((sTemp >> 16) & 0xffff) != sColorKey)
					{
						db = (dTemp >> 16) & 0x1f;
						dg = (dTemp >> 21) & 0x1f;
						dr = (dTemp >> 26) & 0x1f;

						Result = Result & 0xffff;
						Result |= (WORD)( (SHADOW * db) >> 8 |
							((SHADOW * dg) >> 8) << 5 |
							((SHADOW * dr) >> 8) << 10 ) << 16;
					}

					*((DWORD*)lpDest) = Result;
				}
				lpDest += 4;
				lpSprite += 4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;
		}while (--i > 0);

		break;

	case 16:

		// Check the ALPHA value
		if (SHADOW < 0)
			SHADOW = 0;
		else if (SHADOW > 256)
			SHADOW = 256;

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.


		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);

				if (sTemp != sColorKey)
				{
					dTemp = *((WORD*)lpDest);
					db = dTemp & 0x1f;
					dg = (dTemp >> 5) & 0x3f;
					dr = (dTemp >> 11) & 0x1f;

					*((WORD*)lpDest) = (WORD)( (SHADOW * db) >> 8 |
						((SHADOW * dg) >> 8) << 5 |
						((SHADOW * dr) >> 8) << 11 );
				}

				lpDest += 2;
				lpSprite += 2;
			}
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);

				if (sTemp != ( (DWORD)sColorKey | ((DWORD)sColorKey << 16)) )
				{
					dTemp = *((DWORD*)lpDest);
					Result = dTemp;

					if ((sTemp & 0xffff) != sColorKey)
					{
						db = dTemp & 0x1f;
						dg = (dTemp >> 5) & 0x3f;
						dr = (dTemp >> 11) & 0x1f;

						Result = Result & 0xffff0000;
						Result |= (WORD)( (SHADOW * db) >> 8 |
							((SHADOW * dg) >> 8) << 5 |
							((SHADOW * dr) >> 8) << 11 );
					}
					if (((sTemp >> 16) & 0xffff) != sColorKey)
					{
						db = (dTemp >> 16) & 0x1f;
						dg = (dTemp >> 21) & 0x3f;
						dr = (dTemp >> 27) & 0x1f;

						Result = Result & 0xffff;
						Result |= (WORD)( (SHADOW * db) >> 8 |
							((SHADOW * dg) >> 8) << 5 |
							((SHADOW * dr) >> 8) << 11 ) << 16;
					}

					*((DWORD*)lpDest) = Result;
				}
				lpDest += 4;
				lpSprite +=4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;
		}while (--i > 0);

		break;

	case 24:

		// Check the ALPHA value
		if (SHADOW < 0)
			SHADOW = 0;
		else if (SHADOW > 256)
			SHADOW = 256;

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 3);
		lpDest   += (lDestY * dPitch) + (lDestX * 3);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (3 * width));
		dbuf = (WORD)(dPitch - (3 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				
				if ((sTemp & 0xFFFFFF) != sColorKey)
				{
					dTemp = *((DWORD*)lpDest);

					db = dTemp & 0xFF;
					dg = (dTemp >> 8) & 0xFF;
					dr = (dTemp >> 16) & 0xFF;

					Result = (DWORD)( (SHADOW * db) >> 8 |
						((SHADOW * dg) >> 8) << 8 |
						((SHADOW * dr) >> 8) << 16 );

					*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
					lpDest += 2;
					*lpDest = (BYTE)(Result >> 16);
					lpDest++;
				}
				else
				{
					lpDest += 3;
				}
				
				lpSprite += 3;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;
		}while (--i > 0);
		break;

	case 32:

		// Check the ALPHA value
		if (SHADOW < 0)
			SHADOW = 0;
		else if (SHADOW > 256)
			SHADOW = 256;

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 4);
		lpDest   += (lDestY * dPitch) + (lDestX * 4);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (4 * width));
		dbuf = (WORD)(dPitch - (4 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				
				if ((sTemp & 0xFFFFFF) != sColorKey)
				{
					dTemp = *((DWORD*)lpDest);

					db = dTemp & 0xFF;
					dg = (dTemp >> 8) & 0xFF;
					dr = (dTemp >> 16) & 0xFF;

					Result = (DWORD)( (SHADOW * db) >> 8 |
						((SHADOW * dg) >> 8) << 8 |
						((SHADOW * dr) >> 8) << 16 );

					*((WORD*)lpDest) = (WORD)(Result & 0xFFFF);
					lpDest += 2;
					*lpDest = (BYTE)(Result >> 16);
					lpDest += 2;
				}
				else
				{
					lpDest += 4;
				}
				
				lpSprite += 4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;
		}while (--i > 0);
		break;
	} // End RGB Format switch statement


	UnLock();
	dest->UnLock();

	return 0;

}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawBlkShadowFast
// 
// PURPOSE: Performs a 50% alpha blend with the color black and the surface pointed to 
//          by dest.  The transparency color key is ignored.    
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawBlkShadowFast(CDXSurface* dest, LONG lDestX, LONG lDestY, RECT* srcRect)
{
    int register i,j;
	int height,width;
    BYTE* lpSprite;
    BYTE* lpDest;
    unsigned long dPitch, SpritePitch;
    DWORD dTemp;
    WORD sbuf,dbuf;
    DWORD Result;
	BOOL oddWidth = FALSE;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Validate the inputs and clip them to m_clipRect
	if (ValidateBlt(dest, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

    // Set height and width of SPRITE
    height = tRect.bottom - tRect.top;
    width  = tRect.right - tRect.left;

    // Lock down both surfaces for read and write
    Lock();
    dest->Lock();

    // Set the pitch for both surfaces
    SpritePitch = m_DDSD.lPitch;
    dPitch      = dest->m_DDSD.lPitch;

    // Initialize the pointers to the upper left hand corner of the surface
    lpSprite = (BYTE*)m_DDSD.lpSurface;
    lpDest   = (BYTE*)dest->m_DDSD.lpSurface;

	// Start RGB Format switch statement
	switch(dest->m_PixelFormat.bpp)
	{
	case 8:
		// IMHO paletized modes are a thing of the past please feel free to 
		// implement this if you so desire.
		break;

	case 15:

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.


		i = height;
		do
		{
			if (oddWidth)
			{
				dTemp = *((WORD*)lpDest);
				*((WORD*)lpDest) = (WORD)( (dTemp & 0x7BDE) >> 1 );
				lpDest += 2;
			}

			j = width;
			do
			{
				dTemp = *((DWORD*)lpDest);
				Result = (dTemp & 0x7BDE) >> 1;
				Result |= (dTemp & 0x7BDE0000) >> 1;
				*((DWORD*)lpDest) = Result;
				lpDest += 4;

			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);

		break;

	case 16:

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.

		i = height;
		do
		{
			if (oddWidth)
			{
				dTemp = *((WORD*)lpDest);
				*((WORD*)lpDest) = (WORD)( (dTemp & 0xF7DE) >> 1 );
				lpDest += 2;
			}

			j = width;
			do
			{
				dTemp = *((DWORD*)lpDest);
				Result = (dTemp & 0xF7DE) >> 1;
				Result |= (dTemp & 0xF7DE0000) >> 1;
				*((DWORD*)lpDest) = Result;
				lpDest += 4;

			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;

	case 24:
		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 3);
		lpDest   += (lDestY * dPitch) + (lDestX * 3);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (3 * width));
		dbuf = (WORD)(dPitch - (3 * width));
		i = height;
		do
		{
			j = width;
			do
			{
				dTemp = *((DWORD*)lpDest);
				*((WORD*)lpDest) = (WORD)((dTemp & 0xFEFE) >> 1);
				lpDest += 2;
				*lpDest = (BYTE)(((dTemp >> 16) & 0xFE) >> 1);
				lpDest++;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;

	case 32:
		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 4);
		lpDest   += (lDestY * dPitch) + (lDestX * 4);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (4 * width));
		dbuf = (WORD)(dPitch - (4 * width));
		i = height;
		do
		{
			j = width;
			do
			{
				dTemp = *((DWORD*)lpDest);
				*((WORD*)lpDest) = (WORD)((dTemp & 0xFEFE) >> 1);
				lpDest += 2;
				*lpDest = (BYTE)(((dTemp >> 16) & 0xFE) >> 1);
				lpDest += 2;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;
	} // End RGB Format switch statement


    UnLock();
    dest->UnLock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawTransShadowFast
// 
// PURPOSE: Performs a 50% alpha blend with the color black and the surface pointed to 
//          by dest.  Pixels that match the transparency color are not drawn.     
//
// INPUT: dest    - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//        SHADOW  - determines the amount of shadowing, between 0 and 256.
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawTransShadowFast(CDXSurface* dest, LONG lDestX, LONG lDestY, RECT* srcRect)
{
    int register i,j;
	int height,width;
    BYTE* lpSprite;
    BYTE* lpDest;
    unsigned long dPitch, SpritePitch;
    DWORD sColorKey;
    DWORD sTemp,dTemp;
    WORD sbuf,dbuf;
    DWORD Result;
	BOOL oddWidth = FALSE;
    RECT tRect;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Validate the inputs and clip them to m_clipRect
	if (ValidateBlt(dest, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

    // Set height and width of SPRITE
    height = tRect.bottom - tRect.top;
    width  = tRect.right - tRect.left;

    // Lock down both surfaces for read and write
    Lock();
    dest->Lock();

    // Set the pitch for both surfaces
    SpritePitch = m_DDSD.lPitch;
    dPitch      = dest->m_DDSD.lPitch;

    // Initialize the pointers to the upper left hand corner of the surface
    lpSprite = (BYTE*)m_DDSD.lpSurface;
    lpDest   = (BYTE*)dest->m_DDSD.lpSurface;

	// Get the color key for sprite surface
	sColorKey = (DWORD)m_ColorKey;

	// Start RGB Format switch statement
	switch(dest->m_PixelFormat.bpp)
	{
	case 8:
		// IMHO paletized modes are a thing of the past please feel free to 
		// implement this if you so desire.
		break;

	case 15:

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.


		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);

				if (sTemp != sColorKey)
				{
					dTemp = *((WORD*)lpDest);
					*((WORD*)lpDest) = (WORD)( (dTemp & 0x7BDE) >> 1 );
				}

				lpDest += 2;
				lpSprite += 2;
			}

			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				dTemp = *((DWORD*)lpDest);
				Result = dTemp;

				if ((sTemp & 0xFFFF) != sColorKey)
				{
					Result &= 0xFFFF0000;
					Result |= (dTemp & 0x7BDE) >> 1;
				}
				if ((sTemp >> 16) != sColorKey)
				{
					Result &= 0xFFFF;
					Result |= (dTemp & 0x7BDE0000) >> 1;
				}

				*((DWORD*)lpDest) = Result;
				lpDest += 4;
				lpSprite += 4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);

		break;

	case 16:

		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 2);
		lpDest   += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the Sprite width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
			width = width / 2;  //div by 2, processing 2 pixels at a time.

		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSprite);

				if (sTemp != sColorKey)
				{
					dTemp = *((WORD*)lpDest);
					*((WORD*)lpDest) = (WORD)( (dTemp & 0xF7DE) >> 1 );
				}

				lpDest += 2;
				lpSprite += 2;
			}

			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				dTemp = *((DWORD*)lpDest);
				Result = dTemp;

				if ((sTemp & 0xFFFF) != sColorKey)
				{
					Result &= 0xFFFF0000;
					Result |= (dTemp & 0xF7DE) >> 1;
				}
				if ((sTemp >> 16) != sColorKey)
				{
					Result &= 0xFFFF;
					Result |= (dTemp & 0xF7DE0000) >> 1;
				}

				*((DWORD*)lpDest) = Result;
				lpDest += 4;
				lpSprite += 4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;

	case 24:
		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 3);
		lpDest   += (lDestY * dPitch) + (lDestX * 3);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (3 * width));
		dbuf = (WORD)(dPitch - (3 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				
				if ((sTemp & 0xFFFFFF) != sColorKey)
				{
					dTemp = *((DWORD*)lpDest);

					*((WORD*)lpDest) = (WORD)((dTemp & 0xFEFE) >> 1);
					lpDest += 2;
					*lpDest = (BYTE)(((dTemp >> 16) & 0xFE) >> 1);
					lpDest++;
				}
				else
				{
					lpDest += 3;
				}

				lpSprite += 3;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;

	case 32:
		// Initialize the pointers to the first pixel in the rectangle
		lpSprite += (tRect.top * SpritePitch) + (tRect.left * 4);
		lpDest   += (lDestY * dPitch) + (lDestX * 4);

		// Set the horizontal padding
		sbuf = (WORD)(SpritePitch - (4 * width));
		dbuf = (WORD)(dPitch - (4 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSprite);
				
				if ((sTemp & 0xFFFFFF) != sColorKey)
				{
					dTemp = *((DWORD*)lpDest);

					*((WORD*)lpDest) = (WORD)((dTemp & 0xFEFE) >> 1);
					lpDest += 2;
					*lpDest = (BYTE)(((dTemp >> 16) & 0xFE) >> 1);
					lpDest += 2;
				}
				else
				{
					lpDest += 4;
				}

				lpSprite += 4;
			}while (--j > 0);
			lpDest += dbuf;
			lpSprite += sbuf;

		}while (--i > 0);
		break;
	} // End RGB Format switch statement


    UnLock();
    dest->UnLock();

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawBlkScaled
// 
// PURPOSE: Draws a block of pixels to the surface pointed to by lpCDXS.
//          The transparency color key is ignored. The width and height of the 
//          source rect are scaled by SF.   
//
// INPUT: lpCDXS  - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//        SF      - Scale factor, range 0 or >
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: DirectDraw function Blt() performs an Lock/Unlock internally.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawBlkScaled(CDXSurface* lpCDXS, LONG lDestX, LONG lDestY, RECT* srcRect, float SF)

{
	HRESULT rval;
	RECT destRect;
	RECT tmpRect;
	float w,h;
    RECT tRect;

	if(m_lpDDS == NULL)
		return -1;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Test if srcRect lies somewhere on this surface's clipRect
	if (!(ClipRect(&tRect)))
		return 0;

	// calculate the width and height of the source rect
	w = (float)(tRect.right - tRect.left);
	h = (float)(tRect.bottom - tRect.top);

	// Boundary check the Scale Factor
	if (SF < 0)
		SF = 0;

	// From srcRect build the destRect
	destRect.top    = (LONG)( ((float)(lDestY) + (h/2.0)) - ((h*SF)/2.0) );
	destRect.left   = (LONG)( ((float)(lDestX) + (w/2.0)) - ((w*SF)/2.0) );
	destRect.bottom = destRect.top  + (LONG)( h * SF );
	destRect.right  = destRect.left + (LONG)( w * SF );

	// Save off the destRect
	tmpRect = destRect;

	// Test if destRect lies somewhere on lpCDXS surface's clipRect
	if (!(lpCDXS->ClipRect(&destRect)))
		return 0;

	// Compare the returned destRect to the saved tmpRect
	if (destRect.top != tmpRect.top)
		tRect.top += (LONG)( (float)(destRect.top - tmpRect.top) / SF );

	if (destRect.left != tmpRect.left)
		tRect.left += (LONG)( (float)(destRect.left - tmpRect.left) / SF );

	if (destRect.bottom != tmpRect.bottom)
		tRect.bottom += (LONG)( (float)(destRect.bottom - tmpRect.bottom) / SF );

	if (destRect.right != tmpRect.right)
		tRect.right += (LONG)( (float)(destRect.right - tmpRect.right) / SF );

	// Use the hardware to draw the image
	rval = lpCDXS->m_lpDDS->Blt(&destRect, m_lpDDS, &tRect, DDBLT_WAIT, NULL);
	if(rval == DDERR_SURFACELOST) 
		Restore();

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawTransScaled
// 
// PURPOSE: Draws a block of pixels to the surface pointed to by lpCDXS.
//          Pixels matching the transparency color are not drawn. The width and 
//          height of the source rect are scaled by SF.      
//
// INPUT: lpCDXS  - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//        SF      - Scale factor, range 0 or >
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: DirectDraw function Blt() performs an Lock/Unlock internally.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawTransScaled(CDXSurface* lpCDXS, LONG lDestX, LONG lDestY, RECT* srcRect, float SF)
{
	HRESULT rval;
	RECT destRect;
	RECT tmpRect;
	float w,h;
    RECT tRect;

	if(m_lpDDS == NULL)
		return -1;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Test if srcRect lies somewhere on this surface's clipRect
	if (!(ClipRect(&tRect)))
		return 0;

	// calculate the width and height of the source rect
	w = (float)(tRect.right - tRect.left);
	h = (float)(tRect.bottom - tRect.top);

	// Boundary check the Scale Factor
	if (SF < 0)
		SF = 0;

	// From srcRect build the destRect
	destRect.top    = (LONG)( ((float)(lDestY) + (h/2.0)) - ((h*SF)/2.0) );
	destRect.left   = (LONG)( ((float)(lDestX) + (w/2.0)) - ((w*SF)/2.0) );
	destRect.bottom = destRect.top  + (LONG)( h * SF );
	destRect.right  = destRect.left + (LONG)( w * SF );

	// Save off the destRect
	tmpRect = destRect;

	// Test if destRect lies somewhere on lpCDXS surface's clipRect
	if (!(lpCDXS->ClipRect(&destRect)))
		return 0;

	// Compare the returned destRect to the saved tmpRect
	if (destRect.top != tmpRect.top)
		tRect.top += (LONG)( (float)(destRect.top - tmpRect.top) / SF );

	if (destRect.left != tmpRect.left)
		tRect.left += (LONG)( (float)(destRect.left - tmpRect.left) / SF );

	if (destRect.bottom != tmpRect.bottom)
		tRect.bottom += (LONG)( (float)(destRect.bottom - tmpRect.bottom) / SF );

	if (destRect.right != tmpRect.right)
		tRect.right += (LONG)( (float)(destRect.right - tmpRect.right) / SF );

	// Use the hardware to draw the image
	rval = lpCDXS->m_lpDDS->Blt(&destRect, m_lpDDS, &tRect, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	if(rval == DDERR_SURFACELOST) 
		Restore();

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawBlkStretched
// 
// PURPOSE: Draws a stretched block of pixels to the surface pointed to by lpCDXS.
//          The transparency color key is ignored. 
//
// INPUT: lpCDXS  - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//        width   - width of destination rect
//        height  - height of destination rect
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: DirectDraw function Blt() performs an Lock/Unlock internally.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawBlkStretched(CDXSurface* lpCDXS, LONG lDestX, LONG lDestY, RECT* srcRect, LONG width, LONG height)

{
	HRESULT rval;
	RECT destRect;
	RECT tmpRect;
	float w,h;
    RECT tRect;

	if(m_lpDDS == NULL)
		return -1;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Test if srcRect lies somewhere on this surface's clipRect
	if (!(ClipRect(&tRect)))
		return 0;

	// calculate the width and height of the source rect
	w = (float)(tRect.right - tRect.left);
	h = (float)(tRect.bottom - tRect.top);

	// Boundary check the the passed in width and height
	if ( (width <= 0) || (height <= 0) )
		return 0;

	// From srcRect build the destRect
	destRect.top    = lDestY; //(LONG)( ((float)(lDestY) + (h/2.0)) - ( (float)height/2.0) );
	destRect.left   = lDestX; //(LONG)( ((float)(lDestX) + (w/2.0)) - ( (float)width/2.0) );
	destRect.bottom = destRect.top  + height;
	destRect.right  = destRect.left + width;

	// Save off the destRect
	tmpRect = destRect;

	// Test if destRect lies somewhere on lpCDXS surface's clipRect
	if (!(lpCDXS->ClipRect(&destRect)))
		return 0;
/*
	// Compare the returned destRect to the saved tmpRect
	if (destRect.top != tmpRect.top)
		tRect.top += (LONG)( (float)(destRect.top - tmpRect.top) / ( (tmpRect.bottom - tmpRect.top ) / (LONG)h ) );

	if (destRect.left != tmpRect.left)
		tRect.left += (LONG)( (float)(destRect.left - tmpRect.left) / ( (tmpRect.right - tmpRect.left ) / (LONG)w ) );

	if (destRect.bottom != tmpRect.bottom)
		tRect.bottom += (LONG)( (float)(destRect.bottom - tmpRect.bottom) / ( (tmpRect.bottom - tmpRect.top ) / (LONG)h ) );

	if (destRect.right != tmpRect.right)
		tRect.right += (LONG)( (float)(destRect.right - tmpRect.right) / ( (tmpRect.right - tmpRect.left ) / (LONG)w ) );
*/
	// Use the hardware to draw the image
	rval = lpCDXS->m_lpDDS->Blt(&destRect, m_lpDDS, &tRect, DDBLT_WAIT, NULL);
	if(rval == DDERR_SURFACELOST) 
		Restore();

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawTransStretched
// 
// PURPOSE: Draws a stretched block of pixels to the surface pointed to by lpCDXS.
//          Pixels matching the transparency color key are not drawn. 
//
// INPUT: lpCDXS  - pointer to the destination surface
//        lDestX  - X location on the destination surface
//        lDestY  - Y location on the destination surface
//        srcRect - rectangle descipbing the source block of pixels to be drawn
//        width   - width of destination rect
//        height  - height of destination rect
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: DirectDraw function Blt() performs an Lock/Unlock internally.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawTransStretched(CDXSurface* lpCDXS, LONG lDestX, LONG lDestY, RECT* srcRect, LONG width, LONG height)

{
	HRESULT rval;
	RECT destRect;
	RECT tmpRect;
	float w,h;
    RECT tRect;

	if(m_lpDDS == NULL)
		return -1;

    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }


	// Test if srcRect lies somewhere on this surface's clipRect
	if (!(ClipRect(&tRect)))
		return 0;

	// calculate the width and height of the source rect
	w = (float)(tRect.right - tRect.left);
	h = (float)(tRect.bottom - tRect.top);

	// Boundary check the the passed in width and height
	if ( (width <= 0) || (height <= 0) )
		return 0;

	// From srcRect build the destRect
	destRect.top    = lDestY; //(LONG)( ((float)(lDestY) + (h/2.0)) - ( (float)height/2.0) );
	destRect.left   = lDestX; //(LONG)( ((float)(lDestX) + (w/2.0)) - ( (float)width/2.0) );
	destRect.bottom = destRect.top  + height;
	destRect.right  = destRect.left + width;

	// Save off the destRect
	tmpRect = destRect;

	// Test if destRect lies somewhere on lpCDXS surface's clipRect
	if (!(lpCDXS->ClipRect(&destRect)))
		return 0;

  /*
	// Compare the returned destRect to the saved tmpRect
	if (destRect.top != tmpRect.top)
		tRect.top += (LONG)( (float)(destRect.top - tmpRect.top) / ( (tmpRect.bottom - tmpRect.top ) / (LONG)h ) );

	if (destRect.left != tmpRect.left)
		tRect.left += (LONG)( (float)(destRect.left - tmpRect.left) / ( (tmpRect.right - tmpRect.left ) / (LONG)w ) );

	if (destRect.bottom != tmpRect.bottom)
		tRect.bottom += (LONG)( (float)(destRect.bottom - tmpRect.bottom) / ( (tmpRect.bottom - tmpRect.top ) / (LONG)h ) );

	if (destRect.right != tmpRect.right)
		tRect.right += (LONG)( (float)(destRect.right - tmpRect.right) / ( (tmpRect.right - tmpRect.left ) / (LONG)w ) );
  */
	// Use the hardware to draw the image
	rval = lpCDXS->m_lpDDS->Blt(&destRect, m_lpDDS, &tRect, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	if(rval == DDERR_SURFACELOST) 
		Restore();

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Fade
// 
// PURPOSE: Fades all or a portion of the source surface to a destination surface.
//          The fade amount is controlled by the ALPHA value.                 
//
// INPUT: dest    - a CDXSurface pointer that is the destination surface
//        lDestX  - The left hand X location on the destination surface where 
//                  you want the blt to begin.
//        lDestY  - the top Y location on the destination surface where
//                  you want the blt to begin.
//        srcRect - RECT structure that defines what you want blitted
//        ALPHA   - Alpha blending value between 0-256
//
//                  256 --> no fade, pure src image is displayed
//                  0   --> pure black, no src image
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::Fade(CDXSurface* dest, LONG lDestX, LONG lDestY, RECT* srcRect, WORD ALPHA)
{
	int register i,j;
	unsigned long dPitch, sPitch;
	int height,width;
	HRESULT rval;
	BYTE* lpSrc;
	BYTE* lpDest;
	DWORD sTemp;
	DWORD dr,dg,db;
	WORD  sbuf,dbuf;
	BOOL  oddWidth = FALSE;
    RECT tRect;


    if(srcRect == NULL)
    {
        tRect.top    = 0;
        tRect.left   = 0;
        tRect.right  = m_PixelWidth;
        tRect.bottom = m_PixelHeight;
    }
    else
    {
        tRect = *srcRect;
    }

	// Validate the inputs and clip them to m_clipRect
	if (ValidateBlt(dest, &lDestX, &lDestY, &tRect) == FALSE)
		return 0;

	// Set height and width of SPRITE
    height = tRect.bottom - tRect.top;
    width  = tRect.right - tRect.left; 

    // Lock down both surfaces for read and write
    rval = Lock();
    rval = dest->Lock();

	// Set the pitch for both surfaces
    sPitch = m_DDSD.lPitch;
    dPitch = dest->m_DDSD.lPitch;

    // Initialize the pointers to the upper left hand corner of surface
    lpSrc  = (BYTE*)m_DDSD.lpSurface;
    lpDest = (BYTE*)dest->m_DDSD.lpSurface;



	switch(dest->m_PixelFormat.bpp)
	{
	case 8:
		// IMHO paletized modes are a thing of the past please feel free to 
		// implement this if you so desire.
		break;

	case 15:
		// Initialize the pointers to the first pixel in the rectangle
		lpSrc  += (tRect.top * sPitch) + (tRect.left * 2);
		lpDest += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(sPitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
		{
			width = width / 2;  //div by 2, processing 2 pixels at a time.
		}


		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSrc);

				db = sTemp & 0x1f;
				dg = (sTemp >> 5)  & 0x1f;
				dr = (sTemp >> 10) & 0x1f;

				*((WORD*)lpDest) = (WORD)((ALPHA * db) >> 8 |
					                     ((ALPHA * dg) >> 8) << 5 |
					                     ((ALPHA * dr) >> 8) << 10);

				lpDest += 2;
				lpSrc  += 2;
			}

			//
			// 5-5-5 Fading: Read from src and write to dest
			//               Uses optimised MMX style SIMD instructions.
			//
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSrc);

				db =  ((ALPHA * (sTemp & 0x001F001F)) >> 8) & 0x001F001F;
				dg = (((ALPHA * ((sTemp >> 5)  & 0x001F001F)) >> 8) & 0x001F001F) << 5;
				dr = (((ALPHA * ((sTemp >> 10) & 0x001F001F)) >> 8) & 0x001F001F) << 10;

				*((DWORD*)lpDest) = db | dg | dr;

				lpDest += 4;
				lpSrc  += 4;

			}while (--j > 0);

			lpDest += dbuf;
			lpSrc  += sbuf;

		}while (--i > 0);
		

		break;

	case 16:

		// Initialize the pointers to the first pixel in the rectangle
		lpSrc  += (tRect.top * sPitch) + (tRect.left * 2);
		lpDest += (lDestY * dPitch) + (lDestX * 2);

		// Set the horizontal padding
		sbuf = (WORD)(sPitch - (2 * width));
		dbuf = (WORD)(dPitch - (2 * width));

		// Is the width odd or even?
		if (width % 2 == 1)
		{
			oddWidth = TRUE;
			width = (width - 1) / 2; //div by 2, processing 2 pixels at a time.
		}
		else
		{
			width = width / 2;  //div by 2, processing 2 pixels at a time.
		}


		i = height;
		do
		{
			if (oddWidth)
			{
				sTemp = *((WORD*)lpSrc);

				db = sTemp & 0x1f;
				dg = (sTemp >> 5)  & 0x3f;
				dr = (sTemp >> 11) & 0x1f;

				*((WORD*)lpDest) = (WORD)((ALPHA * db) >> 8 |
					                     ((ALPHA * dg) >> 8) << 5 |
					                     ((ALPHA * dr) >> 8) << 11);

				lpDest += 2;
				lpSrc  += 2;
			}

			//
			// 5-6-5 Fading: Read from src and write to dest
			//               Uses optimised MMX style SIMD instructions.
			//
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSrc);

				db =  ((ALPHA * (sTemp & 0x001F001F)) >> 8) & 0x001F001F;
				dg = (((ALPHA * ((sTemp >> 5)  & 0x003F003F)) >> 8) & 0x003F003F) << 5;
				dr = (((ALPHA * ((sTemp >> 11) & 0x001F001F)) >> 8) & 0x001F001F) << 11;

				*((DWORD*)lpDest) = db | dg | dr;

				lpDest += 4;
				lpSrc  += 4;

			}while (--j > 0);

			lpDest += dbuf;
			lpSrc  += sbuf;

		}while (--i > 0);

		break;

	case 24:
		// Initialize the pointers to the first pixel in the rectangle
		lpSrc  += (tRect.top * sPitch) + (tRect.left * 3);
		lpDest += (lDestY * dPitch) + (lDestX * 3);

		// Set the horizontal padding
		sbuf = (WORD)(sPitch - (3 * width));
		dbuf = (WORD)(dPitch - (3 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSrc);

				db =  (ALPHA * (sTemp & 0xFF)) >> 8;
				dg = ((ALPHA * ((sTemp >> 8)  & 0xFF)) >> 8) << 8;
				dr =  (ALPHA * ((sTemp >> 16) & 0xFF)) >> 8;

				*((WORD*)lpDest) = (WORD)(db | dg);
				lpDest += 2;
				*lpDest = (BYTE)dr;

				lpDest ++;
				lpSrc  += 3;

			}while (--j > 0);

			lpDest += dbuf;
			lpSrc  += sbuf;

		}while (--i > 0);
		break;

	case 32:
				// Initialize the pointers to the first pixel in the rectangle
		lpSrc  += (tRect.top * sPitch) + (tRect.left * 4);
		lpDest += (lDestY * dPitch) + (lDestX * 4);

		// Set the horizontal padding
		sbuf = (WORD)(sPitch - (4 * width));
		dbuf = (WORD)(dPitch - (4 * width));

		i = height;
		do
		{
			j = width;
			do
			{
				sTemp = *((DWORD*)lpSrc);

				db =  (ALPHA * (sTemp & 0xFF)) >> 8;
				dg = ((ALPHA * ((sTemp >> 8)  & 0xFF)) >> 8) << 8;
				dr =  (ALPHA * ((sTemp >> 16) & 0xFF)) >> 8;

				*((WORD*)lpDest) = (WORD)(db | dg);
				lpDest += 2;
				*lpDest = (BYTE)dr;

				lpDest += 2;
				lpSrc  += 4;

			}while (--j > 0);

			lpDest += dbuf;
			lpSrc  += sbuf;

		}while (--i > 0);
		break;
	} // End RGB Format switch statement


	UnLock();
	dest->UnLock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetFileName
// 
// PURPOSE: Returns the filename                
//
// INPUT: name - pointer to a string to hold the FileName.  This should be at 
//        least 256 bytes
//
// RETURNS: char pointer that was passed in.  Useful if you want to inline the code
//
//////////////////////////////////////////////////////////////////////////////////
char* CDXSurface::GetFileName(char* name, int len)
{ 
	if(m_FileName != NULL)
		strcpy_s(name, len, m_FileName);
	else
		name = NULL;

	return name; 
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetFontName
// 
// PURPOSE: Returns the font name                
//
// INPUT: name - pointer to a string to hold the font name.  This should be at 
//        least 256 bytes
//
// RETURNS: char pointer that was passed in.  Useful if you want to inline the code
//
//////////////////////////////////////////////////////////////////////////////////
char* CDXSurface::GetFontName(char* name, int len)
{ 
	if(m_FontName != NULL)
		strcpy_s(name, len, m_FontName);
	else
		name = NULL;

	return name; 
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetPixelFormat
// 
// PURPOSE: Fills out the RGBFORMAT structure pointed to by 'format'               
//
// INPUT: format - pointer to an RGBFORMAT structure
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXSurface::GetPixelFormat(RGBFORMAT* format)
{
	format->bpp      = m_PixelFormat.bpp;
	format->depth    = m_PixelFormat.depth;
	format->position = m_PixelFormat.position;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetColorKey
// 
// PURPOSE: Sets the source colour key. This colour will appear transparent when
//          drawn to the screen.                
//
// INPUT: col - 4 bytes value that defines the color
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::SetColorKey(DWORD col)
{
	DDCOLORKEY ddck;
    HRESULT rval;

	if(m_lpDDS == NULL)
		return -1;

	m_ColorKey = col;
	ddck.dwColorSpaceLowValue = col;
	ddck.dwColorSpaceHighValue = col;

	rval = m_lpDDS->SetColorKey(DDCKEY_SRCBLT, &ddck);

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetColorKey
// 
// PURPOSE: Sets the source colour key. The color of the pixel in the top, left 
//          corner will appear transparent when drawn to the screen.                
//
// INPUT: none
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::SetColorKey(void)
{
    DDCOLORKEY		ddck;
	DWORD			dw = CLR_INVALID;
	HRESULT			rval;

	if(m_lpDDS == NULL)
		return -1;

    ZeroMemory(&m_DDSD, sizeof(m_DDSD));
	m_DDSD.dwSize = sizeof(m_DDSD);

    // Lock the surface
	while((rval = Lock()) == DDERR_WASSTILLDRAWING);

    // Did lock succeed?
	if(SUCCEEDED(rval)) 
    {
        // Retrieve the upper left hand corner pixel
		dw = *(DWORD *)m_DDSD.lpSurface;

		if(m_DDSD.ddpfPixelFormat.dwRGBBitCount < 32)
			dw &= (1 << m_DDSD.ddpfPixelFormat.dwRGBBitCount) - 1;

		UnLock();
	}
    else
    {
        // Lock failed return error code
        return -1;
    }

	m_ColorKey = dw;
	ddck.dwColorSpaceLowValue = dw;
	ddck.dwColorSpaceHighValue = dw;

    // Set the color key for the surface
	rval = m_lpDDS->SetColorKey(DDCKEY_SRCBLT, &ddck);

    // If SetColorKey failed return error
    if( FAILED(rval) )
        return -2;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Restore
// 
// PURPOSE: Restores the DirectDrawSurface and bitmap if lost. Called internally 
//          if a Draw function fails. It will also attempt to reload the bitmap
//          if m_FileName is not NULL.             
//
// INPUT: none
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::Restore(void)
{
	HRESULT rval;

	if(m_lpDDS == NULL)
		return -1;

	// In fullscreen mode the back buffer is implicitly created.
	// The DX Restore() function cannot restore implicitly created surfaces.
	// In our case the impicit surface is the back buffer so we need to
	// restore the front buffer instead.
	if( (GetScreen()->GetBack() == this) && (GetScreen()->IsFullScreen() == TRUE) )
  {
    DETAILTRACE(0xc747a0,0);	
		rval = GetScreen()->GetFront()->Restore();
  }
	else
  {
    DETAILTRACE(0xc747a1,0);	
		rval = GetDDS()->Restore();
  };

	if( (m_FileName != NULL) && (SUCCEEDED(rval)) )
        rval = ReLoadBitmap(m_FileName);
		//rval = DDReLoadBitmap(GetDDS(), m_FileName);

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Lock
// 
// PURPOSE: Locks the surface and provides direct access to the display memory.             
//
// INPUT: none
//
// RETURNS: m_LockCount - successful
//          < 0         - failed
//
// NOTE: Remember to call UnLock when you have finished drawing to the display
//       memory. This method will properly restore any lost surfaces when called.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::Lock(void)
{
	HRESULT rval;
  DETAILTRACE(0xc744a0,0);	
    // If the surface is already locked add 1 to ref count and return success
    if( m_LockCount > 0 )
    {
        m_LockCount++;
        return m_LockCount;
    }

  DETAILTRACE(0xc744a1,0);	
	// Make sure the DX surface is valid
	if(m_lpDDS == NULL)
		return -1;

	ZeroMemory(&m_DDSD, sizeof(m_DDSD));
	m_DDSD.dwSize = sizeof(m_DDSD);
  DETAILTRACE(0xc744a2,0);	

	rval = GetDDS()->Lock(NULL, &m_DDSD, DDLOCK_WAIT, NULL);
  DETAILTRACE(0xc744a3,0);	

	while(rval == DDERR_SURFACELOST) 
  {
    DETAILTRACE(0xc744a4,0);	
		Restore();
		ZeroMemory(&m_DDSD, sizeof(m_DDSD));
		m_DDSD.dwSize = sizeof(m_DDSD);
		rval = GetDDS()->Lock(NULL, &m_DDSD, DDLOCK_WAIT, NULL);
	}
  DETAILTRACE(0xc744a5,0);	

  if( SUCCEEDED(rval) )
	{
        m_LockCount++;
		return m_LockCount;
	}
  DETAILTRACE(0xc744a6,0);	

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: UnLock
// 
// PURPOSE: UnLocks the surface and prevents access to display memory.            
//
// INPUT: none
//
// RETURNS: m_LockCount - successful
//          < 0         - failed
//
// NOTE:  Should be called after you have finished with a Lock function. This 
// method will properly restore any lost surfaces when called. 
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::UnLock(void)
{
	HRESULT rval;
	
    // If the lock ref count is > 1 just decrement it and return success
    if( m_LockCount > 1 )
    {
        m_LockCount--;
        return m_LockCount;
    }

	// Make sure the DX surface is valid
	if(m_lpDDS == NULL)
		return -1;

	rval = GetDDS()->Unlock(NULL);

	while(rval == DDERR_SURFACELOST) 
    {
		Restore();
		rval = GetDDS()->Unlock(NULL);
	}

    if( SUCCEEDED(rval) )
	{
        m_LockCount--;
		return m_LockCount;
	}

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetDC
// 
// PURPOSE: Retieves a handle of the display device context (DC) and stores it in
//          the m_DC data member. The device context is used in GDI functions to 
//          draw to the screen.            
//
// INPUT: none
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE:  Remember to call ReleaseDC when you are finished with your GDI functions.
//
//////////////////////////////////////////////////////////////////////////////////
HDC CDXSurface::GetDC(void)
{
    HRESULT rval;

    if(m_DeviceContextCount > 0)
    {
        m_DeviceContextCount++;
        return m_DC;
    }

	// Make sure the DX surface is valid
	if(m_lpDDS == NULL)
		return NULL;

	rval = m_lpDDS->GetDC(&m_DC);

    if( SUCCEEDED(rval) )
        m_DeviceContextCount++;

    return m_DC;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: ReleaseDC
// 
// PURPOSE: Releases the device context. Should be called after you have finished 
//          with a GetDC function.            
//
// INPUT: none
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE:  Should be called after you have finished with a GetDC function.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::ReleaseDC(void)
{
    HRESULT rval;
	
    // 
    if( m_DeviceContextCount > 1 )
    {
        m_DeviceContextCount--;
        return 0;
    }

	// Make sure the DX surface is valid
	if(m_lpDDS == NULL)
		return -1;

	rval = m_lpDDS->ReleaseDC(m_DC);

    if( SUCCEEDED(rval) )
    {
        m_DeviceContextCount--;
        m_DC = NULL;
    }
    else
    {
        return -1;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: ChangeFont
// 
// PURPOSE: Chooses the font to be used by the TextXY function. The FontName 
//          string should be a default Windows font name (HINT: look in your 
//          fonts directory to find a list of font names eg "Comic Sans MS"). 
//          The Width and Height specify the width and height of a single 
//          character in pixels. The Attributes member specifies the weight of 
//          the font in the range 0 through 1000. For example, 400 is normal 
//          and 700 is bold.            
//
// INPUT: FontName   - string holding the name of the font
//        Width      - width of a single character in pixels
//        Height     - height of a single character in pixels
//        Attributes - weight (boldness) of the font
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::ChangeFont(LOGFONT &lf)
{
	if (m_Font != NULL)
  {
    if(DeleteObject(m_Font) != 0)
    {
       ::ZeroMemory(m_FontName, sizeof(m_FontName));
       m_Font = NULL;
    }
    else
    {
      return -1;
    }
  }

  m_Font = CreateFontIndirect(&lf);

  if(m_Font == NULL)
    return -2;

  strcpy_s(m_FontName, sizeof(m_FontName), lf.lfFaceName);
  return (SetFont());
}

HRESULT CDXSurface::ChangeFont(const char* FontName, int Width, int Height, int Attributes,int Italic)
{
	if (m_Font != NULL)
    {		
        if (DeleteObject(m_Font) == TRUE)
        {
            ::ZeroMemory(m_FontName, sizeof(m_FontName));
            m_Font = NULL;
        }
        else
        {
            return -1;
        }
    }

	m_Font = CreateFont(Height, Width,
		0, 0,
		Attributes,
		Italic,
		FALSE,
		FALSE,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		NONANTIALIASED_QUALITY,
		VARIABLE_PITCH,
		FontName);

    if(m_Font == NULL)
        return -2;
 
    strcpy_s(m_FontName, sizeof(m_FontName), FontName);
    return (SetFont());        
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetFont
// 
// PURPOSE: Selects the the currently chosen font from ChangeFont into the DC 
//          obtained by calling GetDC().            
//
// INPUT: none
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::SetFont(void)
{
    HGDIOBJ rval;
    HDC dc;

    dc = GetDC();

    if(dc != NULL)
    {
	    rval = SelectObject(dc, m_Font);
        ReleaseDC();
    }

    if(rval == NULL)
        return -1;
    
    return NOERROR;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: TextXY
// 
// PURPOSE: Draws the text specified by pString at position X,Y in colour Col. 
//          The text background is transparent.            
//
// INPUT: x       - X location of start of the string
//        y       - Y location of start of the string
//        col     - color to draw in.
//        pString - the text string to be drawn
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::TextXY(int x, int y, COLORREF col, LPCTSTR pString)
{
    HRESULT rval;
    HDC dc;

	if( pString==NULL )
		return 0;

    dc = GetDC();

    if(dc != NULL)
    {
	    SetBkMode(dc, TRANSPARENT);
	    SetTextColor(dc, col);
	    rval = ::TextOut(dc, x, y, pString, strlen(pString));

        ReleaseDC();
    }

    if(rval == 0)
        return -1;
    
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawText
// 
// PURPOSE: Draw word wrapped text on a device context inside a user defined 
//          RECT with formatting dimensions.            
//
// INPUT: pString - the text string to be drawn
//        col     - color to draw in.
//        pRect   - bounding rect that the text will be drawn within
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawText(LPCSTR pString, COLORREF col, LPRECT pRect)
{
    HRESULT rval;
    HDC dc;

    dc = GetDC();

    if(dc != NULL)
    {
	    SetBkMode(dc, TRANSPARENT);
	    SetTextColor(dc, col);
	    rval = ::DrawText(dc, pString, strlen(pString), pRect, DT_WORDBREAK);

        ReleaseDC();
    }

    if(rval <= 0)
        return -1;
    
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Swap
// 
// PURPOSE: Swaps two integers            
//
// INPUT: a - first integer
//        b - second integer
//
// RETURNS: nothing
// 
//////////////////////////////////////////////////////////////////////////////////
void Swap(int *a, int *b)
{
	int Temp = *a;
	*a = *b;
	*b = Temp;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: PutPixel8
// 
// PURPOSE: Draws a single pixel to the surface at position X,Y in colour Col.            
//
// INPUT: X   - X location of the pixel
//        Y   - Y location of the pixel
//        Col - color of the pixel
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::PutPixel8(int X, int Y, DWORD Col)
{
    BYTE* Bitmap;

    // Perform some boundry checking
    if (X < m_ClipRect.left || X >= m_ClipRect.right)
		return 0;
	if (Y < m_ClipRect.top || Y >= m_ClipRect.bottom)
		return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();

	Bitmap += Y * m_DDSD.lPitch + X;
	*Bitmap = (BYTE)Col;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: PutPixel16
// 
// PURPOSE: Draws a single pixel to the surface at position X,Y in colour Col.            
//
// INPUT: X   - X location of the pixel
//        Y   - Y location of the pixel
//        Col - color of the pixel
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::PutPixel16(int X, int Y, DWORD Col)
{
    BYTE* Bitmap;

    // Perform some boundry checking
    if (X < m_ClipRect.left || X >= m_ClipRect.right)
		return 0;
	if (Y < m_ClipRect.top || Y >= m_ClipRect.bottom)
		return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();

    Bitmap += Y * m_DDSD.lPitch + X * 2;
    *((WORD*)Bitmap) = (WORD)Col;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: PutPixel24
// 
// PURPOSE: Draws a single pixel to the surface at position X,Y in colour Col.            
//
// INPUT: X   - X location of the pixel
//        Y   - Y location of the pixel
//        Col - color of the pixel
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::PutPixel24(int X, int Y, DWORD Col)
{
    BYTE* Bitmap;

    // Perform some boundry checking
    if (X < m_ClipRect.left || X >= m_ClipRect.right)
		return 0;
	if (Y < m_ClipRect.top || Y >= m_ClipRect.bottom)
		return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();
    
    Bitmap += Y * m_DDSD.lPitch + X * 3;
    *((WORD*)Bitmap) = (WORD)(Col & 0xFFFF);
    Bitmap += 2;
    *Bitmap = (BYTE)((Col >> 16) & 0xFF);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: PutPixel32
// 
// PURPOSE: Draws a single pixel to the surface at position X,Y in colour Col.            
//
// INPUT: X   - X location of the pixel
//        Y   - Y location of the pixel
//        Col - color of the pixel
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::PutPixel32(int X, int Y, DWORD Col)
{
    BYTE* Bitmap;

    // Perform some boundry checking
    if (X < m_ClipRect.left || X >= m_ClipRect.right)
		return 0;
	if (Y < m_ClipRect.top || Y >= m_ClipRect.bottom)
		return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();

    Bitmap += Y * m_DDSD.lPitch + X * 4;
	*((DWORD*)Bitmap) = Col; 

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: PutAAPixel8
// 
// PURPOSE: Draws a single pixel to the surface at position X,Y in colour Col.     
//
// INPUT: X     - X position of the point
//        Y     - Y position of the point
//        Col   - base color of the line
//        ALPHA - alpha blending value ranging from 0-256
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: No Alpha support, sorry
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::PutAAPixel8(int X, int Y, DWORD Col, WORD ALPHA)
{
    BYTE* Bitmap;

	if (X < m_ClipRect.left || X >= m_ClipRect.right)
		return 0;
	if (Y < m_ClipRect.top || Y >= m_ClipRect.bottom)
		return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();

	// No ALPHA support, sorry.
	Bitmap += Y * m_DDSD.lPitch + X;
	*Bitmap = (BYTE)Col;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: PutAAPixel15
// 
// PURPOSE: Draws a single pixel to the surface at position X,Y in colour Col.
//          Alpha blended by ALPHA.       
//
// INPUT: X     - X position of the point
//        Y     - Y position of the point
//        Col   - base color of the line
//        ALPHA - alpha blending value ranging from 0-256
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::PutAAPixel15(int X, int Y, DWORD Col, WORD ALPHA)
{
	DWORD db, dg, dr;
	DWORD sb, sg, sr;
	DWORD sTemp, dTemp;
    BYTE* Bitmap;

	if (X < m_ClipRect.left || X >= m_ClipRect.right)
		return 0;
	if (Y < m_ClipRect.top || Y >= m_ClipRect.bottom)
		return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();

	Bitmap += Y * m_DDSD.lPitch + X + X;

	sTemp = Col;
	dTemp = *((WORD*)Bitmap);

	sb = sTemp & 0x1f;
	db = dTemp & 0x1f;
	sg = (sTemp >> 5) & 0x1f;
	dg = (dTemp >> 5) & 0x1f;
	sr = (sTemp >> 10) & 0x1f;
	dr = (dTemp >> 10) & 0x1f;

	*((WORD*)Bitmap) =	(WORD)((ALPHA * (sb - db) >> 8) + db |
						((ALPHA * (sg - dg) >> 8) + dg) << 5 |
						((ALPHA * (sr - dr) >> 8) + dr) << 10);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: PutAAPixel16
// 
// PURPOSE: Draws a single pixel to the surface at position X,Y in colour Col.
//          Alpha blended by ALPHA.       
//
// INPUT: X     - X position of the point
//        Y     - Y position of the point
//        Col   - base color of the line
//        ALPHA - alpha blending value ranging from 0-256
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::PutAAPixel16(int X, int Y, DWORD Col, WORD ALPHA)
{
	DWORD db, dg, dr;
	DWORD sb, sg, sr;
	DWORD sTemp, dTemp;
    BYTE* Bitmap;

	if (X < m_ClipRect.left || X >= m_ClipRect.right)
		return 0;
	if (Y < m_ClipRect.top || Y >= m_ClipRect.bottom)
		return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();

	Bitmap += Y * m_DDSD.lPitch + X + X;

	sTemp = Col;
	dTemp = *((WORD*)Bitmap);

	sb = sTemp & 0x1f;
	db = dTemp & 0x1f;
	sg = (sTemp >> 5) & 0x3f;
	dg = (dTemp >> 5) & 0x3f;
	sr = (sTemp >> 11) & 0x1f;
	dr = (dTemp >> 11) & 0x1f;

	*((WORD*)Bitmap) =	(WORD)((ALPHA * (sb - db) >> 8) + db |
						((ALPHA * (sg - dg) >> 8) + dg) << 5 |
						((ALPHA * (sr - dr) >> 8) + dr) << 11);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: PutAAPixel24
// 
// PURPOSE: Draws a single pixel to the surface at position X,Y in colour Col.
//          Alpha blended by ALPHA.       
//
// INPUT: X     - X position of the point
//        Y     - Y position of the point
//        Col   - base color of the line
//        ALPHA - alpha blending value ranging from 0-256
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::PutAAPixel24(int X, int Y, DWORD Col, WORD ALPHA)
{
	DWORD db, dg, dr;
	DWORD sb, sg, sr;
	DWORD sTemp, dTemp;
	DWORD result;
    BYTE* Bitmap;

	if (X < m_ClipRect.left || X >= m_ClipRect.right)
		return 0;
	if (Y < m_ClipRect.top || Y >= m_ClipRect.bottom)
		return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();

    Bitmap += Y * m_DDSD.lPitch + (X << 1) + X;

	sTemp = Col;
	dTemp = *((DWORD*)Bitmap);

	sb = sTemp & 0xff;
	db = dTemp & 0xff;
	sg = (sTemp >> 8) & 0xff;
	dg = (dTemp >> 8) & 0xff;
	sr = (sTemp >> 16) & 0xff;
	dr = (dTemp >> 16) & 0xff;

	result =	(DWORD)((ALPHA * (sb - db) >> 8) + db |
				((ALPHA * (sg - dg) >> 8) + dg) << 8 |
				((ALPHA * (sr - dr) >> 8) + dr) << 16);

	*((WORD*)Bitmap) = (WORD)(result & 0xFFFF);
	Bitmap += 2;
	*Bitmap = (BYTE)((result >> 16) & 0xFF);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: PutAAPixel32
// 
// PURPOSE: Draws a single pixel to the surface at position X,Y in colour Col.
//          Alpha blended by ALPHA.       
//
// INPUT: X     - X position of the point
//        Y     - Y position of the point
//        Col   - base color of the line
//        ALPHA - alpha blending value ranging from 0-256
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::PutAAPixel32(int X, int Y, DWORD Col, WORD ALPHA)
{
	DWORD db, dg, dr;
	DWORD sb, sg, sr;
	DWORD sTemp, dTemp;
	DWORD result;
    BYTE* Bitmap;

	if (X < m_ClipRect.left || X >= m_ClipRect.right)
		return 0;
	if (Y < m_ClipRect.top || Y >= m_ClipRect.bottom)
		return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();

    Bitmap += Y * m_DDSD.lPitch + (X << 2);

	sTemp = Col;
	dTemp = *((DWORD*)Bitmap);

	sb = sTemp & 0xff;
	db = dTemp & 0xff;
	sg = (sTemp >> 8) & 0xff;
	dg = (dTemp >> 8) & 0xff;
	sr = (sTemp >> 16) & 0xff;
	dr = (dTemp >> 16) & 0xff;

	result =	(DWORD)((ALPHA * (sb - db) >> 8) + db |
				((ALPHA * (sg - dg) >> 8) + dg) << 8 |
				((ALPHA * (sr - dr) >> 8) + dr) << 16);

	*((DWORD*)Bitmap) = result;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetPixel8
// 
// PURPOSE: Returns the value of the pixel at position X,Y on the surface.        
//
// INPUT: X - X position of the point
//        Y - Y position of the point
//
// RETURNS: 4 bytes value of pixel
//
//////////////////////////////////////////////////////////////////////////////////
DWORD CDXSurface::GetPixel8(int X, int Y)
{
	DWORD Pixel = NULL;
	BYTE* Bitmap;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();

    Bitmap += Y * m_DDSD.lPitch + X;
	Pixel = (DWORD)(*Bitmap);

    return Pixel;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetPixel16
// 
// PURPOSE: Returns the value of the pixel at position X,Y on the surface.        
//
// INPUT: X - X position of the point
//        Y - Y position of the point
//
// RETURNS: 4 bytes value of pixel
//
//////////////////////////////////////////////////////////////////////////////////
DWORD CDXSurface::GetPixel16(int X, int Y)
{
	DWORD Pixel = NULL;
	BYTE* Bitmap;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();

    Bitmap += Y * m_DDSD.lPitch + X * 2;
	Pixel = (DWORD)(*((WORD*)Bitmap));

    return Pixel;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetPixel24
// 
// PURPOSE: Returns the value of the pixel at position X,Y on the surface.        
//
// INPUT: X - X position of the point
//        Y - Y position of the point
//
// RETURNS: 4 bytes value of pixel
//
//////////////////////////////////////////////////////////////////////////////////
DWORD CDXSurface::GetPixel24(int X, int Y)
{
	DWORD Pixel = NULL;
	BYTE* Bitmap;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();

    Bitmap += Y * m_DDSD.lPitch + X * 3;
    Pixel = (DWORD)(*((WORD*)Bitmap));
    Bitmap += 2;
	Pixel |= (DWORD)(*Bitmap) << 16;

    return Pixel;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetPixel32
// 
// PURPOSE: Returns the value of the pixel at position X,Y on the surface.        
//
// INPUT: X - X position of the point
//        Y - Y position of the point
//
// RETURNS: 4 bytes value of pixel
//
//////////////////////////////////////////////////////////////////////////////////
DWORD CDXSurface::GetPixel32(int X, int Y)
{
	DWORD Pixel = NULL;
	BYTE* Bitmap;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    // This function is inline, so no function call penalty
    Bitmap = (BYTE*)GetSurfacePointer();

    Bitmap += Y * m_DDSD.lPitch + X * 4;
    Pixel = *((DWORD*)Bitmap);

    return Pixel;
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: Rect
// 
// PURPOSE: Draws a rectangle on the surface with a top-left coordinate at X1,Y1 
//          and bottom-right coordinate at X2,Y2 in colour Col.        
//
// INPUT: X1  - first X position of the point
//        Y1  - first Y position of the point
//        X2  - second X position of the point
//        Y2  - second Y position of the point
//        Col - color of the rectangle
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::Rect(int X1, int Y1, int X2, int Y2, DWORD Col)
{
    HRESULT rval;

    // Lock the surface once instead of 4 times
    rval = Lock();

    // If we couldn't lock the surface return failure code
    if( FAILED(rval) )
        return -1;

    // Draw the box
	HLine(X1,X2,Y1,Col);
	HLine(X1,X2,Y2,Col);

	VLine(Y1,Y2,X1,Col);
	VLine(Y1,Y2,X2,Col);

    // Unlock the surface
    rval = UnLock();

    // If Unlock failed return failure code
    if( FAILED(rval) )
        return -2;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: RoundedRect
// 
// PURPOSE: Draws a rectanle with rounded corners.  The radius of the arc at 
//          the corners is determined by Radius and the color is determined by Col.        
//
// INPUT: X1     - first X position of the point
//        Y1     - first Y position of the point
//        X2     - second X position of the point
//        Y2     - second Y position of the point
//        Radius - radius of rounded corners
//        Col    - color of the rectangle
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: Thank Micheal Abrash for this routine.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::RoundedRect(int X1, int Y1, int X2, int Y2, int Radius, DWORD Col)
{
	int majorAxis, minorAxis, tmp;
	double radiusSqrd = (double)Radius * Radius;
    HRESULT rval;

    // Lock the surface once instead of multiple times
    rval = Lock();

    // If we couldn't lock the surface return failure code
    if( FAILED(rval) )
        return -1;

	// Determine which is left and right by value
    if (X1 > X2)
    {
        tmp = X1;
        X1  = X2;
		X2  = tmp;
    }

	if (Y1 > Y2)
    {
        tmp = Y1;
        Y1  = Y2;
		Y2  = tmp;
    }

	// Draw the horizontal lines
	HLine(X1+Radius,X2-Radius,Y1,Col);
	HLine(X1+Radius,X2-Radius,Y2,Col);

	// Draw the vertical lines
	VLine(Y1+Radius,Y2-Radius,X1,Col);
	VLine(Y1+Radius,Y2-Radius,X2,Col);

	majorAxis = 0;
	minorAxis = Radius;


	do
	{
		// Upper right quadrant
		PutPixel(X2-Radius+majorAxis, Y1+Radius-minorAxis, Col);
		PutPixel(X2-Radius+minorAxis, Y1+Radius-majorAxis, Col);

		// Upper left quadrant
		PutPixel(X1+Radius-minorAxis, Y1+Radius-majorAxis, Col);
		PutPixel(X1+Radius-majorAxis, Y1+Radius-minorAxis, Col);

		// Lower right quadrant
		PutPixel(X2-Radius+majorAxis, Y2-Radius+minorAxis, Col);
		PutPixel(X2-Radius+minorAxis, Y2-Radius+majorAxis, Col);

		// Lower left quadrant
		PutPixel(X1+Radius-majorAxis, Y2-Radius+minorAxis, Col);
		PutPixel(X1+Radius-minorAxis, Y2-Radius+majorAxis, Col);
		

		majorAxis++;
		minorAxis = (int)(sqrt(radiusSqrd - ((double)majorAxis * majorAxis)) + 0.5);

	}while (majorAxis <= minorAxis);

	// Unlock the surface
    rval = UnLock();

    // If Unlock failed return failure code
    if( FAILED(rval) )
        return -2;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: FillRect
// 
// PURPOSE: Draws a filled rectangle on the surface with a top-left coordinate at 
//          X1,Y1 and bottom-right coordinate at X2,Y2 in colour Col.        
//
// INPUT: X1     - first X position of the point
//        Y1     - first Y position of the point
//        X2     - second X position of the point
//        Y2     - second Y position of the point
//        Col    - color of the rectangle
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::FillRect(int X1, int Y1, int X2, int Y2, DWORD Col)
{
	DDBLTFX ddbltfx;
	RECT Rect = { X1, Y1, X2, Y2 };
    HRESULT rval;

	//  Clip the rect to m_clipRect
	if ( ClipRect(&Rect) == FALSE )
		return 0;
	
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = Col;

	rval = m_lpDDS->Blt(&Rect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &ddbltfx);

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Line
// 
// PURPOSE: Draws a straight line on the surface from X1,Y1 to X2,Y2 in colour Col.        
//
// INPUT: X1     - first X position of the point
//        Y1     - first Y position of the point
//        X2     - second X position of the point
//        Y2     - second Y position of the point
//        Col    - color of the rectangle
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::Line(int X1, int Y1, int X2, int Y2, DWORD Col)
{
	double xStep, yStep, X, Y;
	int xLength, yLength, xCount, yCount;
    HRESULT rval;

    // Lock the surface once instead of multiple times
    rval = Lock();

    // If we couldn't lock the surface return failure code
    if( FAILED(rval) )
        return -1;

    // If the line is horizontal or vertical use the fast version.
	if (X1 == X2)
	{
		rval = VLine(Y1,Y2,X1,Col);
        rval = UnLock();
		return rval;
	}
	else if (Y1 == Y2)
	{
		rval = HLine(X1,X2,Y1,Col);
        rval = UnLock();
		return rval;
	}

	xLength = abs(X2 - X1);
	yLength = abs(Y2 - Y1);

	if(xLength == 0) VLine(Y1, Y2, X1, Col);
	else if(yLength == 0) HLine(X1, X2, Y1, Col);

	else if(xLength > yLength)
	{
		if(X1 > X2)
		{
			Swap(&X1, &X2);
			Swap(&Y1, &Y2);
		}

		yStep = (double)(Y2 - Y1) / (double)(X2 - X1);
		Y = Y1;

		for(xCount = X1; xCount <= X2; xCount++)
		{
			PutPixel(xCount, (int)Y, Col);
			Y += yStep;
		}
	}
	else
	{
		if(Y1 > Y2)
		{
			Swap(&X1, &X2);
			Swap(&Y1, &Y2);
		}

		xStep = (double)(X2 - X1) / (double)(Y2 - Y1);
		X = X1;

		for(yCount = Y1; yCount <= Y2; yCount++)
		{
			PutPixel((int)X, yCount, Col);
			X += xStep;
		}
	}

    // Unlock the surface
    rval = UnLock();

    // If Unlock failed return failure code
    if( FAILED(rval) )
        return -2;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: AALine
// 
// PURPOSE: Draws an antialiased straight line on the surface from X1,Y1 to X2,Y2 
//          in colour Col.            
//
// INPUT: X1  - X position of the first endpoint
//        Y1  - Y position of the first endpoint
//        X2  - X position of the second endpoint
//        Y2  - Y position of the second endpoint
//        Col - base color of the line
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::AALine(int X1, int Y1, int X2, int Y2, DWORD Col)
{
	WORD intensityShift, errorAdj, errorAcc;
	WORD errorAccTemp, weighting, weightComplimentMask;
	LONG deltaX, deltaY, temp,xDir;
    HRESULT rval;

    // Lock the surface once instead of multiple times
    rval = Lock();

    // If we couldn't lock the surface return failure code
    if( FAILED(rval) )
        return -1;

    if (X1 == X2)
	{
		rval = VLine(Y1,Y2,X1,Col);
        rval = UnLock();
		return rval;
	}
	else if (Y1 == Y2)
	{
		rval = HLine(X1,X2,Y1,Col);
        rval = UnLock();
		return rval;
	}

	// Swap the points based on height
	if(Y1 > Y2)
	{
		temp = Y1; Y1 = Y2; Y2 = temp;
		temp = X1; X1 = X2; X2 = temp;
	}

	// Plot the initial pixel in the true color
	PutPixel(X1,Y1,Col);

	// Determine the direction of the slope
	if ((deltaX = X2 - X1) >= 0)
	{
		xDir = 1; // going right --->
	}
	else
	{
		xDir = -1; // going left <---
		deltaX = -deltaX;
	}


	// Calculate the different in Y
	deltaY = Y2 - Y1;


	// Special case 45 degree line
    if ( ( (deltaX < 0) ? deltaX * -1 : deltaX ) == deltaY)
	{
        rval = Line(X1,Y1,X2,Y1,Col);
        rval = UnLock();
		return rval;
	}

	errorAcc = 0;
	intensityShift = 8;
	weightComplimentMask = 256 - 1;


	// 256 = full color
	// 0   = pure black
	if (deltaY > deltaX)
	{
		errorAdj = (WORD)(((DWORD)deltaX << 16) / (DWORD)deltaY);

		while(--deltaY)
		{
			errorAccTemp = errorAcc;
			errorAcc += errorAdj;
			if (errorAcc <= errorAccTemp)
			{
				X1 += xDir;
			}
			Y1++;
			weighting = errorAcc >> intensityShift;
			PutAAPixel(X1,Y1,Col, weighting ^ weightComplimentMask);
			PutAAPixel(X1 + xDir, Y1, Col, weighting);
		}

		// Plot the final pixel in the solid color
		PutPixel(X2,Y2,Col);
	}
	else
	{
		errorAdj = (WORD)(((DWORD)deltaY << 16) / (DWORD)deltaX);

		while(--deltaX)
		{
			errorAccTemp = errorAcc;
			errorAcc += errorAdj;
			if (errorAcc <= errorAccTemp)
			{
				Y1++;
			}
			X1 += xDir;
			weighting = errorAcc >> intensityShift;
			PutAAPixel(X1,Y1,Col, weighting ^ weightComplimentMask);
			PutAAPixel(X1, Y1 + 1, Col, weighting);
		}

		// Plot the final pixel in the solid color
		PutPixel(X2,Y2,Col);
	}

    // Unlock the surface
    rval = UnLock();

    // If Unlock failed return failure code
    if( FAILED(rval) )
        return -2;

    return 0;

}

//////////////////////////////////////////////////////////////////////////////////
// NAME: VLine8
// 
// PURPOSE: Draws a vertical line on the surface from Y1 to Y2 at X in colour Col.            
//
// INPUT: Y1  - Y position of the first endpoint
//        Y2  - Y position of the second endpoint
//        X   - X position of the both endpoints
//        Col - color of the line
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::VLine8(int Y1, int Y2, int X, DWORD Col)
{
    int Length,top,bottom;
    BYTE* Pixel;

    // Do some quick bounds checking
    if (X < m_ClipRect.left)
        return 0;

    if (X >= m_ClipRect.right)
        return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    Pixel = (BYTE*)m_DDSD.lpSurface;

	// Determine which is top and bottom by the values
    if (Y1 > Y2)
    {
        top = Y2;
        bottom = Y1 + 1;
    }
    else
    {
        top = Y1;
        bottom = Y2 + 1;
    }

    if (top >= m_ClipRect.bottom)
        return 0;

    if (bottom < m_ClipRect.top)
        return 0;

    // Clip the line
    if (top < m_ClipRect.top)
        top = m_ClipRect.top;

    if (bottom >= m_ClipRect.bottom)
        bottom = m_ClipRect.bottom - 1;

    Pixel += top * m_DDSD.lPitch;
    Length = bottom - top;

    // Draw the line
	Pixel += X;
	do
	{
		*Pixel = (BYTE)Col;
		Pixel += m_DDSD.lPitch;
		Length--;
	}while (Length > 0);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: VLine16
// 
// PURPOSE: Draws a vertical line on the surface from Y1 to Y2 at X in colour Col.            
//
// INPUT: Y1  - Y position of the first endpoint
//        Y2  - Y position of the second endpoint
//        X   - X position of the both endpoints
//        Col - color of the line
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::VLine16(int Y1, int Y2, int X, DWORD Col)
{
    int Length,top,bottom;
    BYTE* Pixel;

    // Do some quick bounds checking
    if (X < m_ClipRect.left)
        return 0;

    if (X >= m_ClipRect.right)
        return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    Pixel = (BYTE*)m_DDSD.lpSurface;

	// Determine which is top and bottom by the values
    if (Y1 > Y2)
    {
        top = Y2;
        bottom = Y1 + 1;
    }
    else
    {
        top = Y1;
        bottom = Y2 + 1;
    }

    if (top >= m_ClipRect.bottom)
        return 0;

    if (bottom < m_ClipRect.top)
        return 0;

    // Clip the line
    if (top < m_ClipRect.top)
        top = m_ClipRect.top;

    if (bottom >= m_ClipRect.bottom)
        bottom = m_ClipRect.bottom - 1;

    Pixel += top * m_DDSD.lPitch;
    Length = bottom - top;

    // Draw the line
	Pixel += X << 1;

	do
	{
		*((WORD*)Pixel) = (WORD)Col;
		Pixel += m_DDSD.lPitch;
		Length--;
	}while(Length > 0);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: VLine24
// 
// PURPOSE: Draws a vertical line on the surface from Y1 to Y2 at X in colour Col.            
//
// INPUT: Y1  - Y position of the first endpoint
//        Y2  - Y position of the second endpoint
//        X   - X position of the both endpoints
//        Col - color of the line
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::VLine24(int Y1, int Y2, int X, DWORD Col)
{
    int Length,top,bottom;
	DWORD dwCol;
    BYTE* Pixel;

    // Do some quick bounds checking
    if (X < m_ClipRect.left)
        return 0;

    if (X >= m_ClipRect.right)
        return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    Pixel = (BYTE*)m_DDSD.lpSurface;

	// Determine which is top and bottom by the values
    if (Y1 > Y2)
    {
        top = Y2;
        bottom = Y1 + 1;
    }
    else
    {
        top = Y1;
        bottom = Y2 + 1;
    }

    if (top >= m_ClipRect.bottom)
        return 0;

    if (bottom < m_ClipRect.top)
        return 0;

    // Clip the line
    if (top < m_ClipRect.top)
        top = m_ClipRect.top;

    if (bottom >= m_ClipRect.bottom)
        bottom = m_ClipRect.bottom - 1;

    Pixel += top * m_DDSD.lPitch;
    Length = bottom - top;

    // Draw the line
	Pixel += X + X + X;

	dwCol = Col & 0xFFFF;
	Col = (Col >> 16) & 0xFF;
	do
	{
		*((WORD*)Pixel) = (WORD)dwCol;
		Pixel += 2;
		*Pixel = (BYTE)Col;
		Pixel += m_DDSD.lPitch - 2;
		Length--;
	}while(Length > 0);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: VLine32
// 
// PURPOSE: Draws a vertical line on the surface from Y1 to Y2 at X in colour Col.            
//
// INPUT: Y1  - Y position of the first endpoint
//        Y2  - Y position of the second endpoint
//        X   - X position of the both endpoints
//        Col - color of the line
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::VLine32(int Y1, int Y2, int X, DWORD Col)
{
    int Length,top,bottom;
//	DWORD dwCol;
    BYTE* Pixel;

    // Do some quick bounds checking
    if (X < m_ClipRect.left)
        return 0;

    if (X >= m_ClipRect.right)
        return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    Pixel = (BYTE*)m_DDSD.lpSurface;

	// Determine which is top and bottom by the values
    if (Y1 > Y2)
    {
        top = Y2;
        bottom = Y1 + 1;
    }
    else
    {
        top = Y1;
        bottom = Y2 + 1;
    }

    if (top >= m_ClipRect.bottom)
        return 0;

    if (bottom < m_ClipRect.top)
        return 0;

    // Clip the line
    if (top < m_ClipRect.top)
        top = m_ClipRect.top;

    if (bottom >= m_ClipRect.bottom)
        bottom = m_ClipRect.bottom - 1;

    Pixel += top * m_DDSD.lPitch;
    Length = bottom - top;

    // Draw the line
	Pixel += X << 2;

//	dwCol = Col & 0xFFFF;
//	Col = (Col >> 16) & 0xFF;
	do
	{
//		*((DWORD*)Pixel) = dwCol;
		*((DWORD*)Pixel)=Col;
		Pixel += m_DDSD.lPitch;
		Length--;
	}while(Length > 0);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: HLine8
// 
// PURPOSE: Draws a horizontal line on the surface from X1 to X2 at Y in colour Col.            
//
// INPUT: X1  - X position of the first endpoint
//        X2  - X position of the second endpoint
//        Y   - Y position of the both endpoints
//        Col - color of the line
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::HLine8(int X1, int X2, int Y, DWORD Col)
{
    int Length,left,right;
	int i;
    DWORD dwCol;
    BYTE* Pixel;

    // Do some quick bounds checking
    if (Y < m_ClipRect.top)
        return 0;

    if (Y >= m_ClipRect.bottom)
        return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    Pixel = (BYTE*)m_DDSD.lpSurface;

	// Determine which is left and right by value
    if (X1 > X2)
    {
        left = X2;
        right = X1 + 1;
    }
    else
    {
        left = X1;
        right = X2 + 1;
    }

    if (left >= m_ClipRect.right)
        return 0;

    if (right < m_ClipRect.left)
        return 0;

    // Clip the line
    if (left < m_ClipRect.left)
            left = m_ClipRect.left;

    if (right >= m_ClipRect.right)
            right = m_ClipRect.right - 1;

	// Calculate the length of the line
    Length = right - left;
    Pixel += Y * m_DDSD.lPitch;

	Pixel += left;
	i = Length % 4;
	Length -= i;
	for (i; i > 0; i--)
	{
		*Pixel = (BYTE)Col;
		Pixel++;
	}
	if (Length > 3)
	{
		dwCol = Col | (Col << 8) | (Col << 16) | (Col << 24);
		do
		{
			*((DWORD*)Pixel) = dwCol;
			Pixel += 4;
			Length -= 4;
		}while(Length > 0);
	}

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: HLine16
// 
// PURPOSE: Draws a horizontal line on the surface from X1 to X2 at Y in colour Col.            
//
// INPUT: X1  - X position of the first endpoint
//        X2  - X position of the second endpoint
//        Y   - Y position of the both endpoints
//        Col - color of the line
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::HLine16(int X1, int X2, int Y, DWORD Col)
{
    int Length,left,right;
	int i;
    DWORD dwCol;
    BYTE* Pixel;

    // Do some quick bounds checking
    if (Y < m_ClipRect.top)
        return 0;

    if (Y >= m_ClipRect.bottom)
        return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    Pixel = (BYTE*)m_DDSD.lpSurface;

	// Determine which is left and right by value
    if (X1 > X2)
    {
        left = X2;
        right = X1 + 1;
    }
    else
    {
        left = X1;
        right = X2 + 1;
    }

    if (left >= m_ClipRect.right)
        return 0;

    if (right < m_ClipRect.left)
        return 0;

    // Clip the line
    if (left < m_ClipRect.left)
            left = m_ClipRect.left;

    if (right >= m_ClipRect.right)
            right = m_ClipRect.right - 1;

	// Calculate the length of the line
    Length = right - left;
    Pixel += Y * m_DDSD.lPitch;

	Pixel += left << 1;
	i = Length % 2;
	Length -= i;
	for (i; i > 0; i--)
	{
		*((WORD*)Pixel) = (WORD)Col;
		Pixel += 2;
	}
	if (Length > 1)
	{
		dwCol = Col | (Col << 16);
		do
		{
			*((DWORD*)Pixel) = dwCol;
			Pixel += 4;
			Length -= 2;
		}while(Length > 0);
	}

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: HLine24
// 
// PURPOSE: Draws a horizontal line on the surface from X1 to X2 at Y in colour Col.            
//
// INPUT: X1  - X position of the first endpoint
//        X2  - X position of the second endpoint
//        Y   - Y position of the both endpoints
//        Col - color of the line
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::HLine24(int X1, int X2, int Y, DWORD Col)
{
    int Length,left,right;
    DWORD dwCol;
    BYTE* Pixel;

    // Do some quick bounds checking
    if (Y < m_ClipRect.top)
        return 0;

    if (Y >= m_ClipRect.bottom)
        return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    Pixel = (BYTE*)m_DDSD.lpSurface;

	// Determine which is left and right by value
    if (X1 > X2)
    {
        left = X2;
        right = X1 + 1;
    }
    else
    {
        left = X1;
        right = X2 + 1;
    }

    if (left >= m_ClipRect.right)
        return 0;

    if (right < m_ClipRect.left)
        return 0;

    // Clip the line
    if (left < m_ClipRect.left)
            left = m_ClipRect.left;

    if (right >= m_ClipRect.right)
            right = m_ClipRect.right - 1;

	// Calculate the length of the line
    Length = right - left;
    Pixel += Y * m_DDSD.lPitch;

	Pixel += left + left + left;

	dwCol = Col & 0xFFFF;
	Col = (Col >> 16) & 0xFF;
	do
	{
		*((WORD*)Pixel) = (WORD)dwCol;
		Pixel += 2;
		*Pixel = (BYTE)Col;
		Pixel++;
		Length--;
	}while(Length > 0);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: HLine32
// 
// PURPOSE: Draws a horizontal line on the surface from X1 to X2 at Y in colour Col.            
//
// INPUT: X1  - X position of the first endpoint
//        X2  - X position of the second endpoint
//        Y   - Y position of the both endpoints
//        Col - color of the line
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::HLine32(int X1, int X2, int Y, DWORD Col)
{
    int Length,left,right;
//    DWORD dwCol;
    BYTE* Pixel;

    // Do some quick bounds checking
    if (Y < m_ClipRect.top)
        return 0;

    if (Y >= m_ClipRect.bottom)
        return 0;

    // Debug build only check to be sure the surface is locked
    assert(m_LockCount > 0);

    Pixel = (BYTE*)m_DDSD.lpSurface;

	// Determine which is left and right by value
    if (X1 > X2)
    {
        left = X2;
        right = X1 + 1;
    }
    else
    {
        left = X1;
        right = X2 + 1;
    }

    if (left >= m_ClipRect.right)
        return 0;

    if (right < m_ClipRect.left)
        return 0;

    // Clip the line
    if (left < m_ClipRect.left)
            left = m_ClipRect.left;

    if (right >= m_ClipRect.right)
            right = m_ClipRect.right - 1;

	// Calculate the length of the line
    Length = right - left;
    Pixel += Y * m_DDSD.lPitch;

	Pixel += left << 2;

//	dwCol = Col & 0xFFFF;
//	Col = (Col >> 16) & 0xFF;
	do
	{
//		*((DWORD*)Pixel) = dwCol;
		*((DWORD*)Pixel)=Col;
		Pixel += 4;
		Length--;
	}while(Length > 0);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Circle
// 
// PURPOSE: Draws a circle on the surface at position X,Y with Radius in colour Col.            
//
// INPUT: X      - X position of the circle center
//        Y      - Y position of the circle center
//        Radius - radius of the circle
//        Col    - color of the line
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: Thank Micheal Abrash for this routine.
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::Circle(int X, int Y, int Radius, DWORD Col)
{
	int majorAxis, minorAxis;
	double radiusSqrd = (double)Radius * Radius;
    HRESULT rval;

    // Lock the surface once instead of multiple times
    rval = Lock();

    // If we couldn't lock the surface return failure code
    if( FAILED(rval) )
        return -1;

	majorAxis = 0;
	minorAxis = Radius;

	do
	{
		PutPixel(X+majorAxis, Y-minorAxis, Col);
		PutPixel(X-majorAxis, Y-minorAxis, Col);
		PutPixel(X+majorAxis, Y+minorAxis, Col);
		PutPixel(X-majorAxis, Y+minorAxis, Col);
		PutPixel(X+minorAxis, Y-majorAxis, Col);
		PutPixel(X-minorAxis, Y-majorAxis, Col);
		PutPixel(X+minorAxis, Y+majorAxis, Col);
		PutPixel(X-minorAxis, Y+majorAxis, Col);

		majorAxis++;
		minorAxis = (int)(sqrt(radiusSqrd - ((double)majorAxis * majorAxis)) + 0.5);

	}while (majorAxis <= minorAxis);


    // Unlock the surface
    rval = UnLock();

    // If Unlock failed return failure code
    if( FAILED(rval) )
        return -2;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: FillCircle
// 
// PURPOSE: Draws a filled circle on the surface at position X,Y with Radius in colour Col.            
//
// INPUT: X      - X position of the circle center
//        Y      - Y position of the circle center
//        Radius - radius of the circle
//        Col    - color of the line
//
// RETURNS: 0 or > - successful
//          < 0    - failed
//
// NOTE: Thank Micheal Abrash for this routine.
// 
////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::FillCircle(int X, int Y, int Radius, DWORD Col)
{
    int Y1,dx;
    int top, bot;
	double R;
    HRESULT rval;

    // Perform a little bounds checking
    if ((top = Y - Radius) < m_ClipRect.top)
        top = m_ClipRect.top;
    else if (top > m_ClipRect.bottom)
        return 0;

    if ((bot = Y + Radius) > m_ClipRect.bottom)
        bot = m_ClipRect.bottom;
    else if (bot < m_ClipRect.top )
        return 0;

    // Lock the surface once instead of multiple times
    rval = Lock();

    // If we couldn't lock the surface return failure code
    if( FAILED(rval) )
        return -1;

    R = (double)Radius * Radius;

    for(Y1=top; Y1 < bot; Y1++)
    {
        dx = (int)(sqrt(R - ((double)((Y-Y1)*(Y-Y1)))) + 0.5);
		HLine(X-dx, X+dx, Y1, Col);
    }

    // Unlock the surface
    rval = UnLock();

    // If Unlock failed return failure code
    if( FAILED(rval) )
        return -2;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Fill
// 
// PURPOSE: Fills the surface with the specified colour            
//
// INPUT: FillColor - color to fill with
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::Fill(DWORD FillColor)
{
	DDBLTFX ddBltFx;
    HRESULT rval;

	ddBltFx.dwSize = sizeof(DDBLTFX);
	ddBltFx.dwFillColor = FillColor;
	rval = m_lpDDS->Blt(NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &ddBltFx);

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetSurfaceDescriptor
// 
// PURPOSE:        
//
// INPUT: lpddsd - 
//
// RETURNS: nothing
// 
//////////////////////////////////////////////////////////////////////////////////
void CDXSurface::GetSurfaceDescriptor(CDX_LPDDSURFACEDESC lpddsd)
{
    *lpddsd = m_DDSD;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: ReLoadBitmap
// 
// PURPOSE:          
//
// INPUT: none
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::ReLoadBitmap(const char* fileName)
{
	CDX_LPDIRECTDRAWSURFACE lpDDS;

    CDXImage* image;
    HRESULT rval;
    RECT dRect;

    dRect.left   = 0;
    dRect.top    = 0;
    dRect.right  = GetWidth();
    dRect.bottom = GetHeight();

    // If the filename was not specified use m_FileName
	if(fileName == NULL) 
    {
        fileName = m_FileName;
        if(fileName == NULL)
		    return -1;
    }

    // Create the CDXImage object
    image = new CDXImage();

    // Load the file to a DX surface
    lpDDS = image->GetImage(GetScreen()->GetDD(), fileName, CDXMEM_SYSTEMONLY, CDXIMAGE_UNKNOWN);

    // Delete the CDXImage object
    delete image;

    // Check if the creation was successful
	if(lpDDS == NULL) 
		return -2;

    // Blt the temp surface to the internal class surface.
    rval = m_lpDDS->Blt(&dRect, lpDDS, &dRect, DDBLT_WAIT, NULL);

    lpDDS->Release();
    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SaveAsBMP
// 
// PURPOSE:          
//
// INPUT: none
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::SaveAsBMP(const char * szFilename)
{
	HANDLE file_out;
    HRESULT rval;
    DWORD numwrite;
    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER bi;
    DWORD outpixel;
    int outbyte;
    BYTE * WriteBuffer; 
    int BufferIndex;
    int loop, loop2;
    int Width, Height, Pitch;
    WORD i, padding;

	//Make sure its not a NULL filename
	if(szFilename == NULL)
		return -1;

	// Attempt to create the output
	file_out = CreateFile(szFilename,
				          GENERIC_WRITE,
				          FILE_SHARE_WRITE,
				          NULL,
				          CREATE_ALWAYS,
				          FILE_ATTRIBUTE_NORMAL,
				          NULL);

	if (file_out == INVALID_HANDLE_VALUE)
 		 return -2;

    // First we need to fill out the ddsdription of the surface
    Lock();
    UnLock();

    // Setup output buffer stuff, since Windows has paging and we're in flat mode,
    // I just made it as big as the bitmap 
    BufferIndex = 0;
    Width       = m_DDSD.dwWidth;
    Height      = m_DDSD.dwHeight;
    Pitch       = m_DDSD.lPitch;
    WriteBuffer = new BYTE [((Width * 3) + 3) * Height];


    // Write the file header
    ((char *)&(fh.bfType))[0] = 'B';
    ((char *)&(fh.bfType))[1] = 'M';
    fh.bfSize          = (long)(sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + Width*Height*3); //Size in BYTES
    fh.bfReserved1     = 0;
    fh.bfReserved2     = 0;
    fh.bfOffBits       = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    bi.biSize          = sizeof(BITMAPINFOHEADER);
    bi.biWidth         = Width;
    bi.biHeight        = Height;
    bi.biPlanes        = 1;
    bi.biBitCount      = 24;
    bi.biCompression   = BI_RGB;
    bi.biSizeImage     = 0;
    bi.biXPelsPerMeter = 10000;
    bi.biYPelsPerMeter = 10000;
    bi.biClrUsed       = 0;
    bi.biClrImportant  = 0;

    WriteFile(file_out, (char *) &fh, sizeof(BITMAPFILEHEADER), &numwrite, NULL);
    WriteFile(file_out, (char *) &bi, sizeof(BITMAPINFOHEADER), &numwrite, NULL);

    // 32 bit surfaces
    if (m_DDSD.ddpfPixelFormat.dwRGBBitCount==32)	
    {

        //lock the surface and start filling the output buffer
        rval = Lock();
        if ( FAILED(rval) )
        {
	        delete [] WriteBuffer;
	        return -3;
        }

        BYTE* Bitmap_in = (BYTE*)m_DDSD.lpSurface;

        // Determine the padding at the end of a row.
        padding = (Width * 3) % 4;
        if(padding > 0)
            padding = 4 - padding;

        // Loop bottom up
        for (loop =Height-1;loop>=0;loop--)
        {
            for (loop2=0;loop2<Width;loop2++)
            {
                // Load a WORD
                outpixel = *((DWORD *)(Bitmap_in + loop2*4 + loop * Pitch));

                // Load up the Blue component and output it
                outbyte = (((outpixel)&0x000000ff));
                WriteBuffer [BufferIndex++] = outbyte;

                // Load up the green component and output it 
                outbyte = (((outpixel>>8)&0x000000ff)); 
                WriteBuffer [BufferIndex++] = outbyte;

                // Load up the red component and output it 
                outbyte = (((outpixel>>16)&0x000000ff));
                WriteBuffer [BufferIndex++] = outbyte;
            }

            for(i=0; i<padding; i++)
                WriteBuffer[BufferIndex++] = (CHAR)0x00;
        }


        // At this point the buffer should be full, so just write it out
        WriteFile (file_out, WriteBuffer, BufferIndex, &numwrite, NULL);

        // Now unlock the surface and we're done
        UnLock();

    }

    // 24 bit surfaces
    else if (m_DDSD.ddpfPixelFormat.dwRGBBitCount==24)
    {
        //lock the surface and start filling the output buffer
        rval = Lock();
        if ( FAILED(rval) )
        {
	        delete [] WriteBuffer;
	        return -3;
        }

        BYTE* Bitmap_in = (BYTE*)m_DDSD.lpSurface;

        // Determine the padding at the end of a row.
        padding = (Width * 3) % 4;
        if(padding > 0)
            padding = 4 - padding;

        // Loop bottom up
        for (loop =Height-1;loop>=0;loop--)  
        {
            for (loop2=0;loop2<Width;loop2++)
            {
                // Load up the Blue component and output it
                WriteBuffer[BufferIndex++] = *(Bitmap_in + loop2*3   + loop * Pitch); //Bug fix 6-5

                // Load up the green component and output it 
                WriteBuffer[BufferIndex++] = *(Bitmap_in + loop2*3+1 + loop * Pitch); //Bug fix 6-5

                // Load up the red component and output it 
                WriteBuffer[BufferIndex++] = *(Bitmap_in + loop2*3+2 + loop * Pitch);
            }

            for(i=0; i<padding; i++)
                WriteBuffer[BufferIndex++] = (CHAR)0x00;
        }

        // At this point the buffer should be full, so just write it out
        WriteFile(file_out, WriteBuffer, BufferIndex, &numwrite, NULL);

        // Now unlock the surface and we're done
        UnLock();
    }

    // 16 bit surfaces
    else if (m_DDSD.ddpfPixelFormat.dwRGBBitCount==16)	
    {
        // lock the surface and start filling the output buffer
        rval = Lock();
        if ( FAILED(rval) )
        {
	        delete [] WriteBuffer;
	        return -3;
        }

        BYTE* Bitmap_in = (BYTE*)m_DDSD.lpSurface;

        // Determine the padding at the end of a row.
        padding = (Width * 3) % 4;
        if(padding > 0)
            padding = 4 - padding;

        // If the green bitmask equals 565 mode, do 16-bit mode, otherwise do 15-bit mode
        // NOTE: We are reversing the component order (ie. BGR instead of RGB)
	    //    and we are outputting it bottom up because BMP files are backwards and upside down.



        if (m_DDSD.ddpfPixelFormat.dwGBitMask == 0x07E0)
        {
            // Loop bottom up
	        for (loop =Height-1;loop>=0;loop--)
            {
	            for (loop2=0;loop2<Width;loop2++)
                {
	               outpixel = *((WORD *)(Bitmap_in+loop2*2 + loop * Pitch));

	               // Load up the Blue component and output it
	               outbyte = (8*((outpixel)&0x001f));//blue
	               WriteBuffer [BufferIndex++] = outbyte;

	               // Load up the green component and output it 
	               outbyte = (4*((outpixel>>5)&0x003f)); 
		            WriteBuffer [BufferIndex++] = outbyte;

	               // Load up the red component and output it 
	               outbyte = (8*((outpixel>>11)&0x001f));
	               WriteBuffer [BufferIndex++] = outbyte;
	            }

                for(i=0; i<padding; i++)
                    WriteBuffer[BufferIndex++] = (CHAR)0x00;
            }
	        
        }
        else // Assume 555 mode. 15-bit mode
        {
            // Loop bottom up
	        for (loop =Height-1;loop>=0;loop--)  
            {
                for (loop2=0;loop2<Width;loop2++)
                {

                    outpixel = *((WORD *)(Bitmap_in+loop2*2 + loop * Pitch));

                    // Load up the Blue component and output it
                    outbyte = (8*((outpixel)&0x001f));//blue
                    WriteBuffer [BufferIndex++] = outbyte;

                    // Load up the green component and output it 
                    outbyte = (8*((outpixel>>5)&0x001f)); 
                    WriteBuffer [BufferIndex++] = outbyte;

                    // Load up the red component and output it 
                    outbyte = (8*((outpixel>>10)&0x001f));  //BUG FIX here
                    WriteBuffer [BufferIndex++] = outbyte;
                }

                for(i=0; i<padding; i++)
                    WriteBuffer[BufferIndex++] = (CHAR)0x00;
            }
        }

        // At this point the buffer should be full, so just write it out
	    WriteFile(file_out, WriteBuffer, BufferIndex, &numwrite, NULL);

        // Now unlock the surface and we're done
	    UnLock();

    }

    //8 bit surfaces
    else  if (m_DDSD.ddpfPixelFormat.dwRGBBitCount==8) 
    {
        //Get the system palette so we can index each pixel to its corresponding color
        if (m_Screen->GetFront() == NULL)
        {
	        delete [] WriteBuffer;
	        return -4;
        }

        LPDIRECTDRAWPALETTE Pal;
        char bytepal [256*4];

        rval = m_Screen->GetFront()->GetDDS()->GetPalette(&Pal);
        if ( FAILED(rval) )
        {
	        delete [] WriteBuffer;
	        return -5;
        }

        Pal->GetEntries (0, 0, 256, (tagPALETTEENTRY *)&(bytepal[0]));
        Pal->Release();

        //lock the surface and start filling the output buffer
        rval = Lock();
        if ( FAILED(rval) )
        {
	        delete [] WriteBuffer;
	        return -6;
        }

        BYTE* Bitmap_in = (BYTE*)m_DDSD.lpSurface;

        // Determine the padding at the end of a row.
        padding = (Width * 3) % 4;
        if(padding > 0)
            padding = 4 - padding;

        // Ok, now that we've got the palette and the 24-bit entries, we just look up the color and output it
        // NOTE: At the same time we are reversing the component order (ie. BGR instead of RGB)
        //      and we are outputting it bottom up. 

        // Loop bottom up
        for (loop =Height-1;loop>=0;loop--)
        {
            for (loop2=0;loop2<Width;loop2++)
            {
                // Load a byte from the surface
                outpixel = *(Bitmap_in+loop2 + loop * Pitch);

                // Load up the Blue component and output it
                outbyte = bytepal[outpixel*4];//blue
                WriteBuffer[BufferIndex++] = outbyte;

                // Load up the Green component and output it
                outbyte = bytepal[outpixel*4+1];//green
                WriteBuffer[BufferIndex++] = outbyte;

                // Load up the Red component and output it
                outbyte = bytepal[outpixel*4+2];//red
                WriteBuffer[BufferIndex++] = outbyte;
            }

            for(i=0; i<padding; i++)
                WriteBuffer[BufferIndex++] = (CHAR)0x00;
        }

        // At this point the buffer should be full, so just write it out
	    WriteFile(file_out, WriteBuffer, BufferIndex, &numwrite, NULL);

        // Now unlock the surface and we're done
	    UnLock();
    }

    delete [] WriteBuffer;

    CloseHandle (file_out);
    return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Scanrightside
// 
// PURPOSE:          
//
// INPUT: none
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
void CDXSurface::Scanrightside(int x1,int x2,int ytop,int lineheight,char side,int TexWidth,int TexHeight,RECT* dClip)
{
	int x,px,py,xadd,pxadd,pyadd,y;

	if(++lineheight < 1)lineheight=1;

	xadd = ((x2-x1)<<16)/lineheight;

	if(side==1)
	{
		px    = 0;
		py    = 0;
		pxadd = (TexWidth << 16) / lineheight;
		pyadd = 0;
	}
	if(side==2)
	{
		px    = (TexWidth-0) << 16;
		py    = 0;
		pxadd = 0;
		pyadd = (TexHeight << 16) / lineheight;
	}
	if(side==3)
	{
		px    = (TexWidth - 0) << 16;
		py    = (TexHeight - 0) << 16;
		pxadd = (-TexWidth << 16) / lineheight;
		pyadd = 0;
	}
	if(side==4)
	{
		px    = 0;
		py    = (TexHeight - 0) << 16;
		pxadd = 0;
		pyadd = (-TexHeight << 16) / lineheight;
	}

	x = x1 << 16;

	for(y=0; y<=lineheight; y++)
	{
		int yp=ytop+y;
		if( (yp >= dClip->top) && (yp < dClip->bottom) )
		{
			m_rightTable[yp].x  = x;
			m_rightTable[yp].px = px;
			m_rightTable[yp].py = py;
		}
		x  = x + xadd;
		px = px + pxadd;
		py = py + pyadd;
	}
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Scanleftside
// 
// PURPOSE:          
//
// INPUT: none
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
void CDXSurface::Scanleftside(int x1,int x2,int ytop,int lineheight,char side,int TexWidth,int TexHeight,RECT* dClip)
{
	int x,px,py,xadd,pxadd,pyadd,y;

	if(++lineheight<1) lineheight = 1;

	xadd = ((x2-x1) << 16) / lineheight;

	if(side == 1)
	{
		px    = (TexWidth - 0) << 16;
		py    = 0;
		pxadd = (-TexWidth << 16) / lineheight;
		pyadd = 0;
	}
	if(side == 2)
	{
		px    = (TexWidth - 0) << 16;
		py    = (TexHeight - 0) << 16;
		pxadd = 0;
		pyadd = (-TexHeight << 16) / lineheight;
	}
	if(side == 3)
	{
		px    = 0;
		py    = (TexHeight - 0) << 16;
		pxadd = (TexWidth << 16) / lineheight;
		pyadd = 0;
	}
	if(side == 4)
	{
		px    = 0;
		py    = 0;
		pxadd = 0;
		pyadd = (TexHeight << 16) / lineheight;
	}

	x = x1 << 16;

	for(y=0; y<=lineheight; y++)
	{
		int yp = ytop + y;

		if( (yp >= dClip->top) &&( yp < dClip->bottom) )
		{
			m_leftTable[yp].x  = x;
			m_leftTable[yp].px = px;
			m_leftTable[yp].py = py;
		}
		x  = x + xadd;
		px = px + pxadd;
		py = py + pyadd;
	}
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: TextureMap
// 
// PURPOSE:          
//
// INPUT: none
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
void CDXSurface::TextureMap(CDXSurface *dest, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, RECT* area, BOOL bTrans)
{
    BYTE* lpSrc;
	BYTE* lpDest;
	unsigned long dPitch, sPitch;
    DWORD colorKey;
    RECT* dClipRect;

    // Get the ClipRect for the destination surface.
    dClipRect = dest->GetClipRect();

	if(m_leftTable != NULL)
    {
        delete m_leftTable;
        m_leftTable = NULL;
    }

	if(m_rightTable != NULL)
    {
        delete m_rightTable;
        m_rightTable = NULL;
    }
	//Screen->TexMapTableHeight = m_ClipRect.bottom;
	m_leftTable  = new TexMapTable[dClipRect->bottom];
	m_rightTable = new TexMapTable[dClipRect->bottom];

	int TexWidth  = area->right  - area->left;
	int TexHeight = area->bottom - area->top;

	int miny = y1;
	int maxy = y1;
	if(y2<miny) miny = y2;
	if(y2>maxy) maxy = y2;
	if(y3<miny) miny = y3;
	if(y3>maxy) maxy = y3;
	if(y4<miny) miny = y4;
	if(y4>maxy) maxy = y4;

	if(miny >= maxy) return;
	if(maxy < dClipRect->top) return;
	if(miny >= dClipRect->bottom)return;
	if(miny < dClipRect->top) miny = dClipRect->top;
	if(maxy > dClipRect->bottom) maxy = dClipRect->bottom;
	if(maxy - miny<1)return;

	if(y2 < y1) {Scanleftside(x2,x1,y2,y1-y2,1,TexWidth,TexHeight, dClipRect);}
	else        {Scanrightside(x1,x2,y1,y2-y1,1,TexWidth,TexHeight, dClipRect);}

	if(y3 < y2) {Scanleftside (x3,x2,y3,y2-y3,2,TexWidth,TexHeight, dClipRect);}
	else        {Scanrightside (x2,x3,y2,y3-y2,2,TexWidth,TexHeight, dClipRect);}

	if(y4 < y3) {Scanleftside (x4,x3,y4,y3-y4,3,TexWidth,TexHeight, dClipRect);}
	else        {Scanrightside (x3,x4,y3,y4-y3,3,TexWidth,TexHeight, dClipRect);}

	if(y1 < y4) {Scanleftside (x1,x4,y1,y4-y1,4,TexWidth,TexHeight, dClipRect);}
	else        {Scanrightside (x4,x1,y4,y1-y4,4,TexWidth,TexHeight, dClipRect);}

	int polyx1,polyx2,y,linewidth,pxadd,pyadd;
	int texX,texY;

    // Lock down both surfaces for read and write
    Lock();
    dest->Lock();

    // Set the pitch for both surfaces
    sPitch = m_DDSD.lPitch;
    dPitch = dest->m_DDSD.lPitch;

    // Initialize the pointers to the upper left hand corner of the surface
    lpSrc  = (BYTE*)m_DDSD.lpSurface;
    lpDest = (BYTE*)dest->m_DDSD.lpSurface;

	// Get the color key for sprite surface
	colorKey = (DWORD)m_ColorKey;

	if(m_PixelFormat.bpp == 8)
	{
		if(bTrans)
		{
			for(y=miny; y<=maxy; y++)
			{
				polyx1 = m_leftTable[y].x >> 16;
				polyx2 = m_rightTable[y].x >> 16;
				linewidth = polyx2 - polyx1;
				if(linewidth < 1) linewidth = 1;
				pxadd = ((m_rightTable[y].px)-(m_leftTable[y].px)) / linewidth;
				pyadd = ((m_rightTable[y].py)-(m_leftTable[y].py)) / linewidth;

				texX = m_leftTable[y].px;
				texY = m_leftTable[y].py;

				if(polyx1 < dClipRect->left)
				{
					texX  += pxadd * (dClipRect->left - polyx1);
					texY  += pyadd * (dClipRect->left - polyx1);
					polyx1 = dClipRect->left;
				}
				if(polyx2 > dClipRect->right - 0) {polyx2 = dClipRect->right - 0;}

				texX += area->left << 16;
				texY += area->top << 16;
                unsigned char* dst = (lpDest + (y * dPitch) + polyx1 );

				for(int x=polyx1+1; x<polyx2; x++)
				{
					texX += pxadd;
					texY += pyadd;
                    unsigned char c = *(lpSrc + ((texY >> 16) * sPitch) + (texX >> 16) );
					if(c != colorKey) *dst++ = c; else dst++;
				}
			}
		}else
		{
			for(y=miny;y<=maxy;y++)
			{
				polyx1 = m_leftTable[y].x >> 16;
				polyx2 = m_rightTable[y].x >> 16;
				linewidth = polyx2 - polyx1;
				if(linewidth < 1) linewidth = 1;
				pxadd = ( (m_rightTable[y].px) - (m_leftTable[y].px) ) / linewidth;
				pyadd = ( (m_rightTable[y].py) - (m_leftTable[y].py) ) / linewidth;

				texX = m_leftTable[y].px;
				texY = m_leftTable[y].py;

				if(polyx1 < dClipRect->left)
				{
					texX  += pxadd * (dClipRect->left - polyx1);
					texY  += pyadd * (dClipRect->left - polyx1);
					polyx1 = dClipRect->left;
				}
				if(polyx2 > dClipRect->right - 0) {polyx2 = dClipRect->right - 0;}

				texX += area->left << 16;
				texY += area->top << 16;
                unsigned char* dst = (lpDest + (y * dPitch) + polyx1 );

				for(int x=polyx1+1;x<polyx2;x++)
				{
					texX+=pxadd;
					texY+=pyadd;
                    *dst = *(lpSrc + ((texY >> 16) * sPitch) + (texX >> 16) );
                    dst++;
				}
			}
		}
	}else
    if(m_PixelFormat.bpp == 16)
	{
		if(bTrans)
		{
			for(y=miny; y<=maxy; y++)
			{
				polyx1 = m_leftTable[y].x >> 16;
				polyx2 = m_rightTable[y].x >> 16;
				linewidth = polyx2 - polyx1;
				if(linewidth < 1) linewidth = 1;
				pxadd = ((m_rightTable[y].px) - (m_leftTable[y].px)) / linewidth;
				pyadd = ((m_rightTable[y].py) - (m_leftTable[y].py)) / linewidth;

				texX = m_leftTable[y].px;
				texY = m_leftTable[y].py;

				if(polyx1 < dClipRect->left)
				{
					texX  += pxadd * (dClipRect->left - polyx1);
					texY  += pyadd * (dClipRect->left - polyx1);
					polyx1 = dClipRect->left;
				}
				if(polyx2 > dClipRect->right - 0) {polyx2 = dClipRect->right - 0;}

				texX += area->left << 16;
				texY += area->top << 16;
                unsigned char* dst = (lpDest + (y * dPitch) + (polyx1 * 2) );

				for(int x=polyx1+1;x<polyx2;x++)
				{
					texX+=pxadd;
					texY+=pyadd;
                    unsigned short c = *((WORD*)(lpSrc + ((texY >> 16) * sPitch) + ((texX >> 16) * 2) ));
					if(c != colorKey) *((WORD*)dst) = c; 
                    dst += 2;
				}
			}
		}else
		{
			for(y=miny; y<=maxy ;y++)
			{
				polyx1 = m_leftTable[y].x >> 16;
				polyx2 = m_rightTable[y].x >> 16;
				linewidth = polyx2 - polyx1;

				if( linewidth < 1) linewidth = 1;
				pxadd = ((m_rightTable[y].px) - (m_leftTable[y].px)) / linewidth;
				pyadd = ((m_rightTable[y].py) - (m_leftTable[y].py)) / linewidth;

				texX = m_leftTable[y].px;
				texY = m_leftTable[y].py;

				if(polyx1 < dClipRect->left)
				{
					texX  += pxadd * (dClipRect->left - polyx1);
					texY  += pyadd * (dClipRect->left - polyx1);
					polyx1 = dClipRect->left;
				}
				if(polyx2 > dClipRect->right - 0){polyx2 = dClipRect->right - 0;}

				texX += area->left << 16;
				texY += area->top  << 16;
				unsigned char* dst = (lpDest + (y * dPitch) + (polyx1 * 2) );

				for(int x=polyx1+1; x<polyx2; x++)
				{
					texX  += pxadd;
					texY  += pyadd;
					*((WORD*)dst) = *((WORD*)(lpSrc + ((texY >> 16) * sPitch) + ((texX >> 16) * 2) ));
                    dst += 2;
				}
			}
		}	
    } else if(m_PixelFormat.bpp == 24)
	{
		if(bTrans)
		{
			for(y=miny; y<=maxy; y++)
			{
				polyx1 = m_leftTable[y].x >> 16;
				polyx2 = m_rightTable[y].x >> 16;
				linewidth = polyx2 - polyx1;
				if(linewidth < 1) linewidth = 1;
				pxadd=((m_rightTable[y].px) - (m_leftTable[y].px)) / linewidth;
				pyadd=((m_rightTable[y].py) - (m_leftTable[y].py)) / linewidth;

				texX = m_leftTable[y].px;
				texY = m_leftTable[y].py;

				if(polyx1 < dClipRect->left)
				{
					texX  += pxadd * (dClipRect->left - polyx1);
					texY  += pyadd * (dClipRect->left - polyx1);
					polyx1 = dClipRect->left;
				}
				if(polyx2 > dClipRect->right - 0) {polyx2 = dClipRect->right - 0;}

				texX += area->left << 16;
				texY += area->top << 16;
                unsigned char* dst = (lpDest + (y * dPitch) + (polyx1 * 3) );

				for(int x=polyx1+1; x<polyx2; x++)
				{
					texX += pxadd;
					texY += pyadd;
					unsigned int ofs = (texY >> 16) * sPitch + (texX >> 16) * 3;
					DWORD r = *(lpSrc + ofs);
					DWORD g = *(lpSrc + ofs + 1);
					DWORD b = *(lpSrc + ofs + 2);
					if( (b | (g << 8) | (r << 16) ) != colorKey)
					{
// MichaelR: this code would not compile correctly on Borland C++ Builder
// so I wrapped it in this #ifdef because I wasn't sure why it was coded like this
#ifdef _MSC_VER
						*dst++ = unsigned char(r);
						*dst++ = unsigned char(g);
						*dst++ = unsigned char(b);
#else
						*dst++ = (unsigned char)(r);
						*dst++ = (unsigned char)(g);
						*dst++ = (unsigned char)(b);
#endif
					}else dst += 3;
				}
			}
		}else
		{
			for(y=miny; y<=maxy; y++)
			{
				polyx1 = m_leftTable[y].x >> 16;
				polyx2 = m_rightTable[y].x >> 16;
				linewidth = polyx2 - polyx1;
				if(linewidth < 1) linewidth = 1;
				pxadd = ((m_rightTable[y].px) - (m_leftTable[y].px)) / linewidth;
				pyadd = ((m_rightTable[y].py) - (m_leftTable[y].py)) / linewidth;

				texX = m_leftTable[y].px;
				texY = m_leftTable[y].py;

				if(polyx1 < dClipRect->left)
				{
					texX  += pxadd * (dClipRect->left - polyx1);
					texY  += pyadd * (dClipRect->left - polyx1);
					polyx1 = dClipRect->left;
				}
				if(polyx2 > dClipRect->right - 0) {polyx2 = dClipRect->right - 0;}

				texX += area->left << 16;
				texY += area->top << 16;
                unsigned char* dst = (lpDest + (y * dPitch) + (polyx1 * 3) );

				for(int x=polyx1+1; x<polyx2; x++)
				{
					texX += pxadd;
					texY += pyadd;
					unsigned int ofs = (texY >> 16) * sPitch + (texX >> 16) * 3;
					*dst++ = *(lpSrc + ofs);
					*dst++ = *(lpSrc + ofs + 1);
					*dst++ = *(lpSrc + ofs + 2);
				}
			}
		}	
	}else if(m_PixelFormat.bpp == 32)
	{
		if(bTrans)
		{
			for(y=miny; y<=maxy; y++)
			{
				polyx1 = m_leftTable[y].x >> 16;
				polyx2 = m_rightTable[y].x >> 16;
				linewidth = polyx2 - polyx1;
				if(linewidth < 1) linewidth = 1;
				pxadd=((m_rightTable[y].px) - (m_leftTable[y].px)) / linewidth;
				pyadd=((m_rightTable[y].py) - (m_leftTable[y].py)) / linewidth;

				texX = m_leftTable[y].px;
				texY = m_leftTable[y].py;

				if(polyx1 < dClipRect->left)
				{
					texX  += pxadd * (dClipRect->left - polyx1);
					texY  += pyadd * (dClipRect->left - polyx1);
					polyx1 = dClipRect->left;
				}
				if(polyx2 > dClipRect->right - 0) {polyx2 = dClipRect->right - 0;}

				texX += area->left << 16;
				texY += area->top << 16;
                unsigned char* dst = (lpDest + (y * dPitch) + (polyx1 * 4) );

				for(int x=polyx1+1; x<polyx2; x++)
				{
					texX += pxadd;
					texY += pyadd;
					unsigned int ofs = (texY >> 16) * sPitch + (texX >> 16) * 4;
					DWORD r = *(lpSrc + ofs);
					DWORD g = *(lpSrc + ofs + 1);
					DWORD b = *(lpSrc + ofs + 2);
					if( (b | (g << 8) | (r << 16) ) != colorKey)
					{
// MichaelR: this code would not compile correctly on Borland C++ Builder
// so I wrapped it in this #ifdef because I wasn't sure why it was coded like this
#ifdef _MSC_VER
						*dst++ = unsigned char(r);
						*dst++ = unsigned char(g);
						*dst++ = unsigned char(b);
#else
						*dst++ = (unsigned char)(r);
						*dst++ = (unsigned char)(g);
						*dst++ = (unsigned char)(b);
#endif
						dst++;
					}else dst += 4;
				}
			}
		}else
		{
			for(y=miny; y<=maxy; y++)
			{
				polyx1 = m_leftTable[y].x >> 16;
				polyx2 = m_rightTable[y].x >> 16;
				linewidth = polyx2 - polyx1;
				if(linewidth < 1) linewidth = 1;
				pxadd=((m_rightTable[y].px) - (m_leftTable[y].px)) / linewidth;
				pyadd=((m_rightTable[y].py) - (m_leftTable[y].py)) / linewidth;

				texX = m_leftTable[y].px;
				texY = m_leftTable[y].py;

				if(polyx1 < dClipRect->left)
				{
					texX  += pxadd * (dClipRect->left - polyx1);
					texY  += pyadd * (dClipRect->left - polyx1);
					polyx1 = dClipRect->left;
				}
				if(polyx2 > dClipRect->right - 0) {polyx2 = dClipRect->right - 0;}

				texX += area->left << 16;
				texY += area->top << 16;
                unsigned char* dst = (lpDest + (y * dPitch) + (polyx1 * 4) );

				for(int x=polyx1+1; x<polyx2; x++)
				{
					texX += pxadd;
					texY += pyadd;

                    unsigned int ofs = (texY >> 16) * sPitch + (texX >> 16) * 4;
					*dst++ = *(lpSrc + ofs);
					*dst++ = *(lpSrc + ofs + 1);
					*dst++ = *(lpSrc + ofs + 2);

					dst++;
				}
			}
		}	
	}

    // Lock down both surfaces for read and write
    UnLock();
    dest->UnLock();
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawBlkRotoZoom
// 
// PURPOSE:          
//
// INPUT: none
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawBlkRotoZoom(CDXSurface* dest, int midX, int midY, RECT* area, double angle, double scale)
{
	double SinA,CosA;
    BOOL bTrans = FALSE;

    // Clip the source rect to the extents of the surface.
    ClipRect(area);

	int HalfWidth  = int(double((area->right  - area->left) * scale / 2));
	int HalfHeight = int(double((area->bottom - area->top)  * scale / 2));

	SinA = sin(-angle);
	CosA = cos(-angle);

    int x1 = (int)(CosA * -HalfWidth - SinA * -HalfHeight);
    int y1 = (int)(SinA * -HalfWidth + CosA * -HalfHeight);
    int x2 = (int)(CosA * HalfWidth - SinA * -HalfHeight);
    int y2 = (int)(SinA * HalfWidth + CosA * -HalfHeight);
    int x3 = (int)(CosA * HalfWidth - SinA * HalfHeight);
    int y3 = (int)(SinA * HalfWidth + CosA * HalfHeight);
    int x4 = (int)(CosA * -HalfWidth - SinA * HalfHeight);
    int y4 = (int)(SinA * -HalfWidth + CosA * HalfHeight);

	TextureMap(dest, x1+midX, y1+midY, x2+midX, y2+midY, x3+midX, y3+midY, x4+midX, y4+midY, area, bTrans);

    return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: DrawTransRotoZoom
// 
// PURPOSE:          
//
// INPUT: none
//
// RETURNS: 0 or > - successful
//          < 0    - failed
// 
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSurface::DrawTransRotoZoom(CDXSurface* dest, int midX, int midY, RECT* area, double angle, double scale)
{
	double SinA,CosA;
    BOOL bTrans = TRUE;

    // Clip the source rect to the extents of the surface.
    ClipRect(area);

	int HalfWidth  = int(double((area->right  - area->left) * scale / 2));
	int HalfHeight = int(double((area->bottom - area->top)  * scale / 2));

	SinA = sin(-angle);
	CosA = cos(-angle);

    int x1 = (int)(CosA * -HalfWidth - SinA * -HalfHeight);
    int y1 = (int)(SinA * -HalfWidth + CosA * -HalfHeight);
    int x2 = (int)(CosA * HalfWidth - SinA * -HalfHeight);
    int y2 = (int)(SinA * HalfWidth + CosA * -HalfHeight);
    int x3 = (int)(CosA * HalfWidth - SinA * HalfHeight);
    int y3 = (int)(SinA * HalfWidth + CosA * HalfHeight);
    int x4 = (int)(CosA * -HalfWidth - SinA * HalfHeight);
    int y4 = (int)(SinA * -HalfWidth + CosA * HalfHeight);

	TextureMap(dest, x1+midX, y1+midY, x2+midX, y2+midY, x3+midX, y3+midY, x4+midX, y4+midY, area, bTrans);

    return 1;
}