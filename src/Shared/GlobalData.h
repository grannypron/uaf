/******************************************************************************
* Filename: GlobalData.h
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
#ifndef __GLOBAL_DATA__
#define __GLOBAL_DATA__


#include "items.h"
#include "char.h"
#include "party.h"
#include "GameEvent.h"
#include "SoundMgr.h"

//#include "SoundMgr.h"
//#include "SharedQueue.h"
//lint -e1411

class TITLE_SCREEN
{
public:
  enum { tsSplash=0, tsFadeIn=1, tsMovie=2 };
  TITLE_SCREEN() { Clear(); }
  virtual ~TITLE_SCREEN() { Clear(); }
  void Clear()
  {
    TitleBgArt="";
    UseTrans=FALSE;
    UseBlend=FALSE;
    DisplayBy=tsFadeIn;
  }
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  void Import(JReader& jr);
#endif
  TITLE_SCREEN& operator =(const TITLE_SCREEN& src);
  TITLE_SCREEN(const TITLE_SCREEN &src) { *this = src; }

  CString TitleBgArt;
  BOOL UseTrans;
  BOOL UseBlend;
  DWORD DisplayBy;
};

class TITLE_SCREEN_DATA
{
public:
  TITLE_SCREEN_DATA() { Clear(); }
  virtual ~TITLE_SCREEN_DATA() { Clear(); }
  void Clear() { Titles.RemoveAll();  }
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  void Import(JReader& jr);
#endif
  void CrossReference(CR_LIST *pCRList, const CR_REFERENCE *pCRReference);
  
  
  TITLE_SCREEN_DATA& operator =(const TITLE_SCREEN_DATA& src);
  TITLE_SCREEN_DATA(const TITLE_SCREEN_DATA &src) { *this = src; }
  POSITION AddTitle(TITLE_SCREEN &data);
  void SetDefault(CString deftitle);

  DWORD Timeout; // zero means none
  CList<TITLE_SCREEN, TITLE_SCREEN&> Titles;
};


// lists of spells that have special
// meaning to the user interface
//
// **** Not used anymore, except when loading
// **** older designs that still have this data
// **** in the archive.
//
class SPELL_SPECIAL_DATA
{
public:
  SPELL_SPECIAL_DATA() { Clear(); }
  virtual ~SPELL_SPECIAL_DATA() { Clear(); }
  void Clear()
  {
    m_DetectMagicSpells.RemoveAll();
    m_IdentifySpells.RemoveAll();
  }
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  SPELL_SPECIAL_DATA& operator =(const SPELL_SPECIAL_DATA& src);
  SPELL_SPECIAL_DATA(const SPELL_SPECIAL_DATA &src) { *this = src; }

  //GLOBAL_SPELL_ID HasDetectMagicSpell(CHARACTER &data); // return spellData key, or NO_SPELL
  //GLOBAL_SPELL_ID HasIdentifySpell(CHARACTER &data);
  CString HasDetectMagicSpell(CHARACTER &data); // return spellData key, or NO_SPELL
  CString HasIdentifySpell(CHARACTER &data);

  //CList<GLOBAL_SPELL_ID, GLOBAL_SPELL_ID> m_DetectMagicSpells;
  //CList<GLOBAL_SPELL_ID, GLOBAL_SPELL_ID> m_IdentifySpells;
  CList<CString, CString&> m_DetectMagicSpells;
  CList<CString, CString&> m_IdentifySpells;
};

class DIFFICULTY_DATA
{
public:
  DIFFICULTY_DATA() { Clear(); }
  virtual ~DIFFICULTY_DATA() { Clear(); }
  void Clear() 
  { 
    m_Name="";
    m_ModifyHD=m_ModifyQty=m_ModifyMonsterExp=m_ModifyAllExp=FALSE;
    m_HDAmount=m_QtyAmount=m_MExpAmount=m_AExpAmount=0;
  }
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  void Import(JReader& jr);
#endif
  DIFFICULTY_DATA& operator =(const DIFFICULTY_DATA& src);
  DIFFICULTY_DATA(const DIFFICULTY_DATA &src) { *this = src; }

public:
  CString m_Name;
  BOOL m_ModifyHD;
  BOOL m_ModifyQty;
  BOOL m_ModifyMonsterExp;
  BOOL m_ModifyAllExp;
  char m_HDAmount;
  char m_QtyAmount;
  char m_MExpAmount;
  char m_AExpAmount;
};

class DIFFICULTY_LEVEL_DATA
{
public:
  enum { NUM_DIFF_LVLS=5 };

  DIFFICULTY_LEVEL_DATA() { Clear(); }
  virtual ~DIFFICULTY_LEVEL_DATA() { Clear(); }
  void Clear() { for (int i=0;i<NUM_DIFF_LVLS;i++) m_data[i].Clear(); m_defaultLvl=2; }
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  void Import(JReader& jr);
#endif

  DIFFICULTY_LEVEL_DATA& operator =(const DIFFICULTY_LEVEL_DATA& src);
  DIFFICULTY_LEVEL_DATA(const DIFFICULTY_LEVEL_DATA &src) { *this = src; }
  void SetUADefaults();
  BYTE m_defaultLvl;
  DIFFICULTY_DATA m_data[NUM_DIFF_LVLS];
};


class GLOBAL_SOUND_DATA //: public CObject
{
public:
//  DECLARE_SERIAL( GLOBAL_SOUND_DATA )
  GLOBAL_SOUND_DATA();
  virtual ~GLOBAL_SOUND_DATA() { ClearSounds(); }
  void Clear(BOOL ctor=FALSE);
  void ClearSounds();
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);

#ifdef UAFEDITOR
  void CrossReference(CR_LIST *pCRList);
  void Export(JWriter& jw);
  void Import(JReader& jr);
#endif

  GLOBAL_SOUND_DATA& operator =(const GLOBAL_SOUND_DATA& src);
  GLOBAL_SOUND_DATA(const GLOBAL_SOUND_DATA &src) { *this = src; }

  int hCharHit;
  int hCharMiss;
  int hPartyBump;
  int hPartyStep;
  int hDeathMusic;
  //int hIntroMusic;
  CString CharHit;
  CString CharMiss;
  CString PartyBump;
  CString PartyStep;
  CString DeathMusic;
  BACKGROUND_SOUNDS IntroMusic;
  BACKGROUND_SOUNDS CampMusic;
};

const int MAX_QUEST_STAGE = 0xFDE9;
const int QUEST_FAILED_STAGE = 0xFDE9;
const int QUEST_COMPLETED_STAGE = 0xFDE8;
const int NUM_QUEST_STATE_TYPE = 4;
const int NUM_QUEST_ACCEPT_TYPE = 6;
enum QuestStateType { QS_NotStarted, QS_InProg, QS_Complete, QS_Failed };

class QUEST_DATA //: public CObject
{
public:
//   DECLARE_SERIAL( QUEST_DATA )
   // ASLs are named "QUEST_DATA_ATTRIBUTES"
   QUEST_DATA() : quest_asl()
		{ Clear(); }
   virtual ~QUEST_DATA() { Clear(); }
   void Clear() { name=""; stage=0; state=QS_NotStarted; id=0;}
   void Reset() { state=QS_NotStarted; stage=0; }
   void Serialize(CArchive &ar, double ver);
   void Serialize(CAR &ar, double ver);
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   void Import(JReader& jr);
#endif
   QUEST_DATA& operator =(const QUEST_DATA& src);
   QUEST_DATA(const QUEST_DATA &src) 
     // ASLs are named "QUEST_DATA_ATTRIBUTES"
	   :quest_asl()	   
		{ *this = src; }

   CString name;
   QuestStateType state;
   WORD stage;
   int id;   
   A_ASLENTRY_L quest_asl;
};

class SPECIAL_OBJECT_DATA //: public CObject
{
public:
//   DECLARE_SERIAL( SPECIAL_OBJECT_DATA )
   SPECIAL_OBJECT_DATA() 
      // ASLs are named "SPECIAL_OBJECT_DATA_ATTRIBUTES"
		  :obj_asl(),
		  temp_asl(),
      ExamineEvent(0)
		{ Clear(); }
   virtual ~SPECIAL_OBJECT_DATA() { Clear(); }
   void Clear() { CanBeDropped=FALSE;name="";id=0;stage=0;ExamineEvent=0;ExamineLabel="EXAMINE"; }
   void Serialize(CArchive &ar, double ver);
   void Serialize(CAR &ar, double ver);
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   void Import(JReader& jr);
#endif
	 void Save(CAR& car) const;
	 void PrepareForRestore(void);
	 void Restore(CArchive& ar);
	 void Restore(CAR& car);
	 void CommitRestore(void);
   SPECIAL_OBJECT_DATA& operator =(const SPECIAL_OBJECT_DATA& src);
   SPECIAL_OBJECT_DATA(const SPECIAL_OBJECT_DATA &src) 
    // ASLs are named "SPECIAL_OBJECT_DATA_ATTRIBUTES"
		:obj_asl()	,
		temp_asl()
	   { *this = src; }
#ifdef UAFEDITOR
  CString GetSpecialObjectText();
#endif

   CString name;
   WORD stage;
   int id; 
   BOOL CanBeDropped;
   DWORD ExamineEvent;
   CString ExamineLabel;   
   A_ASLENTRY_L obj_asl;
   A_ASLENTRY_L temp_asl;
};

const int MAX_SPECIAL_OBJECTS = 0x7FFFFFFF;

class SPECIAL_OBJECT_LIST //: public CObject
{
  int m_Max;
  //POSITION pos;
  CCriticalSection m_CS;
  OrderedQueue<SPECIAL_OBJECT_DATA, MAX_SPECIAL_OBJECTS> items;
  int GetNextKey();

public:

//  DECLARE_SERIAL( SPECIAL_OBJECT_LIST )
  SPECIAL_OBJECT_LIST()  { Clear(); m_Max = MAX_SPECIAL_OBJECTS; }  
  SPECIAL_OBJECT_LIST(const int MaxObjects)  { Clear(); m_Max = MaxObjects; }
  ~SPECIAL_OBJECT_LIST() { Clear(); }

  //void Clear() { CSingleLock sLock(&m_CS, TRUE); items.RemoveAll(); }
  void Clear() { /*CSingleLock sLock(&m_CS, TRUE);*/ items.RemoveAll(); }
  //int GetCount() { CSingleLock sLock(&m_CS, TRUE); return items.GetCount(); }
  int GetCount() {/* CSingleLock sLock(&m_CS, TRUE);*/ return items.GetCount(); }

  POSITION GetHeadPosition() const { return items.GetHeadPosition(); }

        SPECIAL_OBJECT_DATA& GetAtPos(POSITION p)        { return items.GetAtPos(p); }
  const SPECIAL_OBJECT_DATA& PeekAtPos(POSITION p) const { return items.PeekAtPos(p); }


        SPECIAL_OBJECT_DATA& GetNext(POSITION &p)        { return items.GetNext(p); }
  const SPECIAL_OBJECT_DATA& PeekNext(POSITION &p) const { return *items.PeekNext(p); }

  void Serialize(CArchive &ar, double ver);
  void Serialize(CAR &ar, double ver);
  void Save(CAR& car) const;
  void Restore(CArchive& ar);
  void Restore(CAR& car);
  void CommitRestore(void);

