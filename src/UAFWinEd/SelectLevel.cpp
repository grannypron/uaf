/******************************************************************************
* Filename: SelectLevel.cpp
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
#include "Level.h"
#include "PicSlot.h"
#include "MapStyle.h"
#include "RequestFilename.h"
#include "DesignList.h"
#include "SelectLevel.h"
#include "ConfigFile.h"
#include "class.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int ExportLevelDB(LPCSTR file,
                  int level,
                  LEVEL *levelPointer,
                  CONFIG_FILE *pConfigFile);



static LEVEL *tempLEVEL=NULL;

static LEVEL *GetTempLevel()
{
  if (tempLEVEL==NULL)
    tempLEVEL = new LEVEL;

  return tempLEVEL;
}

static void ReleaseTempLevel()
{
  if (tempLEVEL!=NULL)
    delete tempLEVEL;
  tempLEVEL = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CSelectLevel dialog

CSelectLevel::CSelectLevel(int level, CWnd* pParent /*=NULL*/)
	: CDialog(CSelectLevel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectLevel)
	m_SaveWarn = FALSE;
	//}}AFX_DATA_INIT

  m_level = max(level, 0);
  m_level = min(m_level, MAX_LEVELS-1);
  m_CopyLevel=-1;
  m_ovName="";
  m_SaveWarn = EditorOptionsData.m_LastAutoLevelSaveWarn;
  GetTempLevel();
}

CSelectLevel::~CSelectLevel()
{
  ReleaseTempLevel();
}

