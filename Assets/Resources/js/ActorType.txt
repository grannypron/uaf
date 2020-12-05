function ActorType() {
    this.EnemyAlly = 0;  // long
    this.Flags;  // long
    this.Instance;  // long
    this.Level;  // long
    this.instanceType = 0;
    this.m_classID = "";
    this.m_itemID = "";
    this.m_monsterID = "";
    this.m_characterID = "";
    this.m_raceID = "";
    this.m_spellID = "";
    this.m_schoolID = "";
    this.m_canFinishCasting = false;
    this.Clear();
}

ActorType.prototype.Clear = function () {
    this.m_raceID = "";
    this.m_classID = "";
    this.Flags = 0;
    this.Instance = 0;
    this.Level = 0;
    this.EnemyAlly = EA_ALLY;
    this.instanceType = ActorInstanceType.InstanceType_Unknown;
    this.m_itemID= "";
    this.m_canFinishCasting = false;
}


ActorType.prototype.SetCombatPartySrc = function (friendly) {
    if (friendly == null || friendly == undefined) { friendly = true; }
    this.EnemyAlly = (friendly ? EA_ALLY : EA_ENEMY);
    this.Flags = FLAG_COMBAT | FLAG_PARTY_MEMBER;
}

ActorType.prototype.SetCombatNPCSrc = function (friendly) {
    if (friendly == null || friendly == undefined) { friendly = false; } 
    this.EnemyAlly = (friendly ? EA_ALLY : EA_ENEMY);
    this.Flags = FLAG_COMBAT | FLAG_NPC;
}

ActorType.prototype.SetCombatMonsterSrc = function (friendly) {
    this.EnemyAlly = (friendly ? EA_ALLY : EA_ENEMY);
    this.Flags = FLAG_COMBAT | FLAG_MONSTER;
}
ActorType.prototype.SetPartySrc = function (friendly) {
    if (friendly == null || friendly == undefined) { friendly = false; }
    this.EnemyAlly = (friendly ? EA_ALLY : EA_ENEMY);
    Flags = FLAG_NONCOMBAT | FLAG_PARTY_MEMBER;
}
ActorType.prototype.ToString = function() {
    //return CString((LPCTSTR)(this), sizeof(ActorType));    // PORT NOTE:  Not exactly sure about this trick, but I think it is just a way of pointing to the object on the heap.  Idk why they would use a string pointer, but ok
    return this;    
}

/**TODO
ActorType(int iEnemyAlly, ActorInstanceType iInstanceType)
{
    Clear();
    instanceType = iInstanceType;
    EnemyAlly = iEnemyAlly;
};
ActorType(void){ Clear(); };

void Clear();
BOOL operator == (const ActorType & src) const ;
BOOL operator != (const ActorType & src) const ;
void Serialize(CAR & ar, double ver);
void Serialize(CArchive & ar, double ver);
void XYSet(unsigned short x, unsigned short y);
void XYGet(unsigned short & x, unsigned short & y);
void FromString(const CString& str);



void SetItemSrc();
void SetNPCSrc(BOOL friendly = FALSE);
void SetCreatedCharacterSrc(BOOL friendly = FALSE);
void SetFakeCharacterSrc(BOOL friendly = FALSE);
void SetMonsterSrc(BOOL friendly = FALSE);
void SetFakeCombatantSrc(BOOL friendly = FALSE);
void SetBishopSrc(BOOL friendly = TRUE);

BOOL IsCombatSrc() const ;
BOOL IsPartySrc() const ;
BOOL IsNPCSrc() const ;
BOOL IsMonsterSrc() const ;
BOOL IsItemSrc() const ;
BOOL IsFakeCombatantSrc() const ;
BOOL IsBishopSrc() const ;
BOOL IsFakeCharacterSrc() const ;
BOOL IsCreatedCharacterSrc() const ;
BOOL IsCombatant() const ;
int  DetermineLevel(const SCHOOL_ID& schoolID, int spellLevel);

BOOL IsValidSrc() const ;
#ifdef SpellInitiationScript
void ExitCombat(void);
#endif
    unsigned long GetInstance() const ;
    unsigned long GetSrc() const ;


**/