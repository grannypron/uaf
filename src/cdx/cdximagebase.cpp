//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagebase.cpp,v $
// $Author: stevens-madison $
//
// $Log: cdximagebase.cpp,v $
// Revision 1.5  2009/06/13 00:44:00  stevens-madison
// Temple Caster fix.  Revised method of initiating OnDraw.
//
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.1.1.1  2000/04/22 16:09:26  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 1.2  2000/02/14 16:32:22  jhebert
// Revamped the way we load images.  Before we used CDXFile to read (sometimes
// a BYTE at a time) from the file.  this was slow and inefficent and inflexibile.
// The new design internally uses a character buffer pre-loaded into memory.  So if
// you want to use CDXFile, FILE*, fstream, or your own file manipulation class you can.
// All you have to do is load the entire contents of the file into memory and pass the
// char* pointer.  This is much faster.  Also most of the file types work now.  The
// exceptions I found are:
// 	TGA - 256 color does not load
// 	PCX - true-color does not load correct
//
// Kudos to who ever fixes these.
//
// Revision 1.1  2000/01/19 23:08:16  jhebert
// Adding in the CDXImage files.
//
// 
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////

#include "CDXImageBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDXImageBase::CDXImageBase(void)
{
    m_pBitmapInfo = NULL;
    ZeroMemory(&m_BitmapInfo, sizeof(BITMAPINFO));
    m_iWidth      = 0;
    m_iHeight     = 0;
    m_cBpp        = 0;
    m_pStorage    = NULL;
    m_IsInverted  = FALSE;
	m_IsMirrored  = FALSE;
	m_pPalette    = NULL;
	m_iUsage      = DIB_RGB_COLORS;
    m_lpdds       = NULL;
}

CDXImageBase::~CDXImageBase(void)
{
    CloseFile();

	DeAllocateStorage();
}


//////////////////////////////////////////////////////////////
// Description   : open filename readonly and binary.
//////////////////////////////////////////////////////////////
BOOL CDXImageBase::OpenFile( const char* filename )
{
	CloseFile();

	m_file.OpenRead((char *)filename);
	
	return TRUE;
}


//////////////////////////////////////////////////////////////
// Description   : Close our file
//////////////////////////////////////////////////////////////
BOOL CDXImageBase::CloseFile( void )
{
	m_file.Close();

	return TRUE;
}


//////////////////////////////////////////////////////////////
// Description   :	Create and fill in bitmapinfo structure needed for StretchDIBits()
//////////////////////////////////////////////////////////////
void CDXImageBase::SetupBitmapInfo()
{
	if(m_pBitmapInfo)
	{
		free(m_pBitmapInfo);
		m_pBitmapInfo = NULL;
	}
	//allocate 256 color rgbquads for maximum usage
	m_pBitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO)+(sizeof(RGBQUAD)*256));
	if(m_pBitmapInfo==NULL)
		return;

	ZeroMemory(m_pBitmapInfo, sizeof(BITMAPINFO)+(sizeof(RGBQUAD)*256));

	m_pBitmapInfo->bmiHeader.biSize     = sizeof(m_pBitmapInfo->bmiHeader);
	if(IsMirrored())
		m_pBitmapInfo->bmiHeader.biWidth    = -m_iWidth;
	else
		m_pBitmapInfo->bmiHeader.biWidth    = m_iWidth;

	if(IsInverted())
		m_pBitmapInfo->bmiHeader.biHeight   = -m_iHeight;
	else
		m_pBitmapInfo->bmiHeader.biHeight   = m_iHeight;

	m_pBitmapInfo->bmiHeader.biPlanes   = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = m_cBpp;

}


//////////////////////////////////////////////////////////////
// Description   : Allocate memory storage for size of image
//////////////////////////////////////////////////////////////
BOOL CDXImageBase::AllocateStorage( int w, int h, unsigned char bpp)
{
	m_iWidth  = w;
	m_iHeight = h;
	m_cBpp    = bpp;

  // I modified this a bit!  (bpp>>3) is of no value when
  // bpp is less than 8.
  // I also changed it to allocate lines of a multiple of 4 bytes.
  // PRS  20170912
	//return AllocateStorage( w * h * (bpp>>3) );
	return AllocateStorage((((((w * bpp) + 7)/8) + 3)/4) * 4 * h);
}


//////////////////////////////////////////////////////////////
// Description   :	Allocate memory storage for size of image
//					If you call this version of AllocateStorage directly
//					you MUST call SetImageWidth/Height/Bpp functions before
//					calling CreateSurface;
//////////////////////////////////////////////////////////////
BOOL CDXImageBase::AllocateStorage( unsigned long size)
{
	DeAllocateStorage();

    m_pStorage = new CHAR[size];

	if(m_pStorage==NULL)
		return FALSE;

    return TRUE;
}



//////////////////////////////////////////////////////////////
// Description	 :	Free up memory
//////////////////////////////////////////////////////////////
BOOL CDXImageBase::DeAllocateStorage()
{
	DELETEARRAY(m_pStorage);

	if(m_pBitmapInfo)
	{
		free(m_pBitmapInfo);
		m_pBitmapInfo = NULL;
	}

	return TRUE;
}



//////////////////////////////////////////////////////////////
// Description	 :	Create surface of specified dimensions.
//////////////////////////////////////////////////////////////
BOOL CDXImageBase::CreateSurface(CDX_LPDIRECTDRAW lpDD, BYTE memType)
{
    HRESULT rval;
	CDX_DDSURFACEDESC ddsd;

    m_lpdds = NULL;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize   = sizeof(ddsd);

    switch (memType)
	{
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

	ddsd.dwFlags  = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH | DDSD_CKSRCBLT;
	ddsd.dwWidth  = m_iWidth;
	ddsd.dwHeight = m_iHeight;

	// If we can't create the surface, return NULL
    rval = lpDD->CreateSurface(&ddsd, &m_lpdds, NULL);

	if ( FAILED(rval) )
    {
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////
// Description	 :	copy bits from source to surface
//////////////////////////////////////////////////////////////
CDX_LPDIRECTDRAWSURFACE CDXImageBase::CopyBits()
{
	HDC hdc;
    m_lpdds->GetDC(&hdc);
	int rc = StretchDIBits(hdc,
					0,
					0,
					m_iWidth,
					m_iHeight,
					0,
					0,
					m_iWidth,
					m_iHeight,
					m_pStorage,
					m_pBitmapInfo,
					m_iUsage,
					SRCCOPY);

	m_lpdds->ReleaseDC(hdc);

	//free up the raw memory
	DeAllocateStorage();

	if (rc == GDI_ERROR)
		return NULL;

	return m_lpdds;
}

