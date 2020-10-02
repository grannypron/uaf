//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/ddutils.cpp,v $
// $Author: cocoaspud $
//
// $Log: ddutils.cpp,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.5  2001/07/24 03:57:20  istan
// Modified DDLoadPalette, based on DX8 version in DDUTILS.CPP
// Seems to fix the memory leak problem reported by gd^crazy
//
// Revision 1.4  2000/09/10 23:36:51  istan
// Remarked out the BCB IsMMX function for the time being and replaced with stub
//
// Revision 1.3  2000/05/08 20:06:06  hebertjo
// Fixed a conditional compile issue.
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.1.1.1  2000/04/22 16:13:50  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.8  2000/01/14 00:06:14  jhebert
// First checkin for v3.0
// Minor cleanup.
//
// 
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: ddutils.cpp,v 1.4 2001/10/13 15:19:59 cocoaspud Exp $";
#endif

#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#include "CDX.h"
#include "cdxscreen.h"
#include "cdxsurface.h"

////////////////////////////////////////////////////////////////////////
// Create a DirectDrawSurface from a bitmap resource.
////////////////////////////////////////////////////////////////////////
HRESULT CALLBACK EnumDisplayModesCallback( CDX_LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext )
{
    CDX_VIDEOMODESSTRUCT* Modes;

    Modes = ( CDX_VIDEOMODESSTRUCT* )lpContext;

    // browse through modes list until last element is reached
    while( Modes->NextMode != NULL )
        Modes = ( CDX_VIDEOMODESSTRUCT* )Modes->NextMode;

    // fill in elements
    Modes->Width    = lpDDSurfaceDesc->dwWidth;
    Modes->Height   = lpDDSurfaceDesc->dwHeight;
    Modes->BPP      = lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount;

    // add new element to list
    Modes->NextMode = new CDX_VIDEOMODESSTRUCT;
    Modes           = ( CDX_VIDEOMODESSTRUCT* )Modes->NextMode;
    ZeroMemory( Modes, sizeof( CDX_VIDEOMODESSTRUCT ) );

    return DDENUMRET_OK;
}

////////////////////////////////////////////////////////////////////////
// CALLBACK function for the DirectDraw Driver Enumeration
////////////////////////////////////////////////////////////////////////
#if DIRECTDRAW_VERSION >= CDX_DDVER
BOOL WINAPI DDEnumCallback( GUID FAR *lpGUID, LPSTR lpDesc, LPSTR lpName, LPVOID lpContext, HMONITOR hm )
#else
BOOL WINAPI DDEnumCallback( GUID FAR *lpGUID, LPSTR lpDesc, LPSTR lpName, LPVOID lpContext )
#endif
{
	LPDIRECTDRAW lpDD;
	CDX_DRIVERSTRUCT  *Drivers = (CDX_DRIVERSTRUCT*)lpContext;

    // browse through driver list until last element is reached
    while( Drivers->NextDriver != NULL )
        Drivers = Drivers->NextDriver;

    // try and create a DD device using the specified GUID
    if ( DirectDrawCreate( lpGUID, &lpDD, NULL ) != DD_OK )
    {
		// failed, so ignore this device
		return DDENUMRET_OK;
	}

	// get caps of this DD driver
	Drivers->DDcaps.dwSize = sizeof( Drivers->DDcaps );
	Drivers->HELcaps.dwSize = sizeof( Drivers->DDcaps );
	HRESULT hres = lpDD->GetCaps( &Drivers->DDcaps, &Drivers->HELcaps );
	RELEASE( lpDD );

	if ( hres != DD_OK )
	{
		// failed, so ignore this device
		return DDENUMRET_OK;
	}


    // fill elements
	
	if ( lpGUID == NULL )
	{   
		Drivers->lpGUID = NULL;
	}
	else
	{
		Drivers->lpGUID = &Drivers->guid;
		memcpy( &Drivers->guid, lpGUID, sizeof( GUID ) );
	}
	strcpy( Drivers->lpName, lpName );
	strcpy( Drivers->lpDesc, lpDesc );

#if DIRECTDRAW_VERSION >= CDX_DDVER
	Drivers->hMonitor	= hm;
#endif

    // add new element to list
	Drivers->NextDriver = new CDX_DRIVERSTRUCT;
	ZeroMemory( Drivers->NextDriver, sizeof( CDX_DRIVERSTRUCT ) );

	return DDENUMRET_OK;
}

