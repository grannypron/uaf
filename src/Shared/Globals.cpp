/******************************************************************************
* Filename: Globals.cpp
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
#include "..\shared\stdafx.h"
//#include "externs.h"

#pragma warning(disable:4091)

#ifdef UAFEDITOR
// editor specific includes
#include "..\UAFWinEd\UAFWinEd.h"
#include "..\UAFWinEd\GlobText.h"
#include "..\UAFWinEd\PreviewFileDlg.h"
#include <shlobj.h>
#include "io.h"
#include "..\UAFWinEd\Resource.h"
//#include "ASL.h"
#else
// engine specific includes
#include "..\UAFWin\Dungeon.h"
#include "..\UAFWin\FormattedText.h"
#include "..\UAFWin\GameMenu.h"
#include "..\UAFWin\Combatants.h"
#include "..\UAFWin\GetInput.h"
#include "..\UAFWin\CProcInp.h"
#include "..\UAFWin\Resource.h"
#endif

// shared folder includes
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/timeb.h>
#include <direct.h>
#include <math.h>
#include "PicData.h"
#include "Graphics.h"
#include "GlobalData.h"
#include "Level.h"
#include "GameEvent.h"
#include "FileParse.h"
#include "Viewport.h"
#include "SoundMgr.h"
#include "party.h"
#include "StackWalker.h"
//#include "..\UAFWinEd\Resource.h"
//#include "..\UAFWinEd\ImportWarning.h"
#include "..\UAFWinEd\ImportWarning.h"


extern int DEFAULT_COMBAT_STAT_X;
extern int DEFAULT_COMBAT_STAT_Y;

extern int setPartyXY_x;
extern CString errorText;
extern BOOL EngineRunning;
extern A_CStringPAIR_L ConfigFile_DB;

extern CDXLog g_cdxlog;

#ifdef TRACE_TIMER_DETAILS
class DETAIL_TRACE
{
  FILE *f;
public:
  DETAIL_TRACE(void);
  ~DETAIL_TRACE(void);
//  DWORD    m_id[1000];
//  DWORD    m_data[1000];
//  DWORD    m_time[1000];
//  DWORD    m_virtualTime[1000];
//  int      m_nextIndex;
  void     trace (DWORD id, DWORD data);
};
extern DETAIL_TRACE detailTrace;
#endif


extern const int CHARACTER_VERSION = 0x80000001;  // Not to be confused with index values
extern const double VersionSpellNames = 0.998101;
extern const double VersionSpellIDs =  0.998100;

/*
 * VersionSaveIds adds the old key' values to the serialized 
 * data.  This is handy when loading designs created before
 * VersionSpellNames.  What happens is that the old monster,
 * item, and spell data were converted to use the new 'NAME_ID'
 * but the level data did not get loaded and saved at the 
 * same time.  So the level data continued to have the old
 * key values unless the designer remembered to load and
 * save each level the first time he used an editor of
 * VersioSpellNames or later.  If we save the old key values,
 * then we can link them properly to the old level data.
 * The 'Summoned Monster' in the zone data was the first
 * example of this need (perhaps the only?).
 *
 * So if loading a design < VersionSpellNames we
 *     move the old key values to the 'preSpellNameKey'
 * So if loading a design >= VersionSaveIds we
 *     simply copy the preSpellNameKey.
 * So if we load any other design, we set preSpellNameKey to -1.
 *
 * Starting with 0.998915, the editor offers to backup the
 * old levels and update them to get rid of the 'key' references.
 * This is much cleaner and robust because then the levels
 * contain completely current references.
 */
extern const double VersionSaveIDs = 0.998914;

extern const double PRODUCT_VER = 5.27; //09OCt2020//2OCt2015//22July2015
//extern const double ENGINE_VER = 1.0304;  //13October2014
extern const double ENGINE_VER = 5.27;  //27Jan2016


extern unsigned int gamedatSignature;
//extern const double ENGINE_VER = 0.0000001;


// The current editor and engine versions
//
// Editor version



#ifdef TRACE_TIMER_DETAILS
#define DETAILTRACE(a,b) detailTrace.trace((a),(b));
#else
#define DETAILTRACE(a,b)
#endif





#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef UAFEngine
A_CStringPAIR_L globalSA_debug;
#endif

void gpdlCleanup(void);

CUAFCommandLineInfo cmdLine;

CString ArchiveBlank = "*";//Replacement for empty strings in CArchive file.
//CString ArchiveBlank = "";

//extern CString configMsg;
CString CommonCharSave = "";
CString UAInstallPath = "";
CString cmdPlayfileName;

int net = FALSE;
int ForceSysMemUsage = 1;
int INTER_CHARACTER_DELAY = 15; // delay, in millisecs, between output characters in event text
BOOL UseCustomCursor = FALSE;
BOOL DisableScreenSaver = TRUE;
DWORD SoundPlaybackRate = 22050;

LONGLONG virtualGameTime = 0; // Milliseconds since start-of-play (this session)

// starting x,y where text messages will be written to
int textboxX=18;
int textboxY=328;
int dsgnVerX=-1;
int dsgnVerY=-1;

int BgMaskSurf = -1;
CString BgMask = "bgmask.pcx";

//extern "C" ActorType NULL_ACTOR = { EA_NULL,0,0,0,0,0,InstanceType_Unknown};
const ActorType NULL_ACTOR(EA_NULL,InstanceType_Unknown); //(EA_NULL,0,0,0,0,0,InstanceType_Unknown);
//extern "C" const ActorType INVALID_ACTOR = {EA_NULL,0,0,0,0,0,InstanceType_Invalid};
const ActorType INVALID_ACTOR(EA_NULL,InstanceType_Invalid); // (EA_NULL,0,0,0,0,0,InstanceType_Invalid);

BOOL PlaySoundEffects = TRUE;
BOOL PlayEventSounds = TRUE;
BOOL PlayEffectsDuringEvents = TRUE;

BOOL	bActive = TRUE;   		// is application active?
BOOL  bGameServer = TRUE;
HANDLE hShutdown=NULL;
#ifdef _DEBUG
BOOL LogCombatActions = TRUE;
#else
BOOL LogCombatActions = FALSE;
#endif
BOOL UseErrorLog = TRUE;
int LogDatabaseErrors=1;
int LogCompileErrors=1;
BOOL logDebuggingInfo = FALSE;
int useWallIndex=0;  // Scripts and GPDL functions use wall index rather than ordinal.
int useDoorAndOverlayIndex = 0;// Scripts and GPDL functions use index rather than ordinal.

CCriticalSection ErrorLogCS;
FILE* pErrorLogFile = NULL;

RUNTIME_ENVIRONMENT rte;
CString RUNTIME_ENVIRONMENT::LogDir(void)
{
#ifdef UAFEngine
  if (!EngineRunning) 
  {
    return "";
  };
#endif
  return m_designDir; 
};

#ifdef UAFEDITOR
EDITOR_ENVIRONMENT ede;
#endif

RACE_DATA_TYPE       raceData;
BASE_CLASS_DATA_TYPE baseclassData;
CLASS_DATA_TYPE      classData;
ABILITY_DATA_TYPE    abilityData;

#ifdef USE_TRAITS
TRAIT_DATA_TYPE      traitData;
#endif

SPELLGROUP_DATA_TYPE spellgroupData;

char global_designDir[_MAX_PATH+1];


DWORD prevFileActionTime;


unsigned int CString2Base38(const CString& str)
{
  unsigned int result;
  int i;
  char c;
  int len;
  result = 0;
  len = str.GetLength();
  for (i=0; i<6; i++)
  {
    if (len <= i) c = ' ';
    else c = str.GetAt(i);
    if (c == ' ') c = 1;
    else if ( (c >= '0') && (c <= '9') ) c = c - '0' + 2;
    else if ( (c >= 'A') && (c <= 'Z') ) c = c - 'A' + 12;
    else if ( (c >= 'a') && (c <= 'z') ) c = c - 'a' + 12;
    else c = 0;
    result = result * 38 + c;
  };
  return result;
}

CString CStringFromBase38(unsigned int v)
{
  switch(v)
  {
  case 0: return "WEAPON";
  case 1: return "SHIELD";
  case 2: return "ARMOR";
  case 3: return "HANDS";
  case 4: return "HEAD";
  case 5: return "WAIST";
  case 6: return "ROBE";
  case 7: return "CLOAK";
  case 8: return "FEET";
  case 9: return "FINGER";
  case 10: return "QUIVER";
  case 11: return "CANNOT";
  default:
    {
      int i;
      char result[6];
      for (i=5; i>=0; i--)
      {
        int remainder;
        remainder = v%38;
        v /= 38;
        if (remainder < 1) result[i] = '?';
        else if (remainder < 2) result[i] = ' ';
        else if (remainder < 12) result[i] = '0' + remainder - 2;
        else result[i] = remainder - 12 + 'A';
      };
      return CString(result,6);
    };
  };
}



BOOL MyCopyFile(LPCSTR src, LPCSTR dest, BOOL failIfExists)
{
  BOOL result;
  //WriteDebugString("StartCopy %s to %s\n", src, dest);
  result = CopyFile(src, dest, failIfExists);
  //WriteDebugString("EndCopy \n");
  return result;
}

BOOL MyDeleteFile(LPCSTR name)
{
  //WriteDebugString("Deleting %s\n", name);
  return DeleteFile(name);
}


const char *WeaponClassText[] = {
  "Not Weapon",
  "Hand held, blunt",
  "Hand held, cutting",
  "Hand held, thrown",
  "Sling (no ammo needed)",
  "Bow",
  "Crossbow",
  "Thrown",
  "Ammo",
  "Spell Caster",
  "Spell-like Ability"
};




//RUNTIME_ENVIRONMENT rte;

RUNTIME_ENVIRONMENT::RUNTIME_ENVIRONMENT(void)
{
  Clear();
}

void RUNTIME_ENVIRONMENT::Clear(void)
{
  m_executableDir = "";  // Where the program executable is.
  m_rootDir = "";        // A folder containg designs and perhaps an editor folder
  m_designDir = "";      // A folder containing single design; usually "something.dsn"
  m_dataDir = "";        // The folder containint the levels, etc of the dungeon
  m_soundDir = "";       // The sounds folder of the design; usually "designDir\Resources"
  m_configDir = "";      // The folder in which we will find 'config.txt
  m_saveDir = "";        // The folder in which we store party and character data.
  m_artDir = "";         // The art folder of the design; usually "designDir\Resources"
  m_backupDir = "";
  m_programFilesChecked = false;
}

void RUNTIME_ENVIRONMENT::CheckProgramFiles(void)
{
  if (m_programFilesChecked) return;
  CString temp;
  temp = m_rootDir;
  temp.MakeUpper();
  if (temp.Find("PROGRAM FILES",0) < 0) return;
  m_programFilesChecked = true;
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
      if (dlg.DoModal() != IDOK)
      {
        exit(1);
      };
      if ( (sscanf(dlg.m_numeric,"%d",&answer) != 1) || (answer!=dlg.m_i))
      {
          MsgBoxInfo("Wrong answer....sorry");
          correctResponse = false;
      };
    } while (!correctResponse);
  };
}

void RUNTIME_ENVIRONMENT::DefaultFoldersFromDesign(const CString& design)
{
  int i;
  m_designDir     = design;
  if (m_designDir.GetAt(m_designDir.GetLength()-1) != '\\')
  {
    m_designDir += '\\';
  };
  m_executableDir = m_designDir;
  m_dataDir       = m_designDir + "Data\\";
  m_configDir     = m_dataDir;
  m_saveDir       = m_designDir + "Saves\\";
  m_artDir        = m_designDir + "Resources\\";
  m_soundDir      = m_designDir + "Resources\\";
  m_rootDir       = m_designDir.Left(m_designDir.GetLength()-1);
  i = m_rootDir.ReverseFind('\\');
  if (i >= 2)
  {
    m_rootDir = m_rootDir.Left(i+1);
  };
  CheckProgramFiles();
}



CString RUNTIME_ENVIRONMENT::Dir(SurfaceType sType)
{
  switch (sType)
  {
  case BogusDib:
  case CommonDib:
  case CombatDib:
  case WallDib:
  case DoorDib:
  case BackGndDib:
  case OverlayDib:
  case IconDib:
  case OutdoorCombatDib:
  case BigPicDib:
  case MapDib:
  case SmallPicDib:
  case SpriteDib:
  case TitleDib: 
  case BufferDib:
  case FontDib: 
  case MouseDib:
    break;
  };
  return RootDir();
}
/*
char global_executableDir[_MAX_PATH+1];// Folder containing the editor executable.
char global_rootDir[_MAX_PATH+1];   // Folder containing designs "xxx.dsn" and perhaps a editor folder
char global_dataDir[_MAX_PATH+1];   // Folder within a design
//char global_dungeonDir[_MAX_PATH+1];// Folder containing levels, config, etc.
char global_soundDir[_MAX_PATH+1];  // Folder within a design
char global_configDir[_MAX_PATH+1]; // Folder within a design
char global_artDir[_MAX_PATH+1];    // Folder within a design
char global_saveDir[_MAX_PATH+1];   // Folder within a design
*/
#ifdef UAFEDITOR
//EDITOR_ENVIRONMENT ede;



void EDITOR_ENVIRONMENT::DefaultFoldersFromExecutable(const CString &editorExecutableDir)
{
  m_editorExecutableDir = editorExecutableDir;
  m_editorResourceDir = m_editorExecutableDir + "EditorResources\\";
  m_templateDir = m_editorExecutableDir + "TemplateDesign.dsn\\";
  m_templateDataDir = m_templateDir + "Data\\";
  m_templateSoundDir = m_templateDir + "Resources\\";
  m_templateArtDir = m_templateSoundDir;
  m_editorHelpDir = m_editorResourceDir;
}





/*
char global_editorResourceDir[_MAX_PATH+1]; // Folder containing art, etc. available to the editor.
*/
#endif


void NotImplemented(int code, bool loopForever)

{
  static CMap<int,int,int,int> list;
  int junk;
  if (list.Lookup(code,junk)) return;
  list.SetAt(code,0);
  {
    CString acode;
    acode.Format("Un-Implemented code\n\nError code = %08X", code);
    do
    {
      MsgBoxInfo(acode);
    } while (loopForever); 
  };
}


extern const int MIN_INTELLIGENCE  =3;
extern const int MAX_INTELLIGENCE  =25;
extern const int MIN_STRENGTH      =3;
extern const int MAX_STRENGTH      =25;
extern const int MIN_STRENGTHMOD   =0;
extern const int MAX_STRENGTHMOD   =100;
extern const int MIN_WISDOM        =3;
extern const int MAX_WISDOM        =25;
extern const int MIN_DEXTERITY     =3;
extern const int MAX_DEXTERITY     =25;
extern const int MIN_CONSTITUTION  =3;
extern const int MAX_CONSTITUTION  =25;
extern const int MIN_CHARISMA      =3;
extern const int MAX_CHARISMA      =25;


int CombatScreenX = 14;
int CombatScreenY = 14;
int TILES_HORZ = 8;
int TILES_VERT = 9;



int SCREEN_WIDTH = 640;  // these change to reflect the current
int SCREEN_HEIGHT = 480; // screen resolution for the editor
int COLOR_BITS = 24;
screenRes m_GameScreenRes = res640x480;

COLORREF BACKGROUND_FILL        = RGB(0,0,0);
COLORREF BACKWARD_BACKGROUND_FILL = RGB(0,0,0); // SOme functions want BGR values????
COLORREF BACKGROUND_COMBAT_FILL = RGB(0,0,0);

int currBgFrame = 1;
int currBgSlot = 1;
int lastBgUpdate = 0;

/*
 * Font colors are defined in globalData.availableFonts
 * Font 0 is the traditional font that used to be described by
 * the colors White, Black, Green, etc.
 * You will now find these COLORREF values in availableFonts[0].

COLORREF White  = RGB(255,255,255);
COLORREF Black  = RGB(0,0,0);

// walk text colors
COLORREF Green  = RGB(0,255,0);
COLORREF Red    = RGB(255,0,0);
COLORREF Yellow = RGB(255,255,0);
COLORREF Orange = RGB(255,128,0);
COLORREF Blue   = RGB(0,151,255);

// combat text colors
COLORREF CombatGreen =  RGB(0,254,0);
COLORREF CombatRed   =  RGB(254,0,0);
COLORREF CombatYellow = RGB(254,255,0);
COLORREF CombatBlue   = RGB(0,150,255);
COLORREF CombatOrange = RGB(254,127,0);

*/

// custom colors from the config file
COLORREF CustomColor =  RGB(255,255,255);

// used to highlight character of menu label for keyboard input
COLORREF KeyboardCharColor = RGB(251,133,15);
BOOL UseKeyboardShortcuts = TRUE;

// used by editor to display map
CString MAPART = "MapArt.png";

// used by editor to display overland map
CString OVERLANDART = "OverArt.png";

// used by engine to display area view of map
CString AREAVIEWART = "AreaViewArt.png";

//    deltaX[currFacing] = increment to x when we move forward one square.
//    deltaY[currFacing] = increment to y when we move forward one square.
int deltaY[4] = {-1,0,1,0};
int deltaX[4] = {0,1,0,-1};


// Used at create Character to allow player modification of character stats
// Someday we will move this to globalData and set with the editor.
bool AllowModifyStats=true;

double GetRunAsVersion(int flag)
{
  CString key("RunAsVersion");
  double val=globalData.version;

  // see version constants in GlobalData.h file
  switch (flag)
  {
  case GuidedTourVersion:        key = "GuidedTourVersion"; break;
  case ItemUseEventVersion:      key = "ItemUseEventVersion"; break;
  case SpecialItemKeyQtyVersion: key = "SpecialItemKeyQtyVersion"; break;
  case RunAsVersion: // intentional fall-through
  default: break;
  }

  CString txt = globalData.global_asl.Lookup(key);
  if (!txt.IsEmpty()) val = atof(txt);
  return val;
}

int GetUserGameSpeed()
{
  return party.speed;
}

int GetMsgTimeDelay()
{
  return (GetUserGameSpeed()*333); // for default speed=3, gives 1 sec delay
}

int GetCurrSkillLevel()
{
  return party.skillLevel;
}
int GetAllExpMod()
{
  int lvl = GetCurrSkillLevel();
  if ((lvl < 0) || (lvl > 4)) return 0;
  if (globalData.m_diffLvlData.m_data[lvl].m_ModifyAllExp)
    return globalData.m_diffLvlData.m_data[lvl].m_AExpAmount;
  else
    return 0;
}
int GetMonsterExpMod()
{
  int lvl = GetCurrSkillLevel();
  if ((lvl < 0) || (lvl > 4)) return 0;
  if (globalData.m_diffLvlData.m_data[lvl].m_ModifyMonsterExp)
    return globalData.m_diffLvlData.m_data[lvl].m_MExpAmount;
  else
    return 0;
}
int GetMonsterQtyMod()
{
  int lvl = GetCurrSkillLevel();
  if ((lvl < 0) || (lvl > 4)) return 0;
  if (globalData.m_diffLvlData.m_data[lvl].m_ModifyQty)
    return globalData.m_diffLvlData.m_data[lvl].m_QtyAmount;
  else
    return 0;
}
int GetMonsterHDMod()
{
  int lvl = GetCurrSkillLevel();
  if ((lvl < 0) || (lvl > 4)) return 0;
  if (globalData.m_diffLvlData.m_data[lvl].m_ModifyHD)
    return globalData.m_diffLvlData.m_data[lvl].m_HDAmount;
  else
    return 0;
}


void CreateShutdownEvent()
{
  CloseShutdownEvent();
  hShutdown = CreateEvent(NULL, TRUE, FALSE, "UAFSHUTDOWNEVENT121963");
}

BOOL IsShutdownSignaled()
{
  if (hShutdown != NULL)
    return (WaitForSingleObject(hShutdown,0) == WAIT_OBJECT_0);

  return FALSE;
}

void SignalShutdown()
{
  if (hShutdown != NULL)
    SetEvent(hShutdown);
#ifdef UAFEngine
  ExitSignaled=TRUE;
  //gpdlCleanup();
#endif
}

void CloseShutdownEvent()
{
  if (hShutdown != NULL)
    CloseHandle(hShutdown);
  hShutdown = NULL;
}

CString GetFilenameDrive(CString &filename)
{
  CString tmp("");
  int dot = filename.Find(':');
  if (dot != -1)
  {
    dot++;
    tmp = filename.Left(dot);
  }
  return tmp;
}
void GetFilenameExt(CString &filename, CString &ext)
{
  int dot = filename.ReverseFind('.');
  if (dot != -1)
    ext = filename.Right(filename.GetLength()-dot-1);
  else
    ext = "";
}

BOOL GetFilenamePath(CString &filename, CString &path)
{
  path="";
  if (filename == "")
    return FALSE;

  int index = filename.ReverseFind('\\');
  if (index > 3)
  {
     path = filename.Left(index);
     return TRUE;
  }
  return FALSE;
}

