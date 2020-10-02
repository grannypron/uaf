// Project Name:   [ CDX BitmapFont ]
// Author:         [ Ioannis Karagiorgos - karagior@pluto.fernuni-hagen.de ]
// Date:           [ 7.4.1999 ]
// Revision:       [ 1.00 ]
//////////////////////////////////////////////////////////////////////////////////
#include "..\Shared\stdafx.h"
//#include "externs.h"
#include "CDXBitmapFont.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
CDXBitmapFont::CDXBitmapFont(CDXScreen *Scr,LOGFONT *lf,int Color,int ColKey,int BackColor,int MemSurfaceType)
{
  TextSurface = NULL;
  m_MaxCharHeight=m_MaxCharWidth=0;
  Create( Scr, lf, Color, ColKey, BackColor, MemSurfaceType);
}

// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
CDXBitmapFont::CDXBitmapFont( CDXScreen * Scr , const char * FontNam , int Height , 
                              int Color , int ColKey, int BackColor , int Attributes , int MemSurfaceType )
{
  TextSurface = NULL;
  m_MaxCharHeight=m_MaxCharWidth=0;
  Create( Scr , FontNam , Height , Color , ColKey , BackColor , Attributes, MemSurfaceType );
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
CDXBitmapFont::CDXBitmapFont( CDXScreen * Scr )
{
  TextSurface = NULL;
  m_MaxCharHeight=m_MaxCharWidth=0;
  Create( Scr , "Courier" , 12 , RGB( 255 , 255 , 255 ) , RGB(0,0,0) , RGB(0,0,0) , 
          FW_NORMAL , CDXMEM_VIDTHENSYS);
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
CDXBitmapFont::~CDXBitmapFont( )
{
    if( TextSurface != NULL ) delete TextSurface;    
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::Create(CDXScreen * Scr, LOGFONT *lf, int Color, int ColKey, int BackColor, int MemSurfaceType)
{
    int             X , Y , i;
    char            TextBuffer[ 2 ];
    SIZE            size;
    CDXSurface *    TempSurface;

    m_lf = *lf;
    m_uself=true;

    FontName            = lf->lfFaceName;;
    FontColor           = Color;
    FontBackgroundColor = BackColor;
    FontColorKey        = ColKey;
    //FontHeight          = Height;
    FontAttributes      = lf->lfWeight;;
    TextSurfaceType     = MemSurfaceType;
    Screen              = Scr;

    if( TextSurface != NULL )
    {
      delete TextSurface;
      TextSurface = NULL;
    }
    // create temporary surface in order to get the character widths
    // surface can be created in system memory because it is only needed one time
    TempSurface = new CDXSurface;
    TempSurface->Create( Screen , 100 , 100 , CDXMEM_SYSTEMONLY );

    // the font surface will be width=320 and height as needed
    // i chose width=320 because some video cards dont support surfaces wider
    // than the actual resolution in video memory, so if you choose 
    // videomode 320x200 or 320x240 the surface would be allocated in system memory
    // which would slow down output of fonts.
    TempSurface->ChangeFont( m_lf );

    // loop through all 256 characters and calculate position and size of every character
 
    X = Y = 0;
    HDC hDC = TempSurface->GetDC( );
    
    int oldMode = SetMapMode(hDC, MM_TEXT);
    int logpixy = GetDeviceCaps(hDC, LOGPIXELSY);    
    SetMapMode(hDC, oldMode);

    double points = abs(lf->lfHeight);
    FontHeight = ceil((points * 72.0) / (double)logpixy);
    //FontHeight = MulDiv(abs(lf->lfHeight), 72, logpixy);
    
    TEXTMETRIC texmet;
    GetTextMetrics(hDC, &texmet);

    for( i=0; i<128; i++ )
    {
        TextBuffer[ 0 ] = i;
        TextBuffer[ 1 ] = '\0';
        //if (TextBuffer[0] == 'm')
        //{
        //  int kkk=1;
        //};
        TempSurface->SetFont( );
        SetBkMode( hDC , OPAQUE );
        SetTextColor(hDC, Color);
        SetBkColor( hDC, BackColor );
        GetTextExtentPoint32( hDC , TextBuffer , 1 , &size );
        //size.cy += 1;

        if( X+size.cx >= ((320-texmet.tmMaxCharWidth)-1) )
        {
            X = 0;
            Y += size.cy;
        }

        CDXBitmapFontArray[ i ].Height = size.cy;
        CDXBitmapFontArray[ i ].Width  = size.cx;

        if (size.cy > m_MaxCharHeight)
          m_MaxCharHeight = size.cy;
        if (size.cx > m_MaxCharWidth)
          m_MaxCharWidth = size.cx;

        CDXBitmapFontArray[ i ].Rectangle.left      = X;
        CDXBitmapFontArray[ i ].Rectangle.top       = Y;
        CDXBitmapFontArray[ i ].Rectangle.right     = X + size.cx;
        CDXBitmapFontArray[ i ].Rectangle.bottom    = Y + size.cy;
        X += size.cx;
    }

    TempSurface->ReleaseDC();    
    delete TempSurface;

    TextSurface = new CDXSurface;
    TextSurface->Create( Screen , 320 , Y+m_MaxCharHeight+5 , TextSurfaceType );

    PaintCharactersInSurface( );
}

// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::Create( CDXScreen * Scr , const char * FontNam , int Height , 
                              int Color , int ColKey, int BackColor , int Attributes , int MemSurfaceType )
{
    int             X , Y , i;
    char            TextBuffer[ 2 ];
    SIZE            size;
    CDXSurface *    TempSurface;

    FontName            = FontNam;
    FontColor           = Color;
    FontBackgroundColor = BackColor;
    FontColorKey        = ColKey;
    FontHeight          = Height;
    FontAttributes      = Attributes;
    TextSurfaceType     = MemSurfaceType;
    Screen              = Scr;

    if( TextSurface != NULL )
    {
      delete TextSurface;
      TextSurface = NULL;
    }

    // create temporary surface in order to get the character widths
    // surface can be created in system memory because it is only needed one time
    TempSurface = new CDXSurface;
    TempSurface->Create( Screen , 100 , 100 , CDXMEM_SYSTEMONLY );

    // the font surface will be width=320 and height as needed
    // i chose width=320 because some video cards dont support surfaces wider
    // than the actual resolution in video memory, so if you choose 
    // videomode 320x200 or 320x240 the surface would be allocated in system memory
    // which would slow down output of fonts.
    TempSurface->ChangeFont( FontName , 0 , FontHeight , FontAttributes );

    // loop through all 256 characters and calculate position and size of every character
 
    X = Y = 0;
    HDC hDC = TempSurface->GetDC( );

    for( i=0; i<128; i++ )
    {
        TextBuffer[ 0 ] = i;
        TextBuffer[ 1 ] = '\0';
        
        TempSurface->SetFont( );
        SetBkMode( hDC , OPAQUE );
        SetTextColor( hDC, RGB( 255 , 255 , 255 ) );
        SetBkColor( hDC, RGB( 0 , 0 , 0 ) );
        GetTextExtentPoint32( hDC , TextBuffer , 1 , &size );

        // if x + character width exceed 310 then begin new line
        if( X+size.cx >= 310 )
        {
            X = 0;
            Y += size.cy;
        }

        CDXBitmapFontArray[ i ].Height = size.cy;
        CDXBitmapFontArray[ i ].Width  = size.cx;

        if (size.cy > m_MaxCharHeight)
          m_MaxCharHeight = size.cy;
        if (size.cx > m_MaxCharWidth)
          m_MaxCharWidth = size.cx;

        CDXBitmapFontArray[ i ].Rectangle.left      = X;
        CDXBitmapFontArray[ i ].Rectangle.top       = Y;
        CDXBitmapFontArray[ i ].Rectangle.right     = X + size.cx;
        CDXBitmapFontArray[ i ].Rectangle.bottom    = Y + size.cy;
        X += size.cx;
    }

    TempSurface->ReleaseDC();
    delete TempSurface;

    TextSurface = new CDXSurface;
    TextSurface->Create( Screen , 320 , Y+Height+5 , TextSurfaceType );

    PaintCharactersInSurface( );
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::PaintCharactersInSurface( void )
{
    char    TextBuffer[ 2 ];
    int     i;

    if (TextSurface == NULL)
      return;

    if (m_uself)
      TextSurface->ChangeFont( m_lf );
    else
      TextSurface->ChangeFont( FontName , 0 , FontHeight , FontAttributes );

    TextSurface->SetColorKey( FontColorKey );

    // print all characters on surface
    HDC hDC = TextSurface->GetDC( );
    TextSurface->SetFont( );
    SetBkMode( hDC, OPAQUE );
    SetTextColor( hDC,  FontColor );
    SetBkColor( hDC, FontBackgroundColor );

    HRGN Region;

    for( i=0; i<128; i++ )
    {
        TextBuffer[ 0 ] = i;
        TextBuffer[ 1 ] = '\0';

        // select clip region because some characters like 'j' exceed the drawing rectangle
        // and reach into other characters bitmaps
        Region = CreateRectRgn( CDXBitmapFontArray[i].Rectangle.left ,
                                CDXBitmapFontArray[i].Rectangle.top ,
                                CDXBitmapFontArray[i].Rectangle.right ,
                                CDXBitmapFontArray[i].Rectangle.bottom );
        
        SelectClipRgn( hDC , Region );

        TextOut( hDC, 
                 CDXBitmapFontArray[i].Rectangle.left , 
                 CDXBitmapFontArray[i].Rectangle.top , 
                 TextBuffer , 1 );
        
        DeleteObject( Region );
    }


    TextSurface->ReleaseDC( );
    TextSurface->SetColorKey( FontColorKey );

    /*
    // if you want to see the bitmap we just created...
#ifdef _DEBUG
    char fname[_MAX_PATH+1];
    sprintf(fname, "font_%s_%u.bmp", m_lf.lfFaceName, FontColor);
    TextSurface->SaveAsBMP(fname);
#endif
    */
}




// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
HRESULT CDXBitmapFont::DrawAligned( int X , int Y , int Width , char * Text , CDXSurface * lpDDest , int Align )
{
    int     TWidth, Width2;
    HRESULT rval = DD_OK;
    char    ch;

    if (TextSurface == NULL)
      return DDERR_SURFACELOST;

    TWidth = GetTextWidth( Text );

    if( Align == CDX_CENTERJUSTIFY )
    {
        // if text wider then output width do print only the fitting characters
        if( TWidth < Width )
        {
            X = X + ( Width - TWidth ) / 2;

            while( (ch = *Text++) != '\0' )
            {
                ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
                rval = TextSurface->DrawBlk(lpDDest, X, Y, &CDXBitmapFontArray[ ch ].Rectangle);
    
                if (FAILED(rval))
                {
                  if(rval == DDERR_SURFACELOST) 
                    Restore();
                  else
                    return rval;
                }
                X += CDXBitmapFontArray[ ch ].Width;
            }
        }
        else
        {
            TWidth = 0;
            while( (ch = *Text++) != '\0' )
            {
                ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
                Width2 = CDXBitmapFontArray[ ch ].Width;
                TWidth += Width2;
                if( TWidth > Width )
                    break;
                rval = TextSurface->DrawBlk(lpDDest, X, Y, &CDXBitmapFontArray[ ch ].Rectangle);
                if (FAILED(rval))
                {
                  if(rval == DDERR_SURFACELOST) 
                    Restore();
                  else
                    return rval;
                }
                X += Width2;
            }
        }
    }
    else
        if( Align == CDX_RIGHTJUSTIFY )
        {
            // if text wider then output width do print only the fitting characters
            if( TWidth < Width )
            {
                X = X + ( Width - TWidth );

                while( (ch = *Text++) != '\0' )
                {
                    ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
                    rval = TextSurface->DrawBlk(lpDDest, X, Y, &CDXBitmapFontArray[ ch ].Rectangle);
                    if (FAILED(rval))
                    {
                      if(rval == DDERR_SURFACELOST) 
                        Restore();
                      else
                        return rval;
                    }
                    X += CDXBitmapFontArray[ ch ].Width;
                }
            }
            else
            {
                TWidth = 0;
                while( (ch = *Text++) != '\0' )
                {
                    Width2 = CDXBitmapFontArray[ ch ].Width;
                    TWidth += Width2;
                    if( TWidth > Width )
                        break;
                    ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
                    rval = TextSurface->DrawBlk(lpDDest, X, Y, &CDXBitmapFontArray[ ch ].Rectangle);
                    if (FAILED(rval))
                    {
                      if(rval == DDERR_SURFACELOST) 
                        Restore();
                      else
                        return rval;
                    }
                    X += Width2;
                }
            }
        }

    return rval;
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
HRESULT CDXBitmapFont::DrawAlignedTrans( int X , int Y , int Width , char * Text , CDXSurface * lpDDest , int Align )
{
    int     TWidth, Width2;
    HRESULT rval = DD_OK;
    char    ch;

    if (TextSurface == NULL)
      return DDERR_SURFACELOST;

    TWidth = GetTextWidth( Text );

    if( Align == CDX_CENTERJUSTIFY )
    {
        // if text wider then output width do print only the fitting characters
        if( TWidth < Width )
        {
            X = X + ( Width - TWidth ) / 2;

            while( (ch = *Text++) != '\0' )
            {
                ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
                rval = TextSurface->DrawTrans(lpDDest, X, Y, &CDXBitmapFontArray[ ch ].Rectangle);    
                if (FAILED(rval))
                {
                  if(rval == DDERR_SURFACELOST) 
                    Restore();
                  else
                    return rval;
                }
                X += CDXBitmapFontArray[ ch ].Width;
            }
        }
        else
        {
            TWidth = 0;
            while( (ch = *Text++) != '\0' )
            {
                ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
                Width2 = CDXBitmapFontArray[ ch ].Width;
                TWidth += Width2;
                if( TWidth > Width )
                    break;
                rval = TextSurface->DrawTrans(lpDDest, X, Y, &CDXBitmapFontArray[ ch ].Rectangle);
                if (FAILED(rval))
                {
                  if(rval == DDERR_SURFACELOST) 
                    Restore();
                  else
                    return rval;
                }
                X += Width2;
            }
        }
    }
    else
        if( Align == CDX_RIGHTJUSTIFY )
        {
            // if text wider then output width do print only the fitting characters
            if( TWidth < Width )
            {
                X = X + ( Width - TWidth );

                while( (ch = *Text++) != '\0' )
                {
                    ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
                    rval = TextSurface->DrawTrans(lpDDest, X, Y, &CDXBitmapFontArray[ ch ].Rectangle);
                    if (FAILED(rval))
                    {
                      if(rval == DDERR_SURFACELOST) 
                        Restore();
                      else
                        return rval;
                    }
                    X += CDXBitmapFontArray[ ch ].Width;
                }
            }
            else
            {
                TWidth = 0;
                while( (ch = *Text++) != '\0' )
                {
                    ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
                    Width2 = CDXBitmapFontArray[ ch ].Width;
                    TWidth += Width2;
                    if( TWidth > Width )
                        break;
                    rval = TextSurface->DrawTrans(lpDDest, X, Y, &CDXBitmapFontArray[ ch ].Rectangle);
                    if (FAILED(rval))
                    {
                      if(rval == DDERR_SURFACELOST) 
                        Restore();
                      else
                        return rval;
                    }
                    X += Width2;
                }
            }
        }

    return rval;
}



// --------------------------------------------------------------------------------------
// Length = -1 means draw the complete string, else length characters
// --------------------------------------------------------------------------------------
HRESULT CDXBitmapFont::Draw(int X, int Y, const char * Text , CDXSurface* lpDDest , int Length )
{
    HRESULT rval = DD_OK;
    char    ch;

    if (TextSurface == NULL)
      return DDERR_SURFACELOST;

    while(  ( (ch = *Text++) != '\0' ) &&
            (  Length--      != 0    )    )
    {
        ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
        rval = TextSurface->DrawBlk(lpDDest, X, Y, &CDXBitmapFontArray[ ch ].Rectangle);

        if (FAILED(rval))
        {
          if(rval == DDERR_SURFACELOST) 
            Restore();
          else
            return rval;
        }
        X += CDXBitmapFontArray[ ch ].Width;
    }

    return rval;
}



// --------------------------------------------------------------------------------------
// Length = -1 means draw the complete string, else length characters
// --------------------------------------------------------------------------------------
HRESULT CDXBitmapFont::DrawClipped(int X, int Y, char * Text , CDXSurface* lpDDest, LPRECT ClipRect , int Length )
{
    HRESULT rval = DD_OK;
    RECT    ModSrc;
    int     XSave , YSave;
    char    ch;

    if (TextSurface == NULL)
      return DDERR_SURFACELOST;

    while(  ( (ch = *Text++) != '\0' ) &&
            (  Length--      != 0    )    )

    {
        ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
        ModSrc = CDXBitmapFontArray[ ch ].Rectangle;
        XSave = X; YSave = Y;
        Clip(&X, &Y, &ModSrc, ClipRect);
        
        rval = TextSurface->DrawBlk(lpDDest, X, Y, &ModSrc);

        if (FAILED(rval))
        {
          if(rval == DDERR_SURFACELOST) 
            Restore();
          else
            return rval;
        }

        X = XSave;
        Y = YSave;

        X += CDXBitmapFontArray[ ch ].Width;
    }

    return rval;

}




// --------------------------------------------------------------------------------------
// Length = -1 means draw the complete string, else length characters
// --------------------------------------------------------------------------------------
HRESULT CDXBitmapFont::DrawTrans(int X, int Y, const char * Text , CDXSurface* lpDDest , int Length )
{
    HRESULT rval = DD_OK;
    char    ch;

    if (TextSurface == NULL)
      return DDERR_SURFACELOST;

    while(  ( (ch = *Text++) != '\0' ) &&
            (  Length--      != 0    )    )

    {
        ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
        if ((ch >= 0) && (ch <= MAX_CHAR-1))
        {
          RECT temp = CDXBitmapFontArray[ ch ].Rectangle;


          //The Text drawing routines appear to want top, left, right and bottom.
          //Our rectangles contain top, left, width and height.
          // PRS 4 August 2014
          temp.right;
          temp.bottom;
          

          //rval = TextSurface->DrawTrans(lpDDest, X, Y, &CDXBitmapFontArray[ ch ].Rectangle);
          rval = TextSurface->DrawTrans(lpDDest, X, Y, &temp);
          if (FAILED(rval))
          {
            if(rval == DDERR_SURFACELOST) 
              Restore();
            else
              return rval;
          }
          X += CDXBitmapFontArray[ ch ].Width;
        }
    }

    return rval;
}



// --------------------------------------------------------------------------------------
// Length = -1 means draw the complete string, else length characters
// --------------------------------------------------------------------------------------
HRESULT CDXBitmapFont::DrawTransClipped(int X, int Y, char * Text , CDXSurface* lpDDest, LPRECT ClipRect , int Length )
{
    HRESULT rval = DD_OK;
    RECT    ModSrc;
    int     XSave , YSave;
    char    ch;

    if (TextSurface == NULL)
      return DDERR_SURFACELOST;

    while(  ( (ch = *Text++) != '\0' ) &&
            (  Length--      != 0    )    )

    {
        ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
        ModSrc = CDXBitmapFontArray[ ch ].Rectangle;
        XSave = X; YSave = Y;
        Clip(&X, &Y, &ModSrc, ClipRect);
        rval = TextSurface->DrawTrans(lpDDest, X, Y, &ModSrc);
        if (FAILED(rval))
        {
          if(rval == DDERR_SURFACELOST) 
            Restore();
          else
            return rval;
        }
        X = XSave;
        Y = YSave;
        X += CDXBitmapFontArray[ ch ].Width;
    }

    return rval;

}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::SetFont( char * FontNam , int Height , int Color , int Attributes )
{
    Create( Screen , FontNam , Height , Color , FontColorKey , FontBackgroundColor , Attributes );
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::SetColor( int Color )
{
    FontColor      = Color;

    PaintCharactersInSurface( );
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::SetColorKey( int ColorKey )
{
    FontColorKey = ColorKey;
    if (TextSurface == NULL)
      return;

    TextSurface->SetColorKey( FontColorKey );
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::SetBackgroundColor( int Color )
{
    FontBackgroundColor = Color;

    PaintCharactersInSurface( );
}




// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
int CDXBitmapFont::GetCharacterWidth( char ch )
{
    ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
    return CDXBitmapFontArray[ ch ].Width;
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
int CDXBitmapFont::GetCharacterHeight( char ch )
{
    ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
    return CDXBitmapFontArray[ ch ].Height;
}



// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
int CDXBitmapFont::GetTextWidth(const char * Text, int length )
{
  if (Text==NULL) return 0;
    int  Result;
    char ch;

    Result = 0;

    while (   ((ch = *Text++) != '\0')
           && (length != 0))
    {
        ASSERT( (ch >= 0) && (ch <= MAX_CHAR-1) );
        Result += CDXBitmapFontArray[ ch ].Width;
        length--;
    }

    return Result;
}


// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
void CDXBitmapFont::Restore( void )
{
    if (TextSurface == NULL)
      return;

    TextSurface->Restore();
    PaintCharactersInSurface( );
}

//////////////////////////////////////////////////////////////////////////////////
// Clips a destination rectange and modifys X,Y coords appropriatly
//////////////////////////////////////////////////////////////////////////////////
void Clip(int *DestX, int *DestY, RECT *SrcRect, RECT *DestRect)
{
	// If it's partly off the right side of the screen
	if(*DestX + (SrcRect->right - SrcRect->left) > DestRect->right)
		SrcRect->right -= *DestX + (SrcRect->right-SrcRect->left) - DestRect->right;

	// Partly off the left side of the screen
	if(*DestX < DestRect->left)
	{
		SrcRect->left += DestRect->left - *DestX;
		*DestX = DestRect->left;
	}

	// Partly off the top of the screen
	if(*DestY < DestRect->top)
	{
		SrcRect->top += DestRect->top - *DestY;
		*DestY = DestRect->top;
	}

	// If it's partly off the bottom side of the screen
	if(*DestY + (SrcRect->bottom - SrcRect->top) > DestRect->bottom)
	SrcRect->bottom -= ((SrcRect->bottom-SrcRect->top)+*DestY) - DestRect->bottom;

	return;
}