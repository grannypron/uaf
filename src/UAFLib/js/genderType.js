var genderType = {};
genderType.Male = 0;
genderType.Female = 1;
genderType.Bishop = 2;

function genderTypeObj(values) {
    entityType.call(this, values);
    Object.setPrototypeOf(this, entityType.prototype);
}
