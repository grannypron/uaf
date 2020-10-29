/** TODO **/
function SPELL_ABILITY() {
    this.valid = false;
    this.schoolAbilities = [];
//For each spell school we will record
// * Highest spell level
// * Number of spells that can be memorized at each spell level
    /** TODO: 
    DEFINE_mCARRAY_ACCESS_FUNCTIONS(SchoolAbility,    / array data name      / \
        SCHOOL_ID,        / Search variable type / \
        schoolID,         / search variable name / \
        schoolAbilities,  / array name           / \
        SCHOOL_ABILITY,   / array data type      / \
    SPELL_ABILITY)    / type of *this        /
    **/
}

SPELL_ABILITY.prototype.Clear = function () {
    this.valid = false;
    this.schoolAbilities = [];
}


