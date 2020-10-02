//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagebase.h,v $
// $Author: cocoaspud $
//
// $Log: cdximagebase.h,v $
// Revision 1.5  2001/10/13 15:19:59  cocoaspud
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
// Revision 1.3  2000/02/14 16:32:22  jhebert
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
// Revision 1.2  2000/01/28 16:51:16  MICHAELR
// Fixed #include problem where headers had to be in include path
//
// Revision 1.1  2000/01/19 23:08:16  jhebert
// Adding in the CDXImage files.
//
// 
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////
// CDXImageBase.h: interface for the CDXImageBase class.
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef CDXIMAGEBASE_H
#define CDXIMAGEBASE_H

//#define VC_EXTRALEAN

#include "cdx.h"
#include "cdxfile.h"

#define CDXIMAGE_UNKNOWN  0
#define CDXIMAGE_BMP      1
#define CDXIMAGE_PSD      2
#define CDXIMAGE_TGA      3
#define CDXIMAGE_PCX      4
#define CDXIMAGE_JPG      5
#define CDXIMAGE_PNG      6
#define CDXIMAGE_LAST     7

#define BIG2LIT(a)			(a>>8 & 0xff) | ((a & 0x00ff)<<8)
#define DWORDBIG2LIT(b)     MAKELONG( BIG2LIT(HIWORD(b)), BIG2LIT(LOWORD(b)) );


class CDXImageBase  
{
public:
	CDXImageBase();
	virtual ~CDXImageBase();

	//must implement in derived classes
	virtual BOOL Validate(const CHAR *head, const int headsize, const CHAR *foot, const int footsize) = 0;
	virtual CDX_LPDIRECTDRAWSURFACE GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType ) = 0;

protected:
	void			SetupBitmapInfo();
    BOOL			AllocateStorage( int w, int h, unsigned char bpp );
	BOOL			AllocateStorage( unsigned long size);
	BOOL			DeAllocateStorage(void);
    BOOL			OpenFile( const char *filename );
    BOOL			CloseFile(void);
    BOOL			CreateSurface(CDXSurface* pSurface, BYTE memType);
    CHAR*           GetStoragePtr(void)				{ return m_pStorage; }
	BOOL			IsInverted(void)				{ return m_IsInverted; }
	BOOL			IsMirrored(void)				{ return m_IsMirrored; }
    void			SetInverted(BOOL b)				{ m_IsInverted = b; }
	void			SetMirrored(BOOL b)				{ m_IsMirrored = b; }
    void			SetImageWidth(int w)			{ m_iWidth = w; }
    void			SetImageHeight(int h)			{ m_iHeight = h; }
    void			SetImageBPP(unsigned char bpp)	{ m_cBpp = bpp; }
    int				GetImageWidth(void)				{ return m_iWidth; }
    int				GetImageHeight(void)			{ return m_iHeight; }
    unsigned char	GetImageBPP(void)               { return m_cBpp; }
    unsigned long	GetSizeInBytes(void)			{ return m_iWidth * m_iHeight * (m_cBpp>>3); }
	CDX_LPDIRECTDRAWSURFACE CopyBits(void);
    BOOL			CreateSurface(CDX_LPDIRECTDRAW lpDD, BYTE memType);	

protected:
	BITMAPINFO*     m_pBitmapInfo;  // 
	BITMAPINFO		m_BitmapInfo;   // 
	CDXFile			m_file;         // file handler
    int				m_iWidth;		// signed because if -, that means image is backwards.
    int				m_iHeight;		// signed because if -, that means image is upside down.
    unsigned char   m_cBpp;			// char, valids are only 1,2,4,8,16,24,32
	CHAR*           m_pStorage;     // char, 8bit or less is accessed by byte
    BOOL            m_IsInverted;   // 
	BOOL			m_IsMirrored;   // 
	PALETTEENTRY*   m_pPalette;     // 
	unsigned int	m_iUsage;		// DIB_RGB_COLORS or DIB_PAL_COLORS
	CDX_LPDIRECTDRAWSURFACE m_lpdds;	
};

#endif CDXIMAGEBASE_H
