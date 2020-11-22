/** TODO **/
function SPELL_DATA_TYPE() {
    this.SpellData = new CList();
    this.version = 0.0;
}

SPELL_DATA_TYPE.prototype.Clear = function () {
    this.SpellData.RemoveAll();
    this.version = 0.0; 
}

SPELL_DATA_TYPE.prototype.FreeArt = function() {
    var i, n;
    n = this.GetCount();
    for (i = 0; i < n; i++) {
        this.GetItem(i).FreeArt();
    }
}

SPELL_DATA_TYPE.prototype.GetCount = function () {
    return this.SpellData.GetCount();
}

SPELL_DATA_TYPE.prototype.ClearSounds = function () {
    var i, n;
    n = this.GetCount();
    for (i = 0; i < n; i++) {
        this.GetItem(i).ClearSounds();
    };
}


SPELL_DATA_TYPE.prototype.IsValidSpell = function (spellID) {
    return this.LocateSpell(spellID) >= 0;
}

SPELL_DATA_TYPE.prototype.LocateSpell = function(spellID) {
    var i = 0, n = 0;
    n = this.GetCount();
    for (i = 0; i < n; i++) {
        var pSpellData;
        pSpellData = this.PeekSpell(i);
        if (spellID == pSpellData.SpellID()) return i;
    };
    return -1;
}

SPELL_DATA_TYPE.prototype.GetCount = function () {
    return this.SpellData.GetCount();   // PORT NOTE: was .GetSize()
}