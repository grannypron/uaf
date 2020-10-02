/******************************************************************************
* Filename: EntryPointDlg.h
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
#if !defined(AFX_ENTRYPOINTDLG_H__62919D02_593F_11D2_AC0D_00C04F8E081C__INCLUDED_)
#define AFX_ENTRYPOINTDLG_H__62919D02_593F_11D2_AC0D_00C04F8E081C__INCLUDED_

#include "GlobalData.h"


/////////////////////////////////////////////////////////////////////////////
// CEntryPointDlg dialog

class CEntryPointDlg : public CDialog
{
// Construction
public:
	CEntryPointDlg(LEVEL_STATS &data, int cep, CWnd* pParent = NULL);   // standard constructor
   void GetData(LEVEL_STATS &data);

// Dialog Data
	//{{AFX_DATA(CEntryPointDlg)
	enum { IDD = IDD_ENTRYPOINTDLG };
	CColorBox	m_Color8;
	CColorBox	m_Color7;
	CColorBox	m_Color6;
	CColorBox	m_Color5;
	CColorBox	m_Color4;
	CColorBox	m_Color3;
	CColorBox	m_Color2;
	CColorBox	m_Color1;
	int		m_x1;
	int		m_x2;
	int		m_x3;
	int		m_x4;
	int		m_x5;
	int		m_x6;
	int		m_x7;
	int		m_x8;
	int		m_y1;
	int		m_y2;
	int		m_y3;
	int		m_y4;
	int		m_y5;
	int		m_y6;
	int		m_y7;
	int		m_y8;
	int		m_CurrEP;
	CString	m_MaxX;
	CString	m_MaxY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEntryPointDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   ENTRY_POINT_TYPE m_EntryPoints[MAX_ENTRY_POINTS];

	// Generated message map functions
	//{{AFX_MSG(CEntryPointDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTRYPOINTDLG_H__62919D02_593F_11D2_AC0D_00C04F8E081C__INCLUDED_)
