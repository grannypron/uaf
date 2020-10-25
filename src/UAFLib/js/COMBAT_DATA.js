/** TODO **/
function COMBAT_DATA() {

    this.m_pEvent = null;
    this.monsterDistance = eventDistType.getByNumber(0); // Initially  pEvent->distance.       // PORT NOTE: Defaults to 0
    // Reduced until some monsters are properly placed         

    this.m_hCharDeathSound = 0;
    this.m_hCharMoveSound = 0;
    this.m_hTurnUndeadSound = 0;

    this.m_bStartingNewCombatRound = false;
    this.m_bCombatActive = false;
    this.m_bLastRound = false;
    this.m_bCombatOver = false;
    this.m_bNeedUpdate = false;
    this.m_forceRoundDelay = false;
    this.m_iExpPointTotal = 0;
    this.m_iCurrCaster = 0;
    this.m_iRedirectedSpellTarget = 0;
    this.m_sRedirectedSpellMessage = "";
    this.m_sSpellEffectMessage = "";

    this.m_eUndeadTurnModifier = eventTurnUndeadModType.getByNumber(0);
    this.m_eCombatVictor = combatVictorType.getByNumber(0);

    this.m_iMoveToX = 0; this.m_iMoveToY = 0;
    this.m_iStartTerrainX = 0; this.m_iStartTerrainY = 0;
    this.m_iCursorX = 0; this.m_iCursorY = 0;
    this.m_iPartyStartX = 0; this.m_iPartyStartY = 0;
    this.m_iPartyOriginX = 0; this.m_iPartyOriginY = 0;
    this.m_bNewCombatantSinceLastUpdateCombat = false;
    this.m_iCurrInitiative = 0;
    this.m_iCurrRound = 0;
    this.m_iCurrTurn = 0;
    this.m_iAimPos = 0;

    this.m_eSurprise = eventSurpriseType.getByNumber(0);

    this.m_iActiveScripter = 0;

    // used for morale checks
    this.m_iNumMonFlee = 0;
    this.m_iNumMonSlain = 0;
    this.m_iNumFrndFlee = 0;
    this.m_iNumFrndSlain = 0;

    // flags that alter normal combat behavior
    this.m_bMonsterNoMove = false;
    this.m_bMonsterAlwaysMiss = false;
    this.m_bMonsterAlwaysHit = false;
    this.m_bMonsterNoCast = false;
    this.m_bMonsterPlyrControl = false;
    this.m_bMonsterNoFlee = false;
    this.m_bMonsterNoDeath = false;
    this.m_bPartyAlwaysMiss = false;
    this.m_bPartyAlwaysHit = false;
    this.m_bNoMagic = false;
    this.m_bPartyNeverDies = false;
    this.m_bPartyNoExperience = false;

    // which combat map format is used
    this.m_bIsWilderness = false;

    this.QComb = new QueuedCombatantData();
    this.m_combatRect = new RECT();
    this.m_fullRect = new RECT();
    this.hurledWeapons = new ITEM_LIST();
    this.turningSummary = new TURNING_SUMMARY();
    this.m_iNumCombatants = 0;
    this.m_aCombatants = [];
    this.m_aTempCOMBATANTS = [];

    this.m_nextAuraID = 0;
    this.m_auras = new CList();
    this.m_auraReferenceStack = [];  // auraID
}


COMBAT_DATA.prototype.Clear = function() {
    this.m_bStartingNewCombatRound = false;
    this.m_bIsWilderness = false;
    this.m_bCombatActive = false;
    this.ResetAim();
    this.ClearSounds();
    this.m_hCharDeathSound = -1;
    this.m_hCharMoveSound = -1;
    this.m_hTurnUndeadSound = -1;
    this.m_bLastRound = false;
    this.m_bCombatOver = false;
    this.m_eCombatVictor = combatVictorType.NoWinner;
    this.m_iExpPointTotal = 0;
    this.m_iStartTerrainX = -1;
    this.m_iStartTerrainY = -1;
    this.m_iCursorX = -1;
    this.m_iCursorY = -1;
    this.m_iPartyStartX = -1;
    this.m_iPartyStartY = -1;
    this.m_iCurrInitiative = -1;
    this.m_iCurrRound = -1;
    this.m_iCurrTurn = -1;
    this.m_nextAuraID = 1;
    this.m_auras.RemoveAll();
    this.RemoveAllCombatants();
    this.m_bNeedUpdate = false;
    this.m_bMonsterNoMove = false;
    this.m_bMonsterAlwaysMiss = false;
    this.m_bMonsterAlwaysHit = false;
    this.m_bMonsterNoCast = false
    this.m_bMonsterNoFlee = false;
    this.m_bMonsterPlyrControl = false;
    this.m_bMonsterNoDeath = false;
    this.m_iNumMonFlee = 0;
    this.m_iNumMonSlain = 0;
    this.m_iNumFrndFlee = 0;
    this.m_iNumFrndSlain = 0;
    this.m_bPartyAlwaysMiss = false;
    this.m_bPartyAlwaysHit = false;
    this.m_bPartyNeverDies = false;
    this.m_bNoMagic = false;
    this.m_iCurrCaster = -1;
    this.m_iRedirectedSpellTarget = -1;
    this.QComb.Clear();
    this.m_aCombatants = [Globals.MAX_COMBATANTS]; for (var idx = 0; idx < Globals.MAX_COMBATANTS; idx++) { this.m_aCombatants[idx] = new COMBATANT(); } // PORT NOTE:  C++ initializes this, and so I do here
    this.m_aTempCOMBATANTS = [Globals.MAX_COMBATANTS]; for (var idx = 0; idx < Globals.MAX_COMBATANTS; idx++) { this.m_aTempCOMBATANTS[idx] = new COMBATANT(); } // PORT NOTE:  C++ initializes this, and so I do here
    this.hurledWeapons.Clear();
    this.turningSummary.Clear();
}

COMBAT_DATA.prototype.StartCombatSummary = function() {
    combatSummary.m_thinkCount = 0;
    combatSummary.m_filenum = 0;
}

COMBAT_DATA.prototype.NumCombatants = function () {
    return this.m_iNumCombatants;
}

