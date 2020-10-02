/******************************************************************************
* Filename: GlobalDataDlg.cpp
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
#include "GlobalData.h"
//#include "GlobText.h"
#include "ItemKeyQuestDlg.h"
#include "CharacterEditor.h"
#include "ScrollPicDlg.h"
#include "PicSlot.h"
#include "GlobalSndChooser.h"
//#include "ItemDlg.h"
#include "StartingEquipDlg.h"
#include "MoneyConfigDlg.h"
#include "DiffLvlSettings.h"
#include "EditAttributes.h"
#include "TitleScreenData.h"
#include "Spelldlg.h"
#include "GlobalDataDlg.h"
#include "ConfigFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int ExportGlobalDB(LPCSTR file,
                   GLOBAL_STATS *pGlobalPointer,
                   CONFIG_FILE *pConfigFile);

int ImportGlobalDB(LPCSTR file,
                   GLOBAL_STATS *pGlobalPointer,
                   CONFIG_FILE *pConfigFile);


/////////////////////////////////////////////////////////////////////////////
// CGlobalDataDlg dialog


CGlobalDataDlg::CGlobalDataDlg(GLOBAL_STATS& data, CWnd* pParent /*=NULL*/)
	: CDialog(CGlobalDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGlobalDataDlg)
	m_DesignName = _T("");
	m_Facing = 0;
	m_StartExp = 0;
	m_StartGems = 0;
	m_StartJewelry = 0;
	m_StartPlatinum = 0;
	m_StartTime = 800;
	m_StartX = 0;
	m_StartY = 0;
	m_DSTime = 10;
	m_DTime = 1;
	m_WSTime = 18*60;
	m_WTime = 12*60;
	m_AllowCharCreate = TRUE;
  m_DeadAtZeroHP = FALSE;
	m_MaxPartySize = MAX_PARTY_PCS+2;
	m_MinPCs = 1;
  m_MaxPCs = MAX_PARTY_PCS;
	m_UseAutoDarken = FALSE;
	m_AutoDarkenAmount = 0;
	m_DarkenEndTime = 0;
	m_DarkenStartTime = 0;
	m_CoinName = _T("");
	m_GemName = _T("");
	m_JewelryName = _T("");
	m_StartExpType = 0;
	//}}AFX_DATA_INIT

  m_data = data;
}

void CGlobalDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGlobalDataDlg)
	DDX_Control(pDX, IDC_MOUSECURSOR, m_MouseCursor);
	DDX_Control(pDX, IDC_SCREENBG, m_ScreenBG);
  DDX_Control(pDX, IDC_AreaViewArt, m_AreaViewArt);
  DDX_Control(pDX, IDC_CREDITS, m_Credits),
	DDX_Control(pDX, IDC_SELECTICONBG, m_IconBG);
	DDX_Control(pDX, IDC_COMBATDYING, m_CombatDying);
	DDX_Control(pDX, IDC_COMBATDEATH, m_CombatDeath);
	DDX_Control(pDX, IDC_SELECTVBORDER, m_VBorder);
	DDX_Control(pDX, IDC_SELECTHBORDER, m_HBorder);
	DDX_Control(pDX, IDC_SELECTFRAME, m_Frame);
	DDX_Control(pDX, IDC_SELECTCOMBATVBORDER, m_CombatVBorder);
	DDX_Control(pDX, IDC_SELECTCOMBATHBORDER, m_CombatHBorder);
	DDX_Control(pDX, IDC_SELECTCOMBATCOMMON, m_CombatCommon);
	DDX_Control(pDX, IDC_FONTSELECT, m_FontSelect);
	DDX_Control(pDX, IDC_STARTLEVEL, m_StartLevel);
	DDX_Text(pDX, IDC_DESIGNNAME, m_DesignName);
	DDX_Radio(pDX, IDC_FACENORTH, m_Facing);
	DDX_Text(pDX, IDC_STARTEXP, m_StartExp);
	DDX_Text(pDX, IDC_STARTGEMS, m_StartGems);
	DDX_Text(pDX, IDC_STARTJEWELRY, m_StartJewelry);
	DDX_Text(pDX, IDC_STARTPLATINUM, m_StartPlatinum);
	DDX_Text(pDX, IDC_STARTTIME, m_StartTime);
	DDX_Text(pDX, IDC_STARTX, m_StartX);
	DDX_Text(pDX, IDC_STARTY, m_StartY);
	DDX_Text(pDX, IDC_DUNGSEARCHTIME, m_DSTime);
	DDX_Text(pDX, IDC_DUNGTIME, m_DTime);
	DDX_Text(pDX, IDC_WILDSEARCHTIME, m_WSTime);
	DDX_Text(pDX, IDC_WILDTIME, m_WTime);
	DDX_Check(pDX, IDC_ALLOWCHARCREATE, m_AllowCharCreate);
	DDX_Check(pDX, IDC_DEADATZEROHP, m_DeadAtZeroHP);


	DDX_Text(pDX, IDC_MAXPARTY, m_MaxPartySize);
	DDV_MinMaxInt(pDX, m_MaxPartySize, 1, min(MAX_PARTY_PCS+2, MAX_PARTY_MEMBERS));
	DDX_Text(pDX, IDC_MINPCS, m_MinPCs);
	DDV_MinMaxInt(pDX, m_MinPCs, 1, MAX_PARTY_MEMBERS);
	DDX_Text(pDX, IDC_MAXPCS, m_MaxPCs);
	DDV_MinMaxInt(pDX, m_MaxPCs, 1, MAX_PARTY_MEMBERS);


	DDX_Check(pDX, IDC_USEAUTODARKEN, m_UseAutoDarken);
	DDX_Text(pDX, IDC_AUTODARKENAMOUNT, m_AutoDarkenAmount);
	DDX_Text(pDX, IDC_DARKENENDTIME, m_DarkenEndTime);
	DDX_Text(pDX, IDC_DARKENTIMESTART, m_DarkenStartTime);
	DDX_Text(pDX, IDC_COINNAME, m_CoinName);
	DDX_Text(pDX, IDC_GEMNAME, m_GemName);
	DDX_Text(pDX, IDC_JEWELNAME, m_JewelryName);
	DDX_Radio(pDX, IDC_STARTEXPTYPE, m_StartExpType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGlobalDataDlg, CDialog)
	//{{AFX_MSG_MAP(CGlobalDataDlg)
	ON_BN_CLICKED(IDC_SELECTCOMBATCOMMON, OnSelectcombatcommon)
	ON_BN_CLICKED(IDC_SELECTCOMBATHBORDER, OnSelectcombathborder)
	ON_BN_CLICKED(IDC_SELECTCOMBATVBORDER, OnSelectcombatvborder)
	ON_BN_CLICKED(IDC_SELECTFRAME, OnSelectframe)
	ON_BN_CLICKED(IDC_SELECTHBORDER, OnSelecthborder)
	ON_BN_CLICKED(IDC_SELECTVBORDER, OnSelectvborder)
	ON_CBN_SELCHANGE(IDC_STARTLEVEL, OnSelchangeStartlevel)
	ON_BN_CLICKED(IDC_SOUNDS, OnSounds)
	ON_BN_CLICKED(IDC_FONTSELECT, OnFontselect)
	ON_BN_CLICKED(IDC_COMBATDYING, OnCombatdying)
	ON_BN_CLICKED(IDC_COMBATDEATH, OnCombatdeath)
	ON_BN_CLICKED(IDC_STARTEQUIP, OnStartequip)
	ON_BN_CLICKED(IDC_CONFIGMONEY, OnConfigmoney)
	ON_BN_CLICKED(IDC_SELECTICONBG, OnSelecticonbg)
	ON_BN_CLICKED(IDC_SCREENBG, OnScreenbg)
	ON_BN_CLICKED(IDC_CREDITS, OnCredits)
	ON_BN_CLICKED(IDC_DIFFLVL, OnDifflvl)
	ON_BN_CLICKED(IDC_EDITGLOBALATTRIBUTES, OnEditglobalattributes)
	ON_BN_CLICKED(IDC_MOUSECURSOR, OnMousecursor)
	ON_BN_CLICKED(IDC_SELECTTITLES, OnSelecttitles)
	ON_BN_CLICKED(IDC_FIXSPELLS, OnFixspells)
  ON_BN_CLICKED(IDC_AreaViewArt, OnAreaViewArt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGlobalDataDlg message handlers


void CGlobalDataDlg::FetchData(void)
{
  int i;
  m_StartLevel.ResetContent();
  int count = 0;

  for (i=0;i<MAX_LEVELS;i++)
  {
      if (m_data.levelInfo.stats[i].used)
      {
         m_StartLevel.AddString(m_data.levelInfo.stats[i].level_name);
         m_StartLevel.SetItemData(count, i);

         if (i == m_data.startLevel)
            m_StartLevel.SetCurSel(count);

         count++;
      }
      else
      {
        // Perhaps this is the designated staring level.
        // If so, we will force its presence.
        if (m_data.startLevel == i)
        {
          CString levelName;
          levelName.Format("Level_%03d", i+1);
          m_StartLevel.AddString(levelName);
          m_StartLevel.SetItemData(count, i);
          m_StartLevel.SetCurSel(count);
          count++;
        };
      };
  }

  if (count == 0)
  {
     m_StartLevel.AddString(m_data.levelInfo.stats[0].level_name);
     m_StartLevel.SetItemData(0,0);
     m_data.startLevel = 0;
     m_StartLevel.SetCurSel(0);
  }  

  m_DesignName = m_data.designName;
  m_Facing = m_data.startFacing;
  m_StartExp = m_data.startExp;
  m_StartExpType = m_data.startExpType;
  m_StartGems = m_data.startGem;
  m_StartJewelry = m_data.startJewelry;
  m_StartPlatinum = m_data.startPlatinum;
  m_StartTime = m_data.startTime;
  m_StartX = m_data.startX;
  m_StartY = m_data.startY;

  m_DTime = m_data.DungeonTimeDelta;
  m_DSTime = m_data.DungeonSearchTimeDelta;
  m_WTime = m_data.WildernessTimeDelta;
  m_WSTime = m_data.WildernessSearchTimeDelta;
  m_UseAutoDarken = m_data.AutoDarkenViewport;
  m_AutoDarkenAmount = m_data.AutoDarkenAmount;
  m_DarkenEndTime = m_data.EndDarken;
  m_DarkenStartTime = m_data.StartDarken;

	//m_MinPartySize = m_data.minPartySize;
	//m_MaxPartySize = m_data.maxPartySize;
	m_MinPCs       = m_data.GetMinPCs();
	m_MaxPartySize = m_data.GetMaxPartySize();
  m_MaxPCs       = m_data.GetMaxPCs();

	//m_AllowCharCreate = m_data.allowCharCreate;
  m_AllowCharCreate = m_data.GetAllowCharCreate();
  m_DeadAtZeroHP    = m_data.GetDeadAtZeroHP();

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_data.logfont.lfFaceName, name, MAX_MEDITBUTTON_TEXT-5);
  m_FontSelect.SetWindowText(name);
 
  getBaseName(CombatCursorArt.name, name, MAX_MEDITBUTTON_TEXT);
  m_CombatCommon.SetWindowText(name);

  getBaseName(CombatDeathIconArt.name, name, MAX_MEDITBUTTON_TEXT);
  m_CombatDying.SetWindowText(name);
  
  getBaseName(CombatDeathArt.name, name, MAX_MEDITBUTTON_TEXT);
  m_CombatDeath.SetWindowText(name);

  getBaseName(HBarCombArt.name, name, MAX_MEDITBUTTON_TEXT);
  m_CombatHBorder.SetWindowText(name);

  getBaseName(VBarCombArt.name, name, MAX_MEDITBUTTON_TEXT);
  m_CombatVBorder.SetWindowText(name);

  getBaseName(FrameVPArt.name, name, MAX_MEDITBUTTON_TEXT);
  m_Frame.SetWindowText(name);

  getBaseName(HBarVPArt.name, name, MAX_MEDITBUTTON_TEXT);
  m_HBorder.SetWindowText(name);

  getBaseName(VBarVPArt.name, name, MAX_MEDITBUTTON_TEXT);
  m_VBorder.SetWindowText(name);
    
  getBaseName(m_data.IconBgArt, name, MAX_MEDITBUTTON_TEXT);
  m_IconBG.SetWindowText(name);

  //getBaseName(m_data.TitleBgArt, name, MAX_MEDITBUTTON_TEXT);
  //m_Title.SetWindowText(name);

  getBaseName(m_data.BackgroundArt, name, MAX_MEDITBUTTON_TEXT);
  m_ScreenBG.SetWindowText(name);

  getBaseName(m_data.m_MapArt, name, MAX_MEDITBUTTON_TEXT);
  m_AreaViewArt.SetWindowText(name);

  getBaseName(CursorArt.filename, name, MAX_MEDITBUTTON_TEXT);
  m_MouseCursor.SetWindowText(name);

  m_CoinName.Format("Starting %s", m_data.moneyData.GetName(m_data.moneyData.GetDefaultType()));
  m_GemName.Format("Starting %s", m_data.moneyData.GetName(GemType));
  m_JewelryName.Format("Starting %s", m_data.moneyData.GetName(JewelryType));
}


