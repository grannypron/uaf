/******************************************************************************
* Filename: Graphics.cpp
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
#include "..\Shared\stdafx.h"

//#include "externs.h"
#ifdef UAFEDITOR
//#include "..\UAFWinEd\UAFWinEd.h"
#endif

#ifdef UAFEngine
#include "..\UAFWin\Dungeon.h"
#include "..\UAFWin\MouseThread.h"
#include "GlobalData.h"
#include "..\UAFWin\FormattedText.h"
#endif

#include "Graphics.h"
#include "FileParse.h"

#ifdef UAFEngine
#include "Movie.h"
#include "Viewport.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef WIN10Problem
extern bool WIN10ProblemActive;
#endif


#ifdef TRACE_TIMER_DETAILS
class DETAIL_TRACE
{
  FILE *f;
public:
  DETAIL_TRACE(void);
  ~DETAIL_TRACE(void);
//  DWORD    m_id[1000];
//  DWORD    m_data[1000];
//  DWORD    m_time[1000];
//  DWORD    m_virtualTime[1000];
//  int      m_nextIndex;
  void     trace (DWORD id, DWORD data);
};
extern DETAIL_TRACE detailTrace;
#endif


#ifdef TRACE_TIMER_DETAILS
#define DETAILTRACE(a,b) detailTrace.trace((a),(b));
#else
#define DETAILTRACE(a,b)
#endif




//int MAX_SURFACE_CACHE = 50;
BOOL RunFullscreen=TRUE; // engine only, release build or -windowed option

Graphics GraphicsMgr;

// default
DWORD MemoryUsageFlag = CDXMEM_SYSTEMONLY;


//*****************************************************************************
//    NAME: Graphics::Graphics
//
// PURPOSE: 
//
//*****************************************************************************
Graphics::Graphics() : 
  m_pScreen(NULL)
  ,m_pMouseBuffer(NULL)
  ,m_pMouseSave(NULL)
#ifdef UAFEngine
  ,m_pMovie(NULL)
#endif
{ 
  Clear(); 

  m_SurfMgr.SetName("Pics");
  m_SpriteMgr.SetName("Sprite");

#ifdef UAFEngine
  m_FontMgr.SetName("Fonts");
#endif

  //m_pSurfCacheMgr = new SurfaceCacheMgr(MAX_SURFACE_CACHE ,"SurfCache");
}

//*****************************************************************************
//    NAME: Graphics::~Graphics
//
// PURPOSE: 
//
//*****************************************************************************
Graphics::~Graphics() 
{ 
  Clear(); 
}

//*****************************************************************************
//    NAME: Graphics::UseTransparency
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::UseTransparency(SurfaceType type)
{
  switch (type) 
  {
  case CombatDib:
  case BackGndDib:
  case OutdoorCombatDib:
  case BigPicDib:
  case MapDib:
  case SmallPicDib:
  case CommonDib:
  case BufferDib:
  case TitleDib:
  case SpecialGraphicsOpaqueDib:
    return FALSE;

  case SpriteDib:
  case WallDib:
  case DoorDib:
  case OverlayDib:
  case IconDib:
  case FontDib:
  case MouseDib:
  case TransBufferDib:
  case SpecialGraphicsTransparentDib:
    return TRUE;
  default:
    return FALSE;
  }
}


//*****************************************************************************
//    NAME: Graphics::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::Clear()
{
//  CSingleLock sl(&m_cs, TRUE);

#ifdef UAFEngine
  StopMovie();
#endif
  m_IsFullScreen=FALSE;
  m_bits = 0;
  m_width = 0;
  m_height = 0;
  m_MaxCharWidth = 0;
  m_MaxCharHeight = 0;
  m_SurfMgr.RemoveSurfaceTypes(AllSurfTypes);
  m_SpriteMgr.RemoveSurfaceTypes(AllSurfTypes);
  m_SurfMgr.Clear();
  m_SpriteMgr.Clear();
#ifdef UAFEngine
  m_FontMgr.RemoveSurfaceTypes(AllSurfTypes);
  m_FontMgr.Clear();
  FlipToGDI();
#endif
  // no need to delete this key, the 
  // m_FontMgr takes care of it.
  fontSurface = -1;

  long int tmp = MOUSEBUFFERNUM;
  ReleaseSprite(tmp);
  tmp = MOUSESAVENUM;
  ReleaseSurface(tmp);
  

  //if (m_pSurfCacheMgr != NULL)
  //  delete m_pSurfCacheMgr;
  //m_pSurfCacheMgr=NULL;
  if (m_pScreen != NULL)
    delete m_pScreen;
  m_pScreen = NULL;
}
/*
void DumpCDXScreen(CDXScreen *pScr)
{
  WriteDebugString("  m_lpDD               = %08x\n", pScr->m_lpDD);
  WriteDebugString("  m_ZBuffer            = %08x\n", pScr->m_ZBuffer);
  WriteDebugString("  m_lpDDSFront         = %08x\n", pScr->m_lpDDSFront);
  WriteDebugString("  m_lpDDSBack          = %08x\n", pScr->m_lpDDSBack);
  WriteDebugString("  m_lpDDPalette        = %08x\n", pScr->m_lpDDPalette);
  WriteDebugString("  m_lpClipper          = %08x\n", pScr->m_lpClipper);
  WriteDebugString("  m_dwPixelWidth       = %d\n",   pScr->m_dwPixelWidth);
  WriteDebugString("  m_dwPixelHeight      = %d\n",   pScr->m_dwPixelHeight);
  WriteDebugString("  m_BPP                = %d\n",   pScr->m_BPP);
  WriteDebugString("  m_hWnd               = %08x\n", pScr->m_hWnd);
  WriteDebugString("  m_bFullScreen        = %d\n",   pScr->m_bFullScreen);
  WriteDebugString("  m_NumberOfVideoModes = %d\n",   pScr->m_NumberOfVideoModes);
  WriteDebugString("  m_TripleBuffering    = %d\n",   pScr->m_TripleBuffering);
  WriteDebugString("  m_SelectedDriver     = %d\n",   pScr->m_SelectedDriver);
  WriteDebugString("  m_frameCount         = %d\n",   pScr->m_frameCount);
  WriteDebugString("  m_SurfaceList.m_Count= %d\n",   pScr->m_SurfaceList.GetCount());
  WriteDebugString("  m_lastId             = %d\n",   pScr->m_lastId);
  WriteDebugString("  m_UseClipper         = %d\n",   pScr->m_UseClipper);
}
*/

//*****************************************************************************
//    NAME: Graphics::FlipSurfaces
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::FlipSurfaces() 
{ 
  if (m_pScreen == NULL)
    return;

#ifdef UAFEngine  
  //20130218  if (!bActive) return;

  if (UseCustomCursor)
  {
    MouseRender.LockMouse();
    MouseRender.DrawMouseOnBack();
    MouseRender.UnlockMouse();  
  }
#endif
  //WriteDebugString("FlipSurfaces Flip(TRUE, FALSE, FALSE)\n");
  //DumpCDXScreen(m_pScreen);
  m_pScreen->Flip(TRUE, FALSE, FALSE); 
#ifdef WIN10Problem
  if (WIN10ProblemActive)
    WriteDebugString("Graphics::FlipSurfaces, new front=0x%08x, new back=0x%08x\n",
      (long)m_pScreen->GetFront(), (long)m_pScreen->GetBack());
#endif
}

// set clipping rect for front buffer
//*****************************************************************************
//    NAME: Graphics::SetClipRect
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::SetClipRect(const RECT *pRect) 
{ 
	if (m_pScreen != NULL) 
	{
	  m_pScreen->GetFront()->SetClipRect( const_cast<RECT*>(pRect) ); 
	  m_pScreen->GetBack()->SetClipRect(  const_cast<RECT*>(pRect) );
	}
}

/*
BOOL Graphics::SetClipperHwnd(HWND hWnd) 
{ 
  if (m_pScreen != NULL) 
    return SUCCEEDED(m_pScreen->SetClipperWindow(hWnd)); 
  else return FALSE;  
}
// assign clipper back to application's main window
BOOL Graphics::RestoreClipperHwnd() 
{ 
  if (m_pScreen != NULL) 
    return SUCCEEDED(m_pScreen->RestoreClipper()); 
  else return FALSE; 
}
*/

//*****************************************************************************
//    NAME: Graphics::FlipToGDI
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::FlipToGDI() 
{ 
  if (IsFullScreen())
  {
    if (m_pScreen != NULL) 
      m_pScreen->GetDD()->FlipToGDISurface(); 
  }
}

//*****************************************************************************
//    NAME: Graphics::AddSurface
//
// PURPOSE: 
//
//*****************************************************************************
long Graphics::AddSurface(SurfaceType type, LPRECT pRect) 
{ 
  if (pRect) 
    return AddSurface(type, pRect->right-pRect->left,pRect->bottom-pRect->top);
  else 
    return -1;
}

//*****************************************************************************
//    NAME: Graphics::AddSurface
//
// PURPOSE: 
//
//*****************************************************************************
long Graphics::AddSurface(SurfaceType type, UINT width, UINT height)
{
  if (m_pScreen == NULL)
    return -1;

//  CSingleLock sl(&m_cs, TRUE);

  BOOL success = FALSE;
  long key = -1;
  SurfacePtr pSurf = NULL;

  pSurf = new SurfaceDefType;

  if (pSurf != NULL)
  {
    HRESULT rval = pSurf->Create(m_pScreen, width, height, MemoryUsageFlag);

    if (FAILED(rval))
    {
      char *pErr = DXErrorToString(rval);
      LOGERROR(m_pScreen, pErr);
      success = FALSE;
    }
    else
	    success = TRUE;

    if ((success) && (UseTransparency(type)))
      pSurf->SetColorKey();
  }
  else
  {
    LOGERROR(m_pScreen, "Failed to create and add surface in AddSurface()\n");
    return FALSE;
  }
  
  if (success)
    key = m_SurfMgr.AddSurface(pSurf,type);

  if (key < 0)
    if (pSurf != NULL) delete pSurf;

  return key;
}

//*****************************************************************************
//    NAME: Graphics::ReleaseSurfaceTypes
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::ReleaseSurfaceTypes(DWORD types) 
{ 
  m_SurfMgr.RemoveSurfaceTypes(types); 
  m_SpriteMgr.RemoveSurfaceTypes(types);
#ifdef UAFEngine
  m_FontMgr.RemoveSurfaceTypes(types);
#endif

  if (types == AllSurfTypes)
  {
    ASSERT(m_SurfMgr.GetSurfaceCount()==0);
    ASSERT(m_SpriteMgr.GetSurfaceCount()==0);
#ifdef UAFEngine
    ASSERT(m_FontMgr.GetSurfaceCount()==0);
#endif
  }
}

//*****************************************************************************
//    NAME: Graphics::ReleaseSurface
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::ReleaseSurface(long& key)
{ 
  switch (key)
  {
  case MOUSESAVENUM:
    if (m_pMouseSave != NULL)
      delete m_pMouseSave;
    m_pMouseSave=NULL;
    return TRUE;
  default:
    return m_SurfMgr.RemoveSurface(key); 
  }
}

//*****************************************************************************
//    NAME: Graphics::ReleaseSprite
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::ReleaseSprite(long &key) 
{ 
  switch (key)
  {
  case MOUSEBUFFERNUM:
    if (m_pMouseBuffer != NULL)
      delete m_pMouseBuffer;
    m_pMouseBuffer=NULL;
    return TRUE;
  default:
    return m_SpriteMgr.RemoveSurface(key); 
  }
}

//*****************************************************************************
//    NAME: Graphics::AddMouseSurfaces
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::AddMouseSurfaces(const char *filename)
{
  long int tmp = MOUSEBUFFERNUM;
  ReleaseSprite(tmp);
  tmp = MOUSESAVENUM;
  ReleaseSurface(tmp);

  CString file(filename);
#ifdef UAFEngine
  BOOL success = SearchForFile(file, rte.CursorArtDir());
#else
  BOOL success = SearchForFile(file, rte.CursorArtDir(), ede.TemplateCursorArtDir());
#endif
  int width = 32;
  int height = 32;
  int frames = 15;

  if (success)
  {
    // create mouse image buffer
    SpritePtr pSurf = new SpriteDefType;
    if (pSurf != NULL)
    {
      WriteDebugString("Create(%08x,%s,%d,%d,%d\n",m_pScreen,file,width,height,frames);
      success = SUCCEEDED(pSurf->Create(m_pScreen, file, width, height, frames, CDXMEM_SYSTEMONLY));      

      if (success)
      {
        if (width > pSurf->GetTile()->GetWidth())
          pSurf->GetTile()->m_BlockWidth = pSurf->GetTile()->GetWidth();

        if (height > pSurf->GetTile()->GetHeight())
          pSurf->GetTile()->m_BlockHeight = pSurf->GetTile()->GetHeight();

        if (   (pSurf->GetTile()->m_BlockHeight == pSurf->GetTile()->GetHeight())
            && (pSurf->GetTile()->m_BlockWidth == pSurf->GetTile()->GetWidth()))
        {
          pSurf->GetTile()->m_BlockNum = 1;
          pSurf->GetTile()->m_BlockX = 1;
          pSurf->GetTile()->m_BlockY = 1;
        }
        pSurf->SetColorKey();
        pSurf->SetFrame(0);
      }
      else
      {
        LOGERROR(m_pScreen, "Failed to create and add mouse surface in AddMouseSurfaces()\n");
        if (pSurf!=NULL) delete pSurf;
        return FALSE;
      }
    }
    else
    {
      LOGERROR(m_pScreen, "Failed to allocate mouse surface in AddMouseSurfaces()\n");
      return FALSE;
    }
  
    if (success)
      m_pMouseBuffer = pSurf;
  }
  else
    WriteDebugString("Failed to load cursor file %s in AddMouseSurfaces()\n", filename);

  // create mouse save buffer
  if (success)
  {
    RECT rect;
    GetSpriteRect(MOUSEBUFFERNUM, &rect);
    // PRS 20130228
    //int mwidth = (rect.right-rect.left)+1;
    //int mheight = (rect.bottom-rect.top)+1;
    int mwidth = (rect.right-rect.left);
    int mheight = (rect.bottom-rect.top);
    //hScreenData = AddSurface(BufferDib, mwidth, mheight);

    SurfacePtr pSurf = new SurfaceDefType;
    if (pSurf != NULL)
    {
      HRESULT rval = pSurf->Create(m_pScreen, mwidth, mheight, CDXMEM_SYSTEMONLY);
      WriteDebugString("rval = %d = Create(%08x,%d,%d) SurfaceID = %d\n",rval, m_pScreen, mwidth, mheight, pSurf->GetSurfaceId());
      if (FAILED(rval))
      {
        char *pErr = DXErrorToString(rval);
        LOGERROR(m_pScreen, pErr);
        if (pSurf!=NULL) delete pSurf;
        success = FALSE;
      }
    }
    else
    {
      LOGERROR(m_pScreen, "Failed to create and add surface in AddSurface()\n");
      return FALSE;
    }
  
    if (success)
    {
      m_pMouseSave = pSurf;
      ClearSurface(MOUSESAVENUM);
    }
  }

  return success;
}

