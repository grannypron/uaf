/******************************************************************************
* Filename: Specab.h
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
#ifndef __SPECAB_H__
#define __SPECAB_H__

#include "externs.h"
//#include <afxtempl.h>

#include "ASL.h"

#ifdef OLD_SPECIAL_ABILITIES
/*
#ifdef UAFEngine
class CHARACTER;
struct COMBATANT;
#endif

#define SPECAB_MSG_NONE          0x0000
#define SPECAB_MSG_CASTING       0x0001
#define SPECAB_MSG_ACTIVATESPELL 0x0002
#define SPECAB_MSG_FLEE          0x0004
#define SPECAB_MSG_TURNUNDEAD    0x0008
#define SPECAB_MSG_BANDAGE       0x0010
#define SPECAB_MSG_GUARD         0x0020
#define SPECAB_MSG_ATTACK        0x0040
#define SPECAB_MSG_MOVE          0x0080
#define SPECAB_MSG_ENDTURN       0x0100
#define SPECAB_MSG_DELAY         0x0200
#define SPECAB_MSG_READY         0x0400
#define SPECAB_MSG_UNUSED1       0x0800
#define SPECAB_MSG_UNUSED2       0x1000

#define NUM_SPECAB_MSGS 14

// if adding to this list, be sure to:
//  add another line to spellAbilitiesText[] in GlobtText.cpp.
//  add lines in ItemDB.cpp to load specabs based on name for items/etc

// for spells, items, and monsters
enum specialAbilitiesType 
{
  SA_None=0, 
  SA_Bless=1,                 // done
  SA_Curse=2,                 // done
  SA_UndeadFear=3,
  SA_Enlarge=4,               // done
  SA_Reduce=5,                // done
  SA_CharmPerson=6, 
  SA_DetectMagic=7,           // done
  SA_ReflectGazeAttack=8,
  SA_ProtFromEvil=9,          // done
  SA_ProtFromGood=10,         // done
  SA_Shield=11,               // done
  SA_Sleep=12,                // done
  SA_Fog=13,
  SA_Entangle=14,             // done
  SA_InvisibleToAnimals=15,   // done
  SA_InvisibleToUndead=16,    // done
  SA_NonUndeadFear=17,
  SA_Sanctuary=18,            // done
  SA_Shillelagh=19,           // done
  SA_Displacement=20,         // done
  SA_Wizardry=21,
  SA_VorpalAttack=22,         // done
  SA_HoldPerson=23,           // done
  SA_Silence=24,              // done
  SA_Poisoned=25,             // done
  SA_SlowPoison=26,           // done
  SA_MirrorImage=27,          // done
  SA_Invisible=28,            // done
  SA_Enfeebled=29,            // done
  SA_Blindness=30,            
  SA_Diseased=31,

  SA_LastUnusedSpecAb
};

#define FIRST_SPECIAL_ABILITY SA_Bless
#define LAST_SPECIAL_ABILITY (SA_LastUnusedSpecAb-1)
#define NUM_SPECIAL_ABILITIES SA_LastUnusedSpecAb

struct SPEC_AB_DATA
{
  SPEC_AB_DATA();
  void Clear();
  SPEC_AB_DATA& operator=(const SPEC_AB_DATA& src);
  bool operator==(const SPEC_AB_DATA& src) const;  
  void Serialize(CArchive &ar, double ver);

  bool ExecuteScript();
  bool ExecuteDAScript();

  // activation
  CString script; // conditional activation GPDL script
  CString binary; // compiled script ready to execute

  // deactivation
  CString da_script; // conditional activation GPDL script
  CString da_binary; // compiled script ready to execute

  DWORD MsgTypes; // which combat message types will this over-ride
  BOOL DisplayOnce;
  CString Msgs[NUM_SPECAB_MSGS];
};

class CR_LIST;

class SPECIAL_ABILITIES : public CObject
{
  SPEC_AB_DATA *GetSpecAbData(specialAbilitiesType sa);
  CString GenerateDefaultSpecAbScript(specialAbilitiesType sa);
  CString GenerateDefaultSpecAbDAScript(specialAbilitiesType sa);
  DWORD GenerateDefaultSpecAbMsgTypes(specialAbilitiesType sa);
  CString GenerateDefaultSpecAbMsgFormat(specialAbilitiesType sa);

public:
  DECLARE_SERIAL( SPECIAL_ABILITIES )

  SPECIAL_ABILITIES();
  SPECIAL_ABILITIES(const SPECIAL_ABILITIES& src);
  ~SPECIAL_ABILITIES();

  SPECIAL_ABILITIES& operator=(const SPECIAL_ABILITIES& src);
  bool operator==(const SPECIAL_ABILITIES& src) const;  

  void Clear();
  void Serialize(CArchive &ar, double ver);
  void CrossReference(CR_LIST *pCRList);

  POSITION GetHeadPosition();
  specialAbilitiesType GetAt(POSITION pos);
  specialAbilitiesType GetNext(POSITION &pos);
  SPEC_AB_DATA *GetAtData(POSITION pos);
  SPEC_AB_DATA *GetNextData(POSITION &pos);

  CString GenerateSpecAbFuncName(specialAbilitiesType sa);
  CString GenerateSpecAbFuncStart(specialAbilitiesType sa);
  CString GenerateSpecAbFuncEnd(specialAbilitiesType sa);

  void EnableSpecAb(specialAbilitiesType sa, CString scr="", CString dascr="");
  void DisableSpecAb(specialAbilitiesType sa);
  bool HaveSpecAb(specialAbilitiesType sa);
  bool HaveAtLeastOneSpecAb();

  bool CompileSpecAbScript(specialAbilitiesType sa);
  bool CompileSpecAbDAScript(specialAbilitiesType sa);
  bool ExecuteSpecAbScript(specialAbilitiesType sa);
  bool ExecuteSpecAbDAScript(specialAbilitiesType sa);

  CString GetSpecAbScript(specialAbilitiesType sa); // entire script
  CString GetSpecAbUserScript(specialAbilitiesType sa); // no $FUNC { }
  CString GetSpecAbBinary(specialAbilitiesType sa);

  CString GetSpecAbDAScript(specialAbilitiesType sa); // entire script
  CString GetSpecAbUserDAScript(specialAbilitiesType sa); // no $FUNC { }
  CString GetSpecAbBinaryDA(specialAbilitiesType sa);

  DWORD GetMsgTypes(specialAbilitiesType sa);
  void SetMsgTypes(specialAbilitiesType sa, DWORD types);
  void AddMsgType(specialAbilitiesType sa, DWORD type);
  bool HaveMsgType(specialAbilitiesType sa, DWORD type);
  CString GetMsgFormat(specialAbilitiesType sa, DWORD type);
  void SetMsgFormat(specialAbilitiesType sa, DWORD type, CString &format);

#ifdef UAFEngine
  void EnableAllFor(CHARACTER *pChar);
  void EnableAllFor(COMBATANT *pChar);
  void DisableAllFor(CHARACTER *pChar);
  void DisableAllFor(COMBATANT *pChar);
#endif

  SPEC_AB_DATA specAbData[NUM_SPECIAL_ABILITIES];
};

int ConvertSpecAbFlagToIndex(DWORD flag);
DWORD ConvertSpecAbIndexToFlag(int index);
specialAbilitiesType ConvertRuntimeIfTextToSpecAb(const CString &text);
CString ConvertSpecAbToRuntimeIfText(specialAbilitiesType sa);

*/
#else  if not OLD_SPECIAL_ABILITIES