BOOL CGlobalDataDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  FetchData();

  UpdateData(FALSE);
  SetDefID(IDOK);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGlobalDataDlg::OnOK() 
{
  UpdateData(TRUE);

  m_data.designName = m_DesignName;
  m_data.startFacing = (BYTE)m_Facing;
  m_data.startExp = m_StartExp;
  m_data.startExpType = m_StartExpType;
  m_data.startGem = m_StartGems;
  m_data.startJewelry = m_StartJewelry;
  m_data.startPlatinum = m_StartPlatinum;
  m_data.startTime = m_StartTime;
  m_data.startX = (BYTE)m_StartX;
  m_data.startY = (BYTE)m_StartY;
  m_data.DungeonTimeDelta=(int)m_DTime;
  m_data.DungeonSearchTimeDelta=(int)m_DSTime;
  m_data.WildernessTimeDelta=(int)m_WTime;
  m_data.WildernessSearchTimeDelta=(int)m_WSTime;
  m_data.AutoDarkenViewport = m_UseAutoDarken;
  m_data.AutoDarkenAmount = m_AutoDarkenAmount;
  m_data.EndDarken = m_DarkenEndTime;
  m_data.StartDarken = m_DarkenStartTime;


	//m_data.minPartySize = m_MinPartySize;
	//m_data.maxPartySize = m_MaxPartySize;
	m_data.SetMinPCs(m_MinPCs);
	m_data.SetMaxPCs(m_MaxPCs);
  m_data.SetMaxPartySize(m_MaxPartySize);


	//m_data.allowCharCreate = m_AllowCharCreate;
  m_data.SetAllowCharCreate(m_AllowCharCreate);
  m_data.SetDeadAtZeroHP(m_DeadAtZeroHP);

   int sel = m_StartLevel.GetCurSel();
   
   if (sel != LB_ERR)
      m_data.startLevel = m_StartLevel.GetItemData(sel);

  CDialog::OnOK();
}

