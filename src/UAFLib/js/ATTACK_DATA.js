function ATTACK_DATA() {
    this.monsterAttackDetails = new CList();
    this.Clear();
}

ATTACK_DATA.prototype.Clear = function() {
    this.monsterAttackDetails.RemoveAll();
}

