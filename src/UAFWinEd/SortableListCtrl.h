/******************************************************************************
* Filename: SortableListCtrl.h
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


#if !defined(AFX_SORTABLELISTCTRL_H__3381E8C8_1360_11D3_97B9_005004055F30__INCLUDED_)
#define AFX_SORTABLELISTCTRL_H__3381E8C8_1360_11D3_97B9_005004055F30__INCLUDED_

#include "ArrowHeaderCtrl.h"
#include "ListSort.h"

#include <afxtempl.h>
#include <afxmt.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SortableListCtrl.h : header file
//

#ifdef __cplusplus
extern "C"{
#endif
  typedef struct _INSERTITEM
  {
    short count;
    LV_ITEM *SubItems;
  }INSERTITEM;
  
#ifdef __cplusplus
}
#endif

// message number for inserting an item into 
// the CSortableList maintaining the sorted
// order.
#define WM_INSERT_ITEM_SORTED (WM_USER+1)
#define WM_SORT_LIST (WM_USER+2)

class gxEditCell;

//*****************************************************************************
//
// NAME: SortablePair
//
// TYPE: Class interface definition
//
// PURPOSE: Holds the data pair m_sortType and m_isSortable
//
// NOTES: 
//
//*****************************************************************************
class SortablePair
{
public:
	SortablePair();
	SortablePair(CListSort::EDataType sortType, bool isSortable, bool isEditable);
	virtual ~SortablePair();

	CListSort::EDataType m_sortType;
	bool m_isSortable;
  bool m_isEditable;
};



/////////////////////////////////////////////////////////////////////////////
// CSortableListCtrl window

class CSortableListCtrl : public CListCtrl
{
  friend gxEditCell;

  // Construction
public:
  CSortableListCtrl(bool bFullRowSelect = true);
  
  int InsertItemEx(int Item);

  // Attributes
public:
  enum ESortDirection{sdASCENDING = true, sdDESCENDING = false};
  
protected:
  bool m_DisableVScrollUpdates;
  int m_sortedCol; 
  bool m_sortAscending;
  CArrowHeaderCtrl   m_headerCtrl;
  CCriticalSection m_sortLock;
  bool m_fullRowSelect;
  bool m_IsFontBold;
  bool m_UseParentText;
  HFONT m_hBoldFont;
  bool m_AllowEdits;
  bool m_OneClickEdit;
  
  //CEdit*  EditSubItem (int Item, int Column);
  int     HitTestEx (CPoint& Point, int* Column);  
  void    Resize (int cx, int cy);
  
  
  // Operations
public:
  virtual int InsertColumn(int nCol, 
    const LVCOLUMN* pColumn,
    CListSort::EDataType dataType = CListSort::dtSTRING,
    bool isSortable = true,
    bool isEditable = true);
  
  virtual int InsertColumn(int nCol,
    LPCTSTR lpszColumnHeading,
    int nFormat = LVCFMT_LEFT,
    int nWidth = -1,
    int nSubItem = -1,
    CListSort::EDataType dataType = CListSort::dtSTRING,
    bool isSortable = true,
    bool isEditable = true);
  
  virtual BOOL DeleteColumn(int nCol);
  
  bool SetDefaultSortAttributes(int nCol, CSortableListCtrl::ESortDirection sortDirection);
  LRESULT Sort(WPARAM wParam = 0, LPARAM lParam = 0);

  CArrowHeaderCtrl &GetHeader() { return m_headerCtrl; }

  void SetAllowEdits(bool allow) { m_AllowEdits = allow; }
  void SetAllowOneClickEdit(bool allow) { m_OneClickEdit = allow; }

  void SetFontBold(bool IsBold=true, int FontHeightDelta=0, TCHAR* pFaceName = NULL);
  void SetCellHeight(int PixelHeight);
  
  void PrepareForBulkInsert(int count);
  void FinishBulkInsert();

  void DisableVScrollUpdates(bool disable) { m_DisableVScrollUpdates = disable; }
  void UseParentText(bool use=true) { m_UseParentText = use; }

  BOOL GetCellRect(int iRow, int iCol, int nArea, CRect &rect);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CSortableListCtrl)
protected:
  virtual void PreSubclassWindow();
  //}}AFX_VIRTUAL
  
private:
  inline bool InRange(int val, int valMin, int valMax);
  
  // Implementation
public:
  virtual ~CSortableListCtrl();
  void    SetItemText(int nRow, int nCol, const CString &text);

  void SelectItem(int nRow, bool PutInMiddle=false);
  void SetRowPosition(int nRow, int posFromTop);
  void EnsureVisibleColumn(int nRow, int nCol);

  // only called if m_UseParentText is true
  //
  // must be implemented by derived class to provide cell text for given
  // row/col
  //
  virtual CString LookupItemText(int nRow, int nCol) { ASSERT(FALSE); return ""; }
  virtual void UpdateItemText(int nRow, int nCol, const CString &text) { ASSERT(FALSE); }
  virtual void FormatItemText(int nRow, int nCol, CString &text, CString &mask, CString &marker) { mask=""; marker=""; }

  // called with current cell text, just before user is allowed to edit cell contents
  virtual void OnItemPreEdit(int nRow, int nCol, CString &text) { } 
  
  // called after edit is complete, with new text, return false to disallow change
  virtual bool AcceptItemTextUpdate(int nRow, int nCol, CString &text) { return true; }

  // return true to halt further processing
  virtual BOOL OnCursorChange(CWnd* pWnd, UINT nHitTest, UINT message) { return FALSE; }

  // return true if you draw your own text, 
  // return false if want the list control to use default text display
  //
  // draw text to the given CDC like this
  //
  //   if (nRow or nCol is a cell you want to draw
  //   then 
  //      pDC->DrawText(text, *pRect, DT_CENTER);
  //      return true;
  //   else
  //      return false;

  virtual bool DrawCustomText(NMLVCUSTOMDRAW *pDrawItem, LPCRECT pRect) { return false; }
  
  // Generated message map functions
protected:
  // Map used to track for each colmn: the sort type and if column is sortable 
  CMap<int,int, SortablePair, SortablePair&> m_sortMap;
  CArray<SortablePair, SortablePair&> m_sortArray;
  
  bool SetSortAttributes(int nCol, CSortableListCtrl::ESortDirection sortDirection);
  virtual LRESULT InsertItemSorted(WPARAM pInsertItem, LPARAM lParam);

  LPTSTR AddPool(CString& str);
  CString QueryItemText(int nRow, int nCol);


  //{{AFX_MSG(CSortableListCtrl)
  afx_msg void OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
  
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnCustomDraw(NMHDR* nmHdr, LRESULT* result);
  afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SORTABLELISTCTRL_H__3381E8C8_1360_11D3_97B9_005004055F30__INCLUDED_)
