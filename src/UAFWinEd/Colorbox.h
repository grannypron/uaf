/******************************************************************************
* Filename: Colorbox.h
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
#ifndef __COLORBOX_H__
#define __COLORBOX_H__

/////////////////////////////////////////////////////////////////////////////
// CColorBox window

class CColorBox : public CStatic
{
  COLORREF m_color;

// Construction
public:
	CColorBox();
  virtual ~CColorBox();

  void SetColor(COLORREF color);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorBox)
	//}}AFX_VIRTUAL

// Implementation
public:
   
	// Generated message map functions
protected:
	//{{AFX_MSG(CColorBox)
	afx_msg void OnPaint();
  //}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CColorEditBox window

class CColorEditBox : public CEdit
{
  COLORREF m_color;

// Construction
public:
	CColorEditBox();
  virtual ~CColorEditBox();

  void SetColor(COLORREF color);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorEditBox)
	//}}AFX_VIRTUAL

// Implementation
public:
   
	// Generated message map functions
protected:
	//{{AFX_MSG(CColorEditBox)
  afx_msg BOOL OnEraseBkgnd( CDC* pDC );
  //}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


#endif