/************************************************************************
 *
 *  SPECIAL ABILITES
 *
 *  Each of the possible Special Abilities is defined in the
 *  specialAbilitiesData list which consists of SPECABILITY_DEF entries.
 *  Each of those entries contains the ability name and a list:
 *    1) Associated String list of all definition strings
 *       Each entry of type ASLENTRY.
 *       Definition strings are of type SPECAB_SCRIPT,
 *       SPECAB_CONSTANT, SPECAB_INCLUDE, or SPECAB_VARIABLE.
 *
 *
 *  Each object type (Arrow, Dragon, etc) has a list of the Special
 *  Abilities associated with that object type.  
 *  The list consists of entries of type CStringPAIR.  Each entry
 *  contains the name of the special ability and and a parameter
 *  
 *
 ************************************************************************/


const int MAX_SPECIAL_ABILITIES = 0x00ffffff;

typedef LPCSTR specialAbilitiesType;

class CONFIG_FILE;

//int ReadSpecialAbilitiesDB(LPCSTR file, 
//               A_SPECABILITY_DEF_L *itemsPointer, 
//               CONFIG_FILE *pConfigFile=NULL);



#define SA_ProtFromEvil       "SA_ProtFromEvil"

/*
#define SA_Poisoned           "SA_Poisoned"
*/
#define SA_HoldPerson         "SA_HoldPerson"
#define SA_CharmPerson        "SA_CharmPerson"
#define SA_Diseased           "SA_Diseased"
#ifdef OLD_SPECIAL_ABILITIES
#define SA_DetectMagic        "SA_DetectMagic"
#endif
//#define SA_Sleep              "SA_Sleep"
#define SA_Silence            "SA_Silence"
#define SA_MirrorImage        "SA_MirrorImage"
#define SA_Enfeebled          "SA_Enfeebled"
#define SA_Enlarge            "SA_Enlarge"
#define SA_Reduce             "SA_Reduce"
#define SA_Bless              "SA_Bless"
#define SA_Curse              "SA_Curse"
#define SA_Invisible          "SA_Invisible"
#define SA_InvisibleToAnimals "SA_InvisibleToAnimals"
#define SA_InvisibleToUndead  "SA_InvisibleToUndead"
#define SA_ProtFromGood       "SA_ProtFromGood"
#define SA_Shield             "SA_Shield"
#define SA_Displacement       "SA_Displacement"
//#define SA_Shillelagh         "SA_Shillelagh"
//#define SA_Blindness          "SA_Blindness"
#define SA_Entangle           "SA_Entangle"
//#define SA_Sanctuary          "SA_Sanctuary"
#define SA_VorpalAttack       "SA_VorpalAttack"
#define SA_SlowPoison         "SA_SlowPoison"


#define SPECAB_MSG_NONE          0x0000
#define SPECAB_MSG_CASTING       0x0001
#define SPECAB_MSG_ACTIVATESPELL 0x0002
#define SPECAB_MSG_FLEE          0x0004
#define SPECAB_MSG_TURNUNDEAD    0x0008
#define SPECAB_MSG_BANDAGE       0x0010
#define SPECAB_MSG_GUARD         0x0020
#define SPECAB_MSG_ATTACK        0x0040
#define SPECAB_MSG_MOVE          0x0080
#define SPECAB_MSG_ENDTURN       0x0100
#define SPECAB_MSG_DELAY         0x0200
#define SPECAB_MSG_READY         0x0400
#define SPECAB_MSG_UNUSED1       0x0800
#define SPECAB_MSG_UNUSED2       0x1000

