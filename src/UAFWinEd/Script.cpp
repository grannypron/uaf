#include "stdafx.h"
#include "resource.h"
#include "GlobalData.h"
#include "UAFWinEdView.h"

void  CheckForUnsavedLevel(bool quiet);
bool ImportCharacters(void);


int ImportGlobalDB(LPCSTR file,
  GLOBAL_STATS *pGlobalPointer,
  CONFIG_FILE *pConfigFile,
  BOOL quiet);

int ReadAbilityDB(LPCSTR file,
  ABILITY_DATA_TYPE *abilitiesPointer,
  CONFIG_FILE *pConfigFile = NULL);

int ReadBaseclassDB(CString &file,
  BASE_CLASS_DATA_TYPE *baseclassesPointer,
  CONFIG_FILE *pConfigFile = NULL);

int ReadClassDB(CString &file,
  CLASS_DATA_TYPE *clasesPointer,
  CONFIG_FILE *pConfigFile = NULL);

int ReadItemDB(CString &file,
  ITEM_DATA_TYPE *itemsPointer,
  CONFIG_FILE *pConfigFile = NULL);

int ReadRaceDB(CString &file,
  RACE_DATA_TYPE *racesPointer,
  CONFIG_FILE *pConfigFile = NULL);

int ReadSpellDB(CString &file,
  SPELL_DATA_TYPE *spellsPointer,
  CONFIG_FILE *pConfigFile = NULL);

int ReadMonsterDB(CString &file,
  MONSTER_DATA_TYPE *monstersPointer,
  CONFIG_FILE *pConfigFile = NULL);

int ReadSpecialAbilitiesDB(LPCSTR file,
  A_SPECABILITY_DEF_L *itemsPointer,
  CONFIG_FILE *pConfigFile = NULL);


static FILE *f;
static int lineNumber;
static const int MAXTOKEN = 100;
static const int MAXTOKENLEN = 100;

// level/levellist/5/select
// level/levellist/5/import Level005.txt


static void ScriptError(const CString& m1, const CString& m2)
{
  CString msg;
  msg.Format("Error Executing Script at line number %d\n%s\n%s",
    lineNumber, (LPCSTR)m1, (LPCSTR)m2);
  MessageBox(NULL, msg, "Script Error", MB_OK);
}

static bool CStringToInt(const CString& str, int *n)
{
  int col;
  char chr;
  *n = 0;
  for (col=0;col<str.GetLength();col++)
  {
    chr = str[col];
    if ((chr >= '0') && (chr <= '9'))
    {
      *n = *n * 10 + chr - '0';
      continue;
    };
    return false;
  };
  return true;
}


