function ACTION_TYPE() {
    this.AT_Unknown = 0;
    this.AT_SpellCaster = 1;
    this.AT_Advance = 2;
    this.AT_RangedWeapon = 3;
    this.AT_MeleeWeapon = 4;
    this.AT_Judo = 5; //Hands and feet....no weapon
    this.AT_SpellLikeAbility = 6;
}

function ACTION_TYPEEObj() {
    ACTION_TYPE.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}
