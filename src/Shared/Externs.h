/******************************************************************************
* Filename: Externs.h
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
#ifndef ___EXTERNS_H_
#define ___EXTERNS_H_

//
// This file has become the place to declarations that are
// global to the entire application, both engine and editor.
//

//#include "ASL.h"
/////////////////////////////////////////////////////////
//#include "SurfaceMgr.h"
//
// I replaced this #include with a declaration  PRS
//
enum SurfaceType;
enum FONT_COLOR_NUM;
//
/////////////////////////////////////////////////////////

// Each release gets it's own version number. In addition
// to tagging data files with the current release version,
// the code must check loaded file versions against the 
// current release version to provide backward compatibility.

// Each new version is used in at least one place, directly
// below where the PRODUCT_VER is defined. But older version 
// defines are not always used directly in the code anywhere. 
// The unused defines can be commented out after moving beyond
// the initial release which used it.
//
#define _VERSION_0500_ 0.500
#define _VERSION_0505_ 0.505
//#define _VERSION_0530_ 0.530
#define _VERSION_0560_ 0.560
//#define _VERSION_0562_ 0.562
#define _VERSION_0563_ 0.563
#define _VERSION_0564_ 0.564
//#define _VERSION_0565_ 0.565
#define _VERSION_0566_ 0.566
#define _VERSION_05661_ 0.5661
#define _VERSION_05662_ 0.5662
//#define _VERSION_05663_ 0.5663
//#define _VERSION_0567_ 0.567
//#define _VERSION_05671_ 0.5671
//#define _VERSION_0568_ 0.568
#define _VERSION_0569_ 0.569
#define _VERSION_05691_ 0.5691
//#define _VERSION_0570_ 0.570
//#define _VERSION_0571_ 0.571
#define _VERSION_0572_ 0.572
#define _VERSION_0573_ 0.573
//#define _VERSION_05731_ 0.5731
#define _VERSION_0574_ 0.574
#define _VERSION_0575_ 0.575
//#define _VERSION_05751_ 0.5751
#define _VERSION_0576_ 0.576
//#define _VERSION_0577_ 0.577
#define _VERSION_05771_ 0.5771
#define _VERSION_0600_ 0.6000
//#define _VERSION_0610_ 0.6100
#define _VERSION_0620_ 0.6200
#define _VERSION_0630_ 0.6300
#define _VERSION_0640_ 0.6400
//#define _VERSION_0641_ 0.6410
#define _VERSION_0642_ 0.6420
//#define _VERSION_0650_ 0.6500
//#define _VERSION_0651_ 0.6510
//#define _VERSION_0653_ 0.6530
//#define _VERSION_0654_ 0.6540
//#define _VERSION_0655_ 0.6550
//#define _VERSION_0656_ 0.6560
#define _VERSION_0660_ 0.6600
#define _VERSION_0661_ 0.6610
#define _VERSION_0662_ 0.6620
#define _VERSION_0670_ 0.6700
//#define _VERSION_0671_ 0.6710
//#define _VERSION_0672_ 0.6720
//#define _VERSION_0680_ 0.6800
#define _VERSION_0681_ 0.6810
#define _VERSION_0682_ 0.6820
#define _VERSION_0690_ 0.6900
//#define _VERSION_0691_ 0.6910
#define _VERSION_0692_ 0.6920
#define _VERSION_0693_ 0.6930
#define _VERSION_0694_ 0.6940
//#define _VERSION_06941_ 0.6941
#define _VERSION_0695_ 0.6950
#define _VERSION_0696_ 0.6960
//#define _VERSION_06961_ 0.6961
#define _VERSION_0697_ 0.6970
//#define _VERSION_06971_ 0.6971
#define _VERSION_0698_ 0.6980
#define _VERSION_0699_ 0.6990
#define _VERSION_06991_ 0.6991
#define _VERSION_06992_ 0.6992
//#define _VERSION_0700_ 0.7000
#define _VERSION_0710_ 0.7100
#define _VERSION_0720_ 0.7200
#define _VERSION_0730_ 0.7300
#define _VERSION_0731_ 0.7310
#define _VERSION_0740_ 0.7400
#define _VERSION_0750_ 0.7500
//#define _VERSION_0760_ 0.7600
#define _VERSION_0770_ 0.7700
//#define _VERSION_0771_ 0.7710
#define _VERSION_0780_ 0.7800
#define _VERSION_0790_ 0.7900
//#define _VERSION_0791_ 0.7910
//#define _VERSION_0792_ 0.7920
#define _VERSION_0800_ 0.8000
#define _VERSION_0810_ 0.8100
#define _VERSION_0820_ 0.8200
#define _VERSION_0830_ 0.8300
//#define _VERSION_0831_ 0.8310
#define _VERSION_0840_ 0.8400
#define _VERSION_0841_ 0.8410
#define _VERSION_0842_ 0.8420
#define _VERSION_0850_ 0.8500
#define _VERSION_0851_ 0.8510
#define _VERSION_0852_ 0.8520
#define _VERSION_0860_ 0.8600
#define _VERSION_0870_ 0.8700
//#define _VERSION_0871_ 0.8710
#define _VERSION_0880_ 0.8800
#define _VERSION_0881_ 0.8810
#define _VERSION_0882_ 0.8820
#define _VERSION_0890_ 0.8900
//#define _VERSION_0891_ 0.8910
//#define _VERSION_0892_ 0.8920
#define _VERSION_0900_ 0.9000
//#define _VERSION_0902_ 0.9020
#define _VERSION_0904_ 0.9040
#define _VERSION_0906_ 0.9060
//#define _VERSION_0907_ 0.9070
#define _VERSION_0908_ 0.9080
#define _VERSION_0909_ 0.9090
//#define _VERSION_09091_ 0.9091
#define _VERSION_0910_ 0.910
#define _VERSION_0911_ 0.911
#define _VERSION_0912_ 0.912
#define _VERSION_0913_ 0.913
#define _VERSION_0914_ 0.914
#define _VERSION_0914_003 0.914003
#define _VERSION_0914_004 0.914004
#define _VERSION_0914_005 0.914005
#define _VERSION_0914_006 0.914006
#define _VERSION_0914_007 0.914007
#define _VERSION_0915_    0.915
#define _VERSION_0915_015 0.915015
#define _VERSION_0915_017 0.915017
#define _VERSION_0915_041 0.915041
#define _VERSION_0915_045 0.915045
#define _VERSION_0916     0.916
#define _VERSION_0920_000 0.920000
#define _VERSION_0930     0.930
#define _VERSION_524      5.24
#define _VERSION_525      5.25
#define _VERSION_526      5.26
#define _VERSION_527      5.27
#define _VERSION_528      5.28
#define _VERSION_529      5.29




// Some special versions
#define _ASL_LEVEL_                   _VERSION_0505_
#define _EXTENDED_ASL_LEVEL_          _VERSION_0560_  // Include flags at this version
#define _ITEM_DB_VERSION_             _VERSION_0560_
#define _SPELL_DB_VERSION_            _VERSION_0560_
#define _MONSTER_DB_VERSION_          _VERSION_0500_
#define _SPECIAL_ABILITIES_VERSION_   _VERSION_0930
#define _CELL_CONTENTS_VERSION        5.0



#define newCombatant

// Each character has a 'LEVEL' in each of his baseclasses.
// And depends on experience gained using that baseclass.
#define HIGHEST_CHARACTER_LEVEL 40

// The prime attribute 'SCORE' for a certain character
// capabilities might be Intelligence, Wisdom, Dexterity, etc.
#define HIGHEST_CHARACTER_PRIME 25

// Each Spell has a constant 'LEVEL'.  Higher level spells
// are generally more powerful than lower level spells.
const int MAX_SPELL_LEVEL = 9;


extern const int MIN_INTELLIGENCE;
extern const int MAX_INTELLIGENCE;
extern const int MIN_STRENGTH;
extern const int MAX_STRENGTH;
extern const int MIN_STRENGTHMOD;
extern const int MAX_STRENGTHMOD;
extern const int MIN_WISDOM;
extern const int MAX_WISDOM;
extern const int MIN_DEXTERITY;
extern const int MAX_DEXTERITY;
extern const int MIN_CONSTITUTION;
extern const int MAX_CONSTITUTION;
extern const int MIN_CHARISMA;
extern const int MAX_CHARISMA;

#define NULL_KEYWORD 0

enum IF_KEYWORD_INDEX
{
  /* character keywords */
  CHAR_BASE=0,
  CHAR_NAME=1,
  CHAR_AC=2,
  CHAR_HITPOINTS=3,
  CHAR_THAC0=4,
  CHAR_ADJTHAC0=5,
  CHAR_RDYTOTRAIN=6,
  //CHAR_CLERICEXP=6,
  //CHAR_MAGICUSEREXP=7,
  CHAR_ADJAC=7,
  CHAR_EFFAC=8,
  //CHAR_FIGHTEREXP=8,
  //CHAR_RANGEREXP=9,
  //CHAR_THIEFEXP=10,
  //CHAR_DRUIDEXP=11,
  //CHAR_PALADINEXP=12,
  CHAR_AGE=9,
  CHAR_MAXAGE=10,
  CHAR_MAXMOVE=11,
  CHAR_ADJUSTED_STR=12,
  CHAR_STR=12,
  CHAR_ADJUSTED_STRMOD=13,
  CHAR_STRMOD=13,
  CHAR_ADJUSTED_INT=14,
  CHAR_INT=14,
  CHAR_ADJUSTED_WIS=15,
  CHAR_WIS=15,
  CHAR_ADJUSTED_DEX=16,
  CHAR_DEX=16,
  CHAR_ADJUSTED_CON=17,
  CHAR_CON=17,
  CHAR_ADJUSTED_CHA=18,
  CHAR_CHA=18,
  CHAR_MAXENC=19,
  CHAR_ENC=20,
  CHAR_GENDER=21,
  CHAR_CLASS=22,
  CHAR_ALIGNMENT=23,
  CHAR_STATUS=24,
  CHAR_UNDEAD=25,
  CHAR_SIZE=26,
  CHAR_MAGICRESIST=27,
  CHAR_NBRHITDICE=28,
  CHAR_NBRATTACKS=29,
  CHAR_MORALE=30,
  CHAR_ALLOWPLAYERCONTROL=31,
  CHAR_DETECTINVISIBLE=32,
  CHAR_DETECTTRAPS=33,
  CHAR_DAMAGEBONUS=34,
  CHAR_CHARTYPE=35,
  CHAR_COINAMOUNT1=36,
  CHAR_COINAMOUNT2=37,
  CHAR_COINAMOUNT3=38,
  CHAR_COINAMOUNT4=39,
  CHAR_COINAMOUNT5=40,
  CHAR_COINAMOUNT6=41,
  CHAR_COINAMOUNT7=42,
  CHAR_COINAMOUNT8=43,
  CHAR_COINAMOUNT9=44,
  CHAR_COINAMOUNT10=45,
  CHAR_GEMAMOUNT=46,
  CHAR_JEWELRYAMOUNT=47,
  CHAR_ISMAMMAL=48,
  CHAR_ISANIMAL=49,
  CHAR_ISSNAKE=50,
  CHAR_ISGIANT=51,
  CHAR_ISALWAYSLARGE=52,
  CHAR_HASDWARFACPENALTY=53,
  CHAR_HASGNOMEACPENALTY=54,
  CHAR_HASDWARFTHAC0PENALTY=55,
  CHAR_HASGNOMETHAC0PENALTY=56,
  CHAR_HASRANGERDMGPENALTY=57,
  CHAR_HASPOISONIMMUNITY=58,
  CHAR_HASDEATHIMMUNITY=59,
  CHAR_HASCONFUSIONIMMUNITY=60,
  CHAR_HASVORPALIMMUNITY=61,
  CHAR_CANBEHELDORCHARMED=62,
  CHAR_AFFECTEDBYDISPELEVIL=63,
  CHAR_HITBONUS=64,
  CHAR_MIRRORIMAGE=65,
        
  /* party values */
  PARTY_BASE=512,
  PARTY_FACING=513,
  PARTY_DAYS=514,
  PARTY_HOURS=515,
  PARTY_MINUTES=516,
  PARTY_ACTIVECHAR=517,
  PARTY_MONEYAVAILABLE=518,
  PARTY_TIME=519,
  PARTY_LOCATION=520,

  /* game values */
  GAME_BASE=1024,
  GAME_CURRLEVEL=1025,
  GAME_VERSION=1026,
  GAME_COINNAME1=1027,
  GAME_COINNAME2=1028,
  GAME_COINNAME3=1029,
  GAME_COINNAME4=1030,
  GAME_COINNAME5=1031,
  GAME_COINNAME6=1032,
  GAME_COINNAME7=1033,
  GAME_COINNAME8=1034,
  GAME_COINNAME9=1035,
  GAME_COINNAME10=1036,
  GAME_GEMNAME=1037,
  GAME_JEWELRYNAME=1038,
  GAME_VAULTMONEYAVAILABLE=1039
};

