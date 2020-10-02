/******************************************************************************
* Filename: MainFrm.cpp
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
#pragma warning (disable:4091)
#include "imagehlp.h"

#include "UAFWinEd.h"
//#include <shlwapi.h>

#include "UAFWinEdDoc.h"
#include "UAFWinEdView.h"
#include "GameEvent.h"

#include "CharacterEditor.h"
#include "ChurchDlg.h"
#include "ShopDlg.h"
#include "SpellDlg.h"
#include "ItemEditor.h"
#include "EventViewer.h"
#include "EditWallSlots.h"
#include "EditBGSlots.h"
#include "ItemKeyQuestDlg.h"
#include "GlobalDataDlg.h"
#include "DesignList.h"
#include "SelectLevel.h"
#include "ZoneDlg.h"
#include "SmallPicPoolDlg.h"
#include "IconPicPoolDlg.h"
#include "MonsterEditor.h"
#include "SpellEditor.h"
#include "DbUtils.h"
#include "EntryPointDlg.h"
#include "StepEventDlg.h"
#include "Graphics.h"
#include "MainFrm.h"
#include "FileParse.h"
#include "ItemDB.h"
#include "VersionDlg.h"
#include "ImportFRUAData.h"
#include "ConfigFile.h"
#include "AskSureDlg.h"
#include "SoundMgr.h"
#include "ScrollPicDlg.h"
#include "Party.h"
#include "EditorOptions.h"
#include "DesignStatsDlg.h"
#include "JournalDataDlg.h"
#include "GameResChange.h"
#include "TestPartySelectDlg.h"
#include "SearchParamDlg.h"
#include "XBrowseForFolder.h"
#include "CrossReference.h"
#include "SelectFolderDialog.h"
#include "ChooseSpeclAbDlg.h"
//#include "..\UAFWinEd\Resource.h"
#include "..\UAFWinEd\ImportWarning.h"

extern HBRUSH m_ColorBrushes[MAX_COLOR_SLOTS];
extern void TestAllSpecialAbilities(A_SPECABILITY_DEF_L *pAbilities);

extern const double VersionSpellNames;
extern const double VersionSpellIDs;



void CrossReferenceResources(CR_LIST *pCRList);
void CrossReferenceSavedCharacters(CR_LIST *pCRList);
void CheckSkills(void);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL ScreenModeChanged = FALSE;
BOOL IsTopMost=FALSE;
//BOOL IsDCConfigPresent = FALSE;
BOOL IsUAFConfigPresent = FALSE;

CList <CString, CString&> PartyList;

DWORD timerId;
const DWORD SCREEN_CAP_TIMER = 121933;

void PumpWinMsgs()
{

  MSG msg;
  while (::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
  {
    if (GetMessage(&msg, NULL, 0, 0) > 0)
    {
       ::TranslateMessage(&msg);
       ::DispatchMessage(&msg);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_COMMAND(ID_ITEMS_EVENT, OnItemsEvent)
	ON_UPDATE_COMMAND_UI(ID_ITEMS_EVENT, OnUpdateItemsEvent)
	ON_COMMAND(ID_ITEMS_STARTLOC, OnItemsStartloc)
	ON_UPDATE_COMMAND_UI(ID_ITEMS_STARTLOC, OnUpdateItemsStartloc)
	ON_COMMAND(ID_ITEMS_WALL, OnItemsWall)
	ON_UPDATE_COMMAND_UI(ID_ITEMS_WALL, OnUpdateItemsWall)
	ON_COMMAND(ID_ITEM_ZONE, OnItemZone)
	ON_UPDATE_COMMAND_UI(ID_ITEM_ZONE, OnUpdateItemZone)
	ON_COMMAND(ID_ITEMS_BACKDROP, OnItemsBackdrop)
	ON_UPDATE_COMMAND_UI(ID_ITEMS_BACKDROP, OnUpdateItemsBackdrop)
	ON_COMMAND(ID_VIEW_BACKDROPS, OnViewBackdrops)
	ON_COMMAND(ID_VIEW_WALLSETS, OnViewWallsets)
	ON_COMMAND(ID_VIEW_ZONES, OnViewZones)
	ON_COMMAND(ID_FILE_OPEN_MAP, OnFileOpenMap)
	ON_COMMAND(ID_PASSTHRUWALLS, OnPassthruwalls)
	ON_UPDATE_COMMAND_UI(ID_PASSTHRUWALLS, OnUpdatePassthruwalls)
	ON_COMMAND(ID_VIEW_CHARS, OnViewChars)
	ON_COMMAND(ID_ITEMS_ACCESS, OnItemsAccess)
	ON_UPDATE_COMMAND_UI(ID_ITEMS_ACCESS, OnUpdateItemsAccess)
	ON_COMMAND(ID_VIEW_ENTRYPOINTS, OnViewEntrypoints)
	ON_COMMAND(ID_VIEW_GLOBALDATA, OnViewGlobaldata)
	ON_COMMAND(ID_VIEW_KEYS, OnViewKeys)
	ON_COMMAND(ID_VIEW_LEVELDATA, OnViewLeveldata)
	ON_COMMAND(ID_VIEWAREAMAP, OnViewareamap)
	ON_UPDATE_COMMAND_UI(ID_VIEWAREAMAP, OnUpdateViewareamap)
	ON_COMMAND(ID_FILE_SAVE_AS_MAP, OnFileSaveAsMap)
	ON_COMMAND(ID_FILE_SAVE_MAP, OnFileSaveMap)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_MAP, OnUpdateFileSaveMap)
	ON_COMMAND(ID_VIEW_SPIC_POOL, OnViewSpicPool)
	ON_COMMAND(ID_VIEW_ICON_POOL, OnViewIconPool)
	ON_COMMAND(ID_EDIT_ITEMS, OnEditItems)
	ON_COMMAND(ID_EDIT_MONSTERS, OnEditMonsters)
	ON_COMMAND(ID_EDIT_SPELLS, OnEditSpells)
  ON_COMMAND(ID_EDIT_SPECIAL_ABILITIES, OnEditSpecialAbilities)
  ON_COMMAND(ID_TEST_SPECIAL_ABILITIES, OnTestSpecialAbilities)
	ON_COMMAND(ID_IMPORTEXPORT, OnImportexport)
  ON_COMMAND(IDM_CheckSkills, OnCheckSkills)
	ON_COMMAND(ID_ITEMS_ENTRYPOINT, OnItemsEntrypoint)
	ON_UPDATE_COMMAND_UI(ID_ITEMS_ENTRYPOINT, OnUpdateItemsEntrypoint)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ENTRYPOINTS, OnUpdateViewEntrypoints)
	ON_COMMAND(ID_CLEAR_LEVELDATA, OnClearLeveldata)
	ON_UPDATE_COMMAND_UI(ID_CLEAR_LEVELDATA, OnUpdateClearLeveldata)
	ON_COMMAND(ID_VIEW_STEPEVENTS, OnViewStepevents)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STEPEVENTS, OnUpdateViewStepevents)
	ON_COMMAND(ID_CLEAR_DESIGNDATA, OnClearDesigndata)
	ON_COMMAND(ID_FILE_NEW_MAP, OnFileNewMap)
	ON_COMMAND(ID_DOUBLEFILLWALLSLOT, OnDoublefillwallslot)
	ON_UPDATE_COMMAND_UI(ID_DOUBLEFILLWALLSLOT, OnUpdateDoublefillwallslot)
	ON_COMMAND(ID_VIEW_EVENTS, OnViewEvents)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EVENTS, OnUpdateViewEvents)
	ON_WM_CREATE()
	ON_COMMAND(ID_UNDOLAST, OnUndolast)
	ON_UPDATE_COMMAND_UI(ID_UNDOLAST, OnUpdateUndolast)
	ON_COMMAND(ID_VALIDATE_DESIGN, OnValidateDesign)
	ON_COMMAND(ID_GAME_VERSION, OnGameVersion)
//	ON_COMMAND(ID_IMPORTFRUA, OnImportfrua)
	ON_UPDATE_COMMAND_UI(IDC_BLOCK_Type, OnUpdateBLOCKType)
	ON_COMMAND(IDC_BLOCK_Type, OnBLOCKType)
	ON_UPDATE_COMMAND_UI(IDC_KwikKlik, OnUpdateKwikKlik)
	ON_COMMAND(IDC_KwikKlik, OnKwikKlik)
	ON_UPDATE_COMMAND_UI(ID_VALIDATE_DESIGN, OnUpdateValidateDesign)
	ON_COMMAND(IDC_TILEMAPDATA, OnTilemapdata)
	ON_UPDATE_COMMAND_UI(IDC_TILEMAPDATA, OnUpdateTilemapdata)
	//ON_COMMAND(ID_EDIT_CONFIG, OnEditConfig)
	//ON_COMMAND(ID_RELOAD_CONFIG, OnReloadConfig)
	ON_COMMAND(ID_HOMEPAGE, OnHomepage)
	ON_COMMAND(ID_GLOBAL_TESTSAVEDDESIGN, OnGlobalTestsaveddesign)
	ON_COMMAND(ID_GLOBAL_TESTSAVEDDESIGNFROMSTART, OnGlobalTestsaveddesignfromstart)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WALLSETS, OnUpdateViewWallsets)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BACKDROPS, OnUpdateViewBackdrops)
	ON_COMMAND(ID_DOWNLOADS, OnDownloads)
	ON_COMMAND(ID_LOG_EVENT_TEXT, OnLogEventText)
	ON_UPDATE_COMMAND_UI(ID_LOG_EVENT_TEXT, OnUpdateLogEventText)
	ON_COMMAND(ID_QUADFILLBGSLOT, OnQuadfillbgslot)
	ON_UPDATE_COMMAND_UI(ID_QUADFILLBGSLOT, OnUpdateQuadfillbgslot)
	ON_COMMAND(ID_FILLALLMAPDISTANTBGOFF, OnFillallmapdistantbgoff)
	ON_COMMAND(ID_FILLALLMAPDISTANTBGON, OnFillallmapdistantbgon)
	ON_COMMAND(ID_FILLALLMAPZONE, OnFillallmapzone)
	ON_UPDATE_COMMAND_UI(ID_FILLALLMAPDISTANTBGON, OnUpdateFillallmapdistantbgon)
	ON_UPDATE_COMMAND_UI(ID_FILLALLMAPDISTANTBGOFF, OnUpdateFillallmapdistantbgoff)
	ON_UPDATE_COMMAND_UI(ID_FILLALLMAPZONE, OnUpdateFillallmapzone)
	ON_COMMAND(ID_TOGGLE_TOPMOST, OnToggleTopmost)
	ON_UPDATE_COMMAND_UI(ID_TOGGLE_TOPMOST, OnUpdateToggleTopmost)
	ON_COMMAND(ID_LAUNCHUACONFIG, OnLaunchuaconfig)
	ON_COMMAND(ID_FILLALLBACKDROP_ALLDIR, OnFillallbackdropAlldir)
	ON_UPDATE_COMMAND_UI(ID_FILLALLBACKDROP_ALLDIR, OnUpdateFillallbackdropAlldir)
	ON_COMMAND(ID_FILLALLBACKDROP_EASTDIR, OnFillallbackdropEastdir)
	ON_COMMAND(ID_FILLALLBACKDROP_NORTHDIR, OnFillallbackdropNorthdir)
	ON_COMMAND(ID_FILLALLBACKDROP_SOUTHDIR, OnFillallbackdropSouthdir)
	ON_COMMAND(ID_FILLALLBACKDROP_WESTDIR, OnFillallbackdropWestdir)
	ON_UPDATE_COMMAND_UI(ID_FILLALLBACKDROP_WESTDIR, OnUpdateFillallbackdropWestdir)
	ON_UPDATE_COMMAND_UI(ID_FILLALLBACKDROP_SOUTHDIR, OnUpdateFillallbackdropSouthdir)
	ON_UPDATE_COMMAND_UI(ID_FILLALLBACKDROP_NORTHDIR, OnUpdateFillallbackdropNorthdir)
	ON_UPDATE_COMMAND_UI(ID_FILLALLBACKDROP_EASTDIR, OnUpdateFillallbackdropEastdir)
	ON_COMMAND(ID_FILLUNUSEDBACKDROP_ALLDIR, OnFillunusedbackdropAlldir)
	ON_UPDATE_COMMAND_UI(ID_FILLUNUSEDBACKDROP_ALLDIR, OnUpdateFillunusedbackdropAlldir)
	ON_COMMAND(ID_FILLUNUSEDBACKDROP_EASTDIR, OnFillunusedbackdropEastdir)
	ON_UPDATE_COMMAND_UI(ID_FILLUNUSEDBACKDROP_EASTDIR, OnUpdateFillunusedbackdropEastdir)
	ON_COMMAND(ID_FILLUNUSEDBACKDROP_NORTHDIR, OnFillunusedbackdropNorthdir)
	ON_UPDATE_COMMAND_UI(ID_FILLUNUSEDBACKDROP_NORTHDIR, OnUpdateFillunusedbackdropNorthdir)
	ON_COMMAND(ID_FILLUNUSEDBACKDROP_SOUTHDIR, OnFillunusedbackdropSouthdir)
	ON_UPDATE_COMMAND_UI(ID_FILLUNUSEDBACKDROP_SOUTHDIR, OnUpdateFillunusedbackdropSouthdir)
	ON_COMMAND(ID_FILLUNUSEDBACKDROP_WESTDIR, OnFillunusedbackdropWestdir)
	ON_UPDATE_COMMAND_UI(ID_FILLUNUSEDBACKDROP_WESTDIR, OnUpdateFillunusedbackdropWestdir)
	ON_COMMAND(ID_APP_HELPCONTENTS, OnAppHelpcontents)
	ON_COMMAND(ID_GLOBAL_VIEWARTASTRANSPARENT, OnGlobalViewartastransparent)
	ON_COMMAND(ID_EDITOROPTIONS, OnEditoroptions)
	ON_UPDATE_COMMAND_UI(ID_LAUNCHUACONFIG, OnUpdateLaunchuaconfig)
	ON_COMMAND(ID_LAUNCHDCCONFIG, OnLaunchdcconfig)
	ON_UPDATE_COMMAND_UI(ID_LAUNCHDCCONFIG, OnUpdateLaunchdcconfig)
	ON_COMMAND(ID_GAME_STATS, OnGameStats)
	ON_COMMAND(ID_VIEWJOURNAL, OnViewjournal)
	ON_UPDATE_COMMAND_UI(ID_GLOBAL_TESTSAVEDDESIGN, OnUpdateGlobalTestsaveddesign)
	ON_UPDATE_COMMAND_UI(ID_GLOBAL_TESTSAVEDDESIGNFROMSTART, OnUpdateGlobalTestsaveddesignfromstart)
	ON_COMMAND(ID_APP_SCREENCAPTURE, OnAppScreencapture)
	ON_WM_TIMER()
	ON_COMMAND(ID_GLOBAL_SETTESTPARTY, OnGlobalSettestparty)
	ON_COMMAND(ID_GLOBAL_TESTSAVEDDESIGN_VIEWERRORLOG, OnGlobalTestsaveddesignViewerrorlog)
	ON_COMMAND(ID_GAME_ERRORLOG, OnGameErrorlog)
	//}}AFX_MSG_MAP
  // Global help commands
  ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
  ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
  ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
  ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)

  ON_MESSAGE(ID_DOSCREENCAP,  OnDoScreenCap)
  ON_MESSAGE(MSG_CLOSESEARCHDLG, OnMsgCloseSearchDlg)      

END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction


CMainFrame::CMainFrame() : CFrameWnd()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
  // *** this code requires VC6 or higher to compile
  //
	//if (!m_wndToolBar.CreateEx(this, 
  //                           TBSTYLE_FLAT, 
  //                           WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))      
	//{
	//	TRACE0("Failed to create toolbar\n");
	//	return -1;      // fail to create
	//}

	if (!m_wndToolBar.Create(this, 
                           WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

  if (!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to load toolbar\n");
		return -1;      // fail to load
	}

  if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Delete these three lines if you don't want the toolbar to
	// be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);    

  CString appName;  
  appName = ede.EditorExecutableDir();
  appName += "UAFConfig\\UAFConfig.exe";
  IsUAFConfigPresent = FileExists(appName);
  
  //appName = m_installDir;
  //appName += "\\UAFConfig\\DCConfig.exe";
  //IsDCConfigPresent = FileExists(appName);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{ 
  //cs.dwExStyle |= WS_EX_TOPMOST;
  cs.style |= (WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
	return (CFrameWnd::PreCreateWindow(cs));
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


bool AskAreYouSure(void)
{
  static bool dontask=false;
  if (dontask) return true;
  AskSureDlg dlg;
  int result=dlg.DoModal();
  if (dlg.m_dontAsk) dontask=true;
  return (result==IDOK);
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

//*****************************************************************************
// NAME:    CMainFrame::OnItemsEvent
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnItemsEvent() 
{
  flushOldImages();
	m_mode = EVENT_MODE;
   if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
      ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
}

//*****************************************************************************
// NAME:    CMainFrame::OnUpdateItemsEvent
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdateItemsEvent(CCmdUI* pCmdUI) 
{
  if (m_mode == EVENT_MODE)
     pCmdUI->SetCheck(TRUE);
  else
     pCmdUI->SetCheck(FALSE);	
}

//*****************************************************************************
// NAME:    CMainFrame::OnItemsStartloc
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnItemsStartloc() 
{
  flushOldImages();
	m_mode = START_LOC_MODE;	
   if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
      ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
}

//*****************************************************************************
// NAME:    CMainFrame::OnUpdateItemsStartloc
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdateItemsStartloc(CCmdUI* pCmdUI) 
{
  /*
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->SetCheck(FALSE);
    pCmdUI->Enable(FALSE);
    return;
  }
  else*/
    pCmdUI->Enable(TRUE);


  if (m_mode == START_LOC_MODE)
      pCmdUI->SetCheck(TRUE);
  else
      pCmdUI->SetCheck(FALSE);
}

