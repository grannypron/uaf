/******************************************************************************
* Filename: DlgPicture.h
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
#ifndef __DLGPIC_H__
#define __DLGPIC_H__

#include "GlobalData.h"
#include "Level.h"
#include "PicSlot.h"
#include "PicData.h"
#include "Viewport.h"
#include "Graphics.h"

// These classes provide the functionality to draw DirectX surfaces
// into a CStatic frame on a dialog. They handle resizing the source
// to fit the destination window size, the window clipper, and converting
// the current window x,y to screen x,y. If m_UseTracker = TRUE then
// a bounding box will be drawn around the CStatic frame that has the
// focus to indicate it is selected. Each class takes it's source
// picture(s) and draws them to m_PicDib within the RenderDib virtual
// function. The base class handles drawing the m_PicDib to the screen
// when the CStatic window receives a WM_PAINT.

/////////////////////////////////////////////////////////////////////////////
// CDlgPicture window

class CDlgPicture : public CStatic
{
protected:
  long m_PicDib, m_SrcDib;

  CRect m_PicRect, m_SrcRect;
  COLORREF m_Color;
  CRectTracker m_box;
  BOOL m_UseTracker;

  // dstRect = rect of m_PicDib
  // allows modification of m_PicDib bits before
  // they are drawn to CStatic window
  virtual void RenderDib(CRect &dstRect, long dib);

  // Shrinks dest rect down to m_PicDib's size, if needed,
  // to avoid stretching of m_PicDib during 
  // bitblit to CStatic window
  virtual void DeflateDestRect(CRect &dst, CRect &src);

  // Fills CStatic DC with m_Color, and draws
  // CRectTracker if it is activated
  virtual void PrepareDestDC(CDC *pdc);

// Construction
public:
	CDlgPicture();
  virtual ~CDlgPicture();
  long GetDib() { return m_PicDib; }
  long GetSrcDib() { return m_SrcDib; }
  CRect GetSrcRect() { return m_SrcRect; }
  void GetStats(int &w, int &h, int &bits);
  
  // return TRUE if m_PicDib should be drawn 
  // to CStatic's window
  virtual BOOL ShouldDrawDib();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPicture)
	//}}AFX_VIRTUAL

// Implementation
public:
  BOOL LoadPicture(CString filename, int type, const CString& path1, const CString& path2); // loads m_PicDib
  void Highlight(BOOL activate); // activates CRectTracker

  // Creates an uninitialized m_PicDib that has the height/width
  // of CStatic window client area
  long CreateBlankPicture();

  // releases all loaded surfaces and calls CreateBlankPicture()
  virtual long ErasePicture();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDlgPicture)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDlgWallPicture window

class CDlgWallPicture : public CDlgPicture
{
public:
  CDlgWallPicture();
  virtual ~CDlgWallPicture();
  BYTE m_format;

  // load bitmaps for door, wall, and overlay CDib's
  BOOL LoadSourcePictures(CString &door, CString &overlay, CString &wall, BOOL doorFirst);

  // return TRUE if m_PicDib should be drawn to CStatic window
  BOOL ShouldDrawDib();

  long ErasePicture();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWallPicture)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDlgWallPicture)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

  long m_DoorDib;
  long m_OverlayDib;
  long m_WallDib;  
  BOOL m_doorFirst;
  CString m_door;
  CString m_overlay;
  CString m_wall;

  // dstRect = rect of m_PicDib.
  // Draws door, wall, and overlay CDibs to m_PicDib
  void RenderDib(CRect &dstRect, long dib);
};

/////////////////////////////////////////////////////////////////////////////
// C3DSelectPicture window

// draws RECT E_WALL_RECT from the source wall/overlay/door bitmap

class C3DSelectPicture : public CDlgPicture
{
public:
  C3DSelectPicture();
  virtual ~C3DSelectPicture();
  BYTE m_format;

  // return TRUE if m_PicDib should be drawn to CStatic window
  BOOL ShouldDrawDib();
  BOOL LoadSourcePicture(CString &dibFile, int type);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DSelectPicture)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(C3DSelectPicture)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

  long m_srcDib;  
  CString m_srcFile;

  // dstRect = rect of m_PicDib.
  void RenderDib(CRect &dstRect, long dib);
};


/////////////////////////////////////////////////////////////////////////////
// CDlgPicture3D window

class CDlgPicture3D : public CDlgPicture
{
protected:
  long m_BkGndDib;
  long m_DoorDib;
  long m_OverlayDib;
  long m_WallDib;
  
  CString m_bkgnd;
  CString m_door;
  CString m_overlay;
  CString m_wall;

  void RenderDib(CRect &dstRect, long dib);
  BOOL ShouldDrawDib();

// Construction
public:
	CDlgPicture3D();
  BOOL LoadSourcePictures(CString bkgnd,CString door, CString overlay, CString wall);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPicture3D)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDlgPicture3D();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDlgPicture3D)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class CDlgSmallPicture : public CDlgPicture
{
protected:
   PIC_DATA m_SmallPic;
  
  void RenderDib(CRect &dstRect, long dib);
  BOOL ShouldDrawDib();

  enum { SmallPicTimer = 99 };

// Construction
public:
	CDlgSmallPicture();
   BOOL LoadSourcePictures(PIC_DATA& pic);
   void SetCycleInterval(int msecs);
   void SetNextFrame() { m_SmallPic.SetNextFrame(); }
   int  GetFrame() {return m_SmallPic.GetFrame();}
   void SetFrame(int frame) {m_SmallPic.SetFrame(frame); }
   void SetFlags(DWORD flags) { m_SmallPic.flags=flags; }
   void SetMaxLoops(int loops) { m_SmallPic.MaxLoops=loops; m_SmallPic.LoopCounter=0; }
   PIC_DATA &GetSmallPic() { return m_SmallPic; }

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSmallPicture)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDlgSmallPicture();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDlgSmallPicture)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class CDlgPartialPicture : public CDlgPicture
{
protected:
   CRect m_VisibleRect;
  
  void RenderDib(CRect &dstRect, long dib);

// Construction
public:
	CDlgPartialPicture();

   void SetVisible(int x, int y, CRect& bufferRect);
   BOOL LoadPicture(CString filename, const CString& path1, const CString& path2);
   BOOL LoadPicture(CString filename, SurfaceType type, const CString& path1, const CString& path2);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPartialPicture)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDlgPartialPicture();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDlgPartialPicture)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class CDlgPartialMapPicture : public CDlgPartialPicture
{
protected:
  void RenderDib(CRect &dstRect, long dib);

   int m_CellWidth;
   int m_CellHeight;
   
   BOOL bgLoaded;
   long m_ArtDib;
   CRect m_ArtRect;

// Construction
public:
	CDlgPartialMapPicture();

   void SetVisible(int x, int y);
   BOOL LoadPicture(CString filename, const CString& path1, const CString& path2);
   BOOL LoadBackground(CString filename, CString art);

   void DrawSquare(long dstDib, 
                   HDC hDC,
                   int x, int y, 
                   AREA_MAP_DATA &spot, 
                   BOOL isStartLoc, 
                   int EP,
                   int access);

   void DrawOverlandSquare(long dstDib, 
                           int x, int y, 
                           AREA_MAP_DATA &spot, 
                           BOOL isStartLoc, 
                           int EP);

   BOOL ShouldDrawDib();
   void PrepareDestDC(CDC *pdc);

   int m_CurrX;
   int m_CurrY;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPartialMapPicture)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDlgPartialMapPicture();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDlgPartialMapPicture)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDlgMap3DPicture window


class CDlgMap3DPicture : public CDlgPicture
{
public:
	CDlgMap3DPicture();
  virtual ~CDlgMap3DPicture();

  //void RenderDib(int x, int y, int facing);
  void RenderDib(CRect &dstRect, long dib);
  BOOL ShouldDrawDib();
  void SetCurrPos(int x, int y, int facing) { m_x=x; m_y=y; m_facing=facing; }
  BOOL Initialize();
  void PrepareDestDC(CDC *pdc);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMap3DPicture)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDlgMap3DPicture)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
  int m_x;
  int m_y;
  int m_facing;
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern RECT H_WallSize;
extern RECT HO_WallSize;
extern RECT V_WallSize;
extern RECT VO_WallSize;
extern RECT MapSquareSize;
extern RECT MapBlockSize;
//extern RECT BlackMapSquareRect;
//extern RECT GreyMapSquareRect;
//extern RECT H_WallRects[MAX_WALLSETS];
//extern RECT HO_WallRects[MAX_WALLSETS];
//extern RECT V_WallRects[MAX_WALLSETS];
//extern RECT VO_WallRects[MAX_WALLSETS];
//extern RECT BlockRects[MAX_WALLSETS];
extern RECT WallOffsetRects[17];
#endif
