/******************************************************************************
* Filename: ItemDBDlg.cpp
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
#include "items.h"
#include "MagicalItemProp.h"
#include "GlobText.h"
#include "SoundChooser.h"
#include "SmallPicDlg.h"
#include "EventViewer.h"
#include "ItemDBDlg.h"
#include "ClassDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString CStringFromBase38(unsigned int v);
unsigned int CString2Base38(const CString& str);
/////////////////////////////////////////////////////////////////////////////
// CItemDBDlg dialog

CItemDBDlg::CItemDBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CItemDBDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CItemDBDlg)
	m_BundleQty = 0;
	//m_Cleric = FALSE;
	m_Cost = 0;
	m_DmgBonusLg = 0;
	m_DmgBonusSm = 0;
	m_DmgDiceLg = 0;
	m_DmgDiceSm = 0;
	//m_Druid = FALSE;
	m_Encumbrance = 0;
	m_ExpPts = 0;
	//m_Fighter = FALSE;
	//m_MagicUser = FALSE;
	m_Name = _T("No Item");
	m_NbrDiceLg = 0;
	m_NbrDiceSm = 0;
	//m_Paladin = FALSE;
	m_ProtectionBase = 0;
	m_RangeMax = 0;
	//m_Ranger = FALSE;
	m_ROF = 0.0;
	//m_Thief = FALSE;
	m_IdName = _T("");
	m_CoinName = _T("");
	//m_NonLethal = FALSE;
	m_ExamineLabel = _T("");
	m_AttackMsg = _T("");
	m_CanBeHalvedJoined = TRUE;
	m_CanBeTradeDropSoldDep = TRUE;
	//}}AFX_DATA_INIT

}

void CItemDBDlg::UpdateControls()
{
  CWnd *pWnd;
  BOOL enable;

  pWnd = GetDlgItem(IDC_CANBEHALVED);
  if (m_ItemData.Bundle_Qty < 2)
  {
    m_ItemData.CanBeHalvedJoined=FALSE;    
    if (pWnd != NULL) pWnd->EnableWindow(FALSE);
  }
  else
    if (pWnd != NULL) pWnd->EnableWindow(TRUE);

  if (m_ItemData.Wpn_Type==NotWeapon)
    enable = FALSE;
  else
    enable = ((m_ItemData.Wpn_Type==Ammo)||(m_ItemData.RangeMax > 1));
  if (m_MissileArt.m_hWnd != NULL) m_MissileArt.EnableWindow(enable);
  
  enable = ((WpnUsesAmmo(m_ItemData.Wpn_Type)) || (m_ItemData.Wpn_Type==Ammo));
  pWnd = GetDlgItem(IDC_AMMOTYPE);
  if (pWnd != NULL) pWnd->EnableWindow(enable);

  enable = (m_ItemData.Wpn_Type != NotWeapon);

  pWnd = GetDlgItem(IDC_ISNONLETHAL);
  if (pWnd != NULL) pWnd->EnableWindow(enable);    
  pWnd = GetDlgItem(IDC_ROF_PER_RND);
  if (pWnd != NULL) pWnd->EnableWindow(enable);
  
  pWnd = GetDlgItem(IDC_RANGE);
  if (pWnd != NULL) 
  {
    if (m_ItemData.Wpn_Type==Ammo)
      pWnd->EnableWindow(FALSE);
    else
      pWnd->EnableWindow(enable);
  }
  
  if (m_ItemData.Wpn_Type == Bow) enable = FALSE;
  if (m_ItemData.Wpn_Type == Crossbow) enable = FALSE;
  enable &= (m_ItemData.Wpn_Type != SpellCaster);

  pWnd = GetDlgItem(IDC_DMG_BONUS_LG);
  if (pWnd != NULL) pWnd->EnableWindow(enable);
  pWnd = GetDlgItem(IDC_DMG_BONUS_SM);
  if (pWnd != NULL) pWnd->EnableWindow(enable);
  pWnd = GetDlgItem(IDC_DMG_DICE_LG);
  if (pWnd != NULL) pWnd->EnableWindow(enable);
  pWnd = GetDlgItem(IDC_DMG_DICE_SM);
  if (pWnd != NULL) pWnd->EnableWindow(enable);
  pWnd = GetDlgItem(IDC_NBR_DICE_LG);
  if (pWnd != NULL) pWnd->EnableWindow(enable);
  pWnd = GetDlgItem(IDC_NBR_DICE_SM);
  if (pWnd != NULL) pWnd->EnableWindow(enable);
  if (m_HitArt.m_hWnd != NULL) m_HitArt.EnableWindow(enable);

  if (    (m_ItemData.Wpn_Type == SpellCaster)
       || (m_ItemData.Wpn_Type == SpellLikeAbility))
  {
    pWnd = GetDlgItem(IDC_ID_Display_Name);
    if (pWnd != NULL) pWnd->SetWindowText("ID|Display Name");
    pWnd = GetDlgItem(IDC_ISNONLETHAL);
    if (pWnd != NULL) pWnd->EnableWindow(FALSE);
    pWnd = GetDlgItem(IDC_HITSOUND);
    if (pWnd != NULL) pWnd->EnableWindow(FALSE);
    pWnd = GetDlgItem(IDC_MISSSOUND);
    if (pWnd != NULL) pWnd->EnableWindow(FALSE);
    pWnd = GetDlgItem(IDC_MISSILEART);
    if (pWnd != NULL) pWnd->EnableWindow(FALSE);
  }
  else
  {
    pWnd = GetDlgItem(IDC_ID_Display_Name);
    if (pWnd != NULL) pWnd->SetWindowText("ID Name");
  };
}


void CItemDBDlg::DoDataExchange(CDataExchange* pDX)
{
  long temp;

  if (!pDX->m_bSaveAndValidate)
  {
    m_Name = m_ItemData.UniqueName();
    if (m_Name.GetLength() > MAX_ITEM_NAME) m_Name.SetAt(MAX_ITEM_NAME,'\0');
    m_IdName = m_ItemData.IdName();
    m_ExpPts = m_ItemData.Experience;
    m_Cost = m_ItemData.Cost;
    m_Encumbrance = m_ItemData.Encumbrance;
    m_BundleQty = m_ItemData.Bundle_Qty;
    m_NonLethal = m_ItemData.IsNonLethal;
    m_CanBeHalvedJoined = m_ItemData.CanBeHalvedJoined;
    m_CanBeTradeDropSoldDep = m_ItemData.CanBeTradeDropSoldDep;

#ifdef OldLocation20180129
    m_LocationCarried = CStringFromBase38(m_ItemData.Location_Readied.Location());
#else
    m_LocationCarried = CStringFromBase38(m_ItemData.Location_Readied);
#endif
    //if (m_LocationCarried.m_hWnd != NULL)
      //m_LocationCarried.SetCurSel((int)m_ItemData.Location_Carried);
    
    //if (m_ItemData.Location_Carried == WeaponHand)
    //{
    //  if ((m_ItemData.Hands_to_Use < 0)||(m_ItemData.Hands_to_Use > 2))
    //    m_ItemData.Hands_to_Use = 1;
    //}
    //else if (m_ItemData.Location_Carried == ShieldHand)
    //{
    //  m_ItemData.Hands_to_Use = 1;
    //}
    //else
    //  m_ItemData.Hands_to_Use = 0;

    if (m_HandsNeeded.m_hWnd != NULL)
    {
      m_HandsNeeded.SetCurSel((int)m_ItemData.Hands_to_Use);
      if (m_ItemData.Location_Readied != WeaponHand)
        m_HandsNeeded.EnableWindow(FALSE);
      else
        m_HandsNeeded.EnableWindow(TRUE);
    }

    m_DmgDiceSm = m_ItemData.Dmg_Dice_Sm;
    m_NbrDiceSm = m_ItemData.Nbr_Dice_Sm;
    m_DmgBonusSm = m_ItemData.Dmg_Bonus_Sm;
    m_DmgDiceLg = m_ItemData.Dmg_Dice_Lg;
    m_NbrDiceLg = m_ItemData.Nbr_Dice_Lg;
    m_DmgBonusLg = m_ItemData.Dmg_Bonus_Lg;
    m_ROF = m_ItemData.ROF_Per_Round;
    m_ProtectionBase = m_ItemData.Protection_Base;
    m_ExamineLabel = m_ItemData.ExamineLabel;      
    if (m_ExamineLabel.GetLength()>10) m_ExamineLabel.SetAt(10,'\0');
    m_AttackMsg = m_ItemData.attackMsg;    
    if (m_AttackMsg.GetLength()>20) m_AttackMsg.SetAt(20,'\0');

    if (m_WpnType.m_hWnd != NULL)
      m_WpnType.SetCurSel((int)m_ItemData.Wpn_Type);

    if (m_AmmoType.m_hWnd!=NULL)
    {
      int index = m_AmmoType.FindStringExact(0,m_ItemData.AmmoType);
      if (index == CB_ERR)
      {
        if (m_ItemData.AmmoType.IsEmpty())
        {
          index = 0;
        }
        else
        {
          ASSERT(FALSE);
          index = m_AmmoType.AddString(m_ItemData.AmmoType);
        }        
      }
      m_AmmoType.SetCurSel(index);
    }

    m_RangeMax = m_ItemData.RangeMax;
    if (m_RangeMax < 1) m_RangeMax = 1;
/*  
    if (m_ItemData.Usable_by_Class & ClericFlag)
      m_Cleric = TRUE;
    else
      m_Cleric = FALSE;
  
    if (m_ItemData.Usable_by_Class & DruidFlag)
      m_Druid = TRUE;
    else
      m_Druid = FALSE;
  
    if (m_ItemData.Usable_by_Class & FighterFlag)
      m_Fighter = TRUE;
    else
      m_Fighter = FALSE;
     
    if (m_ItemData.Usable_by_Class & MagicUserFlag)
      m_MagicUser = TRUE;
    else
      m_MagicUser = FALSE;
     
    if (m_ItemData.Usable_by_Class & PaladinFlag)
      m_Paladin = TRUE;
    else
      m_Paladin = FALSE;
  
    if (m_ItemData.Usable_by_Class & RangerFlag)
      m_Ranger = TRUE;
    else
      m_Ranger = FALSE;
  
    if (m_ItemData.Usable_by_Class & ThiefFlag)
      m_Thief = TRUE;
    else
      m_Thief = FALSE;
*/
    UpdateControls();
  }

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemDBDlg)
	DDX_Control(pDX, IDC_EXAMINEEVENT, m_ExamineEvent);
	DDX_Control(pDX, IDC_HITANIM, m_HitArt);
	DDX_Control(pDX, IDC_AMMOTYPE, m_AmmoType);
	DDX_Control(pDX, IDC_LAUNCHSOUND, m_LaunchSound);
	DDX_Control(pDX, IDC_MISSILEART, m_MissileArt);
	DDX_Control(pDX, IDC_MISSSOUND, m_MissSound);
	DDX_Control(pDX, IDC_HITSOUND, m_HitSound);
	DDX_Control(pDX, IDC_WPNTYPE, m_WpnType);
	DDX_Text(pDX, IDC_LOCATIONCARRIED, m_LocationCarried);
	DDX_Control(pDX, IDC_HANDSNEEDED, m_HandsNeeded);
	DDX_Text(pDX, IDC_BUNDLEQTY, m_BundleQty);
	//DDX_Check(pDX, IDC_CLERIC, m_Cleric);
	DDX_Text(pDX, IDC_COST, m_Cost);
	DDX_Text(pDX, IDC_DMG_BONUS_LG, m_DmgBonusLg);
	DDX_Text(pDX, IDC_DMG_BONUS_SM, m_DmgBonusSm);
	DDX_Text(pDX, IDC_DMG_DICE_LG, m_DmgDiceLg);
	DDX_Text(pDX, IDC_DMG_DICE_SM, m_DmgDiceSm);
	//DDX_Check(pDX, IDC_DRUIDCHECK, m_Druid);
	DDX_Text(pDX, IDC_ENCUMBRANCE, m_Encumbrance);
	DDX_Text(pDX, IDC_EXPPTS, m_ExpPts);
	//DDX_Check(pDX, IDC_FIGHTER, m_Fighter);
	//DDX_Check(pDX, IDC_MAGICUSER, m_MagicUser);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_NBR_DICE_LG, m_NbrDiceLg);
	DDX_Text(pDX, IDC_NBR_DICE_SM, m_NbrDiceSm);
	//DDX_Check(pDX, IDC_PALADIN, m_Paladin);
	DDX_Text(pDX, IDC_PROTECTIONBASE, m_ProtectionBase);
	DDX_Text(pDX, IDC_RANGE, m_RangeMax);
	//DDX_Check(pDX, IDC_RANGER, m_Ranger);
	DDX_Text(pDX, IDC_ROF_PER_RND, m_ROF);
	//DDX_Check(pDX, IDC_THIEF, m_Thief);
	DDX_Text(pDX, IDC_IDNAME, m_IdName);
	DDX_Text(pDX, IDC_COINNAME, m_CoinName);
	DDX_Check(pDX, IDC_ISNONLETHAL, m_NonLethal);
	DDX_Text(pDX, IDC_EXAMINELABEL, m_ExamineLabel);
	DDX_Text(pDX, IDC_ITEMATTACKMSG, m_AttackMsg);
	DDX_Check(pDX, IDC_CANBEHALVED, m_CanBeHalvedJoined);
	DDX_Check(pDX, IDC_CANBETRADEDROPSOLDDEP, m_CanBeTradeDropSoldDep);
	//}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
  {
    if (m_Name.GetLength() > MAX_ITEM_NAME) m_Name.SetAt(MAX_ITEM_NAME,'\0');
    m_ItemData.SetUniqueName(m_Name);
    m_ItemData.SetIdName(m_IdName);
    m_ItemData.Experience = m_ExpPts;
    m_ItemData.Cost = m_Cost;
    m_ItemData.Encumbrance = m_Encumbrance;
    m_ItemData.Bundle_Qty = m_BundleQty;
    m_ItemData.IsNonLethal = m_NonLethal;

    //temp = m_LocationCarried.GetCurSel();
    //if (temp == LB_ERR)
    //  temp = 0;

    //m_ItemData.Location_Carried = (itemLocationType) temp;
#ifdef OldLocation20180129
    m_ItemData.Location_Readied = (itemReadiedLocation)CString2Base38(m_LocationCarried);
#else
    m_ItemData.Location_Readied = CString2Base38(m_LocationCarried);
#endif
    temp = m_HandsNeeded.GetCurSel();
    m_ItemData.Hands_to_Use = temp;

    if (m_ItemData.Location_Readied == WeaponHand)
    {
      if ((m_ItemData.Hands_to_Use < 0)||(m_ItemData.Hands_to_Use > 2))
        m_ItemData.Hands_to_Use = 1;
    }
    else if (m_ItemData.Location_Readied == ShieldHand)
    {
      m_ItemData.Hands_to_Use = 1;
    }
    else
      m_ItemData.Hands_to_Use = 0;

    if (m_HandsNeeded.m_hWnd != NULL)
    {
      if (m_ItemData.Location_Readied != WeaponHand)
        m_HandsNeeded.EnableWindow(FALSE);
      else
        m_HandsNeeded.EnableWindow(TRUE);
    }

    m_ItemData.Dmg_Dice_Sm = m_DmgDiceSm;
    m_ItemData.Nbr_Dice_Sm = m_NbrDiceSm;
    m_ItemData.Dmg_Bonus_Sm = m_DmgBonusSm;
    m_ItemData.Dmg_Dice_Lg = m_DmgDiceLg;
    m_ItemData.Nbr_Dice_Lg = m_NbrDiceLg;
    m_ItemData.Dmg_Bonus_Lg = m_DmgBonusLg;
    m_ItemData.ROF_Per_Round = m_ROF;
    m_ItemData.Protection_Base = m_ProtectionBase;   
    m_ItemData.Wpn_Type = (weaponClassType)m_WpnType.GetCurSel();
    m_ItemData.CanBeHalvedJoined = m_CanBeHalvedJoined;
    m_ItemData.CanBeTradeDropSoldDep = m_CanBeTradeDropSoldDep;

    int index = m_AmmoType.GetCurSel();
    if (index != CB_ERR)
    {
      m_AmmoType.GetLBText(index, m_ItemData.AmmoType);
    }
    else
    {
      CString tmp;
      m_AmmoType.GetWindowText(tmp);
      m_ItemData.AmmoType = tmp;
      itemData.AddAmmoType(tmp);
      m_AmmoType.AddString(tmp);
    }

    if (m_RangeMax < 1) m_RangeMax = 1;
    m_ItemData.RangeMax = m_RangeMax;
    /*
    m_ItemData.Usable_by_Class = 0;

 
    if (m_Cleric) m_ItemData.Usable_by_Class |= ClericFlag;
    if (m_Druid) m_ItemData.Usable_by_Class |= DruidFlag;
    if (m_Fighter) m_ItemData.Usable_by_Class |= FighterFlag;
    if (m_MagicUser) m_ItemData.Usable_by_Class |= MagicUserFlag;
    if (m_Paladin) m_ItemData.Usable_by_Class |= PaladinFlag;
    if (m_Ranger) m_ItemData.Usable_by_Class |= RangerFlag;
    if (m_Thief) m_ItemData.Usable_by_Class |= ThiefFlag;    
*/

    if (m_ExamineLabel.GetLength()>10) m_ExamineLabel.SetAt(10,'\0');    
    m_ItemData.ExamineLabel=m_ExamineLabel;
    if (m_AttackMsg.GetLength()>20) m_AttackMsg.SetAt(20,'\0');    
    m_ItemData.attackMsg=m_AttackMsg;

    UpdateControls();
  }
}


