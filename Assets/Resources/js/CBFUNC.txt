function CBFUNC() {
    this.CBF_DEFAULT = 0;
    this.CBF_EXAMINESCRIPT = 1;
    this.CBF_ENDOFSCRIPTS = 2;
}

function CBFUNCObj() {
    CBFUNC.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}

