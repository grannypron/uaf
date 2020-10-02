/******************************************************************************
* Filename: Mapstyle.h
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
// MapStyle dialog

class MapStyle : public CDialog
{
// Construction
public:
	MapStyle(LEVEL_STATS &data, BOOL AllowStyleChange, CWnd* pParent = NULL);   // standard constructor
  CString m_ovName;
  BOOL m_AllowStyleChange;
  LEVEL_STATS m_data;

// Dialog Data
	//{{AFX_DATA(MapStyle)
	enum { IDD = IDD_MAPSTYLE };
	CButton	m_StepSound;
	CButton	m_BumpSound;
	CComboBox	m_AVStyleList;
	CButton	m_Load;
	UINT	m_mapHeight;
	UINT	m_mapWidth;
	int		m_Type;
	BOOL	m_UseDefBG;
	CString	m_MapName;
	CString	m_HeightText;
	CString	m_WidthText;
	int		m_EndTime;
	int		m_StartTime;
	BOOL	m_UseNightMusic;
	BOOL	m_SupressBumpSound;
	BOOL	m_SupressStepSound;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MapStyle)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  int m_OrigWidth;
  int m_OrigHeight;

	// Generated message map functions
	//{{AFX_MSG(MapStyle)
	virtual BOOL OnInitDialog();
	afx_msg void OnDungeon();
	afx_msg void OnUsedefbg();
	afx_msg void OnLoadoverlandmap();
	afx_msg void OnOverland();
	virtual void OnOK();
	afx_msg void OnStepsound();
	afx_msg void OnBumpsound();
	afx_msg void OnBackgroundsounds();
	afx_msg void OnNightsounds();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