#define FIRST_CHAR_KEYWORD CHAR_NAME
#define LAST_CHAR_KEYWORD  CHAR_MIRRORIMAGE

#define FIRST_PARTY_KEYWORD PARTY_FACING
#define LAST_PARTY_KEYWORD  PARTY_LOCATION

#define FIRST_GAME_KEYWORD GAME_CURRLEVEL
#define LAST_GAME_KEYWORD  GAME_VAULTMONEYAVAILABLE

struct IF_KEYWORD_DATA
{ 
  char iftype;   // char, party, or game var type
  char *keyword; // key text, such as '$CHAR_NAME'
  unsigned char type;     // string, byte, int, etc
  char readonly; // 1=read-only, 0=read/write
  IF_KEYWORD_INDEX  keyindex; // enum that matches key text, such CHAR_NAME
  int  virtualtrait; // represents trait that is virtual, or maps to existing memory
};



// graphics
#define USE_DIRECTX
//#define USE_OPENGL  1

enum raceType;
enum genderType;
//enum classType;
enum EventSourceType;
enum eventType;
extern CString GenderNames[3];

class CHARACTER;
struct COMBATANT;
struct QueuedCombatantData;
enum PATH_DIR;
struct COMBAT_MSGS;
class TRAITS;
class PIC_DATA;
class PASSWORD_DATA;
class FileParse;
class CAR;
extern FileParse ConfigFile;
struct PENDING_SPELL;
class SPELL_DATA;
extern int SCREEN_WIDTH;  // these change to reflect the current
extern int SCREEN_HEIGHT; // screen resolution for the editor,
extern int COLOR_BITS;

enum screenRes { res640x480=0, res800x600=1, res1024x768=2, res1152x864=3, res1280x1024=4 };
extern screenRes m_GameScreenRes;

// background colors
extern COLORREF BACKGROUND_FILL;
extern COLORREF BACKWARD_BACKGROUND_FILL;
extern COLORREF BACKGROUND_COMBAT_FILL;

 // used by editor to display map
extern CString MAPART;
extern CString OVERLANDART;


// used by engine to display area view of map
extern CString AREAVIEWART;

class PARTY;
extern PARTY party;
int GetPartySize(PARTY& party);
CString GetCharacterName(PARTY& data, int memberNumber);
genderType GetCharacterGender(PARTY& data, int memberNumber);
genderType GetCombatantGender(int memberNumber);
void SetCharacterGender(PARTY& data, int memberNumber, const CString& gender);
void SetCombatantGender(int memberNumber, const CString& gender);
void InsertCharacterASL(
           PARTY& data, 
           int memberNumber,
           const CString& key,
           const CString& value,
           int flags=0);
void InsertCharacterASL(
           CHARACTER *pCHar, 
           const CString& key,
           const CString& value,
           int flags=0);
CString LookupCharacterASL(
           PARTY& data,
           int memberNumber,
           const CString& key);
CString LookupCharacterASL(
           CHARACTER *pChar,
           const CString& key);
BOOL ExistsPartyASL(PARTY& data, const CString& key);
CString LookupPartyASL(
           PARTY& data,
           const CString& key);
void InsertPartyASL(
           PARTY& data, 
           const CString& key,
           const CString& value,
           int flags=0);
void DeletePartyASL(PARTY& data, const CString& key);

class LEVEL;
extern LEVEL levelData;
int GetAreaWidth(LEVEL& data);
int GetAreaHeight(LEVEL& data);
int GetAreaBlockages(LEVEL& data, int x, int y, int dir);

class GLOBAL_STATS;
extern GLOBAL_STATS globalData;
#define GLOBAL_CONFIG_NAME "GlobalData.txt"
// Names of the keys used rto open doors and such.  "" if unnamed
void GetKeyNames(GLOBAL_STATS& data, CString (*names)[8]);
long int GetKeyboardFont(GLOBAL_STATS& data);
long int GetKeyboardHighlightFont(GLOBAL_STATS& data);
long int GetHighlightFont(GLOBAL_STATS& data);
double GetGameVersion(GLOBAL_STATS& data);
void SetGameVersion(GLOBAL_STATS& data, double version);
CString GetDesignName(GLOBAL_STATS& data);
void DeleteLevelASL(GLOBAL_STATS& data, int level, const CString& key);
CString LookupGlobalASL(GLOBAL_STATS& data, const CString& key);
void InsertGlobalASL(
          GLOBAL_STATS& data,
          const CString& key,
          const CString& value,
          int flags=0);
void InsertLevelASL(
          GLOBAL_STATS& data,
          int level,
          const CString& key,
          const CString& value,
          int flags=0);

void saveSounds(GLOBAL_STATS& data);
void saveArt(GLOBAL_STATS& data);

// item database filename
class ITEM_DATA_TYPE;
#define ITEM_DB_NAME "items.dat"
#define ITEM_CONFIG_NAME "items.txt"
extern ITEM_DATA_TYPE itemData;
void clearData(ITEM_DATA_TYPE& data);
int loadData(ITEM_DATA_TYPE& data, LPCSTR fullPath);
BOOL saveData(ITEM_DATA_TYPE& data);
BOOL saveData(ITEM_DATA_TYPE& data, LPCSTR fullPath);
void saveArt(ITEM_DATA_TYPE& data);
void saveSounds(ITEM_DATA_TYPE& data);

// monster database filename
class MONSTER_DATA_TYPE;
#define MONSTER_DB_NAME "monsters.dat"
#define MONSTER_CONFIG_NAME "monsters.txt"
extern MONSTER_DATA_TYPE monsterData;
void clearData(MONSTER_DATA_TYPE& data);
int loadData(MONSTER_DATA_TYPE& data, LPCSTR fullPath);
BOOL saveData(MONSTER_DATA_TYPE& data);
BOOL saveData(MONSTER_DATA_TYPE& data, LPCSTR fullPath);
void SaveSounds(MONSTER_DATA_TYPE& data);
void SaveArt(MONSTER_DATA_TYPE& data);
void saveArt(MONSTER_DATA_TYPE& data);
void saveSounds(MONSTER_DATA_TYPE& data);

// spell database filename
class SPELL_DATA_TYPE;
#define SPELL_DB_NAME "spells.dat"
#define SPELL_CONFIG_NAME "spells.txt"
int loadData(SPELL_DATA_TYPE& data, LPCSTR fullPath);
BOOL saveData(SPELL_DATA_TYPE& data);
BOOL saveData(SPELL_DATA_TYPE& data, LPCSTR fullPath);
void saveSounds(SPELL_DATA_TYPE& data);
void saveArt(SPELL_DATA_TYPE& data);
void clearData(SPELL_DATA_TYPE& data);

