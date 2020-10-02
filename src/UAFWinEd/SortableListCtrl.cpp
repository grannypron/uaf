/******************************************************************************
* Filename: SortableListCtrl.cpp
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

// SortableListCtrl.cpp : implementation file
//
#include "..\Shared\stdafx.h"
#include "SortableListCtrl.h"
//#include "EditCell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//#define IDC_EDITCELL 0xB499


//*****************************************************************************
// NAME: SortablePair::SortablePair
//
// PURPOSE: Default Constructor
//*****************************************************************************
SortablePair::SortablePair() : 
  m_sortType(CListSort::dtNULL),
  m_isSortable(false), 
  m_isEditable(false)
{
}


//*****************************************************************************
// NAME: SortablePair::SortablePair
//
// PURPOSE:  Constructor
//
// PARMETERS:
//   CListSort::EDataType sortType  : The type of sort to apply to column
//   bool isSortable        : Is this a sortable column
//
// RETURNS: 
//*****************************************************************************
SortablePair::SortablePair(CListSort::EDataType sortType, bool isSortable, bool isEditable)
{
  m_sortType = sortType;
  m_isSortable = isSortable;
  m_isEditable = isEditable;
}

//*****************************************************************************
// NAME: SortablePair::~SortablePair
//
// PURPOSE: Destructor
//*****************************************************************************
SortablePair::~SortablePair()
{
}



/////////////////////////////////////////////////////////////////////////////
// CSortableListCtrl




//*****************************************************************************
// NAME: CSortableListCtrl::CSortableListCtrl
//
// PURPOSE:  Constructor
//
// PARMETERS:
//   bool fullRowSelect : Indicates if full row select is enabled
// 
//
// RETURNS: 
//*****************************************************************************
CSortableListCtrl::CSortableListCtrl(bool fullRowSelect) 
  : m_sortedCol(-1), 
  m_sortAscending(true), 
  m_fullRowSelect(fullRowSelect) /* = true */,
  m_IsFontBold(false),
  m_UseParentText(false),
  m_DisableVScrollUpdates(false),
  m_hBoldFont(NULL),
  m_AllowEdits(true),
  m_OneClickEdit(true)
{
}


//*****************************************************************************
// NAME: CSortableListCtrl::~CSortableListCtrl
//
// PURPOSE: Destructor
//*****************************************************************************
CSortableListCtrl::~CSortableListCtrl()
{
  m_sortArray.RemoveAll();
  if (m_hBoldFont != NULL)
    ::DeleteObject(m_hBoldFont);
}


BEGIN_MESSAGE_MAP(CSortableListCtrl, CListCtrl)
//{{AFX_MSG_MAP(CSortableListCtrl)
ON_NOTIFY(HDN_ITEMCLICKA, 0, OnHeaderClicked) 
	ON_WM_MOUSEWHEEL()
ON_NOTIFY(HDN_ITEMCLICKW, 0, OnHeaderClicked)
ON_MESSAGE (WM_INSERT_ITEM_SORTED, InsertItemSorted)
ON_MESSAGE (WM_SORT_LIST, Sort)
ON_WM_HSCROLL()
ON_WM_VSCROLL()
ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
ON_WM_LBUTTONDOWN()
// PRS 20090715	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortableListCtrl message handlers


//*****************************************************************************
// NAME: CSortableListCtrl::OnHeaderClicked
//
// PURPOSE:  Implements sorting when a user clicks on a column header.
//
// PARMETERS:
//   NMHDR* pNMHDR    : Contains information about a notification message
//   LRESULT* pResult : The result status
//
// RETURNS: void 
//*****************************************************************************
void CSortableListCtrl::OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult) 
{
  // terminate any current cell edits
  if (GetFocus() != this) 
    SetFocus();

  if (m_UseParentText)
    return;
  
  HD_NOTIFY *phdn =  reinterpret_cast<HD_NOTIFY *>(pNMHDR);
  
  //CSingleLock sLock(&m_sortLock);
  //sLock.Lock();
  
  SortablePair sPair;
  
  if( phdn->iButton == 0 )
  {
    if(InRange(phdn->iItem, 0,  m_sortArray.GetUpperBound()))
    {
      SetRedraw(false);
      sPair = m_sortArray[phdn->iItem];

      if(sPair.m_isSortable) 
      {
        // User clicked on header using left mouse button
        if( phdn->iItem == m_sortedCol )
        {
          m_sortAscending = !m_sortAscending;
        }
        else
        {
          m_sortAscending = TRUE;
        }

        m_sortedCol = phdn->iItem;
        CListSort csc(this, m_sortedCol);
        csc.Sort(m_sortAscending, sPair.m_sortType); 
      }

      SetRedraw(true);
      Invalidate();
    }    
  }

  if (sPair.m_isSortable)
  {
    POSITION selectedPosition = GetFirstSelectedItemPosition();
    if(selectedPosition != NULL)
    {
      EnsureVisible(reinterpret_cast<int>(selectedPosition)-1, FALSE);
    }
    m_headerCtrl.SetSortImage( m_sortedCol, m_sortAscending );
  }
  
  //sLock.Unlock();
  *pResult = 0;  
}