COMBAT_DATA.prototype.InitCombatData = function (event) {
    if (event == null) return;
    this.Clear();
    this.StartCombatSummary();

    itemData.FreeArt();
    spellData.FreeArt();
    monsterData.ClearSounds();
    itemData.ClearSounds();
    spellData.ClearSounds();
    pathMgr.Clear();
    Drawtile.ClearEmptyCellSearch();

    this.m_bCombatActive = true;
    this.m_bIsWilderness = event.m_UseOutdoorMap;

    this.m_combatRect.left = Globals.CombatScreenX;
    this.m_combatRect.top = Globals.CombatScreenY;
    this.m_combatRect.right = Globals.CombatScreenX + Globals.TILES_HORZ * COMBAT_TILE_WIDTH;
    this.m_combatRect.bottom = Globals.CombatScreenY + Globals.TILES_VERT * COMBAT_TILE_HEIGHT;

    this.m_fullRect.left = 0;
    this.m_fullRect.top = 0;
    this.m_fullRect.right = Globals.SCREEN_WIDTH;
    this.m_fullRect.bottom = Globals.SCREEN_HEIGHT;

    if (Globals.logDebuggingInfo) {
        /** TODO 
        ConfigFile.FindToken("Monster_NoMove", m_bMonsterNoMove, false); // don't remove these tokens
        ConfigFile.FindToken("Monster_AlwaysMiss", m_bMonsterAlwaysMiss, false);
        ConfigFile.FindToken("Monster_AlwaysHit", m_bMonsterAlwaysHit, false);
        ConfigFile.FindToken("Monster_NoCast", m_bMonsterNoCast, false);
        ConfigFile.FindToken("Monster_PlyrControl", m_bMonsterPlyrControl, false);
        ConfigFile.FindToken("Monster_NoDeath", m_bMonsterNoDeath, false);
        ConfigFile.FindToken("Party_AlwaysMiss", m_bPartyAlwaysMiss, false);
        ConfigFile.FindToken("Party_AlwaysHit", m_bPartyAlwaysHit, false);
        WriteDebugString("Combat Debug Settings:\n");
        WriteDebugString("       MonsNoMove = %u\n", m_bMonsterNoMove);
        WriteDebugString("    MonsAlwaysHit = %u\n", m_bMonsterAlwaysHit);
        WriteDebugString("   MonsAlwaysMiss = %u\n", m_bMonsterAlwaysMiss);
        WriteDebugString("       MonsNoCast = %u\n", m_bMonsterNoCast);
        WriteDebugString("      MonsNoDeath = %u\n", m_bMonsterNoDeath);
        WriteDebugString("  MonsPlyrControl = %u\n", m_bMonsterPlyrControl);
        WriteDebugString("  PartyAlwaysMiss = %u\n", m_bPartyAlwaysMiss);
        WriteDebugString("   PartyAlwaysHit = %u\n\n", m_bPartyAlwaysHit);
        WriteDebugString("     MapTileWidth = %u\n", TILES_HORZ);
        WriteDebugString("    MapTileHeight = %u\n\n", TILES_VERT);
        **/
    };
    this.m_bPartyNeverDies     = event.partyNeverDies;
    this.m_bPartyNoExperience  = event.partyNoExperience;
    this.m_bNoMagic            = event.noMagic;
    this.m_eSurprise           = event.m_eTEMPsurprise;
    this.m_eUndeadTurnModifier = event.turningMod;
    this.m_pEvent = event;

    this.LoadSounds();

    this.m_iPartyStartX = party.Posx;
    this.m_iPartyStartY = party.Posy;

    // this function adjusts the start x,y to reflect
    // x,y in combat terrain map
    if (!this.m_bIsWilderness)
        Drawtile.GenerateIndoorCombatMap(this.m_iPartyStartX, this.m_iPartyStartY, Drawtile.EventDirToPathDir(event.direction));
    else
        Drawtile.GenerateOutdoorCombatMap(this.m_iPartyStartX, this.m_iPartyStartY, Drawtile.EventDirToPathDir(event.direction));

    this.m_iStartTerrainX = 0;
    this.m_iStartTerrainY = 0;
    if (OBSTICAL_TYPE.OBSTICAL_none != Drawtile.ObsticalType(this.m_iPartyStartX, this.m_iPartyStartY, 1, 1, false, true, null)) {
        Globals.WriteDebugString("Starting x,y is not empty\n");
        Globals.die(0xab4cb);
    }

    this.m_iCurrInitiative = 1;  // Characters start with initiative=0 so we will need
    // to start a new round before we can begin.
    this.m_iCurrRound = 0;
    this.m_iCurrTurn = 0;
    this.m_bLastRound = false;
    this.m_bCombatOver = false;
    this.m_bStartingNewCombatRound = true;



    var monsterCount = 0;
    this.monsterDistance = this.m_pEvent.distance;
    for (; ;)  // Try distances until at least one monster in sincluded
    {
        var i;
        // Add all combatants to combatData.m_aCombatants.
        this.AddCombatants();
        for (i = 0; i < this.m_iNumCombatants; i++) {
            Globals.WriteDebugString("DEBUG - Combatant " + i + " starts at (" + this.m_aCombatants[i].x + "," + this.m_aCombatants[i].y + ")\n");
        }

        // Position each combatant on the map.
        this.determineInitCombatPos();

        for (i = 0; i < this.m_iNumCombatants; i++) {
            Globals.WriteDebugString("DEBUG - Combatant " + i + " starts at (" + this.m_aCombatants[i].x + "," + this.m_aCombatants[i].y + ")\n");
        };
        if (Globals.logDebuggingInfo) {
            Globals.WriteDebugString("Finished placing combatants on map\n");
        };
        this.CheckAllAuraPlacements(null);
        // make sure every monster can reach the party
        pathMgr.SetPathSize(1, 1); // 1x1 good enough to let party reach
        for (i = 0; i < this.NumCombatants()/*m_aCombatants.GetSize()*/; i++) {
            if (!this.m_aCombatants[i].IsPartyMember()) {
                var hPath;
                if ((this.m_aCombatants[i].x < 0)
                    || (hPath = pathMgr.GetPath(this.m_aCombatants[i].x, this.m_aCombatants[i].y,
                        this.m_iPartyStartX, this.m_iPartyStartY,
                        this.m_iPartyStartX, this.m_iPartyStartY,
                        false, null, false)) < 0) {
                    if ((this.m_aCombatants[i].x != this.m_iPartyStartX) || (this.m_aCombatants[i].y != this.m_iPartyStartY)) {
                        // no path from monster to party, so remove monster.
                        // Remove unreachable monster from combat map.
                        this.placeCombatant(this.m_aCombatants[i].x, this.m_aCombatants[i].y,
                            NO_DUDE,
                            this.m_aCombatants[i].width,
                            this.m_aCombatants[i].height);

                        // Remove unreachable monster from array combatData.m_aCombatants
                        // by marking position negative.
                        this.m_aCombatants[i].x = -1;
                        Globals.WriteDebugString("Combatant[" + i + "] removed because party could not reach him\n");
                    };
                }
                else
                    pathMgr.FreePath(hPath);
            }
            //else
            //  WriteDebugString("origIndex %i, origKey %i\n", combatants[i].origIndex, combatants[i].origKey);
        }
        // some combatants may not have been placed on the map

        // transfer valid combatants to temp
        var count = 0;

        for (i = 0; i < this.NumCombatants()/*m_aCombatants.GetSize()*/; i++) {
            //WriteDebugString("DEBUG - Compatant %d starts at (%d,%d)\n", i, m_aCombatants[i].x, m_aCombatants[i].y);
            if (this.m_aCombatants[i].x >= 0) {
                {
                    // We need to protect the m_pCharacter <-> m_pCombatant links so that 
                    // they are consistently one-to-one.  The COMBATANT copy operator
                    // will refuse to copy non-NULL links.
                    var pSaveCharPointer;
                    pSaveCharPointer = this.m_aCombatants[i].m_pCharacter;
                    this.m_aCombatants[i].m_pCharacter = null;
                    // Copy a NULL pointer to m_aTempCOMBATANTS
                    this.m_aTempCOMBATANTS[count] = this.m_aCombatants[i]; // PORT NOTE: this was this.m_aTempCOMBATANTS.SetAt(count, this.m_aCombatants[i]);
                    // Move the pointer to m_aTempCOMBATANTS
                    // Make sure they point at each other.
                    this.m_aTempCOMBATANTS[count].m_pCharacter = pSaveCharPointer;
                    pSaveCharPointer.m_pCombatant = this.m_aTempCOMBATANTS[i];
                };
                count++;
            }
            else {
                var pSaveCharPointer;
                var pSaveCombatantPointer;
                pSaveCombatantPointer = m_aCombatants[i];
                pSaveCharPointer = pSaveCombatantPointer.m_pCharacter;
                if (pSaveCharPointer.m_pCombatant != pSaveCombatantPointer) {
                    Globals.die(0x3334d);
                };
                pSaveCharPointer.m_pCombatant = null;
                if (pSaveCombatantPointer.deleteChar) {
                    pSaveCharPointer = null;
                };
                pSaveCombatantPointer.m_pCharacter = null;
            };
            //WriteDebugString("DEBUG - Compatant %d placed at (%d,%d)\n", i, m_aCombatants[i].x, m_aCombatants[i].y);
        }

        // check to see if any were left out
        if (count != this.NumCombatants()/*m_aCombatants.GetSize()*/) {
            Globals.TRACE("Adjusting combatant list from " + this.NumCombatants() + " to " + count + "\n");
            // now we need to clean up the self index
            // and the terrain references to existing combatants
            for (i = 0; i < count; i++) {
                if (this.m_aTempCOMBATANTS[i].self != i) {
                    // this one needs updating
                    this.m_aTempCOMBATANTS[i].self = i;
                    this.placeCombatant(this.m_aTempCOMBATANTS[i].x, this.m_aTempCOMBATANTS[i].y,
                        this.m_aTempCOMBATANTS[i].self,
                        this.m_aTempCOMBATANTS[i].width,
                        this.m_aTempCOMBATANTS[i].height);
                }
            }
        }
        this.m_iNumCombatants = count; //m_aCombatants.GetSize();


        // now move corrected data from Temp back into combatants
        count = 0;
        for (i = 0; i < this.m_iNumCombatants; i++) {
            var pSaveCharPointer;
            // Make sure the combatant <-> character links remain one-to-one
            pSaveCharPointer = this.m_aTempCOMBATANTS[i].m_pCharacter;
            this.m_aTempCOMBATANTS[i].m_pCharacter = null;
            this.m_aCombatants[count] = this.m_aTempCOMBATANTS[i]; // PORT NOTE:  this was this.m_aCombatants.SetAt(count, this.m_aTempCOMBATANTS[i]);
            // Make sure they point at each other.
            pSaveCharPointer.m_pCombatant = this.m_aCombatants[count];
            this.m_aCombatants[count].m_pCharacter = pSaveCharPointer;
            Globals.TRACE("Start Data for combatant " + count + ": self " +this.m_aCombatants[count].self + ", x " + this.m_aCombatants[count].x + ", y " + this.m_aCombatants[count].y + ", f " + this.m_aCombatants[count].GetIsFriendly() + "\n",);
            count++;
        };
        if (Globals.logDebuggingInfo) {
            Globals.WriteDebugString("Total combatants = " + this.m_iNumCombatants + "\n");
        };
        monsterCount = 0;
        // Let us count the monsters.
        for (i = 0; i < this.m_iNumCombatants; i++) {
            if (!this.m_aCombatants[i].IsPartyMember()) monsterCount++;
        };

        if ((monsterCount != 0) || (this.monsterDistance == eventDistType.UpClose)) break;
        this.RemoveAllCombatants();
        this.monsterDistance = (this.monsterDistance == eventDistType.FarAway) ? eventDistType.Nearby : eventDistType.UpClose;
        // Decrease distance until at leas one monster is reachable.
    };



    { // Moved here from AddCombatants so that we don't run the script 
        // for combatants that are removed from the combat.
        var i;
        var highPri, nextPri;
        var hookParameters = new HOOK_PARAMETERS();
        var scriptContext = new SCRIPT_CONTEXT();
        var actor;
        //This has been moved to AFTER the combatants have been placed on the map.
        //  Otherwise we run the scripts for monsters who did not fit on the map and
        //  were removed from the combat.
        //m_iNumCombatants = m_aCombatants.GetSize();
        for (i = 0; i < this.m_iNumCombatants; i++) {
            var priority = "";
            this.m_aCombatants[i].LoadCombatIcon();
            this.m_aCombatants[i].determineIconSize();
            actor = this.m_aCombatants[i].GetContextActor();
            RunTimeIF.SetCharContext(actor);
            scriptContext.SetCharacterContext(this.m_aCombatants[i].m_pCharacter);
            scriptContext.SetCombatantContext(this.m_aCombatants[i]);
            scriptContext.SetClassContext(this.m_aCombatants[i].m_pCharacter.GetClass());
            if (this.m_aCombatants[i].IsPartyMember()) {
                this.m_aCombatants[i].scriptPriority = 750;
            }
            else {
                this.m_aCombatants[i].scriptPriority = 250;
            };
            hookParameters[5] = this.m_aCombatants[i].scriptPriority;  // PORT NOTE: This used, CString.Format, which set the value of the string.  Was  hookParameters[5].Format("%d", this.m_aCombatants[i].scriptPriority);
            priority = this.m_aCombatants[i].RunCombatantScripts(SPECAB.PRE_START_COMBAT,
                SPECAB.ScriptCallback_RunAllScripts,
                null,
                "Initializing new combat");
            scriptContext.Clear();
            RunTimeIF.ClearCharContext();
            if (!(priority == null || priority == "")) {
                var k;
                k = UAFUtil.ScriptAtoI(priority);
                this.m_aCombatants[i].scriptPriority = k;
            };
        };
        highPri = UAFUtil.ByteFromHexString("0x3fffffff");
        // Run through the combatants performing the highPri combatants.
        // At the same time we compute the nextPri.
        do {
            nextPri = UAFUtil.ByteFromHexString("0x80000000");
            for (i = 0; i < this.m_iNumCombatants; i++) {
                if (this.m_aCombatants[i].scriptPriority == highPri) {
                    this.m_aCombatants[i].LoadCombatIcon();
                    this.m_aCombatants[i].determineIconSize();
                    actor = this.m_aCombatants[i].GetContextActor();
                    RunTimeIF.SetCharContext(actor);
                    scriptContext.SetCharacterContext(this.m_aCombatants[i].m_pCharacter);
                    this.m_aCombatants[i].RunCombatantScripts(ON_START_COMBAT,
                        SPECAB.ScriptCallback_RunAllScripts,
                        null,
                        "Initializing new combat");
                    scriptContext.Clear();
                    this.ClearCharContext();
                }
                else {
                    if ((this.m_aCombatants[i].scriptPriority < highPri)
                        && (this.m_aCombatants[i].scriptPriority > nextPri)) {
                        nextPri = this.m_aCombatants[i].scriptPriority;
                    };
                };
            };
            highPri = nextPri;
        } while (nextPri != UAFUtil.ByteFromHexString("0x80000000"));
    };


    this.QComb.Clear();

    this.PlaceCursorOnDude(0, true);
    Drawtile.ClearEmptyCellSearch(); // don't need this data anymore
    {
        var hookParameters = new HOOK_PARAMETERS();
        var scriptContext = new SCRIPT_CONTEXT();
        for (i = 0; i < this.m_iNumCombatants; i++)
        {
            scriptContext.SetCharacterContext(this.m_aCombatants[i].m_pCharacter);
            scriptContext.SetCombatantContext(this.m_aCombatants[i]);
            this.m_aCombatants[i].RunCombatantScripts(SPECAB.GUARDING_BEGIN_COMBAT,
                SPECAB.ScriptCallback_RunAllScripts,
                null,
                "Initializing new combat");
            this.ClearHookParameters();
            scriptContext.Clear();
        };
    };

}