BEGIN_MESSAGE_MAP(CItemDBDlg, CDialog)
	//{{AFX_MSG_MAP(CItemDBDlg)
	ON_CBN_SELCHANGE(IDC_WPNTYPE, OnSelchangeWpntype)
	ON_BN_CLICKED(IDC_MAGICALPROPERTIES, OnMagicalproperties)
	ON_BN_CLICKED(IDC_MISSSOUND, OnMisssound)
	ON_BN_CLICKED(IDC_HITSOUND, OnHitsound)
	ON_BN_CLICKED(IDC_MISSILEART, OnMissileart)
	ON_BN_CLICKED(IDC_LAUNCHSOUND, OnLaunchsound)
  ON_BN_CLICKED(IDC_CLASSLIST, OnClassList)
	ON_BN_CLICKED(IDC_HITANIM, OnHitanim)
	ON_CBN_SELCHANGE(IDC_LOCATIONCARRIED, OnSelchangeLocationcarried)
	ON_EN_KILLFOCUS(IDC_RANGE, OnKillfocusRange)
	ON_EN_CHANGE(IDC_RANGE, OnChangeRange)
	ON_BN_CLICKED(IDC_EXAMINEEVENT, OnExamineevent)
	ON_EN_KILLFOCUS(IDC_BUNDLEQTY, OnKillfocusBundleqty)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemDBDlg message handlers

