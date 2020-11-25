function Globals() {
    this.CHARACTER_VERSION = UAFUtil.ByteFromHexString("0x80000001");
    this.logDebuggingInfo = false;
    this.version = 6.0;
    this.debugSeverity = 0;

    this.PlaySoundEffects = true;

    this.miscError = 0;
    this.miscErrorText = [];

    this.MIN_INTELLIGENCE = 3;
    this.MAX_INTELLIGENCE = 25;
    this.MIN_STRENGTH = 3;
    this.MAX_STRENGTH = 25;
    this.MIN_STRENGTHMOD = 0;
    this.MAX_STRENGTHMOD = 100;
    this.MIN_WISDOM = 3;
    this.MAX_WISDOM = 25;
    this.MIN_DEXTERITY = 3;
    this.MAX_DEXTERITY = 25;
    this.MIN_CONSTITUTION = 3;
    this.MAX_CONSTITUTION = 25;
    this.MIN_CHARISMA = 3;
    this.MAX_CHARISMA = 25;

    this.CombatScreenX = 14;
    this.CombatScreenY = 14;
    this.TILES_HORZ = 8;
    this.TILES_VERT = 9;

    this.SCREEN_WIDTH = 640;  // these change to reflect the current
    this.SCREEN_HEIGHT = 480; // screen resolution for the editor
    this.COLOR_BITS = 24;

    this.START_EXP_VALUE = 0;
    this.START_LVL_VALUE = 1;

    this.MAX_COMBATANTS = 100;
    this.MAX_CHARS_IN_ROW = 3; // how many chars wide the battle line can be
    this.MAX_COMBAT_IDLE_ROUNDS = 20;

    this.Skill_PickPockets = "PickPockets";
    this.Skill_OpenLocks = "OpenLocks";
    this.Skill_FindTraps = "FindTraps";
    this.Skill_MoveSilent = "MoveSilent";
    this.Skill_HideInShadows = "HideInShadows";
    this.Skill_HearNoise = "HearNoise";
    this.Skill_ClimbWalls = "ClimbWalls";
    this.Skill_ReadLanguages = "ReadLanguages";
    this.Skill_Turn = "Turn";
    this.Skill_BackstabMultiplier = "BackstabMultiplier";
    this.Skill_RangerBonusLevel = "RangerBonusLevel";

    this.Ability_Dexterity = "Dexterity";
    this.Ability_Constitution = "Constitution";
    this.Ability_Strength = "Strength";
    this.Ability_Intelligence = "Intelligence";
    this.Ability_Charisma = "Charisma";
    this.Ability_Wisdom = "Wisdom";

    this.Baseclass_magicUser = "magicUser";
    this.Baseclass_thief = "thief";
    this.Baseclass_paladin = "paladin";
    this.Baseclass_cleric = "cleric";
    this.Baseclass_fighter = "fighter";
    this.Baseclass_ranger = "ranger";
    this.Baseclass_druid = "druid";

    this.Skill_BackstabMultiplier = "BackstabMultiplier";

    this.NoSkill = UAFUtil.ByteFromHexString("0x80000000");
    this.NoSkillAdj = -987656789.0;

    this.FakeCharacter = new CHARACTER();
    this.bogusItem = new ITEM_DATA();
}

Globals.prototype.die = function (message) {
    throw message;
}

Globals.prototype.debug = function (msg) {
    var log = System.Console.WriteLine; log(msg);
}

Globals.prototype.TRACE = function (msg) {
    this.debug(msg);
}

Globals.prototype.WriteDebugString = function (msg) {
    this.debug(msg);
}

Globals.prototype.MsgBoxError = function (msg) {
    /** TODO **/
}

Globals.prototype.debugStop = function (msg) {
    var log = Globals.debug(msg); Globals.die("debugStop");
}

Globals.prototype.IsCombatActive = function () {
    return combatData.m_bCombatActive;
}

Globals.prototype.GetMaxPartyMembers = function()
{
    return (Math.min(MAX_PARTY_MEMBERS, globalData.GetMaxPartySize()));
}

