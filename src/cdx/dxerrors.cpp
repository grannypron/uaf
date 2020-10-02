//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/dxerrors.cpp,v $
// $Author: stevens-madison $
//
// $Log: dxerrors.cpp,v $
// Revision 1.9  2015/08/27 19:34:05  stevens-madison
// Update to Visual Studio 2015 Community
//
// Revision 1.8  2013/12/16 01:35:59  cocoaspud
// fixed a few issues pointed out by static analysis tool
//
// Revision 1.7  2010/02/28 21:36:39  cocoaspud
// mods for compat with Visual Studio 2005
//
// Revision 1.6  2002/03/02 13:37:08  cocoaspud
// Completed menu updates for char creation menus
//
// Revision 1.2  2001/11/25 00:39:10  istan
// Added function DisplayFatalError
// Changed cp var in CDXError to be dynamically created
//
// Revision 1.1.1.1  2000/04/22 16:13:58  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.10  2000/01/28 16:30:43  MICHAELR
// Removed dependency on CDX3D.H
// Added include for d3drmwin.h for D3D errors
//
// Revision 2.9  2000/01/14 00:02:15  jhebert
// First checkin for v3.0
// Minor change to comply with other changes
//
// 
//
// $Revision: 1.9 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: dxerrors.cpp,v 1.9 2015/08/27 19:34:05 stevens-madison Exp $";
#endif

#include "CDX.h"
//#include <d3drmwin.h>
#include "cdxscreen.h"
#include <stdarg.h>

////////////////////////////////////////////////////////////////////////
// Displays an error message box and quits the application
////////////////////////////////////////////////////////////////////////
void CDXError( CDXScreen *Screen, char *str, ... )
{
    va_list ap;
	char *cp = NULL;

	cp = new char[1024];

    va_start(ap, str);
    vsprintf(cp, str, ap);
    va_end(ap);

    if(Screen != NULL)
    {
        Screen->GetDD()->FlipToGDISurface();
        MessageBox((HWND)Screen->GetWindowHandle(), cp, "Error", MB_OK | MB_ICONERROR);
    }
    else
    {
        MessageBox(NULL, cp, "Error", MB_OK | MB_ICONERROR);
    }

	DELETEARRAY(cp);

	PostQuitMessage(0);
}

////////////////////////////////////////////////////////////////////////
// Displays an error message box (but does NOT quit app)
////////////////////////////////////////////////////////////////////////
void DisplayFatalError( CDXScreen *Screen, char *str, ... )
{
    va_list ap;
    char *cp = NULL;

	cp = new char[1024];

    va_start(ap, str);
    vsprintf(cp, str, ap);
    va_end(ap);

    if(Screen != NULL)
    {
        Screen->GetDD()->FlipToGDISurface();
		
		// show the mouse pointer if its hidden
		if (GetCursor() == NULL)
			//SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
      SetCursor(LoadCursor(NULL, IDC_ARROW));

        MessageBox((HWND)Screen->GetWindowHandle(), cp, "Error", MB_OK | MB_ICONERROR);
    }
    else
    {
        MessageBox(NULL, cp, "Error", MB_OK | MB_ICONERROR);
    }

	DELETEARRAY(cp);
}