////////////////////////////////////////////////////////////////////////
// Create a DirectDrawSurface from a bitmap resource.
////////////////////////////////////////////////////////////////////////
extern "C" CDX_LPDIRECTDRAWSURFACE DDLoadBitmap(CDX_LPDIRECTDRAW pdd, LPCSTR szBitmap, int dx, int dy, BOOL memoryType)
{
    HBITMAP                 hbm;
    BITMAP                  bm;
	CDX_DDSURFACEDESC       ddsd;
	CDX_LPDIRECTDRAWSURFACE pdds;

    //
    //  try to load the bitmap as a resource, if that fails, try it as a file
    //
    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx, dy, LR_CREATEDIBSECTION);

    if (hbm == NULL)
        hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

    if (hbm == NULL)
        return NULL;

    //
    // get size of the bitmap
    //
    GetObject(hbm, sizeof(bm), &bm);      // get size of bitmap

    //
    // create a DirectDrawSurface for this bitmap
    //
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;

	switch(memoryType) {
		case CDXMEM_SYSTEMONLY:
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			break;

		case CDXMEM_VIDEOONLY:
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
			break;

		case CDXMEM_VIDTHENSYS:
		default:
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
			break;
	}

    ddsd.dwWidth = bm.bmWidth;
    ddsd.dwHeight = bm.bmHeight;

    if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK)
        return NULL;

    DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);

    DeleteObject(hbm);

    return pdds;
}

////////////////////////////////////////////////////////////////////////
// Create a DirectDrawSurface of a specified size
// If ddsd is not NULL, it will be filled with the desc of the new surface
////////////////////////////////////////////////////////////////////////
CDX_LPDIRECTDRAWSURFACE DDCreateSurface(CDX_LPDIRECTDRAW pdd, DWORD width, DWORD height, CDX_LPDDSURFACEDESC  ddsd)
{
	BOOL                    free_ddsd;
	CDX_LPDIRECTDRAWSURFACE pdds;

	// Define what we want (surface description)
	if (ddsd == NULL)
	{
		free_ddsd = TRUE;

		ddsd = (CDX_LPDDSURFACEDESC) malloc(sizeof(CDX_DDSURFACEDESC));

		if (ddsd == NULL)
			return NULL;
	}
	else
	{
		free_ddsd = FALSE;
	}

	ZeroMemory(ddsd, sizeof(ddsd));

	ddsd->dwSize = sizeof(CDX_DDSURFACEDESC);
	ddsd->dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
	ddsd->ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd->dwWidth = width;
	ddsd->dwHeight = height;

	// If we can't create the surface, return NULL
	if (pdd->CreateSurface(ddsd, &pdds, NULL) != DD_OK){
		return NULL;
	}

	if (free_ddsd)
	{
		free(ddsd);
	}

	// Success: return the surface
	return pdds;
}

