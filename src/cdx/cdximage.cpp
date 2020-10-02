//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximage.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdximage.cpp,v $
// Revision 1.7  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.6  2001/01/23 19:17:09  istan
// Added support for JPEG files via OLE
// Thanks to Richard Turner for the code
//
// Revision 1.5  2000/12/24 00:07:55  mindcry
// Fixed a bug that caused creation of empty bmp files when they didn't
// exist or when they where read only.
//
// Revision 1.4  2000/08/21 21:18:57  mindcry
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
// Revision 1.1.1.1  2000/04/22 16:09:20  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 1.3  2000/02/14 16:32:22  jhebert
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
// Revision 1.2  2000/01/21 14:06:04  jhebert
// CDXImage now loads Targa images correctly.
//
// Revision 1.1  2000/01/19 23:08:16  jhebert
// Adding in the CDXImage files.
//
// 
//
// $Revision: 1.7 $
//////////////////////////////////////////////////////////////////////////////////


#include "CDXImage.h"
#include "CDXImageTGA.h"
#include "CDXImagePSD.h"
#include "CDXImageBMP.h"
#include "CDXImageJPG.h"
#include "CDXImagePCX.h"
#include "CDXImagePNG.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDXImage::CDXImage(void){}

CDXImage::~CDXImage(void){}

//////////////////////////////////////////////////////////////
// Description   :  Check which file type specified and call
//					appropriate loader.  return new surface.
//////////////////////////////////////////////////////////////
CDX_LPDIRECTDRAWSURFACE CDXImage::GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType, DWORD type )
{
	CDX_LPDIRECTDRAWSURFACE lpdds = NULL;

	// Call GetImage for the appropriate image type
    lpdds = CreateDDSurface(lpDD, lSize, lpCache, memType, type);

	return lpdds;
}


//////////////////////////////////////////////////////////////
// Description   :  Check which file type specified and call
//					appropriate loader.  return new surface.
//////////////////////////////////////////////////////////////
CDX_LPDIRECTDRAWSURFACE CDXImage::GetImage(CDX_LPDIRECTDRAW lpDD, const char *filename, BYTE memType, DWORD type )
{
	CDX_LPDIRECTDRAWSURFACE lpdds = NULL;
    LONG lSize;
	CDXFile fs;
    CHAR* lpCache;

  strncpy(m_filename, filename, _MAX_PATH);
  m_filename[_MAX_PATH]='\0';

	if( FAILED(fs.OpenRead(filename)) )
		return NULL;

	// Get the file size.
	lSize=fs.FileSize();

    // Allocate memory to hold the data
    lpCache = new CHAR[lSize];
    if(lpCache == NULL)
    {
		fs.Close();
        return NULL;
    }

	if( (DWORD)lSize != fs.Read(lpCache, lSize ) )
    {
        delete [] lpCache;
		fs.Close();
        return NULL;
    }

    // Close the file stream
	fs.Close();

    // Call GetImage for the appropriate image type
    lpdds = CreateDDSurface(lpDD, lSize, lpCache, memType, type);

    // Delete the cache
    delete [] lpCache;

	return lpdds;
}

//////////////////////////////////////////////////////////////
// Description   :  Check which file type specified and call
//					appropriate loader.  return new surface.
//////////////////////////////////////////////////////////////
CDX_LPDIRECTDRAWSURFACE CDXImage::GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CDXFile* fsptr, BYTE memType, DWORD type )
{
	CDX_LPDIRECTDRAWSURFACE lpdds = NULL;
    CHAR* lpCache;
    int length, save;

    length = lSize;

    // If lSize equals zero get the size of the file.
    if(length == 0)
    {
        // Save the pointer location
        save = fsptr->Position();

        // Get the size of the file
        length = fsptr->FileSize();

        // Seek back to save position
        fsptr->Position(save, FILE_BEGIN );
    }

    // Cache the whole file in memory
    // Allocate memory to hold the data
    lpCache = new CHAR[length];
    if(lpCache == NULL)
        return NULL;

    // Read in the data
    if( fsptr->Read(lpCache, length) == 0 )
    {
        delete [] lpCache;
        return NULL;
    }

    // Call GetImage for the appropriate image type
    lpdds = CreateDDSurface(lpDD, length, lpCache, memType, type);

    // Delete the cache
    delete [] lpCache;

	return lpdds;
}

