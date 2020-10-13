/******************************************************************************
* Filename: Graphics.h
* Copyright (c) 2000, UAF Development Team (email CocoaSpud@hotmail.com)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__
#include "externs.h"

#ifdef USE_DIRECTX

//#if _WIN32_WINNT >= 0x0400
//#define NTDX3
//#endif

#define CDXINCLUDEALL
#include "..\cdx\cdx.h"

#ifdef UAFEngine 
#include "..\UAFWin\CDXBitmapFont.h"
#endif

#endif // is USE_DIRECTX

#include "SurfaceMgr.h"

//extern int MAX_SURFACE_CACHE;
extern BOOL RunFullscreen;

// special surface keys
const long MOUSEBUFFERNUM = -96;
const long MOUSESAVENUM   = -97;
const long BACKBUFFERNUM  = -98;
const long FRONTBUFFERNUM = -99;

#ifdef USE_DIRECTX
#define ScreenDefType  CDXScreen
#define ScreenPtr      CDXScreen*
#define SurfaceDefType CDXSurface
#define SurfacePtr     CDXSurface*
#define FontSurfaceDefType CDXBitmapFont
#define FontSurfacePtr     CDXBitmapFont*
#define SpriteDefType CDXSprite
#define SpritePtr CDXSprite*

#ifdef UAFEDITOR
// display popup dialog
#define LOGERROR(pScreen, pErrorStr) {MsgBoxError(pErrorStr);}
#else
// write error to screen
#define LOGERROR(pScreen, pErrorStr) {WriteDebugString("LOGERR %i: %s\n", __LINE__, pErrorStr); SetMiscError(GraphicEngineError);}
#endif

#endif

#ifdef UAFEngine
class Movie;
#endif

/*
struct SurfaceCacheData
{
  SurfaceCacheData() { Clear(); }
  ~SurfaceCacheData() { Clear(); }

  void Clear() { surface=NULL; insertTime=0; }

  SurfaceCacheData& operator =(const SurfaceCacheData& src)
  {
    if (this==&src) return *this;
    surface=src.surface;
    insertTime=src.insertTime;
    return *this;
  }
  SurfaceCacheData(const SurfaceCacheData& src) { *this=src; }

  SurfacePtr surface;
  DWORD insertTime;
};

class SurfaceCacheMgr
{
public:

  SurfaceCacheMgr(DWORD maxsize, CString id);
  ~SurfaceCacheMgr();
  void Clear();
  DWORD GetSize() const { return m_maxsize; }

  BOOL GetSurface(int w, int h, SurfacePtr& surface);
  BOOL AddSurface(SurfacePtr& surface);
  void RemoveOldest();

private:
  DWORD m_maxsize;
  CString m_id;
  CList<SurfaceCacheData, SurfaceCacheData&> m_SurfaceList;
};
*/


class Graphics
{
public:
  Graphics();
  ~Graphics();

  void Clear();
  BOOL IsInitialized() { return m_pScreen != NULL; }
  
  BOOL InitGraphicsFullScreen(HWND hWnd, int &w, int &h, int &bits);
  BOOL InitGraphicsWindowed(HWND hWnd, int w=0, int h=0);
  BOOL IsFullScreen() const { return m_IsFullScreen; }

  long AddSurface(SurfaceType type, UINT width, UINT height);
  long AddSurface(SurfaceType type, LPRECT pRect);
#ifdef UAFEngine
  long AddFileSurface(const char *filename, SurfaceType type, const CString& path);
#else
  long AddFileSurface(const char *filename, SurfaceType type, const CString& path1, const CString& path2);
  long AddFileSurface(const char *filename, const char *appendix, SurfaceType type, const CString& path1, const CString& path2);
#endif
  BOOL AddMouseSurfaces(const char *filename);

  BOOL ReleaseSurface(long& key); 
  void ReleaseSurfaceTypes(DWORD types);
  BOOL ValidSurface(long key);
  BOOL ChangeSurfaceType(long key, SurfaceType type);

  long AddSprite(const char *filename, int width, int height, int frames, int restartFrame, SurfaceType type);
  BOOL ReleaseSprite(long &key);
  BOOL SetFrame(long key, int frame);
  int  GetFrame(long key);
  BOOL SetFirstFrame(long key);
  BOOL SetNextFrame(long key);  
  BOOL IsLastFrame(long key);
  BOOL ValidSprite(long key);
  BOOL GetSpriteRect(long key, LPRECT pRect);

