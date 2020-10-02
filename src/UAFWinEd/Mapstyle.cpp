/******************************************************************************
* Filename: Mapstyle.cpp
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
#include "ScrollPicDlg.h"
#include "SoundChooser.h"
#include "SoundEventDlg.h"

#include "MapStyle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MapStyle dialog


MapStyle::MapStyle(LEVEL_STATS &data, BOOL AllowStyleChange, CWnd* pParent /*=NULL*/)
	: CDialog(MapStyle::IDD, pParent)
{
	//{{AFX_DATA_INIT(MapStyle)
	m_mapHeight = data.area_height;
	m_mapWidth = data.area_width;
	m_Type = (data.overland?OVERLAND_TYPE:DUNGEON_TYPE);
	m_UseDefBG = TRUE;
	m_MapName = data.level_name;
	m_HeightText = _T("");
	m_WidthText = _T("");
	m_EndTime = 0;
	m_StartTime = 0;
	m_UseNightMusic = FALSE;
	m_SupressBumpSound = TRUE;
	m_SupressStepSound = FALSE;
	//}}AFX_DATA_INIT
  m_data = data;
  m_ovName = "";
  m_AllowStyleChange = AllowStyleChange;

  m_mapHeight = max(10, m_mapHeight);
  m_mapHeight = min(MAX_AREA_HEIGHT, m_mapHeight);
  m_mapWidth  = max(10, m_mapWidth);
  m_mapWidth  = min(MAX_AREA_WIDTH, m_mapWidth);

  m_OrigWidth = m_mapWidth;
  m_OrigHeight = m_mapHeight;

  m_SupressBumpSound = data.level_asl.Find("Supress Bump Sound") != NULL;
  m_SupressStepSound = data.level_asl.Find("Supress Step Sound") != NULL;
}


void MapStyle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MapStyle)
	DDX_Control(pDX, IDC_STEPSOUND, m_StepSound);
	DDX_Control(pDX, IDC_BUMPSOUND, m_BumpSound);
	DDX_Control(pDX, IDC_AVStyleList, m_AVStyleList);
	DDX_Control(pDX, IDC_LOADOVERLANDMAP, m_Load);
	DDX_Text(pDX, IDC_MAPHEIGHT, m_mapHeight);
	DDX_Text(pDX, IDC_MAPWIDTH, m_mapWidth);
	DDX_Radio(pDX, IDC_DUNGEON, m_Type);
	DDX_Check(pDX, IDC_USEDEFBG, m_UseDefBG);
	DDX_Text(pDX, IDC_MAPNAME, m_MapName);
	DDX_Text(pDX, IDC_HEIGHTTEXT, m_HeightText);
	DDX_Text(pDX, IDC_WIDTHTEXT, m_WidthText);
	DDX_Text(pDX, IDC_NIGHTTIMEEND, m_EndTime);
	DDX_Text(pDX, IDC_NIGHTTIMESTART, m_StartTime);
	DDX_Check(pDX, IDC_USENIGHTSOUNDS, m_UseNightMusic);
	DDX_Check(pDX, IDC_SupressBumpSound, m_SupressBumpSound);
	DDX_Check(pDX, IDC_SupressStepSound, m_SupressStepSound);
	//}}AFX_DATA_MAP

	//DDV_MinMaxUInt(pDX, m_mapHeight, 10, MAX_AREA_HEIGHT);
	//DDV_MinMaxUInt(pDX, m_mapWidth, 10, MAX_AREA_WIDTH);
}