#ifdef UAFEDITOR
  long Graphics::AddFileSurface(const char *filename, 
                      const char *appendix, 
                      SurfaceType type, 
                      const CString& path1, 
                      const CString& path2)
{
  CString fname;
  int n;
  fname = filename;
  n = fname.ReverseFind('.');
  if (n > 0)
  {
    fname.Format("%s%s%s", fname.Left(n), appendix, fname.Right(fname.GetLength()-n));
    return AddFileSurface(fname, type, path1, path2);
  };
  return -1;
}
#endif


//*****************************************************************************
//    NAME: Graphics::AddSurface
//
// PURPOSE: 
//
//*****************************************************************************
#ifdef UAFEngine
long Graphics::AddFileSurface(const char *filename, SurfaceType type, const CString& path)
#else
long Graphics::AddFileSurface(const char *filename, SurfaceType type, const CString& path1, const CString& path2)
#endif
{
  if (m_pScreen == NULL) { WriteDebugString("Null screen ptr in AddSurface()\n"); return -1; }
  if (filename == NULL) { WriteDebugString("Null filename ptr in AddSurface()\n"); return -1; }
  if (filename[0] == '\0') { WriteDebugString("Empty filename in AddSurface()\n"); return -1; }

//  CSingleLock sl(&m_cs, TRUE);
  BOOL success = FALSE;
  long key = -1;
  SurfacePtr pSurf = NULL;
  
  CString tmp(filename);

#ifdef UAFEngine
  if (!SearchForFile(tmp, path)) // returns full path file
#else
  if (!SearchForFile(tmp, path1, path2)) // returns tmp = full path file
#endif
  {
    if (!debugStrings.AlreadyNoted(CString("CFFIA01") + tmp))
    {
      WriteDebugString("Can't find file in AddSurface(%s)\n", tmp);
    };
    return -1;
  }

  pSurf = new SurfaceDefType;

  if (pSurf != NULL)
  {
#ifdef UAFEngine
    HRESULT rval = pSurf->Create(m_pScreen, (LPCTSTR)(tmp), MemoryUsageFlag);
    WriteDebugString("rval = %d = Create(%08x,%s,%d)  SurfaceID = %d\n",rval, m_pScreen, (LPCTSTR)(tmp),MemoryUsageFlag, pSurf->GetSurfaceId());
#else
    HRESULT rval = pSurf->Create(m_pScreen, (LPCTSTR)(tmp), MemoryUsageFlag);
#endif

    if (FAILED(rval))
    {
      char *pErr = DXErrorToString(rval);
      CString temp;
      temp.Format("Failed to load image %s: %s\n", (LPCTSTR)tmp, pErr);
      LOGERROR(m_pScreen, temp);
      success = FALSE;
    }
    else
	    success = TRUE;

    if ((success) && (UseTransparency(type)))
      pSurf->SetColorKey();
  }
  else
  {
    LOGERROR(m_pScreen, "Failed to create and add surface in AddSurface()\n");
    return FALSE;
  }
  
  if (success)
    key = m_SurfMgr.AddSurface(pSurf, type);

  if (key < 0)
    if (pSurf != NULL) delete pSurf;

  return key;
}

//*****************************************************************************
//    NAME: Graphics::AddSprite
//
// PURPOSE: 
//
//*****************************************************************************
long Graphics::AddSprite(const char *filename, int width, int height, int frames, int restartFrame, SurfaceType picType)
{
  // filename only....no path

  if (m_pScreen == NULL) return -1;
  if (filename == NULL)  return -1;
  if (filename[0]== '\0') return -1;

//  CSingleLock sl(&m_cs, TRUE);

  BOOL success = FALSE;
  long key = -1;
  SpritePtr pSurf = NULL;
  static char rootName[_MAX_PATH+1];
  CString tmp(filename);
  CString path;

  /*
  // Some filenames may have multiple search names,
  // two filenames max, separated by a '|'.
  // This usage is internal only.
  //
  int tokidx = tmp.Find('|');
  if (tokidx >= 0)
  {
    CString fname = tmp.Left(tokidx);

    if (!SearchForFile(fname)) // returns full path file
    {
      fname = tmp.Right(fname.GetLength()-tokidx-1);
      if (!SearchForFile(fname))
      {
        WriteDebugString("Cant find either file in AddSurface(%s)\n", tmp);
        return -1;
      }
      else
        tmp = fname;
    }
    else
      tmp = fname;
  }
  else
  { 
  */
#ifdef UAFEngine
  {
    switch (picType)
    {
    case SmallPicDib: path = rte.SmallPicDir();  break;
    case BigPicDib:   path = rte.PicArtDir();    break;
    case SpriteDib:   path = rte.SpriteArtDir(); break;
    case IconDib:     path = rte.IconArtDir();   break;
    default: WriteDebugString(" Cannot decode picType=%d in LoadPicSurfaces\n", picType);
    };
  }
    if (!SearchForFile(tmp, path)) // returns full path file
    //if (!SearchForFile(tmp, rte.SpriteArtDir())) // returns full path file
#else
    if (!SearchForFile(tmp, rte.SpriteArtDir(), ede.TemplateSpriteArtDir())) // returns full path file
#endif
    {
      if (!debugStrings.AlreadyNoted(CString("CFFIA02") + tmp))
      {
        WriteDebugString("Can't find file in AddSprite(%s)\n", tmp);
      };
      return -1;
    }
  //}

  strcpy(rootName, tmp);
  if (rootName[0]== '\0') return -1;

  pSurf = new SpriteDefType;

  if (pSurf != NULL)
  {
    success = SUCCEEDED(pSurf->Create(m_pScreen, rootName, width, height, frames, MemoryUsageFlag));      
    pSurf->SetType(restartFrame);
    // for sprites, the block size is the size for 1 frame, while the non-block
    // size is for the whole source image.
    if (success)
    {
      if (pSurf->GetTile()->m_BlockWidth > pSurf->GetTile()->GetWidth())
        pSurf->GetTile()->m_BlockWidth = pSurf->GetTile()->GetWidth();
      if (pSurf->GetTile()->m_BlockHeight != pSurf->GetTile()->GetHeight())
        pSurf->GetTile()->m_BlockHeight = pSurf->GetTile()->GetHeight();

      if (frames == 1)
      {
        // if only 1 frame, make sure frame width equals image width
        if (pSurf->GetTile()->m_BlockWidth < pSurf->GetTile()->GetWidth())
          pSurf->GetTile()->m_BlockWidth = pSurf->GetTile()->GetWidth();
      }

      if (   (pSurf->GetTile()->m_BlockHeight == pSurf->GetTile()->GetHeight())
          && (pSurf->GetTile()->m_BlockWidth == pSurf->GetTile()->GetWidth()))
      {
        pSurf->GetTile()->m_BlockNum = 1;
        pSurf->GetTile()->m_BlockX = 1;
        pSurf->GetTile()->m_BlockY = 1;
      }

      if (UseTransparency(picType))
        pSurf->SetColorKey();
    }
  }
  else
  {
    LOGERROR(m_pScreen, "Failed to create and add surface in AddSprite()\n");
    return FALSE;
  }
  
  if (success)
    key = m_SpriteMgr.AddSurface(pSurf, picType);

  if (key < 0)
  {
    LOGERROR(m_pScreen, "Failed call to AddSurface() in AddSprite\n");
    if (pSurf != NULL) delete pSurf;
  }

  return key;
}

//*****************************************************************************
//    NAME: Graphics::SetFirstFrame
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::SetFirstFrame(long key) 
{ 
  return SetFrame(key, 0); 
}

//*****************************************************************************
//    NAME: Graphics::SetFrame
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::SetFrame(long key, int frame)
{
  if (m_pScreen == NULL)
    return FALSE;

//  CSingleLock sl(&m_cs, TRUE);

  SpritePtr pSurf=NULL;
  switch (key)
  {
  case MOUSEBUFFERNUM:
    pSurf = m_pMouseBuffer;
    break;
  default:
    pSurf = m_SpriteMgr.GetSurfacePtr(key);
    break;
  }

  if (pSurf != NULL)
  {
    if ((frame >= 0) && (frame < GetMaxFrames(pSurf)))
    {
      pSurf->SetFrame(frame);
      return TRUE;
    }
  }
  return FALSE;
}

//*****************************************************************************
//    NAME: Graphics::GetFrame
//
// PURPOSE: 
//
//*****************************************************************************
int Graphics::GetFrame(long key)
{
  if (m_pScreen == NULL)
    return 0;

//  CSingleLock sl(&m_cs, TRUE);

  SpritePtr pSurf=NULL;
  switch (key)
  {
  case MOUSEBUFFERNUM:
    pSurf = m_pMouseBuffer;
    break;
  default:
    pSurf = m_SpriteMgr.GetSurfacePtr(key);
    break;
  }

  if (pSurf != NULL)
  {
      return pSurf->GetFrame();
  }
  return 0;
}

