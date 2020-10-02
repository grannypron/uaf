/******************************************************************************
* Filename: Dbutils.h
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
#if !defined(AFX_DBUTILS_H__C50E6EC2_48A9_11D2_8D55_00C04F98D8B4__INCLUDED_)
#define AFX_DBUTILS_H__C50E6EC2_48A9_11D2_8D55_00C04F98D8B4__INCLUDED_


/////////////////////////////////////////////////////////////////////////////
// CDbUtils dialog

class CDbUtils : public CDialog
{
// Construction
public:
	CDbUtils(CWnd* pParent = NULL);   // standard constructor
    bool LiveDangerously(void);
// Dialog Data
	//{{AFX_DATA(CDbUtils)
	enum { IDD = IDD_DBUTILS };
	CString	m_SpellSize;
	CString	m_MonsterSize;
	CString	m_ItemSize;
  CString m_SpecAbSize;
	CString	m_ClassSize;
	CString	m_BaseclassSize;
	CString	m_RaceSize;
	CString	m_AbilitySize;
  BOOL    m_testMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbUtils)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDbUtils)
	afx_msg void OnConvertitemdb();
	afx_msg void OnConvertmonsterdb();
	afx_msg void OnConvertspelldb();
	virtual BOOL OnInitDialog();
	afx_msg void OnExportitemdb();
	afx_msg void OnExportbaseclassdb();
	afx_msg void OnExportmonsterdb();
	afx_msg void OnExportspecabdb();
  afx_msg void OnImportSpecAbDB();
	afx_msg void OnExportspelldb();
	afx_msg void OnExportclassdb();
	afx_msg void OnExportracedb();
	afx_msg void OnLoaddefitems();
	afx_msg void OnLoaddefmonsters();
	afx_msg void OnLoaddefspells();
	afx_msg void OnEdititemtext();
	afx_msg void OnEditmonstertext();
	afx_msg void OnEditspelltext();
	afx_msg void OnConvertclassdb();
	afx_msg void OnConvertbaseclassdb();
	afx_msg void OnConvertracedb();
	afx_msg void OnConvertabilitydb();
	afx_msg void OnExportabilitydb();
	afx_msg void OnExportGlobalSettings();
	afx_msg void OnImportGlobalSettings();
	afx_msg void OnExportCharacters();
	afx_msg void OnImportCharacters();
	afx_msg void OnImportalltext();
          BOOL OnHelpInfo(HELPINFO* pHelpInfo);
 	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

void DisplaySpecialAbilities(const SPECIAL_ABILITIES *pSpecialAbilities, const CString& databaseName);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBUTILS_H__C50E6EC2_48A9_11D2_8D55_00C04F98D8B4__INCLUDED_)