BOOL CItemDBDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  //m_LocationCarried.ResetContent();
  m_LocationCarried.Empty();
  m_WpnType.ResetContent();
  m_HandsNeeded.ResetContent();
  m_AmmoType.ResetContent();

  int i;

  //for (i=0;i<MAX_LOCATION_CARRIED;i++)
    //m_LocationCarried.AddString(LocationCarriedText[i]);

  for (i=0;i<MAX_WEAPON_CLASS;i++)
    m_WpnType.AddString(WeaponClassText[i]);

  for (i=0;i<itemData.m_AmmoTypes.GetSize();i++)
    m_AmmoType.AddString(itemData.m_AmmoTypes[i]);

  m_HandsNeeded.AddString("0");
  m_HandsNeeded.AddString("1");
  m_HandsNeeded.AddString("2");

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_ItemData.MissSound, name, MAX_MEDITBUTTON_TEXT);
  m_MissSound.SetWindowText(name);

  getBaseName(m_ItemData.HitSound, name, MAX_MEDITBUTTON_TEXT);
  m_HitSound.SetWindowText(name);

  getBaseName(m_ItemData.LaunchSound, name, MAX_MEDITBUTTON_TEXT);
  m_LaunchSound.SetWindowText(name);

  getBaseName(m_ItemData.MissileArt.filename, name, MAX_MEDITBUTTON_TEXT);
  m_MissileArt.SetWindowText(name);    
  getBaseName(m_ItemData.HitArt.filename, name, MAX_MEDITBUTTON_TEXT);
  m_HitArt.SetWindowText(name);    

  if (!globalData.eventData.IsValidEvent(m_ItemData.ExamineEvent))
    m_ItemData.ExamineEvent=NO_EVENT;

  getBaseName(GetEventIdDescription(m_ItemData.ExamineEvent, GlobalEventSrc), name, MAX_MEDITBUTTON_TEXT);
  m_ExamineEvent.SetWindowText(name);
  m_ExamineLabel=m_ItemData.ExamineLabel;
  if (m_ExamineLabel.GetLength()>10) m_ExamineLabel.SetAt(10,'\0');
  m_AttackMsg = m_ItemData.attackMsg;    
  if (m_AttackMsg.GetLength()>20) m_AttackMsg.SetAt(20,'\0');

  m_CoinName = globalData.moneyData.GetName(globalData.moneyData.GetDefaultType());
  UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CItemDBDlg::OnSelchangeWpntype() 
{
  UpdateData(TRUE); // get current wpn type
  UpdateData(FALSE); // put data back, adjust controls
}