BOOL StripOneFolderFromFilenamePath(const CString &filename, CString &path)
{
  if (filename.IsEmpty())
    return FALSE;

  path = filename;
 
  // the last char in source string might be a '\'
  int index = path.ReverseFind('\\');
  
  if ((index > 2) && ((index+1) == path.GetLength()))
  {
    path = path.Left(index-1);
    index = path.ReverseFind('\\');
  }

  if (index >= 2)
  {
    // leave the final '\'
    path = filename.Left(index+1);
    return TRUE;
  }

  path="";
  return FALSE;
}

void StripFilenamePath(CString& filename)
{
  if (filename == "")
    return;

  int len;
  len = filename.GetLength();
  //CString saveName;
  // if filaname ends with '\', strip it
  if ((len > 3) && (filename[len-1] == '\\'))
  {
    filename = filename.Left(len-1);
    return;
  };
  if (filename.Find('\\')<0) return;
  int index = filename.ReverseFind('\\');
  if (index >= 2)
  {
    filename = filename.Right(len-(index+1));
    return;
  };  
  return;
}

/*
void AddArtFolderToPath(CString &filename)
{
  if (filename == "")
    return;
  StripFilenamePath(filename);
  CString temp;
  temp.Format("%s%s", global_artDir, filename);
  filename = temp;
}


void AddSoundFolderToPath(CString &filename)
{
  if (filename == "")
    return;
  StripFilenamePath(filename);
  CString temp;
  temp.Format("%s%s", global_soundDir, filename);
  filename = temp;
}
*/
void AddFolderToPath(CString &filename, const CString& dir)
{
  if (filename == "")
    return;
  StripFilenamePath(filename);
  filename = dir + filename;
}


void CUAFCommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast )
{
  if (bFlag)
  {
    char FlagKey[50];
    char FlagValue[256];

    // split cmd line parameter into flag and value
    // lpszParam should be in format of '-flag value'
    // bFlag will be TRUE if cmd line text preceeded by '-' or '/'
    // the '-' or '/' is stripped off by MFC before getting here
    char *ptr = (char*)strchr(lpszParam, ' ');
    if (ptr != NULL)
    {
      int len = min(ptr-lpszParam, 49);
      strncpy(FlagKey, lpszParam, len);
      ptr++;
      strncpy(FlagValue, ptr, 255);
      FlagKey[len]='\0';
      FlagValue[255]='\0';
    }
    else // only flag is present, no value
    {      
      strncpy(FlagKey, lpszParam, 49);
      FlagKey[49]='\0';
      FlagValue[0]='\0';
    }

    // Now figure out what we've got
    if (stricmp(FlagKey, "config") == 0)
    {
      // load a specific config.txt file
      // format: -config c:\temp\config.txt
      m_ConfigFilename = FlagValue;
    }

    if (stricmp(FlagKey, "script") == 0)
    {
      m_ScriptFilename = FlagValue;
    };

    if (stricmp(FlagKey, "pc") == 0)
    {
      // load a player character into the party
      // 
      // only useful if warping to a location
      //
      // format: -pc CharacterName1 -pc CharacterName2
      CString temp(FlagValue);
      m_Party.AddTail(temp);
    }
    
    if (stricmp(FlagKey, "playfile") == 0)
    {
      // format: -playfile filename
      cmdPlayfileName = FlagValue;
    }
    
    if (stricmp(FlagKey, "initial") == 0)
    {
      m_initializeGame = TRUE;
    };
    
#ifdef _DEBUG 
    // debug only flag
    if (stricmp(FlagKey, "mapon") == 0)
    {
      // allow area view regardless of design settings
      // format: -mapon
      m_forceAV = TRUE;
    }
#endif
    if (stricmp(FlagKey, "windowed") == 0)
    {
      RunFullscreen = FALSE;
    };

    if (stricmp(FlagKey, "warp") == 0)
    {
      // warp to level at x,y
      // format: -warp x,y,level,facing

      char tmp[10];
      memset(tmp, 0, sizeof(tmp));
      char *sp = FlagValue;
      // find first comma
      ptr = strchr(sp, ',');
      if (ptr != NULL)
      {        
        strncpy(tmp, sp, ptr-sp);
        tmp[ptr-sp] = '\0';
        m_sx = atoi(tmp);
        
        ptr++;
        sp = ptr;

        // find second comma
        ptr = strchr(sp, ',');
        if (ptr != NULL)
        {        
          strncpy(tmp, sp, ptr-sp);
          tmp[ptr-sp] = '\0';
          m_sy = atoi(tmp);

          ptr++;
          sp = ptr;

          // find third comma
          ptr = strchr(sp, ',');
          if (ptr != NULL)
          {        
            strncpy(tmp, sp, ptr-sp);
            tmp[ptr-sp] = '\0';
            m_level = atoi(tmp);

            // put whats left into facing
            ptr++;
            if ((ptr != NULL) && (*ptr != '\0'))
              m_facing = atoi(ptr);
          }
        }
      }      

      if ((m_sx == -1) || (m_sy == -1) || (m_level == -1) || (m_facing == -1))
      {
        m_sx=-1; m_sy=-1;m_level=-1;m_facing=-1;
      }
    }
  }

  CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
}


// This table is used to name the schiools of magic - PRS Dec 2010
char *ClassFlagText[8] = {
  "Magic User",
  "Cleric",
  "Thief",
  "Fighter",
  "Paladin",
  "Ranger",
  "Druid",
  "Any"
};

DWORD ApplyCostFactor(costFactorType type, DWORD price)
{
  if (type == Free) return 0;

  double orig = (double)price;
  double temp = orig;

  switch (type)
  {
  case Div100:
    temp = orig / 100;
    break;
  case Div50:
    temp = orig / 50;
    break;
  case Div20:
    temp = orig / 20;
    break;
  case Div10:
    temp = orig / 10;
    break;
  case Div5:
    temp = orig / 5;
    break;
  case Div4:
    temp = orig / 4;
    break;
  case Div3:
    temp = orig / 3;
    break;
  case Div2:
    temp = orig / 2;
    break;
  case Div1_5:
    temp = orig / 1.5;
    break;
  case Normal:
    temp = (double)price;
    break;
  case Mult1_5:
    temp = orig * 1.5;
    break;
  case Mult2:
    temp = orig * 2;
    break;
  case Mult3:
    temp = orig * 3;
    break;
  case Mult4:
    temp = orig * 4;
    break;
  case Mult5:
    temp = orig * 5;
    break;
  case Mult10:
    temp = orig * 10;
    break;
  case Mult20:
    temp = orig * 20;
    break;
  case Mult50:
    temp = orig * 50;
    break;
  case Mult100:
    temp = orig * 100;
    break;
  }

  DWORD cost = (DWORD)temp;
  cost = max(1, cost);
  return cost;
}
/*
int SpellClassToCharClass(WORD SpellClassFlag)
{
  switch (SpellClassFlag)
  {
    case MagicUserFlag:      
      return MagicUser;
    case ClericFlag:
      return Cleric;
    case ThiefFlag:
      return Thief;
    case FighterFlag:
      return Fighter;
    case PaladinFlag:
      return Paladin;
    case RangerFlag:
      return Ranger;
    case DruidFlag:
      return Druid;
  }
  return BogusClass;
}
*/
unsigned int GetSystemTextCharWidth()
{
#ifdef UAFEngine
  return GraphicsMgr.GetMaxCharacterWidth();
#else
  return 0;
#endif
}

void displayMiscError(miscErrorType error)
{
#ifdef UAFEngine
   if ((error > NoError) && (error < LastMiscError))
   {
     if (error == ErrorText)
     {
      FormatDisplayText(errorTextData, errorText, FALSE);
     }
     else
     {
      FormatDisplayText(errorTextData, miscErrorText[error], FALSE);
     };
     //DisplayFormattedText(textData);
     if (!debugStrings.AlreadyNoted(CString("DME01") + miscErrorText[error]))
     {
       WriteDebugString("displayMiscError: %s\n", miscErrorText[error]);
     }
     errorTextData.startTime = virtualGameTime;//GetTickCount();
   }
#endif
}

void EditorStatusMsg(LPCSTR ptext)
{
  if (ptext == NULL)
    return;
#if (defined UAFEDITOR)
  ((CUAFWinEdApp*)AfxGetApp())->StatusMsg(ptext);
#else
  WriteDebugString("EditorStatusMsg: %s\n", ptext);
#endif
}

int GetEntryPtX(int ep) { return globalData.levelInfo.GetEPX(ep); }
int GetEntryPtY(int ep) { return globalData.levelInfo.GetEPY(ep); }

CString GetDesignName()
{
  if (globalData.designName.IsEmpty())
    globalData.designName = "Default Design";
  return globalData.designName;
}

/*
CString GetDataPath(char *path)
{
  CString temp;
  temp = global_dataDir;  
  if (strlen(global_dataDir) == 0)
    temp.Format("%s%s.dsn\\", global_designDir, "Data\\");
  if (temp.IsEmpty()) return temp;
  if (path != NULL) strcpy(path, temp);
  return temp;
}

CString GetArtPath(char *path)
{
  CString temp;
  temp = global_artDir;  
  if (strlen(global_artDir) == 0)
    temp.Format("%s%s.dsn\\", global_designDir, "Art\\");
  if (temp.IsEmpty()) return temp;
  if (path != NULL) strcpy(path, temp);
  return temp;
}
*/

CString GetDesignPath(char *path)
{
  CString temp;
#ifdef UAFEngine
  temp = global_designDir;  
#else
    if (rte.DesignDir() == "")
    temp.Format("%s%s.dsn\\", rte.RootDir(), GetDesignName());
  else
    temp = rte.DesignDir();
#endif
  if (temp.IsEmpty()) return temp;
  if (path != NULL) strcpy(path, temp);
  return temp;
}

CString GetDesignFolder(char *path)
{
  CString temp(GetDesignPath());
  if (temp.IsEmpty()) return temp;
  StripFilenamePath(temp);
  if (temp.IsEmpty()) return temp;
  if (path != NULL) strcpy(path, temp);
  return temp;
}

CString GetDesignFolderRoot(char *path)
{
  CString temp(GetDesignFolder());
  if (temp.IsEmpty()) return temp;

  // for 'a.dsn' strip the '.dsn'
  int index = temp.ReverseFind('.');
  if (index >= 1)
  {
    CString saveName="";
    saveName = temp.Left(index);
    temp = saveName;
  }
  if (temp.IsEmpty()) return temp;
  if (path != NULL) strcpy(path, temp);
  return temp;
}

CString GetSavePath(char *path)
{
  return rte.SaveDir();
}

CString GetCommonCharPath(void)
{
  return CommonCharSave;
}

void SetMiscError(int error)
{
#ifdef UAFEngine
  if ((error >= 0) && (error < MAX_MISC_ERROR_MSGS))
  {
    miscError = (miscErrorType)error;
    if (error != NoError)
    {
      if (!debugStrings.AlreadyNoted(CString("SME01") + miscErrorText[error]))
      {
        WriteDebugString("MISC ERROR: %s\n", miscErrorText[error]);
      };
    };
  }
#endif
}

BOOL CreateDesignDirectory()
{
  BOOL success = FALSE;
  //struct _tstat st;
  char path[_MAX_PATH+1];

  GetDesignPath(path);
  int len = strlen(path);
  if (path[len-1] == '\\')
    path[len-1] = '\0';
  WriteDebugString("Entering CreateDesignDirectory(%s)\n", path);
/*
  if (_tstat(path, &st) == 0)
  {
    if (st.st_mode & _S_IFDIR)
    {
      success = TRUE;
    }
    else 
    {
      MsgBoxError("Cannot create design directory\nFile with same name exists",
                  "Design Save Error");
    }
  }
  else 
  {
    if( _mkdir(path) == 0)
      success = TRUE;
  }
*/
  if (PathFileExists(path))
  {
    if (PathIsDirectory(path))
    {
      success = TRUE;
    }
    else
    {
      WriteDebugString("A non-directory file with that name exists\n");
      MsgBoxError("Cannot create design directory\nFile with same name exists",
        "Design Save Error");
    };
  }
  else
  {
    if (_mkdir(path) == 0)
    {
      success = TRUE;
    }
    else
    {
      WriteDebugString("Cannot create that directory\n");
    };
  };

  return success;
}

BOOL CreateSaveDirectory()
{
  BOOL success = FALSE;
  //struct _tstat st;
  char path[_MAX_PATH+1];
  {
    CString temp;
    temp = rte.SaveDir();
    strcpy(path, temp);
  };
  int len = strlen(path);
  if (path[len-1] == '\\')
    path[len-1] = '\0';
  WriteDebugString("Entering CreateSaveDirectory(%s)\n", path);
/*
  if (_tstat(path, &st) == 0)
  {
    if (st.st_mode & _S_IFDIR)
      success = TRUE;
    else 
      WriteDebugString("Cannot create save directory: %s\n", path);
  }
  else 
  {
    if( _mkdir(path) == 0)
      success = TRUE;
    else
      WriteDebugString("Cannot create save directory: %s\n", path);
  }
*/
  if (PathFileExists(path))
  {
    if (PathIsDirectory(path))
    {
      WriteDebugString("Directory already exists\n");
      success = TRUE;
    }
    else
    {
      WriteDebugString("A non-directory file by that name already exists\n");
    };
  }
  else
  {
    if (_mkdir(path) == 0)
      success = TRUE;
    else
      WriteDebugString("Cannot create save directory: %s\n", path);
  };

  return success;
}

BOOL CreateCommonCharDirectory()
{
  BOOL success = FALSE;
  //struct _tstat st;
  char path[_MAX_PATH+1];

  strcpy(path, GetCommonCharPath());
  int len = strlen(path);
  if (len==0) return FALSE;

  if (path[len-1] == '\\')
    path[len-1] = '\0';
  WriteDebugString("Enter CreateCommonCharDirectory(%s)\n", path);
/*
  if (_tstat(path, &st) == 0)
  {
    if (st.st_mode & _S_IFDIR)
      success = TRUE;
    else 
      WriteDebugString("Cannot create common char save directory: %s\n", path);
  }
  else 
  {
    if( _mkdir(path) == 0)
      success = TRUE;
    else
      WriteDebugString("Cannot create common char save directory: %s\n", path);
  }
*/
  if (PathFileExists(path))
  {
    if (PathIsDirectory(path))
    {
      success = TRUE;
    }
    else
    {
      WriteDebugString("A non-directory file with that name exists\n");
    }
  }
  else
  {
    if (_mkdir(path) == 0)
      success = TRUE;
    else
      WriteDebugString("Cannot create common char save directory: %s\n", path);
  };
  return success;
}

int GetCurrentLevel()
{
  return globalData.currLevel;
}

int GetDesignCharStartExp()
{
  return globalData.startExp;
}
int GetDesignCharStartExpType()
{
  return globalData.startExpType;
}

CString GetSpecialItemName(int i) 
{ 
  if (i <= 0) return "No Item";
  return globalData.specialItemData.GetName(i);
}

CString GetSpecialKeyName(int i) 
{ 
  if (i <= 0) return "No Key";
  return globalData.keyData.GetName(i); 
}

int GetDesignStartPlatinum() { return globalData.startPlatinum; }
int GetDesignStartElectrum() { return START_ELECTRUM; }
int GetDesignStartGold() { return START_GOLD; }
int GetDesignStartSilver() { return START_SILVER; }
int GetDesignStartCopper() { return START_COPPER; }
int GetDesignStartGems() { return globalData.startGem; }
int GetDesignStartJewelry() { return globalData.startJewelry; }

#if (defined UAFEDITOR)

//*****************************************************************************
// NAME:    GetEventDescription
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
CString GetEventDescription(DWORD type) 
{ 
  if (type >= NUM_EVENT_TYPES) type = NoEvent;
  return EventListText[type]; 
}

//*****************************************************************************
// NAME:    GetEventIdDescription
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
CString GetEventIdDescription(DWORD EventId, EventSourceType type, GameEventList *pEventList) 
{ 
  if (type==GlobalEventSrc) return GetGlobalEventIdDescription(EventId);
  if (type==CopyEventSrc) return GetCopyEventIdDescription(EventId);

  CString temp;
  if (pEventList == NULL)
  {
    pEventList = &levelData.eventData;
  };
  GameEvent *event = pEventList->GetEvent(EventId);
  if (event != NULL)
    temp.Format("%s (%u)", 
                GetEventDescription(event->GetEventType()), 
                event->GetEventId());
  else
    temp.Format("%s (%u)", 
                EventListText[NoEvent], 
                EventId);
  return temp;
}

CString GetGlobalEventIdDescription(DWORD EventId) 
{ 
  CString temp;
  GameEvent *event = globalData.eventData.GetEvent(EventId);
  if (event != NULL)
    temp.Format("%s (%u)", 
                GetEventDescription(event->GetEventType()), 
                event->GetEventId());
  else
    temp.Format("%s (%u)", 
                EventListText[NoEvent], 
                EventId);
  return temp;
}

CString GetCopyEventIdDescription(DWORD EventId) 
{ 
  CString temp;
  GameEvent *event = CopyEventData.GetEvent(EventId);
  if (event != NULL)
    temp.Format("%s (%u)", 
                GetEventDescription(event->GetEventType()), 
                event->GetEventId());
  else
    temp.Format("%s (%u)", 
                EventListText[NoEvent], 
                EventId);
  return temp;
}

#endif

BOOL IsFileAlreadyOpen(const char *filename)
{
  if (!FileExists(filename)) return FALSE;

  HFILE theFile = HFILE_ERROR;
  DWORD lastErr  = NO_ERROR;

  // Attempt to open the file exclusively.
  theFile = _lopen(filename, OF_READ | OF_SHARE_EXCLUSIVE);

  if (theFile == HFILE_ERROR)
  {
     // Save last error...
     lastErr = GetLastError();
  }
  else
  {
     // If the open was successful, close the file.
     _lclose(theFile);
  }

  // Return TRUE if there is a sharing-violation.
  return ((theFile == HFILE_ERROR) &&
          (lastErr == ERROR_SHARING_VIOLATION));
} 



//*****************************************************************************
// NAME:    FileExists
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
A_CStringInteger_L *fileExistsList = NULL;
BOOL FileExists(const char *file)
{
  //static struct _tstat st;
  int result;

  if (file == NULL)
  {
    //WriteDebugString("NULL ptr to file in FileExists()\n");
    return FALSE;
  }

  
  if (fileExistsList != NULL)
  {
    int *pResult;
    pResult = fileExistsList->Find(file);
    if (pResult != NULL)
    {
      return *pResult != 0;;
    };
  };
  if (!PathFileExists(file))
  //if (_tstat(file, &st) != 0)
  {
      result = 0;
  }
  else
  {
      result = 1;
  };
  if (fileExistsList != NULL)
  {
    fileExistsList->Insert(file, result);
  };
  return result != 0;
}

#ifdef UAFEDITOR
//*****************************************************************************
// NAME:    FolderIsEmpty
//
// PURPOSE: Determine if a folder contains no files.
//
// RETURNS: TRUE if folder is emtpy
//*****************************************************************************
BOOL FolderIsEmpty(const CString& folder)
{
  long result, handle;
  _finddata_t fdata;
  CString f;
  f = folder;
  if (f.Right(1) != '\\') f += '\\';
  f += "*.*";
  for (handle=result=_findfirst(f, &fdata); result!=-1; result=_findnext(handle,&fdata))
  {
    if (result == -1) break;
    if (strcmp(fdata.name, ".") == 0) continue;
    if (strcmp(fdata.name, "..") == 0) continue;
    _findclose(handle);
    return FALSE; 
  };
  if (handle!=-1) _findclose(handle);
  return TRUE;
}
#endif


//*****************************************************************************
// NAME:    FileSize
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int FileSize(const char *file)
{
   // this function is used by WriteDebugString(),
   // so don't call WriteDebugString() while in here.
   struct _tstat st;
   if ((file == NULL) || (strlen(file) == 0))
     return 0;
   int size = 0;
   if (_tstat(file, &st) == 0)
     size = st.st_size;

   return size;
}

#if (defined UAFEDITOR)


static char LastDir[_MAX_PATH+1]= {'\0'};

//*****************************************************************************
// NAME:    GetFilename
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL GetFilename(CString& defaultFilename, CString Ext, BOOL LoadDlg)
{
  BOOL fileChosen = FALSE;  
  char szFilter[_MAX_PATH+1];
  char ExtText[5];
  
  strncpy(ExtText, Ext, 3);
  ExtText[3] = '\0';
  sprintf(szFilter, "%s Files (*.%s)|*.%s||", ExtText, ExtText, ExtText);
  DWORD flags = (OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST);

  //              True = Read, False = Write dialog
  CFileDialog dlg(LoadDlg, Ext, defaultFilename, flags, szFilter);

  if (strlen(LastDir) > 2)
    dlg.m_ofn.lpstrInitialDir = LastDir;
  else
    dlg.m_ofn.lpstrInitialDir = global_designDir;

  if (dlg.DoModal() == IDOK)
  {
     CString tmp("");
     defaultFilename = dlg.GetPathName();
     if (GetFilenamePath(defaultFilename, tmp)) 
     {
       strncpy(LastDir,tmp,_MAX_PATH);
       LastDir[_MAX_PATH]='\0';
     }
     fileChosen = TRUE;
  }
  SetCurrentDirectory(global_designDir);
  return fileChosen;
}