////////////////////////////////////////////////////////////////////////
// Traces an error (DirectDraw).
////////////////////////////////////////////////////////////////////////
void DDError(HRESULT hErr, void* hWnd, LPCSTR szFile, int iLine)
{
	char dderr[256];
	char err[1024];

	switch (hErr)
	{
		case DDERR_ALREADYINITIALIZED : sprintf(dderr, "DDERR_ALREADYINITIALIZED"); break;
		case DDERR_CANNOTATTACHSURFACE : sprintf(dderr, "DDERR_CANNOTATTACHSURFACE"); break;
		case DDERR_CANNOTDETACHSURFACE : sprintf(dderr, "DDERR_CANNOTDETACHSURFACE"); break;
		case DDERR_CURRENTLYNOTAVAIL : sprintf(dderr, "DDERR_CURRENTLYNOTAVAIL"); break;
		case DDERR_EXCEPTION : sprintf(dderr, "DDERR_EXCEPTION"); break;
		case DDERR_GENERIC : sprintf(dderr, "DDERR_GENERIC"); break;
		case DDERR_HEIGHTALIGN : sprintf(dderr, "DDERR_HEIGHTALIGN"); break;
		case DDERR_INCOMPATIBLEPRIMARY : sprintf(dderr, "DDERR_INCOMPATIBLEPRIMARY"); break;
		case DDERR_INVALIDCAPS : sprintf(dderr, "DDERR_INVALIDCAPS"); break;
		case DDERR_INVALIDCLIPLIST : sprintf(dderr, "DDERR_INVALIDCLIPLIST"); break;
		case DDERR_INVALIDMODE : sprintf(dderr, "DDERR_INVALIDMODE"); break;
		case DDERR_INVALIDOBJECT : sprintf(dderr, "DDERR_INVALIDOBJECT"); break;
		case DDERR_INVALIDPARAMS : sprintf(dderr, "DDERR_INVALIDPARAMS"); break;
		case DDERR_INVALIDPIXELFORMAT : sprintf(dderr, "DDERR_INVALIDPIXELFORMAT"); break;
		case DDERR_INVALIDRECT : sprintf(dderr, "DDERR_INVALIDRECT"); break;
		case DDERR_LOCKEDSURFACES : sprintf(dderr, "DDERR_LOCKEDSURFACES"); break;
		case DDERR_NO3D : sprintf(dderr, "DDERR_NO3D"); break;
		case DDERR_NOALPHAHW : sprintf(dderr, "DDERR_NOALPHAHW"); break;
		case DDERR_NOCLIPLIST : sprintf(dderr, "DDERR_NOCLIPLIST"); break;
		case DDERR_NOCOLORCONVHW : sprintf(dderr, "DDERR_NOCOLORCONVHW"); break;
		case DDERR_NOCOOPERATIVELEVELSET : sprintf(dderr, "DDERR_NOCOOPERATIVELEVELSET"); break;
		case DDERR_NOCOLORKEY : sprintf(dderr, "DDERR_NOCOLORKEY"); break;
		case DDERR_NOCOLORKEYHW : sprintf(dderr, "DDERR_NOCOLORKEYHW"); break;
		case DDERR_NODIRECTDRAWSUPPORT : sprintf(dderr, "DDERR_NODIRECTDRAWSUPPORT"); break;
		case DDERR_NOEXCLUSIVEMODE : sprintf(dderr, "DDERR_NOEXCLUSIVEMODE"); break;
		case DDERR_NOFLIPHW : sprintf(dderr, "DDERR_NOFLIPHW"); break;
		case DDERR_NOGDI : sprintf(dderr, "DDERR_NOGDI"); break;
		case DDERR_NOMIRRORHW : sprintf(dderr, "DDERR_NOMIRRORHW"); break;
		case DDERR_NOTFOUND : sprintf(dderr, "DDERR_NOTFOUND"); break;
		case DDERR_NOOVERLAYHW : sprintf(dderr, "DDERR_NOOVERLAYHW"); break;
		case DDERR_NORASTEROPHW : sprintf(dderr, "DDERR_NORASTEROPHW"); break;
		case DDERR_NOROTATIONHW : sprintf(dderr, "DDERR_NOROTATIONHW"); break;
		case DDERR_NOSTRETCHHW : sprintf(dderr, "DDERR_NOSTRETCHHW"); break;
		case DDERR_NOT4BITCOLOR : sprintf(dderr, "DDERR_NOT4BITCOLOR"); break;
		case DDERR_NOT4BITCOLORINDEX : sprintf(dderr, "DDERR_NOT4BITCOLORINDEX"); break;
		case DDERR_NOT8BITCOLOR : sprintf(dderr, "DDERR_NOT8BITCOLOR"); break;
		case DDERR_NOTEXTUREHW : sprintf(dderr, "DDERR_NOTEXTUREHW"); break;
		case DDERR_NOVSYNCHW : sprintf(dderr, "DDERR_NOVSYNCHW"); break;
		case DDERR_NOZBUFFERHW : sprintf(dderr, "DDERR_NOZBUFFERHW"); break;
		case DDERR_NOZOVERLAYHW : sprintf(dderr, "DDERR_NOZOVERLAYHW"); break;
		case DDERR_OUTOFCAPS : sprintf(dderr, "DDERR_OUTOFCAPS"); break;
		case DDERR_OUTOFMEMORY : sprintf(dderr, "DDERR_OUTOFMEMORY"); break;
		case DDERR_OUTOFVIDEOMEMORY : sprintf(dderr, "DDERR_OUTOFVIDEOMEMORY"); break;
		case DDERR_OVERLAYCANTCLIP : sprintf(dderr, "DDERR_OVERLAYCANTCLIP"); break;
		case DDERR_OVERLAYCOLORKEYONLYONEACTIVE : sprintf(dderr, "DDERR_OVERLAYCOLORKEYONLYONEACTIVE"); break;
		case DDERR_PALETTEBUSY : sprintf(dderr, "DDERR_PALETTEBUSY"); break;
		case DDERR_COLORKEYNOTSET : sprintf(dderr, "DDERR_COLORKEYNOTSET"); break;
		case DDERR_SURFACEALREADYATTACHED : sprintf(dderr, "DDERR_SURFACEALREADYATTACHED"); break;
		case DDERR_SURFACEALREADYDEPENDENT : sprintf(dderr, "DDERR_SURFACEALREADYDEPENDENT"); break;
		case DDERR_SURFACEBUSY : sprintf(dderr, "DDERR_SURFACEBUSY"); break;
		case DDERR_CANTLOCKSURFACE : sprintf(dderr, "DDERR_CANTLOCKSURFACE"); break;
		case DDERR_SURFACEISOBSCURED : sprintf(dderr, "DDERR_SURFACEISOBSCURED"); break;
		case DDERR_SURFACELOST : sprintf(dderr, "DDERR_SURFACELOST"); break;
		case DDERR_SURFACENOTATTACHED : sprintf(dderr, "DDERR_SURFACENOTATTACHED"); break;
		case DDERR_TOOBIGHEIGHT : sprintf(dderr, "DDERR_TOOBIGHEIGHT"); break;
		case DDERR_TOOBIGSIZE : sprintf(dderr, "DDERR_TOOBIGSIZE"); break;
		case DDERR_TOOBIGWIDTH : sprintf(dderr, "DDERR_TOOBIGWIDTH"); break;
		case DDERR_UNSUPPORTED : sprintf(dderr, "DDERR_UNSUPPORTED"); break;
		case DDERR_UNSUPPORTEDFORMAT : sprintf(dderr, "DDERR_UNSUPPORTEDFORMAT"); break;
		case DDERR_UNSUPPORTEDMASK : sprintf(dderr, "DDERR_UNSUPPORTEDMASK"); break;
		case DDERR_VERTICALBLANKINPROGRESS : sprintf(dderr, "DDERR_VERTICALBLANKINPROGRESS"); break;
		case DDERR_WASSTILLDRAWING : sprintf(dderr, "DDERR_WASSTILLDRAWING"); break;
		case DDERR_XALIGN : sprintf(dderr, "DDERR_XALIGN"); break;
		case DDERR_INVALIDDIRECTDRAWGUID : sprintf(dderr, "DDERR_INVALIDDIRECTDRAWGUID"); break;
		case DDERR_DIRECTDRAWALREADYCREATED : sprintf(dderr, "DDERR_DIRECTDRAWALREADYCREATED"); break;
		case DDERR_NODIRECTDRAWHW : sprintf(dderr, "DDERR_NODIRECTDRAWHW"); break;
		case DDERR_PRIMARYSURFACEALREADYEXISTS : sprintf(dderr, "DDERR_PRIMARYSURFACEALREADYEXISTS"); break;
		case DDERR_NOEMULATION : sprintf(dderr, "DDERR_NOEMULATION"); break;
		case DDERR_REGIONTOOSMALL : sprintf(dderr, "DDERR_REGIONTOOSMALL"); break;
		case DDERR_CLIPPERISUSINGHWND : sprintf(dderr, "DDERR_CLIPPERISUSINGHWND"); break;
		case DDERR_NOCLIPPERATTACHED : sprintf(dderr, "DDERR_NOCLIPPERATTACHED"); break;
		case DDERR_NOHWND : sprintf(dderr, "DDERR_NOHWND"); break;
		case DDERR_HWNDSUBCLASSED : sprintf(dderr, "DDERR_HWNDSUBCLASSED"); break;
		case DDERR_HWNDALREADYSET : sprintf(dderr, "DDERR_HWNDALREADYSET"); break;
		case DDERR_NOPALETTEATTACHED : sprintf(dderr, "DDERR_NOPALETTEATTACHED"); break;
		case DDERR_NOPALETTEHW : sprintf(dderr, "DDERR_NOPALETTEHW"); break;
		case DDERR_BLTFASTCANTCLIP : sprintf(dderr, "DDERR_BLTFASTCANTCLIP"); break;
		case DDERR_NOBLTHW : sprintf(dderr, "DDERR_NOBLTHW"); break;
		case DDERR_NODDROPSHW : sprintf(dderr, "DDERR_NODDROPSHW"); break;
		case DDERR_OVERLAYNOTVISIBLE : sprintf(dderr, "DDERR_OVERLAYNOTVISIBLE"); break;
		case DDERR_NOOVERLAYDEST : sprintf(dderr, "DDERR_NOOVERLAYDEST"); break;
		case DDERR_INVALIDPOSITION : sprintf(dderr, "DDERR_INVALIDPOSITION"); break;
		case DDERR_NOTAOVERLAYSURFACE : sprintf(dderr, "DDERR_NOTAOVERLAYSURFACE"); break;
		case DDERR_EXCLUSIVEMODEALREADYSET : sprintf(dderr, "DDERR_EXCLUSIVEMODEALREADYSET"); break;
		case DDERR_NOTFLIPPABLE : sprintf(dderr, "DDERR_NOTFLIPPABLE"); break;
		case DDERR_CANTDUPLICATE : sprintf(dderr, "DDERR_CANTDUPLICATE"); break;
		case DDERR_NOTLOCKED : sprintf(dderr, "DDERR_NOTLOCKED"); break;
		case DDERR_CANTCREATEDC : sprintf(dderr, "DDERR_CANTCREATEDC"); break;
		case DDERR_NODC : sprintf(dderr, "DDERR_NODC"); break;
		case DDERR_WRONGMODE : sprintf(dderr, "DDERR_WRONGMODE"); break;
		case DDERR_IMPLICITLYCREATED : sprintf(dderr, "DDERR_IMPLICITLYCREATED"); break;
		case DDERR_NOTPALETTIZED : sprintf(dderr, "DDERR_NOTPALETTIZED"); break;
		case DDERR_UNSUPPORTEDMODE : sprintf(dderr, "DDERR_UNSUPPORTEDMODE"); break;
		case DDERR_NOMIPMAPHW : sprintf(dderr, "DDERR_NOMIPMAPHW"); break;
		case DDERR_INVALIDSURFACETYPE : sprintf(dderr, "DDERR_INVALIDSURFACETYPE"); break;
		case DDERR_DCALREADYCREATED : sprintf(dderr, "DDERR_DCALREADYCREATED"); break;
		case DDERR_CANTPAGELOCK : sprintf(dderr, "DDERR_CANTPAGELOCK"); break;
		case DDERR_CANTPAGEUNLOCK : sprintf(dderr, "DDERR_CANTPAGEUNLOCK"); break;
		case DDERR_NOTPAGELOCKED : sprintf(dderr, "DDERR_NOTPAGELOCKED"); break;
		case DDERR_NOTINITIALIZED : sprintf(dderr, "DDERR_NOTINITIALIZED"); break;

		default : sprintf(dderr, "Unknown Error"); break;
	}

	sprintf(err, "DirectDraw Error %s\nFILE: %s\nLINE: %d", dderr, szFile, iLine);
	MessageBox((HWND)hWnd, err, "Error", MB_OK);
	PostQuitMessage(0);
}

