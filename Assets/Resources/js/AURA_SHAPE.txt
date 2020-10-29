function AURA_SHAPE() {
    this.AURA_SHAPE_NULL = 0;
    this.AURA_SHAPE_GLOBAL = 1;
    this.AURA_SHAPE_ANNULARSECTOR = 2;
}

function AURA_SHAPEObj() {
    AURA_SHAPE.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}