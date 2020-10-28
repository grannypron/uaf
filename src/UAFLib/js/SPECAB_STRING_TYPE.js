function SPECAB_STRING_TYPE() { 
    this.SPECAB_SCRIPT = 1;
    this.SPECAB_CONSTANT = 2;
    this.SPECAB_VARIABLE = 3;
    this.SPECAB_INCLUDE = 4;
    this.SPECAB_BINARYCODE = 5;
    this.SPECAB_SCRIPTERROR = 6;
    this.SPECAB_INTEGERTABLE = 7;
    this.SPECAB_BINARYTABLE = 8;
    this.SPECAB_TABLEERROR = 9;
};

function SPECAB_STRING_TYPEObj() {
    SPECAB_STRING_TYPE.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}

