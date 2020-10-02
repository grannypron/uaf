/******************************************************************************
* Filename: Zonedlg.cpp
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

//#include "UAFWinEd.h"
#include "SmallPicDlg.h"
#include "SelectPicDlg.h"
#include "ScrollPicDlg.h"
#include "ColorBox.h"
//#include "GlobText.h"
//#include "GlobalData.h"
#include "Char.h"
#include "GameEvent.h"
#include "monster.h"
#include "ChooseMonster.h"
#include "ZoneDlg.h"
#include "SoundEventDlg.h"
#include "Help.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZoneDlg dialog


CZoneDlg::CZoneDlg(ZONE_DATA& zones, int curr, CWnd* pParent /*=NULL*/)
	: CDialog(CZoneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZoneDlg)
	m_ZoneName = _T("");
	m_TurningDifficulty = 0;
	m_ZoneNum = 0;
	m_AllowAreaMap = 0;
	m_ZoneMsg = _T("");
	m_EventChance = 0;
	m_EventMinutes = 1;
	m_CantRest = FALSE;
	m_AllowMagic = FALSE;
	m_UseNightSounds = FALSE;
	m_EndTime = 0;
	m_StartTime = 0;
	m_AllowAutoDarken = FALSE;
	//}}AFX_DATA_INIT

   if ((curr < 0) || (curr >= MAX_ZONES))
      curr = 0;

   m_ZoneNum = curr+1;   
   m_zones = zones;
   //m_monster = 0;
   m_monsterID.Clear();
   m_event = 0;
}

CZoneDlg::~CZoneDlg(void)
{
}