#ifdef UAFEDITOR
  void CrossReference(CR_LIST *pCRList, CR_TYPE crType);
  void Export(JWriter& jw, const char *type);
  void Import(JReader& jr, const char *type);
#endif

  SPECIAL_OBJECT_LIST(const SPECIAL_OBJECT_LIST& src) { *this = src; }


  SPECIAL_OBJECT_LIST& operator=(const SPECIAL_OBJECT_LIST& src);
  BOOL HaveItem(int index);
  const CString GetName(const int index) const;
  void SetName(const int index, const CString &name);
  // called when loading from scripts, or
  // when adding from editor GUI
  int AddItem(SPECIAL_OBJECT_DATA &data);
  // called when loading binary monster data that
  // was saved with key info
  int AddItemWithCurrKey(const SPECIAL_OBJECT_DATA &data);
  BOOL DeleteItem(const int index);
  BOOL GetItem(const int index, SPECIAL_OBJECT_DATA &item); 
  BOOL SetItem(const int index, const SPECIAL_OBJECT_DATA &item);
  WORD GetStage(const int index) const;
  void SetStage(const int index, const WORD stage);
  int GetItemKeyUsingSequence(int SeqNum) const;
  BOOL DeleteSpecialObjectEvents(int index);
  CString GetExamineLabel(int index) const;
  DWORD GetExamineEvent(int index) const;
  void SetExamineLabel(int index, CString label);
  void SetExamineEvent(int index, DWORD event);
  BOOL CanBeDropped(int index) const;
  void SetCanBeDropped(int index, BOOL val);
};