//////////////////////////////////////////////////////////////
// Description   :  Check which file type specified and call
//					appropriate loader.  return new surface.
//////////////////////////////////////////////////////////////
CDX_LPDIRECTDRAWSURFACE CDXImage::GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, FILE* fh, BYTE memType, DWORD type )
{
	CDX_LPDIRECTDRAWSURFACE lpdds = NULL;
    CHAR* lpCache;
    int length, save;

    length = lSize;

    // If lSize equals zero get the size of the file.
    if(length == 0)
    {
        // Save the pointer location
        save = ftell(fh);
        if(ferror(fh))
            return NULL;

        // Seek to end of file
        fseek(fh, 0, SEEK_END);
        if(ferror(fh))
            return NULL;

        // Get the size of the file
        length = ftell(fh);
        if(ferror(fh))
            return NULL;

        // Seek back to save position
        fseek(fh, save, SEEK_SET);
        if(ferror(fh))
            return NULL;

    }

    // Cache the whole file in memory
    // Allocate memory to hold the data
    lpCache = new CHAR[length];
    if(lpCache == NULL)
        return NULL;

    // Read in the data
    fread(lpCache, 1, length, fh);
    if(ferror(fh))
    {
        delete [] lpCache;
        return NULL;
    }

    // Call GetImage for the appropriate image type
    lpdds = CreateDDSurface(lpDD, length, lpCache, memType, type);

    // Delete the cache
    delete [] lpCache;

	return lpdds;
}

//////////////////////////////////////////////////////////////
// Description   :  Read in header buffer and footer buffer,
//					One by one call a file format Validate which
//					should check the header and footer buffers
//					for ids or signatures to verify file type.
//////////////////////////////////////////////////////////////
DWORD CDXImage::GetFileFormat(LONG lSize, CHAR* lpCache)
{
	DWORD type = CDXIMAGE_UNKNOWN;
    CHAR* pHead = NULL;
    CHAR* pFoot = NULL;

    pHead = lpCache;
    pFoot = &lpCache[lSize - footsize];

	if(type == CDXIMAGE_UNKNOWN)
	{
		CDXImageBMP bmp;
		if(bmp.Validate(pHead, headsize, pFoot, footsize))
		{
			type = CDXIMAGE_BMP;
		}
	}

	if(type == CDXIMAGE_UNKNOWN)
	{
		CDXImagePCX pcx;
		if(pcx.Validate(pHead, headsize, pFoot, footsize))
		{
			type = CDXIMAGE_PCX;
		}
	}

  if(type == CDXIMAGE_UNKNOWN)
	{    
		CDXImagePNG png;
		if(png.Validate(pHead, headsize, pFoot, footsize))
		{
      if (strlen(m_filename) > 0)
			  type = CDXIMAGE_PNG;
		}
	}

  /*
	if(type == CDXIMAGE_UNKNOWN)
	{
		CDXImagePSD psd;
		if(psd.Validate(pHead, headsize, pFoot, footsize))
		{
			type = CDXIMAGE_PSD;
		}
	}
  */

  if(type == CDXIMAGE_UNKNOWN)
	{
		CDXImageJPG jpg;
		if(jpg.Validate(pHead, headsize, pFoot, footsize))
		{
			type = CDXIMAGE_JPG;
		}
	}

  if(type == CDXIMAGE_UNKNOWN)
	{
	  CDXImageTGA tga;
	  if(tga.Validate(pHead, headsize, pFoot, footsize))
	  {
		  type = CDXIMAGE_TGA;
	  }
  }

	return type;
}

//////////////////////////////////////////////////////////////
// Description   :  Check which file type specified and call
//					appropriate loader.  return new surface.
//////////////////////////////////////////////////////////////
CDX_LPDIRECTDRAWSURFACE CDXImage::CreateDDSurface(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType, DWORD type )
{
	CDX_LPDIRECTDRAWSURFACE lpdds = NULL;

    // Determine the type if UNKNOWN
    if(type == CDXIMAGE_UNKNOWN)
		type = GetFileFormat(lSize, lpCache);

	switch(type)
	{
	case CDXIMAGE_UNKNOWN:
		OutputDebugString("Error: CDXImage - Unsupported Image type.");
		break;

	case CDXIMAGE_BMP:
		{
			CDXImageBMP gf;
			lpdds = gf.GetImage(lpDD, lSize, lpCache, memType);
		}
		break;

	case CDXIMAGE_TGA:
		{
			CDXImageTGA gf;
			lpdds = gf.GetImage(lpDD, lSize, lpCache, memType);
		}
		break;
	
	case CDXIMAGE_PSD:
		{
			CDXImagePSD gf;
			lpdds = gf.GetImage(lpDD, lSize, lpCache, memType);
		}
		break;

	case CDXIMAGE_PCX:
		{
			CDXImagePCX gf;
			lpdds = gf.GetImage(lpDD, lSize, lpCache, memType);
		}
		break;

    case CDXIMAGE_JPG:
		{
			CDXImageJPG gf;
			lpdds = gf.GetImage(lpDD, lSize, lpCache, memType);
		}
		break;

  case CDXIMAGE_PNG:
		{
			CDXImagePNG gf;
			lpdds = gf.GetImage(lpDD, lSize, m_filename, memType);
		}
		break;
	}

	return lpdds;
}