#ifdef UAFEngine
struct LINE_TEXT_LIST;
extern LINE_TEXT_LIST lineListText;
struct SPELL_TEXT_LIST;
extern SPELL_TEXT_LIST spellListText;
#endif

// races data filename
class RACE_DATA_TYPE;
#define RACE_DB_NAME "races.dat"
#define RACE_CONFIG_NAME "races.txt"
#define RACE_ALLOWED_CLASS_ATTRIBUTE "AllowedClass"
extern  RACE_DATA_TYPE raceData;
int  GetCount(RACE_DATA_TYPE& raceData);
int  loadData(RACE_DATA_TYPE& raceData, LPCSTR fullpath);
void loadUADefaults(RACE_DATA_TYPE& data);
BOOL saveData(RACE_DATA_TYPE& data);
BOOL saveData(RACE_DATA_TYPE& data, LPCSTR fullPath);
int  GetDataSize(RACE_DATA_TYPE& data);
void Clear(RACE_DATA_TYPE& data);

// class data filename
class CLASS_DATA_TYPE;
#define CLASS_DB_NAME "classes.dat"
#define CLASS_CONFIG_NAME "classes.txt"
extern CLASS_DATA_TYPE classData;
int  GetCount(CLASS_DATA_TYPE& classData);
BOOL saveData(CLASS_DATA_TYPE& data);
BOOL saveData(CLASS_DATA_TYPE& data, LPCSTR fullpath);
int  loadData(CLASS_DATA_TYPE& data, LPCSTR fullpath);
void loadUADefaults(CLASS_DATA_TYPE& data);
int  GetDataSize(CLASS_DATA_TYPE& data);
void Clear(CLASS_DATA_TYPE& data);

// base classes data filename
class BASE_CLASS_DATA_TYPE;
#define BASE_CLASS_DB_NAME "baseclass.dat"
#define BASE_CLASS_CONFIG_NAME "baseclass.txt"
extern BASE_CLASS_DATA_TYPE baseclassData;
int  GetCount(BASE_CLASS_DATA_TYPE& baseclassData);
BOOL saveData(BASE_CLASS_DATA_TYPE& data);
BOOL saveData(BASE_CLASS_DATA_TYPE& data, LPCSTR fullpath);
int  loadData(BASE_CLASS_DATA_TYPE& data, LPCSTR fullpath);
void loadUADefaults(BASE_CLASS_DATA_TYPE& data);
int  GetDataSize(BASE_CLASS_DATA_TYPE& data);
void Clear(BASE_CLASS_DATA_TYPE& data);

// ability data filename
class ABILITY_DATA_TYPE;
#define ABILITY_DB_NAME "ability.dat"
#define ABILITY_CONFIG_NAME "ability.txt"
//typedef DWORD abilityType;
extern ABILITY_DATA_TYPE abilityData;
int  GetCount(ABILITY_DATA_TYPE& abilityData);
BOOL saveData(ABILITY_DATA_TYPE& data);
BOOL saveData(ABILITY_DATA_TYPE& data, LPCSTR fullpath);
int  loadData(ABILITY_DATA_TYPE& data, LPCSTR fullpath);
void loadUADefaults(ABILITY_DATA_TYPE& data);
int  GetDataSize(ABILITY_DATA_TYPE& data);
void Clear(ABILITY_DATA_TYPE& data);

// spellgroups data filename
class SPELLGROUP_DATA_TYPE;
//#define SPELLGROUP_DB_NAME "spellgroups.dat"
//#define SPELLGROUP_CONFIG_NAME "spellgroups.txt"
extern SPELLGROUP_DATA_TYPE spellgroupData;
int  GetCount(SPELLGROUP_DATA_TYPE& spellgroupData);
BOOL saveData(SPELLGROUP_DATA_TYPE& data);
BOOL saveData(SPELLGROUP_DATA_TYPE& data, LPCSTR fullpath);
int  loadData(SPELLGROUP_DATA_TYPE& data, LPCSTR fullpath);
void loadUADefaults(SPELLGROUP_DATA_TYPE& data);
int  GetDataSize(SPELLGROUP_DATA_TYPE& data);
void Clear(SPELLGROUP_DATA_TYPE& data);


#ifdef USE_TRAITS 
// trait data filename
class TRAIT_DATA_TYPE;
#define TRAIT_DB_NAME "traits.dat"
#define TRAIT_CONFIG_NAME "traits.txt"
extern TRAIT_DATA_TYPE traitData;
int  GetCount(TRAIT_DATA_TYPE& traitData);
BOOL saveData(TRAIT_DATA_TYPE& data);
BOOL saveData(TRAIT_DATA_TYPE& data, LPCSTR fullpath);
int  loadData(TRAIT_DATA_TYPE& data, LPCSTR fullpath);
void loadUADefaults(TRAIT_DATA_TYPE& data);
int  GetDataSize(TRAIT_DATA_TYPE& data);
void Clear(TRAIT_DATA_TYPE& data);
#endif


// special Abilities filename
class A_SPECABILITY_DEF_L;
#define SPECAB_DB_NAME "specialAbilities.dat"
#define SPECAB_CONFIG_NAME "specialAbilities.txt"
BOOL saveData(A_SPECABILITY_DEF_L& data);
BOOL saveData(A_SPECABILITY_DEF_L& data, LPCSTR fullpath);
int  loadData(A_SPECABILITY_DEF_L& data, LPCSTR fullpath);
//void loadUADefaults(TRAIT_DATA_TYPE& data);
//int  GetDataSize(TRAIT_DATA_TYPE& data);

#define MESSAGEMAP_FILE_NAME "messageMap.txt"

#ifdef USE_TRAITS
void Clear(TRAIT_DATA_TYPE& data);
#endif


// Limits imposed on a character or race or whatever.
// Packaged as a DWORD to allow easy transportation between functions.
// Inline functions to extract the pieces.
// Inline function to assemble the pieces.
//lint -e701
typedef DWORD ABILITYLIMITS;
inline int ABILITYMINBASE (ABILITYLIMITS x) { return ((x)>>24)&0xff; };
inline int ABILITYMINMOD  (ABILITYLIMITS x) { return ((x)>>16)&0xff; };
inline int ABILITYMAXBASE (ABILITYLIMITS x) { return ((x)>> 8)&0xff; };
inline int ABILITYMAXMOD  (ABILITYLIMITS x) { return ((x)>> 0)&0xff; };
inline ABILITYLIMITS ASSEMBLEABILITYLIMITS (int n, int nm, int x, int xm)
{  return (((((((n)&0xff)<<8)|((nm)&0xff))<<8)|((x)&0xff))<<8)|((xm)&0xff); };
//lint +e701





class RUNTIME_ENVIRONMENT
{
  // All directory names end with '\'.
  CString m_executableDir;  // Where the program executable is.
  CString m_rootDir;        // A folder containg designs and perhaps an editor folder
  CString m_designDir;      // A folder containing single design; usually "something.dsn"
  CString m_dataDir;        // The folder containint the levels, etc of the dungeon
  CString m_soundDir;       // The sounds folder of the design; usually "designDir\Resources"
  CString m_configDir;      // The folder in which we will find 'config.txt
  CString m_saveDir;        // The folder in which we store party and character data.
  CString m_artDir;         // The art folder of the design; usually "designDir\Resources"
  CString m_backupDir;
  bool    m_programFilesChecked;
public:
  RUNTIME_ENVIRONMENT(void);
  void    Clear(void);
  void    CheckProgramFiles(void);
  void    DefaultFoldersFromDesign(const CString& design);
  void    DefaultFoldersFromRoot(const CString& root, const CString& name);
  void    SetRootDir(const CString& rootDir) {m_rootDir = rootDir;CheckProgramFiles();};
  //Not needed in editor //void    SetExecutableDir(const CString& name);
  CString Dir(SurfaceType sType);
  CString RootDir(void)           {return m_rootDir;   };
  CString DataDir(void)           {return m_dataDir;   };
  CString DesignDir(void)         {return m_designDir; };
  CString MovieDir(void)          {return m_artDir;    };
  CString WindowArtDir(void)      {return m_artDir;    };
  CString CursorArtDir(void)      {return m_artDir;    };
  CString CombatArtDir(void)      {return m_artDir;    };
  CString MapArtDir(void)         {return m_artDir;    };
  CString IconArtDir(void)        {return m_artDir;    };
  CString BackgroundArtDir(void)  {return m_artDir;    };
  CString TitleArtDir(void)       {return m_artDir;    };
  CString SoundDir(void)          {return m_soundDir;  };
  CString AreaViewArtDir(void)    {return m_artDir;    };
  CString ExecutableDir(void)     {return m_designDir; };
  CString SaveDir(void)           {return m_saveDir;   };
  CString SmallPicDir(void)       {return m_artDir;    };
  CString ScreenCapDir(void)      {return m_designDir; };
  CString WallArtDir(void)        {return m_artDir;    };
  CString LogDir(void);
  CString DoorArtDir(void)        {return m_artDir;    };
  CString OverlayArtDir(void)     {return m_artDir;    };
  CString SpellArtDir(void)       {return m_artDir;    };
  CString ConfigDir(void)         {return m_dataDir;   };
  CString PicArtDir(void)         {return m_artDir;    };
  CString CreditsArtDir(void)     {return m_artDir;    };
  CString BackupDir(void)         {return m_backupDir; };
  CString OverlandArtDir(void)    {return m_artDir;    };
  CString SplashArtDir(void)      {return m_artDir;    };
  CString SpriteArtDir(void)      {return m_artDir;    };

  void    DesignDir(const CString& s) {m_designDir = s;};

};