COMBAT_DATA.prototype.PlaceCursorOnDude = function (dude, ForceCenter) {
    if ((dude < 0) || (dude >= this.m_iNumCombatants)) return;
    this.m_iCursorX = this.m_aCombatants[dude].x;
    this.m_iCursorY = this.m_aCombatants[dude].y;
    Drawtile.EnsureVisibleTargetTargetForceCenter(dude, ForceCenter);
}

COMBAT_DATA.prototype.ResetAim = function() {
    this.SetUpdateFlag();
    this.m_iAimPos = this.GetCurrCombatant();

    if ((this.m_iAimPos >= 0) && (this.m_iAimPos < this.m_iNumCombatants)) {
        this.m_iCursorX = this.m_aCombatants[m_iAimPos].x;
        this.m_iCursorY = this.m_aCombatants[m_iAimPos].y;
        Drawtile.EnsureVisibleCursor();
    }
}



COMBAT_DATA.prototype.CheckAuraPlacement = function(pAURA, pMoveData) {
    // If AURA has moved, notify Hooks.
    var recomputeAura = true;
    if (pAURA.attachment[0] == pAURA.attachment[1]) {
        if (((pAURA.attachment[0] == AURA_ATTACHMENT.AURA_ATTACH_COMBATANT)
            && (pAURA.combatantIndex[0] == pAURA.combatantIndex[1])
            && (pAURA.x[0] == this.m_aCombatants[pAURA.combatantIndex[0]].x)
            && (pAURA.y[0] == this.m_aCombatants[pAURA.combatantIndex[0]].y)
        )
            ||
            ((pAURA.attachment[0] == AURA_ATTACHMENT.AURA_ATTACH_COMBATANT_FACING)
                && (pAURA.combatantIndex[0] == pAURA.combatantIndex[1])
                && (pAURA.x[0] == this.m_aCombatants[pAURA.combatantIndex[0]].x)
                && (pAURA.y[0] == this.m_aCombatants[pAURA.combatantIndex[0]].y)
                && (pAURA.facing == this.m_aCombatants[pAURA.combatantIndex[0]].m_iMoveDir)
            )
            || ((pAURA.attachment[0] == AURA_ATTACHMENT.AURA_ATTACH_NONE)
            )
        ) {
            if (
                (pAURA.shape[0] == pAURA.shape[1])
                && (pAURA.size1[0] == pAURA.size1[1])
                && (pAURA.size2[0] == pAURA.size2[1])
                && (pAURA.size3[0] == pAURA.size3[1])
                && (pAURA.size4[0] == pAURA.size4[1])
                && (pAURA.spellID[0] == pAURA.spellID[1])
                && (pAURA.wavelength[0] == pAURA.wavelength[1])
            ) {
                if ((pMoveData == null) || (pAURA.wavelength[0] != AURA_WAVELENGTH_VISIBLE)) {
                    recomputeAura = false;
                };
            };
        };
    };
    if (recomputeAura) {
        if (pAURA.cells == null) {
            pAURA.cells = [MAX_TERRAIN_WIDTH * MAX_TERRAIN_HEIGHT];
        };
        pAURA.attachment[0]      = pAURA.attachment[1];
        pAURA.shape[0]           = pAURA.shape[1];
        pAURA.size1[0]           = pAURA.size1[1];
        pAURA.size2[0]           = pAURA.size2[1];
        pAURA.size3[0]           = pAURA.size3[1];
        pAURA.size4[0]           = pAURA.size4[1];
        pAURA.x[0]               = pAURA.x[1];
        pAURA.y[0]               = pAURA.y[1];
        pAURA.wavelength[0]      = pAURA.wavelength[1];
        pAURA.spellID[0]         = pAURA.spellID[1];
        pAURA.combatantIndex[0]  = pAURA.combatantIndex[1];
        if ((pAURA.attachment[0] == AURA_ATTACH_COMBATANT)
            || (pAURA.attachment[0] == AURA_ATTACH_COMBATANT_FACING)
        ) {
            pAURA.x[0]      = m_aCombatants[pAURA.combatantIndex[0]].x;
            pAURA.y[0]      = m_aCombatants[pAURA.combatantIndex[0]].y;
            pAURA.facing    = m_aCombatants[pAURA.combatantIndex[0]].m_iMoveDir;
        };
        switch (pAURA.shape[0]) {
            // cells[] <= 1 where AURA has effect.
            case AURA_SHAPE.AURA_SHAPE_NULL: this.DetermineNULLCoverage(pAURA); break;
            case AURA_SHAPE.AURA_SHAPE_GLOBAL: this.DetermineGlobalCoverage(pAURA); break;
            case AURA_SHAPE.AURA_SHAPE_ANNULARSECTOR: this.DetermineAnnularCoverage(pAURA); break;
            default:
      /* Really */ Globals.NotImplemented(0x5dd9, false);
                DetermineNULLCoverage(pAURA);
                break;
        };
        pAURA.RemoveSprite();
        var pSpell;
        pSpell = spellData.GetSpell(pAURA.spellID[1]);
        if (pSpell != null) {
            pAURA.AddSprite(pSpell);
        };
    };
    // Now we need to determine if any combatants have entered
    // or exited the AURA's effects.
    // Here is how we will proceed:
    //  For each combatant.
    //  if (combatant in AURA   &&   combatant not in m_combatants list)
    //  {
    //      ENTER(combatant)
    //      Add to list.
    //  }
    //  else if (combatatnt in m_combatants list   &&  combatant not in AURA)
    //  {
    //      EXIT(combatant)
    //      Remove from list
    //  }
    this.PushAuraReference(pAURA.auraID);
    {
        var i;
        for (i = 0; i < this.m_iNumCombatants; i++) {
            var x, y, cellIndex;
            x = this.m_aCombatants[i].x;
            y = this.m_aCombatants[i].y;
            if (x < 0) continue;
            cellIndex = y * Drawtile.MAX_TERRAIN_WIDTH + x;
            {
                if (pAURA.cells[cellIndex] & 1) {
                    // He is now in the AURA.
                    var j;
                    for (j = 0; j < pAURA.m_combatants.GetSize(); j++) {
                        if (pAURA.m_combatants[j] == i) break;
                    }
                    if (j == pAURA.m_combatants.GetSize()) {
                        // He moved into the AURA.
                        var hookParameters = new HOOK_PARAMETERS();
                        var scriptContext = new SCRIPT_CONTEXT();
                        scriptContext.SetCombatantContext(this.m_aCombatants[i]);
                        pAURA.RunAuraScripts(
                            SPECAB.AURA_ENTER,
                            SPECAB.ScriptCallback_RunAllScripts,
                            null,
                            "Aura moved and now includes combatant");
                        pAURA.m_combatants.Add(i);
                    };
                }
                else {
                    // He is not now in the AURA
                    var j;
                    for (j = 0; j < pAURA.m_combatants.GetSize(); j++) {
                        if (pAURA.m_combatants[j] == i) break;
                    };
                    if (j < pAUR.m_combatants.GetSize()) {
                        // He moved ou of the AURA
                        var hookParameters = new HOOK_PARAMETERS();
                        var scriptContext = new SCRIPT_CONTEXT();
                        scriptContext.SetCombatantContext(m_aCombatants[i]);
                        pAURA.RunAuraScripts(
                            SPECAB.AURA_EXIT,
                            SPECAB.ScriptCallback_RunAllScripts,
                            null,
                            "Aura moved and now excludes combatant");
                        pAURA.m_combatants.RemoveAt(j);
                    }
                };
            };
        };
    };
    PopAuraReference();
    return false;
}