////////////////////////////////////////////////////////////////////////
// Traces an error (DirectSound).
////////////////////////////////////////////////////////////////////////
void DSError(HRESULT hErr, void* hWnd)
{
	char dserr[256];
	char err[1024];

	switch (hErr)
	{
		case DSERR_ALLOCATED : sprintf(dserr, "DSERR_ALLOCATED"); break;
		case DSERR_CONTROLUNAVAIL : sprintf(dserr, "DSERR_CONTROLUNAVAIL"); break;
		case DSERR_INVALIDPARAM : sprintf(dserr, "DSERR_INVALIDPARAM"); break;
		case DSERR_INVALIDCALL : sprintf(dserr, "DSERR_INVALIDCALL"); break;
		case DSERR_GENERIC : sprintf(dserr, "DSERR_GENERIC"); break;
		case DSERR_PRIOLEVELNEEDED : sprintf(dserr, "DSERR_PRIOLEVELNEEDED"); break;
		case DSERR_OUTOFMEMORY : sprintf(dserr, "DSERR_OUTOFMEMORY"); break;
		case DSERR_BADFORMAT : sprintf(dserr, "DSERR_BADFORMAT"); break;
		case DSERR_UNSUPPORTED : sprintf(dserr, "DSERR_UNSUPPORTED"); break;
		case DSERR_NODRIVER : sprintf(dserr, "DSERR_NODRIVER"); break;
		case DSERR_ALREADYINITIALIZED : sprintf(dserr, "DSERR_ALREADYINITIALIZED"); break;
		case DSERR_NOAGGREGATION : sprintf(dserr, "DSERR_NOAGGREGATION"); break;
		case DSERR_BUFFERLOST : sprintf(dserr, "DSERR_BUFFERLOST"); break;
		case DSERR_OTHERAPPHASPRIO : sprintf(dserr, "DSERR_OTHERAPPHASPRIO"); break;
		case DSERR_UNINITIALIZED : sprintf(dserr, "DSERR_UNINITIALIZED"); break;

		default : sprintf(dserr, "Unknown Error"); break;
	}

	sprintf(err, "DirectSound Error '%s.'\nContinue with Sound turned off?", dserr);
	int ret = MessageBox( (HWND)hWnd, err, "Error", MB_YESNO );
	if( ret==IDNO ) PostQuitMessage(0);
}


