//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxscreen.cpp,v $
// $Author: stevens-madison $
//
// $Log: cdxscreen.cpp,v $
// Revision 1.11  2013/04/01 14:55:01  stevens-madison
// Working towards Version 1.0.....
// First 'commit' since sourceforge changed URLs.
// Fingers crossed.
//
// Revision 1.10  2009/06/13 00:44:00  stevens-madison
// Temple Caster fix.  Revised method of initiating OnDraw.
//
// Revision 1.9  2009/04/17 23:14:07  stevens-madison
// Turn Undead bug and a couple of changes for GameRules.h and cn_Icon11.png
//
// Revision 1.8  2006/04/16 16:08:10  cocoaspud
// range check race/class definitions in config files
//
// Revision 1.7  2002/03/02 16:07:09  cocoaspud
// copy test char art to design folder
//
// Revision 1.6  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.9  2001/09/22 13:35:23  mindcry
// minor changes
//
// Revision 1.8  2001/07/24 03:56:10  istan
// Modified code to use new version of DDLoadPalette
// Modified LoadPalette to return HRESULT if DDLoadPalette fails
//
// Revision 1.7  2001/02/22 13:45:04  bsimser
// Updated CDXScreen for documentation purposes
//
// Revision 1.6  2000/10/03 01:06:29  istan
// Remarked out init code for DD7 that broke the 3D classes, and used previous method to obtain it.
//
// Revision 1.5  2000/07/19 14:37:10  mindcry
// Minor changes how the DX7 interface is initilized
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
// Revision 1.1.1.1  2000/04/22 16:11:24  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.42  2000/03/06 18:52:50  jhebert
// Updated the CDXSurface::Draw functions so that they accept a pointer to
// a RECT as the srcRect argument.  This allows you to set this to NULL if
// you want to simply use the whole surface as the source.  This affected
// many of the other classes which have all been modified.
//
// Revision 2.41  2000/02/29 21:40:09  jhebert
// Added the calls to FunctionMapper when creating fullscreen.  This was causing
// crashes when tring to draw lines, etc.
//
// Revision 2.40  2000/02/10 19:09:52  jhebert
// ChangeVideoMode Function works now.  Required a check for the front or back
// buffer in the RemoveSurfaceFromList function.
//
// Revision 2.39  2000/01/26 22:53:50  jhebert
// Fixed the issue with apps crashing on exit due to the surfacelist deleting
// the surfaces.  ReleaseAllSurfaces does NOT destroy CDXSurfaces anymore. This
// will be handled by the user or by DirectDraw.
//
// Revision 2.38  2000/01/22 19:46:29  jhebert
// Fixed a bug that hapened when you release the surface from the list.
//
// Revision 2.37  2000/01/20 23:31:16  jhebert
// Added function mapping calls for the front and back buffers.
//
// Revision 2.36  2000/01/19 23:18:38  jhebert
// Added the ChangeVideoMode() function.
//
// Revision 2.35  2000/01/13 23:36:33  jhebert
// First checkin for v3.0.  I still have one major function I want to add but
// otherwise it is pretty much complete. Changes:
// 	- Added headers to all functions
// 	- fixed a few problems in the destructor
// 	- added friend functions to CDXSurface to be able to access private data
//
//
// $Revision: 1.11 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxscreen.cpp,v 1.11 2013/04/01 14:55:01 stevens-madison Exp $";
#endif

#include "CDX.h"
#include "cdxscreen.h"
#include "cdxsurface.h"

void WriteDebugString(const char *ptext, ... );


LONG CDXScreen::m_lastId = 0;

/**
 * \fn void CDXScreen::InitCDXScreen()
 * \brief Initializes all member variables.
 *
 * Initializes all member variables, Create the DirectDraw Object
 * and enumerate all the videomodes and drivers.
 *
 * \retval Nothing
 */
