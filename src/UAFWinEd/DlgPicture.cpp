/******************************************************************************
* Filename: DlgPicture.cpp
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
#include "resource.h"

#include "UAFWinEd.h"
#include "SurfaceMgr.h"
#include "class.h"
#include "PicSlot.h"
#include "PicData.h"
#include "Viewport.h"
#include "Graphics.h"
#include "DlgPicture.h"
#include "AccessDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDlgPicture

CDlgPicture::CDlgPicture() : CStatic()
{
   m_PicDib = -1;
   m_SrcDib = -1;
   m_SrcRect = CRect(0,0,0,0);
   m_PicRect = CRect(0,0,0,0);
   m_Color = RGB(0,0,0);
   
   m_box.m_nHandleSize = 4; 
   m_box.m_nStyle = CRectTracker::hatchedBorder;
   m_UseTracker = FALSE;
}

CDlgPicture::~CDlgPicture()
{
   GraphicsMgr.ReleaseSurface(m_PicDib);
   GraphicsMgr.ReleaseSurface(m_SrcDib);
}


BEGIN_MESSAGE_MAP(CDlgPicture, CStatic)
//{{AFX_MSG_MAP(CDlgPicture)
ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CDlgPicture::ShouldDrawDib(void)
{
  // blank pictures won't have a valid src pic
  // but they still need to have empty windows
  // drawn
  //return (GraphicsMgr.ValidSurface(m_PicDib));

  return (m_SrcRect.Height() > 0);
}

// shrinks dest rect down to size of src rect
// so that src is not  stretched bigger to fit
// dest m_PicDib
void CDlgPicture::DeflateDestRect(CRect &dst, CRect &src)
{
   int h = dst.Height() - src.Height();
   if (h > 0)
      dst.DeflateRect(0,0,0,h);
   
   int w = dst.Width() - src.Width();
   if (w > 0)
      dst.DeflateRect(0,0,w,0);
}

// pDC is dc of CStatic window.
// Fills dc with m_Color, and activates
// CRectTracker if needed.
void CDlgPicture::PrepareDestDC(CDC *pDc)
{
   CRect rect;
   GetClientRect(&rect);
   pDc->FillSolidRect(&rect, m_Color);
   
   if (m_UseTracker)
   {
      GetWindowRect(&rect);
      ScreenToClient(&rect);
      m_box.m_rect = rect;
      m_box.Draw(pDc);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CDlgPicture message handlers

//
// draws m_PicDib to CStatic window
//
// all classes derived from DlgPicture share this
// same OnPaint() function. Each derivation provides
// it's own RenderDib().
void CDlgPicture::OnPaint() 
{
   CRect screenDest, clientDest;
        
   if (m_PicDib <= 0)
      CreateBlankPicture(); 

   // clear dest window, and draw tracking box if needed
   CPaintDC dc(this); // device context for painting
   PrepareDestDC(&dc);

   // If m_PicDib will not be drawn,
   // validate CStatic window to answer WM_PAINT msg.
   if (!ShouldDrawDib())
   {
      ValidateRect(NULL);
      //TRACE("NOT drawing CDlgPicture(ShouldDraw=FALSE) in OnPaint\n");
      return;
   }

   // get the rect of the destination client window
   GetClientRect(&clientDest);
      
   // render m_SrcDib to m_PicDib surface
   // before m_PicDib is drawn to CStatic's DC
   // 
   // Any mismatch in size between the m_SrcDib and
   // m_PicDib will be taken care of here. When the
   // m_PicDib is blitted to the screen, all of it 
   // will be blitted regardless of size.
   //
   RenderDib(clientDest, m_PicDib);
   // get the screen coords of destination window
   GetWindowRect(&screenDest); 
   if (screenDest.top < 0) screenDest.top = 0;
   if (screenDest.left < 0) screenDest.left = 0;

   // blit whatever is in m_PicDib surface to the screen
   // at the given dest rect

    CWnd *pFrame = GetParent();
    HWND hWnd = NULL;

    if (pFrame != NULL)
      hWnd = pFrame->m_hWnd;
    else
      hWnd = this->m_hWnd;

   GraphicsMgr.SetClipperHwnd(hWnd);
   GraphicsMgr.BlitImage(screenDest.left, screenDest.top, m_PicDib, &m_PicRect, 
                         FALSE, FRONTBUFFERNUM, &screenDest);
   GraphicsMgr.RestoreClipperHwnd();     
   // Do not call CStatic::OnPaint() for painting messages
}
// draw src dib to m_PicDib before
// it is drawn to CStatic window.
// dstRect = rect of current m_PicDib bitmap.
void CDlgPicture::RenderDib(CRect &dstRect, long dstDib)
{  
   GraphicsMgr.ClearSurface(dstDib);

   // if source bitmap is smaller than the destination rect,
   // shrink destination rect to match source rect so that
   // the bitmap will not be stretched by the blit to fit the
   // destination rect.
   //
   // if source bitmap is larger than the destination rect,
   // the source will be shrunk by the blit to fit the
   // destination rect.
   //
   
   if (GraphicsMgr.ValidSurface(m_SrcDib))
   {
      DeflateDestRect(dstRect, m_SrcRect);
      GraphicsMgr.BlitImage(dstRect.left, dstRect.top, m_SrcDib, &m_SrcRect, FALSE, dstDib, &dstRect);
      //GraphicsMgr.BlitImage(dstDib, m_SrcDib, &m_SrcRect, &dstRect);
   }
}

BOOL CDlgPicture::LoadPicture(CString filename, int type, const CString& path1, const CString& path2)
{
   if (m_PicDib <= 0)
      CreateBlankPicture();
   
   GraphicsMgr.ReleaseSurface(m_SrcDib);
   m_SrcRect = CRect(0,0,0,0);
   
   Invalidate(FALSE);

   if (filename.IsEmpty())
      return TRUE;     
#ifdef pngcrash
   if (filename == "E:\\UAF\\Outstanding\\170829_AVI_CtD_MINI-MOD\\BG_TEST_16colours.png")
   {
     int kkk=1;
   };
#endif
   m_SrcDib = GraphicsMgr.AddFileSurface(filename, (SurfaceType)type, path1, path2);
   GraphicsMgr.GetSurfaceRect(m_SrcDib, &m_SrcRect);

   return (GraphicsMgr.ValidSurface(m_SrcDib));
}

void CDlgPicture::GetStats(int &w, int &h, int &bits)
{
   GraphicsMgr.GetSurfaceStats(m_SrcDib, w, h, bits);
}

// Do not call from constructors,
// client window must already exist
// for this function to succeed.
long int CDlgPicture::CreateBlankPicture(void)
{
   CRect rect;
   GetClientRect(&rect);
   m_SrcRect = CRect(0,0,0,0); //0,0,ea,cd
   GraphicsMgr.ReleaseSurface(m_PicDib);
   m_PicDib = GraphicsMgr.AddSurface(BufferDib, &rect);
   GraphicsMgr.GetSurfaceRect(m_PicDib, &m_PicRect);
   
   if (m_PicDib <= 0)
   {
      WriteDebugString("Failed to create blank picture\n");
      ASSERT(FALSE);
   }
   //else
   //   WriteDebugString("CDlgPic::CreateBlankPic create surface %i\n", m_PicDib);
   
   Invalidate(FALSE);
   GraphicsMgr.ClearSurface(m_PicDib);
   return m_PicDib;
}

long int CDlgPicture::ErasePicture(void)
{
   GraphicsMgr.ReleaseSurface(m_SrcDib);
   return (CreateBlankPicture());
}

void CDlgPicture::Highlight(BOOL activate)
{
  m_UseTracker = activate;
  Invalidate(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgWallPicture message handlers

BEGIN_MESSAGE_MAP(CDlgWallPicture, CDlgPicture)
//{{AFX_MSG_MAP(CDlgWallPicture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDlgWallPicture::CDlgWallPicture(void) : CDlgPicture()
{
   m_WallDib = -1;
   m_DoorDib = -1;
   m_OverlayDib = -1;
   m_format = DEFAULT_FORMAT_TYPE;
   m_doorFirst = TRUE;
}

CDlgWallPicture::~CDlgWallPicture(void)
{ 
   m_format = DEFAULT_FORMAT_TYPE;
   GraphicsMgr.ReleaseSurface(m_DoorDib);
   GraphicsMgr.ReleaseSurface(m_OverlayDib);
   GraphicsMgr.ReleaseSurface(m_WallDib);
}

long int CDlgWallPicture::ErasePicture(void)
{
   m_format = DEFAULT_FORMAT_TYPE;
   GraphicsMgr.ReleaseSurface(m_DoorDib);
   GraphicsMgr.ReleaseSurface(m_OverlayDib);
   GraphicsMgr.ReleaseSurface(m_WallDib);

   return (CDlgPicture::ErasePicture());
}

BOOL CDlgWallPicture::LoadSourcePictures(CString &door, CString &overlay, CString &wall, BOOL doorFirst)
{
  if (m_PicDib <= 0)
     CreateBlankPicture(); 

  m_doorFirst=doorFirst;
  Invalidate(FALSE);
  GraphicsMgr.ReleaseSurface(m_WallDib);
  GraphicsMgr.ReleaseSurface(m_DoorDib);
  GraphicsMgr.ReleaseSurface(m_OverlayDib);

  RECT rect;
  int tmpFormat = -1;

  // all images in this slot must conform to the same format.
  //
  // let the wall image decide the format if it is loaded, else
  // let the door then the overlay choose it.
  //
  // if no images are loaded, the use the default format.
  //
  // if the wall and door are loaded, the wall picks the format
  // and the door must conform or it will not display properly.
  //
  if (!wall.IsEmpty())    
  {
#ifdef UAFEDITOR
    m_WallDib = GraphicsMgr.AddFileSurface(wall, "_thumb", WallDib, rte.WallArtDir(), ede.TemplateWallArtDir());
    if (m_WallDib < 0)
#endif
    {
      m_WallDib = GraphicsMgr.AddFileSurface(wall, WallDib, rte.WallArtDir(), ede.TemplateWallArtDir());
    };
    m_wall = wall;     
    GraphicsMgr.GetSurfaceRect(m_WallDib,&rect);
    tmpFormat = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
  }
   
  if (!door.IsEmpty())
  {
#ifdef UAFEDITOR
    m_DoorDib = GraphicsMgr.AddFileSurface(door, "_thumb", DoorDib, rte.DoorArtDir(), ede.TemplateDoorArtDir());    
    if (m_DoorDib < 0)
#endif
    {
      m_DoorDib = GraphicsMgr.AddFileSurface(door, DoorDib, rte.DoorArtDir(), ede.TemplateDoorArtDir());    
    };
    m_door = door;
    if (tmpFormat == -1)
    {
      GraphicsMgr.GetSurfaceRect(m_WallDib,&rect);
      tmpFormat = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
    }
  }
   
  if (!overlay.IsEmpty())
  {
#ifdef UAFEDITOR
    m_OverlayDib = GraphicsMgr.AddFileSurface(overlay, "_thumb", OverlayDib, rte.OverlayArtDir(), ede.TemplateOverlandArtDir());   
    if (m_OverlayDib < 0)
#endif
    {
      m_OverlayDib = GraphicsMgr.AddFileSurface(overlay, OverlayDib, rte.OverlayArtDir(), ede.TemplateOverlandArtDir());   
    };
    m_overlay = overlay;
    if (tmpFormat == -1)
    {
      GraphicsMgr.GetSurfaceRect(m_WallDib,&rect);
      tmpFormat = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
    }
  }  

  // if no images loaded, use default
  if (tmpFormat == -1) tmpFormat = DEFAULT_FORMAT_TYPE;
  m_format = tmpFormat;
  return ( (GraphicsMgr.ValidSurface(m_WallDib))||(GraphicsMgr.ValidSurface(m_DoorDib))||(GraphicsMgr.ValidSurface(m_OverlayDib)) );
}

BOOL CDlgWallPicture::ShouldDrawDib(void)
{  
   BOOL result = FALSE;  
   result |= GraphicsMgr.ValidSurface(m_WallDib);
   result |= GraphicsMgr.ValidSurface(m_DoorDib);
   result |= GraphicsMgr.ValidSurface(m_OverlayDib);  
   return result;
}

// draw wall, door, and overlay CDibs to m_PicDib before
// it is drawn to CStatic window.
// dstRect = rect of current m_PicDib bitmap.
void CDlgWallPicture::RenderDib(CRect &dstRect, long dib)
{
   RECT srcRect, destRect;
   
   // adjust source rect to blit only H_WALL_RECT Rect portion of source bitmap
   srcRect = WallFormatMgr.GetRect(m_format, DRAW_H_WALL);
   destRect.top = destRect.left = 0;
   destRect.right = srcRect.right-srcRect.left;
   destRect.bottom = srcRect.bottom-srcRect.top;
   
   GraphicsMgr.ClearSurface(dib);

   if (GraphicsMgr.ValidSurface(m_WallDib))   
     GraphicsMgr.BlitImage(destRect.left, destRect.top, m_WallDib, &srcRect, FALSE, dib, &destRect);
   
   if (!m_doorFirst)
   {
     if (GraphicsMgr.ValidSurface(m_OverlayDib))
       GraphicsMgr.BlitImage(destRect.left, destRect.top, m_OverlayDib, &srcRect, FALSE, dib, &destRect);   
     if (GraphicsMgr.ValidSurface(m_DoorDib))
       GraphicsMgr.BlitImage(destRect.left, destRect.top, m_DoorDib, &srcRect, FALSE, dib, &destRect);
   }
   else
   {
     if (GraphicsMgr.ValidSurface(m_DoorDib))
       GraphicsMgr.BlitImage(destRect.left, destRect.top, m_DoorDib, &srcRect, FALSE, dib, &destRect);
     if (GraphicsMgr.ValidSurface(m_OverlayDib))
       GraphicsMgr.BlitImage(destRect.left, destRect.top, m_OverlayDib, &srcRect, FALSE, dib, &destRect);   
   }
}

/////////////////////////////////////////////////////////////////////////////
// C3DSelectPicture message handlers

C3DSelectPicture::C3DSelectPicture(void)  : CDlgPicture()
{
   m_srcDib = -1;
   m_format = DEFAULT_FORMAT_TYPE;
}

C3DSelectPicture::~C3DSelectPicture(void)
{ 
   GraphicsMgr.ReleaseSurface(m_srcDib);
}

BOOL C3DSelectPicture::ShouldDrawDib(void)
{  
   return (GraphicsMgr.ValidSurface(m_srcDib));
}

// draw CDib to m_PicDib before
// it is drawn to CStatic window.
// dstRect = rect of current m_PicDib bitmap.
void C3DSelectPicture::RenderDib(CRect &dstRect, long dib)
{
   RECT srcRect, destRect;
   
   if (GraphicsMgr.ValidSurface(m_srcDib))
   {
      // adjust source rect to blit only E_WALL_RECT Rect portion of source bitmap
      srcRect = WallFormatMgr.GetRect(m_format, DRAW_E_WALL);

      destRect.top = destRect.left = 0;
      destRect.right = srcRect.right-srcRect.left;
      destRect.bottom = srcRect.bottom-srcRect.top;
      
      GraphicsMgr.ClearSurface(dib); 
      GraphicsMgr.BlitImage(destRect.left, destRect.top, m_srcDib, &srcRect, FALSE, dib, &destRect);
   }
}

BOOL C3DSelectPicture::LoadSourcePicture(CString &dibFile, int type)
{
   if (m_PicDib <= 0)
      CreateBlankPicture(); 

   Invalidate(FALSE);
   GraphicsMgr.ReleaseSurface(m_srcDib);
   m_format = DEFAULT_FORMAT_TYPE;

   if (!dibFile.IsEmpty())
   {            
      m_srcDib = GraphicsMgr.AddFileSurface(dibFile, (SurfaceType)type, CString(""), CString(""));      
      m_srcFile = dibFile;
      RECT rect;
      GraphicsMgr.GetSurfaceRect(m_srcDib,&rect);
      m_format = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
   }
      
   return (GraphicsMgr.ValidSurface(m_srcDib));
}

BEGIN_MESSAGE_MAP(C3DSelectPicture, CDlgPicture)
//{{AFX_MSG_MAP(C3DSelectPicture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPicture3D

CDlgPicture3D::CDlgPicture3D() : CDlgPicture()
{
   m_bkgnd = "";
   m_door = "";
   m_overlay = "";
   m_wall = "";
   m_BkGndDib=-1;
   m_DoorDib=-1;
   m_OverlayDib=-1;
   m_WallDib=-1;
}

CDlgPicture3D::~CDlgPicture3D()
{
   GraphicsMgr.ReleaseSurface(m_BkGndDib);
   GraphicsMgr.ReleaseSurface(m_DoorDib);
   GraphicsMgr.ReleaseSurface(m_OverlayDib);
   GraphicsMgr.ReleaseSurface(m_WallDib);
}


BEGIN_MESSAGE_MAP(CDlgPicture3D, CDlgPicture)
//{{AFX_MSG_MAP(CDlgPicture3D)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgPicture3D::LoadSourcePictures(CString bkgnd,CString door,CString overlay,CString wall)
{
  GraphicsMgr.ReleaseSurface(m_BkGndDib);
  GraphicsMgr.ReleaseSurface(m_DoorDib);
  GraphicsMgr.ReleaseSurface(m_OverlayDib);
  GraphicsMgr.ReleaseSurface(m_WallDib);
    
  m_BkGndDib = GraphicsMgr.AddFileSurface(bkgnd, BackGndDib, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir());  
  m_bkgnd = bkgnd;
 
  m_DoorDib = GraphicsMgr.AddFileSurface(door, DoorDib, rte.DoorArtDir(), ede.TemplateDoorArtDir());
  m_door = door;
 
  m_OverlayDib = GraphicsMgr.AddFileSurface(overlay, OverlayDib, rte.OverlayArtDir(), ede.TemplateOverlandArtDir());
  m_overlay = overlay;
 
  m_WallDib = GraphicsMgr.AddFileSurface(wall, WallDib, rte.WallArtDir(), ede.TemplateWallArtDir());
  m_wall = wall;
      
  return ( (GraphicsMgr.ValidSurface(m_WallDib))||(GraphicsMgr.ValidSurface(m_DoorDib))||(GraphicsMgr.ValidSurface(m_OverlayDib))||(GraphicsMgr.ValidSurface(m_BkGndDib)) );
}

void CDlgPicture3D::RenderDib(CRect &dstRect,long int pDib)
{
   GraphicsMgr.ClearSurface(pDib);
   ValidateRect(NULL);
}

BOOL CDlgPicture3D::ShouldDrawDib(void)
{
   if (!CDlgPicture::ShouldDrawDib())
      return FALSE;
     
   return (   (GraphicsMgr.ValidSurface(m_WallDib))
           || (GraphicsMgr.ValidSurface(m_DoorDib))
           || (GraphicsMgr.ValidSurface(m_BkGndDib))
           || (GraphicsMgr.ValidSurface(m_OverlayDib)));
}

/////////////////////////////////////////////////////////////////////////////
// CDlgPicture3D message handlers


/////////////////////////////////////////////////////////////////////////////
// CDlgSmallPicture

CDlgSmallPicture::CDlgSmallPicture() : CDlgPicture()
{
   m_SmallPic.Clear();
}

CDlgSmallPicture::~CDlgSmallPicture()
{
   m_SmallPic.FreePic();
}


BEGIN_MESSAGE_MAP(CDlgSmallPicture, CDlgPicture)
//{{AFX_MSG_MAP(CDlgSmallPicture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgSmallPicture::LoadSourcePictures(PIC_DATA& pic)
{
   m_SmallPic.FreePic();
   pic.FreePic();
   m_SmallPic = pic;
   
   BOOL success = m_SmallPic.LoadPicSurfaces("");

   if (success)
   {
     m_SmallPic.pWnd = this;
     GraphicsMgr.GetSpriteRect(m_SmallPic.key, &m_SrcRect);
     Invalidate(FALSE);

     SetCycleInterval(m_SmallPic.timeDelay);
   }

   return (GraphicsMgr.ValidSprite(m_SmallPic.key));
}

void CDlgSmallPicture::RenderDib(CRect &dstRect,long int dib)
{
   GraphicsMgr.ClearSurface(dib);

   // if source bitmap is smaller than the destination rect,
   // shrink destination rect to match source rect so that
   // the bitmap will not be stretched by the blit to fit the
   // destination rect.
   //
   // if source bitmap is larger than the destination rect,
   // the source will be shrunk by the blit to fit the
   // destination rect.
   //
   DeflateDestRect(dstRect, m_SrcRect);

   if (GraphicsMgr.ValidSprite(m_SmallPic.key))
     GraphicsMgr.BlitSprite(dstRect.left, dstRect.top, m_SmallPic.key, FALSE, dib, &dstRect);
}

BOOL CDlgSmallPicture::ShouldDrawDib(void)
{
   if (m_SmallPic.key >= 0)
      return TRUE;

   return FALSE;
}

void CDlgSmallPicture::SetCycleInterval(int msecs)
{
  m_SmallPic.SetPicTimeDelay(msecs);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgPartialPicture

CDlgPartialPicture::CDlgPartialPicture() : CDlgPicture()
{
   m_VisibleRect = CRect(0,0,0,0);
}

CDlgPartialPicture::~CDlgPartialPicture()
{

}


BEGIN_MESSAGE_MAP(CDlgPartialPicture, CDlgPicture)
//{{AFX_MSG_MAP(CDlgPartialPicture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgPartialPicture::RenderDib(CRect &dstRect,long dib)
{
   GraphicsMgr.ClearSurface(dib);

   // if source bitmap is smaller than the destination rect,
   // shrink destination rect to match source rect so that
   // the bitmap will not be stretched by the blit to fit the
   // destination rect.
   //
   // if source bitmap is larger than the destination rect,
   // the source will be shrunk by the blit to fit the
   // destination rect.
   //
   DeflateDestRect(dstRect, m_VisibleRect);

   // blit only selected portion of m_SrcDib to dib
   //
   GraphicsMgr.BlitImage(dstRect.left, dstRect.top, m_SrcDib, &m_VisibleRect, FALSE, dib, &dstRect);
}

BOOL CDlgPartialPicture::LoadPicture(CString filename, const CString& path1, const CString& path2)
{
  return LoadPicture(filename, BufferDib, path1, path2);
}

BOOL CDlgPartialPicture::LoadPicture(CString filename, SurfaceType type, const CString& path1, const CString& path2)
{
   if (m_PicDib < 0)
     CreateBlankPicture();

   GraphicsMgr.ReleaseSurface(m_SrcDib);
   m_SrcRect = CRect(0,0,0,0);

   if (filename.IsEmpty())
      return TRUE;

   m_SrcDib = GraphicsMgr.AddFileSurface(filename, type, path1, path2);
   GraphicsMgr.GetSurfaceRect(m_SrcDib, &m_SrcRect);
   Invalidate(FALSE);
   return (GraphicsMgr.ValidSurface(m_SrcDib));
}

void CDlgPartialPicture::SetVisible(int x, int y, CRect& bufferRect)
{
   CRect visible = m_SrcRect; // default is all src visible

   int bwidth = bufferRect.Width();
   int bheight = bufferRect.Height();

   // if src is wider than the dest buffer
   if (m_SrcRect.Width() > bwidth)
   {
      if (x >= 0)
      {
        if (x <= (m_SrcRect.right-bwidth+1))
           visible.left = x;
        else
          visible.left = m_SrcRect.right-bwidth+1;
      }

      visible.right = visible.left + bwidth;

      if (visible.right > m_SrcRect.right)
         visible.right = m_SrcRect.right;
   }

   // if src is taller than the dest buffer
   if (m_SrcRect.Height() > bheight)
   {
      if (y >= 0) 
      {
        if (y <= (m_SrcRect.bottom-bheight+1))
          visible.top = y;
        else
          visible.top = m_SrcRect.bottom-bheight+1;
      }

      visible.bottom = visible.top + bheight;

      if (visible.bottom > m_SrcRect.bottom)
         visible.bottom = m_SrcRect.bottom;
   }

   m_VisibleRect = visible; 
}

/////////////////////////////////////////////////////////////////////////////
// CDlgPartialPicture message handlers



/////////////////////////////////////////////////////////////////////////////
// CDlgPartialMapPicture

//RECT = left,top,right,bottom
RECT H_WallSize = {0,0,4,2};            // Can be overridden by config.txt
RECT HO_WallSize = {0,0,6,2};           // Can be overridden by config.txt  
RECT V_WallSize = {0,0,2,4};            // Can be overridden by config.txt
RECT VO_WallSize = {0,0,2,6};           // Can be overridden by config.txt
RECT MapSquareSize = {0,0,16,16};       // Can be overridden by config.txt
RECT MapBlockSize = {0,0,6,6};          // Can be overridden by config.txt

int EDITOR_TIMER_CYCLE = 250; // Cycle that animation timer runs on.  Background animation won't be able to render any faster than this

// for overland.
// zone and ep color blocks have the same coords as for dungeons,
// arrow rects are also the same.
RECT BlockMapSquareRect = {0,0,16,16};
RECT EventMapSquareRect = {26,0,42,16};


int GetWallRectIndex(int slot)
{
  return slot;
}

int GetBackdropRectIndex(int slot)
{
  return slot;
}

int GetObstructionRectIndex(int slot)
{
  switch (slot)
  {
  case 0: return 0; break;
  case 1: return 1; break;
  case 2: return 14; break;
  case 3: return 15; break;
  case 4: return 2; break;
  case 5: return 3; break;
  case 6: return 4; break;
  case 7: return 5; break;
  case 8: return 6; break;
  case 9: return 7; break;
  case 10: return 8; break;
  case 11: return 9; break;
  case 12: return 10; break;
  case 13: return 11; break;
  case 14: return 12; break;
  case 15: return 13; break;
  }
  return 0;
}

int GetZoneRectIndex(int slot)
{
  return slot;
}

int GetEPRectIndex(int slot)
{
  return slot;
}

int GetAccessRectIndex(int access)
{
  static int RGBRectIndex[8] =  // bit 2=r, bit 1=g, bit 0=b
  {0,9,10,11,4,13,14,15};
  return RGBRectIndex[access];
};

// For each destination block within each square,
// add these offsets to the top,left of 
// the square getting blitted
// ---->>>> These can be overridden by config.txt. <<<<----
RECT WallOffsetRects[17] = {
  // North
  {2,0,6,2}, // 0
  {6,0,10,2}, // 1
  {10,0,14,2}, // 2
  {5,2,11,4}, // 3

  // South
  {2,14,6,16}, // 4
  {6,14,10,16}, // 5
  {10,14,14,16}, // 6
  {5,12,11,14}, // 7

  // East
  {14,2,16,6}, // 8
  {14,6,16,10}, // 9
  {14,10,16,14}, // 10
  {12,5,14,11}, // 11

  //West
  {0,2,2,6}, // 12
  {0,6,2,10}, // 13
  {0,10,2,14}, // 14
  {2,5,4,11},  // 15

  // Center
  {4,4,10,10} // 16
};

CDlgPartialMapPicture::CDlgPartialMapPicture() : CDlgPartialPicture()
{
   m_CurrX = 0;
   m_CurrY = 0;
   m_CellWidth = 0;
   m_CellHeight = 0;
   bgLoaded=FALSE;
   m_ArtDib=-1;
   m_ArtRect = CRect(0,0,0,0);
}

CDlgPartialMapPicture::~CDlgPartialMapPicture()
{
}


BEGIN_MESSAGE_MAP(CDlgPartialMapPicture, CDlgPicture)
//{{AFX_MSG_MAP(CDlgPartialMapPicture)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgPartialMapPicture::RenderDib(CRect &dstRect,long dib)
{
  static BOOL occupied, startLoc;
  static int EP, screenX,screenY, mapX, mapY, levelX, levelY;
  static int MaxHeight=0;
  static int MaxWidth=0;
  static RECT tempRect;
  static RECT cornerRect = {0,0,2,2};

  BYTE MAPSQUAREWIDTH =  MapSquareSize.right-MapSquareSize.left;
  BYTE MAPSQUAREHEIGHT = MapSquareSize.bottom-MapSquareSize.top;
  int occX=-1,occY=-1;

  screenX = 0;
  screenY = 0;
  EP = -1;
  startLoc = FALSE;

  // It used to be that when you switched to a level smaller than
  // the window then the walls and such from the old level stayed
  // around.  The easiest thing to do was to put the thing on a
  // torus and show multiple copies of the squares.  Some people
  // may like this and others may not.  We could provide an option
  // to blank out the unused part of the screen.  PRS 2July00  
  if (m_TileMap)
  {
    MaxHeight = m_CellHeight; //#cells that will fit in window
    MaxWidth  = m_CellWidth;
  }
  else
  {
    int PrevHeight = MaxHeight;
    int PrevWidth = MaxWidth;

    //m_CurrY is current Scroll position.
    MaxHeight = max(0,levelData.area_height - m_CurrY);
    MaxHeight = min(MaxHeight, m_CurrY+m_CellHeight);
    MaxWidth  = max(0,levelData.area_width - m_CurrX);
    MaxWidth  = min(MaxWidth,  m_CurrX+m_CellWidth);

    // if map size has changed since previous display update,
    // then erase entire surface.
    if ((PrevHeight != MaxHeight) || (PrevWidth != MaxWidth))
      GraphicsMgr.ClearSurface(dib);
  }

  // drawing overland map
  if (bgLoaded)
  {
    if (CDlgPartialPicture::ShouldDrawDib())
    {
      // display portion of background image
      CDlgPartialPicture::SetVisible(m_CurrX*SQUARESIZE, m_CurrY*SQUARESIZE, dstRect);
      CDlgPartialPicture::RenderDib(dstRect, dib);
    }
  }
  
  HDC hDC=NULL;
  
  if (m_mapType == DUNGEON_TYPE)
  {
    // if you are running Windows 9x, your
    // computer will lock up when the GetDC()
    // completes. If using WinNT/2k you can
    // safely step into the code between
    // GetSurfaceDC() and ReleaseSurfaceDC().
    // If using Win9x you can set a break point
    // after the ReleaseSurfaceDC() and resume
    // the debugger.
    hDC = GraphicsMgr.GetSurfaceDC(dib);  
  }

  for (mapY=0; mapY<MaxHeight; mapY++)
  {   
    levelY=(mapY+m_CurrY)%levelData.area_height;
    screenY = (mapY * SQUARESIZE) + OFFSET;

    for (mapX=0; mapX<MaxWidth; mapX++)
    {
      levelX=(mapX+m_CurrX)%levelData.area_width;
      screenX = (mapX * SQUARESIZE) + OFFSET;
      occupied = ((levelY == currY) && (levelX == currX));
      if (occupied)
      {
        occX = screenX;
        occY = screenY;
      }

      if (m_mode == START_LOC_MODE)
      {
        if ((globalData.currLevel == globalData.startLevel) && 
            (levelY == globalData.startY) && 
            (levelX == globalData.startX))
           startLoc = TRUE;
        else
           startLoc = FALSE;
      }
      else if (m_mode == ENTRY_POINT_MODE)
      {
        EP = -1;
        for (int e=0;(e<MAX_ENTRY_POINTS) && (EP < 0);e++)
        {
           if (   (levelX == globalData.levelInfo.stats[globalData.currLevel].entryPoints[e].x) 
               && (levelY == globalData.levelInfo.stats[globalData.currLevel].entryPoints[e].y))
              EP = e;
        }
      }

      if (m_mapType == DUNGEON_TYPE)
      {      
        if (   (m_mode == ENTRY_POINT_MODE)
            || (m_mode == ZONE_MODE))
        {
          TranslateRect(screenX, screenY, MapSquareSize, tempRect);
          GraphicsMgr.FillRect(&tempRect, GetBackdropColorBrush(7), hDC);
        }
        else
        {
          TranslateRect(screenX, screenY, MapSquareSize, tempRect);
          GraphicsMgr.FillRect(&tempRect, GetBackdropColorBrush(0), hDC);
        }

        // top,left
        TranslateRect(screenX, screenY, cornerRect, tempRect);
        GraphicsMgr.FillRect(&tempRect, GetBackdropColorBrush(15), hDC);

        // top, right
        TranslateRect(screenX+MAPSQUAREWIDTH-2, screenY, cornerRect, tempRect);
        GraphicsMgr.FillRect(&tempRect, GetBackdropColorBrush(15), hDC);
        
        // bottom, left
        TranslateRect(screenX, screenY+MAPSQUAREHEIGHT-2, cornerRect, tempRect);
        GraphicsMgr.FillRect(&tempRect, GetBackdropColorBrush(15), hDC);

        // bottom, right
        TranslateRect(screenX+MAPSQUAREWIDTH-2, screenY+MAPSQUAREHEIGHT-2, cornerRect, tempRect);
        GraphicsMgr.FillRect(&tempRect, GetBackdropColorBrush(15), hDC);

        DrawSquare(dib, 
                   hDC,
                   screenX,screenY, 
                   levelData.area[levelY][levelX], 
                   startLoc, 
                   EP, 
                   levelAccess.GetAccess(levelX,levelY));
      }
      else
      {
        DrawOverlandSquare(dib, 
                           screenX,screenY, 
                           levelData.area[levelY][levelX], 
                           startLoc, EP);
      }
    }
  }

  if (m_mapType == DUNGEON_TYPE)
    GraphicsMgr.ReleaseSurfaceDC(dib);

  if (occX>=0)
  {    
    TranslateRect(occX+WallOffsetRects[16].left, occY+WallOffsetRects[16].top, ArrowSize, tempRect);
    if (m_mapType == DUNGEON_TYPE)
      GraphicsMgr.BlitImage(dib, m_SrcDib, &ArrowRects[currFacing], &tempRect);
    else
      GraphicsMgr.BlitImage(dib, m_ArtDib, &ArrowRects[currFacing], &tempRect);
  }
}

void CDlgPartialMapPicture::DrawSquare(long dstDib,
                                       HDC hDC,
                                       int x, int y, 
                                       AREA_MAP_DATA &spot, 
                                       BOOL isStartLoc, 
                                       int EP,
                                       int access)
{
  static RECT tempRect;  

  if (spot.wall[0] > 0)
  {
    TranslateRect(x+WallOffsetRects[0].left, y+WallOffsetRects[0].top, H_WallSize, tempRect);
    GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(spot.wall[0]), hDC);

    TranslateRect(x+WallOffsetRects[2].left, y+WallOffsetRects[2].top, H_WallSize, tempRect);
    GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(spot.wall[0]), hDC);

    if (WallSets[spot.wall[0]].doorSurface <= 0)
    {
      TranslateRect(x+WallOffsetRects[1].left, y+WallOffsetRects[1].top, H_WallSize, tempRect);
      GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(spot.wall[0]), hDC);
    }
  }

  if (spot.wall[1] > 0)
  {
    TranslateRect(x+WallOffsetRects[4].left, y+WallOffsetRects[4].top, H_WallSize, tempRect);
    GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(spot.wall[1]), hDC);

    TranslateRect(x+WallOffsetRects[6].left, y+WallOffsetRects[6].top, H_WallSize, tempRect);
    GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(spot.wall[1]), hDC);

    if (WallSets[spot.wall[1]].doorSurface <= 0)
    {
      TranslateRect(x+WallOffsetRects[5].left, y+WallOffsetRects[5].top, H_WallSize, tempRect);
      GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(spot.wall[1]), hDC);
    }
  }

  if (spot.wall[2] > 0)
  {
    TranslateRect(x+WallOffsetRects[8].left, y+WallOffsetRects[8].top, V_WallSize, tempRect);
    GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(spot.wall[2]), hDC);

    TranslateRect(x+WallOffsetRects[10].left, y+WallOffsetRects[10].top, V_WallSize, tempRect);
    GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(spot.wall[2]), hDC);

    if (WallSets[spot.wall[2]].doorSurface <= 0)
    {
      TranslateRect(x+WallOffsetRects[9].left, y+WallOffsetRects[9].top, V_WallSize, tempRect);
      GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(spot.wall[2]), hDC);
    }
  }

  if (spot.wall[3] > 0)
  {
    TranslateRect(x+WallOffsetRects[12].left, y+WallOffsetRects[12].top, V_WallSize, tempRect);
    GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(spot.wall[3]), hDC);

    TranslateRect(x+WallOffsetRects[14].left, y+WallOffsetRects[14].top, V_WallSize, tempRect);
    GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(spot.wall[3]), hDC);

    if (WallSets[spot.wall[3]].doorSurface <= 0)
    {
      TranslateRect(x+WallOffsetRects[13].left, y+WallOffsetRects[13].top, V_WallSize, tempRect);
      GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(spot.wall[3]), hDC);
    }
  }

  switch (m_mode)
  {
  case WALL_MODE:
     if (spot.blockage[0] != OpenBlk)
     {
       TranslateRect(x+WallOffsetRects[3].left, y+WallOffsetRects[3].top, HO_WallSize, tempRect);
       GraphicsMgr.FillRect(&tempRect, GetObstructionColorBrush(spot.blockage[0]), hDC);
     }

     if (spot.blockage[1] != OpenBlk)
     {
       TranslateRect(x+WallOffsetRects[7].left, y+WallOffsetRects[7].top, HO_WallSize, tempRect);
       GraphicsMgr.FillRect(&tempRect, GetObstructionColorBrush(spot.blockage[1]), hDC);
     }

     if (spot.blockage[2] != OpenBlk)
     {
       TranslateRect(x+WallOffsetRects[11].left, y+WallOffsetRects[11].top, VO_WallSize, tempRect);
       GraphicsMgr.FillRect(&tempRect, GetObstructionColorBrush(spot.blockage[2]), hDC);
     }

     if (spot.blockage[3] != OpenBlk)
     {
       TranslateRect(x+WallOffsetRects[15].left, y+WallOffsetRects[15].top, VO_WallSize, tempRect);
       GraphicsMgr.FillRect(&tempRect, GetObstructionColorBrush(spot.blockage[3]), hDC);
     }
     if (m_accessMode)
     {
       TranslateRect(x+WallOffsetRects[16].left, y+WallOffsetRects[16].top, MapBlockSize, tempRect);
       GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(GetAccessRectIndex(access)), hDC);
     };
     break;

  case EVENT_MODE:
     if (spot.eventExists)
     {
       TranslateRect(x+WallOffsetRects[16].left, y+WallOffsetRects[16].top, MapBlockSize, tempRect);
       GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(15), hDC);
     }
     break;

  case ZONE_MODE:
     TranslateRect(x+WallOffsetRects[16].left, y+WallOffsetRects[16].top, MapBlockSize, tempRect);
     GraphicsMgr.FillRect(&tempRect, GetZoneColorBrush(spot.zone), hDC);
     break;

  case BG_MODE:
     if (spot.northBG > 0)
     {
       TranslateRect(x+WallOffsetRects[3].left, y+WallOffsetRects[3].top, HO_WallSize, tempRect);
       GraphicsMgr.FillRect(&tempRect, GetBackdropColorBrush(spot.northBG), hDC);
     }

     if (spot.southBG > 0)
     {
       TranslateRect(x+WallOffsetRects[7].left, y+WallOffsetRects[7].top, HO_WallSize, tempRect);
       GraphicsMgr.FillRect(&tempRect, GetBackdropColorBrush(spot.southBG), hDC);
     }

     if (spot.eastBG > 0)
     {
       TranslateRect(x+WallOffsetRects[11].left, y+WallOffsetRects[11].top, VO_WallSize, tempRect);
       GraphicsMgr.FillRect(&tempRect, GetBackdropColorBrush(spot.eastBG), hDC);
     }

     if (spot.westBG > 0)
     {
       TranslateRect(x+WallOffsetRects[15].left, y+WallOffsetRects[15].top, VO_WallSize, tempRect);
       GraphicsMgr.FillRect(&tempRect, GetBackdropColorBrush(spot.westBG), hDC);
     }     
     break;

  case START_LOC_MODE:
     if (isStartLoc)
     {
       TranslateRect(x+WallOffsetRects[16].left, y+WallOffsetRects[16].top, MapBlockSize, tempRect);
       GraphicsMgr.FillRect(&tempRect, GetWallColorBrush(15), hDC);

     }
     break;

  case ENTRY_POINT_MODE:
     if (EP >= 0)
     {
       TranslateRect(x+WallOffsetRects[16].left, y+WallOffsetRects[16].top, MapBlockSize, tempRect);
       GraphicsMgr.FillRect(&tempRect, GetEPColorBrush(EP), hDC);
     }
     break;


  default:
     ASSERT(FALSE);
     WriteDebugString("Unhandled mode in MapPicture::DrawSquare\n");
     if (m_mode == BLOCKAGE_MODE) m_mode = WALL_MODE;

     break;
  }
}

void CDlgPartialMapPicture::DrawOverlandSquare(long dstDib, 
                                               int x, int y, 
                                               AREA_MAP_DATA &spot, 
                                               BOOL isStartLoc, 
                                               int EP)
{
  static RECT tempRect;

  switch (m_mode)
  {
  case ZONE_MODE:
     TranslateRect(x+WallOffsetRects[16].left, y+WallOffsetRects[16].top, MapBlockSize, tempRect);
     GraphicsMgr.FillRect(dstDib, &tempRect, GetZoneColorBrush(spot.zone));
     break;

  case ENTRY_POINT_MODE:
     if (EP >= 0)
     {
       TranslateRect(x+WallOffsetRects[16].left, y+WallOffsetRects[16].top, MapBlockSize, tempRect);
       GraphicsMgr.FillRect(dstDib, &tempRect, GetEPColorBrush(EP));
     }
     break;

  case START_LOC_MODE:
    if (isStartLoc)
    {
      TranslateRect(x+WallOffsetRects[16].left, y+WallOffsetRects[16].top, MapBlockSize, tempRect);
      GraphicsMgr.FillRect(dstDib, &tempRect, GetWallColorBrush(15));
    }
    break;

  case EVENT_MODE:
     // below
     break;
  case BLOCKAGE_MODE:
     // below
     break;
  default:
     ASSERT(FALSE);
     WriteDebugString("Unhandled mode in MapPicture::DrawOverlandSquare\n");
     if (m_mode == WALL_MODE) m_mode = BLOCKAGE_MODE;
     break;
  }

   if (spot.eventExists)
   {
     TranslateRect(x, y, MapSquareSize, tempRect);
     GraphicsMgr.BlitImage(dstDib, m_ArtDib, &EventMapSquareRect, &tempRect);
   }

   if (spot.bkgrnd) // true means blocked for overland maps
   {
     TranslateRect(x, y, MapSquareSize, tempRect);
     GraphicsMgr.BlitImage(dstDib, m_ArtDib, &BlockMapSquareRect, &tempRect);     
   }
}

BOOL CDlgPartialMapPicture::ShouldDrawDib()
{
  if (bgLoaded)
    return (GraphicsMgr.ValidSurface(m_ArtDib) && GraphicsMgr.ValidSurface(m_PicDib) && CDlgPartialPicture::ShouldDrawDib());
  else
    return (GraphicsMgr.ValidSurface(m_SrcDib) && GraphicsMgr.ValidSurface(m_PicDib));
}

BOOL CDlgPartialMapPicture::LoadPicture(CString filename, const CString& path1, const CString& path2)
{
   if (m_PicDib < 0)
     CreateBlankPicture();

   m_CellWidth = m_PicRect.Width() / SQUARESIZE;
   m_CellHeight = m_PicRect.Height() / SQUARESIZE;

   GraphicsMgr.ReleaseSurface(m_ArtDib);
   m_ArtRect = CRect(0,0,0,0);
   GraphicsMgr.ReleaseSurface(m_SrcDib);
   m_SrcRect = CRect(0,0,0,0);
   bgLoaded = FALSE;

   Invalidate(FALSE);
   SetVisible(0,0);

   if (filename.IsEmpty())
      return FALSE;

   m_SrcDib = GraphicsMgr.AddFileSurface(filename, BufferDib, path1, path2);
   GraphicsMgr.GetSurfaceRect(m_SrcDib, &m_SrcRect);
   return (m_SrcDib > 0);
}

BOOL CDlgPartialMapPicture::LoadBackground(CString filename/*background*/, CString art/*objects on bkgnd*/)
{
   if (m_PicDib < 0)
     CreateBlankPicture();

   m_CellWidth = m_PicRect.Width() / SQUARESIZE;
   m_CellHeight = m_PicRect.Height() / SQUARESIZE;

   GraphicsMgr.ReleaseSurface(m_ArtDib);
   m_ArtRect = CRect(0,0,0,0);
   GraphicsMgr.ReleaseSurface(m_SrcDib);
   m_SrcRect = CRect(0,0,0,0);

   Invalidate(FALSE);
   SetVisible(0,0);

   if (filename.IsEmpty())
      return FALSE;

   if (art.IsEmpty())
     return FALSE;

   bgLoaded = CDlgPartialPicture::LoadPicture(filename, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir());

   if (bgLoaded)
   {
     m_ArtDib = GraphicsMgr.AddFileSurface(art, TransBufferDib, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir());
     GraphicsMgr.GetSurfaceRect(m_ArtDib, &m_ArtRect);
   }

   return (bgLoaded && (GraphicsMgr.ValidSurface(m_SrcDib)));
}

