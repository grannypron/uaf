/******************************************************************************
* Filename: Chardlg.cpp
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
//#include "externs.h"
#include "UAFWinEd.h"
#include "itemdlg.h"
#include "SmallPicDlg.h"
#include "IconPicDlg.h"
#include "GlobText.h"
#include "SpellDlg.h"
#include "GetMoney.h"
#include "GlobalData.h"
#include "EventViewer.h"
#include "CharDlg.h"
#include "EditAttributes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCharacter dialog

//CCharacter::CCharacter(int CharNum, CWnd* pParent /*=NULL*/)
CCharacter::CCharacter(const CHARACTER_ID& characterID, CWnd* pParent /*=NULL*/)
	: CDialog(CCharacter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCharacter)
	m_CharacterName = "No Name";
	m_HitPoints = 0;
	m_Charisma = 3;
  m_Morale = 50;
	m_Constitution = 3;
	m_Dexterity = 3;
	m_Intelligence = 3;
	m_Strength = 3;
	m_Wisdom = 3;
	m_IsPreGen = FALSE;
	//m_Level1 = 0;
	//m_Level2 = 0;
	//m_Level3 = 0;
  m_Level[0] = 0;
  m_Level[1] = 0;
  m_Level[2] = 0;
	m_Class[0] = _T("");
	m_Class[1] = _T("");
	m_Class[2] = _T("");
	m_StartAge = 0;
	m_StrengthBonus = 0;
	m_CanTradeItems = TRUE;
	m_CanBeSaved = TRUE;
	m_TalkLabel = _T("");
	m_ExamineLabel = _T("");
	//m_ExpPoints1 = 0;
	//m_ExpPoints2 = 0;
	//m_ExpPoints3 = 0;
  m_ExpPoints[0] = 0;
  m_ExpPoints[1] = 0;
  m_ExpPoints[2] = 0;
	m_DisableTalkIfDead = TRUE;
	m_AllowInCombat = TRUE;
	//}}AFX_DATA_INIT

  //m_CharNum = CharNum;
  m_characterID = characterID;

  //if (!globalData.charData.GetCharacterData(m_CharNum, m_CharData))
  if (!globalData.charData.FetchCharacter(m_characterID, &m_CharData))
  {
    m_CharData.Clear();
    m_CharData.SetType(NPC_TYPE);
  }
}

