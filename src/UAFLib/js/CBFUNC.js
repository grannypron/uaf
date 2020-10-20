function CBFUNC() {
    this.CBF_DEFAULT = 0;
    this.CBF_EXAMINESCRIPT = 1;
    this.CBF_ENDOFSCRIPTS = 2;
}

function CBFUNCObj() {
    entityType.call(this, CBFUNC);
    Object.setPrototypeOf(this, entityType.prototype);
}