extern RUNTIME_ENVIRONMENT rte;
/*
extern char global_executableDir[_MAX_PATH+1];
extern char global_rootDir[_MAX_PATH+1];     // A folder containg designs and perhaps an editor folder
extern char global_designDir[_MAX_PATH+1];   // A folder containing single design; usually "something.dsn"
//extern char global_dungeonDir[_MAX_PATH+1];  // A sub-folder of the design containing levels, databases, config, etc.
extern char global_dataDir[_MAX_PATH+1];     // The folder containint the levels, etc of the dungeon
extern char global_soundDir[_MAX_PATH+1];    // The sounds folder of the design; usually "fileDir\Sounds"
extern char global_configDir[_MAX_PATH+1];   // The folder in which we will find 'config.txt'.
extern char global_saveDir[_MAX_PATH+1];     // The folder in which we store party and character data.
extern char global_artDir[_MAX_PATH+1];      // The art folder of the design; usually "fileDir\Art"
*/






enum CONFIG_ITEM_STATUS 
{ // Little is lovely.  Higher numbered values are worse.
  CONFIG_STAT_ok=0,
  CONFIG_STAT_comment,   // Comment line.  This is ok, too.
  CONFIG_STAT_delimiter, // \(BEGIN)  and \(END)
  CONFIG_STAT_end,       // No more objects to fetch.
  CONFIG_STAT_key,       // unknown Keyword
  CONFIG_STAT_semantic,  // Overall check of the object's values failed.
  CONFIG_STAT_value,     // Illegal value for keyword
  CONFIG_STAT_syntax,    // Cannot understand intent.  This is pretty much
                         // reserved for CONFIG_FILE itself because we would 
                         // not be calling your function if we could not decode
                         // the statement.
  CONFIG_STAT_intError,  // Internal program error
  CONFIG_STAT_more,      // Possible return from user encoding functions.  It means
                         // to add key/value and return for more key/value pairs.
  CONFIG_STAT_skip ,     // Skip this line and pretend that status is CONFIG_STAT_ok 
  CONFIG_STAT_next,      // Skip this line and pretend that status is CONFIG_STAT_more
};
 
#ifdef UAFEDITOR

class EDITOR_ENVIRONMENT
{
  CString m_editorExecutableDir;  // Where the editor executable is.
  CString m_editorHelpDir;
  CString m_editorResourceDir;
  CString m_templateDir;            // An empty design for 'File/New' operation
  CString m_templateSoundDir;
  CString m_templateArtDir;
  CString m_templateDataDir;
public:
  CString Dir(SurfaceType sType);
  void DefaultFoldersFromExecutable(const CString& executableDir);
  CString EditorExecutableDir(void)   {return m_editorExecutableDir;};
  CString EditorHelpDir(void)         {return m_editorHelpDir;      };
  CString EditorMapArtDir(void)       {return m_editorResourceDir;  };
  CString EditorWindowArtDir(void)    {return m_editorResourceDir;  };

  CString TemplateDir(void)             {return m_templateDir;      };
  CString TemplateIconArtDir(void)      {return m_templateArtDir;   };
  CString TemplateSmallPicDir(void)     {return m_templateArtDir;   };
  CString TemplateAreaArtDir(void)      {return m_templateArtDir;   };  
  CString TemplateSoundDir(void)        {return m_templateSoundDir; };
  CString TemplateAreaViewArtDir(void)  {return m_templateArtDir;   };
  CString TemplateBackgroundArtDir(void){return m_templateArtDir;   };
  CString TemplateOverlandArtDir(void)  {return m_templateArtDir;   };
  CString TemplateMissileArtDir(void)   {return m_templateArtDir;   };
  CString TemplateCombatArtDir(void)    {return m_templateArtDir;   };
  CString TemplateCursorArtDir(void)    {return m_templateArtDir;   };
  CString TemplateCampArtDir(void)      {return m_templateArtDir;   };
  CString TemplateSpellArtDir(void)     {return m_templateArtDir;   };
  CString TemplateWallArtDir(void)      {return m_templateArtDir;   };
  CString TemplateDoorArtDir(void)      {return m_templateArtDir;   };
  CString TemplateOverlayArtDir(void)   {return m_templateArtDir;   };
  CString TemplateSpriteArtDir(void)    {return m_templateArtDir;   };
  CString TemplateDataDir(void)         {return m_templateDataDir;  };
  CString TemplateWindowArtDir(void)    {return m_templateArtDir;   };
  CString TemplateConfigDir(void)       {return m_templateDataDir;  };
};

extern EDITOR_ENVIRONMENT ede;
/*
extern char global_editorResourceDir[_MAX_PATH+1];
*/
#endif


extern BOOL bGameServer;
extern int ExitSignaled;
extern BOOL DisableScreenSaver;
extern LONGLONG virtualGameTime;

extern int ForceSysMemUsage;

// used to report errors
enum miscErrorType {
   NoError,
   UnknownError, 
   DeadChar, 
   UnconsciousChar, 
   FledChar, 
   PetrifiedChar, 
   GoneChar,
   AnimatedChar,
   TempGoneChar,
   RunningChar,
   DyingChar,
   NoMoney, 
   NotEnoughMoney,
   MaxItemsReached, 
   TooMuchWeight,
   TakesTwoHands,
   WrongClass,
   ItemAlreadyReadied,
   NoIcons,
   NoSmallPics,
   NoDesigns,
   LevelLoadError,
   InvalidGameState,
   InvalidEventType,
   InvalidMenuState,
   InvalidTaskState,
   PartyIsFull,
   NPCPartyLimitReached,
   ItemIsReadied,
   MissingMonsterCombatIcons,
   GraphicEngineError,
   DesignVersionError,
   SaveGameVersionError,
   SaveCharVersionError,
   FileLoadError,
   FileSaveError,
   DupCharName,
   NoFreeHands,
   CharacterCreationError,
   ErrorText,
   LastMiscError 
};

extern miscErrorType miscError;
const int MAX_MISC_ERROR_MSGS = LastMiscError+1;
extern char *miscErrorText[MAX_MISC_ERROR_MSGS];

enum costFactorType { Free, 
                      Div100,Div50,Div20,Div10,Div5,Div4,Div3,Div2,Div1_5,
                      Normal,
                      Mult1_5,Mult2,Mult3,Mult4,Mult5,Mult10,Mult20,Mult50,Mult100 };
const int NUM_COST_FACTOR_TYPES = 20;

// upper left corner of viewable area, just inside the border
enum { ViewportScreenX = 14, ViewportScreenY = 16 };

// Size of one combat terrain tile
enum { COMBAT_TILE_WIDTH=48, COMBAT_TILE_HEIGHT=48 };

const int MAX_EVENTS = 0x00FFFFFF; // per level
const int MAX_SPECIAL_KEYS = 8;
const int MAX_SPECIAL_ITEMS = 0x0FFFFFFF;
const int MAX_QUESTS = 0x0FFFFFFF;
const int MAX_ZONES = 16;
const int MAX_STEP_EVENTS = 255; // per level
const int MAX_TIME_EVENTS = 255; // per level

// must be a multiple of 16!
const int MAX_WALLSETS = 192;				//manikus
const int MAX_COLOR_SLOTS = MAX_WALLSETS; // should be as large as max_wallsets

//<<<<<<< Externs.h
//const int MAX_BACKGROUNDS =  96;
//=======
const int MAX_BACKGROUNDS = 192;	//manikus
//>>>>>>> 1.139

#define MAXTASK 30

// object types
enum ObjectTypes { ObjectNone      = 0x00, 
                   ObjectItem      = 0x01, 
                   ObjectSpell     = 0x02, 
                   ObjectMonster   = 0x04, 
                   ObjectCharacter = 0x08,
                   ObjectAll       = 0xFF };

// special item, key, quest flags
//const WORD ITEM_FLAG = 0x01;
//const WORD KEY_FLAG = 0x02;
//const WORD QUEST_FLAG = 0x04;
// Changed to enum july 2009 - PRS
enum QUEST_TYPE
{
  NO_FLAG    = 0x00,
  ITEM_FLAG  = 0x01,
  KEY_FLAG   = 0x02,
  QUEST_FLAG = 0x04
};

enum startEquipType { seNone, sePoor, seModest, seAverage, seProsperous, seP1, seP2, seP3, seP4 };
const int NUM_START_EQUIP_TYPES = 9;

#ifdef UAFEDITOR
enum undeadType { NotUndead, Skeleton, Zombie, Ghoul, Shadow, Wight, Ghast, Wraith, Mummy,
						Spectre, Vampire, Ghost, Lich, Special };
const int NUM_UNDEAD_TYPES = 14;
#endif

const int MAX_ENTRY_POINTS = 8;
const int MAX_LEVELS = 255;

// Max size for an area map
const int MAX_AREA_WIDTH = 100;
const int MAX_AREA_HEIGHT = 100;

// Max qty of non-appraised gem/jewelry
const int MAX_NON_APPRAISED_GEM_JEWELRY = INT_MAX-1;

// Size of the combat map
extern int MAX_TERRAIN_WIDTH;
extern int MAX_TERRAIN_HEIGHT;


// width of stats box on combat screen
extern int COMBAT_STAT_BOX_WIDTH;
extern int TILES_HORZ;
extern int TILES_VERT;
extern int CombatScreenX;
extern int CombatScreenY;

// max player-created characters in party
const int MAX_PARTY_PCS = 12;
// max characters + 2 NPC's added using ADD_NPC event
const int MAX_PARTY_MEMBERS = 12;


#ifdef UAFEngine
int GetMaxPartyPCs();
int GetMaxPartyMembers();
int GetMinPartyPCs();
#endif

// Max allocated sound buffers.
// Includes all global sounds - always allocated, 
// but TS_WALK and TS_COMBAT sounds are never in memory
// at the same time.
// Also includes dynamic buffers for events.
//
const int MAX_SOUND_BUFFERS = 50;

//const int DEFAULT_SUMMONED_MONSTER = 1;
#define DEFAULT_SUMMONED_MONSTER ""


// used to determine encumbrance of carried coins
//extern int COINS_PER_POUND;

// max steps the path mgr will examine before giving
// up on pathing to target
extern int MAX_STEPS;