//*****************************************************************************
//    NAME: Graphics::IsLastFrame
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::IsLastFrame(long key)
{
  if (m_pScreen == NULL)
    return FALSE;

//  CSingleLock sl(&m_cs, TRUE);

  SpritePtr pSurf=NULL;
  switch (key)
  {
  case MOUSEBUFFERNUM:
    pSurf = m_pMouseBuffer;
    break;
  default:
    pSurf = m_SpriteMgr.GetSurfacePtr(key);
    break;
  }

  // frames are numbered 0..MAX-1
  if (pSurf != NULL)
    return (pSurf->GetFrame() >= (GetMaxFrames(pSurf)-1));
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: Graphics::SetNextFrame
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::SetNextFrame(long key)
{
  if (m_pScreen == NULL)
    return FALSE;

//  CSingleLock sl(&m_cs, TRUE);

  SpritePtr pSurf=NULL;
  switch (key)
  {
  case MOUSEBUFFERNUM:
    pSurf = m_pMouseBuffer;
    break;
  default:
    pSurf = m_SpriteMgr.GetSurfacePtr(key);
    break;
  }
  
  if (pSurf != NULL)
  {
    int frame = pSurf->GetFrame() + 1;
    if (frame >= GetMaxFrames(pSurf)) {
        if (pSurf->GetType() > 0) // We use the hijacked "Type" attribute of CDX Sprite here for the restart frame attribute
        {
            frame = pSurf->GetType() - 1;
        }
        else 
        {
            frame = 0;
        }
    }
    pSurf->SetFrame(frame);
    return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
//    NAME: Graphics::GetMaxFrames
//
// PURPOSE: 
//
//*****************************************************************************
int Graphics::GetMaxFrames(SpritePtr pSurf)
{ 
  if (pSurf == NULL) return 0;

  return pSurf->GetTile()->GetBlockCount(); 
}

//*****************************************************************************
//    NAME: Graphics::StepDownBitDepth
//
// PURPOSE: 
//
//*****************************************************************************
int Graphics::StepDownBitDepth(int bits)
{
  int lessBits = 0;
  switch (bits)
  {
  case 32:
    lessBits = 24;
    break;
  case 24:
    lessBits = 16;
    break;
  case 16:
    lessBits = 8;
    break;
  }

  return lessBits;
}

//*****************************************************************************
//    NAME: Graphics::InitGraphicsFullScreen
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::InitGraphicsFullScreen(HWND hWnd, int &w, int &h, int &bits)
{
  //char errorStr[100];

  WriteDebugString("Enter InitFullScreen\n");

  if (m_pScreen != NULL)
    return FALSE;  

  WriteDebugString("Creating pScreen\n");
  m_pScreen = new ScreenDefType;

  if (m_pScreen == NULL)
  {
    LOGERROR(m_pScreen, "Failed to create screen object\n");
    Clear();
    return FALSE;
  }

  WriteDebugString("Checking SysMem Flag\n");
  if (ForceSysMemUsage)
  {
    MemoryUsageFlag = CDXMEM_SYSTEMONLY;
    m_pScreen->CreateBackBufferInVRAM(FALSE);
  }
  else
  {
#ifdef _DEBUG // always use sys mem in debug mode
    MemoryUsageFlag = CDXMEM_SYSTEMONLY;
    m_pScreen->CreateBackBufferInVRAM(FALSE);
#else
    MemoryUsageFlag = CDXMEM_VIDTHENSYS;
    m_pScreen->CreateBackBufferInVRAM(TRUE);
#endif
  }

  if (bits == 8)
  {
    bits = 24;
    WriteDebugString("Adjusted color bits from 8 to 24 in InitGraphicsFullScreen\n");
  }

  WriteDebugString("Checking video modes\n");

  BOOL success = FALSE;
  while ((bits >= 16) && (!success))
  { 
    WriteDebugString("Checking %u x %u x %u\n", w, h, bits);

    if (m_pScreen->CheckIfVideoModeExists(w, h, bits))
    {
      success = TRUE;
      WriteDebugString("Using video mode %u x %u x %u\n", w, h, bits);
    }
    else
    {
      bits = StepDownBitDepth(bits);      
      WriteDebugString("Stepping down bit-depth to %u bits\n", bits);
    }
  }

  if (!success)
  {    
    WriteDebugString("Cannot support %u x %u at any color depth\n", w,h);
    Clear();
    return FALSE;
  }

  WriteDebugString("Creating full screen(%d,%d,%d,%d)\n", hWnd,w,h,bits);
  HRESULT rc = m_pScreen->CreateFullScreen(hWnd, w, h, bits);

  if (FAILED(rc))
  {
    WriteDebugString("Could not create full screen %u x %u x %u video mode - %s\n", w,h,bits, DXErrorToString(rc));
    Clear();
    return FALSE;
  }

  WriteDebugString("screen created ok\n");

  m_bits = bits;
  m_width = w;
  m_height = h;

  /*
  if (m_bits == 8)
  {
    CString file;
    if (!ConfigFile.FindToken("PaletteFile", file))
      file = "PaletteFile.bmp";

    CString fullpath;
#ifdef UAFEDITOR
    fullpath.Format("%s%s", EditorArt, file);
#else
    fullpath = file;
#endif

    if (FAILED(m_pScreen->LoadPalette(fullpath)))
    {
      sprintf(errorStr, "Could not load palette from file %s\n", (LPCSTR)fullpath);
      LOGERROR(m_pScreen, errorStr);
      Clear();
      return FALSE;
    }
  }
  */
  m_IsFullScreen=TRUE;
  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::InitGraphicsWindowed
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::InitGraphicsWindowed(HWND hWnd, int w, int h)
{
  char errorStr[100];

  if (m_pScreen != NULL)
    return FALSE;  

  m_IsFullScreen=FALSE;
  HDC Screen = ::CreateIC("Display", NULL, NULL, NULL);
  m_bits = ::GetDeviceCaps(Screen, BITSPIXEL);  

  if ((w <= 0) || (h <= 0))
  {
    m_width = ::GetDeviceCaps(Screen, HORZRES);
    m_height = ::GetDeviceCaps(Screen, VERTRES);
  }
  else
  {
    m_width = w;
    m_height = h;
  }
  ::DeleteDC(Screen);

  m_pScreen = new ScreenDefType;

  if (m_pScreen == NULL)
  {
    LOGERROR(m_pScreen, "Failed to create screen object\n");
    return FALSE;
  }

  if (ForceSysMemUsage)
  {
    MemoryUsageFlag = CDXMEM_SYSTEMONLY;
    m_pScreen->CreateBackBufferInVRAM(FALSE);
  }
  else
  {
#ifdef _DEBUG // always use sys mem in debug mode
    MemoryUsageFlag = CDXMEM_SYSTEMONLY;
    m_pScreen->CreateBackBufferInVRAM(FALSE);
#else
    MemoryUsageFlag = CDXMEM_VIDTHENSYS;
    m_pScreen->CreateBackBufferInVRAM(TRUE);
#endif
  }

#ifdef UAFEDITOR
  BOOL UseClipper = TRUE;
#else
  // BOOL UseClipper = FALSE;
  BOOL UseClipper = (IsFullScreen()==FALSE);
#endif

 // original PRS //BOOL UseClipper = (IsFullScreen()==FALSE);

  WriteDebugString("CreateWindowed(%d,%d,%d,%d)\n",hWnd,m_width,m_height,UseClipper);
  if (FAILED(m_pScreen->CreateWindowed(hWnd, m_width, m_height, UseClipper)))
  {
    sprintf(errorStr, "Could not set %ux%ux%u video mode\n", 
            m_width,m_height,m_bits);
    LOGERROR(m_pScreen, errorStr);
    Clear();
    return FALSE;
  }

  /*
  if (m_bits == 8)
  {
    CString file;
    if (!ConfigFile.FindToken("PaletteFile", file))
      file = "PaletteFile.bmp";

    if (FAILED(m_pScreen->LoadPalette(file)))
    {
      sprintf(errorStr, "Could not load palette from file %s\n", (LPCSTR)file);
      LOGERROR(m_pScreen, errorStr);
      Clear();
      return FALSE;
    }
  }
  */
  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::ClearSurface
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::ClearSurface(SurfacePtr pSurf, DWORD color, RECT *rect)
{
  if (m_pScreen == NULL)
    return;

  if (pSurf == NULL)
    return;

//  CSingleLock sl(&m_cs, TRUE);
  if (rect == NULL)
    pSurf->Fill(color);
  else
    pSurf->FillRect(rect->left,rect->top, rect->right,rect->bottom, color);
}

//*****************************************************************************
//    NAME: Graphics::ClearSurface
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::ClearSurface(long key, DWORD color, RECT *rect, BOOL FillBg)
{
//  CSingleLock sl(&m_cs, TRUE);
  ClearSurface(GetSurfacePtr(key), color, rect);
    
#ifdef UAFEngine 
  if (FillBg)
  {
    if ((key == FRONTBUFFERNUM) || (key == BACKBUFFERNUM))
    {
      if (globalData.BackgroundSurf >= 0)
      {
        if (rect == NULL)
          BlitImage(0,0, globalData.BackgroundSurf, NULL, FALSE, key, NULL);  
        else
          BlitImage(rect->left,rect->top, globalData.BackgroundSurf, rect, FALSE, key,rect);  
      }
    }
  }
#endif
}

#ifdef UAFEngine

// if pDstRect is NULL, then AVI plays fullscreen
//*****************************************************************************
//    NAME: Graphics::PlayMovie
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::PlayMovie(const char *filename, RECT *pDstRect, BOOL fullscreen)
{
#ifdef avicrash
  WriteDebugString("Graphics::PlayMovie\n");
#endif
  if (m_pScreen == NULL) return FALSE;
  StopMovie();

  if (fullscreen)
  {
    ClearAdventureBackground(BACKBUFFERNUM);
    FlipSurfaces();
  }
#ifdef avicrash
  WriteDebugString("Graphics::PLayMovie - new Movie(m_pScreen)\n");
#endif
  m_pMovie = new Movie(m_pScreen);
  if (m_pMovie==NULL) return FALSE;

  SurfacePtr pSurface = NULL;
  long DestSurf = FRONTBUFFERNUM;

  if (fullscreen)
  {
    DestSurf = FRONTBUFFERNUM;
    pSurface = GetSurfacePtr(DestSurf);
  }
  else
  {
    //DestSurf = BACKBUFFERNUM;
    DestSurf = FRONTBUFFERNUM;
    pSurface = GetSurfacePtr(DestSurf);
  }
#ifdef avicrash
  WriteDebugString("Graphics::PlayMovie - m_pMovie->Create\n");
#endif
  if (!m_pMovie->Create(filename, pSurface)) return FALSE;
#ifdef avicrash
  WriteDebugString("Graphics::PLayMovie - Return from m_pMovie->Create\n");
#endif
  m_MovieRect = m_pMovie->GetSrcRect();
  if ((m_MovieRect.bottom==0)||(m_MovieRect.right==0))
  {
    // could not get src movie size, go fullscreen
    m_MovieRect.bottom=SCREEN_HEIGHT;
    m_MovieRect.right=SCREEN_WIDTH;
    m_MovieRect.top=m_MovieRect.left=0;
  }
#ifdef avicrash
  WriteDebugString("Graphics::PlayMovie - Set up DestRect\n");
#endif
  RECT DestRect;

  if (pDstRect==NULL)
  {
    // center src movie coords in output window
    if (!GetSurfaceRect(DestSurf, &DestRect))
    {
      // could not get dst surface rect, go fullscreen
      DestRect.bottom=SCREEN_HEIGHT;
      DestRect.right=SCREEN_WIDTH;
      DestRect.top=DestRect.left=0;
    }

#ifdef _DEBUG
    // over-ride dest surf settings
    //
    // Use screen settings as guide since
    // full desktop becomes front buffer in debug mode
    //
    DestRect.left=DestRect.top=0;
    switch (m_GameScreenRes)
    {
    case res640x480:
      DestRect.right=640;
      DestRect.bottom=480;
      break;
    case res800x600:
      DestRect.right=800;
      DestRect.bottom=600;
      break;
    case res1024x768:
      DestRect.right=1024;
      DestRect.bottom=768;
      break;
    default:
      DestRect.right=640;
      DestRect.bottom=480;
      break;
    }
#endif    
  }
  else
    DestRect = *pDstRect;

#ifdef avicrash
  WriteDebugString("Graphics::PlayMovie - Finish setting DestRect\n");
#endif
  // clip movie output to destination buffer boundaries,
  // and center it within destination window
  //
  // check to see if src movie width is less than current
  // dst screen width
  if ((DestRect.right-DestRect.left) > m_MovieRect.right)
  {
    DWORD offset = (DestRect.right-DestRect.left)-m_MovieRect.right;
    offset /= 2;
    m_MovieRect.left+=offset;
    m_MovieRect.right+=offset;
  }
  else
  {
    // movie width exceeds screen size
    m_MovieRect.left = DestRect.left;
    m_MovieRect.right = DestRect.right;
  }

  // check to see if src movie height is less than current
  // dst screen height
  if ((DestRect.bottom-DestRect.top) > m_MovieRect.bottom)
  {
    DWORD offset = (DestRect.bottom-DestRect.top)-m_MovieRect.bottom;
    offset /= 2;
    m_MovieRect.top+=offset;
    m_MovieRect.bottom+=offset;
  }
  else
  {
    // movie width exceeds screen size
    m_MovieRect.top = DestRect.top;
    m_MovieRect.bottom = DestRect.bottom;
  }
#ifdef avicrash
  WriteDebugString("Graphics::PlayMovie - about to m_pMovie->Play()\n");
#endif
  if (m_pMovie->Play())
  {
    MouseRender.ShowMouseCursor(FALSE);
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//*****************************************************************************
//    NAME: Graphics::StopMovie
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::StopMovie()
{
  MouseRender.ShowMouseCursor(TRUE);
  if (m_pScreen == NULL) return FALSE;
  if (m_pMovie==NULL) return TRUE;

  m_pMovie->Stop();
  m_pMovie->Destroy();
  delete m_pMovie;
  m_pMovie=NULL;
  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::UpdateMovie
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::UpdateMovie()
{
#ifdef avicrash
  WriteDebugString("Graphics::UpdateMovie\n");
#endif
  if (m_pMovie==NULL) return FALSE;
#ifdef avicrash
  WriteDebugString("Graphics::UpdateMovie - m_pMovie->Update\n");
#endif
  if (m_pMovie->Update(&m_MovieRect))
  {
    return TRUE;
  }
  else
  {
#ifdef avicrash
  WriteDebugString("Graphics::UpdateMovie - ShowMouseCursor\n");
#endif
    MouseRender.ShowMouseCursor(TRUE);
    return FALSE;
  }
#ifdef avicrash
  WriteDebugString("Graphics::UpdateMovie - return\n");
#endif
}

//*****************************************************************************
//    NAME: Graphics::ClearAdventureBackground
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::ClearAdventureBackground(long key)
{
  DETAILTRACE(0x4c3334,0);
  if (m_pScreen == NULL) return;
//  CSingleLock sl(&m_cs, TRUE);
  DETAILTRACE(0x4c3330,0);

  SurfacePtr pSurf = GetSurfacePtr(key);
  if (pSurf == NULL) return;
  DETAILTRACE(0x4c3331,0);

  // If in debug mode and clearing the front surface,
  // this will fill the entire screen, including non-game
  // areas.
#ifdef DebugFull
  WriteDebugString("!!ClearAdventureBackground\n");
#endif
  //pSurf->Fill(BACKGROUND_FILL);  
  pSurf->Fill(BACKWARD_BACKGROUND_FILL);  
  if (globalData.BackgroundSurf >= 0)
  {
    DETAILTRACE(0x4c3332,0);
    BlitImage(0,0, globalData.BackgroundSurf, NULL, FALSE, key);
  };
}
#endif

//*****************************************************************************
//    NAME: Graphics::GetSurfaceRect
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::GetSurfaceRect(long key, LPRECT pRect)
{
  if (m_pScreen == NULL)
    return FALSE;

//  CSingleLock sl(&m_cs, TRUE);
  SurfacePtr pSurf = GetSurfacePtr(key);
  if (pSurf != NULL)
  {
    pRect->top = 0;
    pRect->left = 0;
    pRect->right = pSurf->GetWidth();
    pRect->bottom = pSurf->GetHeight();
    return TRUE;
  }
  else
  {
    memset(pRect, 0, sizeof(RECT));
    return FALSE;
  }
}

//*****************************************************************************
//    NAME: Graphics::ValidSprite
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::ValidSprite(long key) 
{ 
  return m_SpriteMgr.ValidSurface(key); 
}

//*****************************************************************************
//    NAME: Graphics::GetSpriteRect
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::GetSpriteRect(long key, LPRECT pRect)
{
  if (m_pScreen == NULL)
    return FALSE;

//  CSingleLock sl(&m_cs, TRUE);
  SpritePtr pSurf=NULL;
  switch (key)
  {
  case MOUSEBUFFERNUM:
    pSurf = m_pMouseBuffer;
    break;
  default:
    pSurf = m_SpriteMgr.GetSurfacePtr(key);
    break;
  }

  if (pSurf != NULL)
  {
    pRect->top = 0;
    pRect->left = 0;
    pRect->right = pSurf->GetTile()->GetBlockWidth();
    pRect->bottom = pSurf->GetTile()->GetBlockHeight();
    return TRUE;
  }
  else
  {
    memset(pRect, 0, sizeof(RECT));
    return FALSE;
  }
}

//*****************************************************************************
//    NAME: Graphics::GetSurfaceStats
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::GetSurfaceStats(long key, int &width, int &height, int &bits)
{
  if (m_pScreen == NULL)
    return FALSE;

//  CSingleLock sl(&m_cs, TRUE);
  SurfacePtr pSurf = GetSurfacePtr(key);
  if (pSurf != NULL)
  {
    width = pSurf->GetWidth();
    height = pSurf->GetHeight();
    //bits = pSurf->GetColorBits();
    bits = GetColorBits(pSurf);
    return TRUE;
  }
  else
  {
    width = 0;
    height = 0;
    bits = 0;
    return FALSE;
  }
}

//*****************************************************************************
//    NAME: Graphics::GetSpriteSurfaceStats
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::GetSpriteSurfaceStats(long key, int &width, int &height, int &bits)
{
  if (m_pScreen == NULL)
    return FALSE;

//  CSingleLock sl(&m_cs, TRUE);
  SpritePtr pSurf=NULL;
  switch (key)
  {
  case MOUSEBUFFERNUM:
    pSurf = m_pMouseBuffer;
    break;
  default:
    pSurf = m_SpriteMgr.GetSurfacePtr(key);
    break;
  }
  if (pSurf != NULL)
  {
    width = pSurf->GetTile()->GetWidth();
    height = pSurf->GetTile()->GetHeight();
    //bits = pSurf->GetTile()->GetColorBits();
    bits = GetColorBits(pSurf->GetTile());
    return TRUE;
  }
  else
  {
    width = 0;
    height = 0;
    bits = 0;
    return FALSE;
  }
}

#ifdef UAFEngine




//*****************************************************************************
//    NAME: Graphics::CreateBitmappedFont
//
// PURPOSE: 
//
//*****************************************************************************
long Graphics::CreateBitmappedFont(LOGFONT *plf, COLORREF Color, COLORREF BackColor, COLORREF TransColor)
{
  if (m_pScreen == NULL)
    return -1;




//  CSingleLock sl(&m_cs, TRUE); 

  //WriteDebugString("FontSurfaceDefType\n");
  FontSurfacePtr pFont = new FontSurfaceDefType(m_pScreen,plf, Color, TransColor, BackColor, MemoryUsageFlag);
  if (pFont == NULL)
  {
    WriteDebugString("Failed to create font %s with color %u\n", plf->lfFaceName, Color);
    return -1;
  }

  long key = m_FontMgr.AddSurface(pFont, FontDib);
  if (key < 0)
  {
    LOGERROR(m_pScreen, "Failed to create font in CreateBitmappedFont()\n");
    delete pFont;
  }
  else
  {
    fontSurface = key; // save first text surface key
    m_MaxCharWidth = pFont->m_MaxCharWidth;
    m_MaxCharHeight = pFont->m_MaxCharHeight;
  }

  return key;
}

//*****************************************************************************
//    NAME: Graphics::CreateBitmappedFont
//
// PURPOSE: 
//
//*****************************************************************************
long Graphics::CreateBitmappedFont(const char *FontName,int Height, COLORREF Color, COLORREF BackColor, COLORREF TransColor)
{
  if (m_pScreen == NULL)
    return -1;

//  CSingleLock sl(&m_cs, TRUE); 

  FontSurfacePtr pFont = new FontSurfaceDefType(m_pScreen,FontName, Height, Color, TransColor, BackColor, FW_BOLD, MemoryUsageFlag);
  if (pFont == NULL)
  {
    WriteDebugString("Failed to create font %s with color %u\n", FontName, Color);
    return -1;
  }

  long key = m_FontMgr.AddSurface(pFont, FontDib);
  if (key < 0)
  {
    LOGERROR(m_pScreen, "Failed to create font in CreateBitmappedFont()\n");
    delete pFont;
  }
  else
  {
    fontSurface = key; // save first text surface key
    m_MaxCharWidth = pFont->m_MaxCharWidth;
    m_MaxCharHeight = pFont->m_MaxCharHeight;
  }

  return key;
}

//*****************************************************************************
//    NAME: Graphics::DrawText
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::DrawText(int X, int Y, const char *Text, COLORREF color, long dest)
{
  SurfacePtr pSurf = GetSurfacePtr(dest);
  if (pSurf != NULL)
    return (pSurf->TextXY( X, Y, color, Text) == DD_OK);

  return FALSE;
}

static BOOL HighlightOn = FALSE;

/*  PRS 20200208
static BOOL WhiteOn = FALSE;
static BOOL YellowOn = FALSE;
static BOOL OrangeOn = FALSE;
static BOOL RedOn = FALSE;
static BOOL GreenOn = FALSE;
static BOOL BlueOn = FALSE;
static BOOL BrightOrangeOn = FALSE;
static BOOL CyanOn = FALSE;
static BOOL MagentaOn = FALSE;
static BOOL SilverOn = FALSE;
static BOOL BlackOn = FALSE;

BOOL Custom1On = FALSE;
*/

static BOOL prevHighlightOn = FALSE;
static BOOL prevWhiteOn = FALSE;
static BOOL prevYellowOn = FALSE;
static BOOL prevOrangeOn = FALSE;
static BOOL prevRedOn = FALSE;
static BOOL prevGreenOn = FALSE;
static BOOL prevBlueOn = FALSE;
static BOOL prevCustom1On = FALSE;

static BOOL FontColorOn=TRUE;

//*****************************************************************************
//    NAME: Graphics::EnableFontColorTags
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::EnableFontColorTags(BOOL enable)
{
  FontColorOn=enable;
}

//*****************************************************************************
//    NAME: Graphics::GetEnableFontColorTags
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::GetEnableFontColorTags()
{
  return FontColorOn;
}

//*****************************************************************************
//    NAME: Graphics::ResetFontColorTags
//
// PURPOSE: 
//
//*****************************************************************************
/*  PRS 20200208
void Graphics::ResetFontColorTags(void)
{
  HighlightOn = FALSE;
  WhiteOn = FALSE;
  YellowOn = FALSE;
  OrangeOn = FALSE;
  RedOn = FALSE;
  GreenOn = FALSE;
  BlueOn = FALSE;
  Custom1On=FALSE;
}
*/
//*****************************************************************************
//    NAME: Graphics::StoreFontColorTags
//
// PURPOSE: 
//
//*****************************************************************************

/*  PRS 20200208
void Graphics::StoreFontColorTags(void)
{
  prevHighlightOn=HighlightOn;
  prevWhiteOn=WhiteOn;
  prevYellowOn=YellowOn;
  prevOrangeOn=OrangeOn;
  prevRedOn=RedOn;
  prevGreenOn=GreenOn;
  prevBlueOn=BlueOn;
  prevCustom1On=Custom1On;
}

//*****************************************************************************
//    NAME: Graphics::RestoreFontColorTags
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::RestoreFontColorTags(void)
{
  HighlightOn=prevHighlightOn;
  WhiteOn=prevWhiteOn;
  YellowOn=prevYellowOn;
  OrangeOn=prevOrangeOn;
  RedOn=prevRedOn;
  GreenOn=prevGreenOn;
  BlueOn=prevBlueOn;
  Custom1On=prevCustom1On;
}
*/
//*****************************************************************************
//    NAME: Graphics::HighlightText
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::HighlightText(BOOL enable)
{
  HighlightOn = enable;
}

//*****************************************************************************
//    NAME: Graphics::WhiteText
//
// PURPOSE: 
//
//*****************************************************************************
/*  PRS 20200208
void AllColorsOff(void)
{
  WhiteOn = FALSE;
  OrangeOn = FALSE;
  RedOn = FALSE;
  GreenOn = FALSE;
  BlueOn = FALSE;
  BlackOn = FALSE;
  BrightOrangeOn = FALSE;
//  MagentaOn = FALSE;
  CyanOn = FALSE;
  SilverOn = FALSE;
  YellowOn = FALSE;
  Custom1On = FALSE;
}
*/

/*  PRS 20200208
void Graphics::WhiteText(BOOL enable)
{
  AllColorsOff();
  WhiteOn = enable;
}
*/
//*****************************************************************************
//    NAME: Graphics::YellowText
//
// PURPOSE: 
//
//*****************************************************************************
/*  PRS 20200208
void Graphics::YellowText(BOOL enable)
{
  AllColorsOff();
  YellowOn = enable;
}
*/
//*****************************************************************************
//    NAME: Graphics::RedText
//
// PURPOSE: 
//
//*****************************************************************************
/*  PRS 20200208
void Graphics::RedText(BOOL enable)
{
  AllColorsOff();
  RedOn = enable;
}
*/
//*****************************************************************************
//    NAME: Graphics::GreenText
//
// PURPOSE: 
//
//*****************************************************************************
/*  PRS 20200208
void Graphics::GreenText(BOOL enable)
{
  AllColorsOff();
  GreenOn = enable;
}
*/
//*****************************************************************************
//    NAME: Graphics::BlueText
//
// PURPOSE: 
//
//*****************************************************************************
/*  PRS 20200208
void Graphics::BlueText(BOOL enable)
{
  AllColorsOff();
  BlueOn = enable;
}
*/
//*****************************************************************************
/*  PRS 20200208
void Graphics::BlackText(BOOL enable)
{
  AllColorsOff();
  BlackOn = enable;
}
*/
//*****************************************************************************
/*  PRS 20200208
void Graphics::BrightOrangeText(BOOL enable)
{
  AllColorsOff();
  BrightOrangeOn = enable;
}
*/
//*****************************************************************************
/*  PRS 20200208
void Graphics::OrangeText(BOOL enable)
{
  AllColorsOff();
  OrangeOn = enable;
}
*/
//*****************************************************************************
/*  PRS 20200208
void Graphics::SilverText(BOOL enable)
{
  AllColorsOff();
  SilverOn = enable;
}
*/

/*  20200208 PRS
//*****************************************************************************
void Graphics::MagentaText(BOOL enable)
{
  AllColorsOff();
  MagentaOn = enable;
}
*/
//*****************************************************************************
/*  PRS 20200208
void Graphics::CyanText(BOOL enable)
{
  AllColorsOff();
  CyanOn = enable;
}
*/
//*****************************************************************************
//    NAME: Graphics::Custom1Text
//
// PURPOSE: 
//
//*****************************************************************************
/* prs 20200208
void Graphics::Custom1Text(BOOL enable)
{
  AllColorsOff();
  Custom1On = enable;
}
*/
//*****************************************************************************
//    NAME: Graphics::DrawFont
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::DrawFont(int X, int Y, 
                        int baseFontNumber,
                        FONT_COLOR_NUM defaultColorNum,
                        const char *Text, 
                        long SrcFontSurf, 
                        BOOL SlowText, 
                        BOOL UseBackBuffer, 
                        BOOL Transparent, 
                        int KeyIndex)
{
//  int currentBaseFontNumber = baseFontNumber;
  FORMATTED_TEXT FT;
  FORMATTED_TEXT::FTStatus ftstat;
  //  FONT_COLOR_NUM colorNum;
  if (m_pScreen == NULL) return FALSE;
  if (Text == NULL)    return FALSE;
  if (Text[0] == '\0') return FALSE;
  DETAILTRACE(0x4c1126,0);
//  CSingleLock sl(&m_cs, TRUE);  
  const char *pText = Text;
  FontSurfacePtr pSrcFont = m_FontMgr.GetSurfacePtr(SrcFontSurf);
  FontSurfacePtr pFont = pSrcFont;
  FontSurfacePtr pHighlightFont = m_FontMgr.GetSurfacePtr(GetHighlightFont(globalData));
  
  FontSurfacePtr pShortcutFont = NULL;
  DETAILTRACE(0x4c1127,0);

  /* wait until we need this font prs 20191228
  if (pFont == NULL)
  {
    WriteDebugString("Failed to find font surface for key %i\n", SrcFontSurf);
    return FALSE;
  }
  */
  SurfacePtr pDestSurf;
  if (UseBackBuffer)
  {
    pDestSurf = m_pScreen->GetBack();
#ifdef WIN10Problem
    if (WIN10ProblemActive)
      WriteDebugString("DrawFont to BackBuffer 0x%08x\n", (long)pDestSurf);
#endif
  }
  else
  {
    pDestSurf = m_pScreen->GetFront();
    //MouseRender.ShowMouseCursor(FALSE);
#ifdef WIN10Problem
    if (WIN10ProblemActive)
      WriteDebugString("DrawFont to FrontBuffer 0x%08x\n", (long)pDestSurf);
#endif
  }
  DETAILTRACE(0x4c1128,0);

  if (pDestSurf == NULL)
  {
    //MouseRender.ShowMouseCursor(TRUE);
    return FALSE;
  }

  BOOL success = TRUE;
  BOOL UseTransparent = Transparent;
  //static BOOL HighlightOn = (SrcFontSurf == GetHighlightFont(globalData));
  HRESULT rc;
  int delay = 0;
//  int count=0;
//  char escape = '/';

  if (SlowText)
    delay = INTER_CHARACTER_DELAY;

  bool WasHighlightOn = HighlightOn;

  FT.Initialize(pText, 999999, 'w', defaultColorNum, baseFontNumber, 0);
  while (((ftstat = FT.NextChar()) != FORMATTED_TEXT::FTEOF) && success)
  {
    char c[2];
    switch (ftstat)
    {
    case FORMATTED_TEXT::FTPrintable:
      c[0] = FT.CurrentCharacter();
      if (HighlightOn)
      {
        UseTransparent = FALSE;
        pFont = pHighlightFont;
      }
      else
      {
        UseTransparent = Transparent;
        if (pSrcFont == pHighlightFont)
          UseTransparent = FALSE;
        if (FontColorOn)
        {
          pFont = m_FontMgr.GetSurfacePtr(
            globalData.fontLib.GetFont(FT.CurrentFont(), 
                                       FT.CurrentColorNum(),
                                       FT.IsCustomColorActive()));
        }
        else
          pFont = pSrcFont;
      };
      if (pFont == NULL) pFont = pSrcFont;
      if ((KeyIndex >= 0) && (FT.CharIndex() == KeyIndex))
      {
        if ((HighlightOn) || (pSrcFont == pHighlightFont))
          pShortcutFont = m_FontMgr.GetSurfacePtr(GetKeyboardHighlightFont(globalData));
        else
          pShortcutFont = m_FontMgr.GetSurfacePtr(GetKeyboardFont(globalData));
      };
#ifdef WIN10Problem
      if (WIN10ProblemActive)
        WriteDebugString("DrawFont: About to Draw to surface 0x%08x\n", (long)pDestSurf);
#endif
      if (!UseTransparent)
      {
        DETAILTRACE(0x4c1130, (long)pDestSurf);
        if ((FT.CharIndex() == KeyIndex) && (pShortcutFont != NULL))
          rc = pShortcutFont->Draw(X, Y, c, pDestSurf, 1);
        else
          rc = pFont->Draw(X, Y, c, pDestSurf, 1);
      }
      else
      {
        DETAILTRACE(0x4c1131, (long)pDestSurf);
        if ((FT.CharIndex() == KeyIndex) && (pShortcutFont != NULL))
        {
          rc = pShortcutFont->DrawTrans(X, Y, c, pDestSurf, 1);
        }
        else
        {
          rc = pFont->DrawTrans(X, Y, c, pDestSurf, 1);
        };
      };

      if (!FAILED(rc))
      {
        X += pFont->GetCharacterWidth(FT.CurrentCharacter());

        if ((!ispunct(FT.CurrentCharacter())) && (delay > 0))
          theApp.AppSleep(delay);

      }
      else
      {
        char* pErr = DXErrorToString(rc);
        LOGERROR(m_pScreen, pErr);
        success = FALSE;
      };
      break;
    case FORMATTED_TEXT::FTEscape:
    case FORMATTED_TEXT::FTColor:
    case FORMATTED_TEXT::FTDigit:
    case FORMATTED_TEXT::FTFontTag:
      break; // ignore
    case FORMATTED_TEXT::WAIT:
      break; // ignore
    default:
      NotImplemented(0xea91e0, false);
    };
 };
  /*
  while ((*pText != '\0') && (success))
  { // One character at a time.  
    if (FontColorOn)
    {
      bool colorTag = true;;
      while (colorTag)
      {
        colorTag = false;
        if (*pText == escape)
        {
          switch (pText[1])
          {
            //if ((pText[1] == 'h') || (pText[1] == 'H'))
            case 'h':
            case 'H':
            //{
              HighlightText(!HighlightOn);
              pText += 2;
              colorTag = true;
            //  continue;
              break;
            //};
            //if ((pText[1] == 'w') || (pText[1] == 'W'))
            case 'w':
            case 'W':
            //{
              WhiteText(!WhiteOn);
              pText += 2;
              colorTag = true;
            //continue;
              break;
            //};
            //if ((pText[1] == 'y') || (pText[1] == 'Y'))
            case 'y':
            case 'Y':
            //{
              YellowText(!YellowOn);
              pText += 2;
              colorTag = true;
            //continue;
              break;
            //};
            //if ((pText[1] == 'r') || (pText[1] == 'R'))
            case 'r':
            case 'R':
            //{
              RedText(!RedOn);
              pText += 2;
              colorTag = true;
            //continue;
              break;
            //};
            //if ((pText[1] == 'g') || (pText[1] == 'G'))
            case 'g':
            case 'G':
            //{
              GreenText(!GreenOn);
              pText += 2;
              colorTag = true;
            //continue;
              break;
            //};
            //if ((pText[1] == 'b') || (pText[1] == 'B'))
            case 'b':
            case 'B':
            //{
              BlueText(!BlueOn);
              pText += 2;
              colorTag = true;
            //continue;
              break;
            //}
            //if ((pText[1] == 'c') || (pText[1] == 'C'))
            case 'c':
            case 'C':
            //{
              Custom1Text(!Custom1On);
              pText += 2;
              colorTag = true;
            //continue;
              break;
            //};
            //if (pText[1] == '/')
            case '/':
            //{
              escape = '/';
              pText += 2;
              colorTag = true;
            //continue;
              break;
            //};
            //if (pText[1] == '#')
            case '#':
            //{
              escape = '#';
              pText += 2;
              colorTag = true;
            //continue;
              break;
            case 'm':
            case 'M':
              MagentaText(!MagentaOn);
              pText += 2;
              colorTag = true;
              break;
            case 'S':
            case 's':
              SilverText(!SilverOn);
              pText += 2;
              colorTag = true;
              break;
            case 'v':
            case 'V':
              CyanText(!CyanOn);
              pText += 2;
              colorTag = true;
              break;
            case 't':
            case 'T':
              BrightOrangeText(!BrightOrangeOn);
              pText += 2;
              colorTag = true;
              break;
            case 'o':
            case 'O':
              OrangeText(!OrangeOn);
              pText += 2;
              colorTag = true;
              break;
            case 'k':
            case 'K':
              BlackText(!BlackOn);
              pText += 2;
              colorTag = true;
              break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
              if ((pText[2] >= '0') && (pText[2] <= '9'))
              {
                currentBaseFontNumber = (pText[1] - '0') * 10 + pText[2] - '0';
                pText += 3;
                colorTag = TRUE;
              };
              break;
            default:
              break;
          };
        };
      };
    };

    if (*pText != '\0')
    {
      if (HighlightOn)
      {
        UseTransparent = FALSE;
        pFont = pHighlightFont;
      }
      else
      {
        UseTransparent = Transparent;
        if (pSrcFont==pHighlightFont)
          UseTransparent=FALSE;
        if (FontColorOn)
        {
        //  if (WhiteOn)
            //pFont = m_FontMgr.GetSurfacePtr(globalData.WhiteFont);
        //  else if (YellowOn)
            //pFont = m_FontMgr.GetSurfacePtr(globalData.YellowFont);
        //  else if (OrangeOn)
            //pFont = m_FontMgr.GetSurfacePtr(globalData.OrangeFont);
        //  else if (RedOn)
            //pFont = m_FontMgr.GetSurfacePtr(globalData.RedFont);
        //  else if (GreenOn)
            //pFont = m_FontMgr.GetSurfacePtr(globalData.GreenFont);
        //  else if (BlueOn)
            //pFont = m_FontMgr.GetSurfacePtr(globalData.BlueFont);
        //  else if (Custom1On)
            //pFont = m_FontMgr.GetSurfacePtr(globalData.Custom1Font);
        //  else
          if (YellowOn)            colorNum = yellowColor;
          else if (BlueOn)         colorNum = blueColor;
          else if (GreenOn)        colorNum = greenColor;
          else if (RedOn)          colorNum = redColor;
          else if (OrangeOn)       colorNum = orangeColor;
          else if (BrightOrangeOn) colorNum = brightOrangeColor;
          else if (BlackOn)        colorNum = blackColor;
          else if (SilverOn)       colorNum = silverColor;
          else if (MagentaOn)      colorNum = magentaColor;
          else if (CyanOn)         colorNum = cyanColor;
          else                     colorNum = whiteColor;
          //pFont = pSrcFont;
          pFont = m_FontMgr.GetSurfacePtr(
                globalData.fontLib.GetFont(currentBaseFontNumber, colorNum));
        }
        else
          pFont = pSrcFont;

        if (pFont==NULL) pFont = pSrcFont;
      }

      if ((KeyIndex >= 0)&&(count == KeyIndex))
      {
        if ((HighlightOn)||(pSrcFont==pHighlightFont))
          pShortcutFont = m_FontMgr.GetSurfacePtr(GetKeyboardHighlightFont(globalData));
        else
          pShortcutFont = m_FontMgr.GetSurfacePtr(GetKeyboardFont(globalData));
      }
#ifdef WIN10Problem
      if (WIN10ProblemActive)
        WriteDebugString("DrawFont: About to Draw to surface 0x%08x\n", (long)pDestSurf);
#endif
      if (!UseTransparent)
      {
        DETAILTRACE(0x4c1130,(long)pDestSurf);
        if ((count == KeyIndex) && (pShortcutFont != NULL))
          rc = pShortcutFont->Draw(X, Y, pText, pDestSurf, 1);
        else
          rc = pFont->Draw(X, Y, pText, pDestSurf, 1);
      }
      else
      {
        DETAILTRACE(0x4c1131,(long)pDestSurf);
        if ((count == KeyIndex) && (pShortcutFont != NULL))
        {
          rc = pShortcutFont->DrawTrans(X, Y, pText, pDestSurf, 1);
        }
        else
        {
           rc = pFont->DrawTrans(X, Y, pText, pDestSurf, 1);
        }
      }

      if (!FAILED(rc))
      {
        X += pFont->GetCharacterWidth(*pText);     

        if ((!ispunct(*pText)) && (delay > 0))
          theApp.AppSleep(delay);

      }
      else
      {
        char *pErr = DXErrorToString(rc);
        LOGERROR(m_pScreen, pErr);
        success = FALSE;
      }
  	  pText++;
      count++;
    }
  }
  */
  DETAILTRACE(0x4c1129,0);
  HighlightOn = WasHighlightOn; // The iteration over FormattedText can turn the highlight on temporarily.  We restore it to its original setting here

  //MouseRender.ShowMouseCursor(TRUE);
  return success;
}

//*****************************************************************************
//    NAME: Graphics::GetCharacterWidth
//
// PURPOSE: 
//
//*****************************************************************************
int Graphics::GetCharacterWidth( char ch, BOOL IsHighlighted )
{
  if (m_pScreen == NULL)
    return 0;

//  CSingleLock sl(&m_cs, TRUE);
  //FontSurfacePtr pFont = m_FontMgr.GetSurfacePtr(fontSurface);
  FontSurfacePtr pFont = NULL;//m_FontMgr.GetSurfacePtr(fontSurface);

  if (IsHighlighted)
  {
    pFont = m_FontMgr.GetSurfacePtr(GetHighlightFont(globalData));
    if (pFont==NULL)
      pFont = m_FontMgr.GetSurfacePtr(fontSurface); // fallback to last loaded font
  }
  else
  {
    pFont = m_FontMgr.GetSurfacePtr(fontSurface);    
  }
  if (pFont != NULL)
    return pFont->GetCharacterWidth(ch);
  else
    return 0;
}

//*****************************************************************************
//    NAME: Graphics::GetCharacterHeight
//
// PURPOSE: 
//
//*****************************************************************************
int Graphics::GetCharacterHeight( char ch, BOOL IsHighlighted )
{
  if (m_pScreen == NULL)
    return 0;

//  CSingleLock sl(&m_cs, TRUE);
  //FontSurfacePtr pFont = m_FontMgr.GetSurfacePtr(fontSurface);
  FontSurfacePtr pFont = NULL;

  if (IsHighlighted)
  {
    pFont = m_FontMgr.GetSurfacePtr(GetHighlightFont(globalData));
    if (pFont==NULL)
      pFont = m_FontMgr.GetSurfacePtr(fontSurface); // fallback to last loaded font
  }
  else
  {
    pFont = m_FontMgr.GetSurfacePtr(fontSurface);    
  }

  if (pFont != NULL)
    return pFont->GetCharacterHeight(ch);
  else
    return 0;
}
// fontSurface is set in CreateBitmappedFont(), and
// will contain the key of the last font created
//*****************************************************************************
//    NAME: Graphics::GetTextWidth
//
// PURPOSE: 
//
//*****************************************************************************
int Graphics::GetTextWidth(const char *Text, int length, BOOL IsHighlighted )
{
  if (m_pScreen == NULL)
    return 0;

//  CSingleLock sl(&m_cs, TRUE);
  FontSurfacePtr pFont = NULL;//m_FontMgr.GetSurfacePtr(fontSurface);

  if (IsHighlighted)
  {
    pFont = m_FontMgr.GetSurfacePtr(GetHighlightFont(globalData));
    if (pFont==NULL)
      pFont = m_FontMgr.GetSurfacePtr(fontSurface); // fallback to last loaded font
  }
  else
  {
    pFont = m_FontMgr.GetSurfacePtr(fontSurface);    
  }

  if (pFont != NULL)
    return pFont->GetTextWidth(Text, length);
  else
    return 0;
}

//*****************************************************************************
//    NAME: Graphics::FadeInImage
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::FadeInImage(const char *filename, SurfaceType type, const CString& path)
{
  if (m_pScreen == NULL) return FALSE;
  BOOL result = TRUE;
#ifdef UAFEngine
  long SplashBgDib = AddFileSurface(filename, type, path);
#else
  long SplashBgDib = AddFileSurface(filename, type);
#endif

  if (SplashBgDib > 0)
  {
    RECT src, dst;
    src.top = src.left = 0;
    dst.top = dst.left = 0;
    dst.right = m_width;
    dst.bottom = m_height;

    int bits,width, height;


    if (GetSurfaceStats(SplashBgDib, width, height, bits))
    {  
      src.right = width;
      src.bottom = height;
      result = TRUE;
      // fill back buffer with fill color
      ClearSurface(BACKBUFFERNUM, BACKGROUND_FILL);
      // flip it to the front
      FlipSurfaces();
#ifdef DebugFull
      WriteDebugString("!!Screen should be black now\n");
#endif
      // blit image to back buffer
      BlitImage(0, 0, SplashBgDib, &src, FALSE, BACKBUFFERNUM, &dst);
      MouseRender.LockMouse();
      MouseRender.DrawMouseOnBack();
      MouseRender.UnlockMouse();  
      // fade it to the front buffer
      WriteDebugString("!!FadeToSource m_pScreen=%08x\n", m_pScreen);
      m_pScreen->FadeToSource(1500);      
    }
    else
      result = FALSE;
  }
  else 
  {
    debugSeverity = 3;
    WriteDebugString("Failed to load fade-in image: '%s'\n", filename);
    result = FALSE;
  }

  ReleaseSurface(SplashBgDib);
  return result;
}

#endif // ifdef UAFEngine

//*****************************************************************************
//    NAME: Graphics::GetSurfacePtr
//
// PURPOSE: 
//
//*****************************************************************************
SurfacePtr Graphics::GetSurfacePtr(long key)
{
  if (m_pScreen == NULL) return NULL;

//  CSingleLock sl(&m_cs, TRUE);
  switch (key)
  {
  case MOUSESAVENUM:
    return m_pMouseSave;
  case BACKBUFFERNUM:
    return m_pScreen->GetBack();
  case FRONTBUFFERNUM:
    return m_pScreen->GetFront();
  default:
    return m_SurfMgr.GetSurfacePtr(key);
  }
}

//*****************************************************************************
//    NAME: Graphics::GetSurfacePtr
//
// PURPOSE: 
//
//*****************************************************************************
SurfacePtr Graphics::GetSurfacePtr(long key, SurfaceType &type)
{
  if (m_pScreen == NULL)
    return NULL;

//  CSingleLock sl(&m_cs, TRUE);
  switch (key)
  {
  case MOUSESAVENUM:
    type = BufferDib;
    return m_pMouseSave;
  case BACKBUFFERNUM:
    type = BufferDib;
    return m_pScreen->GetBack();
  case FRONTBUFFERNUM:
    type = BufferDib;
    return m_pScreen->GetFront();
  default:
    return m_SurfMgr.GetSurfacePtr(key, type);
  }
}

//*****************************************************************************
//    NAME: Graphics::ValidSurface
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::ValidSurface(long key)
{ 
  if (m_pScreen == NULL)
    return FALSE;

//  CSingleLock sl(&m_cs, TRUE);

  switch (key)
  {
  case MOUSESAVENUM:
    return (m_pMouseSave != NULL);
  case BACKBUFFERNUM:
    return (m_pScreen->GetBack() != NULL);
  case FRONTBUFFERNUM:
    return (m_pScreen->GetFront() != NULL);
  default:
    return m_SurfMgr.ValidSurface(key);
  }
}

//*****************************************************************************
//    NAME: Graphics::GetColorKey
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::GetColorKey(long key, COLORREF &color)
{
  if (m_pScreen == NULL) return FALSE;
//  CSingleLock sl(&m_cs, TRUE);
  color = RGB(0,0,0);
  SurfaceType srcType;
  SurfacePtr pSurf = GetSurfacePtr(key, srcType);
  if (pSurf == NULL) return FALSE;
  if (!UseTransparency(srcType)) return FALSE;
  color = pSurf->GetColorKey();
  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::GetColorAt
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::GetColorAt(long key, int x, int y, COLORREF &color)
{
  if (m_pScreen == NULL) return FALSE;
//  CSingleLock sl(&m_cs, TRUE);
  color = RGB(0,0,0);
  SurfaceType srcType;
  SurfacePtr pSurf = GetSurfacePtr(key, srcType);
  if (pSurf == NULL) return FALSE;
  if (!UseTransparency(srcType)) return FALSE;
  pSurf->Lock();
  color = pSurf->GetPixel(x,y);
  pSurf->UnLock();
  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::SetDestColorKey
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::SetDestColorKey(long key)
{
  if (m_pScreen == NULL) return FALSE;
//  CSingleLock sl(&m_cs, TRUE);
  SurfaceType srcType;
  SurfacePtr pSurf = GetSurfacePtr(key, srcType);
  if (pSurf == NULL) return FALSE;
  //return (pSurf->SetDestColorKey()==0);
  return FALSE;
}

//*****************************************************************************
//    NAME: Graphics::ChangeSurfaceType
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::ChangeSurfaceType(long key, SurfaceType type)
{
  if (m_pScreen == NULL) return FALSE;
//  CSingleLock sl(&m_cs, TRUE);

  if ((key == BACKBUFFERNUM) || (key == FRONTBUFFERNUM) || (key==MOUSEBUFFERNUM) || (key==MOUSESAVENUM))
    return FALSE; // not possible

  return m_SurfMgr.ChangeSurfaceType(key, type);
}

// added to make conversion to CDX easier
//*****************************************************************************
//    NAME: Graphics::BlitImage
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::BlitImage(long dstSurf, long srcSurf, const RECT *srcRect, const RECT *dstRect)
{
//#ifdef WIN10Problem
//  if (WIN10ProblemActive)
//  WriteDebugString("Graphics::BlitImage(%d,%d, [%d, %d, %d, %d],[%d,%d,%d,%d]\n",
//           dstSurf, srcSurf, srcRect->left, srcRect->top, srcRect->right, srcRect->bottom,
//                             dstRect->left, dstRect->top, dstRect->right, dstRect->bottom);
//#endif
  return BlitImage(dstRect->left, dstRect->top, srcSurf, srcRect, FALSE, dstSurf, dstRect);
}
//*****************************************************************************
//    NAME: Graphics::BlitTransImage
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::BlitTransImage(long dstSurf, long srcSurf, const RECT *srcRect, const RECT *dstRect)
{
#ifdef WIN10Problem
  if (WIN10ProblemActive)
  WriteDebugString("Graphics::BlitTransImage(%d,%d, [%d, %d, %d, %d],[%d,%d,%d,%d]\n",
           dstSurf, srcSurf, srcRect->left, srcRect->top, srcRect->right, srcRect->bottom,
                             dstRect->left, dstRect->top, dstRect->right, dstRect->bottom);
#endif
  return BlitTransImage(dstRect->left, dstRect->top, srcSurf, srcRect, FALSE, dstSurf, dstRect);
}

#ifdef WIN10Problem
void DumpSurface(long surf, SurfacePtr pSurf)
{
  RGBFORMAT rgbf;
  pSurf->GetPixelFormat(&rgbf);
  WriteDebugString("******* Dump of CSXSurface number %d, 0x%08x\n", surf, (long)pSurf);
  WriteDebugString("m_PixelFormat=[%d,%d,%d],[%d,%d,%d],%d\n",
                      rgbf.depth.rgbBlue,
                      rgbf.depth.rgbGreen,
                      rgbf.depth.rgbRed,
                      rgbf.position.rgbBlue,
                      rgbf.position.rgbGreen,
                      rgbf.position.rgbRed,
                      rgbf.bpp
    );
}
#endif

//*****************************************************************************
//    NAME: Graphics::BlitImage
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::BlitImage(WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, BOOL flipY, long dstSurf, const RECT *dstRect)
{
  if (m_pScreen == NULL)
    return FALSE;

  //WriteDebugString("BlitImage srcrect %d %d %d %d\n",
  //                  srcRect->left,srcRect->top,srcRect->right,srcRect->bottom);
                    


#if (defined UAFEDITOR)
  // when running the editor, all blits are done
  // to the primary surface.
  ASSERT (dstSurf != BACKBUFFERNUM);    
#endif

//  CSingleLock sl(&m_cs, TRUE);
  HRESULT rc;
  RECT tmpSrcRect;
  LONG dstWidth=0, dstHeight=0;
  SurfaceType srcType;
  SurfacePtr pSrc = GetSurfacePtr(srcSurf, srcType);
  SurfacePtr pDst = GetSurfacePtr(dstSurf);

  if ((pSrc == NULL) || (pDst == NULL))
  {
    LOGERROR(m_pScreen, "Bogus params to BlitImage()\n");
    return FALSE;
  }

  if (srcRect == NULL)
  {
    // blitting entire src surface to dest surface
    // determine src rect
    tmpSrcRect.left = tmpSrcRect.top = 0;
    tmpSrcRect.right = pSrc->GetWidth();
    tmpSrcRect.bottom = pSrc->GetHeight();
  }
  else
    tmpSrcRect = *srcRect;    

  srcRect = &tmpSrcRect;

#ifdef WIN10Problem
  {
    RECT tRect;
    tRect.left=tRect.top=tRect.right=tRect.bottom = 0;
    if (dstRect != NULL)
    {
      tRect = *dstRect;
    };
    if (WIN10ProblemActive)
    WriteDebugString("Graphics::BlitImage(%d,%d, %d, [%d, %d, %d,%d],%d, %d, [%d,%d,%d,%d]\n",
           dstX, dstY, srcSurf, 
           srcRect->left, srcRect->top, srcRect->right, srcRect->bottom,
           flipY, dstSurf,
           tRect.left, tRect.top, tRect.right, tRect.bottom);
  };
#endif


  // In DirectX, stretching means that Blt() must be used rather than
  // BltFast(). However, when in windowed mode and a DirectX clipper is
  // used, Blt() must be used always. The clipper will not be consulted
  // for BltFast().
  //
  // So when compiling for the editor, where a clipper is required, setting
  // Stretch = TRUE forces a call to the CDX function which calls Blt(),
  // otherwise, set Stretch=TRUE only when source and destination RECTs
  // don't match.
  //
#ifdef UAFEngine
  BOOL Stretch = FALSE;
  if (dstRect != NULL)
  {
    if (   ((dstRect->right-dstRect->left) != (srcRect->right - srcRect->left))
        || ((dstRect->bottom-dstRect->top) != (srcRect->bottom - srcRect->top)))
      Stretch = TRUE;
  }
#else
  BOOL Stretch = TRUE;
#endif

  if (Stretch == TRUE)
  {
    if (dstRect == NULL)
    {
      dstWidth  = srcRect->right - srcRect->left;
      dstHeight = srcRect->bottom - srcRect->top;
    }
    else
    {
      dstWidth  = dstRect->right - dstRect->left;
      dstHeight = dstRect->bottom - dstRect->top;
    }
  }

  BYTE retries = 0;
#ifdef WIN10Problem
  if(WIN10ProblemActive)
    WriteDebugString("BlitImage - about to Draw\n");
#endif
  if (UseTransparency(srcType))
  {
#ifdef WIN10Problem
      if (WIN10ProblemActive)
           WriteDebugString("BlitImage - about to Draw using transparency\n");
#endif
    if (flipY) // only the engine uses flip (for posing combat icons)
    {
      ASSERT( Stretch == FALSE );
      // dd Blt
      while (((rc = pSrc->DrawTransHFlip(pDst, dstX, dstY, const_cast<RECT*>(srcRect))) == DDERR_SURFACELOST) && (retries < 3))
      {
        if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
        //m_pScreen->Restore();
        retries++;
      }

      if (FAILED(rc))
      {
        char *pErr = DXErrorToString(rc);
        LOGERROR(m_pScreen, pErr);
        return FALSE;
      }
    }
    else
    {
      if (Stretch)
      {
        // dd Blt
        while (((rc = pSrc->DrawTransStretched(pDst, dstX, dstY, const_cast<RECT*>(srcRect), 
                                      dstWidth, dstHeight)) == DDERR_SURFACELOST) && (retries < 3))
        {
          if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
          //m_pScreen->Restore();
          retries++;
        }
        if (FAILED(rc))
        {
          char *pErr = DXErrorToString(rc);
          LOGERROR(m_pScreen, pErr);
          return FALSE;
        }
      }
      else
      {
        // dd bltfast
        while (((rc = pSrc->DrawTrans(pDst, dstX, dstY, const_cast<RECT*>(srcRect))) == DDERR_SURFACELOST) && (retries < 3))
        {
          if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
          //m_pScreen->Restore();
          retries++;
        }
        if (FAILED(rc))
        {
          char *pErr = DXErrorToString(rc);
          LOGERROR(m_pScreen, pErr);
          return FALSE;
        }
      }
    }
  }
  else  // no transparency
  {
#ifdef WIN10Problem
      if (WIN10ProblemActive)
           WriteDebugString("BlitImage - about to Draw without transparency\n");
#endif
    if (flipY) // only the engine uses flip (for posing combat icons)
    {
#ifdef WIN10Problem
      if (WIN10ProblemActive)
           WriteDebugString("BlitImage - about to Draw without transparency using flipY\n");
#endif
      ASSERT( Stretch == FALSE );
      //  dd blt
      while (((rc = pSrc->DrawBlkHFlip(pDst, dstX, dstY, const_cast<RECT*>(srcRect))) == DDERR_SURFACELOST) && (retries < 3))
      {
        if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
        //m_pScreen->Restore();
        retries++;
      }

      if (FAILED(rc))
      {
        char *pErr = DXErrorToString(rc);
        LOGERROR(m_pScreen, pErr);
        return FALSE;
      }
    }
    else
    {
#ifdef WIN10Problem
      if (WIN10ProblemActive)
           WriteDebugString("BlitImage - about to Draw no transparency no flipY\n");
#endif
      if (Stretch)
      {
        // dd blt
#ifdef DebugFull
        WriteDebugString("!!BlitImage - Stretched\n");
#endif
#ifdef WIN10Problem
     //   if (WIN10ProblemActive)
     //        WriteDebugString("BlitImage - about to Draw no transparency no flipY with stretch\n");
#endif
        while (((rc = pSrc->DrawBlkStretched(pDst, dstX, dstY, const_cast<RECT*>(srcRect), 
                                    dstWidth, dstHeight)) == DDERR_SURFACELOST) && (retries < 3))
        {
          if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
          //pSrc->Restore();
          //pDst->Restore();
          //m_pScreen->Restore();
          retries++;
        }
        if (FAILED(rc))
        {
          char *pErr = DXErrorToString(rc);
          LOGERROR(m_pScreen, pErr);
          return FALSE;
        }
      }
      else
      {
        // dd bltfast
#ifdef DebugFull
        WriteDebugString("!!BlitImage - nonStretched\n");
#endif
#ifdef WIN10Problem
        if (WIN10ProblemActive)
        {
           WriteDebugString("BlitImage - about to DrawBlk no transparency no flipY no stretch\n");
           DumpSurface(dstSurf, pDst);
        };

#endif
        while (((rc = pSrc->DrawBlk(pDst, dstX, dstY, const_cast<RECT*>(srcRect))) == DDERR_SURFACELOST) 
               && (retries < 3))
        {
          HRESULT result;
          if (!CanRestoreDDSurfaces()) return FALSE;
          //if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
          result = pSrc->Restore();
#ifdef WIN10Problem
        if (WIN10ProblemActive)
             WriteDebugString("BlitImage - attempted to restore pSrc; result = %08x\n", result);
#endif
          result = pDst->Restore();
#ifdef WIN10Problem
        if (WIN10ProblemActive)
             WriteDebugString("BlitImage - attempted to restore pDst; result = %08x\n", result);
#endif
          result = m_pScreen->Restore();
#ifdef WIN10Problem
        if (WIN10ProblemActive)
             WriteDebugString("BlitImage - attempted to restore m_pScreen; result = %08x\n", result);
#endif
#ifdef WIN10Problem
        if (WIN10ProblemActive)
             WriteDebugString("BlitImage - DDERR_SURFACELOST - Repeating retries = %d\n", retries);
#endif
          retries++;
        }
        if (FAILED(rc))
        {
          char *pErr = DXErrorToString(rc);
          LOGERROR(m_pScreen, pErr);
          return FALSE;
        }
      }
    }
  }
#ifdef WIN10Problem
  if(WIN10ProblemActive)
    WriteDebugString("BlitImage - return from Draw\n");
#endif

  return TRUE;
}

// force trans color usage, rather than rely on surface type
//*****************************************************************************
//    NAME: Graphics::BlitTransImage
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::BlitTransImage(WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, BOOL flipY, long dstSurf, const RECT *dstRect)
{

#if (defined UAFEDITOR)
  // when running the editor, all blits are done
  // to the primary surface.
  ASSERT (dstSurf != BACKBUFFERNUM);    
#endif

  if (m_pScreen == NULL)
    return FALSE;


//  CSingleLock sl(&m_cs, TRUE);
  HRESULT rc;
  RECT tmpSrcRect;
  LONG dstWidth=0, dstHeight=0;
  SurfaceType srcType;
  SurfacePtr pSrc = GetSurfacePtr(srcSurf, srcType);
  SurfacePtr pDst = GetSurfacePtr(dstSurf);

  if ((pSrc == NULL) || (pDst == NULL))
  {
    LOGERROR(m_pScreen, "Bogus params to BlitTransImage()\n");
    return FALSE;
  }

  if (srcRect == NULL)
  {
    // blitting entire src surface to dest surface
    // determine src rect
    tmpSrcRect.left = tmpSrcRect.top = 0;
    tmpSrcRect.right = pSrc->GetWidth();
    tmpSrcRect.bottom = pSrc->GetHeight();
  }
  else
    tmpSrcRect = *srcRect;    

  srcRect = &tmpSrcRect;

  // In DirectX, stretching means that Blt() must be used rather than
  // BltFast(). However, when in windowed mode and a DirectX clipper is
  // used, Blt() must be used always. The clipper will not be consulted
  // for BltFast().
  //
  // So when compiling for the editor, where a clipper is required, setting
  // Stretch = TRUE forces a call to the CDX function which calls Blt(),
  // otherwise, set Stretch=TRUE only when source and destination RECTs
  // don't match.
  //
#ifdef UAFEngine
  BOOL Stretch = FALSE;
  if (dstRect != NULL)
  {
    if (   ((dstRect->right-dstRect->left) != (srcRect->right - srcRect->left))
        || ((dstRect->bottom-dstRect->top) != (srcRect->bottom - srcRect->top)))
      Stretch = TRUE;
  }
#else
  BOOL Stretch = TRUE;
#endif

  if (Stretch == TRUE)
  {
    if (dstRect == NULL)
    {
      dstWidth  = srcRect->right - srcRect->left;
      dstHeight = srcRect->bottom - srcRect->top;
    }
    else
    {
      dstWidth  = dstRect->right - dstRect->left;
      dstHeight = dstRect->bottom - dstRect->top;
    }
  }

  BYTE retries = 0;
  if (flipY) // only the engine uses flip (for posing combat icons)
  {
    ASSERT( Stretch == FALSE );
    // dd Blt
    while (((rc = pSrc->DrawTransHFlip(pDst, dstX, dstY, const_cast<RECT*>(srcRect))) == DDERR_SURFACELOST) && (retries < 3))
    {
      if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
      //pSrc->Restore();
      //pDst->Restore();
      //m_pScreen->Restore();
      retries++;
    }

    if (FAILED(rc))
    {
      char *pErr = DXErrorToString(rc);
      LOGERROR(m_pScreen, pErr);
      return FALSE;
    }
  }
  else
  {
    if (Stretch)
    {
      // dd Blt
      while (((rc = pSrc->DrawTransStretched(pDst, dstX, dstY, const_cast<RECT*>(srcRect), 
                                    dstWidth, dstHeight)) == DDERR_SURFACELOST) && (retries < 3))
      {
        if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
        //pSrc->Restore();
        //pDst->Restore();
        //m_pScreen->Restore();
        retries++;
      }
      if (FAILED(rc))
      {
        char *pErr = DXErrorToString(rc);
        LOGERROR(m_pScreen, pErr);
        return FALSE;
      }
    }
    else
    {
      // dd bltfast
      while (((rc = pSrc->DrawTrans(pDst, dstX, dstY, const_cast<RECT*>(srcRect))) == DDERR_SURFACELOST) && (retries < 3))
      {
        if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
        //pSrc->Restore();
        //pDst->Restore();
        //m_pScreen->Restore();
        retries++;
      };
      if (FAILED(rc))
      {
        char *pErr = DXErrorToString(rc);
        LOGERROR(m_pScreen, pErr);
        return FALSE;
      };
    };
  }
  return TRUE;
}

BOOL Graphics::DarkenDestSurface(WORD Shadow, const RECT *dstRect, long dstSurf)
{
  if (m_pScreen == NULL)
    return FALSE;

#if (defined UAFEDITOR)
  // when running the editor, all blits are done
  // to the primary surface.
  ASSERT (dstSurf != BACKBUFFERNUM);    
#endif

//  CSingleLock sl(&m_cs, TRUE);
  HRESULT rc;
  SurfacePtr pDst = GetSurfacePtr(dstSurf);

  if ((pDst == NULL) || (dstRect == NULL))
  {
    LOGERROR(m_pScreen, "Bogus params to BlitAlphaBlackImage()\n");
    return FALSE;
  }

  BYTE retries = 0;
  while (((rc = pDst->DrawBlkShadow(pDst, dstRect->left, dstRect->top, const_cast<RECT*>(dstRect), Shadow)) == DDERR_SURFACELOST) && (retries < 3))
  {
    if (!RestoreAll(m_pScreen, pDst, NULL)) 
      return FALSE;
    retries++;
  }

  if (FAILED(rc))
  {
    char *pErr = DXErrorToString(rc);
    LOGERROR(m_pScreen, pErr);
    return FALSE;
  }

  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::BlitAlphaBlackImage
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::BlitAlphaBlackImage(WORD Shad, WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, long dstSurf)
{
  if (m_pScreen == NULL)
    return FALSE;

#if (defined UAFEDITOR)
  // when running the editor, all blits are done
  // to the primary surface.
  ASSERT (dstSurf != BACKBUFFERNUM);    
#endif

//  CSingleLock sl(&m_cs, TRUE);
  HRESULT rc;
  RECT tmpSrcRect;
  SurfaceType srcType;
  SurfacePtr pSrc = GetSurfacePtr(srcSurf, srcType);
  SurfacePtr pDst = GetSurfacePtr(dstSurf);

  if ((pDst == NULL) || (pSrc == NULL))
  {
    LOGERROR(m_pScreen, "Bogus params to BlitAlphaBlackImage()\n");
    return FALSE;
  }

  if (srcRect == NULL) 
  {
    // blitting entire src surface to dest surface
    // determine src rect
    tmpSrcRect.left = tmpSrcRect.top = 0;
    tmpSrcRect.right = pSrc->GetWidth();
    tmpSrcRect.bottom = pSrc->GetHeight();
  }
  else
    tmpSrcRect = *srcRect;    

  srcRect = &tmpSrcRect;

  BYTE retries = 0;
  if (UseTransparency(srcType))
  {
    while (((rc = pSrc->DrawTransShadow(pDst, dstX, dstY, const_cast<RECT*>(srcRect), Shad)) == DDERR_SURFACELOST) && (retries < 3))
    {
      if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
      retries++;
    }
    if (FAILED(rc))
    {
      char *pErr = DXErrorToString(rc);
      LOGERROR(m_pScreen, pErr);
      return FALSE;
    }
  }
  else  // no transparency
  {
    while (((rc = pSrc->DrawBlkShadow(pDst, dstX, dstY, const_cast<RECT*>(srcRect), Shad)) == DDERR_SURFACELOST) && (retries < 3))
    {
      if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
      retries++;
    }
    if (FAILED(rc))
    {
      char *pErr = DXErrorToString(rc);
      LOGERROR(m_pScreen, pErr);
      return FALSE;
    }
  }

  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::BlitAlphaImage
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::BlitAlphaImage(WORD Alpha, WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, long dstSurf)
{
  DETAILTRACE(0x2ab110,0);
  if (m_pScreen == NULL)
    return FALSE;

#if (defined UAFEDITOR)
  // when running the editor, all blits are done
  // to the primary surface.
  ASSERT (dstSurf != BACKBUFFERNUM);    
#endif

//  CSingleLock sl(&m_cs, TRUE);
  HRESULT rc;
  RECT tmpSrcRect;
  SurfaceType srcType;
  DETAILTRACE(0x2ab111,0);
  SurfacePtr pSrc = GetSurfacePtr(srcSurf, srcType);
  //WriteDebugString("BlitAlphImage - dstSurf = %d\n", dstSurf);
  SurfacePtr pDst = GetSurfacePtr(dstSurf);

  if ((pSrc == NULL) || (pDst == NULL))
  {
    LOGERROR(m_pScreen, "Bogus params to BlitAlphaImage()\n");
    return FALSE;
  }

  DETAILTRACE(0x2ab112,0);
  if (srcRect == NULL)
  {
    // blitting entire src surface to dest surface
    // determine src rect
    tmpSrcRect.left = tmpSrcRect.top = 0;
    tmpSrcRect.right = pSrc->GetWidth();
    tmpSrcRect.bottom = pSrc->GetHeight();
  }
  else
    tmpSrcRect = *srcRect;    

  srcRect = &tmpSrcRect;

  BYTE retries = 0;
  DETAILTRACE(0x2ab113,0);
  if (UseTransparency(srcType))
  {    
    DETAILTRACE(0x2ab114,0);
    while (((rc = pSrc->DrawTransAlpha(pDst, dstX, dstY, const_cast<RECT*>(srcRect), Alpha)) == DDERR_SURFACELOST) && (retries < 3))
    {
      if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
      //pSrc->Restore();
      //pDst->Restore();
      //m_pScreen->Restore();
      retries++;
    }
    if (FAILED(rc))
    {
      char *pErr = DXErrorToString(rc);
      LOGERROR(m_pScreen, pErr);
      return FALSE;
    }
  }
  else  // no transparency
  {
    DETAILTRACE(0x2ab115,0);
    //WriteDebugString("pDst=0x%x; dstX=%d; dstY=%d; srcX=%d, srcY=%d, srcR=%d, srcB=%d; Alpha=%d\n",
    //                         pDst,
    //                         dstX,
    //                         dstY,
    //                         srcRect->left,
    //                         srcRect->top,
    //                         srcRect->right,
    //                         srcRect->bottom,
    //                         Alpha);
    while (((rc = pSrc->DrawBlkAlpha(pDst, dstX, dstY, const_cast<RECT*>(srcRect), Alpha)) == DDERR_SURFACELOST) && (retries < 3))
    {
      DETAILTRACE(0x2ab117,0);
      if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
      //pSrc->Restore();
      //pDst->Restore();
      //m_pScreen->Restore();
      retries++;
    }
    DETAILTRACE(0x2ab118,0);
    if (FAILED(rc))
    {
      DETAILTRACE(0x2ab119,0);
      char *pErr = DXErrorToString(rc);
      LOGERROR(m_pScreen, pErr);
      return FALSE;
    }
  }

  DETAILTRACE(0x2ab116,0);
  return TRUE;
}

// force trans color usage, rather than based on surface type
//*****************************************************************************
//    NAME: Graphics::BlitAlphaTransImage
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::BlitAlphaTransImage(WORD Alpha, WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, long dstSurf)
{
  if (m_pScreen == NULL)
    return FALSE;

#if (defined UAFEDITOR)
  // when running the editor, all blits are done
  // to the primary surface.
  ASSERT (dstSurf != BACKBUFFERNUM);    
#endif

//  CSingleLock sl(&m_cs, TRUE);
  HRESULT rc;
  RECT tmpSrcRect;
  SurfaceType srcType;
  SurfacePtr pSrc = GetSurfacePtr(srcSurf, srcType);
  SurfacePtr pDst = GetSurfacePtr(dstSurf);

  if ((pSrc == NULL) || (pDst == NULL))
  {
    LOGERROR(m_pScreen, "Bogus params to BlitAlphaTransImage()\n");
    return FALSE;
  }

  if (srcRect == NULL)
  {
    // blitting entire src surface to dest surface
    // determine src rect
    tmpSrcRect.left = tmpSrcRect.top = 0;
    tmpSrcRect.right = pSrc->GetWidth();
    tmpSrcRect.bottom = pSrc->GetHeight();
  }
  else
    tmpSrcRect = *srcRect;    

  srcRect = &tmpSrcRect;

  BYTE retries = 0;
  while (((rc = pSrc->DrawTransAlpha(pDst, dstX, dstY, const_cast<RECT*>(srcRect), Alpha)) == DDERR_SURFACELOST) && (retries < 3))
  {
    if (!RestoreAll(m_pScreen, pDst, pSrc)) return FALSE;
    //pSrc->Restore();
    //pDst->Restore();
    //m_pScreen->Restore();
    retries++;
  }
  if (FAILED(rc))
  {
    char *pErr = DXErrorToString(rc);
    LOGERROR(m_pScreen, pErr);
    return FALSE;
  }
  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::BlitAlphaTransSprite
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::BlitAlphaSprite(WORD Alpha, WORD dstX, WORD dstY, long srcSurf, long dstSurf)
{
  if (m_pScreen == NULL)
    return FALSE;
  
#if (defined UAFEDITOR)
  // when running the editor, all blits are done
  // to the primary surface.
  ASSERT (dstSurf != BACKBUFFERNUM);    
#endif
  
//  CSingleLock sl(&m_cs, TRUE);
  HRESULT rc;
  //RECT tmpSrcRect;
  SurfaceType srcType;
  
  //SpritePtr pSrc = GetSurfacePtr(srcSurf, srcType);
    
  SpritePtr pSrc=NULL;
  switch (srcSurf)
  {
  case MOUSEBUFFERNUM:
    pSrc = m_pMouseBuffer;
    srcType = TransBufferDib;
    break;
  default:
    pSrc = m_SpriteMgr.GetSurfacePtr(srcSurf, srcType);
    break;
  }

  SurfacePtr pDst = GetSurfacePtr(dstSurf);
  
  if ((pSrc == NULL) || (pDst == NULL))
  {
    LOGERROR(m_pScreen, "Bogus params to BlitAlphaSprite()\n");
    return FALSE;
  }
  
  // blitting entire src surface to dest surface
  // determine src rect
  //tmpSrcRect.left = tmpSrcRect.top = 0;
  //tmpSrcRect.right = pSrc->GetTile()->GetBlockWidth();
  //tmpSrcRect.bottom = pSrc->GetTile()->GetBlockHeight();  
  //const RECT *srcRect = &tmpSrcRect;
  
  BYTE retries = 0;
  
  pSrc->SetPos(dstX, dstY);
  pSrc->SetAlphaValue(Alpha);

  while (((rc = pSrc->Draw(pDst, 0,0, CDXBLT_TRANSALPHA)) == DDERR_SURFACELOST) && (retries < 3))
  {
    if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
    retries++;
  }
  if (FAILED(rc))
  {
    char *pErr = DXErrorToString(rc);
    LOGERROR(m_pScreen, pErr);
    return FALSE;
  }
  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::BlitSpriteImage
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::BlitSpriteImage(WORD dstX, WORD dstY, long srcSurf, const RECT *srcRect, BOOL flipY, long dstSurf, const RECT *dstRect)
{
  if (m_pScreen == NULL)
    return FALSE;

#if (defined UAFEDITOR)
  // when running the editor, all blits are done
  // to the primary surface.
  ASSERT (dstSurf != BACKBUFFERNUM);    
#endif

//  CSingleLock sl(&m_cs, TRUE);
  HRESULT rc;
  RECT tmpSrcRect;
  RECT tmpDstRect;
  SurfaceType srcType;
  SpritePtr pSrc=NULL;
  switch (srcSurf)
  {
  case MOUSEBUFFERNUM:
    pSrc = m_pMouseBuffer;
    srcType = TransBufferDib;
    break;
  default:
    pSrc = m_SpriteMgr.GetSurfacePtr(srcSurf, srcType);
    break;
  }

  if (pSrc!= NULL)
  {
    char name[256];
    if(pSrc->GetTile() !=  NULL)
    {
      if (pSrc->GetTile()->GetFileName(name,256) != NULL)
      {
      };
    };
  };


  SurfacePtr pDst = GetSurfacePtr(dstSurf);

  if ((pSrc == NULL) || (pDst == NULL))
  {
    LOGERROR(m_pScreen, "Bogus params to BlitSpriteImage()\n");
    return FALSE;
  }

  if (srcRect == NULL)
  {
    // blitting entire src surface to dest surface
    // determine src rect
    tmpSrcRect.left = tmpSrcRect.top = 0;
    tmpSrcRect.right = pSrc->GetTile()->GetWidth();
    tmpSrcRect.bottom = pSrc->GetTile()->GetHeight();
  }
  else
    tmpSrcRect = *srcRect;    

  srcRect = &tmpSrcRect;

  if (dstRect == NULL)
  {
    tmpDstRect.left = dstX;
    tmpDstRect.top = dstY;
    tmpDstRect.right = dstX+pSrc->GetTile()->GetWidth();
    tmpDstRect.bottom = dstY+pSrc->GetTile()->GetHeight();
  }
  else
    tmpDstRect = *dstRect;

  dstRect = &tmpDstRect;

#ifdef UAFEngine
  BOOL Stretch = FALSE;
  if (   ((dstRect->right-dstRect->left) != (srcRect->right - srcRect->left))
      || ((dstRect->bottom-dstRect->top) != (srcRect->bottom - srcRect->top)))
    Stretch = TRUE;
#else
  BOOL Stretch = TRUE;
#endif

  BYTE retries = 0;
  if (UseTransparency(srcType))
  {
    if (flipY)
    {
      ASSERT( Stretch == FALSE );
      while (((rc = pSrc->Blit(pDst, srcRect, dstRect, CDXBLT_TRANSHFLIP)) == DDERR_SURFACELOST) && (retries < 3))
      {
        if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
        //pDst->Restore();
       // m_pScreen->Restore();
        retries++;
      }

      if (FAILED(rc))
      {
        char *pErr = DXErrorToString(rc);
        LOGERROR(m_pScreen, pErr);
        return FALSE;
      }
    }
    else
    {
      if (Stretch)
      {
        while (((rc = pSrc->Blit(pDst, srcRect, dstRect, CDXBLT_TRANSSTRETCHED)) == DDERR_SURFACELOST) && (retries < 3))
        {
          if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
          //pDst->Restore();
          //m_pScreen->Restore();
          retries++;
        }
        
        if (FAILED(rc))
        {
          char *pErr = DXErrorToString(rc);
          LOGERROR(m_pScreen, pErr);
          return FALSE;
        }
      }
      else
      {
        while (((rc = pSrc->Blit(pDst,srcRect, dstRect, CDXBLT_TRANS)) == DDERR_SURFACELOST) && (retries < 3))
        {
          if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
          //pDst->Restore();
          //m_pScreen->Restore();
          retries++;
        }
        if (FAILED(rc))
        {
          char *pErr = DXErrorToString(rc);
          LOGERROR(m_pScreen, pErr);
          return FALSE;
        }
      }
    }
  }
  else  // no transparency
  {
    if (flipY)
    {
      ASSERT( Stretch == FALSE );
      while (((rc = pSrc->Blit(pDst, srcRect, dstRect, CDXBLT_BLKHFLIP)) == DDERR_SURFACELOST) && (retries < 3))
      {
        if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
        //pDst->Restore();
        //m_pScreen->Restore();
        retries++;
      }
      if (FAILED(rc))
      {
        char *pErr = DXErrorToString(rc);
        LOGERROR(m_pScreen, pErr);
        return FALSE;
      }
    }
    else
    {
      if (Stretch)
      {
        while (((rc = pSrc->Blit(pDst, srcRect, dstRect, CDXBLT_BLKSTRETCHED)) == DDERR_SURFACELOST) && (retries < 3))
        {
          if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
          //pDst->Restore();
          //m_pScreen->Restore();
          retries++;
        }
        if (FAILED(rc))
        {
          char *pErr = DXErrorToString(rc);
          LOGERROR(m_pScreen, pErr);
          return FALSE;
        }
      }
      else
      {
        while (((rc = pSrc->Blit(pDst, srcRect, dstRect, CDXBLT_BLK)) == DDERR_SURFACELOST) && (retries < 3))
        {
          if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
          //pDst->Restore();
          //m_pScreen->Restore();
          retries++;
        }
        if (FAILED(rc))
        {
          char *pErr = DXErrorToString(rc);
          LOGERROR(m_pScreen, pErr);
          return FALSE;
        }
      }
    }
  }
  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::BlitSprite
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::BlitSprite(WORD dstX, WORD dstY, long srcSurf, BOOL flipY, long dstSurf, const RECT *dstRect)
{
  if (m_pScreen == NULL)
    return FALSE;

#if (defined UAFEDITOR)
  // when running the editor, all blits are done
  // to the primary surface.
  ASSERT (dstSurf != BACKBUFFERNUM);    
#endif
//  CSingleLock sl(&m_cs, TRUE);

  HRESULT rc;
  RECT srcRect;
  LONG dstWidth=0, dstHeight=0;
  SurfaceType srcType;
  
  SpritePtr pSrc=NULL;
  switch (srcSurf)
  {
  case MOUSEBUFFERNUM:
    pSrc = m_pMouseBuffer;
    srcType = TransBufferDib;
    break;
  default:
    pSrc = m_SpriteMgr.GetSurfacePtr(srcSurf, srcType);
    break;
  }

  SurfacePtr pDst = GetSurfacePtr(dstSurf);

  if (pSrc!= NULL)
  {
    char name[256];
    if(pSrc->GetTile() !=  NULL)
    {
      if (pSrc->GetTile()->GetFileName(name,256) != NULL)
      {
      };
    };
  };

  if ((pSrc == NULL) || (pDst == NULL))
  {
    LOGERROR(m_pScreen, "Bogus params to BlitSprite()\n");
    return FALSE;
  }

  // blitting entire src surface to dest surface
  // determine src rect
  srcRect.left = srcRect.top = 0;
  srcRect.right = pSrc->GetTile()->GetBlockWidth();
  srcRect.bottom = pSrc->GetTile()->GetBlockHeight();

  // In DirectX, stretching means that Blt() must be used rather than
  // BltFast(). However, when in windowed mode and a DirectX clipper is
  // used, Blt() must be used always. The clipper will not be consulted
  // for BltFast().
  //
  // So when compiling for the editor, where a clipper is required, setting
  // Stretch = TRUE forces a call to the CDX function which calls Blt(),
  // otherwise, set Stretch=TRUE only when source and destination RECTs
  // don't match.
  //
#ifdef UAFEngine
  BOOL Stretch = FALSE;
  if (dstRect != NULL)
  {
    if (   ((dstRect->right-dstRect->left) != (srcRect.right - srcRect.left))
        || ((dstRect->bottom-dstRect->top) != (srcRect.bottom - srcRect.top)))
      Stretch = TRUE;
  }
#else
  BOOL Stretch = TRUE;
#endif

  if (Stretch == TRUE)
  {
    if (dstRect == NULL)
    {
      dstWidth  = srcRect.right - srcRect.left;
      dstHeight = srcRect.bottom - srcRect.top;
    }
    else
    {
      dstWidth  = dstRect->right - dstRect->left;
      dstHeight = dstRect->bottom - dstRect->top;
    }
  }

  BYTE retries = 0;
  if (UseTransparency(srcType))
  {
    if (flipY)
    {
      ASSERT( Stretch == FALSE );
      pSrc->SetPos(dstX, dstY);
      while (((rc = pSrc->Draw(pDst, 0,0, CDXBLT_TRANSHFLIP)) == DDERR_SURFACELOST) && (retries < 3))
      {
        if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
        //pDst->Restore();
        //m_pScreen->Restore();
        retries++;
      }

      if (FAILED(rc))
      {
        char *pErr = DXErrorToString(rc);
        LOGERROR(m_pScreen, pErr);
        return FALSE;
      }
    }
    else
    {
      if (Stretch)
      {
        pSrc->SetPos(dstX, dstY);
        pSrc->SetStretchWidth(dstWidth);
        pSrc->SetStretchHeight(dstHeight);
        while (((rc = pSrc->Draw(pDst, 0,0, CDXBLT_TRANSSTRETCHED)) == DDERR_SURFACELOST) && (retries < 3))
        {
          if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
          //pDst->Restore();
          //m_pScreen->Restore();
          retries++;
        }
        
        if (FAILED(rc))
        {
          char *pErr = DXErrorToString(rc);
          LOGERROR(m_pScreen, pErr);
          return FALSE;
        }
      }
      else
      {
        pSrc->SetPos(dstX, dstY);
        while (((rc = pSrc->Draw(pDst, 0,0, CDXBLT_TRANS)) == DDERR_SURFACELOST) && (retries < 3))
        {
          if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
          //pDst->Restore();
          //m_pScreen->Restore();
          retries++;
        }
        if (FAILED(rc))
        {
          char *pErr = DXErrorToString(rc);
          LOGERROR(m_pScreen, pErr);
          return FALSE;
        }
      }
    }
  }
  else  // no transparency
  {
    if (flipY)
    {
      ASSERT( Stretch == FALSE );
      pSrc->SetPos(dstX, dstY);
      while (((rc = pSrc->Draw(pDst, 0,0, CDXBLT_BLKHFLIP)) == DDERR_SURFACELOST) && (retries < 3))
      {
        if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
        //pDst->Restore();
        //m_pScreen->Restore();
        retries++;
      }
      if (FAILED(rc))
      {
        char *pErr = DXErrorToString(rc);
        LOGERROR(m_pScreen, pErr);
        return FALSE;
      }
    }
    else
    {
      if (Stretch)
      {
        pSrc->SetPos(dstX, dstY);
        pSrc->SetStretchWidth(dstWidth);
        pSrc->SetStretchHeight(dstHeight);
        while (((rc = pSrc->Draw(pDst, 0,0, CDXBLT_BLKSTRETCHED)) == DDERR_SURFACELOST) && (retries < 3))
        {
          if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
          //pDst->Restore();
          //m_pScreen->Restore();
          retries++;
        }
        if (FAILED(rc))
        {
          char *pErr = DXErrorToString(rc);
          LOGERROR(m_pScreen, pErr);
          return FALSE;
        }
      }
      else
      {
        pSrc->SetPos(dstX, dstY);
        while (((rc = pSrc->Draw(pDst, 0,0, CDXBLT_BLK)) == DDERR_SURFACELOST) && (retries < 3))
        {
          if (!RestoreAll(m_pScreen, pDst, NULL)) return FALSE;
          //pDst->Restore();
          //m_pScreen->Restore();
          retries++;
        }
        if (FAILED(rc))
        {
          char *pErr = DXErrorToString(rc);
          LOGERROR(m_pScreen, pErr);
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}

#ifdef UAFEDITOR

//*****************************************************************************
//    NAME: Graphics::FillRect
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::FillRect(long dstSurf, const RECT *dstRect, COLORREF rgb)
{
  if (m_pScreen == NULL)
    return FALSE;

  // when running the editor, all blits are done
  // to the primary surface.
  ASSERT (dstSurf != BACKBUFFERNUM);    
  //CSingleLock sl(&m_cs, TRUE);

  SurfacePtr pDst = GetSurfacePtr(dstSurf);

  if (pDst == NULL)
  {
    LOGERROR(m_pScreen, "Bogus params to FillRect()\n");
    return FALSE;
  }  

  HBRUSH hBrush = ::CreateSolidBrush(rgb);

  HDC hDC = pDst->GetDC();
  HBRUSH oldBrush = (HBRUSH)::SelectObject(hDC, hBrush);
  ::FillRect(hDC, dstRect, hBrush);
  ::SelectObject(hDC, oldBrush);
  pDst->ReleaseDC();
  ::DeleteObject(hBrush);
  
  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::FillRect
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::FillRect(long dstSurf, const RECT *dstRect, HBRUSH rgbBrush)
{
  if (m_pScreen == NULL)
    return FALSE;

  // when running the editor, all blits are done
  // to the primary surface.
  ASSERT (dstSurf != BACKBUFFERNUM);    

  SurfacePtr pDst = GetSurfacePtr(dstSurf);

  if (pDst == NULL)
  {
    LOGERROR(m_pScreen, "Bogus params to FillRect()\n");
    return FALSE;
  }  

  HDC hDC = pDst->GetDC();
  HBRUSH oldBrush = (HBRUSH)::SelectObject(hDC, rgbBrush);
  ::FillRect(hDC, dstRect, rgbBrush);
  ::SelectObject(hDC, oldBrush);
  pDst->ReleaseDC(); 
  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::FillRect
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::FillRect(const RECT *dstRect, HBRUSH rgbBrush, HDC hDC)
{
  if (m_pScreen == NULL)
    return FALSE;
  ::FillRect(hDC, dstRect, rgbBrush);
  return TRUE;
}

#endif

//*****************************************************************************
//    NAME: Graphics::GetSurfaceDC
//
// PURPOSE: 
//
//*****************************************************************************
HDC Graphics::GetSurfaceDC(long key)
{
  if (m_pScreen == NULL)
    return FALSE;

  SurfacePtr pDst = GetSurfacePtr(key);

  if (pDst == NULL)
  {
    LOGERROR(m_pScreen, "Bogus surf key to GetSurfaceDC()\n");
    return FALSE;
  }  

  return (pDst->GetDC());
}

//*****************************************************************************
//    NAME: Graphics::ReleaseSurfaceDC
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::ReleaseSurfaceDC(long key)
{
  if (m_pScreen == NULL)
    return;
  SurfacePtr pDst = GetSurfacePtr(key);
  if (pDst == NULL)
  {
    LOGERROR(m_pScreen, "Bogus surf key to ReleaseSurfaceDC()\n");
    return;
  }  
  pDst->ReleaseDC();
}

//*****************************************************************************
//    NAME: Graphics::GetColorBits
//
// PURPOSE: 
//
//*****************************************************************************
int Graphics::GetColorBits(SurfacePtr pSurf)
{
  if (pSurf==NULL) return 0;
  RGBFORMAT rgb;  
  pSurf->GetPixelFormat(&rgb);
  return rgb.bpp;
}

/*
SurfaceCacheMgr::SurfaceCacheMgr(DWORD maxsize, CString id)
{
  m_maxsize=maxsize;
  m_id = id;
  WriteDebugString("Init %s surface cache with size %u\n",m_id, maxsize);
}

SurfaceCacheMgr::~SurfaceCacheMgr() { Clear(); }

void SurfaceCacheMgr::Clear()
{
  m_SurfaceList.RemoveAll();
}


BOOL SurfaceCacheMgr::GetSurface(int width, int height, SurfacePtr& surf)
{
  SurfaceCacheData tmp;
  POSITION pos = m_SurfaceList.GetHeadPosition();
  while (pos!=NULL)
  {    
    tmp = m_SurfaceList.GetAt(pos);
    if (   (tmp.surface->GetWidth() == width)
        && (tmp.surface->GetHeight() == height))
    {
      surf = tmp.surface;
      m_SurfaceList.RemoveAt(pos);
      return TRUE;
    }
    m_SurfaceList.GetNext(pos);
  }
  return FALSE;
}

BOOL SurfaceCacheMgr::AddSurface(SurfacePtr& surf)
{
  if ((DWORD)m_SurfaceList.GetCount() >= m_maxsize)
    RemoveOldest();
  SurfaceCacheData tmp;
  tmp.surface = surf;
  tmp.insertTime=timeGetTime();
  m_SurfaceList.AddTail(tmp);
  return TRUE;
}

void SurfaceCacheMgr::RemoveOldest()
{
  DWORD oldestTime=0xFFFFFFFF;
  SurfacePtr oldestSurf=NULL;
  POSITION oldestPos=NULL;

  SurfaceCacheData tmp;
  POSITION pos = m_SurfaceList.GetHeadPosition();
  while (pos!=NULL)
  {    
    tmp = m_SurfaceList.GetAt(pos);
    if (tmp.insertTime < oldestTime)
    {
      oldestTime = tmp.insertTime;
      oldestPos = pos;
      oldestSurf=tmp.surface;
    }
    m_SurfaceList.GetNext(pos);
  }

  if ((oldestPos != NULL) && (oldestTime!=0xFFFFFFFF))
  {
    m_SurfaceList.RemoveAt(oldestPos);
    delete oldestSurf;
  }
}
*/

//*****************************************************************************
//    NAME: Graphics::SetClipperHwnd
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::SetClipperHwnd(HWND hWnd)
{
  if (m_pScreen == NULL) return FALSE;
  HRESULT result=DD_OK;    
  CDXSurface *pFrontSurf = m_pScreen->GetFront();
  if (pFrontSurf == NULL) return FALSE;
  
  CDX_LPDIRECTDRAWSURFACE pDDS;
  pDDS = pFrontSurf->GetDDS();
  if (pDDS == NULL) return FALSE;
  
  LPDIRECTDRAWCLIPPER lpddClipper;
  result = pDDS->GetClipper(&lpddClipper); 
  
  if (FAILED(result)) return FALSE; 
  if  (lpddClipper == NULL) return FALSE;
  
  // clear current window associated with clipper
  result = lpddClipper->SetHWnd(0, NULL);
  if (FAILED(result)) return FALSE; 
  
  result = lpddClipper->SetHWnd(0, hWnd);
  return (SUCCEEDED(result)); 
}

//*****************************************************************************
//    NAME: Graphics::RestoreClipperHwnd
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::RestoreClipperHwnd()
{
  if (m_pScreen == NULL) return FALSE;
  return (SetClipperHwnd((HWND)m_pScreen->GetWindowHandle()));
}

//*****************************************************************************
//    NAME: Graphics::RestoreAll
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::RestoreAll(CDXScreen *pDD, CDXSurface *pSurf1, CDXSurface *pSurf2)
{
  if (!CanRestoreDDSurfaces()) return FALSE;
  if (pSurf1!=NULL) pSurf1->Restore();
  if (pSurf2!=NULL) pSurf2->Restore();
  if (pDD!=NULL) pDD->Restore();
  return TRUE;
}

//*****************************************************************************
//    NAME: Graphics::CanRestoreDDSurfaces
//
// PURPOSE: 
//
//*****************************************************************************
BOOL Graphics::CanRestoreDDSurfaces()
{
  if (m_pScreen == NULL) return FALSE;
  //if (!bActive) return FALSE;

  LPDIRECTDRAW7 lpdd = m_pScreen->GetDD();
  if (lpdd==NULL) return FALSE;

  BOOL result = TRUE;
  HRESULT hr = lpdd->TestCooperativeLevel();

#ifdef UAFEDITOR
  if (hr == DDERR_EXCLUSIVEMODEALREADYSET)
    result = FALSE;
#else
  if (hr == DDERR_NOEXCLUSIVEMODE)
    result = FALSE;
#endif
  return result;
}

//*****************************************************************************
//    NAME: Graphics::DoScreenCapture
//
// PURPOSE: 
//
//*****************************************************************************
void Graphics::DoScreenCapture(const char *path)
{
  if (m_pScreen == NULL) return;
  if (path==NULL) return;
  if (strlen(path) <= 7) return; // must have at lease "C:\a.bmp"
  CDXSurface *pFrontSurf = m_pScreen->GetFront();
  if (pFrontSurf == NULL) return;
  pFrontSurf->SaveAsBMP(path);
}