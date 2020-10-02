//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagebmp.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdximagebmp.cpp,v $
// Revision 1.5  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.5  2000/09/10 23:22:47  istan
// Fixed validate to work with BCB.  Used the hex value for 'BM' instead.
//
// Revision 1.4  2000/05/10 23:29:25  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.3  2000/05/07 19:58:48  hebertjo
// Fixed the BMP Loading bug!!!
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.1.1.1  2000/04/22 16:09:31  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 1.6  2000/03/02 16:21:42  jhebert
// Commented out a stray line that was using the file pointer.
//
// Revision 1.5  2000/02/14 16:32:22  jhebert
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
// Revision 1.4  2000/01/28 16:52:35  MICHAELR
// Added CVS header to top of file
//
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////


#include "cdxdebug.h"
#include "CDXImageBMP.h"

static unsigned char	masks[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

#define	MSWCC_EOL	0
#define	MSWCC_EOB	1
#define	MSWCC_DELTA	2

#define BMP_SIGNATURE 0x4d42		// 0x42 = "B" 0x4d = "M" 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDXImageBMP::CDXImageBMP(void)
{
	m_iColoursUsed = 0;
	m_pbi          = NULL;
}

CDXImageBMP::~CDXImageBMP(void) { }


BOOL CDXImageBMP::Validate(const CHAR *head, const int headsize, const CHAR *foot, const int footsize)
{
	BITMAPFILEHEADER *bfh = (BITMAPFILEHEADER*)head;

	if(bfh->bfType == BMP_SIGNATURE)
		return TRUE;
	
	return FALSE;
}

CDX_LPDIRECTDRAWSURFACE CDXImageBMP::GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType )
{
    CHAR* cPtr    = lpCache;
    DWORD padding = 0; 
    DWORD offset  = 0; 
    DWORD imageSize = 0;

    memcpy(&m_bif, cPtr, sizeof(BITMAPFILEHEADER));
    cPtr += sizeof(BITMAPFILEHEADER);

    memcpy(&m_bi, cPtr, sizeof(BITMAPINFOHEADER));
    cPtr += sizeof(BITMAPINFOHEADER);

	// Determine the size in bytes of the image data.
    padding = ((m_bi.bmiHeader.biBitCount/8) * abs(m_bi.bmiHeader.biWidth)) % 4;
    if(padding > 0)
        padding = 4 - padding;

    imageSize = (m_bi.bmiHeader.biWidth * (m_bi.bmiHeader.biBitCount/8) + padding) * m_bi.bmiHeader.biHeight;

    //allocate raw memory to read in image
    if(!AllocateStorage( imageSize ) )
		return NULL;

    // Set some of the image parameters.
    SetImageHeight(m_bi.bmiHeader.biHeight);
    SetImageWidth(m_bi.bmiHeader.biWidth);
    SetImageBPP((unsigned char)m_bi.bmiHeader.biBitCount);
 
	//if lower than 16,24,32 bit colour, read in the palette.
	if(m_bi.bmiHeader.biBitCount <= 8)
	{
        // Set the number of colors to the max for the mode (MAX 256).
		m_iColoursUsed = 1 << m_bi.bmiHeader.biBitCount;

        // Use what is in the file if it is defined.
		if(m_bi.bmiHeader.biClrUsed != 0)
			m_iColoursUsed = (int)m_bi.bmiHeader.biClrUsed;

        // Allocate the space for the palette.
		m_pbi = (BITMAPINFO*)malloc( sizeof(BITMAPINFO) + (sizeof(RGBQUAD) * m_iColoursUsed) );

        // Copy the header info into the new memory.
		memcpy(&m_pbi->bmiHeader, &m_bi.bmiHeader, sizeof(m_bi.bmiHeader));

		RGBQUAD *Ptr = &m_pbi->bmiColors[0];
		RGBQUAD  buf;

		for(int c=0; c<m_iColoursUsed; c++, Ptr++) 
		{
            memcpy(&buf, cPtr, 4);
            cPtr += 4;
			Ptr->rgbRed      = buf.rgbRed;
			Ptr->rgbGreen    = buf.rgbGreen;
			Ptr->rgbBlue     = buf.rgbBlue;
			Ptr->rgbReserved = 0;
		}
	}

	int ret;

	switch(m_bi.bmiHeader.biCompression)
	{
	case BI_RGB:
        memcpy(GetStoragePtr(), &lpCache[m_bif.bfOffBits], imageSize );
		SetupBitmapInfo();
		break;
	case BI_RLE8:
        offset = m_bif.bfOffBits;
		ret = ReadRLE8Format(lpCache, offset);
		m_pbi->bmiHeader.biCompression = 0;
		break;
	default:
		ret = FALSE;
		DeAllocateStorage();
		break;
	}

	//create surface, this uses member variables set during AllocateStorage
	if(!CreateSurface(lpDD, memType))
		return NULL;

	
	HDC hdc;
    m_lpdds->GetDC(&hdc);
	
	UINT iUsage = DIB_RGB_COLORS;
	int rc = 0;
	if(m_pbi)
		rc = StretchDIBits(hdc,
					0,
					0,
					m_iWidth,
					m_iHeight,
					0,
					0,
					m_iWidth,
					m_iHeight,
					m_pStorage,
					m_pbi,
					iUsage,
					SRCCOPY);
	else
		rc = StretchDIBits(hdc,
					0,
					0,
					m_iWidth,
					m_iHeight,
					0,
					0,
					m_iWidth,
					m_iHeight,
					m_pStorage,
					&m_bi,
					iUsage,
					SRCCOPY);

	m_lpdds->ReleaseDC(hdc);

	//free up the raw memory
	DeAllocateStorage();

	if (rc == GDI_ERROR)
		return NULL;
	
	if(m_pbi)
		free(m_pbi);

	return m_lpdds;
}