#define EDITOR_APP_EVENT "UAFWINED_EVENT"
#define ENGINE_APP_EVENT "UAFWINENG_EVENT"

// These were moved to externs.h because so many
// files needed just these four symbols from char.h
// and monster.h.
#define CHAR_TYPE           ((BYTE)1)
#define NPC_TYPE            ((BYTE)2)
#define MONSTER_TYPE        ((BYTE)3)
#define FAKE_CHARACTER_TYPE ((BYTE)127)
#define IN_PARTY_TYPE       ((BYTE)128)

const int NO_MONSTER = -1;

extern BOOL UseCustomCursor;


#ifdef UAFEDITOR
// baseClass flags
const int MagicUserFlag = 1;
const int ClericFlag    = 2;
const int ThiefFlag     = 4;
const int FighterFlag   = 8;
const int PaladinFlag   = 16;
const int RangerFlag    = 32;
const int DruidFlag     = 64;
const int AllClasses    = 255;
const int NumBaseClass = 7;
#endif 

/*


int SpellClassToCharClass(WORD SpellClassFlag);
//lint -e527

/*
inline BYTE IndexToFlag(BYTE index)
{
  switch (index)
  {
  case 0: return MagicUserFlag;
  case 1: return ClericFlag;
  case 2: return ThiefFlag;
  case 3: return FighterFlag;
  case 4: return PaladinFlag;
  case 5: return RangerFlag;
  case 6: return DruidFlag;
  case 7: return AllClasses;
  default: ASS ERT(FALSE); return 0;
  }
}
*/

/*
inline BYTE FlagToIndex(BYTE flag)
{ 
  switch (flag)
  {
  case MagicUserFlag: return 0;
  case ClericFlag:    return 1;
  case ThiefFlag:     return 2;
  case FighterFlag:   return 3;
  case PaladinFlag:   return 4;
  case RangerFlag:    return 5;
  case DruidFlag:     return 6;
  case AllClasses:    return 7;
  default: ASS ERT(FALSE); return 0;
  }
}
*/

CString ClassFlagToText(WORD types);

//lint +e527
extern char *ClassFlagText[8];

const int NO_DUDE = -1;

// must match order of eventFacingType
enum { FACE_NORTH, FACE_EAST, FACE_SOUTH, FACE_WEST,
       FACE_NW,    FACE_NE,   FACE_SW,    FACE_SE };

// starting coords of textbox
extern int textboxX;
extern int textboxY;
extern int dsgnVerX;
extern int dsgnVerY;

extern int INTER_CHARACTER_DELAY; // in millisecs

extern COLORREF White;
extern COLORREF Black;

extern COLORREF Green;
extern COLORREF Red;
extern COLORREF Blue;
extern COLORREF Yellow;
extern COLORREF Orange;

extern COLORREF CombatGreen;
extern COLORREF CombatRed;
extern COLORREF CombatYellow;
extern COLORREF CombatBlue;
extern COLORREF CombatOrange;

extern COLORREF CustomColor;

extern COLORREF KeyboardCharColor;
extern BOOL UseKeyboardShortcuts;

extern int deltaX[4];
extern int deltaY[4];

extern HANDLE hShutdown;

class INTEGER
{
  int integer;
};

/*
#define DEFINE_CARRAY_ACCESS_FUNCTIONS(name,ID,id,array,data,container) \
  int        Locate##name (const ID& id) const;                         \
  data *Get##name    (const ID& id)                                     \
  {                                                                     \
    return Get##name(Locate##name(id));                                 \
  };                                                                    \
  const data *Peek##name   (const ID& id) const                         \
  {                                                                     \
    return Peek##name(Locate##name(id));                                \
  };                                                                    \
  data *Get##name    (int i)                                            \
  {                                                                     \
    return (i>=0)?&array[i]:NULL;                                       \
  };                                                                    \
  const data *Peek##name   (int i) const                                \
  {                                                                     \
    return const_cast<container*>(this)->Get##name(i);                  \
  };                                                                    \
  int Get##name##Count(void) const                                      \
  {                                                                     \
    return array.GetSize();                                             \
  };                                                                    \
  int Add(const data& d){return array.Add(const_cast<data&>(d));};

*/
template <class TYPE, class ARG_TYPE>
class mCArray : public CArray<TYPE,ARG_TYPE>
{
  // A private operator....illegal to use.
  // This causes a silent copy of the data!!!!  We were doing
  // thousands of these copies simply because we were peeking into
  // the array elements.
  ARG_TYPE operator[](int indx) const {die("Illegal mCArray Reference");};
};



#define DEFINE_mCARRAY_ACCESS_FUNCTIONS(name,ID,id,array,data,container) \
  int        Locate##name (const ID& id) const;                          \
  data *Get##name    (const ID& id)                                      \
  {                                                                      \
    return Get##name(Locate##name(id));                                  \
  };                                                                     \
  const data *Peek##name   (const ID& id) const                          \
  {                                                                      \
    return Peek##name(Locate##name(id));                                 \
  };                                                                     \
  data *Get##name    (int i)                                             \
  {                                                                      \
    return (i>=0)?array.GetData()+i:NULL;                                \
  };                                                                     \
  const data *Peek##name   (int i) const                                 \
  {                                                                      \
    return const_cast<container*>(this)->Get##name(i);                   \
  };                                                                     \
  int Get##name##Count(void) const                                       \
  {                                                                      \
    return array.GetSize();                                              \
  };                                                                     \
  int Add(const data& d){return array.Add(const_cast<data&>(d));};

template <class TYPE, class ARG_TYPE>
class mCList : public CList<TYPE,ARG_TYPE>
{
  // A private operator....illegal to use.
  // This causes a silent copy of the data!!!!  We were doing
  // thousands of these copies simply because we were peeking into
  // the array elements.
  void death(void) const {die("Illegal m_CList constant operation");};
  TYPE GetAt(POSITION& pos) const {death();};
	TYPE GetNext(POSITION& rPosition) const{death();};
	TYPE GetPrev(POSITION& rPosition) const{death();};
	TYPE GetHead(void) const{death();};
	TYPE GetTail(void) const{death();};
public:
  const TYPE *PeekAt(POSITION& pos) const
  {
    return const_cast<mCList<TYPE,ARG_TYPE> *>(this)->GetAt(pos);
  };
  const TYPE *PeekNext(POSITION& pos) const 
  {
    return const_cast<mCList<TYPE,ARG_TYPE> *>(this)->GetNext(pos);
  };
  const TYPE *PeekPrev(POSITION& pos) const
  {
    return const_cast<mCList<TYPE,ARG_TYPE> *>(this)->GetPrev(pos);
  };
  const TYPE *PeekTail(void) const 
  {
    return const_cast<mCList<TYPE,ARG_TYPE> *>(this)->GetTail();
  };
	TYPE *GetNext(POSITION& pos)
  {
    return &CList<TYPE,ARG_TYPE>::GetNext(pos);
  };
	TYPE *GetPrev(POSITION& pos)
  {
    return &CList<TYPE,ARG_TYPE>::GetPrev(pos);
  };
	TYPE *GetHead(void)
  {
    return &CList<TYPE,ARG_TYPE>::GetHead();
  };
	TYPE *GetTail(void)
  {
    return &CList<TYPE,ARG_TYPE>::GetTail();
  };
  TYPE *GetAt(POSITION& pos)
  {
    return &CList<TYPE,ARG_TYPE>::GetAt(pos);
  };
};

/*

#define DEFINE_mCList_ACCESS_FUNCTIONS(name,ID,id,array,data,container)  \
  int        Locate##name (const ID& id) const;                          \
  data *Get##name    (const ID& id)                                      \
  {                                                                      \
    return Get##name(Locate##name(id));                                  \
  };                                                                     \
  const data *Peek##name   (const ID& id) const                          \
  {                                                                      \
    return Peek##name(Locate##name(id));                                 \
  };                                                                     \
  data *Get##name    (int i)                                             \
  {                                                                      \
    return (i>=0)?array.GetData()+i:NULL;                                \
  };                                                                     \
  const data *Peek##name   (int i) const                                 \
  {                                                                      \
    return const_cast<container*>(this)->Get##name(i);                   \
  };                                                                     \
  int Get##name##Count(void) const                                       \
  {                                                                      \
    return array.GetSize();                                              \
  };                                                                     \
  int Add(const data& d){return array.Add(const_cast<data&>(d));};
*/


class BASECLASS_ID:public CString
{;
public:
  BASECLASS_ID(void):CString(){};
  BOOL  IsValidBaseclass(void) const;
  void  operator =(const CString& str) { CString *p = this; *p=str;};
  void  Clear(void) {this->Empty();};
  const CString& UniqueName(void) const {const CString *p = this; return *p;};
  BOOL  IsNoBaseclass(void) const {const CString *p; p=this; return p->IsEmpty();};
  int GetDataSize(void) const {return sizeof(*this);};
  int Serialize(CAR& car); // Return 0 if all is well.
  int Serialize(CArchive& car); // Return 0 if all is well.
};
/*
class BASECLASS_REFERENCE
{
public:
  BASECLASS_ID    m_baseclassID;
public:
  BASECLASS_REFERENCE(void);
  ~BASECLASS_REFERENCE(void);
  BASECLASS_REFERENCE& operator =(const BASECLASS_REFERENCE& src);
  void operator =(const BASECLASS_ID& baseclassID){m_baseclassID = baseclassID;};
  void Clear(void);
  int GetDataSize(void);
  void PreSerialize(BOOL IsStoring);
  int Serialize(CAR& car);
  void PostSerialize(BOOL IsStoring);
};
*/
class BASECLASS_LIST
{
public:
  mCArray<BASECLASS_ID, BASECLASS_ID&> baseclasses;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(BaseclassID,    /* array data name      */ \
                                 BASECLASS_ID,   /* Search variable type */ \
                                 baseclassID,    /* search variable name */ \
                                 baseclasses,    /* array name           */ \
                                 BASECLASS_ID,   /* array data type      */ \
                                 BASECLASS_LIST) /* type of *this        */