COMBAT_DATA.prototype.PushAuraReference = function(auraID) {
    //**TODO**
    throw "todo";
    this.m_auraReferenceStack.Add(auraID);
}


COMBAT_DATA.prototype.DetermineNULLCoverage = function(pAURA) {
    //**TODO**
    throw "todo"
}

COMBAT_DATA.prototype.DetermineAnnularCoverage = function (pAURA) {
    //**TODO**
    throw "todo"
}

COMBAT_DATA.prototype.DetermineGlobalCoverage = function (pAURA) {
    //**TODO**
    throw "todo"
}

COMBAT_DATA.prototype.CheckAllAuraPlacements = function(pMoveData) {
    var pos;
    var redraw;
    var pmd;
    for (pos = this.m_auras.GetHeadPosition(); pos != null;) {
        var pAURA;
        pmd = pMoveData;
        pAURA = this.m_auras.GetNext(pos);
        pos = this.m_auras.NextPos(pos);
        do {
            // Why do we need to call 'CheckAuraPlacement' more than once?
            //
            redraw = this.CheckAuraPlacement(pAURA, pmd);
            pmd = null;
        } while (redraw);
    };
}

COMBAT_DATA.prototype.SetUpdateFlag = function () {
    this.m_bNeedUpdate = true;
}

COMBAT_DATA.prototype.GetCurrCombatant = function () {
    return this.QComb.Top();
}

COMBAT_DATA.prototype.EnsureVisibleCursor = function () {
  /** TODO:  Probably pass this off to a ScreenManager.EnsureVisibleCursor or something like that
  // offscreen cursor? then make visible and center it
  if (!ValidTerrainViewportCoords(combatData.m_iCursorX, combatData.m_iCursorY))
    forceCenter=TRUE;

    EnsureVisible(combatData.m_iCursorX, combatData.m_iCursorY, forceCenter);
    ***/
}

COMBAT_DATA.prototype.ClearSounds = function()
{
    if (this.m_hCharDeathSound >= 0)
        SoundMgr.ClearSound(this.m_hCharDeathSound);
    if (this.m_hCharMoveSound >= 0)
        SoundMgr.ClearSound(this.m_hCharMoveSound);
    if (this.m_hTurnUndeadSound >= 0)
        SoundMgr.ClearSound(this.m_hTurnUndeadSound);
    this.m_hCharDeathSound = -1;
    this.m_hCharMoveSound = -1;
    this.m_hTurnUndeadSound = -1;
}

COMBAT_DATA.prototype.LoadSounds = function () {
    this.ClearSounds();
    //WriteDebugString("106 Calling LoadSound(%s)\n", m_pEvent->DeathSound);
    this.m_hCharDeathSound = SoundMgr.LoadSound(this.m_pEvent.DeathSound);
    this.m_hCharMoveSound = SoundMgr.LoadSound(this.m_pEvent.MoveSound);
    this.m_hTurnUndeadSound = SoundMgr.LoadSound(this.m_pEvent.TurnUndeadSound);
}

COMBAT_DATA.prototype.RemoveAllCombatants = function() {
    for (i = 0; i < this.m_iNumCombatants; i++) {
        var pChar;
        var pCombatant;
        pCombatant = this.m_aCombatants[i];
        pChar = pCombatant.m_pCharacter;

        if (pChar == null) {
            var msg;
            msg = "Combatant[" + i + "] not linked to Character";
            Globals.MsgBoxError(msg, "Warning");
        }
        else {
            if (pChar.m_pCombatant != this.m_aCombatants[i]) {
                var msg;
                msg = "Character not linked to Combatant[" + i + "]";
                MsgBoxError(msg, "Warning");
            }
            else {
                pChar.m_pCombatant = null;
                if (pCombatant.deleteChar) {
                    pChar = null;  //PORT NOTE:  Was delete pChar;
                };
            };
            pCombatant.m_pCharacter = null;
        };
    };
    this.m_iNumCombatants = 0;
}

COMBAT_DATA.prototype.AddCombatants = function () {
    // always add party members first!
    var i;
    this.m_iNumCombatants = 0;
    this.AddCharsToCombatants();
    this.AddMonstersToCombatants();

    /*  This has been moved to AFTER the combatants have been placed on the map.
     *    Otherwise we run the scripts for monsters who did not fit on the map and
     *    were removed from the combat.
     *  //m_iNumCombatants = m_aCombatants.GetSize();
     */
    for (i = 0; i < this.m_iNumCombatants; i++) {
        var priority = "";
        this.m_aCombatants[i].LoadCombatIcon();
        this.m_aCombatants[i].determineIconSize();
    };
}



COMBAT_DATA.prototype.AddCharsToCombatants = function() {
    var temp = new COMBATANT();

    for (i = 0; i < party.numCharacters; i++)
    {
        var stype = party.characters[i].GetAdjStatus();
        if (((stype == charStatusType.Okay) || (stype == charStatusType.Dying))
            && (party.characters[i].GetAllowInCombat())) {
            temp.Clear();
            temp.self = this.m_iNumCombatants;
            //temp.InitFromCharData(i); 

            // Add new combatant to array of combatants.
            //temp.InitFromCharData(i);
            this.m_aCombatants[this.m_iNumCombatants] = temp;
            this.m_iNumCombatants++;
            {
                var j;
                for (j = 0; j < this.m_iNumCombatants; j++) {
                    if (this.m_aCombatants[j].m_pCharacter == null) continue;
                    this.m_aCombatants[j].m_pCharacter.m_pCombatant = this.m_aCombatants[j];
                };
            }
            this.m_aCombatants[this.m_iNumCombatants - 1].InitFromCharData(i);
        }
    }
}