//*****************************************************************************
// NAME: CSortableListCtrl::PreSubclassWindow
//
// PURPOSE:  We have to sub-class the header control so that the DrawItem() 
//       function in CArrowHeaderCtrl can get called.
//
// NOTES: If the listview control was not created in the report view mode, 
//      then you have to change the style of control before trying the 
//        subclass the control. You can use ModifyStyle() for this. 
//        The reason why we need to change the style to the report view mode is 
//        that the header control is created only when the control first taken 
//        to the report view mode.
//*****************************************************************************
void CSortableListCtrl::PreSubclassWindow() 
{
  // TODO: Add your specialized code here and/or call the base class
  
  if(m_fullRowSelect)
  {
    DWORD dwExStyle = GetExtendedStyle();
    dwExStyle |= (LVS_EX_FULLROWSELECT|LVS_SHOWSELALWAYS);
    SetExtendedStyle(dwExStyle);
  }
  
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_ONECLICKACTIVATE);
  SetExtendedStyle(GetExtendedStyle() & ~LVS_EX_TRACKSELECT);   

  CListCtrl::PreSubclassWindow();
  
  m_headerCtrl.SubclassWindow( ::GetDlgItem(m_hWnd,0) );
}

//*****************************************************************************
//    NAME: CSortableListCtrl::SetFontBold
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::SetFontBold(bool IsBold, int FontHeightDelta, TCHAR* pFaceName) 
{ 
  m_IsFontBold=IsBold; 
  
  CFont *pFont = GetFont();
  if (pFont==NULL) return;

  LOGFONT logFont;
  pFont->GetLogFont(&logFont); 

  //_tcscpy(logFont.lfFaceName, _T("Courier"));
  logFont.lfWeight = m_IsFontBold?FW_DEMIBOLD:FW_NORMAL;
  logFont.lfHeight += FontHeightDelta; // negative numbers make font larger
  if (pFaceName != NULL)
    _tcscpy(logFont.lfFaceName, pFaceName);

  if (m_hBoldFont != NULL)
    ::DeleteObject(m_hBoldFont);

  m_hBoldFont = ::CreateFontIndirect(&logFont);    
}

//*****************************************************************************
//    NAME: CSortableListCtrl::SetCellHeight
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::SetCellHeight(int PixelHeight)
{
  // use empty image list to force cell height
  CImageList imageList; 
  imageList.Create(1,PixelHeight,ILC_COLOR,1,1); 
  SetImageList(&imageList, LVSIL_SMALL); 
}

//*****************************************************************************
// NAME: CSortableListCtrl::InsertColumn
//
// PURPOSE:  Overrides CListCtrl::InsertColumn(int, const LVCOLUMN*)
//
// PARMETERS:
//   int nCol           : Column number 
//   const LVCOLUMN* pColumn    : Address of an LVCOLUMN structure that 
//                    contains the attributes of the new column 
//   CListSort::EDataType dataType  : The type of column
//   bool isSortable        : Is the column sortable
//
// RETURNS: void 
//*****************************************************************************
int CSortableListCtrl::InsertColumn(int nCol, 
                                    const LVCOLUMN* pColumn,
                                    CListSort::EDataType dataType,
                                    bool isSortable,
                                    bool isEditable)
{
  if(nCol != (m_sortArray.GetUpperBound() + 1))
  {
    ASSERT(true);
  }
  
  int retVal = CListCtrl::InsertColumn(nCol, pColumn);
  if(retVal != -1)
  {
    SortablePair sp(dataType, isSortable, isEditable);
    //CSingleLock sLock(&m_sortLock);
    //sLock.Lock();
    //m_sortArray.Add((const SortablePair *)SortablePair(dataType, isSortable, isEditable));
    m_sortArray.Add(sp);
    //sLock.Unlock();
  }
  return retVal;
}