  void ClearSurface(SurfacePtr pSurf, DWORD color=0, RECT *rect=NULL);
  void ClearSurface(long key, DWORD color=0, RECT *rect=NULL, BOOL FillBg=TRUE);
  void ClearAdventureBackground(long key);

  BOOL GetSurfaceRect(long key, LPRECT pRect);
  BOOL GetSurfaceStats(long key, int &width, int &height, int &bits);
  BOOL GetSpriteSurfaceStats(long key, int &width, int &height, int &bits);

  void FlipSurfaces();
  void FlipToGDI();
  void DoScreenCapture(const char *path);

#ifdef UAFEngine
  // bitmapped font functions
  long CreateBitmappedFont(LOGFONT *plf, COLORREF Color, COLORREF BackColor, COLORREF TransColor);
  long CreateBitmappedFont(const char *FontName,int Height, COLORREF Color, COLORREF BackColor, COLORREF TransColor);
  void HighlightText(BOOL enable);
  void EnableFontColorTags(BOOL enable);
  BOOL GetEnableFontColorTags();
  /*  20200208  PRS
  void StoreFontColorTags(void);
  void RestoreFontColorTags(void);
  void ResetFontColorTags(void);
  void WhiteText(BOOL enable);
  void YellowText(BOOL enable);
  void RedText(BOOL enable);
  void GreenText(BOOL enable);
  void BlueText(BOOL enable);
  void BlackText(BOOL enable);
  void OrangeText(BOOL enable);
  void BrightOrangeText(BOOL enable);
  void SilverText(BOOL enable);
  void MagentaText(BOOL enable);
  void CyanText(BOOL enable);
  */
// 20200208 PRS     void Custom1Text(BOOL enable);

  // blits created font from font surface
  BOOL DrawFont(int X, int Y, 
                int baseFontNumber,
                FONT_COLOR_NUM defaultColorNum,
                const char *Text, 
                long int SrcFontSurf, 
                BOOL SlowText=FALSE, 
                BOOL UseBackBuffer=TRUE, 
                BOOL Transparent=TRUE,
                int KeyIndex=-1);
  // uses device context and windows TextOut()
  BOOL DrawText(int X, int Y, const char *Text, COLORREF color, long dest);

  int GetCharacterWidth( char ch, BOOL IsHighlighted=FALSE );
  int GetCharacterHeight( char ch, BOOL IsHighlighted=FALSE  );
  int GetTextWidth(const char *Text, int length, BOOL IsHighlighted=FALSE  );
  int GetMaxCharacterHeight() { return m_MaxCharHeight; }
  int GetMaxCharacterWidth() { return m_MaxCharWidth; }

  // movie functions
  BOOL PlayMovie(const char *filename, RECT *pDstRect=NULL, BOOL fullscreen=TRUE);
  BOOL StopMovie();
  BOOL UpdateMovie();
#endif 

  // set clipping rect for front buffer
  void SetClipRect(const RECT *pRect);  
  // for windowed mode, assign clipper to any active window
  BOOL SetClipperHwnd(HWND hWnd);
  // assign clipper back to application's main window
  BOOL RestoreClipperHwnd();

  // Some surface types are fully blitted, some have a
  // source color key.
  BOOL UseTransparency(SurfaceType type);
  BOOL GetColorKey(long key, COLORREF &color);
  BOOL SetDestColorKey(long key);
  BOOL GetColorAt(long key, int x, int y, COLORREF &color);  

  HDC  GetSurfaceDC(long key);
  void ReleaseSurfaceDC(long key);

  BOOL CanRestoreDDSurfaces();
  BOOL RestoreAll(CDXScreen *pDD, CDXSurface *pSurf1, CDXSurface *pSurf2);

