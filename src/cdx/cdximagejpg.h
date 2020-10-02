//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagejpg.h,v $
// $Author: cocoaspud $
//
// $Log: cdximagejpg.h,v $
// Revision 1.6  2001/10/13 15:19:59  cocoaspud
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
// $Revision: 1.6 $
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// --ORIGINAL Notice-- DO NOT REMOVE
//
//	Comments:
//	Thanks to Richard Turner for the code to use the OLE system to load JPEG
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef CDXIMAGEJPG_H
#define CDXIMAGEJPG_H

#include "CDXImageBase.h"
#include <mmreg.h>


//	for DWORD aligning a buffer
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

class CDXImageJPG : public CDXImageBase  
{
public:
	CDXImageJPG();
	virtual ~CDXImageJPG();

	CDX_LPDIRECTDRAWSURFACE GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType );
	BOOL Validate(const char *head, const int headsize, const char *foot, const int footsize);
};


#endif CDXIMAGEJPG_H