void CDXScreen::InitCDXScreen()
{
	HRESULT   rval;

	CDXLOG("START: CDXScreen::InitCDXScreen");
	CDXLOG("Initialize member vars");

	// initialize member variables
	m_lpDD					= NULL;
    m_ZBuffer				= NULL;
	m_lpDDSFront			= NULL;
	m_lpDDSBack				= NULL;
	m_lpDDPalette			= NULL;
	m_lpClipper				= NULL;
    m_dwPixelWidth			= 0;
	m_dwPixelHeight			= 0;
    m_BPP					= 0;
	m_hWnd					= 0;
	m_bFullScreen			= FALSE;
    m_NumberOfVideoModes    = 0;
    m_VideoModes            = NULL;
	m_TripleBuffering		= FALSE;
	m_BackBufferInVRAM		= TRUE;
    m_NumberOfDrivers       = 0;
	m_Drivers               = NULL;
	m_SelectedDriver		= 0;
	m_frameCount			= 0;
	m_accumulatedFrameTime	= 0;
	m_frameStartTime		= timeGetTime();
	m_fps					= 0;
  m_UseClipper = FALSE;

	// create direct draw object
#if DIRECTDRAW_VERSION >= CDX_DDVER
	LPDIRECTDRAW lpDD1;
	
	CDXLOG("CDX_DDVER: 0x%X (%i)", CDX_DDVER, CDX_DDVER);
	CDXLOG("DIRECTDRAW_VERSION: 0x%X (i)", DIRECTDRAW_VERSION, DIRECTDRAW_VERSION);

//	CDXLOG("Attempting a DirectDrawCreateEx");
//	rval = DirectDrawCreateEx( NULL, (VOID**)&m_lpDD, IID_IDirectDraw7, NULL );
  /*
   *      25 May 2009  Paul R Stevens
   *
   * I added the EMULATIONONLY flag.
   *
   * The problem occurred only for Window'ed mode.
   * Blt'ing to the fron buffer failed when Source Color Key was selected.
   * The reult was that nothing got Blt'ed and and error INVALIDPARAM was returned.
   * The DirectX GetCaps indicated that Source Color Key was not supported.
   * I suppose there are other ways around the problem.  Perhaps Blt to back bufffer
   * and flip.  Perhaps get the area, do a manual blt, and put it back.
   * Perhaps ignore the problem.  Doing an extra OnDraw fixed at least some
   * of the problems in that the slow text (for example) eventually did get displayed.
   * Perhaps we should try to restrict this EMULATIONONLY to only the 
   * particular case where Source Color Key does not work.
   * For now, I will leave it this way.  This program is hardly 'graphic-intensive'.
   * I doubt that the emulation will cost much.
   */
	rval = DirectDrawCreate((GUID *)DDCREATE_EMULATIONONLY, &lpDD1, NULL);
	// temporary prs rval = DirectDrawCreate(NULL, &lpDD1, NULL);
	if( FAILED(rval) ) 
    {
        CDXLOG("Failed to create an IID_IDirectDraw Interface!");
    }

	rval = lpDD1->QueryInterface(IID_IDirectDraw7, (LPVOID*)&m_lpDD);
	if( FAILED(rval) ) 
    {
        CDXLOG("Failed to create an IID_IDirectDraw7 Interface!");
    }

	// don't need this pointer anymore, release it
	lpDD1->Release();

  DDCAPS DDHwCaps;
  memset(&DDHwCaps, 0, sizeof(DDCAPS));
  DDHwCaps.dwSize = sizeof(DDCAPS);
  
  rval = m_lpDD->GetCaps(&DDHwCaps, NULL);
  if (SUCCEEDED(rval))
  {
    if (DDHwCaps.ddsCaps.dwCaps & DDCAPS2_PRIMARYGAMMA)
      CDXLOG("GAMMA Control Supported");
    else
      CDXLOG("GAMMA Control NOT Supported");

  }
#else
	CDXLOG("CDX_DDVER: 0x%X (%i)", CDX_DDVER, CDX_DDVER);
	CDXLOG("DIRECTDRAW_VERSION: 0x%X (%i)", DIRECTDRAW_VERSION, DIRECTDRAW_VERSION);

	CDXLOG("Attempting a DirectDrawCreate");

	rval = DirectDrawCreate(NULL, &m_lpDD, NULL);
	if( FAILED(rval) )
    {
        CDXLOG("Failed to create an IID_IDirectDraw Interface!");
    }
#endif

	// Enumerate the drivers
	EnumDrivers( );

    // Enumerate the Video Modes
	EnumVideoModes( );

	CDXLOG("END: CDXScreen::InitCDXScreen");
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: Default constructor
// 
// PURPOSE: Creates an instance of the CDXScreen Object
//
// INPUT:   none
//                   
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
CDXScreen::CDXScreen()
{
	CDXLOG("START: CDXScreen::CDXScreen()");

	InitCDXScreen();

	CDXLOG("END: CDXScreen::CDXScreen()");
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Destructor
// 
// PURPOSE: Destroys the DirectDraw object and returns control to Windows.
//
// INPUT:   none
//                   
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
CDXScreen::~CDXScreen(void)
{
    CDX_VIDEOMODESSTRUCT* Modes;
    CDX_VIDEOMODESSTRUCT* Modes2;
	CDX_DRIVERSTRUCT* Drivers;
    CDX_DRIVERSTRUCT* Drivers2;

    CDXLOG("START: CDXScreen::~CDXScreen");
	CDXLOG("Modes Ptr: %i", m_VideoModes);
	//screen destructor must restore the cooperative level
	//otherwise attempt to release and recreate in fullscreen mode may fail. //(Daniel Polli)
	m_lpDD->SetCooperativeLevel( (HWND)m_hWnd, DDSCL_NORMAL );

	// detstroy linked list with video modes informations
    Modes = m_VideoModes;
    do
    {
        Modes2 = Modes;
        Modes  = ( CDX_VIDEOMODESSTRUCT *)Modes->NextMode;
		if( Modes2 != NULL )
			delete Modes2;
    }
    while( Modes != NULL );

	CDXLOG("Drivers Ptr: %i", m_Drivers);

	// destroy linked list with direct draw drivers informations
    Drivers = m_Drivers;
    do
    {
        Drivers2 = Drivers;
        Drivers  = Drivers->NextDriver;
		if( Drivers2 != NULL )
			delete Drivers2;
    }
    while( Drivers != NULL );

	CDXLOG("Restore Display");
	if(m_lpDD != NULL) 
		m_lpDD->RestoreDisplayMode();

	CDXLOG( "Freeing all surfaces" );
	ReleaseAllSurfaces( );

    // Only release the back buffer if we are in windowed mode. Otherwise
    // releasing the front buffer will take care of it.
	CDXLOG("Delete back buffer");
	if( (GetBack() != NULL) && (IsFullScreen() == FALSE) )
		SAFEDELETE( m_lpDDSBack );

	CDXLOG("Delete front buffer");
	if( GetFront() != NULL )
		SAFEDELETE( m_lpDDSFront );

	CDXLOG("Start releasing objects");

	CDXLOG("Release zBuffer");
	RELEASE(m_ZBuffer);

  if (m_UseClipper)
  {
	  CDXLOG("Release clipper");
    RELEASE(m_lpClipper);
  }

	CDXLOG("Release Palette");
	RELEASE(m_lpDDPalette);

	CDXLOG("Release DirectDraw");
	RELEASE(m_lpDD);

	CDXLOG("END: CDXScreen::~CDXScreen");
}



//////////////////////////////////////////////////////////////////////////////////
// NAME: CreateWindowed
// 
// PURPOSE: initialises CDX for a windowed mode application. The Width and Height
//          are used to create the back buffer size, not the front bffer size. The
//          front buffer is the desktop.
//
// INPUT:   hWnd   - Handle to a window
//          Width  - The width of the surface in pixels
//          Height - The height of the surface in pixels
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::CreateWindowed(void *hWnd, int Width, int Height, BOOL UseClipper)
{
	HRESULT rval;
    HRESULT rc = 0;
	DWORD dwFlags;
	HDC hDC;
	DWORD width, height;
    RECT rect;
	CDX_DDSURFACEDESC ddsd;
	CDX_LPDIRECTDRAWSURFACE lpdds;

	CDXLOG("START: CDXScreen::CreateWindowed(hwnd:%i, width:%i, height:%i)", hWnd, Width, Height);

    // See if we have a valid DirectDraw Interface
    if( GetDD() == NULL)
    {
        CDXLOG( "DirectDraw Interface pointer is NULL" );
        return -1;
    }

    // If the front buffer is valid then exit
    if( m_lpDDSFront != NULL )
    {
        CDXLOG( "Attempted to recreate windowed and the front buffer is not NULL" );
        return -1;
    }

	CDXLOG("Set member variables");

	m_bFullScreen   = FALSE;
	m_dwPixelWidth  = Width;
	m_dwPixelHeight = Height;
	m_hWnd          = hWnd;

	CDXLOG("Get Device Caps");

	hDC = GetDC(NULL);
    if(hDC != NULL)
    {
	    m_BPP = GetDeviceCaps(hDC, PLANES) * GetDeviceCaps(hDC, BITSPIXEL);
	    ReleaseDC(NULL, hDC);
    }
    else
    {
        return -1;
    }

	CDXLOG("Set cooperative level");

	dwFlags = DDSCL_NORMAL;

	rval = m_lpDD->SetCooperativeLevel((HWND)hWnd, dwFlags);
    if( FAILED(rval) ) 
        return -2;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize  = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	CDXLOG("Create front buffer");
	m_lpDDSFront = new CDXSurface();

	CDXLOG("Create back buffer");
	m_lpDDSBack = new CDXSurface();

	CDXLOG("create front buffer surface");
	rval = GetDD()->CreateSurface(&ddsd, &lpdds, NULL);
    SetSurfaceDDSPointer(GetFront(), lpdds);
    if( FAILED(rval) ) 
    {
        SAFEDELETE( m_lpDDSBack );
	    SAFEDELETE( m_lpDDSFront );
        return -4;
    }


	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    

	// Check if backbuffer is to be created in vram or sys ram
	if( m_BackBufferInVRAM == TRUE )
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

	ddsd.dwWidth  = Width;
	ddsd.dwHeight = Height;

	CDXLOG("Create back buffer surface");
	rval = GetDD()->CreateSurface( &ddsd, &lpdds, NULL );
  SetSurfaceDDSPointer(GetBack(), lpdds);
	if( FAILED(rval) ) 
    {
		// if backbuffer creation in vram fails, try in sysram
		if( m_BackBufferInVRAM == TRUE )
		{
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			CDXLOG("Try creating back buffer in system ram");
			rval = GetDD()->CreateSurface( &ddsd, &lpdds, NULL );
            SetSurfaceDDSPointer(GetBack(), lpdds);
			if( FAILED(rval) )
            {
                SAFEDELETE( m_lpDDSBack );
	            SAFEDELETE( m_lpDDSFront );
                return -8;
            }
		}
		else
        {
            SAFEDELETE( m_lpDDSBack );
	        SAFEDELETE( m_lpDDSFront );

            return -8;
        }
    }

  m_UseClipper = UseClipper;
  if (m_UseClipper)
  {
	  CDXLOG("Create clipper object");
	  rval = m_lpDD->CreateClipper(0, &m_lpClipper, NULL);
      if( FAILED(rval) ) 
          rc |= -16;

	  CDXLOG("Set Hwnd (%i) for clipper", hWnd);
	  rval = m_lpClipper->SetHWnd(0, (HWND)hWnd);
      if( FAILED(rval) ) 
          rc |= -32;

	  CDXLOG("Assign clipper object to front surface");
	  rval = GetFront()->GetDDS()->SetClipper(m_lpClipper);
      if( FAILED(rval) ) 
          rc |= -64;
  }

	CDXLOG("Set params for front buffer");
	CDXLOG("W: %i, H: %i", m_dwPixelWidth, m_dwPixelHeight);

	hDC    = GetDC(NULL);
	width  = GetDeviceCaps(hDC, HORZRES);
	height = GetDeviceCaps(hDC, VERTRES);
	ReleaseDC(NULL, hDC);

	SetSurfaceWidth(GetFront(), width);
	SetSurfaceHeight(GetFront(), height);
    rect.top    = 0;
    rect.left   = 0;
    rect.right  = width;
    rect.bottom = height;

	SetSurfaceScreen(GetFront());
	GetFront()->SetClipRect(&rect);
	CDXLOG("Get RGB Format for front buffer");
	SetSurfacePixelFormat(GetFront());
    GetFront()->FunctionMapper();

	CDXLOG("Set params for back buffer");

	SetSurfaceWidth(GetBack(), m_dwPixelWidth);
	SetSurfaceHeight(GetBack(), m_dwPixelHeight);
    rect.top    = 0;
    rect.left   = 0;
    rect.right  = m_dwPixelWidth;
    rect.bottom = m_dwPixelHeight;

	SetSurfaceScreen(GetBack());
	GetBack()->SetClipRect(&rect);
	CDXLOG("Get RGB Format for back buffer");
	SetSurfacePixelFormat(GetBack());
    GetBack()->FunctionMapper();

	CDXLOG("Load default palette");
	// create a default palette
	DDLoadPalette(m_lpDD, &m_lpDDPalette, NULL);
	CDXLOG("Set palette for front surface");
	GetFront()->GetDDS()->SetPalette(m_lpDDPalette);

	CDXLOG("END: CDXScreen::CreateWindowed");

	return rc;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: CreateFullScreen
// 
// PURPOSE: initialises CDX for a full screen mode application. The Width and Height
//          and BPP are used to set the video mode and create the front and back
//          buffers.
//
// INPUT:   hWnd   - Handle to a window
//          Width  - The width of the surface in pixels
//          Height - The height of the surface in pixels
//          BPP    - The number of bits per pixel (ie. 8, 16, 24, 32)
//          bVGA   - Flag that allow old VGA mode resolutions
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::CreateFullScreen(void *hWnd, DWORD Width, DWORD Height, DWORD BPP, BOOL bVGA)
{
	HRESULT rval;
    HRESULT rc = 0;
	DWORD dwFlags;
    RECT rect;
	CDX_DDSURFACEDESC ddsd;
	CDX_DDSCAPS ddscaps;
	CDX_LPDIRECTDRAWSURFACE lpdds;

	CDXLOG("START: CDXScreen::CreateFullScreen(hwnd:%i, width:%i, height:%i, bpp:%i, bVGA:%i)", hWnd, Width, Height, BPP, bVGA);
	
    // See if we have a valid DirectDraw Interface
    if( GetDD() == NULL)
    {
        CDXLOG( "DirectDraw Interface pointer is NULL" );
        return -1;
    }

    // If the front buffer is valid then exit
    if( m_lpDDSFront != NULL )
    {
        CDXLOG( "Attempted to recreate in Fullscreen and the front buffer is not NULL" );
        return -1;
    }

	CDXLOG("Set member variables");

	m_bFullScreen   = TRUE;
	m_dwPixelWidth  = Width;
	m_dwPixelHeight = Height;
	m_BPP           = BPP;
	m_hWnd          = hWnd;

	CDXLOG("Set cooperative level");

	dwFlags = DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT | DDSCL_ALLOWMODEX;

	rval = m_lpDD->SetCooperativeLevel((HWND)hWnd, dwFlags);
    if( FAILED(rval) ) 
        return -2;

	CDXLOG("Set video mode");
#if DIRECTDRAW_VERSION >= CDX_DDVER
	if(bVGA) 
        rval = m_lpDD->SetDisplayMode(m_dwPixelWidth, m_dwPixelHeight, m_BPP, 0, DDSDM_STANDARDVGAMODE);
	else 
        rval = m_lpDD->SetDisplayMode(m_dwPixelWidth, m_dwPixelHeight, m_BPP, 0, 0);
#else
	rval = m_lpDD->SetDisplayMode(m_dwPixelWidth, m_dwPixelHeight, m_BPP);
#endif

    if( FAILED(rval) ) 
        return -4;

	CDXLOG("Create front buffer");
	m_lpDDSFront = new CDXSurface;

	CDXLOG("Create back buffer");
	m_lpDDSBack = new CDXSurface;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize  = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	if( m_BackBufferInVRAM == FALSE )
		ddsd.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_SYSTEMMEMORY;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY;

	// Try to get 2 backbuffers, if it fails only one
	if( m_TripleBuffering == TRUE )
	{
		ddsd.dwBackBufferCount = 2;

		CDXLOG("create front buffer surface (for triple buffering)");
		rval = m_lpDD->CreateSurface(&ddsd, &lpdds, NULL);
        SetSurfaceDDSPointer(GetFront(), lpdds);
		if( FAILED(rval) ) 
		{
			m_TripleBuffering      = FALSE;
			ddsd.dwBackBufferCount = 1;

			CDXLOG("create front buffer surface (can't use triple buffering)");
			rval = m_lpDD->CreateSurface(&ddsd, &lpdds, NULL);
            SetSurfaceDDSPointer(GetFront(), lpdds);

			// if creation of 1 backbuffer in video memory fails, try one in system memory
			if( FAILED(rval) ) 
            {
				if( m_BackBufferInVRAM == TRUE )
				{
					ddsd.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_SYSTEMMEMORY;
					CDXLOG("create front buffer surface (in system ram, can't use triple buffering)");
                    rval = m_lpDD->CreateSurface(&ddsd, &lpdds, NULL);
                    SetSurfaceDDSPointer(GetFront(), lpdds);
                    if( FAILED(rval) ) 
                    {
                        SAFEDELETE( m_lpDDSBack );
	                    SAFEDELETE( m_lpDDSFront );

                        return -8;
                    }
				}
				else
                {
                    SAFEDELETE( m_lpDDSBack );
	                SAFEDELETE( m_lpDDSFront );
                    return -16;
                }
            }
		}
	}
	else
	{
		ddsd.dwBackBufferCount = 1;

		CDXLOG("create front buffer surface");
		rval = m_lpDD->CreateSurface(&ddsd, &lpdds, NULL);
        SetSurfaceDDSPointer(GetFront(), lpdds);
		// if creation of 1 backbuffer in video memory fails, try one in system memory
		if( FAILED(rval) )
        {
			if( m_BackBufferInVRAM == TRUE )
			{
				ddsd.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_SYSTEMMEMORY;
				CDXLOG("create front buffer surface (in system ram)");
				rval = m_lpDD->CreateSurface(&ddsd, &lpdds, NULL);
                SetSurfaceDDSPointer(GetFront(), lpdds);
                if( FAILED(rval) ) 
                {
                    SAFEDELETE( m_lpDDSBack );
	                SAFEDELETE( m_lpDDSFront );
                    return -8;
                }
			}
			else
            {
                SAFEDELETE( m_lpDDSBack );
	            SAFEDELETE( m_lpDDSFront );

                return -16;
            }
        }
	}

    ZeroMemory(&ddscaps, sizeof(ddscaps));
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

	CDXLOG("Get the attached back buffer surface");
	rval = GetFront()->GetDDS()->GetAttachedSurface(&ddscaps, &lpdds);
    SetSurfaceDDSPointer(GetBack(), lpdds);
    if( FAILED(rval) )
    {
        SAFEDELETE( m_lpDDSBack );
	    SAFEDELETE( m_lpDDSFront );

        return -32;
    }

	CDXLOG("Set params for front buffer");

	CDXLOG("W: %i, H: %i", m_dwPixelWidth, m_dwPixelHeight);

	SetSurfaceWidth(GetFront(), m_dwPixelWidth);
	SetSurfaceHeight(GetFront(), m_dwPixelHeight);
    rect.top    = 0;
    rect.left   = 0;
    rect.right  = m_dwPixelWidth;
    rect.bottom = m_dwPixelHeight;

	SetSurfaceScreen(GetFront());
	GetFront()->SetClipRect(&rect);
	CDXLOG("Get RGB Format for front buffer");
	SetSurfacePixelFormat(GetFront());
    GetFront()->FunctionMapper();

	CDXLOG("Set params for back buffer");

	SetSurfaceWidth(GetBack(), m_dwPixelWidth);
	SetSurfaceHeight(GetBack(), m_dwPixelHeight);

	SetSurfaceScreen(GetBack());
	GetBack()->SetClipRect(&rect);
	CDXLOG("Get RGB Format for back buffer");
	SetSurfacePixelFormat(GetBack());
    GetBack()->FunctionMapper();

	CDXLOG("Load default palette");
	// create a default palette
	DDLoadPalette(m_lpDD, &m_lpDDPalette, NULL);
	CDXLOG("Set palette for front surface");
	GetFront()->GetDDS()->SetPalette(m_lpDDPalette);

	CDXLOG("END: CDXScreen::CreateFullScreen");

	return 0;
}

#ifdef DebugFull
BOOL EqualRECT(const RECT& r1, const RECT& r2)
{
  if (r1.left   != r2.left)   return FALSE;
  if (r1.top    != r2.top)    return FALSE;
  if (r1.right  != r2.right)  return FALSE;
  if (r1.bottom != r2.bottom) return FALSE;
  return TRUE;
}
#endif


//////////////////////////////////////////////////////////////////////////////////
// NAME: Flip
// 
// PURPOSE: Displays the contents of the back buffer on screen. If in a windowed
//          mode and FlipWithStretch is TRUE, Flip will stretch the back buffer 
//          to fit into the window. If vsync is FALSE then the flip happens 
//          immediately.  The default is to wait for the vsync.  If displayFPS
//          is set to true the frames per second will be displayed in the upper
//          left hand corner.
//
// INPUT:   VSync            - 
//          FlipWithStretch  - 
//          displayFPS       -
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
// NOTE: In DirectX 3 the fullscreen flip always is vsynced!
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::Flip(BOOL VSync, BOOL FlipWithStretch, BOOL displayFPS)
{
	HRESULT rval;	

	CDXLOG2("START: CDXScreen::Flip");

  // Show the FPS if displayFPS is set to TRUE.
	if (displayFPS)
	{
    CHAR str[32];
  	DWORD now;

	  // Calculate the time it took to build the frame and display it on the back buffer
	  now = timeGetTime();
	  m_accumulatedFrameTime += now - m_frameStartTime;
	  m_frameStartTime = now;

	  if (m_accumulatedFrameTime > 1000)
	  {
		  m_fps = (LONG)( ((float)m_frameCount / (float)m_accumulatedFrameTime) * 1000 );
		  m_accumulatedFrameTime = 0;
		  m_frameCount = 0;
	  }

      // Increment the framecount
	  m_frameCount++;

		// Print the FPS on the back buffer
		memset(str,'\0',32);
		sprintf_s(str, 32, "FPS = %d",m_fps);

		GetBack()->GetDC();
			GetBack()->TextXY(10,20,0x0000FFF0,str);
		GetBack()->ReleaseDC();
	}

	if(m_bFullScreen)
	{
		CDXLOG2("do a fullscreen flip");

		if( VSync == TRUE )
    {
			rval = GetFront()->GetDDS()->Flip(NULL, DDFLIP_WAIT);
#ifdef DebugFull
      WriteDebugString("!!rval = %d = Flip(NULL, DDFLIP_WAIT)\n", rval);
#endif
    }
		else
    {
#if DIRECTDRAW_VERSION >= CDX_DDVER
			rval = GetFront()->GetDDS()->Flip(NULL, DDFLIP_NOVSYNC);
#ifdef DebugFull
      WriteDebugString( "!!rval = %d = Flip(NULL, DDFLIP_NOVSYNC)\n", rval);
#endif
#else
		    rval = GetFront()->GetDDS()->Flip(NULL, DDFLIP_WAIT);
#ifdef DebugFull
        WriteDebugString("!!rval = %d = Flip(NULL, DDFLIP_WAIT)\n", rval);
#endif
#endif
    };
		if(rval == DDERR_SURFACELOST) 
            rval = Restore();
	}
	else
	{
    RECT Window, rSrc;
    POINT pt;
		CDXLOG2("windowed mode flip");

		GetClientRect((HWND)m_hWnd, &Window);
		pt.x = pt.y = 0;
		ClientToScreen((HWND)m_hWnd, &pt);
		OffsetRect(&Window, pt.x, pt.y);

		if( VSync == TRUE )
    {
			WaitForVerticalBlank( );
#ifdef DebugFull
      WriteDebugString("!!WaitForVerticalBlank()\n");
#endif
    };

		// stretch the surface to fit into the client window rect
		if(FlipWithStretch)
    {
#ifdef DebugFull
      {
        DDSURFACEDESC2 sD;
        static RECT prev_Window, prev_rSrc;
        static CDXSurface *prev_Back;
        static DDSURFACEDESC2 prev_sD;
        static IDirectDrawSurface7 *prev_DDS;
        sD.dwSize = sizeof(sD);
        GetBack()->GetDDS()->GetSurfaceDesc(&sD);
        if (   (!EqualRECT(prev_Window,Window))
            || (!EqualRECT(prev_rSrc  ,rSrc))
            || (prev_Back   != GetBack())
            || (prev_DDS    != GetBack()->GetDDS())
           )
        {
          prev_Window = Window;
          prev_rSrc   = rSrc;
          prev_Back   = GetBack();
          prev_DDS    = GetBack()->GetDDS();
          WriteDebugString("!!GetBack() = 0x%08x   GetDDS = 0x%08x\n", prev_Back, prev_DDS);
          WriteDebugString("!!rSrc = %d, %d, %d, %d\n", 0, 0, sD.dwWidth, sD.dwHeight);
          WriteDebugString("!!Window = %d, %d, %d, %d\n", Window.left, Window.top, Window.right, Window.bottom);
        };
      };
#endif
			rval = GetFront()->GetDDS()->Blt(&Window, GetBack()->GetDDS(), NULL, DDBLT_WAIT, NULL);
#ifdef DebugFull
      WriteDebugString( "!!rval = %d = Blt(&Window, GetBack()->GetDDS(), NULL, DDBLT_WAIT, NULL)\n", rval);
#endif
    }
		else
		{
			// size down the Src rect so that it fits into the window without stretching
			// note that anything beyond the RECT is clipped out of the display, but this
			// also allows it to run like a bat out of hades.  Stretches are slow.
			rSrc.left   = 0; 
            rSrc.top    = 0;
			rSrc.right  = Window.right - Window.left;
			rSrc.bottom = Window.bottom - Window.top;
#ifdef DebugFull
      {
        DDSURFACEDESC2 sD;
        static RECT prev_Window, prev_rSrc;
        static CDXSurface *prev_Back;
        static DDSURFACEDESC2 prev_sD;
        static IDirectDrawSurface7 *prev_DDS;
        sD.dwSize = sizeof(sD);
        GetBack()->GetDDS()->GetSurfaceDesc(&sD);
        if (   (!EqualRECT(prev_Window,Window))
            || (!EqualRECT(prev_rSrc  ,rSrc))
            || (prev_Back   != GetBack())
            || (prev_DDS    != GetBack()->GetDDS())
           )
        {
          prev_Window = Window;
          prev_rSrc   = rSrc;
          prev_Back   = GetBack();
          prev_DDS    = GetBack()->GetDDS();
          WriteDebugString("!!GetBack() = 0x%08x   GetDDS = 0x%08x\n", prev_Back, prev_DDS);
          WriteDebugString("!!rSrc = %d, %d, %d, %d\n", rSrc.left, rSrc.top, rSrc.right, rSrc.bottom);
          WriteDebugString("!!Window = %d, %d, %d, %d\n", Window.left, Window.top, Window.right, Window.bottom);
        };
      };
#endif
			rval = GetFront()->GetDDS()->Blt(&Window, GetBack()->GetDDS(), &rSrc, DDBLT_WAIT, NULL);
#ifdef DebugFull
      WriteDebugString( "!!rval = %d = Blt(&Window, GetBack()->GetDDS(), &rSrc, DDBLT_WAIT, NULL)\n", rval);
#endif
		}

		if(rval == DDERR_SURFACELOST) 
            rval = Restore();
	}

	CDXLOG2("END: CDXScreen::Flip");

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Fill
// 
// PURPOSE: Fills the back buffer with the specified colour.
//
// INPUT:   FillColor - The color to fill the backbuffer with
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::Fill(DWORD FillColor)
{
	DDBLTFX ddBltFx;
    HRESULT rval;

	ddBltFx.dwSize = sizeof(DDBLTFX);
	ddBltFx.dwFillColor = FillColor;
	rval = GetBack()->GetDDS()->Blt(NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &ddBltFx);

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: LoadBitmap
// 
// PURPOSE: Loads a .BMP file straight onto the back buffer.
//
// INPUT:   szFilename - Name of file to load
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::LoadBitmap(const char* szFilename)
{
    HRESULT rval;

	CDXLOG("START: CDXScreen::LoadBitmap(szFilename: %s)", szFilename);

	if(szFilename == NULL) 
        return -1;

	CDXLOG("load the bitmap into the back buffer");

	rval = DDReLoadBitmap(GetBack()->GetDDS(), szFilename);

	CDXLOG("END: CDXScreen::LoadBitmap");

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: LoadPalette
// 
// PURPOSE: Loads the palette from a .BMP file.  Should be called before other 
//          bitmap operations.
//
// INPUT:   szFilename - Name of file to load palette from
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::LoadPalette(const char* szFilename)
{
	HRESULT rval = S_OK;

	CDXLOG("START: CDXScreen::LoadBitmap(szFilename: %s)", szFilename);

	if(szFilename == NULL) 
        return -1;
   
	CDXLOG("Load the palette");

	rval = DDLoadPalette(m_lpDD, &m_lpDDPalette, szFilename);
	if (FAILED(rval))
		return rval;

	CDXLOG("Assign the palette to the front surface");
	GetFront()->GetDDS()->SetPalette(m_lpDDPalette);

	CDXLOG("END: CDXScreen::LoadBitmap");

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Restore
// 
// PURPOSE: Restores the DirectDraw surface object(s) if lost.  Called internally 
//          if a flip fails.
//
// INPUT: nothing
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::Restore(void)
{
    HRESULT rval = 0;
	CDXLOG("START: CDXScreen::Restore");

	if(m_bFullScreen)
	{
		// In fullscreen mode the surfaces are chained together and
		// the DX restore function will restore both surfaces.
		if(GetFront()->GetDDS() != NULL && FAILED(GetFront()->GetDDS()->IsLost()) )
		{
			rval = GetFront()->GetDDS()->Restore();
		}
	}
	else
	{
		// In Windowed mode the too surfaces are not related therefore 
		// we need to check both of them.
		if(GetFront()->GetDDS() != NULL && FAILED(GetFront()->GetDDS()->IsLost()) )
		{
			rval = GetFront()->GetDDS()->Restore();
		}

		if(GetBack()->GetDDS() != NULL && FAILED(GetBack()->GetDDS()->IsLost()) )
		{
			rval = GetBack()->GetDDS()->Restore();
		}
	}

	CDXLOG("END: CDXScreen::Restore");

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetColor
// 
// PURPOSE: Sets the red, green and blue values of a single colour in the current
//          palette.
//
// INPUT: col - value of the palitized color that you want to set
//        r   - the amount of red in the color
//        g   - the amount of green in the color
//        b   - the amount of blue in the color
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::SetColor(int col, int r, int g, int b)
{
    HRESULT rval;
	PALETTEENTRY pe[1];

	CDXLOG("START: CDXScreen::SetColor(col:%i, r:%i, g:%i, b:%i)", col, r, g, b);
	CDXLOG("Get the color entry");

	rval = m_lpDDPalette->GetEntries(0, col, 1, pe);
    if( FAILED(rval) )
        return rval;

	pe[0].peRed   = r;
	pe[0].peGreen = g;
	pe[0].peBlue  = b;

	CDXLOG("Set the color entry");

	rval = m_lpDDPalette->SetEntries(0, col, 1, pe);

	CDXLOG("END: CDXScreen::SetColor");

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetColor
// 
// PURPOSE: Returns the red, green and blue values of a single colour in the 
//          current palette.
//
// INPUT: col - value of the palitized color that you want to set
//        r   - pointer to an int to hold the red value
//        g   - pointer to an int to hold the red value
//        b   - pointer to an int to hold the red value
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::GetColor(int col, int *r, int *g, int *b)
{
    HRESULT rval;
	PALETTEENTRY pe[1];

	CDXLOG("START: CDXScreen::GetColor(col:%i, r:%i, g:%i, b:%i)", col, r, g, b);
	CDXLOG("Get the color entry");

	rval = m_lpDDPalette->GetEntries(0, col, 1, pe);

    if(FAILED(rval) )
        return rval;

	*r = pe[0].peRed;
	*g = pe[0].peGreen;
	*b = pe[0].peBlue;

	CDXLOG("r:%i, g:%i, b:%i)", pe[0].peRed, pe[0].peGreen, pe[0].peBlue);
	CDXLOG("END: CDXScreen::GetColor");

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetPalette
// 
// PURPOSE: Sets the palette pointed to by lpPE.  Start indicates the first 
//          entry to be set and Count is the number of palette entries to 
//          be changed.
//
// INPUT: Start - specififies which palette entry to start at
//        Count - specififies how many palette entries to change
//        lpPE  - pointer to the new palette entries
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::SetPalette(int Start, int Count, LPPALETTEENTRY lpPE)
{
    HRESULT rval;

	CDXLOG("START: CDXScreen::SetPalette(start:%i, count:%i, lpPE:%i)", Start, Count, lpPE);

	rval = m_lpDDPalette->SetEntries(0, Start, Count, lpPE);

	CDXLOG("END: CDXScreen::SetPalette");

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetPalette
// 
// PURPOSE: Fills lpPE with values from the current palette.  Start indicates 
//          the first entry to be set and Count is the number of palette entries
//          to be changed.
//
// INPUT: Start - specififies which palette entry to start at
//        Count - specififies how many palette entries to change
//        lpPE  - pointer to where the palette entries will be stored
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::GetPalette(int Start, int Count, LPPALETTEENTRY lpPE)
{
    HRESULT rval;

	CDXLOG("START: CDXScreen::GetPalette(start:%i, count:%i, lpPE:%i)", Start, Count, lpPE);

	rval = m_lpDDPalette->GetEntries(0, Start, Count, lpPE);

	CDXLOG("END: CDXScreen::GetPalette");

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: FillPalette
// 
// PURPOSE: Sets all the palette entries to a single colour.
//
// INPUT: r   - pointer to an int to hold the red value
//        g   - pointer to an int to hold the red value
//        b   - pointer to an int to hold the red value
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::FillPalette(int r, int g, int b)
{
    HRESULT rval;
	PALETTEENTRY pe[256];

	CDXLOG("START: CDXScreen::FillPalette(r:%i, g:%i, b:%i)", r, g, b);
	CDXLOG("Get the color entries");

	rval = m_lpDDPalette->GetEntries(0, 0, 256, pe);
    if(FAILED(rval) )
        return rval;

	CDXLOG("Fill the color entries");

	for(int i = 1; i < 256; i++)
	{
		pe[i].peRed   = r;
		pe[i].peGreen = g;
		pe[i].peBlue  = b;
	}

	CDXLOG("Set the color entries");
	rval = m_lpDDPalette->SetEntries(0, 0, 256, pe);

	CDXLOG("END: CDXScreen::FillPalette");

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GreyScale
// 
// PURPOSE: Converts the current palette into a monochrome palette.
//
// INPUT: none
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::GreyScale(void)
{
    HRESULT rval;
	PALETTEENTRY pe[256];
	int total, grey;

	CDXLOG("START: CDXScreen::GreyScale");
	CDXLOG("Get the color entries");

	rval = m_lpDDPalette->GetEntries(0, 0, 256, pe);
    if( FAILED(rval) )
        return rval;

	CDXLOG("Fill the color entries");

	for(int i = 1; i < 256; i++)
	{
		total = pe[i].peRed + pe[i].peGreen + pe[i].peBlue;
		grey = total / 3;

		pe[i].peRed   = grey;
		pe[i].peGreen = grey;
		pe[i].peBlue  = grey;
	}

	CDXLOG("Set the color entries");

	rval = m_lpDDPalette->SetEntries(0, 0, 256, pe);

	CDXLOG("END: CDXScreen::GreyScale");

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: FadeIn
// 
// PURPOSE: Smoothly fades the current palette into the palette pointed to by lpPE.
//          It takes 3 steps to operate a fade in function.  First you should get 
//          the current palette using GetPalette, then you should set the current 
//          palette to the colour you wish to start from, lastly you call FadeIn
//          with the palette you grabbed in the first step.
//
//          For example:
//
//              CDXScreen Screen;
//              PALETTEENTRY pe[256];
//              Screen->GetPalette(0, 256, pe);
//              Screen->FillPalette(255, 0, 0);
//              Screen->FadeIn(6, pe);
//
//          This code would make the screen flash red then return to how it started,
//          just like a rocket hit in Quake!
//
//          NOTE: This function is for 8bit color modes only.
//
// INPUT: delay - the amount of time to delay between palette adjustments.
//                   0   - no delay
//                   10  - about 1 second
//                   25  - about 2 seconds
//                   50  - about 4 seconds
//                   100 - about 8 seconds
//        lpPE  - pointer to the desired palette table
//                   
//
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::FadeIn(int delay, LPPALETTEENTRY lpPE)
{
    HRESULT rval;
	PALETTEENTRY pe[256];
	DWORD timeNow, timeTarget;

	CDXLOG("START: CDXScreen::FadeIn(delay:%i, lpPE:%i)", delay, lpPE);
	CDXLOG("Get the color entries");

	rval = m_lpDDPalette->GetEntries(0, 0, 256, pe);
    if( FAILED(rval) )
        return rval;

	CDXLOG("Fill the color entries");

	for(int j = 1; j < 84; j++)
	{
		for(int i = 1; i < 256; i++)
		{
			// Red
			if(pe[i].peRed > (lpPE[i].peRed+5)) pe[i].peRed -= 3;
			else if(pe[i].peRed < (lpPE[i].peRed-5)) pe[i].peRed += 3;
			else pe[i].peRed = lpPE[i].peRed;

			// Green
			if(pe[i].peGreen > (lpPE[i].peGreen+5)) pe[i].peGreen -= 3;
			else if(pe[i].peGreen < (lpPE[i].peGreen-5)) pe[i].peGreen += 3;
			else pe[i].peGreen = lpPE[i].peGreen;

			// Blue
			if(pe[i].peBlue > (lpPE[i].peBlue+5)) pe[i].peBlue -= 3;
			else if(pe[i].peBlue < (lpPE[i].peBlue-5)) pe[i].peBlue += 3;
			else pe[i].peBlue = lpPE[i].peBlue;
		}
		// Add a delay here for slower fades
		timeNow    = timeGetTime();
		timeTarget = timeNow + delay;

		while(timeNow < timeTarget)
			timeNow = timeGetTime();

		CDXLOG("Set the color entries");
		rval = m_lpDDPalette->SetEntries(0, 0, 256, pe);
        if( FAILED(rval) )
            return rval;
	}

	CDXLOG("END: CDXScreen::FadeIn");

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: FadeOut
// 
// PURPOSE: Smoothly fades the current palette to black.
//
//          NOTE: This function is for 8bit color modes only.
//
// INPUT: delay - the amount of time to delay between palette adjustments.
//                   0   - no delay
//                   10  - about 1 second
//                   25  - about 2 seconds
//                   50  - about 4 seconds
//                   100 - about 8 seconds
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::FadeOut(int delay)
{
    HRESULT rval;
	PALETTEENTRY pe[256];
	DWORD timeNow, timeTarget;

	CDXLOG("START: CDXScreen::FadeOut(delay:%i)", delay);
	CDXLOG("Get the color entries");

	rval = m_lpDDPalette->GetEntries(0, 0, 256, pe);
    if( FAILED(rval) )
        return rval;

	CDXLOG("Fill the color entries");

	for(int j = 1; j < 84; j++)
	{
		for(int i = 1; i < 256; i++)
		{
			// Red
			if(pe[i].peRed > 5) pe[i].peRed -= 3;
			else pe[i].peRed = 0;

			// Green
			if(pe[i].peGreen > 5) pe[i].peGreen -= 3;
			else pe[i].peGreen = 0;

			// Blue
			if(pe[i].peBlue > 5) pe[i].peBlue -= 3;
			else pe[i].peBlue = 0;
		}
		// Add a delay here for slower fades
		timeNow    = timeGetTime();
		timeTarget = timeNow + delay;

		while(timeNow < timeTarget)
			timeNow = timeGetTime();

		CDXLOG("Set the color entries");
		rval = m_lpDDPalette->SetEntries(0, 0, 256, pe);
        if( FAILED(rval) )
            return rval;
	}

	CDXLOG("END: CDXScreen::FadeOut");

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: FadeTo
// 
// PURPOSE: Smoothly fades the current palette to the specified colour.
//
//          NOTE: This function is for 8bit color modes only.
//
// INPUT:   r     - RED component of the color to fade to
//          b     - BLUE component of the color to fade to
//          g     - GREEN component of the color to fade to
//          delay - the amount of time to delay between palette adjustments.
//                   0 - no delay
//                   10  - about 1 second
//                   25  - about 2 seconds
//                   50  - about 4 seconds
//                   100 - about 8 seconds
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::FadeTo(int r, int g, int b, int delay)
{
    HRESULT rval;
	PALETTEENTRY pe[256];
	DWORD timeNow, timeTarget;

	CDXLOG("START: CDXScreen::FadeTo(r:%i, g:%i, b:%i, delay:%i)", r, g, b, delay);
	CDXLOG("Get the color entries");

	rval = m_lpDDPalette->GetEntries(0, 0, 256, pe);
    if( FAILED(rval) )
        return rval;

	CDXLOG("Fill the color entries");
	for(int j = 1; j < 84; j++)
	{
		for(int i = 1; i < 256; i++)
		{
			// Red
			if(pe[i].peRed > r+5) pe[i].peRed -= 3;
			else if(pe[i].peRed < r-5) pe[i].peRed += 3;
			else pe[i].peRed = r;

			// Green
			if(pe[i].peGreen > g+5) pe[i].peGreen -= 3;
			else if(pe[i].peGreen < g-5) pe[i].peGreen += 3;
			else pe[i].peGreen = g;

			// Blue
			if(pe[i].peBlue > b+5) pe[i].peBlue -= 3;
			else if(pe[i].peBlue < b-5) pe[i].peBlue += 3;
			else pe[i].peBlue = b;
		}
		// Add a delay here for slower fades
		timeNow    = timeGetTime();
		timeTarget = timeNow + delay;

		while(timeNow < timeTarget)
			timeNow = timeGetTime();

		CDXLOG("Get the color entries");
		rval = m_lpDDPalette->SetEntries(0, 0, 256, pe);
        if( FAILED(rval) )
            return rval;
	}

	CDXLOG("END: CDXScreen::FadeTo");

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetNumberOfVideoModes
// 
// PURPOSE: gets the numer of video modes the card supports. This function is 
//          needed if you need a list of all videomodes to know how many modes
//          there are and to get them via GetVideoModeInformation
//
// INPUT:   none
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
DWORD CDXScreen::GetNumberOfVideoModes( void )
{
    return m_NumberOfVideoModes;
}



//////////////////////////////////////////////////////////////////////////////////
// NAME: GetVideoModeInformation
// 
// PURPOSE: Gets information about Videomode Index. Index is a number from 0 
//          to GetNumberOfVideoModes. If an invalid mode index is given, the 
//          return value is false, else true
//
// INPUT:   Index  -
//          Width  - width of video mode in pixels
//          Height - height of video mode in pixels
//          BPP    - color bits per pixel
//                   
// RETURNS: TRUE   - success
//          FALSE  - invalid index
//
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXScreen::GetVideoModeInformation( DWORD Index, DWORD* Width, DWORD* Height, DWORD* BPP )
{
    CDX_VIDEOMODESSTRUCT * Modes;

	CDXLOG("START: CDXScreen::GetVideoModeInformation(index:%i, width:%i, height:%i, bpp:%i)", Index, Width, Height, BPP);
	CDXLOG("Modes Ptr: %i", m_VideoModes);

    Modes = m_VideoModes;

    if( Index < m_NumberOfVideoModes )
    {
		CDXLOG("Index was < m_NumberOfVideoModes");

        while( Index > 0 )
        {
            -- Index;
            Modes = ( CDX_VIDEOMODESSTRUCT * )Modes->NextMode;
        }
        *Width   = Modes->Width;
        *Height  = Modes->Height;
        *BPP     = Modes->BPP;

		CDXLOG("w:%i, h:%i, bpp:%i", Width, Height, BPP);
		CDXLOG("END: CDXScreen::GetVideoModeInformation");

        return TRUE;
    }
    else
	{
		CDXLOG("END: CDXScreen::GetVideoModeInformation");
        return FALSE;
	}
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: CheckIfVideoModeExists
// 
// PURPOSE: checks if the desired video mode exists, if yes return = true , 
//          else return = false
//
// INPUT:   Width  - width of video mode in pixels
//          Height - height of video mode in pixels
//          BPP    - color bits per pixel
//                   
// RETURNS: TRUE   - mode exists
//          FALSE  - mode does not exist
//
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXScreen::CheckIfVideoModeExists( DWORD Width , DWORD Height , DWORD BPP )
{
    BOOL                    found;
    CDX_VIDEOMODESSTRUCT *  Modes;

	CDXLOG("START: CDXScreen::CheckIfVideoModeExists(width:%i, height:%i, bpp:%i)", Width, Height, BPP);
	CDXLOG("Modes Ptr: %i", m_VideoModes);

    Modes = m_VideoModes;
    found = FALSE;

    while( Modes->NextMode != NULL )
    {
		CDXLOG("Try Mode: w:%i, h:%i, bpp:%i", Width, Height, BPP);

        if( ( Modes->Width  == Width  ) &&
            ( Modes->Height == Height ) &&
            ( Modes->BPP    == BPP    ) )
        {
            found = TRUE;
			CDXLOG("Found Mode!");
            break;
        }

        Modes = ( CDX_VIDEOMODESSTRUCT * )Modes->NextMode;
    }

	CDXLOG("END: CDXScreen::CheckIfVideoModeExists");

    return found;
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: WaitForVerticalBlank
// 
// PURPOSE: waits until the vertical blank phase starts
//
// INPUT:   none
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::WaitForVerticalBlank( void )
{
  //return 0; //temporary PRS
	CDXLOG2("START: CDXScreen::WaitForVerticalBlank");

	return m_lpDD->WaitForVerticalBlank(  DDWAITVB_BLOCKBEGIN , 0 );

    CDXLOG2("END: CDXScreen::WaitForVerticalBlank");
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: MakeColor
// 
// PURPOSE: Converts the Color into a Color suitable for the selected color format
//
// INPUT:   Color   - 8-Bits each of red, green, and blue.
//          Surface - CDXSurface to be used as the conversion source.  If set to
//                    NULL the back buffer surface is used.
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
// NOTE: in 8-Bit color modes, the color is not converted
//////////////////////////////////////////////////////////////////////////////////
DWORD CDXScreen::MakeColor( DWORD Color , CDXSurface * Surface )
{
    DDPIXELFORMAT   pf;
    unsigned int    i, j, 
                    rshift, gshift, bshift, 
                    rbits, gbits, bbits,
                    RED , GREEN , BLUE,
                    result;
    
	CDXLOG2("START: CDXScreen::MakeColor(color:%i, surface:%i)", Color, Surface);

    // separate red,green.blue out of Color
    RED   = Color & 255;
    GREEN = ( Color >> 8 ) & 255;
    BLUE  = ( Color >> 16 ) & 255;
 
	CDXLOG2("r:%i, g:%i, b:%i", RED, GREEN, BLUE);

    // get the destination surface pixel format
    ZeroMemory( &pf , sizeof( pf ) );
    pf.dwSize = sizeof(pf);

	CDXLOG2("Get pixel format");
    if( Surface == NULL )
        GetBack()->GetDDS()->GetPixelFormat( &pf );
    else
        Surface->GetDDS()->GetPixelFormat( &pf );

    // convert the color
    if (pf.dwRGBBitCount>8)
    {
		CDXLOG2("Mode depth > 8bit");

        j = (int) pf.dwRBitMask; rshift = 0;
        i = 1; while (!(i&j)) { rshift++; i<<=1; }
        rbits = 0; while (i&j) { rbits++; i<<=1; }
        j = (int) pf.dwGBitMask; gshift = 0;
        i = 1; while (!(i&j)) { gshift++; i<<=1; }
        gbits = 0; while (i&j) { gbits++; i<<=1; }
        j = (int) pf.dwBBitMask; bshift = 0;
        i = 1; while (!(i&j)) { bshift++; i<<=1; }
        bbits = 0; while (i&j) { bbits++; i<<=1; }

        result = (((RED<<rshift)>>(8-rbits)) & pf.dwRBitMask) |
                 (((GREEN<<gshift)>>(8-gbits)) & pf.dwGBitMask) |
                 (((BLUE<<bshift)>>(8-bbits)) & pf.dwBBitMask);
    }
    else
    {
        result = Color;
    }

	CDXLOG2("Result Color: %i", result);
	CDXLOG2("END: CDXScreen::MakeColor");

    return result;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: EnumVideoModes
// 
// PURPOSE: Enumerates all Video Modes
//
// INPUT:   none
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::EnumVideoModes( void )
{
    HRESULT rval;
	CDXLOG("START: CDXScreen::EnumVideoModes");
	CDXLOG("Do some init'ing");

    m_NumberOfVideoModes = 0;
    m_VideoModes = new CDX_VIDEOMODESSTRUCT;
	ZeroMemory( m_VideoModes, sizeof( CDX_VIDEOMODESSTRUCT ) );

	CDXLOG("EnumDisplayModes");

#if DIRECTDRAW_VERSION >= CDX_DDVER
    rval = GetDD()->EnumDisplayModes( DDEDM_STANDARDVGAMODES, NULL, (LPVOID)m_VideoModes, EnumDisplayModesCallback);
#else
    rval = GetDD()->EnumDisplayModes( 0, NULL, (LPVOID)m_VideoModes, EnumDisplayModesCallback);
#endif
    if( FAILED(rval) )
	{
		CDXLOG("END: CDXScreen::EnumVideoModes");
		return rval;
	}

	CDXLOG("Get the number of video modes");

    // Find out how many different video modes were returned
    CDX_VIDEOMODESSTRUCT* Modes = ( CDX_VIDEOMODESSTRUCT* )m_VideoModes;

    while( Modes->NextMode != NULL )
    {
        Modes = ( CDX_VIDEOMODESSTRUCT * )Modes->NextMode;

		CDXLOG("w:%i, h:%i, bpp:%i", Modes->Width, Modes->Height, Modes->BPP);

        ++ m_NumberOfVideoModes;
    }   

	CDXLOG("END: CDXScreen::EnumVideoModes");

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: EnumDrivers
// 
// PURPOSE: Enumerates all Display Drivers
//
// INPUT:   none
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::EnumDrivers( void )
{
    HRESULT rval;

	CDXLOG("START: CDXScreen::EnumDrivers");

	m_NumberOfDrivers = 0;
	m_Drivers = new CDX_DRIVERSTRUCT;
    ZeroMemory( m_Drivers, sizeof( CDX_DRIVERSTRUCT ) );

#if DIRECTDRAW_VERSION >= CDX_DDVER
	CDXLOG("DirectDrawEnumerateEx");
    rval = DirectDrawEnumerateEx( DDEnumCallback, (LPVOID)m_Drivers, DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES );
#else
	CDXLOG("DirectDrawEnumerate");
    rval = DirectDrawEnumerate( DDEnumCallback, (LPVOID)m_Drivers );
#endif

    if( FAILED(rval) )
	{
		CDXLOG("END: CDXScreen::EnumDrivers");
		return rval;
	}

	CDXLOG("Get the number of drivers");

    // Find out how many different drivers were returned
	CDX_DRIVERSTRUCT *Drivers = m_Drivers;

    while( Drivers->NextDriver != NULL )
    {
        Drivers = Drivers->NextDriver;

		CDXLOG("%s", Drivers->lpDesc);

        ++ m_NumberOfDrivers;
    }   

	CDXLOG("END: CDXScreen::EnumDrivers");

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SelectDriver
// 
// PURPOSE: Selects Display Driver and recreates the direct draw interface
//
// INPUT:   none
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::SelectDriver( DWORD Index )
{	
	HRESULT rval;

	CDXLOG("START: CDXScreen::SelectDriver(index:%i)", Index);
	CDXLOG("NumDrivers: %i", m_NumberOfDrivers);

	// Check if the index is correct
	if( Index > m_NumberOfDrivers )
		return -1;

	CDXLOG("Check to see if DX is already setup up a screen mode");

	// Check if direct draw has allready set up a screen mode
	// if so we must exit, else all the surfaces will have the wrong
	// direct draw interface
	if( GetFront() )
		return -2;

	CDXLOG("Find the driver");

	// Find the Indexed driver
	CDX_DRIVERSTRUCT* Drivers = m_Drivers;
	for( DWORD i=0; i<Index; i++ )
	{	
        Drivers = Drivers->NextDriver;

		if( Drivers == NULL ) 
            return -4;
	}

	CDXLOG("Destroy linked list with video mode infos");

	// destroy linked list with video modes informations
    CDX_VIDEOMODESSTRUCT* Modes , * Modes2;
    Modes = m_VideoModes;

    do
    {
        Modes2 = Modes;
        Modes  = ( CDX_VIDEOMODESSTRUCT*)Modes->NextMode;
		if( Modes2 != NULL )
			delete Modes2;
    }
    while( Modes != NULL );

	CDXLOG("Release DirectDraw");
	RELEASE(m_lpDD);

	// create direct draw object
#if DIRECTDRAW_VERSION >= CDX_DDVER
	CDXLOG("Attempting a DirectDrawCreateEx");
	rval = DirectDrawCreateEx( NULL, (VOID**)&m_lpDD, IID_IDirectDraw7, NULL );
	if( FAILED(rval) ) 
    {
        CDXLOG("Failed to create an IID_IDirectDraw7 Interface!");
    }
#else
	CDXLOG("Create DirectDraw");
	rval = DirectDrawCreate( Drivers->lpGUID, &m_lpDD, NULL);
	if( FAILED(rval) ) 
        return rval;
#endif

	// Do video enumeration stuff, again
	EnumVideoModes( );

	CDXLOG("END: CDXScreen::SelectDriver");

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: AddSurfaceToList
// 
// PURPOSE: This call adds a surface to the internal surfaces list. This list can
//          be used to determine if there are any unfreed surfaces when the app
//          exits or for other purposes. This function should be called out of 
//          the CDXSurface Create functions.
//
// INPUT:   Surface - pointer to the CDXSurface to be added to the linked list
//                   
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXScreen::AddSurfaceToList( CDXSurface* lpCDXS )
{
    m_SurfaceList.Add(lpCDXS);
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: ReleaseAllSurfaces
// 
// PURPOSE: This call removes all surfaces from the SurfaceList. It is usually
//          called from CDXScreen destructor, the surfaces are NOT destroyed!
//
// INPUT:   none
//                   
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXScreen::ReleaseAllSurfaces( void )
{
    BOOL bDeleteSurface = FALSE;

    m_SurfaceList.Clear(bDeleteSurface);
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: ReleaseSurfaceFromList
// 
// PURPOSE: This call removes a surface from the SurfaceList.  The Surface itself
//          is NOT destroyed.
//
// INPUT:   Surface - pointer to the CDXSurface to be removed from the linked list
//                   
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXScreen::ReleaseSurfaceFromList( CDXSurface* lpCDXS )
{
    BOOL bDeleteSurface = FALSE;
	CDXSurface* lpcdxs  = lpCDXS;

    if( (lpCDXS != GetFront()) && (lpCDXS != GetBack()) )
        m_SurfaceList.Remove(lpcdxs, bDeleteSurface);
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: ReloadAllSurfaces
// 
// PURPOSE: 
//
// INPUT:   nothing
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::ReloadAllSurfaces(void)
{
    HRESULT rval;
    CDXSurface* pSurface;

    // Restore any offScreen buffers that existed.
    for(pSurface = m_SurfaceList.GetFirst(); pSurface != NULL; pSurface = m_SurfaceList.Next(pSurface))
    {
        // Restore the surface first.
        rval = pSurface->Restore();

        // Reload the graphics to the new surface 
        rval = pSurface->ReLoadBitmap(pSurface->GetCaptureFileName());

        // Delete the file.
        remove(pSurface->GetCaptureFileName());
	}

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SaveAllSurfaces
// 
// PURPOSE: 
//
// INPUT:   nothing
//                   
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::SaveAllSurfaces(void)
{
    HRESULT rval;
    CDXSurface* pSurface;
    DWORD index = 0;
    char filename[256] = {'\0'};

    // Restore any offScreen buffers that existed.
    for(pSurface = m_SurfaceList.GetFirst(); pSurface != NULL; pSurface = m_SurfaceList.Next(pSurface))
    {
        // Create the filename
        sprintf_s(filename, sizeof(filename), "surface%04d.bmp", pSurface->GetSurfaceId());

        // Restore the surface first.
        rval = pSurface->SaveAsBMP(filename);

        // Set the CaptureFileName for this surface.
        pSurface->SetCaptureFileName(filename);
	}

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetNextId
// 
// PURPOSE: 
//
// INPUT:   nothing
//                   
// RETURNS: A number greater then 0.
//
//////////////////////////////////////////////////////////////////////////////////
LONG CDXScreen::GetNextId(void)
{
    m_lastId++;

    return m_lastId; 
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: FadeToSource
// 
// PURPOSE: Fades the screen to to the image found on the back buffer.  This 
//          function works for all graphics modes except 8bpp modes.  It uses 
//          software alpha blending to accomplish the fade therefore it should 
//          work on all graphics cards. It makes a temp copy of the back buffer
//          then alpha blends different shades back to the back buffer then flips
//          it to the front buffer.  I would use it like this:
//
//          Screen->GetBack()->Fill(BLACK);
//          Screen->Flip();
//          ***load image to back buffer***
//          Screen->FadeToSource(1500);
//
// INPUT: duration - the amount of time for the entire fade in milliseconds.
//                   Good values seem to be between 500-2000;
//                   
//
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::FadeToSource(WORD duration)
{
    HRESULT rval;
	float fadeInc;
	LONG ALPHA;
	DWORD fadeBegin;
	DWORD w,h;
	RECT sRect;
	CDXSurface* tmp;

	if (m_BPP == 8)
	{
		// Sorry this does not support 8bpp mode.
		// use FadeIn()
	}
	else
	{
		// Create a tmp surface the size of the back buffer and
		// copy the back buffer to it.
		w = GetBack()->GetWidth();
		h = GetBack()->GetHeight();
		sRect.left   = 0;
		sRect.top    = 0;
		sRect.right  = w;
		sRect.bottom = h;

		tmp  = new CDXSurface();
        if(tmp == NULL)
            return -1;

        rval = tmp->Create(this, w, h, CDXMEM_SYSTEMONLY);
        if( FAILED(rval) )
        {
            SAFEDELETE(tmp);
            return rval;
        }

		rval = GetBack()->DrawBlk(tmp, 0, 0, &sRect);
        if( FAILED(rval) )
        {
            SAFEDELETE(tmp);
            return rval;
        }

		ALPHA = 0;

		// Start with a completely black screen
		rval = GetBack()->Fill(0);
#ifdef DebugFull
    {
      WriteDebugString("!!Set BackBuffer to black\n");
      WriteDebugString("!!GetBack() = 0x%08x->Fill(0) = %d\n", GetBack(), rval);
    };
#endif
        if( FAILED(rval) )
        {
            SAFEDELETE(tmp);
            return rval;
        }

		rval = Flip(TRUE,FALSE);
#ifdef DebugFull
    {
      WriteDebugString("!!Flip(TRUE,FALSE) = %d\n", rval);
    };
#endif
        if( FAILED(rval) )
        {
            SAFEDELETE(tmp);
            return rval;
        }

		// Continue to brighten the display
		while(ALPHA < 256)
		{
			fadeBegin = timeGetTime();

			rval = tmp->Fade(GetBack(),0,0,&sRect,(WORD)ALPHA);
            if( FAILED(rval) )
            {
                SAFEDELETE(tmp);
                return rval;
            }
			rval = Flip(TRUE,FALSE);
            if( FAILED(rval) )
            {
                SAFEDELETE(tmp);
                return rval;
            }

			fadeInc = (float)(duration + 1) / (float)(timeGetTime() - fadeBegin);

			// Do some boundry checking
			if (fadeInc > 256)
				fadeInc = 256;
			else if (fadeInc < 1)
				fadeInc = 1;

			ALPHA += (LONG)(512 / fadeInc);
		}

		// Just to be sure we have a pure copy of the source
		rval = tmp->DrawBlk(GetBack(), 0, 0, &sRect);
        if( FAILED(rval) )
        {
            SAFEDELETE(tmp);
            return rval;
        }

		rval = Flip(TRUE,FALSE);
#ifdef DebugFull
    WriteDebugString("Title screen should be visible now\n");
#endif
        if( FAILED(rval) )
        {
            SAFEDELETE(tmp);
            return rval;
        }
		
		// Cleanup the surface
		SAFEDELETE(tmp);
		tmp = NULL;
	}

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: FadeToBlack
// 
// PURPOSE: Fades the screen to black.  This function works for all graphics 
//          modes(NOTE: it is using the older FadeOut() function for 8bpp modes).
//          It uses software alpha blending to accomplish the fade therefore
//          it should work on all graphics cards. It makes a temp copy of
//          the front buffer then alpha blends different shades to the
//          back buffer then flips it to the front buffer.
//
// INPUT: duration - the amount of time for the entire fade in milliseconds.
//                   In 8bpp this does not hold true. Good values seem to be
//                   between 500-2000;
//
// RETURNS: 0 or > - success
//          < 0    - failed
//
// NOTES: I noticed that this only works if you have your window style set to
//        WS_POPUP.  I'll have to fix it later. -John Hebert
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::FadeToBlack(WORD duration)
{
    HRESULT rval;
	float fadeInc;
	LONG ALPHA;
	DWORD fadeBegin;
	CDXSurface* tmp;
	RECT sRect;
    RECT* pRect;
	DWORD w, h;
	POINT pt;
#ifdef DebugFull
  WriteDebugString("!!Start FadeToBlack()\n");
#endif
	if (m_BPP == 8)
	{
		// Sorry this does not support 8bpp mode.
		// Use FadeOut()
	}
	else
	{
		// Create a tmp surface the size of the back buffer and
		// copy the front buffer to it.
		w = GetBack()->GetWidth();
		h = GetBack()->GetHeight();

		// Need to get the RECT defining the CDX app window
        if(IsFullScreen() == FALSE)
        {
		    GetClientRect((HWND)m_hWnd, &sRect);
		    pt.x = pt.y = 0;
		    ClientToScreen((HWND)m_hWnd, &pt);
		    OffsetRect(&sRect, pt.x, pt.y);
        }
        else
        {
            pRect = GetBack()->GetClipRect();
            sRect = *pRect;
        }

		// Create the temp buffer
        tmp  = new CDXSurface();
        if(tmp == NULL)
            return -1;

        rval = tmp->Create(this, w, h, CDXMEM_SYSTEMONLY);
        if( FAILED(rval) )
        {
            SAFEDELETE(tmp);
            return rval;
        }

		GetFront()->DrawBlk(tmp, 0, 0, &sRect);

		ALPHA = 256;
		sRect.left   = 0;
		sRect.top    = 0;
		sRect.right  = w;
		sRect.bottom = h;

		// Continue fading while decrementing the ALPHA value.
		while(ALPHA > 0)
		{
			fadeBegin = timeGetTime();

			tmp->Fade(GetBack(),0,0,&sRect,(WORD)ALPHA);
			Flip(TRUE,FALSE);

			fadeInc = (float)(duration + 1) / (float)(timeGetTime() - fadeBegin);

			// Do some boundry checking
			if (fadeInc > 256)
				fadeInc = 256;
			else if (fadeInc < 1)
				fadeInc = 1;

			ALPHA -= (LONG)(512 / fadeInc);
		}

		// Cleanup the surface
		delete tmp;
		tmp = NULL;

		// Just to be sure we have a pure BLACK screen
		GetBack()->Fill(0);
		Flip(TRUE,FALSE);
	}

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetSurfaceScreen
// 
// PURPOSE: 
//
// INPUT: pCDXS - 
//
// RETURNS: nothing
//
// NOTE: This function is declared as a friend to the CDXSurface class and
//       therefore has direct access to its member variables.  This was done
//       to so we could keep the CDXSurface variables private.
//
//////////////////////////////////////////////////////////////////////////////////
void CDXScreen::SetSurfaceScreen(CDXSurface* pCDXS)
{
    pCDXS->m_Screen = this;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetSurfaceWidth
// 
// PURPOSE: 
//
// INPUT: pCDXS - 
//
// RETURNS: nothing
//
// NOTE: This function is declared as a friend to the CDXSurface class and
//       therefore has direct access to its member variables.  This was done
//       to so we could keep the CDXSurface variables private.
//
//////////////////////////////////////////////////////////////////////////////////
void CDXScreen::SetSurfaceWidth(CDXSurface* pCDXS, int w)
{
    pCDXS->m_PixelWidth = w;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetSurfaceHeight
// 
// PURPOSE: 
//
// INPUT: pCDXS - 
//
// RETURNS: nothing
//
// NOTE: This function is declared as a friend to the CDXSurface class and
//       therefore has direct access to its member variables.  This was done
//       to so we could keep the CDXSurface variables private.
//
//////////////////////////////////////////////////////////////////////////////////
void CDXScreen::SetSurfaceHeight(CDXSurface* pCDXS, int h)
{
    pCDXS->m_PixelHeight = h;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetSurfacePixelFormat
// 
// PURPOSE: 
//
// INPUT: pCDXS - 
//
// RETURNS: nothing
//
// NOTE: This function is declared as a friend to the CDXSurface class and
//       therefore has direct access to its member variables.  This was done
//       to so we could keep the CDXSurface variables private.
//
//////////////////////////////////////////////////////////////////////////////////
void CDXScreen::SetSurfacePixelFormat(CDXSurface* pCDXS)
{
    GetRGBFormat(pCDXS->GetDDS(),&(pCDXS->m_PixelFormat) );
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetSurfaceDDSPointer
// 
// PURPOSE: 
//
// INPUT: pCDXS - 
//
// RETURNS: nothing
//
// NOTE: This function is declared as a friend to the CDXSurface class and
//       therefore has direct access to its member variables.  This was done
//       to so we could keep the CDXSurface variables private.
//
//////////////////////////////////////////////////////////////////////////////////
void CDXScreen::SetSurfaceDDSPointer(CDXSurface* pCDXS, CDX_LPDIRECTDRAWSURFACE lpdds)
{
    pCDXS->m_lpDDS = lpdds;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: ChangeVideoMode
// 
// PURPOSE: This function will change the video mode to that specified in the 
//          arguments.  All data on the front and back buffers will be lost.
//          Any offscreen surfaces that you had created will be recreated under 
//          the new mode.  However, there is no gaurentee that surfaces that
//          were in video memory will be recreated in video memory (it will
//          attempt to keep surfaces in the same type of memory).  It will also
//          try to reload the graphic data that was on the surface by means
//          of the CDXSurface::m_FileName data member.
//
// INPUT: Width  - width in pixels of the new video mode
//        Height - height in pixels of the new video mode
//        BPP    - color depth (8, 16, 24, 32) If this is set to zero the new
//                 mode will be a windowed mode
//        bVGA   - 
//
// RETURNS: 0 or > - success
//          < 0    - failed
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXScreen::ChangeVideoMode(DWORD Width, DWORD Height, DWORD BPP, BOOL bVGA)
{
    HRESULT rval;
    CDXSurface* pSurface;
    LONG numSurfaces = 0;
    LPDIRECTDRAWPALETTE lpDDPalette = NULL;
    LPDIRECTDRAWCLIPPER lpClipper   = NULL;
	CDX_DDSURFACEDESC ddsd;
	CDX_DDSURFACEDESC ddsdtmp;
	CDX_LPDIRECTDRAWSURFACE lpdds;

    // If we are currently in windowed mode then we must delete the back buffer
    if( (m_lpDDSBack != NULL) && (m_bFullScreen != TRUE) )
        SAFEDELETE( m_lpDDSBack );

    //  ...else in full screen deleting the front buffer will take care of the back as well
    if( m_lpDDSFront != NULL )
        SAFEDELETE(  m_lpDDSFront );

    // Do they want Fullscreen or Windowed Mode?
    if( (BPP > 0) && (m_lpDD != NULL) )
    {
        rval = CreateFullScreen((HWND)GetWindowHandle(),Width, Height, BPP, bVGA);

        if( FAILED(rval) )
            return rval;
    }
    else if(m_lpDD != NULL)
    {
        // Restore the display to the users normal desktop
        m_lpDD->RestoreDisplayMode();

        // Change the window to the right size
        // ????  Width and Height need to be adjusted ????
        SetWindowPos((HWND)m_hWnd, HWND_TOP, 0,0,Width,Height, SWP_SHOWWINDOW);

        rval = CreateWindowed((HWND)GetWindowHandle(),Width, Height, m_UseClipper);
        SetWindowPos((HWND)m_hWnd, HWND_TOP, 0,0,Width,Height, SWP_SHOWWINDOW);

        if( FAILED(rval) )
            return rval;
    }
    else
    {
        return -1;
    }

    // Next we need to restore any offScreen buffers that existed.
    for(pSurface = m_SurfaceList.GetFirst(); pSurface != NULL; pSurface = m_SurfaceList.Next(pSurface))
    {
        // Setup the surface descriptor
        ZeroMemory(&ddsd, sizeof(ddsd));
        ddsd.dwSize   = sizeof(ddsd);
        ddsd.dwFlags  = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
        ddsd.dwWidth  = pSurface->GetWidth();
        ddsd.dwHeight = pSurface->GetHeight();

        // Was the old surface in video or system memory? ????
        pSurface->GetSurfaceDescriptor(&ddsdtmp);
        if(ddsdtmp.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
            ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        else
            ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

        // Did the old surface have texure enabled?
        if( pSurface->GetTextureEnabled() == TRUE )
            ddsd.ddsCaps.dwCaps |= DDSCAPS_TEXTURE;

        // Did the old surface have a palette attached to it?
        pSurface->GetDDS()->GetPalette(&lpDDPalette);

        // Did the old surface have a clipper attached to it?
        pSurface->GetDDS()->GetClipper(&lpClipper);

        // Delete the old DirectDraw Surface
        pSurface->GetDDS()->Release();

        // Create the new DirectDraw Surface
        lpdds = NULL;
        rval = m_lpDD->CreateSurface( &ddsd, &lpdds, NULL );

        // Did it fail the create?
        if( FAILED(rval) )
            return rval;

        //No failure, set the pointer in the CDXSurface object
        SetSurfaceDDSPointer(pSurface, lpdds);

        // Reset the pixel format structure
        SetSurfacePixelFormat(pSurface);

        // Set the colorKey to the upper-left hand corner pixel
        // This is a bit of a hack because the upper-left hand pixel
        // may not be the correct color key.
        pSurface->SetColorKey();

        // Reload the graphics to the new surface 
        pSurface->ReLoadBitmap();

        // Restore the palette if there was one.
        if(lpDDPalette != NULL)
            pSurface->GetDDS()->SetPalette(lpDDPalette);

        // Restore the clipper if there was one.
        if(lpClipper != NULL)
            pSurface->GetDDS()->SetClipper(lpClipper);
	}

	return 0;
}

HRESULT CDXScreen::SetClipperWindow(HWND hWnd)
{
  if (!m_UseClipper) return DD_OK;

  HRESULT result=DD_OK;  
  
  CDXSurface *pFrontSurf = GetFront();
  if (pFrontSurf == NULL) return DDERR_GENERIC;
  
  CDX_LPDIRECTDRAWSURFACE pDDS;
  pDDS = pFrontSurf->GetDDS();
  if (pDDS == NULL) return DDERR_GENERIC;
  
  LPDIRECTDRAWCLIPPER lpddClipper;
  result = pDDS->GetClipper(&lpddClipper); 
  
  if ((FAILED(result)) || (lpddClipper == NULL))
    return result; 
  
  // clear current window associated with clipper
  result = lpddClipper->SetHWnd(0, NULL);
  if (FAILED(result)) return result; 
  
  result = lpddClipper->SetHWnd(0, hWnd);
  return result; 
}

HRESULT CDXScreen::RestoreClipper()
{
  return (SetClipperWindow((HWND)m_hWnd));
}