/******************************************************************************
* Filename: EventViewer.h
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
#include "afxwin.h"
#if !defined(AFX_EVENTVIEWER_H__B7578C93_70F3_11D3_88B6_00105AA4563A__INCLUDED_)
#define AFX_EVENTVIEWER_H__B7578C93_70F3_11D3_88B6_00105AA4563A__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CEventViewer dialog

class CEventViewer : public CDialog
{
// Construction
public:
	CEventViewer(int x, int y, CWnd* pParent = NULL); // standard constructor
  CEventViewer(CString label, DWORD event, CWnd* pParent=NULL);

  DWORD m_SingleEventRoot;

// Dialog Data
	//{{AFX_DATA(CEventViewer)
	enum { IDD = IDD_EVENTVIEWER };
	CButton	m_SpellTrigger;
	CButton	m_GPDLTrigger;
	CButton	m_NPCTrigger;
	CButton	m_Paste;
	CButton	m_Copy;
	CComboBox	m_Gender;
	CButton	m_ConfigZones;
	CButton	m_ConfigStepEvents;
	CComboBox	m_EventType;
	CButton	m_SpKeyTrigger;
	CButton	m_SpItemTrigger;
	CButton	m_ItemTrigger;
	CButton	m_QuestTrigger;
	CComboBox	m_EventTrigger;
	CComboBox	m_Race;
	CComboBox	m_Facing;
	CComboBox	m_Class;
	CComboBox	m_ChainTrigger;
	CButton	m_Insert;
	CButton	m_Delete;
	CButton	m_Edit;
	CTreeCtrl	m_EventTree;
	BOOL	m_OnceOnly;
	int		m_Chance;
	BOOL	m_DeleteAllChained;
	int		m_PartyX; // Also used for quest stage for Quest Stage triggers
	int		m_PartyY;
  CButton m_Attributes;
  //}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventViewer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  int m_x;
  int m_y;
  HTREEITEM m_hCurrItem;
  GameEvent *m_CurrEvent;
  DWORD m_SelectedEventId;
  BYTE  m_CurrFlags;
  BOOL  m_SingleEvent;
  CString m_SingleEventRootLabel;
  CMap<HTREEITEM,HTREEITEM,DWORD,DWORD&> m_MapXY;
  
  enum 
  { 
    NOT_A_CHAIN          = 0x0F,

    // flags
    IS_STEP_EVENT        = 0x10,
    IS_STEP_EVENT_ROOT   = 0x20,
    IS_REST_EVENT_ROOT   = 0x30,
    IS_REST_EVENT        = 0x40,
    IS_SINGLE_EVENT_ROOT = 0x50,
    IS_SINGLE_EVENT      = 0x60,
    IS_TIME_EVENT_ROOT   = 0x70,
    IS_TIME_EVENT        = 0x80
  }; 

  void  PopulateTree(DWORD SelectedEventId);
  void  PopulateTreeFromRootEvent(GameEvent *event);
  void  PopulateTreeChildren(HTREEITEM &rootTree, GameEvent *rootEvent, BYTE flags=0);  
  HTREEITEM AddBlankRoot(int x, int y);
  void  PopulateRestEvents();
  void  PopulateStepEvents();
  void  PopulateTimeEvents();
  void  PopulateSingleEvents();

  void  SetControlStates(void);
  void  PopulateClassBaseclassList(GameEvent *pEvent);
  void  UpdateControls(GameEvent *event);
  void  GetControls(GameEvent *event);
  void  SetControlVisibility(int id, bool show);
  void  ShowControl(int id) {SetControlVisibility(id, true);};
  void  HideControl(int id) {SetControlVisibility(id, false);};
  void  ShowControls(int id1, int id2)
  {
    ShowControl(id1);
    ShowControl(id2);
  };
  void  HideControls(int id1, int id2)
  {
    HideControl(id1);
    HideControl(id2);
  };

  DWORD GetEventId(HTREEITEM hCurr);
  DWORD GetParentEventId(HTREEITEM hCurr);
  BYTE  GetChainIndex(HTREEITEM hCurr);
  BYTE  GetFlags(HTREEITEM hCurr);

  DWORD MakeEventKey(DWORD EventId, BYTE ChainIndex, BYTE Flags=0);
  void  SplitEventKey(DWORD key, DWORD &EventId, BYTE &ChainIndex, BYTE &Flags);

  void  LookupCurrEvent();
  UINT  EditEvent(GameEvent *event);
  void  CreateNewEvent();

  void ExpandChildren(HTREEITEM hParent);
  void CollapseChildren(HTREEITEM hParent);

  afx_msg LRESULT OnMsgFindNext(WPARAM wp, LPARAM lp);
  HTREEITEM FindItemFromString(HTREEITEM hItem, LPCTSTR szItemName, WORD type);
  HTREEITEM AdvanceToNextSearchItem(HTREEITEM hCurrItem);
  HTREEITEM AdvanceToNextSearchTree(HTREEITEM hCurrItem);
  HTREEITEM GetTreeRoot(HTREEITEM hCurrItem);

	// Generated message map functions
	//{{AFX_MSG(CEventViewer)
	afx_msg void OnInsertevent();
	afx_msg void OnEditevent();
	afx_msg void OnDeleteevent();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkEventtree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTriggerspeckey();
	afx_msg void OnTriggerspecitem();
	afx_msg void OnTriggeritem();
	afx_msg void OnQuesttrigger();
	afx_msg void OnSelchangeChaintrigger();
	afx_msg void OnSelchangedEventtree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingEventtree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeEventtrigger();
	afx_msg void OnConfigstepevents();
	afx_msg void OnConfigzones();
	afx_msg void OnDeleteallchained();
	afx_msg void OnExpandall();
	afx_msg void OnCollapsall();
	afx_msg void OnPasteevents();
	afx_msg void OnCopyevents();
	afx_msg void OnNpctrigger();
	afx_msg void OnGpdltrigger();
	afx_msg void OnSearch();
	afx_msg void OnSpelltrigger();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedAttributes();
  afx_msg void OnBnClickedQuestStage();
  int m_questStage;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTVIEWER_H__B7578C93_70F3_11D3_88B6_00105AA4563A__INCLUDED_)