//*****************************************************************************
// NAME: CSortableListCtrl::InsertColumn
//
// PURPOSE:  Overrides CListCtrl::InsertColumn(int, LPCTSTR, int, int, int)
//
// PARMETERS:
//  int nCol            : Column number
//  LPCTSTR lpszColumnHeading   : Address of a string containing the 
//                                    column’s heading.
//  int nFormat           : Integer specifying the alignment of the 
//                    column. 
//  int nWidth            : Width of the column, in pixels.
//  int nSubItem          : Index of the subitem associated with 
//                                    the column. 
//  CListSort::EDataType dataType : The type of column
//  bool isSortable         : Is the column sortable
//
// RETURNS: void 
//*****************************************************************************
int CSortableListCtrl::InsertColumn(int nCol,
                                    LPCTSTR lpszColumnHeading,
                                    int nFormat,
                                    int nWidth,
                                    int nSubItem,
                                    CListSort::EDataType dataType,
                                    bool isSortable,
                                    bool isEditable)
{
  if(nCol != (m_sortArray.GetUpperBound() + 1))
  {
    ASSERT(true);
  }
  
  //CSingleLock sLock(&m_sortLock);
  //sLock.Lock();
  int retVal = CListCtrl::InsertColumn(nCol, 
    lpszColumnHeading, 
    nFormat, 
    nWidth, 
    nSubItem);
  if(retVal != -1)
  {
    SortablePair sp(dataType, isSortable, isEditable);
    //CSingleLock sLock(&m_sortLock);
    //sLock.Lock();
    //m_sortArray.Add(SortablePair(dataType, isSortable, isEditable));
    m_sortArray.Add(sp);
    //sLock.Unlock();
  }
  
  return retVal;
}



//*****************************************************************************
// NAME: CSortableListCtrl::DeleteColumn
//
// PURPOSE:  Overrides CListCtrl::DeleteColumn(int nCol)
//
// PARMETERS:
//   int nCol   : Column to delete
//
// RETURNS: BOOL 
//*****************************************************************************
BOOL CSortableListCtrl::DeleteColumn(int nCol) 
{
  if(!InRange(nCol, 0, m_sortArray.GetUpperBound()))
  {
    ASSERT(true);
  }
  BOOL retVal = CListCtrl::DeleteColumn(nCol);
  if(retVal)
  {
//    CSingleLock sLock(&m_sortLock);
//    sLock.Lock();
    m_sortArray.RemoveAt(nCol);
//    sLock.Unlock();
  }
  return retVal;
}



//*****************************************************************************
// NAME: CSortableListCtrl::SetDefaultSortAttributes
//
// PURPOSE:  Sets the sorting attributes for the list including the
//           arrow on the header.
//
// PARMETERS:
//   int nCol                     : Default sort column
//   CSortableListCtrl::ESortDirection sortDirection  : The sorting 
//                               direction
//
// RETURNS: bool - Return true if sorting attributes are modified
//
// NOTES: This does not actually do sorting.
//       
//*****************************************************************************
bool CSortableListCtrl::SetDefaultSortAttributes(int nCol, 
                                                 CSortableListCtrl::ESortDirection sortDirection)
{
  bool retVal = false;
  if(SetSortAttributes(nCol, sortDirection))
  {
    m_headerCtrl.SetSortImage( m_sortedCol, m_sortAscending );
    retVal = true;
  }
  return retVal;
}