//*****************************************************************************
// NAME:    CMainFrame::OnItemsWall
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnItemsWall() 
{
   flushOldImages();
   m_mode = WALL_MODE;
   if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
      ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
}

//*****************************************************************************
// NAME:    CMainFrame::OnUpdateItemsWall
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdateItemsWall(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->SetCheck(FALSE);
    pCmdUI->Enable(FALSE);
    return;
  }
  else
    pCmdUI->Enable(TRUE);

  if (m_mode == WALL_MODE)
      pCmdUI->SetCheck(TRUE);
  else
      pCmdUI->SetCheck(FALSE);
}

//*****************************************************************************
// NAME:    CMainFrame::OnItemZone
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnItemZone() 
{
   flushOldImages();
   m_mode = ZONE_MODE;	
   if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
      ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
}

//*****************************************************************************
// NAME:    CMainFrame::OnUpdateItemZone
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdateItemZone(CCmdUI* pCmdUI) 
{
  if (m_mode == ZONE_MODE)
      pCmdUI->SetCheck(TRUE);
  else
      pCmdUI->SetCheck(FALSE);	
}

//*****************************************************************************
// NAME:    CMainFrame::OnItemsBackdrop
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnItemsBackdrop() 
{
  flushOldImages();
  if (m_mapType == OVERLAND_TYPE)
    m_mode = BLOCKAGE_MODE;
  else
    m_mode = BG_MODE;	

  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
    ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
}

//*****************************************************************************
// NAME:    CMainFrame::OnUpdateItemsBackdrop
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdateItemsBackdrop(CCmdUI* pCmdUI) 
{
  if (m_mode == BLOCKAGE_MODE)
  {
    pCmdUI->SetCheck(TRUE);
    pCmdUI->SetText("Place &Blockage");
  }
  else if (m_mode == BG_MODE)
  {
    pCmdUI->SetCheck(TRUE);
    pCmdUI->SetText("Place &Backdrop");
  }
  else
    pCmdUI->SetCheck(FALSE);
}

