/******************************************************************************
* Filename: Tavdlg.h
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
// CTavern dialog

class CTavern : public CDialog
{
// Construction
public:
	CTavern(TAVERN& event, CWnd* pParent = NULL);   // standard constructor
	void GetTavernData(TAVERN& event);

// Dialog Data
	//{{AFX_DATA(CTavern)
	enum { IDD = IDD_TAVERN };
	CSpinButtonCtrl	m_TaleSpin;
	CSpinButtonCtrl	m_DrinkSpin;
	CButton	m_ChooseBarkeep;
	BOOL	m_ForceExit;
	BOOL	m_AllowDrinks;
	BOOL	m_AllowFights;
	int		m_DrinkNum;
	CString	m_DrinkName;
	int		m_DrinkPoints;
	CString	m_Text;
	int		m_PointTrigger;
	int		m_TaleNum;
	CString	m_TaleText;
	int		m_TaleOrder;
	BOOL	m_EachTaleOnceOnly;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTavern)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	TAVERN m_event;

	// Generated message map functions
	//{{AFX_MSG(CTavern)
	afx_msg void OnBarkeep();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDeltaposDrinkspin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposTalespin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
