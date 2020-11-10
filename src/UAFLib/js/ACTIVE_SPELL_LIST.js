function ACTIVE_SPELL_LIST() {
    this.m_CS = null;//new CCriticalSection();  /** TODO??** Maybe, if we actually need it */
    this.m_spellList = [];  //OrderedQueue < ACTIVE_SPELL, MAX_ACTIVE_SPELLS > 
    this.xClear();
}

ACTIVE_SPELL_LIST.prototype.xClear = function () {
    this.m_spellList = [];
}

ACTIVE_SPELL_LIST.prototype.ActivateLingerSpellsOnTarget = function (targ, mapx, mapy, width, height) {
    Globals.ASSERT(targ != NO_DUDE, "targ != NO_DUDE");

    var AtLeastOneSpellActivated = false;

    // PORT NOTE:  Rewrote to use a simple array
    for (var idx = 0; idx < this.m_spellList.length; idx++) {
        var as = this.m_spellList[pos];
        Globals.ASSERT(as.key >= 0, "as.key >= 0");

        if (as.LingerSpellAffectsTarget(targ, mapx, mapy, width, height)) {
            AtLeastOneSpellActivated = true;
            as.lingerData.AddTarget(targ);

            var caster = as.caster.GetInstance();
            Globals.ASSERT(caster != NO_DUDE, "caster != NO_DUDE");

            if (caster == NO_DUDE)
                continue;

            var cdude = Globals.GetCombatantPtr(caster);

            combatData.InvokeAreaLingerSpellOnCombatant(cdude.self, targ, as.key, false, null, null, as.spellID);
        }
    }

    return AtLeastOneSpellActivated;
}


ACTIVE_SPELL_LIST.prototype.LingerSpellAffectsTarget = function (targ, mapx, mapy, width, height)
{
    // PORT NOTE:  Rewrote to use a simple array
    for (var idx = 0; idx < this.m_spellList.length; idx++) {
        if (this.m_spellList[pos].LingerSpellAffectsTarget(targ, mapx, mapy, width, height))
            return true;
    }
    return false;
}

/**TODO**

class ACTIVE_SPELL_LIST {

int GetCount() {/* CSingleLock sLock(&m_CS, TRUE);* / return m_spellList.GetCount(); }
POSITION GetHeadPosition() const { return m_spellList.GetHeadPosition(); }
ACTIVE_SPELL & GetAtPos(POSITION pos)  { /*CSingleLock sLock(&m_CS, TRUE);* / return m_spellList.GetAtPos(pos); }
const ACTIVE_SPELL& PeekAtPos(POSITION pos) const { return m_spellList.PeekAtPos(pos); }
ACTIVE_SPELL & GetNext(POSITION & pos) { return m_spellList.GetNext(pos); }
const ACTIVE_SPELL& PeekNext(POSITION & pos) const { return * m_spellList.PeekNext(pos); }
int GetKeyAt(POSITION pos) { return m_spellList.GetKeyAt(pos); }
ACTIVE_SPELL_LIST & operator=(ACTIVE_SPELL_LIST & src);

// called when loading from scripts, or
// when adding from editor GUI
int xAdd(ACTIVE_SPELL & data);
// called when loading binary data that
// was saved with key info
int xAddWithCurrKey(ACTIVE_SPELL & data);
BOOL Set(int index, ACTIVE_SPELL & spell);
BOOL Remove(int index, const SPELL_DATA * pSpell);
ACTIVE_SPELL * Get(int index);
int GetNextKey();
void Serialize(CAR & ar, double ver);
void Serialize(CArchive & ar, double ver);
void PreSerialize(BOOL IsStoring) { }
void PostSerialize(BOOL IsStoring) { }
void ProcessTimeSensitiveData(int roundInc);
void DeactivateActiveSpell(int index, const SPELL_DATA * pSpell);
BOOL LingerSpellBlocksCombatant(int mapx, int mapy, COMBATANT * pCombatant) const ;
void RemoveLingerSpells();
CString RunSEScripts(int x, int y, const CString& scriptName, LPCSTR comment);
#ifdef SpellInitiationScript
void ExitCombat(void);  // Convert Actor references from combat to non-combat
#endif
ACTIVE_SPELL_LIST(ACTIVE_SPELL_LIST & src); // not enabled
};*/