COMBAT_DATA.prototype.AddMonstersToCombatants = function () {
    // STUB - do nothing

/*****************************************************************************
// NAME:    COMBAT_DATA::AddMonstersToCombatants
//
// PURPOSE: 
//
// RETURNS: None
void COMBAT_DATA:: AddMonstersToCombatants()
**TODO**
{
    COMBATANT temp;

    if (m_pEvent -> monsters.GetCount() == 0) {
        WriteDebugString("No monsters in combat event %i\n", m_pEvent -> GetEventId());
        return;
    }

    // how many monsters can we add?
    //int MaxMonstersAllowed = MAX_COMBATANTS - NumCombatants()/*m_aCombatants.GetSize()/ - 1;

    if (m_pEvent -> randomMonster) {
        // pick just one monster/NPC from the list
        int result = RollDice(m_pEvent -> monsters.GetCount(), 1, 0.0);
        COMBATANT * pCombatant;
        result -= 1; // make zero-based index;
        result = max(0, result);

        int count;
        if (m_pEvent -> monsters.monsters[result].UseQty == MONSTER_EVENT:: meUseQty)
        {
            count = m_pEvent -> monsters.monsters[result].qty;
        }
    else
        {
            count = RollDice(m_pEvent -> monsters.monsters[result].qtyDiceSides,
                m_pEvent -> monsters.monsters[result].qtyDiceQty,
                m_pEvent -> monsters.monsters[result].qtyBonus);
            WriteDebugString("Random quantity of combat monster: %i\n", count);
        }

        double mod = GetMonsterQtyMod();
        if (mod != 0.0) count += ((mod / 100.0) * count);
        if (count > MaxMonstersAllowed) count = MaxMonstersAllowed;
        if (count <= 0) count = 1;
        for (int j = 0; j < count; j++)
        {
            /*
             *  // All of this has been duplicated below so that we add the combatant to the combat
             *  // before updating its properties.  This is so that any scripts that reference
             *  // the combatant will find him actually a part of the combat.
             *     temp.Clear();
             *     temp.self = m_iNumCombatants; // init relies on self index
             *
             *     if (m_pEvent->monsters.monsters[result].type == MONSTER_TYPE)
             *       temp.InitFromMonsterData(m_pEvent->monsters.monsters[result].monster,
             *                                m_pEvent->monsters.monsters[result].friendly,
             *                                m_pEvent->monsters.monsters[result].items,
             *                                m_pEvent->monsters.monsters[result].money);
             *     else
             *       temp.InitFromNPCData(m_pEvent->monsters.monsters[result].monster,
             *                            m_pEvent->monsters.monsters[result].friendly,
             *                            m_pEvent->monsters.monsters[result].items,
             *                            m_pEvent->monsters.monsters[result].money);
             *
             *     if (temp.origKey != -1)
             *     {         
             *       temp.self = m_iNumCombatants;
             *       temp.SetMorale(m_pEvent->monsterMorale);
             *       temp.SetMorale( temp.GetMorale() + m_pEvent->monsters.monsters[result].moraleAdj );
             *
             *       int PreSpecAbCount = temp.spellEffects.GetCount();
             *       // turn on the special abilities
             *       Not Implemented(0xc98f4a,FALSE);//temp.specAbs.EnableAllFor(&temp);
             *
             *       int PostSpecAbCount = temp.spellEffects.GetCount();
             *       int Diff = PostSpecAbCount - PreSpecAbCount;
             *       if (Diff > 0)
             *       {
             *         POSITION pos = temp.spellEffects.GetTailPosition();  
             *         while ((pos != NULL) && (Diff > 0))
             *         {        
             *           temp.spellEffects.GetAt(pos).flags |= SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB;
             *           //temp.spellEffects.GetAt(pos).m_gsID = temp.self;
             *           temp.spellEffects.GetAt(pos).SourceChar_ID(temp.self);
             *           temp.spellEffects.GetPrev(pos);
             *           Diff--;
             *         }
             *       }
             *
             *       m_aCombatants.SetAtGrow(m_iNumCombatants, temp);
             *       m_iNumCombatants++;
             *     }
             * /




            // All of this is a duplicate of code above so that we add the combatant to the combat  //*
            // before updating its properties.  This is so that any scripts that reference          //*
            // the combatant will find him actually a part of the combat.                           //*
            temp.Clear();                                                                        //*
            //Not Implemented(0xc54d30,FALSE); // I cannot see how this ever worked.              //*
            // Either it encountered a Not Implemented()          //*
            // or it failed to SetAtGrow().                      //*
            temp.self = m_iNumCombatants; // init relies on self index                           //*
            m_aCombatants.SetAt(m_iNumCombatants, temp);                                         //*
            pCombatant = & m_aCombatants[m_iNumCombatants];
            m_iNumCombatants++;                                                                  //*
            //*
            if (m_pEvent -> monsters.monsters[result].m_type == MONSTER_TYPE)                      //*
                //temp.InitFromMonsterData(m_pEvent->monsters.monsters[result].monster,            //*
                pCombatant -> InitFromMonsterData(m_pEvent -> monsters.monsters[result].monsterID,     //*
                    m_pEvent -> monsters.monsters[result].friendly,      //*
                    m_pEvent -> monsters.monsters[result].items,         //*
                    m_pEvent -> monsters.monsters[result].money);        //*
            else                                                                                 //*
                //temp.InitFromNPCData(m_pEvent->monsters.monsters[result].monster,                //*
                pCombatant -> InitFromNPCData(m_pEvent -> monsters.monsters[result].characterID,       //*
                    m_pEvent -> monsters.monsters[result].friendly,          //*
                    m_pEvent -> monsters.monsters[result].items,             //*
                    m_pEvent -> monsters.monsters[result].money);            //*
            //*
            if (pCombatant -> origKey != -1)                                                              //*
            {                                                                                    //*
                pCombatant -> SetMorale(m_pEvent -> monsterMorale);                                    //*
                pCombatant -> SetMorale(pCombatant -> GetMorale()
                    + m_pEvent -> monsters.monsters[result].moraleAdj);           //*
                //*
                int PreSpecAbCount = pCombatant -> m_pCharacter -> m_spellEffects.GetCount();          //*
                int PostSpecAbCount = pCombatant -> m_pCharacter -> m_spellEffects.GetCount();         //*
                // turn on the special abilities                                                   //*
                //Not Implemented(0xc98f4a,FALSE);//temp.specAbs.EnableAllFor(&temp);               //*
                //*
                int Diff = PostSpecAbCount - PreSpecAbCount;                                       //*
                if (Diff > 0)                                                                      //*
                {                                                                                  //*
                    POSITION pos = pCombatant -> m_pCharacter -> m_spellEffects.GetTailPosition();       //*
                    while ((pos != NULL) && (Diff > 0))                                              //*
                    {                                                                                //*
                        pCombatant -> m_pCharacter -> m_spellEffects.GetAt(pos) -> flags |= SPELL_EFFECTS_DATA:: EFFECT_CHARSPECAB;   //*
                        //temp.spellEffects.GetAt(pos).m_gsID = temp.self;                             //*
                        pCombatant -> m_pCharacter -> m_spellEffects.GetAt(pos) -> SourceChar_ID(pCombatant -> self, true);   //*
                        pCombatant -> m_pCharacter -> m_spellEffects.PeekPrev(pos);                                //*
                        Diff--;                                                                        //*
                    }                                                                                //*
                }                                                                                  //*
            }                                                                                    //*    
        }
    }
    else {
        // add all monsters/NPCs
        for (int i = 0; i < m_pEvent -> monsters.GetCount(); i++)
        {
            int count;
            if (m_pEvent -> monsters.monsters[i].UseQty == MONSTER_EVENT:: meUseQty)
            {
                count = m_pEvent -> monsters.monsters[i].qty;
            }
      else
            {
                count = RollDice(m_pEvent -> monsters.monsters[i].qtyDiceSides,
                    m_pEvent -> monsters.monsters[i].qtyDiceQty,
                    m_pEvent -> monsters.monsters[i].qtyBonus);
                WriteDebugString("Random quantity of combat monster: %i\n", count);
            }

            double mod = GetMonsterQtyMod();
            if (mod != 0.0) count += ((mod / 100.0) * count);
            if (count <= 0) count = 1;

            for (int j = 0; j < count; j++)
            {
                TRACE("%d countLoop\n", timeGetTime());
                /*
                 *  // All of this has been duplicated below so that we add the combatant to the combat
                 *  // before updating its properties.  This is so that any scripts that reference
                 *  // the combatant will find him actually a part of the combat.
                 *       temp.Clear();
                 *       temp.self = m_iNumCombatants; // init relies on self index
                 *
                 *       if (m_pEvent->monsters.monsters[i].type == MONSTER_TYPE)
                 *         temp.InitFromMonsterData(m_pEvent->monsters.monsters[i].monster,
                 *                                  m_pEvent->monsters.monsters[i].friendly,
                 *                                  m_pEvent->monsters.monsters[i].items,
                 *                                  m_pEvent->monsters.monsters[i].money);
                 *       else
                 *         temp.InitFromNPCData(m_pEvent->monsters.monsters[i].monster,
                 *                              m_pEvent->monsters.monsters[i].friendly,
                 *                              m_pEvent->monsters.monsters[i].items,
                 *                              m_pEvent->monsters.monsters[i].money);
                 *
                 *       if (temp.origKey != -1)
                 *       {         
                 *         temp.self = m_iNumCombatants;
                 *         temp.SetMorale(m_pEvent->monsterMorale);
                 *         temp.SetMorale( temp.GetMorale() + m_pEvent->monsters.monsters[i].moraleAdj );
                 *
                 *         // int PreSpecAbCount = temp.spellEffects.GetCount();
                 *         // turn on the special abilities
                 *         // temp.specAbs.EnableAllFor(&temp);
                 *
                 *         // int PostSpecAbCount = temp.spellEffects.GetCount();
                 *         // int Diff = PostSpecAbCount - PreSpecAbCount;
                 *         // if (Diff > 0)
                 *         // {
                 *         //   POSITION pos = temp.spellEffects.GetTailPosition();  
                 *         //   while ((pos != NULL) && (Diff > 0))
                 *         //   {        
                 *         //     temp.spellEffects.GetAt(pos).flags |= SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB;
                 *         //     temp.spellEffects.GetAt(pos).SourceChar_ID(temp.self);
                 *         //     //temp.spellEffects.GetAt(pos).m_gsID = temp.self;
                 *         //     temp.spellEffects.GetPrev(pos);
                 *         //     Diff--;
                 *         //   }
                 *         // }
                 *
                 *         if (NumCombatants() < MaxMonstersAllowed)
                 *         {
                 *           m_aCombatants.SetAtGrow(m_iNumCombatants, temp);
                 *           m_iNumCombatants++;
                 *         }
                 *       }
                 * /




                // All of this is a duplicate of code above so that we add the combatant to the combat      //*
                // before updating its properties.  This is so that any scripts that reference              //*
                // the combatant will find him actually a part of the combat.                               //*
                temp.Clear();                                                                          //*
                temp.self = m_iNumCombatants; // init relies on self index                             //*
                //*
                if (NumCombatants() < MaxMonstersAllowed)                                              //* 
                {                                                                                      //* 
                    COMBATANT * pCombatant;                                                               //*
                    m_aCombatants.SetAt(m_iNumCombatants, temp);                                         //*
                    TRACE("%d after SetAt\n", timeGetTime());
                    m_iNumCombatants++;                                                                  //* 
                    pCombatant = & m_aCombatants[m_iNumCombatants - 1];                                     //*
                    //*
                    if (m_pEvent -> monsters.monsters[i].m_type == MONSTER_TYPE)                             //*
                    {
                        //pCombatant->InitFromMonsterData(m_pEvent->monsters.monsters[i].monster,          //*
                        pCombatant -> InitFromMonsterData(m_pEvent -> monsters.monsters[i].monsterID,          //*
                            m_pEvent -> monsters.monsters[i].friendly,          //*
                            m_pEvent -> monsters.monsters[i].items,             //*
                            m_pEvent -> monsters.monsters[i].money);            //*
                        TRACE("%d after InitFromMonsterData\n", timeGetTime());
                    }
                    else                                                                                 //*
                        //pCombatant->InitFromNPCData(m_pEvent->monsters.monsters[i].monster,              //*
                        pCombatant -> InitFromNPCData(m_pEvent -> monsters.monsters[i].characterID,            //*
                            m_pEvent -> monsters.monsters[i].friendly,               //*
                            m_pEvent -> monsters.monsters[i].items,                  //*
                            m_pEvent -> monsters.monsters[i].money);                 //*
                    //*
                    //if (pCombatant->origKey != NO_DUDE)                                                              //*
                    //if (pCombatant->m_pCharacter->monsterID != "")                                       //*
                    if ((pCombatant -> origKey != NO_DUDE)                                                              //*
                        && (pCombatant -> m_pCharacter -> monsterID != ""))                                       //*
                    {
                        //*

                        /* The following has been replaced 20180131  PRS
                        pCombatant->SetMorale(m_pEvent->monsterMorale);                                           //*
                        pCombatant->SetMorale( pCombatant->GetMorale() + m_pEvent->monsters.monsters[i].moraleAdj );     //*
                        * /
                        /* With this
                        * /
                        pCombatant -> SetMorale(pCombatant -> GetMorale() + m_pEvent -> monsterMorale);                                           //*

                        //*
                        // int PreSpecAbCount = temp.spellEffects.GetCount();                              //*
                        // turn on the special abilities                                                   //*
                        // temp.specAbs.EnableAllFor(&temp);                                               //*
                        //*
                        // int PostSpecAbCount = temp.spellEffects.GetCount();                             //*
                        // int Diff = PostSpecAbCount - PreSpecAbCount;                                    //*
                        // if (Diff > 0)                                                                   //*
                        // {                                                                               //*
                        //   POSITION pos = temp.spellEffects.GetTailPosition();                           //*
                        //   while ((pos != NULL) && (Diff > 0))                                           //*
                        //   {                                                                             //*
                        //     temp.spellEffects.GetAt(pos).flags |= SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB;//*          
                        //     temp.spellEffects.GetAt(pos).SourceChar_ID(temp.self);                      //*
                        //     //temp.spellEffects.GetAt(pos).m_gsID = temp.self;                          //*
                        //     temp.spellEffects.GetPrev(pos);                                             //*
                        //     Diff--;                                                                     //*
                        //   }                                                                             //*
                        // }                                                                               //*
                        //*
                    }                                                                                    //*
                    else {
                        CHARACTER * pCharacter;
                        pCharacter = pCombatant -> m_pCharacter;
                        if (pCharacter != NULL) {
                            pCombatant -> m_pCharacter -> m_pCombatant = NULL;
                            pCombatant -> m_pCharacter = NULL;
                            delete pCharacter;
                        };
                        m_iNumCombatants--;
                    };
                }                                                                                      //*





            }; // for (j)
        }; //for (i)
    }
}

*****************************************************************************/

}