// always a loading dialog, never saves a sound file
BOOL GetSoundFilename(CString& defaultFilename, CString Ext)
{
  static char LastDirectory[_MAX_PATH+1]= {'\0'};
  BOOL fileChosen = FALSE;  
    
  char ExtText[5];  
  strncpy(ExtText, Ext, 3);
  ExtText[3] = '\0';
  
  char szFilter[_MAX_PATH+1];
  strncpy(szFilter, szAllSndFilter, _MAX_PATH); // szAllSndFilter from file Sound.h
  szFilter[_MAX_PATH]='\0';

  DWORD flags = (OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST);

  CFileDialog dlg(TRUE, Ext, defaultFilename, flags, szFilter);

  if (strlen(LastDirectory) > 2)
    dlg.m_ofn.lpstrInitialDir = LastDirectory;
  else
    dlg.m_ofn.lpstrInitialDir = global_designDir;

  if (dlg.DoModal() == IDOK)
  {
     CString tmp("");
     defaultFilename = dlg.GetPathName();
     if (GetFilenamePath(defaultFilename, tmp)) 
     {
       strncpy(LastDirectory,tmp,_MAX_PATH);
       LastDirectory[_MAX_PATH]='\0';
     }
     fileChosen = TRUE;
  }
  SetCurrentDirectory(global_designDir);
  return fileChosen;
}


#define ATTEMPT_PREVIEW
#undef ATTEMPT_PREVIEW


// display image preview next to list of files, and show each image as
// files are highlighted.
BOOL GetFilenameWithPreview(SurfaceType type, CString& defaultFilename)
{
  static char LastDirectory[_MAX_PATH+1]= {'\0'};
  BOOL fileChosen = FALSE;
  char defaultExt[] = "png";
  char szFilter[] = 
    "All Types(*.bmp;*.pcx;*.jpg;*.png;*.tga)|*.bmp;*.pcx;*.jpg;*.png;*.tga|"
    "Bitmaps (*.bmp)|*.bmp|"
    "PCX Files (*.pcx)|*.pcx|"
    "JPEG Files (*.jpg)|*.jpg|"
    "PNG Files (*.png)|*.png|"
    "Targa (*.tga)|*.tga||";

  DWORD flags = (OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST);

  //GetFilenamePath(defaultFilename, defaultDir);

#ifdef ATTEMPT_PREVIEW
  CPreviewFileDlg dlg(TRUE, 
                      defaultExt, 
                      defaultFilename, 
                      flags,
                      szFilter);
#else
  CFileDialog dlg(TRUE, defaultExt, defaultFilename, flags, szFilter);
#endif
  if (strlen(LastDirectory) > 2)
    dlg.m_ofn.lpstrInitialDir = LastDirectory;
  else
    dlg.m_ofn.lpstrInitialDir = global_designDir;

  if (dlg.DoModal() == IDOK)
  {
     CString tmp("");
     defaultFilename = dlg.GetPathName();
     if (GetFilenamePath(defaultFilename, tmp)) 
     {
       strncpy(LastDirectory,tmp,_MAX_PATH);
       LastDirectory[_MAX_PATH]='\0';
     }
     fileChosen = TRUE;
  }
  return fileChosen;
}
/*
//*****************************************************************************
// NAME:    BrowseForFolder
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************

CString BrowseForFolder(HWND hWnd, const char *pTitle)
{
  LPCSTR lpszTitle = pTitle;
  // We're going to use the shell to display a 
  // "Choose Directory" dialog box for the user.

  // set root at Desktop
  LPCITEMIDLIST pRoot = NULL;

  CString strResult = "";
  
  LPMALLOC lpMalloc;  // pointer to IMalloc
  // LpMalloc is a pointer to IMalloc, and IMalloc is the COM 
  // interface the Shell uses to allocate structures and pointers 
  // that it returns from most of its functions. To make the pointer
  // point to something, we call SHGetMalloc():
  
  if (::SHGetMalloc(&lpMalloc) != NOERROR)
    return strResult; // failed to get allocator
  
  char szDisplayName[_MAX_PATH];
  char szBuffer[_MAX_PATH];

  // The two buffers allocated after the call to SHGetMalloc() hold information 
  // that the Browse For Folder dialog box returns. szDisplayName will be put into 
  // the pszDisplayName member of the BROWSEINFO structure. This member receives 
  // the name only of the selected folder, not its path.
  
  BROWSEINFO browseInfo;
  browseInfo.hwndOwner = hWnd;
  
  browseInfo.pidlRoot = pRoot; 
  browseInfo.pszDisplayName = szDisplayName;
  browseInfo.lpszTitle = lpszTitle;   
  browseInfo.ulFlags = 0;   // also passed in
  browseInfo.lpfn = NULL;      // not used
  browseInfo.lParam = 0;      // not used   
  
  LPITEMIDLIST lpItemIDList;

  // Since we’re not having a callback function (look up BrowseCallbackProc in the 
  // documentation for the details), we simply NULL- and zero-out the lpfn and lParam 
  // members of the structure. The preceding LPITEMIDLIST variable will receive the 
  // return value of the SHBrowseForFolder() function. If the return value is NULL, 
  // we know SHBrowseForFolder() failed. So call it, testing for a non-NULL return
  // value:
  
  if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
  {
    // To actually get something useful with the LPITEMIDLIST pointer returned, 
    // we can use SHGetPathFromIDList(), passing our lpItemIDList and szBuffer 
    // variables as arguments. 
    // When the function returns, we should have a pathname to a folder in szBuffer. 
    // If szBuffer[0] is ‘\0’, the NULL-terminator, then we don’t have anything we 
    // can work with. Otherwise, we stuff szBuffer into strResult, using CString’s 
    // very useful assignment operator, and return the pathname to the caller as a 
    // reference to a CString, something that another of CString’s assignment 
    // operators accepts:
    
    // Get the path of the selected folder from the
    // item ID list.
    if (::SHGetPathFromIDList(lpItemIDList, szBuffer))
    {
      // At this point, szBuffer contains the path 
      // the user chose.
      if (szBuffer[0] == '\0')
      {
        // SHGetPathFromIDList failed, or
        // SHBrowseForFolder failed.
        AfxMessageBox("Failed to get directory",
          MB_ICONSTOP|MB_OK);
      }
      
      // We have a path in szBuffer!
      // Return it.
      pRoot = lpItemIDList;
      strResult = szBuffer;
    }
    else
    {
      // The thing referred to by lpItemIDList 
      // might not have been a file system object.
      // For whatever reason, SHGetPathFromIDList
      // didn't work!
      AfxMessageBox("Failed to get directory",
        MB_ICONSTOP|MB_OK);
    }

    // Okay, now for the cleanup. The shell made use of its IMalloc interface 
    // (remember that LPMALLOC pointer we filled earlier) in order to allocate 
    // memory to hold our LPITEMIDLIST variable, which was filled by SHBrowseForFolder(). 
    // We need to clean up by freeing the memory through IMalloc::Free. Since IMalloc 
    // maintains a reference count to keep track of when it should do its own cleanup 
    // (as do all COM interfaces), we need to call the IUnknown::Release method. Memory
    // could leak if we don’t.    
    //
    lpMalloc->Free(lpItemIDList);
    lpMalloc->Release();      
  }
  
  // If we made it this far, SHBrowseForFolder failed.
  return strResult;
}
*/

#endif // ifdef UAFEDITOR

void DeleteAllFilesInFolder(const char *path)
{
  char fullPath[_MAX_PATH+1];

  if (path == NULL)
    return;

   strcpy(fullPath, path);

  int len = strlen(fullPath);
  if (len < 3)
    return;

  if (fullPath[len-1] != '\\')
    strcat(fullPath, "\\");

   strcat(fullPath, "*.*");
 
   CFileFind finder;
   BOOL found = finder.FindFile(fullPath);
   while (found)
   {
      found = finder.FindNextFile();

      if (finder.IsDots())
         continue;

      if (finder.IsDirectory())
        continue;

      // remove read-only flags
      SetFileAttributes(finder.GetFilePath(), FILE_ATTRIBUTE_NORMAL);
      remove(finder.GetFilePath());
   }

   finder.Close();
}

BOOL SearchForFileInPath(CString &file, const char *FileExt, const CString &path, BOOL SearchSubfolders)
{
  if (file.IsEmpty()) return FALSE;
  if (path.IsEmpty()) return FALSE;

  CFileFind finder;
  CString subFolder="";
  CString searchPath(path);

  // Just to be sure that our string comparison will be valid, strip
  // any path information from the incoming filename. We are given the
  // path to search anyway, so path info in the filename can be ignored.
  // 
  // If the file is found, the full path is returned in the filename.
  //
  CString tmpFile(file);
  StripFilenamePath(tmpFile);

  if (searchPath[searchPath.GetLength()-1] != '\\')
    searchPath += "\\";
  CString StartSubFolder(searchPath);

  // This function won't really recurse if the 
  // file extension is provided since folder names
  // won't have a file extension in their name.
  if (FileExt == NULL)
  {
    searchPath += "*.*";  
  }
  else
  {
    if (*FileExt == '\0')
    {
      searchPath += "*.*";  
    }
    else
    {
      searchPath += "*.";
      searchPath += FileExt;
    }
  }

  BOOL found = finder.FindFile(searchPath);
  
  while (found)
  {
    found = finder.FindNextFile();

    if (finder.IsDots())
       continue;

    if (finder.IsDirectory())
    {
      if ((SearchSubfolders) && (finder.GetFileName().CompareNoCase("backup")!=0))
      {
        // recurse into sub-directory
        subFolder = StartSubFolder;
        // add sub folder onto path
        subFolder += finder.GetFileName();
        if (SearchForFileInPath(tmpFile, FileExt, subFolder, SearchSubfolders))
          return TRUE;
      }
      continue; // ignore subfolder
    }

    if (finder.GetFileName().CompareNoCase(tmpFile) == 0)
    {
      file = finder.GetFilePath();
      return TRUE;
    }
  }
  return FALSE;
}


/*
BOOL SearchForArtFile(CString &file, const CString& artDir)
{
  if (FileExists(file)) return TRUE;
  CString fullPath;
  fullPath = artDir + file;
  if (FileExists(fullPath)) 
  {
    file = fullPath;
    return TRUE;
  };
  return FALSE;
};

BOOL SearchForSoundFile(CString &file)
{
  if (FileExists(file)) return TRUE;
  CString fullPath;
  fullPath = rte.SoundDir() + file;
  if (FileExists(file)) 
  {
    file = fullPath;
    return TRUE;
  };
  
  return FALSE;
};
BOOL SearchForFile(CString &file, const CString& dir)
{
  if (FileExists(file)) return TRUE;
  CString fullPath;
  fullPath = rte.SoundDir() + file;
  if (FileExists(file)) 
  {
    file = fullPath;
    return TRUE;
  };
  
  return FALSE;
};
*/

#ifdef UAFEngine
BOOL SearchForFile(CString& file, const CString& runtimeDir)
#else
BOOL SearchForFile(CString& file, const CString& runtimeDir, const CString &editorDir)
#endif
{
  CString fullpath;
  if (runtimeDir != "")
  {
    //CString name;
    //name = file;
    //StripFilenamePath(name);
    fullpath = runtimeDir + file;
  }
  else
  {
    fullpath = file;
  };
  if (FileExists(fullpath))
  {
    file = fullpath;
    return TRUE;
  };

#ifdef UAFEngine
#else
  if (editorDir != "")
  {
    //CString name;
    //name = file;
    //StripFilenamePath(name);
    fullpath = editorDir + file;
    if (FileExists(fullpath))
    {
      file = fullpath;
      return TRUE;
    };
  };
  if (FileExists(file))
  {
    return TRUE;
  };
#endif
  return FALSE;
}


//CCriticalSection ErrorLogCS;
//FILE* pErrorLogFile = NULL;

void CloseErrorLog()
{
//  CSingleLock sl(&ErrorLogCS, TRUE);
  
  UseErrorLog=FALSE;
  if (pErrorLogFile != NULL)
  {
    WriteDebugString("Closing Log File\n");
    fclose(pErrorLogFile);
  };
  pErrorLogFile = NULL;
  CleanupStackWalk();
}

void WriteStackTrace()
{
//  CSingleLock sl(&ErrorLogCS, TRUE);
  ShowStack( pErrorLogFile );
}


WRITE_DEBUG_DIALOG writeDebugDialog = WRITE_DEBUG_DIALOG_NONE;

const int maxDebugLineLength = 65000;

const char *severityCodes = " .,:=+\"^$*@";

int debugSeverity = 0;
void WriteDebugString(const char *ptext, ... )
{
  // Severity = 0-9  ;  0 = information
 
  unsigned int severity;
  severity = debugSeverity;
  debugSeverity = 0;
  if (severity > 9) severity = 9;
  WRITE_DEBUG_DIALOG wdd;
  char errText[maxDebugLineLength+33+20];  // 20 just to be sure!
  static CString ErrorLogName = "";
  static BYTE WriteCount = 0;
  const int MAX_ERROR_LOG_SIZE = 1024000;
  static DWORD prevTime = 0;
  static CArray<CString, CString&> logQueue;
  static bool messagesLost = false;
  
  wdd = writeDebugDialog;
  writeDebugDialog = WRITE_DEBUG_DIALOG_NONE;

#ifdef UAFEngine
  if (ExitSignaled)
    return;
#endif

  DETAILTRACE(0xf53101,0);

  if (!UseErrorLog)
    return;


  try
  {


//    CSingleLock sl(&ErrorLogCS, TRUE);

    // check the size every 10 writes
    if ((WriteCount % 10) == 0)
    {
      // if reached max size, turn it off
      if (FileSize(ErrorLogName) >= MAX_ERROR_LOG_SIZE)
      {
        fprintf(pErrorLogFile, "*** Turning error log off - reached max allowed size ***\n");
        CloseErrorLog();
      }
    }

    va_list marker;	
  //  time_t ltime;
    //_timeb timeBuf;
    //_ftime(&timeBuf);
    DWORD curTime;
    curTime = timeGetTime();
  //  errText[0] = '\0';
    //if (curTime > prevTime) prevTime = curTime;
    sprintf(errText,"%08x %6d %016I64x%c",curTime, curTime-prevTime, virtualGameTime, severityCodes[severity]);
    prevTime = curTime;

    va_start(marker, ptext);  
    int count = _vsnprintf(errText+33, maxDebugLineLength, ptext, marker);
    va_end(marker);

    if (count < 0)
      strcpy(errText, "Error msg exceeded buffer capacity\n");
    else
      errText[509] = '\n';
      errText[510] = '\0'; // guarantee null terminator
    if (   (pErrorLogFile == NULL)
#ifdef UAFEngine
        && EngineRunning
#endif
        && (rte.LogDir() != ""))
    {



      CString path;
#ifdef UAFEDITOR
      path = rte.LogDir();
      path += "UafErr_Edit.txt";
#else
      path = rte.LogDir();
      path += "UafErr_Engine.txt";
#endif
      pErrorLogFile = fopen(path, "w");

      if (pErrorLogFile)
      {
        _timeb timeBuf;
        _ftime(&timeBuf);
  //      time(&ltime);
        if (logQueue.GetSize() != 0)
        {
          int i;
          for (i=0; i<logQueue.GetSize(); i++)
          {
            fprintf(pErrorLogFile, "%s", (LPCTSTR)logQueue.GetAt(i));
          };
          logQueue.RemoveAll();
        };
        fprintf(pErrorLogFile, "Opened Error Log on %s", ctime(&timeBuf.time) /*ctime(&ltime)*/);
        WriteCount=0;
        ErrorLogName = path;
  //      time(&ltime);
        //fprintf(pErrorLogFile, "%s\n", configMsg);
        fflush(pErrorLogFile);
      }
      else
      {
        if (!debugStrings.AlreadyNoted(CString("UTOLF")))
        {
          CString msg;
          msg.Format("Unable to open log file %s", path);
          MsgBoxInfo(msg, "Warning");
        };
        ErrorLogName = "";
        CString temp = "Unable to open log file ";
        temp += path;
        TRACE(temp);
      }
    }
    if (pErrorLogFile) 
    {
      fprintf(pErrorLogFile, "%s", errText); 
      //WriteStackTrace();
      if (wdd != WRITE_DEBUG_DIALOG_NONE)
      {
        MessageBox(NULL, errText, "ALERT", MB_TOPMOST|MB_OK|MB_ICONWARNING);
      };
      fflush(pErrorLogFile);
      WriteCount++;
    }
    else
    {
      if (!messagesLost)
      {
        if (logQueue.GetSize() < 10000)
        {
          CString msg = errText;
          logQueue.Add(msg);
        }
        else
        {
          logQueue.RemoveAll();          
          messagesLost = true;
          CString msg = "Error Messages lost because no Log File Yet Exists\n"
                    "This is the first one.  Any more will be ignored\n\n";
          msg += errText;
          MsgBoxInfo(msg, "Warning");
        };
      };
      TRACE(errText);
    };
  }
  catch (...)
  {
    TRACE("Exception while trying to format error text in WriteDebugString()\n");
  }
}



CString ResolveRelativePath(char *path)
{
  if (memcmp(path, ".\\", 2) != 0) 
  {
    return CString(path);
  };
  return CString(global_designDir) + (path + 2);
}


void ReadPageFormat(const CString& prefix, SELECTION_PAGE_FORMAT& format)
{
  int x, y;
  CString str;
  if (ConfigFile.FindTokens(prefix + "_TITLE_XY", x, y))
  {
    format.Title_x = x;
    format.Title_y = y;
  };
  if (ConfigFile.FindToken(prefix + "_TITLE_FORMAT", str))
  {
    format.Title_Format = str;
  };
  if (ConfigFile.FindToken(prefix + "_HEADER_FORMAT", str))
  {
    format.Header_Format = str;
  };
  if (ConfigFile.FindToken(prefix + "_HEADER2_FORMAT", str))
  {
    format.Header2_Format = str;
  };
  if (ConfigFile.FindToken(prefix + "_HEADER3_FORMAT", str))
  {
    format.Header3_Format = str;
  };
  if (ConfigFile.FindTokens(prefix + "_HEADER_XY", x, y))
  {
    format.Header_x = x;
    format.Header_y = y;
  };
  if (ConfigFile.FindTokens(prefix + "_HEADER2_XY", x, y))
  {
    format.Header2_x = x;
    format.Header2_y = y;
  };
  if (ConfigFile.FindTokens(prefix + "_HEADER3_XY", x, y))
  {
    format.Header3_x = x;
    format.Header3_y = y;
  };
  if (ConfigFile.FindTokens(prefix + "_TABLE_XY", x, y))
  {
    format.Table_x = x;
    format.Table_y = y;
  };
  if (ConfigFile.FindTokens(prefix + "_MENU_XY", x, y))
  {
    format.Menu_x = x;  
    format.Menu_y = y;
  };
  if (ConfigFile.FindTokens(prefix + "_TABLE2_XY", x, y))
  {
    format.Table2_x = x;
    format.Table2_y = y;
  };
  if (ConfigFile.FindTokens(prefix + "_TABLE3_XY", x, y))
  {
    format.Table3_x = x;
    format.Table3_y = y;
  };
  if (ConfigFile.FindToken(prefix + "_TABLE_FORMAT", str))
  {
    format.Table_Format = str;
  };
  if (ConfigFile.FindToken(prefix + "_TABLE2_FORMAT", str))
  {
    format.Table2_Format = str;
  };
  if (ConfigFile.FindToken(prefix + "_TABLE3_FORMAT", str))
  {
    format.Table3_Format = str;
  };
  if (ConfigFile.FindToken(prefix + "_TABLE_SELECTED_FORMAT", str))
  {
    format.Table_Selected_Format = str;
  };
  if (ConfigFile.FindToken(prefix + "_TABLE2_SELECTED_FORMAT", str))
  {
    format.Table2_Selected_Format = str;
  };
  if (ConfigFile.FindToken(prefix + "_TABLE3_SELECTED_FORMAT", str))
  {
    format.Table3_Selected_Format = str;
  };
  if (ConfigFile.FindToken(prefix + "_LINES_PER_PAGE", x))
  {
    format.Lines_Per_Page = x;
  };
  if (ConfigFile.FindToken(prefix + "_LINES_SPACING", x))
  {
    format.Line_Spacing = x;
  };
  if (ConfigFile.FindToken(prefix + "_MENU_SPACING", x))
  {
    format.Menu_Spacing = x;
  };
}