////////////////////////////////////////////////////////////////////////
// Create a DirectDrawSurface from a sized bitmap resource.
////////////////////////////////////////////////////////////////////////
CDX_LPDIRECTDRAWSURFACE DDLoadSizeBitmap(CDX_LPDIRECTDRAW pdd, LPCSTR szBitmap, int *dx, int *dy, BOOL memoryType)
{
	HBITMAP                 hbm;
	BITMAP                  bm;
	CDX_DDSURFACEDESC       ddsd;
	CDX_LPDIRECTDRAWSURFACE pdds;

	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hbm == NULL)
	hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

	if (hbm == NULL)
		 return NULL;

	//
	// get size of the bitmap
	//
	GetObject(hbm, sizeof(bm), &bm);

	//
	// create a DirectDrawSurface for this bitmap
	//
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;

	switch(memoryType) {
		case CDXMEM_SYSTEMONLY:
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			break;

		case CDXMEM_VIDEOONLY:
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
			break;

		case CDXMEM_VIDTHENSYS:
		default:
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
			break;
	}
	
	ddsd.dwWidth = bm.bmWidth;
	ddsd.dwHeight = bm.bmHeight;

	if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK){
		return NULL;
	}
	DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);

	DeleteObject(hbm);

	*dx = ddsd.dwWidth;
	*dy = ddsd.dwHeight;

	return pdds;
}

////////////////////////////////////////////////////////////////////////
// Load a bitmap from a file or resource into a directdraw surface.
// Normaly used to re-load a surface after a restore.
////////////////////////////////////////////////////////////////////////
HRESULT DDReLoadBitmap(CDX_LPDIRECTDRAWSURFACE pdds, LPCSTR szBitmap)
{
    HBITMAP             hbm;
    HRESULT             hr;

    //
    //  try to load the bitmap as a resource, if that fails, try it as a file
    //
    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

    if (hbm == NULL)
        hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

    if (hbm == NULL)
    {
        OutputDebugString("handle is null\n");
        return E_FAIL;
    }

    hr = DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    if (hr != DD_OK)
    {
        OutputDebugString("ddcopybitmap failed\n");
    }


    DeleteObject(hbm);
    return hr;
}

////////////////////////////////////////////////////////////////////////
// Draw a bitmap into a DirectDrawSurface.
////////////////////////////////////////////////////////////////////////
extern "C" HRESULT DDCopyBitmap(CDX_LPDIRECTDRAWSURFACE pdds, HBITMAP hbm, int x, int y, int dx, int dy)
{
    HDC                 hdcImage;
    HDC                 hdc;
    BITMAP              bm;
    HRESULT             hr;
	CDX_DDSURFACEDESC   ddsd;

    if (hbm == NULL || pdds == NULL)
        return E_FAIL;

    //
    // make sure this surface is restored.
    //
    pdds->Restore();

    //
    //  select bitmap into a memoryDC so we can use it.
    //
    hdcImage = CreateCompatibleDC(NULL);
    if (!hdcImage)
        OutputDebugString("createcompatible dc failed\n");
    SelectObject(hdcImage, hbm);

    //
    // get size of the bitmap
    //
    GetObject(hbm, sizeof(bm), &bm);    // get size of bitmap
    dx = dx == 0 ? bm.bmWidth  : dx;    // use the passed size, unless zero
    dy = dy == 0 ? bm.bmHeight : dy;

    //
    // get size of surface.
    //
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    pdds->GetSurfaceDesc(&ddsd);

    if ((hr = pdds->GetDC(&hdc)) == DD_OK)
    {
        StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y, dx, dy, SRCCOPY);
        pdds->ReleaseDC(hdc);
    }

    DeleteDC(hdcImage);

    return hr;
}