BEGIN_MESSAGE_MAP(MapStyle, CDialog)
	//{{AFX_MSG_MAP(MapStyle)
	ON_BN_CLICKED(IDC_DUNGEON, OnDungeon)
	ON_BN_CLICKED(IDC_USEDEFBG, OnUsedefbg)
	ON_BN_CLICKED(IDC_LOADOVERLANDMAP, OnLoadoverlandmap)
	ON_BN_CLICKED(IDC_OVERLAND, OnOverland)
	ON_BN_CLICKED(IDC_STEPSOUND, OnStepsound)
	ON_BN_CLICKED(IDC_BUMPSOUND, OnBumpsound)
	ON_BN_CLICKED(IDC_BACKGROUNDSOUNDS, OnBackgroundsounds)
	ON_BN_CLICKED(IDC_NIGHTSOUNDS, OnNightsounds)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MapStyle message handlers

BOOL MapStyle::OnInitDialog() 
{
	CDialog::OnInitDialog();	
  char name[MAX_MEDITBUTTON_TEXT+1];

  m_WidthText.Format("Width (10-%i)", MAX_AREA_WIDTH);
  m_HeightText.Format("Height (10-%i)", MAX_AREA_HEIGHT);

  if (m_Type == OVERLAND_TYPE) // overland
  {
    if (m_ovName=="")
#ifdef SIMPLE_STRUCTURE
      m_ovName = ede.TemplateOverlandArtDir() + DEFAULT_OVERLAND;
#else
      m_ovName.Format("%s%s", EditorArt, DEFAULT_OVERLAND);
#endif
    m_UseDefBG = FALSE;
    m_Load.EnableWindow(TRUE);
    getBaseName(m_ovName, name, MAX_MEDITBUTTON_TEXT);
    m_Load.SetWindowText(name); // button name
    CWnd *pWnd = GetDlgItem(IDC_MAPHEIGHT);
    if (pWnd != NULL)
      pWnd->EnableWindow(FALSE);    
    pWnd = GetDlgItem(IDC_MAPWIDTH);
    if (pWnd != NULL)
      pWnd->EnableWindow(FALSE);
  }
  else
    m_Load.EnableWindow(FALSE);

  m_AVStyleList.ResetContent();
  for (int i=0;i<MAX_GLOBAL_AVSTYLES;i++)
    m_AVStyleList.AddString(GlobalAVStyleText[i]);

  m_AVStyleList.SetCurSel(m_data.AVStyle);

  if (m_AllowStyleChange)
  {
    MessageBox("You must make the decision now as to whether this is to be a dungeon level "
               "or a wilderness level.  You will not be allowed to change this decison without "
               "deleting the entire level and starting over.","Warning", MB_OK);
  };


  if (!m_AllowStyleChange)
  {
    //m_Load.EnableWindow(FALSE);
    CWnd *pWnd = GetDlgItem(IDC_USEDEFBG);
    if (pWnd != NULL)
      pWnd->EnableWindow(FALSE);    
    pWnd = GetDlgItem(IDC_DUNGEON);
    if (pWnd != NULL)
      pWnd->EnableWindow(FALSE);
    pWnd = GetDlgItem(IDC_OVERLAND);
    if (pWnd != NULL)
      pWnd->EnableWindow(FALSE);
  }

  getBaseName(m_data.StepSound, name, MAX_MEDITBUTTON_TEXT);
  m_StepSound.SetWindowText(name); // button name
  getBaseName(m_data.BumpSound, name, MAX_MEDITBUTTON_TEXT);
  m_BumpSound.SetWindowText(name); // button name

  m_UseNightMusic=m_data.bgSounds.UseNightMusic;
  m_EndTime = m_data.bgSounds.EndTime;
  m_StartTime = m_data.bgSounds.StartTime;

  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MapStyle::OnUsedefbg() 
{
  UpdateData(TRUE);
  if (m_Type == OVERLAND_TYPE) // overland
    m_UseDefBG = FALSE;
  UpdateData(FALSE);	
}

void MapStyle::OnLoadoverlandmap() 
{
  if (m_Type == DUNGEON_TYPE)
    return;

  CScrollPicDlg dlg(m_ovName, rte.OverlandArtDir(), "");

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {   
    int w = dlg.m_srcRect.Width();
    int h = dlg.m_srcRect.Height();

    if ((w > 0) && (h > 0))
    {
      m_ovName = dlg.m_Filename;        
      
      /*
      CString tmp(dlg.m_Filename);
      CopyFileToFolder(tmp, EditorArt, "");      
      StripFilenamePath(tmp);
      m_ovName.Format("%s%s", EditorArt, tmp);
      */

      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_ovName, name, MAX_MEDITBUTTON_TEXT);
      m_Load.SetWindowText(name); // button name

      m_mapWidth = w / SQUARESIZE;
      m_mapHeight = h / SQUARESIZE;

      if (m_mapWidth > MAX_AREA_WIDTH)
        m_mapWidth = MAX_AREA_WIDTH;
      if (m_mapHeight > MAX_AREA_HEIGHT)
        m_mapHeight = MAX_AREA_HEIGHT;

      m_OrigWidth = m_mapWidth;
      m_OrigHeight = m_mapHeight;
      UpdateData(FALSE);
    }
  }	
}

