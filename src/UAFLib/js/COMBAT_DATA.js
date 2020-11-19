/** TODO **/
function COMBAT_DATA() {

    this.m_pEvent = null;
    this.monsterDistance = 0; // Initially  pEvent->distance.       // PORT NOTE: Defaults to 0
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

    this.m_eUndeadTurnModifier = 0;
    this.m_eCombatVictor = 0;

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

    this.m_eSurprise = 0;

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
    this.m_aCombatants = []; //for (var idx = 0; idx < Globals.MAX_COMBATANTS; idx++) { this.m_aCombatants[idx] = new COMBATANT(); }
    this.m_aTempCOMBATANTS = []; //for (var idx = 0; idx < Globals.MAX_COMBATANTS; idx++) { this.m_aTempCOMBATANTS[idx] = new COMBATANT(); }
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
    if (!this.m_bIsWilderness) {
        var xy = Drawtile.GenerateIndoorCombatMap(this.m_iPartyStartX, this.m_iPartyStartY, Drawtile.EventDirToPathDir(event.direction));
        this.m_iPartyStartX = xy.partyX; this.m_iPartyStartY = xy.partyY;    // PORT NOTE: Dealing with pass-by-reference parameters
    } else {
        var xy = Drawtile.GenerateOutdoorCombatMap(this.m_iPartyStartX, this.m_iPartyStartY, Drawtile.EventDirToPathDir(event.direction));
        this.m_iPartyStartX = xy.partyX; this.m_iPartyStartY = xy.partyY;    // PORT NOTE: Dealing with pass-by-reference parameters
    }

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
            //Globals.WriteDebugString("DEBUG - Combatant " + i + " starts at (" + this.m_aCombatants[i].x + "," + this.m_aCombatants[i].y + ")\n");
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
                    || (hPath = pathMgr.GetPathIntIntIntInt(this.m_aCombatants[i].x, this.m_aCombatants[i].y,
                        this.m_iPartyStartX, this.m_iPartyStartY,
                        this.m_iPartyStartX, this.m_iPartyStartY,
                        false, null, false)) < 0) {
                    if ((this.m_aCombatants[i].x != this.m_iPartyStartX) || (this.m_aCombatants[i].y != this.m_iPartyStartY)) {
                        // no path from monster to party, so remove monster.
                        // Remove unreachable monster from combat map.
                        Drawtile.placeCombatant(this.m_aCombatants[i].x, this.m_aCombatants[i].y,
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
                pSaveCombatantPointer = this.m_aCombatants[i];
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
                    Drawtile.placeCombatant(this.m_aTempCOMBATANTS[i].x, this.m_aTempCOMBATANTS[i].y,
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
            Globals.TRACE("Start Data for combatant " + count + ": self " + this.m_aCombatants[count].self + ", x " + this.m_aCombatants[count].x + ", y " + this.m_aCombatants[count].y + ", f " + this.m_aCombatants[count].GetIsFriendly() + "\n",);
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
            if (!UAFUtil.IsEmpty(priority)) {
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
                    this.m_aCombatants[i].RunCombatantScripts(SPECAB.ON_START_COMBAT,
                        SPECAB.ScriptCallback_RunAllScripts,
                        null,
                        "Initializing new combat");
                    scriptContext.Clear();
                    RunTimeIF.ClearCharContext();
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
            SPECAB.ClearHookParameters();
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
            pAURA.cells = [];
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
            pAURA.x[0]      = this.m_aCombatants[pAURA.combatantIndex[0]].x;
            pAURA.y[0]      = this.m_aCombatants[pAURA.combatantIndex[0]].y;
            pAURA.facing    = this.m_aCombatants[pAURA.combatantIndex[0]].m_iMoveDir;
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
    this.AddMonstersToCombatants();         // PORT NOTE:  x and y of Combatants are set to -1 at this point

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
            temp = new COMBATANT();   // PORT NOTE:  I don't know how this was only temp.Clear() here, but it was.  Should have had multiple copies of the same combatant object in the m_aCombatants array...
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
    var temp = new COMBATANT();
    if (this.m_pEvent.monsters.GetCount() == 0) {
        Globals.WriteDebugString("No monsters in combat event " + this.m_pEvent.GetEventId() + "\n");
        return;
    }

    // how many monsters can we add?
    var MaxMonstersAllowed = Globals.MAX_COMBATANTS - this.NumCombatants() - 1;

    if (this.m_pEvent.randomMonster) {
        // pick just one monster/NPC from the list
        var result = Globals.RollDice(this.m_pEvent.monsters.GetCount(), 1, 0.0);
        var pCombatant;
        result -= 1; // make zero-based index;
        result = Math.max(0, result);

        var count;
        if (this.m_pEvent.monsters.monsters[result].UseQty == MONSTER_EVENT.meUseQty)
        {
            count = this.m_pEvent.monsters.monsters[result].qty;
        }
    else
        {
            count = Globals.RollDice(this.m_pEvent.monsters.monsters[result].qtyDiceSides,
                this.m_pEvent.monsters.monsters[result].qtyDiceQty,
                this.m_pEvent.monsters.monsters[result].qtyBonus);
            Globals.WriteDebugString("Random quantity of combat monster: " + count + "\n");
        }

        var mod = Globals.GetMonsterQtyMod();
        if (mod != 0.0) count += ((mod / 100.0) * count);
        if (count > MaxMonstersAllowed) count = MaxMonstersAllowed;
        if (count <= 0) count = 1;
        for (var j = 0; j < count; j++)
        {
            temp = new COMBATANT();    // PORT NOTE:  I don't know how this was only temp.Clear() here, but it was.  Should have had multiple copies of the same combatant object in the m_aCombatants array...
            temp.self = this.m_iNumCombatants; // init relies on self index                           
            this.m_aCombatants[this.m_iNumCombatants] = temp;                                         
            pCombatant = this.m_aCombatants[this.m_iNumCombatants];
            this.m_iNumCombatants++;                                                                  
            
            if (this.m_pEvent.monsters.monsters[result].m_type == MONSTER_TYPE)                      
                pCombatant.InitFromMonsterDataID(this.m_pEvent.monsters.monsters[result].monsterID,     
                    this.m_pEvent.monsters.monsters[result].friendly,      
                    this.m_pEvent.monsters.monsters[result].items,         
                    this.m_pEvent.monsters.monsters[result].money);        
            else                                                                                 
                pCombatant.InitFromNPCData(this.m_pEvent.monsters.monsters[result].characterID,       
                    this.m_pEvent.monsters.monsters[result].friendly,          
                    this.m_pEvent.monsters.monsters[result].items,             
                    this.m_pEvent.monsters.monsters[result].money);            
            
            if (pCombatant.origKey != -1)                                                              
            {                                                                                    
                pCombatant.SetMorale(this.m_pEvent.monsterMorale);                                    
                pCombatant.SetMorale(pCombatant.GetMorale()
                    + this.m_pEvent.monsters.monsters[result].moraleAdj);           
                
                var PreSpecAbCount = pCombatant.m_pCharacter.m_spellEffects.GetCount();          
                var PostSpecAbCount = pCombatant.m_pCharacter.m_spellEffects.GetCount();         
                // turn on the special abilities                                                   
                
                var Diff = PostSpecAbCount - PreSpecAbCount;                                       
                if (Diff > 0)                                                                      
                {                               
                    var pos = pCombatant.m_pCharacter.m_spellEffects.GetTailPosition();       
                    while ((pos != null) && (Diff > 0))                                              
                    {                                                                                
                        pCombatant.m_pCharacter.m_spellEffects.GetAt(pos).flags |= SPELL_EFFECTS_DATA.EFFECT_CHARSPECAB;   
                        pCombatant.m_pCharacter.m_spellEffects.GetAt(pos).SourceChar_ID(pCombatant.self, true);   
                        pos = pCombatant.m_pCharacter.m_spellEffects.PrevPos();
                        Diff--;                                                                        
                    }                                                                                
                }                                                                                  
            }                                                                                        
        }
    }
    else {
        // add all monsters/NPCs
        for (var i = 0; i < this.m_pEvent.monsters.GetCount(); i++)
        {
            var count;
            if (this.m_pEvent.monsters.monsters[i].UseQty == MONSTER_EVENT.meUseQty)
            {
                count = this.m_pEvent.monsters.monsters[i].qty;
            }
      else
            {
                count = Globals.RollDice(this.m_pEvent.monsters.monsters[i].qtyDiceSides,
                    this.m_pEvent.monsters.monsters[i].qtyDiceQty,
                    this.m_pEvent.monsters.monsters[i].qtyBonus);
                Globals.WriteDebugString("Random quantity of combat monster: " + count + "\n");
            }

            var mod = Globals.GetMonsterQtyMod();
            if (mod != 0.0) count += ((mod / 100.0) * count);
            if (count <= 0) count = 1;

            for (var j = 0; j < count; j++)
            {
                Globals.TRACE(Globals.timeGetTime() + " countLoop\n");  // PORT NOTE:  Left out

                // All of this is a duplicate of code above so that we add the combatant to the combat      
                // before updating its properties.  This is so that any scripts that reference              
                // the combatant will find him actually a part of the combat.                               
                temp = new COMBATANT();     // PORT NOTE:  I don't know how this was only temp.Clear() here, but it was.  Should have had multiple copies of the same combatant object in the m_aCombatants array...                                                                     
                temp.self = this.m_iNumCombatants; // init relies on self index                             
                
                if (this.NumCombatants() < MaxMonstersAllowed)                                               
                {                                                                                       
                    var pCombatant;                                                               
                    this.m_aCombatants.push(temp); // PORT NOTE: Was this.m_aCombatants.SetAt(this.m_iNumCombatants, temp);
                    Globals.TRACE(Globals.timeGetTime() + " after SetAt\n");
                    this.m_iNumCombatants++;
                    pCombatant = this.m_aCombatants[this.m_iNumCombatants - 1];
                    if (this.m_pEvent.monsters.monsters[i].m_type == MONSTER_TYPE)                             
                    {
                        pCombatant.InitFromMonsterDataID(this.m_pEvent.monsters.monsters[i].monsterID,          
                            this.m_pEvent.monsters.monsters[i].friendly,          
                            this.m_pEvent.monsters.monsters[i].items,             
                            this.m_pEvent.monsters.monsters[i].money);            
                        Globals.TRACE(Globals.timeGetTime() + " after InitFromMonsterData\n");
                    }
                    else                                                                                 
                        pCombatant.InitFromNPCData(this.m_pEvent.monsters.monsters[i].characterID,            
                            this.m_pEvent.monsters.monsters[i].friendly,               
                            this.m_pEvent.monsters.monsters[i].items,                  
                            this.m_pEvent.monsters.monsters[i].money);                 
                    
                    if ((pCombatant.origKey != NO_DUDE)                                                              
                        && (pCombatant.m_pCharacter.monsterID != ""))                                       
                    {
                        pCombatant.SetMorale(pCombatant.GetMorale() + this.m_pEvent.monsterMorale);                                           
                    }                                                                                    
                    else {
                        var pCharacter;
                        pCharacter = pCombatant.m_pCharacter;
                        if (pCharacter != null) {
                            pCombatant.m_pCharacter.m_pCombatant = null;
                            pCombatant.m_pCharacter = null;
                            delete pCharacter;
                        };
                        m_iNumCombatants--;
                    };
                }                                                                                      
            }; // for (j)
        }; //for (i)
    }
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
        if (!UAFUtil.IsEmpty(scriptResult)) {
            if (scriptResult.GetLength() == partyIndoorCombatArrangement.length)
            {
                partyArrangement = scriptResult;
            }
      else
            {
                if (!debugStrings.AlreadyNoted(CString("PASWS"))) {
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
                // PORT NOTE: I think F = Front, R = Right, L = Left, B = Back
                case 0: // Headed North
                    monsterArrangement.dx['F'.charCodeAt(0) - 'A'.charCodeAt(0)] = -1;
                    monsterArrangement.dx['R'.charCodeAt(0) - 'A'.charCodeAt(0)] = 1;
                    monsterArrangement.dx['L'.charCodeAt(0) - 'A'.charCodeAt(0)] = -1;
                    monsterArrangement.dx['B'.charCodeAt(0) - 'A'.charCodeAt(0)] = 1;
                    monsterArrangement.dy['F'.charCodeAt(0) - 'A'.charCodeAt(0)] = -1;
                    monsterArrangement.dy['R'.charCodeAt(0) - 'A'.charCodeAt(0)] = 0;
                    monsterArrangement.dy['L'.charCodeAt(0) - 'A'.charCodeAt(0)] = 0;
                    monsterArrangement.dy['B'.charCodeAt(0) - 'A'.charCodeAt(0)] = 1;
                    break;
                case 1: // Headed East
                    monsterArrangement.dx['F'.charCodeAt(0) - 'A'.charCodeAt(0)] = 1;
                    monsterArrangement.dx['R'.charCodeAt(0) - 'A'.charCodeAt(0)] = 1;
                    monsterArrangement.dx['L'.charCodeAt(0) - 'A'.charCodeAt(0)] = -1;
                    monsterArrangement.dx['B'.charCodeAt(0) - 'A'.charCodeAt(0)] = -1;
                    monsterArrangement.dy['F'.charCodeAt(0) - 'A'.charCodeAt(0)] = 0;
                    monsterArrangement.dy['R'.charCodeAt(0) - 'A'.charCodeAt(0)] = 1;
                    monsterArrangement.dy['L'.charCodeAt(0) - 'A'.charCodeAt(0)] = -1;
                    monsterArrangement.dy['B'.charCodeAt(0) - 'A'.charCodeAt(0)] = 0;
                    break;
                case 2: // Headed south
                    monsterArrangement.dx['F'.charCodeAt(0) - 'A'.charCodeAt(0)] = 1;
                    monsterArrangement.dx['R'.charCodeAt(0) - 'A'.charCodeAt(0)] = -1;
                    monsterArrangement.dx['L'.charCodeAt(0) - 'A'.charCodeAt(0)] = 1;
                    monsterArrangement.dx['B'.charCodeAt(0) - 'A'.charCodeAt(0)] = -1;
                    monsterArrangement.dy['F'.charCodeAt(0) - 'A'.charCodeAt(0)] = 1;
                    monsterArrangement.dy['R'.charCodeAt(0) - 'A'.charCodeAt(0)] = 0;
                    monsterArrangement.dy['L'.charCodeAt(0) - 'A'.charCodeAt(0)] = 0;
                    monsterArrangement.dy['B'.charCodeAt(0) - 'A'.charCodeAt(0)] = -1;
                    break;
                case 3: // Headed West
                    monsterArrangement.dx['F'.charCodeAt(0) - 'A'.charCodeAt(0)] = -1;
                    monsterArrangement.dx['R'.charCodeAt(0) - 'A'.charCodeAt(0)] = -1;
                    monsterArrangement.dx['L'.charCodeAt(0) - 'A'.charCodeAt(0)] = 1;
                    monsterArrangement.dx['B'.charCodeAt(0) - 'A'.charCodeAt(0)] = 1;
                    monsterArrangement.dy['F'.charCodeAt(0) - 'A'.charCodeAt(0)] = 0;
                    monsterArrangement.dy['R'.charCodeAt(0) - 'A'.charCodeAt(0)] = -1;
                    monsterArrangement.dy['L'.charCodeAt(0) - 'A'.charCodeAt(0)] = 1;
                    monsterArrangement.dy['B'.charCodeAt(0) - 'A'.charCodeAt(0)] = 0;
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
            switch (this.monsterDistance) {

                /**TODO: This should actually call the CombatPlacement script once we get GDPL compilation working 
                 $IF($GET_PARTY_FACING() =#2){$MonsterPlacement("FbPV500E");}
                    -$ELSE{$MonsterPlacement("bPV500E");};
                 $IF($GET_PARTY_FACING() =#2){$MonsterPlacement("17FbPV500E");}
                    -$ELSE{$MonsterPlacement("16FbPV500E");};
                 $IF($GET_PARTY_FACING() =#2){$MonsterPlacement("10FbPV500E");}
                   -$ELSE{$MonsterPlacement("9FbPV500E");};
                 */
                case eventDistType.UpClose:
                    this.MonsterPlacementCallback("FbPV500E");
                    //SPECAB.RunGlobalScript("CombatPlacement", "PlaceMonsterClose", true);  // Callback function immediately below.
                    break;
                case eventDistType.Nearby:
                    this.MonsterPlacementCallback("10FbPV500E");
                    //SPECAB.RunGlobalScript("CombatPlacement", "PlaceMonsterNear", true);  // Callback function immediately below.
                    break;
                case eventDistType.FarAway:
                    this.MonsterPlacementCallback("10FbPV500E");
                    //SPECAB.RunGlobalScript("CombatPlacement", "PlaceMonsterFar", true);  // Callback function immediately below.
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
    queue = []; for (var idx = 0; idx < Drawtile.MAX_TERRAIN_WIDTH * Drawtile.MAX_TERRAIN_HEIGHT; idx++) { queue[idx] = 0; }
    monsterArrangement.distanceFromParty = []; for (var idx = 0; idx < Drawtile.MAX_TERRAIN_WIDTH * Drawtile.MAX_TERRAIN_HEIGHT; idx++) { queue[idx] = 0; }
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
                    if (OBSTICAL_TYPE.OBSTICAL_none != Drawtile.ObsticalType(pDude.x, pDude.y, pDude.width, pDude.height, true, ignoreCurrentCombatant, null))
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

    if (OBSTICAL_TYPE.OBSTICAL_none != Drawtile.ObsticalType(pDude.x, pDude.y, pDude.width, pDude.height, true, ignoreCurrentCombatant, null))
        Globals.WriteDebugString("getNextCharCombatPos() returns a cell containing a wall\n");
    else
        Globals.TRACE("Placing char at combat pos " + pDude.x + "," + pDude.y + "\n");
}


COMBAT_DATA.prototype.PlaceCharacter = function(pDude, x, y, ignoreCurrentCombatant) {
    if (OBSTICAL_TYPE.OBSTICAL_none == Drawtile.ObsticalType(x, y, pDude.width, pDude.height, true, ignoreCurrentCombatant, null)) {
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

  

var COMBAT_DATA_currDir = null; // PORT NOTE: originally a static int variable

COMBAT_DATA.prototype.getNextMonsterCombatDirection = function(i, reset) {
  // monsterPartyX is postion of party as seen by the monsters.
  // dir is direction of party from first monster.

    if (COMBAT_DATA_currDir == null) { COMBAT_DATA_currDir = eventDirType.Any; }  // PORT NOTE:  Initializing static - could not do it in declaration outside of this method because eventDirType is not defined yet
    var currDir = COMBAT_DATA_currDir;

    lastDir = 0;

    if (reset) {
        reset = false;
        currDir = eventDirType.Any;
    };

    var facing;
    lastDir = currDir;
    var obj = this.GetNextDirection(currDir, facing); currDir = obj.pCurrDir;  facing = obj.pFacing;   //PORT NOTE:  Dealing with C++ pass by reference parameters

    var dir = -1;


    // calling findEmptyCell() will search for empty cell
    // in expanding circle.
    // try to find something in direction away from party first
    switch (currDir) {
        case eventDirType.North: dir = 0; monsterArrangement.numMonster[0]++; break;
        case eventDirType.East: dir = 1; monsterArrangement.numMonster[1]++; break;
        case eventDirType.South: dir = 2; monsterArrangement.numMonster[2]++; break;
        case eventDirType.West: dir = 3; monsterArrangement.numMonster[3]++; break;
        default:
            Globals.ASSERT(true, "currDir not found");
            dir = PATH_DIR.PathNorth;
            // PORT NOTE:  Might be a bug in here that monsterArrangement.numMonster[0] needs to go upsomehow
    };
    monsterArrangement.monsterPlacement[i].directionFromParty = dir;
    monsterArrangement.monsterPlacement[i].facing = facing;

}



COMBAT_DATA.prototype.GetNextDirection = function(pCurrDir, pFacing) {
    // Get the next direction to place monsters in
    switch (this.m_pEvent.direction) {
        case eventDirType.Any:
            switch (pCurrDir) {
                case eventDirType.North:
        pCurrDir = eventDirType.East;
                    break;
                case eventDirType.East:
        pCurrDir = eventDirType.South;
                    break;
                case eventDirType.South:
        pCurrDir = eventDirType.West;
                    break;
                case eventDirType.West:
        pCurrDir = eventDirType.North;
                    break;
                default:
                    switch (party.GetPartyFacing()) {
                        case PATH_DIR.FACE_NORTH:
          pCurrDir = eventDirType.North;
                            break;
                        case PATH_DIR.FACE_EAST:
          pCurrDir = eventDirType.East;
                            break;
                        case PATH_DIR.FACE_SOUTH:
          pCurrDir = eventDirType.South;
                            break;
                        case PATH_DIR.FACE_WEST:
          pCurrDir = eventDirType.West;
                            break;
                    }
                    break;
            }
            break;

        case eventDirType.InFront:
            switch (party.GetPartyFacing()) {
                case PATH_DIR.FACE_NORTH:
        pCurrDir = eventDirType.North;
                    break;
                case PATH_DIR.FACE_EAST:
        pCurrDir = eventDirType.East;
                    break;
                case PATH_DIR.FACE_SOUTH:
        pCurrDir = eventDirType.South;
                    break;
                case PATH_DIR.FACE_WEST:
        pCurrDir = eventDirType.West;
                    break;
            }
            break;

        case eventDirType.N_W_E:
            if (pCurrDir == eventDirType.West)
         pCurrDir = eventDirType.North;
      else if (pCurrDir == eventDirType.North)
         pCurrDir = eventDirType.East;
      else
         pCurrDir = eventDirType.West;
            break;
        case eventDirType.North:
      pCurrDir = eventDirType.North;
            break;
        case eventDirType.W_S_E:
            if (pCurrDir == eventDirType.East)
         pCurrDir = eventDirType.South;
      else if (pCurrDir == eventDirType.South)
         pCurrDir = eventDirType.West;
      else
         pCurrDir = eventDirType.East;
            break;
        case eventDirType.South:
      pCurrDir = eventDirType.South;
            break;
        case eventDirType.N_S_E:
            if (pCurrDir == eventDirType.North)
         pCurrDir = eventDirType.East;
      else if (pCurrDir == eventDirType.East)
         pCurrDir = eventDirType.South;
      else
         pCurrDir = eventDirType.North;
            break;
        case eventDirType.East:
      pCurrDir = eventDirType.East;
            break;
        case eventDirType.N_S_W:
            if (pCurrDir == eventDirType.South)
         pCurrDir = eventDirType.West;
      else if (pCurrDir == eventDirType.West)
         pCurrDir = eventDirType.North;
      else
         pCurrDir = eventDirType.South;
            break;
        case eventDirType.West:
      pCurrDir = eventDirType.West;
            break;
        case eventDirType.N_S:
            if (pCurrDir == eventDirType.North)
         pCurrDir = eventDirType.South;
      else
         pCurrDir = eventDirType.North;
            break;
        case eventDirType.N_E:
            if (pCurrDir == eventDirType.North)
         pCurrDir = eventDirType.East;
      else
         pCurrDir = eventDirType.North;
            break;
        case eventDirType.N_W:
            if (pCurrDir == eventDirType.West)
         pCurrDir = eventDirType.North;
      else
         pCurrDir = eventDirType.West;
            break;
        case eventDirType.S_E:
            if (pCurrDir == eventDirType.East)
         pCurrDir = eventDirType.South;
      else
         pCurrDir = eventDirType.East;
            break;
        case eventDirType.S_W:
            if (pCurrDir == eventDirType.South)
         pCurrDir = eventDirType.West;
      else
         pCurrDir = eventDirType.South;
            break;
        case eventDirType.E_W:
            if (pCurrDir == eventDirType.East)
         pCurrDir = eventDirType.West;
      else
         pCurrDir = eventDirType.East;
            break;
    }

    switch (pCurrDir) {
        case eventDirType.North:
     pFacing = PATH_DIR.FACE_WEST;
            Globals.TRACE("Placing monster NORTH of party\n");
            break;
        case eventDirType.East:
     pFacing = PATH_DIR.FACE_WEST;
            Globals.TRACE("Placing monster EAST of party\n");
            break;

        case eventDirType.South:
     pFacing = PATH_DIR.FACE_EAST;
            Globals.TRACE("Placing monster SOUTH of party\n");
            break;
        case eventDirType.West:
     pFacing = PATH_DIR.FACE_EAST;
            Globals.TRACE("Placing monster WEST of party\n");
            break;
    }

    // PORT NOTE:  Dealing with C++ pass by reference parameters
    var returnObj = {};
    returnObj.pCurrDir = pCurrDir;
    returnObj.pFacing = pFacing;
    return returnObj;
}


var COMBAT_DATA_errorMessage = false;
var COMBAT_DATA_activeMessage = false;
COMBAT_DATA.prototype.MonsterPlacementCallback = function(turtlecode) {
    var result = "";

    var errorMessage = COMBAT_DATA_errorMessage;  // PORT NOTE:  Originally a static variable
    var i = 0, n = 0, repeat = 0;
    var activeMessage = COMBAT_DATA_activeMessage; // PORT NOTE:  Originally a static variable;

    if (!monsterArrangement.active) {
        if (!activeMessage) {
            activeMessage = true;
            Globals.WriteDebugString("GPDL function $MonsterPlacement() called other than in response to MonsterPlacement hook\n");
        };
        return "0";
    };
    n = turtlecode.length;
    for (i = 0; i < n; i++) {
        var c = turtlecode.charAt(i);
        switch (c) {
            case 'V':
                monsterArrangement.lineOfSight = true;
                repeat = 0;
                break;
            case 'S':
                {
                    if (monsterArrangement.currentMonster < 0) this.FindCurrentMonsterToPlace();
                    if (monsterArrangement.currentMonster >= 0) {
                        var pCombatant;
                        pCombatant = combatData.GetCombatant(monsterArrangement.currentMonster);
                        monsterArrangement.turtleWidth = pCombatant.width;
                        monsterArrangement.turtleHeight = pCombatant.height;
                    };
                };
                repeat = 0;
                break;
            case '?':
                {
                    var ot = new OBSTICAL_TYPE();
                    if (monsterArrangement.currentMonster < 0) this.FindCurrentMonsterToPlace();
                    if (monsterArrangement.currentMonster < 0) {
                        result += '0';
                    }
                    else {
                        var oc;
                        ot = ObsticalType(monsterArrangement.turtleX + monsterArrangement.partyX,
                            monsterArrangement.turtleY + monsterArrangement.partyY,
                            monsterArrangement.turtleWidth,
                            monsterArrangement.turtleHeight,
                            true,
                            true,
                            null);
                        switch (ot) {
                            case OBSTACLE_TYPE.OBSTICAL_wall: oc = 'w'; break;
                            case OBSTACLE_TYPE.OBSTICAL_lingeringSpell: oc = 's'; break;
                            case OBSTACLE_TYPE.OBSTICAL_occupied: oc = 'o'; break;
                            case OBSTACLE_TYPE.OBSTICAL_offMap: oc = 'i'; break;
                            default: oc = 'n'; break;
                        };
                        result += oc;
                    };
                };
                repeat = 0;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                repeat = 10 * repeat + (c.charCodeAt(0) - '0'.charCodeAt(0));
                break;
            case 'L':
            case 'F':
            case 'R':
            case 'B':
                if (repeat == 0) repeat = 1;
                monsterArrangement.turtleX += repeat * monsterArrangement.dx[c.charCodeAt(0) - 'A'.charCodeAt(0)];
                monsterArrangement.turtleY += repeat * monsterArrangement.dy[c.charCodeAt(0) - 'A'.charCodeAt(0)];
                repeat = 0;
                break;
            case 'P':
                {
                    // Is there an unplaced monster in this direction?
                    var m;
                    m = monsterArrangement.currentMonster;
                    if (m < 0) this.FindCurrentMonsterToPlace();
                    m = monsterArrangement.currentMonster;
                    if (m < 0) {
                        result += '0';
                    }
                    else {
                        this.PlantCombatant(monsterArrangement.turtleX, monsterArrangement.turtleY);
                    };
                };
                repeat = 0;
                break;
            case 'b':
                {
                    switch (monsterArrangement.curDirFromParty) {
                        case 0: // North
                            monsterArrangement.limitMaxY = monsterArrangement.turtleY;
                            break;
                        case 1: // East
                            monsterArrangement.limitMinX = monsterArrangement.turtleX - monsterArrangement.turtleY;
                            break;
                        case 2: // South
                            monsterArrangement.limitMinY = monsterArrangement.turtleY;
                            break;
                        case 3: // West
                            monsterArrangement.limitMaxX = monsterArrangement.turtleX - monsterArrangement.turtleY;
                            break;
                    };
                };
                repeat = 0;
                break;
            case 'f':
                {
                    switch (monsterArrangement.curDirFromParty) {
                        case 0: // North
                            monsterArrangement.limitMinY = monsterArrangement.turtleY;
                            break;
                        case 1: // East
                            monsterArrangement.limitMaxX = monsterArrangement.turtleX - monsterArrangement.turtleY;
                            break;
                        case 2: // South
                            monsterArrangement.limitMaxY = monsterArrangement.turtleY;
                            break;
                        case 3: // West
                            monsterArrangement.limitMinX = monsterArrangement.turtleX - monsterArrangement.turtleY;
                            break;
                    };
                };
                repeat = 0;
                break;
            case 'l':  // 
                {
                    switch (monsterArrangement.curDirFromParty) {
                        case 0: // North
                            monsterArrangement.limitMinX = monsterArrangement.turtleX - monsterArrangement.turtleY;
                            break;
                        case 1: // East
                            monsterArrangement.limitMinY = monsterArrangement.turtleY;
                            break;
                        case 2: // South
                            monsterArrangement.limitMaxX = monsterArrangement.turtleX - monsterArrangement.turtleY;
                            break;
                        case 3: // West
                            monsterArrangement.limitMaxY = monsterArrangement.turtleY;
                            break;
                    };
                };
                repeat = 0;
                break;
            case 'r':
                {
                    switch (monsterArrangement.curDirFromParty) {
                        case 0: // North
                            monsterArrangement.limitMaxX = monsterArrangement.turtleX - monsterArrangement.turtleY;
                            break;
                        case 1: // East
                            monsterArrangement.limitMaxY = monsterArrangement.turtleY;
                            break;
                        case 2: // South
                            monsterArrangement.limitMinX = monsterArrangement.turtleX - monsterArrangement.turtleY;
                            break;
                        case 3: // West
                            monsterArrangement.limitMinY = monsterArrangement.turtleY;
                            break;
                    };
                };
                repeat = 0;
                break;
            case 'd':
                {
                    var j = 0, dx = 0, dy = 0, d2 = 0;
                    monsterArrangement.distance = UAFUtil.ByteFromHexString("0x7fffffff");
                    for (j = 0; j < monsterArrangement.numParty; j++) {
                        dx = monsterArrangement.turtleX - monsterArrangement.partyPositions[j].x;
                        dy = monsterArrangement.turtleY - monsterArrangement.partyPositions[j].x;
                        d2 = dx * dx + dy * dy;
                        if (d2 < monsterArrangement.distance) {
                            monsterArrangement.distance = d2;
                        };
                    };
                };
                repeat = 0;
                break;
            case 'E':
                {
                    var radius = 0, maxRadius = 0;
                    var cellsAttempted;
                    var cellAttIndx = 0, maxCells = 0;
                    maxRadius = Drawtile.MAX_TERRAIN_WIDTH;
                    if (maxRadius > Drawtile.MAX_TERRAIN_HEIGHT) maxRadius = Drawtile.MAX_TERRAIN_HEIGHT;
                    maxRadius = maxRadius / 4;
                    maxCells = (2 * maxRadius + 1) * (2 * maxRadius + 1);
                    cellsAttempted = [];
                    cellAttIndx = 2 * maxRadius * (maxRadius + 1);
                    if (cellsAttempted == null) {
                        Globals.die(0x439805);
                    };
                    for (var idx = 0; idx < maxCells; idx++) { cellsAttempted[idx] = 0; } // PORT NOTE: takes the place of memset(cellsAttempted,0,maxCells);
                    this.FindCurrentMonsterToPlace();
                    if (repeat == 0) repeat = 1;
                    if (monsterArrangement.currentMonster >= 0) {
                        for (radius = 0;
                            radius <= maxRadius;
                            cellAttIndx -= (2 * radius + 2) * (2 * maxRadius + 1) + 1, radius++)  // Gotta impose some limit!
                        {
                            cellAttIndx = Math.floor(cellAttIndx); // PORT NOTE:  C++ just cast to an int, floor works as JS equivalent as long as we stay on positive integers
                            var x = 0, y = 0, left = 0, right = 0, top = 0, bottom = 0;
                            left = monsterArrangement.turtleX - radius;
                            right = monsterArrangement.turtleX + radius;
                            top = monsterArrangement.turtleY - radius;
                            bottom = monsterArrangement.turtleY + radius;
                            for (x = left; x <= right; x++, cellAttIndx += 2 * (maxRadius - radius)) {
                                for (y = top; y <= bottom; y++, cellAttIndx++) {
                                    var dx = 0, dy = 0;
                                    if (monsterArrangement.currentMonster < 0) break;
                                    if (repeat <= 0) break;
                                    dx = x - monsterArrangement.turtleX;
                                    dy = y - monsterArrangement.turtleY;
                                    if (dx * dx + dy * dy > radius * radius) continue;
                                    if (cellsAttempted[cellAttIndx] != 0) continue;
                                    cellsAttempted[cellAttIndx] = 1;
                                    if (this.PlantCombatant(x, y)) {
                                        repeat--;
                                        this.FindCurrentMonsterToPlace(); // See if there is another.
                                    };
                                };
                            };
                        };
                    };
                    delete cellsAttempted;
                };
                repeat = 0;
                break;
            case 'w':
                switch (monsterArrangement.curDirFromParty) {
                    case 0:
                        MoveTurtleY(monsterArrangement.partyMinY); break;
                        break;
                    case 1:
                        MoveTurtleX(monsterArrangement.partyMaxX); break;
                        break;
                    case 2:
                        MoveTurtleY(monsterArrangement.partyMaxY); break;
                        break;
                    case 3:
                        MoveTurtleX(monsterArrangement.partyMinX); break;
                        break;
                }
                break;
            case 'n':
                switch (monsterArrangement.curDirFromParty) {
                    case 0:
                        MoveTurtleX(monsterArrangement.partyMaxY); break;
                        break;
                    case 1:
                        MoveTurtleX(monsterArrangement.partyMinX); break;
                        break;
                    case 2:
                        MoveTurtleY(monsterArrangement.partyMinY); break;
                        break;
                    case 3:
                        MoveTurtleX(monsterArrangement.partyMaxX); break;
                        break;
                }
                break;
            case 'p':
                switch (monsterArrangement.curDirFromParty) {
                    case 0:
                        MoveTurtleX(monsterArrangement.partyMinX); break;
                        break;
                    case 1:
                        MoveTurtleY(monsterArrangement.partyMinY); break;
                        break;
                    case 2:
                        MoveTurtleX(monsterArrangement.partyMaxX); break;
                        break;
                    case 3:
                        MoveTurtleY(monsterArrangement.partyMaxY); break;
                        break;
                }
                break;
            case 's':
                switch (monsterArrangement.curDirFromParty) {
                    case 0:
                        MoveTurtleX(monsterArrangement.partyMaxX); break;
                        break;
                    case 1:
                        MoveTurtleY(monsterArrangement.partyMaxY); break;
                        break;
                    case 2:
                        MoveTurtleX(monsterArrangement.partyMinX); break;
                        break;
                    case 3:
                        MoveTurtleY(monsterArrangement.partyMinY); break;
                        break;
                }
                break;
            case 'u':
                monsterArrangement.turtleStack[3] = monsterArrangement.turtleStack[1];
                monsterArrangement.turtleStack[2] = monsterArrangement.turtleStack[0];
                monsterArrangement.turtleStack[0] = monsterArrangement.turtleX;
                monsterArrangement.turtleStack[1] = monsterArrangement.turtleY;
                break;
            case 'o':
                monsterArrangement.turtleX = monsterArrangement.turtleStack[0];
                monsterArrangement.turtleY = monsterArrangement.turtleStack[1];
                monsterArrangement.turtleStack[0] = monsterArrangement.turtleStack[2];
                monsterArrangement.turtleStack[1] = monsterArrangement.turtleStack[3];
                monsterArrangement.turtleStack[3] = 0;
                monsterArrangement.turtleStack[2] = 0;
                break;
            default:
                if (!errorMessage) {
                    WriteDebugString("Illegal command byte in $MonsterPlacement() code = %s\n", turtlecode);
                    errorMessage = true;
                };
                result += 'e';
                repeat = 0;
                break;
        };

    };

    return result;

}

COMBAT_DATA.prototype.FindCurrentMonsterToPlace = function () {
    var i = 0, n = 0;
    n = combatData.NumCombatants();
    monsterArrangement.turtleWidth = 1;
    monsterArrangement.turtleHeight = 1;
    for (i = 0; i < n; i++) {
        if ((monsterArrangement.monsterPlacement[i].directionFromParty == monsterArrangement.curDirFromParty)
            && (monsterArrangement.monsterPlacement[i].placeX < 0)) {
            monsterArrangement.currentMonster = i;
            return;
        };
    };
    monsterArrangement.currentMonster = -1;
}

COMBAT_DATA.prototype.PlantCombatant = function(partyRelativeX, partyRelativeY) {
    var n;
    var pCombatant;
    n = monsterArrangement.currentMonster;
    pCombatant = combatData.GetCombatant(n);
    if (!this.CanPlaceMonsterHere(partyRelativeX + monsterArrangement.partyX,
        partyRelativeY + monsterArrangement.partyY,
        pCombatant.width,
        pCombatant.height)) {
        return false;
    };

    if (partyRelativeX - partyRelativeY < monsterArrangement.limitMinX) return false;
    if (partyRelativeX - partyRelativeY > monsterArrangement.limitMaxX) return false;
    if (partyRelativeY < monsterArrangement.limitMinY) return false;
    if (partyRelativeY > monsterArrangement.limitMaxY) return false;
    if (monsterArrangement.distance >= 0) {
        var i = 0, dx = 0, dy = 0;
        for (i = 0; i < monsterArrangement.numParty; i++) {
            dx = partyRelativeX - monsterArrangement.partyPositions[i].x;
            dy = partyRelativeY - monsterArrangement.partyPositions[i].y;
            if (dx * dx + dy * dy < monsterArrangement.distance) {
                return false;
            };
        };
    };
    if (monsterArrangement.lineOfSight) {
        if (!this.WithinSight(partyRelativeX, partyRelativeY)) {
            return false;
        };
    };
    pCombatant.x = combatData.m_aCombatants[n].x = partyRelativeX + monsterArrangement.partyX;   // PORT NOTE:  pCombatant is a pointer to the entry in the m_aCombatants array, so I have to set it here in JS - original code was pCombatant.x 
    pCombatant.y = combatData.m_aCombatants[n].y = partyRelativeY + monsterArrangement.partyY;
    Drawtile.placeCombatant(partyRelativeX + monsterArrangement.partyX,
        partyRelativeY + monsterArrangement.partyY,
        n,
        pCombatant.width,
        pCombatant.height);
    monsterArrangement.monsterPlacement[n].placeX = partyRelativeX + monsterArrangement.partyX;
    monsterArrangement.monsterPlacement[n].placeY = partyRelativeY + monsterArrangement.partyY;
    monsterArrangement.currentMonster = -1;
    return true;
}

COMBAT_DATA.prototype.CanPlaceMonsterHere = function(x, y, w, h)
    // See 'findEmptyCell' for the source of this algorithm
{
    if (OBSTICAL_TYPE.OBSTICAL_none != Drawtile.ObsticalType(x, y, w, h, true, true, null)) {
        return false;
    };
    return true;  // For now......no additional checks for distance, etc.
}


COMBAT_DATA.prototype.WithinSight = function(partyRelativeX, partyRelativeY)
{
    //Is the turtle within sight of any party member or previously placed monster?
    var tx = 0, ty = 0;  //Turtle position.  Save some typing.
    var i = 0, n = 0;
    tx = monsterArrangement.partyX + partyRelativeX;
    ty = monsterArrangement.partyY + partyRelativeY;
    n = combatData.NumCombatants();
    for (i = 0; i < n; i++) {
        var pMP;
        pMP = monsterArrangement.monsterPlacement[i];
        if (pMP.placeX > 0) {
            if (Drawtile.IsLineOfSight(tx,
                ty,
                pMP.placeX,
                pMP.placeY)) {
                return true;
            };
        };
    };
    return false;
}

COMBAT_DATA.prototype.PlayCombatMove = function()
{
    if (!Globals.PlaySoundEffects) return;
    if (!SoundMgr.ValidSound(this.m_hCharMoveSound))
        this.m_hCharMoveSound = SoundMgr.LoadSound(this.m_pEvent.MoveSound);
    if (!SoundMgr.PlaySound(this.m_hCharMoveSound))
        this.m_hCharMoveSound = -1;
}

COMBAT_DATA.prototype.PlaceCursorOnCurrentDude = function (ForceCenter) {
    var dude = this.GetCurrCombatant();
    if ((dude < 0) || (dude >= this.m_iNumCombatants)) return;
    if ((this.m_iCursorX != this.m_aCombatants[dude].x)
        || (this.m_iCursorY != this.m_aCombatants[dude].y)
    ) {
        this.m_iCursorX = this.m_aCombatants[dude].x;
        this.m_iCursorY = this.m_aCombatants[dude].y;
        Drawtile.EnsureVisibleTarget(dude, ForceCenter);
    };
}

COMBAT_DATA.prototype.getCombatantPtr = function(index) {
    if ((index < 0) || (index >= this.m_iNumCombatants)) {
        return null;
    }
    return this.m_aCombatants[index];
}


COMBAT_DATA.prototype.IsValidTarget = function(pAttacker, pTarget, targetValidity) {
    var hookParameters = new HOOK_PARAMETERS();
    var scriptContext = new SCRIPT_CONTEXT();
    var result = "";
    var answer = 0;
    if ((targetValidity != null)
        && targetValidity >= 0) return pAttacker.targetValidity == 1;

    scriptContext.SetCombatantContext(pTarget);
    scriptContext.SetAttackerContext(pAttacker);
    scriptContext.SetTargetContextCombatant(pTarget);

    result = pTarget.RunCombatantScripts(
        SPECAB.IS_VALID_TARGET,
        SPECAB.ScriptCallback_LookForChar,
        "YN",
        "Determine if valid target");
    answer = 1;
    if (!UAFUtil.IsEmpty(result == null)) {
        if (result[0] == 'N') answer = 0;
    }
    if (targetValidity != null) targetValidity = answer;
    return { answer: (answer == 1), targetValidity: targetValidity };
}

COMBAT_DATA.prototype.UpdateCombat = function () {
    var isUpdate = false;

    if (this.QComb.StartOfTurn() || this.QComb.RestartInterruptedTurn()) {
        this.QComb.NotStartOfTurn();
        return;
    };



    this.m_bStartingNewCombatRound = false;

    var dude = this.GetCurrCombatant();

    if ((dude == NO_DUDE)
        || this.m_aCombatants[dude].IsDone(
            (this.QComb.NumFreeAttacks() + this.QComb.NumGuardAttacks()) > 0,
            "Updating combat state"
        )
    ) {
        this.getNextCombatant();
        dude = this.GetCurrCombatant();

        if (dude == NO_DUDE) {
            this.StartNewRound();  // Performs a GetNextCombatant();
            dude = this.GetCurrCombatant();
            if (dude != NO_DUDE) {
                this.m_aCombatants[dude].OnStartCombatantAction();
            };
            this.DetermineIfCombatOver();

            if (this.IsCombatOver()) {
                this.DetermineVictoryExpPoints();
                isUpdate = true;
            };


            return;
        }

        if (dude != NO_DUDE) {
            isUpdate = true;
            this.m_aCombatants[dude].OnStartCombatantAction();
        }
    } // if dude is done with his turn
    else {
    };

    this.DetermineIfCombatOver();  // PRS 24 Oct 2009

    if (this.CheckIdleTime()) {
        // force end to combat if nobody does anything for awhile
        this.m_bCombatOver = true;
    }

    if (this.IsCombatOver()) {
        this.DetermineVictoryExpPoints();
        isUpdate = true;
    }
    else {
        if (this.IsStartOfTurn()) {
        }
        else {
            // there is a delay between combatant updates, if prev combatant is not
            // current combatant     
            if (dude != NO_DUDE) {

                // PRS July 2009 isUpdate |= m_aCombatants[dude].UpdateCombatant();
                this.m_aCombatants[dude].UpdateCombatant();
            };
        };
    };

    isUpdate |= this.m_bNeedUpdate;
    this.m_bNeedUpdate = false;

    return;
}

COMBAT_DATA.prototype.getNextCombatant = function() {
    var dude = 0;

    while ((dude = this.QComb.Top()) != NO_DUDE) {
        this.m_aCombatants[dude].m_isCombatReady = -1; // Force check before any moves
        if (this.m_aCombatants[dude].IsDone(false, "Determine next combatant to take turn")) {
            this.QComb.Pop();
            if (this.QComb.Top() != NO_DUDE) {
                if (this.QComb.DelayedX() >= 0) {
            /* Really */ Globals.NotImplemented(0x34c3, false);
                };
            };
        }
        else {
            Globals.TRACE("getNextCombatant() returns " + dude + " (queued)\n");
            return dude;
        }
    }

    var i = 0;
    var found = false;
    dude = NO_DUDE;

    while ((this.m_iCurrInitiative <= 22) && (!found)) {
        if (pendingSpellList.ProcessTimeSensitiveData(0, this.m_iCurrInitiative)) {
            dude = this.QComb.Top();
            if (dude != NO_DUDE) {
                this.m_aCombatants[dude].m_isCombatReady = -1; // Force check before any moves
                if (this.m_aCombatants[dude].IsDone(false, "Determine next combatant to take turn")) {
                    this.QComb.Pop();
                    if (this.QComb.Top() != NO_DUDE) {
                        if (this.QComb.DelayedX() >= 0) {
                /* Really */ Globals.NotImplemented(0x34c4, false);
                        };
                    };
                }
                else {
                    Globals.TRACE("getNextCombatant() returns " + dude + " (queued) for pending spell.\n");
                    return dude;
                }

            };
        };

        i = 0;
        while ((i < this.m_iNumCombatants) && (!found)) {
            if (this.m_aCombatants[i].m_iInitiative == this.m_iCurrInitiative) {
                this.m_aCombatants[i].m_isCombatReady = -2;  // Value to indicate unknown.
                if (!this.m_aCombatants[i].IsDone(false, "Determine next combatantant to take turn")) {
                    dude = i;
                    found = true;
                };
            };
            i++;
        };

        if (!found)
            this.m_iCurrInitiative++;
    }
    if (dude != NO_DUDE) {
        this.QComb.Push(dude, true, 0, 0);
        this.turningSummary.Clear();
        {
            var j = dude;
            if (this.m_aCombatants[j].State() == individualCombatantState.ICS_ContinueGuarding) {
                this.m_aCombatants[j].State(individualCombatantState.ICS_None);
            };

            {
                /* The calculation of availAttacks has been moved from
                 * StartNewRound() to getNextCombatant().
                 * I did this because Manikus complained that a spell
                 * given during a combat round that increased the number
                 * of attacks on a target did not take effect until the
                 * start of the following round.  PRS 20091215
                 *
                 * The code has been moved back to StartNewRound.
                 * I did this because some things that tak place after
                 * the start-of-round but before the combatant's turn
                 * require that we know the number of available attacks.
                 * In particular, 'Guarding Attacks' and 'Free Attacks'
                 * are based on the number of available attacks.  We will
                 * have to solve the original problem differently.
                 * PRS 20111214
                 *
                 * double leftoverAttacks=m_aCombatants[j].availAttacks;             
                 * m_aCombatants[j].determineAvailAttacks();
                 * double maxAttacks = ceil(m_aCombatants[j].availAttacks);
                 * m_aCombatants[j].availAttacks += leftoverAttacks;
                 * m_aCombatants[j].availAttacks = min(m_aCombatants[j].availAttacks, maxAttacks);
                 * m_aCombatants[j].m_iMovement = 0;      
                 * m_aCombatants[j].m_iNumDiagonalMoves = 0;
                 */
            };
        };


        //PlaceCursorOnCurrentDude();
        Globals.TRACE("getNextCombatant() returns " + dude + "\n");
    }
    else
        Globals.TRACE("getNextCombatant() returns NO_DUDE\n");

    return dude;
}


COMBAT_DATA.prototype.StartNewRound = function() {
    var hookParameters = new HOOK_PARAMETERS();
    var scriptContext = new SCRIPT_CONTEXT();
    this.SetUpdateFlag();

    if (this.m_bLastRound)
        this.m_bCombatOver = true;

    Globals.TRACE("Start new combat round\n");

    // battle rages on
    this.CheckDyingCombatants();
    this.CheckMorale();
    var i;
    for (i = 0; i < this.m_iNumCombatants; i++) {
        var continueGuarding = false;

        {
            scriptContext.SetCharacterContext(this.m_aCombatants[i].m_pCharacter);
            scriptContext.SetCombatantContext(this.m_aCombatants[i]);
            hookParameters[5] = "" + this.m_aCombatants[i].State();

            this.m_aCombatants[i].RunCombatantScripts(SPECAB.START_COMBAT_ROUND,
                SPECAB.ScriptCallback_RunAllScripts,
                null,
                "Starting a new combat round");
            this.m_aCombatants[i].RunCombatantScripts(SPECAB.GUARDING_START_OF_ROUND,
                SPECAB.ScriptCallback_RunAllScripts,
                null,
                "Starting a new combat round");
            continueGuarding = hookParameters[6].indexOf('G') >= 0;
            SPECAB.ClearHookParameters();
            scriptContext.Clear();
        };


        this.m_aCombatants[i].m_isCombatReady = -1; // Force a search of SPECIAL_ABILITIES in IsDone()
        if ((this.m_aCombatants[i].charCanTakeAction()) && (this.m_aCombatants[i].IsDone(false, "Starting a new combat round"))) {
            if (this.m_aCombatants[i].State() != individualCombatantState.ICS_Casting) {
                this.m_aCombatants[i].turnIsDone = fa;se;
                // stay guarding until told otherwise
                if (this.m_aCombatants[i].OnAuto(false)) {
                    if (this.m_aCombatants[i].State() != individualCombatantState.ICS_Guarding)
                        this.m_aCombatants[i].State(individualCombatantState.ICS_None);
                }
                else {
                    if ((this.m_aCombatants[i].State() == individualCombatantState.ICS_Guarding) && continueGuarding) {
                        this.m_aCombatants[i].State(individualCombatantState.ICS_ContinueGuarding);
                    }
                    else {
                        this.m_aCombatants[i].State(individualCombatantState.ICS_None);
                    };
                };
            }
            {
                /* The calculation of availAttacks has been moved from
                 * StartNewRound() to getNextCombatant().
                 * I did this because Manikus complained that a spell
                 * given during a combat round that increased the number
                 * of attacks on a target did not take effect until the
                 * start of the following round.  PRS 20091215
                 *
                 * The code has been moved back to StartNewRound.
                 * I did this because some things that take place after
                 * the start-of-round but before the combatant's turn
                 * require that we know the number of available attacks.
                 * In particular, 'Guarding Attacks' and 'Free Attacks'
                 * are based on the number of available attacks.  We will
                 * have to solve the original problem differently.
                 * PRS 20111214
                 */
                var leftoverAttacks = this.m_aCombatants[i].availAttacks;
                this.m_aCombatants[i].determineNbrAttacks();
                this.m_aCombatants[i].determineAvailAttacks(this.m_aCombatants[i].GetNbrAttacks());
                var maxAttacks = Math.ceil(m_aCombatants[i].availAttacks);
                this.m_aCombatants[i].availAttacks += leftoverAttacks;
                this.m_aCombatants[i].availAttacks = Math.min(this.m_aCombatants[i].availAttacks, maxAttacks);
                this.m_aCombatants[i].m_iMovement = 0;
                this.m_aCombatants[i].m_iNumDiagonalMoves = 0;
            };
        }

        if (this.m_aCombatants[i].charOnCombatMap(false, true)) {
            if (activeSpellList.LingerSpellAffectsTarget(this.m_aCombatants[i].self,
                this.m_aCombatants[i].x,
                this.m_aCombatants[i].y,
                this.m_aCombatants[i].width,
                this.m_aCombatants[i].height)) {
                // A lingering spell is active at the location 
                // this dude is located at.
                TRACE("%i re-targeted by at least one active linger spell at location %i,%i\n",
                    this.m_aCombatants[i].self, this.m_aCombatants[i].x, this.m_aCombatants[i].y);

                activeSpellList.ActivateLingerSpellsOnTarget(this.m_aCombatants[i].self,
                    this.m_aCombatants[i].x,
                    this.m_aCombatants[i].y,
                    this.m_aCombatants[i].width,
                    this.m_aCombatants[i].height);
            };
            this.m_aCombatants[i].m_pCharacter.ProcessLingeringSpellEffects();
        };
        {
            var pos;
            for (pos = this.m_auras.GetHeadPosition(); pos != null;) {
                var n = 0, j = 0;
                var pAURA;
                pAURA = this.m_auras.GetAt(pos);
                this.m_auras.GetNext(pos); pos = this.m_auras.NextPos(pos);    // PORT NOTE:  Manually advancing pointer because no pass-by-reference
                n = pAURA.m_combatants.GetSize();
                for (j = 0; j < n; j++) {
                    if (pAURA.m_combatants[j] == i) {
                        this.PushAuraReference(pAURA.auraID);
                        pAURA.RunAuraScripts(
                            SPECAB.AURA_EFFECT,
                            SPECAB.ScriptCallback_RunAllScripts,
                            null,
                            "Starting a new combat round");
                        this.PopAuraReference();
                        break;
                    }
                }
            }
        }
    }

    {
        var pos;
        var hookParams = new HOOK_PARAMETERS();
        scriptContext.Clear();
        for (pos = this.m_auras.GetHeadPosition(); pos != null;) {
            var pAURA;
            pAURA = this.m_auras.GetAt(pos);
            this.PushAuraReference(pAURA.auraID);
            this.m_auras.GetNext(pos); pos = this.m_auras.NextPos(pos);    // PORT NOTE:  Manually advancing pointer because no pass-by-reference
            pAURA.RunAuraScripts(
                SPECAB.AURA_ROUND,
                SPECAB.ScriptCallback_RunAllScripts,
                null,
                "Starting a new combat round");
            this.PopAuraReference();
        };
    };
    this.m_iCurrInitiative = 1;
    this.m_iCurrRound++;
    if (this.m_iCurrRound % 10 == 0)
        this.m_iCurrTurn++;

    party.incrementClock(1);

    this.DetermineCombatInitiative();

    for (i = 0; i < this.m_iNumCombatants; i++) {
    };

    this.getNextCombatant();

    //PRS 20110223 m_iPrevRndCombatant = GetCurrCombatant();
    this.m_bStartingNewCombatRound = true;
}

COMBAT_DATA.prototype.CheckDyingCombatants = function() {
    for (var i = 0; i < this.m_iNumCombatants; i++)
    {
        if (this.m_aCombatants[i].GetAdjStatus() == individualCombatantState.Dying) {
            if (!this.m_aCombatants[i].isBandaged)
                this.m_aCombatants[i].TakeDamage(1, false, null, false, null);
        }
    }
}

COMBAT_DATA.prototype.CheckMorale = function() {
    for (var i = 0; i < this.m_iNumCombatants; i++)
        this.m_aCombatants[i].CheckMorale();
}

COMBAT_DATA.prototype.getNumCombatants = function(pNumParty, pNumMons)
{
    pNumParty = 0;
    pNumMons = 0;

    // count friendly pc's and npc's in numParty
    // count non-friendly pc's, npc's, and monsters in numMons
    // do not count friendly monsters in numParty

    for (var i = 0; i < this.m_iNumCombatants; i++)
    {
        if (this.m_aCombatants[i].charOnCombatMap(false, true)) {
            if ((this.m_aCombatants[i].IsPartyMember()) || (this.m_aCombatants[i].GetIsFriendly())) pNumParty++;
            else pNumMons++;
        }
    }
    return { pNumParty: pNumParty, pNumMons: pNumMons };
}

COMBAT_DATA.prototype.DetermineCombatInitiative = function() {
    var i;
    for (i = 0; i < this.m_iNumCombatants; i++) {
        this.m_aCombatants[i].RollInitiative(this.m_eSurprise);
    }

    // if one side was initially surprised, it only
    // works for the first round so reset it.
    this.m_eSurprise = eventSurpriseType.Neither;
    {
        var line = "";
        var swap = 0;
        var combatant = []; for (idx = 0; idx < Globals.MAX_COMBATANTS; idx++) { combatant[idx] = 0; }   // PORT NOTE: C++ automatically initializes arrays (by size, anyway)
        for (i = 0; i < this.m_iNumCombatants; i++) {
            combatant[i] = i;
        }
        for (swap = 1; swap != 0;) {
            swap = 0;
            for (i = 0; i < this.m_iNumCombatants - 1; i++) {
                if (this.m_aCombatants[combatant[i]].m_iInitiative > this.m_aCombatants[combatant[i + 1]].m_iInitiative) {
                    var temp;
                    temp = combatant[i];
                    combatant[i] = combatant[i + 1];
                    combatant[i + 1] = temp;
                    swap = 1;
                }
            }
        }
        if (Globals.logDebuggingInfo) {
            line = "Initiatives for round " + this.m_iCurrRound + ": ";
        }
        {
            var scriptContext = new SCRIPT_CONTEXT();
            for (i = 0; i < this.m_iNumCombatants; i++) {
                var hookParameters = new HOOK_PARAMETERS();
                var pCombatant;
                var result = "";
                scriptContext.SetCombatantContext(this.m_aCombatants[i]);
                hookParameters[5] = "" + this.m_aCombatants[i].m_iInitiative;
                hookParameters[6] = "" + this.m_aCombatants[i].GetMorale();
                hookParameters[7] = "" + this.m_pEvent.monsterMorale;
                pCombatant = this.m_aCombatants[i];
                result = pCombatant.RunCombatantScripts(
                    SPECAB.ADJUST_INITIATIVE,
                    SPECAB.ScriptCallback_RunAllScripts,
                    null,
                    "Determine combat initiative");
                result = pCombatant.m_pCharacter.RunCharacterScripts(
                    SPECAB.ADJUST_INITIATIVE,
                    SPECAB.ScriptCallback_RunAllScripts,
                    null,
                    "Determine combat initiative");
                if (pCombatant.GetType() == MONSTER_TYPE) {
                    var monsterID;
                    monsterID = pCombatant.m_pCharacter.monsterID;
                    {
                        var pMonsterData;
                        pMonsterData = monsterData.GetMonster(monsterID);
                        if (pMonsterData != null) {
                            result = pMonsterData.RunMonsterScripts(
                                SPECAB.ADJUST_INITIATIVE,
                                SPECAB.ScriptCallback_RunAllScripts,
                                hookParameters,
                                "Determine combat initiative");
                        }
                    }
                }
                if (!UAFUtil.IsEmpty(result)) {
                    var adj = parseInt(result);
                    if (!isNaN(adj)) {
                        var initiative = parseInt(hookParameters[5]);
                        if (!isNaN(initiative)) {    //  PORT NOTE:  This is a little different in that that sscanf would have returned ANY digit in the result, but it's probably meant to just quickly parse the int
                            this.m_aCombatants[i].m_iInitiative += adj;
                            if (this.m_aCombatants[i].m_iInitiative < INITIATIVE.INITIATIVE_AlwaysFirst)
                            {
                                this.m_aCombatants[i].m_iInitiative = INITIATIVE.INITIATIVE_AlwaysFirst;
                            }
                            if (this.m_aCombatants[i].m_iInitiative > INITIATIVE.INITIATIVE_Never)
                            {
                                this.m_aCombatants[i].m_iInitiative = INITIATIVE.INITIATIVE_Never;
                            }
                        }
                    }
                }
            }
        }
        for (i = 0; i < this.m_iNumCombatants; i++) {
            var initiative = combatant[i] + "[" + this.m_aCombatants[combatant[i]].m_iInitiative + "] ";
            line += initiative;
        }
        if (Globals.logDebuggingInfo) {
            Globals.WriteDebugString(line + '\n');
        }
    }
}

COMBAT_DATA.prototype.DetermineIfCombatOver = function() {
    var dude = this.GetCurrCombatant();
    var numFriendly = 0, numUnfriendly = 0, numPartyMembers = 0;

    {
        var i = 0;
        numFriendly = 0;
        numUnfriendly = 0;
        numPartyMembers = 0;
        for (i = 0; i < this.m_iNumCombatants; i++) {
            var pCombatant;
            pCombatant = this.m_aCombatants[i];
            if (pCombatant.charOnCombatMap(false, false)) {
                if (pCombatant.IsPartyMember()) {
                    numPartyMembers++;
                }
                if (pCombatant.GetIsFriendly()) numFriendly++;
                else numUnfriendly++;
            }
        }
    };

    if (numUnfriendly == 0) {
        // party wins
        this.m_eCombatVictor = combatVictorType.PartyWins;
        this.m_bLastRound = true;
        this.m_bCombatOver = true;
    }
    else if ((numFriendly == 0) || (numPartyMembers == 0)) {
        // monsters win, or party ran away
        this.m_eCombatVictor = combatVictorType.MonsterWins;

        if (dude != NO_DUDE)
            this.m_bLastRound = true;
        else
            this.m_bCombatOver = true;
    }
    else
        this.m_eCombatVictor = combatVictorType.NoWinner;
}

COMBAT_DATA.prototype.IsCombatOver = function () {
    return this.m_bCombatOver;
}



COMBAT_DATA.prototype.CheckIdleTime = function() {
    var minIdleTime = 1000000;
    var diff;
    for (var i = 0; i < this.m_iNumCombatants; i++)
    {
        diff = this.m_iCurrRound - this.m_aCombatants[i].lastAttackRound;
        if (diff < minIdleTime)
            minIdleTime = diff;
    }

    return (minIdleTime > Globals.MAX_COMBAT_IDLE_ROUNDS);
}


// have we changed to the next active combatant
COMBAT_DATA.prototype.IsStartOfTurn = function()
{
    return this.QComb.StartOfTurn() || this.QComb.RestartInterruptedTurn();
}

COMBAT_DATA.prototype.IsFreeAttacker = function() {
    return (this.QComb.Top() != NO_DUDE)
        && !this.QComb.ChangeStats()
        && this.QComb.NumFreeAttacks();
}