void CZoneDlg::DoDataExchange(CDataExchange* pDX)
{
   int zone;

   if (!pDX->m_bSaveAndValidate)
   {
      zone = m_ZoneNum-1;

      m_ZoneColor.SetColor(GetZoneColor(zone));
      //m_monster = m_zones.zones[zone].summonedMonster;
      m_monsterID = m_zones.zones[zone].summonedMonster;
      m_AllowAreaMap = (int)m_zones.zones[zone].allowMap;
      m_TurningDifficulty = m_zones.zones[zone].addedTurningDifficulty;
      m_AllowMagic = m_zones.zones[zone].AllowMagic;
      m_AllowAutoDarken = m_zones.zones[zone].AllowAutoDarken;
      m_IndoorCombatArt = m_zones.zones[zone].indoorCombatArt;
      m_OutdoorCombatArt = m_zones.zones[zone].outdoorCombatArt;
      m_CampArt = m_zones.zones[zone].campArt;
      m_TreasureArt = m_zones.zones[zone].treasurePicture;
      m_ZoneName = m_zones.zones[zone].zoneName;
      m_ZoneMsg = m_zones.zones[zone].zoneMsg;

      m_CantRest = !m_zones.zones[zone].restEvent.allowResting;
      m_EventChance = m_zones.zones[zone].restEvent.chance;
      m_EventMinutes = m_zones.zones[zone].restEvent.everyMin;

      char name[MAX_MEDITBUTTON_TEXT+1];

      if (m_ChooseCampArt.m_hWnd != NULL)
      {
         getBaseName(m_CampArt.filename, name, MAX_MEDITBUTTON_TEXT);
         m_ChooseCampArt.SetWindowText(name);
      }

      if (m_ChooseTreasureArt.m_hWnd != NULL)
      {
         getBaseName(m_TreasureArt.filename, name, MAX_MEDITBUTTON_TEXT);
         m_ChooseTreasureArt.SetWindowText(name);
      }

      if (m_ICArt.m_hWnd != NULL)
      {
         getBaseName(m_IndoorCombatArt, name, MAX_MEDITBUTTON_TEXT);
         m_ICArt.SetWindowText(name);
      }

      if (m_OCArt.m_hWnd != NULL)
      {
         getBaseName(m_OutdoorCombatArt, name, MAX_MEDITBUTTON_TEXT);
         m_OCArt.SetWindowText(name);
      }

      if (m_SummonedMonster.m_hWnd != NULL)
      {
        //getBaseName(monsterData.GetMonsterName(m_monster), name, MAX_MEDITBUTTON_TEXT);
        getBaseName(m_monsterID.UniqueName(), name, MAX_MEDITBUTTON_TEXT);
        m_SummonedMonster.SetWindowText(name);
      }

   }

	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CZoneDlg)
	DDX_Control(pDX, IDC_AVART, m_AVArt);
	DDX_Control(pDX, IDC_SUMMONEDMONSTER, m_SummonedMonster);
	DDX_Control(pDX, IDC_OUTDOORCOMBAT, m_OCArt);
	DDX_Control(pDX, IDC_INDOORCOMBAT, m_ICArt);
	DDX_Control(pDX, IDC_TREASUREART, m_ChooseTreasureArt);
	DDX_Control(pDX, IDC_CAMPART, m_ChooseCampArt);
	DDX_Control(pDX, IDC_ZONECOLOR, m_ZoneColor);
	DDX_Control(pDX, IDC_ZONESPIN, m_ZoneSpin);
	DDX_Text(pDX, IDC_ZONENAME, m_ZoneName);
	DDV_MaxChars(pDX, m_ZoneName, 50);
	DDX_Text(pDX, IDC_TURNINGDIFFICULTY, m_TurningDifficulty);
	DDX_Text(pDX, IDC_ZONENUMBER, m_ZoneNum);
	DDX_Radio(pDX, IDC_ALLOWAREAMAP, m_AllowAreaMap);
	DDX_Text(pDX, IDC_ZONEMSG, m_ZoneMsg);
	DDX_Text(pDX, IDC_EVENTCHANCE, m_EventChance);
	DDV_MinMaxUInt(pDX, m_EventChance, 0, 100);
	DDX_Text(pDX, IDC_EVENTMINUTES, m_EventMinutes);
	DDV_MinMaxUInt(pDX, m_EventMinutes, 0, 255);
	DDX_Check(pDX, IDC_CANTREST, m_CantRest);
	DDX_Check(pDX, IDC_ALLOWMAGIC, m_AllowMagic);
	DDX_Check(pDX, IDC_USENIGHTSOUNDS, m_UseNightSounds);
	DDX_Text(pDX, IDC_NIGHTTIMEEND, m_EndTime);
	DDX_Text(pDX, IDC_NIGHTTIMESTART, m_StartTime);
	DDX_Check(pDX, IDC_ALLOWAUTODARKEN, m_AllowAutoDarken);
	//}}AFX_DATA_MAP

   if (pDX->m_bSaveAndValidate)
   {
      zone = m_ZoneNum-1;

      //m_zones.zones[zone].summonedMonster = m_monster;
      m_zones.zones[zone].summonedMonster = m_monsterID;
      m_zones.zones[zone].allowMap = (MappingType)m_AllowAreaMap;
      m_zones.zones[zone].AllowMagic = m_AllowMagic;
      m_zones.zones[zone].AllowAutoDarken = m_AllowAutoDarken;
      m_zones.zones[zone].addedTurningDifficulty = m_TurningDifficulty;
      m_zones.zones[zone].indoorCombatArt = m_IndoorCombatArt;
      m_zones.zones[zone].outdoorCombatArt = m_OutdoorCombatArt;
      m_zones.zones[zone].campArt = m_CampArt;
      m_zones.zones[zone].treasurePicture = m_TreasureArt;
      m_zones.zones[zone].zoneName = m_ZoneName;
      m_zones.zones[zone].zoneMsg = m_ZoneMsg;
      m_zones.zones[zone].restEvent.allowResting = !m_CantRest;
      m_zones.zones[zone].restEvent.chance = m_EventChance;
      m_zones.zones[zone].restEvent.everyMin = m_EventMinutes;
   }
}