void MapStyle::OnDungeon() 
{
  UpdateData(TRUE);
  m_Type = DUNGEON_TYPE;
  m_UseDefBG = TRUE;
  m_Load.EnableWindow(FALSE);
  CWnd *pWnd = GetDlgItem(IDC_MAPWIDTH);
  if (pWnd != NULL)
    pWnd->EnableWindow(TRUE);
  pWnd = GetDlgItem(IDC_MAPHEIGHT);
  if (pWnd != NULL)
    pWnd->EnableWindow(TRUE);
  pWnd = GetDlgItem(IDC_USEDEFBG);
  if (pWnd != NULL)
    pWnd->EnableWindow(TRUE);
  UpdateData(FALSE);
}

void MapStyle::OnOverland() 
{
  UpdateData(TRUE);  
  m_Type = OVERLAND_TYPE;
  m_UseDefBG = FALSE;
  m_Load.EnableWindow(TRUE);
  
  CWnd *pWnd = GetDlgItem(IDC_MAPWIDTH);
  if (pWnd != NULL)
    pWnd->EnableWindow(FALSE);
  pWnd = GetDlgItem(IDC_MAPHEIGHT);
  if (pWnd != NULL)
    pWnd->EnableWindow(FALSE);
  pWnd = GetDlgItem(IDC_USEDEFBG);
  if (pWnd != NULL)
    pWnd->EnableWindow(FALSE);

  if (m_ovName=="")
#ifdef SIMPLE_STRUCTURE
    m_ovName = ede.TemplateOverlandArtDir() + DEFAULT_OVERLAND;
#else
    m_ovName.Format("%s%s", EditorArt, DEFAULT_OVERLAND);
#endif

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_ovName, name, MAX_MEDITBUTTON_TEXT);
  m_Load.SetWindowText(name); // button name

  // need to determine width/height based on image size  
  RECT SrcRect;
  long Dib = GraphicsMgr.AddFileSurface(m_ovName, BufferDib, rte.OverlandArtDir(), ede.TemplateOverlandArtDir());
  GraphicsMgr.GetSurfaceRect(Dib, &SrcRect);
  GraphicsMgr.ReleaseSurface(Dib);

  int w = SrcRect.right - SrcRect.left;
  int h = SrcRect.bottom - SrcRect.top;

  m_mapWidth = w / SQUARESIZE;
  m_mapHeight = h / SQUARESIZE;

  if (m_mapWidth > MAX_AREA_WIDTH)
    m_mapWidth = MAX_AREA_WIDTH;
  if (m_mapHeight > MAX_AREA_HEIGHT)
    m_mapHeight = MAX_AREA_HEIGHT;

  m_OrigWidth = m_mapWidth;
  m_OrigHeight = m_mapHeight;

  UpdateData(FALSE);	
}