  void Clear(void) {baseclasses.RemoveAll();};
  int GetCount(void) const {return baseclasses.GetSize();};
  //int   LocateBaseclassID(const BASECLASS_ID& baseclassID) const;
  //const BASECLASS_ID *PeekBaseclassID(int i) const {return &const_cast<BASECLASS_LIST*>(this)->baseclasses[i];};
  //      BASECLASS_ID *GetBaseclassID(int i)        {return &baseclasses[i];};
  //int GetSize(void) const;
  void Remove(const BASECLASS_ID& baseclassID);
  void operator =(const BASECLASS_LIST& baseclassList);
  void operator =(const CArray<BASECLASS_ID, BASECLASS_ID&> *pBaseclasses)
    {
      baseclasses.RemoveAll();
      baseclasses.Append(*pBaseclasses);
    };
  BOOL operator == (const BASECLASS_LIST& baseclassList) const;
  BOOL operator != (const BASECLASS_LIST& baseclassList) const {return !(*this == baseclassList);};
  BOOL operator &  (const BASECLASS_LIST& baseclassList) const;  //Any overlap?
  void Serialize(CArchive& ar);
  void Serialize(CAR& car);
};

class CLASS_ID:public CString
{;
public:
  CLASS_ID(void):CString(){};
  BOOL  IsValidClass(void) const;
  void  operator =(const CString& str) { CString *p = this; *p=str;};
  void  Clear(void) {this->Empty();};
  const CString& UniqueName(void) const {const CString *p = this; return *p;};
  BOOL  IsNoClass(void) const {const CString *p; p=this; return p->IsEmpty();};
  CString UniqueName(void){return *this;};
  int Serialize(CAR& car); // Return 0 if all is well.
  int Serialize(CArchive& car); // Return 0 if all is well.
};

class RACE_ID:public CString
{;
public:
  RACE_ID(void):CString(){};
  BOOL  IsValidRace(void) const;
  void  operator =(const CString& str) { CString *p = this; *p=str;};
  void  Clear(void) {this->Empty();};
  const CString& UniqueName(void) const {const CString *p = this; return *p;};
  BOOL  IsNoRace(void) const {const CString *p; p=this; return p->IsEmpty();};
  int GetDataSize(void) const {return sizeof(*this) + UniqueName().GetLength();};
};


class ABILITY_ID:public CString  // Like Strength, Wisdom, etc.
{;
public:
  ABILITY_ID(void):CString(){};
  BOOL  IsValidAbility(void) const {return !IsNoAbility();};
  ABILITY_ID&  operator =(const CString& str) { CString *p = this; *p=str; return *this;};
  void  Clear(void) {this->Empty();};
  const CString& UniqueName(void) const {const CString *p = this; return *p;};
  BOOL  IsNoAbility(void) const {const CString *p; p=this; return p->IsEmpty();};
  int GetDataSize(void) const {return sizeof(*this) + UniqueName().GetLength();};
};



class SPELL_ID:public CString
{
public:
  //SPELL_ID(const CString& uniqueName):CString(uniqueName) {};
  SPELL_ID(void):CString(){};
  BOOL  IsValidSpell(void) const;
  void  operator =(const CString& str) { CString *p = this; *p=str;};
  void  Clear(void) {this->Empty();};
  const CString& UniqueName(void) const {const CString *p = this; return *p;};
  BOOL  IsNoSpell(void) const {const CString *p; p=this; return p->IsEmpty();};
};

class SPELLGROUP_ID:public CString
{;
public:
  SPELLGROUP_ID(void):CString(){};
  BOOL  IsValidSpellgroup(void) const;
  void  operator =(const CString& str) { CString *p = this; *p=str;};
  void  Clear(void) {this->Empty();};
  const CString& UniqueName(void) const {const CString *p = this; return *p;};
  BOOL  IsNoSpellgroup(void) const {const CString *p; p=this; return p->IsEmpty();};
  int GetDataSize(void) const {return sizeof(*this) + UniqueName().GetLength();};
};



class SCHOOL_ID:public CString
{
public:
  //SPELL_ID(const CString& uniqueName):CString(uniqueName) {};
  SCHOOL_ID(void):CString(){};
  BOOL  IsValidSchool(void) const;
  void  operator =(const CString& str) { CString *p = this; *p=str;};
  void  Clear(void) {this->Empty();};
  const CString& UniqueName(void) const {const CString *p = this; return *p;};
  BOOL  IsNoSchool(void) const {const CString *p; p=this; return p->IsEmpty();};
};


class SCHOOL_LIST
{
  CArray<SCHOOL_ID, SCHOOL_ID&> schools;
public:
  int GetCount(void) const {return schools.GetSize();};
  int LocateSchool(const SCHOOL_ID& schoolID) const;
  const SCHOOL_ID *PeekSchool(int i) const {return &const_cast<SCHOOL_LIST*>(this)->schools[i];};
  void Add(const SCHOOL_ID& schoolID);
  void Clear(void) {schools.RemoveAll();};
};


#define GemString     "_$GEM$_"
#define JewelryString "_$JEWELRY$_"

class ITEM_ID:public CString
{
  friend class ITEM_DATA_TYPE;
  mutable int hint;
public:
  //ITEM_ID(const CString& uniqueName):CString(uniqueName) {};
  ITEM_ID(void):CString(){hint=-1;};
  BOOL    IsValidItem(void) const;
  BOOL    IsGemType(void) const { return *this == GemString;};
  BOOL    IsJewelryType(void) const { return *this == JewelryString;};
  BOOL    IsMoney(void) const {return IsGemType() || IsJewelryType();};
  void    operator =(const CString& str) { CString *p = this; *p=str; hint=-1; };
  void    Clear(void) {this->Empty();};
  const   CString& UniqueName(void) const {const CString *p = this; return *p;};
  BOOL    IsNoItem(void) const {const CString *p; p=this; return p->IsEmpty();};
  ITEM_ID CreateGem(CString name)     {*this = name; return *this;};
  ITEM_ID CreateJewelry(CString name) {*this = name; return *this;};
};


class MONSTER_ID:public CString
{
public:
  MONSTER_ID(void):CString(){};
  BOOL    IsValidMonster(void) const;
  void    operator =(const CString& str) { CString *p = this; *p=str;};
  void    Clear(void) {this->Empty();};
  const   CString& UniqueName(void) const {const CString *p = this; return *p;};
  BOOL    IsNoMonster(void) const {const CString *p; p=this; return p->IsEmpty();};
};

class CHARACTER_ID: public CString
{
public:
  CHARACTER_ID(void):CString(){};
  BOOL    IsValidNPC(void) const;
  void    operator =(const CString& str) { CString *p = this; *p=str;};
  void    Clear(void) {this->Empty();};
  const   CString& UniqueName(void) const {const CString *p = this; return *p;};
};

class SKILL_ID: public CString
{
public:
  SKILL_ID(void):CString(){};
  void    operator =(const CString& str) { CString *p = this; *p=str;};
  void    Clear(void) {this->Empty();};
  const   CString& UniqueName(void) const {const CString *p = this; return *p;};
};

class CLASS_DATA;
class BASECLASS_ID;
class BASE_CLASS_DATA;
class RACE_DATA;

struct SKILL_COMPUTATION
{
  const  CHARACTER       *pChar;
         SKILL_ID         skillID;
  const  RACE_DATA       *pRace;
  const  CLASS_DATA      *pClass;
         BASECLASS_ID     baseclassID;  // or ""
  //const  BASE_CLASS_DATA *pBaseclass; // or NULL
  const  BASE_CLASS_DATA **ppBestBaseclass; // or NULL
         int              baseclassLevel;
         int              baseclassValue;
         int              raceValue;
         int              baseVal;   // Best of races and baseclasses
         double           bestRaceAdj;
         double           bestBaseclassAdj;
         double           abilityAdj;
         double           tempAdj;
         double           finalAdjustedValue;
         bool             minimize;
         bool             includeTempAdj;

  SKILL_COMPUTATION(const CHARACTER *pCH, const CString& skid, bool minimize, bool includeTempAdj);
};

class SUBSTRINGS:public CString
{
public:
  void operator =(const CString& str){CString *p = this; *p = str;};
  bool NextField(int *col, CString *result) const;
  bool HeadAndTail(CString *head, CString *tail) const;
};

class ToHitComputation;

class BASE_CLASS_DATA;
class RACE_DATA;

//BOOL GetSkillBaseValue(const SKILL_ID& skillID,
//                       const CHARACTER *pChar,
//                       const BASE_CLASS_DATA **ppBestBaseclass,
//                             int             *pBestBaseVAlue,
//                             int             *value,
//                             bool             minimize);
void GetSkillBaseValue(SKILL_COMPUTATION& SC);

//BOOL GetSkillAdjustment(const SKILL_ID& skillID,
//                       const CHARACTER *pChar,
//                       const BASE_CLASS_DATA **ppBestBaseclass,
//                             int             *pBestBaseVAlue,
//                             int             *value,
//                             bool             minimize);
BOOL GetSkillAdjustment(SKILL_COMPUTATION& SC);


//BOOL GetAdjSkillValue(const SKILL_ID&        skillID,
//                     const CHARACTER       *pChar,
//                     const BASE_CLASS_DATA **pBestBaseclass,
//                           int             *pBestBaseValue,
//                           int             *adjValue,
//                           bool             minimize);
void GetAdjSkillValue(SKILL_COMPUTATION& SC);


class SOUND_BYTE:public CString
{
  mutable int hSound;
public:
  SOUND_BYTE(void):CString() {hSound = -1;};
  //SOUND_BYTE(int h):CString() {hSound = h;};
  void operator =(const CString& name) {hSound=-1; CString *p=this; *p=name;};
  BOOL Play() const;
  void TotalDisable(void){hSound = -2;};
  void Clear(void);
};





