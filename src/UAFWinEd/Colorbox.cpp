/******************************************************************************
* Filename: Colorbox.cpp
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

#include "UAFWinEd.h"
#include "ColorBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorBox

CColorBox::CColorBox() : CStatic()
{
  m_color = RGB(0,0,0);
}

CColorBox::~CColorBox()
{
}


BEGIN_MESSAGE_MAP(CColorBox, CStatic)
	//{{AFX_MSG_MAP(CColorBox)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CColorBox::SetColor(COLORREF color)
{
  if (m_color != color)
  {
    m_color = color;

    if (m_hWnd != NULL)
      Invalidate(FALSE);
  }
}

/////////////////////////////////////////////////////////////////////////////
// CColorBox message handlers

void CColorBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect clientRect;
  GetClientRect(clientRect);
  dc.FillSolidRect(clientRect, m_color);
	// Do not call CStatic::OnPaint() for painting messages
}


/////////////////////////////////////////////////////////////////////////////
// CColorEditBox

CColorEditBox::CColorEditBox() : CEdit()
{
  m_color = RGB(0,0,0);
}

CColorEditBox::~CColorEditBox()
{
}


BEGIN_MESSAGE_MAP(CColorEditBox, CEdit)
	//{{AFX_MSG_MAP(CColorEditBox)
  ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CColorEditBox::SetColor(COLORREF color)
{
  if (m_color != color)
  {
    m_color = color;

    if (m_hWnd != NULL)
      Invalidate(FALSE);
  }
}

/////////////////////////////////////////////////////////////////////////////
// CColorEditBox message handlers



afx_msg BOOL CColorEditBox::OnEraseBkgnd( CDC* pDC )
{
	CRect clientRect;
  GetClientRect(clientRect);
  pDC->FillSolidRect(clientRect, m_color);
// pDC->SetBkColor(m_color);  // Byproduct of FillSolidRect()
  pDC->SetBkMode(TRANSPARENT);
  return TRUE;
}

