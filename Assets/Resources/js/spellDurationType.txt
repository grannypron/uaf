var spellDurationType = function () {
    this.inRounds = 1;
    this.byDamageTaken = 2;
    this.inHours = 3;
    this.inDays = 4;
    this.Permanent = 5;
    this.byNbrAttacks = 6;
    this.NUM_SPELL_DURATION_TYPES = 6;
}

function spellDurationTypeObj() {
    spellDurationType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}