//extern "C" /* disable C++ name decorations */
//{ 
  #define EA_NULL     0x000
  #define EA_ENEMY    0x001
  #define EA_ALLY     0x002
  #define EA_LOCATION 0x004
    
  #define FLAG_COMBAT             0x001
  #define FLAG_NONCOMBAT          0x002
  #define FLAG_XY                 0x004
  #define FLAG_PARTY_MEMBER       0x008
  #define FLAG_ITEM               0x010
  #define FLAG_MONSTER            0x020
  #define FLAG_NPC                0x040
  #define FLAG_FAKE_COMBATANT     0x080
  #define FLAG_COMBATANT          0x100      // Combatant versus character
  #define FLAG_CREATED_CHARACTER  0x200
  #define FLAG_FAKE_CHARACTER     0x400
  #define FLAG_BISHOP             0x800

  #define LEVEL_UNKNOWN           0xffffffff
enum ActorInstanceType
{
  InstanceType_Unknown           = 0,
  InstanceType_UniquePartyID     = 1,
  InstanceType_OrigIndex         = 2,
  InstanceType_Invalid           = 3,
  InstanceType_CombatantIndex    = 4,
  InstanceType_GlobalDataIndex   = 5,
  InstanceType_NewCombatant      = 6,
#ifdef SpellInitiationScript
  InstanceType_NullActor         = 7,  // And any other flags are meaningless
#endif
};

enum ActorInstanceOffsets
{
  NewCombatantInstanceOffset = 10000,
};

  struct ActorType
  {
    //ActorType( unsigned long iEnemyAlly,
    //           unsigned long iRace,
    //           unsigned long iClass,
    //           unsigned long iFlags,
    //           unsigned long iInstance,
    //           unsigned long iLevel,
    //           ActorInstanceType iInstanceType
    //         )
    //{
    //  Race = iRace;
    //  Class = iClass;
    //  Flags = iFlags;
    //  Instance = iInstance;
    //  Level = iLevel;
    //  EnemyAlly=iEnemyAlly;
    //  instanceType = iInstanceType;
    //};
    ActorType(int iEnemyAlly, ActorInstanceType iInstanceType)
    {
      Clear();
      instanceType = iInstanceType;
      EnemyAlly = iEnemyAlly;
    };
    ActorType(void){ Clear(); };

    void Clear();
    BOOL operator==(const ActorType &src) const;
    BOOL operator!=(const ActorType &src) const;
    void Serialize(CAR &ar, double ver);
    void Serialize(CArchive &ar, double ver);
    void XYSet(unsigned short x, unsigned short y);
    void XYGet(unsigned short &x, unsigned short &y);
    CString ToString(void) const {return CString((LPCTSTR)(this), sizeof (ActorType));};
    void FromString(const CString& str);


    void SetCombatPartySrc(BOOL friendly=TRUE);
    void SetCombatNPCSrc(BOOL friendly=FALSE);
    void SetCombatMonsterSrc(BOOL friendly=FALSE);
    
    void SetPartySrc(BOOL friendly=TRUE);
    void SetItemSrc();
    void SetNPCSrc(BOOL friendly=FALSE);
    void SetCreatedCharacterSrc(BOOL friendly=FALSE);
    void SetFakeCharacterSrc(BOOL friendly=FALSE);
    void SetMonsterSrc(BOOL friendly=FALSE);
    void SetFakeCombatantSrc(BOOL friendly=FALSE);
    void SetBishopSrc(BOOL friendly=TRUE);

    BOOL IsCombatSrc() const;
    BOOL IsPartySrc() const;
    BOOL IsNPCSrc() const;
    BOOL IsMonsterSrc() const;
    BOOL IsItemSrc() const;
    BOOL IsFakeCombatantSrc() const;
    BOOL IsBishopSrc() const;
    BOOL IsFakeCharacterSrc() const;
    BOOL IsCreatedCharacterSrc() const;
    BOOL IsCombatant() const;
    int  DetermineLevel(const SCHOOL_ID& schoolID, int spellLevel);

    unsigned long GetInstance() const;
    unsigned long GetSrc() const; 
    BOOL IsValidSrc() const;
#ifdef SpellInitiationScript
    void ExitCombat(void);
#endif

    unsigned long EnemyAlly;
    //unsigned long Race;
    //unsigned long Class;
    unsigned long Flags;
    unsigned long Instance;
    unsigned long Level;
    ActorInstanceType instanceType;
    CLASS_ID      m_classID;
    ITEM_ID       m_itemID;
    MONSTER_ID    m_monsterID;
    CHARACTER_ID  m_characterID;
    RACE_ID       m_raceID;
    SPELL_ID      m_spellID;
    SCHOOL_ID     m_schoolID;
    bool          m_canFinishCasting;
  };
  
  extern const ActorType NULL_ACTOR;
  extern const ActorType INVALID_ACTOR;

  // de-reference context data and return indicated CHARACTER data
  CHARACTER &GetCharacterContext(ActorType X, const char *msg);
  // de-reference context data and return indicated COMBATANT data
  COMBATANT &GetCombatCharacterContext(const ActorType& X);
  // de-reference context data and return indicated target CHARACTER data
  CHARACTER &GetTargetCharacterContext(/*ActorType X*/);
  CHARACTER *GetCurrentlyActiveContext(const ActorType *pActor, const char *msg);


  // access to global character context data
  extern void ClearCharContextStack(void);           // pop all
  extern int HaveCharContext(void);

  extern void SetCharContext(const ActorType *data); // push context
  extern void ClearCharContext(void);         // pop  context
  extern ActorType GetCharContext(void);      // get  context head

  // access to global target context data
  extern int HaveTargetContext(void);
  extern void SetTargetContext(const ActorType *data);
  extern void ClearTargetContext(void);
  extern ActorType GetTargetContext(void);

//} // extern 'C'

CString GetActorDebugText(ActorType &actor);

void SpellActivate(const PENDING_SPELL &data, bool abortIfSetPartyXY, ToHitComputation *pToHitComputation);
BOOL NeedSpellTargeting(SPELL_DATA *pData);


CHARACTER &GetPartyMember(int index);
COMBATANT *GetCombatantPtr(int index);
int InvokeSpellOnCombatant(int caster, int target, int activespellkey, ToHitComputation *pToHitComputation);

extern int net;

extern BOOL PlaySoundEffects;
extern BOOL PlayEventSounds;
extern BOOL PlayEffectsDuringEvents;

extern BOOL bActive;     // is application active?
extern BOOL UseErrorLog;
extern int LogDatabaseErrors;
extern int LogCompileErrors; // In databases
extern BOOL UseOldAVStyle;
//extern BOOL UseAVZoneStyle;

enum WRITE_DEBUG_DIALOG
{
  WRITE_DEBUG_DIALOG_NONE = 0,
  WRITE_DEBUG_DIALOG_WARNING = 1,
};

#if (defined UAFEDITOR) || (defined UAFEngine)
  void WriteDebugString(const char *text, ...);
  extern WRITE_DEBUG_DIALOG writeDebugDialog;
  extern int debugSeverity;
#else
#define WriteDebugString TRACE
#endif

void seedMT(DWORD seed);
float RollDice( int sides, int numTimes, float bonus=0.0);
BOOL IsFileAlreadyOpen(const char *filename);
BOOL FileExists(const char *file);
BOOL FolderIsEmpty(const CString& folder);
int FileSize(const char *file);
CString GetDesignName();
int GetDesignCharStartExp();
int GetDesignCharStartExpType();
CString GetSpecialItemName(int i);
CString GetSpecialKeyName(int i);
int GetCurrentLevel();
int GetDesignStartPlatinum();
int GetDesignStartElectrum();
int GetDesignStartGold();
int GetDesignStartSilver();
int GetDesignStartCopper();
int GetDesignStartGems();
int GetDesignStartJewelry();
double GetRunAsVersion(int flag);
int GetCurrSkillLevel();
int GetAllExpMod();
int GetMonsterExpMod();
int GetMonsterQtyMod();
int GetMonsterHDMod();
void DoTaskStackTrace();
BOOL IsCombatActive();

void StripFilenamePath(CString& filename);
BOOL StripOneFolderFromFilenamePath(const CString &filename, CString &path);
CString GetFilenameDrive(CString &filename);
BOOL GetFilenamePath(CString &filename, CString &path);
void GetFilenameExt(CString &filename, CString &ext);

void AddFolderToPath(CString &filename, const CString& dir);
void NotImplemented(int code, bool loopForever);
//void AddSoundFolderToPath(CString &filename);

BOOL SearchForFileInPath(CString &file, const char *FileExt, const CString &path, BOOL SearchSubfolders);

#ifdef UAFEngine
BOOL SearchForFile(CString& file, const CString& runtimeDir);
#else
BOOL SearchForFile(CString& 
                   file, const CString& runtimeDir, const CString &editorDir);
#endif
//BOOL SearchForSoundFile(CString& file);
CString GetDataPath(char *path=NULL); // full path: 'c:\UAF\my_design.dsn\Data\'
CString GetArtPath(char *path=NULL);  // full path: 'c:\UAF\my_design.dsn\Resource\'

BOOL FindImageWithValidExt(CString &filename, const CString& directory);
BOOL FindImageWithValidExtInFolder(CString &filename, CString &folder, BOOL recurse);

BOOL CreateSaveDirectory();
BOOL CreateDesignDirectory();
BOOL CreateCommonCharDirectory();
CString GetDesignPath(char *path=NULL); // full path: 'c:\UAF\my_design.dsn'
CString GetDesignFolder(char *path=NULL); // design folder only: 'my_design.dsn'
CString GetDesignFolderRoot(char *path=NULL); // design folder root only: 'my_design'
CString GetSavePath(char *path=NULL); // saved game path: 'c:\uaf\my_design\save'
CString GetCommonCharPath(void);
CString GetEventDescription(DWORD type);
class GameEventList;
CString GetEventIdDescription(DWORD EventId, EventSourceType type, GameEventList *pEventList=NULL);
CString GetGlobalEventIdDescription(DWORD EventId);
CString GetCopyEventIdDescription(DWORD EventId);