var partyIndoorCombatArrangement = 
    // North
    "AB" + "ABBB" + "bBABBB" + "ABBBBCCC" + "bBABBBBCCC" + "bBABBBACCCBC" + "bBABBBCBACCCBC" + "bBABBBCBACDCBCCC" + "cBbBABBBCBACDCBCCC" + "cBbBABBBCBbCACDCBCCC" + "cBbBABBBCBDBbCACDCBCCC" + "cBbBABBBCBDBbCACDCBCECCC" + 
//East
        "bA" + "bAAB" + "cbbAAB" + "bAABcAbB" + "cbbAABcAbB" + "cbbAABdbbBcA" + "cbbAABBCdbbBcA" + "cbbAABBCdbACcAbB" + "dccbbAABBCdbACcAbB" + "dccbbAABBCecACdbcAbB" + "dccbbAABBCCDecACdbcAbB" + "dccbbAABBCCDecBDdbcAbBAC" + 
//South
        "AA" + "AAbA" + "BAAAbA" + "AAbAbbcb" + "BAAAbAbbcb" + "BAAAbAAbcbbb" + "CABAAAbAAbcbbb" + "CABAAAbABbcbAbbb" + "CABAAAbAcABbcbAbbb" + "CABAAAbAcABbdbcbAbbb" + "DACABAAAbAcABbdbcbAbbb" + "DACABAAAbAcACbdbBbcbAbbb" + 
//West
        "BB" + "BBAA" + "CCBBAA" + "BBAACBBA" + "CCBBAACBBA" + "CCBBAADCBACB" + "CCBBAAbbDCBACB" + "CCBBAAbbDCAbCBBA" + "CCBBAAbbccDCAbCBBA" + "CCBBAAbbccDCbcAbCBBA" + "DDCCBBAAbbccDCbcAbCBBA" + "DDCCBBAAbbccEDbcAbDCCBBA"
    //// North
    //  "AA" "AABA" "AABABC" "AABABCCC" "AABACABCCC" "AABACABCCCDC" "AABACABCCCDCDA" "AABACABCCCDCDAEC"
    ////East
    //  "AA" "AABB" "AABBcA" "AABBcAbB" "AABBCCcAbB" "AABBCCcAbBAC" "AABBCCcAbBACDD" "AABBCCcAbBACDDBD"
    ////South
    //  "AA" "AAbA" "AAbAbc" "AAbAbccc" "AAbAcAbccc" "AAbAcAbcccdc" "AAbAcAbcccdcdA" "AAbAcAbcccdcdAec"
    ////West
    //  "AA" "AAbB" "AAbBCA" "AAbBCABB" "AAbBcCCABB" "AAbBcCCABBAC" "AAbBcCCABBACdD" "AAbBcCCABBACdDbD"
    ;


var partyOutdoorCombatArrangement =
    // North
    "AB" + 
"ABBB" + 
"bBABBB" + 
"bBABBBCB" + 
"cBbBABBBCB" + 
"cBbbABBBCBDB" + 
"dBcBbBABBBCBDB" + 
"eBdBcBbBABBBCBDB" + 
"eBdBcBbBABBBCBDBEB" + 
"fBeBdBcBbBABBBCBDBEB" + 
"fBeBdBcBbBABBBCBDBEBFB" + 
"gBfBeBdBcBbBABBBCBDBEBFB" + 
//East
"bA" + 
"bAAB" + 
"cbbAAB" + 
"cbbAABBC" + 
"dccbbAABBC" + 
"dccbbAABBCCD" + 
"eddccbbAABBCCD" + 
"eddccbbAABBCCDDE" + 
"feeddccbbAABBCCDDE" + 
"feeddccbbAABBCCDDEEF" + 
"gffeeddccbbAABBCCDDEEF" + 
"gffeeddccbbAABBCCDDEEFFG" + 
//South
"AA" + 
"AAbA" + 
"BAAAbA" + 
"CABAAAbA" + 
"CABAAAbAbA" + 
"DACABAAAbAcA" + 
"DACABAAAbAcAdA" + 
"EADACABAAAbAcAdA" + 
"EADACABAAAbAcAdAeA" + 
"FAEADACABAAAbAcAdAeA" + 
"FAEADACABAAAbAcAdAeAfA" + 
"GAFAEADACABAAAbAcAdAeAfA" + 
//West
"BB" + 
"BBAA" + 
"CCBBAA" + 
"CCBBAAbb" + 
"DDCCBBAAbb" + 
"DDCCBBAAbbcc" + 
"EEDDCCBBAAbbcc" + 
"EEDDCCBBAAbbccdd" + 
"FFEEDDCCBBAAbbccdd" + 
"FFEEDDCCBBAAbbccddee" + 
"GGFFEEDDCCBBAAbbccddee" + 
"GGFFEEDDCCBBAAbbccddeeff"
    ;
  /*
// North
"AA"
"AABA"
"bAAABA"
"bAAABACA"
"cAbAAABACA"
"cAbAAABACADA"
"dAcAbAAABACADA"
"dAcAbAAABACADAEA"

//East
"AA"
"AABB"
"bbAABB"
"bbAABBCC"
"ccbbAABBCC"
"ccbbAABBCCDD"
"ddccbbAABBCCDD"
"ddccbbAABBCCDDEE"

//South
"AA"
"AAbA"
"BAAAbA"
"BAAAbAcA"
"CABAAAbAcA"
"CABAAAbAcAdA"
"DACABAAAbAcAdA"
"DACABAAAbAcAdAeA"

//West
"AA"
"AAbb"
"BBAAbb"
"BBAAbbcc"
"CCBBAAbbcc"
"CCBBAAbbccdd"
"DDCCBBAAbbccdd"
"DDCCBBAAbbccddee";
*/



