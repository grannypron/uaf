function AURA_ATTACHMENT() {
    this.AURA_ATTACH_NONE = 0;
    this.AURA_ATTACH_COMBATANT = 1;
    this.AURA_ATTACH_COMBATANT_FACING = 2;
    this.AURA_ATTACH_XY = 2;
}

function AURA_ATTACHMENTObj() {
    AURA_ATTACHMENT.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}