////////////////////////////////////////////////////////////////////////
// Create a DirectDraw palette object from a bitmap resoure.
// If the resource does not exist or NULL is passed create a
// default 332 palette.
////////////////////////////////////////////////////////////////////////
extern "C" HRESULT DDLoadPalette(CDX_LPDIRECTDRAW pdd,
								 CDX_LPDIRECTDRAWPALETTE* ppPalette,
								 LPCSTR szBitmap)
{
    HRSRC             hResource      = NULL;
    RGBQUAD*          pRGB           = NULL;
    BITMAPINFOHEADER* pbi = NULL;
    PALETTEENTRY      aPalette[256];
    HANDLE            hFile = NULL;
    DWORD             iColor;
    DWORD             dwColors;
    BITMAPFILEHEADER  bf;
    BITMAPINFOHEADER  bi;
    DWORD             dwBytesRead;

    if( pdd == NULL || szBitmap == NULL || ppPalette == NULL )
        return E_INVALIDARG;

    *ppPalette = NULL;

    //
    // build a 332 palette as the default.
    //
    for (iColor=0; iColor<256; iColor++)
    {
        aPalette[iColor].peRed   = (BYTE)(((iColor >> 5) & 0x07) * 255 / 7);
        aPalette[iColor].peGreen = (BYTE)(((iColor >> 2) & 0x07) * 255 / 7);
        aPalette[iColor].peBlue  = (BYTE)(((iColor >> 0) & 0x03) * 255 / 3);
        aPalette[iColor].peFlags = (BYTE)0;
    }


    //  Try to load the bitmap as a resource, if that fails, try it as a file
    hResource = FindResource( NULL, szBitmap, RT_BITMAP );
    if( hResource )
    {
        pbi = (LPBITMAPINFOHEADER) LockResource( LoadResource( NULL, hResource ) );       
        if( NULL == pbi )
            return E_FAIL;

        pRGB = (RGBQUAD*) ( (BYTE*) pbi + pbi->biSize );

        // Figure out how many colors there are
        if( pbi == NULL || pbi->biSize < sizeof(BITMAPINFOHEADER) )
            dwColors = 0;
        else if( pbi->biBitCount > 8 )
            dwColors = 0;
        else if( pbi->biClrUsed == 0 )
            dwColors = 1 << pbi->biBitCount;
        else
            dwColors = pbi->biClrUsed;

        //  A DIB color table has its colors stored BGR not RGB
        //  so flip them around.
        for( iColor = 0; iColor < dwColors; iColor++ )
        {
            aPalette[iColor].peRed   = pRGB[iColor].rgbRed;
            aPalette[iColor].peGreen = pRGB[iColor].rgbGreen;
            aPalette[iColor].peBlue  = pRGB[iColor].rgbBlue;
            aPalette[iColor].peFlags = 0;
        }

        return pdd->CreatePalette( DDPCAPS_8BIT, aPalette, ppPalette, NULL );
    }

    // Attempt to load bitmap as a file
    hFile = CreateFile( szBitmap, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
    if( INVALID_HANDLE_VALUE != hFile )
	{
		// Read the BITMAPFILEHEADER
		ReadFile( hFile, &bf, sizeof(bf), &dwBytesRead, NULL );
		if( dwBytesRead != sizeof(bf) )
		{
			CloseHandle( hFile );
			return E_FAIL;
		}

		// Read the BITMAPINFOHEADER
		ReadFile( hFile, &bi, sizeof(bi), &dwBytesRead, NULL );
		if( dwBytesRead != sizeof(bi) )
		{
			CloseHandle( hFile );
			return E_FAIL;
		}

		// Read the PALETTEENTRY 
		ReadFile( hFile, aPalette, sizeof(aPalette), &dwBytesRead, NULL );
		if( dwBytesRead != sizeof(aPalette) )
		{
			CloseHandle( hFile );
			return E_FAIL;
		}

		CloseHandle( hFile );

		// Figure out how many colors there are
		if( bi.biSize != sizeof(BITMAPINFOHEADER) )
			dwColors = 0;
		else if (bi.biBitCount > 8)
			dwColors = 0;
		else if (bi.biClrUsed == 0)
			dwColors = 1 << bi.biBitCount;
		else
			dwColors = bi.biClrUsed;

		//  A DIB color table has its colors stored BGR not RGB
		//  so flip them around since DirectDraw uses RGB
		for( iColor = 0; iColor < dwColors; iColor++ )
		{
			BYTE r = aPalette[iColor].peRed;
			aPalette[iColor].peRed  = aPalette[iColor].peBlue;
			aPalette[iColor].peBlue = r;
		}
	}

    return pdd->CreatePalette( DDPCAPS_8BIT, aPalette, ppPalette, NULL );
}

////////////////////////////////////////////////////////////////////////
// Convert a RGB color to a pysical color.
// We do this by leting GDI SetPixel() do the color matching
// then we lock the memory and see what it got mapped to.
////////////////////////////////////////////////////////////////////////
extern "C" DWORD DDColorMatch(CDX_LPDIRECTDRAWSURFACE pdds, COLORREF rgb)
{
    COLORREF rgbT;
    HDC hdc;
    DWORD dw = CLR_INVALID;
    HRESULT hres;
	CDX_DDSURFACEDESC ddsd;

    //
    //  use GDI SetPixel to color match for us
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);             // save current pixel value
        SetPixel(hdc, 0, 0, rgb);               // set our value
        pdds->ReleaseDC(hdc);
    }

    //
    // now lock the surface so we can read back the converted color
    //
    ddsd.dwSize = sizeof(ddsd);
    while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
        ;

    if (hres == DD_OK)
    {
        dw  = *(DWORD *)ddsd.lpSurface;                     // get DWORD
        dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount)-1;  // mask it to bpp
        pdds->Unlock(NULL);
    }

    //
    //  now put the color that was there back.
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        SetPixel(hdc, 0, 0, rgbT);
        pdds->ReleaseDC(hdc);
    }

    return dw;
}