#define NUM_SPECAB_MSGS 14

enum SPECAB_STRING_TYPE
{
  SPECAB_SCRIPT       = 1,
  SPECAB_CONSTANT     = 2,
  SPECAB_VARIABLE     = 3,
  SPECAB_INCLUDE      = 4,
  SPECAB_BINARYCODE   = 5,
  SPECAB_SCRIPTERROR  = 6,
  SPECAB_INTEGERTABLE = 7,
  SPECAB_BINARYTABLE  = 8,
  SPECAB_TABLEERROR   = 9,
};

/*
struct SPEC_AB_DATA
{
  SPEC_AB_DATA(); 
  void Clear();
  SPEC_AB_DATA& operator=(const SPEC_AB_DATA& src);
  bool operator==(const SPEC_AB_DATA& src) const;  
  void Serialize(CArchive &ar, double ver);

  bool ExecuteScript();
  bool ExecuteDAScript();

  // activation
  CString script; // conditional activation GPDL script
  CString binary; // compiled script ready to execute

  // deactivation
  CString da_script; // conditional activation GPDL script
  CString da_binary; // compiled script ready to execute

  DWORD MsgTypes; // which combat message types will this over-ride
  BOOL DisplayOnce;
  CString Msgs[NUM_SPECAB_MSGS];
};
*/


//class SPECAB_DATA
//{
//};

/*
class SPECAB_DATA_TYPE
{
  CCriticalSection m_CS;
  OrderedQueue<SPEC_AB_DATA, MAX_SPECIAL_ABILITIES> SpecAbData;

  A_INTEGER_L specAbList;  // Convert name to SA_ID
public:
  BOOL IsLegalID(SA_ID saID);
  POSITION GetHeadPosition() { return SpecAbData.GetHeadPosition(); };
  SPEC_AB_DATA& GetAt(POSITION pos) { return SpecAbData.GetAt(pos); };
  SPEC_AB_DATA& GetNext(POSITION &pos) { return SpecAbData.GetNext(pos); };
  SPEC_AB_DATA& GetSpecialAbilitiesData(SA_ID saID);
  SA_ID GetKeyAt(POSITION pos) { return SpecAbData.GetKeyAt(pos); };
  const CString& GetName(SA_ID saID) const;
  const CString& spellAbilitiesText(specialAbilitiesType sa); 

};
*/


class SPECABILITY_DEF  // Defines one of the game's special abilities
{
  friend class A_SPECABILITY_DEF_L;

  /****************************************************
   *     data
   ***************************************************/
  CString          m_specAbName;
  A_ASLENTRY_L     m_abilityStrings;


  /****************************************************
   *  Constructors and copy operators
   ***************************************************/
private:
  // These are private because if they are made public it is FAR TOO EASY
  // to cause these CPU-intensive copies accidentally.  It is seldom necessary.
  // If you must COPY then use the Copy function explicitly and put a comment
  // justifying your actions!!!
  SPECABILITY_DEF   (const SPECABILITY_DEF& speAbDef);
  SPECABILITY_DEF&  operator =(const SPECABILITY_DEF& specAbDef);

  /****************************************************
   *     Functions
   ***************************************************/
public:
  SPECABILITY_DEF (void);
  SPECABILITY_DEF (const CString& name);
  ~SPECABILITY_DEF(void);
  void            Clear(void);
  void            Copy(const SPECABILITY_DEF& specab);
  void            Copy(const SPECABILITY_DEF& specab, const CString& newName);
  bool            operator == (const SPECABILITY_DEF& specab) const;
  int             Compare(const SPECABILITY_DEF& specAbDef) const ;
  int             Compare(LPCSTR key) const;
  const CString&  Name() const {return m_specAbName;};
  const CString&  Lookup(LPCSTR name) const;
  ASLENTRY       *Find(LPCSTR name) const;
  CString         Delete(const CString& name); // Returns value
  POSITION        GetStartPosition(void) const;
  void            GetNextEntry(POSITION& pos, const ASLENTRY **ppEntry) const;
  void            Insert(const CString& key, const CString& value, unsigned char flags);
  void            Swap (SPECABILITY_DEF& specAbDef);
  void            Serialize(CAR& car);
};



class A_SPECABILITY_DEF_L:BTree
{
  /****************************************************
   *  Constructors and copy operators
   ***************************************************/
private:
  // These are private because if they are made public it is FAR TOO EASY
  // to cause these CPU-intensive copies accidentally.  It is seldom necessary.
  // If you must COPY then use the Copy function explicitly and put a comment
  // justifying your actions!!!
  A_SPECABILITY_DEF_L& operator =(const A_SPECABILITY_DEF_L& specDef);
  A_SPECABILITY_DEF_L  (const A_SPECABILITY_DEF_L& specDef);
public:
  A_SPECABILITY_DEF_L (void);



public:
  void Clear(void);
        int               GetCount(void) const;
  void                    Copy(const A_SPECABILITY_DEF_L& psa);
  ~A_SPECABILITY_DEF_L();
  bool                    operator == (const A_SPECABILITY_DEF_L& psa) const;
  POSITION                GetStartPosition(void) const;
        SPECABILITY_DEF  *GetNext(POSITION& pos) const;
        SPECABILITY_DEF  *FindAbility(LPCSTR name) const;
  const CString&          spellAbilitiesText(specialAbilitiesType sa) const;
  const CString&          FindString(specialAbilitiesType sa, LPCSTR stringName);
  void                    DeleteString(specialAbilitiesType sa, LPCSTR stringName);
        void              InsertString(LPCSTR abilityName, const CString& key, const CString& value, unsigned char flags);
        SPECABILITY_DEF  *InsertAbility(const CString& name);
  void                    Serialize(CAR& car);
  void                    CrossReference(class CR_LIST* pCRList) const;
  void                    Delete(const CString& name);
//  void                    SwapList(const CString& abilityName, SPECABILITY_DEF& specab);
};

