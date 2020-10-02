/******************************************************************************
* Filename: SpellAttribute.cpp
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
#include "RunTimeIF.h"
#include "Char.h"
#include "GameRules.h"
#include "EditText.h"
#include "SpellAttribute.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//static const CString DefaultActivation("$RETURN true;");
static CFont Font;

static GPDLCOMP gpdlCompSpellAttribEdit;
static double LastDebugRoll=0.0;

/////////////////////////////////////////////////////////////////////////////
// CSpellAttribute dialog


CSpellAttribute::CSpellAttribute(SPELL_EFFECTS_DATA &data, CWnd* pParent /*=NULL*/)
	: CDialog(CSpellAttribute::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpellAttribute)
	m_AffectTarget = TRUE;
	m_AffectTargeter = FALSE;
	m_ChangeText = _T("");
	m_ChangeType = 0;
	m_IsCumulative = FALSE;
	//}}AFX_DATA_INIT
  m_data = data;
  LastDebugRoll=0.0;
}

CSpellAttribute::~CSpellAttribute()
{
  Font.DeleteObject();
}

void CSpellAttribute::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpellAttribute)
	DDX_Control(pDX, IDC_ATTNAME, m_AttNameList);
	DDX_Check(pDX, IDC_AFFECTSTARGET, m_AffectTarget);
	DDX_Check(pDX, IDC_AFFECTSTARGETER, m_AffectTargeter);
	DDX_Text(pDX, IDC_CHANGEMADETEXT, m_ChangeText);
	DDX_Radio(pDX, IDC_CHANGEDELTA, m_ChangeType);
	DDX_Check(pDX, IDC_CUMULATIVE, m_IsCumulative);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpellAttribute, CDialog)
	//{{AFX_MSG_MAP(CSpellAttribute)
	ON_BN_CLICKED(IDC_ROLLDICE, OnRolldice)
	ON_BN_CLICKED(IDC_EDITSCRIPTS, OnEditscripts)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpellAttribute message handlers

