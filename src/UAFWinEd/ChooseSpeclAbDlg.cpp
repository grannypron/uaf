/******************************************************************************
* Filename: ChooseSpeclAbDlg.cpp
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
#include "EditText.h"
#include "SpecAbMsgs.h"
#include "GPDLOpCodes.h"
#include "gpdlcomp.h"
#include "ChooseSpeclAbDlg.h"
#include "AttibuteDataEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CString emptyString;
extern CString ActivationScriptName;
extern CString DeActivationScriptName;

#define typeCol 1
#define valueCol 2


#ifdef OLD_SPECIAL_ABILITIES

/////////////////////////////////////////////////////////////////////////////
// CChooseSpeclAbDlg dialog


CChooseSpeclAbDlg::CChooseSpeclAbDlg(const SPECIAL_ABILITIES &sa, const CString& title, CWnd* pParent /*=NULL*/)
	: CDialog(CChooseSpeclAbDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseSpeclAbDlg)
	//}}AFX_DATA_INIT
  m_sa.Copy(sa);
  m_ReadOnly=FALSE;
  m_NoMsgTypes=FALSE;
  m_title = title;
}


void CChooseSpeclAbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseSpeclAbDlg)
	DDX_Control(pDX, IDC_SPECABLIST, m_SpecAbList);
	DDX_Control(pDX, IDC_SPECABCOMBO, m_SpecAbCombo);
	//}}AFX_DATA_MAP
  m_MsgCol=0;
  m_ScriptCol=0;
}