////////////////////////////////////////////////////////////////////////
// Set a color key for a surface, given a RGB.
// If you pass CLR_INVALID as the color key, the pixel
// in the upper-left corner will be used.
////////////////////////////////////////////////////////////////////////
extern "C" HRESULT DDSetColorKey(CDX_LPDIRECTDRAWSURFACE pdds, COLORREF rgb)
{
    DDCOLORKEY          ddck;

    ddck.dwColorSpaceLowValue  = DDColorMatch(pdds, rgb);
    ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
    return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}

////////////////////////////////////////////////////////////////////////
// Fills out a RGBFORMAT structure.
////////////////////////////////////////////////////////////////////////
BOOL GetRGBFormat( CDX_LPDIRECTDRAWSURFACE Surface, RGBFORMAT* rgb)
{
    BYTE shiftcount;
	CDX_DDSURFACEDESC ddsd;

	//get a surface description
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_PIXELFORMAT;

    if (Surface->GetSurfaceDesc ( &ddsd ) != DD_OK )
        return FALSE;

	// Check if the surface is 4 or 8 bpp
	if (ddsd.ddpfPixelFormat.dwRGBBitCount <= 8)
	{
		rgb->bpp = ddsd.ddpfPixelFormat.dwRGBBitCount;
		
		rgb->depth.rgbRed      = 0;
		rgb->position.rgbRed   = 0;

		rgb->depth.rgbGreen    = 0;
		rgb->position.rgbGreen = 0;

		rgb->depth.rgbBlue     = 0;
		rgb->position.rgbBlue  = 0;
		
		return TRUE;
	}

	//Get the RED channel data for 15, 16, 24, or 32 bpp surface
    shiftcount = 0;
    while ( !(ddsd.ddpfPixelFormat.dwRBitMask & 1) && (shiftcount < 32))
    {
        ddsd.ddpfPixelFormat.dwRBitMask >>= 1;
        shiftcount++;
    }

	// depth is the highest possible value that this channel could
	// be set to. It is cast to a BYTE because 255 is the highest
	// value for all modes.
    rgb->depth.rgbRed = (BYTE) ddsd.ddpfPixelFormat.dwRBitMask;

	// Position represents where the color channel starts, bitwise, 
	// in the pixel. Another way of saying this that Position is 
	// eqaul to the number of zeros to the right of the channel's
	// bitmask.
    rgb->position.rgbRed = shiftcount;

    //Get the GREEN channel data for 15, 16, 24, or 32 bpp surface
    shiftcount = 0;
    while ( !(ddsd.ddpfPixelFormat.dwGBitMask & 1) && (shiftcount < 32))
    {
        ddsd.ddpfPixelFormat.dwGBitMask >>= 1;
        shiftcount++;
    }
    rgb->depth.rgbGreen =(BYTE)ddsd.ddpfPixelFormat.dwGBitMask;
    rgb->position.rgbGreen = shiftcount;

    //Get the BLUE channel data for 15, 16, 24, or 32 bpp surface
    shiftcount = 0;
    while ( !(ddsd.ddpfPixelFormat.dwBBitMask & 1) && (shiftcount < 32))
    {
        ddsd.ddpfPixelFormat.dwBBitMask >>= 1;
        shiftcount++;
    }
    rgb->depth.rgbBlue =(BYTE)ddsd.ddpfPixelFormat.dwBBitMask;
    rgb->position.rgbBlue = shiftcount;

	//  Determine the bpp for the surface. (15, 16, 24, or 32)
	if (ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB)
	{
		if (ddsd.ddpfPixelFormat.dwRGBBitCount == 16)
		{
			if ((rgb->depth.rgbBlue + rgb->depth.rgbGreen + rgb->depth.rgbRed) == 93)
				rgb->bpp = 15;
			else
				rgb->bpp = 16;
		}
		else if (ddsd.ddpfPixelFormat.dwRGBBitCount == 24)
			rgb->bpp = 24;
		else if (ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
			rgb->bpp = 32;
		else
			rgb->bpp = 0;
	}
	else
	{
		rgb->bpp = 0;
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// Is the machine capable of MMX instructions. Functions for both
// MSVC and Borland C. Alpha doesn't support MMX, it has MVI.
////////////////////////////////////////////////////////////////////////
#ifdef _M_ALPHA
BOOL __cdecl IsMMX(void)
{
	return FALSE;
}
#else
#ifdef _MSC_VER
BOOL __cdecl IsMMX(void)
{
    SYSTEM_INFO si;
    int nCPUFeatures=0;
    GetSystemInfo(&si);
    if (si.dwProcessorType != PROCESSOR_INTEL_386 && si.dwProcessorType != PROCESSOR_INTEL_486)
    {
        try
        {
            __asm
            {
                ; we must push/pop the registers << CPUID>>  writes to, as the
				; optimiser doesn't know about << CPUID>> , and so doesn't expect
				; these registers to change.
                push eax
                push ebx
                push ecx
                push edx

                ; << CPUID>> 
                ; eax=0,1,2 -> CPU info in eax,ebx,ecx,edx
                mov eax,1
                _emit 0x0f
                _emit 0xa2
                mov nCPUFeatures,edx

                pop edx
                pop ecx
                pop ebx
                pop eax
            }
        }
        catch(...) // just to be sure...
        {
			return false;
        }
    }
    return (nCPUFeatures & 0x00800000) != 0;
}
#else
#if 0
// MichaelR: I've remarked this out for the time being
// because 1) its not really used yet. 2) its causing problems with
// the compilers anyways 3) So I don't have to keep modifying my snapshot
// everytime I redownload it

typedef void (*ASMProc)(); 
BOOL __cdecl IsMMX(void)
{

    SYSTEM_INFO si;
    int nCPUFeatures=0;
   
    char xMMXCode[2] = {0x0f, 0xa2};
    ASMProc function_xMMXCode;

    GetSystemInfo(&si);
    if (si.dwProcessorType != PROCESSOR_INTEL_386 && si.dwProcessorType != PROCESSOR_INTEL_486)
    {
        try
        {
            __asm
            {
                push eax
                push ebx
                push ecx
                push edx

                mov eax,1
            }
            function_xMMXCode = (ASMProc)xMMXCode;
            function_xMMXCode();
            __asm {
                mov nCPUFeatures,edx

                pop edx
                pop ecx
                pop ebx
                pop eax
            }
        }
        catch(...) // just to be sure...
        {
            return false;
        }
    }
    return (nCPUFeatures & 0x00800000) != 0;
}
#endif

BOOL __cdecl IsMMX(void)
{
	return FALSE;
}

#endif

#endif // #ifdef _M_ALPHA