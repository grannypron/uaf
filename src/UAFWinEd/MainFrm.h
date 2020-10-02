/******************************************************************************
* Filename: MainFrm.h
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
#if !defined(AFX_MAINFRM_H__56306A0B_E92E_11D3_891D_00105AA4563A__INCLUDED_)
#define AFX_MAINFRM_H__56306A0B_E92E_11D3_891D_00105AA4563A__INCLUDED_

#define MSG_CLOSESEARCHDLG  0xB403

class CGlobalDataDlg;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

    enum { ID_DOSCREENCAP=0xB401 };

// Attributes
public:

// Operations
public:
   void SaveUAFolder();
   void RestoreUAFolder();
   void SaveState();
   void SaveAs(const CString& suggestedDesignDir);
   void RestoreState();
   BOOL LoadDesign(LPCSTR name, int startLvl=-1);
   CStatusBar  m_wndStatusBar;

   BOOL LoadTemplateDesign(void);
   BOOL ConfigureNewDesign(CGlobalDataDlg *dlg);

   BOOL SaveNewDesign(void);



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar    m_wndToolBar;

  afx_msg LRESULT OnMsgCloseSearchDlg(WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnDoScreenCap(WPARAM, LPARAM);
  
// Generated message map functions
public:
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnItemsEvent();
	afx_msg void OnUpdateItemsEvent(CCmdUI* pCmdUI);
	afx_msg void OnItemsStartloc();
	afx_msg void OnUpdateItemsStartloc(CCmdUI* pCmdUI);
	afx_msg void OnItemsWall();
	afx_msg void OnUpdateItemsWall(CCmdUI* pCmdUI);
	afx_msg void OnItemZone();
	afx_msg void OnUpdateItemZone(CCmdUI* pCmdUI);
	afx_msg void OnItemsBackdrop();
	afx_msg void OnUpdateItemsBackdrop(CCmdUI* pCmdUI);
	afx_msg void OnViewBackdrops();
	afx_msg void OnViewWallsets();
	afx_msg void OnViewZones();
	afx_msg void OnFileOpenMap();
	afx_msg void OnPassthruwalls();
	afx_msg void OnUpdatePassthruwalls(CCmdUI* pCmdUI);
	afx_msg void OnViewChars();
	afx_msg void OnItemsAccess();
	afx_msg void OnUpdateItemsAccess(CCmdUI* pCmdUI);
	afx_msg void OnViewEntrypoints();
	afx_msg void OnViewGlobaldata();
	afx_msg void OnViewKeys();
	afx_msg void OnViewLeveldata();
	afx_msg void OnViewareamap();
	afx_msg void OnUpdateViewareamap(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAsMap();
	afx_msg void OnFileSaveMap();
  afx_msg void OnUpdateFileSaveMap(CCmdUI * pCmdUI);
	afx_msg void OnViewSpicPool();
	afx_msg void OnViewIconPool();
	afx_msg void OnEditItems();
	afx_msg void OnEditMonsters();
	afx_msg void OnEditSpells();
	afx_msg void OnEditSpecialAbilities();
	afx_msg void OnTestSpecialAbilities();
	afx_msg void OnImportexport();
	afx_msg void OnCheckSkills();
	afx_msg void OnItemsEntrypoint();
	afx_msg void OnUpdateItemsEntrypoint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewEntrypoints(CCmdUI* pCmdUI);
	afx_msg void OnClearLeveldata();
	afx_msg void OnUpdateClearLeveldata(CCmdUI* pCmdUI);
	afx_msg void OnViewStepevents();
	afx_msg void OnUpdateViewStepevents(CCmdUI* pCmdUI);
	afx_msg void OnClearDesigndata();
	afx_msg void OnFileNewMap();
	afx_msg void OnDoublefillwallslot();
	afx_msg void OnUpdateDoublefillwallslot(CCmdUI* pCmdUI);
	afx_msg void OnViewEvents();
	afx_msg void OnUpdateViewEvents(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUndolast();
	afx_msg void OnUpdateUndolast(CCmdUI* pCmdUI);
	afx_msg void OnValidateDesign();
	afx_msg void OnGameVersion();
	afx_msg void OnImportfrua();
	afx_msg void OnUpdateBLOCKType(CCmdUI* pCmdUI);
	afx_msg void OnBLOCKType();
	afx_msg void OnUpdateKwikKlik(CCmdUI* pCmdUI);
	afx_msg void OnKwikKlik();
	afx_msg void OnUpdateValidateDesign(CCmdUI* pCmdUI);
	afx_msg void OnTilemapdata();
	afx_msg void OnUpdateTilemapdata(CCmdUI* pCmdUI);
//	afx_msg void OnEditConfig();
//	afx_msg void OnReloadConfig();
	afx_msg void OnHomepage();
	afx_msg void OnGlobalTestsaveddesign();
	afx_msg void OnGlobalTestsaveddesignfromstart();
	afx_msg void OnUpdateViewWallsets(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewBackdrops(CCmdUI* pCmdUI);
	afx_msg void OnDownloads();
	afx_msg void OnLogEventText();
	afx_msg void OnUpdateLogEventText(CCmdUI* pCmdUI);
	afx_msg void OnQuadfillbgslot();
	afx_msg void OnUpdateQuadfillbgslot(CCmdUI* pCmdUI);
	afx_msg void OnFillallmapdistantbgoff();
	afx_msg void OnFillallmapdistantbgon();
	afx_msg void OnFillallmapzone();
	afx_msg void OnUpdateFillallmapdistantbgon(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFillallmapdistantbgoff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFillallmapzone(CCmdUI* pCmdUI);
	afx_msg void OnToggleTopmost();
	afx_msg void OnUpdateToggleTopmost(CCmdUI* pCmdUI);
	afx_msg void OnLaunchuaconfig();
	afx_msg void OnFillallbackdropAlldir();
	afx_msg void OnUpdateFillallbackdropAlldir(CCmdUI* pCmdUI);
	afx_msg void OnFillallbackdropEastdir();
	afx_msg void OnFillallbackdropNorthdir();
	afx_msg void OnFillallbackdropSouthdir();
	afx_msg void OnFillallbackdropWestdir();
	afx_msg void OnUpdateFillallbackdropWestdir(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFillallbackdropSouthdir(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFillallbackdropNorthdir(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFillallbackdropEastdir(CCmdUI* pCmdUI);
	afx_msg void OnFillunusedbackdropAlldir();
	afx_msg void OnUpdateFillunusedbackdropAlldir(CCmdUI* pCmdUI);
	afx_msg void OnFillunusedbackdropEastdir();
	afx_msg void OnUpdateFillunusedbackdropEastdir(CCmdUI* pCmdUI);
	afx_msg void OnFillunusedbackdropNorthdir();
	afx_msg void OnUpdateFillunusedbackdropNorthdir(CCmdUI* pCmdUI);
	afx_msg void OnFillunusedbackdropSouthdir();
	afx_msg void OnUpdateFillunusedbackdropSouthdir(CCmdUI* pCmdUI);
	afx_msg void OnFillunusedbackdropWestdir();
	afx_msg void OnUpdateFillunusedbackdropWestdir(CCmdUI* pCmdUI);
	afx_msg void OnAppHelpcontents();
	afx_msg void OnGlobalViewartastransparent();
	afx_msg void OnEditoroptions();
	afx_msg void OnUpdateLaunchuaconfig(CCmdUI* pCmdUI);
	afx_msg void OnLaunchdcconfig();
	afx_msg void OnUpdateLaunchdcconfig(CCmdUI* pCmdUI);
	afx_msg void OnGameStats();
	afx_msg void OnViewjournal();
	afx_msg void OnUpdateGlobalTestsaveddesign(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGlobalTestsaveddesignfromstart(CCmdUI* pCmdUI);
	afx_msg void OnAppScreencapture();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGlobalSettestparty();
	afx_msg void OnGlobalTestsaveddesignViewerrorlog();
	afx_msg void OnGameErrorlog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__56306A0B_E92E_11D3_891D_00105AA4563A__INCLUDED_)