static bool SelectLevel(int levelOrdinal)
{
  BOOL ok = FALSE;
  int levelIndex = levelOrdinal - 1;
  if (levelIndex >= MAX_LEVELS)
  {
    ScriptError("Level Number too large", "");
    return false;
  }
  {
    CWaitCursor cursor;
    WriteDebugString("Selecting level %d\n", levelOrdinal);
    {
      if (    (globalData.currLevel != levelIndex)
           || (globalData.levelInfo.stats[levelIndex].used == 0))
      {
        CheckForUnsavedLevel(true);

        currX = 0;
        currY = 0;
        currFacing = FACE_EAST;
        party.facing = currFacing;
        // if level exists, load it
        // else working with new un-saved level
        if (levelExists(levelIndex))  // If the Levelnnn.lvl file exists
        {
          if (!LoadLevel(levelIndex))
          {
            CString lvl;
            lvl.Format("Failed to load level number %d", levelOrdinal);
            ScriptError("lvl", "");
            return false;
          }
          else
          {
            ok = TRUE; // existing level loaded ok
          }
        }
        else
          ok = TRUE; // new level     

          if (ok)
          {
            globalData.currLevel = levelIndex; // new or loaded level
            globalData.levelInfo.stats[levelIndex].used = 1;
          };
      }
      //      else
      //      {
      //        levelData.SetSlots();
      //        ok = TRUE; // using same level
      //      }
      //    }
      //    else
    };
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

  if (pWallSlotMgr != NULL) pWallSlotMgr->Clear();
  if (pBgSlotMgr != NULL) pBgSlotMgr->Clear();

  CUAFWinEdView* pView = (CUAFWinEdView*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
  if (pView != NULL)
    pView->ResetView();

  if (((CUAFWinEdApp*)AfxGetApp())->m_pDoc != NULL)
    ((CUAFWinEdApp*)AfxGetApp())->m_pDoc->UpdateAllViews(NULL);
  return true;
}

static bool ImportLevel(int levelOrdinal, const CString& filename)
{
  CString filePath;
  int finalStatus;
  int levelIndex;
  levelIndex = levelOrdinal - 1;
  if (!SelectLevel(levelOrdinal)) return false;
  filePath = rte.DataDir() + filename;
  {
    CFile cf;
    WriteDebugString("Starting to import level %d\n", levelOrdinal);
    if (!cf.Open(filePath, CFile::modeRead))
    {
      CString msg;
      msg.Format("Unable to open script file:\n%s", (LPCSTR)filePath);
      MsgBoxInfo(msg);
      return false;
    }
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
      return false;
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
      catch (int e)
      {
        e = 1;
        CString msg;
        msg.Format("Attempting to load level %d\n", levelOrdinal);
        MsgBoxInfo(msg + jr.GetErrorMessage());
        finalStatus = CONFIG_STAT_semantic;
      };
      if (finalStatus != CONFIG_STAT_ok)
      {
        MsgBoxInfo("We failed to import the Level data");
        AfxGetApp()->DoWaitCursor(-1);
        return false;
      };
      tempLevelStats.used = globalData.levelInfo.stats[levelIndex].used;
      if (finalStatus == CONFIG_STAT_ok)
      {
        //CString origLevelName;
        //origLevelName = globalData.levelInfo.stats[m_level].level_name;
        globalData.levelInfo.stats[levelIndex] = tempLevelStats;
        //globalData.levelInfo.stats[m_level].level_name = origLevelName; 
        saveLevel(tempLevel, levelIndex);
        LoadLevel(levelIndex);
        SetModified();
      };
    };
  };
  AfxGetApp()->DoWaitCursor(-1);
  return true;
}

static bool ImportGlobals()
{
  int status;
  WriteDebugString("Starting to import GlobalData.txt\n");
  status = ImportGlobalDB(rte.DataDir() + "GlobalData.txt", &globalData, NULL, TRUE);
  if (status != 0)
  {
    MsgBoxError("Failed to load GlobalData.txt");
  };
  return status == 0;
}

static bool ImportAbilities()
{
  CString filename;
  int status;
  CString errorMsg;
  filename = rte.DataDir() + ABILITY_CONFIG_NAME;

  WriteDebugString("Beginning decode of \'%s\'\n", filename);

  status = ReadAbilityDB(filename, &abilityData);
  switch (status)
  {
  case 0:
    break;
  case 1:
    WriteDebugString("Minor errors in Abilities database\n");
    break;
  case -1:
    errorMsg = "Cannot read Abilities database file";
    break;
  case -2:
    errorMsg = "Abilities database file appears to be the wrong kind of file";
    break;
  case -3:
    errorMsg = "Internal program error reading abilities database file";
    break;
  default:
    errorMsg = "Unknown error reading abilities database file";
    break;
  };
  if (errorMsg.IsEmpty()) return true;
  WriteDebugString(errorMsg + '\n');
  ScriptError(errorMsg,"");
  return false;

}

static bool ImportBaseclasses()
{
  // TODO: Add your control notification handler code here

  CString filename;
  filename = rte.DataDir() + BASE_CLASS_CONFIG_NAME;
  {
    SET_MODIFIED;
    AfxGetApp()->DoWaitCursor(1);
    WriteDebugString("Starting to import Baseclasses\n");
    int status = ReadBaseclassDB(filename, &baseclassData, NULL);
    AfxGetApp()->DoWaitCursor(-1);
    CString msg;
    switch (status)
    {
    case 0:
      WriteDebugString("Baseclass database read successfully");
      break;
    case 1:
      WriteDebugString("Minor errors in Baseclass database");
      break;
    case -1:
      msg = "Cannot read Baseclass database file";
      break;
    case -2:
      msg = "Baseclass database file appears to be the wrong kind of file";
      break;
    case -3:
      msg = "Internal program error reading Baseclass database file";
      break;
    default:
      msg = "Unknown error reading Baseclass database file";
      break;
    };
    if (msg.IsEmpty()) return true;
    WriteDebugString("%s\n", msg);
    ScriptError(msg,"");
    return false;
  }
}

static bool ImportClasses()
{
  CString filename;
  filename = rte.DataDir() + CLASS_CONFIG_NAME;
  {
    SET_MODIFIED;
    AfxGetApp()->DoWaitCursor(1);
    WriteDebugString("Starting to import classes]n");
    int status = ReadClassDB(filename, &classData);
    AfxGetApp()->DoWaitCursor(-1);
    CString msg;
    switch (status)
    {
    case 0:
      WriteDebugString("Class database read successfully");
      break;
    case 1:
      WriteDebugString("Minor errors in Class database");
      break;
    case -1:
      msg = "Cannot read Class database file";
      break;
    case -2:
      msg = "Class database file appears to be the wrong kind of file";
      break;
    case -3:
      msg = "Internal program error reading Class database file";
      break;
    default:
      msg = "Unknown error reading Class database file";
      break;
    };
    if (msg.IsEmpty()) return true;
    WriteDebugString("%s\n", msg);
    ScriptError(msg, "Database Status");
    return false;
  }
}

static bool ImportRaces()
{
  // TODO: Add your control notification handler code here

  CString filename;
  filename = rte.DataDir() + RACE_CONFIG_NAME;
  {
    SET_MODIFIED;
    AfxGetApp()->DoWaitCursor(1);
    WriteDebugString("Starting to import races\n");
    int status = ReadRaceDB(filename, &raceData);
    AfxGetApp()->DoWaitCursor(-1);

    CString msg;
    switch (status)
    {
    case 0:
      WriteDebugString("Race database read successfully");
      break;
    case 1:
      WriteDebugString("Minor errors in Race database");
      break;
    case -1:
      msg = "Cannot read Race database file";
      break;
    case -2:
      msg = "Race database file appears to be the wrong kind of file";
      break;
    case -3:
      msg = "Internal program error reading Race database file";
      break;
    default:
      msg = "Unknown error reading Race database file";
      break;
    };
    if (msg.IsEmpty()) return true;
    WriteDebugString("%s\n", msg);
    ScriptError(msg, "Database Status");
    return false;
  }
}

static bool ImportSpells()
{
  CString filename;
  filename = rte.DataDir() + SPELL_CONFIG_NAME;
  {
    SET_MODIFIED;
    AfxGetApp()->DoWaitCursor(1);
    WriteDebugString("Starting to import Spells\n");
    int status = ReadSpellDB(filename, &spellData);
    AfxGetApp()->DoWaitCursor(-1);

    CString msg;
    switch (status)
    {
    case 0:
      WriteDebugString("Spell database read successfully");
      break;
    case 1:
      WriteDebugString("Minor errors in Spells database");
      break;
    case -1:
      msg = "Cannot read Spells database file";
      break;
    case -2:
      msg = "Spells database file appears to be the wrong kind of file";
      break;
    case -3:
      msg = "Internal program error reading spells database file";
      break;
    default:
      msg = "Unknown error reading spells database file";
      break;
    };
    if (msg.IsEmpty()) return true;
    WriteDebugString("%s\n", msg);
    ScriptError(msg, "Database Status");
    return false;
  };
}

static bool ImportItems()
{
  CString filename;
  filename = rte.DataDir() + ITEM_CONFIG_NAME;
  {
    SET_MODIFIED;

    AfxGetApp()->DoWaitCursor(1);
    WriteDebugString("Starting to import items\n");
    int status = ReadItemDB(filename, &itemData);
    AfxGetApp()->DoWaitCursor(-1);
    CString msg;
    switch (status)
    {
    case 0:
      WriteDebugString("Items database file read successfully");
      break;
    case 1:
      WriteDebugString("Minor errors in Items database");
      break;
    case -1:
      msg = "Cannot read Items database file";
      break;
    case -2:
      msg = "Items database file appears to be the wrong kind of file";
      break;
    case -3:
      msg = "Internal program error reading items database file";
      break;
    default:
      msg = "Unknown error reading items database file";
      break;
    };
    if (msg.IsEmpty()) return true;
    WriteDebugString("%s\n", msg);
    MsgBoxInfo(msg, "Database Status");
    return false;
  }
}

static bool ImporMonsters()
{
  CString filename;
  filename = rte.DataDir() + MONSTER_CONFIG_NAME;
  {
    AfxGetApp()->DoWaitCursor(1);
    WriteDebugString("Starting to import monsters\n");
    int status = ReadMonsterDB(filename, &monsterData);
    AfxGetApp()->DoWaitCursor(-1);
    CString msg;
    switch (status)
    {
    case 0:
      WriteDebugString("Monsters database file read successfully");
      break;
    case 1:
      WriteDebugString("Minor errors in Monsters database");
      break;
    case -1:
      msg = "Cannot read Monsters database file";
      break;
    case -2:
      msg = "Monsters database file appears to be the wrong kind of file";
      break;
    case -3:
      msg = "Internal program error reading monsters database file";
      break;
    default:
      msg = "Unknown error reading monsters database file";
      break;
    };
    if (msg.IsEmpty()) return true;
    WriteDebugString("%s\n", msg);
    ScriptError(msg, "Database Status");
    return false;
  }
}

static bool ImportSpecialAbilities()
{
  CString filename;
  filename = rte.DataDir() + SPECAB_CONFIG_NAME;
  {
    SET_MODIFIED;

    AfxGetApp()->DoWaitCursor(1);
    WriteDebugString("Starting to import Special Abilities");
    int status = ReadSpecialAbilitiesDB(filename, &specialAbilitiesData);
    AfxGetApp()->DoWaitCursor(-1);
    CString msg;
    switch (status)
    {
    case 0:
      WriteDebugString("Special Abilities database file read successfully");
      break;
    case 1:
      WriteDebugString("Minor errors in Special Abilities database");
      break;
    case -1:
      msg = "Cannot read Special Abilities database file";
      break;
    case -2:
      msg = "Special Abilities database file appears to be the wrong kind of file";
      break;
    case -3:
      msg = "Internal program error reading Special Abilities database file";
      break;
    default:
      msg = "Unknown error reading Special Abilities database file";
      break;
    };
    if (msg.IsEmpty()) return true;
    WriteDebugString("%s\n", msg);
    ScriptError(msg, "Database Status");
    return false;
  }
};



static bool ImportDatabase(const CString& dbname)
{
  if (dbname.CompareNoCase("Global Settings") == 0)   return ImportGlobals();
  if (dbname.CompareNoCase("abilities") == 0)         return ImportAbilities();
  if (dbname.CompareNoCase("base classes") == 0)      return ImportBaseclasses();
  if (dbname.CompareNoCase("classes") == 0)           return ImportClasses();
  if (dbname.CompareNoCase("races") == 0)             return ImportRaces();
  if (dbname.CompareNoCase("spells") == 0)            return ImportSpells();
  if (dbname.CompareNoCase("items") == 0)             return ImportItems();
  if (dbname.CompareNoCase("monsters") == 0)          return ImporMonsters();
  if (dbname.CompareNoCase("Special Abilities") == 0) return ImportSpecialAbilities();
  if (dbname.CompareNoCase("Characters") == 0)        return ImportCharacters();
  ScriptError("Script requests import of unknown database", dbname);
  return false;
}

bool ProcessScriptLine(int num, const CString tokens[])
{
  if (tokens[0].CompareNoCase("level") == 0)
  {
    if (tokens[1].CompareNoCase("level list") == 0)
    {
      int n;
      if (CStringToInt(tokens[2], &n))
      {
        if (tokens[3].CompareNoCase("select") == 0)
        {
          return SelectLevel(n);
        };
        if (tokens[3].CompareNoCase("import") == 0)
        {
          return ImportLevel(n, tokens[4]);
        };
        ScriptError(tokens[3], "Unrecognized fourth command");
        return false;
      };
      ScriptError(tokens[2], "Is not a number");
      return false;
    };
    ScriptError(tokens[1], "Unrecognized second command");
  };
  if (tokens[0].CompareNoCase("Database") == 0)
  {
    if (tokens[1].CompareNoCase("Import-Export databases") == 0)
    {
      if (tokens[2].CompareNoCase("Import") == 0)
      {
        return ImportDatabase(tokens[3]);
      }
      else
      {
        ScriptError(tokens[2], "Unrecognized third command");
        return false;
      };
    }
    else
    {
      ScriptError(tokens[1], "Unrecognized second command");
      return false;
    };
  };
  ScriptError(tokens[0], "Unrecognized first command");
  return false;
}

void ProcessScriptFile(const CString& filename)
{
  int numToken = 0;
  f = fopen((LPCSTR)filename, "r");
  CString tokens[MAXTOKEN];
  if (f == NULL)
  {
    MessageBox(NULL, CString("Cannot open script file ") + filename, "Warning", MB_OK);
    return;
  }
  lineNumber = 0;
  for (;;)
  {
    int col, len;
    char line[1000];
    char token[MAXTOKENLEN+1];
    if (fgets(line, 999, f) == NULL) break;
    lineNumber++;
    if (line[0] == '#') continue;
    for (col = 0, numToken=0; numToken<MAXTOKEN;)
    {
      while (line[col] == ' ') col++;
      if (line[col] == 0) break;
      if (line[col] == '\n') break;
      // We appear to have found a token;
      len = 0;
      while (   (line[col] != '/') 
             && (line[col] != 0) 
             && (line[col] != '\n')
             && (len < MAXTOKENLEN))
      {
        token[len++] = line[col++];
      }
      token[len] = 0;
      tokens[numToken++] = token;
      if (line[col] == '/') col++;
    };

    if (!ProcessScriptLine(numToken, tokens)) break;
  }
  fclose(f);
}