void CGlobalDataDlg::GetData(GLOBAL_STATS& data)
{
  data = m_data;
}

void CGlobalDataDlg::OnSelectcombatcommon() 
{
  CScrollPicDlg dlg(CombatCursorArt.name, rte.CombatArtDir(), "");

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    CombatCursorArt.name = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_CombatCommon.SetWindowText(name);
  }
}

void CGlobalDataDlg::OnSelectcombathborder() 
{
  CScrollPicDlg dlg(HBarCombArt.name, rte.WindowArtDir(), "");

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    HBarCombArt.name = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_CombatHBorder.SetWindowText(name);
  }
}

void CGlobalDataDlg::OnSelectcombatvborder() 
{
  CScrollPicDlg dlg(VBarCombArt.name, rte.WindowArtDir(), "");

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    VBarCombArt.name = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_CombatVBorder.SetWindowText(name);
  }
}

void CGlobalDataDlg::OnSelectframe() 
{
  CScrollPicDlg dlg(FrameVPArt.name, rte.WindowArtDir(), "");

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    FrameVPArt.name = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_Frame.SetWindowText(name);
  }
}

void CGlobalDataDlg::OnSelecthborder() 
{
  CScrollPicDlg dlg(
         HBarVPArt.name, 
         rte.WindowArtDir(), 
         ""); // Horizontal Bar for ViewPort Art

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    HBarVPArt.name = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_HBorder.SetWindowText(name);
  }
}

void CGlobalDataDlg::OnSelectvborder() 
{
  CScrollPicDlg dlg(VBarVPArt.name, rte.WindowArtDir(), "");

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    VBarVPArt.name = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_VBorder.SetWindowText(name);
  }
}

void CGlobalDataDlg::OnSelchangeStartlevel() 
{
   int sel = m_StartLevel.GetCurSel();
   
   if (sel != LB_ERR)
      m_data.startLevel = m_StartLevel.GetItemData(sel);
}