Globals.prototype.StripFilenamePath = function (filename) {
    if (UAFUtil.IsEmpty(filename))
        return filename;

    // if filaname ends with '\', strip it
    if (filename.charAt(filename.length - 1) == "\\") {
        filename = filename.substring(0, filename.length - 1);
    }
    if (filename.indexOf('\\') < 0) return filename;
    return filename.substr(filename.lastIndexOf("\\") + 1);
}

Globals.prototype.ASSERT = function (val, msg) {
    if (!val) {
        throw "Assertion failed.\n" + msg;
    }
}

Globals.prototype.GetGameVersion = function(data) {
    return data.version;
}

Globals.prototype.RollDice = function(sides, numTimes, bonus) {
    if (sides <= 0) return bonus;//0.0;
    if (numTimes <= 0) return bonus;//0.0;
    var result = 0.0;
    for (var i = 0; i < numTimes; i++)
    result += ((this.randomMT() % sides) + 1);
    result += bonus;
    return result;
}

Globals.prototype.randomMT = function () {
    // Passing this off to an RNG that can be swapped in
    return RNG.newDWORD();
}


Globals.prototype.GetMonsterQtyMod = function()
{
    var lvl = Globals.GetCurrSkillLevel();
    if ((lvl < 0) || (lvl > 4)) return 0;
    if (globalData.m_diffLvlData.m_data[lvl].m_ModifyQty)
        return globalData.m_diffLvlData.m_data[lvl].m_QtyAmount;
    else
        return 0;
}

Globals.prototype.timeGetTime = function() {
    return (new Date()).getTime();
}

Globals.prototype.GetCurrSkillLevel = function()
{
    return party.skillLevel;
}

Globals.prototype.GetConfigMonsterPlyrControl = function () {
    return combatData.m_bMonsterPlyrControl;
}

Globals.prototype.SetMiscError = function (error) {
    if ((error >= 0) && (error < MAX_MISC_ERROR_MSGS)) {
        miscError = error;
        if (error != miscErrorType.NoError) {
            if (!debugStrings.AlreadyNoted("SME01" + miscErrorText[error])) {
                this.WriteDebugString("MISC ERROR: " + miscErrorText[error] + "\n");
            };
        };
    }
}

Globals.prototype.GetConfigMonsterNoMove = function () {
    return combatData.m_bMonsterNoMove;
}

Globals.prototype.PlayCombatMove = function () {
    combatData.PlayCombatMove();
}

Globals.prototype.GetQueuedCombatants = function () {
    return combatData.QComb;
}

Globals.prototype.PlaceCursorOnCurrentDude = function () {
    combatData.PlaceCursorOnCurrentDude();
}

Globals.prototype.GetCombatantPtr = function (index) {
    return (combatData.getCombatantPtr(index));;
}

Globals.prototype.GetCurrentRound = function () {
    return combatData.m_iCurrRound;
}

Globals.prototype.GetConfigPartyAlwaysMiss = function () {
    return combatData.m_bPartyAlwaysMiss;
}

Globals.prototype.GetConfigPartyAlwaysHit = function () {
    return combatData.m_bPartyAlwaysHit;
}

Globals.prototype.GetConfigMonsterAlwaysMiss = function () {
    return combatData.m_bMonsterAlwaysMiss;
}

Globals.prototype.GetConfigMonsterAlwaysHit = function () {
    return combatData.m_bMonsterAlwaysHit;
}

