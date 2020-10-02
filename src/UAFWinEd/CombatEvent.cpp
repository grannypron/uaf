/******************************************************************************
* Filename: CombatEvent.cpp
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
#include "globtext.h"
#include "GlobalData.h"
#include "GameEvent.h"
#include "SoundChooser.h"
#include "SoundEventDlg.h"
#include "Monster.h"
#include "ChooseCombatMonsters.h"
#include "SelectPicDlg.h"
#include "CombatEvent.h"
#include "Help.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCombat dialog


CCombat::CCombat(COMBAT_EVENT_DATA& combat, CWnd* pParent /*=NULL*/)
	: CDialog(CCombat::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCombat)
	m_AutoApproach = FALSE;
	m_NoMagic = FALSE;
	m_Morale = 0;
	m_NoTreasure = FALSE;
	m_Outdoors = FALSE;
	m_PartyNeverDies = FALSE;
	m_Text = _T("");
	m_RandomMonster = FALSE;
	m_PartyNoExperience = FALSE;
	//}}AFX_DATA_INIT

	m_combat = combat;
}


void CCombat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCombat)
	DDX_Control(pDX, IDC_TURNUNDEADSOUND, m_TurnUndeadSound);
	DDX_Control(pDX, IDC_DEATHSOUND, m_DeathSound);
	DDX_Control(pDX, IDC_MOVESOUND, m_MoveSound);
	DDX_Control(pDX, IDC_SEEMONSTER, m_SeeMonster);
	DDX_Control(pDX, IDC_TURNUNDEAD, m_TurnUndead);
	DDX_Control(pDX, IDC_SURPRISE, m_Surprise);
	DDX_Control(pDX, IDC_DISTANCE, m_Distance);
	DDX_Control(pDX, IDC_DIRECTION, m_Direction);
	DDX_Check(pDX, IDC_AUTOAPPROACH, m_AutoApproach);
	DDX_Check(pDX, IDC_MAGICNOTWORK, m_NoMagic);
	DDX_Text(pDX, IDC_MORALE, m_Morale);
	DDV_MinMaxInt(pDX, m_Morale, 0, 100);
	DDX_Check(pDX, IDC_NOTREASURE, m_NoTreasure);
	DDX_Check(pDX, IDC_OUTDOORS, m_Outdoors);
	DDX_Check(pDX, IDC_PARTYNEVERDIES, m_PartyNeverDies);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Check(pDX, IDC_RANDOMMONSTER, m_RandomMonster);
	DDX_Check(pDX, IDC_PARTYNOEXPERIENCE, m_PartyNoExperience);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCombat, CDialog)
	//{{AFX_MSG_MAP(CCombat)
	ON_BN_CLICKED(IDC_CHOOSEMONSTERS, OnChoosemonsters)
	ON_CBN_SELCHANGE(IDC_DIRECTION, OnSelchangeDirection)
	ON_CBN_SELCHANGE(IDC_DISTANCE, OnSelchangeDistance)
	ON_BN_CLICKED(IDC_SEEMONSTER, OnSeemonster)
	ON_CBN_SELCHANGE(IDC_SURPRISE, OnSelchangeSurprise)
	ON_CBN_SELCHANGE(IDC_TURNUNDEAD, OnSelchangeTurnundead)
	ON_BN_CLICKED(IDC_DEATHSOUND, OnDeathsound)
	ON_BN_CLICKED(IDC_MOVESOUND, OnMovesound)
	ON_BN_CLICKED(IDC_TURNUNDEADSOUND, OnTurnundeadsound)
	ON_BN_CLICKED(IDC_BGSOUNDS, OnBgsounds)
  ON_BN_CLICKED(IDC_COMBAT_EVENT, OnCombatEventQuestion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCombat message handlers

void CCombat::OnChoosemonsters() 
{
	CChooseCombatMonsters dlg(m_combat.monsters);

	UINT result = dlg.DoModal();

	if (result == IDOK)
		dlg.GetData(m_combat.monsters);	
}

void CCombat::OnSelchangeDirection() 
{
	m_combat.direction = 
		(eventDirType) m_Direction.GetCurSel();
}

void CCombat::OnSelchangeDistance() 
{
	m_combat.distance = 
		(eventDistType) m_Distance.GetCurSel();		
}

void CCombat::OnSeemonster() 
{
  UpdateData(TRUE);

   CSelectPicDlg dlg(m_combat.pic, SmallPicDib | BigPicDib | SpriteDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_combat.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_combat.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_SeeMonster.SetWindowText(name);
      UpdateData(FALSE);
   }
}

void CCombat::OnSelchangeSurprise() 
{
	m_combat.m_eSurprise = 
		(eventSurpriseType) m_Surprise.GetCurSel();
}

void CCombat::OnSelchangeTurnundead() 
{
	m_combat.turningMod =
		(eventTurnUndeadModType) m_TurnUndead.GetCurSel();
}

void CCombat::OnOK() 
{
	UpdateData(TRUE);
	
	m_combat.distance = (eventDistType)m_Distance.GetCurSel();
	m_combat.direction = (eventDirType)m_Direction.GetCurSel();
	m_combat.m_eSurprise = (eventSurpriseType)m_Surprise.GetCurSel();
	m_combat.autoApproach = m_AutoApproach;
	m_combat.outdoors = m_Outdoors;
	m_combat.noMonsterTreasure = m_NoTreasure;
	m_combat.partyNeverDies = m_PartyNeverDies;
  m_combat.partyNoExperience = m_PartyNoExperience;
	m_combat.noMagic = m_NoMagic;
	m_combat.monsterMorale = m_Morale;
	m_combat.turningMod = (eventTurnUndeadModType)m_TurnUndead.GetCurSel();
	m_combat.text = m_Text;
  m_combat.randomMonster = m_RandomMonster;

	CDialog::OnOK();
}

BOOL CCombat::OnInitDialog() 
{
	int i;
	CDialog::OnInitDialog();

	m_TurnUndead.ResetContent();
	m_Surprise.ResetContent();
	m_Distance.ResetContent();
	m_Direction.ResetContent();
	
	for (i=0;i<NUM_TURN_MOD_TYPES;i++)
		m_TurnUndead.AddString(TurnModText[i]);
	for (i=0;i<NUM_SURPRISE_TYPES;i++)
		m_Surprise.AddString(SurpriseText[i]);
	for (i=0;i<NUM_DIST_TYPES;i++)
		m_Distance.AddString(DistanceText[i]);
	for (i=0;i<NUM_DIR_TYPES;i++)
		m_Direction.AddString(DirectionText[i]);

	m_TurnUndead.SetCurSel((int)m_combat.turningMod);
	m_Surprise.SetCurSel((int)m_combat.m_eSurprise);
	m_Distance.SetCurSel((int)m_combat.distance);
	m_Direction.SetCurSel((int)m_combat.direction);
	m_AutoApproach = m_combat.autoApproach;
	m_NoMagic = m_combat.noMagic;
	m_Morale = m_combat.monsterMorale;
	m_NoTreasure = m_combat.noMonsterTreasure;
	m_Outdoors = m_combat.outdoors;
	m_PartyNeverDies = m_combat.partyNeverDies;
  m_PartyNoExperience = m_combat.partyNoExperience;
	m_Text = m_combat.text;
  m_RandomMonster = m_combat.randomMonster;

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_combat.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_SeeMonster.SetWindowText(name);

  getBaseName(m_combat.MoveSound, name, MAX_MEDITBUTTON_TEXT);
  m_MoveSound.SetWindowText(name);
 
  getBaseName(m_combat.DeathSound, name, MAX_MEDITBUTTON_TEXT);
  m_DeathSound.SetWindowText(name);
  
  getBaseName(m_combat.TurnUndeadSound, name, MAX_MEDITBUTTON_TEXT);
  m_TurnUndeadSound.SetWindowText(name);

  GotoDlgCtrl(GetDlgItem(IDC_TEXT));

	UpdateData(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCombat::GetCombatData(COMBAT_EVENT_DATA& combat)
{
	combat = m_combat;
}

void CCombat::OnDeathsound() 
{
  CSoundChooser dlg(m_combat.DeathSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_combat.DeathSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_combat.DeathSound, name, MAX_MEDITBUTTON_TEXT);
    m_DeathSound.SetWindowText(name);
  }
}

void CCombat::OnMovesound() 
{
  CSoundChooser dlg(m_combat.MoveSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_combat.MoveSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_combat.MoveSound, name, MAX_MEDITBUTTON_TEXT);
    m_MoveSound.SetWindowText(name);
  }
}

void CCombat::OnTurnundeadsound() 
{
  CSoundChooser dlg(m_combat.TurnUndeadSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_combat.TurnUndeadSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_combat.TurnUndeadSound, name, MAX_MEDITBUTTON_TEXT);
    m_TurnUndeadSound.SetWindowText(name);
  }	
}

void CCombat::OnBgsounds()
{	
  UpdateData(TRUE);
  SOUND_EVENT tempEvent;
  POSITION pos = m_combat.bgSounds.backgroundSounds.sounds.GetHeadPosition();
  while (pos != NULL)
  {
    tempEvent.sounds.AddTail(m_combat.bgSounds.backgroundSounds.sounds.GetAt(pos));
    m_combat.bgSounds.backgroundSounds.sounds.GetNext(pos);
  }

  CSoundEventDlg dlg(tempEvent);
  if (dlg.DoModal()==IDOK)
  {
    m_combat.bgSounds.backgroundSounds.Clear();
    dlg.GetData(tempEvent);

    pos = tempEvent.sounds.GetHeadPosition();
    while (pos != NULL)
    {
      m_combat.bgSounds.backgroundSounds.sounds.AddTail(tempEvent.sounds.GetAt(pos));
      tempEvent.sounds.GetNext(pos);
    }
  }
  UpdateData(FALSE);
}

void CCombat::OnCombatEventQuestion()
{
  CHelp dlg("COMBAT_EVENT");
  dlg.DoModal();
}
