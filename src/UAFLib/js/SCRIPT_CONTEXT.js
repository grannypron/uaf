function SCRIPT_CONTEXT() {

    // The global variable uses this constructor as well as all the other script contexts.
    // So if this is the first/global construction, then initialize appropriately
    if (!pScriptContext) {
        this.m_prevScriptContext = null;
    } else {
        this.m_prevScriptContext = pScriptContext;
        // Now the global variable is the one that I just created
        pScriptContext = this;
    }

    this.scriptSourceType = 0;   // eg: CHARACTER or SPELL or CLASS or EVENT
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
    this.itemKey = 0;          // The key in the character's 'myItems' inventory
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
}

SCRIPT_CONTEXT.prototype.SetCombatantContext = function (pCombatant) {
    var actor;
    actor = pCombatant.GetContextActor();
    this.combatantContext = actor.ToString();
}

SCRIPT_CONTEXT.prototype.SetCharacterContext = function(pChar) {
    var actor;
    actor = pChar.GetContextActor();
    this.characterContext = actor.ToString();
}

SCRIPT_CONTEXT.prototype.SetClassContext = function (classID) {
    this.pClassContext = classData.PeekClass(classID);
}


SCRIPT_CONTEXT.prototype.SetTargetContextCombatant = function(pCombatant) {
    var actor;
    this.pCombatant.GetContextActor();
    this.targetContext = actor.ToString();
}

SCRIPT_CONTEXT.prototype.SetAbilitySpecAbCStrPair = function (pSpecAb, pAb) {
    this.contextASL = null;
    this.context = pSpecAb;
    if (pAb != null) specAb = pAb;
    else {
        specAb.Clear();
    };
}

SCRIPT_CONTEXT.prototype.SetSA_Source_Type = function(source_Type) {
    this.scriptSourceType = source_Type;
}

SCRIPT_CONTEXT.prototype.SetSA_Source_Name = function(source_Name) {
    this.sourceName = source_Name;
}

SCRIPT_CONTEXT.prototype.SetSA_ScriptName = function(name) {
    this.scriptName = name;
}

SCRIPT_CONTEXT.prototype.ClearAbility = function() {
    this.specAb = [];  //PORT NOTE:  was .Clear()
}


SCRIPT_CONTEXT.prototype.Clear = function() {
    this.pItemContext = null;
    this.attackerContext = "";
    this.characterContext = "";
    this.targetContext = "";
    this.combatantContext = "";
    this.pClassContext = null;
    this.pSpellContext = null;
    this.pSpellgroupContext = null;
    this.pMonstertypeContext = null;
    this.pRaceContext = null;
    this.pAbilityContext = null;
//#ifdef USE_TRAITS               //PORT NOTE:   ??
    this.pTraitContext = null;
//#endif
    this.context = null;

    this.specAb = [];  //PORT NOTE:  was .Clear();
}

SCRIPT_CONTEXT.prototype.SetMonsterTypeContext = function(pMonster)
{
    this.pMonstertypeContext = pMonster;
}


SCRIPT_CONTEXT.prototype.SetAttackerContext = function(pCombatant)
{
    var actor;
    actor = pCombatant.GetContextActor();
    this.attackerContext = actor.ToString();
}


SCRIPT_CONTEXT.prototype.SetTargetContextCombatant = function(pCombatant)
{
    var actor;
    actor = pCombatant.GetContextActor();
    targetContext = actor.ToString();
}

SCRIPT_CONTEXT.prototype.SetItemContext = function(pItem)
{
    this.pItemContext = pItem;
    if (pItem != null && !UAFUtil.IsEmpty(pItem)) {    //PORT NOTE:  Had to add IsEmpty check to prevent NPE because pItemID could be an empty string in my new scheme with IDs as strings
        this.SetSpellContext(pItem.SpellID());
    }
}

SCRIPT_CONTEXT.prototype.SetItemContextKey = function (key) {
    this.itemKey = key;
}

SCRIPT_CONTEXT.prototype.SetSpellContext = function (pSpell) {
    this.pSpellContext = pSpell;
}


SCRIPT_CONTEXT.prototype.SetTargetContextCOMBATANT = function(pCombatant) {
    var actor;
    actor = pCombatant.GetContextActor();
    this.targetContext = actor.ToString();
}

SCRIPT_CONTEXT.prototype.SetAttackerContext = function(pCombatant)
{
    var actor;
    actor = pCombatant.GetContextActor();
    this.attackerContext = actor.ToString();
}


SCRIPT_CONTEXT.prototype.GetCharacterContext = function(msg) {
    if (UAFUtil.IsEmpty(this.characterContext))
    {
        this.MsgBoxErrorAlert(msg);
    };
    return this.characterContext;
}

SCRIPT_CONTEXT.prototype.GetCombatantContext = function (msg) {
    if (UAFUtil.IsEmpty(this.combatantContext))
    {
        this.MsgBoxErrorAlert(msg);
    }
    return this.combatantContext;
}


// PORT NOTE: Scaled down
SCRIPT_CONTEXT.prototype.MsgBoxErrorAlert = function (msg) {
    Globals.debug(msg);
}

SCRIPT_CONTEXT.prototype.GetMonsterTypeContext = function(msg) {
    if (this.pMonstertypeContext == null) {
        this.MsgBoxErrorAlert(msg);
        return bogusMonsterType;
    }
    return this.pMonstertypeContext;
}

$GET_MONSTERTYPE_SA = function (monsterID, sa) {
    var pMonsterData;
    pMonsterData = monsterData.GetMonster(monsterID);
    if (pMonsterData == null) {
        return this.NO_SUCH_SA;
    } else {
        var csp;
        csp = pMonsterData.specAbs.FindAbility(sa);
        if (csp == null) 
            return this.NO_SUCH_SA;
        else
            return csp.Value();
    }
}

function $GET_HOOK_PARAM(idx) {
    return SPECAB.p_hook_parameters[idx];
}

function $NextCreatureIndex(filter, idx) {

    if (UAFUtil.IsEmpty(idx)) {
        idx = 0;
    } else {
        idx++;
    }

    if (Globals.IsCombatActive()) {
        for (; idx < combatData.NumCombatants(); idx++) {
            var pCOMBATANT;
            pCOMBATANT = combatData.GetCombatant(idx);
            if (filter & 1) {
                if (!pCOMBATANT.IsAlive()) continue;
            };
            if (filter & 2) {
                if (pCOMBATANT.friendly) continue;
            };
            if (filter & 4) {
                if (!pCOMBATANT.friendly) continue;
            };
            if (filter & 8) {
                if (!pCOMBATANT.charOnCombatMap(false, true)) continue;
            };
            break;
        }
        if (idx >= combatData.NumCombatants()) {
            return "";
        }
        else {
            idx;
        }
    }

}



function $SET_HOOK_PARAM(idx, val) {
    SPECAB.p_hook_parameters[idx] = val;
}
