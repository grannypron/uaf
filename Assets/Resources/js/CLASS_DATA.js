function CLASS_DATA() {
    this.m_baseclasses = new BASECLASS_LIST();
    this.hitDiceLevelBonus = [];    //HIT_DICE_LEVEL_BONUS
    this.strengthBonusDice = new DICEPLUS();
    this.preSpellNameKey = 0;
    this.m_name = "";
    this.m_specialAbilities = new SPECIAL_ABILITIES();
    this.m_startingEquipment = new ITEM_LIST();
    this.hitDiceBaseclassID = "";
};

CLASS_DATA.prototype.GetCount = function () {
    return this.m_baseclasses.length;
}

/** TODO

  friend class CLASS_DATA_TYPE;
  friend static void DclassBaseclass(CONFIG_PKT *pkt);
  friend static void DclassHitDiceBonus(CONFIG_PKT *pkt);
  friend static void DclassHitDiceLevelBonus(CONFIG_PKT *pkt);
  int DetermineCharHitDice(const CHARACTER *pChar) const;
CString RunClassScripts(LPCSTR     scriptName,
    CBRESULT(* fnc)(CBFUNC func, CString * scriptResult, void * pkt),
    void      * pkt,
    LPCSTR     comment) const
        {
            return m_specialAbilities.RunScripts(scriptName,
                fnc,
                pkt,
                comment,
                ScriptSourceType_Class,
                m_name);
  };
public:
CLASS_DATA(void);
~CLASS_DATA(void);
CLASS_DATA & operator =(const CLASS_DATA& src);
BOOL operator == (const CLASS_DATA& src) const ;
void Clear(void);

//The following should someday become unnecessary.  When
//classType is defined as a DWORD like spellType, abilityType, etc.
//For now, classType is an enum.i
CLASS_ID ClassID(void) const { CLASS_ID cid; cid = m_name; return cid;};
CLASS_ID WhatIsYourClassType(void) const { return ClassID();};
int GetDataSize(void) const ;



int                 LocateBaseclass(const BASECLASS_ID& baseclassID) const ;
const BASECLASS_ID * PeekBaseclassID(int i) const { return m_baseclasses.PeekBaseclassID(i);};
void AddBaseclass(const BASECLASS_ID& baseclassID);
// If not already present.
void AddAbilityLimit(ABILITY_ID abilityID,
    int min, int minmod,
    int max, int maxmod);
// Or replace the current one.
int Serialize(CAR & car);
void PreSerialize(BOOL IsStoring);
void PostSerialize(BOOL IsStoring);
ABILITYLIMITS GetAbilityLimits(ABILITY_ID abilityID) const ; // Min/Max allowed for this class
WORD GetAllowedAlignments(void) const ;

int GetCharTHAC0(const CHARACTER * pCharacter) const ;
void ComputeCharSavingThrows(const CHARACTER *, int * ppd, int * rsw, int * pp, int * br, int * sp) const ;
void GetThiefSkills(const CHARACTER * pChar,
    int * pp,
    int * ol,
    int * ft,
    int * ms,
    int * hs,
    int * hn,
    int * cw,
    int * rl) const ;


bool IsRaceAllowed(const RACE_ID& raceID) const ;
void CreateAllowedSchoolList(SCHOOL_LIST * pSchoolList) const ;
BOOL MatchSpellBaseclass(const SPELL_DATA * pSpell) const ;
BOOL MatchSpellSchool(const SCHOOL_LIST * pSchoolList) const ;
const BASECLASS_LIST * BaseclassList(void) const { return & m_baseclasses;};

};

*/