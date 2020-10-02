/******************************************************************************
* Filename: SpellEditor.cpp
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
#include "SpellEditor.h"
//#include "SpellSpecialConfig.h"

//#ifdef _DEBUG
#include "SpellDBDlgEx.h"
//#else
//#include "SpellDBDlg.h"
//#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpellEditor dialog


CSpellEditor::CSpellEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CSpellEditor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpellEditor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_pSpellData=NULL;
}


void CSpellEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpellEditor)
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpellEditor, CDialog)
	//{{AFX_MSG_MAP(CSpellEditor)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_BN_CLICKED(IDC_ADDSPELL, OnAddspell)
	ON_BN_CLICKED(IDC_DELETESPELL, OnDeletespell)
	ON_BN_CLICKED(IDC_EDITSPELL, OnEditspell)
	ON_BN_CLICKED(IDC_COPYLEVEL, OnCopylevel)
	ON_BN_CLICKED(IDC_PASTELEVEL, OnPastelevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpellEditor message handlers

void CSpellEditor::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnEditspell();	
	*pResult = 0;
}

void CSpellEditor::OnAddspell() 
{
  ASSERT(m_pSpellData!=NULL);
  CString msg;
  bool success;
  SET_MODIFIED;

  SPELL_DATA data;
  //data.schoolMask = MagicUserFlag;
  //data.CastMask = MagicUserFlag;
  data.Name = "New Spell";
  data.Level = 1;
  CSpellDBDlgEx dlg(data);

  do 
  {
    SPELL_ID spellID;
    success=false;
    if (dlg.DoModal()!=IDOK) break;
    success=true;
    //if (m_pSpellData->FindName(dlg.m_SpellData.Name).IsValidSpell()) 
    spellID = dlg.m_SpellData.Name;
    if (m_pSpellData->LocateSpell(spellID) >= 0) 
    {
      msg.LoadString(IDS_HELP_spell_DupName);
      MsgBoxError(msg);
      success=false;
    };
  } while (success==false);

  if (success)
  {
    int spellIndex;
    spellIndex = m_pSpellData->AddSpell(&dlg.m_SpellData);    
    AddToAvailList(spellIndex);

    // this would be nice, but it alters all spell keys,
    // so all spell references would need to be updated also.
    /*    
    //m_pSpellData->SortSpells();
    m_AvailList.DeleteAllItems();
    POSITION pos = m_pSpellData->GetHeadPosition();
    while (pos != NULL)
      AddToAvailList(m_pSpellData->GetNext(pos));
    */
  }
}



void CSpellEditor::OnDeletespell() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    SET_MODIFIED;
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    //GLOBAL_SPELL_ID flag;
    SPELL_ID spellID;
    //flag.UniqueIdentifier(m_AvailList.GetItemData(nItem));
    spellID = m_AvailList.GetItemText(nItem,2);
    m_pSpellData->RemoveSpell(spellID);
  
    m_AvailList.DeleteAllItems();
    int a_count = 0;
    //POSITION spos = m_pSpellData->GetHeadPosition();
    //while (spos != NULL)
    {
      int i, n;
      n = m_pSpellData->GetCount();
      for (i=0;  i<n; i++)
      {
        //AddToAvailList(m_pSpellData->GetNext(spos));
        AddToAvailList(i);
        a_count++;
      };
    };

    if (a_count > 0)
    {
      //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_AvailList.EnsureVisible(0, FALSE);
      m_AvailList.SelectItem(0, TRUE);
    }
  }
}

void CSpellEditor::OnEditspell() 
{
  ASSERT(m_pSpellData!=NULL);
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    //GLOBAL_SPELL_ID flag;
    SPELL_ID spellID;
    //flag.UniqueIdentifier(m_AvailList.GetItemData(nItem));
    spellID = m_AvailList.GetItemText(nItem,2);
    //SPELL_DATA *pData = m_pSpellData->GetSpellData(flag);
    const SPELL_DATA *pData = m_pSpellData->PeekSpell(spellID);

    SPELL_DATA data;
    if (pData == NULL)
      data.Name = "Invalid Spell";
    else
      data = *pData;
    
    CSpellDBDlgEx dlg(data);

    if (dlg.DoModal() == IDOK)
    {
      SET_MODIFIED;
      //m_pSpellData->SetSpell(flag, dlg.m_SpellData);
      m_pSpellData->SetSpell(spellID, dlg.m_SpellData);
      UpdateAvailList(nItem, dlg.m_SpellData);
    }
  }	
}


