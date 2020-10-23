/** TODO **/
function SPECIAL_ABILITIES() {
    this.m_specialAbilities = new A_CStringPAIR_L();
    this.m_readOnly = false;
}


SPECIAL_ABILITIES.prototype.Clear = function () {
    this.m_specialAbilities.Clear();
}

SPECIAL_ABILITIES.prototype.GetHeadPosition = function () {
    return this.m_specialAbilities.GetStartPosition();
}

SPECIAL_ABILITIES.prototype.FindAbility = function (key) {
    for (i = 0; i < this.m_specialAbilities; i++) {
        if (this.m_specialAbilities[i] != null && this.m_specialAbilities[i].m_key == key) {
            return this.m_specialAbilities[i];
        }
    }
}

SPECIAL_ABILITIES.prototype.GetNextData = function(pos) {
    var pEntry;
    pEntry = this.m_specialAbilities.GetNextAssoc(pos);

    return pEntry;
}

SPECIAL_ABILITIES.prototype.NextPos = function(pos) {
    return this.m_specialAbilities.NextPos(pos);
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

SPECIAL_ABILITIES.prototype.GetCount = function (){
    this.m_specialAbilities.GetCount();
}

SPECIAL_ABILITIES.prototype.RunScripts = function(scriptName, fnc, pkt, comment, sourceType, sourceName) {
    var scripts = [SPECAB.MAX_SPEC_AB];
    var saAbility = [SPECAB.MAX_SPEC_AB];
    var binScript = "";
    var numScript = 0;
    var i;
    var pAbility = null;
    var pSpecAb;
    var pSpecString;
    var pos;
    pos = this.GetHeadPosition();
    while (pos != null) {
        pAbility = this.GetNextData(pos);
        pos = this.NextPos(pos);    // PORT NOTE:  Get the list of ability keys that I have stored (i.e. in this collection)
        if (pAbility == null) break;  // Why would this happen?
        pSpecAb = specialAbilitiesData.FindAbility(pAbility.m_key);  // PORT NOTE:  Look up my ability key in the global abilities database
        if (pSpecAb == null) continue;
        pSpecString = pSpecAb.Find(scriptName);
        if (pSpecString == null) continue;
        if (numScript >= SPECAB.MAX_SPEC_AB) continue;
        if (pSpecString.Flags() == SPECAB_STRING_TYPE.SPECAB_SCRIPT) {
            var gpdlcomp = new GPDLCOMP();
            binScript = gpdlcomp.CompileScript(frontEnd + pSpecString.Value() + backEnd, SAentries);
            if (binScript[0] != 0) {
                Globals.WriteDebugString("* * * * Script Error in " + pAbility.Key() + "[" + scriptName + "]\n");
                pSpecString.Value(binScript);
                pSpecString.Flags(SPECAB_STRING_TYPE.SPECAB_SCRIPTERROR);
                pSpecAb.Insert(scriptName, binScript, SPECAB_STRING_TYPE.SPECAB_SCRIPTERROR);
                continue;
            };
            pSpecString.Value(binScript);
            pSpecString.Flags(SPECAB_STRING_TYPE.SPECAB_BINARYCODE);
            pSpecAb.Insert(scriptName, binScript, SPECAB_STRING_TYPE.SPECAB_BINARYCODE);
        };
        if (pSpecString.Flags() != SPECAB_STRING_TYPE.SPECAB_BINARYCODE) continue;
        saAbility[numScript] = pAbility;
        scripts[numScript++] = pSpecString;
    };
    if (numScript != 0) {
        var callbackResult;
        for (i = 0; i < numScript; i++) {
            pSpecString = scripts[i];
            pScriptContext.SetAbility(this, saAbility[i]);
            pScriptContext.SetSA_Source_Type(sourceType);
            pScriptContext.SetSA_Source_Name(sourceName);
            pScriptContext.SetSA_ScriptName(pSpecString.Key());
            gpdlStack.Push();
            SPECAB.p_hook_parameters[0] = gpdlStack.activeGPDL().ExecuteScript(scripts[i].Value(), 1, null, 0);
            gpdlStack.Pop();
            pScriptContext.ClearAbility();
            callbackResult = fnc(CBFUNC.CBF_EXAMINESCRIPT, SPECAB.p_hook_parameters[0], pkt);

            if ((globalLoggingFlags & 1) || (globalSA_debug.Find(saAbility[i].Key()) != NULL)) {
                WriteDebugString("@@SA \"%s\" Script \"%s\": %s%s returned \"%s\"\n",
                    saAbility[i].Key(),
                    scripts[i].Key(),
                    pScriptContext.GetSourceTypeName(),
                    sourceName,
                    SPECAB.p_hook_parameters[0]);
            };

            if (callbackResult == CBR_STOP) {
                return SPECAB.p_hook_parameters[0];
            };
        };
        fnc(CBFUNC.CBF_ENDOFSCRIPTS, SPECAB.p_hook_parameters[0], pkt);
    }
    else {
        fnc(CBFUNC.CBF_DEFAULT, SPECAB.p_hook_parameters[0], pkt);
    };
    return SPECAB.p_hook_parameters[0];
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
        var i;
        this.p_hook_parameters = [];
        for (i = 0; i < 10; i++) {
            this.p_hook_parameters[i] = "";
        };
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

SPECAB.prototype.loadData = function (data, fullPath) {

    /** TODO  - temporarily using a stub here because the compression is just too hard right now **/
    var loader = new UAFLib.SpecabilityLoader();
    var data = loader.load("C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\SpecialAbilities.xml");

    var enumerator = data.GetEnumerator();
    while (enumerator.MoveNext()) {
        var key = enumerator.Current.Key;
        var value = enumerator.Current.Value;
        for (var idxAsl = 0; idxAsl < value.length; idxAsl++) {
            try {
                specialAbilitiesData.InsertString(key, value[idxAsl][0], value[idxAsl][1], 2);
            } catch (ex) {
                Globals.debug("SA load failed on " + key);
            }
        }
    }
    return true;

/*
    var e;
    var myFile;
    var success = true;

    if (globalData.version < _SPECIAL_ABILITIES_VERSION_) return success;

    var fileBytes;
    try {
        fileBytes = System.IO.File.ReadAllBytes(fullPath);
    } catch (ex) {
        Globals.WriteDebugString("Unable to open special abilities db file " + fullPath + ", error " + ex + " \n");
        return 0;
    }

    data.Clear();


    {
        Globals.WriteDebugString("Loading special abilities DB\n");
        var ar = new CAR(System.IO.File.OpenRead(fullPath), 6.0);
//        try {
            data.SerializeCAR(ar);
//        }
//        catch (ex)
//        {
//            throw ex;
//            Globals.WriteDebugString("Unable to load special abilities data file " + fullPath + "\n");
//            success = false;
//        }
        ar.Close();
    }

    if (data.GetCount() == 0)
        Globals.WriteDebugString("No special abilities in special abilities db file\n");

    return success;*/
}

SPECAB.prototype.ScriptCallback_MinMax = function (func, scriptResult, pkt) {
    var minmax = pkt;
    var n;
    var pStr;
    if (func != CBFUNC.CBF_EXAMINESCRIPT) return CBRESULT.CBR_CONTINUE;
    pStr = scriptResult;
    // If it starts with a number we simply move the
    // max down to that number and the min up to that number.
    if (UAFUtil.IsDigit(pStr[0])) {
        n = UAFUtil.ScriptAtoI(pStr, minmax[0], minmax[1]);
        if (n > minmax[0]) minmax[0] = n;
        if (n < minmax[1]) minmax[1] = n;
        return CBRESULT.CBR_CONTINUE;
    };
    for (; ;) {
        if (pStr[0] == '=') {
            n = UAFUtil.ScriptAtoI(pStr, minmax[0], minmax[1]);
            if (n > minmax[0]) minmax[0] = n;
            if (n < minmax[1]) minmax[1] = n;
            continue;
        };
        if (pStr[0] == '<') {
            pStr++;
            n = SUAFUtil.criptAtoI(pStr, minmax[0], 0x7fffffff);
            minmax[1] = n;
            continue;
        };
        if (pStr[0] == '>') {
            pStr++;
            n = UAFUtil.ScriptAtoI(pStr, 0, minmax[1]);
            minmax[0] = n;
            continue;
        };
        return CBRESULT.CBR_CONTINUE;
    };
}



var SPECAB = new SPECAB();