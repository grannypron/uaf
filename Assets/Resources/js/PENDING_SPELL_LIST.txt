function PENDING_SPELL_LIST() {
    //CCriticalSection m_CS;
    this.m_spells = new OrderedQueue();
    this.Clear();
}

PENDING_SPELL_LIST.prototype.Clear = function () {
    this.m_spells.RemoveAll();
}

PENDING_SPELL_LIST.prototype.GetCount = function () {
    return this.m_spells.GetCount();
}

PENDING_SPELL_LIST.prototype.GetHeadPosition = function () {
    return this.m_spells.GetHeadPosition();
}

PENDING_SPELL_LIST.prototype.ProcessTimeSensitiveData = function (roundInc, currInitiative) {
    if (!currInitiative) { currInitiative = -1; }

    var castIt = false;
    //
    // what happens to pending spells when combat ends?
    //
    Globals.ASSERT(Globals.IsCombatActive());
    if (Globals.IsCombatActive() == false) return false;

    var pos = this.GetHeadPosition();
    while (pos != null) {
        var tmp = pos;
        this.GetNext(tmp);
        var data = this.PeekAtPos(pos);

        Globals.ASSERT(data.caster.IsValidSrc());
        Globals.ASSERT(data.caster.IsCombatSrc());

        switch (data.caster.GetSrc()) {
            case FLAG_PARTY_MEMBER:
            case FLAG_NPC:
            case FLAG_MONSTER:
                {
                    var type = spellCastingTimeType.GetByDWORD(data.flags);
                    switch (type) {
                        case spellCastingTimeType.sctImmediate:
                            castIt = true;
                            break;
                        case spellCastingTimeType.sctInitiative:
                            {
                                castIt = ((currInitiative >= data.waitUntil) || (roundInc > 0));
                            }
                            break;
                        case spellCastingTimeType.sctRounds:
                        case spellCastingTimeType.sctTurns:
                            castIt = (data.waitUntil <= this.GetCurrentRound());
                            break;
                    }
                }
                break;

            case FLAG_ITEM:
                // item spells are cast right away
                castIt = true;
                break;
        }

        if (castIt) {
            if (logDebuggingInfo) {
                Globals.WriteDebugString("Activating pending spell for char " + data.caster.Instance + ", sdbkey " + data.spellID.UniqueName() + ", turn " + combatData.m_iCurrTurn + ", round " + combatData.m_iCurrRound + ", initiative " + combatData.m_iCurrInitiative + "\n");
                Globals.WriteDebugString((this.GetCount() - 1) + " pending spells remaining\n");
            };
            this.SpellActivate(data, false, null);
            this.RemoveAt(pos);
        }

        pos = tmp;
    }
    return castIt;
}

/*
PENDING_SPELL & GetAtPos(POSITION pos)       { return m_spells.GetAtPos(pos); }
const PENDING_SPELL& PeekAtPos(POSITION pos) const { return m_spells.PeekAtPos(pos);
void RemoveAt(POSITION pos) { m_spells.RemoveAt(pos); }
PENDING_SPELL & GetNext(POSITION & pos) { return m_spells.GetNext(pos); }
int             GetKeyAt(POSITION pos) { return m_spells.GetKeyAt(pos); }
int Add(PENDING_SPELL & data);
BOOL Set(int index, PENDING_SPELL & spell);
BOOL Remove(int index);
PENDING_SPELL * Get(int index);
int GetNextKey();
PENDING_SPELL_LIST(PENDING_SPELL_LIST & src); // not enabled
PENDING_SPELL_LIST & operator=(PENDING_SPELL_LIST & src); // not enabled
*/