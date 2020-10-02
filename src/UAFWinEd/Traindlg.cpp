/******************************************************************************
* Filename: Traindlg.cpp
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

//#include "UAFWinEd.h"
#include "GlobalData.h"
#include "Char.h"
#include "GameEvent.h"

#include "SelectPicDlg.h"
#include "TrainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrainingHall dialog


CTrainingHall::CTrainingHall(TRAININGHALL& halls, CWnd* pParent /*=NULL*/):
	CDialog(CTrainingHall::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrainingHall)
	m_ForceExit = FALSE;
//	m_Clerics = FALSE;
//	m_Druids = FALSE;
//	m_Fighters = FALSE;
//	m_MagicUsers = FALSE;
//	m_Paladins = FALSE;
//	m_Rangers = FALSE;
	m_Text = _T("");
//	m_Thiefs = FALSE;
	m_Cost = 0;
	m_CoinName = _T("");
	//}}AFX_DATA_INIT

	m_halls = halls;
}


void CTrainingHall::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrainingHall)
	DDX_Control(pDX, IDC_TRAINER, m_ChooseTrainer);
  DDX_Control(pDX, IDC_CLASSLIST, m_BaseclassList);
	DDX_Check(pDX, IDC_FORCEEXIT, m_ForceExit);
	//DDX_Check(pDX, IDC_CLERICS, m_Clerics);
	//DDX_Check(pDX, IDC_DRUIDS, m_Druids);
	//DDX_Check(pDX, IDC_FIGHTERS, m_Fighters);
	//DDX_Check(pDX, IDC_MAGICUSERS, m_MagicUsers);
	//DDX_Check(pDX, IDC_PALADINS, m_Paladins);
	//DDX_Check(pDX, IDC_RANGERS, m_Rangers);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	//DDX_Check(pDX, IDC_THIEFS, m_Thiefs);
	DDX_Text(pDX, IDC_TRAINCOST, m_Cost);
	DDX_Text(pDX, IDC_COINNAME, m_CoinName);
	//}}AFX_DATA_MAP
  if (!pDX->m_bSaveAndValidate)
  {
    int i, n;
    m_BaseclassList.DeleteAllItems();
    n = m_availBaseclasses.GetSize();
    for (i=0; i<n; i++)
    {
      if (m_availBaseclasses[i].present)
      {
        int j;
        char number[30];
        j = m_BaseclassList.InsertItem(m_BaseclassList.GetItemCount(), m_availBaseclasses[i].baseclassID);
        sprintf(number, "%d", m_availBaseclasses[i].minLevel);
        m_BaseclassList.SetItemText(j, 1, number);
        sprintf(number, "%d", m_availBaseclasses[i].maxLevel);
        m_BaseclassList.SetItemText(j, 2, number);
        m_BaseclassList.SetItemData(j, i);
      };
    };
  };
}


