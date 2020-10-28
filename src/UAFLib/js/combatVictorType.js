function combatVictorType() {
    this.NoWinner = 0;
    this.PartyWins = 1;
    this.PartyRanAway = 2;
    this.MonsterWins = 3;
}

function combatVictorTypeObj() {
    combatVictorType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}
