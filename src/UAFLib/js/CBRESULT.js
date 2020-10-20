function CBRESULT() {
    this.CBR_CONTINUE = 0;
    this.CBR_STOP = 1;
}

function CBRESULTObj() {
    entityType.call(this, CBRESULT);
    Object.setPrototypeOf(this, entityType.prototype);
}