BEGIN_MESSAGE_MAP(CTrainingHall, CDialog)
	//{{AFX_MSG_MAP(CTrainingHall)
	ON_BN_CLICKED(IDC_TRAINER, OnTrainer)
  ON_BN_CLICKED(IDC_EDIT,    OnEdit)
  ON_BN_CLICKED(IDC_ADD,     OnAdd)
  ON_BN_CLICKED(IDC_DELETE,  OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrainingHall message handlers

void CTrainingHall::OnEdit() 
{
  int nItem, index;
  POSITION pos;
  CTrainable dlg(&m_availBaseclasses);
  pos = m_BaseclassList.GetFirstSelectedItemPosition();
  if (pos == NULL) return;
  nItem = m_BaseclassList.GetNextSelectedItem(pos);
  index = m_BaseclassList.GetItemData(nItem);
  dlg.result.baseclassID = m_availBaseclasses[index].baseclassID;
  dlg.result.minLevel = m_availBaseclasses[index].minLevel;
  dlg.result.maxLevel = m_availBaseclasses[index].maxLevel;
  dlg.result.present = TRUE;
  if (   (dlg.DoModal() == IDOK) 
      && !dlg.result.baseclassID.IsEmpty()
      && dlg.result.present)
  {
    int i, n;
    n = m_availBaseclasses.GetSize();
    for (i=0; i<n; i++)
    {
      if (m_availBaseclasses[i].baseclassID == dlg.result.baseclassID) break;
    };
    if (i == n)
    {
      m_availBaseclasses.Add(dlg.result);
    };
    m_availBaseclasses[i].minLevel = dlg.result.minLevel;
    m_availBaseclasses[i].maxLevel = dlg.result.maxLevel;
    m_availBaseclasses[i].present = TRUE;
    UpdateData(FALSE);
  };
};

void CTrainingHall::OnAdd() 
{
  CTrainable dlg(&m_availBaseclasses);
  dlg.result.minLevel = 1;
  dlg.result.maxLevel = 40;
  dlg.result.baseclassID.Empty();
  dlg.result.present = FALSE;
  if (dlg.DoModal() == IDOK)
  {
    int i, n;
    if (!dlg.result.present) return;
    n = m_availBaseclasses.GetSize();
    for (i=0; i<n; i++)
    {
      if (dlg.result.baseclassID == m_availBaseclasses[i].baseclassID)
      {
        break;
      };
    };
    if (i == n)
    {
      i = m_availBaseclasses.Add(dlg.result);
    };
    m_availBaseclasses[i].present = TRUE;
    m_availBaseclasses[i].minLevel = dlg.result.minLevel;
    m_availBaseclasses[i].maxLevel = dlg.result.maxLevel;
    UpdateData(FALSE);
    return;
  };
}


void CTrainingHall::AddToBaseclassList(int availIndex)
{
  int listIdx;

  char name[999];
  char minText[10];
  char maxText[10];

  const BASECLASSDEF *pBaseclassDef;
  pBaseclassDef = &m_availBaseclasses[availIndex];

  listIdx = m_BaseclassList.GetItemCount();

  strcpy(name, pBaseclassDef->baseclassID);
  sprintf(minText, "%i", pBaseclassDef->minLevel);
  sprintf(maxText, "%i", pBaseclassDef->maxLevel);

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = name;
	item.cchTextMax = 0;
	item.iImage = 0;
  item.lParam = availIndex;

  listIdx = m_BaseclassList.InsertItem(&item);
  m_BaseclassList.SetItemText(listIdx, 1, minText); 
  m_BaseclassList.SetItemText(listIdx, 2, maxText); 
  //m_BaseclassList.SelectItem(listIdx, TRUE);
}



void CTrainingHall::OnDelete() 
{
  POSITION pos;
  int i, n, nItem;
  int data;
  pos = m_BaseclassList.GetFirstSelectedItemPosition();
  if (pos == NULL) return;
  nItem = m_BaseclassList.GetNextSelectedItem(pos);
  data = m_BaseclassList.GetItemData(nItem);
  m_availBaseclasses[data].present = false;

  m_BaseclassList.DeleteAllItems();
  n = m_availBaseclasses.GetSize();
  for (i=0; i<n; i++)
  {
    if (!m_availBaseclasses[i].present) continue;
    AddToBaseclassList(i);
  };
  m_BaseclassList.SetItemState(nItem, LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
}



void CTrainingHall::OnTrainer() 
{
  CSelectPicDlg dlg(m_halls.pic, SmallPicDib);

  if (dlg.DoModal() == IDOK)
  {
    dlg.GetData(m_halls.pic);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_halls.pic.filename, name, MAX_MEDITBUTTON_TEXT);
    m_ChooseTrainer.SetWindowText(name);
  }
}


BOOL CTrainingHall::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ForceExit = m_halls.forceExit;
//  m_Clerics = m_halls.TrainCleric;
//  m_Druids = m_halls.TrainDruid;
//  m_Fighters = m_halls.TrainFighter;
//  m_MagicUsers = m_halls.TrainMagicUser;
//  m_Paladins = m_halls.TrainPaladin;
//  m_Rangers = m_halls.TrainRanger;
//  m_Thiefs = m_halls.TrainThief;
  m_Text = m_halls.text;
  m_Cost = m_halls.Cost;

  DWORD style;
  style = m_BaseclassList.GetStyle();
  //m_BaseclassList.SetStyle(style|LVS_SHOWSELALWAYS);
  DWORD exStyle;
  exStyle = m_BaseclassList.GetExtendedStyle();
  m_BaseclassList.SetExtendedStyle(exStyle|LVS_EX_FULLROWSELECT);

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_halls.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_ChooseTrainer.SetWindowText(name);


  m_BaseclassList.InsertColumn(0, "Baseclass", LVCFMT_LEFT, 100, 0);
  m_BaseclassList.InsertColumn(1, "Min Level", LVCFMT_LEFT, 100, 1);
  m_BaseclassList.InsertColumn(2, "Max Level", LVCFMT_LEFT, 100, 2);

  {
    int i, n;
    n = m_halls.GetTrainableBaseclassCount();
    for (i=0; i<n; i++)
    {
      BASECLASSDEF baseclassDef;
      baseclassDef.baseclassID = m_halls.PeekTrainableBaseclass(i)->baseclassID;
      baseclassDef.minLevel = m_halls.PeekTrainableBaseclass(i)->minLevel;
      baseclassDef.maxLevel = m_halls.PeekTrainableBaseclass(i)->maxLevel;
      baseclassDef.present = TRUE;
      if (baseclassDef.baseclassID == "$$Help") continue;
      m_availBaseclasses.Add(baseclassDef);
    };
  };

  {
    int i, n;
    n = baseclassData.GetCount();
    for (i=0; i<n; i++)
    {
      BASECLASSDEF baseclassDef;
      const BASE_CLASS_DATA *pBaseclass;
      pBaseclass = baseclassData.PeekBaseclass(i);
      baseclassDef.baseclassID = pBaseclass->BaseclassID();
      baseclassDef.minLevel = 1;
      baseclassDef.maxLevel = 40;
      baseclassDef.present = FALSE;
      if (baseclassDef.baseclassID == "$$Help") continue;
      {
        int j,m;
        m = m_availBaseclasses.GetSize();
        for (j=0; j<m; j++)
        {
          if (m_availBaseclasses[j].baseclassID == baseclassDef.baseclassID) break;
        };
        if (j == m)
        {
          m_availBaseclasses.Add(baseclassDef);
        };
      };
    };
  };
  {
    int i, n;
    n = m_availBaseclasses.GetSize();
    for (i=0; i<n; i++)
    {
      char number[40];
      int j;
      j = m_BaseclassList.InsertItem(i, m_availBaseclasses[i].baseclassID);
      sprintf(number,"%d", m_availBaseclasses[i].minLevel);
      m_BaseclassList.SetItemText(j,1,number);
      sprintf(number,"%d", m_availBaseclasses[i].maxLevel);
      m_BaseclassList.SetItemText(j,2,number);
      m_BaseclassList.SetItemData(j, i);
    };
  };

  m_BaseclassList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
  m_BaseclassList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);


  //GotoDlgCtrl(GetDlgItem(IDC_TEXT));
  m_CoinName = globalData.moneyData.GetName(globalData.moneyData.GetDefaultType());
  UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTrainingHall::GetHallData(TRAININGHALL& halls) 
{
	halls = m_halls;	
}

