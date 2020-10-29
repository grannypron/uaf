function OBSTICAL_TYPE() {
    this.OBSTICAL_none = 0;
    this.OBSTICAL_wall = 1;
    this.OBSTICAL_occupied = 2;
    this.OBSTICAL_offMap = 3;
    this.OBSTICAL_lingeringSpell = 4;
}

function OBSTICAL_TYPEObj() {
    OBSTICAL_TYPE.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}