void CCharacter::DoDataExchange(CDataExchange* pDX)
{
   if (!pDX->m_bSaveAndValidate)
   {
      m_CharacterName = m_CharData.GetName();
      m_HitPoints = m_CharData.GetMaxHitPoints();
      m_Charisma = m_CharData.GetPermCha();
      m_Morale = m_CharData.GetMorale();
      m_Constitution = m_CharData.GetPermCon();
      m_Dexterity = m_CharData.GetPermDex();
      m_Intelligence = m_CharData.GetPermInt();
      m_Strength = m_CharData.GetPermStr();
      m_StrengthBonus = m_CharData.GetPermStrMod();
      m_Wisdom = m_CharData.GetPermWis();
      m_IsPreGen = m_CharData.IsPreGen;
      m_StartAge = m_CharData.GetAge();
      m_CanTradeItems = m_CharData.IsAbleToTrade();
      m_CanBeSaved = m_CharData.CanBeSaved;
      m_AllowInCombat = m_CharData.GetAllowInCombat();;
      m_DisableTalkIfDead=m_CharData.DisableTalkIfDead;
      m_TalkLabel=m_CharData.TalkLabel;
      if (m_TalkLabel.GetLength()>10) m_TalkLabel.SetAt(10,'\0');
      m_ExamineLabel = m_CharData.ExamineLabel;      
      if (m_ExamineLabel.GetLength()>10) m_ExamineLabel.SetAt(10,'\0');

      if (m_ChooseAlignment.m_hWnd != NULL)
         m_ChooseAlignment.SetCurSel((int)m_CharData.GetAlignment());

      if (m_ChooseRace.m_hWnd != NULL)
      {
        //int rkey = m_CharData.race();
        RACE_ID rID = m_CharData.RaceID();
        int j;
        j =raceData.LocateRace(rID);
        m_ChooseRace.SetCurSel(0);
        for (int i=0;i<m_ChooseRace.GetCount();i++)
        {
          //if ((int)m_ChooseRace.GetItemData(i) == rkey)
          if ((int)m_ChooseRace.GetItemData(i) == j)
          {
            m_ChooseRace.SetCurSel(i);
            break;
          }
        }
      }

      if (m_ChooseClass.m_hWnd != NULL)
      {
         //m_ChooseClass.SetCurSel((int)m_CharData.GetClass());
        //int key = m_CharData.GetClass();
        CLASS_ID cID = m_CharData.GetClass();
        int j;
        j = classData.LocateClass(cID);
        for (int i=0;i<m_ChooseClass.GetCount();i++)
        {
          //if ((int)m_ChooseClass.GetItemData(i) == key)
          if ((int)m_ChooseClass.GetItemData(i) == j)
          {
            m_ChooseClass.SetCurSel(i);
            break;
          }
        }        
      }

      if (m_ChooseGender.m_hWnd != NULL)
         m_ChooseGender.SetCurSel(m_CharData.GetGender());

      char name[MAX_MEDITBUTTON_TEXT+1];

      if (m_ChooseIcon.m_hWnd != NULL)
      {
         getBaseName(m_CharData.icon.filename, name, MAX_MEDITBUTTON_TEXT);
         m_ChooseIcon.SetWindowText(name);
      }
    
      if (m_ChooseSpic.m_hWnd != NULL)
      {
         getBaseName(m_CharData.smallPic.filename, name, MAX_MEDITBUTTON_TEXT);
         m_ChooseSpic.SetWindowText(name);
      }

      UpdateLevels(pDX->m_bSaveAndValidate);
      UpdateSpellAbility();
   }

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharacter)
	DDX_Control(pDX, IDC_EXAMINEEVENT, m_ExamineEvent);
	DDX_Control(pDX, IDC_TALKEVENT, m_TalkEvent);
	DDX_Control(pDX, IDC_CHARSPIC, m_ChooseSpic);
	DDX_Control(pDX, IDC_CHARICONPIC, m_ChooseIcon);
	DDX_Control(pDX, IDC_CHOOSERACE, m_ChooseRace);
	DDX_Control(pDX, IDC_CHOOSEGENDER, m_ChooseGender);
	DDX_Control(pDX, IDC_CHOOSECLASS, m_ChooseClass);
	DDX_Control(pDX, IDC_CHOOSEALIGNMENT, m_ChooseAlignment);
	DDX_Control(pDX, IDC_CHOOSESPELLS, m_ChooseSpells);
	DDX_Control(pDX, IDC_CHARITEMS, m_CharItems);
	DDX_Text(pDX, IDC_CHARACTERNAME, m_CharacterName);
	DDV_MaxChars(pDX, m_CharacterName, 30);
	DDX_Text(pDX, IDC_HITPOINTS, m_HitPoints);
	DDX_Text(pDX, IDC_CHARISMA, m_Charisma);
	DDV_MinMaxInt(pDX, m_Charisma, 3, 25);
  DDX_Text(pDX, IDC_MORALE2, m_Morale);
  DDV_MinMaxInt(pDX, m_Morale, 0, 100);
	DDX_Text(pDX, IDC_CONSTITUTION, m_Constitution);
	DDV_MinMaxInt(pDX, m_Constitution, 3, 25);
	DDX_Text(pDX, IDC_DEXTERITY, m_Dexterity);
	DDV_MinMaxInt(pDX, m_Dexterity, 3, 25);
	DDX_Text(pDX, IDC_INTELLIGENCE, m_Intelligence);
	DDV_MinMaxInt(pDX, m_Intelligence, 3, 25);
	DDX_Text(pDX, IDC_STRENGTH, m_Strength);
	DDV_MinMaxInt(pDX, m_Strength, 3, 25);
	DDX_Text(pDX, IDC_WISDOM, m_Wisdom);
	DDV_MinMaxInt(pDX, m_Wisdom, 3, 25);
	DDX_Check(pDX, IDC_ISPREGEN, m_IsPreGen);
	//DDX_Text(pDX, IDC_LEVEL1, m_Level1);
	//DDX_Text(pDX, IDC_LEVEL2, m_Level2);
	//DDX_Text(pDX, IDC_LEVEL3, m_Level3);
	DDX_Text(pDX, IDC_LEVEL1, m_Level[0]);
	DDX_Text(pDX, IDC_LEVEL2, m_Level[1]);
	DDX_Text(pDX, IDC_LEVEL3, m_Level[2]);
	DDX_Text(pDX, IDC_L1CLASS, m_Class[0]);
	DDX_Text(pDX, IDC_L2CLASS, m_Class[1]);
	DDX_Text(pDX, IDC_L3CLASS, m_Class[2]);
	DDX_Text(pDX, IDC_STARTAGE, m_StartAge);
	DDX_Text(pDX, IDC_STRENGTHBONUS, m_StrengthBonus);
	DDV_MinMaxInt(pDX, m_StrengthBonus, 0, 100);
	DDX_Check(pDX, IDC_CANTRADEITEMS, m_CanTradeItems);
	DDX_Check(pDX, IDC_CANBESAVED, m_CanBeSaved);
	DDX_Text(pDX, IDC_TALKLABEL, m_TalkLabel);
	DDX_Text(pDX, IDC_EXAMINELABEL, m_ExamineLabel);
	//DDX_Text(pDX, IDC_EXPPTS1, m_ExpPoints1);
	//DDX_Text(pDX, IDC_EXPPTS2, m_ExpPoints2);
	//DDX_Text(pDX, IDC_EXPPTS3, m_ExpPoints3);
	DDX_Text(pDX, IDC_EXPPTS1, m_ExpPoints[0]);
	DDX_Text(pDX, IDC_EXPPTS2, m_ExpPoints[1]);
	DDX_Text(pDX, IDC_EXPPTS3, m_ExpPoints[2]);
	DDX_Check(pDX, IDC_DISABLETALKIFDEAD, m_DisableTalkIfDead);
	DDX_Check(pDX, IDC_ALLOWEDINCOMBAT, m_AllowInCombat);
	//}}AFX_DATA_MAP

   if (pDX->m_bSaveAndValidate)
   {
      m_CharData.SetAge(m_StartAge);
      m_CharData.SetName(m_CharacterName);
      m_CharData.characterID = m_CharData.name;
      m_CharData.SetHitPoints(m_HitPoints);
      m_CharData.SetMaxHitPoints(m_HitPoints);
      m_CharData.SetPermCha(m_Charisma);
      m_CharData.SetMorale(m_Morale);
      m_CharData.SetPermCon(m_Constitution);
      m_CharData.SetPermDex(m_Dexterity);
      m_CharData.SetPermInt(m_Intelligence);
      m_CharData.SetPermStr(m_Strength);
      if (m_Strength < 18) m_StrengthBonus=0;
      m_CharData.SetPermStrMod(m_StrengthBonus);
      m_CharData.SetPermWis(m_Wisdom);
      m_CharData.IsPreGen = m_IsPreGen;
      m_CharData.SetAbleToTrade(m_CanTradeItems);
      m_CharData.CanBeSaved = m_CanBeSaved;
      m_CharData.SetAllowInCombat(m_AllowInCombat);

      m_CharData.DisableTalkIfDead = m_DisableTalkIfDead;
      if (m_TalkLabel.GetLength()>10) m_TalkLabel.SetAt(10,'\0');
      m_CharData.TalkLabel=m_TalkLabel;
      if (m_ExamineLabel.GetLength()>10) m_ExamineLabel.SetAt(10,'\0');
      m_CharData.ExamineLabel=m_ExamineLabel;
      
      m_CharData.SetAlignment((alignmentType)m_ChooseAlignment.GetCurSel());

      int index;
      index = m_ChooseRace.GetCurSel();

      //int rkey = m_ChooseRace.GetItemData(index);
      //m_CharData.race ((raceType)rkey);
      //m_CharData.RaceID ((raceType)rkey);
      m_CharData.RaceID(raceData.PeekRace(index)->RaceID());

      index = m_ChooseClass.GetCurSel();
      if (index >= 0)
      {
        //int ckey = m_ChooseClass.GetItemData(index);
        //m_CharData.SetClass ((classType)ckey);      
        //m_CharData.SetClass((classType)m_ChooseClass.GetCurSel());
        m_CharData.SetClass(classData.PeekClass(index)->ClassID());
      };


      m_CharData.SetGender((genderType)m_ChooseGender.GetCurSel());
  
      UpdateLevels(pDX->m_bSaveAndValidate);
      UpdateSpellAbility();
   }
}