BEGIN_MESSAGE_MAP(CChooseSpeclAbDlg, CDialog)
	//{{AFX_MSG_MAP(CChooseSpeclAbDlg)
	ON_BN_CLICKED(IDC_ADDSPECAB, OnAddspecab)
	ON_BN_CLICKED(IDC_DELETESPECAB, OnDeletespecab)
	ON_BN_CLICKED(IDC_EDITSPECAB, OnEditspecab)
	ON_NOTIFY(NM_DBLCLK, IDC_SPECABLIST, OnDblclkSpecablist)
	ON_BN_CLICKED(IDC_EDITMSGTYPES, OnEditmsgtypes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseSpeclAbDlg message handlers

BOOL CChooseSpeclAbDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  if (m_ReadOnly) 
    m_NoMsgTypes=TRUE;

  CWnd *pwnd;
  if (m_ReadOnly)
  {    
    pwnd = GetDlgItem(IDC_EDITSPECAB);
    if (pwnd!=NULL)
    {
      pwnd->EnableWindow(FALSE);
      pwnd->ShowWindow(SW_HIDE);
    }
  }

  // only items can change/use msg types
  if (m_NoMsgTypes)
  {
    pwnd = GetDlgItem(IDC_EDITMSGTYPES);
    if (pwnd!=NULL)
    {
      pwnd->EnableWindow(FALSE);
      pwnd->ShowWindow(SW_HIDE);
    }
  }

  m_SpecAbCombo.ResetContent();
  for (int a=1;a<NUM_SPECIAL_ABILITIES;a++)
  {
    int index = m_SpecAbCombo.AddString(spellAbilitiesText[a]);
    m_SpecAbCombo.SetItemData(index, a);
  }

  m_SpecAbCombo.SetCurSel(0);

	// insert columns
	LV_COLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

  int col=0;

	lvc.iSubItem = col;
	lvc.pszText = "Name";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_LEFT;
	m_SpecAbList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

  col++;

  if (!m_NoMsgTypes)
  {
	  lvc.iSubItem = col;
	  lvc.pszText = "Over-Ride Combat Msg Types";
	  lvc.cx = 120;
	  lvc.fmt = LVCFMT_LEFT;
    m_SpecAbList.InsertColumn(col,&lvc, CListSort::dtSTRING, true);
    m_MsgCol = col;
    col++;
  }

  if (!m_ReadOnly)
  {
	  lvc.iSubItem = col;
	  lvc.pszText = "Script";
	  lvc.cx = 180;
	  lvc.fmt = LVCFMT_LEFT;
    m_SpecAbList.InsertColumn(col,&lvc, CListSort::dtSTRING, true);
    m_ScriptCol=col;
    col++;
  }

	m_SpecAbList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  int count=0;
  POSITION pos = m_sa.GetHeadPosition();
  //for (int i=0; i<m_sa.specAbs.GetSize(); i++)
  while (pos != NULL)
  {
    AddSpecialAbilityToList(m_sa.GetNext(pos));
    count++;
  }

  if (count > 0)
  {
    m_SpecAbList.Sort();
    //m_SpecAbList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_SpecAbList.EnsureVisible(0, FALSE);
    m_SpecAbList.SelectItem(0, TRUE);
  }
  SetWindowText(m_title);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChooseSpeclAbDlg::OnOK() 
{
	UpdateData(TRUE);

	CDialog::OnOK();
}

void CChooseSpeclAbDlg::OnAddspecab() 
{
  int sel = m_SpecAbCombo.GetCurSel();
  if (sel<0) return;
  specialAbilitiesType sa = (specialAbilitiesType)m_SpecAbCombo.GetItemData(sel);
  m_sa.EnableSpecAb(sa); 
  AddSpecialAbilityToList(sa);
}


BOOL CChooseSpeclAbDlg::AddSpecialAbilityToList(specialAbilitiesType sa)
{
  // no duplicates
  for (int i=0;i<m_SpecAbList.GetItemCount();i++)
  {
    if (((specialAbilitiesType)m_SpecAbList.GetItemData(i))==sa)
      return FALSE;
  }

  CString spec("");
  CString msgs("");
  CString activ("");
  LVITEM item;
  spec = spellAbilitiesText[sa];

  int count=0;
  DWORD types = m_sa.GetMsgTypes(sa);
  for (int t=1;t<NUM_SPECAB_MSGS;t++)
  {
    DWORD flag = 1<<t;
    if (types & flag)
    {
      msgs += SpecAbMsgText[ConvertSpecAbFlagToIndex(flag)];
      msgs += ",";
      count++;
    }
  }
  if (count==0)
    msgs=SpecAbMsgText[0];
  
  activ = m_sa.GetSpecAbUserScript(sa); //GetSpecAbScript(sa);
  activ.Remove('\r');
  activ.Remove('\n');
  activ.Remove('\t');

  int listIdx = m_SpecAbList.GetItemCount();

	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = spec.GetBuffer(spec.GetLength()+1);
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = (int)sa; 

  m_SpecAbList.InsertItem(&item);  

  if (!m_NoMsgTypes)
    m_SpecAbList.SetItemText(listIdx, m_MsgCol, msgs);
  if (!m_ReadOnly)
    m_SpecAbList.SetItemText(listIdx, m_ScriptCol, activ);
  
  //m_SpecAbList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_SpecAbList.SelectItem(listIdx, TRUE);
  return TRUE;
}

void CChooseSpeclAbDlg::UpdateSpecialAbilityList(int index, specialAbilitiesType sa)
{
  CString spec("");
  CString msgs("");
  CString activ("");

  spec = spellAbilitiesText[sa];

  int count=0;
  DWORD types = m_sa.GetMsgTypes(sa);
  for (int t=1;t<NUM_SPECAB_MSGS;t++)
  {
    DWORD flag = 1<<t;
    if (types & flag)
    {
      msgs += SpecAbMsgText[ConvertSpecAbFlagToIndex(flag)];
      msgs += ",";
      count++;
    }
  }
  if (count==0)
    msgs=SpecAbMsgText[0];

  activ = m_sa.GetSpecAbUserScript(sa); //GetSpecAbScript(sa);
  activ.Remove('\r');
  activ.Remove('\n');
  activ.Remove('\t');

  m_SpecAbList.SetItemText(index, 0, spec);
  if (!m_NoMsgTypes)
    m_SpecAbList.SetItemText(index, m_MsgCol, msgs);
  if (!m_ReadOnly)
    m_SpecAbList.SetItemText(index, m_ScriptCol, activ);
  //m_SpecAbList.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
  m_SpecAbList.SelectItem(index, TRUE);
}

void CChooseSpeclAbDlg::OnDeletespecab() 
{
  POSITION pos = m_SpecAbList.GetFirstSelectedItemPosition();
  
  if (pos != NULL)
  {
    while (pos != NULL)
    {
      int nItem = m_SpecAbList.GetNextSelectedItem(pos);
      DWORD flag = m_SpecAbList.GetItemData(nItem);
      m_sa.DisableSpecAb((specialAbilitiesType)flag);
    }
    
    m_SpecAbList.DeleteAllItems();
    int count=0;
    POSITION pos2 = m_sa.GetHeadPosition();
    while (pos2 != NULL)
    {
      AddSpecialAbilityToList(m_sa.GetNext(pos2));
      count++;
    }
    
    if (count > 0)
    {
      //m_SpecAbList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_SpecAbList.EnsureVisible(0, FALSE);
      m_SpecAbList.SelectItem(0, TRUE);
    }
  }
}

void CChooseSpeclAbDlg::OnEditspecab() 
{
  POSITION pos = m_SpecAbList.GetFirstSelectedItemPosition();
  
  if (pos != NULL)
  {
    while (pos != NULL)
    {
      int nItem = m_SpecAbList.GetNextSelectedItem(pos);
      DWORD flag = m_SpecAbList.GetItemData(nItem);
      specialAbilitiesType sa = (specialAbilitiesType)flag;

      CString script, script2;
      CString start = m_sa.GenerateSpecAbFuncStart(sa);
      CString end = m_sa.GenerateSpecAbFuncEnd(sa);
      
      ScriptList list;
      list.SetSize(2);
      list[0].name = ActivationScriptName;
      list[0].script = m_sa.GetSpecAbUserScript(sa);

      list[1].name = DeActivationScriptName;
      list[1].script = m_sa.GetSpecAbUserDAScript(sa);

      CEditText dlg(start, end, m_sa.GenerateSpecAbFuncName(sa), list);
      dlg.m_ScriptFilter = SPELL_USAGE;
      
      if (dlg.DoModal()==IDOK)
      {
        if (!dlg.m_list[0].script.IsEmpty())
        {
          script = start + dlg.m_list[0].script + end;

          if (!dlg.m_list[1].script.IsEmpty()) // if empty, generate default deactivate script
            script2=start + dlg.m_list[1].script + end;
          
          m_sa.EnableSpecAb(sa, script, script2);
        }
        else
          m_sa.DisableSpecAb(sa);

        UpdateSpecialAbilityList(nItem, sa);
      }
    }
  }
}

void CChooseSpeclAbDlg::OnDblclkSpecablist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEditspecab();	
	*pResult = 0;
}