void CGlobalDataDlg::OnSounds() 
{
  CGlobalSndChooser dlg;
  dlg.CharHit = m_data.sounds.CharHit;
  dlg.CharMiss = m_data.sounds.CharMiss;
  dlg.PartyBump = m_data.sounds.PartyBump;
  dlg.PartyStep = m_data.sounds.PartyStep;
  dlg.DeathMusic = m_data.sounds.DeathMusic;
  dlg.IntroMusic = m_data.sounds.IntroMusic;
  dlg.CampMusic = m_data.sounds.CampMusic;
	
  if (dlg.DoModal() == IDOK)
  {
    m_data.sounds.CharHit = dlg.CharHit;
    m_data.sounds.CharMiss = dlg.CharMiss;
    m_data.sounds.PartyBump = dlg.PartyBump;
    m_data.sounds.PartyStep = dlg.PartyStep;
    m_data.sounds.DeathMusic = dlg.DeathMusic;
    m_data.sounds.IntroMusic = dlg.IntroMusic;
    m_data.sounds.CampMusic = dlg.CampMusic;
  }
}

void CGlobalDataDlg::OnFontselect() 
{  
  /*
  LOGFONT lf;
  memset(&lf, 0, sizeof(LOGFONT));
  CClientDC dc(this);
  int logpixy = dc.GetDeviceCaps(LOGPIXELSY);
  int oldMode = dc.SetMapMode(MM_TEXT);
  lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
  lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  lf.lfQuality = DEFAULT_QUALITY;
  lf.lfPitchAndFamily = (DEFAULT_PITCH | FF_DONTCARE);
  lf.lfHeight = -MulDiv(m_data.fontSize, logpixy, 72);
  strcpy(lf.lfFaceName, m_data.font);
  lf.lfCharSet = ANSI_CHARSET;
  lf.lfWeight = FW_BOLD;
  */

  //WriteDebugString("Font GetDeviceCaps(YPIX) in %i\n", logpixy);
  //WriteDebugString("Font point size in %i\n", m_data.fontSize);
  //WriteDebugString("Font height in %i\n", lf.lfHeight);
  DWORD flags = CF_INITTOLOGFONTSTRUCT|CF_SCREENFONTS | CF_NOVERTFONTS | CF_NOSCRIPTSEL | CF_FORCEFONTEXIST;
  CFontDialog dlg(&m_data.logfont, flags);
  
  if (dlg.DoModal() == IDOK)
  {
    dlg.GetCurrentFont(&m_data.logfont);
    /*
    m_data.font = dlg.GetFaceName();
    //WriteDebugString("Font height out %i\n", lf.lfHeight);
    //WriteDebugString("Font GetDeviceCaps(YPIX) out %i\n", logpixy);
    m_data.fontSize = MulDiv(abs(dlg.m_lf.lfHeight), 72, logpixy);
    //WriteDebugString("Font point size out %i\n", m_data.fontSize);
    //m_data.fontSize = max(8, m_data.fontSize);
    */

    // none of these will work with CDXBitmapFont
    m_data.logfont.lfItalic=0;
    m_data.logfont.lfStrikeOut=0;
    m_data.logfont.lfUnderline=0;
    
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_data.logfont.lfFaceName, name, MAX_MEDITBUTTON_TEXT-5);
    m_FontSelect.SetWindowText(name);
  }
  //dc.SetMapMode(oldMode);
}

void CGlobalDataDlg::OnCombatdying() 
{
  CScrollPicDlg dlg(CombatDeathIconArt.name, rte.CombatArtDir(), "");

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    CombatDeathIconArt.name = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_CombatDying.SetWindowText(name);
  }	
}

void CGlobalDataDlg::OnCombatdeath() 
{
  CScrollPicDlg dlg(CombatDeathArt.name, rte.CombatArtDir(), "");

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    CombatDeathArt.name = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_CombatDeath.SetWindowText(name);
  }		
}

