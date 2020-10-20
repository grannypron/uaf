function eventTurnUndeadModType() {
    this.None = 0;
    this.Hard = 1;
    this.Difficult = 2;
    this.Impossible = 3;

    this.NUM_TURN_MOD_TYPES = 4;
}

function eventTurnUndeadModTypeObj() {
    entityType.call(this, charStatusType);
    Object.setPrototypeOf(this, entityType.prototype);
}