void CChooseSpeclAbDlg::OnEditmsgtypes() 
{
  POSITION pos = m_SpecAbList.GetFirstSelectedItemPosition();
  
  if (pos != NULL)
  {
    int nItem = m_SpecAbList.GetNextSelectedItem(pos);
    DWORD flag = m_SpecAbList.GetItemData(nItem);
    specialAbilitiesType sa = (specialAbilitiesType)flag;
  	CSpecAbMsgs dlg(m_sa, sa);
    if (dlg.DoModal())
    {
      dlg.GetData(m_sa);
      UpdateSpecialAbilityList(nItem, sa);
    }
  }	
}

#else

/////////////////////////////////////////////////////////////////////////////
// CChooseSpeclAbDlg dialog

BEGIN_MESSAGE_MAP(CChooseSpeclAbDlg, CDialog)
	//{{AFX_MSG_MAP(CChooseSpeclAbDlg)
	ON_CBN_SELCHANGE(IDC_SPECABCOMBO, OnSelchangeAbility)
	ON_BN_CLICKED(IDC_ADDSPECAB, OnAddspecab)
  ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETELine, OnDeleteLine)
  ON_BN_CLICKED(IDC_DeleteSpecAbility, OnDeleteSpecAbility)
  ON_BN_CLICKED(IDC_DuplicateSpecAbility, OnDuplicateSpecAbility)
  ON_BN_CLICKED(IDC_RenameSpecAbility, OnRenameSpecAbility)
	ON_BN_CLICKED(IDC_EDITSPECAB, OnEditspecab)
	ON_NOTIFY(NM_DBLCLK, IDC_SPECABLIST, OnDblclkSpecablist)
  ON_NOTIFY(NM_CLICK, IDC_SPECABLIST, OnClkSpecablist)
	//ON_BN_CLICKED(IDC_EDITMSGTYPES, OnEditmsgtypes)
  ON_BN_CLICKED(IDC_SCRIPT,OnClickScript)
  ON_BN_CLICKED(IDC_Constant,OnClickConstant)
  ON_BN_CLICKED(IDC_Variable,OnClickVariable)
  ON_BN_CLICKED(IDC_Include,OnClickInclude)
  ON_BN_CLICKED(IDC_IntegerTable,OnClickIntegerTable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




CChooseSpeclAbDlg::CChooseSpeclAbDlg(const A_SPECABILITY_DEF_L &saList, const CString& title, CWnd* pParent /*=NULL*/)
	: CDialog(CChooseSpeclAbDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseSpeclAbDlg)
	//}}AFX_DATA_INIT
  m_saList.Copy(saList);
  m_title = title;
}


BOOL CChooseSpeclAbDlg::AddSpecialAbilityToList(const ASLENTRY *pEntry)
{
  // no duplicates
  char text[200];
  CString typeText;
  for (int i=0;i<m_SpecAbList.GetItemCount();i++)
  {
    m_SpecAbList.GetItemText(i, 0, text, 199);
    if (pEntry->Key() == text)
      return FALSE;
  }

  CString spec("");
  //CString msgs("");
  //CString activ("");
  LVITEM item;
  spec = pEntry->Key();

  //int count=0;
  //msgs += "Not Implemented in CChooseSpeclAbDlg::AddSpecialAbilityToList";
  //msgs += '.';
  //count++;
  //if (count==0)
  //  msgs=SpecAbMsgText[0];
  
  //activ = m_saList.GetSpecAbUserScript(sa); //GetSpecAbScript(sa);
  //activ.Remove('\r');
  //activ.Remove('\n');
  //activ.Remove('\t');

  int listIdx = m_SpecAbList.GetItemCount();

	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = spec.GetBuffer(spec.GetLength()+1);
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = 0; 
  item.iIndent = 0;

  m_SpecAbList.InsertItem(&item);  

  switch(pEntry->Flags())
  {
  case SPECAB_SCRIPT:       typeText = "Script"; break;
  case SPECAB_CONSTANT:     typeText = "Constant"; break;
  case SPECAB_VARIABLE:     typeText = "Variable"; break;
  case SPECAB_INCLUDE:      typeText = "Include"; break;
  case SPECAB_INTEGERTABLE: typeText = "Integer Table"; break;
  default:                  typeText = "Unknown"; break;
  };

  m_SpecAbList.SetItemText(listIdx, typeCol, typeText);//msgs);
  m_SpecAbList.SetItemText(listIdx, valueCol, pEntry->Value());//activ);
  
  //m_SpecAbList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_SpecAbList.SelectItem(listIdx, TRUE);
  return TRUE;
}

void CChooseSpeclAbDlg::FillListBox(void)
{
  POSITION pos;
  int indx, saveSelection;
  CString name;
  saveSelection = m_listSelection;
  indx = m_SpecAbCombo.GetCurSel();
  m_SpecAbList.DeleteAllItems();

  if (indx == CB_ERR) return;

  m_SpecAbCombo.GetLBText(indx, name);
  m_newAbilityName = name+"_new";
  
  const SPECABILITY_DEF *pSpecAb;
  pSpecAb = m_saList.FindAbility(name);
  pos = pSpecAb->GetStartPosition();
  while (pos != NULL)
  {
    const ASLENTRY *pEntry;
    pSpecAb->GetNextEntry(pos, &pEntry);
    AddSpecialAbilityToList(pEntry);
  };
  m_listSelection = saveSelection;
  if (m_SpecAbList.GetItemCount() > 0)
  {
    m_SpecAbList.SetItemState(m_listSelection, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
    m_SpecAbList.EnsureVisible(m_listSelection, FALSE);
    m_SpecAbList.Sort();
    //m_SpecAbList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_SpecAbList.EnsureVisible(0, FALSE);
    ProcessStringSelection();
  }
  //m_SpecAbList.SetItemState(indx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
  //m_SpecAbList.EnsureVisible(indx, FALSE);
}


void CChooseSpeclAbDlg::RebuildDialogBox(void)
{
  // only items can change/use msg types
  m_SpecAbCombo.ResetContent();
  {
    POSITION pos;
    const SPECABILITY_DEF* pSaData;
    pos = m_saList.GetStartPosition();
    while (pos != NULL)
    {
      int index;
      pSaData = m_saList.GetNext(pos);
      index = m_SpecAbCombo.AddString(pSaData->Name());
    };
  };

  m_SpecAbCombo.SetCurSel(0);
  FillListBox();
  UpdateData(false);
}

BOOL CChooseSpeclAbDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();



	// insert columns
	LV_COLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

  int col=0;

	lvc.iSubItem = col;
	lvc.pszText = "Name";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_LEFT;
	m_SpecAbList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

  col++;

//  if (!m_NoMsgTypes)
//  {
	  lvc.iSubItem = col;
	  lvc.pszText = "Type";
	  lvc.cx = 60;
	  lvc.fmt = LVCFMT_LEFT;
    m_SpecAbList.InsertColumn(col,&lvc, CListSort::dtSTRING, true);
//    m_MsgCol = col;
    col++;
//  }

//  if (!m_ReadOnly)
//  {
	  lvc.iSubItem = col;
	  lvc.pszText = "Value";
	  lvc.cx = 700;
	  lvc.fmt = LVCFMT_LEFT;
    m_SpecAbList.InsertColumn(col,&lvc, CListSort::dtSTRING, true);
//    m_ScriptCol=col;
    col++;
//  }

	m_SpecAbList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  
  SetWindowText(m_title);

  RebuildDialogBox();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CChooseSpeclAbDlg::SetStringType(SPECAB_STRING_TYPE type)
{
  m_script.SetCheck(0);
  m_constant.SetCheck(0);
  m_variable.SetCheck(0);
  m_include.SetCheck(0);
  m_integerTable.SetCheck(0);
  switch(type)
  {
  case SPECAB_SCRIPT:       m_script.SetCheck(1);       break;
  case SPECAB_CONSTANT:     m_constant.SetCheck(1);     break;
  case SPECAB_VARIABLE:     m_variable.SetCheck(1);     break;
  case SPECAB_INCLUDE:      m_include.SetCheck(1);      break;
  case SPECAB_INTEGERTABLE: m_integerTable.SetCheck(1); break;
  };
}

SPECABILITY_DEF *CChooseSpeclAbDlg::FindCurrentlySelectedAbility(void)
{
  int abIndx;
  CString abName;
  abIndx = m_SpecAbCombo.GetCurSel();
  if (abIndx == CB_ERR) return NULL;
  m_SpecAbCombo.GetLBText(abIndx, abName);
  return m_saList.FindAbility(abName);
}

ASLENTRY *CChooseSpeclAbDlg::FindCurrentlySelectedString(void)
{
  POSITION pos;
  char text[200];
  CString abName;
  ASLENTRY *pEntry;
  const SPECABILITY_DEF *pSpecAb;
  pos = m_SpecAbList.GetFirstSelectedItemPosition();
  if (pos == NULL) return NULL;
  m_listSelection = m_SpecAbList.GetNextSelectedItem(pos);
  m_SpecAbList.GetItemText(m_listSelection, 0, text, 199);
  pSpecAb = FindCurrentlySelectedAbility();
  if (pSpecAb == NULL) return NULL;    
  pEntry = pSpecAb->Find(text);
  return pEntry;
}



void CChooseSpeclAbDlg::ProcessStringSelection(void)
{
  POSITION pos;
  CString abName;
  const ASLENTRY *pEntry;
  pos = m_SpecAbList.GetFirstSelectedItemPosition();
  if (pos == NULL)
  {
    //die("CChooseSpecAbDlg::ProcessStringSelection POS==NULL"); //Not Implemented(0x5fa21b, FALSE);
  }
  else
  {
    pEntry = FindCurrentlySelectedString();
    if (pEntry == NULL) return;
    SetStringType((SPECAB_STRING_TYPE)pEntry->Flags());
  };
}


void CChooseSpeclAbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseSpeclAbDlg)
	DDX_Control(pDX, IDC_SPECABLIST, m_SpecAbList);
	DDX_Control(pDX, IDC_SPECABCOMBO, m_SpecAbCombo);
  DDX_Control(pDX, IDC_SCRIPT, m_script);
  DDX_Control(pDX, IDC_Constant, m_constant);
  DDX_Control(pDX, IDC_Variable, m_variable);
  DDX_Control(pDX, IDC_Include, m_include);
  DDX_Control(pDX, IDC_IntegerTable, m_integerTable);
  DDX_Text   (pDX, IDC_NewAbilityName, m_newAbilityName);
	//}}AFX_DATA_MAP
}




void CChooseSpeclAbDlg::OnOK() 
{
	UpdateData(TRUE);

	CDialog::OnOK();
}


afx_msg void CChooseSpeclAbDlg::OnAdd()
{
  SPECABILITY_DEF *pSpecAb;
  pSpecAb = FindCurrentlySelectedAbility();
  if (pSpecAb == NULL) return;
  CAttibuteDataEdit dlg("","");
  dlg.DoModal();
  if (dlg.m_Name.IsEmpty()) return;
  pSpecAb->Insert(dlg.m_Name, dlg.m_Value, 0);
  FillListBox();
  UpdateData(false);
}

afx_msg void CChooseSpeclAbDlg::OnAddspecab()
{
  int indx;
  UpdateData(true);
  if (m_newAbilityName.IsEmpty()) 
  {
    MsgBoxInfo("You must provide a new ability name");
    return;
  };
  m_saList.InsertAbility(m_newAbilityName);
  indx = m_SpecAbCombo.AddString(m_newAbilityName);
  m_SpecAbCombo.SetCurSel(indx);
  FillListBox();
  UpdateData(false);
};

afx_msg void CChooseSpeclAbDlg::OnDeleteSpecAbility()
{
  CString name;
  UpdateData(true);
  name = FindCurrentlySelectedAbility()->Name();
  m_saList.Delete(name);
  RebuildDialogBox();
}

afx_msg void CChooseSpeclAbDlg::OnDuplicateSpecAbility()
{
  CString name;
  SPECABILITY_DEF *pOldSpecAb, *pNewSpecAb;
  SPECABILITY_DEF specAb;
  UpdateData(true);
  if (m_newAbilityName.IsEmpty())
  {
    MsgBoxInfo("You must provide a new ability name");
    return;
  };
  name = FindCurrentlySelectedAbility()->Name();
  pOldSpecAb = m_saList.FindAbility(name);
  specAb.Copy(*pOldSpecAb, m_newAbilityName);
  pNewSpecAb = m_saList.InsertAbility(m_newAbilityName);
  pNewSpecAb->Copy(specAb);
  RebuildDialogBox();
}

afx_msg void CChooseSpeclAbDlg::OnRenameSpecAbility()
{
  CString name;
  SPECABILITY_DEF *pOldSpecAb, *pNewSpecAb;
  SPECABILITY_DEF specAb;
  UpdateData(true);
  if (m_newAbilityName.IsEmpty())
  {
    MsgBoxInfo("You must provide a new ability name");
    return;
  };
  name = FindCurrentlySelectedAbility()->Name();
  pOldSpecAb = m_saList.FindAbility(name);
  specAb.Copy(*pOldSpecAb, m_newAbilityName);
  pNewSpecAb = m_saList.InsertAbility(m_newAbilityName);
  pNewSpecAb->Copy(specAb);
  m_saList.Delete(name);
  RebuildDialogBox();
}



afx_msg void CChooseSpeclAbDlg::OnDeleteLine()
{
  ASLENTRY *pEntry;
  pEntry = FindCurrentlySelectedString();
  if (pEntry == NULL) return;
  m_saList.DeleteString(FindCurrentlySelectedAbility()->Name(), pEntry->Key());
  FindCurrentlySelectedString();
  if (m_listSelection <0) return;
  m_SpecAbList.DeleteItem(m_listSelection);
};


#define CTX_CHARACTER 0x00000001
#define CTX_MySelf    0x00000002


struct SUPPORTED_CONTEXT
{
  char    hookName[30];
  DWORD   supportedContexts;
};

SUPPORTED_CONTEXT supportedContexts[] =
{
  {"AdjustInitiative"                 ,CTX_Combatant                                     },
  {"AutoAction"                       ,CTX_Combatant                                     },
  {"CanReady"                         ,CTX_Character|CTX_Item                            },
  {"CanReady"                         ,CTX_Item     |CTX_Character                       },
  {"CanCastSpells"                    ,CTX_Character|CTX_Combatant|CTX_Class             },
  {"CanMemorizeSpells"                ,CTX_Character|CTX_Class                           },
  {"CharDisplayStatus"                ,CTX_Character|CTX_Combatant|CTX_Spell             },
  {"CanSpellDetectMagic"              ,CTX_Character|CTX_Spell                           },
  {"ComputeDamage"                    ,CTX_Attacker |CTX_Target   |CTX_Item              },
  {"CanScribeOrWhatever"              ,CTX_Character|CTX_Class                           },
  {"ComputeSpellDamage"               ,CTX_Attacker |CTX_Target   |CTX_Spell             },
  {"CanAim"                           ,CTX_Character|CTX_Combatant|CTX_Item              },
  {"CanCureOrWhatever"                ,CTX_Character|CTX_Class                           },
  {"CureOrWhatever"                   ,CTX_Character|CTX_Class                           },
  {"CanIdentify"                      ,CTX_Character|CTX_Item                            },
  {"CanLayOrWhatever"                 ,CTX_Character|CTX_Class                           },
  {"CanKnowSpell"                     ,CTX_Character|CTX_Spell                           },
  {"CombatMainMenu"                   ,CTX_Character|CTX_Combatant|CTX_Class             },
  {"CombatSpecialAction"              ,CTX_Attacker |CTX_Target                          },
  {"CreateMonsterCombatant"           ,CTX_Character|CTX_MonsterType                     },
  {"CreateCharacterSpells"            ,CTX_Character|CTX_Class    |CTX_Spell             },
  {"CreateCharacterSpellsMinMax"      ,CTX_Character|CTX_Class                           },
  {"DisplayCharacterClass"            ,CTX_Character                                     },
  {"DisplayCharacterRace"             ,CTX_Character                                     },
  {"DrawMoveMenu"                     ,CTX_Character|CTX_Combatant                       },
  {"DoesAttackSucceed"                ,CTX_Attacker |CTX_Target   |CTX_Item              },
  {"DoesSpellAttackSucceed"           ,CTX_Attacker |CTX_Target   |CTX_Class | CTX_Spell },
  {"FreeAttack-CanFreeAttack"         ,CTX_Target   |CTX_Attacker                        },
  {"FixCharacter"                     ,CTX_Character|CTX_Spell                           },
  {"FreeAttack-AttackOver"            ,CTX_Combatant                                     },
  {"Guarding-CanGuard"                ,CTX_Combatant                                     },
  {"Guarding-StartOfRound"            ,CTX_Character|CTX_Combatant                       },
  {"Guarding-CanGuardAttack"          ,CTX_Attacker |CTX_Target                          },
  {"Guarding-AttackOver"              ,CTX_Combatant                                     },
  {"Guarding-BeginCombat"             ,CTX_Character|CTX_Combatant                       },
  {"GetAdjMaxMovement"                ,CTX_Character                                     },
  {"GetNumberOfAttacks"               ,CTX_Character|CTX_Combatant|CTX_Item              },
  {"GetItemTargetHitBonus"            ,CTX_Attacker |CTX_Target   |CTX_Item              },
  {"Identify"                         ,CTX_Character|CTX_Item                            },
  {"InvokeSpellOnTarget"              ,CTX_Attacker |CTX_Target   |CTX_Spell             },
  {"IsCombatReady"                    ,CTX_Character|CTX_Combatant                       },
  {"IsValidTarget"                    ,CTX_Attacker |CTX_Target   |CTX_Spell             },
  {"LayOrWhatever"                    ,CTX_Character|CTX_Class                           },
  {"OnReady"                          ,CTX_Character|CTX_Item                            },
  {"OnStartCombatAction"              ,CTX_Character|CTX_Combatant                       },
  {"OnStartCombat"                    ,CTX_Character                                     },
  {"OnStep"                           ,CTX_Combatant                                     },
  {"OnEndCombat"                      ,CTX_Character                                     },
  {"OnUseLastWeapon"                  ,CTX_Character                                     },
  {"OnReadyCombatEquipment"           ,CTX_Character                                     },
  {"PostClass"                        ,CTX_Character                                     },
  {"PostCreateCharacter"              ,CTX_Character|CTX_Class    |CTX_Race|CTX_BaseClass},
  {"PartyArrangement"                 ,0                                                 },
  {"PartyOriginNorth"                 ,0                                                 },
  {"PartyOriginEast"                  ,0                                                 },
  {"PartyOriginSouth"                 ,0                                                 },
  {"PartyOriginWest"                  ,0                                                 },
  {"PlaceMonsterClose"                ,0                                                 },
  {"PlaceMonsterNear"                 ,0                                                 },
  {"PlaceMonsterFar"                  ,0                                                 },
  {"RedirectSpell"                    ,CTX_Attacker |CTX_Target   |CTX_Spell             },
  {"ScribeOrWhatever"                 ,CTX_Character|CTX_Class                           },
  {"SpellLingerBlockage"              ,CTX_Combatant|CTX_Spell                           },
  {"StartCombatRound"                 ,CTX_Character|CTX_Combatant                       },
  {"SpellDuration"                    ,CTX_Attacker |CTX_Spell                           },
  {"SpellRange"                       ,CTX_Attacker |CTX_Spell                           },
  {"SpellNumTargets"                  ,CTX_Attacker |CTX_Spell                           },
  {"SpellCasterLevel"                 ,CTX_Attacker |CTX_Spell                           },
  {"SpellLingeringScripts"            ,CTX_Spell                                         },
  {"TrainingCharacter"                ,CTX_Character                                     },
  {"TransferModule"                   ,0                                                 },
  {"CharacterViewScreen"              ,CTX_Character                                     },
};

const unsigned int numSupportedContexts = sizeof(supportedContexts) / sizeof(supportedContexts[0]);


afx_msg void CChooseSpeclAbDlg::OnEditspecab()
{
  CString start("$PUBLIC $FUNC SpecAbTest() { ");
  CString end(" \n} SpecAbTest;");
  CString func = "SpecAbTest";

  SScriptInfo scriptInfo;
  ScriptList scriptList;
  ASLENTRY *pEntry;
  //DWORD context;
  pEntry = FindCurrentlySelectedString();
  if (pEntry == NULL) return;
  scriptInfo.name = pEntry->Key();
  scriptInfo.script = pEntry->Value();
  scriptList.SetAtGrow(0,scriptInfo);
  //SetSpecialAbilityContext(NULL, pEntry);
  //context = CTX_CHARACTER;
  CEditText dlg(start,end,func, pEntry->Flags() == SPECAB_SCRIPT, scriptList, NULL);
  {
    int i;
    for (i=0; i<numSupportedContexts; i++)
    {
      if (scriptInfo.name == supportedContexts[i].hookName)
      {
        dlg.availableContexts = supportedContexts[i].supportedContexts;
        break;
      }
    };
  };
  dlg.m_ScriptFilter = SPECIALABILITIES_USAGE;
  if (dlg.DoModal() == IDOK)
  {
    pEntry->Value(dlg.m_Text);
  };
  //ClearSpecialAbilityContext();
  FillListBox();
  UpdateData(false);
}





afx_msg void CChooseSpeclAbDlg::OnDblclkSpecablist(NMHDR* pNMHDR, LRESULT* pResult)
{
  UpdateData(true);
  //m_SpecAbCombo.SetCurSel
  ProcessStringSelection();  // Update gui
  OnEditspecab();
}

afx_msg void CChooseSpeclAbDlg::OnClkSpecablist(NMHDR* pNMHDR, LRESULT* pResult)
{
  UpdateData(true);
  //m_SpecAbCombo.SetCurSel
  ProcessStringSelection();  // Update gui
}



//afx_msg void CChooseSpeclAbDlg::OnEditmsgtypes()
//{
//  Not Implemented(05,FALSE);
//};

afx_msg void CChooseSpeclAbDlg::OnSelchangeAbility()
{
  UpdateData(true);
  FillListBox();
  UpdateData(false);
};

void CChooseSpeclAbDlg::UpdateStringType(SPECAB_STRING_TYPE type)
{
  ASLENTRY *pEntry;
  pEntry = FindCurrentlySelectedString();
  if (pEntry == NULL) return;
  pEntry->Flags(type);
  FillListBox();
  UpdateData(false);
}


afx_msg void CChooseSpeclAbDlg::OnClickScript()
{
  UpdateStringType(SPECAB_SCRIPT);
};

afx_msg void CChooseSpeclAbDlg::OnClickConstant()
{
 UpdateStringType(SPECAB_CONSTANT);
};

afx_msg void CChooseSpeclAbDlg::OnClickVariable()
{
  UpdateStringType(SPECAB_VARIABLE);
};

afx_msg void CChooseSpeclAbDlg::OnClickInclude()
{
  UpdateStringType(SPECAB_INCLUDE);
};

afx_msg void CChooseSpeclAbDlg::OnClickIntegerTable()
{
  UpdateStringType(SPECAB_INTEGERTABLE);
};

bool CChooseSpeclAbDlg::GetData(A_SPECABILITY_DEF_L &saList)
{ 
  bool result;
  result = !(saList == m_saList);
  saList.Copy(m_saList);
  return result;
}

#endif