COMBAT_DATA.prototype.determineInitCombatPos = function () {
    var i;
    var pReset = true;
    var mReset = true;
    var partyArrangement;
    monsterArrangement.Activate(this.NumCombatants());



    partyArrangement = partyIndoorCombatArrangement;
    {
        var hookParameters = new HOOK_PARAMETERS();
        var scriptContext = new SCRIPT_CONTEXT();
        var offset;
        var pc;
        hookParameters[5] = "0";
        hookParameters[6] = "0";
        switch (party.facing) {
            case 0:
                SPECAB.RunGlobalScript("Global_Combat", "PartyOriginNorth", true);
                break;
            case 1:
                SPECAB.RunGlobalScript("Global_Combat", "PartyOriginEast", true);
                break;
            case 2:
                //WriteDebugString("DEBUG - Running PartyOriginSouth script\n");
                SPECAB.RunGlobalScript("Global_Combat", "PartyOriginSouth", true);
                break;
            case 3:
                SPECAB.RunGlobalScript("Global_Combat", "PartyOriginWest", true);
                break;
        };
        pc = hookParameters[5];
        offset = UAFUtil.ScriptAtoI(pc, -8, +8);
        this.m_iPartyOriginX = this.m_iPartyStartX + offset;
        pc = hookParameters[6];
        offset = UAFUtil.ScriptAtoI(pc, -8, +8);
        this.m_iPartyOriginY = this.m_iPartyStartY + offset;
    };


    //WriteDebugString("DEBUG - Initial Party position = (%d, %d)\n", m_iPartyOriginX, m_iPartyOriginY);



    partyArrangement = partyIndoorCombatArrangement;
    {
        var hookParameters = new HOOK_PARAMETERS();
        var scriptContext = new SCRIPT_CONTEXT();
        var scriptResult = "";
        hookParameters[5] = "I";
        //partyArrangement = partyIndoorCombatArrangement;
        if (this.m_bIsWilderness) {
            hookParameters[5] = "O";
            partyArrangement = partyOutdoorCombatArrangement;
        }
        else {
            hookParameters[5] = "I";
            partyArrangement = partyIndoorCombatArrangement;
        };
        scriptResult = SPECAB.RunGlobalScript("Global_Combat", "PartyArrangement", true);
        if (!(scriptResult == null || scriptResult == "")) {
            if (scriptResult.GetLength() == partyIndoorCombatArrangement.length)
            {
                partyArrangement = scriptResult;
            }
      else
            {
                if (!DEBUG_STRINGS.debugStrings.AlreadyNoted(CString("PASWS"))) {
                    DEBUG_STRINGS.debugSeverity = 9;
                    Globals.WriteDebugString("Party arrangement string is wrong size\n");
                    Globals.MsgBoxInfo("Party arrangement string is wrong size");
                }
            };
        };
    };

    //WriteDebugString("DEBUG - partyArrangement = %s\n", partyArrangement);
    // Allocate a global structure to contain information about monster placement.

    // PC's and NPC's should be listed in combatData by marching order
    for (i = 0; i < this.NumCombatants(); i++) {
        if (this.m_aCombatants[i].GetIsFriendly()) {
            var rx;
            var ry;
            this.determineInitCombatPosCombatant(this.m_aCombatants[i],
                                i, 
                                pReset,
                                true,
                                partyArrangement);
            monsterArrangement.monsterPlacement[i].placeX = this.m_aCombatants[i].x;
            monsterArrangement.monsterPlacement[i].placeY = this.m_aCombatants[i].y;
            monsterArrangement.numParty++;
            rx = this.m_aCombatants[i].x - this.m_iPartyOriginX;
            ry = this.m_aCombatants[i].y - this.m_iPartyOriginY;
            if (rx > monsterArrangement.partyMaxX) monsterArrangement.partyMaxX = rx;
            if (rx < monsterArrangement.partyMinX) monsterArrangement.partyMinX = rx;
            if (ry > monsterArrangement.partyMaxY) monsterArrangement.partyMaxY = ry;
            if (ry < monsterArrangement.partyMinY) monsterArrangement.partyMinY = ry;
        }
        else {
            this.getNextMonsterCombatDirection(i, mReset);
        }
    };

    monsterArrangement.totalNumMonster = monsterArrangement.numMonster[0]
        + monsterArrangement.numMonster[1]
        + monsterArrangement.numMonster[2]
        + monsterArrangement.numMonster[3];
    {
        // We are going to call the MonsterArrangement hook to determine the
        // monster arrangement in battle.
        // We will call the hook four times....once for north, east, south, and west.
        var hookParameters = new HOOK_PARAMETERS();
        monsterArrangement.partyX = this.m_iPartyOriginX;
        monsterArrangement.partyY = this.m_iPartyOriginY;
        for (i = 0; i < monsterArrangement.numParty; i++) {
            monsterArrangement.partyPositions[i].x =
                this.m_aCombatants[i].x - monsterArrangement.partyX;
            monsterArrangement.partyPositions[i].y =
                this.m_aCombatants[i].y - monsterArrangement.partyY;
        };
        this.ComputeDistanceFromParty();
        for (i = 0; i < 4; i++)  // each of four directions
        {
            if (monsterArrangement.numMonster[i] == 0) continue;
            monsterArrangement.curDirFromParty = i;
            monsterArrangement.currentMonster = -1;
            monsterArrangement.turtleWidth = 1;
            monsterArrangement.turtleHeight = 1;
            monsterArrangement.lineOfSight = false;
            monsterArrangement.partyAccessible = true;
            monsterArrangement.limitMinY = UAFUtil.ByteFromHexString("0x80000000");
            monsterArrangement.limitMinX = UAFUtil.ByteFromHexString("0x80000000");
            monsterArrangement.limitMaxY = UAFUtil.ByteFromHexString("0x7fffffff");
            monsterArrangement.limitMaxX = UAFUtil.ByteFromHexString("0x7fffffff");
            switch (i) {
                case 0: // Headed North
                    monsterArrangement.dx['F' - 'A'] = -1;
                    monsterArrangement.dx['R' - 'A'] = 1;
                    monsterArrangement.dx['L' - 'A'] = -1;
                    monsterArrangement.dx['B' - 'A'] = 1;
                    monsterArrangement.dy['F' - 'A'] = -1;
                    monsterArrangement.dy['R' - 'A'] = 0;
                    monsterArrangement.dy['L' - 'A'] = 0;
                    monsterArrangement.dy['B' - 'A'] = 1;
                    break;
                case 1: // Headed East
                    monsterArrangement.dx['F' - 'A'] = 1;
                    monsterArrangement.dx['R' - 'A'] = 1;
                    monsterArrangement.dx['L' - 'A'] = -1;
                    monsterArrangement.dx['B' - 'A'] = -1;
                    monsterArrangement.dy['F' - 'A'] = 0;
                    monsterArrangement.dy['R' - 'A'] = 1;
                    monsterArrangement.dy['L' - 'A'] = -1;
                    monsterArrangement.dy['B' - 'A'] = 0;
                    break;
                case 2: // Headed south
                    monsterArrangement.dx['F' - 'A'] = 1;
                    monsterArrangement.dx['R' - 'A'] = -1;
                    monsterArrangement.dx['L' - 'A'] = 1;
                    monsterArrangement.dx['B' - 'A'] = -1;
                    monsterArrangement.dy['F' - 'A'] = 1;
                    monsterArrangement.dy['R' - 'A'] = 0;
                    monsterArrangement.dy['L' - 'A'] = 0;
                    monsterArrangement.dy['B' - 'A'] = -1;
                    break;
                case 3: // Headed West
                    monsterArrangement.dx['F' - 'A'] = -1;
                    monsterArrangement.dx['R' - 'A'] = -1;
                    monsterArrangement.dx['L' - 'A'] = 1;
                    monsterArrangement.dx['B' - 'A'] = 1;
                    monsterArrangement.dy['F' - 'A'] = 0;
                    monsterArrangement.dy['R' - 'A'] = -1;
                    monsterArrangement.dy['L' - 'A'] = 1;
                    monsterArrangement.dy['B' - 'A'] = 0;
                    break;
            };
            monsterArrangement.turtleX = 0;
            monsterArrangement.turtleStack[0] = 0;
            monsterArrangement.turtleStack[1] = 0;
            monsterArrangement.turtleStack[2] = 0;
            monsterArrangement.turtleStack[3] = 0;
            monsterArrangement.turtleY = 0;
            monsterArrangement.distance = -1;
            // 31 July 2014 PRS switch (m_pEvent->distance) 
            switch (monsterDistance) {
                case UpClose:
                    SPECAB.RunGlobalScript("CombatPlacement", "PlaceMonsterClose", true);  // Callback function immediately below.
                    break;
                case Nearby:
                    SPECAB.RunGlobalScript("CombatPlacement", "PlaceMonsterNear", true);  // Callback function immediately below.
                    break;
                case FarAway:
                    SPECAB.RunGlobalScript("CombatPlacement", "PlaceMonsterFar", true);  // Callback function immediately below.
            };
        };
    };

    monsterArrangement.deActivate();
}

