function ToHitComputation() {
    this.m_effectiveTargetAC = 0;
    this.m_effectiveTHAC0 = 0;
    this.m_didHit = -1;  //-1 Nothing known.  0 Miss  1 Hit
    this.m_beginSpellScriptFailure = 0;
    this.m_successful = 0; // HitDice and Spell Begin Script both succeeded.
    this.m_toHitDiceRoll = 0;
    this.m_attDiceBonus = 0;
    this.m_isBackStab = false;
    this.m_backstabMultiplier = 0;

}

ToHitComputation.prototype.DidHit = function () {
    return this.m_didHit > 0;
}

ToHitComputation.prototype.Rolled = function () {
    return this.m_toHitDiceRoll;
}

ToHitComputation.prototype.IsBackStab = function () {
    return this.m_isBackStab;
}

ToHitComputation.prototype.BackstabMultiplier = function () {
    return this.m_backstabMultiplier;
}

ToHitComputation.prototype.BeginSpellScriptFailure = function () {
    return this.m_beginSpellScriptFailure;
}

ToHitComputation.prototype.SetSuccessful = function (v) {
    this.m_successful = v;
}

ToHitComputation.prototype.GetSuccessful = function() {
    return this.m_successful;
};


/**TODO
int ToHitComputation:: ComputeEffectiveTHAC0(const COMBATANT * pAttacker,
                                            const COMBATANT * pTarget,
                                            //GLOBAL_ITEM_ID weaponID,
                                            const ITEM_ID& weaponID,
    int distance);


int AttackDiceBonus(void)const { return m_attDiceBonus;};
void BeginSpellScriptFailure(int v){ m_beginSpellScriptFailure = v; };
*/