void CGlobalDataDlg::OnStartequip() 
{
  ITEM_LIST tmpStartEquip;
  ITEM_LIST tmpClericEquip;
  ITEM_LIST tmpDruidEquip;
  ITEM_LIST tmpFighterEquip;
  ITEM_LIST tmpMagicUserEquip;
  ITEM_LIST tmpPaladinEquip;
  ITEM_LIST tmpRangerEquip;
  ITEM_LIST tmpThiefEquip;

  tmpStartEquip     = m_data.startEquip;
  tmpClericEquip    = m_data.ClericEquip;
  tmpDruidEquip     = m_data.DruidEquip;
  tmpFighterEquip   = m_data.FighterEquip;
  tmpMagicUserEquip = m_data.MagicUserEquip;
  tmpPaladinEquip   = m_data.PaladinEquip;
  tmpRangerEquip    = m_data.RangerEquip;
  tmpThiefEquip     = m_data.ThiefEquip;

  CStartingEquipDlg dlg(&m_data);
  if (dlg.DoModal()!=IDOK)
  {
    //dlg.GetData(m_data);
    m_data.startEquip     = tmpStartEquip;
    m_data.ClericEquip    = tmpClericEquip;
    m_data.DruidEquip     = tmpDruidEquip;
    m_data.FighterEquip   = tmpFighterEquip;
    m_data.MagicUserEquip = tmpMagicUserEquip;
    m_data.PaladinEquip   = tmpPaladinEquip;
    m_data.RangerEquip    = tmpRangerEquip;
    m_data.ThiefEquip     = tmpThiefEquip;
  }
}


void CGlobalDataDlg::OnConfigmoney() 
{
  CMoneyConfigDlg dlg(m_data.moneyData);
  if (dlg.DoModal()==IDOK)
  {
    UpdateData(TRUE);
    dlg.GetData(m_data.moneyData);
    m_CoinName.Format("Starting %s", m_data.moneyData.GetName(m_data.moneyData.GetDefaultType()));
    UpdateData(FALSE);
  }
}

void CGlobalDataDlg::OnSelecticonbg() 
{
  CScrollPicDlg dlg(m_data.IconBgArt, rte.IconArtDir(), "");

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    m_data.IconBgArt = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_IconBG.SetWindowText(name);
  }		
}
/*
void CGlobalDataDlg::OnSelecttitle() 
{
  CScrollPicDlg dlg(m_data.TitleBgArt);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    m_data.TitleBgArt = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_Title.SetWindowText(name);
  }	
}
*/
void CGlobalDataDlg::OnScreenbg() 
{
  CScrollPicDlg dlg(m_data.BackgroundArt, rte.BackgroundArtDir(), "");

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    m_data.BackgroundArt = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_ScreenBG.SetWindowText(name);
  }		
}

void CGlobalDataDlg::OnAreaViewArt() 
{
  CScrollPicDlg dlg(m_data.m_MapArt, rte.MapArtDir(), "");

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    m_data.m_MapArt = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    StripFilenamePath(m_data.m_MapArt);
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_AreaViewArt.SetWindowText(name);
  }		
}

void CGlobalDataDlg::OnCredits() 
{
  CScrollPicDlg dlg(m_data.CreditsBgArt, rte.BackgroundArtDir(), "");

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    m_data.CreditsBgArt = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_Credits.SetWindowText(name);
  }		
}

void CGlobalDataDlg::OnDifflvl() 
{
  CDiffLvlSettings dlg(m_data.m_diffLvlData);
  if (dlg.DoModal()==IDOK)
    dlg.GetData(m_data.m_diffLvlData);
}

void CGlobalDataDlg::OnEditglobalattributes() 
{
  CEditAttributes dlg(m_data.global_asl, "");
  if (dlg.DoModal()==IDOK)
    dlg.GetData(m_data.global_asl);	
}

void CGlobalDataDlg::OnMousecursor() 
{
  CScrollPicDlg dlg(CursorArt.filename, rte.CursorArtDir(),"");

  if (dlg.DoModal()==IDOK)
  {
    CursorArt.filename = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(dlg.m_Filename, name, MAX_MEDITBUTTON_TEXT);
    m_MouseCursor.SetWindowText(name);
  }			
}

void CGlobalDataDlg::OnSelecttitles() 
{
  CTitleScreenData dlg(m_data.titleData);
  if (dlg.DoModal()==IDOK)
    dlg.GetData(m_data.titleData);
}

void CGlobalDataDlg::OnFixspells() 
{
  m_data.fixSpellBook.spellLimits.SetToMaxLimits();
	CSpell dlg(m_data.fixSpellBook);
  dlg.m_NewTitle = "Choose spells used for 'FIX' menu";

  if (dlg.DoModal() == IDOK)
    dlg.GetSpellList(m_data.fixSpellBook);
}

