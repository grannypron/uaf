//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagepsd.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdximagepsd.cpp,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.3  2000/05/10 23:29:25  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.1.1.1  2000/04/22 16:09:45  hebertjo
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
// Revision 1.2  2000/01/28 16:53:58  MICHAELR
// Added CVS header to top of file
//
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////

#include "CDXImagePSD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDXImagePSD::CDXImagePSD(void) { }

CDXImagePSD::~CDXImagePSD(void) { }


BOOL CDXImagePSD::Validate(const CHAR *head, const int headsize, const CHAR *foot, const int footsize)
{
	_psdHeader	*h = (_psdHeader*)head;

	if(strcmp(h->magic, "8BPS")!=0)
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Creates a CDXSurface from a PSD file
//////////////////////////////////////////////////////////////////////////////////
CDX_LPDIRECTDRAWSURFACE CDXImagePSD::GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType )
{
	WORD compressionType;

	ZeroMemory(&m_psdInfo, sizeof(m_psdInfo));

    memcpy(&m_psdInfo, lpCache, sizeof(m_psdInfo));
    lpCache += sizeof(m_psdInfo);
   	
	// Version must be equal to 1
	m_psdInfo.version = BIG2LIT(m_psdInfo.version);
	if(m_psdInfo.version != 1)
		return NULL;

	// Make sure there's at least one layer, and no more than our constant.
	m_psdInfo.channels = BIG2LIT(m_psdInfo.channels);
	if ( (m_psdInfo.channels < 0) || (m_psdInfo.channels > MAX_PSD_CHANNELS) )
		return NULL;

	// We can only load in files with 8 bits per channel (24bpp)
	m_psdInfo.bpc = BIG2LIT(m_psdInfo.bpc);
	if(m_psdInfo.bpc != 8)
		return NULL;

	m_psdInfo.height = DWORDBIG2LIT(m_psdInfo.height);
	m_psdInfo.width  = DWORDBIG2LIT(m_psdInfo.width);

	// Make sure the color mode is RGB.
	// Valid options are:
	//   0: Bitmap
	//   1: Grayscale
	//   2: Indexed color
	//   3: RGB color
	//   4: CMYK color
	//   7: Multichannel
	//   8: Duotone
	//   9: Lab color
	m_psdInfo.colormode = BIG2LIT(m_psdInfo.colormode);
	if(m_psdInfo.colormode != 3)
		return NULL;

	// Skip the Mode Data.  (It's the palette for indexed color; 
	// other info for other modes.)
	if(m_psdInfo.ModeDataCount)
    {
        lpCache += m_psdInfo.ModeDataCount;
    }

	// Skip the image resources.  (resolution, pen tool paths, etc)
    DWORD ResourceDataCount = *((DWORD*)lpCache);
    lpCache += sizeof(DWORD);

	ResourceDataCount = DWORDBIG2LIT(ResourceDataCount);
	if(ResourceDataCount)
    {
        lpCache += ResourceDataCount;
    }

	// Skip the reserved data.
    DWORD ReservedDataCount = *((DWORD*)lpCache);
    lpCache += sizeof(DWORD);

	ReservedDataCount = DWORDBIG2LIT(ReservedDataCount);
	if(ReservedDataCount)
    {
        lpCache += sizeof(DWORD);
    }

	// Find out if the data is compressed. (if it's not RLE, we can't
	// handle it) Known values:
	//   0: no compression
	//   1: RLE compressed
    compressionType = *((WORD*)lpCache);
    lpCache += sizeof(WORD);

	compressionType = BIG2LIT(compressionType);
	if (compressionType > 1)
		return NULL;

	// Allocate enough memory for all the pixels (4 bytes each)
	if(!AllocateStorage( m_psdInfo.width, m_psdInfo.height, 32 ))
		return NULL;

	// Unpack the PSD into the memory we just allocated
	UnPackPSD( lpCache, (unsigned char*)m_pStorage, m_psdInfo.channels, compressionType);

	// Create the DirectX surface
	if(CreateSurface(lpDD, memType) == FALSE)
	{
		DeAllocateStorage();
		return NULL;
	}
	
	SetInverted(TRUE);
	SetupBitmapInfo();

	// Draw the bitmap on the DirectX surface
	HDC hdc;
    m_lpdds->GetDC(&hdc);
	int rc = StretchDIBits(hdc,
					0,
					0,
					m_iWidth,
					m_iHeight,
					0,
					0,
					m_iWidth,
					m_iHeight,
					m_pStorage,
					m_pBitmapInfo,
					m_iUsage,
					SRCCOPY);
	
	m_lpdds->ReleaseDC(hdc);
	if (rc == GDI_ERROR)
	{
		DeAllocateStorage();
		return NULL;
	}

	// If we're loading the Alpha Blending Chanel
    BYTE* alphaMask;

	if(m_psdInfo.channels > 3)
	{
		alphaMask = new BYTE[m_iWidth * m_iHeight];

		// Go through and load it all in
		for(int x=0; x < m_iWidth; x++)
		{
			for(int y=0; y < m_iHeight; y++)
			{
				alphaMask[(y * m_iWidth) + x] = (unsigned char)m_pStorage[(((y * m_iWidth) + x) * 4) + 3];
			}
		}
	}
	else
	{
		alphaMask = NULL;
	}





//JJH:  Must fix this !!!
    //pSurface->SetAlphaMaskPointer(alphaMask);
    delete [] alphaMask;
//JJH:  Must fix this !!!





	// De-allocate the memory we created to load in the pixels
	DeAllocateStorage();
	
	return m_lpdds;
}

