//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxdebug.h,v $
// $Author: cocoaspud $
//
// $Log: cdxdebug.h,v $
// Revision 1.5  2010/02/28 21:34:49  cocoaspud
// *** empty log message ***
//
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.1.1.1  2000/04/22 16:09:12  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 1.1  1999/05/30 03:32:04  MICHAELR
// add to the repository
//
//
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////
#ifndef CDXDEBUG_H
#define CDXDEBUG_H

#include <assert.h>

#ifdef  NDEBUG

#define CDXASSERT(exp)     ((void)0)

#else

//#define CDXASSERT(exp) (void)( (exp) || (_assert(#exp, __FILE__, __LINE__), 0) )
#define CDXASSERT(exp) (void)( assert(#exp) )

#endif

#endif CDXDEBUG_H