extern A_SPECABILITY_DEF_L specialAbilitiesData;


class CR_LIST;
class CR_REFERENCE;
/*
class SPECABILITY  // Names one of an item's (or monster ....) special abilities.
{
  friend class A_SPECABILITY_L;
  CString         m_specAbName;
  A_CStringPAIR_L m_parameters;
public:
  ~SPECABILITY(void) {m_parameters.Clear();};
  const CString&  Name(void) const;
  bool            Find(LPCSTR name) const;
  int             Compare (const SPECABILITY& a) const {return m_specAbName.Compare(a.m_specAbName);};
  bool operator ==(const SPECABILITY& specAb) const;
};

class A_SPECABILITY_L:BTree
{
public:
  void Clear(void);
  ~A_SPECABILITY_L(void);
  bool                operator == (const A_SPECABILITY_L& psa) const;
  A_SPECABILITY_L&    operator = (const A_SPECABILITY_L& psa);
  POSITION            GetStartPosition(void) const;
  void                GetNextAssoc(POSITION &pos, const SPECABILITY **ppEntry) const;
  BOOL                IsLegalID(specialAbilitiesType sa) const;
  const SPECABILITY  *Insert(specialAbilitiesType sa);
  const SPECABILITY  *Find(specialAbilitiesType sa) const;
};
*/


//struct ScriptContext;


class SPECIAL_ABILITIES
{
  // A list of special abilityes for an item-type, monster-type, etc. (Arrow, Dragon, etc.)
  // Each ability name is also has the name of a 'Base-Class'.

  /**************************************************
   *    The only data member
   **************************************************/
  A_CStringPAIR_L m_specialAbilities;
  bool m_readOnly; // Not serialized

  /****************************************************
   *  Constructors and copy operators
   ***************************************************/
private:
  // These are private because if they are made public it is FAR TOO EASY
  // to cause these CPU-intensive copies accidentally.  It is seldom necessary.
  // If you must COPY then use the Copy function explicitly and put a comment
  // justifying your actions!!!
  //SPECIAL_ABILITIES&   operator =(const SPECIAL_ABILITIES& sa);
  SPECIAL_ABILITIES   (const SPECIAL_ABILITIES& sa);

  //friend struct COMBATANT;
  //friend class  CHARACTER;
  //friend class  ITEM_DATA;
  //friend class  SPELL_DATA;
  //friend class  CLASS_DATA;
  //friend class  BASE_CLASS_DATA;
  //friend class  RACE_DATA;
  //friend class  MONSTER_DATA;
  //friend struct COMBAT_DATA;


public:
  SPECIAL_ABILITIES   (bool readOnly);

  /****************************************************
   *    Functions
   ****************************************************/
public:
                     ~SPECIAL_ABILITIES(void);

  const A_CStringPAIR_L&  SpecialAbilities(void) const {return m_specialAbilities;};
  void                    Copy(const SPECIAL_ABILITIES& spab);
  void                    Copy(const A_ASLENTRY_L& asl);
  bool                    operator == (const SPECIAL_ABILITIES& psa) const;
  void                    Clear(void);
  void                    Serialize(CAR &ar, double ver, LPCSTR objName, LPCSTR objType);
  void                    Serialize(CArchive &ar, double ver, LPCSTR objName, LPCSTR objType);
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  void Import(JReader& jr);
#endif

  CStringPAIR            *FindAbility(specialAbilitiesType sa) const;

//  void                InsertString(specialAbilitiesType sa, const CString& stringName, const CString& string, unsigned char type);
  void     InsertAbility(const CString& name, const CString& parameter, LPCSTR d1, LPCSTR d2);
  CString  DeleteAbility(const CString& name);  // Returns parameter or NO_SUCH_SA
  void     InsertAbility_RO(const CString& name, const CString& parameter, LPCSTR d1, LPCSTR d2);
  CString  DeleteAbility_RO(const CString& name);  // Returns parameter or NO_SUCH_SA
//  void     Dump(void); // Dump list of SAs to $DEBUG log

  POSITION            GetHeadPosition(void) const { return m_specialAbilities.GetStartPosition();};
  const CString&      GetAt(POSITION pos) const;
  const CString&      GetNext(POSITION &pos) const;
  const CStringPAIR  *GetNextData(POSITION& pos) const;

  void                EnableSpecAb(specialAbilitiesType sa); // adds with default activation script
  void                EnableSpecAb(specialAbilitiesType sa, const CString& script, const CString& script2);
  void                DisableSpecAb(specialAbilitiesType sa);

