//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagepsd.h,v $
// $Author: cocoaspud $
//
// $Log: cdximagepsd.h,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.1.1.1  2000/04/22 16:09:45  hebertjo
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
// Revision 1.1  2000/01/19 23:08:59  jhebert
// Adding CDXImage to the library.
//
// 
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// CDXImagePSD.h: interface for the CDXImagePSD class.
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef CDXIMAGEPSD_H
#define CDXIMAGEPSD_H

#include "cdximagebase.h"

#pragma pack(1)
struct _psdHeader 
{
	char	magic[4];	//magic identifier (8BPS)
	WORD	version;	//must be 1
	WORD	pad[3];		//unneeded data (always 0)
	WORD	channels;	//number of channels
	DWORD	height;
	DWORD	width;
	WORD	bpc;		//bits per channel. must be 8
	WORD	colormode;	//colormode.  must be 3
	DWORD   ModeDataCount;
};
#pragma pack()

#define MAX_PSD_CHANNELS	24

class CDXImagePSD : public  CDXImageBase
{
public:
	CDXImagePSD(void);
	virtual ~CDXImagePSD(void);

	CDX_LPDIRECTDRAWSURFACE GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType);
	BOOL Validate(const CHAR *head, const int headsize, const CHAR *foot, const int footsize);

private:
	void		UnPackPSD( CHAR* lpCache, BYTE* pixels, WORD channelCnt, WORD compression );
	_psdHeader	m_psdInfo;
};

#endif CDXIMAGEPSD_H
