function SPELL_LIST() {
    this.spells = [];
}

SPELL_LIST.prototype.SerializeCAR = function(ar, version) {
    if (ar.IsStoring()) {
        var i, n;
        n = this.GetCount();
        ar.writeInt(n);
        for (i = 0; i < n; i++) {
            this.GetCharacterSpell(i).SerializeCAR(ar, version);
        };
    }
    else {
        var count;
        var spell = new CHARACTER_SPELL();
        this.Clear();
        count = ar.readInt();
        for (i = 0; i < count; i++)
        {
            spell.SerializeCAR(ar, version);
            this.AddSpell(spell);
        }
    }
}

SPELL_LIST.prototype.Clear = function () {
    this.spells = [];
}

SPELL_LIST.prototype.GetCharacterSpell = function (i) {
    return this.spells[i];
}

SPELL_LIST.prototype.AddSpell = function (spell) {
    return this.spells.push(spell);
}
