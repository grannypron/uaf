function GameRules() {
    this.NUM_RACE_TYPES = 6;
    this.NUM_GENDER_TYPES = 2;
    this.NUM_ALIGNMENT_TYPES = 9;
    this.NUM_CLASS_TYPES = 19;
    this.NUM_CHAR_STATUS_TYPES = 10;

    this.SKILL_DIE = 6;
}

GameRules.prototype.rollSkillDie = function(abilityName) {
    if (globalData.version < Globals._VERSION_0870_) {
        var tmp1 = Globals.RollDice(this.SKILL_DIE, 3);
        var tmp2 = Globals.RollDice(this.SKILL_DIE, 3);
        var tmp3 = Globals.RollDice(this.SKILL_DIE, 3);
        return (Math.max(tmp1, Math.max(tmp2, tmp3)));
    }
    else {
        var result;
        var pAbilityData;
        var tmp1 = 0, tmp2 = 0, tmp3 = 0;

        pAbilityData = abilityData.PeekAbilityData(abilityData.LocateAbilityData(abilityName));
        if (pAbilityData != null) {
            if (pAbilityData.RollAbility(result))
                tmp1 = result;
            if (pAbilityData.RollAbility(result))
                tmp2 = result;
            if (pAbilityData.RollAbility(result))
                tmp3 = result;
        };

        return (Math.max(tmp1, Math.max(tmp2, tmp3)));
    }
}

