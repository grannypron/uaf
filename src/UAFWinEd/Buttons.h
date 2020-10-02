/******************************************************************************
* Filename: Buttons.h
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
// CButtons dialog

class CButtons : public CDialog
{
// Construction
public:
	CButtons(ENCOUNTER_BUTTON_DATA& buttons, CWnd* pParent = NULL);   // standard constructor
	void GetButtonData(ENCOUNTER_BUTTON_DATA& buttons);

// Dialog Data
	//{{AFX_DATA(CButtons)
	enum { IDD = IDD_BUTTONS };
	CComboBox	m_Option5;
	CComboBox	m_Option4;
	CComboBox	m_Option3;
	CComboBox	m_Option2;
	CComboBox	m_Option1;
	CString	m_Label1;
	CString	m_Label2;
	CString	m_Label3;
	CString	m_Label4;
	CString	m_Label5;
	BOOL	m_Present1;
	BOOL	m_Present2;
	BOOL	m_Present3;
	BOOL	m_Present4;
	BOOL	m_Present5;
	BOOL	m_UpClose1;
	BOOL	m_UpClose2;
	BOOL	m_UpClose3;
	BOOL	m_UpClose4;
	BOOL	m_UpClose5;
	BOOL	m_OnlyUpClose1;
	BOOL	m_OnlyUpClose2;
	BOOL	m_OnlyUpClose3;
	BOOL	m_OnlyUpClose4;
	BOOL	m_OnlyUpClose5;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtons)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	ENCOUNTER_BUTTON_DATA m_buttons;
  void SetButtonStates();


	// Generated message map functions
	//{{AFX_MSG(CButtons)
	afx_msg void OnSelchangeButtonoption1();
	afx_msg void OnSelchangeButtonoption2();
	afx_msg void OnSelchangeButtonoption3();
	afx_msg void OnSelchangeButtonoption4();
	afx_msg void OnSelchangeButtonoption5();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