class QUEST_LIST //: public CObject
{
  //POSITION pos;
  CCriticalSection m_CS;
  OrderedQueue<QUEST_DATA, MAX_QUESTS> quests;

  int GetNextKey();

public:
//  DECLARE_SERIAL( QUEST_LIST )
  QUEST_LIST() { Clear(); }
  ~QUEST_LIST() { Clear(); }

  //void Clear() { CSingleLock sLock(&m_CS, TRUE); quests.RemoveAll(); }
  void Clear() { /*CSingleLock sLock(&m_CS, TRUE);*/ quests.RemoveAll(); }
  //int GetCount() { CSingleLock sLock(&m_CS, TRUE); return quests.GetCount(); }
  int GetCount() { /*CSingleLock sLock(&m_CS, TRUE);*/ return quests.GetCount(); }

  QUEST_LIST(const QUEST_LIST& src) { *this = src; }
  QUEST_LIST& operator=(const QUEST_LIST& src);

  POSITION GetHeadPosition() const { return quests.GetHeadPosition(); }
        QUEST_DATA& GetAtPos(POSITION p)        { return quests.GetAtPos(p); }
  const QUEST_DATA& PeekAtPos(POSITION p) const { return quests.PeekAtPos(p); }
  QUEST_DATA& GetNext(POSITION &p) { return quests.GetNext(p); }