BEGIN_MESSAGE_MAP(CCharacter, CDialog)
	//{{AFX_MSG_MAP(CCharacter)
	ON_BN_CLICKED(IDC_CHARITEMS, OnCharitems)
	ON_BN_CLICKED(IDC_CHARSPIC, OnCharspic)
	ON_BN_CLICKED(IDC_CHARICONPIC, OnChariconpic)
	ON_BN_CLICKED(IDC_CHOOSESPELLS, OnChoosespells)
  ON_BN_CLICKED(IDC_SPECABS, OnSpecAbs)
  ON_BN_CLICKED(IDC_SpecAbsHelp, OnSpecAbsHelp)
	ON_CBN_SELCHANGE(IDC_CHOOSECLASS, OnSelchangeChooseclass)
	ON_BN_CLICKED(IDC_MONEY, OnMoney)
	ON_BN_CLICKED(IDC_ROLLSTATS, OnRollstats)
	ON_BN_CLICKED(IDC_TALKEVENT, OnTalkevent)
	ON_BN_CLICKED(IDC_EXAMINEEVENT, OnExamineevent)
	ON_EN_KILLFOCUS(IDC_EXPPTS1, OnKillfocusExppts1)
	ON_EN_KILLFOCUS(IDC_EXPPTS2, OnKillfocusExppts2)
	ON_EN_KILLFOCUS(IDC_EXPPTS3, OnKillfocusExppts3)
	ON_EN_KILLFOCUS(IDC_LEVEL1, OnKillfocusLevel1)
	ON_EN_KILLFOCUS(IDC_LEVEL2, OnKillfocusLevel2)
	ON_EN_KILLFOCUS(IDC_LEVEL3, OnKillfocusLevel3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharacter message handlers


BOOL CCharacter::OnInitDialog() 
{
	CDialog::OnInitDialog();

  int i, n;
	m_ChooseRace.ResetContent();
	m_ChooseGender.ResetContent();
	m_ChooseClass.ResetContent();
	m_ChooseAlignment.ResetContent();
  if (m_CharData.GetPermStr() < 3) m_CharData.SetPermStr(3);
  if (m_CharData.GetPermCha() < 3) m_CharData.SetPermCha(3);
  if (m_CharData.GetPermCon() < 3) m_CharData.SetPermCon(3);
  if (m_CharData.GetPermDex() < 3) m_CharData.SetPermDex(3);
  if (m_CharData.GetPermInt() < 3) m_CharData.SetPermInt(3);
  if (m_CharData.GetPermWis() < 3) m_CharData.SetPermWis(3);
  
  //for (i=0;i<NUM_RACE_TYPES; i++)
  //  m_ChooseRace.AddString(RaceText[i]);
  //POSITION pos = raceData.GetHeadPosition();
  //while (pos!=NULL)
  n = raceData.GetCount();
  for (i=0; i<n; i++)
  {
    const RACE_DATA  *pRace;
    pRace = raceData.PeekRace(i);
    if (pRace->Name() == "$$Help") continue;
    //int index = m_ChooseRace.AddString(raceData.PeekAtPos(pos).Name());
    int index = m_ChooseRace.AddString(pRace->RaceID());
    //m_ChooseRace.SetItemData(index, raceData.GetKeyAt(pos));
    m_ChooseRace.SetItemData(index, i);
    //raceData.GetNext(pos);
  }
  {
    int count;
    count = 0;
    //for (i=0;i<NUM_CLASS_TYPES; i++)
    //  m_ChooseClass.AddString(ClassText[i]);
    //pos = classData.GetHeadPosition();
    //while ( (pos!=NULL) && (count<NUM_CLASS_TYPES))
    n = classData.GetCount();
    for (i=0; i<n; i++)
    {
      count++;
      //int index = m_ChooseClass.AddString(classData.PeekAtPos(pos).m_name);
      int index = m_ChooseClass.AddString(classData.PeekClass(i)->ClassID());
      //m_ChooseClass.SetItemData(index, classData.GetKeyAt(pos));
      m_ChooseClass.SetItemData(index, i);
      //classData.GetNext(pos);
    };
  };
  
  for (i=0;i<NUM_GENDER_TYPES; i++)
    m_ChooseGender.AddString(genderText[i]);
  for (i=0;i<NUM_ALIGNMENT_TYPES;i++)
    m_ChooseAlignment.AddString(alignmentText[i]);
  char name[MAX_MEDITBUTTON_TEXT+1];

  if (!globalData.eventData.IsValidEvent(m_CharData.TalkEvent))
    m_CharData.TalkEvent=NO_EVENT;

  getBaseName(GetEventIdDescription(m_CharData.TalkEvent, GlobalEventSrc), name, MAX_MEDITBUTTON_TEXT);
  m_TalkEvent.SetWindowText(name);
  m_TalkLabel=m_CharData.TalkLabel;
  if (m_TalkLabel.GetLength()>10) m_TalkLabel.SetAt(10,'\0');

  if (!globalData.eventData.IsValidEvent(m_CharData.ExamineEvent))
    m_CharData.ExamineEvent=NO_EVENT;

  getBaseName(GetEventIdDescription(m_CharData.ExamineEvent, GlobalEventSrc), name, MAX_MEDITBUTTON_TEXT);
  m_ExamineEvent.SetWindowText(name);
  m_ExamineLabel=m_CharData.ExamineLabel;
  if (m_ExamineLabel.GetLength()>10) m_ExamineLabel.SetAt(10,'\0');

	UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCharacter::OnCharitems() 
{
  UpdateData(TRUE);
	CItemDlg iDlg(m_CharData.myItems);
	iDlg.m_AllItemsAreIdentified = TRUE;

	UINT result = iDlg.DoModal();

	if (result == IDOK)
		iDlg.GetItemList(m_CharData.myItems);				
}

void CCharacter::UpdateSpellAbility()
{
  CWnd *pWnd;
  
  m_CharData.UpdateSpellAbility();


/*
  switch (m_CharData.GetClass())
  {
   case Cleric:
   case Ranger:
   case Paladin:
   case Druid:
   case C_F:
   case C_R:
   case C_T:
   case F_D:
   case MagicUser:
   case F_MU:
   case F_MU_T:
   case MU_T:
   case C_F_MU:
   case C_MU:
   case D_MU:
   case F_MU_D:
*/

  //if (m_CharData.m_spellbook.GetCount() > 0)
  {
      pWnd = GetDlgItem(IDC_CHOOSESPELLS);
      if (pWnd != NULL)
        pWnd->EnableWindow(TRUE);
/*
      break;
   default:
*/
  }
  //else
  //{
  //    pWnd = GetDlgItem(IDC_CHOOSESPELLS);
  //    if (pWnd != NULL)
  //      pWnd->EnableWindow(FALSE);
/*
      break;
*/
  //} 
}

void CCharacter::OnCharspic() 
{
  UpdateData(TRUE);


	CSmallPicDlg dlg(m_CharData.smallPic);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    m_CharData.smallPic = dlg.m_SmallPic;
    m_CharData.smallPic.picType = SmallPicDib;
  }

  UpdateData(FALSE);
}

void CCharacter::OnChariconpic() 
{
  UpdateData(TRUE);

	CIconPicDlg dlg(m_CharData.icon);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
    dlg.GetData(m_CharData.icon);

  UpdateData(FALSE);
}

void CCharacter::OnChoosespells()
{
  UpdateData(TRUE);  

	CSpell dlg(m_CharData.m_spellbook);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
    dlg.GetSpellList(m_CharData.m_spellbook);

  UpdateData(FALSE);
}

void CCharacter::OnSelchangeChooseclass() 
{
  UpdateData(TRUE);
  //m_CharData.m_spellbook.Clear();
  UpdateData(FALSE);
}

void CCharacter::UpdateLevels(BOOL ddx)
{
   if (ddx)
      SetLevels();
   else
      GetLevels();
}

void CCharacter::GetLevels()
{

/*
   int l1 = 0;
   int l2 = 0;
   int l3 = 0;
   int ep1=0;
   int ep2=0;
   int ep3=0;

   m_L1Class="";
   m_L2Class="";
   m_L3Class="";
*/
  
  int level[3];
  int exp[3];
    
  int i;
  for (i=0; i<3; i++)
  {
    level[i] = 0;
    exp[i] = 0;
    m_Class[i] = "";
  };

   //int charClass = m_CharData.GetClass();
   CLASS_ID classID = m_CharData.GetClass();



/*
   switch (charClass)
   {
   case Fighter:
      l1 = m_CharData.GetCurrentLevel(FighterFlag);
      ep1 = m_CharData.GetCurrExp(FighterFlag);

      // 2 Dec 2009 PRS
      //m_L1Class = ClassText[Fighter];
      m_L1Class = classData.ClassName(Fighter);

      break;
   case Cleric:
      l1 = m_CharData.GetCurrentLevel(ClericFlag);
      ep1 = m_CharData.GetCurrExp(ClericFlag);

      // 2 Dec 2009 PRS      
      //m_L1Class = ClassText[Cleric];
      m_L1Class = classData.ClassName(Cleric);

      break;
   case Ranger:
      l1 = m_CharData.GetCurrentLevel(RangerFlag);
      ep1 = m_CharData.GetCurrExp(RangerFlag);

      //2Dec 2009 PRS
      //m_L1Class = ClassText[Ranger];
      m_L1Class = classData.ClassName(Ranger);

      break;
   case Paladin:
      l1 = m_CharData.GetCurrentLevel(PaladinFlag);
      ep1 = m_CharData.GetCurrExp(PaladinFlag);

      //2 DEC 2009 PRS      
      //m_L1Class = ClassText[Paladin];
      m_L1Class = classData.ClassName(Paladin);

      break;
   case MagicUser:
      l1 = m_CharData.GetCurrentLevel(MagicUserFlag);
      ep1 = m_CharData.GetCurrExp(MagicUserFlag);

      //2 Dec 2009 PRS      
      //m_L1Class = ClassText[MagicUser];
      m_L1Class = classData.ClassName(MagicUser);

      break;
   case Thief:
      l1 = m_CharData.GetCurrentLevel(ThiefFlag);
      ep1 = m_CharData.GetCurrExp(ThiefFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[Thief];
      m_L1Class = classData.ClassName(Thief);

      break;
   case Druid:
      l1 = m_CharData.GetCurrentLevel(DruidFlag);
      ep1 = m_CharData.GetCurrExp(DruidFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[Druid];
      m_L1Class = classData.ClassName(Druid);

      break;
   case C_F:
      l1 = m_CharData.GetCurrentLevel(ClericFlag);
      ep1 = m_CharData.GetCurrExp(ClericFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[Cleric];
      m_L1Class = classData.ClassName(Cleric);

      l2 = m_CharData.GetCurrentLevel(FighterFlag);
      ep2 = m_CharData.GetCurrExp(FighterFlag);

      //2Dec2009 PRS      
      //m_L2Class = ClassText[Fighter];   
      m_L2Class = classData.ClassName(Fighter);   

      break;
   case C_F_MU:
      l1 = m_CharData.GetCurrentLevel(ClericFlag);
      ep1 = m_CharData.GetCurrExp(ClericFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[Cleric];
      m_L1Class = classData.ClassName(Cleric);

      l2 = m_CharData.GetCurrentLevel(FighterFlag);
      ep2 = m_CharData.GetCurrExp(FighterFlag);

      //2Dec2009 PRS      
      //m_L2Class = ClassText[Fighter];
      m_L2Class = classData.ClassName(Fighter);

      l3 = m_CharData.GetCurrentLevel(MagicUserFlag);
      ep3 = m_CharData.GetCurrExp(MagicUserFlag);

      //2Dec2009 PRS      
      //m_L3Class = ClassText[MagicUser];   
      m_L3Class = classData.ClassName(MagicUser);   

      break;
   case C_R:
      l1 = m_CharData.GetCurrentLevel(ClericFlag);
      ep1 = m_CharData.GetCurrExp(ClericFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[Cleric];
      m_L1Class = classData.ClassName(Cleric);

      l2 = m_CharData.GetCurrentLevel(RangerFlag);
      ep2 = m_CharData.GetCurrExp(RangerFlag);

      //2Dec2009 PRS      
      //m_L2Class = ClassText[Ranger];
      m_L2Class = classData.ClassName(Ranger);

      break;
   case C_MU:
      l1 = m_CharData.GetCurrentLevel(ClericFlag);
      ep1 = m_CharData.GetCurrExp(ClericFlag);
      
      //2Dec2009 PRS      
      //m_L1Class = ClassText[Cleric];
      m_L1Class = classData.ClassName(Cleric);

      l2 = m_CharData.GetCurrentLevel(MagicUserFlag);
      ep2 = m_CharData.GetCurrExp(MagicUserFlag);

      //2Dec2009 PRS      
      //m_L2Class = ClassText[MagicUser];   
      m_L2Class = classData.ClassName(MagicUser);   

      break;
   case C_T:
      l1 = m_CharData.GetCurrentLevel(ClericFlag);
      ep1 = m_CharData.GetCurrExp(ClericFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[Cleric];
      m_L1Class = classData.ClassName(Cleric);

      l2 = m_CharData.GetCurrentLevel(ThiefFlag);
      ep2 = m_CharData.GetCurrExp(ThiefFlag);

      //2Dec2009 PRS      
      //m_L2Class = ClassText[Thief];   
      m_L2Class = classData.ClassName(Thief);   

      break;
   case F_MU:
      l1 = m_CharData.GetCurrentLevel(FighterFlag);
      ep1 = m_CharData.GetCurrExp(FighterFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[Fighter];
      m_L1Class = classData.ClassName(Fighter);

      l2 = m_CharData.GetCurrentLevel(MagicUserFlag);
      ep2 = m_CharData.GetCurrExp(MagicUserFlag);

      //2Dec2009 PRS      
      //m_L2Class = ClassText[MagicUser];   
      m_L2Class = classData.ClassName(MagicUser);   

      break;
   case F_T:
      l1 = m_CharData.GetCurrentLevel(FighterFlag);
      ep1 = m_CharData.GetCurrExp(FighterFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[Fighter];
      m_L1Class = classData.ClassName(Fighter);

      l2 = m_CharData.GetCurrentLevel(ThiefFlag);
      ep2 = m_CharData.GetCurrExp(ThiefFlag);

      //2Dec2009 PRS      
      //m_L2Class = ClassText[Thief];   
      m_L2Class = classData.ClassName(Thief);   

      break;
   case F_MU_T:
      l1 = m_CharData.GetCurrentLevel(FighterFlag);
      ep1 = m_CharData.GetCurrExp(FighterFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[Fighter];
      m_L1Class = classData.ClassName(Fighter);

      l2 = m_CharData.GetCurrentLevel(MagicUserFlag);
      ep2 = m_CharData.GetCurrExp(MagicUserFlag);

      //2Dec2009 PRS      
      //m_L2Class = ClassText[MagicUser];   
      m_L2Class = classData.ClassName(MagicUser);   

      l3 = m_CharData.GetCurrentLevel(ThiefFlag);
      ep3 = m_CharData.GetCurrExp(ThiefFlag);

      //2Dec2009 PRS      
      //m_L3Class = ClassText[Thief];   
      m_L3Class = classData.ClassName(Thief);   

      break;
   case MU_T:
      l1 = m_CharData.GetCurrentLevel(MagicUserFlag);
      ep1 = m_CharData.GetCurrExp(MagicUserFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[MagicUser];   
      m_L1Class = classData.ClassName(MagicUser);   

      l2 = m_CharData.GetCurrentLevel(ThiefFlag);
      ep2 = m_CharData.GetCurrExp(ThiefFlag);

      //2Dec2009 PRS      
      //m_L2Class = ClassText[Thief];   
      m_L2Class = classData.ClassName(Thief);   

      break;
   case F_D:
      l1 = m_CharData.GetCurrentLevel(FighterFlag);
      ep1 = m_CharData.GetCurrExp(FighterFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[Fighter];
      m_L1Class = classData.ClassName(Fighter);

      l2 = m_CharData.GetCurrentLevel(DruidFlag);
      ep2 = m_CharData.GetCurrExp(DruidFlag);

      //2Dec2009 PRS      
      //m_L2Class = ClassText[Druid];
      m_L2Class = classData.ClassName(Druid);

      break;
   case D_MU:
      l1 = m_CharData.GetCurrentLevel(DruidFlag);
      ep1 = m_CharData.GetCurrExp(DruidFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[Druid];
      m_L1Class = classData.ClassName(Druid);

      l2 = m_CharData.GetCurrentLevel(MagicUserFlag);
      ep2 = m_CharData.GetCurrExp(MagicUserFlag);

      //2Dec2009 PRS      
      //m_L2Class = ClassText[MagicUser];   
      m_L2Class = classData.ClassName(MagicUser);   

      break;
   case F_MU_D:
      l1 = m_CharData.GetCurrentLevel(FighterFlag);
      ep1 = m_CharData.GetCurrExp(FighterFlag);

      //2Dec2009 PRS      
      //m_L1Class = ClassText[Fighter];
      m_L1Class = classData.ClassName(Fighter);

      l2 = m_CharData.GetCurrentLevel(MagicUserFlag);
      ep2 = m_CharData.GetCurrExp(MagicUserFlag);

      //2Dec2009 PRS      
      //m_L2Class = ClassText[MagicUser];   
      m_L2Class = classData.ClassName(MagicUser);   

      l3 = m_CharData.GetCurrentLevel(DruidFlag);
      ep3 = m_CharData.GetCurrExp(DruidFlag);

      //2Dec2009 PRS      
      //m_L3Class = ClassText[Druid];
      m_L3Class = classData.ClassName(Druid);

      break;
   }
*/
   {
     int idx, j, n;
     const CLASS_DATA *pClass;
     pClass = classData.PeekClass(classID);
     n = 0;
     if (pClass != NULL)
     {
      n = pClass->GetCount();
     };
     j = 0;
     for (idx=0; (idx<n) && (j<3); idx++)
     {
       const BASECLASS_ID *pBaseclassID;
       const BASE_CLASS_DATA *pBaseclass;
       pBaseclassID = pClass->PeekBaseclassID(idx);
       pBaseclass = baseclassData.PeekBaseclass(*pBaseclassID);
       if (pBaseclass == NULL) continue;
       m_Class[j] = *pBaseclassID;
       level[j] = m_CharData.GetCurrentLevel(*pBaseclassID);
       exp[j]   = m_CharData.GetCurrExp(*pBaseclassID);
       j++;
     };   };






   //m_Level1 = max(l1,1);
   //m_Level2 = max(l2,1);
   //m_Level3 = max(l3,1);
   m_Level[0] = max(level[0],1);
   m_Level[1] = max(level[1],1);
   m_Level[2] = max(level[2],1);

   //m_ExpPoints1 = ep1;
   //m_ExpPoints2 = ep2;
   //m_ExpPoints3 = ep3;
   m_ExpPoints[0] = exp[0];
   m_ExpPoints[1] = exp[1];
   m_ExpPoints[2] = exp[2];

   CWnd *pWnd;
   BOOL enable;

   //if (m_L1Class.IsEmpty())
   if (m_Class[0].IsEmpty())
     enable = FALSE;
   else
     enable = TRUE;

   pWnd = GetDlgItem(IDC_LEVEL1);
   if (pWnd != NULL)
      pWnd->EnableWindow(enable);
   pWnd = GetDlgItem(IDC_EXPPTS1);
   if (pWnd != NULL)
      pWnd->EnableWindow(enable);

   //if (m_L2Class.IsEmpty())
   if (m_Class[1].IsEmpty())
     enable = FALSE;
   else
     enable = TRUE;

   pWnd = GetDlgItem(IDC_LEVEL2);
   if (pWnd != NULL)
     pWnd->EnableWindow(enable);
   pWnd = GetDlgItem(IDC_EXPPTS2);
   if (pWnd != NULL)
      pWnd->EnableWindow(enable);

   //if (m_L3Class.IsEmpty())
   if (m_Class[2].IsEmpty())
     enable = FALSE;
   else
     enable = TRUE;

   pWnd = GetDlgItem(IDC_LEVEL3);
   if (pWnd != NULL)
      pWnd->EnableWindow(enable);
   pWnd = GetDlgItem(IDC_EXPPTS3);
   if (pWnd != NULL)
      pWnd->EnableWindow(enable);

   //if (m_L1Class.IsEmpty())
   //  m_L1Class = "N/A";

   //if (m_L2Class.IsEmpty())
   //  m_L2Class = "N/A";

   //if (m_L3Class.IsEmpty())
   //  m_L3Class = "N/A";
   if (m_Class[0].IsEmpty())
     m_Class[0] = "N/A";

   if (m_Class[1].IsEmpty())
     m_Class[1] = "N/A";

   if (m_Class[2].IsEmpty())
     m_Class[2] = "N/A";
}


void CCharacter::SetLevels()
{
  int i;
   //int l1 = m_Level1;
   //int l2 = m_Level2;
   //int l3 = m_Level3;
   //int ep1 = m_ExpPoints1;
   //int ep2 = m_ExpPoints2;
   //int ep3 = m_ExpPoints3;
   int level[3];
   int exp[3];
   for (i=0; i<3; i++)
   {
     level[i] = m_Level[i];
     exp[i] = m_ExpPoints[i];
   };
   

   m_CharData.ClearLevels();


   
/*   
   switch (m_CharData.GetClass())
   {
   case Fighter:
      m_CharData.SetCurrentLevel(FighterFlag, l1);
      m_CharData.SetCurrExp(FighterFlag, ep1);
      break;
   case Cleric:
      m_CharData.SetCurrentLevel(ClericFlag, l1);
      m_CharData.SetCurrExp(ClericFlag, ep1);
      break;
   case Ranger:
      m_CharData.SetCurrentLevel(RangerFlag, l1);
      m_CharData.SetCurrExp(RangerFlag, ep1);
      break;
   case Paladin:
      m_CharData.SetCurrentLevel(PaladinFlag, l1);
      m_CharData.SetCurrExp(PaladinFlag, ep1);
      break;
   case MagicUser:
      m_CharData.SetCurrentLevel(MagicUserFlag, l1);
      m_CharData.SetCurrExp(MagicUserFlag, ep1);
      break;
   case Thief:
      m_CharData.SetCurrentLevel(ThiefFlag, l1);
      m_CharData.SetCurrExp(ThiefFlag, ep1);
      break;
   case Druid:
      m_CharData.SetCurrentLevel(DruidFlag, l1);
      m_CharData.SetCurrExp(DruidFlag, ep1);
      break;
   case C_F:
      m_CharData.SetCurrentLevel(ClericFlag, l1);
      m_CharData.SetCurrExp(ClericFlag, ep1);
      m_CharData.SetCurrentLevel(FighterFlag, l2);
      m_CharData.SetCurrExp(FighterFlag, ep2);
      break;
   case C_F_MU:
      m_CharData.SetCurrentLevel(ClericFlag, l1);
      m_CharData.SetCurrExp(ClericFlag, ep1);
      m_CharData.SetCurrentLevel(FighterFlag, l2);
      m_CharData.SetCurrExp(FighterFlag, ep2);
      m_CharData.SetCurrentLevel(MagicUserFlag, l3);
      m_CharData.SetCurrExp(MagicUserFlag, ep3);
      break;
   case C_R:
      m_CharData.SetCurrentLevel(ClericFlag, l1);
      m_CharData.SetCurrExp(ClericFlag, ep1);
      m_CharData.SetCurrentLevel(RangerFlag, l2);
      m_CharData.SetCurrExp(RangerFlag, ep2);
      break;
   case C_MU:
      m_CharData.SetCurrentLevel(ClericFlag, l1);
      m_CharData.SetCurrExp(ClericFlag, ep1);
      m_CharData.SetCurrentLevel(MagicUserFlag, l2);
      m_CharData.SetCurrExp(MagicUserFlag, ep2);
      break;
   case C_T:
      m_CharData.SetCurrentLevel(ClericFlag, l1);
      m_CharData.SetCurrExp(ClericFlag, ep1);
      m_CharData.SetCurrentLevel(ThiefFlag, l2);
      m_CharData.SetCurrExp(ThiefFlag, ep2);
      break;
   case F_MU:
      m_CharData.SetCurrentLevel(FighterFlag, l1);
      m_CharData.SetCurrExp(FighterFlag, ep1);
      m_CharData.SetCurrentLevel(MagicUserFlag, l2);
      m_CharData.SetCurrExp(MagicUserFlag, ep2);
      break;
   case F_T:
      m_CharData.SetCurrentLevel(FighterFlag, l1);
      m_CharData.SetCurrExp(FighterFlag, ep1);
      m_CharData.SetCurrentLevel(ThiefFlag, l2);
      m_CharData.SetCurrExp(ThiefFlag, ep2);
      break;
   case F_MU_T:
      m_CharData.SetCurrentLevel(FighterFlag, l1);
      m_CharData.SetCurrExp(FighterFlag, ep1);
      m_CharData.SetCurrentLevel(MagicUserFlag, l2);
      m_CharData.SetCurrExp(MagicUserFlag, ep2);
      m_CharData.SetCurrentLevel(ThiefFlag, l3);
      m_CharData.SetCurrExp(ThiefFlag, ep3);
      break;
   case MU_T:
      m_CharData.SetCurrentLevel(MagicUserFlag, l1);
      m_CharData.SetCurrExp(MagicUserFlag, ep1);
      m_CharData.SetCurrentLevel(ThiefFlag, l2);
      m_CharData.SetCurrExp(ThiefFlag, ep2);
      break;
   case F_D:
      m_CharData.SetCurrentLevel(FighterFlag, l1);
      m_CharData.SetCurrExp(FighterFlag, ep1);
      m_CharData.SetCurrentLevel(DruidFlag, l2);
      m_CharData.SetCurrExp(DruidFlag, ep2);
      break;
   case D_MU:
      m_CharData.SetCurrentLevel(DruidFlag, l1);
      m_CharData.SetCurrExp(DruidFlag, ep1);
      m_CharData.SetCurrentLevel(MagicUserFlag, l2);
      m_CharData.SetCurrExp(MagicUserFlag, ep2);
      break;
   case F_MU_D:
      m_CharData.SetCurrentLevel(FighterFlag, l1);
      m_CharData.SetCurrExp(FighterFlag, ep1);
      m_CharData.SetCurrentLevel(MagicUserFlag, l2);
      m_CharData.SetCurrExp(MagicUserFlag, ep2);
      m_CharData.SetCurrentLevel(DruidFlag, l3);
      m_CharData.SetCurrExp(DruidFlag, ep3);
      break;
   }
*/
  {
    int idx, j, n;
    CLASS_ID classID;
    const CLASS_DATA *pClass;
    classID = m_CharData.GetClass();
    pClass = classData.PeekClass(classID);
    if (pClass != NULL)
    {
      m_CharData.ClearExperience(); 
      n = pClass->GetCount();
      j = 0;
      for (idx =0; (idx<n) && (j<3); idx++)
      {
        const BASECLASS_ID *pBaseclassID;
        const BASE_CLASS_DATA *pBaseclass;
        pBaseclassID = pClass->PeekBaseclassID(idx);
        pBaseclass = baseclassData.PeekBaseclass(*pBaseclassID);
        if (pBaseclass != NULL)
        {
          int newLevel;
          newLevel = pBaseclass->GetMaxLevel();
          if (level[j] < newLevel) newLevel = level[j]; 
          m_CharData.SetCurrentLevel(*pBaseclassID, newLevel);
          // set experience based on levels
          // Only sets the experience level if the designer hasn't specified the desired exp
          // or if the set exp exceeds the maximum allowable for that level.
          {
            //int minexp = GetMinFighterExpForLevel(m_CharData.GetCurrentLevel(FighterFlag));
            int minexp, maxexp, currexp;
            minexp = pBaseclass->GetMinExpForLevel(newLevel);
            //int maxexp = GetMaxFighterExpForLevel(m_CharData.GetCurrentLevel(FighterFlag));
            maxexp = pBaseclass->GetMaxExpForLevel(newLevel);
            //int currexp = m_CharData.GetCurrExp(FighterFlag);
            currexp = m_CharData.GetCurrExp(*pBaseclassID);
            if ((currexp < minexp) && (currexp == 0)) currexp = minexp;
            if (currexp > maxexp) currexp = maxexp-1;
            //m_CharData.SetCurrExp(FighterFlag, currexp);
            m_CharData.SetCurrExp(*pBaseclassID, currexp);
          };
          j++;
        };
      };
    };
  };


/*  Taken care of just above this line.
   // set experience based on levels
   // Only sets the experience level if the designer hasn't specified the desired exp
   // or if the set exp exceeds the maximum allowable for that level.
   classType ctype = m_CharData.GetClass();
   m_CharData.ClearExperience(); 

   if (isFighter(ctype))
   {      
      int minexp = GetMinFighterExpForLevel(m_CharData.GetCurrentLevel(FighterFlag));
      int maxexp = GetMaxFighterExpForLevel(m_CharData.GetCurrentLevel(FighterFlag));
      int currexp = m_CharData.GetCurrExp(FighterFlag);
      if ((currexp < minexp) && (currexp == 0)) currexp = minexp;
      if (currexp > maxexp) currexp = maxexp-1;
      m_CharData.SetCurrExp(FighterFlag, currexp);
   }

   if (isPaladin(ctype))
   {
      int minexp = GetMinPaladinExpForLevel(m_CharData.GetCurrentLevel(PaladinFlag));
      int maxexp = GetMaxPaladinExpForLevel(m_CharData.GetCurrentLevel(PaladinFlag));
      int currexp = m_CharData.GetCurrExp(PaladinFlag);
      if ((currexp < minexp) && (currexp == 0)) currexp = minexp;
      if (currexp > maxexp) currexp = maxexp-1;
      m_CharData.SetCurrExp(PaladinFlag,  currexp );
   }

   if (isRanger(ctype))
   {
      int minexp = GetMinRangerExpForLevel(m_CharData.GetCurrentLevel(RangerFlag));
      int maxexp = GetMaxRangerExpForLevel(m_CharData.GetCurrentLevel(RangerFlag));
      int currexp = m_CharData.GetCurrExp(RangerFlag);
      if ((currexp < minexp) && (currexp == 0)) currexp = minexp;
      if (currexp > maxexp) currexp = maxexp-1;
      m_CharData.SetCurrExp(RangerFlag,  currexp );
   }

   if (isCleric(ctype))
   {
      int minexp = GetMinClericExpForLevel(m_CharData.GetCurrentLevel(ClericFlag));
      int maxexp = GetMaxClericExpForLevel(m_CharData.GetCurrentLevel(ClericFlag));
      int currexp = m_CharData.GetCurrExp(ClericFlag);
      if ((currexp < minexp) && (currexp == 0)) currexp = minexp;
      if (currexp > maxexp) currexp = maxexp-1;
      m_CharData.SetCurrExp(ClericFlag,  currexp );
   }

   if (isDruid(ctype))
   {
      int minexp = GetMinDruidExpForLevel(m_CharData.GetCurrentLevel(DruidFlag));
      int maxexp = GetMaxDruidExpForLevel(m_CharData.GetCurrentLevel(DruidFlag));
      int currexp = m_CharData.GetCurrExp(DruidFlag);
      if ((currexp < minexp) && (currexp == 0)) currexp = minexp;
      if (currexp > maxexp) currexp = maxexp-1;
      m_CharData.SetCurrExp(DruidFlag,  currexp);
   }

   if (isMagicUser(ctype))
   {
      int minexp = GetMinMagicUserExpForLevel(m_CharData.GetCurrentLevel(MagicUserFlag));
      int maxexp = GetMaxMagicUserExpForLevel(m_CharData.GetCurrentLevel(MagicUserFlag));
      int currexp = m_CharData.GetCurrExp(MagicUserFlag);
      if ((currexp < minexp) && (currexp == 0)) currexp = minexp;
      if (currexp > maxexp) currexp = maxexp-1;
      m_CharData.SetCurrExp(MagicUserFlag,  currexp );
   }

   if (isThief(ctype))
   {
      int minexp = GetMinThiefExpForLevel(m_CharData.GetCurrentLevel(ThiefFlag));
      int maxexp = GetMaxThiefExpForLevel(m_CharData.GetCurrentLevel(ThiefFlag));
      int currexp = m_CharData.GetCurrExp(ThiefFlag);
      if ((currexp < minexp) && (currexp == 0)) currexp = minexp;
      if (currexp > maxexp) currexp = maxexp-1;
      m_CharData.SetCurrExp(ThiefFlag,  currexp );
   }
*/
}


void CCharacter::OnMoney() 
{
  UpdateData(TRUE);
  int cash = m_CharData.money.ConvertToDefault(m_CharData.money.Total(), m_CharData.money.GetBaseType());

  CGetMoney dlg(m_CharData.money.NumGems(), 
                m_CharData.money.NumJewelry(),
                cash);

  if (dlg.DoModal() == IDOK)
  {
    m_CharData.money.Clear();
    for (int g=0;g<dlg.m_Gems;g++)
      m_CharData.money.AddGem();
    for (int j=0;j<dlg.m_Jewelry;j++)
      m_CharData.money.AddJewelry();
    m_CharData.money.Add(m_CharData.money.GetDefaultType(), dlg.m_Platinum);
  }
}

void CCharacter::OnRollstats() 
{
  UpdateData(TRUE);
	m_CharData.generateNewCharacter(GetDesignCharStartExp(),GetDesignCharStartExpType());
  UpdateData(FALSE);	
}

void CCharacter::OnOK() 
{
  if (UpdateData(TRUE))
  {
    m_CharData.UpdateStats(FALSE);
	  CDialog::OnOK();
  }
}

void CCharacter::OnTalkevent() 
{
  UpdateData(TRUE);
  CEventViewer dlg(m_CharData.GetNPCEventText(), m_CharData.TalkEvent);
  if (dlg.DoModal() == IDOK)
  {
    m_CharData.TalkEvent = dlg.m_SingleEventRoot;

    if (!globalData.eventData.IsValidEvent(m_CharData.TalkEvent))
      m_CharData.TalkEvent=NO_EVENT;

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetEventIdDescription(m_CharData.TalkEvent, GlobalEventSrc), name, MAX_MEDITBUTTON_TEXT);
    m_TalkEvent.SetWindowText(name);
  }	
}

