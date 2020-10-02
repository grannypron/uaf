//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagejpg.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdximagejpg.cpp,v $
// Revision 1.3  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.2  2001/01/23 19:17:09  istan
// Added support for JPEG files via OLE
// Thanks to Richard Turner for the code
//
// Revision 1.1.1.1  2000/04/22 16:09:40  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 1.1  2000/01/19 23:08:59  jhebert
// Adding CDXImage to the library.
//
// 
//
// $Revision: 1.3 $
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// --ORIGINAL Notice-- DO NOT REMOVE
//
//	Comments:
//	Thanks to Richard Turner for the code to use the OLE system to load JPEG
//
//////////////////////////////////////////////////////////////////////////////////

#include "CDXImageJPG.h"
#include <ocidl.h>
#include <olectl.h>

#define HIMETRIC_INCH	2540

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDXImageJPG::CDXImageJPG()
{
}

CDXImageJPG::~CDXImageJPG()
{
}

BOOL CDXImageJPG::Validate(const char *head, const int headsize, const char *foot, const int footsize)
{
	if (headsize < 10)
		return FALSE;

	if (((BYTE)head[0] == 0xFF) && ((BYTE)head[2] == 0xFF) && (strncmp(&head[6], "JFIF", 4)==0))
		return TRUE; // JPG

	return FALSE;
}

CDX_LPDIRECTDRAWSURFACE CDXImageJPG::GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType )
{
	LPVOID pvData = NULL;
	// alloc memory based on file size
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, lSize);
	pvData = GlobalLock(hGlobal);

	DWORD dwBytesRead = lSize;
	// read file and store in global memory
	memcpy(pvData, lpCache, lSize);

	GlobalUnlock(hGlobal);

	LPSTREAM pstm = NULL;
	// create IStream* from global memory
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
	if ( (!SUCCEEDED(hr)) || (pstm == NULL))
		return NULL;

	// Create IPicture from image file
	LPPICTURE gpPicture=NULL;
	hr = ::OleLoadPicture(pstm, lSize, FALSE, IID_IPicture, (LPVOID *)&gpPicture);
	if ( (!SUCCEEDED(hr)) || (gpPicture == NULL))
		return NULL;

	pstm->Release();

	// get width and height of picture
	long hmWidth;
	long hmHeight;
	gpPicture->get_Width(&hmWidth);
	gpPicture->get_Height(&hmHeight);

	HDC Screen = ::CreateIC("Display", NULL, NULL, NULL);

	// convert himetric to pixels
	int nWidth	= MulDiv(hmWidth, GetDeviceCaps(Screen, LOGPIXELSX), HIMETRIC_INCH);
	int nHeight	= MulDiv(hmHeight, GetDeviceCaps(Screen, LOGPIXELSY), HIMETRIC_INCH);

	::DeleteDC(Screen);

	SetImageWidth(nWidth);
	SetImageHeight(nHeight);
	SetImageBPP(24);
	SetInverted(TRUE);

	//create surface, this uses member variables set during AllocateStorage
	if(!CreateSurface(lpDD, memType))
		return NULL;

	HDC hdc;
	m_lpdds->GetDC(&hdc);

	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = nWidth;
	rc.bottom = nHeight;

	// display picture using IPicture::Render
	gpPicture->Render(hdc, 0, 0, nWidth, nHeight, 0, hmHeight, hmWidth, -hmHeight, &rc);
	
	m_lpdds->ReleaseDC(hdc);

	if (gpPicture)
		gpPicture->Release();

	return m_lpdds;
}

