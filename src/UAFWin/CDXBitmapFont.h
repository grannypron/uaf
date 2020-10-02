//////////////////////////////////////////////////////////////////////////////////
// Project Name:   [ CDX BitmapFont ]
// Author:         [ Ioannis Karagiorgos - karagior@pluto.fernuni-hagen.de ]
// Date:           [ 7.4.1999 ]
// Revision:       [ 1.00 ]
//////////////////////////////////////////////////////////////////////////////////


#ifndef __CDX_BITMAPFONT__
#define __CDX_BITMAPFONT__

//#define NTDX3
#define CDXINCLUDEALL
#include "cdx.h"


#ifndef CDX_CENTERJUSTIFY
#define CDX_CENTERJUSTIFY 1
#endif
#ifndef CDX_RIGHTJUSTIFY
#define CDX_RIGHTJUSTIFY 3
#endif

extern CDXAPI void Clip(int *DestX, int *DestY, RECT *SrcRect, RECT *DestRect);

class CDXBitmapFont
{
  CDXBitmapFont() { } // default ctor not used
public:
  
  enum { MAX_CHAR=128 };
  typedef struct 
  {
    RECT    Rectangle;
    int   Width , Height;
  } CDXBITMAPFONT;
  
  CDXBITMAPFONT CDXBitmapFontArray[ MAX_CHAR ];
  LOGFONT m_lf;
  bool m_uself;
  int       FontColor;
  int       FontBackgroundColor;
  int       FontColorKey;
  int       FontHeight;
  int       FontAttributes;
  const char * FontName;
  int       TextSurfaceType;

  int m_MaxCharHeight;
  int m_MaxCharWidth;
  
  CDXSurface *  TextSurface;
  CDXScreen  *  Screen;
  
  CDXBitmapFont( CDXScreen * Scr );
  CDXBitmapFont(CDXScreen *Scr,LOGFONT *lf,int Color,int ColKey,int BackColor,int MemSurfaceType=CDXMEM_VIDTHENSYS);
  CDXBitmapFont( CDXScreen * Scr , const char * FontNam , int Height , int Color = RGB( 255 , 255 , 255 ), int ColKey = RGB(0,0,0),
     int BackColor = RGB(0,0,0) , int Attributes = FW_NORMAL , int SurfaceType = CDXMEM_VIDTHENSYS );
  ~CDXBitmapFont();
  
  void Create(CDXScreen * Scr, LOGFONT *lf, int Color, int ColKey, int BackColor, int SurfaceType=CDXMEM_VIDTHENSYS);
  void Create( CDXScreen * Scr , const char * FontNam , int Height , int Color , int ColKey, 
    int BackColor , int Attributes = FW_NORMAL , int SurfaceType = CDXMEM_VIDTHENSYS );
  void PaintCharactersInSurface( void );
  
  HRESULT Draw(int X, int Y, const char * Text , CDXSurface* lpDDest , int Length = -1 );
  HRESULT DrawClipped(int X, int Y, char * Text , CDXSurface* lpDDest, LPRECT ClipRect , int Length = -1);
  HRESULT DrawTrans(int X, int Y, const char * Text , CDXSurface* lpDDest , int Length = -1);
  HRESULT DrawTransClipped(int X, int Y, char * Text , CDXSurface* lpDDest, LPRECT ClipRect , int Length = -1);
  HRESULT DrawAligned( int X , int Y , int Width , char * Text , CDXSurface * lpDDest , int Align );
  HRESULT DrawAlignedTrans( int X , int Y , int Width , char * Text , CDXSurface * lpDDest , int Align );
  
  void SetFont( char * FontNam , int Height , int Color , int Attributes = FW_NORMAL );
  void SetColor( int Color );
  void SetColorKey( int ColorKey );
  void SetBackgroundColor( int Color );
  
  int GetCharacterWidth( char ch );
  int GetCharacterHeight( char ch );
  int GetTextWidth(const char * Text, int length=-1 );
  
  void Restore( );
};

#endif