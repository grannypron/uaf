//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxtypes.h,v $
// $Author: cocoaspud $
//
// $Log: cdxtypes.h,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.1.1.1  2000/04/22 16:13:41  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 1.2  2000/01/21 19:13:20  jhebert
// Initial checkin for v3.0.
//
// Revision 1.1  1999/07/15 03:34:16  MICHAELR
// Added to repository
//
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////

#ifndef _H_CDXTYPES
#define _H_CDXTYPES

/////////////////////////////////////////////////////////////////////
// signed types
/////////////////////////////////////////////////////////////////////
#undef INT8
#define INT8 signed char

#undef INT16
#define INT16 signed short

#undef INT32
#define INT32 signed long

#undef INT64
#define INT64 __int64


/////////////////////////////////////////////////////////////////////
// unsigned types
/////////////////////////////////////////////////////////////////////
#undef UINT8
#define UINT8 unsigned char

#undef UINT16
#define UINT16 unsigned short

#undef UINT32
#define UINT32 unsigned long

#undef UINT64
#define UINT64 unsigned __int64


#undef CDXBOOL
#define CDXBOOL int

#endif _H_CDXTYPES