//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximage.h,v $
// $Author: cocoaspud $
//
// $Log: cdximage.h,v $
// Revision 1.5  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.5  2000/08/29 19:42:28  mindcry
// changed #include <cdxfile.h> to #include "cdxfile.h" thank s to Howard Lightstone for pointing me that
//
// Revision 1.4  2000/08/21 21:20:28  mindcry
// Removed all fstreams and exchanged them with CDXFile. I had to do this
// cause fstream wasn't working right with MFC apps/games and also in some
// circumstances it crashed or not opened the file. Sorry for breaking
// backwards compatibility in some ways. But it was necessary.
//
// Revision 1.3  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.2  2000/04/26 18:45:43  hebertjo
// Made many small changes to fix numerous bugs and some enhancements.
//
// Revision 1.1.1.1  2000/04/22 16:09:23  hebertjo
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
// CDXImage.h: interface for the CDXImage class.
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef CDXIMAGE_H
#define CDXIMAGE_H

#include "cdxfile.h"
#include "cdximagebase.h"

const int headsize = 256;
const int footsize = 64;

class CDXImage  
{
public:
	CDXImage(void);
	virtual ~CDXImage(void);

	CDX_LPDIRECTDRAWSURFACE GetImage(CDX_LPDIRECTDRAW lpDD, const char* filename, BYTE memType = CDXMEM_VIDTHENSYS, DWORD type = CDXIMAGE_UNKNOWN );
    CDX_LPDIRECTDRAWSURFACE GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType = CDXMEM_VIDTHENSYS, DWORD type = CDXIMAGE_UNKNOWN );
    CDX_LPDIRECTDRAWSURFACE GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CDXFile* fsptr, BYTE memType = CDXMEM_VIDTHENSYS, DWORD type = CDXIMAGE_UNKNOWN);
    CDX_LPDIRECTDRAWSURFACE GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, FILE* fh, BYTE memType = CDXMEM_VIDTHENSYS, DWORD type = CDXIMAGE_UNKNOWN);
	CDX_LPDIRECTDRAWSURFACE CreateDDSurface(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType, DWORD type );

private:
	DWORD GetFileFormat(LONG lSize, CHAR* lpCache);
  char m_filename[_MAX_PATH+1];

};

#endif CDXIMAGE_H