void EditorStatusMsg(LPCSTR text);
void SetMiscError(int error);
unsigned int GetSystemTextCharWidth();
void displayMiscError(miscErrorType error);
void DisplayText(int x, int y, 
                 int fontNumber,
                 LPCSTR text, 
                 // prs 20191220COLORREF color=White, 
                 FONT_COLOR_NUM colorNum,
                 BOOL customColor,
                 BOOL highlight=FALSE, 
                 BOOL slowText=FALSE, 
                 BOOL UseBackBuffer=TRUE);
// prs 20191220  COLORREF ASCII_TO_COLOR(const CString& asc, COLORREF default);
FONT_COLOR_NUM ASCII_TO_COLOR(const CString& asc, FONT_COLOR_NUM default);
void DisplayColoredText(int x, int y, 
                        int fontNumber,
                        LPCSTR text,
                        // prs 20191220  COLORREF color = White,
                        FONT_COLOR_NUM colorNum,
                        BOOL customColor,
                        BOOL highlight,
                        BOOL slowText,
                        BOOL UseBackBuffer);
int GetEntryPtX(int ep);
int GetEntryPtY(int ep);
DWORD ApplyCostFactor(costFactorType type, DWORD price);

void CreateShutdownEvent();
BOOL IsShutdownSignaled();
void CloseShutdownEvent();
void SignalShutdown();

BOOL LoadConfigFile(const char *path);
void DeleteAllFilesInFolder(const char *path);
void CloseErrorLog();
//void WriteStackTrace();
void WriteValidateLog(FILE *pFile, const char *text, ... );
int backup(const CString& filename, CString bak="bak");
void BackupDesignFiles();
BOOL CopyFileToFolder(CString srcFilename, CString dstFolder, CString bak);
BOOL CopyFileIfDifferent(const CString& srcFilename, const CString& dstFilename, BOOL FailIfExists);
double GetDesignVersion(const char *fname);
BOOL SaveGameExists(int num);
BOOL LoadAreaViewArt(const char *file, long &surf, BOOL &UseAVZones);
unsigned long HashString(const char *s, const unsigned long hashsize);
BOOL IsTriggerTimeInCurrentTime(int start, int stop);

BOOL CheckDxInstall();

void MsgBoxError(const char *pMsg, const char *pTitle=NULL, int maxDisplay=5);
void MsgBoxInfo(const char *pMsg, const char *pTitle=NULL);
BOOL EventChainError(const char *msg, bool offerToFix);
int  MsgBoxYesNo(const char *pMsg, const char *pTitle=NULL);

// used to specify which data needs to be validated by editor
const WORD VAL_ICON = 1;
const WORD VAL_SOUNDS = 2;
const WORD VAL_SPIC = 4;   // small pic 1, 2, 3
const WORD VAL_SPIC2 = 8;
const WORD VAL_SPIC3 = 16;
const WORD VAL_ALL = 0xFFFF;

struct DICEDATA
{
  DICEDATA(int n,int s,int b) {nbr=n;sides=s;bonus=b;}
  DICEDATA() {Clear();};
  void Clear(void) {nbr=sides=bonus=0;};
  BOOL  operator ==(const DICEDATA& d) const
  {
    return (nbr==d.nbr)&&(sides==d.sides)&&(bonus==d.bonus);
  };
  int nbr;
  int sides;
  int bonus;
};

struct CHARSKILLDATA
{
  CHARSKILLDATA() 
  { 
    m_str=m_strMod=m_int=m_wis=m_dex=m_con=m_cha=0; 
    //strKey=intelKey=wisKey=dexKey=conKey=chaKey=0xffffffff; 
  }
	BYTE m_str;
	BYTE m_strMod;
	BYTE m_int;
	BYTE m_wis;
	BYTE m_dex;
	BYTE m_con;
	BYTE m_cha;
  //abilityType strKey;
  //abilityType intelKey;
  //abilityType wisKey;
  //abilityType dexKey;
  //abilityType conKey;
  //abilityType chaKey;
};


typedef CList<int,int> MAPTARGETLIST; //In combat a COMBATANT_ID; else unique party ID
typedef CList<POINT,POINT&> MAPLOCATIONLIST;

#if (defined UAFEDITOR)
CString BrowseForFolder(HWND hWnd, const char *pTitle);
BOOL GetFilename(CString& defaultFilename, CString Ext="EXE", BOOL OpenDlg=TRUE);
BOOL GetFilenameWithPreview(SurfaceType type, CString& defaultFilename);
BOOL GetSoundFilename(CString& defaultFilename, CString Ext="WAV");
void OpenTextFileForEdit(const char *path);
extern int MaxDefaultArtSlots;
#else // UAFEngine only vars
extern BOOL EditorRunning;
bool EditorMode(void);
#endif

class CUAFCommandLineInfo : public CCommandLineInfo
{
public:
  CUAFCommandLineInfo() : CCommandLineInfo() 
  { 
    m_forceAV=FALSE;
    m_initializeGame = FALSE;
    m_ConfigFilename=""; 
    m_level=-1;
    m_sx=-1;m_sy=-1;m_facing=-1; 
    m_Party.RemoveAll();
  }
  CString m_ConfigFilename;
  CString m_ScriptFilename;
  int m_level;
  int m_sx;
  int m_sy;
  int m_facing;
  BOOL m_forceAV;
  BOOL m_initializeGame;
  BOOL m_fruaConvert;
  CList<CString, CString&> m_Party;
  // virtual callback
  void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );
  
};

extern CUAFCommandLineInfo cmdLine;

// ArchiveString and DeArchiveString
//
// Empty CStrings are stored as a single
// character '*'. When loaded, the '*'
// is replaced with "" (empty string) again.
//
// The actual replacement char can be changed
// by altering ArchiveBlank. Default is '*'.
// 
// Many released versions of DC have
// used '*' so this must be supported
// in DAS regardless of what is used
// in ArchiveBlank.
//
extern CString ArchiveBlank;
#define AS(archive,cstring) \
{                           \
  if (cstring.IsEmpty())    \
    archive << ArchiveBlank;\
  else                      \
    archive << cstring;     \
}

#define DAS(archive,cstring)    \
{                               \
  archive >> cstring;         \
  if ((cstring==ArchiveBlank)||(cstring=="*"))\
    cstring="";               \
}

#ifdef UAFEngine
eventType GetCurrEventType();
void EnableUserInput(BOOL enable);
int CurrCombatTurn();
//int CurrCombatRound();
int CurrCombatInitiative();
BOOL IsCurrentCombatant(int dude);
int GetUserGameSpeed();
int GetMsgTimeDelay();
void IncNumFriendFlee();
int GetNumFriendFlee();
void IncNumFriendSlain();
int GetNumFriendSlain();
void IncNumMonsterFlee();
int GetNumMonsterFlee();
void IncNumMonsterSlain();
int GetNumMonsterSlain();
int GetNumCombatants();
void GetNumCombatants(int* pNumParty, int* pNumMonsters);
BOOL GetConfigMonsterNoFlee();
BOOL GetConfigPartyAlwaysMiss();
BOOL GetConfigPartyAlwaysHit();
BOOL GetConfigMonsterAlwaysMiss();
BOOL GetConfigMonsterAlwaysHit();
BOOL GetConfigNoMagic();
BOOL GetConfigMonsterNoCast();
BOOL GetConfigMonsterNoDeath();
BOOL GetConfigPartyNeverDies();
BOOL GetConfigMonsterPlyrControl();
BOOL GetConfigMonsterNoMove();
BOOL GetConfigPartyNoExperience();
void PlayCombatMove();
void PlayCombatTurnUndead();
void PlayCombatDeath();
void PlaceCursorOnCurrentDude();
int GetCurrentRound();
QueuedCombatantData &GetQueuedCombatants();
void GetStartTerrain(int* px, int* py);
RECT &GetCombatRect();
RECT &GetFullRect();
extern COMBAT_MSGS combatMsgs;

COMBATANT *GetLastAttackerOf(COMBATANT *pDude);
COMBATANT *GetLeastDamagedFriendly();
COMBATANT *GetLeastDamagedEnemy();
COMBATANT *GetMostDamagedFriendly();
COMBATANT *GetMostDamagedEnemy();

COMBATANT *GetNearestTo(COMBATANT *pDude);
COMBATANT *GetNearestEnemyTo(COMBATANT *pDude);

int GetCombatantX(int index);
int GetCombatantY(int index);

#endif // ifdef UAFEngine

#if _MSC_VER == 1100     // Need CRegKey implementation explicitly in VC5
/////////////////////////////////////////////////////////////////////////////
// CRegKey
// 
// Copied from "c:\Program Files\DevStudio\VC\atl\include\atlimpl.cpp"  
//
//   PRS
LONG CRegKey::Close();
LONG CRegKey::Create(HKEY hKeyParent, LPCTSTR lpszKeyName,
	LPTSTR lpszClass, DWORD dwOptions, REGSAM samDesired,
	LPSECURITY_ATTRIBUTES lpSecAttr, LPDWORD lpdwDisposition);
LONG CRegKey::Open(HKEY hKeyParent, LPCTSTR lpszKeyName, REGSAM samDesired);
LONG CRegKey::QueryValue(DWORD& dwValue, LPCTSTR lpszValueName);
LONG CRegKey::QueryValue(LPTSTR szValue, LPCTSTR lpszValueName, DWORD* pdwCount);
LONG WINAPI CRegKey::SetValue(HKEY hKeyParent, LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName);
LONG CRegKey::SetKeyValue(LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName);
HRESULT CRegKey::SetValue(LPCTSTR lpszValue, LPCTSTR lpszValueName);

#endif // CRegKey implementaion for VC5






#endif