void MapStyle::OnOK() 
{
  UpdateData(TRUE);

  if (m_Type == OVERLAND_TYPE)
  {
    m_mapWidth = m_OrigWidth;
    m_mapHeight = m_OrigHeight;
  }

  m_mapHeight = max(10, m_mapHeight);
  m_mapHeight = min(MAX_AREA_HEIGHT, m_mapHeight);
  m_mapWidth  = max(10, m_mapWidth);
  m_mapWidth  = min(MAX_AREA_WIDTH, m_mapWidth);

  m_data.AVStyle = (GlobalAreaViewStyle)m_AVStyleList.GetCurSel();
  m_data.area_height = m_mapHeight;
  m_data.area_width = m_mapWidth;
  m_data.level_name = m_MapName;
  m_data.overland = (m_Type==OVERLAND_TYPE);
  m_data.bgSounds.UseNightMusic = m_UseNightMusic;
  m_data.bgSounds.EndTime = m_EndTime;
  m_data.bgSounds.StartTime = m_StartTime;

  m_data.level_asl.Delete("Supress Step Sound");
  if (m_SupressStepSound) 
        m_data.level_asl.Insert("Supress Step Sound","",ASLF_DESIGN);
  m_data.level_asl.Delete("Supress Bump Sound");
  if (m_SupressBumpSound) 
        m_data.level_asl.Insert("Supress Bump Sound","",ASLF_DESIGN);

	CDialog::OnOK();
}

void MapStyle::OnStepsound() 
{
  CSoundChooser dlg(m_data.StepSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_data.StepSound = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_data.StepSound, name, MAX_MEDITBUTTON_TEXT);
    m_StepSound.SetWindowText(name);	
  }	
}

void MapStyle::OnBumpsound() 
{
  CSoundChooser dlg(m_data.BumpSound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_data.BumpSound = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_data.BumpSound, name, MAX_MEDITBUTTON_TEXT);
    m_BumpSound.SetWindowText(name);	
  }		
}

void MapStyle::OnBackgroundsounds() 
{	
  UpdateData(TRUE);
  SOUND_EVENT tempEvent;
  POSITION pos = m_data.bgSounds.backgroundSounds.sounds.GetHeadPosition();
  while (pos != NULL)
  {
    tempEvent.sounds.AddTail(m_data.bgSounds.backgroundSounds.sounds.GetAt(pos));
    m_data.bgSounds.backgroundSounds.sounds.GetNext(pos);
  }

  CSoundEventDlg dlg(tempEvent);
  if (dlg.DoModal()==IDOK)
  {
    m_data.bgSounds.backgroundSounds.Clear();
    dlg.GetData(tempEvent);

    pos = tempEvent.sounds.GetHeadPosition();
    while (pos != NULL)
    {
      m_data.bgSounds.backgroundSounds.sounds.AddTail(tempEvent.sounds.GetAt(pos));
      tempEvent.sounds.GetNext(pos);
    }
  }
  UpdateData(FALSE);
}

void MapStyle::OnNightsounds() 
{
  UpdateData(TRUE);
  SOUND_EVENT tempEvent;
  POSITION pos = m_data.bgSounds.nightSounds.sounds.GetHeadPosition();
  while (pos != NULL)
  {
    tempEvent.sounds.AddTail(m_data.bgSounds.nightSounds.sounds.GetAt(pos));
    m_data.bgSounds.nightSounds.sounds.GetNext(pos);
  }

  CSoundEventDlg dlg(tempEvent);
  if (dlg.DoModal()==IDOK)
  {
    m_data.bgSounds.nightSounds.Clear();
    dlg.GetData(tempEvent);

    pos = tempEvent.sounds.GetHeadPosition();
    while (pos != NULL)
    {
      m_data.bgSounds.nightSounds.sounds.AddTail(tempEvent.sounds.GetAt(pos));
      tempEvent.sounds.GetNext(pos);
    }
  }	

  m_UseNightMusic = (m_data.bgSounds.nightSounds.sounds.GetCount()>0);
  UpdateData(FALSE);
}
