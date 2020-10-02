//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdx.h,v $
// $Author: cocoaspud $
//
// $Log: cdx.h,v $
// Revision 1.5  2010/02/28 21:35:25  cocoaspud
// mods for compat with Visual Studio 2005
//
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.3  2001/07/24 03:55:26  istan
// Modification of declare of DDLoadPalette
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.1.1.1  2000/04/22 16:09:12  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.18  2000/03/21 00:04:47  jhebert
// Forgot to rename the header in this file for the MIDI stuff.
//
// Revision 2.17  2000/03/20 22:52:39  jhebert
// Changes to add MIDI support.
//
// Revision 2.16  2000/01/21 19:06:18  jhebert
// Added cdxtypes.h include.
//
// Revision 2.15  2000/01/13 23:48:51  jhebert
// first checkin for v3.0
// minor rearrangment and cleanup.
//
// 
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////
#ifndef CDX_H
#define CDX_H

//////////////////////////////////////////////////////////////////////////////////
//
//   The following section if for building CDX as a DLL
//   Begin CDX DLL section 
//
//      NOTE: _CDX_BUILDDLL & _CDX_USEDLL should NOT be used together
//
#if defined(_CDX_BUILDDLL) && defined(_CDX_USEDLL)
#error Sorry, you cannot use _CDX_BUILDDLL & _CDX_USEDLL together
#endif

// this is initially defined as nothing
#define CDXAPI

// this should be specified to make CDX as a DLL
#ifdef _CDX_BUILDDLL
#undef CDXAPI
#define CDXAPI __declspec(dllexport)
#endif

// the following should be specified if the user wants to
// use cdx.dll.  Otherwise chaos ensues when you try to delete objects
#ifdef _CDX_USEDLL
#undef CDXAPI
#define CDXAPI __declspec(dllimport)
#endif
//
//   End CDX DLL section 
//
//////////////////////////////////////////////////////////////////////////////////

// If you want to support DX running under NT 4.0 (currently only 
// supporting DX3) then define NTDX3 in your program
#ifdef NTDX3

#define DIRECTDRAW_VERSION	      0x0300
#define DIRECTINPUT_VERSION	      0x0300
#define DIRECTSOUND_VERSION	      0x0300
#define CDX_LPDIRECTDRAWSURFACE   LPDIRECTDRAWSURFACE
#define CDX_LPDIRECTDRAW          LPDIRECTDRAW
#define CDX_DDSURFACEDESC         DDSURFACEDESC
#define CDX_LPDDSURFACEDESC       LPDDSURFACEDESC
#define CDX_DDSCAPS               DDSCAPS
#define CDX_LPDIRECTDRAWPALETTE   LPDIRECTDRAWPALETTE

#else

// CDX currently supports DX7
#define DIRECTDRAW_VERSION	      0x0700
#define DIRECTINPUT_VERSION	      0x0700
#define DIRECTSOUND_VERSION	      0x0700
#define CDX_LPDIRECTDRAWSURFACE   LPDIRECTDRAWSURFACE7
#define CDX_LPDIRECTDRAW          LPDIRECTDRAW7
#define CDX_DDSURFACEDESC         DDSURFACEDESC2
#define CDX_LPDDSURFACEDESC       LPDDSURFACEDESC2
#define CDX_DDSCAPS               DDSCAPS2
#define CDX_LPDIRECTDRAWPALETTE   LPDIRECTDRAWPALETTE

#endif

// CDX Currently support DirectX 7
#define CDX_DDVER		0x0700
#define CDX_DIVER		0x0700
#define CDX_DSVER		0x0700

// System includes
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>

// DirectX includes
#include <ddraw.h>
#include <dinput.h>
#include <dsound.h>
//#include <dplay.h>
//#include <dplobby.h>

// CDX Includes
#include "cdxtypes.h"
#include "cdxlog.h"
#include "cdxstructs.h"
#include "cdxsurface.h"
#include "cdxscreen.h"


//////////////////////////////////////////////////////////////////////////////////
// For those lazy coders, #define CDXINCLUDEALL and all cdx headers will get 
// included. Otherwise, you need to #include all header files you will need 
// (depending on modules you are going to use)
//////////////////////////////////////////////////////////////////////////////////
#ifdef CDXINCLUDEALL
	#include "cdxfile.h"
	#include "cdxsurface.h"
	#include "cdxlayer.h"
	#include "cdxtile.h"
	#include "cdxmap.h"
	#include "cdxsprite.h"
	#include "cdxsound.h"
	#include "cdxmusic.h"
	#include "cdxinput.h"
	//#include "cdxlink.h"
	#include "cdxresource.h"
	#include "cdxpacker.h"
    //#include "cdxmidi.h"
    #include "cdximage.h"
#endif


//////////////////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////////////////
extern "C"
{
	// CDX Functions
	//extern CDXAPI void Clip(int *DestX, int *DestY, RECT *SrcRect, RECT *DestRect);

	// DDUTILS Functions
	extern CDXAPI HRESULT DDLoadPalette(CDX_LPDIRECTDRAW pdd, CDX_LPDIRECTDRAWPALETTE* ppPalette, LPCSTR szBitmap);
	extern CDXAPI CDX_LPDIRECTDRAWSURFACE DDLoadBitmap(CDX_LPDIRECTDRAW pdd, LPCSTR szBitmap, int dx, int dy, BOOL memoryType);
	extern CDXAPI CDX_LPDIRECTDRAWSURFACE DDLoadSizeBitmap(CDX_LPDIRECTDRAW pdd, LPCSTR szBitmap, int *dx, int *dy, BOOL memoryType);

	// Why no extern CDXAPI on this one???
	CDX_LPDIRECTDRAWSURFACE DDCreateSurface(CDX_LPDIRECTDRAW pdd, DWORD width, DWORD height, CDX_LPDDSURFACEDESC ddsd);

	// Why no CDXAPI on these ???
	extern HRESULT       DDReLoadBitmap(CDX_LPDIRECTDRAWSURFACE pdds, LPCSTR szBitmap);
	extern HRESULT       DDCopyBitmap(CDX_LPDIRECTDRAWSURFACE pdds, HBITMAP hbm, int x, int y, int dx, int dy);
	extern DWORD         DDColorMatch(CDX_LPDIRECTDRAWSURFACE pdds, COLORREF rgb);
	extern HRESULT       DDSetColorKey(CDX_LPDIRECTDRAWSURFACE pdds, COLORREF rgb);
    extern BOOL          GetRGBFormat(CDX_LPDIRECTDRAWSURFACE Surface, RGBFORMAT* rgb);
	extern BOOL __cdecl  IsMMX(void);

	// DirectX Errors
	extern CDXAPI void DDError(HRESULT hErr, void* hWnd, LPCSTR szFile, int iLine);
	extern CDXAPI void CDXError( CDXScreen *Screen, char *str, ...);
	extern CDXAPI void DSError(HRESULT hErr, void* hWnd);
	extern CDXAPI char* DXErrorToString( HRESULT hErr );
}

//////////////////////////////////////////////////////////////////////////////////
// MACROS
//////////////////////////////////////////////////////////////////////////////////
#define SAFEDELETE(x) if(x != NULL) { delete x; x = NULL; }
#define DELETEARRAY(x) if(x != NULL) { delete [] x; x = NULL; }
#define RELEASE(x) if(x != NULL) { x->Release(); x = NULL; }
#define ODS(a) OutputDebugString(a)

#endif