  const CString&      GetString(specialAbilitiesType sa) const;
  DWORD               GetMsgTypes(specialAbilitiesType sa) const;
  const CString&      GetSpecAbUserScript(specialAbilitiesType sa) const;
//        CString       GenerateSpecAbFuncStart(specialAbilitiesType sa) const;
//        CString       GenerateSpecAbFuncEnd(specialAbilitiesType sa) const;
  const CString&      GetSpecAbUserDAScript(specialAbilitiesType sa) const;
        CString       GenerateSpecAbFuncName(specialAbilitiesType sa) const;
  bool                HaveAtLeastOneSpecAb() const;
  void                CrossReference(CR_LIST *pCRList, CR_REFERENCE *pCRReference) const;
  void                SetMsgFormat(specialAbilitiesType sa, DWORD type, CString &format);
        CString       GetMsgFormat(specialAbilitiesType sa, DWORD type) const;
  void                SetMsgTypes(specialAbilitiesType sa, DWORD types);
  bool                HaveSpecAb(specialAbilitiesType sa) const;

  void                EnableAllFor(CHARACTER *pChar);
  void                DisableAllFor(CHARACTER * pChar);
  int                 GetCount(void) const {return m_specialAbilities.GetCount();};
  int                 GetDataSize(void) const {return m_specialAbilities.GetDataSize();};

  CString             RunScripts(LPCSTR                  scriptName, 
                                 enum CBRESULT         (*fnc)(enum CBFUNC func, CString *scriptResult, void *pkt),
                                 void                   *pkt,
                                 LPCSTR                  comment,
                                 enum SCRIPT_SOURCE_TYPE SA_Source_Type,
                                 LPCSTR                  SA_Source_Name
                                ) const;

};



void ClearHookParameters(void);
#define NUMHOOKPARAM 10
extern class HOOK_PARAMETERS *p_hook_parameters; //CString hookParameters[NUMHOOKPARAM];
CString RunGlobalScript(const char *SAname, const char *scriptName, bool nullSA);

class HOOK_PARAMETERS
{
  HOOK_PARAMETERS *m_prevHookParameters;
  CString m_hookParameters[NUMHOOKPARAM];
public:
  HOOK_PARAMETERS(void)
  {
    m_prevHookParameters=p_hook_parameters;
    p_hook_parameters = this;
  };
  ~HOOK_PARAMETERS(void)
  {
    p_hook_parameters = m_prevHookParameters;
  };
  CString& operator[](int i) {return m_hookParameters[i];};
};



#define MAX_SPEC_AB   100

#define INVOKE_SPELL_ON_TARGET      "InvokeSpellOnTarget"
#define GET_ADJ_MAX_MOVEMENT        "GetAdjMaxMovement"
#define CAN_CAST_SPELLS             "CanCastSpells"
#define CAN_MEMORIZE_SPELLS         "CanMemorizeSpells"
#define IS_COMBAT_READY             "IsCombatReady"
#define CHAR_DISPLAY_STATUS         "CharDisplayStatus"
#define GET_NUMBER_OF_ATTACKS       "GetNumberOfAttacks"
#define GET_ITEM_TARGET_HIT_BONUS   "GetItemTargetHitBonus"
#define SPECIAL_GRAPHICS            "SpecialGraphics"

//#define ON_READY_WEAPON             "OnReadyWeapon"
//#define ON_UNREADY_WEAPON           "OnUnReadyWeapon"
//#define ON_READY_SHIELD             "OnReadyShield"
//#define ON_UNREADY_SHIELD           "OnUnReadyShield"
//#define ON_READY_ARMOR              "OnReadyArmor"
//#define ON_UNREADY_ARMOR            "OnUnReadyArmor"
//#define ON_READY_GAUNTLETS          "OnReadyGauntlets"
//#define ON_UNREADY_GAUNTLETS        "OnUnReadyGauntlets"
//#define ON_READY_HELM               "OnReadyHelm"
//#define ON_UNREADY_HELM             "OnUnReadyHelm"
//#define ON_READY_BELT               "OnReadyBelt"
//#define ON_UNREADY_BELT             "OnUnReadyBelt"
//#define ON_READY_ROBE               "OnReadyRobe"
//#define ON_UNREADY_ROBE             "OnUnReadyRobe"
//#define ON_READY_CLOAK              "OnReadyCloak"
//#define ON_UNREADY_CLOAK            "OnUnReadyCloak"
//#define ON_READY_BOOTS              "OnReadyBoots"
//#define ON_UNREADY_BOOTS            "OnUnReadyBoots"
//#define ON_READY_RING               "OnReadyRing"
//#define ON_UNREADY_RING             "OnUnReadyRing"
//#define ON_READY_AMMO               "OnReadyAmmo"
//#define ON_UNREADY_AMMO             "OnUnReadyAmmo"
#define CAN_READY                   "CanReady"
#define ON_READY                    "OnReady"
#define ON_UNREADY                  "OnUnReady"


