/******************************************************************************
* Filename: SpellDBDlgEx.cpp
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
#include "SoundChooser.h"
#include "SmallPicDlg.h"
#include "ChooseSpeclAbDlg.h"
#include "SpellAttribute.h"
#include "EditAttributes.h"
#include "EditText.h"
#include "..\shared\GpdlComp.h"
#include "SpellDBDlgEx.h"
#include "ClassDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpellDBDlgEx dialog


//*****************************************************************************
//    NAME: CSpellDBDlgEx::CSpellDBDlgEx
//
// PURPOSE: 
//
//*****************************************************************************
CSpellDBDlgEx::CSpellDBDlgEx(SPELL_DATA &data, CWnd* pParent /*=NULL*/)
	: CDialog(CSpellDBDlgEx::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpellDBDlgEx)
	m_Price = 0;
  m_priority = 500;
	m_CastTime = 0;
	m_AllowedInCamp = TRUE;
  m_AllowedInCombat = TRUE;
	m_DurationText = _T("");
	m_canTargetFriend = FALSE;
  m_canTargetEnemy = TRUE;
	m_Level = 1;
	m_Name = _T("New Spell");
    m_Description = _T("");
	//m_NumTargetText = _T("");
	m_TargetP1Text = _T("");
	//m_TargetRangeText = _T("");
	m_TargetP2Text = _T("");
	m_TargetP3Text = _T("");
	m_TargetP4Text = _T("");
	m_TargetP5Text = _T("");
	m_CoinName = _T("");
	//m_UsableByParty = TRUE;
	m_CanMemorize = TRUE;
	//m_CanScribeAtLevelChange = FALSE;
	m_AllowScribing = FALSE;
	m_IsCumulative = FALSE;
	m_CastMsg = _T("");
	//m_TargRangeHelp = _T("");
	//m_NumTargHelp = _T("");
	//m_TargNumberLabel = _T("");
	//m_TargRangeLabel = _T("");
	m_TargP2Label = _T("");
	m_TargP2Label = _T("");
	m_TargP3Label = _T("");
	m_TargP4Label = _T("");
	m_TargP5Label = _T("");
	m_Lingers = FALSE;
	m_LingerOnceOnly = FALSE;
	m_EffectDurationText = _T("");
	m_CanBeDispelled = FALSE;
	//m_AutoScribeAtLevelChange = FALSE;
	m_AutoScribe = FALSE;
	//m_MagicUser = FALSE;
	//m_Fighter = FALSE;
	//m_Druid = FALSE;
	//m_Cleric = FALSE;
	//m_Paladin = FALSE;
	//m_Ranger = FALSE;
	//m_Thief = FALSE;
	//}}AFX_DATA_INIT
  m_SpellData = data;
  //if (m_SpellData.schoolMask & ClericFlag)
  //{
  //  m_SpellData.CanScribeAtLevelChange=TRUE;
  //  m_SpellData.AutoScribeAtLevelChange=TRUE;
  //}
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::EnableCheckBox
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::EnableCheckBox(int id)
{
  CWnd *pWnd = GetDlgItem(id);
  if (pWnd != NULL) 
  {
    pWnd->EnableWindow(TRUE);
  }
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::DisableCheckBox
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::DisableCheckBox(int id)
{
  CWnd *pWnd = GetDlgItem(id);
  if (pWnd != NULL) 
  {
    pWnd->EnableWindow(FALSE);
  }
}


//*****************************************************************************
//    NAME: CSpellDBDlgEx::DoDataExchange
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::DoDataExchange(CDataExchange* pDX)
{
  CWnd *pWnd;
  BOOL enable;
  int type;
  CString lingerText;
  CString lingerEffectsText;
  char tempEffect[] = "Temporary Effect for the Duration of Spell\n(Else Effect Repeated Each Round)";
  
  if (!pDX->m_bSaveAndValidate)
  {
    m_CastMsg        = m_SpellData.CastMsg;
    m_CastTime       = m_SpellData.Casting_Time;
    //m_CombatOnly   = m_SpellData.Combat_Only;
    //m_AllowedInCombat= !m_SpellData.NotInCombat();
    m_AllowedInCombat= m_SpellData.InCombat();
    //m_AllowedInCamp  = !m_SpellData.NotInCamp();
    m_AllowedInCamp  = m_SpellData.InCamp();
    m_CanBeDispelled = m_SpellData.CanBeDispelled;
    //m_UsableByParty  = m_SpellData.UsableByParty;
    m_CanMemorize    = m_SpellData.CanMemorize;
    m_canTargetFriend= m_SpellData.CanTargetFriend;
    m_canTargetEnemy= m_SpellData.CanTargetEnemy;
    m_IsCumulative   = m_SpellData.IsCumulative;
    m_Level          = m_SpellData.Level;
    m_Price          = m_SpellData.Cast_Cost;
    m_priority       = m_SpellData.Cast_Priority;
    m_Name           = m_SpellData.Name;
    m_Lingers        = m_SpellData.Lingers;
    m_LingerOnceOnly = m_SpellData.LingerOnceOnly;
    m_Description     = m_SpellData.Description;
    if (m_Lingers) m_AllowedInCamp = FALSE;
    
    if (m_Name.GetLength() > MAX_SPELL_NAME) m_Name.SetAt(MAX_SPELL_NAME,'\0');
    //if (m_SpellData.schoolMask & ClericFlag)
    //{
    //  m_SpellData.CanScribeAtLevelChange=TRUE;
    //  m_SpellData.AutoScribeAtLevelChange=TRUE;
    //}
    //m_CanScribeAtLevelChange = m_SpellData.CanScribeAtLevelChange;
    m_AllowScribing = m_SpellData.AllowScribe;
    //m_AutoScribeAtLevelChange = m_SpellData.AutoScribeAtLevelChange;
    m_AutoScribe = m_SpellData.AutoScribe;
    
    if (m_DurationRate.m_hWnd != NULL)
      m_DurationRate.SetCurSel((int)m_SpellData.Duration_Rate);
    if (m_TargetingType.m_hWnd != NULL)
      m_TargetingType.SetCurSel((int)m_SpellData.Targeting);   
    if (m_SaveVersus.m_hWnd != NULL)
      m_SaveVersus.SetCurSel((int)m_SpellData.Save_Vs);


    {
      bool show;
    if (m_SaveResult.m_hWnd != NULL)
      m_SaveResult.SetCurSel((int)m_SpellData.Save_Result);
      show =    (m_SpellData.Save_Result != NoSave)
             && (m_SpellData.Save_Result != UseTHAC0);
      pWnd = GetDlgItem(IDC_SAVEVERSUS);
      if (pWnd != NULL) pWnd->EnableWindow(show);
      pWnd = GetDlgItem(IDC_SaveVsLabel);
      if (pWnd != NULL) pWnd->EnableWindow(show);
    }


    if (m_Class.m_hWnd != NULL)
      //m_Class.SetCurSel(FlagToIndex(m_SpellData.schoolMask));
    {
      SCHOOL_ID schoolID;
      schoolID = m_SpellData.schoolID;
      int indx;
      for (indx=0; indx<m_schools.GetSize(); indx++)
      {
        if (schoolID.IsEmpty())
        {
          m_SpellData.schoolID = m_schools[indx];
          schoolID = m_schools[indx];
        };
        if (m_schools[indx] == schoolID) break;
      };
      m_Class.SetCurSel(indx);
    };
    if (m_CastTimeType.m_hWnd != NULL)
      m_CastTimeType.SetCurSel((int)m_SpellData.Casting_Time_Type);
    
    //if (m_SpellData.schoolMask & MagicUserFlag)
    {
      pWnd = GetDlgItem(IDC_GIVETOMU);
      if (pWnd != NULL) pWnd->EnableWindow(TRUE);
      pWnd = GetDlgItem(IDC_AUTOSCRIBEATLEVELCHANGE);
      if (pWnd != NULL) pWnd->EnableWindow(TRUE);
    }
    //else
    //{
    //  pWnd = GetDlgItem(IDC_GIVETOMU);
    //  if (pWnd != NULL) pWnd->EnableWindow(FALSE);
    //  pWnd = GetDlgItem(IDC_AUTOSCRIBEATLEVELCHANGE);
    //  if (pWnd != NULL) pWnd->EnableWindow(FALSE);
    //}
    
    if (m_SpellData.Duration_Rate == Permanent)
      enable = FALSE;
    else
      enable = TRUE;      
    
    pWnd = GetDlgItem(IDC_DURATIONTEXT);
    if (pWnd != NULL) pWnd->EnableWindow(enable);
    
    if (   (m_SpellData.Targeting == Self) 
      || (m_SpellData.Targeting == WholeParty) 
      || (m_SpellData.Targeting == TouchedTargets))
      enable = FALSE;
    else
      enable = TRUE;
    
    pWnd = GetDlgItem(IDC_TARGETRANGETEXT);      
    if (pWnd != NULL) pWnd->EnableWindow(enable);
    
    BOOL EnableLinger = TRUE;
    lingerText = "Lingers on Map";
    lingerEffectsText = "Affects targets once\n(Else once per round)";
    switch (m_SpellData.Targeting)
    {
    case Self:
      enable = FALSE;
      //m_SpellData.NumTargets.DecodeText("1");
      //m_SpellData.NumTargets().DecodeText("1");
      //m_NumTargHelp="Self Only";
      //m_TargNumberLabel = "Self";
      m_TargP1Label.Empty();
      lingerText = "Lingers on Targets";
      lingerEffectsText = tempEffect;
      //EnableLinger = FALSE;
      break;
    case SelectedByCount:
      enable = TRUE;
      //m_NumTargHelp="Max Selectable";
      //m_TargNumberLabel = "Quantity";
      m_TargP1Label = "Quantity";
      //EnableLinger = TRUE;
      lingerText = "Lingers on Targets";
      lingerEffectsText = tempEffect;
      break;
    case WholeParty:
      enable = FALSE;
      //m_SpellData.NumTargets.DecodeText("0");
      m_TargP1Label.Empty();
      m_SpellData.P1.DecodeText("0");
      //m_NumTargHelp="All Party Members";
      //m_TargNumberLabel = "Party";
      //m_TargNumberLabel = "Party";
      lingerText = "Lingers on Targets";
      lingerEffectsText = tempEffect;
      //EnableLinger = FALSE;
      break;
    case TouchedTargets:
      enable = TRUE;
      //m_NumTargHelp="Max Touched";
      //m_TargNumberLabel = "Quantity";
      m_TargP1Label = "Quantity";
      lingerText = "Lingers on Targets";
      lingerEffectsText = tempEffect;
      //EnableLinger = FALSE;
      break;
    case AreaCircle:
      enable = TRUE;
      //m_NumTargHelp="Max Targs Within Radius";
      //m_TargNumberLabel = "Quantity";
      m_TargP1Label = "Quantity";
      break;
    case AreaLinePickStart:
      enable = TRUE;
      //m_SpellData.NumTargets.DecodeText("1");
      m_TargP1Label = "Width";
      //m_SpellData.P1.DecodeText("1");
      //m_NumTargHelp="Line Width";
      //m_TargNumberLabel = "Width";
      break;
    case AreaLinePickEnd:
      enable = TRUE;
      //m_SpellData.NumTargets.DecodeText("1");
      m_TargP1Label = "Width";
      //m_SpellData.P1.DecodeText("1");
      //m_NumTargHelp="Line Width";
      //m_TargNumberLabel = "Width";
      break;
    case AreaSquare:
      enable = TRUE;
      //m_NumTargHelp="Square Width";
      m_TargP1Label = "Width";
      //m_TargNumberLabel = "Width";
      break;
    case AreaCone:
      enable = TRUE;
      //m_NumTargHelp="Width of Cone at End";
      //m_TargNumberLabel = "Width";
      m_TargP1Label = "Width";
      break;
    case SelectByHitDice:
      enable = TRUE;
      //m_NumTargHelp="Max Hit Dice";
      //m_TargNumberLabel = "Number";
      m_TargP1Label = "Number";
      lingerText = "Lingers on Targets";
      lingerEffectsText = tempEffect;
      //EnableLinger = FALSE;
      break;
    }

    pWnd = GetDlgItem(IDC_TARG_P1_LABEL);
    if (pWnd != NULL) pWnd->EnableWindow(enable);
    pWnd = GetDlgItem(IDC_TARG_P1_TEXT);
    if (pWnd != NULL) pWnd->EnableWindow(enable);
    
    if (EnableLinger)
    {
      switch (m_SpellData.Duration_Rate)
      {
      case inRounds:
      case inHours:
      case inDays:
        // can use linger
        break;
      default:
        EnableLinger = FALSE; // cannot use linger
        break;
      }
    }
    else
    {
      m_Lingers = FALSE;
      m_LingerOnceOnly = FALSE;
      m_SpellData.Lingers=FALSE;
      m_SpellData.LingerOnceOnly=FALSE;
      m_SpellData.EffectDuration.Clear();
      m_EffectDurationText = "";
    }

    pWnd = GetDlgItem(IDC_LINGERS);
    if (pWnd != NULL) 
    {
      pWnd->EnableWindow(EnableLinger);    
      pWnd->SetWindowText(lingerText);
    };
    
    if (!m_Lingers) // disable these two unless linger option is turned on
      EnableLinger=FALSE;
    
    pWnd = GetDlgItem(IDC_LINGERAFFECTSONCEONLY);
    if (pWnd != NULL) pWnd->SetWindowText(lingerEffectsText);
    if (pWnd != NULL) pWnd->ShowWindow(EnableLinger);    

    pWnd = GetDlgItem(IDC_EFFECTDURATIONTEXT);
    if (pWnd != NULL) pWnd->ShowWindow(EnableLinger);
    
    pWnd = GetDlgItem(IDC_DurationText1);
    if (pWnd != NULL) pWnd->ShowWindow(EnableLinger);
    
    pWnd = GetDlgItem(IDC_DurationText2);
    if (pWnd != NULL) pWnd->ShowWindow(EnableLinger);
    
    switch (m_SpellData.Targeting)
    {
    case Self:
      enable   = FALSE;
      //m_TargRangeHelp="Range 0";
      //m_TargRangeLabel = "Self";
      m_TargP2Label.Empty();
      //m_SpellData.TargetRange.DecodeText("0");
      //m_SpellData.TargetRange().DecodeText("0");
      break;
    case SelectedByCount:
      enable = FALSE;
      //m_TargRangeHelp="Max Range From Caster";
      //m_TargRangeLabel = "Range";
      m_TargP2Label.Empty();
      break;
    case WholeParty:
      enable = FALSE;
      //m_SpellData.TargetRange.DecodeText("0");
      //m_SpellData.TargetRange().DecodeText("0");
      //m_TargRangeHelp="Works At Any Range";
      //m_TargRangeLabel = "Party";
      m_TargP2Label.Empty();
      break;
    case TouchedTargets:
      enable = FALSE;
      //m_TargRangeHelp="Range 1";
      //m_SpellData.TargetRange.DecodeText("1");
      //m_SpellData.TargetRange().DecodeText("1");
      //m_TargRangeLabel = "Touch";
      m_TargP2Label.Empty();
      break;
    case AreaCircle:
      enable = TRUE;
      //m_TargRangeHelp="Circle Radius";
      //m_TargRangeLabel = "Radius";
      m_TargP2Label = "Radius";
      break;
    case AreaLinePickStart:
      enable = TRUE;
      //m_TargRangeHelp="Line Length";
      //m_TargRangeLabel = "Length";
      m_TargP2Label = "Length";
      break;
    case AreaLinePickEnd:
      enable = TRUE;
      //m_TargRangeHelp="Line Length";
      //m_TargRangeLabel = "Length";
      m_TargP2Label = "Length";
      break;
    case AreaSquare:
      enable = TRUE;
      //m_TargRangeHelp="Square Height";
      //m_TargRangeLabel = "Height";
      m_TargP2Label = "Height";
      break;
    case AreaCone:
      enable = TRUE;
      //m_TargRangeHelp="Cone Length";
      //m_TargRangeLabel = "Length";
      m_TargP2Label = "Length";
      break;
    case SelectByHitDice:
      enable = FALSE;
      //m_TargRangeHelp="Max Range From Caster";
      //m_TargRangeLabel = "Range";
      m_TargP2Label.Empty();
      break;
    }
    
    //pWnd = GetDlgItem(IDC_TARGETRANGETEXT);
    pWnd = GetDlgItem(IDC_TARG_P2_LABEL);
    if (pWnd != NULL) pWnd->EnableWindow(enable);
    pWnd = GetDlgItem(IDC_TARG_P2_TEXT);
    if (pWnd != NULL) pWnd->EnableWindow(enable);
    
    //m_NumTargetText = m_SpellData.NumTargets.EncodeText();
    //m_NumTargetText = m_SpellData.NumTargets().EncodeText();
    m_TargetP1Text = m_SpellData.P1.EncodeText();
    //m_TargetRangeText = m_SpellData.TargetRange.EncodeText();
    //m_TargetRangeText = m_SpellData.TargetRange().EncodeText();
    m_TargetP2Text = m_SpellData.P2.EncodeText();
    m_TargetP3Text = m_SpellData.P3.EncodeText();
    m_TargetP4Text = m_SpellData.P4.EncodeText();
    m_TargetP5Text = m_SpellData.P5.EncodeText();
    m_DurationText = m_SpellData.Duration.EncodeText();
    m_EffectDurationText = m_SpellData.EffectDuration.EncodeText();


    switch (m_SpellData.Targeting)
    {
    case Self:
      enable = FALSE;
      m_TargP3Label.Empty();
      break;
    case SelectedByCount:
      enable = TRUE;
      m_TargP3Label = "Range";
      break;
    case WholeParty:
      enable = FALSE;
      m_TargP3Label.Empty();
      break;
    case TouchedTargets:
      enable = FALSE;
      m_TargP3Label.Empty();
      break;
    case AreaCircle:
      enable = TRUE;
      m_TargP3Label = "Range" ;
      break;
    case AreaLinePickStart:
      enable = TRUE;
      m_TargP3Label = "Range";
      break;
    case AreaLinePickEnd:
      enable = TRUE;
      m_TargP3Label = "Range";
      break;
    case AreaSquare:
      enable = TRUE;
      m_TargP3Label = "Range";
      m_TargP2Label = "Height";
      break;
    case AreaCone:
      enable = TRUE;
      m_TargP3Label = "Range";
      break;
    case SelectByHitDice:
      enable = TRUE;
      m_TargP3Label = "Range";
      break;
    };
    pWnd = GetDlgItem(IDC_TARG_P3_LABEL);
    if (pWnd != NULL) pWnd->EnableWindow(enable);
    pWnd = GetDlgItem(IDC_TARG_P3_TEXT);
    if (pWnd != NULL) pWnd->EnableWindow(enable);
    

    switch (m_SpellData.Targeting)
    {
    case Self:
      enable = FALSE;
      m_TargP4Label.Empty();
      break;
    case SelectedByCount:
      enable = FALSE;
      m_TargP4Label.Empty();
      break;
    case WholeParty:
      enable = FALSE;
      m_TargP4Label.Empty();
      break;
    case TouchedTargets:
      enable = FALSE;
      m_TargP4Label.Empty();
      break;
    case AreaCircle:
      enable = FALSE;
      m_TargP4Label.Empty();
      break;
    case AreaLinePickStart:
      enable = FALSE;
      m_TargP4Label.Empty();
      break;
    case AreaLinePickEnd:
      enable = FALSE;
      m_TargP4Label.Empty();
      break;
    case AreaSquare:
      enable = FALSE;
      m_TargP4Label.Empty();
      break;
    case AreaCone:
      enable = FALSE;
      m_TargP4Label.Empty();
      break;
    case SelectByHitDice:
      enable = FALSE;
      m_TargP4Label.Empty();
      break;
    };
    pWnd = GetDlgItem(IDC_TARG_P4_LABEL);
    if (pWnd != NULL) pWnd->EnableWindow(enable);
    pWnd = GetDlgItem(IDC_TARG_P4_TEXT);
    if (pWnd != NULL) pWnd->EnableWindow(enable);


    switch (m_SpellData.Targeting)
    {
    case Self:
      enable = FALSE;
      m_TargP5Label.Empty();
      break;
    case SelectedByCount:
      enable = FALSE;
      m_TargP5Label.Empty();
      break;
    case WholeParty:
      enable = FALSE;
      m_TargP5Label.Empty();
      break;
    case TouchedTargets:
      enable = FALSE;
      m_TargP5Label.Empty();
      break;
    case AreaCircle:
      enable = FALSE;
      m_TargP5Label.Empty();
      break;
    case AreaLinePickStart:
      enable = FALSE;
      m_TargP5Label.Empty();
      break;
    case AreaLinePickEnd:
      enable = FALSE;
      m_TargP5Label.Empty();
      break;
    case AreaSquare:
      enable = FALSE;
      m_TargP5Label.Empty();
      break;
    case AreaCone:
      enable = FALSE;
      m_TargP5Label.Empty();
      break;
    case SelectByHitDice:
      enable = FALSE;
      m_TargP5Label.Empty();
      break;
    };
    pWnd = GetDlgItem(IDC_TARG_P5_LABEL);
    if (pWnd != NULL) pWnd->EnableWindow(enable);
    pWnd = GetDlgItem(IDC_TARG_P5_TEXT);
    if (pWnd != NULL) pWnd->EnableWindow(enable);

  
}
  
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSpellDBDlgEx)
  DDX_Control(pDX, IDC_COVERAGESOUND, m_CoverageSound);
  DDX_Control(pDX, IDC_INROUTESOUND, m_InRouteSound);
  DDX_Control(pDX, IDC_SPELLLINGERSOUND, m_LingerSound);
  DDX_Control(pDX, IDC_SPELLHITSOUND, m_HitSound);
  DDX_Control(pDX, IDC_CASTANIM, m_CastAnim);
  DDX_Control(pDX, IDC_CASTTIMETYPE, m_CastTimeType);
  DDX_Control(pDX, IDC_DURATIONRATE, m_DurationRate);
  DDX_Control(pDX, IDC_TARGETINGTYPE, m_TargetingType);
  DDX_Control(pDX, IDC_TARGCOVANIM, m_TargetCovAnim);
  DDX_Control(pDX, IDC_SPELLSOUND, m_Sound);
  DDX_Control(pDX, IDC_SAVEVERSUS, m_SaveVersus);
  DDX_Control(pDX, IDC_SAVERESULT, m_SaveResult);
  DDX_Control(pDX, IDC_LINGERANIM, m_LingerAnim);
  DDX_Control(pDX, IDC_INROUTEANIM, m_InRouteAnim);
  DDX_Control(pDX, IDC_HITANIM, m_HitAnim);
  DDX_Control(pDX, IDC_CLASS, m_Class);
  DDX_Control(pDX, IDC_ATTRIBUTELIST, m_AttList);
  DDX_Text(pDX, IDC_CASTCOST, m_Price);
	DDX_Text(pDX, IDC_PRIORITY, m_priority);
	DDV_MinMaxInt(pDX, m_priority, -0x7ffffff, 0x7fffffff);
  DDX_Text(pDX, IDC_CASTTIME, m_CastTime);
  DDX_Check(pDX, IDC_ALLOWEDINCAMP, m_AllowedInCamp);
  DDX_Check(pDX, IDC_ALLOWEDINCOMBAT, m_AllowedInCombat);
  DDX_Text(pDX, IDC_DURATIONTEXT, m_DurationText);
  DDX_Check(pDX, IDC_FRIENDLYFIREOK, m_canTargetFriend);
  DDX_Check(pDX, IDC_ENEMYFIREOK, m_canTargetEnemy);
  DDX_Text(pDX, IDC_LEVEL, m_Level);
  DDX_Text(pDX, IDC_NAME, m_Name);
  //DDX_Text(pDX, IDC_NUMTARGETTEXT, m_NumTargetText);
  DDX_Text(pDX, IDC_TARG_P1_TEXT, m_TargetP1Text);
  //DDX_Text(pDX, IDC_TARGETRANGETEXT, m_TargetRangeText);
  DDX_Text(pDX, IDC_TARG_P2_TEXT, m_TargetP2Text);
  DDX_Text(pDX, IDC_TARG_P3_TEXT, m_TargetP3Text);
  DDX_Text(pDX, IDC_TARG_P4_TEXT, m_TargetP4Text);
  DDX_Text(pDX, IDC_TARG_P5_TEXT, m_TargetP5Text);
  DDX_Text(pDX, IDC_COINNAME, m_CoinName);
  //DDX_Check(pDX, IDC_USABLEBYPLAYER, m_UsableByParty);
  DDX_Check(pDX, IDC_CANMEMORIZE, m_CanMemorize);
  //DDX_Check(pDX, IDC_GIVETOMU, m_CanScribeAtLevelChange);
  DDX_Check(pDX, IDC_GIVETOMU, m_AllowScribing);
  DDX_Check(pDX, IDC_ISCUMULATIVE, m_IsCumulative);
  DDX_Text(pDX, IDC_CASTMSG, m_CastMsg);
  //DDX_Text(pDX, IDC_TARGRANGEHELP, m_TargRangeHelp);
  //DDX_Text(pDX, IDC_NUMTARGETHELP, m_NumTargHelp);
  //DDX_Text(pDX, IDC_TARGNUMBERLABEL, m_TargNumberLabel);
  DDX_Text(pDX, IDC_TARG_P1_LABEL, m_TargP1Label);
  //DDX_Text(pDX, IDC_TARGRANGELABEL, m_TargRangeLabel);
  DDX_Text(pDX, IDC_TARG_P2_LABEL, m_TargP2Label);
  DDX_Text(pDX, IDC_TARG_P3_LABEL, m_TargP3Label);
  DDX_Text(pDX, IDC_TARG_P4_LABEL, m_TargP4Label);
  DDX_Text(pDX, IDC_TARG_P5_LABEL, m_TargP5Label);
  DDX_Check(pDX, IDC_LINGERS, m_Lingers);
  DDX_Check(pDX, IDC_LINGERAFFECTSONCEONLY, m_LingerOnceOnly);
	DDX_Text(pDX, IDC_EFFECTDURATIONTEXT, m_EffectDurationText);
	DDX_Check(pDX, IDC_CANBEDISPELLED, m_CanBeDispelled);
	//DDX_Check(pDX, IDC_AUTOSCRIBEATLEVELCHANGE, m_AutoScribeAtLevelChange);
	DDX_Check(pDX, IDC_AUTOSCRIBE, m_AutoScribe);
	//DDX_Check(pDX, IDC_MAGICUSER, m_MagicUser);
	//DDX_Check(pDX, IDC_FIGHTER, m_Fighter);
	//DDX_Check(pDX, IDC_DRUIDCHECK, m_Druid);
	//DDX_Check(pDX, IDC_CLERIC, m_Cleric);
	//DDX_Check(pDX, IDC_PALADIN, m_Paladin);
	//DDX_Check(pDX, IDC_RANGER, m_Ranger);
	//DDX_Check(pDX, IDC_THIEF, m_Thief);
	//}}AFX_DATA_MAP
    DDX_Text(pDX, IDC_DESC, m_Description);

  if (pDX->m_bSaveAndValidate)
  {
    m_SpellData.CastMsg = m_CastMsg;
    m_SpellData.Casting_Time = m_CastTime;
    //m_SpellData.Combat_Only = m_CombatOnly;
    //m_SpellData.NotInCombat(!m_AllowedInCombat);
    //m_SpellData.NotInCamp(!m_AllowedInCamp);
    m_SpellData.InCombat(m_AllowedInCombat);
    m_SpellData.InCamp(m_AllowedInCamp);
    m_SpellData.CanBeDispelled = m_CanBeDispelled;
    //m_SpellData.UsableByParty = m_UsableByParty;
    m_SpellData.CanMemorize = m_CanMemorize;
    m_SpellData.CanTargetFriend = m_canTargetFriend ;
    m_SpellData.CanTargetEnemy = m_canTargetEnemy ;
    m_SpellData.IsCumulative = m_IsCumulative;
    m_SpellData.Level = m_Level;
    if (m_Name.GetLength() > MAX_SPELL_NAME) m_Name.SetAt(MAX_SPELL_NAME,'\0');
    m_SpellData.Name = m_Name;
    m_SpellData.Description = m_Description;
    m_SpellData.Cast_Cost = m_Price;
    m_SpellData.Cast_Priority = m_priority;
    m_SpellData.Duration_Rate = (spellDurationType)m_DurationRate.GetCurSel();
    m_SpellData.Targeting = (spellTargetingType)m_TargetingType.GetCurSel();
    m_SpellData.Save_Vs = (spellSaveVersusType)m_SaveVersus.GetCurSel();
    m_SpellData.Save_Result = (spellSaveEffectType)m_SaveResult.GetCurSel();
    m_SpellData.Casting_Time_Type = (spellCastingTimeType)m_CastTimeType.GetCurSel();
    //m_SpellData.schoolMask = IndexToFlag(m_Class.GetCurSel());
    {
      int i, k;
      i = m_Class.GetCurSel();
      k = m_Class.GetItemData(i);
      m_SpellData.schoolID = m_schools[k];
    };
    //if (m_SpellData.schoolMask & ClericFlag)
    //{
    //  m_CanScribeAtLevelChange=TRUE;
    //  m_AutoScribeAtLevelChange=TRUE;
    //}
    //m_SpellData.CanScribeAtLevelChange = m_CanScribeAtLevelChange;    
    m_SpellData.AllowScribe = m_AllowScribing;    
    //m_SpellData.AutoScribeAtLevelChange = m_AutoScribeAtLevelChange;    
    m_SpellData.AutoScribe = m_AutoScribe;    
    
    m_SpellData.Lingers = m_Lingers;
    //if (m_SpellData.Lingers) m_SpellData.Combat_Only = TRUE;
    //if (m_SpellData.Lingers) m_SpellData.NotInCamp(TRUE);
    if (m_SpellData.Lingers) m_SpellData.InCamp(FALSE);
    m_SpellData.LingerOnceOnly = m_LingerOnceOnly;
        
    //if (m_SpellData.schoolMask & MagicUserFlag)
    {
      pWnd = GetDlgItem(IDC_GIVETOMU);
      if (pWnd != NULL) pWnd->EnableWindow(TRUE);      
      pWnd = GetDlgItem(IDC_AUTOSCRIBEATLEVELCHANGE);
      if (pWnd != NULL) pWnd->EnableWindow(TRUE);
    //}
    //else
    //{
    //  pWnd = GetDlgItem(IDC_GIVETOMU);
    //  if (pWnd != NULL) pWnd->EnableWindow(FALSE);
    //  pWnd = GetDlgItem(IDC_AUTOSCRIBEATLEVELCHANGE);
    //  if (pWnd != NULL) pWnd->EnableWindow(FALSE);
    }
    
    type = m_DurationRate.GetCurSel();
    
    if (type != LB_ERR)
    {    
      if (type == Permanent)
        enable = FALSE;
      else
        enable = TRUE;      
      
      pWnd = GetDlgItem(IDC_DURATIONTEXT);
      if (pWnd != NULL) pWnd->EnableWindow(enable);
    }  
    
    type = m_TargetingType.GetCurSel();
    
//    if (type != LB_ERR)
//    {
//      if ((type == Self) || (type == WholeParty) || (type == TouchedTargets))
//        enable = FALSE;
//      else
//        enable = TRUE;
//      
//      pWnd = GetDlgItem(IDC_TARGETRANGETEXT);      
//      if (pWnd != NULL) pWnd->EnableWindow(enable);
//      
//      if ((type == Self) || (type == WholeParty))
//        enable = FALSE;
//      else
//        enable = TRUE;
//      
//      pWnd = GetDlgItem(IDC_NUMTARGETTEXT);
//      if (pWnd != NULL) pWnd->EnableWindow(enable);
//    }  
    
    //m_SpellData.NumTargets.DecodeText(m_NumTargetText);
    //m_SpellData.NumTargets().DecodeText(m_NumTargetText);
    m_SpellData.P1.DecodeText(m_TargetP1Text);
    //m_SpellData.TargetRange.DecodeText(m_TargetRangeText);
    //m_SpellData.TargetRange().DecodeText(m_TargetRangeText);
    m_SpellData.P2.DecodeText(m_TargetP2Text);
    m_SpellData.P3.DecodeText(m_TargetP3Text);
    m_SpellData.P4.DecodeText(m_TargetP4Text);
    m_SpellData.P5.DecodeText(m_TargetP5Text);
    m_SpellData.Duration.DecodeText(m_DurationText);
    m_SpellData.EffectDuration.DecodeText(m_EffectDurationText);

    //switch (m_SpellData.Targeting)
    //{
    //case Self:
    //case SelectedByCount:
    //case WholeParty:
    //case SelectByHitDice:
    //case TouchedTargets:
      //m_Lingers = FALSE;
      //m_LingerOnceOnly = FALSE;
      //m_SpellData.Lingers=FALSE;
      //m_SpellData.LingerOnceOnly=FALSE;      
      //break;
    //}
  }
}


