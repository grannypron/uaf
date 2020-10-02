//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdximagetga.h,v $
// $Author: cocoaspud $
//
// $Log: cdximagetga.h,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.2  2000/02/14 16:32:22  jhebert
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
// Revision 1.1  2000/01/19 23:08:59  jhebert
// Adding CDXImage to the library.
//
// 
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// CDXImageTGA.h: interface for the CDXImageTGA class.
//
//A Targa file is a header, followed by an identification string, followed by
//a color map, followed by the data for the image. Both the identification
//string and the color-map can be of zero length.
//////////////////////////////////////////////////////////////////////////////////

#ifndef CDXIMAGETGA_H
#define CDXIMAGETGA_H

#include "cdximagebase.h"


#define	TGA_NO_IMAGE	0		// No image data included in file
#define	TGA_UNCOMP_CM	1		// Uncompressed, Color-Mapped (VDA/D 
								// and Targa M-8 images)             
#define	TGA_UNCOMP_RGB	2		// Uncompressed, RGB images (eg: ICB 
								// Targa 16, 24 and 32)              
#define	TGA_UNCOMP_BW	3		// Uncompressed, B/W images (eg:     
								// Targa 8 and Targa M-8 images)     
#define	TGA_RL_CM		9		// Run-length, Color-Mapped (VDA/D   
								// and Targa M-8 images)             
#define	TGA_RL_RGB		10		// Run-length, RGB images (eg: ICB   
								// Targa 16, 24 and 32)              
#define	TGA_RL_BW		11		// Run-length, B/W images (eg: Targa 
								// 8 and Targa M-8)                  
#define	TGA_COMP_CM		32		// Compressed Color-Mapped (VDA/D)   
								// data using Huffman, Delta, and    
								// run length encoding               
#define	TGA_COMP_CM_4	33		// Compressed Color-Mapped (VDA/D)   
								// data using Huffman, Delta, and    
								// run length encoding in 4 passes   

#define	IDB_ATTRIBUTES	0x0f	// How many attrib bits per pixel    
								// ie: 1 for T16, 8 for T32          
#define	IDB_ORIGIN		0x20	// Origin in top left corner bit     
								// else its in bottom left corner    
#define	IDB_INTERLEAVE	0xc0	// Interleave bits as defined below  
#define	IDB_NON_INT		0x00	// Non-Interlaced                    
#define	IDB_2_WAY		0x40	// 2 way (even/odd) interleaving     
#define	IDB_4_WAY		0x80	// 4 way interleaving (eg: AT&T PC)  


#pragma pack(1)

struct _tgaHeader 
{
	unsigned char     cIDLength;
	unsigned char     cColorMapType;
	unsigned char     cType;
	unsigned short    iFirstColourMapEntry;
	unsigned short    iColourMapLength;
	unsigned char     cColourMapEntrySize;
	unsigned short    iImageXOrigin;
	unsigned short    iImageYOrigin;
	unsigned short    iWidth;
	unsigned short    iHeight;
	unsigned char     cBPP;
	unsigned char     cDescriptorBits;
};

struct _tgaFooter 
{
    unsigned long   extarea_offset;
    unsigned long   devdir_offset;
    unsigned char   signature[16];
    unsigned char   period;         // always be "."
    unsigned char   null;           // (0x00)
};

struct _tgaStruct 
{
	_tgaHeader    header;
	char          *ID;
	_tgaFooter    footer;
	unsigned char *color_map;
};

#pragma pack()

/////////////////////////////////////////////////////////////////////////


class CDXImageTGA : public  CDXImageBase
{
public:
	CDXImageTGA(void);
	virtual ~CDXImageTGA(void);

    CDX_LPDIRECTDRAWSURFACE GetImage(CDX_LPDIRECTDRAW lpDD, LONG lSize, CHAR* lpCache, BYTE memType);
	BOOL Validate(const CHAR *head, const int headsize, const CHAR *foot, const int footsize);
	void t16_t24(BYTE *dest, const BYTE *src, int n);
	void t32_t24(BYTE *dest, const BYTE *src, int n);

private:
	_tgaStruct	m_tgaInfo;
};

#endif CDXIMAGETGA_H