//*****************************************************************************
// NAME:    CMainFrame::OnItemsEntrypoint
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnItemsEntrypoint() 
{	
  flushOldImages();
  m_mode = ENTRY_POINT_MODE;	

  CUAFWinEdView* pView = (CUAFWinEdView*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
  if (pView != NULL)
    pView->m_oldEP = -1;


  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
    ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
}

//*****************************************************************************
// NAME:    CMainFrame::OnUpdateItemsEntrypoint
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdateItemsEntrypoint(CCmdUI* pCmdUI) 
{
    if (m_mode == ENTRY_POINT_MODE)
        pCmdUI->SetCheck(TRUE);
    else
        pCmdUI->SetCheck(FALSE);
}

//*****************************************************************************
// NAME:    CMainFrame::OnViewBackdrops
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnViewBackdrops() 
{
  CEditBGSlots dlg;
  if (dlg.DoModal() == IDOK)
  {
    CUAFWinEdView* pView = (CUAFWinEdView*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
    if (pView != NULL) pView->m_oldBG = -1;
    currBkGnd = dlg.m_Slot;

    GraphicsMgr.ReleaseSurface(BackgroundSets[currBkGnd].backgroundSurfaceAlt);
    BackgroundSets[currBkGnd].backgroundFileAltHash=0;
    //SurfaceType type = (BackgroundSets[currBkGnd].useTransparency?TransBufferDib:BackGndDib);
    //BackgroundSets[currBkGnd].backgroundSurfaceAlt = GraphicsMgr.AddSurface(BackgroundSets[currBkGnd].backgroundFileAlt, type);
    BackgroundSets[currBkGnd].GetBackgroundAltSurface();

    if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
      ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
  }
  flushOldImages();
}

//*****************************************************************************
// NAME:    CMainFrame::OnViewWallsets
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnViewWallsets() 
{
  CEditWallSlots dlg;
  if (dlg.DoModal() == IDOK)
  {
    currWallSlot = dlg.m_Slot;
    currBlockage = dlg.m_Obstruction;
    CUAFWinEdView* pView = (CUAFWinEdView*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
    if (pView != NULL) pView->m_oldWall = -1;    
    if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
      ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
  }
  flushOldImages();
}

//*****************************************************************************
// NAME:    CMainFrame::OnViewZones
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnViewZones() 
{
   CZoneDlg dlg(levelData.zoneData, currZone);

   DWORD result = dlg.DoModal();

   if (result == IDOK)
   {
      CUAFWinEdView* pView = (CUAFWinEdView*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
      if (pView != NULL)
        pView->m_oldZone = -1;

      SET_MODIFIED;
      levelData.zoneData = dlg.m_zones;
      currZone = dlg.m_ZoneNum-1;

      if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
         ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
   }
   flushOldImages();
}





//*****************************************************************************
// NAME:    CMainFrame::OnFileSaveAsMap
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnFileSaveAsMap() 
{
  // save folder and designName are not attached anymore
  // but we need something in there as default
  WriteDebugString("Entering 'OnFileSaveAsMap'\n");
  SaveAs(rte.DesignDir());
  WriteDebugString("Exiting 'OnFileSaveAsMap'\n");
  //SaveAs("");  //  

}




//#ifndef __AFX_H__
//#include "windows.h"
//#include "crtdbg.h"
//#include "tchar.h"
//#endif

#include "Shlobj.h"
//#include "io.h"

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  // If the BFFM_INITIALIZED message is received
  // set the path to the start path.
  switch (uMsg)
  {
    case BFFM_INITIALIZED:
    {
      if (NULL != lpData)
      {
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
      }
    }
  }

  return 0; // The function should always return 0.
}


void CMainFrame::SaveAs(const CString& suggestedDesignDir) 
{
  // The suggested design directory should already exist!
  //DWORD result;
  char designDir[_MAX_PATH + 1];
  CString newFolder;
  CString initFolder;
  BOOL success = TRUE;
  WriteDebugString("Entering SaveAs(%s)\n", suggestedDesignDir);
  if (globalData.designName.IsEmpty())
    globalData.designName = "Default Design";

  //CSelectFolderDialog newFolderDialog(FALSE, suggestedDesignDir,
	//					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	//					NULL, NULL);

  //newFolderDialog.SetTitle("Select Folder for New Design");
/*
  CFileDialog dlg (  false,                // Save Diialog
                    "",                    // Default extension (ignored)
                    "Filename Ignored",    // Initial filename (ignored)
                    0,                     // No flags
                    NULL,                  // No Filter
                    NULL                   // Parent window
                  );
*/
  if (suggestedDesignDir != "")
  {
    initFolder = suggestedDesignDir;
  }
  else
  {
    initFolder = rte.RootDir();
  };
//  dlg.m_ofn.lpstrInitialDir = initFolder;
  if (globalData.designName == "Untitled")
  {
    int ans;
    ans = MessageBox("This design has name 'Untitled'.  Do you really want to save it with this name?",
                     "Warning",
                     MB_YESNO | MB_ICONWARNING);
    if (ans == IDNO) return;
  };
  //result = newFolderDialog.DoModal();
  CString name(GetDesignFolderRoot(NULL));

//  dlg.m_ofn.lpstrTitle = "Save As (Provide a folder name with extension 'dsn'.  File name is ignored!)";
//  result = dlg.DoModal();
  WriteDebugString("SaveAs is about to browse for a folder\n");
  {
    BROWSEINFO bi = { 0 };
    char szCurrent[MAX_PATH+1];
    strcpy(szCurrent, rte.DesignDir());
    bi.ulFlags |= BIF_NEWDIALOGSTYLE;
    bi.lpszTitle = "Select a Folder to Contain the Design";
    bi.lpfn = BrowseCallbackProc;
    bi.lParam = (LPARAM)szCurrent;
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    success = FALSE;
    if (pidl != 0)
    {
      // get the name of the folder
      //TCHAR path[MAX_PATH+1];
      if (SHGetPathFromIDList(pidl, designDir))
      {
        success = TRUE;
      }

      // free memory used
      IMalloc * imalloc = 0;
      if (SUCCEEDED(SHGetMalloc(&imalloc)))
      {
        imalloc->Free(pidl);
        imalloc->Release();
      }
    }
  };




  //success = XBrowseForFolder(
  //  NULL,
  //  initFolder,
  //  -1,
  //  "Search for new game folder",
  //  designDir,
  //  MAX_PATH,
  //  TRUE,
  //  FALSE);



  //if (result == IDOK)
  if (success)
  {
    //PumpWinMsgs();
    RUNTIME_ENVIRONMENT myRte;
    CWaitCursor cursor;
    CString pathname;
    CString possibleDesignDirectory;
//    pathname = dlg.GetPathName();
    //pathname = newFolderDialog.GetSelectedPath();
    pathname = designDir;
    WriteDebugString("SaveAs BrowseForFolder Succeeded - path=%s\n", pathname);
    CString temp;
    temp = pathname;
    temp.MakeUpper();
    if (temp.Find("PROGRAM FILES", 0) < 0) return;
    {
      CImportWarning dlg;
      int answer;
      bool correctResponse;
      dlg.m_warning = "What you are doing can be very upsetting.\r\n "
        "Windows does not like you to put things in its Program Files\r\n"
        "directory.  Vista, in fact, will not allow it and will hide your\r\n"
        "files in:\r\n"
        "C:\\users\\username\\AppData\\Local\\VirtualStore\\Program Files\\directoryName\r\n"
        "and never tell you what it has done to make your life more difficult.\r\n"
        "This is just a warning.  You can do what you wish.\r\n"
			  "Enter the number you see below and press 'OK'.";
      do
      {
        correctResponse = true;
        dlg.DoModal();
        if ( (sscanf(dlg.m_numeric,"%d",&answer) != 1) || (answer!=dlg.m_i))
        {
          MsgBoxInfo("Wrong answer....sorry");
          correctResponse = false;
        };
      } while (!correctResponse);
    };



    if (pathname.Right(1) == "\\")
    {
      pathname = pathname.Left(pathname.GetLength()-1);
    };
    if (!FileExists(pathname))
    {
      WriteDebugString("SaveAs - No such folder exists\n");
      MsgBoxInfo("No such folder exists","Save Failed");
      return;
    }
    else
    {
      //CString oldFolder = m_designFolder;
      possibleDesignDirectory = pathname;
      if (possibleDesignDirectory.Right(1) == "\\")
      {
        possibleDesignDirectory = possibleDesignDirectory.Left(possibleDesignDirectory.GetLength()-1);
      };
      if (   (possibleDesignDirectory.GetLength() == 2)
          && (possibleDesignDirectory.GetAt(1) == ':'))
      {
        possibleDesignDirectory += '\\';
      };
      if (FileExists(possibleDesignDirectory))
      {
        DWORD attr;
        attr = GetFileAttributes(possibleDesignDirectory);
        if (attr & FILE_ATTRIBUTE_DIRECTORY)
        {
          // It better be a directory!!!
          // Record the various design directory names.
          myRte.DefaultFoldersFromDesign(possibleDesignDirectory + '\\');
          //
          // Now look in the directory and see if it is empty.
          if (!FolderIsEmpty(myRte.DesignDir()))
          {
            //{
            //  MessageBox("You cannot write over designs until the editor is better tested","Error",MB_OK);
            //  return;
            //};
            // It appears that we are trying to save this design
            // in an existing folder with existing files.
            int ans;
            CString msg;
            msg.Format("That folder is not empty.  Do you want overwrite the information in the folder named:\n%s\n",
                             myRte.DesignDir());
            ans = MessageBox(msg,
                             "Warning",
                             MB_YESNO | MB_ICONQUESTION);
            if (ans == IDNO)
            {
              return;
            };
            //else
            //{
            //  forceCopy = true;
            //};
          }
          else
          {
            // Attempting to 'Save As' in existing directory.
            // No 'game.dat' file exists here already.
            // All is well - proceed.
          };
        }
        else  // design directory is not a directory!
        {
          WriteDebugString("SaveAs - File is not a directory\n");
          NotImplemented(0xcc4433,FALSE);
        };
      }
      else
      {
        NotImplemented(0xff41ab, FALSE);
      };
    };


    newFolder = myRte.DesignDir();
    newFolder = newFolder.Left(newFolder.GetLength()-1);

    if (!FileExists(newFolder))
    {
      if (_mkdir(newFolder) != 0)
      {
        NotImplemented(0x98a34b, false);
      };
    };


    if (myRte.DesignDir() != rte.DesignDir())
    {
      // copy existing design files to new location
      if (rte.DesignDir() == "")
      {
        // We appear to be saving a new design.
        // 'WallArtDir' covers all files in 'Resources' directory
        if (!CopyDesignFiles(myRte.WallArtDir()))
        {
          return;
        };
      }
      else
      {
        // Copy files from existing design.
        CString designPath;
        designPath = myRte.DesignDir();
        if (designPath.GetAt(designPath.GetLength()-1) == '\\')
        {
          designPath = designPath.Left(designPath.GetLength()-1);
        };
        if (!CopyDesignFiles(designPath))
        {
          return;
        };
      };
    }

    // save current level
    // Switch runtime environments.
    rte = myRte;
    success = saveDesign();

    if (success)
    {
      // Copy the config if it does not already exist.
      CopyFile(ede.TemplateDataDir()+"config.txt", rte.ConfigDir()+"config.txt",TRUE);
    };

    if (success)
    {
      CLEAR_MODIFIED;
      if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
         ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
      CloseErrorLog();
      UseErrorLog = TRUE;
      WriteDebugString("Finished 'Save As' operation\n");
    }
  }
  else
  {
    WriteDebugString("BrowseForFolder failed\n");
  };

  if (!success)
    MsgBoxError("Could not save game design", "Save Design Error");
  ((CUAFWinEdApp*)AfxGetApp())->SetTitle();
}









//*****************************************************************************
// NAME:    CMainFrame::OnFileSaveMap
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnFileSaveMap() 
{
  BOOL success = TRUE;
  //{
 	//    MsgBoxInfo("You cannot save until this editor has further testing");
	//    return;
  //};
  
  WriteDebugString("Entering 'OnFileSaveMap'\n");
  if (globalData.designName.IsEmpty())
  {
    SaveAs("");
    WriteDebugString("Exiting 'OnFileSaveMap'\n");
    return;
  }


  if (globalData.version < VersionSpellNames)
  {  
    AfxGetApp()->DoWaitCursor(-1);  
    CImportWarning dlg;
    int answer;
    dlg.m_warning = "You have just converted a design from an old format to\r\n"
                    "a new format.  This is not very well tested!.  You are\r\n"
                    "about to write over the top of the old design and it will\r\n"
                    "be gone forever.  If anything goes wrong, your design will\r\n"
                    "be lost!  You should probably save your design in a different\r\n"
                    "place using 'Save As'.  If you really want to destroy your\r\n"
                    "old design then you can.  But you should back it up first!";
    if (dlg.DoModal() != IDOK)
    {
	    MsgBoxInfo("That is a wise choice");
	    return;
    };
    if ( (sscanf(dlg.m_numeric,"%d",&answer) != 1) || (answer!=dlg.m_i))
    {
        MsgBoxInfo("Wrong answer....sorry");
        return;
    };
  };



  CWaitCursor cursor;
  success = saveDesign();
  if (success)
  {
    CLEAR_MODIFIED;
    if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
       ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
  }

  if (!success)
      MsgBoxError("Could not save game design", "Save Design Error");
  ((CUAFWinEdApp*)AfxGetApp())->SetTitle();
  WriteDebugString("Exiting 'OnFileSaveMap'\n");
}


void RUNTIME_ENVIRONMENT::DefaultFoldersFromRoot(const CString& root, const CString& name)
{
  m_rootDir = root;
  DefaultFoldersFromDesign(m_rootDir + name + "\\");
}

BOOL CMainFrame::LoadDesign(LPCSTR name, int startLvl)
{
  BOOL success = TRUE;
  CWaitCursor cursor;   
  DWORD lvlCount=0;
  BOOL UsingGlobalStart=TRUE;

  CopyEventData.Clear();
  levelData.Clear();
  UndoData.RemoveAll();
  
  CString msg;
  msg.Format("Begin loading design data for %s\n",name);
  WriteDebugString(msg);   
  
  if (loadDesign(name))
  {
    WriteDebugString("Finished loading design data\n");

    //CString temp;

    //StripOneFolderFromFilenamePath(name, temp);
    rte.DefaultFoldersFromDesign(name);


    bool found=false;
    int lvl;

    if (startLvl >= 0)
    {
      lvl = startLvl;
      UsingGlobalStart=FALSE;
    }
    else
    {
      lvl = globalData.startLevel;
      UsingGlobalStart=TRUE;
    }

    while ((!found)&&(lvlCount<MAX_LEVELS))
    {
      if (levelExists(lvl))
      {
         WriteDebugString("About to call LoadLevel for LEVEL%.3i.LVL.\n", lvl+1);

         if (LoadLevel(lvl))
         {
           found=true;
           CLEAR_MODIFIED;
           globalData.currLevel = lvl;

           if (!globalData.levelInfo.stats[lvl].used)
           {
             WriteDebugString("Adjusting globalData level 'Used' to TRUE for start level\n");
             globalData.levelInfo.stats[lvl].used=TRUE;
           }

           if ((UsingGlobalStart)&&(lvl!=globalData.startLevel))
           {               
             CString message;
             message.Format("The original start level %i could not be loaded.\nIt has been changed to be level %i\n",
                        globalData.startLevel+1, lvl+1);               
             MsgBoxError(message);
             globalData.startLevel=lvl;
           }
         }
      }
      else
      {
        if ((UsingGlobalStart)&&(lvl==globalData.startLevel))
        {
          WriteDebugString("Start level LEVEL%.3i.LVL doesn't exist, searching for valid level\n", lvl+1);
        }
        else if ((!UsingGlobalStart) && (lvl==startLvl))
        {
          WriteDebugString("Start level LEVEL%.3i.LVL doesn't exist, searching for valid level\n", lvl+1);
        }
        else if (globalData.levelInfo.stats[lvl].used)
        {
          WriteDebugString("Level LEVEL%.3i.LVL doesn't exist, searching for valid level\n", lvl+1);
        }
      }

      lvl++;lvlCount++;
      // no need to retry same level
      if ((UsingGlobalStart) && (lvl==globalData.startLevel)) lvl++;
      if ((!UsingGlobalStart) && (lvl==startLvl)) lvl++;

      if (lvl >= MAX_LEVELS) lvl=0;
    }

    if (!found)
    {
       MsgBoxError("Failed to load start level for design","Level Loading Error");
       clearDesign();
       success = FALSE;
    }
  }
  else
  {
    MsgBoxError("Failed to load design data file","Design Loading Error");
    clearDesign();
    success = FALSE;
  }

  //if (success)
  {
    m_mapType = (globalData.levelInfo.stats[globalData.currLevel].overland ? OVERLAND_TYPE : DUNGEON_TYPE);

    if (m_mapType == OVERLAND_TYPE)
    {
      if (m_mode == WALL_MODE)
        m_mode = BLOCKAGE_MODE;
    }
  }

  return success;
}

#define xbrowseOpen
//*****************************************************************************
// NAME:    CMainFrame::OnFileOpenMap
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnFileOpenMap() 
{


#ifdef xbrowseOpen
char designDir[MAX_PATH+1], newDesignDir[MAX_PATH+1];
  if (!XBrowseForFolder(NULL, rte.RootDir(), -1, "Search for Design folder", designDir, _MAX_PATH, TRUE, FALSE))
  {
    return;
  };
#else
  char newDesignDir[_MAX_PATH+1];
  CSelectFolderDialog selectFolderDialog(FALSE,rte.RootDir(),
    						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                NULL,NULL);
  selectFolderDialog.SetTitle("Select Design Folder to Open");
  if (selectFolderDialog.DoModal() == IDOK)
#endif
  { 

#ifdef xbrowseOpen
    {
      int len;
      strcpy(newDesignDir, designDir);
      len = strlen(newDesignDir);
      if (len > 0)
      {
        if (newDesignDir[len-1] != '\\')
        {
          strcat(newDesignDir, "\\");
        };
      };
    };
#else
    {
      int len;
      strcpy(newDesignDir, selectFolderDialog.GetSelectedPath());
      len = strlen(newDesignDir);
      if (len > 0)
      {
        if (newDesignDir[len-1] != '\\')
        {
          strcat(newDesignDir, "\\");
        };
      };
    };
#endif
    /*
    if (!globalData.designName.IsEmpty() && IS_MODIFIED)
    {
      if (globalData.designName.CompareNoCase("Default Design")!=0)
      {
        CString msg;
        msg = "You have made changes to this design.\n"
              "Do you want to save it?";
        int response = MsgBoxYesNo(msg, "Design Warning");
        if (response == IDYES)
        {
          PumpWinMsgs();
          CWaitCursor cursor;
          if (!saveDesign())
            MsgBoxError("Failed to save design", "Save Error");
          else
            CLEAR_MODIFIED;
        }
      }
    }
    */
    if (CheckSaveChangedDesign("Do you want to discard the changes and open the new design?"))
    {

      PumpWinMsgs();

      rte.DefaultFoldersFromDesign(newDesignDir);
      if (!LoadConfigFile(rte.ConfigDir()))
      {
        MessageBox("Cannot load Design"," Fatal Error", MB_ICONSTOP|MB_OK);
        abort();
      };
      {
        for (int i=0;i<MAX_COLOR_SLOTS;i++)
        {
          ::DeleteObject(m_ColorBrushes[i]);
          m_ColorBrushes[i] = ::CreateSolidBrush(m_Colors[i]);
        };
      };


      if (LoadDesign(newDesignDir))
      {
        currX = globalData.startX;
        currY = globalData.startY;

        if ((currX < 0) || (currX >= globalData.levelInfo.stats[globalData.startLevel].area_width))
        {
          currX  = max(0, currX);
          currX  = min(globalData.levelInfo.stats[globalData.startLevel].area_width-1, currX);
        }
        if ((currY < 0) || (currY >= globalData.levelInfo.stats[globalData.startLevel].area_height))
        {
          currY  = max(0, currY);
          currY  = min(globalData.levelInfo.stats[globalData.startLevel].area_height-1, currY);
        }
        currFacing = globalData.startFacing;
        party.facing = currFacing;
        party.Posy = currY;
        party.Posx = currX;
      }
      else
      {
        if (!LoadDesign(ede.TemplateDir()))
        {
          MessageBox("Cannot load Default Design"," Fatal Error", MB_ICONSTOP|MB_OK);
          abort();
        };
      };
    
      WriteDebugString("About to call ResetView.\n");
      CUAFWinEdView* pView = (CUAFWinEdView*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
      if (pView != NULL) pView->ResetView();

      WriteDebugString("About to call UpdateAllViews.\n");
      if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
        ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
    }
  };
  ((CUAFWinEdApp*)AfxGetApp())->SetTitle();
  WriteDebugString("Exiting CMainFrame::OnFileOpenMap\n");
}


BOOL CMainFrame::LoadTemplateDesign(void)
{
  BOOL success;
  rte.DefaultFoldersFromDesign(ede.TemplateDir());
  success = LoadDesign(rte.DesignDir());
  return success;
}

BOOL CMainFrame::ConfigureNewDesign(CGlobalDataDlg *dlg)
{
  do
  {
    int result;
    result = dlg->DoModal();
    if (result == IDCANCEL)
    {
      return FALSE;
    };
    dlg->GetData(globalData);
    if (globalData.designName == "")
    {
      MsgBoxInfo("You must supply a 'Design Name'","Error");
    };
  } while (globalData.designName == "");

  return TRUE;
}

BOOL CMainFrame::SaveNewDesign(void)
{
  bool folderCreated;
  CString root;
  RUNTIME_ENVIRONMENT newRte;
  StripOneFolderFromFilenamePath(rte.RootDir(), root);
  newRte.DefaultFoldersFromRoot(root, globalData.designName + ".dsn");
  if (FileExists(newRte.DesignDir().Left(newRte.DesignDir().GetLength()-1)))
  {
    folderCreated = false;
  }
  else
  {
    folderCreated = true;
    MakeSureDirectoryPathExists(newRte.DesignDir());
  };
  SaveAs(newRte.DesignDir());
    //
    // If we created a folder for the design
    // AND the user designated a different folder
    // AND the folder we created is empty
    // THEN delete the folder we created.
  if (    folderCreated
       && (newRte.DesignDir() != rte.DesignDir())
       && FolderIsEmpty(newRte.DesignDir())
     )
  {
    rmdir(newRte.DesignDir());
  };
  return FALSE;
}


//*****************************************************************************
// NAME:    CMainFrame::OnNewdesign
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnFileNewMap() 
{

  /*
  if (!globalData.designName.IsEmpty() && IS_MODIFIED)
  {
    CString temp;
    temp.Format("Creating a new design will clear current design data.\nSave current design - %s?", 
                globalData.designName);

    int response = MsgBoxYesNo(temp, "Design Warning");

    if (response == IDYES)
    {
      PumpWinMsgs();
      CWaitCursor cursor;
      if (!saveDesign())
      {
        MsgBoxError("Failed to save design", "Save Error");
      }
      else
        CLEAR_MODIFIED;
    }
  }
  */
  CheckSaveChangedDesign("Do you want to discard the changes and open a new design?");

  LoadTemplateDesign();
  globalData.designName = "";
  {
    CGlobalDataDlg dlg(globalData);
    ConfigureNewDesign(&dlg);
  };
  SaveNewDesign();
  ((CUAFWinEdApp*)AfxGetApp())->SetTitle();
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
      ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);

}

//*****************************************************************************
// NAME:    CMainFrame::OnPassthruwalls
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnPassthruwalls() 
{
	PassThruWalls = !PassThruWalls;	
}

//*****************************************************************************
// NAME:    CMainFrame::OnUpdatePassthruwalls
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdatePassthruwalls(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->SetCheck(FALSE);
    pCmdUI->Enable(FALSE);
    return;
  }
  else
    pCmdUI->Enable(TRUE);

	pCmdUI->SetCheck(PassThruWalls);	
}

//*****************************************************************************
// NAME:    CMainFrame::OnViewChars
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnViewChars() 
{
	CCharacterEditor dlg;
  dlg.DoModal();
  flushOldImages();
}

//*****************************************************************************
// NAME:    CMainFrame::OnItemsAccess
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnItemsAccess() 
{
   m_accessMode=!m_accessMode;
   if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
      ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
}

//*****************************************************************************
// NAME:    CMainFrame::OnUpdateItemsAccess
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdateItemsAccess(CCmdUI* pCmdUI) 
{  
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->SetCheck(FALSE);
    pCmdUI->Enable(FALSE);
    return;
  }

  pCmdUI->Enable( (m_mode == WALL_MODE) );
  if (m_mode == WALL_MODE)
    pCmdUI->SetCheck(m_accessMode);
}

//*****************************************************************************
// NAME:    CMainFrame::OnViewEntrypoints
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnViewEntrypoints() 
{
	CEntryPointDlg dlg(globalData.levelInfo.stats[globalData.currLevel], currEntryPoint);

  if (dlg.DoModal() == IDOK)
  {
     dlg.GetData(globalData.levelInfo.stats[globalData.currLevel]);
     SET_MODIFIED;

     if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
       ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
  }
  flushOldImages();
}

//*****************************************************************************
// NAME:    CMainFrame::OnViewGlobaldata
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnViewGlobaldata() 
{
	CGlobalDataDlg dlg(globalData);	
	if (dlg.DoModal() == IDOK)
  {
		dlg.GetData(globalData);	
    SET_MODIFIED;
    if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
    {
      ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);   
      ((CUAFWinEdApp*)AfxGetApp())->SetTitle();
    }
  }
  flushOldImages();
}

