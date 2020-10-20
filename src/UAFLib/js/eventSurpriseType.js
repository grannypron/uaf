function eventSurpriseType() { 
    this.Neither = 0;
    this.PartySurprised = 1;
    this.MonsterSurprised = 2;
    this.NUM_SURPRISE_TYPES = 3;
};

function eventSurpriseTypeObj() {
    entityType.call(this, eventSurpriseType);
    Object.setPrototypeOf(this, entityType.prototype);
}