//*****************************************************************************
// NAME: CSortableListCtrl::SetSortAttributes
//
// PURPOSE:  Sets the sorting attributes for the list, the arrow on the header
//       is not affected.
//
// PARMETERS:
//   int nCol                     : Default sort column
//   CSortableListCtrl::ESortDirection sortDirection  : The sorting 
//                               direction
//
// RETURNS: bool - Return true if sorting attributes are modified
//*****************************************************************************
bool CSortableListCtrl::SetSortAttributes(int nCol, 
                                          CSortableListCtrl::ESortDirection sortDirection)
{
  SortablePair sPair;
  
  if(InRange(nCol, 0, m_sortArray.GetUpperBound()))
  {
    sPair = m_sortArray[nCol];
  }
  
  if(sPair.m_isSortable)
  { 
    m_sortAscending = (sortDirection == sdASCENDING) ? TRUE : FALSE;
    m_sortedCol = nCol;
    return true;
  }
  
  return false;
}

//*****************************************************************************
// NAME: CSortableListCtrl::Sort
//
// PURPOSE: Sorts the CSortableListCtrl based upon the m_sortAscending
//      and m_sortedCol members.
//*****************************************************************************
LRESULT CSortableListCtrl::Sort(WPARAM wParam, LPARAM lParam)
{
  // terminate any current cell edits
  if (GetFocus() != this) 
    SetFocus();

  bool retVal = false;
  
  SortablePair sPair;
  
  if(InRange(m_sortedCol, 0, m_sortArray.GetUpperBound()))
  {
    sPair = m_sortArray[m_sortedCol];
  }
  
  if(sPair.m_isSortable)
  {
    
    CListSort csc(this, m_sortedCol);
    
    csc.Sort(m_sortAscending, sPair.m_sortType); 
    retVal = true;
  }
  return retVal;
}

//*****************************************************************************
// NAME: CSortableListCtrl::InsertItemSorted
//
// PURPOSE: Inserts an item based upon the m_sortAscending
//      and m_sortedCol members.
//
// PARMETERS:
//   INSERTITEM * pInsertItem : Poniter to INSERTITEM structure
//   LPARAM   lParam            : Unused
//
// RETURNS: int - The position where the item was inserted
//
// NOTES: This method may also be called by sending/posting a message to
//      window handle using WM_INSERT_ITEM_SORTED as the message.
//      If the column is not sortable or the column equals -1 then the item
//      is inserted at the 0 position.
//*****************************************************************************
LRESULT CSortableListCtrl::InsertItemSorted(WPARAM pInsertItem, LPARAM lParam)
{
  // terminate any current cell edits
  if (GetFocus() != this) 
    SetFocus();

//  CSingleLock sLock(&m_sortLock);
//  sLock.Lock();
  
  int position = 0;
  
  SortablePair sPair;
  
  if(InRange(m_sortedCol, 0, m_sortArray.GetUpperBound()))
  {
    sPair = m_sortArray[m_sortedCol];
  }
  
  if(sPair.m_isSortable)
  {
    LONG lParamSort = sPair.m_sortType;
    if(!m_sortAscending)
    {
      lParamSort = lParamSort * -1;
    }
    
    CListSort *csc = new CListSort(this, m_sortedCol);
    position = csc->GetItemPosition(&(((INSERTITEM *)pInsertItem)->SubItems[m_sortedCol]), lParamSort);
    delete csc;
  }
  short count = ((INSERTITEM *)pInsertItem)->count;
  for(int i = 0; i < count; i++)
  {
    ((INSERTITEM *)pInsertItem)->SubItems[i].iItem = position;
    
    if(i == 0)
    {
      InsertItem(&(((INSERTITEM *)pInsertItem)->SubItems[i]));
    }
    else
    {
      SetItem(&(((INSERTITEM *)pInsertItem)->SubItems[i]));
      
    }
  }
  
  //sLock.Unlock();
  return position;
}


//*****************************************************************************
// NAME: CSortableListCtrl::InRange
//
// PURPOSE:  Determines if val is between valMin and valMax inclusive
//
// PARMETERS:
//   int val    :
//   int valMin   :
//   int valMax   :
//
// RETURNS: bool - true if val is between valMin and valMax inclusive
//*****************************************************************************
bool CSortableListCtrl::InRange(int val, int valMin, int valMax)
{
  bool retVal = false;
  if((val >= valMin) && (val <= valMax))
  {
    retVal = true;
  }
  return retVal;
}


