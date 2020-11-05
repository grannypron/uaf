function MonsterPenaltyType() {
    this.PenaltyDwarfAC = 1;
    this.PenaltyGnomeAC = 2;
    this.PenaltyDwarfTHAC0 = 4;
    this.PenaltyGnomeTHAC0 = 8;
    this.PenaltyRangerDmg = 16
}

function MonsterPenaltyTypeObj() {
    MonsterPenaltyType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}