void CItemDBDlg::OnClassList() 
{
  UpdateData(TRUE); // get current wpn type
  //CClassDlg dlg(m_ItemData.Usable_by_Class);
  CClassDlg dlg(&m_ItemData.usableByBaseclass);
  if (dlg.DoModal() == IDOK)
  {
    m_ItemData.usableByBaseclass.RemoveAll();
    m_ItemData.usableByBaseclass.Append(dlg.m_selectedBaseclasses.baseclasses);
  };
  UpdateData(FALSE); // put data back, adjust controls
}

void CItemDBDlg::OnMagicalproperties() 
{
  CMagicalItemProp dlg(m_ItemData);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
    m_ItemData = dlg.m_ItemData;
}

void CItemDBDlg::OnMisssound() 
{
  CSoundChooser dlg(m_ItemData.MissSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_ItemData.MissSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_ItemData.MissSound, name, MAX_MEDITBUTTON_TEXT);
    m_MissSound.SetWindowText(name);
  }
}

void CItemDBDlg::OnHitsound() 
{
  CSoundChooser dlg(m_ItemData.HitSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_ItemData.HitSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_ItemData.HitSound, name, MAX_MEDITBUTTON_TEXT);
    m_HitSound.SetWindowText(name);
  }	
}

void CItemDBDlg::OnMissileart() 
{
  UpdateData(TRUE);

  m_ItemData.MissileArt.picType = SpriteDib;

	CSmallPicDlg dlg(m_ItemData.MissileArt);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    char name[MAX_MEDITBUTTON_TEXT+1];
    m_ItemData.MissileArt = dlg.m_SmallPic;
    m_ItemData.MissileArt.picType = SpriteDib;
    getBaseName(m_ItemData.MissileArt.filename, name, MAX_MEDITBUTTON_TEXT);
    m_MissileArt.SetWindowText(name);    	
  }

  UpdateData(FALSE);
}