BOOL LoadConfigFile(const char *path)
{
  CString tmp;

#ifdef UAFEDITOR
  char temp[_MAX_PATH*2];
#endif
  //int len;
  //SetCurrentDirectory(path);
  tmp = path;
  if (tmp.Right(1) == '\\') tmp += "config.txt";
  if (!ConfigFile.Parse(tmp,1))
  {
    MsgBoxError("Unable to find and load config.txt file","Config.txt Error");
    return FALSE;
  }

  ConfigFile_DB.Clear();
  {
    int i, n;
    n = ConfigFile.GetCount();
    for (i=0; i<n; i++)
    {
      CString key, value;
      CStringPAIR pair;
      ConfigFile.GetTokenAt(i, key, value);
      ConfigFile_DB.Insert(CStringPAIR(key,value));
    };
  };

  WriteDebugString("Loading Config File = '%s'\n", ConfigFile.GetFilename());

#ifdef UAFEDITOR
  {
    sprintf(temp, "Editor Art Dir = %s\n", (LPCSTR)ede.EditorWindowArtDir());
    WriteDebugString(temp);
  };
#endif

#ifdef UAFEDITOR

  //ConfigFile.FindToken("Install_Dir", global_designDir, sizeof(global_gameDir));
    


   
  {
    sprintf(temp, "Editor Art Dir = %s\n", (LPCSTR)ede.EditorWindowArtDir());
    WriteDebugString(temp);

    sprintf(temp, "%s%s", (LPCSTR)ede.EditorMapArtDir(), (LPCSTR)MAPART);
  };
  tmp = temp;
   //if (!SearchForFile(tmp))
  CString art;
  art = ede.EditorMapArtDir();
  if (!FindImageWithValidExtInFolder(tmp, art, FALSE))
  {
    CString error;
    error.Format("Failed to find required art file %s\nPlease re-install Dungeon Craft", tmp);
    WriteDebugString("%s\n", error);
    MsgBoxError(error, "Installation Error");
    return FALSE;
  }
  {
    char tempPath[_MAX_PATH*2+1];
    sprintf(tempPath, "%s%s", (LPCSTR)ede.TemplateOverlandArtDir(), (LPCSTR)OVERLANDART);
  
    tmp = tempPath;
    //if (!SearchForFile(tmp))
    {
      CString artDir = ede.TemplateOverlandArtDir();
      if (!FindImageWithValidExtInFolder(tmp, artDir, FALSE))
      {
        CString error;
        error.Format("Failed to find required art file %s\nPlease re-install Dungeon Craft", tempPath);
        WriteDebugString("%s\n", error);
        MsgBoxError(error, "Installation Error");
        return FALSE;
      };
    };
  };
  ConfigFile.FindToken("MAX_UNDO_ACTIONS", MAX_UNDO_ACTIONS);

  int tRed, tGreen, tBlue;
  for (int c=0;c<MAX_COLOR_SLOTS;c++)
  {
    tmp.Format("EDITOR_COLOR_%i", c+1);
    if (ConfigFile.FindTokens(tmp, tRed, tGreen, tBlue))
      m_Colors[c] = RGB(tRed, tGreen, tBlue);      
  }

  if (ConfigFile.FindToken("DEF_CHAR_DEATH", tmp))
    DEF_CHAR_DEATH_SOUND = tmp;
  if (ConfigFile.FindToken("DEF_CHAR_MOVE", tmp))
    DEF_CHAR_MOVE_SOUND = tmp;
  if (ConfigFile.FindToken("DEF_HIT", tmp))
    DEF_HIT_SOUND = tmp;
  if (ConfigFile.FindToken("DEF_MISS", tmp))
    DEF_MISS_SOUND = tmp;
  if (ConfigFile.FindToken("DEF_CAST", tmp))
    DEF_CAST_SOUND = tmp;
  if (ConfigFile.FindToken("DEF_BUMP", tmp))
    DEF_BUMP_SOUND = tmp;
  if (ConfigFile.FindToken("DEF_STEP", tmp))
    DEF_STEP_SOUND = tmp;
  if (ConfigFile.FindToken("DEF_INTRO", tmp))
    DEF_INTRO_SOUND = tmp;
  if (ConfigFile.FindToken("DEF_DEATH", tmp))
    DEF_DEATH_SOUND = tmp;
  if (ConfigFile.FindToken("DEF_HORZ_BAR", tmp))
    DEFAULT_HB = tmp;
  if (ConfigFile.FindToken("DEF_VERT_BAR", tmp))
    DEFAULT_VB = tmp;
  if (ConfigFile.FindToken("DEF_FRAME", tmp))
    DEFAULT_FR = tmp;
  if (ConfigFile.FindToken("DEF_COMBAT_CURSOR", tmp))
    DEFAULT_CC = tmp;
  if (ConfigFile.FindToken("DEF_COMBAT_DEATH_ICON", tmp))
    DEFAULT_CDI = tmp;
  if (ConfigFile.FindToken("DEF_COMBAT_PETRIFIED_ICON", tmp))
    DEFAULT_CPI = tmp;
  if (ConfigFile.FindToken("DEF_COMBAT_DEATH", tmp))
    DEFAULT_CD = tmp;
  if (ConfigFile.FindToken("DEF_COMBAT_WALLS", tmp))
    DEFAULT_CWD = tmp;
  if (ConfigFile.FindToken("DEF_COMBAT_WALLS_WILDERNESS", tmp))
    DEFAULT_CWW = tmp;
  if (ConfigFile.FindToken("DEF_CAMP", tmp))
    DEFAULT_CA = tmp;
  if (ConfigFile.FindToken("DEF_TREASURE", tmp))
    DEFAULT_TR = tmp;
  if (ConfigFile.FindToken("DEF_ICONBG", tmp))
    DEFAULT_IB = tmp;
  if (ConfigFile.FindToken("DEFAULT_CURSOR", tmp))
    DEFAULT_CURSOR = tmp;   
  ConfigFile.FindToken("MAX_DEFAULT_ART_SLOTS", MaxDefaultArtSlots);
#endif

  if (ConfigFile.FindToken("DEF_MONSTER_ICON", tmp))
    DEFAULT_MONSTER_ICON = tmp;


  {
    CString t;
    g_cdxlog.LogType(0);
    if (ConfigFile.FindToken("CDX_LOG", t))
    {
      g_cdxlog.LogType(1);
    };
  };


#ifdef UAFEngine
  int x,y,z,x2,y2;


  if (ConfigFile.FindTokens("BACKGROUND_FILL", x,y,z))
  {
    BACKGROUND_FILL = RGB(x,y,z);
    BACKWARD_BACKGROUND_FILL = RGB(z,y,x);
  };
  if (ConfigFile.FindTokens("BACKGROUND_COMBAT_FILL", x,y,z))
    BACKGROUND_COMBAT_FILL = RGB(z,y,x);
  ConfigFile.FindToken("Inter_Character_Delay", INTER_CHARACTER_DELAY, false);
     
  if (ConfigFile.FindTokens("DESIGN_VERSION", x, y))
  {
    dsgnVerX = x;
    dsgnVerY = y;
  }

  if (ConfigFile.FindTokens("COMBAT_STAT_BOX", x, y))
  {
    DEFAULT_COMBAT_STAT_X = x;
    DEFAULT_COMBAT_STAT_Y = y;
  };

  if (ConfigFile.FindTokens("COMBAT_SCREEN_SIZE", x, y))
  {
    TILES_HORZ = x;
    TILES_VERT = y;
  };

  if (ConfigFile.FindTokens("COMBAT_SCREEN_XY", x, y))
  {
    CombatScreenX = x;
    CombatScreenY = y;
  };

  if (ConfigFile.FindTokens("DEFAULT_MENU_HORZ", x, y))
  {
    DEFAULT_MENU_HORZ_X = x;
    DEFAULT_MENU_HORZ_Y = y;
  }
  DEFAULT_MENU_COMBAT_HORZ_X = -1;
  if (ConfigFile.FindTokens("DEFAULT_MENU_COMBAT_HORZ", x, y))
  {
    DEFAULT_MENU_COMBAT_HORZ_X = x;
    DEFAULT_MENU_COMBAT_HORZ_Y = y;
  }
  if (ConfigFile.FindTokens("DEFAULT_MENU_VERT", x, y))
  {
    DEFAULT_MENU_VERT_X = x;
    DEFAULT_MENU_VERT_Y = y;
  }

  if (ConfigFile.FindTokens("DEFAULT_MENU_TEXTBOX", x, y))
  {
    // these menus start in middle of textbox area (ie: question list event)
    DEFAULT_MENU_TEXTBOX_X = x;
    DEFAULT_MENU_TEXTBOX_Y = y;
  }
   


  /*  **********************************
   *  The colors White, Green, etc no longer are used.
   *  They are now contained in globalData.availableFonts[0].
   *  We maintain these config parameters for backward compatability.
   */
  if (ConfigFile.FindTokens("COLOR_WHITE", x, y, z))
// prs 20191221   White = RGB(x,y,z);
// prs 20191227   globalData.availableFonts[0].color[whiteColor] = RGB(x, y, z);
    globalData.fontLib.SetFontColor(0, whiteColor, RGB(x, y, z));
  if (ConfigFile.FindTokens("COLOR_BLACK", x,y,z))
// prs 20191221    Black = RGB(x,y,z);
//    globalData.availableFonts[0].color[blackColor] = RGB(x, y, z);
    globalData.fontLib.SetFontColor(0, blackColor, RGB(x, y, z));
  if (ConfigFile.FindTokens("COLOR_GREEN", x,y,z))
// prs 20191221    Green = RGB(x,y,z);
//    globalData.availableFonts[0].color[greenColor] = RGB(x, y, z);
    globalData.fontLib.SetFontColor(0, greenColor, RGB(x, y, z));
  if (ConfigFile.FindTokens("COLOR_RED", x,y,z))
// prs 20191221    Red = RGB(x,y,z);
//    globalData.availableFonts[0].color[redColor] = RGB(x, y, z);
    globalData.fontLib.SetFontColor(0, redColor, RGB(x, y, z));
  if (ConfigFile.FindTokens("COLOR_YELLOW", x,y,z))
// prs 20191221    Yellow = RGB(x,y,z);
//    globalData.availableFonts[0].color[yellowColor] = RGB(x, y, z);
    globalData.fontLib.SetFontColor(0, yellowColor, RGB(x, y, z));
  if (ConfigFile.FindTokens("COLOR_ORANGE", x,y,z))
// prs 20191221    Orange = RGB(x,y,z);
//    globalData.availableFonts[0].color[orangeColor] = RGB(x, y, z);
    globalData.fontLib.SetFontColor(0, orangeColor, RGB(x, y, z));
  if (ConfigFile.FindTokens("COLOR_BLUE", x,y,z))
// prs 20191221    Blue = RGB(x,y,z);
//    globalData.availableFonts[0].color[blueColor] = RGB(x, y, z);
    globalData.fontLib.SetFontColor(0, blueColor, RGB(x, y, z));
  if (ConfigFile.FindTokens("COLOR_COMBAT_GREEN", x,y,z))
// prs 20191221    CombatGreen = RGB(x,y,z);
//    globalData.availableFonts[0].color[combatGreenColor] = RGB(x, y, z);
    globalData.fontLib.SetFontColor(0, combatGreenColor, RGB(x, y, z));
  if (ConfigFile.FindTokens("COLOR_COMBAT_RED", x,y,z))
// prs 20191221    CombatRed = RGB(x,y,z);
//    globalData.availableFonts[0].color[combatRedColor] = RGB(x, y, z);
    globalData.fontLib.SetFontColor(0, combatRedColor, RGB(x, y, z));
  if (ConfigFile.FindTokens("COLOR_COMBAT_YELLOW", x,y,z))
// prs 20191221    CombatYellow = RGB(x,y,z);
//    globalData.availableFonts[0].color[combatYellowColor] = RGB(x, y, z);
    globalData.fontLib.SetFontColor(0, combatYellowColor, RGB(x, y, z));
  if (ConfigFile.FindTokens("COLOR_COMBAT_ORANGE", x,y,z))
// prs 20191221    CombatOrange = RGB(x,y,z);
//    globalData.availableFonts[0].color[combatOrangeColor] = RGB(x, y, z);
    globalData.fontLib.SetFontColor(0, combatOrangeColor, RGB(x, y, z));
  if (ConfigFile.FindTokens("COLOR_COMBAT_BLUE", x,y,z))
// prs 20191221    CombatBlue = RGB(x,y,z);
//    globalData.availableFonts[0].color[combatBlueColor] = RGB(x, y, z);
    globalData.fontLib.SetFontColor(0, combatBlueColor, RGB(x, y, z));



  if (ConfigFile.FindTokens("COLOR_KEYBOARD_SHORTCUT", x,y,z))
    KeyboardCharColor = RGB(x,y,z);
  ConfigFile.FindToken("USE_KEYBOARD_SHORTCUT", UseKeyboardShortcuts);
    
  if (!ConfigFile.FindToken("Screen_Width", SCREEN_WIDTH))
    SCREEN_WIDTH = 640;
  if (!ConfigFile.FindToken("Screen_Height", SCREEN_HEIGHT))
    SCREEN_HEIGHT = 480;
  if (!ConfigFile.FindToken("Color_Bits", COLOR_BITS))
    COLOR_BITS = 16;

  {
    int fNum;
    CString fName;
    while (ConfigFile.FindTokens("FONT_NAME", fNum, fName, true))
    {
      if ((fNum >= 0) && (fNum < NUMTEXTFONT))
      {
        globalData.fontLib.SetFontName(fNum, fName);
      };
    };
  };


  {
    int fNum, r, g, b;
    //FONT_COLOR_NUM cNum;
    //CString cName;
    while (ConfigFile.FindTokens("FONT_COLOR", fNum, r, g, b, true))
    {
      //cNum = ASCII_TO_COLOR(cName, whiteColor);
      //if (cNum == illegalColor) continue;
      if ((fNum >= 0) && (fNum < NUMTEXTFONT))
      {
        // prs 20191227 globalData.availableFonts[fNum].color[cNum] = RGB(r, g, b);
        globalData.fontLib.SetFontColor(fNum, zeroColor, RGB(r, g, b));
      };
    };
  };




  {
    int fNum;
    //FONT_COLOR_NUM cNum;
    CString ftype;
    while (ConfigFile.FindTokens("FONT_TYPE", fNum, ftype, true))
    {
      if ((fNum >= 0) && (fNum < NUMTEXTFONT))
      {
        globalData.fontLib.SetFontType(fNum, ftype);
      };
    };
  };


  if (ConfigFile.FindTokens("COLOR_CUSTOM_1", x, y, z))
  {
    CustomColor = RGB(x, y, z);
    globalData.fontLib.SetCustomColor(CustomColor);
  };




#ifdef _DEBUG
  HDC CurrScreen = ::CreateIC("Display", NULL, NULL, NULL);
  int CurrBits = ::GetDeviceCaps(CurrScreen, BITSPIXEL);  
  int CurrWidth = ::GetDeviceCaps(CurrScreen, HORZRES);
  ::DeleteDC(CurrScreen);

  if (SCREEN_WIDTH > CurrWidth)
  {
    WriteDebugString("SCREEN_WIDTH %u in config.txt higher than curr screen width %i, lowering it\n", 
                     SCREEN_WIDTH, CurrWidth);
    SCREEN_WIDTH = CurrWidth;
  }
#endif

  switch (SCREEN_WIDTH)
  {
  case 640:
    m_GameScreenRes = res640x480;
    SCREEN_HEIGHT=480;
    break;
  case 800:
    m_GameScreenRes = res800x600;
    SCREEN_HEIGHT=600;
    break;
  case 1024:
    m_GameScreenRes = res1024x768;
    SCREEN_HEIGHT=768;
    break;
  case 1280:
    m_GameScreenRes = res1280x1024;
    SCREEN_HEIGHT=1024;
    break;
  default:
    SCREEN_WIDTH=640;SCREEN_HEIGHT=480;
    m_GameScreenRes = res640x480;
    break;
  }

  if ((COLOR_BITS < 16) || (COLOR_BITS > 32))
  {
    WriteDebugString("Invalid color bits %u in config.txt, reset to 16\n", COLOR_BITS);
    COLOR_BITS = 16;     
  }

#ifdef _DEBUG
  if (COLOR_BITS > CurrBits)
  {
    WriteDebugString("Color bits %u in config.txt higher than curr screen bits %i, lowering it\n", 
                     COLOR_BITS, CurrBits);
    COLOR_BITS = CurrBits;
  }
#endif

  //
  // the old way to define a textbox size/location
  //
  {
    if (ConfigFile.FindTokens("TEXTBOX", x, y))
    {
      textboxX = x;
      textboxY = y;
    
      TEXTBOX_WIDTH = SCREEN_WIDTH - (textboxX*2);
      TEXTBOX_LINES = 5;
    }
   
    if (!ConfigFile.FindToken("TextBox_Lines", TEXTBOX_LINES)) //
      TEXTBOX_LINES = 5; // default for 640x480   
  
    TEXTBOX_HEIGHT = 16 * TEXTBOX_LINES;
  }

  //
  // the new way to define textbox size/location
  if (ConfigFile.FindTokens("TEXTBOX_RECT", x, y, x2, y2))
  {
    textboxX = x;
    textboxY = y;
   
    TEXTBOX_WIDTH = x2 - x - 1;
    TEXTBOX_HEIGHT = y2 - y - 1;
    TEXTBOX_LINES = (TEXTBOX_HEIGHT / 16);
     
    ASSERT(TEXTBOX_WIDTH > 0);
    ASSERT(TEXTBOX_HEIGHT > 0);
    ASSERT(TEXTBOX_LINES > 0);
  }

  if (ConfigFile.FindToken("START_AGE", START_AGE))
    START_AGE = max(1, START_AGE);
  if (ConfigFile.FindToken("AGE_RANGE", AGE_RANGE))
    AGE_RANGE = max(0, AGE_RANGE);
  if (ConfigFile.FindTokens("PARTYNAMES", x,y,x2,y2))        //DLD
  {                                                          //
        PartyNames_wx = x;                                   //                                     	    
        PartyNames_wy = y;                                   //  	    
        PartyNames_nx = x2;                                  //   	    
        PartyNames_ny = y2;                                  //   	    
  }                                                          //
  else                                                       //
  {                                                          //
        PartyNames_wx = 18;                                  //   	    
        PartyNames_wy = 18;                                  //   	    
        PartyNames_nx = 275;                                 //    	    
        PartyNames_ny = 18;                                  //   	    
  }                                                          //
  if (ConfigFile.FindTokens("LOCATIONTEXT", x,y))            //
  {                                                          //
	  LocationText_x = x;                                      //
    LocationText_y = y;                                      //
  }                                                          //
  if (ConfigFile.FindTokens("VERSIONTEXT", x,y))             //
  {                                                          //
	  VersionText_x = x;                                       //
    VersionText_y = y;                                       //
  }                                                          //
  if (ConfigFile.FindTokens("BIGPICSIZE", x,y,x2,y2))        //
  {                                                          //
	  BigPic_x = x;                                            //
    BigPic_y = y;                                            //
	  BigPic_w = x2;                                            //
    BigPic_h = y2;                                            //
  }                                                          //

  Select_Race_Format.Title_Format = "Select a Race";
  ReadPageFormat(CString("SELECT_RACE"), Select_Race_Format);

  Select_Choosestats_Format.Title_x = 18;     // Name
  Select_Choosestats_Format.Title_y = 35;
  Select_Choosestats_Format.Header_x = 220;   // Status
  Select_Choosestats_Format.Header_y = 35; 
  Select_Choosestats_Format.Header2_x = 18;   // STR
  Select_Choosestats_Format.Header2_y = 172;
  Select_Choosestats_Format.Header3_x = 18;   // AC
  Select_Choosestats_Format.Header3_y = 325;
  Select_Choosestats_Format.Menu_x = 220;     // Encumbrance
  Select_Choosestats_Format.Menu_y = 325;
  ReadPageFormat(CString("SELECT_ROLL_CHARACTER_STATS"), Select_Choosestats_Format);

  Select_Class_Format.Title_Format = "Select a Class";
  ReadPageFormat(CString("SELECT_CLASS"), Select_Class_Format);

  Select_Gender_Format.Title_Format = "Select a Gender";
  ReadPageFormat(CString("SELECT_GENDER"), Select_Gender_Format);

  Select_Alignment_Format.Title_Format = "Select an Alignment";
  ReadPageFormat(CString("SELECT_ALIGNMENT"), Select_Alignment_Format);

  ReadPageFormat(CString("SELECT_MAIN_FUNCTION"), Select_Main_Function_Format);

  ReadPageFormat(CString("SELECT_ADD_CHARACTER"), Select_Add_Character_Format);

  if (!ConfigFile.FindToken("COMBAT_MAP_WIDTH", MAX_TERRAIN_WIDTH))
     MAX_TERRAIN_WIDTH = 50;
  if (!ConfigFile.FindToken("COMBAT_MAP_HEIGHT", MAX_TERRAIN_HEIGHT))
     MAX_TERRAIN_HEIGHT = 50;

  // there's no evidence to suggest 500 as a max, but we
  // need to have a max. Even 500 seems a little too large.
  MAX_TERRAIN_WIDTH = max(25, MAX_TERRAIN_WIDTH);
  MAX_TERRAIN_WIDTH = min(500, MAX_TERRAIN_WIDTH);
  MAX_TERRAIN_HEIGHT = max(25, MAX_TERRAIN_HEIGHT);
  MAX_TERRAIN_HEIGHT = min(500, MAX_TERRAIN_HEIGHT);
   
  if (!ConfigFile.FindToken("COMBAT_STAT_BOX_WIDTH", COMBAT_STAT_BOX_WIDTH))
     COMBAT_STAT_BOX_WIDTH = 240;
  if (COMBAT_STAT_BOX_WIDTH <= 50)
    COMBAT_STAT_BOX_WIDTH = 50;
  // min width is just enough to display stats as currently formatted
  //COMBAT_STAT_BOX_WIDTH = max( COMBAT_STAT_BOX_WIDTH, 240 );

  ConfigFile.FindToken("DISABLE_SCREENSAVER", DisableScreenSaver);
  WriteDebugString("DisableScreenSaver = %i\n", DisableScreenSaver);

  ConfigFile.FindToken("SOUND_PLAYBACK_RATE", SoundPlaybackRate);

#else // ifdef UAFEngine

  int GameWidth=0;
  if (!ConfigFile.FindToken("Screen_Width", GameWidth, false))
    GameWidth = 640;

  switch (GameWidth)
  {
  case 640:
    m_GameScreenRes = res640x480;
    break;
  case 800:
    m_GameScreenRes = res800x600;
    break;
  case 1024:
    m_GameScreenRes = res1024x768;
    break;
  default:
    m_GameScreenRes = res640x480;
    break;
  }

#endif // ifdef UAFEngine

  {
    char tempPath[2*_MAX_PATH+1];
    ConfigFile.FindToken("Force_SysMem", ForceSysMemUsage);
    sprintf(tempPath, "Force_SysMem = %u\n", ForceSysMemUsage);
    WriteDebugString(tempPath);
  };

  ConfigFile.FindToken("LOG_ERRORS", UseErrorLog);
  ConfigFile.FindToken("LOG_COMBAT_ACTIONS", LogCombatActions);
  ConfigFile.FindToken("LOG_DATABASE_ERRORS", LogDatabaseErrors);
  ConfigFile.FindToken("LOG_COMPILE_ERRORS", LogCompileErrors);

  {
    CString debugDate;
    logDebuggingInfo = FALSE;
    if (ConfigFile.FindToken("DEBUG", debugDate))
    {
      CString today;
      int year, month, day;
      time_t t = time(0);   // get time now
      struct tm *now = localtime(&t);
      year = now->tm_year + 1900;
      month = now->tm_mon + 1;
      day = now->tm_mday;
      today.Format("%04d%02d%02d", year, month, day);
      if (today == debugDate) logDebuggingInfo = TRUE;
    }
  };

  WriteDebugString(UseErrorLog?"Error Log ON\n":"Error Log OFF\n");

  ConfigureArtCoords();

  // remove tokens we just read and don't need anymore
  ConfigFile.FlushTokens();
  return TRUE;
}