//*****************************************************************************
// NAME:    CMainFrame::OnViewKeys
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnViewKeys() 
{
	CItemKeyQuestDlg dlg(ITEM_FLAG | KEY_FLAG | QUEST_FLAG,&globalData);	
	if (dlg.DoModal()==IDOK)
  { 
    dlg.GetData(&globalData);
  };
  flushOldImages();
}

void CheckForUnsavedLevel(bool quiet)
{
  if (globalData.levelInfo.stats[globalData.currLevel].used && IS_MODIFIED)
  {
    int response = IDYES;
    //if (m_SaveWarn)
    if (!quiet)
    {
      CString temp;
      temp.Format("Do you want to save current level - %s?", 
                  globalData.levelInfo.stats[globalData.currLevel].level_name);

      response = MsgBoxYesNo(temp, "Level Warning");
    }
    
    if (response == IDYES)
    {
      if (globalData.designName.IsEmpty())
        globalData.designName = "Default Design";

      AfxGetApp()->DoWaitCursor(1);
      BOOL success = saveDesign();
      //BOOL success = saveLevel(levelData, globalData.currLevel);
      AfxGetApp()->DoWaitCursor(-1);

      if (success)
        CLEAR_MODIFIED;
      else
        MsgBoxError("Failed to save design", "Save Error");
    }
  }
}