void CItemDBDlg::OnLaunchsound() 
{
  CSoundChooser dlg(m_ItemData.LaunchSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_ItemData.LaunchSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_ItemData.LaunchSound, name, MAX_MEDITBUTTON_TEXT);
    m_LaunchSound.SetWindowText(name);
  }
}

void CItemDBDlg::OnHitanim() 
{
  UpdateData(TRUE);
  m_ItemData.HitArt.picType = SpriteDib;
	CSmallPicDlg dlg(m_ItemData.HitArt);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    char name[MAX_MEDITBUTTON_TEXT+1];
    m_ItemData.HitArt = dlg.m_SmallPic;
    m_ItemData.HitArt.picType = SpriteDib;
    getBaseName(m_ItemData.HitArt.filename, name, MAX_MEDITBUTTON_TEXT);
    m_HitArt.SetWindowText(name);    	
  }
  UpdateData(FALSE);	
}

void CItemDBDlg::OnSelchangeLocationcarried() 
{
  UpdateData(TRUE); // get current wpn type
  UpdateData(FALSE); // put data back, adjust controls
}

void CItemDBDlg::OnOK() 
{
  UpdateData(TRUE);
  if ((m_ItemData.Wpn_Type != Ammo)&&(m_ItemData.Wpn_Type != NotWeapon))
  {
    if (m_ItemData.RangeMax==0)
    {
      if (MsgBoxYesNo("This weapon has a range of zero, which will not work. Continue Anyway?")==IDNO) return;
    }
  }
	
	CDialog::OnOK();
}

void CItemDBDlg::OnKillfocusRange() 
{
  UpdateData(TRUE); // get current range
  UpdateData(FALSE); // put data back, adjust controls	
}

void CItemDBDlg::OnChangeRange() 
{
  //UpdateData(TRUE); // get current range
  //UpdateData(FALSE); // put data back, adjust controls		
}

void CItemDBDlg::OnExamineevent() 
{
  UpdateData(TRUE);
  CEventViewer dlg(m_ItemData.GetItemEventText(), m_ItemData.ExamineEvent);
  if (dlg.DoModal() == IDOK)
  {
    m_ItemData.ExamineEvent = dlg.m_SingleEventRoot;

    if (!globalData.eventData.IsValidEvent(m_ItemData.ExamineEvent))
      m_ItemData.ExamineEvent=NO_EVENT;

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetEventIdDescription(m_ItemData.ExamineEvent, GlobalEventSrc), name, MAX_MEDITBUTTON_TEXT);
    m_ExamineEvent.SetWindowText(name);
  }		
}

void CItemDBDlg::OnKillfocusBundleqty() 
{
	UpdateData(TRUE);
  UpdateData(FALSE);	
}
