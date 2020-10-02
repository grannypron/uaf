/******************************************************************************
* Filename: ScrollPicDlg.cpp
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
//#include "PicSlot.h"
#include "ScrollPicDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// These variables are for horizontal scrolling. 

static int xMinScroll;       // minimum horizontal scroll value 
static int xCurrentScroll;   // current horizontal scroll value 
static int xMaxScroll;       // maximum horizontal scroll value 
 
// These variables are for vertical scrolling. 
 
static int yMinScroll;       // minimum vertical scroll value 
static int yCurrentScroll;   // current vertical scroll value 
static int yMaxScroll;       // maximum vertical scroll value 
 

/////////////////////////////////////////////////////////////////////////////
// CScrollPicDlg dialog


CScrollPicDlg::CScrollPicDlg(const CString& filename, 
                             const CString& path1,
                             const CString& path2,
                             SurfaceType type, CWnd* pParent /*=NULL*/)
	: CDialog(CScrollPicDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScrollPicDlg)
	m_Filename = _T("");
	//}}AFX_DATA_INIT

  m_Filename = filename;
  m_path1 = path1;
  m_path2 = path2;
  m_type = type;
  currX = 0;
  currY = 0;
  m_srcRect.SetRectEmpty();

  xMinScroll=0;
  xMaxScroll=0;
  xCurrentScroll=0;
  yMinScroll=0;
  yMaxScroll=0;
  yCurrentScroll=0;  
}


void CScrollPicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScrollPicDlg)
	DDX_Control(pDX, IDC_VERTSCROLLPIC, m_VertScroll);
	DDX_Control(pDX, IDC_ScrollPicture, m_Picture);
	DDX_Control(pDX, IDC_HORZSCROLLPIC, m_HorzScroll);
	DDX_Text(pDX, IDC_PICFILENAME, m_Filename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScrollPicDlg, CDialog)
	//{{AFX_MSG_MAP(CScrollPicDlg)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_SELECTFILE, OnSelectfile)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScrollPicDlg message handlers

BOOL CScrollPicDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  if (!LoadFile(m_Filename))
  {
    MsgBoxError("Failed to load picture", "File Error");
  }
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScrollPicDlg::SetSBData(CRect SrcRect)
{
  m_srcRect = SrcRect;

  SCROLLINFO data;
  memset(&data, 0, sizeof(data));
  data.cbSize = sizeof(data);
  data.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;

  m_Picture.GetClientRect(&m_BufferRect);
  WORD xNewSize = m_BufferRect.Width();
  WORD yNewSize = m_BufferRect.Height();

  int tmp = SrcRect.Width()-xNewSize;
  xMaxScroll = max(tmp, 0); 
  xCurrentScroll = min(xCurrentScroll, xMaxScroll); 
  data.nMin   = xMinScroll; 
  data.nMax   = xMaxScroll; 
  data.nPage  = min(PAGE_SIZE,xMaxScroll);//xNewSize; 
  data.nPos   = xCurrentScroll; 
  
  if (xMaxScroll > PAGE_SIZE)
  {
    m_HorzScroll.ShowScrollBar(TRUE);
    m_HorzScroll.SetScrollInfo(&data);
  }
  else
  {
    m_HorzScroll.ShowScrollBar(FALSE);
  }

  yMaxScroll = max(SrcRect.Height() - yNewSize, 0); 
  yCurrentScroll = min(yCurrentScroll, yMaxScroll); 
  memset(&data, 0, sizeof(data));
  data.cbSize = sizeof(data); 
  data.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
  data.nMin   = yMinScroll; 
  data.nMax   = yMaxScroll; 
  data.nPage  = min(PAGE_SIZE,yMaxScroll);//yNewSize; 
  data.nPos   = yCurrentScroll; 

  if (yMaxScroll > PAGE_SIZE)
  {
    m_VertScroll.ShowScrollBar(TRUE);
    m_VertScroll.SetScrollInfo(&data);
  }
  else
  {
    m_VertScroll.ShowScrollBar(FALSE);
  }
}