#define ON_USE_LAST_WEAPON             "OnUseLastWeapon"
#define PRE_START_COMBAT               "PreStartCombat"
#define ON_START_COMBAT                "OnStartCombat"
#define ON_END_COMBAT                  "OnEndCombat"
#ifdef SpellDetectMagic
#define CAN_SPELL_DETECT_MAGIC         "CanSpellDetectMagic"
#else
#define CAN_CHARACTER_DETECT_MAGIC     "CanCharacterDetectMagic"
#endif
#define DOES_ATTACK_SUCCEED            "DoesAttackSucceed"
#define DOES_SPELL_ATTACK_SUCCEED      "DoesSpellAttackSucceed"
#define COMPUTE_DAMAGE                 "ComputeDamage"
#define COMPUTE_SPELL_DAMAGE           "ComputeSpellDamage"
#define RANGER_DAMAGE_BONUS            "RangerDamageBonus"
#define ON_READYCOMBATEQUIPMENT        "OnReadyCombatEquipment"
#define START_COMBAT_ROUND             "StartCombatRound"
#define IS_VALID_TARGET                "IsValidTarget"
#define ON_START_COMBATANT_ACTION      "OnStartCombatantAction"
#define ON_STEP                        "OnStep"
#define DRAW_MOVE_MENU                 "DrawMoveMenu"
#define AUTO_ACTION                    "AutoAction"
#define CAN_KNOW_SPELL                 "CanKnowSpell"
#define DISPLAY_CHARACTER_CLASS        "DisplayCharacterClass"
#define DISPLAY_CHARACTER_BASECLASS    "DisplayCharacterBaseclass"
#define DISPLAY_CHARACTER_RACE         "DisplayCharacterRace"
#define SELECT_CHARACTER_RACE          "SelectCharacterRace"
#define SELECT_CHARACTER_CLASS         "SelectCharacterClass"
#define IS_BASECLASS_ALLOWED           "IsBaseclassAllowed"
#define TRAINING_CHARACTER             "TrainingCharacter"
#define CHANGE_CLASS_FROM              "ChangeClassFrom"
#define CHANGE_CLASS_TO                "ChangeClassTo"
#define TRANSFER_MODULE                "TransferModule"
#define COMBAT_MAIN_MENU               "CombatMainMenu"
#define COMBAT_SPECIAL_ACTION          "CombatSpecialAction"
#define REDIRECT_SPELL                 "RedirectSpell"
#define CAN_CURE_OR_WHATEVER           "CanCureOrWhatever"
#define CURE_OR_WHATEVER               "CureOrWhatever"
#define CREATE_MONSTER_COMBATANT       "CreateMonsterCombatant"
#define FIX_CHARACTER                  "FixCharacter"
#define SPELL_LINGER_BLOCKAGE          "SpellLingerBlockage"
#define JOIN_PARTY                     "JoinParty"
#define CAN_AIM                        "CanAim"
#define SPELL_DURATION                 "SpellDuration"
#define SPELL_RANGE                    "SpellRange"
#define SPELL_NUMTARGETS               "SpellNumTargets"
#define SPELL_CASTER_LEVEL             "SpellCasterLevel"

#define BEGIN_RESTING                  "BeginResting"
#define GUARDING_BEGIN_COMBAT          "Guarding-BeginCombat"
#define GUARDING_CAN_GUARD             "Guarding-CanGuard"
#define GUARDING_GUARD                 "Guarding-Guard"
#define GUARDING_START_OF_ROUND        "Guarding-StartOfRound"
#define GUARDING_CAN_GUARD_ATTACK      "Guarding-CanGuardAttack"
#define FREEATTACK_CAN_FREE_ATTACK     "FreeAttack-CanFreeAttack"
#define FREEATTACK_ATTACKOVER          "FreeAttack-AttackOver"
#define GUARDING_ATTACKOVER            "Guarding-AttackOver"
#define SPELL_LINGERING_SCRIPTS        "SpellLingeringScripts"
#define ADJUST_INITIATIVE              "AdjustInitiative"

#define CHARACTER_CREATED_SA           "Global_CreateCharacter"
#define POST_CREATE_CHARACTER          "PostCreateCharacter"
#define POST_CLASS                     "PostClass"
#define KNOWABLE_SPELLS                "KnowableSpells"
#define KNOWABLE_SPELLS_MINMAX         "KnowableSpellsMinMax"
#define LEARN_SPELLS                   "LearnSpells"
#define LEARN_SPELLS_NUM               "LearnSpellsNum"
#define IDENTIFY                       "Identify"
#define CAN_IDENTIFY                   "CanIdentify"
#define OVERRIDE_SPELL_SELECTION       "OverrideSpellSelection"
#define OVERRIDE_SPELL_TARGETING       "OverrideSpellTargeting"
#define PROCESS_SPELL_TARGET_OVERRIDE  "ProcessSpellTargetOverride"
#define CHARACTER_VIEW_SCREEN          "CharacterViewScreen"
#define EVENT_MENU_TEXT                "EventMenuText"
#define EVENT_TEXT1                    "EventText1"
#define EVENT_TAVERN_DRINK             "EventTavernDrink"

#define NEW_LAY

#ifdef NEW_LAY
#define CAN_LAY_OR_WHATEVER            "CanLayOrWhatever"
#define LAY_OR_WHATEVER                "LayOrWhatever"
#endif

#define CAN_SCRIBE_OR_WHATEVER         "CanScribeOrWhatever"
#define SCRIBE_OR_WHATEVER             "ScribeOrWhatever"
#define CAN_EXAMINE_OR_WHATEVER        "CanExamineOrWhatever"
#define EXAMINE_OR_WHATEVER            "ExamineOrWhatever"
#define CAN_TURN                       "CanTurn"
#define TURN_ATTEMPT                   "TurnAttempt"
#define AURA_CREATE                    "AURA_Create"
#define AURA_EXIT                      "AURA_Exit"
#define AURA_ENTER                     "AURA_Enter"
#define AURA_ROUND                     "AURA_Round"
#define AURA_EFFECT                    "AURA_Effect"
#define GLOBAL_TIME                    "Global_Time"
#define TIME_ELAPSED                   "OnTimeElapsed"

#define NO_SUCH_SA "-?-?-"


enum CBRESULT
{
  CBR_CONTINUE = 0,
  CBR_STOP,
};

enum CBFUNC
{
  CBF_DEFAULT,
  CBF_EXAMINESCRIPT,
  CBF_ENDOFSCRIPTS
};