  void Reset(); 
  void Serialize(CArchive &ar, double ver);
  void Serialize(CAR &ar, double ver);
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  void Import(JReader& jr);
#endif

  BOOL HaveQuest(int index);
  const CString GetName(const int id) const;
  void SetName(const int id, const CString &name);
  int  FindQuest(const CString& name) const;
  WORD GetStage(const int id) const;
  WORD GetStage(const CString& questName) const;
  void SetStage(const int id, const WORD stage);
  void SetStage(const CString& questName, const WORD stage);
  void IncStage(const int id, const WORD value);
  BOOL IsPresent(const int id) const;
  BOOL IsInProgress(const int id) const; 
  BOOL IsComplete(const int id) const; 
  BOOL IsFailed(const int id) const; 
  BOOL StageEqual(const int id, const int stage) const;
  void SetNotStarted(const int id);
  void SetInProgress(const int id); 
  void SetComplete(const int id); 
  void SetFailed(const int id); 
  // called when loading from scripts, or
  // when adding from editor GUI
  int AddQuest(QUEST_DATA &data);
  // called when loading binary monster data that
  // was saved with key info
  int AddQuestWithCurrKey(const QUEST_DATA &data);
  BOOL DeleteQuest(const int index);
  BOOL GetQuest(const int id, QUEST_DATA &quest); 
  BOOL SetQuest(const int id, const QUEST_DATA &quest);
};

const int MAX_GLOBAL_VAULTS = 15;

class GLOBAL_VAULT_DATA //: public CObject
{
public:
//  DECLARE_SERIAL( GLOBAL_VAULT_DATA )
  GLOBAL_VAULT_DATA() { Clear(); }
  ~GLOBAL_VAULT_DATA() { Clear(); }

  GLOBAL_VAULT_DATA& operator =(const GLOBAL_VAULT_DATA& src);
  void Clear();
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
  void CrossReference(CR_LIST *pCRList);

  MONEY_SACK money;
  ITEM_LIST items;
};

struct ENTRY_POINT_TYPE
{
  ENTRY_POINT_TYPE() { facing=0;x=0;y=0; }
  BOOL operator ==(ENTRY_POINT_TYPE& src)
  {
    if (&src == this) return TRUE;
    if (facing != src.facing) return FALSE;
    if (x != src.x) return FALSE;
    if (y != src.y) return FALSE;
    return TRUE;
  }
  BYTE facing;
  int x;
  int y;  
};

// these settins override the area view config in each zone, and
// will be applied to the entire level
const int MAX_GLOBAL_AVSTYLES = 3;
enum GlobalAreaViewStyle { AnyView, OnlyAreaView, Only3DView };


enum OVERRIDE_TYPE
{
  // The 'USER' values are the values specified by the designer's scripts.
  // THey are nominally the 'ordinal' values, but this can be overridden
  // by global configuration values.  See SetMapOverride().
  WALL_OVERRIDE_USER = 0,
  DOOR_OVERRIDE_USER = 1,
  BACKGROUND_OVERRIDE_USER = 2,
  OVERLAY_OVERRIDE_USER = 3,
  BLOCKAGE_OVERRIDE = 4,
  NUM_OVERRIDE_TYPE = 5,

  // the following are computed from the 'USER' values
  WALL_OVERRIDE_INDEX = 6, 
  DOOR_OVERRIDE_INDEX = 7,
  BACKGROUND_OVERRIDE_INDEX = 8,
  OVERLAY_OVERRIDE_INDEX = 9,
};