//*****************************************************************************
//    NAME: CSortableListCtrl::Resize
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::Resize(int cx, int cy)
{
  CRect Rect (0, 0, cx, cy);
  MoveWindow (&Rect);
  InvalidateRect (Rect);
  SetColumnWidth (2, LVSCW_AUTOSIZE_USEHEADER);
}

//*****************************************************************************
//    NAME: CSortableListCtrl::EditSubItem
//
// PURPOSE: 
//
//    NOTE: The returned pointer should not be saved
//
//*****************************************************************************
/*
CEdit* CSortableListCtrl::EditSubItem(int Item, int Column)
{
  // rbt
  SetFocus(); 
  
  if ((Item < 0) || (Item >= GetItemCount()))
    return NULL; 
  if ((Column < 0) || (Column >= m_headerCtrl.GetItemCount()))
    return NULL;
  
  // Make sure that the item is visible
  EnsureVisibleColumn(Item, Column);
  
  // Get Column alignment
  LV_COLUMN lvCol;
  lvCol.mask = LVCF_FMT;
  GetColumn(Column, &lvCol);

  DWORD dwStyle;  
  if ((lvCol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
    dwStyle = ES_LEFT;
  else if ((lvCol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
    dwStyle = ES_RIGHT;
  else 
    dwStyle = ES_CENTER;
  
  SortablePair sPair;
  
  if (InRange(Column, 0, m_sortArray.GetUpperBound()))
  {
    sPair = m_sortArray[Column];
    if (sPair.m_sortType == CListSort::dtINT)
      dwStyle |= ES_NUMBER;
  }

  CString text = QueryItemText(Item, Column);
  
  OnItemPreEdit(Item, Column, text);

  // Get the column offset
  int Offset = 0;
  for (int iColumn = 0; iColumn < Column; iColumn++)
    Offset += GetColumnWidth(iColumn);
  
  CRect Rect;
  GetItemRect(Item, &Rect, LVIR_BOUNDS);  
  CRect ClientRect;
  GetClientRect(&ClientRect);

  Rect.left += Offset+4;
  Rect.right = Rect.left + GetColumnWidth(Column) - 3;

  if (Rect.right > ClientRect.right)
    Rect.right = ClientRect.right;
  
  dwStyle |= WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;

  CEdit *pEdit = new gxEditCell(this, Item, Column, text);
  
  pEdit->Create(dwStyle, Rect, this, IDC_EDITCELL);
  
  return pEdit;
}
*/
//*****************************************************************************
//    NAME: CSortableListCtrl::HitTestEx
//
// PURPOSE: 
//
//*****************************************************************************
int CSortableListCtrl::HitTestEx(CPoint& Point, int* pColumn)
{
  int ColumnNum = 0;
  int Row = HitTest (Point, NULL);
  
  if (pColumn)
    *pColumn = 0;
    
  // Get the top and bottom row visible
  Row = GetTopIndex();
  int Bottom = Row + GetCountPerPage();
  if (Bottom > GetItemCount())
    Bottom = GetItemCount();
  
  // Get the number of columns
  int nColumnCount = m_headerCtrl.GetItemCount();
  
  // Loop through the visible rows
  for (; Row <= Bottom; Row++)
  {
    // Get bounding rect of item and check whether point falls in it.
    CRect Rect;
    GetItemRect(Row, &Rect, LVIR_BOUNDS);

    if (Rect.PtInRect(Point))
    {
      // Now find the column
      for (ColumnNum = 0; ColumnNum < nColumnCount; ColumnNum++)
      {
        int ColWidth = GetColumnWidth(ColumnNum);
        if (Point.x >= Rect.left && Point.x <= (Rect.left + ColWidth))
        {
          if (pColumn)
            *pColumn = ColumnNum;
          return Row;
        }
        Rect.left += ColWidth;
      }
    }
  }
  return -1;
}

//*****************************************************************************
//    NAME: hexNumberToInt
//
// PURPOSE: 
//
//*****************************************************************************
BOOL hexNumberToInt(CString HexNumber, int& Number)
{
  TCHAR* pStopString;
  Number = _tcstoul(HexNumber, &pStopString, 16);
  return Number != ULONG_MAX;
} // hexNumberToInt