void CDlgPartialMapPicture::SetVisible(int x, int y)
{
  m_CurrX = x;
  m_CurrY = y;
}

BOOL CDlgPartialMapPicture::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE; // don't bother to erase
}

void CDlgPartialMapPicture::PrepareDestDC(CDC *pdc)
{
  // do nothing
}

/////////////////////////////////////////////////////////////////////////////
// CMap3DPicture

BEGIN_MESSAGE_MAP(CDlgMap3DPicture, CDlgPicture)
//{{AFX_MSG_MAP(CDlgMap3DPicture)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDlgMap3DPicture::CDlgMap3DPicture() : CDlgPicture()
{
  m_x=m_y=m_facing=0;currTime=0;
}

CDlgMap3DPicture::~CDlgMap3DPicture()
{
  if (CTimer::Active()) {
      CTimer::StopTimer();
  }
  if (globalData.MapArtSurf >= 0)
    GraphicsMgr.ReleaseSurface(globalData.MapArtSurf);
}

BOOL CDlgMap3DPicture::Initialize()
{
  CreateBlankPicture();

  // need the AreaView art to draw area view to 3d viewport
  if (globalData.MapArtSurf > 0)
    GraphicsMgr.ReleaseSurface(globalData.MapArtSurf);

  if (!LoadAreaViewArt(globalData.m_MapArt, globalData.MapArtSurf, globalData.UseAVZones))
  {
    ASSERT(FALSE);
    WriteDebugString("Failed LoadAreaViewArt() in CDlgMap3DPicture()\n");
    return FALSE;
  }

  currTime = 0;
  CTimer::StopTimer();
  CTimer::StartPeriodicTimer(EDITOR_TIMER_CYCLE);

  return TRUE;
}

