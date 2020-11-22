function INITIATIVE() {
    this.INITIATIVE_AlwaysFirst = 1;
    this.INITIATIVE_FirstDefault = 9;
    this.INITIATIVE_LastDefault = 18;
    this.INITIATIVE_Never = 23;
}

function INITIATIVEObj() {
    INITIATIVE.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}