ToHitComputation.prototype.Compute4 = function(pAttacker, targetIndex, pTarget, wpn)
{
    var attDiceSides = 0;
    var attDiceNum = 0;
    var distance = 0;
    var bsm = 0;
    var itemID = "";
    var backstabSkillID = "";


    distance = Drawtile.Distance6(pAttacker.self, pAttacker.x, pAttacker.y,
        pTarget.self, pTarget.x, pTarget.y);

    attDiceSides = 20;
    attDiceNum = 1;

    this.m_attDiceBonus = 0;

    itemID = "";
    if (wpn != NO_READY_ITEM) {
        itemID = pAttacker.m_pCharacter.myItems.GetItem(wpn);
    };

    this.m_attDiceBonus = pAttacker.ModifyAttackRollDice(           // PORT NOTE:  PBR parameter here
        pTarget.m_pCharacter,
        attDiceNum,
        attDiceSides,
        this.m_attDiceBonus).pBonus;

    this.m_attDiceBonus = pAttacker.ModifyAttackRollDiceForItem(
        pTarget.m_pCharacter,
        itemID,  // Investigate this!!
        attDiceNum,
        attDiceSides,
        this.m_attDiceBonus,
        distance).pBonus;

    this.m_attDiceBonus = pTarget.ModifyAttackRollDiceAsTarget(
        pAttacker.m_pCharacter,
        attDiceNum,
        attDiceSides,
        this.m_attDiceBonus).pBonus;

    this.m_attDiceBonus = pTarget.ModifyAttackRollDiceForItemAsTarget(
        pAttacker.m_pCharacter,
        itemID,  //Investigate this!!
        attDiceNum,
        attDiceSides,
        this.m_attDiceBonus).pBonus;

    this.m_toHitDiceRoll = Globals.RollDice(attDiceSides,
        attDiceNum,
        this.m_attDiceBonus);


    bsm = pAttacker.m_pCharacter.GetAdjSkillValue(Globals.Skill_BackstabMultiplier, false, true);
    if (bsm == Globals.NoSkill) {
        this.m_isBackStab = false;
    }
    else {
        this.m_isBackStab = pAttacker.DetermineIfBackStab(wpn, targetIndex);
    };

    this.m_attDiceBonus += pAttacker.GetAttackBonus(itemID, distance);
    if (this.m_toHitDiceRoll < 1) this.m_toHitDiceRoll = 1;
    if (this.m_toHitDiceRoll > 20) this.m_toHitDiceRoll = 20;

    this.m_effectiveTHAC0 = this.ComputeEffectiveTHAC0(pAttacker, pTarget, itemID, distance);

    if (pAttacker.friendly) {
        if (Globals.GetConfigPartyAlwaysMiss()) this.m_toHitDiceRoll = this.m_effectiveTHAC0 - 1;
        else if (Globals.GetConfigPartyAlwaysHit()) this.m_toHitDiceRoll = this.m_effectiveTHAC0;
    }
    else {
        if (GetConfigMonsterAlwaysMiss()) this.m_toHitDiceRoll = this.m_effectiveTHAC0 - 1;
        else if (GetConfigMonsterAlwaysHit()) this.m_toHitDiceRoll = this.m_effectiveTHAC0;
    };
    {
        var result = "", finalResult = "", itemName = "";
        var hookParameters = new HOOK_PARAMETERS();
        var scriptContext = new SCRIPT_CONTEXT();
        var pItem;
        scriptContext.SetItemContext(itemID == "" ? null : itemID);  // Includes spell Context!    // PORT NOTE:  Had to modify since itemID is just a string now
        scriptContext.SetItemContextKey(wpn);
        scriptContext.SetTargetContextCOMBATANT(pTarget);
        scriptContext.SetAttackerContext(pAttacker);

        if (itemData.IsValidItem(itemID)) {
            itemName = scriptContext.GetItemContext("Bogus Item Context").UniqueName();
        }
        else {
            itemName = "Bogus Item";
        };
        hookParameters[5] = "" + this.m_toHitDiceRoll;
        hookParameters[6] = "" + this.m_effectiveTHAC0;
        var resultObj = pAttacker.RunCombatantScripts
            (SPECAB.DOES_ATTACK_SUCCEED,
                SPECAB.ScriptCallback_RunAllScripts,
                "YN",
                "To-Hit versus THAC0");
        result = resultObj.scriptResult;
        if (!UAFUtil.IsEmpty(result)) finalResult = result;
        result = pAttacker.m_pCharacter.RunCharacterScripts
            (SPECAB.DOES_ATTACK_SUCCEED,
                SPECAB.ScriptCallback_RunAllScripts,
                "YN",
                "To-Hit versus THAC0");
        if (!UAFUtil.IsEmpty(result)) finalResult = result;
        result = pTarget.RunCombatantScripts
            (SPECAB.DOES_ATTACK_SUCCEED,
                SPECAB.ScriptCallback_RunAllScripts,
                "YN",
                "To-Hit versus THAC0");
        if (!UAFUtil.IsEmpty(result)) finalResult = result;
        result = pTarget.m_pCharacter.RunCharacterScripts
            (SPECAB.DOES_ATTACK_SUCCEED,
                SPECAB.ScriptCallback_RunAllScripts,
                "YN",
                "To-Hit versus THAC0");
        if (!UAFUtil.IsEmpty(result)) finalResult = result;
        if (itemData.IsValidItem(itemID)) {
            pItem = itemData.GetItem(itemID);
            if (pItem != null) {
                result = pItem.RunItemScripts
                    (SPECAB.DOES_ATTACK_SUCCEED,
                        SPECAB.ScriptCallback_RunAllScripts,
                        "YN",
                        "To-Hit versus THAC0");
                if (!UAFUtil.IsEmpty(result)) finalResult = result;
                if (scriptContext.pSpellContext != null) {
                    result = scriptContext.GetSpellContext(null).RunSpellScripts
                        (SPECAB.DOES_ATTACK_SUCCEED,
                            SPECAB.ScriptCallback_RunAllScripts,
                            "YN",
                            "To-Hit versus THAC0");
                    if (!UAFUtil.IsEmpty(result)) finalResult = result;
                };
            };
        };
        var pClass;
        pClass = classData.PeekClass(pAttacker.GetClass());
        if (pClass != null) {
            result = pClass.RunClassScripts
                (SPECAB.DOES_ATTACK_SUCCEED,
                    SPECAB.ScriptCallback_RunAllScripts,
                    "YN",
                    "To-Hit versus THAC0");
        }
        else {
            result = "";
        };
        if (!UAFUtil.IsEmpty(result)) finalResult = result;
        {
            var pRace;
            pRace = pAttacker.m_pCharacter.PeekRaceData();
            result = "";
            if (pRace != null) {
                result = pRace.RunRaceScripts
                    (SPECAB.DOES_ATTACK_SUCCEED,
                        SPECAB.ScriptCallback_RunAllScripts,
                        "YN",
                        "To-Hit versus THAC0");
                if (!UAFUtil.IsEmpty(result)) finalResult = result;
            };
        };
        {
            var pTargClass;
            pTargClass = classData.PeekClass(pTarget.GetClass());
            result = "";
            if (pTargClass != null) {
                result = pTargClass.RunClassScripts
                    (SPECAB.DOES_ATTACK_SUCCEED,
                        SPECAB.ScriptCallback_RunAllScripts,
                        "YN",
                        "To-Hit versus THAC0");
                if (!UAFUtil.IsEmpty(result)) finalResult = result;
            };
        }
        {
            var pRace;
            pRace = pTarget.m_pCharacter.PeekRaceData();
            result = "";
            if (pRace != null) {
                result = pRace.RunRaceScripts
                    (SPECAB.DOES_ATTACK_SUCCEED,
                        SPECAB.ScriptCallback_RunAllScripts,
                        "YN",
                        "To-Hit versus THAC0");
                if (!UAFUtil.IsEmpty(result)) finalResult = result;
            };
        };
        {
            if (pTarget.GetType() == MONSTER_TYPE) {
                var monsterID;
                monsterID = pTarget.m_pCharacter.monsterID;
                {
                    var pMonsterData;
                    pMonsterData = monsterData.PeekMonster(monsterID);
                    result = "";
                    if (pMonsterData != null) {
                        result = pMonsterData.RunMonsterScripts(
                            SPECAB.DOES_ATTACK_SUCCEED,
                            SPECAB.ScriptCallback_RunAllScripts,
                            null,
                            "To-Hit versus THAC0");
                        if (!UAFUtil.IsEmpty(result)) finalResult = result;
                    };
                };
            };

        };
        {
            if (finalResult.indexOf('N') >= 0) this.m_didHit = 0;
            else if (finalResult.indexOf('Y') >= 0) this.m_didHit = 1;
        };
    };

    if (this.m_didHit < 0) this.m_didHit = (this.m_toHitDiceRoll >= this.m_effectiveTHAC0);
}


