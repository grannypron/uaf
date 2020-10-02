/******************************************************************************
* Filename: Zonedlg.h
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
#if !defined(AFX_ZONEDLG_H__494CCDF2_4362_11D2_AC08_00C04F8E081C__INCLUDED_)
#define AFX_ZONEDLG_H__494CCDF2_4362_11D2_AC08_00C04F8E081C__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CZoneDlg dialog
#include "Level.h"

class CZoneDlg : public CDialog
{
// Construction
public:
	CZoneDlg(ZONE_DATA& zones, int curr, CWnd* pParent = NULL);   // standard constructor
  void GetData(ZONE_DATA &zones) { zones = m_zones; }

   virtual ~CZoneDlg(void);

   ZONE_DATA m_zones;

// Dialog Data
	//{{AFX_DATA(CZoneDlg)
	enum { IDD = IDD_ZONEDLG };
	CButton	m_AVArt;
	CButton	m_SummonedMonster;
	CButton	m_OCArt;
	CButton	m_ICArt;
	CButton	m_ChooseTreasureArt;
	CButton	m_ChooseCampArt;
	CColorBox	m_ZoneColor;
	CSpinButtonCtrl	m_ZoneSpin;
	CString	m_ZoneName;
	int		m_TurningDifficulty;
	int		m_ZoneNum;
	int		m_AllowAreaMap;
	CString	m_ZoneMsg;
	UINT	m_EventChance;
	UINT	m_EventMinutes;
	BOOL	m_CantRest;
	BOOL	m_AllowMagic;
	BOOL	m_UseNightSounds;
	int		m_EndTime;
	int		m_StartTime;
	BOOL	m_AllowAutoDarken;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZoneDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   //int m_monster;
   MONSTER_ID m_monsterID;
   int m_event;
   PIC_DATA m_CampArt;
   PIC_DATA m_TreasureArt;

   CString m_IndoorCombatArt;
   CString m_OutdoorCombatArt;
   
	// Generated message map functions
	//{{AFX_MSG(CZoneDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCampart();
	afx_msg void OnIndoorcombat();
	afx_msg void OnOutdoorcombat();
	afx_msg void OnSummonedmonster();
	afx_msg void OnTreasureart();
	afx_msg void OnDeltaposZonespin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNightsounds();
	afx_msg void OnBackgroundsounds();
	virtual void OnOK();
	afx_msg void OnAvart();
  afx_msg void OnZoneDataQuestion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZONEDLG_H__494CCDF2_4362_11D2_AC08_00C04F8E081C__INCLUDED_)