BOOL CScrollPicDlg::LoadFile(CString file)
{
  BOOL success = FALSE;

  if (file.IsEmpty())
  {
    SetSBData(CRect(0,0,0,0));
    m_Picture.ErasePicture();
    success = TRUE;
  }
  else
  {
    if (m_Picture.LoadPicture(file, m_type, m_path1, m_path2))
    {
      xMinScroll=0;
      xMaxScroll=0;
      xCurrentScroll=0;
      yMinScroll=0;
      yMaxScroll=0;
      yCurrentScroll=0;  
      SetSBData(m_Picture.GetSrcRect());
      m_Picture.SetVisible(xCurrentScroll, yCurrentScroll, m_BufferRect);
      success = TRUE;
    }
  }

   if (success)
   {
      m_Filename = file;
      UpdateData(FALSE);
   }

  return success;
}

void CScrollPicDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  int yNewPos;    // new position 

  switch (nSBCode)
  {
  case SB_LEFT: // far top
    yNewPos = 0;
    break;
  case SB_RIGHT: // far bottom
    yNewPos = yMaxScroll;
    break;
  case SB_LINELEFT: // arrow up
    yNewPos = yCurrentScroll - LINE_SIZE; 
    break;
  case SB_LINERIGHT: // arrow right
    yNewPos = yCurrentScroll + LINE_SIZE; 
    break;
  case SB_PAGELEFT: // click-in-track up
    yNewPos = yCurrentScroll - PAGE_SIZE; 
    break;
  case SB_PAGERIGHT: // click-in-track down
    yNewPos = yCurrentScroll + PAGE_SIZE; 
    break;
  case SB_THUMBPOSITION: // button dragged to nPos
    yNewPos = nPos;
    break;
  default:
    yNewPos = yCurrentScroll;
    break;
  }

  yNewPos = max(0, yNewPos); 
  yNewPos = min(yMaxScroll, yNewPos); 

  // If the current position does not change, do not scroll.
  if (yNewPos == yCurrentScroll) 
      return; 

  // Reset the current scroll position. 
  yCurrentScroll = yNewPos; 
  m_VertScroll.SetScrollPos(yCurrentScroll);
  m_Picture.SetVisible(xCurrentScroll, yCurrentScroll, m_BufferRect);
  m_Picture.Invalidate(FALSE);
}

void CScrollPicDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  int xNewPos;    // new position 

  switch (nSBCode)
  {
  case SB_LEFT: // far left
    xNewPos = 0;
    break;
  case SB_RIGHT: // far right
    xNewPos = xMaxScroll;
    break;
  case SB_LINELEFT: // arrow left
    xNewPos = xCurrentScroll - LINE_SIZE; 
    break;
  case SB_LINERIGHT: // arrow right
    xNewPos = xCurrentScroll + LINE_SIZE; 
    break;
  case SB_PAGELEFT: // click-in-track left
    xNewPos = xCurrentScroll - PAGE_SIZE; 
    break;
  case SB_PAGERIGHT: // click-in-track right
    xNewPos = xCurrentScroll + PAGE_SIZE; 
    break;
  case SB_THUMBPOSITION: // button dragged to nPos
    xNewPos = nPos;
    break;
  default:
    xNewPos = xCurrentScroll;
    break;
  }

  xNewPos = max(0, xNewPos); 
  xNewPos = min(xMaxScroll, xNewPos); 

  // If the current position does not change, do not scroll.
  if (xNewPos == xCurrentScroll) 
      return; 

  // Reset the current scroll position. 
  xCurrentScroll = xNewPos; 
  m_HorzScroll.SetScrollPos(xCurrentScroll);
  m_Picture.SetVisible(xCurrentScroll, yCurrentScroll, m_BufferRect);
  m_Picture.Invalidate(FALSE);
}

void CScrollPicDlg::OnSelectfile() 
{
  CString file;
  file = m_Filename;
  
  if (GetFilenameWithPreview(IconDib, file)) // type can be anything
    LoadFile(file);
}

void CScrollPicDlg::OnClear() 
{
	m_Filename="";
  LoadFile(m_Filename);  
}
