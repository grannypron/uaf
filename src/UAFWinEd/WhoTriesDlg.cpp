/******************************************************************************
* Filename: WhoTriesDlg.cpp
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
#include "globtext.h"
//#include "GlobalData.h"
#include "Char.h"
#include "GameEvent.h"
#include "TeleportDlg.h"
#include "SelectPicDlg.h"
#include "WhoTriesDlg.h"
#include "ChooseOptions.h"
#include "Help.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern const CString Ability_Dexterity;
extern const CString Ability_Constitution;
extern const CString Ability_Strength;
extern const CString Ability_Intelligence;
extern const CString Ability_Charisma;
extern const CString Ability_Wisdom;




/////////////////////////////////////////////////////////////////////////////
// CWhoTriesDlg dialog


CWhoTriesDlg::CWhoTriesDlg(WHO_TRIES_EVENT_DATA& data, CWnd* pParent /*=NULL*/)
	: CDialog(CWhoTriesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWhoTriesDlg)
	m_AlwaysFails = FALSE;
	m_AlwaysSucceeds = FALSE;
	//m_CheckCha = FALSE;
	//m_CheckCon = FALSE;
	//m_CheckDex = FALSE;
	//m_CheckInt = FALSE;
	//m_CheckStr = FALSE;
	//m_CheckStrBonus = 0;
	//m_CheckWis = FALSE;
	//m_CompareTo = -1;
	//m_CompareDieSides = 0;
	m_FailText = _T("");
	m_NumTries = 0;
	m_SuccessText = _T("");
	m_Text = _T("");
	//}}AFX_DATA_INIT

  m_data = data;
}


void CWhoTriesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWhoTriesDlg)
	DDX_Control(pDX, IDC_CHOOSEART, m_ChooseArt);
	DDX_Control(pDX, IDC_SUCCESSTELEPORT, m_SuccessTeleport);
	DDX_Control(pDX, IDC_SUCCESSACTION, m_SuccessAction);
	DDX_Control(pDX, IDC_FAILTELEPORT, m_FailTeleport);
	DDX_Control(pDX, IDC_FAILACTION, m_FailAction);
	DDX_Check(pDX, IDC_ALWAYSFAILS, m_AlwaysFails);
	DDX_Check(pDX, IDC_ALWAYSSUCCEEDS, m_AlwaysSucceeds);
	//DDX_Check(pDX, IDC_CHECKCHARISMA, m_CheckCha);
	//DDX_Check(pDX, IDC_CHECKCON, m_CheckCon);
	//DDX_Check(pDX, IDC_CHECKDEX, m_CheckDex);
	//DDX_Check(pDX, IDC_CHECKINT, m_CheckInt);
	//DDX_Check(pDX, IDC_CHECKSTR, m_CheckStr);
	//DDX_Text(pDX, IDC_CHECKSTRBONUS, m_CheckStrBonus);
	//DDX_Check(pDX, IDC_CHECKWIS, m_CheckWis);
	//DDX_Radio(pDX, IDC_COMPARETODIE, m_CompareTo);
	//DDX_Text(pDX, IDC_DIESIDES, m_CompareDieSides);
	DDX_Text(pDX, IDC_FAILTEXT, m_FailText);
	DDX_Text(pDX, IDC_NUMTRIES, m_NumTries);
	DDX_Text(pDX, IDC_SUCCESSTEXT, m_SuccessText);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWhoTriesDlg, CDialog)
	//{{AFX_MSG_MAP(CWhoTriesDlg)
	//ON_BN_CLICKED(IDC_CHECKSTR, OnCheckstr)
	ON_BN_CLICKED(IDC_ALWAYSFAILS, OnAlwaysfails)
	ON_BN_CLICKED(IDC_ALWAYSSUCCEEDS, OnAlwayssucceeds)
	ON_CBN_SELCHANGE(IDC_FAILACTION, OnSelchangeFailaction)
	ON_BN_CLICKED(IDC_FAILTELEPORT, OnFailteleport)
	ON_CBN_SELCHANGE(IDC_SUCCESSACTION, OnSelchangeSuccessaction)
	ON_BN_CLICKED(IDC_SUCCESSTELEPORT, OnSuccessteleport)
	ON_BN_CLICKED(IDC_CHOOSEART, OnChooseart)
	ON_BN_CLICKED(IDC_SelectSkills, OnSelectSkills)
  ON_BN_CLICKED(IDC_WHO_TRIES, OnWhoTriesQuestion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
struct WHO_TRIES_EVENT_DATA {
  BOOL alwaysSucceeds;
  BOOL alwaysFails;
  BOOL checkSTR;
  BOOL checkINT;
  BOOL checkWIS;
  BOOL checkDEX;
  BOOL checkCON;
  BOOL checkCHA;
  BOOL checkPP;
  BOOL checkOL;
  BOOL checkFT;
  BOOL checkMS;
  BOOL checkHS;
  BOOL checkHN;
  BOOL checkCW;
  BOOL checkRL;
  BYTE strBonus;
  BOOL compareToDie; // if false, then compareDie is actual compare value
  int  compareDie;   // dice sides, or actual compare value
  int  NbrTries;
  TRANSFER_DATA successTransferData;
  TRANSFER_DATA failTransferData;
  int successChain;
  int failChain;
  long UserResult;
  passwordActionType successAction;
  passwordActionType failAction;
};
*/

/////////////////////////////////////////////////////////////////////////////
// CWhoTriesDlg message handlers

BOOL CWhoTriesDlg::OnInitDialog() 
{
   int i;
   CDialog::OnInitDialog();

   m_skills.Copy(m_data.event_asl);
   m_FailText = m_data.text3;
   m_SuccessText = m_data.text2;
   m_Text = m_data.text;
   
   m_SuccessAction.ResetContent();
   m_FailAction.ResetContent();
   
   for (i=0;i<NUM_PASSACTION_TYPES;i++)
   {
      m_SuccessAction.AddString(PasswordActionText[i]);
      m_FailAction.AddString(PasswordActionText[i]);
   }
   
   m_SuccessAction.SetCurSel((int)m_data.successAction);
   m_FailAction.SetCurSel((int)m_data.failAction);
   
   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
   m_ChooseArt.SetWindowText(name);

   m_AlwaysSucceeds = m_data.alwaysSucceeds;
   m_AlwaysFails = m_data.alwaysFails;
   //m_CheckStr = m_data.checkSTR;
   //m_CheckInt = m_data.checkINT;
   //m_CheckWis = m_data.checkWIS;
   //m_CheckDex = m_data.checkDEX;
   //m_CheckCon = m_data.checkCON;
   //m_CheckCha = m_data.checkCHA;
   //m_CheckStrBonus = m_data.strBonus;

   //if (m_data.compareToDie)
   //  m_CompareTo = 0;
   //else
   //  m_CompareTo = 1;

   //m_CompareDieSides = m_data.compareDie;
   m_NumTries = m_data.NbrTries;

 // CWnd *pWnd = GetDlgItem(IDC_CHECKSTRBONUS);
 // if (pWnd != NULL)
 //    pWnd->EnableWindow(m_CheckStr);

  if (m_AlwaysSucceeds)
    m_AlwaysFails = FALSE;

   GotoDlgCtrl(GetDlgItem(IDC_TEXT));
   UpdateData(FALSE);
   SetControlStates();
   
   return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/*
void CWhoTriesDlg::OnCheckstr() 
{
  UpdateData(TRUE);
  
  CWnd *pWnd = GetDlgItem(IDC_CHECKSTRBONUS);      
  if (pWnd != NULL)
     pWnd->EnableWindow(m_CheckStr);
}
*/
void CWhoTriesDlg::OnAlwaysfails() 
{
  UpdateData(TRUE);
  
  if (m_AlwaysFails)
  {
    m_AlwaysSucceeds = FALSE;
    UpdateData(FALSE);
  }
}

void CWhoTriesDlg::OnAlwayssucceeds() 
{
  UpdateData(TRUE);
  
  if (m_AlwaysSucceeds)
  {
    m_AlwaysFails = FALSE;
    UpdateData(FALSE);
  }
}

void CWhoTriesDlg::OnSelchangeFailaction() 
{
   UpdateData(TRUE);
   
   m_data.failAction = 
      (passwordActionType) m_FailAction.GetCurSel();
   
   SetControlStates();
   UpdateData(FALSE);
}

void CWhoTriesDlg::OnFailteleport() 
{
	CTeleportDlg dlg(m_data.failTransferData);

  if (dlg.DoModal() == IDOK)
    dlg.GetData(m_data.failTransferData);
}

void CWhoTriesDlg::OnSelchangeSuccessaction() 
{
   UpdateData(TRUE);
   
   m_data.successAction = 
      (passwordActionType) m_SuccessAction.GetCurSel();
   
   SetControlStates();
   UpdateData(FALSE);
}

void CWhoTriesDlg::OnSuccessteleport() 
{
	CTeleportDlg dlg(m_data.successTransferData);

  if (dlg.DoModal() == IDOK)
    dlg.GetData(m_data.successTransferData);
}

void CWhoTriesDlg::SetControlStates(void)
{
   m_SuccessTeleport.EnableWindow(FALSE);
   m_FailTeleport.EnableWindow(FALSE);
   
   switch (m_data.successAction) 
   {
   case ChainEvent:
      break;
   case Teleport:
      m_SuccessTeleport.EnableWindow(TRUE);
      break;
   default:
      break;
   }
   
   switch (m_data.failAction) 
   {
   case ChainEvent:
      break;
   case Teleport:
      m_FailTeleport.EnableWindow(TRUE);
      break;
   default:
      break;
   }
}

void CWhoTriesDlg::OnOK() 
{
  UpdateData(TRUE);

  m_data.event_asl.Copy(m_skills);
  m_data.text = m_Text;  
  m_data.text3 = m_FailText;
  m_data.text2 = m_SuccessText;

  m_data.successAction = 
    (passwordActionType) m_SuccessAction.GetCurSel();
  m_data.failAction =
    (passwordActionType) m_FailAction.GetCurSel();
  m_data.alwaysSucceeds = m_AlwaysSucceeds;
  m_data.alwaysFails = m_AlwaysFails;
  m_data.checkSTR = FALSE; //m_CheckStr;
  m_data.checkINT = FALSE; //m_CheckInt;
  m_data.checkWIS = FALSE; //m_CheckWis;
  m_data.checkDEX = FALSE; //m_CheckDex;
  m_data.checkCON = FALSE; //m_CheckCon;
  m_data.checkCHA = FALSE; //m_CheckCha;
  m_data.strBonus = FALSE; //m_CheckStrBonus;

  //if (m_CompareTo == 0)
  //  m_data.compareToDie = TRUE;
  //else
  //  m_data.compareToDie = FALSE;

  //m_data.compareDie = m_CompareDieSides;
  m_data.NbrTries = m_NumTries;

	CDialog::OnOK();
}

void CWhoTriesDlg::GetData(WHO_TRIES_EVENT_DATA &data)
{
  data = m_data;
}

void CWhoTriesDlg::OnChooseart() 
{
   CSelectPicDlg dlg(m_data.pic, SmallPicDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_data.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_ChooseArt.SetWindowText(name);
   }
}

void CWhoTriesDlg::OnSelectSkills(void)
{
  CString suggestionKey, helpKey;
  ASLENTRY *pSkills;
  SUBSTRINGS skills;
  CArray<CStringPAIR, CStringPAIR&> suggestionList;
  {
    int i, n;
    CStringPAIR entry;
    n = baseclassData.GetCount();
    for (i=0; i<n; i++)
    {
      const BASE_CLASS_DATA *pBaseclass;
      int j,m;
      pBaseclass = baseclassData.PeekBaseclass(i);
      m = pBaseclass->GetSkillCount();
      for (j=0; j<m; j++)
      {
        const SKILL *pSkill;
        pSkill = pBaseclass->PeekSkill(j);
        {
          int k, l;
          CString suggestion;
          suggestion = pSkill->skillID + ",50";
          l = suggestionList.GetSize();
          for (k=0; k<l; k++)
          {
            if (suggestion == suggestionList[k].Value()) break;
          };
          if (k != l) continue;
          entry.m_key = "Skill";
          entry.m_value = suggestion;
          suggestionList.Add(entry);
        };
      };
    };
  }
  {
    int i, n;
    CStringPAIR entry;
    n = raceData.GetCount();
    for (i=0; i<n; i++)
    {
      const RACE_DATA *pRace;
      int j,m;
      pRace = raceData.PeekRace(i);
      m = pRace->GetSkillCount();
      for (j=0; j<m; j++)
      {
        const SKILL *pSkill;
        pSkill = pRace->PeekSkill(j);
        {
          int k, l;
          CString suggestion;
          suggestion = pSkill->skillID + ",10";
          l = suggestionList.GetSize();
          for (k=0; k<l; k++)
          {
            if (suggestion == suggestionList[k].Value()) break;
          };
          if (k != l) continue;
          entry.m_key = "Skill";
          entry.m_value = suggestion;
          suggestionList.Add(entry);
        };
      };
    };
  };
  {
    CStringPAIR entry;
    entry.m_key = "Ability";
    entry.m_value = Ability_Strength+",50";
    suggestionList.Add(entry);
    entry.m_value = Ability_Wisdom+",50";
    suggestionList.Add(entry);
    entry.m_value = Ability_Intelligence+",50";
    suggestionList.Add(entry);
    entry.m_value = Ability_Dexterity+",50";
    suggestionList.Add(entry);
    entry.m_value = Ability_Constitution+",50";
    suggestionList.Add(entry);
    entry.m_value = Ability_Charisma+",50";
    suggestionList.Add(entry);
  };
  //suggestionKey = "$EVENT_WhoTries_Suggestion";
  helpKey = "$EVENT_WhoTries_Help";
  pSkills = m_skills.Find("Attempt");
  if (pSkills != NULL)
  {
    skills = pSkills->Value();
  };
  CChooseOptions dlg(&skills, suggestionKey, &suggestionList, helpKey);
  if (dlg.DoModal() == IDOK)
  {
    m_skills.Insert("Attempt", dlg.m_result, ASLF_EDITOR);
  }
}

void CWhoTriesDlg::OnWhoTriesQuestion() 
{
	CHelp dlg("WHO_TRIES");

  dlg.DoModal();
}