enum CELL_ITEM_TYPE
{
  CIT_Bogus,
  CIT_DroppedItem    // Something the party can put into their inventory
};

class CELL_ITEM
{
  ITEM_ID m_itemID;
  int m_charges;
  int m_qty;
  int m_identified;
  int m_paid;
  CELL_ITEM_TYPE m_type;
  BYTE m_cursed;
public:
  void Serialize(CAR& car);
  CELL_ITEM(void){m_charges=m_qty=m_identified=m_cursed=m_paid=0;m_type=CIT_Bogus;};
  void operator = (const ITEM *pItem);
  void operator = (const CELL_ITEM *pCellItem);
};

class CELL_ROW_CONTENTS
{
  unsigned int m_row;
  unsigned int m_numItem;
  CELL_ITEM **m_pCellItem;
public:
  CELL_ROW_CONTENTS(void){m_row=m_numItem=0; m_pCellItem = NULL;};
  ~CELL_ROW_CONTENTS(void);
  void Serialize(CAR& car);
  void Clear(void);
  void Add(CELL_ITEM *pCellItem);
  int GetCount(void) const;
  void operator = (const CELL_ROW_CONTENTS *pCellRowContents);
  unsigned int Row(void){return m_row;};
  void Row(unsigned int row){m_row = row;};
};

class CELL_COLUMN_CONTENTS  // For a single column of the map.
{
  unsigned int m_column;
  unsigned int m_numRow;
  CELL_ROW_CONTENTS **m_pCellRowContents;
public:
  void Serialize(CAR& car);
  void Clear(void);
  unsigned int Column(void){return m_column;};
  void Column(unsigned int column){m_column = column;};
  void Add(CELL_ITEM *pCellItem, unsigned int y);
  int GetCount(unsigned int y) const;
  void operator = (const CELL_COLUMN_CONTENTS *pCellColumnContents);
  CELL_COLUMN_CONTENTS(void){m_column=m_numRow=0; m_pCellRowContents=NULL;};
  ~CELL_COLUMN_CONTENTS(void);
};

class CELL_LEVEL_CONTENTS
{
  unsigned int m_numColumn;
  CELL_COLUMN_CONTENTS** m_pCellColumnContents;
public:
  void Serialize(CAR& car);
  void PostSerialize(void) {};
  void PreSerialize(void) {};
  void Export(JWriter& jw, const char* name);
  CELL_LEVEL_CONTENTS(void) { m_pCellColumnContents = NULL; m_numColumn = 0; };
  ~CELL_LEVEL_CONTENTS(void);
  void Clear(void);
  bool IsEmpty(void) {return m_numColumn == 0;};
  void operator = (const CELL_LEVEL_CONTENTS& src);
  void Add(CELL_ITEM *pCellItem, unsigned int x, unsigned int y);
  int GetCount(unsigned int x, unsigned int y) const;
};

class CELL_OVERRIDE
{
public:
  unsigned char m_overrides[NUM_OVERRIDE_TYPE][4];  // Four facing directions.
  void Clear(void){memset(this,0xff,sizeof(*this));};
};

class ROW_OVERRIDES
{
public:
  int m_numCol;
  CELL_OVERRIDE *m_cellOverrides;
  ROW_OVERRIDES(void){m_numCol=0;m_cellOverrides=NULL;};
  ~ROW_OVERRIDES(void);
  void Serialize(CArchive& ar);
  void Serialize(CAR& car);
};

class WALL_OVERRIDES
{
  friend class LEVEL_STATS;
  int m_numRow;
  ROW_OVERRIDES **m_row;
  void Clear(void);
public:
  WALL_OVERRIDES(void){m_row=NULL;m_numRow=0;};
  ~WALL_OVERRIDES(void);
  void Serialize(CArchive& ar);
  void Serialize(CAR& car);
  void PostSerialize(void) {};
  void PreSerialize(void) {};
  void Export(JWriter& jw, const char* name);
  //int  GetMapOverride(OVERRIDE_TYPE ovt, int x, int y, int facing);
  CELL_OVERRIDE *FindCellOverride(OVERRIDE_TYPE ovt, int x, int y, int facing);
  void operator = (const WALL_OVERRIDES& src);
};

#define LEVEL_STATS_VERSION 2

