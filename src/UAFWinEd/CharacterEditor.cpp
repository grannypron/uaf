/******************************************************************************
* Filename: CharacterEditor.cpp
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

#include "..\Shared\stdafx.h"
#include "resource.h"

#include "UAFWinEd.h"
#include "GlobText.h"
#include "GlobalData.h"
#include "CharDlg.h"
#include "PicSlot.h"
#include "CharacterEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCharacterEditor dialog


CCharacterEditor::CCharacterEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CCharacterEditor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCharacterEditor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCharacterEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharacterEditor)
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCharacterEditor, CDialog)
	//{{AFX_MSG_MAP(CCharacterEditor)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_BN_CLICKED(IDC_DELETEITEM, OnDeleteitem)
	ON_BN_CLICKED(IDC_EDITITEM, OnEdititem)
	ON_BN_CLICKED(IDC_ADDITEM, OnAdditem)
  ON_BN_CLICKED(IDC_Copy, OnCopy)
  ON_BN_CLICKED(IDC_Paste, OnPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharacterEditor message handlers

void CCharacterEditor::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdititem();		
	*pResult = 0;
}

void CCharacterEditor::OnDeleteitem() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    DWORD flag = m_AvailList.GetItemData(nItem);

    globalData.charData.DeleteCharacterEvents(flag);
    globalData.charData.RemoveCharacter(flag);

    m_AvailList.DeleteAllItems();
    int a_count = 0;
    //pos = globalData.charData.GetHeadPosition();
    //while (pos != NULL)
    int i, n;
    n = globalData.charData.GetCount();
    for (i=0; i<n; i++)
    {
      //AddToAvailList(globalData.charData.GetNext(pos));
      AddToAvailList(i);
      a_count++;
    }

    if (a_count > 0)
    {
      //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      m_AvailList.SelectItem(0, TRUE);
    }	
  }
  SET_MODIFIED;
}

void CCharacterEditor::OnEdititem() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    //DWORD flag = m_AvailList.GetItemData(nItem);
    DWORD indx = m_AvailList.GetItemData(nItem);
    //CCharacter dlg(flag);
    CCharacter dlg(globalData.charData.PeekCharacter(indx)->CharacterID());
    DWORD tempEvent = dlg.m_CharData.TalkEvent;
    DWORD tempEvent2 = dlg.m_CharData.ExamineEvent;

    if (dlg.DoModal() == IDOK)
    {
      //globalData.charData.SetCharacter(flag, &dlg.m_CharData);
      globalData.charData.SetCharacter(indx, &dlg.m_CharData);
      UpdateAvailList(nItem, dlg.m_CharData);
    }
    else
    {
      // cannot undo talk event edits
      if (tempEvent != dlg.m_CharData.TalkEvent)
      {
        SET_MODIFIED;
        // talk event chain was changed, force update in db
        tempEvent = dlg.m_CharData.TalkEvent;
        //globalData.charData.GetCharacterData(flag, dlg.m_CharData);
        //globalData.charData.GetCharacter(flag, dlg.m_CharData);
        //globalData.charData.FetchCharacter(flag, &dlg.m_CharData);
        globalData.charData.FetchCharacter(indx, &dlg.m_CharData);
        dlg.m_CharData.TalkEvent = tempEvent;
        //globalData.charData.SetCharacter(flag, dlg.m_CharData);
        //globalData.charData.SetCharacter(flag, &dlg.m_CharData);
        globalData.charData.SetCharacter(indx, &dlg.m_CharData);
      }
      if (tempEvent2 != dlg.m_CharData.ExamineEvent)
      {
        SET_MODIFIED;
        // event chain was changed, force update in db
        tempEvent2 = dlg.m_CharData.ExamineEvent;
        //globalData.charData.GetCharacterData(flag, dlg.m_CharData);
        //globalData.charData.FetchCharacter(flag, &dlg.m_CharData);
        globalData.charData.FetchCharacter(indx, &dlg.m_CharData);
        dlg.m_CharData.ExamineEvent = tempEvent2;
        //globalData.charData.SetCharacter(flag, dlg.m_CharData);
        //globalData.charData.SetCharacter(flag, &dlg.m_CharData);
        globalData.charData.SetCharacter(indx, &dlg.m_CharData);
      }
    }
  }	
}

void CCharacterEditor::OnAdditem() 
{
  CHARACTER spData;  
  int charIndex;
  spData.SetType(NPC_TYPE);
  spData.generateNewCharacter(GetDesignCharStartExp(),GetDesignCharStartExpType());  
  if (spData.GetMaxHitPoints() == 0)
  {
    MsgBoxError("Failed to create new character\nRace/Class configuration files may have an error");
    return;
  }
  
  spData.SetName("New Character");
  spData.icon.filename = ede.TemplateIconArtDir() + DEFAULT_NPC_ICON;
  spData.icon.picType = IconDib;
  spData.icon.SetDefaults();
  spData.smallPic.filename.Format("%s%s",ede.TemplateSmallPicDir(), DEFAULT_NPC_PIC);
  spData.smallPic.picType = SmallPicDib;
  spData.smallPic.SetDefaults();

  charIndex = globalData.charData.AddCharacter(&spData);
  AddToAvailList(charIndex);
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    DWORD indx = m_AvailList.GetItemData(nItem);
    CHARACTER *pChar;
    CHARACTER_ID characterID;
    pChar = globalData.charData.GetCharacter(indx);
    characterID = pChar->CharacterID();
    CCharacter dlg(characterID);
    if (dlg.DoModal() == IDOK)
    {
      globalData.charData.SetCharacter(indx, &dlg.m_CharData);
      UpdateAvailList(nItem, dlg.m_CharData);
      SET_MODIFIED;
    }
    else
    {
      globalData.eventData.DeleteEvents(dlg.m_CharData.TalkEvent);
      globalData.eventData.DeleteEvents(dlg.m_CharData.ExamineEvent);
      OnDeleteitem();
    }
  }	
}


BOOL CCharacterEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Name";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_LEFT;
	m_AvailList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Race";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 2;
	lvc.pszText = "Class";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(2,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 3;
	lvc.pszText = "Hitpoints";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(3,&lvc, CListSort::dtINT, true);

	m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  //POSITION pos = globalData.charData.GetHeadPosition();
  //while (pos != NULL)
  {
    int i, n;
    n = globalData.charData.GetCount();
    for (i=0; i<n; i++)
    {
      //AddToAvailList(globalData.charData.GetNext(pos));
      AddToAvailList(i);
    };
  };
 
  if (m_AvailList.GetItemCount() > 0)
  {
    m_AvailList.Sort();
    m_AvailList.SelectItem(0, TRUE);
  }

  CWnd *pWnd = GetDlgItem(IDC_Paste);
  if (pWnd!=NULL) pWnd->EnableWindow(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCharacterEditor::AddToAvailList(int charIndex)
{
  int listIdx = m_AvailList.GetItemCount();

  char name[MAX_ITEM_NAME+1];
  char race[MAX_ITEM_NAME+1];
  char charClass[50];
  char hp[50];

  CHARACTER *pChar;

  pChar = globalData.charData.GetCharacter(charIndex);

  //strncpy(name, data.GetName(), MAX_ITEM_NAME);
  strncpy(name, pChar->GetName(), MAX_ITEM_NAME);
  name[MAX_ITEM_NAME]='\0';
  //strcpy(race, RaceText[data.race()]);
  strcpy(race, pChar->RaceID());
  //prs 2 dec 2009
  //strcpy(charClass, ClassText[data.GetClass()]);
  strcpy(charClass, pChar->GetClass());

  //sprintf(hp, "%i", data.GetMaxHitPoints());
  sprintf(hp, "%i", pChar->GetMaxHitPoints());

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = name;
	item.cchTextMax = 0;
	item.iImage = 0;

	//item.lParam = data.uniqueKey;
  item.lParam = charIndex;

  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemText(listIdx, 1, race); 
  m_AvailList.SetItemText(listIdx, 2, charClass); 
  m_AvailList.SetItemText(listIdx, 3, hp);   
  
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  //m_AvailList.EnsureVisible(listIdx, FALSE);
  m_AvailList.SelectItem(listIdx, TRUE);
}

void CCharacterEditor::UpdateAvailList(int item, CHARACTER &data)
{
  char name[MAX_ITEM_NAME+1];
  char race[MAX_ITEM_NAME+1];
  char charClass[50];
  char hp[50];

  strncpy(name, data.GetName(), MAX_ITEM_NAME);
  name[MAX_ITEM_NAME]='\0';
  //strcpy(race, RaceText[data.race()]);
  strcpy(race, data.RaceID());

  // 2 Dec 2009 PRS
  //strcpy(charClass, ClassText[data.GetClass()]);
  strcpy(charClass, data.GetClass());

  
  sprintf(hp, "%i", data.GetMaxHitPoints());

  m_AvailList.SetItemText(item, 0, name); 
  m_AvailList.SetItemText(item, 1, race); 
  m_AvailList.SetItemText(item, 2, charClass); 
  m_AvailList.SetItemText(item, 3, hp); 

  //m_AvailList.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
  //m_AvailList.EnsureVisible(item, FALSE);
  m_AvailList.SelectItem(item, TRUE);
}


CHARACTER copyCharacter, *pChar;

void CCharacterEditor::OnCopy() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos == NULL) return;

  int n = m_AvailList.GetNextSelectedItem(pos);
  //DWORD flag = m_AvailList.GetItemData(n);
  int index = m_AvailList.GetItemData(n);
  //pChar = globalData.charData.GetCharacterData(flag);
  pChar = globalData.charData.GetCharacter(index);
  copyCharacter = *pChar;
  CWnd *pWnd = GetDlgItem(IDC_Paste);
  if (pWnd!=NULL) pWnd->EnableWindow(TRUE);

}

void CCharacterEditor::OnPaste() 
{

  //if (copyCharacter.uniqueKey <= 0) return;


  int indx;
  indx = globalData.charData.AddCharacter(&copyCharacter);
  //AddToAvailList(copyCharacter);
  AddToAvailList(indx);
  SET_MODIFIED;
}