//*****************************************************************************
//    NAME: CSortableListCtrl::InsertItemEx
//
// PURPOSE: 
//
//*****************************************************************************
int CSortableListCtrl::InsertItemEx(int Item)
{
  if ((GetWindowLong (m_hWnd, GWL_STYLE) & LVS_EDITLABELS) == 0)
  {
    // must enable label edit flag in resource editor if you
    // want in-place editing.
    ASSERT(FALSE);
    return -1;
  }
  
  int Result = InsertItem (Item + 1, _T(""));
  CString ItemVal, Temp;
  if (Item == 0)
    ItemVal = _T("1000");
  else
  {
    int HexVal;
    Temp = QueryItemText(Item - 1, 1);
    hexNumberToInt (Temp, HexVal);
    ItemVal.Format (_T("%x"), HexVal + 1);
  }
  
  CSortableListCtrl::SetItemText (Item, 1, ItemVal);
  
  SetColumnWidth (2, LVSCW_AUTOSIZE_USEHEADER);
  
  return Result;
}

//*****************************************************************************
//    NAME: CSortableListCtrl::OnHScroll
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  // terminate any current cell edits
  if (GetFocus() != this) 
    SetFocus();

  CListCtrl::OnHScroll (nSBCode, nPos, pScrollBar);
}

//*****************************************************************************
//    NAME: CSortableListCtrl::OnVScroll
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{ 
  // terminate any current cell edits
  if (GetFocus() != this) 
    SetFocus();
  
  if (m_DisableVScrollUpdates)
  {
    // no real-time window updates while scrolling
    SCROLLINFO si;
    switch (nSBCode)
    {
    case SB_THUMBPOSITION:
      // wait for thumb thingy to be released at new location
      ZeroMemory(&si, sizeof(si));
      si.cbSize = sizeof(si);
      si.fMask = SIF_TRACKPOS;

      if (!GetScrollInfo(SB_VERT, &si) )
        return; // GetScrollInfo failed

      // Update the list after we have released the scroll handle
      CListCtrl::EnsureVisible(si.nTrackPos, FALSE); //CListCtrl::GetScrollPos()
      break;

    // Don't update the list while dragging the scrolling thumb
    case SB_THUMBTRACK:
      // ignore real-time thumb thingy position updates
      return;
    }
  }

  CListCtrl::OnVScroll (nSBCode, nPos, pScrollBar);
}

//*****************************************************************************
//    NAME: CSortableListCtrl::OnEndLabelEdit
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
  LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
  LV_ITEM *plvItem = &plvDispInfo->item;
  
  if (plvItem->pszText != NULL)
  {
    CSortableListCtrl::SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
  }
  *pResult = FALSE;
}

//*****************************************************************************
//    NAME: CSortableListCtrl::OnLButtonDown
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::OnLButtonDown(UINT nFlags, CPoint Point) 
{
//  POSITION InitialPos = GetFirstSelectedItemPosition();

  CListCtrl::OnLButtonDown (nFlags, Point);

  return;
  /*
  if (!m_AllowEdits)
    return;
  
  // giving item initial focus?
  if (InitialPos != GetFirstSelectedItemPosition())
  {
    if (!m_OneClickEdit)
      return;
  }

  // Make sure that the ListView is in LVS_REPORT
  if ((GetWindowLong (m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
    return;
  
  if ((GetWindowLong (m_hWnd, GWL_STYLE) & LVS_EDITLABELS) == 0)
    return;

  int ColNum;
  int Index = HitTestEx(Point, &ColNum);
  
  if (Index != -1)
  {
    SortablePair sPair;  
    if (!InRange(ColNum, 0, m_sortArray.GetUpperBound()))
      return;

    sPair = m_sortArray[ColNum];

    // are edits allowed for this cell?
    if (!sPair.m_isEditable)
      return;

    EditSubItem(Index, ColNum);
  }
  */
}


