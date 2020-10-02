/******************************************************************************
* Filename: Encnter.h
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
// CEncounter dialog

class CEncounter : public CDialog
{
// Construction
public:
	CEncounter(ENCOUNTER_DATA& encounter, CWnd* pParent = NULL);   // standard constructor
	void GetEncounterData(ENCOUNTER_DATA& encounter);
	
// Dialog Data
	//{{AFX_DATA(CEncounter)
	enum { IDD = IDD_ENCOUNTER };
	CButton	m_SeeMonster;
	CComboBox	m_Distance;
	CComboBox	m_RangeZero;
	int		m_MonsterSpeed;
	CString	m_Text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEncounter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
		ENCOUNTER_DATA m_encounter;
    void SetButtonStates();

	// Generated message map functions
	//{{AFX_MSG(CEncounter)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonchoices();
	afx_msg void OnSelchangeDistance();
	afx_msg void OnSelchangeRangezero();
	afx_msg void OnSeemonster();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