BEGIN_MESSAGE_MAP(CZoneDlg, CDialog)
	//{{AFX_MSG_MAP(CZoneDlg)
	ON_BN_CLICKED(IDC_CAMPART, OnCampart)
	ON_BN_CLICKED(IDC_INDOORCOMBAT, OnIndoorcombat)
	ON_BN_CLICKED(IDC_OUTDOORCOMBAT, OnOutdoorcombat)
	ON_BN_CLICKED(IDC_SUMMONEDMONSTER, OnSummonedmonster)
	ON_BN_CLICKED(IDC_TREASUREART, OnTreasureart)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ZONESPIN, OnDeltaposZonespin)
	ON_BN_CLICKED(IDC_NIGHTSOUNDS, OnNightsounds)
	ON_BN_CLICKED(IDC_BACKGROUNDSOUNDS, OnBackgroundsounds)
	ON_BN_CLICKED(IDC_AVART, OnAvart)
  ON_BN_CLICKED(IDC_ZONE_DATA, OnZoneDataQuestion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZoneDlg message handlers

BOOL CZoneDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ZoneSpin.SetRange(1, MAX_ZONES);
	m_ZoneSpin.SetPos(m_ZoneNum);
   
   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_CampArt.filename, name, MAX_MEDITBUTTON_TEXT);
   m_ChooseCampArt.SetWindowText(name);

   getBaseName(m_TreasureArt.filename, name, MAX_MEDITBUTTON_TEXT);
   m_ChooseTreasureArt.SetWindowText(name);

   getBaseName(m_IndoorCombatArt, name, MAX_MEDITBUTTON_TEXT);
   m_ICArt.SetWindowText(name);

   getBaseName(m_OutdoorCombatArt, name, MAX_MEDITBUTTON_TEXT);
   m_OCArt.SetWindowText(name);

   //getBaseName(monsterData.GetMonsterName(m_monster), name, MAX_MEDITBUTTON_TEXT);
   getBaseName(m_monsterID.UniqueName(), name, MAX_MEDITBUTTON_TEXT);
   m_SummonedMonster.SetWindowText(name);

    getBaseName(m_zones.AVArt, name, MAX_MEDITBUTTON_TEXT);
    m_AVArt.SetWindowText(name);

   m_UseNightSounds=m_zones.zones[m_ZoneNum-1].bgSounds.UseNightMusic;
   m_EndTime = m_zones.zones[m_ZoneNum-1].bgSounds.EndTime;
   m_StartTime = m_zones.zones[m_ZoneNum-1].bgSounds.StartTime;

   GotoDlgCtrl(GetDlgItem(IDC_ZONEMSG));
   UpdateData(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CZoneDlg::OnCampart() 
{
   CSmallPicDlg dlg(m_CampArt);

   DWORD result = dlg.DoModal();

   if (result == IDOK)
   {
      m_CampArt = dlg.m_SmallPic;
      m_CampArt.picType = SmallPicDib;
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_CampArt.filename, name, MAX_MEDITBUTTON_TEXT);
      m_ChooseCampArt.SetWindowText(name);
   }
}

void CZoneDlg::OnIndoorcombat() 
{
   CScrollPicDlg dlg(m_IndoorCombatArt, rte.CombatArtDir(), "");
   
   DWORD result = dlg.DoModal();

   if (result == IDOK)
   {
      m_IndoorCombatArt = dlg.m_Filename;
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_IndoorCombatArt, name, MAX_MEDITBUTTON_TEXT);
      m_ICArt.SetWindowText(name);
   }
}

void CZoneDlg::OnOutdoorcombat() 
{
   CScrollPicDlg dlg(m_OutdoorCombatArt, rte.CombatArtDir(), "");
   
   DWORD result = dlg.DoModal();

   if (result == IDOK)
   {
      m_OutdoorCombatArt = dlg.m_Filename;
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_OutdoorCombatArt, name, MAX_MEDITBUTTON_TEXT);
      m_OCArt.SetWindowText(name);
   }
}

void CZoneDlg::OnSummonedmonster() 
{
	//CChooseMonster dlg(m_monster, 1); // select 1 monster
	CChooseMonster dlg(m_monsterID, 1); // select 1 monster
  if (dlg.DoModal() == IDOK)
  {
     char name[MAX_MEDITBUTTON_TEXT+1];
     //m_monster = dlg.m_MonsterNum;
     m_monsterID = dlg.m_monsterID;
     //getBaseName(monsterData.GetMonsterName(m_monster), name, MAX_MEDITBUTTON_TEXT);
     getBaseName(m_monsterID.UniqueName(), name, MAX_MEDITBUTTON_TEXT);
     m_SummonedMonster.SetWindowText(name);
  }	
}