  // copies srcRect onto dstRect area of dst surface
#ifdef UAFEDITOR
  // no backbuffer in editor, blit directly to main window
  BOOL BlitImage(WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, BOOL flipY=FALSE, long dstSurf=FRONTBUFFERNUM, const RECT *dstRect=NULL);
  BOOL BlitTransImage(WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, BOOL flipY=FALSE, long dstSurf=FRONTBUFFERNUM, const RECT *dstRect=NULL);
  BOOL BlitAlphaBlackImage(WORD Shadow, WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, long dstSurf=FRONTBUFFERNUM);
  BOOL BlitAlphaImage(WORD Shadow, WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, long dstSurf=FRONTBUFFERNUM);
  BOOL BlitAlphaTransImage(WORD Alpha, WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, long dstSurf=FRONTBUFFERNUM);
  BOOL BlitSprite(WORD dstX, WORD dstY, long srcSurf, BOOL flipY=FALSE, long dstSurf=FRONTBUFFERNUM, const RECT *dstRect=NULL);
  BOOL BlitSpriteImage(WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, BOOL flipY=FALSE, long dstSurf=FRONTBUFFERNUM, const RECT *dstRect=NULL);
  BOOL BlitAlphaSprite(WORD Alpha, WORD dstX, WORD dstY, long srcSurf, long dstSurf=FRONTBUFFERNUM);    
  BOOL FillRect(long dstSurf, const RECT *dstRect, COLORREF rgb);
  BOOL FillRect(long dstSurf, const RECT *dstRect, HBRUSH rgbBrush);
  BOOL FillRect(const RECT *dstRect, HBRUSH rgbBrush, HDC hDC);
  BOOL DarkenDestSurface(WORD Shadow, const RECT *dstRect, long dstSurf=FRONTBUFFERNUM);
#else
  BOOL BlitImage(WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, BOOL flipY=FALSE, long dstSurf=BACKBUFFERNUM, const RECT *dstRect=NULL);
  BOOL BlitTransImage(WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, BOOL flipY=FALSE, long dstSurf=BACKBUFFERNUM, const RECT *dstRect=NULL);
  BOOL BlitAlphaBlackImage(WORD Shadow, WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, long dstSurf=BACKBUFFERNUM);
  BOOL BlitAlphaImage(WORD Shadow, WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, long dstSurf=BACKBUFFERNUM);
  BOOL BlitAlphaTransImage(WORD Alpha, WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, long dstSurf=BACKBUFFERNUM);
  BOOL BlitSprite(WORD dstX, WORD dstY, long srcSurf, BOOL flipY=FALSE, long dstSurf=BACKBUFFERNUM, const RECT *dstRect=NULL);
  BOOL BlitSpriteImage(WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, BOOL flipY=FALSE, long dstSurf=BACKBUFFERNUM, const RECT *dstRect=NULL);
  BOOL BlitAlphaSprite(WORD Alpha, WORD dstX, WORD dstY, long srcSurf, long dstSurf=BACKBUFFERNUM);    
  BOOL FadeInImage(const char *filename, SurfaceType type, const CString& path);
  BOOL FadeToBlack() { if (m_pScreen) return (m_pScreen->FadeToBlack(1500)==DD_OK); return FALSE; }
  BOOL DarkenDestSurface(WORD Shadow, const RECT *dstRect, long dstSurf=BACKBUFFERNUM);
#endif

  // added to make conversion to CDX easier
  BOOL BlitImage(long dstSurf, long srcSurf, const RECT *srcRect, const RECT *dstRect);
  BOOL BlitTransImage(long dstSurf, long srcSurf, const RECT *srcRect, const RECT *dstRect);

private:
  CCriticalSection m_cs;

  SurfaceMgr<SurfacePtr>     m_SurfMgr;
  SurfaceMgr<SpritePtr>      m_SpriteMgr;
#ifdef UAFEngine
  SurfaceMgr<FontSurfacePtr> m_FontMgr;
  Movie *m_pMovie;
#endif
  ScreenPtr m_pScreen;

  // front/back buffer info
  BYTE m_bits;
  UINT m_width;
  UINT m_height;

  // largest character dimensions in created font bitmap
  int m_MaxCharWidth;
  int m_MaxCharHeight;
  // handle to first font surface created. All font surfs use the same
  // font style so it doesn't matter which font surface is used to
  // determine font character sizes.
  long fontSurface; 
  BOOL m_IsFullScreen;

  int StepDownBitDepth(int bits);

  SurfacePtr GetSurfacePtr(long key);
  SurfacePtr GetSurfacePtr(long key, SurfaceType &type);
  int GetColorBits(SurfacePtr pSurf);  
  int  GetMaxFrames(SpritePtr pSurf);

  SpritePtr m_pMouseBuffer;
  SurfacePtr m_pMouseSave;
  RECT m_MovieRect;
};

extern Graphics GraphicsMgr;

#endif // __GRAPHICS_H__