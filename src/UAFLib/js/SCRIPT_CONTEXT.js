function SCRIPT_CONTEXT() {
    this.m_prevScriptContext = null;
    this.m_prevScriptContext = SPECAB.pScriptContext;
    this.pScriptContext = this;
    this.scriptSourceType = SCRIPT_SOURCE_TYPE.getByNumber(0);   // eg: CHARACTER or SPELL or CLASS or EVENT
    this.sourceName = "";            // eg: if sourcetype implies a named Special Ability
    //        then SA name: "George" or "Heal"
    //
    this.scriptName = "";
    this.specAb = [];                     // The SPECIAL_ABILITY that triggered the script
    this.attackerContext = "";
    this.targetContext = "";
    this.characterContext = "";
    this.combatantContext = "";


    this.context = null;          // The context of RunScripts()
    this.pItemContext = null;
    this.pBaseclassContext = null;
    this.pClassContext = null;
    this.pSpellContext = null;
    this.pMonstertypeContext = null;
    this.pSpellgroupContext = null;
    this.pRaceContext = null;
    this.pAbilityContext = null;
    this.pToHitComputationContext = null;
    //#ifdef USE_TRAITS         // PORT NOTE: ???
    this.pTraitContext = null;
    //#endif
    this.pCreatedCharacterContext = null;
    this.scriptSourceType = SCRIPT_SOURCE_TYPE.ScriptSourceType_Unknown;
    this.eventX = -1;
    this.eventY = -1;
    this.eventID = -1;
    this.eventLevel = -1;
};

SCRIPT_CONTEXT.prototype.SetCombatantContext = function (pCombatant) {
    var actor;
    actor = pCombatant.GetContextActor();
    this.combatantContext = actor.ToString();
}

SCRIPT_CONTEXT.prototype.SetCharacterContext = function(pChar)
{
    var actor;
    actor = pChar.GetContextActor();
    this.characterContext = actor.ToString();
}