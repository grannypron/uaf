function CHARACTER_SPELL() {
    this.selected = 0;  // TRUE if dude will memorize this spell again
    this.memorized = 0; // 
    this.spellID = 0;      // SpellDbKey to retrieve data from spellData table
    this.level = 0;      // spell level, here for convenience
    this.memTime = 0;    // total time spent on memorizing this spell
    this.JustMemorized = false;
    
}

CHARACTER_SPELL.prototype.Clear = function () {
    this.memorized = 0;
    this.spellID = 0;
    this.level = 0;
    this.selected = 0;
    this.memTime = 0;
    this.JustMemorized = false;
}

CHARACTER_SPELL.prototype.SerializeCAR = function (ar, version) {
    if (ar.IsStoring()) {
        ar.writeString(this.spellID);
        ar.writeInt(this.memorized);
        ar.writeInt(this.level);
        ar.writeInt(this.selected);
    }
    else {
        this.Clear();
        {
            this.spellID = ar.readString();
        };
        this.memorized = ar.readInt();
        this.level = ar.readInt();
        this.selected = ar.readInt();
    }

}

/**TODO void Serialize(CArchive & ar, double version);
BOOL IsSelected() const { return (selected > 0); }
BOOL IsMemorized() const { return (memorized > 0); }
void ResetMemorizedTime() { memTime = 0; }
BOOL IncMemorizedTime(int minuteInc, bool all);
BOOL MemTimeSufficient();
void SetMemorized(bool all);
void SetUnMemorized();
BOOL HaveUnmemorized() const ;

**/