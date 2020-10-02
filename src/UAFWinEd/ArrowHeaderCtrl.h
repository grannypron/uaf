/******************************************************************************
* Filename: ArrowHeaderCtrl.h
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
*
* Code downloaded from http://www.codeguru.com
******************************************************************************/
#if !defined(AFX_ARROWHEADERCTRL_H__9EF3CCC3_1396_11D3_97BA_005004055F30__INCLUDED_)
#define AFX_ARROWHEADERCTRL_H__9EF3CCC3_1396_11D3_97BA_005004055F30__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CArrowHeaderCtrl window

class CArrowHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	CArrowHeaderCtrl();

// Attributes
public:

protected:
        int     m_sortCol;
        BOOL    m_sortAsc;

// Operations
public:
	int SetSortImage( int nCol, BOOL bAsc );
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArrowHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CArrowHeaderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CArrowHeaderCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARROWHEADERCTRL_H__9EF3CCC3_1396_11D3_97BA_005004055F30__INCLUDED_)
