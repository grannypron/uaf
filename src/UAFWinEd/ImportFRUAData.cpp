/******************************************************************************
* Filename: ImportFRUAData.cpp
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
// ImportFRUAData.cpp : implementation file
//

#include "..\Shared\stdafx.h"
//#include "..\Shared\Version.h"
#include "resource.h"

#include "uafwined.h"
#include "uafwinedview.h"
#include "Mainfrm.h"
#include "SoundMgr.h"
#include  "Party.h"
#include "ImportFRUAData.h"
#include "SelectFolderDialog.h"

#include "UAImport.h"

extern CString UAInstallPath;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static void PumpWinMsgs()
{
  const int MAX_WIN_MSGS = 30;
  int count=0;
  MSG msg;
  while ((::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)) && (count<MAX_WIN_MSGS))
  {
    if (GetMessage(&msg, NULL, 0, 0) > 0)
    {
       ::TranslateMessage(&msg);
       ::DispatchMessage(&msg);
    }
    count++;
  }
}

/////////////////////////////////////////////////////////////////////////////
// CImportFRUAData dialog


CImportFRUAData::CImportFRUAData(CWnd* pParent /*=NULL*/)
	: CDialog(CImportFRUAData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportFRUAData)
	m_AllInFolder = TRUE;
	m_UAPath = _T("");
	m_IncItems = FALSE;
	m_IncMonsters = FALSE;
	m_ClearMonsters = TRUE;
	m_ClearItems = TRUE;
	m_IncItemsWithMonsters = TRUE;
	m_UseLoadedArt = FALSE;
	//}}AFX_DATA_INIT
  //ClearImportBuffers();
  UseDefaultArtForImport=true;
  m_UseLoadedArt = !UseDefaultArtForImport;
}


