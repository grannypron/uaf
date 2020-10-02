/******************************************************************************
* Filename: Spelldlg.cpp
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
#include "SpellDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpell dialog


CSpell::CSpell(spellBookType &books, DWORD MaxSelectable, CWnd* pParent /*=NULL*/)
	: CDialog(CSpell::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpell)
	m_CharClassText = _T("");
	m_MaxSpells = _T("");
	//}}AFX_DATA_INIT

  m_NewTitle="";
	m_books = books;
  m_max = MaxSelectable; // 0 = no limit
  if (m_max != 0)
    m_MaxSpells.Format("(Max Spells = %i)", m_max);
  else
    m_MaxSpells="";
}


void CSpell::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpell)
	DDX_Control(pDX, IDC_CHOSENLIST, m_ChosenList);
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	DDX_Text(pDX, IDC_CHARCLASSTEXT, m_CharClassText);
	DDX_Text(pDX, IDC_MAXSPELLS, m_MaxSpells);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpell, CDialog)
	//{{AFX_MSG_MAP(CSpell)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_NOTIFY(NM_DBLCLK, IDC_CHOSENLIST, OnDblclkChosenlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpell message handlers

BOOL CSpell::OnInitDialog() 
{
	CDialog::OnInitDialog();

  //m_CharClassText = ClassFlagToText(m_books.spellLimits.m_spellClasses);
  {
    CString allowedSchools;
    //n = m_books.spellLimits.GetSchoolCount();
    //NotImplemented(0xacbd,false); n = 0;
    /*
    for (i=0; i<n; i++)
    {
      SCHOOL_ID schoolID;
      CString schoolName;
      schoolID = m_books.spellLimits.PeekSpellLimits(i)->schoolID;
      schoolName = schoolID.UniqueName();
      if (!allowedSchools.IsEmpty())
      {
        allowedSchools += " / ";
      };
      allowedSchools += schoolName;
    };
    */
    m_CharClassText = allowedSchools;
  };


	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Level";
	lvc.cx = 20;
	lvc.fmt = LVCFMT_LEFT;
	m_AvailList.InsertColumn(0,&lvc, CListSort::dtINT, true);
	m_ChosenList.InsertColumn(0,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Type";
	lvc.cx = 75;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);
  m_ChosenList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 2;
	lvc.pszText = "Name";
	lvc.cx = 200;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(2,&lvc, CListSort::dtSTRING, true);
  m_ChosenList.InsertColumn(2,&lvc, CListSort::dtSTRING, true);

	m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
  m_ChosenList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  int a_count = 0;
  {
    int i, n;
    //POSITION pos = spellData.GetHeadPosition();
    //while (pos != NULL)
    n = spellData.GetCount();
    for (i=0; i<n; i++)
    {
      //if (m_books.spellLimits.m_spellClasses & spellData.PeekAtPos(pos).schoolMask)
      //if (m_books.spellLimits.m_spellClasses & spellData.PeekSpellData(pos)->schoolMask)
      //if (m_books.spellLimits.PeekSpellLimits(spellData.PeekSpell(i)->schoolID ) >= 0)
        //AddToAvailList(spellData.PeekAtPos(pos));
        //AddToAvailList(*spellData.PeekSpellData(pos));
      const SPELL_DATA *pSpell;
      pSpell = spellData.PeekSpell(i);
      if (m_books.LocateCharacterSpell(pSpell->SpellID()) >= 0) continue;
      AddToAvailList(*pSpell);
      //spellData.GetNext(pos);
      a_count++;
    }
  };

  if (a_count > 0)
  {
    m_AvailList.Sort();
    //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_AvailList.EnsureVisible(0, FALSE);
    m_AvailList.SelectItem(0, TRUE);
  }

  int c_count = 0;
  //m_books.SortSpells();
/*
  pos = m_books.GetHeadPosition();
  while (pos != NULL)
  {
    AddToChosenList(m_books.GetNext(pos).spell);
    c_count++;
  }
*/
  {
    int i,n;
    n = m_books.GetCount();
    for (i=n-1; i>=0; i--)
    {
      SPELL_ID spellID;
      spellID = m_books.PeekCharacterSpell(i)->spellID;
      if (spellID.IsNoSpell())
      {
        m_books.RemoveSpell(i);
      }
      else
      {
        AddToChosenList(spellID);
        c_count++;
      };
    };
  };

  if (c_count > 0)
  {
    m_ChosenList.Sort();
    //m_ChosenList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_ChosenList.EnsureVisible(0, FALSE);
    m_ChosenList.SelectItem(0, TRUE);
  }

  if (!m_NewTitle.IsEmpty())
    SetWindowText(m_NewTitle);
  
  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpell::AddToAvailList(const SPELL_DATA &data)
{
  int listIdx = m_AvailList.GetItemCount();

  char level[3];
  char type[200];

  LVITEM item;
  if ((data.Level < 1) || (data.Level > MAX_SPELL_LEVEL) ) return;
  sprintf(level, "%u", data.Level);
  //strcpy(type, ClassFlagToText(data.schoolMask));
  strcpy(type, data.schoolID);


	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = level;
	item.cchTextMax = 0;
	item.iImage = 0;
	//item.lParam = data.m_gsID.m_key;
	//item.lParam = data.m_gsID.UniqueIdentifier();
	item.lParam = 0;

  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemText(listIdx, 1, type); 
  m_AvailList.SetItemText(listIdx, 2, data.Name); 
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}

void CSpell::DeleteFromAvailList(int nItem)
{
  m_AvailList.DeleteItem(nItem);
}

//void CSpell::AddToChosenList(GLOBAL_SPELL_ID spell)
void CSpell::AddToChosenList(SPELL_ID spellID)
{
  //SPELL_DATA *data;
  const SPELL_DATA *pSpell;
  //if ((data = spellData.GetSpellData(spell)) == NULL)
  if ((pSpell = spellData.PeekSpell(spellID)) == NULL)
  {
    WriteDebugString("Bogus spell index in spell book in CSpell::AddToChosenList\n");
    return;
  }

  int listIdx = m_ChosenList.GetItemCount();
  
  char level[3];
  char type[200];

  LVITEM item;
  //sprintf(level, "%u", data->Level);
  sprintf(level, "%u", pSpell->Level);
  //strcpy(type, ClassFlagToText(data->schoolMask));
  strcpy(type, pSpell->schoolID);

	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = level;
	item.cchTextMax = 0;
	item.iImage = 0;
	//item.lParam = data->m_gsID.m_key;
	item.lParam = 0;

  m_ChosenList.InsertItem(&item);
  m_ChosenList.SetItemText(listIdx, 1, type); 
  //ChosenList.SetItemText(listIdx, 2, data->Name); 
  m_ChosenList.SetItemText(listIdx, 2, pSpell->Name);
  //m_ChosenList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_ChosenList.SelectItem(listIdx, TRUE);
}

void CSpell::GetSpellList(spellBookType &books)
{
	books = m_books;
}

void CSpell::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
  if ((m_max > 0) && ((DWORD)m_books.SpellsKnown() >= m_max))
  {
    //if (m_max > 1)
    {
      MsgBoxInfo("Delete a selected spell to make room for new spell first");
      return;
    }
    //// just remove single spell (only one allowed)
    ////GLOBAL_SPELL_ID flag;
    //SPELL_ID spellID;
    ////flag.UniqueIdentifier(m_ChosenList.GetItemData(0));
    //spellID = m_ChosenList.GetItemText(0,2);
    //const SPELL_DATA *pSpell;
    //pSpell = spellData.PeekSpell(spellID);
    ////m_books.KnowSpell(spellData.GetSpellSchool(flag),
    ////                  spellData.GetSpellSchool(flag),
    ////                  flag,
    ////                  spellData.GetSpellLevel(flag),
    ////                  FALSE);
    ////m_books.KnowSpell(pSpell->schoolMask,
    ////                  pSpell->schoolMask,
    ////                  spellID,
    ////                  pSpell->Level,
    ////                  FALSE);
    //BASECLASS_ID baseclassID;
    //m_books.KnowSpell(baseclassID,
    //                  pSpell->schoolID,
    //                  spellID,
    //                  pSpell->Level,
    //                  FALSE);
    //m_ChosenList.DeleteAllItems();
  }

  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
     while (pos != NULL)
     {
        int nItem = m_AvailList.GetNextSelectedItem(pos);
        //GLOBAL_SPELL_ID flag;
        SPELL_ID spellID;
        const SPELL_DATA *pSpell;
        //flag.UniqueIdentifier(m_AvailList.GetItemData(nItem));
        spellID = m_AvailList.GetItemText(nItem,2);
        pSpell = spellData.PeekSpell(spellID);
        //if (!m_books.IsKnown(flag))
        if (!m_books.IsKnown(spellID))
        {
          //if (m_books.KnowSpell(spellData.GetSpellSchool(flag),
          //                      spellData.GetSpellSchool(flag),
          //                      flag,
          //                      spellData.GetSpellLevel(flag),
          //                      TRUE))
          //if (m_books.KnowSpell(pSpell->schoolMask,
          //                      pSpell->schoolMask,
          //                      spellID,
          //                      pSpell->Level,
          //                      TRUE))
          BASECLASS_ID baseclassID;
          if (m_books.KnowSpell(baseclassID,
                                pSpell->schoolID,
                                spellID,
                                pSpell->Level,
                                TRUE))
          {
            //AddToChosenList(flag);
            AddToChosenList(spellID);
            DeleteFromAvailList(nItem);
          }
          else
          {
            MsgBoxInfo("This spell cannot be added due to spell limitations");
            //WriteDebugString("Unable to know spell %u\n", flag);
            WriteDebugString("Unable to know spell %s\n", spellID.UniqueName());
          }
        }
     }
  }		
	
	*pResult = 0;
}

