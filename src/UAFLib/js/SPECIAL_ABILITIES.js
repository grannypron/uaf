/** TODO **/
function SPECIAL_ABILITIES() {
    this.m_specialAbilities = new A_CStringPAIR_L();
    this.m_readOnly = false;
}


SPECIAL_ABILITIES.prototype.Clear = function () {
    this.m_specialAbilities.Clear();
}


SPECIAL_ABILITIES.prototype.SerializeCAR = function(ar, version, objName, objType) {
    if ((version <= 0.920) && !ar.IsStoring()) {
        if (ar.IsStoring()) {
      /* Really */ Globals.NotImplemented(0x8da3f, false);
        }
        else {
      /* Really */ Globals.NotImplemented(0x54fa23, false);
        };
    }
    else {
        this.m_specialAbilities.SerializeCAR(ar);
    };
}


SPECIAL_ABILITIES.prototype.InsertAbility_RO = function(name, param, d1, d2) {
    this.m_specialAbilities.InsertKeyValue(name, param);
    /**TODO 
    if (globalSA_debug.Find(name) != null) {
        WriteDebugString("@@SA InsertAbility \"%s\" %s%s\n", name, d1, d2);
    }; **/
}


SPECIAL_ABILITIES.prototype.InsertAbility = function(name, param, d1, d2) {
    if (this.m_readOnly) {
        //if (!debugStrings.AlreadyNoted(CString("ATISAIROS")))   **TODO**
        //    writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
        Globals.WriteDebugString("Attempt to Insert SA in read-only structure\n");
        return;
    };
    this.InsertAbility_RO(name, param, d1, d2);
}

