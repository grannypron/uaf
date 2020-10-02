//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagepng.h,v $
// $Author: cocoaspud $
//
// $Revision: 1.3 $
//////////////////////////////////////////////////////////////////////////////////

#ifndef CDXIMAGEPNG_H
#define CDXIMAGEPNG_H

#include "CDXImageBase.h"

class CDXImagePNG : public CDXImageBase  
{
public:
	CDXImagePNG(void);
	virtual ~CDXImagePNG(void);

	CDX_LPDIRECTDRAWSURFACE GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType);
	BOOL Validate(const CHAR *head, const int headsize, const CHAR *foot, const int footsize);

};


#endif CDXImagePNG