class LEVEL_STATS //: public CObject
{
public:
//  DECLARE_SERIAL( LEVEL_STATS )
  LEVEL_STATS() 
	{ Clear(); }
  ~LEVEL_STATS() { Clear(); }
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
#ifdef UAFEDITOR
  CONFIG_ITEM_STATUS Export(JWriter& jw);
  CONFIG_ITEM_STATUS Import(JReader& jr);
  void CrossReference(CR_LIST *pCRList, int levelNum);
#endif
  void Save(CAR& car);
  void Restore(CArchive& ar);
  void Restore(CAR& car);
  void CommitRestore(void);
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring);

  LEVEL_STATS& operator =(const LEVEL_STATS& src);
  BOOL operator ==(LEVEL_STATS& src);
  void Clear();
  void SetDefaults();

	BYTE area_height;
	BYTE area_width;
  ENTRY_POINT_TYPE entryPoints[MAX_ENTRY_POINTS];
  BOOL used;
  BOOL overland;
  GlobalAreaViewStyle AVStyle;
  CString level_name;
  CString StepSound;
  CString BumpSound;
  BACKGROUND_SOUND_DATA bgSounds;

  int m_version; //20110910 PRS
  A_ASLENTRY_L level_asl;
  A_ASLENTRY_L temp_asl;
  WALL_OVERRIDES m_wallOverrides;
  CELL_LEVEL_CONTENTS m_cellContents;

  void SetMapOverride(OVERRIDE_TYPE ovt, int x, int y, int facing, unsigned int value);
  int  GetMapOverride(OVERRIDE_TYPE ovt, int x, int y, int facing);
};

class LEVEL_INFO //: public CObject
{
public:
//   DECLARE_SERIAL( LEVEL_INFO )
   LEVEL_INFO() { Clear(); }
   ~LEVEL_INFO() { Clear(); }

   LEVEL_INFO& operator =(const LEVEL_INFO& src);
   void Clear(); 
   void Clear(int level); 

   void Serialize(CArchive &ar);
   void Serialize(CAR &car);
   void CrossReference(CR_LIST *pCRList, const CR_REFERENCE *pCRReference);

   void Save(CAR& car);
   void Restore(CArchive& ar);
   void Restore(CAR& car);
   void CommitRestore(void);
   void SaveSounds(void);

   int GetEPX(int ep);
   int GetEPY(int ep);
   BYTE GetEPFacing(int ep);
  
   int numLevels;
   LEVEL_STATS stats[MAX_LEVELS];   
};

enum { START_EXP_VALUE=0, START_LVL_VALUE=1 };

struct COMBAT_TREASURE_DATA
{
  ITEM_LIST items;
  MONEY_SACK money;
  void Serialize(CAR& car, double version);
};

enum FONT_COLOR_NUM
{
  zeroColor = 0,
  whiteColor = 0,
  yellowColor,
  orangeColor,
  brightOrangeColor,
  redColor,
  greenColor,
  blueColor,
  cyanColor,
  blackColor,
  magentaColor,
  silverColor,  // Must be the last
  // The folowing are for internal use only
  combatWhiteColor,
  combatYellowColor,
  combatOrangeColor,
  combatRedColor,
  combatGreenColor,
  combatBlueColor,
  combatCyanColor,
  combatBlackColor,
  combatMagentaColor,
  combatSilverColor,
  customColorNum,
  BACKGROUND_FILL_COLOR_NUM,
  illegalColor
};

const int NumColorNum = silverColor - whiteColor + 1;
/*
struct AVAIL_FONT
{
  CString fontName;
  long int fontNum[NumColorNum];
  COLORREF color[NumColorNum];
  AVAIL_FONT(void)
  {
    int i;for(i=0;i<NumColorNum;i++)fontNum[i]=-1;
    fontName = "SYSTEM";
    color[whiteColor]        = RGB(255, 255, 255);
    color[yellowColor]       = RGB(255, 255, 0);
    color[orangeColor]       = RGB(255, 128, 0);
    color[redColor]          = RGB(255, 0, 0);
    color[greenColor]        = RGB(0, 255, 0);
    color[blueColor]         = RGB(128, 128, 255);
    color[cyanColor]         = RGB(0, 255, 255);
    color[blackColor]        = RGB(0, 0, 0);
    color[magentaColor]      = RGB(255, 0, 255);
    color[brightOrangeColor] = RGB(255, 128, 0);
    color[silverColor]       = RGB(192, 192, 192);
  };
  void operator =(const AVAIL_FONT& src)
  {
    NotImplemented(0x4afdb, false);
  };
  void ReleaseFonts(void);
  BOOL LoadFont(LOGFONT& logfont, COLORREF backColor, COLORREF transColor);
};
*/