//*****************************************************************************
//    NAME: CSortableListCtrl::OnCustomDraw
//
// PURPOSE: Only used to make the font bold (if requested)
//
//*****************************************************************************
void CSortableListCtrl::OnCustomDraw(NMHDR* nmHdr, LRESULT* result)
{
  NMLVCUSTOMDRAW* customDrawItem = reinterpret_cast<NMLVCUSTOMDRAW*>(nmHdr);
  
  // Initialize to default processing:
  *result = CDRF_DODEFAULT;
    
  // if about ready to start drawing list items
  if (CDDS_PREPAINT == customDrawItem->nmcd.dwDrawStage)
  {
    //*result = CDRF_NOTIFYITEMDRAW; // requests notifications before and after list is drawn
    *result = CDRF_NOTIFYSUBITEMDRAW; // call back for every row/col cell
  }  
  else if (CDDS_ITEMPREPAINT == customDrawItem->nmcd.dwDrawStage)
  {
    // stage is just before all list items are drawn
    //
    // changes to drawing properties will affect all list items
    //
    if (m_IsFontBold)
    {
      ::SelectObject(customDrawItem->nmcd.hdc, m_hBoldFont);    
       
      // Tell Windows to paint the control itself, using the new font
      *result = CDRF_NEWFONT|CDRF_NOTIFYSUBITEMDRAW;
    }
    else
      *result = CDRF_NOTIFYSUBITEMDRAW;
  }
  else if ((CDDS_ITEMPREPAINT|CDDS_SUBITEM) == customDrawItem->nmcd.dwDrawStage)
  {
    CRect rc;
    GetCellRect(customDrawItem->nmcd.dwItemSpec, customDrawItem->iSubItem, LVIR_LABEL, rc);
    
    if (DrawCustomText(customDrawItem, rc))
    {
      *result= CDRF_SKIPDEFAULT;
    }
  }
}

//*****************************************************************************
//    NAME: CSortableListCtrl::GetCellRect
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CSortableListCtrl::GetCellRect(int iRow, int iCol, int nArea, CRect &rect)
{
  if (iCol)
  return GetSubItemRect(iRow, iCol, nArea, rect);

  if (m_headerCtrl.GetItemCount() == 1)
   return GetItemRect(iRow, rect, nArea);

  iCol = 1;
  CRect rCol1;
  if (!GetSubItemRect(iRow, iCol, nArea, rCol1))
  return FALSE;

  if (!GetItemRect(iRow, rect, nArea))
  return FALSE;

  rect.right = rCol1.left;

  return TRUE;
}


//*****************************************************************************
//    NAME: CSortableListCtrl::PrepareForBulkInsert
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::PrepareForBulkInsert(int count)
{
  // count is the number of records that are about to be added to this
  // control before a call to FinishBulkInsert() is made.
  //
  if (count <= 0) return;
  LockWindowUpdate();
  SetRedraw(FALSE);
  SetItemCount(count);
}

//*****************************************************************************
//    NAME: CSortableListCtrl::FinishBulkInsert
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::FinishBulkInsert()
{
  SetRedraw(TRUE);
  UnlockWindowUpdate();
}


//*****************************************************************************
//    NAME: CSortableListCtrl::OnGetDispInfo
//
// PURPOSE: 
//
//*****************************************************************************

void CSortableListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CString strField = QueryItemText(pDispInfo->item.iItem, 
		                    					   pDispInfo->item.iSubItem);

		pDispInfo->item.pszText = AddPool(strField);
	}	
  
	*pResult = 0;
}

//*****************************************************************************
//    NAME: CSortableListCtrl::QueryItemText
//
// PURPOSE: 
//
//*****************************************************************************
CString CSortableListCtrl::QueryItemText(int nRow, int nCol)
{
	CString strField("?");
    
  ASSERT( nRow < GetItemCount() );

  if (!m_UseParentText)
  {
    // get text from control
    strField = GetItemText(nRow, nCol);
  }
  else
  {
    // get text from derived class
    strField = LookupItemText(nRow, nCol);
  }

	return strField;
}

//*****************************************************************************
//    NAME: CSortableListCtrl::SetItemText
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::SetItemText(int nRow, int nCol, const CString &text)
{
  ASSERT( nRow < GetItemCount() );

  if (!m_UseParentText)
  {
    // set text in control
    CListCtrl::SetItemText(nRow, nCol, text);
  }
  else
  {
    // set text in derived class
    UpdateItemText(nRow, nCol, text);
  }
}