//PORT NOTE:  This is global static, but I put it in a class because I didn't have a class for the whole Combatants.* file
COMBAT_DATA.prototype.ComputeDistanceFromParty = function() {
    // Fill the monsterArrangement.distanceFromParty array.
    var queue;
    var queueStart;
    var queueLen;
    queue = [Drawtile.MAX_TERRAIN_WIDTH * Drawtile.MAX_TERRAIN_HEIGHT];
    monsterArrangement.distanceFromParty = [Drawtile.MAX_TERRAIN_WIDTH * Drawtile.MAX_TERRAIN_HEIGHT];
    // 254 represents cells whose distance is unknown.
    // 255 represents a cell that is inaccessible.
    // 0 --> 253 represents a cell the party can reach.
    queueLen = 1;
    queueStart = 0;
    queue[queueStart] = Drawtile.MAX_TERRAIN_WIDTH * monsterArrangement.partyY + monsterArrangement.partyX;
    monsterArrangement.distanceFromParty[queue[queueStart]] = 0;
    while (queueLen > 0) {
        var d;
        var x, y, dx, dy;
        queueLen--;
        x = queue[queueStart] % Drawtile.MAX_TERRAIN_WIDTH;
        y = queue[queueStart] / Drawtile.MAX_TERRAIN_WIDTH;
        d = monsterArrangement.distanceFromParty[queue[queueStart]] + 1;
        queueStart = (queueStart + 1) % (Drawtile.MAX_TERRAIN_WIDTH * Drawtile.MAX_TERRAIN_HEIGHT);
        if (d > 253) {
            d = 253;
        };
        // Examine the 8 cells around x and y;
        for (dx = -1; dx <= 1; dx++) {
            for (dy = -1; dy <= 1; dy++) {
                var X, Y, index;
                if ((dx == 0) && (dy == 0)) continue;
                X = x + dx;
                Y = y + dy;
                if (X < 0) continue;
                if (Y < 0) continue;
                if (X >= Drawtile.MAX_TERRAIN_WIDTH) continue;
                if (Y >= Drawtile.MAX_TERRAIN_HEIGHT) continue;
                index = Y * Drawtile.MAX_TERRAIN_WIDTH + X;
                if (monsterArrangement.distanceFromParty[index] == 254) {
                    // If this cell is accessible add it to stack; else mark it inaccessible.
                    var cell;
                    cell = terrain[y][x].cell;
                    if ((cell < 1) || (cell > CurrentTileCount)) continue;
                    if (CurrentTileData[cell].tile_passable > 0) {
                        var j;
                        j = (queueStart + queueLen) % (Drawtile.MAX_TERRAIN_WIDTH * Drawtile.MAX_TERRAIN_HEIGHT);
                        monsterArrangement.distanceFromParty[index] = d;
                        queue[j] = index;
                        queueLen++;
                    }
                    else {
                        monsterArrangement.distanceFromParty[index] = 255;
                    };
                };
            };
        };
    };
    delete queue;
}


COMBAT_DATA.prototype.determineInitCombatPosCombatant = function(pCombatant, i, reset, ignoreCurrentCombatant, partyArrangement) {
    if (pCombatant.GetIsFriendly()) {
        //#ifdef newPartyArrangement                  // PORT NOTE: I THINK this is on
        var facing = 0;
        var dirStr = 0;
        var partySize = 0;
        var partyArrangementIndex = 0;
        // Save the position of the party itself as seen by the monsters.
        // Manikus says this is the position of the party member in the
        // case that the party consists of one character.
        facing = party.GetPartyFacing();

        switch (facing) {
            default:  // To keep the compiler quiet.
            case FACE_NORTH: dirStr = 0; break;
            case FACE_EAST: dirStr = 1; break;
            case FACE_SOUTH: dirStr = 2; break;
            case FACE_WEST: dirStr = 3; break;
        };
        partyArrangementIndex = dirStr * (MAX_PARTY_MEMBERS + 1) * (MAX_PARTY_MEMBERS);
        // partyArrangement points to one of four direction strings.
        //
        // Now find the string that corresponds to the party size.
        partySize = party.numCharacters;
        partyArrangementIndex += (partySize - 1) * (partySize);   //n*(a+b)/2=(ps-1)*(2+2*(ps-1))/2=(ps-1)*(1+ps-1)
        // Now we assume that the marching order is equal to i.
        partyArrangementIndex += 2 * i;

        this.getNextCharCombatPos(pCombatant,
            reset,
            ignoreCurrentCombatant,
            partyArrangement + partyArrangementIndex);
        //#else
        //     getNextCharCombatPos(pCombatant, reset, ignoreCurrentCombatant);
        //#endif
        if (pCombatant.x >= 0) {
            Drawtile.placeCombatant(pCombatant.x,
                pCombatant.y,
                i,
                pCombatant.width,
                pCombatant.height);
        }
    }
    else {
        /*
         * #ifdef newMonsterArrangement // PORT NOTE: I THINK this is on, but all the code in the true clause was commented out
        //#else
           getNextMonsterCombatPos(*pCombatant);
           if (pCombatant->x >= 0)
           {
             placeCombatant(pCombatant->x, 
                            pCombatant->y, 
                            i, 
                            pCombatant->width,
                            pCombatant->height);
           }
         //#endif
         */
    }
}

var COMBAT_DATA_initialX = 0; //
var COMBAT_DATA_initialY = 0;
var COMBAT_DATA_searchOrder = [-1, -1, 0, -1, 1, -1, -1, 0, 1, 0, -1, 1, 0, 1, 1, 1];

COMBAT_DATA.prototype.getNextCharCombatPos = function(pDude,  pReset, ignoreCurrentCombatant, partyArrangement) {
    var initialX = COMBAT_DATA_initialX;        // PORT NOTE: these two were originally static int variables
    var initialY = COMBAT_DATA_initialY;
    var bestX = 0, bestY = 0;
    var x = 0, y = 0, dx = 0, dy = 0;
    var dir = 0, delta = 0;
    var i = 0;
    var searchOrder = COMBAT_DATA_searchOrder; // PORT NOTE:  Another static variable

    if (pReset) {
        initialX = this.m_iPartyOriginX;
        initialY = this.m_iPartyOriginY;
        pReset = false;
    }
    bestX = initialX;
    bestY = initialY;
    bestX += UAFUtil.Decode(partyArrangement[0]);
    bestY += UAFUtil.Decode(partyArrangement[1]);
    //WriteDebugString("DEBUG - initialX=%d; initialY=%d; bestX=%d; bestY=%d\n",initialX, initialY, bestX, bestY);
    dir = 3;
    i = -1;
    for (delta = 0; delta < 8; delta++)  // search 225 cells.....enough is enough!
    {
        dx = 1;
        dy = 0;
        x = bestX - delta;
        y = bestY - delta;
        for (; dir < 4; dir++) {
            for (; i < 2 * delta; i++) {
                if (this.PlaceCharacter(pDude, x, y, ignoreCurrentCombatant)) {
                    if (OBSTICAL_TYPE.OBSTICAL_none != OBSTICAL_TYPE.ObsticalType(pDude.x, pDude.y, pDude.width, pDude.height, true, ignoreCurrentCombatant, null))
                        Globals.WriteDebugString("getNextCharCombatPos() returns a cell containing a wall\n");
                    else
                        Globals.TRACE("Placing char at combat pos " + pDude.x + "," + pDude.y + "\n");
                    return;
                };
                x += dx;
                y += dy;
            };
            i = dx;
            dx = -dy;
            dy = i;
            i = 0;

        };
        dir = 0;
    };

    if (OBSTICAL_TYPE.OBSTICAL_none != OBSTICAL_TYPE.ObsticalType(pDude.x, pDude.y, pDude.width, pDude.height, true, ignoreCurrentCombatant, null))
        Globals.WriteDebugString("getNextCharCombatPos() returns a cell containing a wall\n");
    else
        Globals.TRACE("Placing char at combat pos " + pDude.x + "," + pDude.y + "\n");
}


COMBAT_DATA.prototype.PlaceCharacter = function(pDude, x, y, ignoreCurrentCombatant) {
    if (OBSTICAL_TYPE.OBSTICAL_none == OBSTICAL_TYPE.ObsticalType(x, y, pDude.width, pDude.height, true, ignoreCurrentCombatant, null)) {
        pDude.x = x;
        pDude.y = y;
        return true;
    }
    return false;
}


//#ifdef newCombatant  // PORT NOTE:  I think this is true
COMBAT_DATA.prototype.GetCombatant = function (dude) {
    if (!this.IsValidDude(dude)) return null;
    if ((this.m_aCombatants[dude].m_pCharacter != null)
        && (this.m_aCombatants[dude].m_pCharacter.m_pCombatant != this.m_aCombatants[dude])
    ) {
        Globals.die(0x8ce2ab);
    };
    return this.m_aCombatants[dude];
}
//#else
//COMBATANT * GetCombatant(int dude){ return IsValidDude(dude) ?& m_aCombatants[dude] : NULL; };
//#endif

COMBAT_DATA.prototype.IsValidDude = function(dude) {
    return (dude >= 0) && (dude < this.NumCombatants());
}
