/******************************************************************************
* Filename: Campevnt.h
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
// CCampEvent dialog

class CCampEvent : public CDialog
{
// Construction
public:
	CCampEvent(CAMP_EVENT_DATA& camp, CWnd* pParent = NULL);   // standard constructor
	void GetCampData(CAMP_EVENT_DATA& camp);

// Dialog Data
	//{{AFX_DATA(CCampEvent)
	enum { IDD = IDD_CAMPEVENT };
	CButton	m_ChooseMusic;
	CButton	m_CampArt;
	BOOL	m_ForceBackup;
	CString	m_Text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCampEvent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CAMP_EVENT_DATA m_camp;

	// Generated message map functions
	//{{AFX_MSG(CCampEvent)
	virtual void OnOK();
	afx_msg void OnCampart();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