void CZoneDlg::OnTreasureart() 
{
   CSelectPicDlg dlg(m_TreasureArt, SmallPicDib | BigPicDib | SpriteDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_TreasureArt);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_TreasureArt.filename, name, MAX_MEDITBUTTON_TEXT);
      m_ChooseTreasureArt.SetWindowText(name);
   }
}

void CZoneDlg::OnDeltaposZonespin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	UpdateData(TRUE);

	m_ZoneNum += pNMUpDown->iDelta;

	if (m_ZoneNum <= 0)
		m_ZoneNum = MAX_ZONES;

	if (m_ZoneNum > MAX_ZONES)
		m_ZoneNum = 1;

	UpdateData(FALSE);	
	*pResult = 0;
}


void CZoneDlg::OnBackgroundsounds() 
{	
  UpdateData(TRUE);
  SOUND_EVENT tempEvent;
  POSITION pos = m_zones.zones[m_ZoneNum-1].bgSounds.backgroundSounds.sounds.GetHeadPosition();
  while (pos != NULL)
  {
    tempEvent.sounds.AddTail(m_zones.zones[m_ZoneNum-1].bgSounds.backgroundSounds.sounds.GetAt(pos));
    m_zones.zones[m_ZoneNum-1].bgSounds.backgroundSounds.sounds.GetNext(pos);
  }

  CSoundEventDlg dlg(tempEvent);
  if (dlg.DoModal()==IDOK)
  {
    m_zones.zones[m_ZoneNum-1].bgSounds.backgroundSounds.Clear();
    dlg.GetData(tempEvent);

    pos = tempEvent.sounds.GetHeadPosition();
    while (pos != NULL)
    {
      m_zones.zones[m_ZoneNum-1].bgSounds.backgroundSounds.sounds.AddTail(tempEvent.sounds.GetAt(pos));
      tempEvent.sounds.GetNext(pos);
    }
  }
  UpdateData(FALSE);
}

void CZoneDlg::OnNightsounds() 
{
  UpdateData(TRUE);
  SOUND_EVENT tempEvent;
  POSITION pos = m_zones.zones[m_ZoneNum-1].bgSounds.nightSounds.sounds.GetHeadPosition();
  while (pos != NULL)
  {
    tempEvent.sounds.AddTail(m_zones.zones[m_ZoneNum-1].bgSounds.nightSounds.sounds.GetAt(pos));
    m_zones.zones[m_ZoneNum-1].bgSounds.nightSounds.sounds.GetNext(pos);
  }

  CSoundEventDlg dlg(tempEvent);
  if (dlg.DoModal()==IDOK)
  {
    m_zones.zones[m_ZoneNum-1].bgSounds.nightSounds.Clear();
    dlg.GetData(tempEvent);

    pos = tempEvent.sounds.GetHeadPosition();
    while (pos != NULL)
    {
      m_zones.zones[m_ZoneNum-1].bgSounds.nightSounds.sounds.AddTail(tempEvent.sounds.GetAt(pos));
      tempEvent.sounds.GetNext(pos);
    }
  }	

  m_UseNightSounds = (m_zones.zones[m_ZoneNum-1].bgSounds.nightSounds.sounds.GetCount()>0);
  UpdateData(FALSE);
}


void CZoneDlg::OnOK() 
{
  UpdateData(TRUE);
  m_zones.zones[m_ZoneNum-1].bgSounds.UseNightMusic = m_UseNightSounds;
  m_zones.zones[m_ZoneNum-1].bgSounds.EndTime = m_EndTime;
  m_zones.zones[m_ZoneNum-1].bgSounds.StartTime = m_StartTime;
	
	CDialog::OnOK();
}

void CZoneDlg::OnAvart() 
{
   CScrollPicDlg dlg(m_zones.AVArt, rte.AreaViewArtDir(), "");
   
   DWORD result = dlg.DoModal();

   if (result == IDOK)
   {
      m_zones.AVArt = dlg.m_Filename;
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_zones.AVArt, name, MAX_MEDITBUTTON_TEXT);
      m_AVArt.SetWindowText(name);
   }
}

void CZoneDlg::OnZoneDataQuestion() 
{
  CHelp dlg("ZONE_DATA");
  dlg.DoModal();
}

