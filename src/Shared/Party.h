/******************************************************************************
* Filename: Party.h
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
#ifndef __PARTY_H__
#define __PARTY_H__

#include "GameEvent.h"
#include "SharedQueue.h"
#include "level.h"
#include "char.h"
#include "TagList.h"

const int DRUNK_THRESHOLD = 60;
const int DRINK_POINT_DEC_PER_HOUR = 10;
const int MAX_TEMP_CHARS = 30;

const int MAX_JOURNAL_ENTRIES = 0x00FFFFFF;
extern const char *JKEY_ENTRIES;
extern const char *JKEY_KEY;
extern const char *JKEY_ORIGKEY;
extern const char *JKEY_TEXT;
extern const char *JKEY_MTEXT;
extern const char *JKEY_JOURNAL;
struct JOURNAL_ENTRY
{
  JOURNAL_ENTRY() { Clear(); }
  void Clear() { entry=-1; origentry=-1;text=""; };
  JOURNAL_ENTRY& operator =(const JOURNAL_ENTRY& src);
  void Serialize(CArchive &ar);
  void Serialize(CAR &car); //New 20121208 PRS
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  void Import(JReader& jr);
#endif

  int entry; // journal key
  int origentry; // src journal key
  CString text;
};

struct JOURNAL_DATA
{
  JOURNAL_DATA() { Clear(); }
  void Clear() { jlist.RemoveAll(); }
  int GetCount() { return jlist.GetCount(); }
  int GetNextKey()
  {
    int key=0;
    POSITION pos;
    if (GetCount() > 0)
    {
      key = jlist.GetTail().entry;
      if (key >= INT_MAX)
      {
        // Need to wrap key.
        // Search for non-sequential gap between current keys.
        // A gap is guaranteed since the key range is greater than
        // MAX_JOURNAL_ENTRIES.
        // Gaps occur when deletions are made to item list
        pos = jlist.GetHeadPosition();
        BOOL found = FALSE;        
        int prevKey = 0;
        int currKey;
        while ((pos != NULL) && (!found))
        {
          currKey = jlist.GetNext(pos).entry;
          if (currKey != prevKey+1)
          {
            key = prevKey+1;
            found = TRUE;
          }
          else
            prevKey = currKey; 
        }
        VERIFY(found); // should never happen
      }
      else
        key++;
    }
    else
      key = 1;
    return key;
  }
  int Add(JOURNAL_ENTRY &data) 
  {
    int newkey = -1;
    if (GetCount() < MAX_JOURNAL_ENTRIES)
    {
      data.entry = GetNextKey();
      jlist.Insert(data, data.entry);
      newkey = data.entry;
    }
    return newkey;
  }
  int AddWithCurrKey(JOURNAL_ENTRY &data) 
  { 
    if (GetCount() < MAX_JOURNAL_ENTRIES)
    {
      jlist.Insert(data, data.entry); 
      return data.entry;
    }
    return -1;
  }
  void Delete(JOURNAL_ENTRY &data)
  {
    POSITION pos = jlist.GetHeadPosition();
    while (pos != NULL)
    {
      if (jlist.PeekAtPos(pos).entry == data.entry)
      {
        jlist.RemoveAt(pos);
        return;
      }
      jlist.GetNext(pos);
    }
  }
  BOOL Get(int entry, JOURNAL_ENTRY &data)
  {
    POSITION pos;
    if ((pos = jlist.FindKeyPos(entry)) != NULL)
    {
      data = jlist.PeekAtPos(pos);
      return TRUE;
    }
    else
      return FALSE;
  }
  BOOL Set(int entry, JOURNAL_ENTRY &data)
  {
    POSITION pos;
    if ((pos = jlist.FindKeyPos(entry)) != NULL)
    {
      jlist.GetAtPos(pos) = data;
      return TRUE;
    }
    else
      return FALSE;
  }
  BOOL HaveGlobalJournalEntryAlready(int origkey)
  {
    POSITION pos = jlist.GetHeadPosition();
    while (pos != NULL)
    {
      if (jlist.GetNext(pos).origentry == origkey)
        return TRUE;
    }
    return FALSE;
  }
  void Serialize(CArchive &ar)
  {
    if (ar.IsStoring())
    {
      ar << GetCount();
      POSITION pos = jlist.GetHeadPosition();
      while (pos != NULL)
        jlist.GetNext(pos).Serialize(ar);
    }
    else
    {
      JOURNAL_ENTRY data;
      int count;
      ar >> count;
      ASSERT((count>=0)&&(count<=MAX_JOURNAL_ENTRIES));
      for (int i=0;i<count;i++)
      {
        data.Serialize(ar);
        AddWithCurrKey(data);
      }
    }
  }
  void Serialize(CAR &ar)
  {
    if (ar.IsStoring())
    {
      ar << GetCount();
      POSITION pos = jlist.GetHeadPosition();
      while (pos != NULL)
        jlist.GetNext(pos).Serialize(ar);
    }
    else
    {
      JOURNAL_ENTRY data;
      int count;
      ar >> count;
      ASSERT((count>=0)&&(count<=MAX_JOURNAL_ENTRIES));
      for (int i=0;i<count;i++)
      {
        data.Serialize(ar);
        AddWithCurrKey(data);
      }
    }
  }

#ifdef UAFEDITOR
  void Export(JWriter& jw)
  {
    POSITION pos = jlist.GetHeadPosition();
    jw.StartArray(JKEY_JOURNAL);
    while (pos != NULL)
    {
      jw.NextEntry();
      jlist.GetNext(pos).Export(jw);
    };
    jw.EndArray();
  }
  void Import(JReader& jr)
  {
    jr.Optional();
    if (!jr.StartArray(JKEY_JOURNAL)) return;
    while (jr.Optional(), jr.NextEntry())
    {
      JOURNAL_ENTRY entry;
      entry.Import(jr);
      Add(entry);
    };
    jr.EndArray();
  }
#endif

  JOURNAL_DATA& operator =(const JOURNAL_DATA& src)
  {
    if (this==&src) return *this;
    Clear();
    POSITION pos = src.jlist.GetHeadPosition();
    while (pos != NULL)
      AddWithCurrKey(const_cast<JOURNAL_DATA&>(src).jlist.GetNext(pos));
    return *this;
  }

  OrderedQueue<JOURNAL_ENTRY, MAX_JOURNAL_ENTRIES> jlist;
  //CList<JOURNAL_ENTRY,JOURNAL_ENTRY&> jlist;
};

#ifdef UAFEngine
struct TEMP_IN_PARTY_DATA
{
  //int key;
  CHARACTER_ID characterID;
  BOOL isInParty;
  BOOL isPC;
};

struct TEMP_CHARACTERS
{
private:
	CArray<TEMP_IN_PARTY_DATA, TEMP_IN_PARTY_DATA&> inParty;
	CHAR_LIST temps;

public:
  TEMP_CHARACTERS();
  ~TEMP_CHARACTERS();
  int numChars();

  void Clear();
  void Swap(int i, int j)
  {
    TEMP_IN_PARTY_DATA temp;
    temp = inParty[i];
    inParty[i] = inParty[i+1];
    inParty[i+1] = temp;
  };
  CString GetCharName(int index);
  void RemoveThoseInParty();
  void RemoveNPCIfInParty();
  void AddSavedCharacters();
  void RemoveSavedCharacters();
  int CountInPartyMembers();
  int CountInPartyPCs();
  void SetInParty(int index, BOOL status, BOOL isPC);
  BOOL InParty(int index); 
  int GetKey(int index);
  //CHARACTER GetCharacter(int inPartyIndex);
  CHARACTER *GetCharacter(int inPartyIndex);
  void deleteTemp(int index);
  int addTempCharacter(CHARACTER &dude);
  //int addPreGenCharacter(CHARACTER &dude, DWORD key);
  int addPreGenCharacter(const CHARACTER &dude);
  BOOL IsCharacterInTemps(const CHARACTER &dude) const;
};

extern TEMP_CHARACTERS tempChars;
#endif // UAFEngine

// one bit per area map spot to indicate if
// party has been in this spot.
//
class VISIT_DATA //: public CObject
{
private:
  bool operator==(const VISIT_DATA& src); // not allowed

public:
//  DECLARE_SERIAL( VISIT_DATA )
    VISIT_DATA();
  ~VISIT_DATA();
  VISIT_DATA& operator =(const VISIT_DATA& src);

  void Clear();
  void Serialize(CArchive &ar);
  void Serialize(CAR &car);
  BOOL IsVisited(int level, int x, int y);
  void SetVisited(int level, int x, int y);

  TAG_LIST_2D *visited[MAX_LEVELS];
  //VISIT_FLAGS visited[MAX_AREA_HEIGHT];
};


// keeps cumulative count of steps
// taken in each zone.
struct STEP_COUNTER 
{
private:
  bool operator==(const STEP_COUNTER& src); // not allowed

public:
  STEP_COUNTER() { Clear(); }
  ~STEP_COUNTER() { Clear(); }
  void Clear();
  STEP_COUNTER& operator =(const STEP_COUNTER& src);
  STEP_COUNTER(const STEP_COUNTER& src);

  unsigned long int stepCount[MAX_ZONES];
};

// This data is part of the party data.
// and so is also part of the save game data as well.
//
class TRIGGER_FLAGS //: public CObject
{
private:
  bool operator==(const TRIGGER_FLAGS& src); // not allowed

public:
//  DECLARE_SERIAL( TRIGGER_FLAGS )
  TRIGGER_FLAGS() { Clear(); }
  ~TRIGGER_FLAGS() { Clear(); }
  void Clear(void) { markNeverHappened(); };
  TRIGGER_FLAGS& operator =(const TRIGGER_FLAGS& src);
  TRIGGER_FLAGS(const TRIGGER_FLAGS& src);
  void Serialize(CArchive &ar);
  void Serialize(CAR &car);

  BOOL HasEventHappened()   { return (eventResult == HasHappenedAtLeastOnce); }
  void markEventHappened(void) {eventResult = HasHappenedAtLeastOnce;};
  void markNeverHappened(void) {eventResult = HasNeverHappened;};

  int eventStatusUnused;
  eventResultType eventResult;
};

// event triggers tracked per level
class LEVEL_FLAG_DATA //: public CObject
{
private:
  TRIGGER_FLAGS flags; // temp only
  bool operator==(const LEVEL_FLAG_DATA& src); // not allowed

public:
//  DECLARE_SERIAL( LEVEL_FLAG_DATA )
  LEVEL_FLAG_DATA() { Clear(); }
  ~LEVEL_FLAG_DATA() { Clear(); }
  void Clear() { stepData.Clear(); flagData.RemoveAll(); }
  LEVEL_FLAG_DATA& operator =(const LEVEL_FLAG_DATA& src); 
  LEVEL_FLAG_DATA(const LEVEL_FLAG_DATA& src);
  void Serialize(CArchive &ar);
  void Serialize(CAR &car);

  void IncZoneStepCount(int zone);
  unsigned long GetZoneStepCount(int zone);
  BOOL HasEventHappened(DWORD event);
  void markEventHappened(DWORD event);

  OrderedQueue<TRIGGER_FLAGS, MAX_EVENTS> flagData;
  STEP_COUNTER stepData;
};

// all event triggers for all levels
class EVENT_TRIGGER_DATA //: public CObject
{
private:
  bool operator==(const EVENT_TRIGGER_DATA& src); // not allowed

public:
//  DECLARE_SERIAL( EVENT_TRIGGER_DATA )
  EVENT_TRIGGER_DATA() { Clear(); }
  ~EVENT_TRIGGER_DATA() { Clear(); }
  void Serialize(CArchive &ar);
  void Serialize(CAR &car);

  EVENT_TRIGGER_DATA& operator =(const EVENT_TRIGGER_DATA& src);
  EVENT_TRIGGER_DATA(const EVENT_TRIGGER_DATA& src);

  void Clear();
  BOOL CheckLevel(int level);
  void IncZoneStepCount(int level, int zone); 
  unsigned long GetZoneStepCount(int level, int zone); 
  BOOL HasEventHappened(int level, DWORD event);
  void MarkEventHappened(int level, DWORD event); 

  CArray<LEVEL_FLAG_DATA, LEVEL_FLAG_DATA&> data;
};

#define MAX_TASK_STATE_SAVE_BYTES 5

struct TASK_STATE_SAVE
{
  void Clear() { id=0;flags=0;memset(data,0,sizeof(data)); datacount=0; }
  unsigned int id;
  unsigned int flags;
  unsigned char datacount;
  unsigned int data[MAX_TASK_STATE_SAVE_BYTES];
};

struct TASK_STATE_DATA
{
  unsigned int count;
  TASK_STATE_SAVE tdata[MAXTASK];
};



enum PartyBlockageType
{
  PartyBlock_none = 0,  
  PartyBlock_openSecret,
  PartyBlock_locked,
  PartyBlock_lockedSecret,
  PartyBlock_lockedWizard,
  PartyBlock_lockedWizardSecret,
  PartyBlock_blocked,
  PartyBlock_falseDoor,
  PartyBlock_unknown,
  PartyBlock_specialKey0,
  PartyBlock_specialKey1,
  PartyBlock_specialKey2,
  PartyBlock_specialKey3,
  PartyBlock_specialKey4,
  PartyBlock_specialKey5,
  PartyBlock_specialKey6,
  PartyBlock_specialKey7,
};



class PARTY //: public CObject
{
private:
  bool operator==(const PARTY& src); // not allowed

public:
//  DECLARE_SERIAL( PARTY )
  // ASLs are named "PARTY_ATTRIBUTES"
  PARTY() :party_asl() { Clear(TRUE); }
  ~PARTY() { Clear(TRUE); }
  PARTY& operator =(const PARTY& src);
  PARTY(const PARTY& src);
#ifdef UAFEngine
  void Serialize(CArchive &ar);
  void Serialize(CAR &car);
#endif
  void Clear(BOOL IsConstructor=FALSE);

  BOOL IsVisited(int lvl, int x, int y) { return (visitData.IsVisited(lvl, x,y)); }
  void SetVisited(int lvl, int x, int y) { visitData.SetVisited(lvl,x,y); }

  void IncZoneStepCount(int zlevel, int zone) { eventFlags.IncZoneStepCount(zlevel, zone); }
  unsigned long GetZoneStepCount(int zlevel, int zone) { return (eventFlags.GetZoneStepCount(zlevel, zone)); }

  //BOOL addNPCToParty(int npc, int hitPointMod, BOOL useOrig=TRUE);
  BOOL addNPCToParty(const CHARACTER_ID& characterID, int hitPointMod, BOOL useOrig=TRUE);
  void RunJoinPartyMemorizeScripts(int charIndex);
  CString ForEachPartyMember(const CString& SAName, const CString& scriptName);
  //void removeNPCFromParty(int npc);
  void removeNPCFromParty(const CHARACTER_ID& characterID);
  void removeCharacter(int loser, BOOL AddToTemps=TRUE);
  void purgeCharacter(int loser);
  PartyBlockageType newForwardPosition(int relativeDirection, int &newX, int &newY);
  int  DetermineSpriteDistance(int distance);
  BOOL movePartyForward(int relativeDirection=0); // 0=facing 1=right, etc
  BOOL movePartyBackward(); // Can this be movePartyForward(2) ????
  void turnPartyLeft();
  void turnPartyRight();
  void poolPartyGold();  
  void sharePartyGold();
  void sharePartyGoldOnce();
  void AutoUpConvertPoolMoney();
  int  GetPoolGoldValue();
  BOOL PartyHasMoney();
  void deductPoolMoney(itemClassType type, int qty);

  void setPartyAdventureState();
  void setPartyLevelState(int level, int x, int y, int f);
  void turnParty180();
  void distributeExpPoints(int total);
  CHARACTER& getActiveChar();
  CHARACTER *FindCharacter(int uniquePartyID);

  void incrementClock(int minutes);
  void setPartyTime(int time);
  int get24HourClock(); // no days 0000-2359
  DWORD getElapsedMinutes(); // since 0 hr, 0 min, 0 day, not design start time!
  void clearPartyFlags();
  //BOOL isNPCinParty(int npc);
  BOOL isNPCinParty(const CHARACTER_ID& characterID) const;
  void addSpecialItem(int item);
  void addSpecialKey(int key);
  void removeSpecialItem(int item);
  void removeSpecialKey(int key);
  BOOL hasSpecialItem(int item);
  BOOL hasSpecialKey(int key);
  BYTE getPartySpeed();
  void nextItem();
  void prevItem();
  void nextCharacter();
  void prevCharacter();

  enum PartyBlockageType PartyBlockage(BlockageType type, int facing);

  void AddPooledMoneyToVault(int WhichVault);
  void RemovePooledMoneyFromVault(int WhichVault);

  BOOL IsPartyDrunk();
  void UpdatePartyDrinkPoints();
  //BOOL PartyHasItem(GLOBAL_ITEM_ID giID);
  BOOL PartyHasItem(const ITEM_ID& itemID);
  BOOL PartyInDaytime();
  BOOL PartyIsSearching();
  int GetPartyFacing();
  BOOL PartyIsDetectingTraps();
  BOOL PartyIsDetectingInvisible();
  BOOL PartyIsDetectingMagic();
  //BOOL PartyHasClass(classType type);
  BOOL PartyHasClass(const CLASS_ID& classID) const;
  BOOL PartyHasBaseclass(const BASECLASS_ID& baseclassID) const;
  //BOOL PartyHasRace(raceType type);
  BOOL PartyHasRace(const RACE_ID& raceID) const;
  BOOL PartyHasGender(genderType type);
  //BOOL PartyHasNPC(int npckey);
  BOOL PartyHasNPC(const CHARACTER_ID& characterID) const;
  //BOOL PartyHasSpellMemorized(GLOBAL_SPELL_ID spellDbKey);
  BOOL PartyHasSpellMemorized(const SPELL_ID& spellID);

  CHARACTER& GetCharacter(int activeChar, const char *msg);
  CHARACTER& GetStrongestCharacter(void);
  CHARACTER& GetBestLockpicker(int *bestSkillValue);
  int        GetBestSkill(const SKILL_ID& skillID, int *bestSkillValue, bool minimize);
  void getCharWeaponText(int index, CString &wpn, CString &dmg);
  CHARACTER *addTempToParty(CHARACTER &luckyDude);
  BOOL TeleportParty(TRANSFER_DATA &dest);
  int GetHealingPointsNeeded();  
//  void FixParty(CHARACTER &caster, BOOL cast); // use specified character to heal
  void FixParty(BOOL cast); // loops thru all party members
  BOOL GetCharThatNeedsHealedFirst(CHARACTER &dude);
  bool ProcessTimeSensitiveData(LONGLONG currTime);
  void BeginResting(void);
  void HealParty(HEAL_PARTY_DATA &data);
  int RelativeToActualFacing(int relativeDirection);
  void RemoveSpellEffect(DWORD parent, const SPELL_DATA *pSpell, bool endSpell);
  DWORD CalcRestTime(void);

  // performs the looting for take items event
  void TakePartyItems(TAKE_PARTY_ITEMS_DATA &data);
  int TakePartyItemQty(int dude, takeItemQtyType type, int amount, int &data);

  // alter the party order for active character
  void DecCharacterOrder(void);
  void IncCharacterOrder(void);

  void MarkEventHappened(int lvl, DWORD id) { eventFlags.MarkEventHappened(lvl, id); }
  BOOL HasEventHappened(int lvl, DWORD id) { return (eventFlags.HasEventHappened(lvl, id)); }

  int NumNPCs();
#ifdef UAFEngine
  BOOL CanAddNPC();
#endif

  int GetDayMusicFlag() const { return DayMusic; }
  void SetDayMusicFlag(int flag) { DayMusic = flag; }

	
  TASK_STATE_DATA stateStack;
  CString name;
	BOOL adventuring;
	BOOL areaView;
	BOOL searching;
  BOOL looking; // stays true during one game loop only
  BOOL resting;
	BYTE level;
	BYTE speed;
	BYTE facing;
	BYTE activeCharacter;  // index into PARTY::characters array
	BYTE activeItem;
  BYTE tradeItem;
  BYTE tradeGiver;
	BYTE skillLevel;
  BYTE numCharacters;
	BOOL moneyPooled;
  int  Posx;
  int  Posy;
  int  PrevPosx;
  int  PrevPosy;
  int  PrevRelFacing;
  int tradeQty;
  int days;
  int hours;
  int minutes;
  int drinkPoints;
  int ActiveScripter;
  int DayMusic;
  //********  These variables used to be defined as static 
  //********  variables in PARTY::ProcessTimeSensitiveData().
  //********  That made them impossible to see in the debugger.
  //********  But, more importantly, they are lost when the
  //********  player does a save/reload.  Someday, we must
  //********  include these variables in the PARTY :Serialize.
  int minutesRested;
#ifdef OLD_AUTO_HEAL
  int minutesUntilAutoHeal;
#endif
  //********


  MONEY_SACK poolSack;
  EVENT_TRIGGER_DATA eventFlags;
  VISIT_DATA visitData;
  BLOCKAGE_STATUS blockageData;
  JOURNAL_DATA journal;
  CHARACTER characters[MAX_PARTY_MEMBERS];
  A_ASLENTRY_L party_asl; 

private:

	//BOOL detectingTraps;
	//BOOL detectingInvisible;
  //BOOL detectingMagic;

  int GetNextUniquePartyID();

};

#endif