//*****************************************************************************
// NAME:    CMainFrame::OnViewLeveldata
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnViewLeveldata() 
{
  BOOL ok = FALSE;

  // until the editor gets better error handling, it is important
  // that a valid level is loaded or created before leaving this
  // function. Thus, the while loop.
  while (!ok)
  {
    // this dialog is not supposed to exit until a valid 'used'
    // level is chosen. That doesn't guarantee the level will load.
    CString ovName="";
    if (globalData.levelInfo.stats[globalData.currLevel].overland)
      ovName = BackgroundSets[1].backgroundFile;

    levelData.GetSlots();

    CSelectLevel dlg(globalData.currLevel);  
    dlg.m_ovName = ovName;

    DWORD result = dlg.DoModal();

    if ((result == IDOK) || (globalData.currLevel != dlg.m_level))
    {    
      if (globalData.currLevel != dlg.m_level)
      {
        CheckForUnsavedLevel(false);

        currX=0;
        currY=0;
        currFacing=FACE_EAST;
        party.facing = currFacing;
        // if level exists, load it
        // else working with new un-saved level
        if (levelExists(dlg.m_level))
        {
          if (!LoadLevel(dlg.m_level))
          {
            MsgBoxError("Failed to load level", "Level Loading Error");
          }
          else
          {
            ok = TRUE; // existing level loaded ok
          }
        }
        else
          ok = TRUE; // new level     

        if (ok)
          globalData.currLevel = dlg.m_level; // new or loaded level
      }
      else
      {
        levelData.SetSlots();
        ok=TRUE; // using same level
      }
    }
    else
    {
      levelData.SetSlots();
      ok = TRUE; // still using same level
    }
  }

  m_mapType = (globalData.levelInfo.stats[globalData.currLevel].overland ? OVERLAND_TYPE : DUNGEON_TYPE);

  if (m_mapType == OVERLAND_TYPE)
    m_mode = BLOCKAGE_MODE;
  else
    m_mode = WALL_MODE;  

  levelData.area_height = globalData.levelInfo.stats[globalData.currLevel].area_height;
  levelData.area_width = globalData.levelInfo.stats[globalData.currLevel].area_width;

  if ((currX < 0) || (currX >= levelData.area_width))
    currX = 0;
  if ((currY < 0) || (currY >= levelData.area_height))
    currY = 0;

  if (pWallSlotMgr!=NULL) pWallSlotMgr->Clear(); 
  if (pBgSlotMgr!=NULL) pBgSlotMgr->Clear();

  CUAFWinEdView* pView = (CUAFWinEdView*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
  if (pView != NULL)
    pView->ResetView();

  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
     ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);   
}

//*****************************************************************************
// NAME:    CMainFrame::OnViewareamap
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnViewareamap() 
{
  globalData.useAreaView = !globalData.useAreaView;	

  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
    ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
}

//*****************************************************************************
// NAME:    CMainFrame::OnUpdateViewareamap
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdateViewareamap(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->SetCheck(FALSE);
    pCmdUI->Enable(FALSE);
    return;
  }
  else
    pCmdUI->Enable(TRUE);

  pCmdUI->SetCheck(globalData.useAreaView);
}

//*****************************************************************************
// NAME:    CMainFrame::OnUpdateFileSaveMap
//
// PURPOSE: Disable save when using editor's default design
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdateFileSaveMap(CCmdUI* pCmdUI) 
{
  
  pCmdUI->Enable(
               (rte.DesignDir() != ede.TemplateDir())
            && (rte.DesignDir() != ""));
  return;
}




//*****************************************************************************
// NAME:    CMainFrame::OnViewSpicPool
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnViewSpicPool() 
{
  CSmallPicPoolDlg dlg;
  dlg.DoModal();
  flushOldImages();
}

//*****************************************************************************
// NAME:    CMainFrame::OnViewIconPool
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnViewIconPool() 
{
  CIconPicPoolDlg dlg;
  dlg.DoModal();
  flushOldImages();
}


//*****************************************************************************
// NAME:    CMainFrame::OnEditItems
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnEditItems() 
{ 
  ITEM_DATA_TYPE tempItemData;
  BOOL SaveChangeFlag = IS_MODIFIED;
  CItemEditor dlg;

  tempItemData=itemData;  // Save in case edit is canceled.
  dlg.m_pItemData=&itemData;
  if (dlg.DoModal()!=IDOK)
  {
    if (!(tempItemData==itemData))  // Was anything changed?
    {
      if (AskAreYouSure()) // Are you sure you want to lose your changes?
      {
        itemData=tempItemData;  // Restore old values
        if (!SaveChangeFlag) CLEAR_MODIFIED;
      };
    };
  };
  flushOldImages();
}

//*****************************************************************************
// NAME:    CMainFrame::OnEditMonsters
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnEditMonsters() 
{ 
  MONSTER_DATA_TYPE tempMonsterData;
  BOOL SaveChangeFlag = IS_MODIFIED;
  CMonsterEditor dlg;

  tempMonsterData=monsterData;  // Save in case edit is canceled.
  dlg.m_pMonsterData=&monsterData;
  if (dlg.DoModal()!=IDOK)
  {
    if (!(tempMonsterData==monsterData))  // Was anything changed?
    {
      if (AskAreYouSure()) // Are you sure you want to lose your changes?
      {
        monsterData=tempMonsterData;  // Restore old values
        if (!SaveChangeFlag) CLEAR_MODIFIED;
      };
    };
  };
  flushOldImages();
}


//*****************************************************************************
// NAME:    CMainFrame::OnEditSpells
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnEditSpells() 
{ 
  SPELL_DATA_TYPE tempSpellData;
  BOOL  SaveChangeFlag = IS_MODIFIED;
  CSpellEditor dlg;

  tempSpellData=spellData;  // Save in case edit is canceled.
  dlg.m_pSpellData=&spellData;
  if (dlg.DoModal()!=IDOK)
  {
    if (!(tempSpellData==spellData))  // Was anything changed?
    {
      if (AskAreYouSure()) // Are you sure you want to lose your changes?
      {
        if (!SaveChangeFlag) CLEAR_MODIFIED;
        spellData=tempSpellData;  // Restore old values
      };
    };
  };
  flushOldImages();
}


#ifdef OLD_SPECIAL_ABILITIES
#else
//*****************************************************************************
// NAME:    CMainFrame::OnEditSpecialAbilities
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnEditSpecialAbilities() 
{ 
  A_SPECABILITY_DEF_L temp;
  temp.Copy(specialAbilitiesData);
  CChooseSpeclAbDlg dlg(temp,"Edit Special Abilities");
  if (dlg.DoModal() == IDOK)
  {
    if (dlg.GetData(specialAbilitiesData)) SET_MODIFIED;
  };
}

//*****************************************************************************
// NAME:    CMainFrame::OnTestSpecialAbilities
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnTestSpecialAbilities() 
{ 
  A_SPECABILITY_DEF_L temp;
  temp.Copy(specialAbilitiesData);
  TestAllSpecialAbilities(&temp);
}


#endif

//*****************************************************************************
// NAME:    CMainFrame::OnImportexport
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnImportexport() 
{
  CDbUtils dlg;
  dlg.DoModal();
  flushOldImages();
}

void CMainFrame::OnCheckSkills(void)
{
  CheckSkills();
}

