/******************************************************************************
* Filename: Addnpc.h
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
// AddNPC dialog

class AddNPC : public CDialog
{
// Construction
public:
	AddNPC(ADD_NPC_DATA& add, CWnd* pParent = NULL);   // standard constructor
	void GetNPCData(ADD_NPC_DATA& add);

// Dialog Data
	//{{AFX_DATA(AddNPC)
	enum { IDD = IDD_ADDNPC };
	CButton	m_SeeChar;
	CComboBox	m_Distance;
	CButton	m_AddChar;
	int		m_PcntHitPoints;
	CString	m_Text;
	BOOL	m_UseOriginal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AddNPC)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	ADD_NPC_DATA m_add;

	// Generated message map functions
	//{{AFX_MSG(AddNPC)
	afx_msg void OnAddchar();
	afx_msg void OnSelchangeDistance();
	afx_msg void OnSeechar();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
