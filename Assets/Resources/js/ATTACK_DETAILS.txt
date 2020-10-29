function ATTACK_DETAILS() {
    this.sides = 0;
    this.nbr = 0;
    this.bonus = 0;
    this.attackMsg = "";
    this.spellID = "";
    this.spellclass = 0;
    this.spelllevel = 0;
    this.Clear();
}

ATTACK_DETAILS.prototype.Clear = function() {
    this.sides = 0;
    this.nbr = 0;
    this.bonus = 0;
    this.attackMsg = "";
    this.spellID = "";
    this.spellclass = 0;
    this.spelllevel = 0;
}