////////////////////////////////////////////////////////////////////////
// DXErrorToString
// Returns a pointer to a string describing the given DD, D3D or D3DRM error code.
////////////////////////////////////////////////////////////////////////
char* DXErrorToString(HRESULT hErr)
{
  switch(hErr) 
  {
    case DD_OK:
  /* Also includes D3D_OK and D3DRM_OK */
        return "No error.\0";
    case DDERR_ALREADYINITIALIZED:
        return "This object is already initialized.\0";
    case DDERR_BLTFASTCANTCLIP:
        return "Return if a clipper object is attached to the source surface passed into a BltFast call.\0";
    case DDERR_CANNOTATTACHSURFACE:
        return "This surface can not be attached to the requested surface.\0";
    case DDERR_CANNOTDETACHSURFACE:
        return "This surface can not be detached from the requested surface.\0";
    case DDERR_CANTCREATEDC:
        return "Windows can not create any more DCs.\0";
    case DDERR_CANTDUPLICATE:
        return "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.\0";
    case DDERR_CLIPPERISUSINGHWND:
        return "An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd.\0";
    case DDERR_COLORKEYNOTSET:
        return "No src color key specified for this operation.\0";
    case DDERR_CURRENTLYNOTAVAIL:
        return "Support is currently not available.\0";
    case DDERR_DIRECTDRAWALREADYCREATED:
        return "A DirectDraw object representing this driver has already been created for this process.\0";
    case DDERR_EXCEPTION:
        return "An exception was encountered while performing the requested operation.\0";
    case DDERR_EXCLUSIVEMODEALREADYSET:
        return "An attempt was made to set the cooperative level when it was already set to exclusive.\0";
    case DDERR_GENERIC:
        return "Generic failure.\0";
    case DDERR_HEIGHTALIGN:
        return "Height of rectangle provided is not a multiple of reqd alignment.\0";
    case DDERR_HWNDALREADYSET:
        return "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.\0";
    case DDERR_HWNDSUBCLASSED:
        return "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.\0";
    case DDERR_IMPLICITLYCREATED:
        return "This surface can not be restored because it is an implicitly created surface.\0";
    case DDERR_INCOMPATIBLEPRIMARY:
        return "Unable to match primary surface creation request with existing primary surface.\0";
    case DDERR_INVALIDCAPS:
        return "One or more of the caps bits passed to the callback are incorrect.\0";
    case DDERR_INVALIDCLIPLIST:
        return "DirectDraw does not support the provided cliplist.\0";
    case DDERR_INVALIDDIRECTDRAWGUID:
        return "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.\0";
    case DDERR_INVALIDMODE:
        return "DirectDraw does not support the requested mode.\0";
    case DDERR_INVALIDOBJECT:
        return "DirectDraw received a pointer that was an invalid DIRECTDRAW object.\0";
    case DDERR_INVALIDPARAMS:
        return "One or more of the parameters passed to the function are incorrect.\0";
    case DDERR_INVALIDPIXELFORMAT:
        return "The pixel format was invalid as specified.\0";
    case DDERR_INVALIDPOSITION:
        return "Returned when the position of the overlay on the destination is no longer legal for that destination.\0";
    case DDERR_INVALIDRECT:
        return "Rectangle provided was invalid.\0";
    case DDERR_LOCKEDSURFACES:
        return "Operation could not be carried out because one or more surfaces are locked.\0";
    case DDERR_NO3D:
        return "There is no 3D present.\0";
    case DDERR_NOALPHAHW:
        return "Operation could not be carried out because there is no alpha accleration hardware present or available.\0";
    case DDERR_NOBLTHW:
        return "No blitter hardware present.\0";
    case DDERR_NOCLIPLIST:
        return "No cliplist available.\0";
    case DDERR_NOCLIPPERATTACHED:
        return "No clipper object attached to surface object.\0";
    case DDERR_NOCOLORCONVHW:
        return "Operation could not be carried out because there is no color conversion hardware present or available.\0";
    case DDERR_NOCOLORKEY:
        return "Surface doesn't currently have a color key\0";
    case DDERR_NOCOLORKEYHW:
        return "Operation could not be carried out because there is no hardware support of the destination color key.\0";
    case DDERR_NOCOOPERATIVELEVELSET:
        return "Create function called without DirectDraw object method SetCooperativeLevel being called.\0";
    case DDERR_NODC:
        return "No DC was ever created for this surface.\0";
    case DDERR_NODDROPSHW:
        return "No DirectDraw ROP hardware.\0";
    case DDERR_NODIRECTDRAWHW:
        return "A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.\0";
    case DDERR_NOEMULATION:
        return "Software emulation not available.\0";
    case DDERR_NOEXCLUSIVEMODE:
        return "Operation requires the application to have exclusive mode but the application does not have exclusive mode.\0";
    case DDERR_NOFLIPHW:
        return "Flipping visible surfaces is not supported.\0";
    case DDERR_NOGDI:
        return "There is no GDI present.\0";
    case DDERR_NOHWND:
        return "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.\0";
    case DDERR_NOMIRRORHW:
        return "Operation could not be carried out because there is no hardware present or available.\0";
    case DDERR_NOOVERLAYDEST:
        return "Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination.\0";
    case DDERR_NOOVERLAYHW:
        return "Operation could not be carried out because there is no overlay hardware present or available.\0";
    case DDERR_NOPALETTEATTACHED:
        return "No palette object attached to this surface.\0";
    case DDERR_NOPALETTEHW:
        return "No hardware support for 16 or 256 color palettes.\0";
    case DDERR_NORASTEROPHW:
        return "Operation could not be carried out because there is no appropriate raster op hardware present or available.\0";
    case DDERR_NOROTATIONHW:
        return "Operation could not be carried out because there is no rotation hardware present or available.\0";
    case DDERR_NOSTRETCHHW:
        return "Operation could not be carried out because there is no hardware support for stretching.\0";
    case DDERR_NOT4BITCOLOR:
        return "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.\0";
    case DDERR_NOT4BITCOLORINDEX:
        return "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.\0";
    case DDERR_NOT8BITCOLOR:
        return "DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color.\0";
    case DDERR_NOTAOVERLAYSURFACE:
        return "Returned when an overlay member is called for a non-overlay surface.\0";
    case DDERR_NOTEXTUREHW:
        return "Operation could not be carried out because there is no texture mapping hardware present or available.\0";
    case DDERR_NOTFLIPPABLE:
        return "An attempt has been made to flip a surface that is not flippable.\0";
    case DDERR_NOTFOUND:
        return "Requested item was not found.\0";
    case DDERR_NOTLOCKED:
        return "Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.\0";
    case DDERR_NOTPALETTIZED:
        return "The surface being used is not a palette-based surface.\0";
    case DDERR_NOVSYNCHW:
        return "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.\0";
    case DDERR_NOZBUFFERHW:
        return "Operation could not be carried out because there is no hardware support for zbuffer blitting.\0";
    case DDERR_NOZOVERLAYHW:
        return "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.\0";
    case DDERR_OUTOFCAPS:
        return "The hardware needed for the requested operation has already been allocated.\0";
    case DDERR_OUTOFMEMORY:
        return "DirectDraw does not have enough memory to perform the operation.\0";
    case DDERR_OUTOFVIDEOMEMORY:
        return "DirectDraw does not have enough memory to perform the operation.\0";
    case DDERR_OVERLAYCANTCLIP:
        return "The hardware does not support clipped overlays.\0";
    case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
        return "Can only have ony color key active at one time for overlays.\0";
    case DDERR_OVERLAYNOTVISIBLE:
        return "Returned when GetOverlayPosition is called on a hidden overlay.\0";
    case DDERR_PALETTEBUSY:
        return "Access to this palette is being refused because the palette is already locked by another thread.\0";
    case DDERR_PRIMARYSURFACEALREADYEXISTS:
        return "This process already has created a primary surface.\0";
    case DDERR_REGIONTOOSMALL:
        return "Region passed to Clipper::GetClipList is too small.\0";
    case DDERR_SURFACEALREADYATTACHED:
        return "This surface is already attached to the surface it is being attached to.\0";
    case DDERR_SURFACEALREADYDEPENDENT:
        return "This surface is already a dependency of the surface it is being made a dependency of.\0";
    case DDERR_SURFACEBUSY:
        return "Access to this surface is being refused because the surface is already locked by another thread.\0";
    case DDERR_SURFACEISOBSCURED:
        return "Access to surface refused because the surface is obscured.\0";
    case DDERR_SURFACELOST:
        return "Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have Restore called on it.\0";
    case DDERR_SURFACENOTATTACHED:
        return "The requested surface is not attached.\0";
    case DDERR_TOOBIGHEIGHT:
        return "Height requested by DirectDraw is too large.\0";
    case DDERR_TOOBIGSIZE:
        return "Size requested by DirectDraw is too large, but the individual height and width are OK.\0";
    case DDERR_TOOBIGWIDTH:
        return "Width requested by DirectDraw is too large.\0";
    case DDERR_UNSUPPORTED:
        return "Action not supported.\0";
    case DDERR_UNSUPPORTEDFORMAT:
        return "FOURCC format requested is unsupported by DirectDraw.\0";
    case DDERR_UNSUPPORTEDMASK:
        return "Bitmask in the pixel format requested is unsupported by DirectDraw.\0";
    case DDERR_VERTICALBLANKINPROGRESS:
        return "Vertical blank is in progress.\0";
    case DDERR_WASSTILLDRAWING:
        return "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.\0";
    case DDERR_WRONGMODE:
        return "This surface can not be restored because it was created in a different mode.\0";
    case DDERR_XALIGN:		
        return "Rectangle provided was not horizontally aligned on required boundary.\0";
		/*
    case D3DERR_BADMAJORVERSION:
        return "D3DERR_BADMAJORVERSION\0";
    case D3DERR_BADMINORVERSION:
        return "D3DERR_BADMINORVERSION\0";
    case D3DERR_EXECUTE_LOCKED:
        return "D3DERR_EXECUTE_LOCKED\0";
    case D3DERR_EXECUTE_NOT_LOCKED:
        return "D3DERR_EXECUTE_NOT_LOCKED\0";
    case D3DERR_EXECUTE_CREATE_FAILED:
        return "D3DERR_EXECUTE_CREATE_FAILED\0";
    case D3DERR_EXECUTE_DESTROY_FAILED:
        return "D3DERR_EXECUTE_DESTROY_FAILED\0";
    case D3DERR_EXECUTE_LOCK_FAILED:
        return "D3DERR_EXECUTE_LOCK_FAILED\0";
    case D3DERR_EXECUTE_UNLOCK_FAILED:
        return "D3DERR_EXECUTE_UNLOCK_FAILED\0";
    case D3DERR_EXECUTE_FAILED:
        return "D3DERR_EXECUTE_FAILED\0";
    case D3DERR_EXECUTE_CLIPPED_FAILED:
        return "D3DERR_EXECUTE_CLIPPED_FAILED\0";
    case D3DERR_TEXTURE_NO_SUPPORT:
        return "D3DERR_TEXTURE_NO_SUPPORT\0";
    case D3DERR_TEXTURE_NOT_LOCKED:
        return "D3DERR_TEXTURE_NOT_LOCKED\0";
    case D3DERR_TEXTURE_LOCKED:
        return "D3DERR_TEXTURELOCKED\0";
    case D3DERR_TEXTURE_CREATE_FAILED:
        return "D3DERR_TEXTURE_CREATE_FAILED\0";
    case D3DERR_TEXTURE_DESTROY_FAILED:
        return "D3DERR_TEXTURE_DESTROY_FAILED\0";
    case D3DERR_TEXTURE_LOCK_FAILED:
        return "D3DERR_TEXTURE_LOCK_FAILED\0";
    case D3DERR_TEXTURE_UNLOCK_FAILED:
        return "D3DERR_TEXTURE_UNLOCK_FAILED\0";
    case D3DERR_TEXTURE_LOAD_FAILED:
        return "D3DERR_TEXTURE_LOAD_FAILED\0";
    case D3DERR_MATRIX_CREATE_FAILED:
        return "D3DERR_MATRIX_CREATE_FAILED\0";
    case D3DERR_MATRIX_DESTROY_FAILED:
        return "D3DERR_MATRIX_DESTROY_FAILED\0";
    case D3DERR_MATRIX_SETDATA_FAILED:
        return "D3DERR_MATRIX_SETDATA_FAILED\0";
    case D3DERR_SETVIEWPORTDATA_FAILED:
        return "D3DERR_SETVIEWPORTDATA_FAILED\0";
    case D3DERR_MATERIAL_CREATE_FAILED:
        return "D3DERR_MATERIAL_CREATE_FAILED\0";
    case D3DERR_MATERIAL_DESTROY_FAILED:
        return "D3DERR_MATERIAL_DESTROY_FAILED\0";
    case D3DERR_MATERIAL_SETDATA_FAILED:
        return "D3DERR_MATERIAL_SETDATA_FAILED\0";
    case D3DERR_LIGHT_SET_FAILED:
        return "D3DERR_LIGHT_SET_FAILED\0";
	  case D3DRMERR_BADOBJECT:
	      return "D3DRMERR_BADOBJECT\0";
	  case D3DRMERR_BADTYPE:
	      return "D3DRMERR_BADTYPE\0";
	  case D3DRMERR_BADALLOC:
	      return "D3DRMERR_BADALLOC\0";
	  case D3DRMERR_FACEUSED:
	      return "D3DRMERR_FACEUSED\0";
	  case D3DRMERR_NOTFOUND:
	      return "D3DRMERR_NOTFOUND\0";
	  case D3DRMERR_NOTDONEYET:
	      return "D3DRMERR_NOTDONEYET\0";
	  case D3DRMERR_FILENOTFOUND:
	      return "The file was not found.\0";
	  case D3DRMERR_BADFILE:
	      return "D3DRMERR_BADFILE\0";
	  case D3DRMERR_BADDEVICE:
	      return "D3DRMERR_BADDEVICE\0";
	  case D3DRMERR_BADVALUE:
	      return "D3DRMERR_BADVALUE\0";
	  case D3DRMERR_BADMAJORVERSION:
	      return "D3DRMERR_BADMAJORVERSION\0";
	  case D3DRMERR_BADMINORVERSION:
	      return "D3DRMERR_BADMINORVERSION\0";
	  case D3DRMERR_UNABLETOEXECUTE:
	      return "D3DRMERR_UNABLETOEXECUTE\0";
		  */
    default:
        return "Unrecognized error value.\0";
  }
}
