function RACE_DATA() {

    this.preSpellNameKey = 0;
    this.m_race_asl = new A_ASLENTRY_L();
    this.m_temp_asl = new A_ASLENTRY_L();
    this.m_name = "";
    this.m_weight = new DICEPLUS();
    this.m_height = new DICEPLUS();
    this.m_age = new DICEPLUS();
    this.m_maxAge = new DICEPLUS();
    this.m_baseMovement = new DICEPLUS(); // Positive
    this.m_canChangeClass = false;
    this.m_dwarfResistance = false;
    this.m_gnomeResistance = false;
    this.m_findSecretDoor = 0;
    this.m_findSecretDoorSearching = 0;

    this.m_abilityRequired = [];  //ABILITY_REQ

    this.m_skills = [];   //SKILL
    this.m_skillAdjustmentsAbility = [];    //SKILL_ADJUSTMENT_ABILITY
    this.m_skillAdjustmentsBaseclass = [];   //SKILL_ADJUSTMENT_BASECLASS
    this.m_skillAdjustmentsRace = [];   //SKILL_ADJUSTMENT_RACE
    this.m_skillAdjustmentsScript = [];    //SKILL_ADJUSTMENT_SCRIPT
    this.m_specAbs = new SPECIAL_ABILITIES();

    /*
    friend static void raceObject(CONFIG_PKT * pkt);
friend static void DraceSpecialAbility(CONFIG_PKT * pkt);
friend static void DraceMultiClass(CONFIG_PKT * pkt);
friend static void DraceKey(CONFIG_PKT * pkt);
friend int ReadRaceDB(LPCSTR file,
    RACE_DATA_TYPE * racePointer,
    CONFIG_FILE * pConfigFile);



int LocateAbilityReq(const CString& abilityName) const ;
void GetSkillValue(SKILL_COMPUTATION & SC) const ;
void UpdateSkillValue(SKILL_COMPUTATION & SC) const ;
CString RunRaceScripts(LPCSTR     scriptName,
    CBRESULT(* fnc)(CBFUNC func, CString * scriptResult, void * pkt),
    void      * pkt,
    LPCSTR     comment
) const
    {
        return m_specAbs.RunScripts(scriptName,
            fnc,
            pkt,
            comment,
            ScriptSourceType_Race,
            m_name);
  };
RACE_DATA();
RACE_DATA(const RACE_DATA & src);
~RACE_DATA();
RACE_DATA * operator=(const RACE_DATA &src);
RACE_ID RaceID(void) const { RACE_ID raceID; raceID = m_name; return raceID;};
int GetDataSize(void) const ;
void Clear(void);
void LogUndefinedMultiClassReferences(void);
int Serialize(CAR & car, const CString& version);
void PreSerialize(BOOL IsStoring);
void PostSerialize(BOOL IsStoring);
//void AddAbilityRequirement(ABILITY_LIMITS& abReq);
void AddAbilityRequirement(ABILITY_REQ & abReq);
void AddAllowedClass(const CString& className);
CString Name(void) const { return m_name; }
DWORD GetWeight(void) const ;
DWORD GetHeight(void) const ;
DWORD GetStartAge(void) const ;
DWORD GetMaxAge(void) const ;
DWORD GetBaseMovement(void) const ;
short GetMinAbility(const CString& abilityName) const ;
short GetMaxAbility(const CString& abilityName) const ;

//Check to see if a particular class is allowed.
bool IsAllowedClass(const CString& className) const ;
};

*/
}

RACE_DATA.prototype.RaceID = function () {
    return this.m_name;
}