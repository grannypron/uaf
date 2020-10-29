function CBRESULT() {
    this.CBR_CONTINUE = 0;
    this.CBR_STOP = 1;
}

function CBRESULTObj() {
    CBRESULT.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}

