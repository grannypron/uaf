//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagepcx.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdximagepcx.cpp,v $
// Revision 1.5  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.4  2000/02/14 16:32:22  jhebert
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
// Revision 1.3  2000/01/28 16:53:58  MICHAELR
// Added CVS header to top of file
//
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////

#include "cdxdebug.h"
#include "CDXImagePCX.h"

#define PCX_MANUF_ZSOFT			0x0a
#define PCX_PC_PAINTBRUSH_VER	5
#define PCX_ENCODING_RGB		1


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDXImagePCX::CDXImagePCX(void)
{
	m_iColoursUsed = 0;
}

CDXImagePCX::~CDXImagePCX(void) { }


BOOL CDXImagePCX::Validate(const CHAR *head, const int headsize, const CHAR *foot, const int footsize)
{
	LPPCX_HEADER pcxh = (LPPCX_HEADER)head;

	if (   (pcxh->Manufacturer == PCX_MANUF_ZSOFT) 
      && (pcxh->Version == PCX_PC_PAINTBRUSH_VER)
      && (pcxh->Encoding == PCX_ENCODING_RGB))
		return TRUE;
	
	return FALSE;
}

int CDXImagePCX::DecodePcxLine(char *dest, char* &src, int bytes)
{
  int n=0;
  int i;
  char c;
  
  do 
  {
    c = *src & 0xFF;
    src++;

    if ((c & 0xC0) == 0xC0)
    {
      i = c & 0x3F;
      c = *src;
      src++;
      while (i--)
        dest[n++] = c;
    }
    else
      dest[n++] = c;
  }
  while (n < bytes);

  return n;
}

CDX_LPDIRECTDRAWSURFACE CDXImagePCX::GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType )
{
	int width, height, bpp;
	int colors = 0;
    char* lpTmp = lpCache;  

    memcpy(&m_hdr, lpTmp, sizeof(PCX_HEADER) );
    lpTmp += sizeof(PCX_HEADER);

    // if we have an 8-bit file, keep the color count, otherwise
	// a "-1" means we run paletteless
	if( (m_hdr.BitsPerPixel == 8) && (m_hdr.NPlanes == 1) )
		colors = 256;

	// true BPP = BPP * Num of Planes (i.e. 8 x 1 = 8bit, 8 x 3 = 24bit)
	bpp = m_hdr.BitsPerPixel * m_hdr.NPlanes;

	// calc the pixel size of the image
	width  = (m_hdr.Xmax - m_hdr.Xmin) + 1;
	height = (m_hdr.Ymax - m_hdr.Ymin) + 1;
  
    int ScanLineWidth = m_hdr.NPlanes * m_hdr.BytesPerLine;
    char *pScanLine = new char[ScanLineWidth+1];

    if (pScanLine == NULL)
        return NULL;

	if(!AllocateStorage(width, height, bpp))
		return NULL;

	char *buf = (char*)GetStoragePtr();

	for (int h=0; h < height; h++)
	{
        // decode each plane of RGB data, each having m_hdr.BytesPerLine
        for (int p=0;p<m_hdr.NPlanes;p++) 
        {
            if (DecodePcxLine(pScanLine+(p*m_hdr.BytesPerLine), lpTmp, m_hdr.BytesPerLine) != m_hdr.BytesPerLine)
            {
                delete [] pScanLine;
                return NULL;
            }
        }

        // we need to deal with this on a row by row basis
        char *row = (char*)(&buf[h*width*m_hdr.NPlanes]);

        if (colors > 0)
        {
            for (int w=0; w < width; w++)
                *row++ = pScanLine[w];
        }
        else
        {
            for (int w=0; w < width; w++)
            { 
                *row++ = pScanLine[w + (m_hdr.BytesPerLine*2)];
                *row++ = pScanLine[w + (m_hdr.BytesPerLine*1)];
                *row++ = pScanLine[w + (m_hdr.BytesPerLine*0)];
            }
        }
    }

    if (pScanLine != NULL)
        delete [] pScanLine;

	// create surface, this uses member variables set during AllocateStorage
	if(!CreateSurface(lpDD, memType))
		return NULL;

	// set the image upside down.
	SetInverted(TRUE);

	// setup the BitmapInfo structure required for StretchDIBits
	SetupBitmapInfo();
	m_pBitmapInfo->bmiHeader.biBitCount = bpp;

    // If colors is not equal to "-1" then we have to deal with the palette.
    // Memory for the palette is allocated in the call to SetupBitmapInfo.
	if (colors > 0)
	{
		RGBQUAD *Ptr = &m_pBitmapInfo->bmiColors[0];
		RGBQUAD  buf;
		ZeroMemory(&buf, sizeof(buf));
		
		
		// READ 256 colour PALETTE
        lpTmp = &lpCache[lSize - 769];
        int c = (int)((CHAR)*lpTmp);
        lpTmp++;

		if(c == 12)
		{
			for(c=0; c<colors; c++, Ptr++) 
			{
                memcpy(&buf, lpTmp, 3);
                lpTmp += 3;
				Ptr->rgbRed      = buf.rgbBlue;
				Ptr->rgbGreen    = buf.rgbGreen;
				Ptr->rgbBlue     = buf.rgbRed;
				Ptr->rgbReserved = 0;
			}
		}
	}

	//return pointer to surface
	return CopyBits();
}