class FONT_DESC
{
  friend class FONT_LIBRARY;
  CString m_fontName;
  long int m_fontHandle;  // -1 = not yet defined; -2 = Definition failed
  COLORREF m_color;
  COLORREF m_backColor;
  COLORREF m_transColor;
  bool     m_italic;
  bool     m_bold;
public:
  FONT_DESC(void) { 
    m_fontHandle = -1; 
    m_fontName.Empty();
    m_color = RGB(255, 255, 255);
    m_backColor = RGB(0, 0, 0);
    m_transColor = RGB(0, 0, 0);
    m_italic = false;
    m_bold = false;
  };
  ~FONT_DESC(void) { ReleaseFont(); };
  void operator =(const FONT_DESC& src);
  void LoadFont(void);
  void ReleaseFont(void);
};

#define NUMTEXTFONT 100

class FONT_LIBRARY
{
  long int CheckFont(int baseFontNumber, FONT_COLOR_NUM colorNum, BOOL custom);
public:
  FONT_DESC m_fonts[2*NUMTEXTFONT];
  void operator =(const FONT_LIBRARY& src);
  long int GetFont(int baseFont, FONT_COLOR_NUM colorNum, BOOL custom);
  COLORREF GetColor(int baseFont, FONT_COLOR_NUM colorNum);
  void LoadFonts(void);
  void ReleaseFonts(void);
  void SetFontColor(int baseFont, FONT_COLOR_NUM colorNum, COLORREF color);
  void SetCustomColor(COLORREF color);
  void SetFontName(int fontNum, const CString& fontName);
  void SetFontType(int fontNum, const CString& fontType);
};

class GLOBAL_STATS // 20121212 PRS: public CObject
{
public:
  //DECLARE_SERIAL( GLOBAL_STATS )
  GLOBAL_STATS();
  ~GLOBAL_STATS() { Clear(TRUE); }
  void Clear(BOOL ctor=FALSE, BOOL npcclear=TRUE);
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
#ifdef UAFEDITOR
  CONFIG_ITEM_STATUS Export(JWriter& jw); 
  CONFIG_ITEM_STATUS Import(JReader& jr, BOOL quiet); 
#endif
  void Restore(CArchive& ar);
  void Restore(CAR& car);
  void Save(CAR& car);
  void CommitRestore(void);
#ifdef UAFEDITOR
  void CrossReferenceEquipment(ITEM_LIST *pItemList, const char *etype, CR_LIST *pCRList);
  void CrossReference(CR_LIST *pCRList);
#endif

  GLOBAL_STATS& operator =(const GLOBAL_STATS& src);

  BOOL AddItemToVault(int which, ITEM item);
  BOOL RemoveItemFromVault(int which, int index, int qty);
  void UpdateMoneyInVault(int which, itemClassType type, int qty);
  void AddMoney(int which, MONEY_SACK &src);
  void TransferMoney(int which, MONEY_SACK &src);
  BOOL VaultHasMoney(int which=-1);
  double VaultTotal(int which=-1);

  void SaveArt();
  void SaveSounds();
  BOOL LoadSounds();
  BOOL ConfirmUsedLevels();
// prs 20191221  BOOL LoadFonts(COLORREF green, COLORREF yellow, COLORREF red, COLORREF blue, COLORREF orange, COLORREF keyb, COLORREF BackColor, COLORREF TransColor, COLORREF Custom);
  BOOL LoadSystemFonts(COLORREF keyb, 
                       COLORREF BackColor, 
                       COLORREF TransColor, 
                       COLORREF Custom);
  void ReleaseFonts();
  void ReleaseSurfaces();

   double version;
   double SaveGameVersion; // gets set when saved game is loaded
   CString designName;
   int startLevel; // starting map level for new game
   int currLevel;
   BYTE startX;
   BYTE startY;
   BYTE startFacing;
   BOOL useAreaView;
   int startTime;
   int startExp;
   int startExpType; // indicates if startExp is exp or lvl value
   //int UNUSED_startEquip;
   int startPlatinum;
   int startGem;
   int startJewelry;
   
private:
   // int minPartySize;
   // int maxPartySize;
   int minPCs;
   int maxParty_maxPCs;  // Upper 16 bits = maxPartySize; Lower = maxNumPCs
public:
   int GetMinPCs(void)         {return minPCs;};
   int GetMaxPartySize(void)   {return maxParty_maxPCs >> 16;};
   int GetMaxPCs(void)         {return maxParty_maxPCs & 0xffff;};
   void SetMinPCs(int n)       {minPCs = n;};
   void SetMaxPartySize(int n) {maxParty_maxPCs = (maxParty_maxPCs & 0xffff) | (n <<16);};
   void SetMaxPCs(int n)       {maxParty_maxPCs = (maxParty_maxPCs & 0xffff0000) | n;};