CBRESULT ScriptCallback_RunAllScripts       (CBFUNC func, CString *scriptResult, void *pkt);
CBRESULT ScriptCallback_MinMax              (CBFUNC func, CString *scriptResult, void *pkt);
CBRESULT ScriptCallback_LookForChar         (CBFUNC func, CString *scriptResult, void *pkt);
CBRESULT ScriptCallback_RelOrAbs            (CBFUNC func, CString *scriptResult, void *pkt);
CBRESULT ScriptCallback_YessesAndNoes       (CBFUNC func, CString *scriptResult, void *pkt);
CBRESULT ScriptCallback_AddToHookParameter1 (CBFUNC func, CString *scriptResult, void *pkt);


class ITEM_DATA;;
class CLASS_DATA;
class BASE_CLASS_DATA;
class SPELL_DATA;
class SPELLGROUP_DATA;
//class GLOBAL_SPELL_ID;
class GLOBAL_ITEM_ID;
class MONSTER_DATA;
class RACE_DATA;
class ABILITY_DATA;

#ifdef USE_TRAITS
class TRAIT_DATA;
#endif

int ConvertSpecAbFlagToIndex(DWORD flag);
specialAbilitiesType ConvertRuntimeIfTextToSpecAb(const CString &text);
CString ConvertSpecAbToRuntimeIfText(specialAbilitiesType sa);

/*
struct SpecialAbilityContext
{
//  friend struct SPECIAL_ABILITY_CONTEXT
private:
  SPECIAL_ABILITIES *pSpecialAbilities;
public:
  SpecialAbilityContext(void){Clear();};

  SPECIAL_ABILITIES *SpecialAbilities(void) {return pSpecialAbilities;};
  void SetContext(SPECIAL_ABILITIES *pSpecAb){pSpecialAbilities=pSpecAb;};
  CString DeleteAbility(const CString& name); // Return parameter or NO_SUCH_SA
  void Clear(void){pSpecialAbilities = NULL;};
  void SetParam(const CString& name, const CString& value);
};
*/


enum SCRIPT_SOURCE_TYPE
{
  ScriptSourceType_Unknown = 0,
  ScriptSourceType_Class,
  ScriptSourceType_Item,
  ScriptSourceType_Race,
  ScriptSourceType_Baseclass,
  ScriptSourceType_Spell,
  ScriptSourceType_Character,
  ScriptSourceType_Monster,
  ScriptSourceType_Combatant,
  ScriptSourceType_Aura,
  ScriptSourceType_Event,
  ScriptSourceType_EventTrigger,
  ScriptSourceType_ForEachPrtyMember,
};

struct SCRIPT_CONTEXT;
extern SCRIPT_CONTEXT *pScriptContext;
class ToHitComputation;

