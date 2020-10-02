//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagetga.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdximagetga.cpp,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
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
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////

#include "CDXImageTGA.h"

// helper function to read next two
// bytes in stream and return them
// as a WORD
WORD GetWord(BYTE* &buf)
{
  WORD temp;
  temp = (*buf & 0xff);
  buf++;
  temp += ((*buf & 0xff) << 8);
  buf++;
  return temp;
}

CDXImageTGA::CDXImageTGA(void) 
{ 
    m_tgaInfo.color_map = NULL; 
}

CDXImageTGA::~CDXImageTGA(void) 
{ 
  if (m_tgaInfo.color_map != NULL)
    delete [] m_tgaInfo.color_map;

  m_tgaInfo.color_map = NULL;
}


//////////////////////////////////////////////////////////////
// Description   :	look at footer for TGA signature and check header bpp.
//////////////////////////////////////////////////////////////
BOOL CDXImageTGA::Validate(const CHAR *head, const int headsize, const CHAR *foot, const int footsize)
{
	_tgaHeader* h = (_tgaHeader*)head;
	_tgaFooter* f = (_tgaFooter*)(&foot[footsize-sizeof(_tgaFooter)]);
	
	if(strncmp(&foot[footsize - 18], "TRUEVISION-XFILE", 16) == 0)
	{
		if( h->cBPP==8 || h->cBPP==16 || h->cBPP==24 || h->cBPP==32 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////
// Description   :	-Read in header of TGA,
//					-Check for inverted bitmap
//					-Make sure its valid bit depth
//					-Allocate memory and read data into
//					-Create surface and copy data to surface
//					-Freeup memory
//					-return pointer to new surface
//////////////////////////////////////////////////////////////
CDX_LPDIRECTDRAWSURFACE CDXImageTGA::GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType )
{
	ZeroMemory(&m_tgaInfo, sizeof(m_tgaInfo));
 
    memcpy(&m_tgaInfo.header, lpCache, sizeof(m_tgaInfo.header));
    BYTE *lpData = (BYTE*)lpCache;
    lpData += sizeof(m_tgaInfo.header);
    lpData += m_tgaInfo.header.cIDLength;

	// check if flag specifies the image to be vertically inverted
    if (m_tgaInfo.header.cDescriptorBits & 0x20)
        SetInverted(TRUE);

	// allocate raw memory to read in image
	if(m_tgaInfo.header.cBPP == 8)
	{
		if(!AllocateStorage(m_tgaInfo.header.iWidth, m_tgaInfo.header.iHeight, 8))
			return NULL;
	}
	else
	{
		if(!AllocateStorage(m_tgaInfo.header.iWidth, m_tgaInfo.header.iHeight, 24))
			return NULL;
	}

	int i;	

	if( m_tgaInfo.header.cColorMapType )
	{
        int color_map_length = m_tgaInfo.header.iColourMapLength;
        int color_map_start  = m_tgaInfo.header.iFirstColourMapEntry;
		int bpp_pal = m_tgaInfo.header.cColourMapEntrySize;
		
        if( bpp_pal == 15 ) bpp_pal = 16;

        if (m_tgaInfo.header.cBPP == 8)
        {
            m_tgaInfo.color_map = new BYTE[768];

            if (m_tgaInfo.color_map == NULL)
                return NULL;

            memset(m_tgaInfo.color_map, 0, 768);

            switch (bpp_pal)
            {
            case 16:
                for (i=color_map_start;i<color_map_length;i++)
                {
                    if (i >= 256) break;
                    WORD data = GetWord(lpData);

                    m_tgaInfo.color_map[i + 2] = ((data >> 10) & 0x1F) << 3; // blue
                    m_tgaInfo.color_map[i + 1] = ((data >> 5) & 0x1F) << 3; // green
                    m_tgaInfo.color_map[i + 0] = (data & 0x1F) << 3; // red
                }
                break;

            case 24:
                for (i=color_map_start;i<color_map_length;i++)
                {
                    if (i >= 256) break;

                    m_tgaInfo.color_map[(i*3) + 2] = (*lpData)++; // blue
                    m_tgaInfo.color_map[(i*3) + 1] = (*lpData)++; // green
                    m_tgaInfo.color_map[(i*3) + 0] = (*lpData)++; // red
                }
                break;
            }		
        }
        else
        {
		    lpData += (long)color_map_length * ( (long)bpp_pal / 8L );
        }
	}

    lpData = ((BYTE*)lpCache) + 
           sizeof(m_tgaInfo.header) + 
           m_tgaInfo.header.cIDLength +
           (m_tgaInfo.header.iColourMapLength *
           ( (m_tgaInfo.header.cColourMapEntrySize == 15?16:m_tgaInfo.header.cColourMapEntrySize)/8 ));


	BYTE *p = (BYTE*)GetStoragePtr();  

    // dest buffer width is either 1 byte per pixel or 3 bytes per pixel
    int dstWidth (m_cBpp == 8 ? m_tgaInfo.header.iWidth : m_tgaInfo.header.iWidth*3);

    // src buffer width is 1,2,3, or 4 bytes per pixel for 8.16.24.32 bpp
    int srcWidth = m_tgaInfo.header.iWidth * (m_cBpp / 8);

	WORD c;
	BYTE r,g,b;

	switch(m_tgaInfo.header.cType)
	{
	case TGA_RL_CM:
	case TGA_RL_BW:
	case TGA_RL_RGB:
		{
			int x=0, y=0;

			switch( m_tgaInfo.header.cBPP )
			{
			case 8:
				{
					while( y < m_iHeight )
					{
            int i, cnt = *lpData++;

						if ( cnt == -1 )
						{
							return NULL;
						}

						if ( cnt & 0x80 )
						{
              BYTE v = *lpData++;

							for ( i=0x80; i<=cnt; i++ )
							{
								p[x++] = v;
								if ( x >= srcWidth )
								{ 
									x = 0; 
									y++; 
									p += dstWidth; 
								}
                if (y > m_iHeight)
                  break;
							}
						}
						else
            {
							for ( i=0; i<=cnt; i++ )
							{
                p[x++] = *lpData++;

								if ( x >= srcWidth )
								{ 
									x = 0; 
									y++; 
									p += dstWidth; 
								}
                if (y > m_iHeight)
                  break;
							}
            }
					}
				}
				break;


//			We have 3 bytes per pixel in the data array, indexed by p.
//			We will read 2 bytes per pixel into the right hand 2/3 of each line.
//			Then we will expand leftwards to fill the full width afterwards.
			case 16:
				{				
					while ( y < m_iHeight )
					{
            int i, cnt = *lpData++;
                
						if( cnt == -1 )
							return NULL;

						if ( cnt & 0x80 )
						{
					    c = GetWord(lpData);
					    r = ((c >> 10) & 0x1f) << 3;
					    g = ((c >> 5) & 0x1f) << 3;
					    b = (c & 0x1f) << 3;

							for ( i=0x80; i<=cnt; i++ )
							{
					      p[x++] = r;
					      p[x++] = g;
					      p[x++] = b;

                if ( x >= srcWidth )
								{ 
									x = 0; 
									y++; 
									p += dstWidth; 
								}
                if (y >= m_iHeight)
                  break;
							}
						}
						else
            {
							for ( i=0; i<=cnt; i++ )
							{
					      c = GetWord(lpData);
					      r = ((c >> 10) & 0x1f) << 3;
					      g = ((c >> 5) & 0x1f) << 3;
					      b = (c & 0x1f) << 3;

					      p[x++] = r;
					      p[x++] = g;
					      p[x++] = b;

								if ( x >= srcWidth )
								{ 
									x = 0; 
									y++; 
									p += dstWidth; 
								}
                if (y >= m_iHeight)
                  break;
							}
            }
					}
        }
				break;

			case 24:
				{
					while( y < m_iHeight )
					{
            int i, cnt = *lpData++;

						if ( cnt == -1 )
						{
							return NULL;
						}
						if ( cnt & 0x80 )
						{
              BYTE v1 = *lpData++;
              BYTE v2 = *lpData++;
              BYTE v3 = *lpData++;

							for ( i=0x80; i<=cnt; i++ )
							{
								p[x++] = v1;
								p[x++] = v2;
								p[x++] = v3;
								if( x >= srcWidth )
								{ 
									x = 0; 
									y++; 
									p += dstWidth; 
								}
                if (y >= m_iHeight)
                  break;
							}
						}
						else
						{
							for ( i=0; i<=cnt; i++ )
							{
                p[x++] = *lpData++;
                p[x++] = *lpData++;
                p[x++] = *lpData++;

								if ( x >= srcWidth )
								{ 
									x = 0; 
									y++; 
									p += dstWidth; 
								}
                if (y >= m_iHeight)
                  break;
							}
						}
					}
				}
				break;

			case 32:
				{
					while ( y < m_iHeight )
					{
            int i, cnt = *lpData++;

						if ( cnt == -1 )
						{
							return NULL;
						}
						if ( cnt & 0x80 )
						{
              BYTE v1 = *lpData++;
              BYTE v2 = *lpData++;
              BYTE v3 = *lpData++;
              lpData++;  // Discard the alpha byte

							for ( i=0x80; i<=cnt; i++ )
							{
								p[x++] = v1;
								p[x++] = v2;
								p[x++] = v3;
								if ( x >= srcWidth )
								{
									x = 0;
									y++;
									p += dstWidth;
								}
                if (y >= m_iHeight)
                  break;
							}
						}
						else
						{
							for ( i = 0; i <= cnt; i++ )
							{
                p[x++] = *lpData++;
                p[x++] = *lpData++;
                p[x++] = *lpData++;
                lpData++; // Discard the alpha channel

								if ( x >= srcWidth )
								{
									x = 0;
									y++;
									p += dstWidth;
								}
                if (y >= m_iHeight)
                  break;
							}
						}
					}
				}
				break;
			}
		}
		break;

	case TGA_UNCOMP_BW:
	case TGA_UNCOMP_CM:
	case TGA_UNCOMP_RGB:
		switch ( m_tgaInfo.header.cBPP )
		{
		case 8:
			for( i=0; i<m_iHeight; i++, p+=dstWidth )
      {
          memcpy(p, lpData, srcWidth);
          lpData += srcWidth;
      }
			break;

		case 16:
			for( i=0; i<m_iHeight; i++)
			{
				for( int w=0;w<m_iWidth;w++) 
        {
					c = GetWord(lpData);
					r = ((c >> 10) & 0x1f) << 3;
					g = ((c >> 5) & 0x1f) << 3;
					b = (c & 0x1f) << 3;
					*p++ = r;
					*p++ = g;
					*p++ = b;
				}
      }
			break;

		case 24:
			for( i=0; i<m_iHeight; i++, p+=dstWidth )
			{
        memcpy(p, lpData, srcWidth);
        lpData += srcWidth;
			}
			break;

		case 32:
			{
				BYTE *linebuf;
				if ( (linebuf = (BYTE *)malloc((m_iWidth<<2))) == NULL )
					return NULL;
				
				for( i=0; i<m_iHeight; i++, p+=dstWidth )
				{
                    memcpy(linebuf, lpData, srcWidth);
                    lpData += srcWidth;
					t32_t24(p, linebuf, m_iWidth);
				}
				
				free(linebuf);
			}
			break;
		}
	}

	//create surface, this uses member variables set during AllocateStorage
	if(!CreateSurface(lpDD, memType))
		return NULL;

	//setup the BitmapInfo structure required for StretchDIBits
	SetupBitmapInfo();

	if (m_tgaInfo.color_map != NULL)
	{   
        RGBQUAD buf;
        memset(&buf, 0, sizeof(buf));

		for (int c=0; c < 256; c++) 
		{      
            memcpy(&buf, &m_tgaInfo.color_map[c*3], 3);

			m_pBitmapInfo->bmiColors[c].rgbRed      = buf.rgbBlue;
			m_pBitmapInfo->bmiColors[c].rgbGreen    = buf.rgbGreen;
			m_pBitmapInfo->bmiColors[c].rgbBlue     = buf.rgbRed;
			m_pBitmapInfo->bmiColors[c].rgbReserved = 0;
		}

        delete [] m_tgaInfo.color_map;
        m_tgaInfo.color_map = NULL;
	}

	//return pointer to surface
	return CopyBits();
}

void CDXImageTGA::t16_t24(BYTE *dest, const BYTE *src, int n)
{
	while ( n-- )
	{
		WORD l = *src++;
		WORD h = *src++;
		WORD w = l + (h << 8);
		
		*dest++ = (BYTE) ((w & 0x001fU) << 3);
		*dest++ = (BYTE) ((w & 0x03e0U) >> 2);
		*dest++ = (BYTE) ((w & 0x7c00U) >> 7);
	}
}

void CDXImageTGA::t32_t24(BYTE *dest, const BYTE *src, int n)
{
	while( n-- )
	{
		*dest++ = *src++;
		*dest++ = *src++;
		*dest++ = *src++;
		src++;
	}
}
