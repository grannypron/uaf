function DamageComputation() {
    this.m_damage = "";
    this.m_spellID = "";
    this.m_isNonLethal = false;
    this.m_message = "";

    /**TODO
    public:
    DamageComputation(void){ };
    int Damage(void){ return m_damage; };
    BOOL IsNonLethal(void){ return m_isNonLethal; };
    SPELL_ID SpellID(void) { return m_spellID; };
    CString Message(void){ return m_message; };
    */
}


DamageComputation.prototype.Compute = function(pAttacker, pTarget, wpn, toHitRoll, isBackStab, backstabMultiplier) {
    var itemID = "";
    var distance = 0;

    var dmgDiceQty = 0, dmgDiceSides = 0, dmgDiceBonus = 0;

    distance = Drawtile.Distance6(pAttacker.self, pAttacker.x, pAttacker.y,
        pTarget.self, pTarget.x, pTarget.y);

    itemID = null;
    if (wpn != NO_READY_ITEM) {
        itemID = pAttacker.m_pCharacter.myItems.GetItem(wpn);
    }

    this.m_damage = 0;
    var result = pAttacker.GetDamageDice(wpn, dmgDiceQty, dmgDiceSides, dmgDiceBonus, this.m_isNonLethal, pTarget.isLargeDude(), this.m_spellID);
    // PORT NOTE:  Handling pass-by-reference params
    dmgDiceQty = result.pNum;
    dmgDiceSides = result.pSides;
    dmgDiceBonus = result.pBonus;
    this.m_isNonLethal = result.pNonLethal;
    this.m_spellID = result.pSpellName;

    result = pAttacker.ModifyAttackDamageDice(
                    pTarget.m_pCharacter,   
                    dmgDiceQty,
                    dmgDiceSides, 
                    dmgDiceBonus, 
                    this.m_isNonLethal);
    dmgDiceBonus = result.pBonus;        // PORT NOTE:  Handling pass-by-reference params

    result = pAttacker.ModifyAttackDamageDiceForItem(
                    pTarget.m_pCharacter,
                    itemID,
                    dmgDiceQty,
                    dmgDiceSides,  
                    dmgDiceBonus,  
                    this.m_isNonLethal,
                    distance);
    dmgDiceBonus = result.pBonus;       // PORT NOTE:  Handling pass-by-reference params

    pTarget.ModifyAttackDamageDiceAsTarget(         // PORT NOTE:  This method only returns false so has no effect
                    pAttacker.m_pCharacter,
                    dmgDiceQty,
                    dmgDiceSides,
                    dmgDiceBonus,
                    this.m_isNonLethal);

    result = pTarget.ModifyAttackDamageDiceForItemAsTarget(
                    pAttacker.m_pCharacter,
                    itemID,
                    dmgDiceQty,
                    dmgDiceSides,
                    dmgDiceBonus,
                    this.m_isNonLethal,
                    toHitRoll);
    dmgDiceBonus = result.pBonus; // PORT NOTE:  Handling pass-by-reference params

    this.m_damage = Globals.RollDice(dmgDiceSides, dmgDiceQty, dmgDiceBonus);

    if (this.m_damage <= 0)
        this.m_damage = 1;

    Globals.TRACE(pAttacker.self + " rolls " + this.m_damage + " damage using " + dmgDiceSides + " sides, " + dmgDiceQty + " qty, " + dmgDiceBonus + " bonus (" + (this.m_isNonLethal ? "NonLethal" : "Lethal") + ")\n");

    if (this.m_damage > 0) {
        if (isBackStab) {
            this.m_damage = this.m_damage * backstabMultiplier / 100;
        };
        {
            var result = "";
            var hookParameters = new HOOK_PARAMETERS();
            var scriptContext = new SCRIPT_CONTEXT();
            var pWeapon = null;
            scriptContext.SetItemContext(itemID);
            if (!itemData.IsNoItem(itemID)) {
                pWeapon = itemData.GetItem(itemID);
            };
            hookParameters[2] = "" + this.m_damage;
            if (pWeapon != null) {
                if (pWeapon.Wpn_Type == weaponClassType.Ammo) {
                    // We need to find the launching weapon.
                    var launchWpnIndex = 0;
                    launchWpnIndex = pAttacker.m_pCharacter.myItems.GetReadiedItem(Items.WeaponHand, 0);
                    if (launchWpnIndex != NO_READY_ITEM) {
                        var launchID = "";
                        var pLauncher;
                        launchID = pAttacker.m_pCharacter.myItems.GetItem(launchWpnIndex);
                        pLauncher = itemData.GetItem(launchID);
                        hookParameters[3] = "" + pLauncher.Attack_Bonus;
                        this.m_damage += pLauncher.Attack_Bonus;
                    };
                }
                hookParameters[4] = "" + pWeapon.Attack_Bonus;
                m_damage += pWeapon.Attack_Bonus;
            };
            hookParameters[5] = "" + this.m_damage;
            hookParameters[6] = "" + this.m_isNonLethal;
            scriptContext.SetAttackerContext(pAttacker);
            scriptContext.SetTargetContextCOMBATANT(pTarget);
            if (pWeapon != null) {
                scriptContext.SetItemContext(pWeapon);
            }
            else {
                scriptContext.SetItemContext(Globals.bogusItem);
            };
            scriptContext.SetItemContextKey(wpn);
            if (pWeapon != null) {
                result = pWeapon.RunItemScripts(
                    SPECAB.COMPUTE_DAMAGE,
                    SPECAB.ScriptCallback_RunAllScripts,
                    null,
                    "Compute damage of attack");
            };
            if (pTarget.GetType() == MONSTER_TYPE) {
                var monsterID = "";
                monsterID = pTarget.m_pCharacter.monsterID;
                {
                    var pMonsterData;
                    pMonsterData = monsterData.PeekMonster(monsterID);
                    if (pMonsterData != null) {
                        result = pMonsterData.RunMonsterScripts(
                            SPECAB.COMPUTE_DAMAGE,
                            SPECAB.ScriptCallback_RunAllScripts,
                            null,
                            "Compute damage of attack");
                    }
                }
            }
            result = pTarget.RunCombatantScripts(
                SPECAB.COMPUTE_DAMAGE,
                SPECAB.ScriptCallback_RunAllScripts,
                null,
                "Compute damage of attack");
            this.m_message = hookParameters[7];
            {
                var temp = 0;
                if (!isNaN(parseInt(hookParameters[5]))) this.m_damage = parseInt(hookParameters[5]);  //  PORT NOTE:  This is a little different in that that sscanf would have returned ANY digit in the result, but it's probably meant to just quickly parse the int
            }
        }
    }

}

DamageComputation.prototype.Damage = function () {
    return this.m_damage;
}

DamageComputation.prototype.IsNonLethal = function () {
    return this.m_isNonLethal;
}

DamageComputation.prototype.SpellID = function () {
    return this.m_spellID;
}

DamageComputation.prototype.Message = function () {
    return this.m_message;
}