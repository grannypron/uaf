var genderType = function() {
    this.Male = 0;
    this.Female = 1;
    this.Bishop = 2;
}

function genderTypeObj() {
    genderType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}