   BOOL GetAllowCharCreate(void) {return (flags & flg_allowCharCreate) != 0;};
   void SetAllowCharCreate(BOOL v) {if (v) flags |= flg_allowCharCreate; else flags &= ~flg_allowCharCreate;};
   BOOL GetDeadAtZeroHP(void) {return (flags & flg_deadAtZeroHP) != 0;};
   void SetDeadAtZeroHP(BOOL v) {if (v) flags |= flg_deadAtZeroHP; else flags &= ~flg_deadAtZeroHP;};

   //BOOL allowCharCreate;
   int flags;
   enum
   {
     flg_allowCharCreate = 0x00000001,
     flg_deadAtZeroHP    = 0x00000002,
   };
   int DungeonTimeDelta;
   int DungeonSearchTimeDelta;
   int WildernessTimeDelta;
   int WildernessSearchTimeDelta;
   BOOL AutoDarkenViewport;
   BOOL AutoDarkenAmount;
   int StartDarken;
   int EndDarken;
   CString m_MapArt;  // various area/outdoor map images
   CString IconBgArt;  // background when selecting icons
   CString BackgroundArt; // fills screen
   CString CreditsBgArt; // displayed at game exit
   LOGFONT logfont; // font used during gameplay

// prs 20191227   AVAIL_FONT availableFonts[10];  // 10 fonts each with NumColorNum colors.
   FONT_LIBRARY fontLib;
//   long int WhiteFont;    // white characters
//   long int YellowFont;   // yellow characters
//   long int OrangeFont;   // orange characters
//   long int RedFont;      // red characters 
//   long int GreenFont;    // green characters
//   long int BlueFont;     // blue characters
   long int KeyboardFont; // font for keyboard shortcut characters
   long int KeyboardHighlightFont; // font for shortcuts in highlighted text
   long int HighlightFont; // highlighted characters
   long int Custom1Font;
   long int MagicAdjFont; // for stats being modified by magic
   long MapArtSurf;
   long BackgroundSurf;
   BOOL UseAVZones;

#ifdef UAFEDITOR
   ITEM_LIST startEquip;
   ITEM_LIST ClericEquip;
   ITEM_LIST FighterEquip;
   ITEM_LIST MagicUserEquip;
   ITEM_LIST ThiefEquip;
   ITEM_LIST RangerEquip;
   ITEM_LIST PaladinEquip;
   ITEM_LIST DruidEquip;
#endif
   GLOBAL_SOUND_DATA sounds;
   SPECIAL_OBJECT_LIST keyData;
   SPECIAL_OBJECT_LIST specialItemData;
   QUEST_LIST questData;
	 CHAR_LIST charData; // pre-gen characters are shared across all levels
   GLOBAL_VAULT_DATA vault[MAX_GLOBAL_VAULTS];
   LEVEL_INFO levelInfo;
   MONEY_DATA_TYPE moneyData;
   GameEventList eventData; // global events, npc talk, item use, etc
   DIFFICULTY_LEVEL_DATA m_diffLvlData;
   JOURNAL_DATA journalData;
   TITLE_SCREEN_DATA titleData;
   spellBookType fixSpellBook; // spells used when 'FIX' menu pressed to heal party
   A_ASLENTRY_L global_asl;
   A_ASLENTRY_L temp_asl;
   COMBAT_TREASURE_DATA combatTreasure;
   TYMER_DATA timerData;
};

BYTE CurrLevelWidth();
BYTE CurrLevelHeight();


void PlayCharHit(BOOL play=TRUE); 
void PlayCharMiss(BOOL play=TRUE); 
void PlayPartyStep(BOOL play=TRUE); 
void PlayPartyBump(BOOL play=TRUE); 
void PlayIntro(BOOL play=TRUE); 
void PlayDeath(BOOL play=TRUE); 
void PlayCampMusic(BOOL play=TRUE);
void CheckAndPlayBackgroundMusic(int &UsingDayMusic);

// global attribute flags for RunAs versions
enum {
  RunAsVersion=0,
  GuidedTourVersion=1,
  ItemUseEventVersion=2,
  SpecialItemKeyQtyVersion=3
};

//lint +e1411
#endif