BEGIN_MESSAGE_MAP(CSpellDBDlgEx, CDialog)
	//{{AFX_MSG_MAP(CSpellDBDlgEx)
	ON_CBN_SELCHANGE(IDC_DURATIONRATE, OnSelchangeDurationrate)
	ON_CBN_SELCHANGE(IDC_TARGETINGTYPE, OnSelchangeTargetingtype)
	ON_BN_CLICKED(IDC_SPELLSOUND, OnSpellsound)
	ON_BN_CLICKED(IDC_HITANIM, OnHitanim)
	ON_BN_CLICKED(IDC_INROUTEANIM, OnInrouteanim)
	ON_BN_CLICKED(IDC_LINGERANIM, OnLingeranim)
	ON_BN_CLICKED(IDC_TARGCOVANIM, OnTargcovanim)
	ON_BN_CLICKED(IDC_DELETEATTRIBUTE, OnDeleteattribute)
	ON_BN_CLICKED(IDC_ADDATTRIBUTE, OnAddattribute)
	ON_BN_CLICKED(IDC_EDITATTRIBUTE, OnEditattribute)
	ON_NOTIFY(NM_DBLCLK, IDC_ATTRIBUTELIST, OnDblclkAttributelist)
	ON_CBN_SELCHANGE(IDC_CLASS, OnSelchangeClass)
  ON_CBN_SELCHANGE(IDC_SAVERESULT, OnChangeSave)
	ON_BN_CLICKED(IDC_CASTANIM, OnCastanim)
	ON_BN_CLICKED(IDC_INROUTESOUND, OnInroutesound)
	ON_BN_CLICKED(IDC_COVERAGESOUND, OnCoveragesound)
	ON_BN_CLICKED(IDC_SPELLHITSOUND, OnSpellhitsound)
	ON_BN_CLICKED(IDC_SPELLLINGERSOUND, OnSpelllingersound)
	ON_BN_CLICKED(IDC_SPECABS, OnSpecabs)
	ON_BN_CLICKED(IDC_EDITSPELLATTRIBS, OnEditspellattribs)
	ON_BN_CLICKED(IDC_SPELLSCRIPTS, OnSpellscripts)
	ON_BN_CLICKED(IDC_LINGERS, OnLingers)
  ON_BN_CLICKED(IDC_CLASSES, OnClassList)
	ON_BN_CLICKED(IDC_AUTOSCRIBEATLEVELCHANGE, OnAutoscribeatlevelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpellDBDlgEx message handlers

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnInitDialog
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CSpellDBDlgEx::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_TargetingType.ResetContent();
  m_SaveVersus.ResetContent();
  m_SaveResult.ResetContent();
  m_DurationRate.ResetContent();
  m_Class.ResetContent();
  m_CastTimeType.ResetContent();

  int i;
  for (i=0;i<NUM_SPELL_TARGETING_TYPES;i++)
    m_TargetingType.AddString(spellTargetingText[i]);
  for (i=0;i<NUM_SAVE_EFFECT_TYPES;i++)
    m_SaveResult.AddString(SaveEffectText[i]);
  for (i=0;i<NUM_SAVE_VERSUS_TYPES;i++)
    m_SaveVersus.AddString(SaveVersusText[i]);

  //for (i=0;i<NumClassFlags;i++)
  //  m_Class.AddString(ClassFlagText[i]);
  //m_Class.AddString(ClassFlagToText(MagicUserFlag));
  //m_Class.AddString(ClassFlagToText(ClericFlag));
  {
    int idx, n;
    n = spellData.GetCount();
    for (idx=0; idx<n; idx++)
    {
      int j, k, m;
      m = m_schools.GetSize();
      for (k=0; k<m; k++)
      {
        if (m_schools[k] == spellData.PeekSpell(idx)->schoolID) break;
      };
      if (k == m)
      {
        j = m_Class.AddString(spellData.PeekSpell(idx)->schoolID);
        m_schools.Add(spellData.GetSpell(idx)->schoolID);
        m_Class.SetItemData(j, k);
      };
    };
  };

  for (i=0;i<NUM_SPELL_DURATION_TYPES;i++)
    m_DurationRate.AddString(spellDurationText[i]);
  for (i=0;i<NUM_SPELL_CASTING_TIME_TYPES;i++)
    m_CastTimeType.AddString(spellCastingTimeText[i]);
  
  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_SpellData.CastSound, name, MAX_MEDITBUTTON_TEXT);
  m_Sound.SetWindowText(name);
  getBaseName(m_SpellData.MissileSound, name, MAX_MEDITBUTTON_TEXT);
  m_InRouteSound.SetWindowText(name);
  getBaseName(m_SpellData.CoverageSound, name, MAX_MEDITBUTTON_TEXT);
  m_CoverageSound.SetWindowText(name);
  getBaseName(m_SpellData.HitSound, name, MAX_MEDITBUTTON_TEXT);
  m_HitSound.SetWindowText(name);
  getBaseName(m_SpellData.LingerSound, name, MAX_MEDITBUTTON_TEXT);
  m_LingerSound.SetWindowText(name);

  getBaseName(m_SpellData.CastArt.filename, name, MAX_MEDITBUTTON_TEXT);
  m_CastAnim.SetWindowText(name);    	
  getBaseName(m_SpellData.TargHitArt.filename, name, MAX_MEDITBUTTON_TEXT);
  m_HitAnim.SetWindowText(name);    	
  getBaseName(m_SpellData.MissileArt.filename, name, MAX_MEDITBUTTON_TEXT);
  m_InRouteAnim.SetWindowText(name);    	
  getBaseName(m_SpellData.TargLingerArt.filename, name, MAX_MEDITBUTTON_TEXT);
  m_LingerAnim.SetWindowText(name);    	
  getBaseName(m_SpellData.TargCoverageArt.filename, name, MAX_MEDITBUTTON_TEXT);
  m_TargetCovAnim.SetWindowText(name);    	

	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Affected";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_LEFT;
	m_AttList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Change By";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_LEFT;
  m_AttList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

  lvc.iSubItem = 2;
	lvc.pszText = "Cumulative";
	lvc.cx = 65;
	lvc.fmt = LVCFMT_LEFT;
  m_AttList.InsertColumn(2,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 3;
	lvc.pszText = "Activation";
	lvc.cx = 80;
	lvc.fmt = LVCFMT_LEFT;
  m_AttList.InsertColumn(3,&lvc, CListSort::dtSTRING, true);

	m_AttList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  int a_count = 0;
  POSITION pos = m_SpellData.m_EffectedAttributes.GetHeadPosition();
  while (pos != NULL)
  {
    SPELL_EFFECTS_DATA data = *m_SpellData.m_EffectedAttributes.GetAt(pos);
    //data.m_gsID.m_key = (DWORD)pos;
    // This is terrible, I think ;-)
    data.SourcePosition_ID(pos); // What in the world are we doing?
    AddToAvailList(data);
    m_SpellData.m_EffectedAttributes.GetNext(pos);
    a_count++;
  }

  if (a_count > 0)
  {
    m_AttList.Sort();
    //m_AttList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_AttList.EnsureVisible(0, FALSE);
    m_AttList.SelectItem(0, TRUE);
  }
  m_CoinName = globalData.moneyData.GetName(globalData.moneyData.GetDefaultType());  

  /*
  if (m_SpellData.ClassMask == MagicUserFlag)
  {
    // only one class can cast a MagicUser spell, and that's MagicUser's!
    //
    // check the box, then disable it
    DisableCheckBox(IDC_MAGICUSER);
    m_MagicUser=TRUE;
    
    // disable/clear the rest
    DisableCheckBox(IDC_FIGHTER);
    m_Fighter=FALSE;
    DisableCheckBox(IDC_DRUIDCHECK);
    m_Druid = FALSE;
    DisableCheckBox(IDC_CLERIC);
    m_Cleric = FALSE;
    DisableCheckBox(IDC_PALADIN);
    m_Paladin=FALSE;
    DisableCheckBox(IDC_RANGER);
    m_Ranger=FALSE;
    DisableCheckBox(IDC_THIEF);
    m_Thief=FALSE;          
  }
  else // ClericFlag
  {
    // only class that cannot cast a cleric spell - Not Ever!
    // 
    // disable and clear the check
    DisableCheckBox(IDC_MAGICUSER);
    m_MagicUser=FALSE;
    
    // check cleric by default
    EnableCheckBox(IDC_CLERIC);
    //m_Cleric=TRUE;
    m_Cleric = FALSE;
    
    // enable the rest but don't check them
    EnableCheckBox(IDC_FIGHTER);
    m_Fighter=FALSE;
    EnableCheckBox(IDC_DRUIDCHECK);
    m_Druid=FALSE;
    EnableCheckBox(IDC_PALADIN);
    m_Paladin=FALSE;
    EnableCheckBox(IDC_RANGER);
    m_Ranger=FALSE;
    EnableCheckBox(IDC_THIEF);
    m_Thief=FALSE;
  }
  */
  // this new flag is certainly not going
  // to be correct for older designs
  //
  // try to fix it..
  
  
  
  
/*  
  // Changed at Manikus' request December 2010
#define Manikus20101216
#ifdef Manikus20101216
  if (m_SpellData.CastMask == 0)
  {
    if (m_SpellData.schoolMask == MagicUserFlag)
    {
      m_SpellData.CastMask = MagicUserFlag;
    }
    else
    {
      m_SpellData.CastMask = ClericFlag;
    };
  };
#else.
  if (m_SpellData.schoolMask == MagicUserFlag)
  {
    // only MU can cast MU spells
    m_SpellData.CastMask = MagicUserFlag;
  }
  else // ClericFlag
  {
    // MU and Cleric are mutually exclusive
    //
    // remove MU flag
    m_SpellData.CastMask &= ~MagicUserFlag;
    
    // if that was the only bit set,
    // then at least set the cleric flag
    if (m_SpellData.CastMask == 0)
      m_SpellData.CastMask = ClericFlag;
  }
#endif
*/









  /*
  for (i=0;i<NumClassFlags;i++)
  {
    WORD flag = (m_SpellData.CastMask & (0x0001 << i));
    switch (flag) 
    {
    case MagicUserFlag: m_MagicUser=TRUE; break;
    case ClericFlag:    m_Cleric=TRUE; break;
    case ThiefFlag:     m_Thief=TRUE; break;
    case FighterFlag:   m_Fighter=TRUE; break;
    case PaladinFlag:   m_Paladin=TRUE; break;
    case RangerFlag:    m_Ranger=TRUE; break;
    case DruidFlag:     m_Druid=TRUE; break;
    }
  }
  */
  UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnSelchangeDurationrate
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnSelchangeDurationrate() 
{
   UpdateData(TRUE);
   UpdateData(FALSE);
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnSelchangeTargetingtype
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnSelchangeTargetingtype() 
{
   UpdateData(TRUE);
   UpdateData(FALSE);	
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnCastanim
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnCastanim() 
{
  UpdateData(TRUE);

	CSmallPicDlg dlg(m_SpellData.CastArt);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    char name[MAX_MEDITBUTTON_TEXT+1];
    m_SpellData.CastArt = dlg.m_SmallPic;
    m_SpellData.CastArt.picType = SpriteDib;
    getBaseName(m_SpellData.CastArt.filename, name, MAX_MEDITBUTTON_TEXT);
    m_CastAnim.SetWindowText(name);    	
  }

  UpdateData(FALSE);	
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnHitanim
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnHitanim() 
{
  UpdateData(TRUE);

	CSmallPicDlg dlg(m_SpellData.TargHitArt);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    char name[MAX_MEDITBUTTON_TEXT+1];
    m_SpellData.TargHitArt = dlg.m_SmallPic;
    m_SpellData.TargHitArt.picType = SpriteDib;
    getBaseName(m_SpellData.TargHitArt.filename, name, MAX_MEDITBUTTON_TEXT);
    m_HitAnim.SetWindowText(name);    	
  }

  UpdateData(FALSE);		
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnInrouteanim
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnInrouteanim() 
{
  UpdateData(TRUE);

	CSmallPicDlg dlg(m_SpellData.MissileArt);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    char name[MAX_MEDITBUTTON_TEXT+1];
    m_SpellData.MissileArt = dlg.m_SmallPic;
    m_SpellData.MissileArt.picType = SpriteDib;
    getBaseName(m_SpellData.MissileArt.filename, name, MAX_MEDITBUTTON_TEXT);
    m_InRouteAnim.SetWindowText(name);    	
  }

  UpdateData(FALSE);		
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnLingeranim
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnLingeranim() 
{
  UpdateData(TRUE);

	CSmallPicDlg dlg(m_SpellData.TargLingerArt);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    char name[MAX_MEDITBUTTON_TEXT+1];
    m_SpellData.TargLingerArt = dlg.m_SmallPic;
    m_SpellData.TargLingerArt.picType = SpriteDib;
    getBaseName(m_SpellData.TargLingerArt.filename, name, MAX_MEDITBUTTON_TEXT);
    m_LingerAnim.SetWindowText(name);    	
  }

  UpdateData(FALSE);	
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnTargcovanim
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnTargcovanim() 
{
  UpdateData(TRUE);

	CSmallPicDlg dlg(m_SpellData.TargCoverageArt);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    char name[MAX_MEDITBUTTON_TEXT+1];
    m_SpellData.TargCoverageArt = dlg.m_SmallPic;
    m_SpellData.TargCoverageArt.picType = SpriteDib;
    getBaseName(m_SpellData.TargCoverageArt.filename, name, MAX_MEDITBUTTON_TEXT);
    m_TargetCovAnim.SetWindowText(name);    	
  }

  UpdateData(FALSE);		
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnDeleteattribute
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnDeleteattribute() 
{
  POSITION pos = m_AttList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    SET_MODIFIED;
    int nItem = m_AttList.GetNextSelectedItem(pos);
    POSITION flag = (POSITION)m_AttList.GetItemData(nItem);
    m_SpellData.m_EffectedAttributes.RemoveAt(flag);
  
    m_AttList.DeleteAllItems();
    int a_count = 0;
    POSITION eapos = m_SpellData.m_EffectedAttributes.GetHeadPosition();
    while (eapos != NULL)
    {
      SPELL_EFFECTS_DATA data = *m_SpellData.m_EffectedAttributes.GetAt(eapos);
      //data.m_gsID.m_key = (DWORD)eapos;
      // Seems bad.  PRS
      data.SourcePosition_ID(eapos);
      AddToAvailList(data);
      m_SpellData.m_EffectedAttributes.GetNext(eapos);
      a_count++;
    }

    if (a_count > 0)
    {
      //m_AttList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_AttList.EnsureVisible(0, FALSE);
      m_AttList.SelectItem(0, TRUE);
    }
  }	
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnAddattribute
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnAddattribute() 
{
  SPELL_EFFECTS_DATA data;
  CSpellAttribute dlg(data);
  if (dlg.DoModal()==IDOK)
  {
    SET_MODIFIED;
    dlg.GetData(data);
    //data.m_gsID.m_key = (DWORD)m_SpellData.AddEffect(data);
    // Seems bad.  PRS 
    data.SourcePosition_ID(m_SpellData.AddEffect(data));
    if (data.SourcePosition_ID()!=NULL) // returns NULL if duplicate
      AddToAvailList(data);
  }	
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnOK
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnOK() 
{
  UpdateData(TRUE);
	m_SpellData.CompileScripts();
  /*
  m_SpellData.CastMask = 0;
  if (m_MagicUser) m_SpellData.CastMask |= MagicUserFlag;
  if (m_Cleric) m_SpellData.CastMask |= ClericFlag;
  if (m_Fighter) m_SpellData.CastMask |= FighterFlag;
  if (m_Ranger) m_SpellData.CastMask |= RangerFlag;
  if (m_Druid) m_SpellData.CastMask |= DruidFlag;
  if (m_Paladin) m_SpellData.CastMask |= PaladinFlag;
  if (m_Thief) m_SpellData.CastMask |= ThiefFlag;
  */
	CDialog::OnOK();
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnEditattribute
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnEditattribute() 
{
  POSITION pos = m_AttList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_AttList.GetNextSelectedItem(pos);
    POSITION flag = (POSITION)m_AttList.GetItemData(nItem);
    SPELL_EFFECTS_DATA data = *m_SpellData.m_EffectedAttributes.GetAt(flag);
    CSpellAttribute dlg(data);
    if (dlg.DoModal() == IDOK)
    {
      dlg.GetData(data);
      SET_MODIFIED;
      *m_SpellData.m_EffectedAttributes.GetAt(flag) = data;
      m_SpellData.CompileScript(*m_SpellData.m_EffectedAttributes.GetAt(flag));
      UpdateAvailList(nItem, data);
    }
  }		
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnDblclkAttributelist
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnDblclkAttributelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnEditattribute();	
	*pResult = 0;
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::AddToAvailList
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::AddToAvailList(SPELL_EFFECTS_DATA &data)
{
  int listIdx = m_AttList.GetItemCount();

  CString att("");
  CString chng("");
  CString cumul("");
  CString activ("");
  LVITEM item;

  att=data.AffectedAttr();
  chng=data.changeData.EncodeText();
  cumul.Format("%s", ((data.flags&SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE)?"Yes":"No"));
  activ = data.ActivationScript();
  activ.Remove('\r');
  activ.Remove('\n');
  activ.Remove('\t');

	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = att.GetBuffer(att.GetLength()+1);
	item.cchTextMax = 0;
	item.iImage = 0;
	//PRS July 2009item.lParam = data.m_gsID.m_key;
	item.lParam = data.SourceInteger_ID();

  m_AttList.InsertItem(&item);
  m_AttList.SetItemText(listIdx, 1, chng); 
  m_AttList.SetItemText(listIdx, 2, cumul); 
  m_AttList.SetItemText(listIdx, 3, activ); 
  //m_AttList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AttList.SelectItem(listIdx, TRUE);
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::UpdateAvailList
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::UpdateAvailList(int item, SPELL_EFFECTS_DATA &data)
{
  CString att("");
  CString chng("");
  CString cumul("");
  CString activ("");

  att=data.AffectedAttr();
  chng=data.changeData.EncodeText();
  cumul.Format("%s", ((data.flags&SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE)?"Yes":"No"));
  activ = data.ActivationScript();
  activ.Remove('\r');
  activ.Remove('\n');
  activ.Remove('\t');

  m_AttList.SetItemText(item, 0, att); 
  m_AttList.SetItemText(item, 1, chng); 
  m_AttList.SetItemText(item, 2, cumul); 
  m_AttList.SetItemText(item, 3, activ); 
  //m_AttList.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
  m_AttList.SelectItem(item, TRUE);
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnSelchangeClass
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnSelchangeClass() 
{
  if (m_Class.m_hWnd == NULL) return;

   UpdateData(TRUE);
     /*

   if (m_Class.GetCurSel() == FlagToIndex(MagicUserFlag))
   {
     // only one class can cast a MagicUser spell, and that's MagicUser's!
     //
     // check the box, then disable it
     DisableCheckBox(IDC_MAGICUSER);
     m_MagicUser=TRUE;
     
     // disable/clear the rest
     DisableCheckBox(IDC_FIGHTER);
     m_Fighter=FALSE;
     DisableCheckBox(IDC_DRUIDCHECK);
     m_Druid = FALSE;
     DisableCheckBox(IDC_CLERIC);
     m_Cleric = FALSE;
     DisableCheckBox(IDC_PALADIN);
     m_Paladin=FALSE;
     DisableCheckBox(IDC_RANGER);
     m_Ranger=FALSE;
     DisableCheckBox(IDC_THIEF);
     m_Thief=FALSE;          
   }
   else // ClericFlag
   {
     // only class that cannot cast a cleric spell - Not Ever!
     // 
     // disable and clear the check
     DisableCheckBox(IDC_MAGICUSER);
     m_MagicUser=FALSE;
     
     // check cleric by default
     EnableCheckBox(IDC_CLERIC);
     m_Cleric=TRUE;
     
     // enable the rest but don't check them
     EnableCheckBox(IDC_FIGHTER);
     m_Fighter=FALSE;
     EnableCheckBox(IDC_DRUIDCHECK);
     m_Druid=FALSE;
     EnableCheckBox(IDC_PALADIN);
     m_Paladin=FALSE;
     EnableCheckBox(IDC_RANGER);
     m_Ranger=FALSE;
     EnableCheckBox(IDC_THIEF);
     m_Thief=FALSE;
   }
  */
   UpdateData(FALSE);
}

void CSpellDBDlgEx::OnChangeSave() 
{
  if (m_Class.m_hWnd == NULL) return;

   UpdateData(TRUE);
   UpdateData(FALSE);
}


//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnSpellsound
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnSpellsound() 
{
  CSoundChooser dlg(m_SpellData.CastSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_SpellData.CastSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_SpellData.CastSound, name, MAX_MEDITBUTTON_TEXT);
    m_Sound.SetWindowText(name);
  }	
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnInroutesound
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnInroutesound() 
{
  CSoundChooser dlg(m_SpellData.MissileSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_SpellData.MissileSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_SpellData.MissileSound, name, MAX_MEDITBUTTON_TEXT);
    m_InRouteSound.SetWindowText(name);
  }		
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnCoveragesound
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnCoveragesound() 
{
  CSoundChooser dlg(m_SpellData.CoverageSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_SpellData.CoverageSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_SpellData.CoverageSound, name, MAX_MEDITBUTTON_TEXT);
    m_CoverageSound.SetWindowText(name);
  }	
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnSpellhitsound
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnSpellhitsound() 
{
  CSoundChooser dlg(m_SpellData.HitSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_SpellData.HitSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_SpellData.HitSound, name, MAX_MEDITBUTTON_TEXT);
    m_HitSound.SetWindowText(name);
  }	
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnSpelllingersound
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnSpelllingersound() 
{
  CSoundChooser dlg(m_SpellData.LingerSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_SpellData.LingerSound = dlg.m_Filename;	
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_SpellData.LingerSound, name, MAX_MEDITBUTTON_TEXT);
    m_LingerSound.SetWindowText(name);
  }	
}
//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnSpecabs
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnSpecabs() 
{
  CString title;
  title.Format("Special Abilities for spell %s", m_SpellData.Name);
  CEditAttributes dlg(m_SpellData.specAbs, title);
  if (dlg.DoModal() == IDOK)
  {;
    m_SpellData.specAbs.Copy(dlg.m_data);
  };
//  NotImplemented(0x7f6f3d, FALSE);
//  CChooseSpeclAbDlg	dlg(m_SpellData.specAbs, title);
//  //dlg.m_NoMsgTypes=TRUE; // cannot edit msg types
//  if (dlg.DoModal() == IDOK)
//   dlg.GetData(m_SpellData.specAbs);
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnEditspellattribs
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnEditspellattribs() 
{
  CEditAttributes dlg(m_SpellData.spell_asl, "");
  if (dlg.DoModal()==IDOK)
    dlg.GetData(m_SpellData.spell_asl);	
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnSpellscripts
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnSpellscripts() 
{
  CString start("$PUBLIC $FUNC spelltest() { ");
  CString end(" } spelltest;");
  CString func = "spelltest";

  ScriptList list;
  list.SetSize(7);
  list[0].name = "Spell Begin Script";
  list[0].script = m_SpellData.SpellBeginSource;

  list[1].name = "Spell End Script";
  list[1].script = m_SpellData.SpellEndSource;
  
  list[2].name = "Saving Throw Script";
  list[2].script = m_SpellData.SavingThrowSource;
  
  list[3].name = "Saving Throw Succeeded Script";
  list[3].script = m_SpellData.SavingThrowSucceededSource;
  
  list[4].name = "Saving Throw Failed Script";
  list[4].script = m_SpellData.SavingThrowFailedSource;
  
  list[5].name = "Spell Initiation Script";
  list[5].script = m_SpellData.SpellInitiationSource;

  list[6].name = "Spell Termination Script";
  list[6].script = m_SpellData.SpellTerminationSource;
  
  CEditText dlg(start, end, func, true, list);
  dlg.m_ScriptFilter = SPELL_USAGE;
  dlg.m_helpKey = "SPELL-SCRIPTS";
  if (dlg.DoModal()==IDOK)
  {
    m_SpellData.SpellBeginSource = dlg.m_list[0].script;
    m_SpellData.SpellEndSource = dlg.m_list[1].script;
    m_SpellData.SavingThrowSource = dlg.m_list[2].script;
    m_SpellData.SavingThrowSucceededSource = dlg.m_list[3].script;
    m_SpellData.SavingThrowFailedSource = dlg.m_list[4].script;
    m_SpellData.SpellInitiationSource = dlg.m_list[5].script;
    m_SpellData.SpellTerminationSource = dlg.m_list[6].script;
  }
}

//*****************************************************************************
//    NAME: CSpellDBDlgEx::OnLingers
//
// PURPOSE: 
//
//*****************************************************************************
void CSpellDBDlgEx::OnLingers() 
{
  UpdateData(TRUE);
  UpdateData(FALSE);  
}

void CSpellDBDlgEx::OnAutoscribeatlevelchange() 
{
	// TODO: Add your control notification handler code here
	
}

afx_msg void CSpellDBDlgEx::OnClassList(void)
{
  //CClassDlg dlg(m_SpellData.CastMask);
  CClassDlg dlg(&m_SpellData.allowedBaseclasses.baseclasses);
  if (dlg.DoModal() == IDOK)
  {
    //m_SpellData.CastMask = dlg.m_selectedClassesMask;
    m_SpellData.allowedBaseclasses = dlg.m_selectedBaseclasses;
  };
}