//*****************************************************************************
// NAME:    CMainFrame::OnUpdateViewEntrypoints
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdateViewEntrypoints(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(TRUE);
}
//*****************************************************************************
// NAME:    CMainFrame::OnUpdateClearLeveldata
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdateClearLeveldata(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(TRUE);
}
//*****************************************************************************
// NAME:    CMainFrame::OnClearLeveldata
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnClearLeveldata() 
{
   if (MsgBoxYesNo("Clear current level and reset to defaults?", "Level Warning") == IDYES)
   {
     PumpWinMsgs();
     CWaitCursor cursor;
     SET_MODIFIED;
     
     //flushOldImages();
     if (pWallSlotMgr!=NULL) pWallSlotMgr->Clear(); 
     if (pBgSlotMgr!=NULL) pBgSlotMgr->Clear();

     levelData.Clear();
     fillDefaultArtSlots(levelData,globalData.levelInfo.stats[globalData.currLevel].overland);
     levelData.SetSlots();
     if (loadWallSetSurfaces())
     {
       currBlockage = BlockedBlk;
       currWallSlot = FindFirstLoadedWall();
       currBkGnd = FindFirstLoadedBackground();
       if (currWallSlot < 0) currWallSlot=0;
       if (currBkGnd < 0) currBkGnd=0;
     }

     fillDefaultLevelData(levelData, globalData.currLevel, 
                          globalData.levelInfo.stats[globalData.currLevel].area_width, 
                          globalData.levelInfo.stats[globalData.currLevel].area_height, 
                          globalData.levelInfo.stats[globalData.currLevel].overland, 
                          TRUE);        

      levelData.GetSlots();

      CUAFWinEdView* pView = (CUAFWinEdView*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
      if (pView != NULL)
        pView->ResetView();

      if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
      {
        ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
        ((CUAFWinEdApp*)AfxGetApp())->SetTitle();
      }
   }
}
//*****************************************************************************
// NAME:    CMainFrame::OnClearDesigndata
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnClearDesigndata() 
{
   if (MsgBoxYesNo("Clear current design?\nThis action will reset defaults and clear all levels.", 
                    "Design Warning") == IDYES)
   {
      PumpWinMsgs();
      CWaitCursor cursor;
      SET_MODIFIED;
      LoadConfigFile(ConfigFile.GetFilename());      
      
      CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
      pApp->LoadDefaultDBFiles(); 

      clearDesign(TRUE);     
      for (int i=0;i<MAX_LEVELS;i++)
      {
        if (!globalData.levelInfo.stats[i].used)
          deleteLevel(i);
      }
      
      //flushOldImages();
      if (pWallSlotMgr!=NULL) pWallSlotMgr->Clear(); 
      if (pBgSlotMgr!=NULL) pBgSlotMgr->Clear();

      CUAFWinEdView* pView = (CUAFWinEdView*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
      if (pView != NULL)
        pView->ResetView();

      if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
      {
        ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
        ((CUAFWinEdApp*)AfxGetApp())->SetTitle();
      }
      if (MsgBoxYesNo("Would you also like to delete all files in the design folder?", 
                       "Delete Design Files Warning") == IDYES)
      {
         CString path = GetDesignPath(NULL);
         DeleteAllFilesInFolder(path);	
      }
   }
}

//*****************************************************************************
// NAME:    CMainFrame::OnViewStepevents
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnViewStepevents() 
{
   CStepEventDlg dlg(levelData);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(levelData);	
      SET_MODIFIED;
   }
   flushOldImages();
}

//*****************************************************************************
// NAME:    CMainFrame::OnUpdateViewStepevents
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::OnUpdateViewStepevents(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewEvents() 
{
  CEventViewer dlg(currX,currY);
  dlg.DoModal();	
  flushOldImages();
}

void CMainFrame::OnUpdateViewEvents(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnDoublefillwallslot() 
{
  DoubleFillWallSlot = !DoubleFillWallSlot;	
}

void CMainFrame::OnUpdateDoublefillwallslot(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->SetCheck(FALSE);
    pCmdUI->Enable(FALSE);
    return;
  }
  else
    pCmdUI->Enable(TRUE);

  pCmdUI->SetCheck(DoubleFillWallSlot);	
}

extern CString UAInstallPath;

void CMainFrame::SaveUAFolder()
{
  if (UAInstallPath.IsEmpty()) return;

  CString RegStr(UAInstallPath);
  CRegKey key;
  key.Create(HKEY_CURRENT_USER,"SOFTWARE\\UAFEditor");
  key.SetValue(RegStr, "UAInstall");
  key.Close();
}

void CMainFrame::RestoreUAFolder()
{
  char temp[_MAX_PATH+1];
  DWORD len = sizeof(temp);
  CRegKey key;
  key.Create(HKEY_CURRENT_USER, "SOFTWARE\\UAFEditor");
  DWORD rc = key.QueryValue(temp, "UAInstall", &len);
  key.Close();

  if (rc != ERROR_SUCCESS) return;
  if (strlen(temp) == 0) return;
  UAInstallPath = temp;
}

// used to save and restore the positions of open windows
static const TCHAR RegFmtStr[] = "%d,%d,%d,%d,%d,%d,%d,%d,%d";

//*****************************************************************************
// NAME:    CMainFrame::SaveState
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************

void CMainFrame::SaveState()
{
  if (ScreenModeChanged) return;
  WINDOWPLACEMENT wp;
  wp.length = sizeof(WINDOWPLACEMENT);
  GetWindowPlacement(&wp);
  CString RegStr;
  RegStr.Format(RegFmtStr,
                wp.rcNormalPosition.top,
                wp.rcNormalPosition.left,
                wp.rcNormalPosition.bottom,
                wp.rcNormalPosition.right,
                wp.ptMinPosition.x,
                wp.ptMinPosition.y,
                wp.ptMaxPosition.x,
                wp.ptMaxPosition.y,
                wp.showCmd);

  /*  The following does not work with Visual Studio 2015.
   * I'm going to try using the Windows API to accomplish this.
   * Wish me luck!
   *
   * CRegKey key;
   * key.Create(HKEY_LOCAL_MACHINE,"SOFTWARE\\UAFEditor");
   * key.SetValue(RegStr, "WindowPositions");
   * key.Close();
   */
  LONG status;
  HKEY hkey;
  status = RegOpenKeyEx(HKEY_CURRENT_USER,
                        "SOFTWARE\\UAFEditor",
                        0,
                        KEY_ALL_ACCESS,
                        &hkey);
  if (status == ERROR_FILE_NOT_FOUND)
  {
    status = RegCreateKeyEx(HKEY_CURRENT_USER,
                            "SOFTWARE\\UAFEditor",
                            0,
                            0,
                            REG_OPTION_NON_VOLATILE,
                            KEY_ALL_ACCESS,
                            0,
                            &hkey,
                            0);
  };
  if ((status == ERROR_SUCCESS) && (hkey != NULL))
  {
    status = RegSetValueEx(hkey,
                           "WindowPositions",
                           0,
                           REG_SZ,
                           (const BYTE *)(LPCSTR)RegStr,
                           (DWORD)(RegStr.GetLength() + 1));
    RegCloseKey(hkey);
  }
  else
  {
    MessageBox("Failed to write registry \"WindowPosition\"", "Alert", MB_OK);
  };
}

//*****************************************************************************
// NAME:    CMainFrame::RestoreState
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CMainFrame::RestoreState()
{
  char temp[512];
  DWORD len = sizeof(temp);
  CRegKey key;
  key.Create(HKEY_CURRENT_USER, "SOFTWARE\\UAFEditor");
  DWORD rc = key.QueryValue(temp, "WindowPositions", &len);
  key.Close();

  if (rc != ERROR_SUCCESS)
    return;

  if (strlen(temp) == 0)
    return;

  WINDOWPLACEMENT wp;
  wp.length = sizeof(WINDOWPLACEMENT);
  wp.flags = WPF_SETMINPOSITION;
  sscanf((LPCTSTR)temp,
         RegFmtStr,
         &wp.rcNormalPosition.top,
         &wp.rcNormalPosition.left,
         &wp.rcNormalPosition.bottom,
         &wp.rcNormalPosition.right,
         &wp.ptMinPosition.x,
         &wp.ptMinPosition.y,
         &wp.ptMaxPosition.x,
         &wp.ptMaxPosition.y,
         &wp.showCmd);
  SetWindowPlacement(&wp);
}

void CMainFrame::OnUndolast() 
{
  UndoDataType temp;
  if (UndoData.GetCount() > 0)
  {
    temp = UndoData.RemoveTail();
    UndoAction(temp);	
    if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
      ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
  }
}

void CMainFrame::OnUpdateUndolast(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(UndoData.GetCount() > 0);
}


LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
  switch (message)
  {
  case WM_POWERBROADCAST:
   if(wParam == PBT_APMQUERYSUSPEND)
      return BROADCAST_QUERY_DENY;
   return TRUE;

  case MM_MCINOTIFY:
    MidiStopNotify(wParam, lParam);
    break;

  case WM_SYSCOMMAND:
    // save window state if user clicks on the 'x' to exit editor
    if (wParam == SC_CLOSE)
    {
      DestroySearchDlg();
  /*
      if (!globalData.designName.IsEmpty() && IS_MODIFIED)
      {
        if (globalData.designName.CompareNoCase("Default Design")!=0)
        {
          int response = MsgBoxYesNo("Save current design?", "Design Warning");
          if (response == IDYES)
          {
            CWaitCursor cursor;
            if (!saveDesign())
              MsgBoxError("Failed to save design", "Save Error");
            else
            {
              CLEAR_MODIFIED;
              SaveState();
            };
          };
        };
      };
      */
      if (!CheckSaveChangedDesign("Do you wnat to discard the changes?")) return 0;
    };
    break;

  case WM_SHOWWINDOW:
  case WM_ACTIVATE:
    if ((bActive == FALSE) && ((wParam==WA_ACTIVE)||(wParam==WA_CLICKACTIVE)))
    {
      bActive = TRUE;      
      WriteDebugString("Editor active: %s\n", (bActive?"Yes":"No"));
      if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
        ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
    }
    break;

  case WM_ACTIVATEAPP:
    if ((!bActive)&&(wParam))
    {
      ::SetForegroundWindow( AfxGetMainWnd()->m_hWnd );
      if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
        ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
    }
    bActive = wParam;
    WriteDebugString("Editor active: %s\n", (bActive?"Yes":"No"));
    break;  
  }

  return CFrameWnd::WindowProc(message, wParam, lParam);
}


void CrossReferencePicPool(CR_LIST *pCRList)
{
  CR_REFERENCE crReference;
  POSITION pos;
  crReference.m_referenceName = "PIC Pool";
  crReference.m_referenceType = CR_TYPE_global;
  for (pos = global_SmallPicImport.GetHeadPosition(); pos!=NULL; global_SmallPicImport.GetNext(pos))
  {
    global_SmallPicImport.GetAt(pos)->CrossReference(pCRList, &crReference);
  };
}

void CrossReferenceIconPool(CR_LIST *pCRList)
{
  CR_REFERENCE crReference;
  POSITION pos;
  crReference.m_referenceName = "Icon Pool";
  crReference.m_referenceType = CR_TYPE_global;
  for (pos = global_IconPicImport.GetHeadPosition(); pos!=NULL; global_IconPicImport.GetNext(pos))
  {
    global_IconPicImport.GetAt(pos)->CrossReference(pCRList, &crReference);
  };
}


void CMainFrame::OnValidateDesign() 
{
  EditorStatusMsg("Validating event data...");
  BOOL result = TRUE;
  result = levelData.eventData.Validate();

  if (result)
  {
    EditorStatusMsg("Validation successful");
    MsgBoxInfo("Validation Successful", "Design Validation");
  }
  else
  {
    EditorStatusMsg("Validation failed");
#ifdef VERIFYFILE
    MsgBoxError("Some events/spells/monsters/items are incomplete or invalid.\nCheck the file Verify.txt for details.", "Design Validation");
#else
    MsgBoxError("Some events/spells/monsters/items are incomplete or invalid.\nUse Global/CrossReference for details.", "Design Validation");
#endif
  }
  {
    CR_LIST crList;
    CR_REFERENCE crReference;
    crReference.m_referenceName = "C++code";
    crReference.m_referenceType = CR_TYPE_code;
    crList.CR_AddPicReference(MAPART, &crReference);
    crList.CR_AddPicReference(OVERLANDART, &crReference);
    crList.CR_AddPicReference(AREAVIEWART, &crReference);
    crList.CR_AddPicReference(DEFAULT_TITLE_BG, &crReference);
    crList.CR_AddPicReference(DEFAULT_CREDITS_BG, &crReference);
    crList.CR_AddPicReference(DEFAULT_CC, &crReference);
    crList.CR_AddPicReference(DEFAULT_CDI, &crReference);
    crList.CR_AddPicReference(DEFAULT_CD, &crReference);
    crList.CR_AddPicReference(DEFAULT_CWD, &crReference);
    crList.CR_AddPicReference(DEFAULT_CWW, &crReference);
    crList.CR_AddPicReference(DEFAULT_FR, &crReference);
    crList.CR_AddPicReference(DEFAULT_HB, &crReference);
    crList.CR_AddPicReference(DEFAULT_VB, &crReference);
    crList.CR_AddPicReference(DEFAULT_IB, &crReference);
    crList.CR_AddPicReference(DEFAULT_CA, &crReference);
    crList.CR_AddPicReference(DEFAULT_TR, &crReference);
    crList.CR_AddPicReference(DEFAULT_MONSTER_ICON, &crReference);
    crList.CR_AddPicReference(DEFAULT_IIRT, &crReference);
    crList.CR_AddPicReference(DEFAULT_IIRS, &crReference);
    crList.CR_AddPicReference(DEFAULT_IIRA, &crReference);
    crList.CR_AddPicReference(DEFAULT_SIR, &crReference);
    crList.CR_AddPicReference(DEFAULT_STC, &crReference);
    crList.CR_AddPicReference(DEFAULT_STH, &crReference);
    crList.CR_AddPicReference(DEFAULT_STL, &crReference);
    crList.CR_AddPicReference(DEFAULT_NPC_ICON, &crReference);
    crList.CR_AddPicReference(DEFAULT_NPC_PIC, &crReference);
    crList.CR_AddPicReference(DEFAULT_CURSOR, &crReference);
    crList.CR_AddPicReference(DEFAULT_OVERLAND, &crReference);
    crList.CR_AddPicReference(DEFAULT_ZONEAVART, &crReference);
    crList.CR_AddPicReference(AREAVIEWART, &crReference);
    crList.CR_AddSoundReference(DEF_STEP_SOUND, &crReference);
    crList.CR_AddSoundReference(DEF_BUMP_SOUND, &crReference);
    crList.CR_AddSoundReference(DEF_INTRO_SOUND, &crReference);
    crList.CR_AddSoundReference(DEF_DEATH_SOUND, &crReference);
    crList.CR_AddSoundReference(DEF_HIT_SOUND, &crReference);
    crList.CR_AddSoundReference(DEF_MISS_SOUND, &crReference);
    crList.CR_AddSoundReference(DEF_LAUNCH_SOUND, &crReference);
    crList.CR_AddSoundReference(DEF_CAST_SOUND, &crReference);
    crList.CR_AddSoundReference(DEF_CHAR_DEATH_SOUND, &crReference);
    crList.CR_AddSoundReference(DEF_CHAR_MOVE_SOUND, &crReference);


    CrossReferencePicPool(&crList); //global_SmallPicImport.CrossReference();
    CrossReferenceIconPool(&crList);
    monsterData.CrossReference(&crList);
    spellData.CrossReference(&crList);
    itemData.CrossReference(&crList);
    globalData.CrossReference(&crList);
    CrossReferenceResources(&crList);
    CrossReferenceSavedCharacters(&crList);
    specialAbilitiesData.CrossReference(&crList);
    {
      CrossReference dlg(&crList);
      dlg.DoModal();
    }

  };
}



void CMainFrame::OnGameVersion() 
{
	VersionDlg dlg;
	dlg.m_VersionText.Format("Game Version = %4.7f",globalData.version);
	dlg.DoModal();
  flushOldImages();
}

void CMainFrame::OnImportfrua() 
{
  if (IS_MODIFIED)
  {    
  }

  CImportFRUAData dlg;
  dlg.DoModal();
  flushOldImages();
}

void CMainFrame::OnUpdateBLOCKType(CCmdUI* pCmdUI) 
{
	 pCmdUI->SetCheck(m_BLOCK_Type);		
}

void CMainFrame::OnBLOCKType() 
{
  // toggle current state
  m_BLOCK_Type = !m_BLOCK_Type;
  
  CUAFWinEdView* pView = (CUAFWinEdView*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
  CDataExchange DX(pView, false);
  pView->UpdateBlockageBox(&DX);  // Erase the whole thing.
}

void CMainFrame::OnUpdateKwikKlik(CCmdUI* pCmdUI) 
{
	 pCmdUI->SetCheck(m_KwikKlik);		
}

void CMainFrame::OnKwikKlik() 
{
  // toggle current state
  m_KwikKlik = !m_KwikKlik;
}

void CMainFrame::OnUpdateValidateDesign(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(levelData.eventData.GetCount() > 0);	
}

void CMainFrame::OnTilemapdata() 
{
  m_TileMap = !m_TileMap;	
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
    ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateTilemapdata(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->SetCheck(FALSE);
    pCmdUI->Enable(FALSE);
    return;
  }
  else
    pCmdUI->Enable(TRUE);

  pCmdUI->SetCheck(m_TileMap);
}

//void CMainFrame::OnEditConfig() 
//{  
//  OpenTextFileForEdit(ConfigFile.GetFilename());
//}

//void CMainFrame::OnReloadConfig() 
//{
//  FileParse tempCONFIG;
//  tempCONFIG = ConfigFile;
//
//  if (!LoadConfigFile(tempCONFIG.GetFilename()))
//  {
//    CString msg;
//    msg.Format("Failed to load config file: %s\nRestoring original Config", tempCONFIG.GetFilename());
//    MsgBoxError("Config File Load Error");
//    ConfigFile = tempCONFIG;
//  }	
//
//  if (pWallSlotMgr!=NULL) pWallSlotMgr->Clear(); 
//  if (pBgSlotMgr!=NULL) pBgSlotMgr->Clear();
//
// if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
//   ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
//}

void CMainFrame::OnUpdateGlobalTestsaveddesign(CCmdUI* pCmdUI) 
{
	
}

void CMainFrame::OnUpdateGlobalTestsaveddesignfromstart(CCmdUI* pCmdUI) 
{
	
}

void CopyTestCharArt()
{
  CString src;
  CString dst;
  src = ede.TemplateIconArtDir() + DEFAULT_NPC_ICON;
  dst = rte.IconArtDir() + DEFAULT_NPC_ICON;
  CopyFile(src,dst,FALSE);
  src = ede.TemplateSmallPicDir() + DEFAULT_NPC_PIC;
  dst = rte.SmallPicDir() + DEFAULT_NPC_PIC;
  CopyFile(src,dst,FALSE);
}

void DeleteTestCharArt()
{
  CString dst;
  dst.Format("%s%s",GetDesignPath(), DEFAULT_NPC_ICON);
  DeleteFile(dst);
  
  dst.Format("%s%s",GetDesignPath(), DEFAULT_NPC_PIC);
  DeleteFile(dst);
}

void CMainFrame::OnGlobalTestsaveddesign() 
{
  HANDLE EngineEvent = CreateEvent(NULL, FALSE, FALSE, ENGINE_APP_EVENT);
  if (ERROR_ALREADY_EXISTS == GetLastError())
  {
    MsgBoxError("An instance of the engine is running already");
    CloseHandle(EngineEvent);
    return;
  }
  CloseHandle(EngineEvent);

  CString appName;
  char commandLine[_MAX_PATH+1];
  char workDir[_MAX_PATH+1];


  appName = rte.ExecutableDir() + "DungeonCraft.exe";
  GetDesignPath(workDir);
  if (workDir[strlen(workDir)-1] == '\\')
    workDir[strlen(workDir)-1] = '\0';



  if (PartyList.GetCount() > 0)
  {
    // format: -pc CharacterName1 -pc CharacterName2

    CString partyNames("");
    POSITION pos = PartyList.GetHeadPosition();
    while (pos != NULL)
    {
      CString tmp;
      CString name = PartyList.GetNext(pos);

      if (!name.IsEmpty())
      {
        tmp.Format(" \"-pc %s\"", name);
        partyNames += tmp;
      }
    }

    if (!partyNames.IsEmpty())
    {
      sprintf(commandLine, "\"%s\" \"-warp %i,%i,%i,%i\" %s",
              workDir, currX, currY, globalData.currLevel, currFacing, (LPCSTR)partyNames);
    }
    else
    {
      sprintf(commandLine, "\"%s\" \"-warp %i,%i,%i,%i\"",
              workDir, currX, currY, globalData.currLevel, currFacing);  
    }
  }
  else
  {
    sprintf(commandLine, "\"%s\" \"-warp %i,%i,%i,%i\"",
            workDir, currX, currY, globalData.currLevel, currFacing);
  }



  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  memset(&pi, 0, sizeof(pi));
  memset(&si, 0, sizeof(si));

  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_FORCEONFEEDBACK;

  if (IS_MODIFIED)
  {
    CString temp;
    temp.Format("Do you want to run the game engine using\n\n"
                "  Design: %s\n"
                "  Engine: %s\n\nMake sure your design is saved!",
                workDir, appName);
              
    int result = MsgBoxYesNo(temp, "About to run external program");

    if (result != IDYES)
      return;
  }

  //WriteDebugString("Testing design, destroying SoundMgr(): %s\n", commandLine);
  //if (pSndMgr != NULL)
  //  delete pSndMgr;

  CopyTestCharArt();

  WriteDebugString("Testing Design, command line = \'%s\'\n", commandLine);

  BOOL success = CreateProcess((LPCTSTR)appName,
    commandLine,
                               NULL, NULL,
                               FALSE,
                               CREATE_NEW_PROCESS_GROUP,
                               NULL,
                               workDir,
                               &si,
                               &pi);
  if (!success)
  {
    DWORD error;
    error = GetLastError();
    WriteDebugString("Failed CreateProcess(a:%s, c:%s, w:%s)\n",
                     appName, commandLine, workDir);
    CString msg;
    msg.Format("Unable to start %s\nPlease check paths in config.txt and\nmake sure design is saved",
               appName);
    MsgBoxError(msg, "CreateProcess Failure");
  }
  //else
  //{
  //  WriteDebugString("Waiting for engine to exit from test run...\n");
  //  if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0)
  //    WriteDebugString("Error after WaitForSingleObject()\n");
  //}
  //WriteDebugString("Engine exit signaled after test run, creating SoundMgr()\n");
  //pSndMgr = new SoundMgr(AfxGetMainWnd()->m_hWnd);
}

void CMainFrame::OnGlobalTestsaveddesignfromstart() 
{
  HANDLE EngineEvent = CreateEvent(NULL, FALSE, FALSE, ENGINE_APP_EVENT);
  if (ERROR_ALREADY_EXISTS == GetLastError())
  {
    MsgBoxError("An instance of the engine is running already");
    CloseHandle(EngineEvent);
    return;
  }
  CloseHandle(EngineEvent);

  CString appName;
  char commandLine[_MAX_PATH+1];
  char workDir[_MAX_PATH+1];

  char designDir[_MAX_PATH+1];
  appName = rte.ExecutableDir() + "DungeonCraft.exe";
  strcpy(designDir, rte.DesignDir());
  if (designDir[strlen(designDir)-1] == '\\')
    designDir[strlen(designDir)-1] = '\0';
  sprintf(workDir, "%s", designDir);
  sprintf(commandLine, "\"%s\"", designDir);
  
  

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  memset(&pi, 0, sizeof(pi));
  memset(&si, 0, sizeof(si));

  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_FORCEONFEEDBACK;

  CString temp;
  temp.Format("Do you want to run the game engine using\n\n"
              "  Design:\n   %s\n\n"
              "  Engine:\n   %s\n\nMake sure your design is saved!",
              workDir, appName);
              
  int result = MsgBoxYesNo(temp, "About to run external program");

  if (result != IDYES)
    return;

  //WriteDebugString("Destroying SoundMgr()\n");
  //if (pSndMgr != NULL)
  //  delete pSndMgr;
  CopyTestCharArt();

  BOOL success = CreateProcess(appName,
    commandLine,
                               NULL, NULL,
                               FALSE,
                               CREATE_NEW_PROCESS_GROUP,
                               NULL,
                               workDir,
                               &si,
                               &pi);
  if (!success)
  {
    DWORD error;
    error = GetLastError();
    WriteDebugString("Failed CreateProcess(a:%s, c:%s, w:%s)\n",
                     appName, commandLine, workDir);
    CString msg;
    msg.Format("Unable to start %s\nPlease check paths in config.txt and\nmake sure design is saved",
               appName);
    MsgBoxError(msg, "CreateProcess Failure");
  }
  //else
  //{
  //  WriteDebugString("Waiting for engine to exit...\n");
  //  if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0)
  //    WriteDebugString("Error after WaitForSingleObject()\n");
  //}
  //WriteDebugString("Engine exit signaled, creating SoundMgr()\n");
  //pSndMgr = new SoundMgr(AfxGetMainWnd()->m_hWnd);
}

void CMainFrame::OnUpdateViewWallsets(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);	
}

void CMainFrame::OnUpdateViewBackdrops(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
    pCmdUI->Enable(FALSE);
  else
    pCmdUI->Enable(TRUE);		
}

void CMainFrame::OnHomepage() 
{
  ShellExecute(this->m_hWnd, 
    "open",
    "http://uaf.sourceforge.net", 
    NULL, NULL, SW_SHOW);	
}

void CMainFrame::OnDownloads() 
{
  ShellExecute(this->m_hWnd, 
    "open",
    "http://www.frua.dns2go.com",
    NULL, NULL, SW_SHOW);		
}

void CMainFrame::OnLogEventText() 
{
  levelData.eventData.LogEventText();	
  globalData.eventData.LogEventText();
}

void CMainFrame::OnUpdateLogEventText(CCmdUI* pCmdUI) 
{
  BOOL enable = ((levelData.eventData.GetCount()>0)||(globalData.eventData.GetCount()>0));
  pCmdUI->Enable(enable);		
}

void CMainFrame::OnQuadfillbgslot() 
{
  QuadFillBGSlot = !QuadFillBGSlot;	
}

void CMainFrame::OnUpdateQuadfillbgslot(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck(QuadFillBGSlot);
  pCmdUI->Enable( m_mode == BG_MODE );
}

void CMainFrame::OnFillallmapdistantbgoff() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
  {
    for (j=0;j<levelData.area_width;j++)
    {
      levelData.area[i][j].ShowDistantBG = FALSE;
      levelData.area[i][j].DistantBGInBands = FALSE;
    }
  }

  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
    ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
}