Globals.prototype.GetAdjSkillValue = function(SC)
{
    // Determined the base value for the skill
    SC = this.GetSkillValue(SC);
    // Now we compute the adjustments.
    SC = this.UpdateSkillValue(SC);
    SC.abilityAdj = abilityData.TotalAdjustments(Globals.NoSkillAdj);
    if (SC.includeTempAdj) {
        SC = SC.pChar.ApplyTempSkillAdjustments(SC);
    };
    if ((SC.baseVal == Globals.NoSkill)
        && (SC.bestRaceAdj == Globals.NoSkillAdj)
        && (SC.bestBaseclassAdj == Globals.NoSkillAdj)
        && (SC.abilityAdj == Globals.NoSkillAdj)
        && (SC.tempAdj == Globals.NoSkillAdj)) return SC;
    SC.finalAdjustedValue = 0.0;
    if (SC.baseVal != Globals.NoSkill) SC.finalAdjustedValue = SC.baseVal;
    if (SC.bestRaceAdj != Globals.NoSkillAdj) SC.finalAdjustedValue += SC.bestRaceAdj;
    if (SC.bestBaseclassAdj != Globals.NoSkillAdj) SC.finalAdjustedValue += SC.bestBaseclassAdj;
    if (SC.abilityAdj != Globals.NoSkillAdj) SC.finalAdjustedValue += SC.abilityAdj;
    if (SC.includeTempAdj) {
        if (SC.tempAdj != Globals.NoSkillAdj) SC.finalAdjustedValue += SC.tempAdj;
    };
    return SC;
}


Globals.prototype.GetSkillValue = function(SC)
{
    // First we will find the largest value of the skill
    // in the baseclass and race databases.
    var baseclassID = "";
    var pBaseclass;
    if (SC.ppBestBaseclass != null) SC.ppBestBaseclass = null;
    // Find the largest skill value among the baseclasses
    {
        var j = 0, m = 0;
        if (SC.pClass != null) {
            m = SC.pClass.GetCount();
            for (j = 0; j < m; j++) {
                baseclassID = SC.pClass.PeekBaseclassID(j);
                if (UAFUtil.IsEmpty(SC.baseclassID) || (SC.baseclassID == baseclassID)) {
                    pBaseclass = baseclassData.PeekBaseclass(baseclassID);
                    if (pBaseclass == null) continue;
                    SC = pBaseclass.GetSkillValue(SC);
                }
            }
        }
    }
    // Find the largest skill value in the race.
    if (SC.pRace != null) {
        SC = SC.pRace.GetSkillValue(SC);
    };
    // Now find the best starting value (baseclass or race)  
    SC.baseVal = SC.baseclassValue;
    if (SC.raceValue != Globals.NoSkill) {
        if (SC.baseVal == Globals.NoSkill) SC.baseVal = SC.raceValue;
        else {
            if (SC.minimize) {
                if (SC.raceValue < SC.baseVal) SC.baseVal = SC.raceValue;
            }
            else {
                if (SC.raceValue > SC.baseVal) SC.baseVal = SC.raceValue;
            }
        }
    }
    return SC;
}



Globals.prototype.UpdateSkillValue = function(SC) {
    abilityData.ClearAdjustments(Globals.NoSkillAdj);
    if (SC.pRace != null) {
        SC.pRace.UpdateSkillValue(SC);
    }
    {
        var i = 0, n = 0;
        n = SC.pClass.GetCount();
        for (i = 0; i < n; i++) {
            var baseclassID = "";
            var pBaseclass;
            SC.pClass.PeekBaseclassID(i);
            if (SC.baseclassID.IsEmpty() || (SC.baseclassID == baseclassID)) {
                pBaseclass = baseclassData.PeekBaseclass(baseclassID);
                if (pBaseclass == null) continue;
                if (UAFUtil.IsEmpty(SC.baseclassID) || (SC.baseclassID == pBaseclass.BaseclassID())) {
                    SC = pBaseclass.UpdateSkillValue(SC)
                }
            }
        }
    }
    return SC;
}


Globals.prototype.GetAttackerTHAC0 = function(pAttacker) {
    return pAttacker.m_pCharacter.GetTHAC0();
}

Globals.prototype.GetWeaponToHitBonus = function(pWeapon) {
    if (pWeapon != null) {
        return pWeapon.Attack_Bonus;
    }
    else {
        return 0;
    }
}


