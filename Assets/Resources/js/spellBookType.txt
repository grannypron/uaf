/** TODO **/
function spellBookType() {
    this.spellLimits = new spellLimitsType();
    this.list = new SPELL_LIST();
    this.spellPrepTimeNeeded = 0;  //DWORD 
    this.spellPrepTimeUsed = 0;  //DWORD 
    this.Clear();
}

spellBookType.prototype.Clear = function () {
    this.spellLimits.Clear();
    this.list.Clear();
    this.spellPrepTimeNeeded = 0;
    this.spellPrepTimeUsed = 0;
}

spellBookType.prototype.SerializeCAR = function(ar, version) {
    this.spellLimits.SerializeCAR(ar, version);
    this.list.SerializeCAR(ar, version);
}

