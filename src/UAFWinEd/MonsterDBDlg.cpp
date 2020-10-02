/******************************************************************************
* Filename: MonsterDBDlg.cpp
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
//#include "..\Shared\Version.h"
#include "resource.h"

#include "UAFWinEd.h"
#include "IconPicDlg.h"
#include "itemdlg.h"
#include "ChooseSpeclAbDlg.h"
#include "SoundChooser.h"
#include "MonsterAttacksDlg.h"
#include "GetMoney.h"
#include "MonsterDBDlg.h"
#include "EditAttributes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonsterDBDlg dialog


CMonsterDBDlg::CMonsterDBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMonsterDBDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMonsterDBDlg)
	m_ArmorClass = 10;
	m_HitDiceBonus = 0;
	m_Intelligence = 10;
	m_MagicResistance = 0;
	m_Morale = 50;
	m_Movement = 10;
	m_Name = _T("");
	m_THAC0 = 10;
	m_XPValue = 50;
	m_HitDice = 0.0f;
	m_FAnimal = FALSE;
	m_FGiant = FALSE;
	m_FLarge = FALSE;
	m_FMammal = FALSE;
	m_FSnake = FALSE;
	m_IConfusion = FALSE;
	m_IDeath = FALSE;
	m_IPoison = FALSE;
	m_IVorpal = FALSE;
	m_OAffByDispelEvil = FALSE;
	m_OCanBeHeldCharm = FALSE;
	m_PDwAC = FALSE;
	m_PDwTHAC0 = FALSE;
	m_PGnAC = FALSE;
	m_PGnTHAC0 = FALSE;
	m_PRangDmg = FALSE;
	m_AttackCount = _T("No Attacks");
	m_UseHitDice = 0;
	//}}AFX_DATA_INIT
}

void CMonsterDBDlg::DoDataExchange(CDataExchange* pDX)
{
  if (!pDX->m_bSaveAndValidate)
  {
    m_Name = m_MonsterData.Name;
    if (m_Name.GetLength() > MAX_MONSTER_NAME) m_Name.SetAt(MAX_MONSTER_NAME,'\0');
    m_Intelligence = m_MonsterData.Intelligence;
    m_ArmorClass = m_MonsterData.Armor_Class;
    m_Movement = m_MonsterData.Movement;
    m_HitDice = m_MonsterData.Hit_Dice;
    m_UseHitDice = (m_MonsterData.UseHitDice?0:1);
    if (!m_MonsterData.UseHitDice) m_MonsterData.Hit_Dice_Bonus = 0;
    m_HitDiceBonus = m_MonsterData.Hit_Dice_Bonus;
    m_THAC0 = m_MonsterData.THAC0;
    //m_NbrAttacks = m_MonsterData.Nbr_Attacks;
    //m_AttackDice = m_MonsterData.Dmg_Dice_for_Attack;
    //m_AttackBonus = m_MonsterData.Dmg_Dice_Bonus;
    //m_AttackNbrDice = m_MonsterData.Nbr_Dmg_Dice;
    m_MagicResistance = m_MonsterData.Magic_Resistance;
    if (m_Size.m_hWnd != NULL)
      m_Size.SetCurSel((int)m_MonsterData.Size);
    if (m_Class.m_hWnd != NULL)
    {
      int i, n;
      CLASS_ID classID;
      m_Class.SetCurSel(-1);
      n = m_Class.GetCount();
      for (i=0; i<n; i++)
      {
        int j;
        j = m_Class.GetItemData(i);
        if (m_MonsterData.classID == m_availClasses[j])
        {
          m_Class.SetCurSel(i);
        };
      };
     };
    if (m_Race.m_hWnd != NULL)
    {
      int i, n;
      RACE_ID raceID;
      m_Race.SetCurSel(-1);
      n = m_Race.GetCount();
      for (i=0; i<n; i++)
      {
        int j;
        j = m_Race.GetItemData(i);
        if (m_MonsterData.raceID == m_availRaces[j])
        {
          m_Race.SetCurSel(i);
        };
      };
    };
   //if (m_UndeadType.m_hWnd != NULL)
    //  m_UndeadType.SetCurSel((int)m_MonsterData.undeadStatus);
    m_Morale = m_MonsterData.Morale;
    m_XPValue = m_MonsterData.XP_Value;

    m_FAnimal = ((m_MonsterData.Form_Type & FormAnimal) > 0);
    m_FMammal = ((m_MonsterData.Form_Type & FormMammal) > 0);
    m_FSnake = ((m_MonsterData.Form_Type & FormSnake) > 0);
    m_FGiant = ((m_MonsterData.Form_Type & FormGiant) > 0);
    m_FLarge = ((m_MonsterData.Form_Type & FormLarge) > 0);

    m_PDwAC = ((m_MonsterData.Penalty_Type & PenaltyDwarfAC) > 0);
    m_PGnAC = ((m_MonsterData.Penalty_Type & PenaltyGnomeAC) > 0);
    m_PDwTHAC0 = ((m_MonsterData.Penalty_Type & PenaltyDwarfTHAC0) > 0);
    m_PGnTHAC0 = ((m_MonsterData.Penalty_Type & PenaltyGnomeTHAC0) > 0);
    m_PRangDmg = ((m_MonsterData.Penalty_Type & PenaltyRangerDmg) > 0);

    m_IConfusion = ((m_MonsterData.Immunity_Type & ImmuneConfusion) > 0);
    m_IPoison = ((m_MonsterData.Immunity_Type & ImmunePoison) > 0);
    m_IDeath = ((m_MonsterData.Immunity_Type & ImmuneDeath) > 0);
    m_IVorpal = ((m_MonsterData.Immunity_Type & ImmuneVorpal) > 0);

    m_OAffByDispelEvil = ((m_MonsterData.MiscOptions_Type & OptionAffectedByDispelEvil) > 0);
    m_OCanBeHeldCharm = ((m_MonsterData.MiscOptions_Type & OptionCanBeHeldCharmed) > 0);
  }

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonsterDBDlg)
	DDX_Control(pDX, IDC_MONSTERPIC, m_MonsterPic);
	//DDX_Control(pDX, IDC_UNDEADTYPE, m_UndeadType);
	DDX_Text(pDX, IDC_UndeadType, m_undeadType);
	DDX_Control(pDX, IDC_MOVESOUND, m_MoveSound);
	DDX_Control(pDX, IDC_DEATHSOUND, m_DeathSound);
	DDX_Control(pDX, IDC_MISSSOUND, m_MissSound);
	DDX_Control(pDX, IDC_HITSOUND, m_HitSound);
	DDX_Control(pDX, IDC_CHOOSEICON, m_Icon);
	DDX_Control(pDX, IDC_MONSTERSIZE, m_Size);
  DDX_Control(pDX, IDC_MONSTERCLASS, m_Class);
  DDX_Control(pDX, IDC_RACE, m_Race);
	DDX_Text(pDX, IDC_ARMORCLASS, m_ArmorClass);
	DDX_Text(pDX, IDC_HITDICEBONUS, m_HitDiceBonus);
	DDX_Text(pDX, IDC_INTELLIGENCE, m_Intelligence);
	DDX_Text(pDX, IDC_MAGICRESISTANCE, m_MagicResistance);
	DDX_Text(pDX, IDC_MORALE, m_Morale);
	DDX_Text(pDX, IDC_MOVEMENT, m_Movement);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_THAC0, m_THAC0);
	DDX_Text(pDX, IDC_XPVALUE, m_XPValue);
	DDX_Text(pDX, IDC_HITDICE, m_HitDice);
	DDX_Check(pDX, IDC_MF_ANIMAL, m_FAnimal);
	DDX_Check(pDX, IDC_MF_GIANT, m_FGiant);
	DDX_Check(pDX, IDC_MF_LARGE, m_FLarge);
	DDX_Check(pDX, IDC_MF_MAMMAL, m_FMammal);
	DDX_Check(pDX, IDC_MF_SNAKE, m_FSnake);
	DDX_Check(pDX, IDC_MI_CONFUSION, m_IConfusion);
	DDX_Check(pDX, IDC_MI_DEATH, m_IDeath);
	DDX_Check(pDX, IDC_MI_POISON, m_IPoison);
	DDX_Check(pDX, IDC_MI_VORPAL, m_IVorpal);
	DDX_Check(pDX, IDC_MO_AFFBYDISPEVIL, m_OAffByDispelEvil);
	DDX_Check(pDX, IDC_MO_HELDCHARM, m_OCanBeHeldCharm);
	DDX_Check(pDX, IDC_MP_DWAC, m_PDwAC);
	DDX_Check(pDX, IDC_MP_DWTHAC0, m_PDwTHAC0);
	DDX_Check(pDX, IDC_MP_GNAC, m_PGnAC);
	DDX_Check(pDX, IDC_MP_GNTHAC0, m_PGnTHAC0);
	DDX_Check(pDX, IDC_MP_RANGDMG, m_PRangDmg);
	DDX_Text(pDX, IDC_ATTACKCOUNT, m_AttackCount);
	DDX_Radio(pDX, IDC_USEHITDICE, m_UseHitDice);
	//}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
  {
    if (m_Name.GetLength() > MAX_MONSTER_NAME) m_Name.SetAt(MAX_MONSTER_NAME,'\0');
    m_MonsterData.Name = m_Name;
    m_MonsterData.Intelligence = m_Intelligence;
    m_MonsterData.Armor_Class = m_ArmorClass;
    m_MonsterData.Movement = m_Movement;
    m_MonsterData.Hit_Dice = m_HitDice;
    m_MonsterData.UseHitDice = ((m_UseHitDice==0)?TRUE:FALSE);
    if (!m_MonsterData.UseHitDice) m_HitDiceBonus = 0;
    m_MonsterData.Hit_Dice_Bonus = m_HitDiceBonus;
    m_MonsterData.THAC0 = m_THAC0;
    //m_MonsterData.Nbr_Attacks = m_NbrAttacks;
    //m_MonsterData.Dmg_Dice_for_Attack = m_AttackDice;
    //m_MonsterData.Dmg_Dice_Bonus = m_AttackBonus;
    //m_MonsterData.Nbr_Dmg_Dice = m_AttackNbrDice;
    m_MonsterData.Magic_Resistance = m_MagicResistance;
    m_MonsterData.Size = (creatureSizeType)m_Size.GetCurSel();
    {
      int classIndex;
      classIndex = m_Class.GetCurSel();
      if (classIndex >= 0)
      {
        m_MonsterData.classID = m_availClasses[m_Class.GetItemData(classIndex)];
      }
      else
      {
        m_MonsterData.classID.Empty();
      };
    };
    {
      int raceIndex;
      raceIndex = m_Race.GetCurSel();
      if (raceIndex >= 0)
      {
        m_MonsterData.raceID = m_availRaces[m_Race.GetItemData(raceIndex)];
      }
      else
      {
        m_MonsterData.raceID.Empty();
      };
    };
    //m_MonsterData.undeadStatus = (undeadType)m_UndeadType.GetCurSel();
    m_MonsterData.undeadType = m_undeadType;
    m_MonsterData.Morale = m_Morale;
    m_MonsterData.XP_Value = m_XPValue;
    m_MonsterData.Form_Type = 0;
    m_MonsterData.Penalty_Type = 0;
    m_MonsterData.Immunity_Type = 0;
    m_MonsterData.MiscOptions_Type = 0;

    if (m_FAnimal)
      m_MonsterData.Form_Type |= FormAnimal;
    if (m_FMammal)
      m_MonsterData.Form_Type |= FormMammal;
    if (m_FSnake)
      m_MonsterData.Form_Type |= FormSnake;
    if (m_FGiant)
      m_MonsterData.Form_Type |= FormGiant;
    if (m_FLarge)
      m_MonsterData.Form_Type |= FormLarge;

    if (m_PDwAC)
      m_MonsterData.Penalty_Type |= PenaltyDwarfAC;
    if (m_PGnAC)
      m_MonsterData.Penalty_Type |= PenaltyGnomeAC;
    if (m_PDwTHAC0)
      m_MonsterData.Penalty_Type |= PenaltyDwarfTHAC0;
    if (m_PGnTHAC0)
      m_MonsterData.Penalty_Type |= PenaltyGnomeTHAC0;
    if (m_PRangDmg)
      m_MonsterData.Penalty_Type |= PenaltyRangerDmg;

    if (m_IConfusion)
      m_MonsterData.Immunity_Type |= ImmuneConfusion;
    if (m_IPoison)
      m_MonsterData.Immunity_Type |= ImmunePoison;
    if (m_IDeath)
      m_MonsterData.Immunity_Type |= ImmuneDeath;
    if (m_IVorpal)
      m_MonsterData.Immunity_Type |= ImmuneVorpal;

    if (m_OAffByDispelEvil)
      m_MonsterData.MiscOptions_Type |= OptionAffectedByDispelEvil;
    if (m_OCanBeHeldCharm)
      m_MonsterData.MiscOptions_Type |= OptionCanBeHeldCharmed;
  }
}


BEGIN_MESSAGE_MAP(CMonsterDBDlg, CDialog)
	//{{AFX_MSG_MAP(CMonsterDBDlg)
	ON_BN_CLICKED(IDC_CHOOSE_SP_ATTACKS, OnChooseSpAttacks)
	ON_BN_CLICKED(IDC_CHOOSEICON, OnChooseicon)
	ON_BN_CLICKED(IDC_MISSSOUND, OnMisssound)
	ON_BN_CLICKED(IDC_HITSOUND, OnHitsound)
	ON_BN_CLICKED(IDC_MOVESOUND, OnMovesound)
	ON_BN_CLICKED(IDC_DEATHSOUND, OnDeathsound)
	ON_BN_CLICKED(IDC_ITEMS, OnItems)
	ON_BN_CLICKED(IDC_ATTACKDATA, OnAttackdata)
	ON_BN_CLICKED(IDC_DEFAULTMONEY, OnDefaultmoney)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonsterDBDlg message handlers

//*****************************************************************************
//    NAME: CMonsterDBDlg::OnInitDialog
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CMonsterDBDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_MonsterPic.CreateBlankPicture();
  m_Class.ResetContent();
  m_Race.ResetContent();
  m_Size.ResetContent();
  //m_UndeadType.ResetContent();

  m_Size.AddString("S");
  m_Size.AddString("M");
  m_Size.AddString("L");
  m_Size.SetCurSel(1);

  //for (int i=0;i<NUM_UNDEAD_TYPES;i++)
  //  m_UndeadType.AddString(UndeadTypeText[i]);
  //m_UndeadType.SetCurSel((int)m_MonsterData.undeadStatus);
  m_undeadType = m_MonsterData.undeadType;

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_MonsterData.MissSound, name, MAX_MEDITBUTTON_TEXT);
  m_MissSound.SetWindowText(name);

  getBaseName(m_MonsterData.HitSound, name, MAX_MEDITBUTTON_TEXT);
  m_HitSound.SetWindowText(name);

  getBaseName(m_MonsterData.MoveSound, name, MAX_MEDITBUTTON_TEXT);
  m_MoveSound.SetWindowText(name);

  getBaseName(m_MonsterData.DeathSound, name, MAX_MEDITBUTTON_TEXT);
  m_DeathSound.SetWindowText(name);

  getBaseName(m_MonsterData.Icon.filename, name, MAX_MEDITBUTTON_TEXT);
  m_Icon.SetWindowText(name);

  m_AttackCount.Format("%i Attacks",m_MonsterData.attackData.GetMonsterAttackDetailsCount());

  if (m_MonsterData.Icon.filename != "")
    m_MonsterPic.LoadPicture(m_MonsterData.Icon.filename, IconDib, rte.IconArtDir(), ede.TemplateBackgroundArtDir());


  {
    int i, n;
    BOOL matchMonster;
    matchMonster = false;
    n = classData.GetCount();
    m_availClasses.RemoveAll();
    for (i=0; i<n; i++)
    {
      int indx;
      CLASS_ID classID;
      classID = classData.PeekClass(i)->ClassID();
      m_availClasses.Add(classID);
      indx = m_Class.AddString(m_availClasses[i]);
      m_Class.SetItemData(indx, i);
      if (m_MonsterData.classID == m_availClasses[i]) 
      {
        matchMonster = TRUE;
      };
      if (!matchMonster)
      {
        if (m_MonsterData.classID != "")
        {
          m_availClasses.Add(m_MonsterData.classID);
        };
      };
    };
  };

  {
    int i, n;
    BOOL matchMonster;
    matchMonster = false;
    n = raceData.GetCount();
    m_availRaces.RemoveAll();
    for (i=0; i<n; i++)
    {
      int indx;
      RACE_ID raceID;
      raceID = raceData.PeekRace(i)->RaceID();
      if (raceID == "$$Help") continue;
      m_availRaces.Add(raceID);
      indx = m_Race.AddString(m_availRaces[i]);
      m_Race.SetItemData(indx, i);
      if (m_MonsterData.raceID == m_availRaces[i]) 
      {
        matchMonster = TRUE;
        m_Race.SetCurSel(i);
      };
      if (!matchMonster)
      {
        if (m_MonsterData.raceID != "")
        {
          m_availRaces.Add(m_MonsterData.raceID);
        };
      };
    };
  };



  UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//*****************************************************************************
//    NAME: CMonsterDBDlg::OnChooseSpAttacks
//
// PURPOSE: 
//
//*****************************************************************************
void CMonsterDBDlg::OnChooseSpAttacks() 
{
  CString title;
  title.Format("Special Abilities for monster type %s", m_MonsterData.Name);
  CEditAttributes dlg(m_MonsterData.specAbs, title);
  if (dlg.DoModal() == IDOK)
  {;
    m_MonsterData.specAbs.Copy(dlg.m_data);
  };
//  NotImplemented(0x759001f,FALSE);
//   CChooseSpeclAbDlg	dlg(m_MonsterData.specAbs, title);
//   //dlg.m_NoMsgTypes=TRUE; // cannot edit msg types
//   if (dlg.DoModal() == IDOK)
//     dlg.GetData(m_MonsterData.specAbs);
}

//*****************************************************************************
//    NAME: CMonsterDBDlg::OnChooseicon
//
// PURPOSE: 
//
//*****************************************************************************
void CMonsterDBDlg::OnChooseicon() 
{
  CIconPicDlg dlg(m_MonsterData.Icon);
  
  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    dlg.GetData(m_MonsterData.Icon);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_MonsterData.Icon.filename, name, MAX_MEDITBUTTON_TEXT);
    m_Icon.SetWindowText(name);

    if (m_MonsterData.Icon.filename != "")
      m_MonsterPic.LoadPicture(m_MonsterData.Icon.filename, IconDib, rte.IconArtDir(), ede.TemplateIconArtDir());
    else
      m_MonsterPic.ErasePicture();
  }
}

//*****************************************************************************
//    NAME: CMonsterDBDlg::OnMisssound
//
// PURPOSE: 
//
//*****************************************************************************
void CMonsterDBDlg::OnMisssound() 
{
  CSoundChooser dlg(m_MonsterData.MissSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_MonsterData.MissSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_MonsterData.MissSound, name, MAX_MEDITBUTTON_TEXT);
    m_MissSound.SetWindowText(name);
  }
}

//*****************************************************************************
//    NAME: CMonsterDBDlg::OnHitsound
//
// PURPOSE: 
//
//*****************************************************************************
void CMonsterDBDlg::OnHitsound() 
{
  CSoundChooser dlg(m_MonsterData.HitSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_MonsterData.HitSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_MonsterData.HitSound, name, MAX_MEDITBUTTON_TEXT);
    m_HitSound.SetWindowText(name);
  }
}

//*****************************************************************************
//    NAME: CMonsterDBDlg::OnMovesound
//
// PURPOSE: 
//
//*****************************************************************************
void CMonsterDBDlg::OnMovesound() 
{
  CSoundChooser dlg(m_MonsterData.MoveSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_MonsterData.MoveSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_MonsterData.MoveSound, name, MAX_MEDITBUTTON_TEXT);
    m_MoveSound.SetWindowText(name);
  }	
}

//*****************************************************************************
//    NAME: CMonsterDBDlg::OnDeathsound
//
// PURPOSE: 
//
//*****************************************************************************
void CMonsterDBDlg::OnDeathsound() 
{
  CSoundChooser dlg(m_MonsterData.DeathSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_MonsterData.DeathSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_MonsterData.DeathSound, name, MAX_MEDITBUTTON_TEXT);
    m_DeathSound.SetWindowText(name);
  }
}

//*****************************************************************************
//    NAME: CMonsterDBDlg::OnAttackdata
//
// PURPOSE: 
//
//*****************************************************************************
void CMonsterDBDlg::OnAttackdata() 
{
	CMonsterAttacksDlg dlg(m_MonsterData.attackData);
  if (dlg.DoModal()==IDOK)
  {
    UpdateData(TRUE);
    dlg.GetData(m_MonsterData.attackData);	
    m_AttackCount.Format("%i Attacks",m_MonsterData.attackData.GetMonsterAttackDetailsCount());
    UpdateData(FALSE);
  }
}

//*****************************************************************************
//    NAME: CMonsterDBDlg::OnOK
//
// PURPOSE: 
//
//*****************************************************************************
void CMonsterDBDlg::OnOK() 
{
  UpdateData(TRUE);
  if (m_MonsterData.attackData.GetMonsterAttackDetailsCount()==0)
  {
    if (MsgBoxYesNo("This monster has zero attacks defined, which will not work in combat. Continue Anyway?")==IDNO) return;
  }
	CDialog::OnOK();
}

//*****************************************************************************
//    NAME: CMonsterDBDlg::OnItems
//
// PURPOSE: 
//
//*****************************************************************************
void CMonsterDBDlg::OnItems() 
{
  CItemDlg dlg(m_MonsterData.myItems);
  dlg.m_AllItemsAreIdentified = FALSE;
  if (dlg.DoModal() == IDOK)
    dlg.GetItemList(m_MonsterData.myItems);
}

//*****************************************************************************
//    NAME: CMonsterDBDlg::OnDefaultmoney
//
// PURPOSE: 
//
//*****************************************************************************
void CMonsterDBDlg::OnDefaultmoney() 
{
  UpdateData(TRUE);
  int cash = m_MonsterData.money.ConvertToDefault(m_MonsterData.money.Total(), 
                                                  m_MonsterData.money.GetBaseType());
  
  CGetMoney dlg(m_MonsterData.money.NumGems(), 
                m_MonsterData.money.NumJewelry(),
                cash);
  
  if (dlg.DoModal() == IDOK)
  {
    m_MonsterData.money.Clear();
    for (int g=0;g<dlg.m_Gems;g++)
      m_MonsterData.money.AddGem();
    for (int j=0;j<dlg.m_Jewelry;j++)
      m_MonsterData.money.AddJewelry();
    m_MonsterData.money.Add(m_MonsterData.money.GetDefaultType(), dlg.m_Platinum);
  }	
}