function SPECAB() {

    this.MAX_SPEC_AB = 100;

    this.INVOKE_SPELL_ON_TARGET =       "InvokeSpellOnTarget";
    this.GET_ADJ_MAX_MOVEMENT =         "GetAdjMaxMovement";
    this.CAN_CAST_SPELLS =              "CanCastSpells";
    this.CAN_MEMORIZE_SPELLS =          "CanMemorizeSpells";
    this.IS_COMBAT_READY =              "IsCombatReady";
    this.CHAR_DISPLAY_STATUS =          "CharDisplayStatus";
    this.GET_NUMBER_OF_ATTACKS =        "GetNumberOfAttacks";
    this.GET_ITEM_TARGET_HIT_BONUS =    "GetItemTargetHitBonus";
    this.SPECIAL_GRAPHICS =             "SpecialGraphics";

//    this.ON_READY_WEAPON             "OnReadyWeapon"
//    this.ON_UNREADY_WEAPON           "OnUnReadyWeapon"
//    this.ON_READY_SHIELD             "OnReadyShield"
//    this.ON_UNREADY_SHIELD           "OnUnReadyShield"
//    this.ON_READY_ARMOR              "OnReadyArmor"
//    this.ON_UNREADY_ARMOR            "OnUnReadyArmor"
//    this.ON_READY_GAUNTLETS          "OnReadyGauntlets"
//    this.ON_UNREADY_GAUNTLETS        "OnUnReadyGauntlets"
//    this.ON_READY_HELM               "OnReadyHelm"
//    this.ON_UNREADY_HELM             "OnUnReadyHelm"
//    this.ON_READY_BELT               "OnReadyBelt"
//    this.ON_UNREADY_BELT             "OnUnReadyBelt"
//    this.ON_READY_ROBE               "OnReadyRobe"
//    this.ON_UNREADY_ROBE             "OnUnReadyRobe"
//    this.ON_READY_CLOAK              "OnReadyCloak"
//    this.ON_UNREADY_CLOAK            "OnUnReadyCloak"
//    this.ON_READY_BOOTS              "OnReadyBoots"
//    this.ON_UNREADY_BOOTS            "OnUnReadyBoots"
//    this.ON_READY_RING               "OnReadyRing"
//    this.ON_UNREADY_RING             "OnUnReadyRing"
//    this.ON_READY_AMMO               "OnReadyAmmo"
//    this.ON_UNREADY_AMMO             "OnUnReadyAmmo"
    this.CAN_READY =                    "CanReady";
    this.ON_READY =                     "OnReady";
    this.ON_UNREADY =                   "OnUnReady";

    
    this.ON_USE_LAST_WEAPON             = "OnUseLastWeapon";
    this.PRE_START_COMBAT               = "PreStartCombat";
    this.ON_START_COMBAT                = "OnStartCombat";
    this.ON_END_COMBAT                  = "OnEndCombat";
//#ifdef SpellDetectMagic                                           // PORT NOTE: Don't know which to choose
    this.CAN_SPELL_DETECT_MAGIC         = "CanSpellDetectMagic";
//#else
//    this.CAN_CHARACTER_DETECT_MAGIC     = "CanCharacterDetectMagic";
//#endif
    this.DOES_ATTACK_SUCCEED            = "DoesAttackSucceed";
    this.DOES_SPELL_ATTACK_SUCCEED      = "DoesSpellAttackSucceed";
    this.COMPUTE_DAMAGE                 = "ComputeDamage";
    this.COMPUTE_SPELL_DAMAGE           = "ComputeSpellDamage";
    this.RANGER_DAMAGE_BONUS            = "RangerDamageBonus";
    this.ON_READYCOMBATEQUIPMENT        = "OnReadyCombatEquipment";
    this.START_COMBAT_ROUND             = "StartCombatRound";
    this.IS_VALID_TARGET                = "IsValidTarget";
    this.ON_START_COMBATANT_ACTION      = "OnStartCombatantAction";
    this.ON_STEP                        = "OnStep";
    this.DRAW_MOVE_MENU                 = "DrawMoveMenu";
    this.AUTO_ACTION                    = "AutoAction";
    this.CAN_KNOW_SPELL                 = "CanKnowSpell";
    this.DISPLAY_CHARACTER_CLASS        = "DisplayCharacterClass";
    this.DISPLAY_CHARACTER_BASECLASS    = "DisplayCharacterBaseclass";
    this.DISPLAY_CHARACTER_RACE         = "DisplayCharacterRace";
    this.SELECT_CHARACTER_RACE          = "SelectCharacterRace";
    this.SELECT_CHARACTER_CLASS         = "SelectCharacterClass";
    this.IS_BASECLASS_ALLOWED           = "IsBaseclassAllowed";
    this.TRAINING_CHARACTER             = "TrainingCharacter";
    this.CHANGE_CLASS_FROM              = "ChangeClassFrom";
    this.CHANGE_CLASS_TO                = "ChangeClassTo";
    this.TRANSFER_MODULE                = "TransferModule";
    this.COMBAT_MAIN_MENU               = "CombatMainMenu";
    this.COMBAT_SPECIAL_ACTION          = "CombatSpecialAction";
    this.REDIRECT_SPELL                 = "RedirectSpell";
    this.CAN_CURE_OR_WHATEVER           = "CanCureOrWhatever";
    this.CURE_OR_WHATEVER               = "CureOrWhatever";
    this.CREATE_MONSTER_COMBATANT       = "CreateMonsterCombatant";
    this.FIX_CHARACTER                  = "FixCharacter";
    this.SPELL_LINGER_BLOCKAGE          = "SpellLingerBlockage";
    this.JOIN_PARTY                     = "JoinParty";
    this.CAN_AIM                        = "CanAim";
    this.SPELL_DURATION                 = "SpellDuration";
    this.SPELL_RANGE                    = "SpellRange";
    this.SPELL_NUMTARGETS               = "SpellNumTargets";
    this.SPELL_CASTER_LEVEL             = "SpellCasterLevel";

    this.BEGIN_RESTING                  = "BeginResting";
    this.GUARDING_BEGIN_COMBAT          = "Guarding-BeginCombat";
    this.GUARDING_CAN_GUARD             = "Guarding-CanGuard";
    this.GUARDING_GUARD                 = "Guarding-Guard";
    this.GUARDING_START_OF_ROUND        = "Guarding-StartOfRound";
    this.GUARDING_CAN_GUARD_ATTACK      = "Guarding-CanGuardAttack";
    this.FREEATTACK_CAN_FREE_ATTACK     = "FreeAttack-CanFreeAttack";
    this.FREEATTACK_ATTACKOVER          = "FreeAttack-AttackOver";
    this.GUARDING_ATTACKOVER            = "Guarding-AttackOver";
    this.SPELL_LINGERING_SCRIPTS        = "SpellLingeringScripts";
    this.ADJUST_INITIATIVE              = "AdjustInitiative";

    this.CHARACTER_CREATED_SA           = "Global_CreateCharacter";
    this.POST_CREATE_CHARACTER          = "PostCreateCharacter";
    this.POST_CLASS                     = "PostClass";
    this.KNOWABLE_SPELLS                = "KnowableSpells";
    this.KNOWABLE_SPELLS_MINMAX         = "KnowableSpellsMinMax";
    this.LEARN_SPELLS                   = "LearnSpells";
    this.LEARN_SPELLS_NUM               = "LearnSpellsNum";
    this.IDENTIFY                       = "Identify";
    this.CAN_IDENTIFY                   = "CanIdentify";
    this.OVERRIDE_SPELL_SELECTION       = "OverrideSpellSelection";
    this.OVERRIDE_SPELL_TARGETING       = "OverrideSpellTargeting";
    this.PROCESS_SPELL_TARGET_OVERRIDE  = "ProcessSpellTargetOverride";
    this.CHARACTER_VIEW_SCREEN          = "CharacterViewScreen";
    this.EVENT_MENU_TEXT                = "EventMenuText";
    this.EVENT_TEXT1                    = "EventText1";
    this.EVENT_TAVERN_DRINK             = "EventTavernDrink";

    this.CAN_LAY_OR_WHATEVER            = "CanLayOrWhatever";
    this.LAY_OR_WHATEVER                = "LayOrWhatever";

    this.CAN_SCRIBE_OR_WHATEVER         = "CanScribeOrWhatever";
    this.SCRIBE_OR_WHATEVER             = "ScribeOrWhatever";
    this.CAN_EXAMINE_OR_WHATEVER        = "CanExamineOrWhatever";
    this.EXAMINE_OR_WHATEVER            = "ExamineOrWhatever";
    this.CAN_TURN                       = "CanTurn";
    this.TURN_ATTEMPT                   = "TurnAttempt";
    this.AURA_CREATE                    = "AURA_Create";
    this.AURA_EXIT                      = "AURA_Exit";
    this.AURA_ENTER                     = "AURA_Enter";
    this.AURA_ROUND                     = "AURA_Round";
    this.AURA_EFFECT                    = "AURA_Effect";
    this.GLOBAL_TIME                    = "Global_Time";
    this.TIME_ELAPSED                   = "OnTimeElapsed";

    this.NO_SUCH_SA                     = "-?-?-= ";

    this.ClearHookParameters = function () {
        throw "todo";
    }

    this.NUMHOOKPARAM = 10;
    this.p_hook_parameters = new HOOK_PARAMETERS();

    this.MAX_SPEC_AB = 100;
    this.specialAbilitiesData = new A_SPECABILITY_DEF_L();


    this.defaultGlobalScripts =
        [new DEFAULT_GLOBAL_SCRIPT(
            "CombatPlacement", new ASLENTRY(
            "PlaceMonsterFar",
            "$IF($GET_PARTY_FACING() >=#2)" +
            "{" +
                "$MonsterPlacement(\"17FbPV500E\");" +
            "}" +
            "$ELSE" +
            "{" +
                "$MonsterPlacement(\"16FbPV500E\");" +
            "};",
                SPECAB_STRING_TYPE.SPECAB_SCRIPT)
            )
        ];

    this.numDefaultGlobalScripts = this.defaultGlobalScripts.length;

    this.RunGlobalScript = function (SAname, scriptName, nullSA) {
        var saAbility = []; for (i = 0; i < SPECAB.MAX_SPEC_AB; i++) { saAbility[i] = ["", ""]; }
        var binScript;
        var pSpecAb = null;
        pSpecString = null;

        pSpecAb = this.specialAbilitiesData.FindAbility(SAname);
        if (pSpecAb == null) {
            // Perhaps we can find a default value for this script
            var i;
            for (i = 0; i < SPECAB.numDefaultGlobalScripts; i++) {
                if (SAname == SPECAB.defaultGlobalScripts[i].abilityName) {
                    if (SPECAB.defaultGlobalScripts[i].specString.Key() == scriptName) {
                        pSpecString = SPECAB.defaultGlobalScripts[i].specString;
                        break;
                    }
                };
            };
        };
        if ((pSpecAb != null) || (pSpecString != null)) {
            if (pSpecAb != null) {
                pSpecString = pSpecAb.Find(scriptName);
            };
            if (pSpecString != null) {
                if (pSpecString.Flags() == SPECAB_STRING_TYPE.SPECAB_SCRIPT) {
                    var gpdlcomp = new GPDLCOMP();
                    binScript = gpdlcomp.CompileScript(frontEnd + pSpecString.Value() + backEnd, SAentries);
                    if (binScript[0] != 0) {
                        Globals.WriteDebugString("* * * * Script Error in %s[%s]\n", SAname, scriptName);
                        pSpecString.SetValue(binScript);                                    // PORT NOTE: Changed name here because of non-overridable functions in JS
                        pSpecString.SetFlags(SPECAB_STRING_TYPE.SPECAB_SCRIPTERROR);        // PORT NOTE: Changed name here because of non-overridable functions in JS
                        if (pSpecAb != null) {
                            pSpecAb.Insert(scriptName, binScript, SPECAB_STRING_TYPE.SPECAB_SCRIPTERROR);
                        };
                    }
                    else {
                        pSpecString.SetValue(binScript);                                    // PORT NOTE: Changed name here because of non-overridable functions in JS
                        pSpecString.SetFlags(SPECAB_BINARYCODE);                            // PORT NOTE: Changed name here because of non-overridable functions in JS
                        if (pSpecAb != null) {
                            pSpecAb.Insert(scriptName, binScript, SPECAB_STRING_TYPE.SPECAB_BINARYCODE);
                        };
                    };
                };
                if (pSpecString.Flags() == SPECAB_STRING_TYPE.SPECAB_BINARYCODE) {
                    var origSA = new CStringPAIR();
                    var pOrigContext = new SPECIAL_ABILITIES();
                    gpdlStack.Push();
                    if (nullSA) {
                        var SAcontext = new CStringPAIR(SAname, "");
                        origSA = pScriptContext.specAb;
                        pOrigContext = pScriptContext.context;
                        pScriptContext.SetAbility(null, SAcontext);
                        pScriptContext.SetSA_ScriptName(scriptName);
                    };
                    this.p_hook_parameters[0] = gpdlStack.activeGPDL().ExecuteScript(pSpecString.Value(), 1, null, 0);
                    gpdlStack.Pop();
                    if (nullSA) {
                        pScriptContext.specAb = origSA;
                    };
                    if (globalSA_debug.Find(SAname) != null) {
                        Globals.WriteDebugString("@@SA \"" + SAname + "\" Script \"" + scriptName + "\": \n");
                    };
                }
            };
        }
        else {
            SPECAB.ScriptCallback_RunAllScripts(CBFUNC.CBF_DEFAULT, this.p_hook_parameters[0], null);
        };
        return this.p_hook_parameters[0];
    }


    this.ScriptCallback_RunAllScripts = function(func, scriptResult, pkt)
    {
        return CBRESULT.CBR_CONTINUE;
    }

};



function HOOK_PARAMETERS() {
    this.m_prevHookParameters = null;
    this.m_hookParameters = [SPECAB.NUMHOOKPARAM];
    this.m_prevHookParameters = SPECAB.p_hook_parameters;
    SPECAB.p_hook_parameters = this;
};


function SCRIPT_CONTEXT() {
    this.m_prevScriptContext = null;
    this.m_prevScriptContext = SPECAB.pScriptContext;
    this.pScriptContext = this;

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


var SPECAB = new SPECAB();