BOOL CSpellEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_AvailList.DeleteAllItems();

	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Level";
	lvc.cx = 30;
	lvc.fmt = LVCFMT_LEFT;
	m_AvailList.InsertColumn(0,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Type";
	lvc.cx = 75;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 2;
	lvc.pszText = "Name";
	lvc.cx = 700;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(2,&lvc, CListSort::dtSTRING, true);	

  int a_count = 0;
  //POSITION pos = m_pSpellData->GetHeadPosition();
  //while (pos != NULL)
  {
    int i, n;
    n = m_pSpellData->GetCount();
    for (i=0; i<n; i++)
    {
//#ifdef _DEBUG
      //WriteDebugString("Spell \'%s\', key %u\n", 
      WriteDebugString("Spell '%s'\n", 
                       //m_pSpellData->PeekAtPos(pos).Name,m_pSpellData->PeekAtPos(pos).m_gsID.IllegalConversion());
                       //m_pSpellData->PeekSpellData(pos)->Name);
                       m_pSpellData->PeekSpell(i)->Name);
//#endif
      //AddToAvailList(m_pSpellData->GetNext(pos));
      AddToAvailList(i);
      a_count++;
    };
  };

  if (a_count > 0)
  {
    // this multi-sort makes the output look nicely ordered, but it can get
    // slow if the list of spells becomes large. Leave it until someone complains.
    m_AvailList.SetDefaultSortAttributes(2, CSortableListCtrl::sdASCENDING);
    m_AvailList.Sort();
    m_AvailList.SetDefaultSortAttributes(1, CSortableListCtrl::sdASCENDING);
    m_AvailList.Sort();
    m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
    m_AvailList.Sort();

    //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_AvailList.EnsureVisible(0, FALSE);
    m_AvailList.SelectItem(0, TRUE);
  }
  else
  {
     m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
  }

  UpdateData(FALSE);  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpellEditor::AddToAvailList(int spellIndex)
{
  int listIdx = m_AvailList.GetItemCount();

  char level[3];
  char type[200];
  char name[MAX_SPELL_NAME+1];
  
  LVITEM item;
  const SPELL_DATA *pSpell;
  pSpell = m_pSpellData->PeekSpell(spellIndex);
  strncpy(name, pSpell->Name, MAX_SPELL_NAME);
  name[MAX_SPELL_NAME]='\0';
  sprintf(level, "%u", pSpell->Level);
  //strcpy(type, ClassFlagToText(data.schoolMask));
  strcpy(type, pSpell->schoolID);

	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = level;
	item.cchTextMax = 0;
	item.iImage = 0;
  item.lParam = spellIndex;
	// PRS Jul 2009item.lParam = data.m_gsID.m_key;
	//item.lParam = data.m_gsID.UniqueIdentifier();


  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemText(listIdx, 1, type); 
  m_AvailList.SetItemText(listIdx, 2, name); 
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}

void CSpellEditor::UpdateAvailList(int item, SPELL_DATA &data)
{
  char level[3];
  char type[200];
  char name[MAX_SPELL_NAME+1];

  sprintf(level, "%u", data.Level);
  //strcpy(type, ClassFlagToText(data.schoolMask));
  strcpy(type, data.schoolID);
  strncpy(name, data.Name, MAX_SPELL_NAME);
  name[MAX_SPELL_NAME]='\0';

  m_AvailList.SetItemText(item, 0, level); 
  m_AvailList.SetItemText(item, 1, type); 
  m_AvailList.SetItemText(item, 2, name); 
  //m_AvailList.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(item, TRUE);
}

void CSpellEditor::OnCopylevel() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos == NULL) return;
  int nItem = m_AvailList.GetNextSelectedItem(pos);
  //m_CopyLevel.UniqueIdentifier(m_AvailList.GetItemData(nItem));
  int indx = m_AvailList.GetItemData(nItem);
  m_CopySpellID = m_pSpellData->PeekSpell(indx)->SpellID();
  
  CWnd *pWnd = GetDlgItem(IDC_PASTELEVEL);
  //if (m_pSpellData->HaveSpell(m_CopyLevel))
  if (m_pSpellData->LocateSpell(m_CopySpellID) >= 0)
  {
    if (pWnd!=NULL) pWnd->EnableWindow(TRUE);
  }
  else
  {
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
  }	
}

void CSpellEditor::OnPastelevel() 
{
  //if (!m_CopyLevel.IsValidSpell()) return;
  if (!m_CopySpellID.IsValidSpell()) return;
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos == NULL) return;
  int nItem = m_AvailList.GetNextSelectedItem(pos);
  //GLOBAL_SPELL_ID pasteLevel;
  //pasteLevel.UniqueIdentifier(m_AvailList.GetItemData(nItem));
  //pasteLevel = m_AvailList.GetItemText(nItem, 2);
  int pasteIndx = m_AvailList.GetItemData(nItem);
  SPELL_ID pasteSpellID = m_pSpellData->PeekSpell(pasteIndx)->SpellID();
  if (!m_pSpellData->HaveSpell(pasteSpellID)) return;
  
  // copy src item to dest item
  //SPELL_DATA *srcItem = m_pSpellData->GetSpellData(m_CopyLevel);
  //SPELL_DATA *dstItem = m_pSpellData->GetSpellData(pasteLevel);
  SPELL_DATA *srcItem = m_pSpellData->GetSpell(m_CopySpellID);
  SPELL_DATA *dstItem = m_pSpellData->GetSpell(pasteSpellID);
  if (srcItem==NULL) return;
  if (dstItem==NULL) return;
  /*
  CWaitCursor cursor; // this name search may take awhile...
  DWORD index=1;
  BOOL validname=false;
  CString name;
  while (!validname)
  {
    name.Format("%s/%u", srcItem->Name, index);
    if (name == dstItem->Name)
      validname=true;
    else
      validname = (!m_pSpellData->FindName(name).IsValidSpell());
    if (index >= MAX_SPELLS)
      break;
    index++;
  }
  
  if (validname)
  {
    SPELL_DATA newData = *srcItem;
    newData.Name=name;
    newData.m_gsID = pasteLevel;

    newData.CompileScripts();

    m_pSpellData->SetSpell(pasteLevel, newData);
    UpdateAvailList(nItem, newData);
  }	
  */
  {
    SPELL_DATA newData = *srcItem;
    newData.Name = dstItem->Name;
    //newData.m_gsID = pasteLevel;
    newData.CompileScripts();
    m_pSpellData->SetSpell(pasteSpellID, newData);
    UpdateAvailList(nItem, newData);
  };
}