typedef HRESULT(WINAPI * DIRECTDRAWCREATE)( GUID*, LPDIRECTDRAW*, IUnknown* );
typedef HRESULT(WINAPI * DIRECTDRAWCREATEEX)( GUID*, VOID**, REFIID, IUnknown* );
typedef HRESULT(WINAPI * DIRECTINPUTCREATE)( HINSTANCE, DWORD, LPDIRECTINPUT*,
                                             IUnknown* );

//-----------------------------------------------------------------------------
// Name: GetDXVersion()
// Desc: This function returns the DirectX version number as follows:
//          0x0000 = No DirectX installed
//          0x0100 = DirectX version 1 installed
//          0x0200 = DirectX 2 installed
//          0x0300 = DirectX 3 installed
//          0x0500 = At least DirectX 5 installed.
//          0x0600 = At least DirectX 6 installed.
//          0x0601 = At least DirectX 6.1 installed.
//          0x0700 = At least DirectX 7 installed.
//          0x0800 = At least DirectX 8 installed.
// 
//       Please note that this code is intended as a general guideline. Your
//       app will probably be able to simply query for functionality (via
//       QueryInterface) for one or two components.
//
//       Please also note:
//          "if( dwDXVersion != 0x500 ) return FALSE;" is VERY BAD. 
//          "if( dwDXVersion <  0x500 ) return FALSE;" is MUCH BETTER.
//       to ensure your app will run on future releases of DirectX.
//-----------------------------------------------------------------------------
DWORD GetDXVersion()
{
    DIRECTDRAWCREATE     DirectDrawCreate   = NULL;
    DIRECTDRAWCREATEEX   DirectDrawCreateEx = NULL;
    DIRECTINPUTCREATE    DirectInputCreate  = NULL;
    HINSTANCE            hDDrawDLL          = NULL;
    HINSTANCE            hDInputDLL         = NULL;
    HINSTANCE            hD3D8DLL           = NULL;
    LPDIRECTDRAW         pDDraw             = NULL;
    LPDIRECTDRAW2        pDDraw2            = NULL;
    LPDIRECTDRAWSURFACE  pSurf              = NULL;
    LPDIRECTDRAWSURFACE3 pSurf3             = NULL;
    LPDIRECTDRAWSURFACE4 pSurf4             = NULL;
    DWORD                dwDXVersion        = 0;
    HRESULT              hr;

    // First see if DDRAW.DLL even exists.
    hDDrawDLL = LoadLibrary( "DDRAW.DLL" );
    if( hDDrawDLL == NULL )
    {
        dwDXVersion = 0;
        return dwDXVersion;
    }

    // See if we can create the DirectDraw object.
    DirectDrawCreate = (DIRECTDRAWCREATE)GetProcAddress( hDDrawDLL, "DirectDrawCreate" );
    if( DirectDrawCreate == NULL )
    {
        dwDXVersion = 0;
        FreeLibrary( hDDrawDLL );
        OutputDebugString( "Couldn't LoadLibrary DDraw\r\n" );
        return dwDXVersion;
    }

    hr = DirectDrawCreate( NULL, &pDDraw, NULL );
    if( FAILED(hr) )
    {
        dwDXVersion = 0;
        FreeLibrary( hDDrawDLL );
        OutputDebugString( "Couldn't create DDraw\r\n" );
        return dwDXVersion;
    }

    // So DirectDraw exists.  We are at least DX1.
    dwDXVersion = 0x100;

    // Let's see if IID_IDirectDraw2 exists.
    hr = pDDraw->QueryInterface( IID_IDirectDraw2, (VOID**)&pDDraw2 );
    if( FAILED(hr) )
    {
        // No IDirectDraw2 exists... must be DX1
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        OutputDebugString( "Couldn't QI DDraw2\r\n" );
        return dwDXVersion;
    }

    // IDirectDraw2 exists. We must be at least DX2
    pDDraw2->Release();
    dwDXVersion = 0x200;


	//-------------------------------------------------------------------------
    // DirectX 3.0 Checks
	//-------------------------------------------------------------------------

    // DirectInput was added for DX3
    hDInputDLL = LoadLibrary( "DINPUT.DLL" );
    if( hDInputDLL == NULL )
    {
        // No DInput... must not be DX3
        OutputDebugString( "Couldn't LoadLibrary DInput\r\n" );
        pDDraw->Release();
        return dwDXVersion;
    }

    DirectInputCreate = (DIRECTINPUTCREATE)GetProcAddress( hDInputDLL,
                                                        "DirectInputCreateA" );
    if( DirectInputCreate == NULL )
    {
        // No DInput... must be DX2
        FreeLibrary( hDInputDLL );
        FreeLibrary( hDDrawDLL );
        pDDraw->Release();
        OutputDebugString( "Couldn't GetProcAddress DInputCreate\r\n" );
        return dwDXVersion;
    }

    // DirectInputCreate exists. We are at least DX3
    dwDXVersion = 0x300;
    FreeLibrary( hDInputDLL );

    // Can do checks for 3a vs 3b here


	//-------------------------------------------------------------------------
    // DirectX 5.0 Checks
	//-------------------------------------------------------------------------

    // We can tell if DX5 is present by checking for the existence of
    // IDirectDrawSurface3. First, we need a surface to QI off of.
    DDSURFACEDESC ddsd;
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    hr = pDDraw->SetCooperativeLevel( NULL, DDSCL_NORMAL );
    //hr = pDDraw->SetCooperativeLevel( theApp.m_pMainWnd->GetSafeHwnd(), DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN );
    if( FAILED(hr) )
    {
        // Failure. This means DDraw isn't properly installed.
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        dwDXVersion = 0;
        OutputDebugString( "Couldn't Set coop level\r\n" );
        return dwDXVersion;
    }

    hr = pDDraw->CreateSurface( &ddsd, &pSurf, NULL );
    if( FAILED(hr) )
    {
        // Failure. This means DDraw isn't properly installed.
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        dwDXVersion = 0;
        OutputDebugString( "Couldn't CreateSurface\r\n" );
        return dwDXVersion;
    }

    // Query for the IDirectDrawSurface3 interface
    if( FAILED( pSurf->QueryInterface( IID_IDirectDrawSurface3,
                                       (VOID**)&pSurf3 ) ) )
    {
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    // QI for IDirectDrawSurface3 succeeded. We must be at least DX5
    dwDXVersion = 0x500;


	//-------------------------------------------------------------------------
    // DirectX 6.0 Checks
	//-------------------------------------------------------------------------

    // The IDirectDrawSurface4 interface was introduced with DX 6.0
    if( FAILED( pSurf->QueryInterface( IID_IDirectDrawSurface4,
                                       (VOID**)&pSurf4 ) ) )
    {
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    // IDirectDrawSurface4 was create successfully. We must be at least DX6
    dwDXVersion = 0x600;
    pSurf->Release();
    pDDraw->Release();


	//-------------------------------------------------------------------------
    // DirectX 6.1 Checks
	//-------------------------------------------------------------------------


    // manikus could not pass the DX6.1 test on his machine running Windows server 2003.
    // But the program ran just fine if we ignored the test.  So I am removing the
    // test for DX6.1 to see if that will suffice to make him happy.  There is no
    // need for this test in our particular case.
    // Paul R. Stevens 14 Oct 2008
    //
//    // Check for DMusic, which was introduced with DX6.1
//    LPDIRECTMUSIC pDMusic = NULL;
//    CoInitialize( NULL );
//    hr = CoCreateInstance( CLSID_DirectMusic, NULL, CLSCTX_INPROC_SERVER,
//                           IID_IDirectMusic, (VOID**)&pDMusic );
//    if( FAILED(hr) )
//    {
//        OutputDebugString( "Couldn't create CLSID_DirectMusic\r\n" );
//        FreeLibrary( hDDrawDLL );
//        return dwDXVersion;
//    }
//
//    // DirectMusic was created successfully. We must be at least DX6.1
//    dwDXVersion = 0x601;
//    pDMusic->Release();
//    CoUninitialize();
    

	//-------------------------------------------------------------------------
    // DirectX 7.0 Checks
	//-------------------------------------------------------------------------

    // Check for DirectX 7 by creating a DDraw7 object
    LPDIRECTDRAW7 pDD7;
    DirectDrawCreateEx = (DIRECTDRAWCREATEEX)GetProcAddress( hDDrawDLL,
                                                       "DirectDrawCreateEx" );
    if( NULL == DirectDrawCreateEx )
    {
        FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    if( FAILED( DirectDrawCreateEx( NULL, (VOID**)&pDD7, IID_IDirectDraw7,
                                    NULL ) ) )
    {
        FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    // DDraw7 was created successfully. We must be at least DX7.0
    dwDXVersion = 0x700;
    pDD7->Release();


	//-------------------------------------------------------------------------
    // DirectX 8.0 Checks
	//-------------------------------------------------------------------------

    // Simply see if D3D8.dll exists.
    hD3D8DLL = LoadLibrary( "D3D8.DLL" );
    if( hD3D8DLL == NULL )
    {
	    FreeLibrary( hDDrawDLL );
        return dwDXVersion;
    }

    // D3D8.dll exists. We must be at least DX8.0
    dwDXVersion = 0x800;


	//-------------------------------------------------------------------------
    // End of checking for versions of DirectX 
	//-------------------------------------------------------------------------

    // Close open libraries and return
    FreeLibrary( hDDrawDLL );
    FreeLibrary( hD3D8DLL );
    
    return dwDXVersion;
}

CString GetSystemVersion()
{
   OSVERSIONINFOEX osvi;

   // Try calling GetVersionEx using the OSVERSIONINFOEX structure,
   // which is supported on Windows 2000.
   //
   // If that fails, try using the OSVERSIONINFO structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   //osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   //if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   //{
      // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.

      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
         return "OS Detect Error";
   //}

   CString os="";
   switch (osvi.dwPlatformId)
   {
      case VER_PLATFORM_WIN32_NT:
       {
      // Test for the product.
         if ( osvi.dwMajorVersion <= 4 )
            os = "Microsoft Windows NT";
         if ( osvi.dwMajorVersion == 5 )
            os = "Microsoft Windows 2000";

      // Test for workstation versus server.
         //if( bOsVersionInfoEx )
         //{
         //   if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_NT )
         //      os += " Professional";
         //   if ( osvi.dwPlatformId == VER_NT_SERVER )
         //      os += " Server";
        // }
         //else
         {
            HKEY hKey;
            char szProductType[80];
            DWORD dwBufLen;

            RegOpenKeyEx( HKEY_LOCAL_MACHINE,
               "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
               0, KEY_QUERY_VALUE, &hKey );
            RegQueryValueEx( hKey, "ProductType", NULL, NULL,
               (LPBYTE) szProductType, &dwBufLen);
            RegCloseKey( hKey );
            if ( lstrcmpi( "WINNT", szProductType) == 0 )
               os += " Workstation";
            if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
               os += " Server";
         }

      // Display version, service pack (if any), and build number.
         CString ver;
         ver.Format(" version %d.%d %s (Build %d)",
                    osvi.dwMajorVersion,
                    osvi.dwMinorVersion,
                    osvi.szCSDVersion,
                    osvi.dwBuildNumber & 0xFFFF);
         os += ver;
       }
       break;

      case VER_PLATFORM_WIN32_WINDOWS:

         if ((osvi.dwMajorVersion > 4) || 
            ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 0)))
         {
             os += "Microsoft Windows 98";
         } 
         else
           os += "Microsoft Windows 95";
         break;

      case VER_PLATFORM_WIN32s:
         os += "Microsoft Win32s";
         break;
   }
   return os; 
}

CString GetFreeDiskSpace()
{
  CString temp("");
  
  typedef BOOL (WINAPI *PFN)(LPCSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER);

  PFN pGetDiskFreeSpaceEx;
  pGetDiskFreeSpaceEx = (PFN)GetProcAddress(GetModuleHandle("kernel32.dll"),"GetDiskFreeSpaceExA");

  if (pGetDiskFreeSpaceEx != NULL)
  {
    DWORD DriveMask = GetLogicalDrives();

    int index = 2; // skip drives A: and B:
    
    DWORD letter = (DWORD)ldexp(1.0,index);
    while (DriveMask & letter)
    {
      // these are 64-bit integers
      ULARGE_INTEGER i64FreeBytesToCaller;
      ULARGE_INTEGER i64TotalBytes;
      ULARGE_INTEGER i64FreeBytes;

      CString drive="";
      drive.Format("%c:\\", 'C'+(index-2));

      if (GetDriveType(drive)==DRIVE_FIXED)
      {
        BOOL fResult = pGetDiskFreeSpaceEx(drive,
                               (PULARGE_INTEGER)&i64FreeBytesToCaller,
                               (PULARGE_INTEGER)&i64TotalBytes,
                               (PULARGE_INTEGER)&i64FreeBytes);

        // only warn if hard drive free space under 1MB
        if ((fResult)&&(i64FreeBytesToCaller.QuadPart < 1*(1024*1024)))
        {
          // Process GetDiskFreeSpaceEx results.
          // only care about free bytes available to user (quota)
          CString tmp;
          drive.Format("%c", 'C'+(index-2));
          tmp.Format("Free Disk Space Warning: Drive %s = %I64u bytes free\n", drive, i64FreeBytesToCaller.QuadPart);
          temp += tmp;
        }
      }

      index++;
      letter = (DWORD)ldexp(1.0,index);
    }
  }
  else 
    temp += "Free Disk Space Warning: Pre-Win95 OSR2 OS - no free space info\n";

  return temp;
}

CString GetInternetExplorerVersion()
{
  CString tmp="";

  CRegKey key;
  DWORD rc = key.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Internet Explorer");  
  if (rc==ERROR_SUCCESS)
  {
    char tmpText[512+1];
    DWORD tmpTextLen = 512;
    if (key.QueryValue(tmpText, "Version", &tmpTextLen)==ERROR_SUCCESS)
    {
      tmpText[512]='\0'; 
      tmp=tmpText; 
    }
  }
  return tmp;
}
BOOL CheckDxInstall()
{
  DWORD dxVer;
  CString dxString;
  CString osString;
  CString driveString;
  CString ieString;

  dxVer = GetDXVersion();

  switch (dxVer)
  {
  case 0x0000:
    dxString = "DirectX not installed";
    break;
  case 0x0100:
    dxString = "DirectX 1";
    break;
  case 0x0200:
    dxString = "DirectX 2";
    break;
  case 0x0300:
    dxString = "DirectX 3";
    break;
  case 0x0500:
    dxString = "DirectX 5";
    break;
  case 0x0600:
    dxString = "DirectX 6";
    break;
  case 0x0601:
    dxString = "DirectX 6.1";
    break;
  case 0x0700:
    dxString = "DirectX 7";
    break;
  case 0x0800:
    dxString = "DirectX 8";
    break;
  default:
    dxString = "Unknown DirectX version";
    break;
  }

  osString = GetSystemVersion();
  driveString = GetFreeDiskSpace();
  ieString = GetInternetExplorerVersion();

  WriteDebugString("OS: %s\n", osString);
  WriteDebugString("DX: %s\n", dxString);
  if (driveString.GetLength()>0)
    WriteDebugString("%s", driveString);

  if (ieString.IsEmpty())
  {
    ieString = "IE version 3 or less";
  }

  WriteDebugString("IE: %s\n", ieString);
  
  if (dxVer < 0x0700)
  {
    MsgBoxError("DirectX 7 or higher is required to use Dungeon Craft", "DirectX Version Error");
    return FALSE;
  }

  return TRUE;
}

double GetDesignVersion(const char *fname)
{
  CFile myFile;
  double DesignVer=0.0;
  
  try
  {
    if (!myFile.Open(fname, CFile::modeRead))
    {
      WriteDebugString("Open Error: Unable to determine design version from file %s\n", fname);
      return 0.0;
    }   
    __int64 hdr;
    //CArchive ar(&myFile, CArchive::load);  
    myFile.Read(&hdr, 8);
    WriteDebugString("First 8 bytes of 'game.dat' = %08I64X\n", hdr);
    if (hdr != 0xFABCDEFABCDEFABFI64)
    {
      myFile.SeekToBegin();
    }
    myFile.Read(&DesignVer, sizeof(DesignVer));
    {
      char data[4096];
      int numData;
      gamedatSignature = 67;
      myFile.SeekToBegin();
      while ((numData = myFile.Read(data, 4096)) != 0)
      {
        int i;
        for (i=0; i<numData; i++)
        {
          gamedatSignature = 3*gamedatSignature + data[i];
        };
      };
    }
    myFile.Close();
  }
  catch (...)
  {
    WriteDebugString("Unable to determine design version from file %s\n", fname);
  }

  if (DesignVer <= 0.0)
    WriteDebugString("Error %u trying to determine design version from file %s\n", 
                     GetLastError(), fname);
  return DesignVer;
}

int backup(const CString &filename, CString bak)
{
  if (!FileExists(filename))
    return -1;

  // Rename the file as .BAK, and delete any old .BAK
  int flen;
  char *newName;
  CFile file;
  flen=filename.GetLength()+1;
  newName=(char *)_alloca(flen+50);  // 4 should suffice
  if (newName==NULL) 
  {
    AfxMessageBox("Cannot allocate memory",MB_ICONERROR);
    return -1;
  };
  strcpy(newName,filename);

  char *pExt = strrchr(newName,'.');
  if (pExt != NULL)
  {
    // advance past '.'
    pExt++;
    // replace curr ext with 'bak'
    strcpy(pExt, bak);
  }
  
  if (FileExists(newName))
  {
    try
    {
      // make sure file is not read-only
      SetFileAttributes(newName, FILE_ATTRIBUTE_NORMAL);
      file.Remove(newName);
    }
    catch (CFileException* /*e*/)
    {
      CString msg;
      msg.Format("Cannot remove file:\n\"%s\"",newName);
      AfxMessageBox(msg,MB_ICONERROR);
      return -1;
    };
  };
  try
  {
    // make sure file is not read-only
    SetFileAttributes(filename, FILE_ATTRIBUTE_NORMAL);
    file.Rename(filename,newName);    
  }
  catch (CFileException* /*e*/)
  {
    CString msg;
    msg.Format("Cannot rename file:\n\"%s\"\nas:\n\"newName\"",
                filename, newName);
    AfxMessageBox(msg,MB_ICONERROR);
    return -1;
  };
  return 0;
}
/*
int backup(const CString& filename)
{
  if (!FileExists(filename))
    return -1;

  // Rename the file as .BAK, and delete any old .BAK
  int flen;
  char *newName;
  CFile file;
  //CFileStatus fileStatus;
  flen=filename.GetLength();
  newName=(char *)_alloca(flen+50);  // 4 should suffice
  if (newName==NULL) 
  {
    MsgBoxError("Cannot allocate memory");
    return -1;
  };
  strcpy(newName,filename);

  // this function requires IE4 to be installed
  //PathRenameExtension(newName,".bak");
  char *pExt = strrchr(newName,'.');
  if (pExt != NULL)
  {
    // advance past '.'
    pExt++;
    // replace curr ext with 'bak'
    strcpy(pExt,"bak");
  }
  
  //if (file.GetStatus(newName,fileStatus)) // MFC bug only returns low byte of file attributes
  if (FileExists(newName))
  {
    try
    {
      // make sure file is not read-only
      SetFileAttributes(newName, FILE_ATTRIBUTE_NORMAL);
      file.Remove(newName);
    }
    catch (CFileException* )
    {
      CString msg;
      msg.Format("Cannot remove file:\n\"%s\"",newName);
      MsgBoxError(msg);
      return -1;
    };
  };
  try
  {
    // make sure file is not read-only
    SetFileAttributes(filename, FILE_ATTRIBUTE_NORMAL);
    file.Rename(filename,newName);    
  }
  catch (CFileException* )
  {
    CString msg;
    msg.Format("Cannot rename file:\n\"%s\"\nas:\n\"newName\"",
                filename, newName);
    MsgBoxError(msg);
    return -1;
  };
  return 0;
}
*/
BOOL SaveGameExists(int num)
{
  //char fullPath[_MAX_PATH+1];
  char numText[2];
  wsprintf(numText, "%c", 'A' + num);

  CString fullPath;
  fullPath = GetSavePath();

  //sprintf(fullPath, "%sSave", global_designDir);    
  //strcat(fullPath, numText);
  //strcat(fullPath, ".pty");
  fullPath += "Save";
  fullPath += numText;
  fullPath += ".pty";
  return (FileExists(fullPath));
}

#if (defined UAFEDITOR)
/*
void OpenTextFileForEdit(const char *path)
{
  SHELLEXECUTEINFO ShExecInfo;

  ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
  ShExecInfo.fMask = NULL;
  ShExecInfo.hwnd = NULL;
  ShExecInfo.lpVerb = NULL;
  ShExecInfo.lpFile = path;
  ShExecInfo.lpParameters = NULL;
  ShExecInfo.lpDirectory = NULL;
  ShExecInfo.nShow = SW_SHOWNORMAL;
  ShExecInfo.hInstApp = NULL;

  ShellExecuteEx(&ShExecInfo);    
}
*/
#endif

static char rootName[_MAX_PATH+1];
BOOL FindImageWithValidExtInFolder(CString &filename, CString &folder, BOOL recurse)
{

  strcpy(rootName, filename);
  char *index = strrchr(rootName, '.');
  if (index != NULL)
    *index = '\0'; // knock off ext
  
  CString temp("");
  CString path(folder);

  temp.Format("%s.png", rootName);
  if (SearchForFileInPath(temp, "png", path, recurse))
  {
    filename = temp;
    return TRUE;
  }

  temp.Format("%s.pcx", rootName);
  if (SearchForFileInPath(temp, "PCX", path, recurse))
  {
    filename = temp;
    return TRUE;
  }

  temp.Format("%s.bmp", rootName);
  if (SearchForFileInPath(temp, "BMP", path, recurse))
  {
    filename = temp;
    return TRUE;
  }

  temp.Format("%s.jpg", rootName);
  if (SearchForFileInPath(temp, "jpg", path, recurse))
  {
    filename = temp;
    return TRUE;
  }

  temp.Format("%s.tga", rootName);
  if (SearchForFileInPath(temp, "tga", path, recurse))
  {
    filename = temp;
    return TRUE;
  }

  return FALSE;
}



BOOL FindImageWithValidExt(CString &filename, const CString& directory)
{
  static char root[_MAX_PATH+1];
  {
    CString fullName;
    fullName = directory + filename;
    if (FileExists(fullName)) return TRUE;
#ifdef UAFEngine
    return FALSE;
#endif
  };
#ifndef UAFEngine
#pragma warning (suppress:4996)
  strcpy(root, filename);
  char *index = strrchr(root, '.');
  if (index != NULL)
    *index = '\0'; // knock off ext
  
  CString temp("");
  CString path("");
  CString path2("");
  BOOL recurse;

#ifdef UAFEDITOR
  path = directory;
  recurse = FALSE;
#else
  path = global_designDir; // design folder
  path2 = GetSavePath();
  recurse = FALSE;
#endif

  if (FindImageWithValidExtInFolder(filename, path, recurse))
    return TRUE;

  if (path2.IsEmpty()) return FALSE;

  if (FindImageWithValidExtInFolder(filename, path2, recurse))
    return TRUE;

  return FALSE;
#endif // ~UAFEngine
}

  
  
  
  
BOOL LoadAreaViewArt(const char *file, long &surf, BOOL &UseAVZones)
{
  // if already loaded then return
  if (GraphicsMgr.ValidSurface(surf))
    return TRUE;

  UseOldAVStyle=FALSE;
  UseAVZones=FALSE;
#ifdef UAFEngine
  surf = GraphicsMgr.AddFileSurface(file, TransBufferDib, rte.AreaViewArtDir());
#else
  surf = GraphicsMgr.AddFileSurface(file, TransBufferDib, rte.AreaViewArtDir(), ede.TemplateAreaArtDir());
#endif

  if (!GraphicsMgr.ValidSurface(surf))
  {
    WriteDebugString("Failed to LoadAreaViewArt(%s)\n", file);
    return FALSE;
  }

  // Older versions of this file didn't use transparency and will have a color
  // key of white. The newer version has a color key of black.
  COLORREF color;
  if (!GraphicsMgr.GetColorKey(surf, color))
  {
    WriteDebugString("Failed to GetColorKey() for area view art %s\n", file);
    return FALSE;
  }

  if (color != RGB(0,0,0))
  {
    UseOldAVStyle=TRUE;
    return GraphicsMgr.ChangeSurfaceType(surf, BufferDib);
  }

  // older versions of this file don't have unique horz/vert wall sections
  // but they still use the transparent color
  if (!GraphicsMgr.GetColorAt(surf, 0,11, color))
  {
    WriteDebugString("Failed to GetColorAt() for area view art %s\n", file);
    return FALSE;
  }

  if (color != RGB(0,0,0))
    UseOldAVStyle=TRUE;

  RECT tmpRect;
  GraphicsMgr.GetSurfaceRect(surf, &tmpRect);
  // Make sure area view art is big enough to contain
  // the zone squares before enabling their usage.
  // The old style file was 92x80 pixels
  if (   ((tmpRect.right-tmpRect.left) >= 92)
      && ((tmpRect.bottom-tmpRect.top) > 80))
    UseAVZones = TRUE;

  return TRUE;
}

int MsgBoxYesNo(const char *pMsg, const char *pTitle)
{
  CString title;
  if (pTitle != NULL)
    title = pTitle;
  else
    title = "Answer Yes or No";
  DWORD flags = MB_YESNO | MB_ICONWARNING | MB_TOPMOST;
  HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
  if (hWnd == NULL)
    flags |= MB_TASKMODAL;

  return (::MessageBox(hWnd, pMsg, title, flags));
}


CMap<CString, LPCTSTR, int, int> errorMessages;
// Keeps track of what error messages we have
// issued during this execution of UAFwin. 

void MsgBoxError(const char *pMsg, const char *pTitle, int maxDisplay)
{
  CString temp;
  CString title;
  int value;
  if (pTitle != NULL)
    title = pTitle;
  else
    title = "Error";

  if (pTitle == NULL)
    temp.Format("(MB) %s\n", pMsg);
  else
    temp.Format("(MB) %s: %s\n", pTitle, pMsg);

  //temp = temp.Left(475);
  WriteDebugString("%s\n",temp);
  value = 0;
  if (errorMessages.Lookup(pMsg, value) && (value>=maxDisplay))
  {
    return;
  };
  errorMessages.SetAt(pMsg, ++value);

  DWORD flags = MB_ICONERROR | MB_OK | MB_TOPMOST;
  HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
  if (hWnd == NULL)
    flags |= MB_TASKMODAL;

  if (value >= maxDisplay)
  {
    temp.Format("%s\nThis message will not be displayed again", pMsg);
  }
  else
  {
    temp = pMsg;
  };
  ::MessageBox(hWnd, temp, title, flags);
}

void MsgBoxInfo(const char *pMsg, const char *pTitle)
{
  CString title;
  if (pTitle != NULL)
    title = pTitle;
  else
    title = "Information";

  DWORD flags = MB_ICONINFORMATION | MB_OK | MB_TOPMOST | MB_APPLMODAL;
  HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
  if (hWnd == NULL)
    flags |= MB_TASKMODAL;

  ::MessageBox(hWnd, pMsg, title, flags);
}

BOOL EventChainError(const char *pMsg, bool offerToFix)
{

  //Returns true if user wants us to try and fix the error
  CString message;
  int result;
  message = "msfID: ";
  message += pMsg;
  message += "\n\nAn error has occured that we have ignored for ten years."
             "  It is time we fixed it.  It was probably caused by deleting "
             "an event.  Please report the MsgID and anything you can remember "
             "about what kind of event, chaining, etc. and what you did.  "
             "\n\nThanks - Paul Stevens;  prsteven@facstaff.wisc.edu";
  DWORD flags = MB_ICONINFORMATION | MB_OK | MB_TOPMOST | MB_APPLMODAL;
  if (offerToFix)
  {
    message += "\n\nShould we try to fix this?";
    flags = MB_ICONINFORMATION | MB_YESNO | MB_TOPMOST | MB_APPLMODAL;
  };
  HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
  if (hWnd == NULL)
    flags |= MB_TASKMODAL;

  result = ::MessageBox(hWnd, 
                         message, 
                        "Event chaining error", 
                        flags);
  if (offerToFix && (result == IDYES)) return true;
  return false;
}


// make sure you declare the lvalue properly when using this
// function.
//
// CHARACTER temp = GetPartyMember() <--- BAD!!!, destructor will ruin source char
//
// CHARACTER &temp = GetPartyMember() <-- correct way
//
CHARACTER &GetPartyMember(int index, const char *msg)
{
  return party.GetCharacter(index, msg);
}

extern CHARACTER FakeCharacter;

extern "C" BOOL ActorType::IsValidSrc() const
{
#ifdef UAFEngine
  if (IsCreatedCharacterSrc()) return pScriptContext->GetCreatedCharacterContext(NULL) != &FakeCharacter;
  if (IsFakeCombatantSrc()) return Instance >= 0xfffffffe;
  if (IsFakeCharacterSrc()) return TRUE;
  if (IsCombatSrc()) return ((Instance>=0)&&(Instance<(unsigned long)combatData.NumCombatants()));
  if (IsPartySrc()) 
  {
    if (instanceType == InstanceType_UniquePartyID)
    {
      int i;
      for(i=0;i<party.numCharacters; i++)
      {
        if (party.characters[i].uniquePartyID == Instance) return TRUE;
      };
      return FALSE;
    };
    return ((Instance>=0)&&(Instance<party.numCharacters));
  };
  //if (IsNPCSrc()) return (globalData.charData.HaveKey(Instance));
  if (IsNPCSrc()) return (globalData.charData.HaveNPC(m_characterID));
  //GLOBAL_ITEM_ID giID;
  //giID.UniqueIdentifier(Instance);
  if (IsItemSrc()) 
  {
    //return (itemData.HaveItem(giID));
    return itemData.HaveItem(m_itemID);
  };
  if (IsMonsterSrc()) 
  {
    //return (monsterData.HaveMonster(Instance));
    return (monsterData.HaveMonster(m_monsterID));
  };
  die(0xab51d);
  return FALSE;
#else
  return TRUE;
#endif
}

CString GetActorDebugText(ActorType &actor)
{
  CString tmp;
  tmp.Format("Actor Data\nEnemyAlly %u\nRace %s\nClass %s\nFlags %u\nInstance %u\nInstance Type %u\nLevel %u",
             actor.EnemyAlly,
             //actor.Race,
             actor.m_raceID.UniqueName(),
             //actor.Class,
             actor.m_classID.UniqueName(),
             actor.Flags,
             actor.Instance,
             actor.instanceType,
             actor.Level);
#ifdef UAFEngine
  if (IsCombatActive())
  {
    CString temp;
    temp.Format("\nNumCombants %i", GetNumCombatants());
    tmp += temp;
  }
#endif

  CString temp;
  temp.Format("\nNumPartyMembers %i\n", GetPartySize(party));
  tmp += temp;    

  return tmp;
}

#ifdef UAFEngine

genderType GetCombatantGender(int memberNumber)
{
  if ((memberNumber < 0) || (memberNumber >= combatData.NumCombatants()))
    return Male;
  return (combatData.GetCombatant(memberNumber)->GetGender());
}

void SetCombatantGender(int memberNumber, const CString& gender)
{
  if ((memberNumber < 0) || (memberNumber >= combatData.NumCombatants()))return;
  combatData.GetCombatant(memberNumber)->SetGender(gender);
}

void IncNumFriendFlee()   { combatData.m_iNumFrndFlee++;       }
int  GetNumFriendFlee()   { return combatData.m_iNumFrndFlee;  }
void IncNumFriendSlain()  { combatData.m_iNumFrndSlain++;      }
int  GetNumFriendSlain()  { return combatData.m_iNumFrndSlain; }
void IncNumMonsterFlee()  { combatData.m_iNumMonFlee++;        }
int  GetNumMonsterFlee()  { return combatData.m_iNumMonFlee;   }
void IncNumMonsterSlain() { combatData.m_iNumMonSlain++;       }
int  GetNumMonsterSlain() { return combatData.m_iNumMonSlain;  }
int  GetNumCombatants()   { return combatData.NumCombatants(); }

void GetNumCombatants(int* pNumPty, int* pNumMonsters) 
{ 
  combatData.getNumCombatants(pNumPty, pNumMonsters); 
}

BOOL IsCurrentCombatant(int dude) 
{ 
  return (combatData.QComb.Top()==dude);
}

BOOL GetConfigMonsterNoFlee()      { return combatData.m_bMonsterNoFlee;     }
BOOL GetConfigPartyAlwaysMiss()    { return combatData.m_bPartyAlwaysMiss;   }
BOOL GetConfigPartyAlwaysHit()     { return combatData.m_bPartyAlwaysHit;    }
BOOL GetConfigMonsterAlwaysMiss()  { return combatData.m_bMonsterAlwaysMiss; }
BOOL GetConfigMonsterAlwaysHit()   { return combatData.m_bMonsterAlwaysHit;  }
BOOL GetConfigNoMagic()            { return combatData.m_bNoMagic;           }
BOOL GetConfigMonsterNoCast()      { return combatData.m_bMonsterNoCast;     }
BOOL GetConfigMonsterNoDeath()     { return combatData.m_bMonsterNoDeath;    }
BOOL GetConfigPartyNeverDies()     { return combatData.m_bPartyNeverDies;    }
BOOL GetConfigMonsterPlyrControl() { return combatData.m_bMonsterPlyrControl;}
BOOL GetConfigMonsterNoMove()      { return combatData.m_bMonsterNoMove;     }
BOOL GetConfigPartyNoExperience()  { return combatData.m_bPartyNoExperience; }

void PlayCombatMove()           { combatData.PlayCombatMove();           }
void PlayCombatTurnUndead()     { combatData.PlayCombatTurnUndead();     }
void PlayCombatDeath()          { combatData.PlayCombatDeath();          }
void PlaceCursorOnCurrentDude() { combatData.PlaceCursorOnCurrentDude(); }
int  GetCurrentRound()          { return combatData.m_iCurrRound;        }
RECT &GetCombatRect()           { return combatData.m_combatRect;        }
RECT &GetFullRect()             { return combatData.m_fullRect;          }

QueuedCombatantData &GetQueuedCombatants() { return combatData.QComb; }

void GetStartTerrain(int* px, int* py)
{
  *px = combatData.m_iStartTerrainX;
  *py = combatData.m_iStartTerrainY;
}

/*
COMBATANT &GetCombatant(int index)
{
  return combatData.getCombatant(index);
}
*/
COMBATANT *GetCombatantPtr(int index)
{
  return (combatData.getCombatantPtr(index));
}

int InvokeSpellOnCombatant(int caster, 
                           int target, 
                           int activespellkey, 
                           ToHitComputation *pToHitComputation)
{
  ASSERT( IsCombatActive() == TRUE );
  return (combatData.InvokePendingSpellOnCombatant(caster,
                                                   target,
                                                   activespellkey, 
                                                   false,
                                                   NULL,
                                                   NULL,
                                                   SPELL_ID(),
                                                   NULL,
                                                   pToHitComputation));
}

void ActorType::Serialize(CAR &ar, double ver)
{
  if (ar.IsStoring())
  {
    //ar << Class;
    ar << m_classID;
    ar << EnemyAlly;
    ar << Flags;
    ar << Instance;
    //ar << Race;
    ar << m_raceID;
    ar << Level;
    ar << m_itemID;
  }
  else
  {
    //ar >> Class;
    ar >> m_classID;
    ar >> EnemyAlly;
    ar >> Flags;
    ar >> Instance;
    //ar >> Race;
    ar >> m_raceID;
    if (ver >= _VERSION_06991_)
      ar >> Level;
    ar >> m_itemID;
  }
}
void ActorType::Serialize(CArchive &ar, double ver)
{
  if (ar.IsStoring())
  {
    //ar << Class;
    ar << m_classID;
    ar << EnemyAlly;
    ar << Flags;
    ar << Instance;
    //ar << Race;
    ar << m_raceID;
    ar << Level;
    ar << m_itemID;
  }
  else
  {
    //ar >> Class;
    ar >> m_classID;
    ar >> EnemyAlly;
    ar >> Flags;
    ar >> Instance;
    //ar >> Race;
    ar >> m_raceID;
    if (ver >= _VERSION_06991_)
      ar >> Level;
    ar >> m_itemID;
  }
}

BOOL NeedSpellTargeting(SPELL_DATA *pData)
{
  ASSERT( pData != NULL);
  if (pData==NULL) return FALSE;

  // Spell targeting when not in combat will only
  // involve party members.
  //
  // spellTargetingType { Self, SelectedByCount, WholeParty, TouchedTargets, AreaCircle, SelectByHitDice };
  //
  // Targeting (non-combat):
  // 
  //   Self: affects caster
  //   WholeParty,AreaCircle: affects whole party 
  //   SelectedByCount, TouchedTargets: affects x number of targeted party members
  //   SelectByHitDice: affects targeted party members until HD limit reached
  //
  //   Self, WholeParty, and AreaCircle don't need target selection, cast immediately
  //   SelectedByCount,TouchedTargets,SelectByHitDice require individual target selection
  //
  // Targeting (combat):
  //
  //   Self: affects caster
  //   WholeParty: affects whole party 
  //   AreaCircle: select target map square, all targets within range are affected
  //   SelectedByCount: affects x number of targets
  //   TouchedTargets: affects 1 target within range 1
  //   SelectByHitDice: select targets until HD limit reached
  //
  //   Self and WholeParty don't need target selection, cast immediately
  //   AreaCircle, SelectedByCount,TouchedTargets,SelectByHitDice require individual target selection

  if (IsCombatActive())
  {
    switch (pData->Targeting)
    {
    case Self:              return FALSE;
    case SelectedByCount:   return TRUE;
    case WholeParty:        return FALSE;
    case TouchedTargets:    return TRUE;
    case AreaCircle:        return TRUE;
    case AreaLinePickStart: return TRUE;
    case AreaLinePickEnd:   return TRUE;
    case AreaSquare:        return TRUE;
    case AreaCone:          return TRUE;
    case SelectByHitDice:   return TRUE;
    }
  }
  else // non-combat
  {
    // if FALSE, assume whole party affected
    switch (pData->Targeting)
    {
    case Self:              return FALSE;
    case SelectedByCount:   return TRUE;
    case WholeParty:        return FALSE;
    case TouchedTargets:    return TRUE;
    case AreaCircle:        return FALSE;
    case AreaLinePickStart: return FALSE;
    case AreaLinePickEnd:   return FALSE;
    case AreaSquare:        return FALSE;
    case AreaCone:          return FALSE;
    case SelectByHitDice:   return TRUE;
    }
  }
  die(0xab51e);
  return FALSE;
}

void SpellActivate(const PENDING_SPELL &data, 
                   bool abortIfSetPartyXY, 
                   ToHitComputation *pToHitComputation)
{
  ASSERT(data.caster.IsValidSrc());
  //int src = data.caster.GetInstance();
  if (data.caster.IsCombatSrc())
  {
    switch (data.caster.GetSrc())
    {
    case FLAG_PARTY_MEMBER:
    case FLAG_NPC:
    case FLAG_MONSTER:
      combatData.GetCombatant(data.caster.GetInstance())->SpellActivate(data);
      break;
    case FLAG_ITEM:
      itemData.SpellActivate(data.caster.m_itemID, data);
      break;
    }
  }
  else
  {
    switch (data.caster.GetSrc())
    {
    case FLAG_PARTY_MEMBER:
      switch (data.caster.instanceType)
      {
      case InstanceType_UniquePartyID:
        {
          setPartyXY_x = -1;
          party.FindCharacter(data.caster.Instance)->SpellActivate(data, NULL, abortIfSetPartyXY,pToHitComputation);
          if (setPartyXY_x >= 0)
          {
            break;
          };
        };
        break;
      case InstanceType_OrigIndex: party.characters[data.caster.GetInstance()].SpellActivate(data, NULL, false, pToHitComputation);
        break;
      default:
        die(0xc329c0);
      };
      break;
    case FLAG_NPC:
      globalData.charData.SpellActivate(data.caster.GetInstance(), data, pToHitComputation);
      break;
    case FLAG_BISHOP:
      globalData.charData.SpellActivate(data.caster.GetInstance(), data, pToHitComputation);
      break;
    case FLAG_ITEM:
      itemData.SpellActivate(data.caster.m_itemID, data);
      break;
    case FLAG_MONSTER:
      die(0xab51f);
      //monsterData.SpellActivate(src, data);
      break;
    }
  }
}

// prs 20191220  COLORREF ASCII_TO_COLOR(const CString& asc, COLORREF default)
FONT_COLOR_NUM ASCII_TO_COLOR(const CString& asc, FONT_COLOR_NUM default)
{
  if (asc.CompareNoCase("White") == 0) return whiteColor;
  if (asc.CompareNoCase("Red") == 0) return redColor;
  if (asc.CompareNoCase("Yellow") == 0) return yellowColor;
  if (asc.CompareNoCase("Orange") == 0) return orangeColor;
  if (asc.CompareNoCase("Green") == 0) return greenColor;
  if (asc.CompareNoCase("Black") == 0) return blackColor;
  if (asc.CompareNoCase("Cyan") == 0) return cyanColor;
  if (asc.CompareNoCase("Magenta") == 0) return magentaColor;
  if (asc.CompareNoCase("Silver") == 0) return silverColor;
  if (asc.CompareNoCase("Blue") == 0) return blueColor;
  return default;
}

void DisplayText(int x, int y,
                 int fontNumber,
                 LPCSTR ptext,
                 FONT_COLOR_NUM colorNum,
                 BOOL customColor,
                 BOOL highlight, 
                 BOOL slowText, 
                 BOOL UseBackBuffer)
{   
   long int SrcFont;

   if (highlight)
   {
     SrcFont = globalData.HighlightFont;
   }
   else
   {
     switch (colorNum)
     {
     case whiteColor:
     case orangeColor:
     case yellowColor:
     case cyanColor:
     case greenColor:
     case redColor:
     case blueColor:
     case blackColor:
     case magentaColor:
     case silverColor:
// prs 20191227       SrcFont = globalData.availableFonts[0].fontNum[colorNum];
       SrcFont = globalData.fontLib.GetFont(0, colorNum, customColor);
       break;
     case combatBlueColor:
// prs 20191227       SrcFont = globalData.availableFonts[0].fontNum[blueColor];
       SrcFont = globalData.fontLib.GetFont(0, blueColor, customColor);
       break;
     case combatGreenColor:
// prs 20191227       SrcFont = globalData.availableFonts[0].fontNum[greenColor];
       SrcFont = globalData.fontLib.GetFont(0, greenColor, customColor);
       break;
     case combatRedColor:
// prs 20191227       SrcFont = globalData.availableFonts[0].fontNum[redColor];
       SrcFont = globalData.fontLib.GetFont(0, redColor, customColor);
       break;
     case combatOrangeColor:
// prs 20191227       SrcFont = globalData.availableFonts[0].fontNum[orangeColor];
       SrcFont = globalData.fontLib.GetFont(0, orangeColor, customColor);
       break;
     case combatYellowColor:
// prs 20191227       SrcFont = globalData.availableFonts[0].fontNum[yellowColor];
       SrcFont = globalData.fontLib.GetFont(0, yellowColor, customColor);
       break;
       /*
     if (color == White)
      SrcFont = globalData.WhiteFont;
     else if ((color == CombatYellow) || (color == Yellow))
       SrcFont = globalData.YellowFont;
     else if ((color == CombatOrange) || (color == Orange))
       SrcFont = globalData.OrangeFont;
     else if ((color == CombatRed) || (color == Red))
       SrcFont = globalData.RedFont;
     else if ((color == CombatGreen) || (color == Green))
       SrcFont = globalData.GreenFont;
     else if ((color == CombatBlue) || (color == Blue))
       SrcFont = globalData.BlueFont;
       */
     default:
       if (colorNum == customColorNum)
         SrcFont = globalData.Custom1Font;
       else if (colorNum == RGB(255, 128, 0))
         SrcFont = globalData.MagicAdjFont;
       else
         SrcFont = -1;
       break;
     };
   }

   if (SrcFont == -1)
   {
     if (colorNum != BACKGROUND_FILL_COLOR_NUM)
       WriteDebugString("Bogus handle to requested font in DisplayText()\n");
     return;
   }
   //slowText = 0;
   //UseBackBuffer = 1;
   DETAILTRACE(0x4c1124,0);
   GraphicsMgr.DrawFont(x,y, 
                        fontNumber, colorNum, 
                        ptext, 
                        SrcFont, 
                        slowText,
                        UseBackBuffer);
   DETAILTRACE(0x4c1125,0);
}

// prs 20191220  void displayColoredText(int x, int y, LPCSTR ptext, COLORREF color, BOOL highlight, BOOL slowText, BOOL UseBackBuffer)
void DisplayColoredText(int x, int y, 
                        int fontNumber, 
                        LPCSTR ptext, 
                        FONT_COLOR_NUM colorNum, BOOL customColor,
                        BOOL highlight, BOOL slowText, BOOL UseBackBuffer)
{
  BOOL saveFontColorTag;
  saveFontColorTag = GraphicsMgr.GetEnableFontColorTags();
  GraphicsMgr.EnableFontColorTags(TRUE);
  /* 20200208  PRS
  GraphicsMgr.StoreFontColorTags();
  GraphicsMgr.ResetFontColorTags();
  */
// prs 20191220   displayText(x, y, ptext, color, highlight, slowText,UseBackBuffer);
  DisplayText(x, y, 
              fontNumber, 
              ptext, 
              colorNum, customColor,
              highlight, slowText, UseBackBuffer);
  /* 20200208  PRS
  GraphicsMgr.EnableFontColorTags(saveFontColorTag);
  GraphicsMgr.RestoreFontColorTags();
  */
  GraphicsMgr.EnableFontColorTags(saveFontColorTag);
}

void EnableUserInput(BOOL enable)
{
  ProcInput.AllowInput(enable);
}

void DoTaskStackTrace()
{
  ProcInput.StackTrace();
}

eventType GetCurrEventType()
{
  return (ProcInput.GetCurrEventType());
}

#endif // ifdef UAFEngine

BOOL IsCombatActive()
{
#ifdef UAFEDITOR
  return FALSE;
#else
  return combatData.m_bCombatActive;
#endif
}


//extern "C"
//{
void ActorType::Clear()
{ 
  //Race = 0;
  m_raceID.Clear();
  //Class = 0;
  m_classID.Clear();
  Flags = 0;
  Instance = 0;
  Level = 0;
  EnemyAlly=EA_ALLY;
  instanceType = InstanceType_Unknown;
  m_itemID.Clear();
  m_canFinishCasting = false;
}

int ActorType::DetermineLevel(const SCHOOL_ID& schoolID, int spellLevel)
{
  // Search the baseclasses to find the one with
  // the highest level that can cast spells in this schoolID.
  CHARACTER *pChar;
  const SCHOOL_ABILITY *pSchoolAbility;
  pChar = GetCurrentlyActiveContext(this, "ActorType::DetermineLevel");
  pSchoolAbility = pChar->spellAbility.PeekSchoolAbility(schoolID);
  Level = 0;
  if (pSchoolAbility != NULL) 
  {
    Level = pSchoolAbility->baseclassLevel[spellLevel-1];
  }
  return Level;
}



BOOL ActorType::operator==(const ActorType &src) const
{
  if (EnemyAlly != src.EnemyAlly) return FALSE;
  //if (Race != src.Race) return FALSE;
  if (m_raceID != src.m_raceID) return FALSE;
  //if (Class != src.Class) return FALSE;
  if (m_classID != src.m_classID) return FALSE;
  if (Flags != src.Flags) return FALSE;
  if (Instance != src.Instance) return FALSE;
  if (Level != src.Level) return FALSE;
  if (m_itemID != src.m_itemID) return FALSE;
  return TRUE;
}

void ActorType::FromString(const CString& str)
{
  Flags = 0;
  if (str.GetLength() == 0)
  {
    *this = NULL_ACTOR;
  }
  else
  {
    if (str.GetLength() >= sizeof(ActorType))
    {
      *this = *((ActorType *)(LPCSTR)str);
    };
  };
  if (!IsValidSrc())
  {
    die(0xab520); 
    CString errmsg;
    errmsg.Format("Invalid source string \'%s\' in Actor::FromString()\n", str);
    errmsg += GetActorDebugText(*this);
    WriteDebugString(errmsg);
    //WriteStackTrace();
    *this = NULL_ACTOR; 
  }
  return;// actor;
}

BOOL ActorType::operator!=(const ActorType &src) const { return !(*this==src); }
void ActorType::XYSet(unsigned short x, unsigned short y) { Instance=((x<<16)|y); }
void ActorType::XYGet(unsigned short &x, unsigned short &y) { x=Instance>>16;y=Instance&0x00FF; }

void ActorType::SetCombatPartySrc(BOOL friendly) { EnemyAlly=(friendly?EA_ALLY:EA_ENEMY); Flags=FLAG_COMBAT|FLAG_PARTY_MEMBER; }
void ActorType::SetCombatNPCSrc(BOOL friendly) { EnemyAlly=(friendly?EA_ALLY:EA_ENEMY); Flags=FLAG_COMBAT|FLAG_NPC; }
void ActorType::SetCombatMonsterSrc(BOOL friendly) { EnemyAlly=(friendly?EA_ALLY:EA_ENEMY); Flags=FLAG_COMBAT|FLAG_MONSTER; }

void ActorType::SetPartySrc(BOOL friendly) { EnemyAlly=(friendly?EA_ALLY:EA_ENEMY); Flags=FLAG_NONCOMBAT|FLAG_PARTY_MEMBER; }
void ActorType::SetItemSrc() { EnemyAlly=FLAG_ITEM; Flags=FLAG_NONCOMBAT|FLAG_ITEM; }
void ActorType::SetNPCSrc(BOOL friendly) { EnemyAlly=(friendly?EA_ALLY:EA_ENEMY); Flags=FLAG_NONCOMBAT|FLAG_NPC; }
void ActorType::SetCreatedCharacterSrc(BOOL friendly) { EnemyAlly=(friendly?EA_ALLY:EA_ENEMY); Flags=FLAG_NONCOMBAT|FLAG_CREATED_CHARACTER; }
void ActorType::SetBishopSrc(BOOL friendly) { EnemyAlly=(friendly?EA_ALLY:EA_ENEMY); Flags=FLAG_NONCOMBAT|FLAG_BISHOP|FLAG_FAKE_CHARACTER; }
void ActorType::SetFakeCharacterSrc(BOOL friendly) { EnemyAlly=(friendly?EA_ALLY:EA_ENEMY); Flags=FLAG_NONCOMBAT|FLAG_FAKE_CHARACTER; }
void ActorType::SetMonsterSrc(BOOL friendly) { EnemyAlly=(friendly?EA_ALLY:EA_ENEMY); Flags=FLAG_NONCOMBAT|FLAG_MONSTER; }
void ActorType::SetFakeCombatantSrc(BOOL friendly) { Flags=FLAG_FAKE_COMBATANT|FLAG_COMBAT; }

BOOL ActorType::IsCombatSrc() const { return (Flags&FLAG_COMBAT); }
BOOL ActorType::IsPartySrc() const { return ((Flags&FLAG_PARTY_MEMBER)&&(!IsCombatSrc())); }
BOOL ActorType::IsNPCSrc() const { return ((Flags&FLAG_NPC)&&(!IsCombatSrc())); }
BOOL ActorType::IsMonsterSrc() const { return ((Flags&FLAG_MONSTER)&&(!IsCombatSrc())); }
BOOL ActorType::IsItemSrc() const { return (Flags&FLAG_ITEM); }
BOOL ActorType::IsFakeCombatantSrc() const {return (Flags&FLAG_FAKE_COMBATANT);};
BOOL ActorType::IsFakeCharacterSrc() const {return (Flags&FLAG_FAKE_CHARACTER);};
BOOL ActorType::IsBishopSrc() const {return Flags&FLAG_BISHOP;};
BOOL ActorType::IsCreatedCharacterSrc() const {return Flags&FLAG_CREATED_CHARACTER;};

unsigned long ActorType::GetInstance() const { return Instance; }
unsigned long ActorType::GetSrc() const
{
  if (Flags&FLAG_PARTY_MEMBER) return FLAG_PARTY_MEMBER;
  if (Flags&FLAG_NPC) return FLAG_NPC;
  if (Flags&FLAG_MONSTER) return FLAG_MONSTER;
  if (Flags&FLAG_ITEM) return FLAG_ITEM;
  if (Flags&FLAG_BISHOP) return FLAG_BISHOP;
  die(0xab521);
  return 0;
}
//} // extern C

#ifdef SpellInitiationScript
void ActorType::ExitCombat(void)
{
  if ((Flags & FLAG_COMBAT) == 0) return;
  {
    switch(instanceType)
    {
    case InstanceType_CombatantIndex:
    {
      COMBATANT *pCombatant;
      CHARACTER *pCharacter;
      pCombatant = combatData.GetCombatant(Instance);
      pCharacter = pCombatant->m_pCharacter;
      if (pCombatant == NULL)
      {
        die(0x9070);
      };
      if (!pCharacter->characterID.IsEmpty())
      {
        // We can replace a PC combatant with a PC non-combatant rather easily
        Flags &= ~(FLAG_COMBAT | FLAG_FAKE_COMBATANT | FLAG_COMBATANT);
        Flags |= FLAG_NONCOMBAT;
        instanceType = InstanceType_UniquePartyID;
        Instance = pCharacter->uniquePartyID;
      }
      else
      {
        // A monster will not survive when the combat is closed.
        // We will replace him with a NULL character
        Clear();
        instanceType = InstanceType_NullActor;
      };
      break;
    };
    default: die(0xdd6a);
    };
  };
}
#endif

unsigned long HashString(const char *s, const unsigned long hashsize)
{
 const char *p;
 unsigned long h=0, g;
 if (s==NULL)
 {
   die(0xab522);
   return 0;
 }
 for (p=s; *p!=0; p++)
 {
   h = (h<<4) + (*p);
   if ((g = (h & 0xf0000000)) != 0)
   {
     h = h ^ (g>>24);
     h = h ^ g;
   }
 } 
 return (h % hashsize);
}

#if _MSC_VER == 1100     // Need CRegKey implementation explicitly in VC5
/////////////////////////////////////////////////////////////////////////////
// CRegKey
// 
// Copied from "c:\Program Files\DevStudio\VC\atl\include\atlimpl.cpp"  
//
//   PRS


LONG CRegKey::Close()
{
	LONG lRes = ERROR_SUCCESS;
	if (m_hKey != NULL)
	{
		lRes = RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
	return lRes;
}

LONG CRegKey::Create(HKEY hKeyParent, LPCTSTR lpszKeyName,
	LPTSTR lpszClass, DWORD dwOptions, REGSAM samDesired,
	LPSECURITY_ATTRIBUTES lpSecAttr, LPDWORD lpdwDisposition)
{
	_ASSERTE(hKeyParent != NULL);
	DWORD dw;
	HKEY hKey = NULL;
	LONG lRes = RegCreateKeyEx(hKeyParent, lpszKeyName, 0,
		lpszClass, dwOptions, samDesired, lpSecAttr, &hKey, &dw);
	if (lpdwDisposition != NULL)
		*lpdwDisposition = dw;
	if (lRes == ERROR_SUCCESS)
	{
		lRes = Close();
		m_hKey = hKey;
	}
	return lRes;
}

LONG CRegKey::Open(HKEY hKeyParent, LPCTSTR lpszKeyName, REGSAM samDesired)
{
	_ASSERTE(hKeyParent != NULL);
	HKEY hKey = NULL;
	LONG lRes = RegOpenKeyEx(hKeyParent, lpszKeyName, 0, samDesired, &hKey);
	if (lRes == ERROR_SUCCESS)
	{
		lRes = Close();
		_ASSERTE(lRes == ERROR_SUCCESS);
		m_hKey = hKey;
	}
	return lRes;
}

LONG CRegKey::QueryValue(DWORD& dwValue, LPCTSTR lpszValueName)
{
	DWORD dwType = NULL;
	DWORD dwCount = sizeof(DWORD);
	LONG lRes = RegQueryValueEx(m_hKey, (LPTSTR)lpszValueName, NULL, &dwType,
		(LPBYTE)&dwValue, &dwCount);
	_ASSERTE((lRes!=ERROR_SUCCESS) || (dwType == REG_DWORD));
	_ASSERTE((lRes!=ERROR_SUCCESS) || (dwCount == sizeof(DWORD)));
	return lRes;
}

LONG CRegKey::QueryValue(LPTSTR szValue, LPCTSTR lpszValueName, DWORD* pdwCount)
{
	_ASSERTE(pdwCount != NULL);
	DWORD dwType = NULL;
	LONG lRes = RegQueryValueEx(m_hKey, (LPTSTR)lpszValueName, NULL, &dwType,
		(LPBYTE)szValue, pdwCount);
	_ASSERTE((lRes!=ERROR_SUCCESS) || (dwType == REG_SZ) ||
			 (dwType == REG_MULTI_SZ) || (dwType == REG_EXPAND_SZ));
	return lRes;
}

LONG WINAPI CRegKey::SetValue(HKEY hKeyParent, LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName)
{
	_ASSERTE(lpszValue != NULL);
	CRegKey key;
	LONG lRes = key.Create(hKeyParent, lpszKeyName);
	if (lRes == ERROR_SUCCESS)
		lRes = key.SetValue(lpszValue, lpszValueName);
	return lRes;
}

LONG CRegKey::SetKeyValue(LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName)
{
	_ASSERTE(lpszValue != NULL);
	CRegKey key;
	LONG lRes = key.Create(m_hKey, lpszKeyName);
	if (lRes == ERROR_SUCCESS)
		lRes = key.SetValue(lpszValue, lpszValueName);
	return lRes;
}

HRESULT CRegKey::SetValue(LPCTSTR lpszValue, LPCTSTR lpszValueName)
{
	_ASSERTE(lpszValue != NULL);
	_ASSERTE(m_hKey != NULL);
	return RegSetValueEx(m_hKey, lpszValueName, NULL, REG_SZ,
		(BYTE * const)lpszValue, (lstrlen(lpszValue)+1)*sizeof(TCHAR));
}


#endif // CRegKey implementaion for VC5

// This is the ``Mersenne Twister'' random number generator MT19937, which
// generates pseudorandom integers uniformly distributed in 0..(2^32 - 1)
// starting from any odd seed in 0..(2^32 - 1).  This version is a recode
// by Shawn Cokus (Cokus@math.washington.edu) on March 8, 1998 of a version by
// Takuji Nishimura (who had suggestions from Topher Cooper and Marc Rieffel in
// July-August 1997).
//
// Effectiveness of the recoding (on Goedel2.math.washington.edu, a DEC Alpha
// running OSF/1) using GCC -O3 as a compiler: before recoding: 51.6 sec. to
// generate 300 million random numbers; after recoding: 24.0 sec. for the same
// (i.e., 46.5% of original time), so speed is now about 12.5 million random
// number generations per second on this machine.
//
// According to the URL <http://www.math.keio.ac.jp/~matumoto/emt.html>
// (and paraphrasing a bit in places), the Mersenne Twister is ``designed
// with consideration of the flaws of various existing generators,'' has
// a period of 2^19937 - 1, gives a sequence that is 623-dimensionally
// equidistributed, and ``has passed many stringent tests, including the
// die-hard test of G. Marsaglia and the load test of P. Hellekalek and
// S. Wegenkittl.''  It is efficient in memory usage (typically using 2506
// to 5012 bytes of static data, depending on data type sizes, and the code
// is quite short as well).  It generates random numbers in batches of 624
// at a time, so the caching and pipelining of modern systems is exploited.
// It is also divide- and mod-free.
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation (either version 2 of the License or, at your
// option, any later version).  This library is distributed in the hope that
// it will be useful, but WITHOUT ANY WARRANTY, without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
// the GNU Library General Public License for more details.  You should have
// received a copy of the GNU Library General Public License along with this
// library; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA 02111-1307, USA.
//
// The code as Shawn received it included the following notice:
//
//   Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.  When
//   you use this, send an e-mail to <matumoto@math.keio.ac.jp> with
//   an appropriate reference to your work.
//
// It would be nice to CC: <Cokus@math.washington.edu> when you write.
#define LSV              (624)                 // length of state vector
#define PERIOD              (397)                 // a period parameter
#define MAGIC              (0x9908B0DFU)         // a magic constant
#define hiBit(u)       ((u) & 0x80000000U)   // mask all but highest   bit of u
#define loBit(u)       ((u) & 0x00000001U)   // mask all but lowest    bit of u
#define loBits(u)      ((u) & 0x7FFFFFFFU)   // mask     the highest   bit of u
#define mixBits(u, v)  (hiBit(u)|loBits(v))  // move hi bit of u to hi bit of v

static DWORD   state[LSV+1];     // state vector + 1 extra to not violate ANSI C
static DWORD   *next;          // next random value is computed from here
static int     left = -1;      // can *next++ this many times before reloading


void seedMT(DWORD seed)
 {
    //
    // We initialize state[0..(LSV-1)] via the generator
    //
    //   x_new = (69069 * x_old) mod 2^32
    //
    // from Line 15 of Table 1, p. 106, Sec. 3.3.4 of Knuth's
    // _The Art of Computer Programming_, Volume 2, 3rd ed.
    //
    // Notes (SJC): I do not know what the initial state requirements
    // of the Mersenne Twister are, but it seems this seeding generator
    // could be better.  It achieves the maximum period for its modulus
    // (2^30) iff x_initial is odd (p. 20-21, Sec. 3.2.1.2, Knuth); if
    // x_initial can be even, you have sequences like 0, 0, 0, ...;
    // 2^31, 2^31, 2^31, ...; 2^30, 2^30, 2^30, ...; 2^29, 2^29 + 2^31,
    // 2^29, 2^29 + 2^31, ..., etc. so I force seed to be odd below.
    //
    // Even if x_initial is odd, if x_initial is 1 mod 4 then
    //
    //   the          lowest bit of x is always 1,
    //   the  next-to-lowest bit of x is always 0,
    //   the 2nd-from-lowest bit of x alternates      ... 0 1 0 1 0 1 0 1 ... ,
    //   the 3rd-from-lowest bit of x 4-cycles        ... 0 1 1 0 0 1 1 0 ... ,
    //   the 4th-from-lowest bit of x has the 8-cycle ... 0 0 0 1 1 1 1 0 ... ,
    //    ...
    //
    // and if x_initial is 3 mod 4 then
    //
    //   the          lowest bit of x is always 1,
    //   the  next-to-lowest bit of x is always 1,
    //   the 2nd-from-lowest bit of x alternates      ... 0 1 0 1 0 1 0 1 ... ,
    //   the 3rd-from-lowest bit of x 4-cycles        ... 0 0 1 1 0 0 1 1 ... ,
    //   the 4th-from-lowest bit of x has the 8-cycle ... 0 0 1 1 1 1 0 0 ... ,
    //    ...
    //
    // The generator's potency (min. s>=0 with (69069-1)^s = 0 mod 2^32) is
    // 16, which seems to be alright by p. 25, Sec. 3.2.1.3 of Knuth.  It
    // also does well in the dimension 2..5 spectral tests, but it could be
    // better in dimension 6 (Line 15, Table 1, p. 106, Sec. 3.3.4, Knuth).
    //
    // Note that the random number user does not see the values generated
    // here directly since reloadMT() will always munge them first, so maybe
    // none of all of this matters.  In fact, the seed values made here could
    // even be extra-special desirable if the Mersenne Twister theory says
    // so-- that's why the only change I made is to restrict to odd seeds.
    //

    DWORD x = (seed | 1U) & 0xFFFFFFFFU, *s = state;
    int    j;

    for (left=0, *s++=x, j=LSV; --j; *s++=(x*=69069U) & 0xFFFFFFFFU)
      ;
 }


DWORD reloadMT(void)
 {
    DWORD *p0=state, *p2=state+2, *pM=state+PERIOD, s0, s1;
    int    j;

    if(left < -1)
        seedMT(GetTickCount()); //seedMT(4357U);

    left=LSV-1, next=state+1;

    for(s0=state[0], s1=state[1], j=LSV-PERIOD+1; --j; s0=s1, s1=*p2++)
        *p0++ = *pM++ ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? MAGIC : 0U);

    for(pM=state, j=PERIOD; --j; s0=s1, s1=*p2++)
        *p0++ = *pM++ ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? MAGIC : 0U);

    s1=state[0], *p0 = *pM ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? MAGIC : 0U);
    s1 ^= (s1 >> 11);
    s1 ^= (s1 <<  7) & 0x9D2C5680U;
    s1 ^= (s1 << 15) & 0xEFC60000U;
    return(s1 ^ (s1 >> 18));
 }

inline DWORD randomMT(void)
 {
    DWORD y;

    if(--left < 0)
        return(reloadMT());

    y  = *next++;
    y ^= (y >> 11);
    y ^= (y <<  7) & 0x9D2C5680U;
    y ^= (y << 15) & 0xEFC60000U;
    return(y ^ (y >> 18));
 }


#define FNV_prime 16777619
#define offset_basis 2166136261

DWORD Hash(char *pC, int len)
{
  DWORD hash;
  int i;
  hash = offset_basis;
  for (i=0; i<len; i++)
  {
    hash = hash ^ (unsigned)pC[i];
    hash = hash * FNV_prime;
  };
  return hash;
}

//*****************************************************************************
// NAME: RollDice (sides, numTimes, bonus)
//
// PURPOSE:  
//*****************************************************************************
float RollDice( int sides, int numTimes, float bonus)
{
  if (sides <= 0) return bonus;//0.0;
  if (numTimes <= 0) return bonus;//0.0;
	float result=0.0;
	for (int i=0;i<numTimes;i++)
    result += ((randomMT()%sides) + 1);
		//result += ((rand()%sides) + 1);
  result += bonus;
//  WriteDebugString("DEBUG - RollDice(sides=%i, times=%i, bonus=%f) result=%f\n",
//                   sides, numTimes, bonus, result);
	return result;
}

BOOL IsTriggerTimeInCurrentTime(int start, int stop)
{
  BOOL result = FALSE;
  // 24 hour time -> 0000-2359
  int ptime = party.get24HourClock();
  // if start > end then time period wraps across midnight (over two days)
  if (!(start > stop))
  {
    if ((ptime >= start) && (ptime <= stop)) result = TRUE;
  }
  else 
  {
    if ((ptime >= start) || (ptime <= stop)) result = TRUE;
  }
  return result;
}

#ifdef UAFEngine

int GetMaxPartyPCs()
{
  return (min( MAX_PARTY_PCS, globalData.GetMaxPCs() ));
}

int GetMinPartyPCs()
{
  return (max( 1, globalData.GetMinPCs() ));
}

int GetMaxPartyMembers()
{
  return (min( MAX_PARTY_MEMBERS, globalData.GetMaxPartySize() ));
}

#endif

/*
CString ClassFlagToText(WORD types)
{
  CString tmp("");
  if (types == AllClasses)
  {
    tmp = ClassFlagText[FlagToIndex(AllClasses)];
  }
  else
  {
    for (int i=0;i<NumBaseClass;i++)
    {
      BYTE mask = types & IndexToFlag(i);
      if (mask)
      {
        if (!tmp.IsEmpty())
          tmp += ", ";

        tmp += ClassFlagText[FlagToIndex(mask)];
      }
    }
  }

  if (tmp.IsEmpty())
  {
    ASS ERT(FALSE); // mistake of some sort?
    tmp="ClassFlagError!!";
  }

  return tmp;
}
*/


#if (defined UAFEDITOR)

BOOL CreateBackupDesignDirectory()
{
  BOOL success = FALSE;
 // struct _tstat st;
  char path[_MAX_PATH+1];

  GetDesignPath(path);
  strcat(path, "Backup");

/*
  if (_tstat(path, &st) == 0)
  {
    if (st.st_mode & _S_IFDIR)
    {
      success = TRUE;
    }
    else 
    {
      MsgBoxError("Cannot create backup design directory\nFile with same name exists",
                   "Design Save Error");
    }
  }
  else 
  {
    if( _mkdir(path) == 0)
      success = TRUE;
  }
*/
  WriteDebugString("Entering CreateBackupDesignDirectory(%s)\n", path);
  if (PathFileExists(path))
  {
    if (PathIsDirectory(path))
    {
      success = TRUE;
    }
    else
    {
      WriteDebugString("A non-directory file of that name already exists\n");
    };
  }
  else
  {
    if (_mkdir(path) == 0)
    {
      success = TRUE;
    }
    else
    {
      WriteDebugString("Attempt to create directory failed\n");
    };
  };

  DeleteAllFilesInFolder(path);
  return success;
}


BOOL CopyFileIfDifferent(const CString& srcFilename, const CString& dstFilename, BOOL FailIfExists)
{
  FILE *fa, *fb;
  if (srcFilename == dstFilename) return TRUE;
  fa = fopen(srcFilename,"rb");
  if (fa == NULL) return FALSE;
  fb = fopen(dstFilename,"rb");
  if (fb != NULL)
  {
    if (fseek(fa, 0, SEEK_END) == fseek(fb, 0, SEEK_END))
    {
      //Here we should read and compare the files.
    };
    fclose (fb);
  }
  fclose(fa);
  return MyCopyFile(srcFilename, dstFilename, FailIfExists);
}

BOOL CopyFileToFolder(CString srcFilename, CString dstFolder, CString bak)
{
  if (!FileExists(srcFilename)) return FALSE;

  SetFileAttributes(srcFilename, FILE_ATTRIBUTE_NORMAL);
  
  CString dstFile(dstFolder);
  CString srcFile(srcFilename);
  StripFilenamePath(srcFile);
  dstFile += "\\";
  dstFile += srcFile;  

  if (FileExists(dstFile))
  {
    SetFileAttributes(dstFile, FILE_ATTRIBUTE_NORMAL);

    if (!bak.IsEmpty())
    {
      if (backup(dstFile, bak)!=0) 
        return FALSE;
    }
  }

  return (MyCopyFile(srcFilename, dstFile, TRUE));
}

void CopyFileType(CString FileExt, CString src, CString dest)
{
  char searchpath[_MAX_PATH+1];
  CString dname;

  strcpy(searchpath, src);
  if (searchpath[strlen(searchpath)-1] != '\\')
    strcat(searchpath, "\\");
  strcat(searchpath, "*.");
  strcat(searchpath, FileExt);
 
   CFileFind finder;
   BOOL found = finder.FindFile(searchpath);

   while (found)
   {
      found = finder.FindNextFile();

      if (finder.IsDots())
         continue;
      if (finder.IsDirectory())
        continue;

      dname.Format("%s\\%s", dest, finder.GetFileName());
      SetFileAttributes(dname, FILE_ATTRIBUTE_NORMAL);
      MyCopyFile(finder.GetFilePath(), dname, FALSE);
      SetFileAttributes(dname, FILE_ATTRIBUTE_NORMAL);
   }
}

void BackupFileType(CString FileExt)
{
  //char path[_MAX_PATH+1];
  //char searchpath[_MAX_PATH+1];
  CString destPath;
  CString searchPath;

  destPath = rte.BackupDir();
  
  CopyFileType(FileExt, searchPath, destPath);
}


BOOL CreateEmptyDirectory(const CString& dname)
{
 // struct _tstat st;
  // first create new folder
  WriteDebugString("Entering CreateEmptyDirectory(%s)\n", dname);
  /*
  if (_tstat(dname, &st) == 0)
  {
    DeleteAllFilesInFolder(dname);
  }
  else 
  {
    if( _mkdir(dname) != 0)
    {
      MsgBoxError("Cannot create design directory\n",
                  "Design Copy Error");
      return FALSE;
    }
  };
  */
  if (PathFileExists(dname))
  {
    if (PathIsDirectory(dname))
    {
      DeleteAllFilesInFolder(dname);
    }
    else
    {
      WriteDebugString("Path is not a folder\n");
      return FALSE;
    }
  }
  else
  {
    if (_mkdir(dname) != 0)
    {
      WriteDebugString("Attempt to create folder failed\n");
      MsgBoxError("Cannot create design directory\n",
        "Design Copy Error");
      return FALSE;
    }
  };
  return TRUE;
}



BOOL CopyDesignFiles(CString newFolder)
{
 // struct _tstat st;
  char path[_MAX_PATH+1];
  strcpy(path, newFolder);
  WriteDebugString("Entering CopyDesignFiles(%s)\n", newFolder);
  EditorStatusMsg("Creating new design folder...");

  // first create new folder
/*
  if (_tstat(path, &st) == 0)
  {
    if (!(st.st_mode & _S_IFDIR))
    {
      MsgBoxError("Cannot create design directory\nFile with same name exists",
                  "Design Copy Error");
      return FALSE;
    }
  }
  else 
  {
    if( _mkdir(path) != 0)
    {
      MsgBoxError("Cannot create design directory\n",
                  "Design Copy Error");
      return FALSE;
    }
  };
*/
  if (PathFileExists(newFolder))
  {
    if (!PathIsDirectory(newFolder))
    {
      WriteDebugString("Given path is not a folder name\n");
      return FALSE;
    }
  }
  else
  {
    if (_mkdir(path) != 0)
    {
      WriteDebugString("Attempt to create directory failed\n");
      MsgBoxError("Cannot create design directory\n",
        "Design Copy Error");
      return FALSE;
    }
  };
  // make sure new folder is empty
  DeleteAllFilesInFolder(path);

  // now copy the existing files to the new folder

  EditorStatusMsg("Copying all files to new design folder...");

  {
    strcpy(path, rte.DesignDir());
    if (path[strlen(path)-1] == '\\')
    {
      path[strlen(path)-1] = 0;
    };
  
    CString source, destination;
    source = path;
    destination = newFolder;
    CopyFileType("exe", source, destination);  // Works even if we change executable name
    MyCopyFile(source+"\\Readme.txt", destination+"\\Readme.txt", TRUE);

    source = path;
    source += "\\Data";
    destination = newFolder + "\\Data";
    if (!CreateEmptyDirectory(destination)) return FALSE;
    CopyFileType("*", source, destination);

    source = path;
    source += "\\Resources";
    destination = newFolder + "\\Resources";
    if (!CreateEmptyDirectory(destination)) return FALSE;
    CopyFileType("*", source, destination);

    source = path;
    source += "\\Saves";
    destination = newFolder + "\\Saves";
    if (!CreateEmptyDirectory(destination)) return FALSE;;
    CopyFileType("*", source, destination);
  };
  return TRUE;
}

void BackupDesignFiles()
{
  CString fullPath;
  fullPath = rte.DataDir() + "game.dat";

  double DesignVersion = GetDesignVersion(fullPath);
  if (DesignVersion <= 0.5) 
    return; // don't backup bogus design files

  EditorStatusMsg("Creating backups of design files...");

  CreateBackupDesignDirectory();
  BackupFileType("dat");
  BackupFileType("lvl");
  BackupFileType("txt");
}

#endif

#ifdef UAFEngine


struct SUBSTR
{
  int start;
  int len;
};

struct STR_SUM
{
  STR_SUM(void);
  ~STR_SUM(void);
  void Summarize(const CString& str);
  BOOL IsEqual(int i, STR_SUM *cmp, int j);

  const char *pStr;
  int numSub;  // Number of sub-strings
  SUBSTR *subs;   // Start column and length of each sub-string;
};

STR_SUM::STR_SUM(void)
{
  numSub = 0;
  subs = NULL;
}

STR_SUM::~STR_SUM(void)
{
  if (subs != NULL) free(subs);
  subs = NULL;
}

void STR_SUM::Summarize(const CString& str)
{
  int col;
  char delim;
  if ((subs == NULL) || (_msize(subs) < str.GetLength()*sizeof(SUBSTR)))
  {
    subs = (SUBSTR *)realloc(subs, str.GetLength() * sizeof(SUBSTR));
  };
  pStr = (LPCSTR)str;
  col = 0;
  delim = pStr[0];
  numSub = 0;
  while (pStr[col++] != 0) // skip delimiter
  {
    // For each sub-string
    subs[numSub].start = col;
    while ( (pStr[col] != 0) && (pStr[col] != delim) )
    {
      col++;
    };
    subs[numSub].len = col - subs[numSub].start;
    numSub++;
  };
}

BOOL STR_SUM::IsEqual(int i, STR_SUM *pCom, int j)
{
  if (subs[i].len != pCom->subs[j].len) return FALSE;
  return memcmp((LPCSTR)(pStr) + subs[i].start,
                (LPCSTR)(pCom->pStr) + pCom->subs[j].start,
                subs[i].len) == 0;
}

STR_SUM srcSum;
STR_SUM filterSum;

CString DelimitedStringFilter(const CString& src, 
                              const CString& filter,
                              const CString& function)
{
  if (src.GetLength() == 0) return src;
  srcSum.Summarize(src);
  filterSum.Summarize(filter);
  char *result;
  int resultCol = 0;
  result = (char *)malloc(src.GetLength() + 1);
  if (function == "AndNot")
  {
    int i, j;
    for (i=0; i<srcSum.numSub; i++)
    {
      for (j=0; j<filterSum.numSub; j++)
      {
        if (srcSum.IsEqual(i, &filterSum, j)) break;
      };
      if (j == filterSum.numSub)
      {
        int k;
        k = srcSum.subs[i].start;
        result[resultCol++] = src[0];
        memcpy(result+resultCol, 
               (LPCSTR)(srcSum.pStr)+k,
               srcSum.subs[i].len);
        resultCol += srcSum.subs[i].len;
      };
    };
    result[resultCol] =  0;
    return result;
  };
  return function;
}

#endif

JOURNAL_ENTRY& JOURNAL_ENTRY::operator =(const JOURNAL_ENTRY& src)
{
  if (this==&src) return *this;
  Clear();
  entry=src.entry;
  origentry=src.origentry;
  text=src.text;
  return *this;
}
void JOURNAL_ENTRY::Serialize(CArchive &ar)
{
  if (ar.IsStoring())
  {
    ar << entry;
    ar << origentry;
    AS(ar, text);
  }
  else
  {
    ar >> entry;
    ar >> origentry;
    DAS(ar, text);
  }
};

void JOURNAL_ENTRY::Serialize(CAR &car) //New 20121208 PRS
{
  if (car.IsStoring())
  {
    car << entry;
    car << origentry;
    AS(car, text);
  }
  else
  {
    car >> entry;
    car >> origentry;
    DAS(car, text);
  }
};
#ifdef UAFEDITOR
void JOURNAL_ENTRY::Export(JWriter& jw)
{
  jw.StartList();
  jw.NameAndValue(JKEY_KEY, entry);
  jw.NameAndValue(JKEY_ORIGKEY, origentry);
  if (!text.IsEmpty())  jw.NameAndMultiLineString(JKEY_MTEXT, text);
  //jw.NameAndValue(JKEY_TEXT, text);
  jw.EndList();
};

void JOURNAL_ENTRY::Import(JReader& jr)
{
  jr.StartList();
  jr.NameAndValue(JKEY_KEY, entry);
  jr.NameAndValue(JKEY_ORIGKEY, origentry);
  //jr.NameAndValue(JKEY_TEXT, text);
  jr.Optional(); jr.NameAndValue(JKEY_TEXT, text);
  jr.Optional(); jr.NameAndMultiLineString(JKEY_MTEXT, text);
  jr.EndList();
};
#endif