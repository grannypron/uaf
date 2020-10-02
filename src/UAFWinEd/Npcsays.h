/******************************************************************************
* Filename: Npcsays.h
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
/////////////////////////////////////////////////////////////////////////////
// CNPCSays dialog

class CNPCSays : public CDialog
{     
// Construction
public:
	CNPCSays(NPC_SAYS_DATA& says, CWnd* pParent = NULL);   // standard constructor
	void GetNPCSaysData(NPC_SAYS_DATA& says);

// Dialog Data
	//{{AFX_DATA(CNPCSays)
	enum { IDD = IDD_NPCSAYS };
	CButton	m_ChooseArt;
	CButton	m_ChooseSound;
	CComboBox	m_Distance;
	CButton	m_ChooseChar;
	CString	m_Text;
	BOOL	m_HighlightText;
	BOOL	m_MustHitReturn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNPCSays)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	NPC_SAYS_DATA m_says;

	// Generated message map functions
	//{{AFX_MSG(CNPCSays)
	afx_msg void OnChar();
	afx_msg void OnSelchangeDistance();
	afx_msg void OnSound();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnArt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