void CSpell::OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
  POSITION pos = m_ChosenList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
     while (pos != NULL)
     {
        BASECLASS_ID baseclassID;
        int nItem = m_ChosenList.GetNextSelectedItem(pos);
        //GLOBAL_SPELL_ID flag;
        SPELL_ID spellID;
        const SPELL_DATA *pSpell;
        //flag.UniqueIdentifier(m_ChosenList.GetItemData(nItem));
        spellID = m_ChosenList.GetItemText(nItem,2);
        pSpell = spellData.PeekSpell(spellID);
        //m_books.KnowSpell(pSpell->schoolMask,
        //                  pSpell->schoolMask,
        //                  spellID,
        //                  pSpell->Level,
        //                  FALSE);
        if (m_books.IsKnown(spellID))
        {
          if (m_books.KnowSpell(baseclassID,
                                pSpell->schoolID,
                                spellID,
                                pSpell->Level,
                                FALSE))
          {
            int n;
            int k;
            AddToAvailList(*pSpell);
            m_AvailList.Sort();
            //m_AvailList.SelectItem(nItem, TRUE);
            n = m_AvailList.GetItemCount();
            for (k=0; k<n; k++)
            {
              SPELL_ID itemSpellID;
              itemSpellID = m_AvailList.GetItemText(k,2);
              if (spellID == itemSpellID) break;
            };
            m_AvailList.SelectItem(k, TRUE);
          };
        };
     }


    m_ChosenList.DeleteAllItems();
    int c_count = 0;
    //pos = m_books.GetHeadPosition();
    //while (pos != NULL)
    //{
    //  AddToChosenList(m_books.GetNext(pos).spell);
    //  c_count++;
    //}
    {
      int i,n;
      n = m_books.GetCount();
      for (i=0; i<n; i++)
      {
        AddToChosenList(m_books.PeekCharacterSpell(i)->spellID);
        c_count++;
      };
    };

    if (c_count > 0)
    {
      //m_ChosenList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_ChosenList.EnsureVisible(0, FALSE);
      m_ChosenList.SelectItem(0, TRUE);
    }
  }		
	
	*pResult = 0;
}