//*****************************************************************************
//    NAME: CSortableListCtrl::AddPool
//
// PURPOSE: 
//
//*****************************************************************************
LPTSTR CSortableListCtrl::AddPool(CString& str)
{
	static CString cStrPool[3];
	static int nextFree = 0;
	LPTSTR pstrRetVal;
	int nOldest = nextFree;

	nextFree++;
	nextFree %= 3;

	cStrPool[nextFree] = str;
	pstrRetVal = cStrPool[nextFree].LockBuffer();
	cStrPool[nOldest].ReleaseBuffer();

	return pstrRetVal;
}	//	AddPool

//*****************************************************************************
//    NAME: CSortableListCtrl::OnMouseWheel
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CSortableListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
  // terminate any current cell edits
  if (GetFocus() != this) 
    SetFocus();
	
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

//*****************************************************************************
//    NAME: CSortableListCtrl::SelectItem
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::SelectItem(int nRow, bool PutInMiddle)
{
  int total = GetItemCount();
  if ((nRow < 0) || (nRow >= total))
    return;      

  if (PutInMiddle)
  {
    int NumVisibleRows = GetCountPerPage();

    int NewTopRow = nRow - (NumVisibleRows/2);
    int NewBottomRow = nRow + (NumVisibleRows/2);

    if (NewTopRow >= 0)
      EnsureVisible(NewTopRow, TRUE);
    if (NewBottomRow < total)
      EnsureVisible(NewBottomRow, TRUE);
  }

  SetItemState(nRow, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
  EnsureVisible(nRow, FALSE);
  SetSelectionMark(nRow);
}

//*****************************************************************************
//    NAME: CSortableListCtrl::SetRowPosition
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::SetRowPosition(int nRow, int posFromTop)
{
  int total = GetItemCount();

  if ((nRow < 0) || (nRow >= total) || (nRow < posFromTop))
    return;      

  int NumVisibleRows = GetCountPerPage();

  int NewTopRow = nRow - posFromTop;

  int NewBottomRow = NewTopRow + NumVisibleRows;

  if (NewTopRow >= 0)
    EnsureVisible(NewTopRow, TRUE);

  if (NewBottomRow < total)
    EnsureVisible(NewBottomRow, TRUE);

  EnsureVisible(nRow, FALSE);
}

//*****************************************************************************
//    NAME: CSortableListCtrl::EnsureVisibleColumn
//
// PURPOSE: 
//
//*****************************************************************************
void CSortableListCtrl::EnsureVisibleColumn(int nRow, int nCol)
{
  if ((nRow < 0) || (nRow >= GetItemCount()))
    return;      
  if ((nCol < 0) || (nCol >= m_headerCtrl.GetItemCount()))
    return;

  EnsureVisible(nRow, FALSE);

  // all values represent pixels

  int StartColPosition = 0;
  for (int ColIndex=0; ColIndex < nCol; ColIndex++)
    StartColPosition += GetColumnWidth(ColIndex);

  int CurrentScrollPosition = GetScrollPos(SB_HORZ);
    
  CRect ClientRect;
  GetClientRect(&ClientRect);

  // right = left + window width
  int CurrentRightEdge = CurrentScrollPosition + (ClientRect.right-ClientRect.left);

  int CellWidth = GetColumnWidth(nCol);

  // make sure cell is fully visible, the left or right sides can be 
  // partially hidden
  if ((StartColPosition >= CurrentScrollPosition) && ((StartColPosition+CellWidth) < CurrentRightEdge))
    return; // this column is already fully visible

  const int HowMuchOfAdjacentCellToExpose = 15;

  int delta = 0;
  if (StartColPosition < CurrentScrollPosition)
  {
    // scroll to the left
    delta = StartColPosition - CurrentScrollPosition - HowMuchOfAdjacentCellToExpose;
  }
  else
  {
    // scroll to the right
    delta = (StartColPosition - CurrentRightEdge) + CellWidth + HowMuchOfAdjacentCellToExpose;
  }

  if (delta == 0)
    return;

  CSize ScrollAmount(delta, 0);
  Scroll(ScrollAmount);
}

//*****************************************************************************
//    NAME: CSortableListCtrl::OnSetCursor
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CSortableListCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (OnCursorChange(pWnd, nHitTest, message))
    return TRUE;

	return CListCtrl::OnSetCursor(pWnd, nHitTest, message);
}