void CImportFRUAData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportFRUAData)
	DDX_Check(pDX, IDC_ALLINFOLDER, m_AllInFolder);
	DDX_Text(pDX, IDC_UAPATH, m_UAPath);
	DDX_Check(pDX, IDC_INCITEMS, m_IncItems);
	DDX_Check(pDX, IDC_INCMONSTERS, m_IncMonsters);
	DDX_Check(pDX, IDC_CLEARMONSTERS, m_ClearMonsters);
	DDX_Check(pDX, IDC_CLEARITEMS, m_ClearItems);
	DDX_Check(pDX, IDC_INCITEMS2, m_IncItemsWithMonsters);
	DDX_Check(pDX, IDC_USECURRENTART, m_UseLoadedArt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportFRUAData, CDialog)
	//{{AFX_MSG_MAP(CImportFRUAData)
	ON_BN_CLICKED(IDC_IMPORTFRUADESIGN, OnImportfruadesign)
	ON_BN_CLICKED(IDC_IMPORTFRUAITEMS, OnImportfruaitems)
	ON_BN_CLICKED(IDC_IMPORTFRUAMONSTERS, OnImportfruamonsters)
	ON_BN_CLICKED(IDC_IMPORTFRUASPELLS, OnImportfruaspells)
	ON_BN_CLICKED(IDC_ALLINFOLDER, OnAllinfolder)
	ON_BN_CLICKED(IDC_UASELECT, OnUaselect)
	ON_BN_CLICKED(IDC_IMPORTFRUAART, OnImportfruaart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CImportFRUAData::ImportItems(const char *path)
{
  AfxGetApp()->DoWaitCursor(1);
  //CWaitCursor wc;

  if (m_ClearItems) itemData.Clear();
  ClearItemBuffers();
  EditorStatusMsg("Importing FRUA item data...");
  BOOL success = ImportUAItemsToUAF(path);

  AfxGetApp()->DoWaitCursor(-1);

  return success;
}

int s_count = 0;
int t_count = 0;

void CImportFRUAData::ImportAllMonsters(const char *path)
{
  //CWaitCursor wc;
  AfxGetApp()->DoWaitCursor(1);

  if (m_ClearMonsters) 
  {
    monsterData.Clear();
    globalData.charData.Clear();
  }

  ClearMonsterBuffers();
  CFileFind finder;
  CString searchPath;
  searchPath.Format("%sMonst*.dat", path);

  s_count = 0;
  t_count = 0;

  BOOL found = finder.FindFile(searchPath);

  while (found)
  {
    found = finder.FindNextFile();

    if (finder.IsDots())
       continue;
    if (finder.IsDirectory())
      continue;

    if (ImportMonsterToUAF(finder.GetFilePath()))
      s_count++;
    t_count++;
  }
  AfxGetApp()->DoWaitCursor(-1);
}

void CImportFRUAData::ClearDesign()
{
   AfxGetApp()->DoWaitCursor(1);

   int i; 
   BOOL clearNPC = FALSE;
   if ((m_IncMonsters) && (m_ClearMonsters)) clearNPC=TRUE;
   globalData.Clear(FALSE, clearNPC);
   levelData.Clear();

   for (i=0;i<MAX_LEVELS;i++)
   {
      deleteAllLevelArt(i);
      deleteAllLevelSound(i);
   }

   deleteAllLevelArt(GLOBAL_ART);
   deleteAllLevelSound(GLOBAL_SOUND);


   HBarVPArt.Clear();
   VBarVPArt.Clear();
   FrameVPArt.Clear();
   VBarCombArt.Clear();
   HBarCombArt.Clear();
   CombatCursorArt.Clear();
   CombatDeathIconArt.Clear();
   CombatDeathArt.Clear();
   CursorArt.Clear();

   global_SmallPicImport.RemoveAll();
   global_IconPicImport.RemoveAll();

   HBarVPArt.name         = /*ede.EditorWindowArtDir() + */DEFAULT_HB;
   VBarVPArt.name         = /*ede.EditorWindowArtDir() + */DEFAULT_VB;
   FrameVPArt.name        = /*ede.EditorWindowArtDir() + */DEFAULT_FR;
   VBarCombArt.name       = /*ede.EditorWindowArtDir() + */DEFAULT_VB;
   HBarCombArt.name       = /*ede.EditorWindowArtDir() + */DEFAULT_HB;
   CombatWallArt.name     = /*ede.DesignCombatArtDir() + */DEFAULT_CWD;
   CombatCursorArt.name   = /*ede.DesignCursorArtDir() + */DEFAULT_CC;
   CombatDeathIconArt.name= /*ede.DesignCombatArtDir() + */DEFAULT_CDI;
   CombatDeathArt.name    = /*ede.DesignCombatArtDir() + */DEFAULT_CD;
   CursorArt.filename     = /*ede.DesignCursorArtDir() + */DEFAULT_CURSOR;
   CursorArt.FrameHeight = 32;
   CursorArt.FrameWidth = 32;
   CursorArt.NumFrames = 15;
   CursorArt.picType = TransBufferDib;
   CursorArt.style = PIC_DATA::AS_None;
   CursorArt.timeDelay = 0;
   
   // load up the default icons/sprites that
   // are in the editor/DefaultArt folder.
   //
   // Loads all icons named Icon1.pcx..Icon50.pcx
   // Loads all icons named SPic1.pcx..SPic50.pcx
   i=1;
   CString temp;
   CString path;
   PIC_DATA pic;
   pic.picType = SmallPicDib;
   pic.SetDefaults();

   PumpWinMsgs();
   AfxGetApp()->DoWaitCursor(1);

   while (i <= 50)
   {
     temp.Format("prt_SPic%u.png", i);
     // uses root name to search for any matches
     // using valid image extensions (bmp, pcx, etc)
     if (FindImageWithValidExt(temp, ede.TemplateSmallPicDir()))
     {
       pic.filename = temp;
       global_SmallPicImport.AddTail(pic);
     }
     i++;
   }

   PumpWinMsgs();
   AfxGetApp()->DoWaitCursor(1);

   i=1;
   pic.Clear();
   pic.picType = IconDib;
   pic.SetDefaults();
   while (i <= 50)
   {
     temp.Format("cn_Icon%u.png", i);
     if (FindImageWithValidExt(temp, ede.TemplateIconArtDir()))
     {
       pic.filename = temp;
       global_IconPicImport.AddTail(pic);
     }
     i++;
   }

   PumpWinMsgs();
   AfxGetApp()->DoWaitCursor(1);

   currX = globalData.startX;
   currY = globalData.startY;
   currFacing = globalData.startFacing;
   party.facing = currFacing;

   /*
   fillDefaultArtSlots(levelData,globalData.levelInfo.stats[globalData.startLevel].overland);
   levelData.SetSlots();
   if (loadWallSetSurfaces())
   {
     currBlockage = BlockedBlk;
     currWallSlot = FindFirstLoadedWall();
     currBkGnd = FindFirstLoadedBackground();
   }

   fillDefaultLevelData(levelData, globalData.startLevel, 
                        globalData.levelInfo.stats[globalData.startLevel].area_width, 
                        globalData.levelInfo.stats[globalData.startLevel].area_height, 
                        globalData.levelInfo.stats[globalData.startLevel].overland, 
                        TRUE); 
   levelData.GetSlots();
   */

   AfxGetApp()->DoWaitCursor(-1);
}

/////////////////////////////////////////////////////////////////////////////
// CImportFRUAData message handlers

void CImportFRUAData::OnImportfruadesign() 
{
  UpdateData(TRUE);

  if ((m_IncItems) && (m_UAPath==""))
  {
    MsgBoxError("You must specifiy a path to your UA installation folder\n");
    return;
  }
  
  bool finished = false;
  CString dpath;
  while (!finished)
  {
    CSelectFolderDialog dlg(FALSE, m_UAPath,
						                OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						                NULL, NULL);

    dlg.SetTitle("Select folder of FRUA design to import");
    if (dlg.DoModal() != IDOK) return;

    //CString path = BrowseForFolder(AfxGetMainWnd()->m_hWnd, "Select folder of FRUA design to import");
    dpath = dlg.GetSelectedPath();

    if (dpath == "") // user hit cancel button
      return;
    dpath.MakeUpper();
    if (strstr(dpath, ".DSN") == NULL)
    {
      CString msg;
      msg.Format("\'%s\' is not a valid design folder.\nPlease select a folder that ends with \'.dsn\'", dpath);
      MsgBoxError(msg, "Design Folder Not Selected");
    }
    else
      finished = true;
  }

  if (dpath.GetAt(strlen(dpath)-1) != '\\')
    dpath += "\\";

  BOOL success = TRUE;

  ClearDesign();
  ClearImportBuffers();
  if (!UseDefaultArtForImport) 
    levelData.GetSlots();

  // items too?
  if (m_IncItems)
  {
    CString path = m_UAPath;
    if (!path.IsEmpty()) 
    {
      if (path.GetAt(strlen(path)-1) != '\\')
        path += "\\";   
      path += "DISK1";
    }
    if (path=="") return;
    if (path.GetAt(strlen(path)-1) != '\\')
      path += "\\";
    success = ImportItems(path);
  }

  if ((success) && (m_IncMonsters))
  {
    CString path = dpath;
    if (path=="") return;
    if (path.GetAt(strlen(path)-1) != '\\')
      path += "\\";
    ImportAllMonsters(path);
  }

  if (success)
  {  
    //CWaitCursor wc;
    AfxGetApp()->DoWaitCursor(1);

    EditorStatusMsg("Importing FRUA design data...");
    success = ImportGameDat(dpath);

    AfxGetApp()->DoWaitCursor(-1);	
  }

  if (success)
  {
    EditorStatusMsg("Import of FRUA data complete");

    CString name = globalData.designName;
    int level = globalData.startLevel;

    if (loadDesign(name))
      LoadLevel(level);

   currX = globalData.startX;
   currY = globalData.startY;
   currFacing = globalData.startFacing;   
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
   party.Posx = currX;
   party.Posy = currY;
   party.facing = currFacing;

    CUAFWinEdView* pView = (CUAFWinEdView*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
    if (pView != NULL)
      pView->ResetView();

    if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
      ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);

    EditorStatusMsg("Import of FRUA design successful");
    CString msg;
    msg.Format("FRUA design imported successfully from %s", dpath);
    MsgBoxInfo(msg,"Import Status");

  }
  else
  {
    EditorStatusMsg("Import of FRUA design failed");
    CString msg;
    msg.Format("FRUA design FAILED to import from %s", dpath);
    MsgBoxError(msg,"Import Status");
  }
}

void CImportFRUAData::OnImportfruaitems() 
{
  UpdateData(TRUE);

  if (m_UAPath=="")
  {
    MsgBoxError("You must specifiy a path to your UA installation folder\n");
    return;
  }  

  CString path = m_UAPath;
  if (!path.IsEmpty()) 
  {
    if (path.GetAt(strlen(path)-1) != '\\')
      path += "\\";   
    path += "DISK1";
  }

  CSelectFolderDialog dlg(FALSE, path,
						              OFN_OVERWRITEPROMPT,
						              NULL, NULL);

  dlg.SetTitle("Select folder with FRUA items to import, usually DISK1 folder");
  if (dlg.DoModal() != IDOK) return;

  /*
  CString path = m_UAPath; //BrowseForFolder(AfxGetMainWnd()->m_hWnd, "Select folder with FRUA items to import\n(Item.dat and Items.dat, usually in DISK1 folder)");
  
  if (path == "")
  {
    path = BrowseForFolder(AfxGetMainWnd()->m_hWnd, "Select folder with FRUA items to import\n(Item.dat and Items.dat, usually in DISK1 folder)");
    if (path == "") // user hit cancel button
      return;
  }
  else
    path += "\\DISK1";
  */

  path = dlg.GetSelectedPath();
  if (path=="") return;
  if (path.GetAt(strlen(path)-1) != '\\')
    path += "\\";

  BOOL success = ImportItems(path);
  CString msg;
  if (!success)
  {
    msg.Format("FRUA items FAILED to import from %s", path);
    EditorStatusMsg(msg);
    MsgBoxError(msg,"Import Status");
  }
  else
  {
    msg.Format("FRUA items imported successfully from %s", path);
    EditorStatusMsg(msg);
    MsgBoxInfo(msg,"Import Status");
  }
}

void CImportFRUAData::OnImportfruamonsters() 
{
  UpdateData(TRUE);

  if ((m_IncItemsWithMonsters) && (m_UAPath==""))
  {
    MsgBoxError("You must specifiy a path to your UA installation folder\n");
    return;
  }

  if (m_IncItemsWithMonsters)
  {
    CString path = m_UAPath;
    if (!path.IsEmpty()) 
    {
      if (path.GetAt(strlen(path)-1) != '\\')
        path += "\\";   
      path += "DISK1";
    }
    if (path=="") return;
    if (path.GetAt(strlen(path)-1) != '\\')
      path += "\\";
    ImportItems(path);
  }

  if (!m_AllInFolder)
  {
    CString filename;
    filename = m_UAPath;
    if (!filename.IsEmpty()) 
    {
      if (filename.GetAt(strlen(filename)-1) != '\\')
        filename += "\\";   
      filename += "DISK2\\MONST001.DAT";
    }

    if (GetFilename(filename, "DAT", TRUE))
    {
      if (ImportMonsterToUAF(filename))
      {      
        MsgBoxInfo("FRUA Monster/NPC imported successfully","Import Status");
      }
      else
        MsgBoxError("FRUA Monster/NPC import failed","Import Status");
    }
  }
  else
  {   
    CString path = m_UAPath;
    if (!path.IsEmpty()) 
    {
      if (path.GetAt(strlen(path)-1) != '\\')
        path += "\\";   
      path += "DISK2";
    }

    CSelectFolderDialog dlg(FALSE, path,
						                OFN_OVERWRITEPROMPT,
						                NULL, NULL);

    dlg.SetTitle("Select folder with FRUA MONST*.DAT files to import, usually DISK2 folder");
    if (dlg.DoModal() != IDOK) return;
    
    /*
    CString path = m_UAPath; //BrowseForFolder(AfxGetMainWnd()->m_hWnd, "Select folder with FRUA MONST*.DAT files to import");

    if (path == "")
    {
      path = BrowseForFolder(AfxGetMainWnd()->m_hWnd, "Select folder with FRUA MONST*.DAT files to import");
      if (path == "") // user hit cancel button
        return;
    }
    else
      path += "\\DISK2";      
    */

    path = dlg.GetSelectedPath();

    if (path=="") return;
    if (path.GetAt(strlen(path)-1) != '\\')
      path += "\\";

    ImportAllMonsters(path);
    CString msg;
    msg.Format("%u of %u DAT files imported from %s\n%i Monsters and %i NPCs", 
               s_count, t_count, path, ImportMonsterCount, ImportNPCCount);
    MsgBoxInfo( msg,"Import Status");
  }
}

void CImportFRUAData::OnImportfruaspells() 
{
  /*
  CString path = BrowseForFolder(AfxGetMainWnd()->m_hWnd, "Select folder with FRUA spells to import\n(Ckit.exe, usually in main FRUA folder)");
  
  if (path == "") // user hit cancel button
    return;

  path += "\\";

  AfxGetApp()->DoWaitCursor(1);

  EditorStatusMsg("Importing FRUA spell data...");
  BOOL success = FALSE; //ImportUASpellsToUAF(path);

  AfxGetApp()->DoWaitCursor(-1);	

  if (!success)
  {
    EditorStatusMsg("Import of FRUA spells failed");
    MsgBoxError("FRUA spells FAILED to import","Import Status");
  }
  else
  {
    EditorStatusMsg("Import of FRUA spells successful");
    MsgBoxInfo("FRUA spells imported successfully","Import Status");
  }
  */
}

void CImportFRUAData::OnAllinfolder() 
{
	// TODO: Add your control notification handler code here
	
}

void CImportFRUAData::OnUaselect() 
{
  UpdateData(TRUE);

  CSelectFolderDialog dlg(FALSE, m_UAPath,
						              OFN_OVERWRITEPROMPT,
						              NULL, NULL);

  dlg.SetTitle("Select FRUA installation folder");
  if (dlg.DoModal() != IDOK) return;

	CString path = dlg.GetSelectedPath(); //BrowseForFolder(AfxGetMainWnd()->m_hWnd, "Select FRUA installation folder");
  if (path.IsEmpty()) return;

  CString temp(path);
  temp.MakeUpper();
  if (strstr(temp, ".DSN") != NULL)
    MsgBoxError("This appears to be a design folder. Select your root UA install folder.\n\n(example: \'C:\\UA\')");

  UAInstallPath = path;
  ((CMainFrame*)AfxGetMainWnd())->SaveUAFolder();
  m_UAPath=path;
  UpdateData(FALSE);
}

BOOL CImportFRUAData::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	((CMainFrame*)AfxGetMainWnd())->RestoreUAFolder();
  m_UAPath = UAInstallPath;
  UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImportFRUAData::OnImportfruaart() 
{ 

  UpdateData(TRUE);

  CSelectFolderDialog dlg(FALSE, rte.DesignDir(),
						              OFN_OVERWRITEPROMPT,
						              NULL, NULL);

  dlg.SetTitle("Select FRUA Wall and Backdrop Art Folder");
  if (dlg.DoModal() != IDOK) return;

	CString folder = dlg.GetSelectedPath();
  if (folder.IsEmpty()) return;

  // initialize all default art slots
  // Leave slot 0 blank!
  // 

  MaxDefaultArtSlots = 80;
  CString temp;

  int wallcount=0;
  int bgcount=0;

  PumpWinMsgs();

  AfxGetApp()->DoWaitCursor(1);

  int i;
  for (i=1;i<=MaxDefaultArtSlots; i++)
  {
    WallSets[i].Clear();
    bool found=false;

    temp.Format("wa_Wall%u.png", i);
    if (FindImageWithValidExtInFolder(temp, folder, TRUE))
    {
      WallSets[i].wallFile = temp;
      found=true;
    }

    temp.Format("dr_Door%u.png", i);
    if (FindImageWithValidExtInFolder(temp, folder, TRUE))
    {
      WallSets[i].doorFile = temp;
      found=true;
    }

    temp.Format("ov_Overlay%u.png", i);
    if (FindImageWithValidExtInFolder(temp, folder, TRUE))
    {
      WallSets[i].overlayFile = temp;
      found=true;
    }

    if (found)
      wallcount++;
  }

  for (i=1;i<MaxDefaultArtSlots; i++)
  {
    BackgroundSets[i].Clear();

    temp.Format("bd_Background%u.png", i);
    if (FindImageWithValidExtInFolder(temp, folder, TRUE))
    {
      BackgroundSets[i].backgroundFile = temp;
      bgcount++;
    }
  }  

  AfxGetApp()->DoWaitCursor(-1);

  CString msg;
  msg.Format("Imported %i wall slots and %i backdrop slots\nUse this art for import?", wallcount, bgcount);
  int result = MsgBoxYesNo(msg, "Confirm Usaqe of Imported UA Art");
  if (result == IDYES)
    UseDefaultArtForImport = false;
  else
    UseDefaultArtForImport = true;

  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
    ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);

  m_UseLoadedArt = !UseDefaultArtForImport;
  UpdateData(FALSE);
}