Globals.prototype.GetEnvironmentalBonusTHAC0 = function(pAttacker, pWeapon, distance) {
    var environmentalBonusTHAC0 = 0;
    if (pWeapon != null) {
        var weaponType;
        weaponType = pWeapon.Wpn_Type;
        switch (weaponType) {
            case weaponClassType.NotWeapon:
            case weaponClassType.HandBlunt:
            case weaponClassType.HandCutting:
                environmentalBonusTHAC0 = pAttacker.m_pCharacter.GetHitBonus();
                break;
            case weaponClassType.HandThrow:
                environmentalBonusTHAC0 = (distance == 0) ? pAttacker.m_pCharacter.GetHitBonus() : 0;
                break;
            case weaponClassType.SlingNoAmmo:
            case weaponClassType.Bow:
            case weaponClassType.Crossbow:
            case weaponClassType.Throw:
                environmentalBonusTHAC0 = 0;
                break;
            case weaponClassType.Ammo:
                environmentalBonusTHAC0 = 0;
                break;
            case weaponClassType.SpellCaster:
            case weaponClassType.SpellLikeAbility:
                environmentalBonusTHAC0 = 0;
                break;
            default:
                environmentalBonusTHAC0 = 0;
                break;
        };
    }
    else {
        environmentalBonusTHAC0 = 0;
    }
    return environmentalBonusTHAC0;
}

Globals.prototype.GetEffectiveTargetAC = function(pTarget, pAttacker, itemID) {
    var effectiveTargetAC = 0;
    effectiveTargetAC = pTarget.GetAdjAC();
    pTarget.ModifyACAsTarget(pAttacker.m_pCharacter, effectiveTargetAC, itemID);
    effectiveTargetAC += pTarget.m_pCharacter.myItems.GetProtectModForRdyItems();
    return effectiveTargetAC;
}


Globals.prototype.PlayCharMiss = function (play) {
    if (play == null || play == undefined) { play = true; }       // Default parameter
    if (play && SoundMgr.PlaySoundEffects) {
        if (!SoundMgr.ValidSound(globalData.sounds.hCharMiss))
            globalData.sounds.hCharMiss = SoundMgr.LoadSound(globalData.sounds.CharMiss);
        if (!SoundMgr.PlaySound(globalData.sounds.hCharMiss))
            globalData.sounds.hCharMiss = -1;
    }
    else
        SoundMgr.StopSound(globalData.sounds.hCharMiss);

}


Globals.prototype.PlayCharHit = function(play) {
    if (play == null || play == undefined) { play = true; }     // Default parameter
    if ((play) && SoundMgr.PlaySoundEffects) {
        if (!SoundMgr.ValidSound(globalData.sounds.hCharHit))
            globalData.sounds.hCharHit = SoundMgr.LoadSound(globalData.sounds.CharHit);
        if (!SoundMgr.PlaySound(globalData.sounds.hCharHit))
            globalData.sounds.hCharHit = -1;
    }
    else
        SoundMgr.StopSound(globalData.sounds.hCharHit);
}

Globals.prototype.GetConfigMonsterNoDeath = function () {
    return combatData.m_bMonsterNoDeath;
}

Globals.prototype.IncNumMonsterSlain = function () {
    combatData.m_iNumMonSlain++;
}

Globals.prototype.My_free = function (addr) {
    delete addr;
}

Globals.prototype.GetNumCombatants2 = function(pNumPty, pNumMonsters)
{
    return combatData.getNumCombatants(pNumPty, pNumMonsters);
}

Globals.prototype.GetNumCombatants = function () {
    return combatData.NumCombatants();
}

Globals.prototype.GetNumFriendFlee = function () {
    return combatData.m_iNumFrndFlee;
}

Globals.prototype.GetNumFriendSlain = function () {
    return combatData.m_iNumFrndSlain;
}

Globals.prototype.GetNumMonsterFlee = function () {
    return combatData.m_iNumMonFlee;
}

Globals.prototype.GetNumMonsterSlain = function () {
    return combatData.m_iNumMonSlain;
}

Globals.prototype.EnableUserInput = function (val) {
    Globals.debug("EnableUserInput: " + val);
}

Globals.prototype.GetAllExpMod = function() {
    var lvl = Globals.GetCurrSkillLevel();
    if ((lvl < 0) || (lvl > 4)) return 0;
    if (globalData.m_diffLvlData.m_data[lvl].m_ModifyAllExp)
        return globalData.m_diffLvlData.m_data[lvl].m_AExpAmount;
    else
        return 0;
}