void CMainFrame::OnFillallmapdistantbgon() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
  {
    for (j=0;j<levelData.area_width;j++)
    {
      levelData.area[i][j].ShowDistantBG = TRUE;
      levelData.area[i][j].DistantBGInBands = TRUE;
    }
  }

  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
    ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);	
}

void CMainFrame::OnFillallmapzone() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
    for (j=0;j<levelData.area_width;j++)
      levelData.area[i][j].zone = currZone;

  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
    ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);	
}

void CMainFrame::OnUpdateFillallmapdistantbgon(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }
  pCmdUI->Enable( (m_mode == BG_MODE) || (m_mode == WALL_MODE) );
}

void CMainFrame::OnUpdateFillallmapdistantbgoff(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }
  pCmdUI->Enable( (m_mode == BG_MODE) || (m_mode == WALL_MODE) );
}

void CMainFrame::OnUpdateFillallmapzone(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( (m_mode == ZONE_MODE) );	
}

void CMainFrame::OnToggleTopmost() 
{
  IsTopMost = !IsTopMost;
  const CWnd *pWnd = IsTopMost?&wndTopMost:&wndNoTopMost;
  SetWindowPos(pWnd, 0,0,0,0, (SWP_NOSIZE|SWP_NOMOVE));	
}

void CMainFrame::OnUpdateToggleTopmost(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck(IsTopMost);
}