BOOL CDlgMap3DPicture::ShouldDrawDib()
{
  return (GraphicsMgr.ValidSurface(m_PicDib));
}

void CDlgMap3DPicture::RenderDib(CRect &dstRect, long dib)
{
  if (!ShouldDrawDib())
    return;

  GraphicsMgr.ClearSurface(dib);

  //if (m_mapType == OVERLAND_TYPE)
  //  return;

  int oldX = ViewportX;
  int oldY = ViewportY;
  ViewportX = 0;
  ViewportY = 0;

  // this procedure is used by both engine
  // and editor
  updateViewport(m_x, m_y, m_facing, dib, FALSE);

  ViewportX = oldX;
  ViewportY = oldY;
}

BOOL CDlgMap3DPicture::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE; // dont bother to erase
}

void CDlgMap3DPicture::PrepareDestDC(CDC *pdc)
{
  // do nothing
}

VOID CDlgMap3DPicture::OnTimerEvent(UINT TimerId) {
    BackgroundSlotMemType currBg = BackgroundSets[currBgSlot];
    if (currBgFrame > currBg.NumFrames) {
        currBgFrame = 1;
    }
    if ((currTime - lastBgUpdate) >= (DWORD)currBg.timeDelay) {
        currBgFrame++;
        lastBgUpdate = currTime;
        Invalidate();
    }
    currTime += EDITOR_TIMER_CYCLE;
}