ToHitComputation.prototype.ComputeEffectiveTHAC0 = function(pAttacker, pTarget, weaponID, distance) {
    /*
    ********************* Revised xxxxx Algorithm *******************
    ********************* (name to be determined)
    
    0)AttackerTHAC0 = THAC0 from character's stats (14)
    1)something001 = some function(weaponType-HandCutting, etc) (0)
    2)something002 = AttackerTHAC0 - something001 (14 - 0 = 14)
    3)WeaponToHitBonus = WeaponBonus from weapon description (5)
    4)something003 = something002 - WeaponToHitBonus (14 - 5 = 9)
    5)EffectiveTargetAC = some function (target and shields and etc) (9)
    6)EffectiveTHAC0 = something003 - EffectiveTargetAC (9 - 9 = 0)
    
    
    Re-writing gives:
    6) EffectiveTHAC0 = (something002 - WeaponToHitBonus) - EffectiveTargetAC
    6) EffectiveTHAC0 = something002 - WeaponToHitBonus - EffectiveTargetAC
    6) EffectiveTHAC0 = (AttackerTHAC0 - somethin001) - WeaponToHitBonus - EffectiveTargetAC
    6) EffectiveTHAC0 = AttackerTHAC0 - somethin001 - WeaponToHitBonus - EffectiveTargetAC
    6) EffectiveTHAC0 = AttackerTHAC0 - environmentalBonusTHAC0 - WeaponToHitBonus - EffectiveTargetAC
    
    
    Then use EffectiveTHAC0 as parameter[6]
    **************** End of xxxxx Algorithm **************** 
    
    Eric Cone (manikus) wrote to me:
    
    I am officially signing off on this. :)
    ComputerEffectiveTHAC0 seems like a good name to me.
    
    
    something001 -> environmentalBonusTHAC0
    something002 -> adjustedAttackerTHAC0
    something003 -> RealizedTHAC0
     
    
    */
    var effectiveTHAC0 = 0;
    var attackerTHAC0 = 0;
    var environmentalBonusTHAC0 = 0;
    var weaponToHitBonus = 0;
    var effectiveTargetAC = 0;

    var pWeapon = null;
    if (itemData.IsValidItem(weaponID)) {                  // PORT NOTE:  Changed IsValidItem a bit
        pWeapon = itemData.GetItem(weaponID);
    };

    //////////////////////// attackerTHAC0
    attackerTHAC0 = Globals.GetAttackerTHAC0(pAttacker);

    /////////////////////// weaponToHitBonus
    weaponToHitBonus = Globals.GetWeaponToHitBonus(pWeapon);

    /////////////////////// environmentalBonusTHAC0
    environmentalBonusTHAC0 = Globals.GetEnvironmentalBonusTHAC0(pAttacker, pWeapon, distance);

    ///////////////////////// effectiveTargtAC
    effectiveTargetAC = Globals.GetEffectiveTargetAC(pTarget, pAttacker, weaponID);

    ///////////////////////// effectiveTHAC0
    effectiveTHAC0 = attackerTHAC0 - environmentalBonusTHAC0 - weaponToHitBonus - effectiveTargetAC;

    return effectiveTHAC0;
}