struct SCRIPT_CONTEXT
  {
  SCRIPT_CONTEXT *m_prevScriptContext;

  SCRIPT_CONTEXT(void)
  {
    m_prevScriptContext=pScriptContext;
    pScriptContext = this;

    context                   = NULL;          // The context of RunScripts()
    pItemContext              = NULL;
    pBaseclassContext         = NULL;
    pClassContext             = NULL;
    pSpellContext             = NULL;
    pMonstertypeContext       = NULL;
    pSpellgroupContext        = NULL;
    pRaceContext              = NULL;
    pAbilityContext           = NULL;
    pToHitComputationContext  = NULL;
#ifdef USE_TRAITS
    pTraitContext             = NULL;
#endif
    pCreatedCharacterContext  = NULL;
    scriptSourceType = ScriptSourceType_Unknown;
    eventX = -1;
    eventY = -1;
    eventID = -1;
    eventLevel = -1;
  };
  ~SCRIPT_CONTEXT(void)
  {
    pScriptContext = m_prevScriptContext;
  };
  
  
  SCRIPT_SOURCE_TYPE  scriptSourceType;   // eg: CHARACTER or SPELL or CLASS or EVENT
  LPCSTR           sourceName;            // eg: if sourcetype implies a named Special Ability
                                          //        then SA name: "George" or "Heal"
                                          //
  LPCSTR           scriptName;
  CStringPAIR      specAb;                     // The SPECIAL_ABILITY that triggered the script
  //ASLENTRY         specAbASL;
  CString          attackerContext;
  CString          targetContext;
  CString          characterContext;
  CString          combatantContext;
  /*
  SpecialAbilityContext *context;          // The context of RunScripts()
  SpecialAbilityContext itemContext;
  SpecialAbilityContext attackerContext;
  SpecialAbilityContext targetContext;
  SpecialAbilityContext characterContext;
  SpecialAbilityContext combatantContext;
  SpecialAbilityContext classContext;
  SpecialAbilityContext spellContext;
*/
  SPECIAL_ABILITIES      *context;          // The context of RunScripts()
  A_ASLENTRY_L           *contextASL;
  const ITEM_DATA        *pItemContext;
  int                     itemKey;          // The key in the character's 'myItems' inventory
  const CLASS_DATA       *pClassContext;
  const BASE_CLASS_DATA  *pBaseclassContext;
  const SPELL_DATA       *pSpellContext;
  const MONSTER_DATA     *pMonstertypeContext;
  SPELLGROUP_DATA        *pSpellgroupContext;
  const RACE_DATA        *pRaceContext;
  ABILITY_DATA           *pAbilityContext;
  const ToHitComputation *pToHitComputationContext;
  int                    eventLevel;
  int                    eventX;
  int                    eventY;
  int                    eventID;

#ifdef USE_TRAITS
  TRAIT_DATA            *pTraitContext;
#endif

  CHARACTER             *pCreatedCharacterContext;

  const CStringPAIR *GetAbility(void) const;
  void SetAbility(const SPECIAL_ABILITIES *pSpecAb, const CStringPAIR *pAb);
  void SetAbility(const A_ASLENTRY_L *pSpecAb, const ASLENTRY *pAb);
  void ClearAbility(void);
  CString RemoveAbility(void); // Remove the SA that triggerd the script (remove specAb from context)
                               // Return the parameter or NO_SUCH_SA
  void SetParam(const CString& value);  // Set specAb's parameter in context

  void MsgBoxErrorAlert(const char *context) const; // Eg: MsgBoxErrorAlert("Spell");

  void SetSA_Source_Type          (SCRIPT_SOURCE_TYPE source_Type);
  void SetSA_Source_Name          (LPCSTR source_Name);
  void SetSA_ScriptName           (LPCSTR scriptName);
  void SetSA_EventLevel           (int l){eventLevel = l;};
  void SetSA_EventX               (int x){eventX = x;};
  void SetSA_EventY               (int y){eventY = y;};
  void SetSA_EventID              (int i){eventID = i;};
  void SetItemContext             (const ITEM_DATA *pItem);
  //void SetItemContext             (GLOBAL_ITEM_ID giID);
  void SetItemContext             (const ITEM_ID& itemID);
  void SetItemContextKey          (int key){itemKey = key;};
  void SetMonsterTypeContext      (const MONSTER_DATA *pMonsterType);
  void SetAttackerContext         (const COMBATANT *pCombatant);
  void SetTargetContext           (const COMBATANT *pCombatant);
  void SetTargetContext           (const ActorType *pActor);
  void SetCharacterContext        (const CHARACTER *pCharacter);
  void SetCreatedCharacterContext (CHARACTER *pCharacter);
  void SetCombatantContext        (const COMBATANT *pCombatant);
  void SetClassContext            (const CLASS_DATA *pClass);
  void SetClassContext            (const CLASS_ID classID);
  void SetRaceContext             (const RACE_DATA *pRace);
  void SetBaseClassContext        (const BASE_CLASS_DATA *pBaseClass);
  void SetSpellContext            (const SPELL_DATA *pSpell);
  //void SetSpellContext            (GLOBAL_SPELL_ID gsID);
  void SetSpellContext            (const SPELL_ID& spellID);
  void SetSpellgroupContext       (SPELLGROUP_DATA *pSpellgroup);
  void SetAttackerContext         (const CHARACTER *pCharacter);
  void SetTargetContext           (const CHARACTER *pCharacter);
  void SetToHitComputationContext(const ToHitComputation *pToHitCC);
  void Clear(void);
  const SPECIAL_ABILITIES *GetItemSAContext(const char *msg) const;
  SPECIAL_ABILITIES *GetAttackerSAContext(const char *msg) const;
  SPECIAL_ABILITIES *GetTargetSAContext(const char *msg) const;
  SPECIAL_ABILITIES *GetCharacterSAContext(const char *msg) const;
#ifdef UAFEngine
  SPECIAL_ABILITIES *GetCombatantSAContext(const char *msg) const;
#endif
  const SPECIAL_ABILITIES *GetClassSAContext(const char *msg) const;
  const SPECIAL_ABILITIES *GetBaseclassSAContext(const char *msg) const;
  const SPECIAL_ABILITIES *GetSpellSAContext(const char *msg) const;
  const SPECIAL_ABILITIES *GetSpellgroupSAContext(const char *msg) const;
  const SPECIAL_ABILITIES *GetMonstertypeSAContext(const char *msg) const;
  const SPECIAL_ABILITIES *GetRaceSAContext(const char *msg) const;
  const SPECIAL_ABILITIES *GetAbilitySAContext(const char *msg) const;
  const SPECIAL_ABILITIES *GetTraitSAContext(const char *msg) const;
  
  const ITEM_DATA       *GetItemContext(const char *msg) const;
  int                    GetItemContextKey(void){return itemKey;};
  CString                GetAttackerContext(const char *msg) const;
  CString                GetTargetContext(const char *msg) const;
  CString                GetCharacterContext(const char *msg) const;
  CString                GetCombatantContext(const char *msg) const;
  COMBATANT             *GetAttackerCombatant(const char *msg) const;
  COMBATANT             *GetTargetCombatant(const char *msg) const;
  CHARACTER             *GetCharacter(const char *msg) const;
  CHARACTER             *GetCreatedCharacterContext(const char *msg) const;
  COMBATANT             *GetCombatant(const char *msg) const;
  const CLASS_DATA      *GetClassContext(const char *msg) const;
  const BASE_CLASS_DATA *GetBaseclassContext(const char *msg) const;
  const RACE_DATA       *GetRaceContext(const char *msg) const;
  const SPELL_DATA      *GetSpellContext(const char *msg) const;
  const MONSTER_DATA    *GetMonsterTypeContext(const char *msg) const;
  SPELLGROUP_DATA       *GetSpellgroupContext(const char *msg) const;
  const MONSTER_DATA    *GetMonstertypeContext(const char *msg) const;
  ABILITY_DATA          *GetAbilityContext(const char *msg) const;
  CString                GetSourceTypeName(void) const;
  CString                GetSourceName(void) const;

#ifdef USE_TRAITS
  TRAIT_DATA            *GetTraitContext(void);
#endif
};


extern SCRIPT_CONTEXT *pScriptContext;




#endif




#endif