void CSelectLevel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectLevel)
	DDX_Control(pDX, IDC_LEVELLIST, m_LevelList);
	DDX_Check(pDX, IDC_SAVEWARN, m_SaveWarn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectLevel, CDialog)
	//{{AFX_MSG_MAP(CSelectLevel)
	ON_BN_CLICKED(IDC_COPYLEVEL, OnCopylevel)
	ON_BN_CLICKED(IDC_DELETELEVEL, OnDeletelevel)
	ON_BN_CLICKED(IDC_CLEARLEVEL, OnClearlevel)
	ON_NOTIFY(NM_DBLCLK, IDC_LEVELLIST, OnDblclkLevellist)
	ON_BN_CLICKED(IDC_EDITPROPS, OnEditprops)
	ON_BN_CLICKED(IDC_PASTELEVEL, OnPastelevel)
	ON_BN_CLICKED(IDC_SAVEWARN, OnSavewarn)
	ON_BN_CLICKED(IDC_Export, OnExportLevel)
	ON_BN_CLICKED(IDC_Import, OnImportLevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectLevel message handlers

// when exiting this dialog, check the level that
// was passed in to see if it is still good before
// returning to main program. If not, scan for first
// good level.

int CSelectLevel::CheckCurrentLevel()
{
  if (globalData.levelInfo.stats[m_level].used)
    return m_level;

  m_level = 0;
  for (int i=0;i<MAX_LEVELS;i++)
  {
    if (globalData.levelInfo.stats[i].used)
    {
      m_level = i;
      return i;
    }
  }  

  return 0;
}
/*
void CSelectLevel::CheckForUnsavedLevel()
{
  if (globalData.levelInfo.stats[globalData.currLevel].used && IS_MODIFIED)
  {
    int response = IDYES;
    if (m_SaveWarn)
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
      //BOOL success = saveDesign();
      BOOL success = saveLevel(levelData, globalData.currLevel);
      AfxGetApp()->DoWaitCursor(-1);

      if (success)
        CLEAR_MODIFIED;
      else
        MsgBoxError("Failed to save design", "Save Error");
    }
  }
}
*/

BOOL CSelectLevel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Level";
	lvc.cx = 60;
	lvc.fmt = LVCFMT_LEFT;
  m_LevelList.InsertColumn(0,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Name";
	lvc.cx = 125;
	lvc.fmt = LVCFMT_LEFT;
	m_LevelList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 2;
	lvc.pszText = "Used";
	lvc.cx = 45;
	lvc.fmt = LVCFMT_LEFT;
  m_LevelList.InsertColumn(2,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 3;
	lvc.pszText = "Overland";
	lvc.cx = 60;
	lvc.fmt = LVCFMT_LEFT;
  m_LevelList.InsertColumn(3,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 4;
	lvc.pszText = "Width";
	lvc.cx = 45;
	lvc.fmt = LVCFMT_LEFT;
  m_LevelList.InsertColumn(4,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 5;
	lvc.pszText = "Height";
	lvc.cx = 45;
	lvc.fmt = LVCFMT_LEFT;
  m_LevelList.InsertColumn(5,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 6;
	lvc.pszText = "Area View";
	lvc.cx = 60;
	lvc.fmt = LVCFMT_LEFT;
  m_LevelList.InsertColumn(6,&lvc, CListSort::dtINT, true);

  m_LevelList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  CString levellist("");

  for (int i=0;i<MAX_LEVELS;i++)
  {
    // make sure the 'used' flag isn't TRUE
    // if the level file doesn't exist
    if (globalData.levelInfo.stats[i].used)
    {
      if (!levelExists(i))
      {
        globalData.levelInfo.stats[i].used = FALSE;
        CString tmp;
        tmp.Format("%i", i+1);
        if (levellist.IsEmpty())
        {
          levellist += tmp;
        }
        else
        {
          levellist += ",";
          levellist += tmp;
        }
      }
    }
    AddToAvailList(i);
  }

  m_LevelList.Sort();
  //m_LevelList.SetItemState(m_level, LVIS_SELECTED, LVIS_SELECTED);
  //m_LevelList.EnsureVisible(m_level, FALSE);
  m_LevelList.SelectItem(m_level, TRUE);

  m_CopyLevel=-1;
  CWnd *pWnd = GetDlgItem(IDC_PASTELEVEL);
  if (pWnd!=NULL) pWnd->EnableWindow(FALSE);

  UpdateData(FALSE);

  if (!levellist.IsEmpty())
  {
    CString msg;
    msg.Format("The LVL files for the following levels are missing:\n%s",
                levellist);
    MsgBoxError(msg, "Missing Level Warning");
  }
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectLevel::AddToAvailList(int level)
{
  int listIdx = m_LevelList.GetItemCount();

  char num[4];
  char name[51];
  char used[4];
  char overland[4];
  char width[6];
  char height[6];
  char aview[15];

  sprintf(num, "%u", level+1);

  strncpy(name, globalData.levelInfo.stats[level].level_name, 50);
  name[50]='\0';

  if (globalData.levelInfo.stats[level].used)
    strcpy(used, "Yes");
  else
    strcpy(used, "No");

  if (globalData.levelInfo.stats[level].overland)
    strcpy(overland, "Yes");
  else
    strcpy(overland, "No");

  if (globalData.levelInfo.stats[level].used)
  {
    sprintf(width, "%u", globalData.levelInfo.stats[level].area_width);
    sprintf(height, "%u", globalData.levelInfo.stats[level].area_height);
  }
  else 
  {
    strcpy(width, "0");
    strcpy(height, "0");
  }

  switch (globalData.levelInfo.stats[level].AVStyle)
  {
  case AnyView:         
    strcpy(aview, "Any"); 
    break;
  case OnlyAreaView:    
    if (globalData.levelInfo.stats[level].overland)
      strcpy(aview, "Large Only"); 
    else
      strcpy(aview, "Area Only");
    break;
  case Only3DView:
    if (globalData.levelInfo.stats[level].overland)
      strcpy(aview, "Small Only"); 
    else
      strcpy(aview, "3D Only"); 
    break;
    default: strcpy(aview, "Unknown"); break;
  }

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = num;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = level;

  m_LevelList.InsertItem(&item);

  m_LevelList.SetItemText(listIdx, 1, name); 
  m_LevelList.SetItemText(listIdx, 2, used); 
  m_LevelList.SetItemText(listIdx, 3, overland); 
  m_LevelList.SetItemText(listIdx, 4, width); 
  m_LevelList.SetItemText(listIdx, 5, height); 
  m_LevelList.SetItemText(listIdx, 6, aview); 

  //m_LevelList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_LevelList.SelectItem(listIdx, TRUE);
}

void CSelectLevel::UpdateAvailList(int ListItem)
{
  char name[51];
  char used[4];
  char overland[4];
  char width[6];
  char height[6];
  char aview[15];

  int level = m_LevelList.GetItemData(ListItem);

  strncpy(name, globalData.levelInfo.stats[level].level_name, 50);
  name[50]='\0';

  if (globalData.levelInfo.stats[level].used)
    strcpy(used, "Yes");
  else
    strcpy(used, "No");

  if (globalData.levelInfo.stats[level].overland)
    strcpy(overland, "Yes");
  else
    strcpy(overland, "No");

  if (globalData.levelInfo.stats[level].used)
  {
    sprintf(width, "%u", globalData.levelInfo.stats[level].area_width);
    sprintf(height, "%u", globalData.levelInfo.stats[level].area_height);
  }
  else 
  {
    strcpy(width, "0");
    strcpy(height, "0");
  }

  switch (globalData.levelInfo.stats[level].AVStyle)
  {
  case AnyView:         
    strcpy(aview, "Any"); 
    break;
  case OnlyAreaView:    
    if (globalData.levelInfo.stats[level].overland)
      strcpy(aview, "Large Only"); 
    else
      strcpy(aview, "Area Only");
    break;
  case Only3DView:
    if (globalData.levelInfo.stats[level].overland)
      strcpy(aview, "Small Only"); 
    else
      strcpy(aview, "3D Only"); 
    break;
  default: strcpy(aview, "Unknown"); break;
  }

  m_LevelList.SetItemText(level, 1, name); 
  m_LevelList.SetItemText(level, 2, used); 
  m_LevelList.SetItemText(level, 3, overland); 
  m_LevelList.SetItemText(level, 4, width); 
  m_LevelList.SetItemText(level, 5, height); 
  m_LevelList.SetItemText(level, 6, aview); 

  //m_LevelList.SetItemState(level, LVIS_SELECTED, LVIS_SELECTED);
  m_LevelList.SelectItem(level, TRUE);
}

void CSelectLevel::OnDblclkLevellist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();	
	*pResult = 0;
}

void CSelectLevel::OnEditprops() 
{
  UpdateData(TRUE);

  /*
    ** these properties can be modified by the CMapStyle dialog **

  BYTE area_height;
	BYTE area_width;
  BOOL overland;
  GlobalAreaViewStyle AVStyle;
  CString level_name;
  CString StepSound;
  CString BumpSound;
  BACKGROUND_SOUND_DATA bgSounds;
  */

  POSITION pos = m_LevelList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_LevelList.GetNextSelectedItem(pos);
    int level = m_LevelList.GetItemData(nItem);
    m_level = level;
    tempLEVEL->Clear();
    BOOL Loaded=FALSE;
    BOOL IsNewLevel=FALSE;
    LEVEL *pLevel=tempLEVEL;

    // if existing overland map, we need to populate
    // the array of background slots to get the
    // correct overland map name
    CString ovName="";
    if (globalData.levelInfo.stats[level].used)        
    {
      if (level == globalData.currLevel)
      {
        pLevel = &levelData;
        Loaded=TRUE;
      }
      else 
      {
        if (levelExists(level))
        {
          if (LoadLevel(*pLevel, level))
            Loaded=TRUE;
        }
      }
    }
    else
    {
      if (level == globalData.currLevel)
        pLevel = &levelData;
    }

    if (Loaded)
    {
      if (globalData.levelInfo.stats[level].overland)
        ovName=pLevel->m_BackgroundSets[1].backgroundFile;
    }

    MapStyle dlg(globalData.levelInfo.stats[level], !globalData.levelInfo.stats[level].used);
    dlg.m_data = globalData.levelInfo.stats[level];
    dlg.m_ovName = ovName;

    if (dlg.DoModal()==IDOK)
    { 
      // only need to save the level data if something about it has changed.
      BOOL SaveIt=FALSE;

      // can't change this without deleting level and starting over
      if (globalData.levelInfo.stats[level].used)
      {
        dlg.m_data.overland = globalData.levelInfo.stats[level].overland;
        if (dlg.m_data.overland) dlg.m_Type = OVERLAND_TYPE;
        else dlg.m_Type = DUNGEON_TYPE;
      }

      //if (level != globalData.currLevel)
      //  CheckForUnsavedLevel();

     // ChangedSinceLastSave |= 
     //   (!(dlg.m_data == globalData.levelInfo.stats[level]));
      if (!(dlg.m_data == globalData.levelInfo.stats[level]))
          SET_MODIFIED;
      



      if (dlg.m_Type == OVERLAND_TYPE)
      {
        if (dlg.m_ovName=="")
#ifdef SIMPLE_STRUCTURE
          dlg.m_ovName = ede.TemplateOverlandArtDir() + DEFAULT_OVERLAND;
#else
          dlg.m_ovName.Format("%s%s", EditorArt, DEFAULT_OVERLAND);
#endif
        m_ovName = dlg.m_ovName;
        if (pLevel->m_BackgroundSets[1].backgroundFile.CompareNoCase(m_ovName)!=0)
          SaveIt=TRUE;
        pLevel->m_BackgroundSets[1].backgroundFile = m_ovName;
        //if (level == globalData.currLevel)
        //  BackgroundSets[1].backgroundFile=m_ovName;
      }

      if (!globalData.levelInfo.stats[level].used)
      {
        IsNewLevel=TRUE;
        globalData.levelInfo.stats[level] = dlg.m_data;
        globalData.levelInfo.stats[level].used = TRUE; 
        
        fillDefaultLevelData(*pLevel, level, 
                             dlg.m_mapWidth, dlg.m_mapHeight, 
                             (dlg.m_Type == OVERLAND_TYPE),                            
                             dlg.m_UseDefBG);
        // not loading the art in this case, so it's ok to 
        // fill art slots after fillDefaultLevelData()
        fillDefaultArtSlots(*pLevel, (dlg.m_Type == OVERLAND_TYPE));

        if (dlg.m_Type == OVERLAND_TYPE)
          pLevel->m_BackgroundSets[1].backgroundFile = m_ovName;

        SaveIt=TRUE;
        //globalData.currLevel = level;
      }
      else
      {
        ASSERT(Loaded);

        if (dlg.m_mapHeight != globalData.levelInfo.stats[level].area_height)
        {
          SaveIt=TRUE;
          if (dlg.m_mapHeight < globalData.levelInfo.stats[level].area_height)
          {
            // if height shrinking, clear unused data
            for (int i=dlg.m_mapHeight;i<globalData.levelInfo.stats[level].area_height;i++)
              for (int j=0;j<globalData.levelInfo.stats[level].area_width;j++)
              {
                if (pLevel->area[i][j].eventExists)
                  pLevel->eventData.DeleteEvents(pLevel->eventData.GetFirstEvent(j,i));
                pLevel->area[i][j].Clear();
              }
          }
          else
          {
            if (!globalData.levelInfo.stats[level].overland)
            {
              // if height expanding, fill in default background
              long surf = FindFirstLoadedBackground();
              if (surf >= 0)
              {
                for (unsigned int i=globalData.levelInfo.stats[level].area_height;i<dlg.m_mapHeight;i++)
                  for (unsigned int j=0;j<globalData.levelInfo.stats[level].area_width;j++)
                    pLevel->area[i][j].SetAllBG(surf);
              }
            }
          }
        }

        if (dlg.m_mapWidth != globalData.levelInfo.stats[level].area_width)
        {
          SaveIt=TRUE;
          if (dlg.m_mapWidth < globalData.levelInfo.stats[level].area_width)
          {
            // if width shrinking, clear unused data
            for (int i=0;i<globalData.levelInfo.stats[level].area_height;i++)
              for (int j=dlg.m_mapWidth;j<globalData.levelInfo.stats[level].area_width;j++)
              {
                if (pLevel->area[i][j].eventExists)
                  pLevel->eventData.DeleteEvents(pLevel->eventData.GetFirstEvent(j,i));
                pLevel->area[i][j].Clear();
              }
          }
          else
          {
            if (!globalData.levelInfo.stats[level].overland)
            {
              // if width expanding, fill in default background
              long surf = FindFirstLoadedBackground();
              if (surf >= 0)
              {
                for (unsigned int i=0;i<globalData.levelInfo.stats[level].area_height;i++)
                  for (unsigned int j=globalData.levelInfo.stats[level].area_width;j<dlg.m_mapWidth;j++)
                    pLevel->area[i][j].SetAllBG(surf);
              }
            }
          }
        }
        globalData.levelInfo.stats[level] = dlg.m_data;    
        globalData.levelInfo.stats[level].used = TRUE;
      }

      if (SaveIt)
      {
        int response = IDYES;
        if ((!IsNewLevel) && (m_SaveWarn))
        {
          CString temp;
          if (level == globalData.currLevel)
          {
            temp.Format("Ok to save level - \'%s\'?\n"
                        "Changes may be lost otherwise", 
                        globalData.levelInfo.stats[level].level_name);
          }
          else
          {
            temp.Format("Ok to save level - \'%s\'?\n"
                        "Changes will be lost otherwise", 
                        globalData.levelInfo.stats[level].level_name);
          }

          response = MsgBoxYesNo(temp, "Level Save Permission");
        }
    
        if (response == IDYES)
        {
          pLevel->SaveArt(level);
          saveLevel(*pLevel, level);          
        }
      }
      UpdateAvailList(nItem);
    }
  }
}

void CSelectLevel::OnOK() 
{
  UpdateData(TRUE);

  POSITION pos = m_LevelList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_LevelList.GetNextSelectedItem(pos);
    int level = m_LevelList.GetItemData(nItem);
    m_level = level;
    
    //if (m_level != globalData.currLevel)
    //  CheckForUnsavedLevel();

    if (!globalData.levelInfo.stats[m_level].used)
    {
      CWaitCursor cursor;
      OnEditprops();

      if (!globalData.levelInfo.stats[m_level].used)
        m_level = -1;

      /*
      MapStyle dlg(globalData.levelInfo.stats[m_level], TRUE);

      if (dlg.DoModal() == IDOK)
      { 
        
        globalData.levelInfo.stats[m_level] = dlg.m_data;
        //fillDefaultArtSlots((dlg.m_Type == OVERLAND_TYPE));
        //fillDefaultLevelData(m_level, 
        //                     dlg.m_mapWidth, dlg.m_mapHeight, 
        //                     (dlg.m_Type == OVERLAND_TYPE), 
        //                     dlg.m_UseDefBG);        

        if (dlg.m_Type == OVERLAND_TYPE)
        {
          if (dlg.m_ovName=="")
            dlg.m_ovName.Format("%s%s", EditorArt, DEFAULT_OVERLAND);
          m_ovName = dlg.m_ovName;
        }

        UpdateAvailList(nItem);
      }
      else
        m_level = -1;
      */
    }
  }

  globalData.levelInfo.numLevels = 0;

  for (int i=0;i<MAX_LEVELS;i++)
  {
    if (globalData.levelInfo.stats[i].used)
      globalData.levelInfo.numLevels++;
  }
  
  if (globalData.levelInfo.numLevels > 0)
  {
    // will return first level if m_level is currently invalid
    m_level = CheckCurrentLevel();
    ReleaseTempLevel();
    EndDialog(IDOK);
  }
  else
  {
    MsgBoxError("There must be at least one available level", "Level Warning");
  }
}

void CSelectLevel::OnImportLevel(void)
{
  CString filename;
  POSITION pos;
  int nItem;
  int finalStatus;
  UpdateData();
  pos = m_LevelList.GetFirstSelectedItemPosition();
  nItem = m_LevelList.GetNextSelectedItem(pos);
  m_level = m_LevelList.GetItemData(nItem);
  if (m_level != globalData.currLevel)
  {
    MsgBoxInfo("You can only import the 'Currently Selected Level'.  First, select the level you wish to replace.");
    return;
  };
  filename.Format("%sLevel%03d.txt", rte.DataDir(), m_level+1);
  if (GetFilename(filename, "txt", TRUE))
  {  
    CFile cf;
    cf.Open(filename, CFile::modeRead);
    JReader jr(&cf);
    AfxGetApp()->DoWaitCursor(1);
    try
    {
      finalStatus = jr.Initialize();
      if (finalStatus != 0) 
      {
        throw 1;
      };
    }
    catch (int)
    {
      CString msg;
      msg.Format("Unable to Import Level\n%s", jr.GetErrorMessage());
      MsgBoxInfo(msg);
      AfxGetApp()->DoWaitCursor(-1);
      return;
    };
    {
      LEVEL tempLevel;
      LEVEL_STATS tempLevelStats;
      try
      {
         jr.StartList();
         finalStatus = tempLevelStats.Import(jr);
         finalStatus = tempLevel.Import(jr);
         jr.EndList();
      }
      catch(int e)
      {
        e = 1;
        MsgBoxInfo(jr.GetErrorMessage());
        finalStatus = CONFIG_STAT_semantic;
      };
      if (finalStatus != CONFIG_STAT_ok)
      {
        MsgBoxInfo("We failed to import the Level data");
        AfxGetApp()->DoWaitCursor(-1);
        return;
      };
      tempLevelStats.used = globalData.levelInfo.stats[m_level].used;
      if (!(globalData.levelInfo.stats[m_level] == tempLevelStats))
      {
        int result;
        result = MessageBox( "Changing global level stats", "Warning", MB_OKCANCEL);
        if (result != IDOK) finalStatus = CONFIG_STAT_syntax;
      };
      if (!(levelData == tempLevel))
      {
        int result;
        result = MessageBox("Changing level data", "Warning", MB_OKCANCEL);
        if (result != IDOK) finalStatus = CONFIG_STAT_syntax;
      };
      if (finalStatus == CONFIG_STAT_ok)
      {
        //CString origLevelName;
        //origLevelName = globalData.levelInfo.stats[m_level].level_name;
        globalData.levelInfo.stats[m_level] = tempLevelStats;
        //globalData.levelInfo.stats[m_level].level_name = origLevelName; 
        saveLevel(tempLevel, m_level);
        LoadLevel(m_level);
      };
    };
    AfxGetApp()->DoWaitCursor(-1);
    SET_MODIFIED;
    CString temp;
    if (finalStatus == CONFIG_STAT_ok)
    {
      MsgBoxInfo("Operation Successful", "Data File Message");
    }
    else
    {
      MsgBoxInfo("Level Data Not Loaded", "Data File Message");
    };
    UpdateData(FALSE);
  };
}


void CSelectLevel::OnExportLevel(void)
{
   CString fullPath;
   POSITION pos;
   int nItem;
   UpdateData();
   pos = m_LevelList.GetFirstSelectedItemPosition();
   nItem = m_LevelList.GetNextSelectedItem(pos);
   m_level = m_LevelList.GetItemData(nItem);
   if (m_level != globalData.currLevel)
   {
     MsgBoxInfo("You can only export the 'Currently Selected Level'.  First, select a level and then attempt the Export.");
     return;
   };
   fullPath.Format("%sLevel%.3i.txt", rte.DataDir(), m_level+1);
//   if (GetFilename(filename, "TXT", FALSE))
//   {
     CONFIG_FILE ConfFile;
     AfxGetApp()->DoWaitCursor(1);
     BOOL success = (ExportLevelDB(fullPath, m_level, &levelData, &ConfFile)==0);
//     if (m_testMode)
//     {
//       MONSTER_DATA_TYPE monsterDataTest;
//       if (ReadMonsterDB(filename, &monsterDataTest, &ConfFile) == 0)
//       {
//         monsterDataTest.version = monsterData.version;
//         if (!(monsterDataTest == monsterData))
//         {
//           MsgBoxError("Data Mismatch when importing the file we wrote", "Export/Import error");
//         };
//       }
//       else
//       {
//         MsgBoxError("Error reading back the file we wrote", "Export/Import error"); 
//       };
//     };

//     AfxGetApp()->DoWaitCursor(-1);

     if (!success)
        MsgBoxError("Operation Failed!", "Database Error");
     else
        MsgBoxInfo("Operation Successful", "Database Message");
//   }
   m_testMode = FALSE;
   UpdateData(FALSE);
}

void CSelectLevel::OnCopylevel() 
{

  POSITION pos = m_LevelList.GetFirstSelectedItemPosition();
  if (pos == NULL) return;
  int nItem = m_LevelList.GetNextSelectedItem(pos);
  m_CopyLevel = m_LevelList.GetItemData(nItem);

  CString fullPath;
  fullPath.Format("%sLevel%.3i.lvl",rte.DataDir(),m_CopyLevel+1);

  CWnd *pWnd = GetDlgItem(IDC_PASTELEVEL);
  if (FileExists(fullPath))
  {
    if (pWnd!=NULL) pWnd->EnableWindow(TRUE);
  }
  else
  {
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    MsgBoxInfo("Source level must be saved before it can be copied");
  }
}

void CSelectLevel::OnPastelevel() 
{
  if (m_CopyLevel < 0) return;
  POSITION pos = m_LevelList.GetFirstSelectedItemPosition();
  if (pos == NULL) return;
  int nItem = m_LevelList.GetNextSelectedItem(pos);
  int PasteLevel = m_LevelList.GetItemData(nItem);

  // copy level file
  CString srcFile;
  CString dstFile;

  CString fullPath;
  fullPath.Format("%sLevel%.3i.lvl", rte.DataDir(), m_CopyLevel+1);
  srcFile = fullPath;

  if (m_CopyLevel == globalData.currLevel)
  {
    if ((!FileExists(fullPath)) || IS_MODIFIED)
    {
        int response = IDYES;
        if (m_SaveWarn)
        {
          CString msg;
          msg.Format("Ok to save level - \'%s\'?\n"
                      "Level needs to be saved for copy/paste operation", 
                      globalData.levelInfo.stats[m_CopyLevel].level_name);

          response = MsgBoxYesNo(msg, "Level Save Permission");
        }
    
        if (response == IDYES)
          saveLevel(levelData, m_CopyLevel);
        else
          return;
    }
  }
  else
  {
    if (!FileExists(fullPath))
    {
      CWnd *pWnd = GetDlgItem(IDC_PASTELEVEL);
      if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
      MsgBoxInfo("Source level must be saved before it can be copied");
      return;
    }
  }

  fullPath.Format("%sLevel%.3i.lvl", rte.DataDir(), PasteLevel+1);
  dstFile = fullPath;

  if (FileExists(fullPath))
  {
    int result = MsgBoxYesNo("Destination level exists, Overwrite?", "Warning");
    if (result != IDYES) return;
    SetFileAttributes(dstFile, FILE_ATTRIBUTE_NORMAL);
  }  

  // update global level list
  globalData.levelInfo.stats[PasteLevel] = globalData.levelInfo.stats[m_CopyLevel];
  
  SetFileAttributes(dstFile, FILE_ATTRIBUTE_NORMAL);
  if (!CopyFile(srcFile, dstFile, FALSE))
    WriteDebugString("CopyFile failed (%u): \'%s\' to \'%s\'\n", GetLastError(), srcFile, dstFile);
  SET_MODIFIED;
  UpdateAvailList(nItem);
}

void CSelectLevel::OnClearlevel() 
{
  int result = MsgBoxYesNo("Clear this entire level of all events and reset defaults?",
                           "Level Clear Confirmation");                           
  if (result != IDYES) return;

  POSITION pos = m_LevelList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    SET_MODIFIED;
    int nItem = m_LevelList.GetNextSelectedItem(pos);
    int level = m_LevelList.GetItemData(nItem);
   
    if (level == globalData.currLevel)
    {
      if (pWallSlotMgr!=NULL) pWallSlotMgr->Clear(); 
      if (pBgSlotMgr!=NULL) pBgSlotMgr->Clear();

      levelData.Clear();
      fillDefaultArtSlots(levelData,globalData.levelInfo.stats[level].overland);
      levelData.SetSlots();
      if (loadWallSetSurfaces())
      {
        currBlockage = BlockedBlk;
        currWallSlot = FindFirstLoadedWall();
        currBkGnd = FindFirstLoadedBackground();
      }

      fillDefaultLevelData(levelData, level, 
                           globalData.levelInfo.stats[level].area_width, 
                           globalData.levelInfo.stats[level].area_height, 
                           globalData.levelInfo.stats[level].overland, 
                           TRUE);        

      levelData.GetSlots();
    }
    else
    {
      if (!globalData.levelInfo.stats[level].used)
        return;
      if (!levelExists(level))
        return;

      tempLEVEL->Clear();      
      fillDefaultLevelData(*tempLEVEL, level, 
                           globalData.levelInfo.stats[level].area_width, 
                           globalData.levelInfo.stats[level].area_height, 
                           globalData.levelInfo.stats[level].overland, 
                           TRUE);  
      fillDefaultArtSlots(*tempLEVEL,globalData.levelInfo.stats[level].overland);
      saveLevel(*tempLEVEL, level);
    }   

    UpdateAvailList(nItem);
  }		
}

void CSelectLevel::OnDeletelevel() 
{
  int result = MsgBoxYesNo("Delete this entire level?",
                           "Level Delete Confirm");                           
  if (result != IDYES) return;

  POSITION pos = m_LevelList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    SET_MODIFIED;
    int nItem = m_LevelList.GetNextSelectedItem(pos);
    int level = m_LevelList.GetItemData(nItem);
    
    deleteLevel(level);
    //globalData.levelInfo.stats[level].level_name.Format("Level %i", level+1);   

    UpdateAvailList(nItem);

    globalData.levelInfo.numLevels = 0;
    for (int i=0;i<MAX_LEVELS;i++)
    {
      if (globalData.levelInfo.stats[i].used)
        globalData.levelInfo.numLevels++;
    }
  }	
}

void CSelectLevel::OnSavewarn() 
{
  UpdateData(TRUE);
  EditorOptionsData.m_LastAutoLevelSaveWarn = m_SaveWarn;
}
