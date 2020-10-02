/******************************************************************************
* Filename: UAFWinEdView.h
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

#if !defined(AFX_UAFWINEDVIEW_H__56306A0F_E92E_11D3_891D_00105AA4563A__INCLUDED_)
#define AFX_UAFWINEDVIEW_H__56306A0F_E92E_11D3_891D_00105AA4563A__INCLUDED_

#include "UAFWinEd.h"
#include "DlgPicture.h"
#include "ColorBox.h"

extern int EDITOR_RESOURCE_ID;

class CUAFWinEdView : public CFormView
{
   int m_IDD;

protected: // create from serialization only
	CUAFWinEdView();
	DECLARE_DYNCREATE(CUAFWinEdView)

public:
   int m_picX;
   int m_picY;
   int m_PopUpX; // Location of cursor at time Popup Menu displayed
   int m_PopUpY;
   int m_oldWall, m_oldBlockage, m_oldBG, m_oldZone, m_oldMode, m_oldX, m_oldY, m_oldFacing;
   int m_oldEP,m_oldMaxHeight,m_oldMaxWidth,m_oldLevel;

   void SelectWall(void);
   void SelectBackground(void);
   void SelectZone(void);
   void SelectEntryPoint(void);
   BOOL CheckButtonStates(void);
   BOOL ClickInsideGrid(CPoint point);
   void HandleLeftClick(UINT squareX, UINT squareY, UINT dx, UINT dy);
   void HandlePictureLeftClick(UINT square);
   void HandlePictureRightClick(UINT square);
   void HandleRightClick(UINT squareX, UINT squareY);
   BOOL MouseToMap(CPoint pt, UINT &x, UINT &y, UINT *dx=NULL, UINT *dy=NULL);
   BOOL MouseToPicture(CPoint point, UINT& square);
   void MapToMouse(CPoint& pt, UINT x, UINT y);
   int MousePointToWall(UINT dx, UINT dy);
   BOOL ProcessKeyPress(HWND hWnd, WPARAM nChar, bool control=false, bool shift=false);
   void UpdateBlockageBox(CDataExchange *pDX);
   void Move(int direction); // direction relative to currFacing
   void MoveTo(int x, int y);
   void SetSlotButtonText(int curr);

// Attributes
public:
 	 CUAFWinEdDoc* GetDocument();
//   TFXDataTip tip;
   BOOL Initialize(void);
   unsigned int m_mouseX, m_mouseY; // Mouse position on map (or -1)
   void SetSBData();
   void ResetView();
	 void MoveMapLeft();
   void MoveMapRight();
   void MoveMapUp();
   void MoveMapDown();

   void MoveMapHorz(int newPos);
   void MoveMapVert(int newPos);

public:
	//{{AFX_DATA(CUAFWinEdView)
	enum { IDD = IDD_3DPICDLG };
	CButton	m_ShowBands;
	CDlgPartialMapPicture	m_MapPicture;
	CColorBox	m_ColorOpen;
	CColorBox	m_ColorBlk;
	CScrollBar	m_VertScroll;
	CScrollBar	m_HorzScroll;
	CColorBox	m_Color9;
	CColorBox	m_Color8;
	CColorBox	m_Color7;
	CColorBox	m_Color6;
	CColorBox	m_Color5;
	CColorBox	m_Color4;
	CColorBox	m_Color3;
	CColorBox	m_Color2;
	CColorBox	m_Color16;
	CColorBox	m_Color15;
	CColorBox	m_Color14;
	CColorBox	m_Color13;
	CColorBox	m_Color12;
	CColorBox	m_Color11;
	CColorBox	m_Color10;
	CColorBox	m_Color1;
  CColorEditBox m_CellStatus;
	CDlgMap3DPicture	m_PictureFrame;
	CDlgPicture	m_BGPic;
	CColorBox	m_ZoneColor;
	CDlgWallPicture	m_WallSlotPic;
	CColorBox	m_WallSlotColor;
	CColorBox	m_ObColor;
	CColorBox	m_BGColor;
	CButton	m_PlaceRight;
	CButton	m_PlaceLeft;
	CButton	m_SelectButton;
	CButton	m_PlaceButton;
	CString	m_ModeText;
	CString	m_ObText;
	CString	m_ZoneText;
	CString	m_CurrX;
	CString	m_CurrY;
	CString	m_CurrFacing;
	CString	m_DesignName;
	CString	m_LevelName;
	BOOL	m_Select1;
	BOOL	m_Select10;
	BOOL	m_Select11;
	BOOL	m_Select12;
	BOOL	m_Select13;
	BOOL	m_Select14;
	BOOL	m_Select15;
	BOOL	m_Select16;
	BOOL	m_Select2;
	BOOL	m_Select3;
	BOOL	m_Select4;
	BOOL	m_Select5;
	BOOL	m_Select6;
	BOOL	m_Select7;
	BOOL	m_Select8;
	BOOL	m_Select9;
	CString	m_BGSlot;
	CString	m_WallSlot;
	BOOL	m_BlockageBlk;
	BOOL	m_BlockageOpen;
	CString	m_CellStatusText;
	CString	m_MaxHeight;
	CString	m_MaxWidth;
	BOOL	m_ShowDistantBackdrops;
	//}}AFX_DATA

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUAFWinEdView)
	public:
   virtual void OnDraw(CDC* pDC);  // overridden to draw this view
   virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
   virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUAFWinEdView();
  void SetCurrentVal(int curr);
  void SetCurrentButton(int curr);
  void SetCurrentBlockage(int curr);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CUAFWinEdView)
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPlacecurrent();
	afx_msg void OnPlaceleft();
	afx_msg void OnPlaceright();
	afx_msg void OnSelectcurrent();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSelect1();
	afx_msg void OnSelect10();
	afx_msg void OnSelect11();
	afx_msg void OnSelect12();
	afx_msg void OnSelect13();
	afx_msg void OnSelect14();
	afx_msg void OnSelect15();
	afx_msg void OnSelect16();
	afx_msg void OnSelect2();
	afx_msg void OnSelect3();
	afx_msg void OnSelect4();
	afx_msg void OnSelect5();
	afx_msg void OnSelect6();
	afx_msg void OnSelect7();
	afx_msg void OnSelect8();
	afx_msg void OnSelect9();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBlockageblk();
	afx_msg void OnBlockageopen();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnAccessoptions();
  afx_msg void OnMAPpopEditEvent();
  afx_msg void OnMAPpopCancel();
	afx_msg void OnTogglebgmethod();
	afx_msg void OnTogglebgbands();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in UAFWinEdView.cpp
inline CUAFWinEdDoc* CUAFWinEdView::GetDocument()
   { return (CUAFWinEdDoc*)m_pDocument; }
#endif


void PlaceEvent(UINT squareX,UINT squareY);
void PlaceFront(int facing, int x, int y);
void PlaceLeft(int x, int y);
void PlaceRight(int x, int y);
void PlaceWall(int whichSide, int x, int y, int wall=-1, int blockage=-1);
void PlaceBlockage(int x, int y, BOOL isBlock);
void PlaceBackground(int whichSide, int x, int y, int bg=-1);
void UndoAction(UndoDataType &data);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UAFWINEDVIEW_H__56306A0F_E92E_11D3_891D_00105AA4563A__INCLUDED_)
