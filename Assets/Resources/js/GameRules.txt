function GameRules() {
    this.START_AGE = 17;

    this.NUM_RACE_TYPES = 6;
    this.NUM_GENDER_TYPES = 2;
    this.NUM_ALIGNMENT_TYPES = 9;
    this.NUM_CLASS_TYPES = 19;
    this.NUM_CHAR_STATUS_TYPES = 10;

    this.MAX_MOVE = 12;

    this.FIGHTER_HIT_DIE = 10;
    this.CLERIC_HIT_DIE = 8;
    this.THIEF_HIT_DIE = 6;
    this.MAGIC_USER_HIT_DIE = 4;
    this.RANGER_HIT_DIE = 8;
    this.DRUID_HIT_DIE = this.CLERIC_HIT_DIE;
    this.PALADIN_HIT_DIE = this.FIGHTER_HIT_DIE;

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

GameRules.prototype.determineStrengthProperties = function(strength, strengthMod, hitBonus, dmgBon, openDoor, openMagicDoor, BB_LG) {
    switch (strength) {
        case 1:
            hitBonus = -5;
            dmgBon = -4;
            openDoor = 1;      // locked door
            openMagicDoor = 0; // locked needing spell to open
            BB_LG = 0;
            break;
        case 2:
            hitBonus = -3;
            dmgBon = -2;
            openDoor = 1;
            openMagicDoor = 0;
            BB_LG = 0;
            break;
        case 3:
            hitBonus = -3;
            dmgBon = -1;
            openDoor = 2;
            openMagicDoor = 0;
            BB_LG = 0;
            break;
        case 4:
        case 5:
            hitBonus = -2;
            dmgBon = -1;
            openDoor = 3;
            openMagicDoor = 0;
            BB_LG = 0;
            break;
        case 6:
        case 7:
            hitBonus = -1;
            dmgBon = 0;
            openDoor = 4;
            openMagicDoor = 0;
            BB_LG = 0;
            break;
        case 8:
        case 9:
            hitBonus = 0;
            dmgBon = 0;
            openDoor = 5;
            openMagicDoor = 0;
            BB_LG = 1;
            break;
        case 10:
        case 11:
            hitBonus = 0;
            dmgBon = 0;
            openDoor = 6;
            openMagicDoor = 0;
            BB_LG = 2;
            break;
        case 12:
        case 13:
            hitBonus = 0;
            dmgBon = 0;
            openDoor = 7;
            openMagicDoor = 0;
            BB_LG = 4;
            break;
        case 14:
        case 15:
            hitBonus = 0;
            dmgBon = 0;
            openDoor = 8;
            openMagicDoor = 0;
            BB_LG = 7;
            break;
        case 16:
            hitBonus = 0;
            dmgBon = 1;
            openDoor = 9;
            openMagicDoor = 0;
            BB_LG = 10;
            break;
        case 17:
            hitBonus = 1;
            dmgBon = 1;
            openDoor = 10;
            openMagicDoor = 0;
            BB_LG = 13;
            break;
        case 18:
            if (strengthMod == 0) {
                hitBonus = 1;
                dmgBon = 2;
                openDoor = 11;
                openMagicDoor = 0;
                BB_LG = 16;
            }
            else if (strengthMod < 51) {
                hitBonus = 1;
                dmgBon = 3;
                openDoor = 12;
                openMagicDoor = 0;
                BB_LG = 20;
            }
            else if (strengthMod < 76) {
                hitBonus = 2;
                dmgBon = 3;
                openDoor = 13;
                openMagicDoor = 0;
                BB_LG = 25;
            }
            else if (strengthMod < 91) {
                hitBonus = 2;
                dmgBon = 4;
                openDoor = 14;
                openMagicDoor = 0;
                BB_LG = 30;
            }
            else if (strengthMod < 100) {
                hitBonus = 2;
                dmgBon = 5;
                openDoor = 15;
                openMagicDoor = 3;
                BB_LG = 35;
            }
            else {
                hitBonus = 3;
                dmgBon = 6;
                openDoor = 16;
                openMagicDoor = 6;
                BB_LG = 40;
            }
            break;
        case 19:
            hitBonus = 3;
            dmgBon = 7;
            openDoor = 16;
            openMagicDoor = 8;
            BB_LG = 50;
            break;
        case 20:
            hitBonus = 3;
            dmgBon = 8;
            openDoor = 17;
            openMagicDoor = 10;
            BB_LG = 60;
            break;
        case 21:
            hitBonus = 4;
            dmgBon = 9;
            openDoor = 17;
            openMagicDoor = 12;
            BB_LG = 70;
            break;
        case 22:
            hitBonus = 4;
            dmgBon = 10;
            openDoor = 18;
            openMagicDoor = 14;
            BB_LG = 80;
            break;
        case 23:
            hitBonus = 5;
            dmgBon = 11;
            openDoor = 18;
            openMagicDoor = 16;
            BB_LG = 90;
            break;
        case 24:
            hitBonus = 6;
            dmgBon = 12;
            openDoor = 19;
            openMagicDoor = 17;
            BB_LG = 95;
            break;
        case 25:
            hitBonus = 7;
            dmgBon = 16;
            openDoor = 19;
            openMagicDoor = 18;
            BB_LG = 99;
            break;
        default:
            hitBonus = 0;
            dmgBon = 0;
            openDoor = 0;
            openMagicDoor = 0;
            BB_LG = 0;
            Globals.WriteDebugString("Unhandled strength " + strength + " in determineStrengthProperties()\n");
            break;
    }
}

GameRules.prototype.DetermineNormalEncumbranceStrength = function(str, strMod) {
    var result = 0;
    var BaseAllowance = 350;
    // normal is based on str 8-11 able to carry 350gp
    switch (str) {
        case 1:
        case 2:
        case 3: // -350
            result = BaseAllowance - 350;
            break;
        case 4:
        case 5: // -250
            result = BaseAllowance - 250;
            break;
        case 6:
        case 7: // -150
            result = BaseAllowance - 150;
            break;
        case 8:
        case 9: // normal
        case 10:
        case 11:
            result = BaseAllowance;
            break;
        case 12:
        case 13:
            result = BaseAllowance + 100;
            break;
        case 14:
        case 15:
            result = BaseAllowance + 200;
            break;
        case 16:
            result = BaseAllowance + 350;
            break;
        case 17:
            result = BaseAllowance + 500;
            break;
        case 18:
            {
                var mod = strMod;
                if (mod == 0)
                    result = BaseAllowance + 750;
                else if (mod < 51)
                    result = BaseAllowance + 1000;
                else if (mod < 76)
                    result = BaseAllowance + 1250;
                else if (mod < 91)
                    result = BaseAllowance + 1500;
                else if (mod < 100)
                    result = BaseAllowance + 2000;
                else
                    result = BaseAllowance + 3000;
            }
            break;
        case 19:  // cases 19-25 by manikus 4-5-11
            result = BaseAllowance + 4500;
            break;
        case 20:
            result = BaseAllowance + 5000;
            break;
        case 21:
            result = BaseAllowance + 6000;
            break;
        case 22:
            result = BaseAllowance + 7500;
            break;
        case 23:
            result = BaseAllowance + 9000;
            break;
        case 24:
            result = BaseAllowance + 12000;
            break;
        case 25:
            result = BaseAllowance + 15000;
            break;
        default:
            result = BaseAllowance;
    }
    if (result <= 0) result = 1;
    return result;
}
