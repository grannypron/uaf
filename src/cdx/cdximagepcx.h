//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagepcx.h,v $
// $Author: cocoaspud $
//
// $Log: cdximagepcx.h,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
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
// Revision 1.1  2000/01/19 23:08:59  jhebert
// Adding CDXImage to the library.
//
// 
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// CDXImagePCX.h: interface for the CDXImagePCX class.
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef CDXIMAGEPCX_H
#define CDXIMAGEPCX_H

#include "CDXImageBase.h"

// Structures relating to the PCX format
typedef struct _PCX_RGB
{
	unsigned char	Red;
	unsigned char	Green;
	unsigned char	Blue;
}  PCX_RGB, *LPPCX_RGB;


typedef struct _PCX_HEADER
{
	char	Manufacturer;
	char	Version;
	char	Encoding;
	char	BitsPerPixel;
	short	Xmin, Ymin, Xmax, Ymax;
	short	HDpi, VDpi;
	char	Colormap[48];
	char	Reserved;
	char	NPlanes;
	short	BytesPerLine;
	short	PaletteInfo;
	short	HscreenSize;
	short	VscreenSize;
	char	Filler[54];
} PCX_HEADER, *LPPCX_HEADER;

class CDXImagePCX : public CDXImageBase  
{
public:
	CDXImagePCX(void);
	virtual ~CDXImagePCX(void);

    CDX_LPDIRECTDRAWSURFACE GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType);
	BOOL Validate(const CHAR *head, const int headsize, const CHAR *foot, const int footsize);

private:
    int DecodePcxLine(char *dest, char* &src, int bytes);

	PCX_HEADER m_hdr;
	int        m_iColoursUsed;
};

#endif CDXIMAGEPCX_H