BOOL CDXImageBMP::ReadRLE8Format(CHAR* lpCache, DWORD offset)
{
	CHAR* Ptr  = GetStoragePtr();
    CHAR* cPtr = &lpCache[offset];
    int x = 0, y = 0;
    BOOL eof8 = FALSE;
	int cLinesWorth = ((m_cBpp * m_iWidth + 31) / 32) * 4;

    while( !eof8 )
    {
        BYTE c = (BYTE)*cPtr;
        cPtr++;
        BYTE d = (BYTE)*cPtr;
        cPtr++;

        if ( c )
		{
			CDXASSERT(Ptr);
            memset(Ptr, d, c);
            x += c;
            Ptr += c;
		}
        else
		{
            switch( d )
			{
				//...sMSWCC_EOL:56:
			case MSWCC_EOL:
				{
					int to_eol = cLinesWorth - x;

					CDXASSERT(Ptr);
					memset(Ptr, 0, (size_t) to_eol);
					Ptr += to_eol;
					x = 0;
					if ( ++y == m_iWidth )
						eof8 = TRUE;
				}
				break;
				//...e
				//...sMSWCC_EOB:56:
			case MSWCC_EOB:
				if ( y < m_iHeight )
				{
					int to_eol = cLinesWorth - x;
					
					CDXASSERT(Ptr);
					memset(Ptr, 0, (size_t) to_eol);
					x = 0; y++;
					Ptr += to_eol;
					while ( y < m_iHeight )
					{
						memset(Ptr, 0, (size_t) cLinesWorth);
						Ptr += cLinesWorth;
						y++;
					}
				}
				eof8 = TRUE;
				break;
				//...e
				//...sMSWCC_DELTA:56:
			case MSWCC_DELTA:
				{
                    BYTE dx = (BYTE)*cPtr;
                    cPtr++;
					BYTE dy = (BYTE)*cPtr;
                    cPtr++;
					int fill = dx + dy * cLinesWorth;

					CDXASSERT(Ptr);
					memset(Ptr, 0, (size_t) fill);
					Ptr += fill;
					x += dx; y += dy;
					if ( y == m_iHeight )
						eof8 = TRUE;
				}
				break;
				//...e
				//...sdefault:56:
			default:
				{
					int n = (int) d;
					
					while ( n-- > 0 )
					{
                        *Ptr++ = *cPtr;
                        cPtr++;
					}
					x += d;
					if ( d & 1 )
                        cPtr++; // Align
				}
				break;
			}
		}
	}

	return TRUE;
}