BOOL CSpellAttribute::OnInitDialog() 
{
	CDialog::OnInitDialog();

#ifndef _DEBUG
  CWnd *pDebugWnd = GetDlgItem(IDC_ROLLDICE);
  if (pDebugWnd!=NULL) 
  {
    pDebugWnd->EnableWindow(FALSE);
    pDebugWnd->ShowWindow(SW_HIDE);
  }

#endif

  Font.CreatePointFont(100,"Courier",NULL);
  CWnd *pWnd = GetDlgItem(IDC_SCRIPT);
  if (pWnd!=NULL)
    pWnd->SetFont(&Font);
  pWnd = GetDlgItem(IDC_SCRIPT2);
  if (pWnd!=NULL)
    pWnd->SetFont(&Font);

  m_ChangeText = m_data.changeData.EncodeText();;  
  m_IsCumulative = ((m_data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE) > 0);
  if (m_data.flags & SPELL_EFFECTS_DATA::EFFECT_PCNT) 
    m_ChangeType = 1;
  else if (m_data.flags & SPELL_EFFECTS_DATA::EFFECT_ABS)
    m_ChangeType = 2;
  else if (m_data.flags & SPELL_EFFECTS_DATA::EFFECT_REMOVEALL)
    m_ChangeType = 3;
  else 
    m_ChangeType = 0; // delta

  m_AffectTarget = ((m_data.flags & SPELL_EFFECTS_DATA::EFFECT_TARGET) > 0);
  m_AffectTargeter = ((m_data.flags & SPELL_EFFECTS_DATA::EFFECT_TARGETER) > 0);

  m_AttNameList.ResetContent();

  int i, sel=-1;
  char key[50];

  for (i=FIRST_CHAR_KEYWORD;i<=LAST_CHAR_KEYWORD;i++)
  {
    if (!IsDataReadOnly(i))
    {
      GetDataText(i, key);
      int index = m_AttNameList.AddString(key);
      m_AttNameList.SetItemData(index, i);
    }
  }
  for (i=FIRST_PARTY_KEYWORD;i<=LAST_PARTY_KEYWORD;i++)
  {
    if (!IsDataReadOnly(i))
    {
      GetDataText(i, key);
      int index = m_AttNameList.AddString(key);
      m_AttNameList.SetItemData(index, i);
    }
  }
  for (i=FIRST_GAME_KEYWORD;i<=LAST_GAME_KEYWORD;i++)
  {
    if (!IsDataReadOnly(i))
    {
      GetDataText(i, key);
      int index = m_AttNameList.AddString(key);
      m_AttNameList.SetItemData(index, i);
    }
  }

  if (!m_data.AffectedAttr().IsEmpty())
  {
    CString tmp;
    for (i=0;i<m_AttNameList.GetCount();i++)
    {
      m_AttNameList.GetLBText(i, tmp);
      if (stricmp(tmp, m_data.AffectedAttr())==0)
      {
        sel = i;
        break;
      }
    }

    if (sel == -1)
    {
      // could not find affected attribute in list of available attributes
      CString msg;
      msg.Format("Attribute \'%s\' is not correct or is read-only\n", m_data.AffectedAttr());
      MsgBoxError(msg,"Spell Attribute Error");
    }
  }

  sel = max(0, sel);
  m_AttNameList.SetCurSel(sel);

  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpellAttribute::OnOK() 
{
	UpdateData(TRUE);
    
  CString msg;
  if (!m_data.changeData.DecodeText(m_ChangeText, msg))
    WriteDebugString("SpellAttribute DicePlus error: %s\n", msg);

  if (m_IsCumulative) 
    m_data.flags |= SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE;
  else
    m_data.flags &= ~SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE;

  if (m_ChangeType==1) // pcnt
  {
    m_data.flags &= ~(SPELL_EFFECTS_DATA::EFFECT_ABS|SPELL_EFFECTS_DATA::EFFECT_REMOVEALL);
    m_data.flags |= SPELL_EFFECTS_DATA::EFFECT_PCNT;
  }
  else if (m_ChangeType==2) // absolute
  {
    m_data.flags &= ~(SPELL_EFFECTS_DATA::EFFECT_PCNT|SPELL_EFFECTS_DATA::EFFECT_REMOVEALL);
    m_data.flags |= SPELL_EFFECTS_DATA::EFFECT_ABS;
  }
  else if (m_ChangeType==3) // remove all
  {
    m_data.flags &= ~(SPELL_EFFECTS_DATA::EFFECT_PCNT|SPELL_EFFECTS_DATA::EFFECT_ABS);
    m_data.flags |= SPELL_EFFECTS_DATA::EFFECT_REMOVEALL;
  }
  else // delta
  {
    m_data.flags &= 
      ~(SPELL_EFFECTS_DATA::EFFECT_PCNT|SPELL_EFFECTS_DATA::EFFECT_ABS|SPELL_EFFECTS_DATA::EFFECT_REMOVEALL);
  }  

  if (m_AffectTarget) 
    m_data.flags |= SPELL_EFFECTS_DATA::EFFECT_TARGET;
  else
    m_data.flags &= ~SPELL_EFFECTS_DATA::EFFECT_TARGET;

  if (m_AffectTargeter) 
    m_data.flags |= SPELL_EFFECTS_DATA::EFFECT_TARGETER;
  else
    m_data.flags &= ~SPELL_EFFECTS_DATA::EFFECT_TARGETER;

  m_data.flags |= SPELL_EFFECTS_DATA::EFFECT_SPELL;

  int curr = m_AttNameList.GetCurSel();
  int varindex = m_AttNameList.GetItemData(curr);
  char keytext[50];
  GetDataText(varindex, keytext);
  m_data.AffectedAttr(keytext);     

	CDialog::OnOK();
}

void CSpellAttribute::OnRolldice() 
{
  UpdateData(TRUE);

  LastDebugRoll = 0.0;

  CString msg;
  if (!m_data.changeData.DecodeText(m_ChangeText, msg))
  {    
#ifdef _DEBUG
    MsgBoxInfo(msg, "Error Parsing DicePlus Expression");
#endif
    return;
  }

  int WhatLevel = 1;
  DWORD StartExp = 1+GetMinFighterExpForLevel(WhatLevel);

  CHARACTER tmpDude;
  ActorType actor;
  tmpDude.SetAlignment(ChaoticNeutral);
  //tmpDude.race(Human);
  tmpDude.RaceID(raceData.PeekRace(0)->RaceID());
  tmpDude.SetGender(Male);
  //tmpDude.SetClass(Fighter);
  tmpDude.SetClass(classData.PeekClass(0)->ClassID());
  tmpDude.generateNewCharacter(StartExp,0);
  tmpDude.SetName("TmpScriptCheck");
  tmpDude.SetType(CHAR_TYPE);
  // This really does no good since there is no party defined when
  // using the editor. However, some sort of valid source needs
  // to be specified. The result is that lookups for character
  // data will return 0. That includes all data not directly
  // contained in the ActorType itself (ie: abilities, gender).
  tmpDude.SetPartyMember(); 

  if (tmpDude.GetMaxHitPoints() == 0)
  {
    MsgBoxError("Failed to create character (used for running dice script)\nRace/Class configuration files may have an error");
    return;
  }

  //tmpDude.GetContext(&actor, FighterFlag);
  tmpDude.GetContext(&actor);
  SetCharContext(&actor);
  SetTargetContext(&actor);

  double result=0.0;
  m_data.changeData.Roll(result);
  LastDebugRoll = result;

#ifdef _DEBUG
  msg.Format("For human/male/fighter/level %i, dice result is %f", WhatLevel, result);
  MsgBoxInfo(msg);
#endif

  ClearTargetContext();
  ClearCharContext();
}

void CSpellAttribute::OnEditscripts() 
{
  CString start("$PUBLIC $FUNC spelltest() { ");
  CString end(" } spelltest;");
  CString func = "spelltest";

  ScriptList list;
  list.SetSize(2);
  list[0].name = "Activation Script (must return 0,false or 1,true)";
  list[0].script = m_data.ActivationScript();

  list[1].name = "Modification Script (modifies \'change by\' value";
  list[1].script = m_data.ModificationScript();

  //list[2].name = "Saving Throw Script";
  //list[2].script = m_data.SavingThrowScript();

  //list[3].name = "Saving Throw Failed Script";
  //list[3].script = m_data.SavingThrowFailedScript();

  //list[4].name = "Saving Throw Succeeded Script";
  //list[4].script = m_data.SavingThrowSucceededScript();

  CEditText dlg(start, end, func, true, list);
  dlg.m_ScriptFilter = SPELL_USAGE;
  dlg.m_helpKey = "SPELL-EFFECT-SCRIPTS";

  if (dlg.DoModal()==IDOK)
  {
    m_data.ActivationScript          (dlg.m_list[0].script);
    m_data.ModificationScript        (dlg.m_list[1].script);
    //m_data.SavingThrowScript         (dlg.m_list[2].script);
    //m_data.SavingThrowFailedScript   (dlg.m_list[3].script);
    //m_data.SavingThrowSucceededScript(dlg.m_list[4].script);
  }	
}
