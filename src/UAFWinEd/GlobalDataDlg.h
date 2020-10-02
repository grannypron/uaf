/******************************************************************************
* Filename: GlobalDataDlg.h
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
#ifndef __GLOBAL_DATA_DLG_H__
#define __GLOBAL_DATA_DLG_H__


#include "GlobalData.h"

class CGlobalDataDlg : public CDialog
{
// Construction
public:
	CGlobalDataDlg(GLOBAL_STATS& data, CWnd* pParent = NULL);   // standard constructor
  void GetData(GLOBAL_STATS& data);
  void FetchData(void);

// Dialog Data
	//{{AFX_DATA(CGlobalDataDlg)
	enum { IDD = IDD_GLOBALDATA };
	CButton	m_MouseCursor;
	CButton	m_ScreenBG;
  CButton m_AreaViewArt;
  CButton m_Credits;
	CButton	m_IconBG;
	CButton	m_CombatDying;
	CButton	m_CombatDeath;
	CButton	m_VBorder;
	CButton	m_HBorder;
	CButton	m_Frame;
	CButton	m_CombatVBorder;
	CButton	m_CombatHBorder;
	CButton	m_CombatCommon;
	CButton	m_FontSelect;
	CComboBox	m_StartLevel;
	CString	m_DesignName;
	int		m_Facing;
	int		m_StartExp;
	int		m_StartGems;
	int		m_StartJewelry;
	int		m_StartPlatinum;
	int		m_StartTime;
	int		m_StartX;
	int		m_StartY;
	UINT	m_DSTime;
	UINT	m_DTime;
	UINT	m_WSTime;
	UINT	m_WTime;
	BOOL	m_AllowCharCreate;
  BOOL  m_DeadAtZeroHP;
	int		m_MaxPartySize;
	int		m_MinPCs;
  int   m_MaxPCs;
	BOOL	m_UseAutoDarken;
	int		m_AutoDarkenAmount;
	int		m_DarkenEndTime;
	int		m_DarkenStartTime;
	CString	m_CoinName;
	CString	m_GemName;
	CString	m_JewelryName;
	int		m_StartExpType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGlobalDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  GLOBAL_STATS m_data;

	// Generated message map functions
	//{{AFX_MSG(CGlobalDataDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelectcombatcommon();
	afx_msg void OnSelectcombathborder();
	afx_msg void OnSelectcombatvborder();
	afx_msg void OnSelectframe();
	afx_msg void OnSelecthborder();
	afx_msg void OnSelectvborder();
	afx_msg void OnSelchangeStartlevel();
	afx_msg void OnSounds();
	afx_msg void OnFontselect();
	afx_msg void OnCombatdying();
	afx_msg void OnCombatdeath();
	afx_msg void OnStartequip();
	afx_msg void OnConfigmoney();
	afx_msg void OnSelecticonbg();
	afx_msg void OnScreenbg();
  afx_msg void OnAreaViewArt();
	afx_msg void OnCredits();
	afx_msg void OnDifflvl();
	afx_msg void OnEditglobalattributes();
	afx_msg void OnMousecursor();
	afx_msg void OnSelecttitles();
	afx_msg void OnFixspells();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif