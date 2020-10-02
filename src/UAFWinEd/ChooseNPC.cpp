/******************************************************************************
* Filename: ChooseNPC.cpp
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
#include "ChooseNPC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseNPC dialog


//CChooseNPC::CChooseNPC(int CharNum, CWnd* pParent /*=NULL*/)
CChooseNPC::CChooseNPC(const CHARACTER_ID& characterID, CWnd* pParent /*=NULL*/)
	: CDialog(CChooseNPC::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseNPC)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  //m_CharNum = CharNum;
  m_characterID = characterID;
}


void CChooseNPC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseNPC)
	DDX_Control(pDX, IDC_CHOSENLIST, m_ChosenList);
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseNPC, CDialog)
	//{{AFX_MSG_MAP(CChooseNPC)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_NOTIFY(NM_DBLCLK, IDC_CHOSENLIST, OnDblclkChosenlist)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseNPC message handlers

void CChooseNPC::OnAdd() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    OnDelete();
    DWORD flag = m_AvailList.GetItemData(nItem);

 //   m_ChosenList.DeleteAllItems();
    AddToChosenList((int)flag);
  }	
}

void CChooseNPC::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnAdd();	
	*pResult = 0;
}

void CChooseNPC::OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnDelete();	
	*pResult = 0;
}

void CChooseNPC::OnDelete() 
{
	m_ChosenList.DeleteAllItems();	
}

BOOL CChooseNPC::OnInitDialog() 
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
	m_ChosenList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Race";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);
  m_ChosenList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 2;
	lvc.pszText = "Class";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(2,&lvc, CListSort::dtSTRING, true);
  m_ChosenList.InsertColumn(2,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 3;
	lvc.pszText = "Hitpoints";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(3,&lvc, CListSort::dtINT, true);
  m_ChosenList.InsertColumn(3,&lvc, CListSort::dtINT, true);

	m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
  m_ChosenList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

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
    //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_AvailList.EnsureVisible(0, FALSE);  
    m_AvailList.SelectItem(0, TRUE);
  }

  {
    int i;
    //if (globalData.charData.HaveKey(m_CharNum))
    i = globalData.charData.LocateCharacter(m_characterID);
      //AddToChosenList(m_CharNum);
    if (i >= 0)
      AddToChosenList(i);
  };
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//void CChooseNPC::AddToAvailList(CHARACTER &data)
void CChooseNPC::AddToAvailList(int i)
{
  CHARACTER *pCharacter;
  int listIdx = m_AvailList.GetItemCount();

  char name[MAX_ITEM_NAME+1];
  char race[MAX_ITEM_NAME+1];
  char charClass[50];
  char hp[50];

  pCharacter = globalData.charData.GetCharacter(i);

  //strcpy(name, data.GetName());
  strcpy(name, pCharacter->GetName());
  //strcpy(race, RaceText[data.race()]);
  strcpy(race, pCharacter->RaceID());

  //2Dec2009 PRS
  //strcpy(charClass, ClassText[data.GetClass()]);
  //strcpy(charClass, classData.ClassName(data.GetClass()));
  strcpy(charClass, pCharacter->GetClass());

  //sprintf(hp, "%i", data.GetMaxHitPoints());
  sprintf(hp, "%i", pCharacter->GetMaxHitPoints());

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
	item.lParam = i;

  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemText(listIdx, 1, race); 
  m_AvailList.SetItemText(listIdx, 2, charClass); 
  m_AvailList.SetItemText(listIdx, 3, hp);   
}

void CChooseNPC::AddToChosenList(int index)
{
  CHARACTER data;
  //if (!globalData.charData.GetCharacterData(index, data))
  if (!globalData.charData.FetchCharacter(index, &data))
  {
    WriteDebugString("Bogus item index in CChooseNPC::AddToChosenList\n");
    return;
  }

  int listIdx = m_ChosenList.GetItemCount();

  char name[MAX_ITEM_NAME+1];
  char race[MAX_ITEM_NAME+1];
  char charClass[50];
  char hp[50];

  strcpy(name, data.GetName());
  //strcpy(race, RaceText[data.race()]);
  strcpy(race, data.RaceID());

  //2Dec2009 PRS
  //strcpy(charClass, ClassText[data.GetClass()]);
  //strcpy(charClass, classData.ClassName(data.GetClass()));
  strcpy(charClass, data.GetClass());

  sprintf(hp, "%i", data.GetMaxHitPoints());

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
	item.lParam = index;

  m_ChosenList.InsertItem(&item);
  m_ChosenList.SetItemText(listIdx, 1, race); 
  m_ChosenList.SetItemText(listIdx, 2, charClass); 
  m_ChosenList.SetItemText(listIdx, 3, hp); 
  //m_ChosenList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_ChosenList.SelectItem(listIdx, TRUE);
}

void CChooseNPC::OnOK() 
{
  if (m_ChosenList.GetItemCount() > 0)
    //m_CharNum = (int)m_ChosenList.GetItemData(0);
    m_characterID = globalData.charData.PeekCharacter(m_ChosenList.GetItemData(0))->CharacterID();
	
	CDialog::OnOK();
}