void CCharacter::OnExamineevent() 
{
  UpdateData(TRUE);
  CEventViewer dlg(m_CharData.GetNPCEventText(), m_CharData.ExamineEvent);
  if (dlg.DoModal() == IDOK)
  {
    m_CharData.ExamineEvent = dlg.m_SingleEventRoot;

    if (!globalData.eventData.IsValidEvent(m_CharData.ExamineEvent))
      m_CharData.ExamineEvent=NO_EVENT;

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetEventIdDescription(m_CharData.ExamineEvent, GlobalEventSrc), name, MAX_MEDITBUTTON_TEXT);
    m_ExamineEvent.SetWindowText(name);
  }	
}


void CCharacter::OnSpecAbs() 
{
  CString title;
  title.Format("Special Abilities for NPC %s", m_CharData.name);
  CEditAttributes dlg(m_CharData.specAbs, title);
  if (dlg.DoModal() == IDOK)
  {;
    m_CharData.specAbs.Copy(dlg.m_data);
  };
//   CChooseSpeclAbDlg	dlg(m_ItemData.specAbs, title);
//   if (dlg.DoModal() == IDOK)
//     dlg.GetData(m_ItemData.specAbs);
}

void CCharacter::OnSpecAbsHelp(void)
{
  char text[3001];
  LoadString(GetModuleHandle(NULL),IDS_HELP_NPC_Specabs,text,3000);
  MessageBox(text, "NPC Special Ability", MB_OK);
}


void CCharacter::OnKillfocusExppts1() 
{
  UpdateData(TRUE);
  UpdateData(FALSE);
}

void CCharacter::OnKillfocusExppts2() 
{
  UpdateData(TRUE);
  UpdateData(FALSE);
}

void CCharacter::OnKillfocusExppts3() 
{
  UpdateData(TRUE);
  UpdateData(FALSE);
}

void CCharacter::OnKillfocusLevel1() 
{
	UpdateData(TRUE);
  UpdateData(FALSE);
}

void CCharacter::OnKillfocusLevel2() 
{
	UpdateData(TRUE);
  UpdateData(FALSE);
}

void CCharacter::OnKillfocusLevel3() 
{
	UpdateData(TRUE);
  UpdateData(FALSE);
}
