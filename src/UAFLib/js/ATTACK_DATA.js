function ATTACK_DATA() {
    this.monsterAttackDetails = new CList();
    this.Clear();
}

ATTACK_DATA.prototype.Clear = function() {
    this.monsterAttackDetails.RemoveAll();
}

ATTACK_DATA.prototype.GetMonsterAttackDetailsCount = function () {
    return this.monsterAttackDetails.GetCount();
}

ATTACK_DATA.prototype.PeekMonsterAttackDetails = function (index) {
    return this.monsterAttackDetails.GetAtPos(index);
}