void CMainFrame::OnLaunchdcconfig() 
{
  CGameResChange dlg;
  dlg.DoModal();

  /*
  CString appName;
  char cmdLine[_MAX_PATH+1];
  appName = m_installDir;
  appName += "\\UAFConfig\\DCConfig.exe";
  sprintf(cmdLine, "%s", (LPCSTR)appName);

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  memset(&pi, 0, sizeof(pi));
  memset(&si, 0, sizeof(si));

  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_FORCEONFEEDBACK;

  BOOL success = CreateProcess(appName,
                               cmdLine,
                               NULL, NULL,
                               FALSE,
                               CREATE_NEW_PROCESS_GROUP,
                               NULL,
                               NULL,
                               &si,
                               &pi);
  if (!success)
  {
    WriteDebugString("Failed CreateProcess(a:%s, c:%s)\n",
                     appName, cmdLine);
    CString msg;
    msg.Format("Unable to start %s\n", appName);
    MsgBoxError(msg, "CreateProcess Failure");
  }
  */
}

void CMainFrame::OnUpdateLaunchdcconfig(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateLaunchuaconfig(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(IsUAFConfigPresent);
}

void CMainFrame::OnLaunchuaconfig() 
{
  CString appName;
  char commandLine[_MAX_PATH+1];
  appName = ede.EditorExecutableDir();
  appName += "UAFConfig\\UAFConfig.exe";
  sprintf(commandLine, "%s", (LPCSTR)appName);

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  memset(&pi, 0, sizeof(pi));
  memset(&si, 0, sizeof(si));

  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_FORCEONFEEDBACK;

  BOOL success = CreateProcess(appName,
    commandLine,
                               NULL, NULL,
                               FALSE,
                               CREATE_NEW_PROCESS_GROUP,
                               NULL,
                               NULL,
                               &si,
                               &pi);
  if (!success)
  {
    WriteDebugString("Failed CreateProcess(a:%s, c:%s)\n",
                     appName, commandLine);
    CString msg;
    msg.Format("Unable to start %s\n", appName);
    MsgBoxError(msg, "CreateProcess Failure");
  }
}

void CMainFrame::OnFillallbackdropAlldir() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
   for (j=0;j<levelData.area_width;j++)
      levelData.area[i][j].SetAllBG(currBkGnd);

  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
     ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);	
}

void CMainFrame::OnFillallbackdropEastdir() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
   for (j=0;j<levelData.area_width;j++)
      levelData.area[i][j].eastBG = currBkGnd;

  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
     ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);		
}

void CMainFrame::OnFillallbackdropNorthdir() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
   for (j=0;j<levelData.area_width;j++)
      levelData.area[i][j].northBG = currBkGnd;

  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
     ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);		
}

void CMainFrame::OnFillallbackdropSouthdir() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
   for (j=0;j<levelData.area_width;j++)
      levelData.area[i][j].southBG = currBkGnd;

  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
     ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);			
}

void CMainFrame::OnFillallbackdropWestdir() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
   for (j=0;j<levelData.area_width;j++)
      levelData.area[i][j].westBG = currBkGnd;

  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
     ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);		
}

void CMainFrame::OnUpdateFillallbackdropAlldir(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

 	pCmdUI->Enable( (m_mode == BG_MODE) );	
}

void CMainFrame::OnUpdateFillallbackdropWestdir(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

 	pCmdUI->Enable( (m_mode == BG_MODE) );	
}

void CMainFrame::OnUpdateFillallbackdropSouthdir(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

 	pCmdUI->Enable( (m_mode == BG_MODE) );	
}

void CMainFrame::OnUpdateFillallbackdropNorthdir(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

 	pCmdUI->Enable( (m_mode == BG_MODE) );	
}

void CMainFrame::OnUpdateFillallbackdropEastdir(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

 	pCmdUI->Enable( (m_mode == BG_MODE) );	
}

void CMainFrame::OnFillunusedbackdropAlldir() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
  {
   for (j=0;j<levelData.area_width;j++)
   {
      if (levelData.area[i][j].northBG == 0)
        levelData.area[i][j].northBG = (BYTE)currBkGnd;
      if (levelData.area[i][j].eastBG == 0)
        levelData.area[i][j].eastBG = (BYTE)currBkGnd;
      if (levelData.area[i][j].southBG == 0)
        levelData.area[i][j].southBG = (BYTE)currBkGnd;
      if (levelData.area[i][j].westBG == 0)
        levelData.area[i][j].westBG = (BYTE)currBkGnd;
   }
  }
  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
     ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateFillunusedbackdropAlldir(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

 	pCmdUI->Enable( (m_mode == BG_MODE) );		
}

void CMainFrame::OnFillunusedbackdropEastdir() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
  {
    for (j=0;j<levelData.area_width;j++)
    {
      if (levelData.area[i][j].eastBG == 0)
        levelData.area[i][j].eastBG = (BYTE)currBkGnd;
    }
  }
  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
     ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);	
}

void CMainFrame::OnUpdateFillunusedbackdropEastdir(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

 	pCmdUI->Enable( (m_mode == BG_MODE) );		
}

void CMainFrame::OnFillunusedbackdropNorthdir() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
  {
    for (j=0;j<levelData.area_width;j++)
    {
      if (levelData.area[i][j].northBG == 0)
        levelData.area[i][j].northBG = (BYTE)currBkGnd;
    }
  }
  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
     ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);	
}

void CMainFrame::OnUpdateFillunusedbackdropNorthdir(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

 	pCmdUI->Enable( (m_mode == BG_MODE) );		
}

void CMainFrame::OnFillunusedbackdropSouthdir() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
  {
    for (j=0;j<levelData.area_width;j++)
    {
      if (levelData.area[i][j].southBG == 0)
        levelData.area[i][j].southBG = (BYTE)currBkGnd;
    }
  }
  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
     ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);		
}

void CMainFrame::OnUpdateFillunusedbackdropSouthdir(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

 	pCmdUI->Enable( (m_mode == BG_MODE) );		
}

void CMainFrame::OnFillunusedbackdropWestdir() 
{
  int i,j;
  for (i=0;i<levelData.area_height;i++)
  {
    for (j=0;j<levelData.area_width;j++)
    {
      if (levelData.area[i][j].westBG == 0)
        levelData.area[i][j].westBG = (BYTE)currBkGnd;
    }
  }
  SET_MODIFIED;
  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
     ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);	
}

void CMainFrame::OnUpdateFillunusedbackdropWestdir(CCmdUI* pCmdUI) 
{
  if (m_mapType == OVERLAND_TYPE)
  {
    pCmdUI->Enable(FALSE);
    return;
  }

 	pCmdUI->Enable( (m_mode == BG_MODE) );		
}

void CMainFrame::OnAppHelpcontents() 
{
  CString path;
#ifdef SIMPLE_STRUCTURE
  path = ede.EditorHelpDir() + "uaforever.hlp";
#else
  path.Format("%s\\help\\uaforever.hlp", m_installDir);
#endif

  ShellExecute(this->m_hWnd, 
    "open",
    path, 
    NULL, NULL, SW_SHOW);		
}

void CMainFrame::OnGlobalViewartastransparent() 
{
  CScrollPicDlg dlg("", "", "", TransBufferDib);
  dlg.DoModal();
}

void CMainFrame::OnEditoroptions() 
{
  EditorOptionsData.m_LastPassWalls=PassThruWalls;
  EditorOptionsData.m_LastDoubleFillWall=DoubleFillWallSlot;
  EditorOptionsData.m_LastQuadFileBg=QuadFillBGSlot;
  EditorOptionsData.m_LastKwikKlik=m_KwikKlik;
  EditorOptionsData.m_LastBlockType=m_BLOCK_Type;
  EditorOptionsData.m_LastTileMap=m_TileMap;

  CEditorOptions dlg(EditorOptionsData);
  if (dlg.DoModal()==IDOK)
  {
    dlg.GetData(EditorOptionsData);

    PassThruWalls=EditorOptionsData.m_LastPassWalls;
    DoubleFillWallSlot=EditorOptionsData.m_LastDoubleFillWall;
    QuadFillBGSlot=EditorOptionsData.m_LastQuadFileBg;
    m_KwikKlik=EditorOptionsData.m_LastKwikKlik;
    m_BLOCK_Type=EditorOptionsData.m_LastBlockType;
    m_TileMap=EditorOptionsData.m_LastTileMap;

    if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
       ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);	
  }
}


void CMainFrame::OnGameStats() 
{
  CDesignStatsDlg dlg;
  dlg.DoModal();	
}

void CMainFrame::OnViewjournal() 
{
  CJournalDataDlg dlg;
  dlg.DoModal();	
}

LRESULT CMainFrame::OnMsgCloseSearchDlg(WPARAM wp, LPARAM lp) 
{
  DestroySearchDlg();
  return 1;
}

LRESULT CMainFrame::OnDoScreenCap(WPARAM wp, LPARAM lp) 
{
  // user requested a screen capture
#ifdef SIMPLE_STRUCTURE
  CString path = rte.ScreenCapDir();
#else
  CString path=m_installDir;
#endif
  path += "DCScreenCapture.bmp";
  DeleteFile(path);
  GraphicsMgr.DoScreenCapture(path);
  CString msg;
  msg.Format("Image saved as %s", path);
  MsgBoxInfo(msg, "Screen Capture"); 
  return 1;
}

void CMainFrame::OnAppScreencapture() 
{
  timerId = SetTimer(SCREEN_CAP_TIMER, 2000, NULL);  
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent==SCREEN_CAP_TIMER)
  {
    KillTimer(timerId);
    PostMessage((UINT)ID_DOSCREENCAP, (WPARAM)0, (WPARAM)0);    
  }
  else
	  CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnGlobalSettestparty() 
{
	CTestPartySelectDlg dlg;
  
  POSITION pos = PartyList.GetHeadPosition();
  while (pos != NULL)
    dlg.PartyList.AddTail(PartyList.GetNext(pos));

  if (dlg.DoModal() == IDOK)
  {
    PartyList.RemoveAll();
    pos = dlg.PartyList.GetHeadPosition();
    while (pos != NULL)
      PartyList.AddTail( dlg.PartyList.GetNext(pos) );
  }
}

void CMainFrame::OnGlobalTestsaveddesignViewerrorlog() 
{
  CString path;
#ifdef SIMPLE_STRUCTURE
  path = rte.LogDir() + "UafErr_Engine.txt";
#else
  path.Format("%s\\UAFWin\\UafErr_Eng.txt", m_installDir);
#endif
  
  ShellExecute(this->m_hWnd, 
              "open",
              path, 
              NULL, NULL, SW_SHOW);			
}

void CMainFrame::OnGameErrorlog() 
{
  CString path;
#ifdef SIMPLE_STRUCTURE
  path = rte.LogDir() + "UafErr_Edit.txt";
#else
  path.Format("%s\\UAFWinEd\\UafErr_Edit.txt", m_installDir);
#endif
  
  ShellExecute(this->m_hWnd, 
    "open",
    path, 
    NULL, NULL, SW_SHOW);	
}