void CTrainingHall::OnOK() 
{
	UpdateData(TRUE);

	m_halls.forceExit = m_ForceExit;
  m_halls.trainableBaseclasses.RemoveAll();
  {
    int i,n;
    n = m_availBaseclasses.GetSize();
    for (i=0; i<n; i++)
    {
      if (m_availBaseclasses[i].present)
      {
        TRAINABLE_BASECLASS tbc;
        tbc.baseclassID = m_availBaseclasses[i].baseclassID;
        tbc.minLevel = m_availBaseclasses[i].minLevel;
        tbc.maxLevel = m_availBaseclasses[i].maxLevel;
        m_halls.Add(tbc);
      };
    };
//  m_halls.TrainCleric = m_Clerics;
//  m_halls.TrainDruid = m_Druids;
//  m_halls.TrainFighter = m_Fighters;
//  m_halls.TrainMagicUser = m_MagicUsers;
//  m_halls.TrainPaladin = m_Paladins;
//  m_halls.TrainRanger = m_Rangers;
//  m_halls.TrainThief = m_Thiefs;
  };
  m_halls.text = m_Text;
  m_halls.Cost = m_Cost;
	
	CDialog::OnOK();
}


CTrainable::CTrainable(CArray<BASECLASSDEF, BASECLASSDEF&> *pBaselasses, CWnd *pParent)	: CDialog(CTrainable::IDD, pParent)
{
  pAvailBaseclass = pBaselasses;
//  m_baseclassName = (*pAvailClass)[0].classID;
  m_minLevel = 3;
  m_maxLevel = 4;
}


BEGIN_MESSAGE_MAP(CTrainable, CDialog)
	//{{AFX_MSG_MAP(CTrainable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CTrainable::OnOK(void)
{
  UpdateData(TRUE);
  char x[100];
  m_baseclassName.GetWindowText(x, 99);
  result.baseclassID = x;
  result.minLevel = m_minLevel;
  result.maxLevel = m_maxLevel;
  result.present = TRUE;
  CDialog::OnOK();
}

BOOL CTrainable::OnInitDialog(void)
{
  CDialog::OnInitDialog();
  UpdateData(true);
  m_baseclassName.ResetContent();
  {
    int i, n;
    n = pAvailBaseclass->GetSize();
    for (i=0; i<n; i++)
    {
      if ((*pAvailBaseclass)[i].present) continue;
      m_baseclassName.AddString((*pAvailBaseclass)[i].baseclassID);
    };
  };
  m_baseclassName.SetWindowText(result.baseclassID);
  m_minLevel = result.minLevel;
  m_maxLevel = result.maxLevel;
  result.present = FALSE;
  UpdateData(FALSE);
  return TRUE;
}

void CTrainable::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrainingHall)
	DDX_Control(pDX, IDC_NAME, m_baseclassName);
	DDX_Text(pDX, IDC_MIN,  m_minLevel);
	DDX_Text(pDX, IDC_MAX,  m_maxLevel);
  DDV_MinMaxUInt(pDX, m_maxLevel, 1, 40);
  DDV_MinMaxUInt(pDX, m_minLevel, 1, 40);
	//}}AFX_DATA_MAP
}
