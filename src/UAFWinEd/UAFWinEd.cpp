/******************************************************************************
* Filename: UAFWinEd.cpp
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
//#include "..\Shared\ProjectVersion.h"
#include "resource.h"
//#include "externs.h"

#include <afxpriv.h>
#include "UAFWinEd.h"

#include "MainFrm.h"
#include "UAFWinEdDoc.h"
#include "UAFWinEdView.h"

#include "items.h"
#include "monster.h"
#include "spell.h"
//#include "class.h"
#include "Graphics.h"
#include "Level.h"
#include "SoundMgr.h"
#include "FileParse.h"
#include "itemdb.h"
#include "PicSlot.h"
#include "Party.h"
#include "SearchParamDlg.h"
#include "XBrowseForFolder.h"
#include "GlobalDataDlg.h"
#pragma warning (disable:4091)
#include "imagehlp.h"
#include "ImportWarning.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void ImportGlobal(CString filename);
void ProcessScriptFile(const CString& filename);

extern const double VersionSpellNames;
extern const double VersionSpellIDs;
extern const double PRODUCT_VER;


CEditorOptionsData EditorOptionsData;

HANDLE m_hAppEvent=NULL;

COLORREF m_Colors[MAX_COLOR_SLOTS];
HBRUSH   m_ColorBrushes[MAX_COLOR_SLOTS];

BOOL PassThruWalls = TRUE;
BOOL DoubleFillWallSlot = TRUE;
BOOL QuadFillBGSlot = TRUE;
BOOL MODIFIED_FLAG = FALSE;
BOOL PrevSaveState = FALSE;

int currBkGnd = 1;
int currCombat = 0;
int currWallSlot = 1;
int currBlockage = 0;
int currZone = 0;
int currEntryPoint = 0;
int currX = 0;
int currY = 0;
int currFacing = FACE_EAST;

// How many default images will be loaded on startup.
// Make sure it is less than MAX_WALLSETS and
// MAX_BACKGROUNDS
int MaxDefaultArtSlots = 16;

int m_mapType = DUNGEON_TYPE;
int m_mode = WALL_MODE;

bool m_accessMode=false; // Set if we are to show level access in WALL_MODE
BOOL m_KwikKlik=TRUE;
BOOL m_BLOCK_Type = TRUE;
BOOL m_TileMap = FALSE;


COLORREF GetWallColor(int slot)
{
  if ((slot < 0) || (slot >= MAX_COLOR_SLOTS)) slot=0;
  return m_Colors[slot];
}

HBRUSH GetWallColorBrush(int slot)
{
  if ((slot < 0) || (slot >= MAX_COLOR_SLOTS)) slot=0;
  return m_ColorBrushes[slot];
}
COLORREF GetBackdropColor(int slot)
{
  if ((slot < 0) || (slot >= MAX_COLOR_SLOTS)) slot=0;
  return m_Colors[slot];
}
HBRUSH GetBackdropColorBrush(int slot)
{
  if ((slot < 0) || (slot >= MAX_COLOR_SLOTS)) slot=0;
  return m_ColorBrushes[slot];
}

COLORREF GetObstructionColor(int slot)
{
  if ((slot < 0) || (slot >= MAX_COLOR_SLOTS)) slot=0;
  if (slot == 0) return m_Colors[0];
  if (slot == 1) return m_Colors[1];
  if (slot == 2) return m_Colors[14];
  if (slot == 3) return m_Colors[15];
  return m_Colors[slot-2];
}
HBRUSH GetObstructionColorBrush(int slot)
{
  if ((slot < 0) || (slot >= MAX_COLOR_SLOTS)) slot=0;
  if (slot == 0) return m_ColorBrushes[0];
  if (slot == 1) return m_ColorBrushes[1];
  if (slot == 2) return m_ColorBrushes[14];
  if (slot == 3) return m_ColorBrushes[15];
  return m_ColorBrushes[slot-2];
}

COLORREF GetZoneColor(int slot)
{
  if ((slot < 0) || (slot >= MAX_COLOR_SLOTS)) slot=0;
  return m_Colors[slot];
}
HBRUSH GetZoneColorBrush(int slot)
{
  if ((slot < 0) || (slot >= MAX_COLOR_SLOTS)) slot=0;
  return m_ColorBrushes[slot];
}
COLORREF GetEPColor(int slot)
{
  if ((slot < 0) || (slot >= MAX_COLOR_SLOTS)) slot=0;
  return m_Colors[slot];
}
HBRUSH GetEPColorBrush(int slot)
{
  if ((slot < 0) || (slot >= MAX_COLOR_SLOTS)) slot=0;
  return m_ColorBrushes[slot];
}

CEditorOptionsData::CEditorOptionsData()
{
  Clear();
}

void CEditorOptionsData::Clear()
{
  m_AutoReloadDesign=FALSE;
  m_LastSaveSuccess=FALSE;
  m_LastLoadSuccess=FALSE;
  m_LastDesign="";
  m_LastDesignLevel=0;
  m_LastDesignX=0;
  m_LastDesignY=0;
  m_EditorScreenRes=res640x480;
  m_LastBkgnd=1;
  m_LastWall=1;
  m_LastFacing=FACE_EAST;
  m_LastMode=WALL_MODE;
  m_LastKwikKlik=TRUE;
  m_LastBlockType=TRUE;
  m_LastTileMap=FALSE;
  m_LastPassWalls=TRUE;
  m_LastDoubleFillWall=TRUE;
  m_LastQuadFileBg=TRUE;
  m_LastAreaViewMode=FALSE;
  m_LastAutoLevelSaveWarn=FALSE;
  m_LastEventTypeUsed=TextStatement;
}

CEditorOptionsData& CEditorOptionsData::operator=(const CEditorOptionsData& src)
{
  if (this==&src) return *this;
  Clear();
  
  m_AutoReloadDesign=src.m_AutoReloadDesign;
  m_LastSaveSuccess=src.m_LastSaveSuccess;
  m_LastLoadSuccess=src.m_LastLoadSuccess;
  m_LastDesign=src.m_LastDesign;
  m_EditorScreenRes=src.m_EditorScreenRes;
  m_LastDesignLevel=src.m_LastDesignLevel;
  m_LastDesignX=src.m_LastDesignX;
  m_LastDesignY=src.m_LastDesignY;
  m_LastBkgnd=src.m_LastBkgnd;
  m_LastWall=src.m_LastWall;
  m_LastFacing=src.m_LastFacing;
  m_LastMode=src.m_LastMode;
  m_LastKwikKlik=src.m_LastKwikKlik;
  m_LastBlockType=src.m_LastBlockType;
  m_LastTileMap=src.m_LastTileMap;
  m_LastPassWalls=src.m_LastPassWalls;
  m_LastDoubleFillWall=src.m_LastDoubleFillWall;
  m_LastQuadFileBg=src.m_LastQuadFileBg;
  m_LastAreaViewMode=src.m_LastAreaViewMode;
  m_LastAutoLevelSaveWarn=src.m_LastAutoLevelSaveWarn;
  m_LastEventTypeUsed=src.m_LastEventTypeUsed;

  return *this;
}

class RegistryManipulate
{
  HKEY hkey;
public:
  bool Create(void);
  int  SetValue(DWORD value, const char *name);
  int  SetValue(const char *value, const char *name);
  int  QueryValue(DWORD& value, const char *name);
  int  QueryValue(CString* value, const char *name);
  int  QueryValue(char* value, const char *name, DWORD *size);
  void Close(void);
};

int RegistryManipulate::SetValue(DWORD value, const char *name)
{
  int status;
  status = RegSetValueEx(hkey,
                         name,
                         0,
                         REG_DWORD,
                         (BYTE *)&value,
                         sizeof(DWORD));
  return status;
}
int RegistryManipulate::QueryValue(DWORD& value, const char *name)
{
  int status;
  DWORD temp, type, size=sizeof(DWORD);
  status = RegQueryValueEx(hkey,
                           name,
                           0,
                           &type,
                           (BYTE *)&temp,
                           &size);
  if (temp != 0xcccccccc)
  {
    value = temp;
    return status;
  };
  return ERROR_INVALID_DATA;
}
int RegistryManipulate::SetValue(const char *value, const char *name)
{
  int status;
  status = RegSetValueEx(hkey,
                         name,
                         0,
                         REG_SZ,
                         (BYTE *)&value,
                         strlen(value)+1);
  return status;
}
int RegistryManipulate::QueryValue(char *value, const char *name, DWORD *len)
{
  DWORD status, type;
  status = RegQueryValueEx(hkey,
                           name,
                           0,
                           &type,
                           (BYTE *)value,
                           len);
  return status;
}
void RegistryManipulate::Close(void)
{
  RegCloseKey(hkey);
  hkey = NULL;
}
bool RegistryManipulate::Create(void)
{
  int status;
  status = RegCreateKeyEx(HKEY_CURRENT_USER,
    "SOFTWARE\\UAFEditor",
    0,
    0,
    REG_OPTION_NON_VOLATILE,
    KEY_ALL_ACCESS,
    0,
    &hkey,
    0);
  return status == ERROR_SUCCESS;
}

BOOL CEditorOptionsData::Serialize(BOOL IsStoring)
{
#define AUTO_RELOAD_KEY "AutoReloadDesign"
#define LAST_SAVE_SUCCESS_KEY "LastSaveSuccess"
#define LAST_LOAD_SUCCESS_KEY "LastLoadSuccess"
#define LAST_DESIGN_KEY "LastDesignName"
#define SCREEN_RES_KEY "ScreenRes"
#define LAST_DESIGN_LEVEL_KEY "LastDesignLevel"
#define LAST_DESIGN_X_KEY "LastDesignX"
#define LAST_DESIGN_Y_KEY "LastDesignY"
#define LAST_BGND_KEY "LastBgnd"
#define LAST_WALL_KEY "LastWall"
#define LAST_FACING_KEY "LastFacing"
#define LAST_MODE_KEY "LastMode"
#define LAST_KWIKKLIK_KEY "LastKwikKlik"
#define LAST_BLOCKTYPE_KEY "LastBlockType"
#define LAST_TILEMAP_KEY "LastTileMap"
#define LAST_PASSWALL_KEY "LastPassWall"
#define LAST_DOUBLEWALL_KEY "LastDoubleFillWall"
#define LAST_QUADBG_KEY "LastQuadFillBg"
#define LAST_AVMODE_KEY "LastAVMode"
#define LAST_AUTOLEVELSAVEWARN_KEY "LastAutoLevelSaveWarn"
#define LAST_EVENTTYPEUSED_KEY "LastEventTypeUsed"
  
  BOOL success=TRUE;

  /* This does not seem to work in Visual Studio 2015.
   * So I will attempt to use the Windows API.
   * Wish me luck.
   *
   * CRegKey key;
   * DWORD rc = key.Create(HKEY_CURRENT_USER, "SOFTWARE\\UAFEditor");  
   * if (rc!=ERROR_SUCCESS) return FALSE;
   */
  RegistryManipulate key;
  key.Create();
  if (IsStoring)
  {
    /*
    LONG SetValue( DWORD dwValue, LPCTSTR lpszValueName );
    LONG SetValue( LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL );
    LONG SetValue( HKEY hKeyParent, LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL );
  
    Return Value
      If successful, returns ERROR_SUCCESS; otherwise, an error value.
    Parameters
      dwValue [in] Specifies the data to be stored.
      lpszValueName [in] Specifies the value field to be set. If a value field with this name does not already exist in the key, it is added.
      lpszValue [in] Specifies the data to be stored. This parameter must be non-NULL.
      hKeyParent [in] The handle of an open key.
      lpszKeyName [in] Specifies the name of a key to be created or opened. This name must be a subkey of hKeyParent.
    */

    if (key.SetValue(m_AutoReloadDesign, AUTO_RELOAD_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastSaveSuccess, LAST_SAVE_SUCCESS_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastLoadSuccess, LAST_LOAD_SUCCESS_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastDesign, LAST_DESIGN_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_EditorScreenRes, SCREEN_RES_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastDesignLevel, LAST_DESIGN_LEVEL_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastDesignX, LAST_DESIGN_X_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastDesignY, LAST_DESIGN_Y_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastBkgnd, LAST_BGND_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastWall, LAST_WALL_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastFacing, LAST_FACING_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastMode, LAST_MODE_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastKwikKlik, LAST_KWIKKLIK_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastBlockType, LAST_BLOCKTYPE_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastTileMap, LAST_TILEMAP_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastPassWalls, LAST_PASSWALL_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastDoubleFillWall, LAST_DOUBLEWALL_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastQuadFileBg, LAST_QUADBG_KEY)!=ERROR_SUCCESS)
      success = FALSE;    
    if (key.SetValue(m_LastAreaViewMode, LAST_AVMODE_KEY)!=ERROR_SUCCESS)
      success = FALSE;    
    if (key.SetValue(m_LastAutoLevelSaveWarn, LAST_AUTOLEVELSAVEWARN_KEY)!=ERROR_SUCCESS)
      success = FALSE;
    if (key.SetValue(m_LastEventTypeUsed, LAST_EVENTTYPEUSED_KEY)!=ERROR_SUCCESS)
      success = FALSE;
  }
  else
  {
    /*
    LONG QueryValue( DWORD& dwValue, LPCTSTR lpszValueName );
    LONG QueryValue( LPTSTR szValue, LPCTSTR lpszValueName, DWORD* pdwCount )

    Return Value
      If successful, returns ERROR_SUCCESS; otherwise, an error value.
    Parameters
      dwValue [out] The value field’s numerical data.
      lpszValueName [in] Specifies the value field to be queried.
      szValue [out] The value field’s string data.
      pdwCount [in, out] The size of the string data. Its value is initially set to the size of the szValue buffer.
    */

    DWORD tmp;
    char tmpText[512+1];
    DWORD tmpTextLen = 512;
    if (key.QueryValue(tmp, AUTO_RELOAD_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_AutoReloadDesign=tmp;
    if (key.QueryValue(tmp, LAST_SAVE_SUCCESS_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastSaveSuccess=tmp;
    if (key.QueryValue(tmp, LAST_LOAD_SUCCESS_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastLoadSuccess=tmp;
    if (key.QueryValue(tmpText, LAST_DESIGN_KEY, &tmpTextLen)!=ERROR_SUCCESS) success = FALSE;
    else { tmpText[512]='\0'; m_LastDesign=tmpText; }
    if (key.QueryValue(tmp, SCREEN_RES_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_EditorScreenRes=tmp;
    if (key.QueryValue(tmp, LAST_DESIGN_LEVEL_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastDesignLevel=tmp;
    if (key.QueryValue(tmp, LAST_DESIGN_X_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastDesignX=tmp;
    if (key.QueryValue(tmp, LAST_DESIGN_Y_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastDesignY=tmp;
    if (key.QueryValue(tmp, LAST_BGND_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastBkgnd=tmp;
    if (key.QueryValue(tmp, LAST_WALL_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastWall=tmp;
    if (key.QueryValue(tmp, LAST_FACING_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastFacing=tmp;
    if (key.QueryValue(tmp, LAST_MODE_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastMode=tmp;
    if (key.QueryValue(tmp, LAST_KWIKKLIK_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastKwikKlik=tmp;
    if (key.QueryValue(tmp, LAST_BLOCKTYPE_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastBlockType=tmp;
    if (key.QueryValue(tmp, LAST_TILEMAP_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastTileMap=tmp;
    if (key.QueryValue(tmp, LAST_PASSWALL_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastPassWalls=tmp;
    if (key.QueryValue(tmp, LAST_DOUBLEWALL_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastDoubleFillWall=tmp;
    if (key.QueryValue(tmp, LAST_QUADBG_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastQuadFileBg=tmp;    
    if (key.QueryValue(tmp, LAST_AVMODE_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastAreaViewMode=tmp;    
    if (key.QueryValue(tmp, LAST_AUTOLEVELSAVEWARN_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastAutoLevelSaveWarn=tmp;
    if (key.QueryValue(tmp, LAST_EVENTTYPEUSED_KEY)!=ERROR_SUCCESS) success = FALSE;
    else m_LastEventTypeUsed=tmp;
    
    if (success)
    {
      if (m_LastFacing >= 4) m_LastFacing=0;
      if (m_LastWall >= MAX_WALLSETS) m_LastWall=0;
      if (m_LastBkgnd >= MAX_BACKGROUNDS) m_LastBkgnd=0;
      if (m_LastDesignX >= MAX_AREA_WIDTH) m_LastDesignX=0;
      if (m_LastDesignY >= MAX_AREA_HEIGHT) m_LastDesignY=0;
      if (m_LastDesignLevel >= MAX_LEVELS) m_LastDesignLevel=0;
    }
  }

  key.Close();
  return success;
}

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdApp

BEGIN_MESSAGE_MAP(CUAFWinEdApp, CWinApp)
  //{{AFX_MSG_MAP(CUAFWinEdApp)
  ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
  ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_APP_HelpWallEdit, OnAPPHelpWallEdit)
	//}}AFX_MSG_MAP
  // Standard file based document commands
  //ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
  //ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdApp construction

CUAFWinEdApp::CUAFWinEdApp() : CWinApp()
{
   m_pDoc = NULL;

   // default all groups of 16 to the same 16 colors.
   // can be changed by color settings in config.txt
   for (int i=0;i<MAX_COLOR_SLOTS/16;i++)
   {
     int start = i*16;
     m_Colors[start++] = RGB(  0,  0,  0); // black
     m_Colors[start++] = RGB(  0,  0,128); // dk blue
     m_Colors[start++] = RGB(  0,128,  0); // dk green
     m_Colors[start++] = RGB(  0,128,128); // turquoise
     m_Colors[start++] = RGB(255,  0,  0); // dk red
     m_Colors[start++] = RGB(192,  0,128); // dk purple
     m_Colors[start++] = RGB(192, 96,  0); // brown
     m_Colors[start++] = RGB(192,192,192); // lt grey
     m_Colors[start++] = RGB(128,128,128); // dk grey
     m_Colors[start++] = RGB(  0,128,255); // blue
     m_Colors[start++] = RGB(  0,255,  0); // lt green
     m_Colors[start++] = RGB(  0,255,255); // lt blue
     m_Colors[start++] = RGB(255,128,  0); // orange
     m_Colors[start++] = RGB(255,128,255); // pink
     m_Colors[start++] = RGB(255,255,  0); // yellow
     m_Colors[start++] = RGB(255,255,255); // white 
   }
   
   //_set_sbh_threshold(0);
   currX = 0;
   currY = 0;
   currFacing = FACE_EAST;  
   party.facing = currFacing;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUAFWinEdApp object

CUAFWinEdApp theApp;

bool CUAFWinEdApp::OpenDesign(CString filename)
{
  CString configFolderName;
  BOOL success;
  BOOL bNewDesign;
  bNewDesign = FALSE;
  rte.DesignDir("");
  UndoData.RemoveAll();

  {
    char designDir[_MAX_PATH+1];
    rte.Clear();
    int i;
    if (filename == "")
    {
      filename = ede.EditorExecutableDir();
      if (filename.GetAt(filename.GetLength()-1) == '\\')
      {
        filename = filename.Left(filename.GetLength()-1);
      };
      {
        CString msg;
        msg.Format("Editor folder = %s\n",filename);
        WriteDebugString(msg);
      };
      i = filename.ReverseFind('\\');
      if (i >= 0)
      {
        rte.SetRootDir(filename.Left(i+1));
      }
      else
      {
        MsgBoxError("Cannot find folder that contains designs");
        return FALSE;
      };
      // Now we will search for game folders in the root folder.
      if (!XBrowseForFolder(NULL, rte.RootDir(), -1, "Search for Design folder", designDir, _MAX_PATH, TRUE, TRUE))
      {
        designDir[0] = 0;
          //MsgBoxError("Cannot find folder that contains designs");
          //return TFALSE;
          // Creating a new design.  Use editor's DefaultDesign
        rte.DefaultFoldersFromDesign(ede.TemplateDir());
        bNewDesign = TRUE;
      }
      else
      {
        int len;
        WriteDebugString("User chose \"%s\" as design folder\n", designDir);
        len = strlen(designDir);
        if (designDir[len-1] != '\\') strcat(designDir, "\\");
        rte.DefaultFoldersFromDesign(designDir);
        /*
        if ( filename.Right(11) == "\\config.txt")
        {
          // It appears that a config filename was specified on the command line.
          if (FileExists(filename))
          {
            // Strip off the "config.txt".  The rest is the dataDir
            strcpy(global_configDir, filename.Left(filename.GetLength()-10));
            strcpy(global_dataDir, global_configDir);
            filename = filename.Left(filename.GetLength() - 11);
            i = filename.ReverseFind('\\');
            if (i >= 0)
            {
              strcpy(global_designDir, filename.Left(i+1));
              filename = filename.Left(i);
              i = filename.ReverseFind('\\');
              if (i >= 0)
              {
                strcpy(global_rootDir, filename.Left(i+1));
              };
            };
    
          };
        };
        */
      };
    }
    else
    {
      rte.DefaultFoldersFromDesign(filename);
    };
/*
    if (global_designDir[strlen(global_designDir)-1] != '\\')
    {
      strcat(global_designDir,"\\");
    }

    if (global_configDir[0] == 0)
    {
      strcpy(global_configDir, global_designDir);
      strcat(global_configDir, "Data\\");
    };

    if (global_dataDir[0] == 0)
    {
      strcpy(global_dataDir, global_designDir);
      strcat(global_dataDir, "Data\\");
    };

    if (global_artDir[0] == 0)
    {
      strcpy(global_artDir, global_designDir);
      strcat(global_artDir, "Resources\\");
    };

    if (global_saveDir[0] == 0)
    {
      strcpy(global_saveDir, global_designDir);
      strcat(global_saveDir, "Saves\\");
    };
    */
  };




  configFolderName = rte.ConfigDir();
  if (configFolderName == "")
  {
    configFolderName = ede.TemplateDataDir();
  };
  //configFilename += "config.txt";
  if (!LoadConfigFile(configFolderName + "config.txt"))
    return FALSE;

  BOOL OptionsLoaded = EditorOptionsData.Serialize(FALSE); // load config data
  if (!OptionsLoaded) EditorOptionsData.Clear();
  
  if ((EditorOptionsData.m_EditorScreenRes < res640x480) || (EditorOptionsData.m_EditorScreenRes > res1280x1024))
    EditorOptionsData.m_EditorScreenRes = res640x480;


  switch (m_GameScreenRes)
  {
  case res640x480:
    if (EditorOptionsData.m_EditorScreenRes==res1152x864)
    {
      // switch editor to legal mode for this game res
      EditorOptionsData.m_EditorScreenRes=res640x480;
    }

    switch (EditorOptionsData.m_EditorScreenRes)
    {
    case res640x480:
      EDITOR_RESOURCE_ID = IDD_3DPICDLG_640x480g_640x480e;
      WriteDebugString("Editor 640x480, Game 640x480\n");
      break;
    case res800x600:
      EDITOR_RESOURCE_ID = IDD_3DPICDLG_640x480g_800x600e;
      WriteDebugString("Editor 800x600, Game 640x480\n");
      break;
    case res1024x768:
      EDITOR_RESOURCE_ID = IDD_3DPICDLG_640x480g_1024x768e;
      WriteDebugString("Editor 1024x768, Game 640x480\n");
      break;
    case res1280x1024:
      EDITOR_RESOURCE_ID = IDD_3DPICDLG_640x480g_1280x1024e;
      WriteDebugString("Editor 1280x1024, Game 640x480\n");
      break;
    }
    break;

  case res800x600:
    if (   (EditorOptionsData.m_EditorScreenRes==res640x480)
        || (EditorOptionsData.m_EditorScreenRes==res1152x864))        
    {
      // switch editor to legal mode for this game res
      EditorOptionsData.m_EditorScreenRes=res800x600;
    }

    switch (EditorOptionsData.m_EditorScreenRes)
    {
    case res800x600:
      EDITOR_RESOURCE_ID = IDD_3DPICDLG_800x600g_800x600e;
      WriteDebugString("Editor 800x600, Game 800x600\n");
      break;
    case res1024x768:
      EDITOR_RESOURCE_ID = IDD_3DPICDLG_800x600g_1024x768e;
      WriteDebugString("Editor 1024x768, Game 800x600\n");
      break;
    case res1280x1024:
      EDITOR_RESOURCE_ID = IDD_3DPICDLG_800x600g_1280x1024e;
      WriteDebugString("Editor 1280x1024, Game 800x600\n");
      break;
    }
    break;

  case res1024x768:
    if (   (EditorOptionsData.m_EditorScreenRes==res640x480)
        || (EditorOptionsData.m_EditorScreenRes==res800x600))
    {
      // switch editor to legal mode for this game res
      EditorOptionsData.m_EditorScreenRes=res1024x768;
    }

    switch (EditorOptionsData.m_EditorScreenRes)
    {
    case res1024x768:
      EDITOR_RESOURCE_ID = IDD_3DPICDLG_1024x768;
      WriteDebugString("Editor 1024x768, Game 1024x768\n");
      break;
    case res1152x864:
      EDITOR_RESOURCE_ID = IDD_3DPICDLG_1152x864;
      WriteDebugString("Editor 1152x864, Game 1024x768\n");
      break;
    case res1280x1024:
      EDITOR_RESOURCE_ID = IDD_3DPICDLG_1280x1024;
      WriteDebugString("Editor 1280x1024, Game 1024x768\n");
      break;
    }
    break;

  default:
    EDITOR_RESOURCE_ID = IDD_3DPICDLG;
    EditorOptionsData.m_EditorScreenRes=res640x480;
    WriteDebugString("Editor running at 640x480 (bogus m_GameScreenRes setting)\n");
    break;
  }
  {
/*
    if (bNewDesign)
    {

      success = ((CMainFrame*)m_pMainWnd)->ConfigureNewDesign(&dlg);
      if (!success)
      {
        return false;
      };
    };
*/
    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.
    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CUAFWinEdDoc),
      RUNTIME_CLASS(CMainFrame),       // main SDI frame window
      RUNTIME_CLASS(CUAFWinEdView));
    AddDocTemplate(pDocTemplate);

    // Parse command line for standard shell commands, DDE, file open
    //CUAFCommandLineInfo cmdLine;

    WriteDebugString("Editor Version: %4.7f\n", PRODUCT_VER);

    for (int i=0;i<MAX_COLOR_SLOTS;i++)
      m_ColorBrushes[i] = ::CreateSolidBrush(m_Colors[i]);

    levelData.Clear();

    if (pWallSlotMgr==NULL)
      pWallSlotMgr = new ImageCacheMgr(WALL_CACHE_SIZE, "WallSlot");
    if (pBgSlotMgr==NULL)
      pBgSlotMgr = new ImageCacheMgr(BACKGROUND_CACHE_SIZE, "BgSlot");

    // Dispatch commands specified on the command line.
    // This also creates and initializes the UAFWinEdView class
    DWORD flags;
    flags = GetFileAttributes(cmdLine.m_strFileName);
    if (flags != 0xffffffff)
    {
      if (flags & FILE_ATTRIBUTE_DIRECTORY)
      {
        if (cmdLine.m_strFileName.Right(1) != '\\')
        {
          cmdLine.m_strFileName += '\\';
        };
        cmdLine.m_strFileName += "Data\\config.txt";
      };
    };
    if (!ProcessShellCommand(cmdLine))
      return FALSE;

    if (!GraphicsMgr.IsInitialized())
    {
      MsgBoxError("Unable to initialize Graphics/DirectX! - Exiting editor",
                  "DirectX Failure");
      return FALSE;
    }

    /*
    // The one and only window has been initialized, so show and update it.
    m_pMainWnd->ShowWindow(SW_SHOW);
    ((CMainFrame*)m_pMainWnd)->RestoreState();
    m_pMainWnd->UpdateWindow();
    */

    m_pDoc = GetDocPtr();   
    if (m_pDoc == NULL)
    {
      WriteDebugString("Failed to get doc ptr in InitInstance\n");
      return FALSE;
    }
  
    m_pMainWnd->ShowWindow(SW_SHOW);
    ((CMainFrame*)m_pMainWnd)->RestoreState();
    m_pMainWnd->UpdateWindow();

    CUAFWinEdView* pView = (CUAFWinEdView*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
    if (pView != NULL) pView->ResetView();

    //if (ReloadSuccess)
    if (EditorOptionsData.m_LastDesign!="")
    {
      if (levelData.ValidAreaMapCoords(EditorOptionsData.m_LastDesignX, EditorOptionsData.m_LastDesignY))
      {
        currX = EditorOptionsData.m_LastDesignX;
        currY = EditorOptionsData.m_LastDesignY;
        if (pView != NULL) pView->MoveMapHorz(currX);
        if (pView != NULL) pView->MoveMapVert(currY);
      }
    }

    if (OptionsLoaded)
    {
      PassThruWalls=EditorOptionsData.m_LastPassWalls;
      DoubleFillWallSlot=EditorOptionsData.m_LastDoubleFillWall;
      QuadFillBGSlot=EditorOptionsData.m_LastQuadFileBg;
      currBkGnd=EditorOptionsData.m_LastBkgnd;
      currWallSlot=EditorOptionsData.m_LastWall;
      currFacing=EditorOptionsData.m_LastFacing;
      m_mode=EditorOptionsData.m_LastMode;
      m_KwikKlik=EditorOptionsData.m_LastKwikKlik;
      m_BLOCK_Type=EditorOptionsData.m_LastBlockType;
      m_TileMap=EditorOptionsData.m_LastTileMap;
      globalData.useAreaView = EditorOptionsData.m_LastAreaViewMode;
    }

     if (currWallSlot >= MaxDefaultArtSlots)
      currWallSlot=FindFirstLoadedWall();

     if (currBkGnd >= MaxDefaultArtSlots)
      currBkGnd=FindFirstLoadedBackground();

     m_mapType = (globalData.levelInfo.stats[globalData.currLevel].overland ? OVERLAND_TYPE : DUNGEON_TYPE);

     if (m_mapType == OVERLAND_TYPE)
    {
      if (m_mode == WALL_MODE)
        m_mode = BLOCKAGE_MODE;
    }
    else
    {
      if (m_mode == BLOCKAGE_MODE)
        m_mode = WALL_MODE;
    }

    m_pDoc->UpdateAllViews(FALSE);

    SetTitle();

#ifdef _DEBUG
  CString temp;
  temp.Format("Sizeof PARTY = %u\n", sizeof(party));
  TRACE(temp);
  temp.Format("Sizeof SPELL_DATA = %u\n", spellData.GetDataSize());
  TRACE(temp);
  temp.Format("Sizeof MONSTER_DATA = %u\n", monsterData.GetDataSize());
  TRACE(temp);
  temp.Format("Sizeof ITEM_DATA = %u\n", itemData.GetDataSize());
  TRACE(temp);
  temp.Format("Sizeof GLOBAL_DATA = %u\n", sizeof(globalData));
  TRACE(temp);
  temp.Format("Sizeof LEVEL_DATA = %u\n", sizeof(levelData));
  TRACE(temp);
  temp.Format("Sizeof RACE_DATA = %u\n", GetDataSize(raceData));
  TRACE(temp);
  temp.Format("Sizeof ABILITY_DATA = %u\n", GetDataSize(abilityData));
  TRACE(temp);
  temp.Format("Sizeof BASECLASS_DATA = %u\n", GetDataSize(baseclassData));
  TRACE(temp);
  temp.Format("Sizeof CLASS_DATA = %u\n", GetDataSize(classData));
  TRACE(temp);
  temp.Format("Sizeof SPELLGROUP_DATA = %u\n", GetDataSize(spellgroupData));
  TRACE(temp);
#endif
    if (bNewDesign)
    {
      globalData.designName = "";
      CGlobalDataDlg dlg(globalData);
  //    dlg.GetData(globalData);
  //    BOOL success;
      success = ((CMainFrame*)m_pMainWnd)->ConfigureNewDesign(&dlg);
      if (!success) return false;
      ((CMainFrame*)m_pMainWnd)->SaveNewDesign();
    };
  };
  if (cmdLine.m_fruaConvert)
  {
    MessageBox(this->GetMainWnd()->m_hWnd, "a", "B", MB_OK);
  };
  return true;
}


/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdApp initialization

BOOL CUAFWinEdApp::InitInstance()
{
  CString filename;
  bool success;
  // only allow one instance of the editor to run at a time
  m_hAppEvent = CreateEvent(NULL, FALSE, FALSE, EDITOR_APP_EVENT);
  if (ERROR_ALREADY_EXISTS == GetLastError())
  {
    MsgBoxError("An instance of the editor is running already");
    CloseHandle(m_hAppEvent);
    m_hAppEvent=NULL;
    return FALSE;
  }

#ifdef _AFXDLL
  Enable3dControls();     // Call this when using MFC in a shared DLL
#else
  Enable3dControlsStatic(); // Call this when linking to MFC statically
#endif
  WriteDebugString("DungeonCraft Editor version %g\n", PRODUCT_VER);
  HRESULT hRes = ::CoInitialize(NULL);
  if (!SUCCEEDED(hRes))
  {
    WriteDebugString("Failed to CoInitialize OLE\n");
    return FALSE;
  }

   //if (!CheckDxInstall())
   //  return FALSE;

  // Change the registry key under which our settings are stored.
  SetRegistryKey(_T("UAF Editor"));

  LoadStdProfileSettings(0);
  { 
    char exDir[_MAX_PATH+1];
    CString executableDir;
    GetModuleFileName(NULL, exDir, _MAX_PATH);
    char *ptr = strrchr(exDir, '\\');
    if (ptr != NULL) *(ptr+1) = 0;  // Delete the .exe name
    executableDir = exDir;
    ede.DefaultFoldersFromExecutable(CString(executableDir));
  };
  ParseCommandLine(cmdLine);

  // if -config flag used on command line, use specified config file
  if (cmdLine.m_ConfigFilename != "")
  {
    filename = cmdLine.m_ConfigFilename;
  }
  else if (cmdLine.m_strFileName != "")
  {
    filename = cmdLine.m_strFileName;
  }
  //AfxEnableControlContainer();
  success = OpenDesign(filename);
  while (!success)
  {
    return FALSE;
  };
  if (!cmdLine.m_ScriptFilename.IsEmpty())
  {
    ProcessScriptFile(rte.DataDir() + cmdLine.m_ScriptFilename);
  };
  return TRUE;
}

CString ErrMsg(int status, char *dbname, int count)
{
  CString result;
  result.Format("%4d %s - ",count, dbname);
  switch(status)
  {
   case 0:
     break;
   case 1:
     WriteDebugString("Minor errors in %s database\n", dbname);
     result += " Minor Errors";
     break;
   case -1:
     WriteDebugString("Cannot read %s database file\n", dbname);
     result += " No File";
     break;
   case -2:
     WriteDebugString("%s database file appears to be the wrong kind of file\n", dbname);
     result += " Wrong File";
     break;
   case -3:
     WriteDebugString("Internal program error reading %s database file\n", dbname);
     result += " Internal Error";
     break;
   default:
     WriteDebugString("Unknown error reading %s database file\n", dbname);
     result += " Unknown Error";
     break;
  };
  result += '\n';
  return result;
}

CString CUAFWinEdApp::LoadDefaultDBFiles()
{
  CWaitCursor cursor;
  CString result;

   // for the editor, the txt config files are
   // loaded - not the *.dat files
   char file[_MAX_PATH+1];   
   int status;
   int count;


   //strcpy(file, rte.DataDir());
   //strcat(file, GLOBAL_CONFIG_NAME);
   //ImportGlobal(file);


   strcpy(file, rte.DataDir());
   strcat(file, RACE_CONFIG_NAME);

   WriteDebugString("Beginning decode of \'%s\'\n", file);

   status = ReadRaceDB(file, &raceData);
   count=GetCount(raceData);
   WriteDebugString("Num races in %s = %u\n", file, count);      
   
   result += ErrMsg(status, "Races", count);

   /*  Removed the "Import Abilities" button 20100909 at Manikus' suggestion PRS
     Added it back 20121205 so we can update syntax for such things as race references */
   strcpy(file, rte.DataDir());
   strcat(file, ABILITY_CONFIG_NAME);

   WriteDebugString("Beginning decode of \'%s\'\n", file);

   status = ReadAbilityDB(file, &abilityData);
   switch (status)
   {
   case 0:
     break;
   case 1:
     WriteDebugString("Minor errors in Abilities database\n");
     break;
   case -1:
     WriteDebugString("Cannot read Abilities database file\n");
     break;
   case -2:
     WriteDebugString("Abilities database file appears to be the wrong kind of file\n");
     break;
   case -3:
     WriteDebugString("Internal program error reading abilities database file\n");
     break;
   default:
     WriteDebugString("Unknown error reading abilities database file\n");
     break;
   };
   
   count=GetCount(abilityData);
   WriteDebugString("Num abilities in %s = %u\n", file, count);      
   result += ErrMsg(status, "Abilities", count);
/* */

/*
#ifdef SIMPLE_STRUCTURE
   strcpy(file, rte.DataDir());
#else
   strcpy(file, m_installDir);
#endif
   strcat(file, SPELLGROUP_CONFIG_NAME);

   WriteDebugString("Beginning decode of \'%s\'\n", file);

   status = ReadSpellgroupDB(file, &spellgroupData);
   count=GetCount(spellgroupData);
   WriteDebugString("Num Spell Groups in %s = %u\n", file, count);      

   result += ErrMsg(status, "SpellGroups", count);
*/   
   strcpy(file, rte.DataDir());
   strcat(file, BASE_CLASS_CONFIG_NAME);

   WriteDebugString("Beginning decode of \'%s\'\n", file);

   status = ReadBaseclassDB(file, &baseclassData);
   
   count=GetCount(baseclassData);
   WriteDebugString("Num Baseclasses in %s = %u\n", file, count);      
   result += ErrMsg(status, "BaseClass", count);
   
   strcpy(file, rte.DataDir());
   strcat(file, CLASS_CONFIG_NAME);

   WriteDebugString("Beginning decode of \'%s\'\n", file);

   status = ReadClassDB(file, &classData);

   count=GetCount(classData);
   WriteDebugString("Num classes in %s = %u\n", file, count);      
   result += ErrMsg(status, "Classes", count);


#ifdef USE_TRAITS
   strcpy(file, rte.DataDir());
   strcat(file, TRAIT_CONFIG_NAME);

   WriteDebugString("Beginning decode of \'%s\'\n", file);

   status = ReadTraitDB(file, &traitData);
   
   count=GetCount(traitData);
   WriteDebugString("Num traits in %s = %u\n", file, count);      
   result += ErrMsg(status, "Traits", count);
#endif


   strcpy(file, rte.DataDir());
   strcat(file, SPELL_CONFIG_NAME);

   WriteDebugString("Beginning decode of \'%s\'\n", file);

   //  Load spells first so we can reference spell names 
   //  in items database and monster database   
   status = ReadSpellDB(file, &spellData);
   count =spellData.GetCount();
   WriteDebugString("Num spells in %s = %u\n", file, count);      
   result += ErrMsg(status, "Spells", count);

   strcpy(file, rte.DataDir());
   strcat(file, ITEM_CONFIG_NAME);

   WriteDebugString("Beginning decode of \'%s\'\n", file);

   // load items next so the monsters can reference item names
   status = ReadItemDB(file, &itemData);
   
   count=itemData.GetCount();
   WriteDebugString("Num items in %s = %u\n", file, count);      
   result += ErrMsg(status, "Items", count);

   strcpy(file, rte.DataDir());
   strcat(file, MONSTER_CONFIG_NAME);

   WriteDebugString("Beginning decode of \'%s\'\n", file);

   status = ReadMonsterDB(file, &monsterData);
   count=monsterData.GetCount();
   WriteDebugString("Num monsters in %s = %u\n", file, count);    
   result += ErrMsg(status, "Monsters", count);

   strcpy(file, rte.DataDir());
   strcat(file, SPECAB_CONFIG_NAME);

   WriteDebugString("Beginning decode of \'%s\'\n", file);

   status = ReadSpecialAbilitiesDB(file, &specialAbilitiesData);
   
   count=specialAbilitiesData.GetCount();
   WriteDebugString("Num Special Abilities in %s = %u\n", file, count);      
   result += ErrMsg(status, "Special Abilities", count);


   CopyEventData.Clear();
   CopyEventData.SetEventSource(CopyEventSrc);

   return result;
}

CUAFWinEdDoc *CUAFWinEdApp::GetDocPtr(void)
{
   // if already have it, don't look again
   if (m_pDoc != NULL)
     return m_pDoc;

   BOOL found = FALSE;
   CUAFWinEdDoc *pDoc = NULL;
   POSITION posTemplate = GetFirstDocTemplatePosition();  
   
   if (posTemplate != NULL)
   {
      CDocTemplate* pTemplate = GetNextDocTemplate(posTemplate);
      
      if (pTemplate != NULL)
      {
         POSITION posDoc = pTemplate->GetFirstDocPosition();
         
         if (posDoc != NULL)
         {
            pDoc = (CUAFWinEdDoc*)pTemplate->GetNextDoc(posDoc);
            
            if (pDoc == NULL)
            {
               WriteDebugString("NULL ptr to document returned by GetNextDoc in CUAFWinEd::InitInstance\n");
               return NULL;
            }
            
            if (!pDoc->IsKindOf( RUNTIME_CLASS(CUAFWinEdDoc) ))
            {
               WriteDebugString("Ptr to incorrect document type returned by GetNextDoc in CUAFWinEd::InitInstance\n");
               return NULL;
            }
            else
               found = TRUE;
         }
         else {
            WriteDebugString("NULL doc position returned by GetFirstDocPosition in CUAFWinEd::InitInstance\n");
            return NULL;
         }
      }
      else {
         WriteDebugString("NULL ptr to doc template position returned by GetNextDocTemplate in CUAFWinEd::InitInstance\n");
         return NULL;
      }
   }
   else {
      WriteDebugString("NULL ptr to doc template returned by GetFirstDocTemplatePosition in CUAFWinEd::InitInstance\n");
      return NULL;
   }
   
  //int flags = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
  //flags |= (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF);
  //_CrtSetDbgFlag(flags);

   if (found)
      return pDoc;
   else
      return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
  CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_Version;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  //{{AFX_MSG(CAboutDlg)
    // No message handlers
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_Version = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_VersionText, m_Version);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
  //{{AFX_MSG_MAP(CAboutDlg)
    // No message handlers
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CUAFWinEdApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
  aboutDlg.m_Version.Format("Version %7.7f",PRODUCT_VER);
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdApp message handlers

int CUAFWinEdApp::ExitInstance() 
{  
  CopyEventData.Clear();

  EditorOptionsData.m_LastPassWalls=PassThruWalls;
  EditorOptionsData.m_LastDoubleFillWall=DoubleFillWallSlot;
  EditorOptionsData.m_LastQuadFileBg=QuadFillBGSlot;
  EditorOptionsData.m_LastKwikKlik=m_KwikKlik;
  EditorOptionsData.m_LastBlockType=m_BLOCK_Type;
  EditorOptionsData.m_LastTileMap=m_TileMap;
  EditorOptionsData.m_LastBkgnd=currBkGnd;
  EditorOptionsData.m_LastWall=currWallSlot;
  EditorOptionsData.m_LastFacing=currFacing;
  EditorOptionsData.m_LastMode=m_mode;
  EditorOptionsData.m_LastAreaViewMode=globalData.useAreaView;
  EditorOptionsData.m_LastDesignX = currX;
  EditorOptionsData.m_LastDesignY = currY;
  EditorOptionsData.Serialize(TRUE); // store config data

   m_pDoc = NULL;
   UndoData.RemoveAll();   
   StopAllSounds();
   ReleaseAllSounds();

   if (pSndMgr != NULL)
   {
      delete pSndMgr;
      pSndMgr = NULL;
   }

  levelData.Clear();
  GraphicsMgr.Clear();

  if (pWallSlotMgr!=NULL)
    delete pWallSlotMgr;
  if (pBgSlotMgr!=NULL)
    delete pBgSlotMgr;
  pWallSlotMgr=NULL;
  pBgSlotMgr=NULL;
  

  for (int i=0;i<MAX_COLOR_SLOTS;i++)     
    ::DeleteObject(m_ColorBrushes[i]);

  //Closedown the OLE subsystem
  ::CoUninitialize();

  CloseErrorLog();
  if (m_hAppEvent!=NULL)
    CloseHandle(m_hAppEvent);   
   
  return CWinApp::ExitInstance();
}

void CUAFWinEdApp::OnAppExit() 
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
          CLEAR_MODIFIED;
      }
    }
  }
  */
  if (!CheckSaveChangedDesign("Do you want to discard the changes?")) return;

 ((CMainFrame*)m_pMainWnd)->SaveState();
 CWinApp::OnAppExit(); 
}


void FillControl(CColorBox *pWnd, COLORREF fillColor)
{
  pWnd->SetColor(fillColor);
}

void CUAFWinEdApp::StatusMsg(CString text)
{
  CMainFrame* pFrame = (CMainFrame *)m_pMainWnd;

   if (pFrame)
   {
     CStatusBar* pStatus = &pFrame->m_wndStatusBar;
     if (pStatus->m_hWnd != NULL)
       pStatus->SetPaneText(0, text);
   }
}

void getBaseName(const char *full, char *base, int baseLen, BOOL LeaveExt)
{
   if ((full == NULL) || (base == NULL) || (baseLen == 0))
   {
      WriteDebugString("Bogus data in getBaseName()\n");
      return;
   }

   base[0] = '\0';

   CString temp;
   CString result;
   temp = full;
   temp.Remove('\r');
   temp.Remove('\n');
   temp.Remove('\t');
   temp.Replace("&", "&&");

   int index = temp.ReverseFind('\\');

   if (index > 3)
      result = temp.Right(temp.GetLength()-(index+1));
   else
      result = temp;

   if (!LeaveExt)
   {
     // remove file extension too
     index = result.ReverseFind('.');

     if (index > 0)
        result = result.Left(index);
   }
 
   strncpy(base, result, baseLen);

   if (strlen(base) <= 0)
      strcpy(base, "Nothing");

   base[baseLen] = '\0';
}

void fillDefaultArtSlots(LEVEL &data, BOOL overland)
{
  CString temp;

  // initialize default art slots
  // Leave slot 0 blank!
  // 

  if (MaxDefaultArtSlots >= MAX_WALLSETS)
    MaxDefaultArtSlots = MAX_WALLSETS-1;

  int i;
  for (i=1;i<=MaxDefaultArtSlots; i++)
  {
    data.m_WallSets[i].Clear();

    if (!overland)
    {
      temp.Format("wa_Wall%u.png", i);
      if (FindImageWithValidExt(temp, ede.TemplateWallArtDir()))
        data.m_WallSets[i].wallFile = temp;

      temp.Format("dr_Door%u.png", i);
      if (FindImageWithValidExt(temp,ede.TemplateDoorArtDir()))
        data.m_WallSets[i].doorFile = temp;

      temp.Format("ov_Overlay%u.png", i);
      if (FindImageWithValidExt(temp,ede.TemplateOverlayArtDir()))
        data.m_WallSets[i].overlayFile = temp;
    }
  }

  if (MaxDefaultArtSlots >= MAX_BACKGROUNDS)
    MaxDefaultArtSlots = MAX_BACKGROUNDS-1;

  for (i=1;i<MaxDefaultArtSlots; i++)
  {
    if (!overland)
    {
      data.m_BackgroundSets[i].Clear();

      temp.Format("bd_Background%u.png", i);
      if (FindImageWithValidExt(temp, ede.TemplateBackgroundArtDir()))
        data.m_BackgroundSets[i].backgroundFile = temp;
    }
    else 
    {
      if (i==1)
      {
        temp = DEFAULT_OVERLAND;
        if (FindImageWithValidExt(temp,ede.TemplateOverlandArtDir()))
          data.m_BackgroundSets[i].backgroundFile = temp;
      }
      else
        data.m_BackgroundSets[i].Clear();
    }
  }
}

long int FindFirstLoadedBackground()
{
  for (int b=1;b<MAX_BACKGROUNDS; b++)
  {
    if (BackgroundSets[b].backgroundSurface > 0)
      return b;
  }
  return 0;
}

long int FindFirstLoadedWall()
{
  for (int w=1;w<MAX_WALLSETS; w++)
  {
    if (WallSets[w].wallSurface > 0)
      return w;
  }
  return 0;
}

void fillDefaultLevelData(LEVEL &data,int level, int width, int height, BOOL overland, BOOL Fill)
{
  if (!overland)
  {
    if ((width < 10)||(width>MAX_AREA_WIDTH)) width=50;
    if ((height < 10)||(height>MAX_AREA_HEIGHT)) height=50;
  }
  else
  {
    Fill = FALSE;
  }

  SET_MODIFIED;

  globalData.levelInfo.stats[level].area_height = height;
  globalData.levelInfo.stats[level].area_width = width;
  globalData.levelInfo.stats[level].used = TRUE;
  globalData.levelInfo.stats[level].overland = overland;

  data.Clear();
  data.area_height = height;
  data.area_width = width;

  //deleteAllLevelArt(level);

  if ((currX < 0) || (currX >= width))
  {
    currX  = max(0, currX);
    currX  = min(width-1, currX);
  }
  if ((currY < 0) || (currY >= height))
  {
    currY  = max(0, currY);
    currY  = min(height-1, currY);
  }

  //fillDefaultArtSlots(overland);

  if (Fill)
  {
    long surf = FindFirstLoadedBackground();
    if (surf >= 0)
    {
        for (int i=0;i<globalData.levelInfo.stats[level].area_height;i++)
        {
            for (unsigned int j=0;j<globalData.levelInfo.stats[level].area_width;j++)
            {
               data.area[i][j].SetAllBG(surf);
            }
        }
    }
  }

  globalData.levelInfo.numLevels = 0;

  for (int i=0;i<MAX_LEVELS;i++)
  {
    if (globalData.levelInfo.stats[i].used)
      globalData.levelInfo.numLevels++;
  }
}

void CUAFWinEdApp::HELP(int helpID)
{
	CWnd* pMainWnd = AfxGetMainWnd();
	ASSERT_VALID(pMainWnd);

//	pMainWnd->PostMessage(WM_KICKIDLE); // trigger idle update

  CString keyword ;
  keyword.LoadString(helpID);

  ::WinHelp(pMainWnd->m_hWnd,
#ifdef SIMPLE_STRUCTURE
            ede.EditorHelpDir() + "UAForever.hlp",
#else
            CString(m_installDir)+"help\\UAForever.hlp",
#endif
            HELP_KEY,
            (unsigned long)((LPCSTR)keyword));
}

void CUAFWinEdApp::OnAPPHelpWallEdit() 
{
	// TODO: Add your command handler code here
  HELP(IDS_HELP_WallEditor);

}


void CUAFWinEdApp::WinHelp(DWORD dwData, UINT nCmd) 
{
	// TODO: Add your specialized code here and/or call the base class
  switch (dwData)
  {
  case HID_BASE_RESOURCE + IDD_DBUTILS:
    HELP(IDS_HELP_DBUtils);
    break;
  case HID_BASE_RESOURCE + IDD_LOGICBLOCK:
    HELP(IDS_HELP_LogicBlock);
    break;
  case HID_BASE_RESOURCE + IDD_MAPSTYLE:
    HELP(IDS_HELP_MapStyle);
    break;
  };
}

void CUAFWinEdApp::SetTitle()
{
  CString AppTitle;
  CString currDesign;
  
#ifdef SIMPLE_STRUCTURE
  currDesign = rte.DesignDir();
  if (currDesign.GetLength() > 40)
  {
    currDesign = currDesign.Left(18) + " ...." + currDesign.Right(18);
  }
#else
  currDesign = (GetDesignFolder());
#endif

  if (!currDesign.IsEmpty())
  {
    AppTitle.Format("Dungeon Craft Design Editor - ver %01.7f: %s%s", 
                        PRODUCT_VER, currDesign, globalData.designName);
    if (IS_MODIFIED) AppTitle += '*';
  }
  else
    AppTitle.Format("Dungeon Craft Design Editor - ver %01.7f: <no folder>\\%s", PRODUCT_VER, globalData.designName);  

  m_pMainWnd->SetWindowText(AppTitle);
}

BOOL CUAFWinEdApp::OnIdle(LONG lCount) 
{
  if (PrevSaveState != IS_MODIFIED)
  {
    SetTitle();
    PrevSaveState = IS_MODIFIED;    
  }
  //if (lCount==1)
  //  flushOldImages();
	return CWinApp::OnIdle(lCount);
}

void SetModified(void)
{
  MODIFIED_FLAG = TRUE;
}
void ClearModified(void)
{
  MODIFIED_FLAG = FALSE;
}
BOOL Modified(void)
{
  return MODIFIED_FLAG;
}


static int mallocSequence=0;
void *My_malloc(int size)
{
  int *result;
  result = (int *)malloc(size + sizeof (int));
  if (result == NULL)
  {
    exit(1);
  };
  *result = ++mallocSequence;
  return (void *)(result + 1);
}

void My_free(void *addr)
{
  free((int *)addr - 1);
}

void *My_realloc(void *addr, int size)
{
  int *result;
  if (addr == NULL) return My_malloc(size);
  result = (int *)realloc((int *)addr-1, size + sizeof (int));
  if (result == NULL)
  {
    exit(1);
  };
  *result = ++mallocSequence;
  return (void *)(result + 1);
}
