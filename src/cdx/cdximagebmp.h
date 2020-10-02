//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagebmp.h,v $
// $Author: cocoaspud $
//
// $Log: cdximagebmp.h,v $
// Revision 1.5  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.1.1.1  2000/04/22 16:09:31  hebertjo
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

//////////////////////////////////////////////////////////////////////////////////
// CDXImageBMP.h: interface for the CDXImageBMP class.
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef CDXIMAGEBMP_H
#define CDXIMAGEBMP_H

#include "CDXImageBase.h"

class CDXImageBMP : public CDXImageBase  
{
public:
	CDXImageBMP(void);
	virtual ~CDXImageBMP(void);

	CDX_LPDIRECTDRAWSURFACE GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType);
	BOOL Validate(const CHAR *head, const int headsize, const CHAR *foot, const int footsize);

private:
	BOOL ReadRLE8Format(CHAR* lpCache, DWORD offset);

	LPBITMAPINFO		m_pbi;
	BITMAPINFO			m_bi;
	BITMAPFILEHEADER	m_bif;
	int					m_iColoursUsed;
};

#endif CDXIMAGEBMP_H
