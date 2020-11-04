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
    if (wpn != Items.NO_READY_ITEM) {
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
        this.m_attDiceBonus).pBonus;


    bsm = pAttacker.m_pCharacter.GetAdjSkillValue(Globals.Skill_BackstabMultiplier, false, true);
    if (bsm == Globals.NoSkill) {
        this.m_isBackStab = false;
    }
    else {
        this.m_isBackStab = pAttacker.DetermineIfBackStab(wpn, targetIndex);
    };


    this.m_attDiceBonus += pAttacker.GetAttackBonus(itemID, distance);
    if (m_toHitDiceRoll < 1) this.m_toHitDiceRoll = 1;
    if (m_toHitDiceRoll > 20) this.m_toHitDiceRoll = 20;

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
        scriptContext.SetItemContext(itemID);  // Includes spell Context!
        scriptContext.SetItemContextKey(wpn);
        scriptContext.SetTargetContext(pTarget);
        scriptContext.SetAttackerContext(pAttacker);
        if (itemID.IsValidItem()) {
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
        if (!(result == null || result == "")) finalResult = result;
        result = pAttacker.m_pCharacter.RunCharacterScripts
            (SPECAB.DOES_ATTACK_SUCCEED,
                SPECAB.ScriptCallback_RunAllScripts,
                "YN",
                "To-Hit versus THAC0");
        if (!(result == null || result == "")) finalResult = result;
        result = pTarget.RunCombatantScripts
            (SPECAB.DOES_ATTACK_SUCCEED,
                SPECAB.ScriptCallback_RunAllScripts,
                "YN",
                "To-Hit versus THAC0");
        if (!(result == null || result == "")) finalResult = result;
        result = pTarget.m_pCharacter.RunCharacterScripts
            (SPECAB.DOES_ATTACK_SUCCEED,
                SPECAB.ScriptCallback_RunAllScripts,
                "YN",
                "To-Hit versus THAC0");
        if (!(result == null || result == "")) finalResult = result;
        if (itemData.IsValidItem(itemID)) {
            pItem = itemData.GetItem(itemID);
            if (pItem != null) {
                result = pItem.RunItemScripts
                    (SPECAB.DOES_ATTACK_SUCCEED,
                        SPECAB.ScriptCallback_RunAllScripts,
                        "YN",
                        "To-Hit versus THAC0");
                if (!(result == null || result == "")) finalResult = result;
                if (scriptContext.pSpellContext != null) {
                    result = scriptContext.GetSpellContext(null).RunSpellScripts
                        (SPECAB.DOES_ATTACK_SUCCEED,
                            SPECAB.ScriptCallback_RunAllScripts,
                            "YN",
                            "To-Hit versus THAC0");
                    if (!(result == null || result == "")) finalResult = result;
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
        if (!(result == null || result == "")) finalResult = result;
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
                if (!(result == null || result == "")) finalResult = result;
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
                if (!(result == null || result == "")) finalResult = result;
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
                if (!(result == null || result == "")) finalResult = result;
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
                        if (!(result == null || result == "")) finalResult = result;
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