//////////////////////////////////////////////////////////////////////////////////
// Given a file handle this method will unpack a PSD file
//////////////////////////////////////////////////////////////////////////////////
void CDXImagePSD::UnPackPSD( CHAR* lpCache, unsigned char* pixels, WORD channelCnt, WORD compression )
{
	int Default[4] = {0,0,0,255};
	int chn[4]     = {2,1,0,3};
	int PixelCount = m_iWidth * m_iHeight;

	if(compression) 
	{
        // Skip over the scan line byte counts.
        lpCache += m_iHeight * channelCnt * 2;

		for(int c=0; c < 4; c++) 
		{
			int pn = 0;
			int channel = chn[c];

			if(channel >= channelCnt) 
			{
				for(pn=0; pn < PixelCount ;pn++)
				{
					pixels[(pn * 4) + channel] = Default[channel];
				}
			}
			else // Uncompress
			{
				int	count = 0;

				while(count < PixelCount)
				{
                    int len = (int)( *((BYTE*)lpCache));
                    lpCache++;

					if(len == 128) 
					{
					} 
					else if(len < 128) // No RLE  
					{
						len++;
						count += len;

						while(len)
						{
                            pixels[(pn * 4) + channel] = (unsigned char)*lpCache;
                            lpCache++;

							pn++;
							len--;
						}
					}
					else if(len > 128)  // RLE packed
					{
						len ^= 0x0FF;
						len += 2;
                        unsigned char val = (unsigned char)*lpCache;
                        lpCache++;

						count += len;
						while(len) 
						{
							pixels[(pn * 4) + channel] = val;
							pn++;
							len--;
						}
					}
				}
			}
		}
		
	} 
	else 
	{
		for(int c=0; c < 4; c++) 
		{
			int channel = chn[c];
			if (channel > channelCnt) 
			{
				for(int pn=0; pn < PixelCount; pn++)
				{
					pixels[(pn*4) + channel] = Default[channel];
				}
			} 
			else 
			{
				for(int n=0; n < PixelCount; n++) 
				{
                    pixels[(n*4) + channel] = (unsigned char)*lpCache;
                    lpCache++;

				}
			}
		}
	}	
}


