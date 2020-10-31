/** TODO **/
function COMBATANT() {
    this.self = 0;
    this.x = 0, this.y = 0;  // During creation x is the newMonsters index!
    this.hPath = 0;
    this.m_iInitiative = 0; //
    this.scriptPriority = 0;
    this.m_target = 0; // index of target
    this.targetValidity = 0; //-1=unknown; 0=notValid; 1=valid
    this.m_iFacing = 0;
    this.m_iMovement = 0;
    this.m_iMoveDir = 0;
    this.m_iNumDiagonalMoves = 0;  // In this round.
    this.moveX = 0;
    this.moveY = 0;
    this.lastAttackRound = 0;
    this.availAttacks = 0.0;  // double
    this.continueAttack = false; // interrupt a series of multiple attacks?
    this.iFleeingFlags = 0;  // See FLEEING_FLAGS
    this.iAutoFlags = 0;     // See AUTO_FLAGS
    this.isTurned =  false;  // this guy has been turned and is fleeing from the cleric that did it
    this.hasTurnedUndead = false; // only get to turn undead once per combat
    this.m_iLastAttacker = 0; // who tried to hit us most recently?
    this.m_iLastAttacked = 0; // who did we try to hit most recently?
    this.m_eLastAction = 0; // PORT NOTE:  enum type - setting to default of 0
    this.turnIsDone = false;
    this.isBandaged = false;
    this.bandageWho = 0;
    this.didMove = false;
    this.m_ICS = 0; // PORT NOTE:  enum type - setting to default of 0
    this.friendly = false;
    this.m_adjFriendly = 0; // 0=leave alone; 1=friendly; 2=not friendly; 3=toggle friendly
    this.blitDeathTile = 0;
    this.m_spellIDBeingCast = "";
    this.m_secondarySpellIDBeingCast = "";
    this.m_itemSpellIDBeingCast = "";
    this.Wpn_Type = 0; // Not serializedUsed during item spell casting process. // PORT NOTE:  enum type - setting to default of 0
    this.width = 0;
    this.height = 0;
    this.targetPos = 0;  // PORT NOTE:  POSITION, but that is really just a list index
    this.origKey = 0;
    this.combatant_pendingSpellKey = 0;
    this.combatant_activeSpellKey = 0; // -1 in idle state
    this.combatant_spellCastingLevel = 0; //level at which combatant_activeSpellKey was cast
    this.forceAttackPose = false;
    this.m_isCombatReady = 0;
    this.m_spellDamageHP = 0;
    // *******************************
    this.m_useTempHP = false; // During combat spell processing adjustments are made here
    this.m_tempHP = 0;     // rather than to the character's HP.  At the end of spell
    // processing, we transfer the temp value to the character.
    // *******************************
    this.m_specialActionName = "";
    this.m_specialActionIndex = 0;

    if (this.m_pCharacter != null) {
        Globals.die(0xffd21c);
    };
    this.combatantSA = new SPECIAL_ABILITIES();
    this.combatantSA.Clear();
    this.combattargets = new CList();
    this.m_preCombatMorale = 0;
    this.targeters = new CList();
    this.Clear();
    /*
enum INITIATIVE {
    INITIATIVE_AlwaysFirst = 1,
    INITIATIVE_FirstDefault = 9,
    INITIATIVE_LastDefault = 18,
    INITIATIVE_Never = 23,
};*/
}

COMBATANT.prototype.Clear = function () {
    this.m_pCharacter = null;
    this.m_useTempHP = false;
    this.deleteChar = false;
    this.self = -1;
    this.x = -1; this.y = -1;
    this.hPath = -1;
    this.m_iInitiative = 0;
    this.m_target = -1;
    this.targetValidity = -1;
    this.m_iFacing = 0;
    this.m_iMoveDir = 0;
    this.m_iMovement = 0;
    this.m_iNumDiagonalMoves = 0;
    this.moveX = -1; moveY = -1;
    this.lastAttackRound = 0;
    this.availAttacks = 0.0;
    this.continueAttack = false;
    this.turnIsDone = false;
    this.isBandaged = false;
    this.bandageWho = -1;
    this.origKey = -1;
    this.didMove = false;
    this.State(individualCombatantState.ICS_None);
    this.friendly = true;
    this.m_adjFriendly = 0;  // No adjustment
    this.blitDeathTile = false;
    this.width = 0; this.height = 0;
    this.targetPos = null;
    this.iFleeingFlags = 0;
    this.isTurned = false;
    this.hasTurnedUndead = false;
    this.m_iLastAttacker = -1;
    this.m_iLastAttacked = -1;
    this.m_eLastAction = LASTACTION.LA_None;
    this.RemoveAllTargets();
    this.targeters = [];
    this.m_spellIDBeingCast = "";
    this.m_itemSpellIDBeingCast = "";
    this.Wpn_Type = weaponClassType.NotWeapon;
    this.combatant_pendingSpellKey = -1;
    this.combatant_activeSpellKey = -1;
    this.combatant_spellCastingLevel = -1;
    this.forceAttackPose = false;
    this.m_isCombatReady = -1;
    this.m_specialActionName = "";
    this.m_specialActionIndex = -1;
    this.combatantSA = new SPECIAL_ABILITIES();
    this.combatantSA.InsertAbility("Competing", "", "Combatant Constructor ", "");
}

COMBATANT.prototype.GetName = function () {
    return this.m_pCharacter.GetName();
};

COMBATANT.prototype.State = function (ICS) {
    if (this.m_pCharacter != null) {
        //WriteDebugString("DEBUG - COMBATANT(%s)::State(%d)\n", m_pCharacter->GetName(),ICS);
    };
    if ((this.m_ICS != individualCombatantState.ICS_Guarding) && (ICS == individualCombatantState.ICS_Guarding)) {
        Globals.debug(this.m_ICS + " / " + ICS);
        this.EnterGuardingState();
    };
/*
#ifdef TraceFreeAttacks
    if (((m_ICS == ICS_Guarding)
        || (m_ICS == ICS_ContinueGuarding)
        || (ICS == ICS_Guarding)
        || (ICS == ICS_ContinueGuarding)
    )
        && (m_ICS != ICS)
    ) {
        WriteDebugString("TFA - %s enters state %s\n", GetName(), CombatantStateText[ICS]);
    };
#endif */
    this.m_ICS = ICS;
}

COMBATANT.prototype.EnterGuardingState = function () {
    var hookParameters = new HOOK_PARAMETERS();
    var scriptContext = new SCRIPT_CONTEXT();
    scriptContext.SetCombatantContext(this);
    this.RunCombatantScripts(SPECAB.GUARDING_GUARD,
        SPECAB.ScriptCallback_RunAllScripts,
        null,
        "Combatant entering guarding state");
}

COMBATANT.prototype.RunCombatantScripts = function (scriptName, fnc, pkt, comment) {
        return this.combatantSA.RunScripts(scriptName,
            fnc,
            pkt,
            comment,
            SCRIPT_SOURCE_TYPE.ScriptSourceType_Combatant,
//#ifdef newCombatant                           // PORT NOTE: Don't know which to choose
            this.GetName()
//#else
//name
//#endif
            );
};

COMBATANT.prototype.RemoveAllTargets = function () {
    this.combattargets.RemoveAll();
    this.SetCurrTarget();
}

COMBATANT.prototype.SetCurrTarget = function () {
    if (!this.combattargets.IsEmpty()) {
        this.targetPos = this.combattargets.GetHeadPosition();
        this.SetTarget(this.combattargets.GetAtPos(this.targetPos));
    }
    else {
        this.SetTarget(NO_DUDE);
        this.targetPos = null;
    }
}

COMBATANT.prototype.SetTarget = function (t) {
    this.m_target = t; this.targetValidity = -1;
} //Not yet checked


COMBATANT.prototype.StartAttack = function(targ, additionalAttacks) {
    Globals.ASSERT(this.self != NO_DUDE, "Combatant.js::StartAttack");
    this.continueAttack = false;
    if (targ == NO_DUDE) {
        Globals.die(0xab4c5);
        Globals.WriteDebugString("StartAttack() for " + self + " called with invalid target\n");
        return false;
    }
    if (this.availAttacks + this.additionalAttacks <= 0) {
        Globals.die(0xab4c6);
        Globals.WriteDebugString("StartAttack() for " + self + " called with availAttacks=0\n");
        return false;
    }
    if (!this.charOnCombatMap(false, true)) {
        Globals.die(0xab4c7);
        Globals.WriteDebugString("StartAttack() for " + self + " called when combatant not on map\n",);
        return false;
    }
    {
        var pTarget;
        pTarget = this.GetCombatantPtr(targ);

        if (!pTarget.charOnCombatMap(false, true)) {
            return false;
        };
    };
    this.State(ICS_Attacking);
    this.StopCasting(false, false);
    this.EnsureVisibleTarget(targ);
    this.continueAttack = true;
    return true;
}

COMBATANT.prototype.InitFromCharData = function(dude) {
    if (this.m_pCharacter != null) {
        Globals.die(0x6659c);
    }
    this.m_pCharacter = party.characters[dude];
    if (this.m_pCharacter.m_pCombatant != null) {
        Globals.die(0x5cbff);
    };
    this.m_pCharacter.m_pCombatant = this;
    this.m_preCombatMorale = this.m_pCharacter.GetMorale();

    this.SetPartyMember(true);
    this.State(individualCombatantState.ICS_None);
    origKey = dude;
    this.m_iMovement = 0;
    this.m_iNumDiagonalMoves = 0;

    this.friendly = true;
    this.m_adjFriendly = 0;
    // NPC in party cannot be controlled in combat unless it is a 
    // pre-generated character
    //20140915 PRS SetAllowPlayerControl( (GetType()==CHAR_TYPE) || (m_pCharacter->IsPreGen) );
    this.SetAllowPlayerControl(true);
    this.m_iFacing = this.DetermineInitialFacing(party.facing);
    this.m_iMoveDir = this.m_iFacing;
    Globals.debug();
    this.determineNbrAttacks(); // lookup max possible defaults
    if (this.GetAdjAutomatic()) {
        this.ReadyBestArmor();
        this.ReadyBestWpn(NO_DUDE);
        this.ReadyBestShield();
    }
    this.determineNbrAttacks();
    this.determineAvailAttacks(this.GetNbrAttacks()); // now take ready wpn into account
    //WriteDebugString("add 2: origIndex %i, origKey %i\n", origIndex, origKey);
}

COMBATANT.prototype.SetPartyMember = function (flag) {
    if (flag == null || flag == undefined) { flag = true; }
    this.m_pCharacter.SetPartyMember(flag);
}

COMBATANT.prototype.SetAllowPlayerControl = function (flag) {
    this.m_pCharacter.SetAllowPlayerControl(flag);
}

COMBATANT.prototype.determineNbrAttacks = function () {
    return this.m_pCharacter.determineNbrAttacks();
}

COMBATANT.prototype.determineAvailAttacks = function (numberOfAttacks) {
    this.availAttacks = numberOfAttacks;

    /** //PORT NOTE:  Nah not worth it for the debug message
    var temp = availAttacks;
    var t = timeGetTime();
    TRACE("%d determineAvailAttacks\n", t);
    TRACE("Setting availAttacks for %i to %f\n", self, temp);   
    */
}

COMBATANT.prototype.DetermineInitialFacing = function(PartyFacing) {
    switch (PartyFacing) {
        case FACE_NORTH:
        case FACE_SOUTH:
            if (Globals.RollDice(100, 1, 0) <= 50)
                return FACE_EAST;
            else
                return FACE_WEST;

        case FACE_EAST:
            return FACE_EAST;
        case FACE_WEST:
            return FACE_WEST;
    }
    return FACE_EAST;
}

COMBATANT.prototype.GetContextActor = function() {
    var pActor = new ActorType();
    pActor.Clear();

    if (this.IsPartyMember()) {
        pActor.SetCombatPartySrc(this.friendly);
    }
    else {
        if (this.GetType() == MONSTER_TYPE)
            pActor.SetCombatMonsterSrc(this.friendly);
        else if (this.GetType() == NPC_TYPE)
            pActor.SetCombatNPCSrc(this.friendly);
        //#ifdef newCombatantx                            // PORT NOTE: ????
        else
            Globals.die(0xea667);
        //#endif
    }
    return pActor;
}

COMBATANT.prototype.IsPartyMember = function()  {
    return this.m_pCharacter.IsPartyMember();
}

COMBATANT.prototype.GetAdjAutomatic = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    return this.m_pCharacter.GetAdjAutomatic(flags);
}

COMBATANT.prototype.GetNbrAttacks = function () {
    return this.m_pCharacter.GetNbrAttacks();
}

COMBATANT.prototype.LoadCombatIcon = function () {
   /**TODO**/
}

COMBATANT.prototype.determineIconSize = function () {
    this.width = 1;
    this.height = 1;
    /**TODO:  They use the size of the image here to determine and set the grid width/height of the combatant 
     * Hmmm... Not sure what I want to do with this yet given the goal of decoupling
     * from the graphics code.  Stub for now.

      * var w = 0, h = 0, bits = 0;
//#ifdef newCombatant            // PORT NOTE: I think this is on
    if (GraphicsMgr.GetSpriteSurfaceStats(m_pCharacter -> icon.key, w, h, bits))
//#else
//    if (GraphicsMgr.GetSpriteSurfaceStats(icon.key, w, h, bits))
//#endif
    {
        int imagewidth = w; // pixel width

        // (width / frame_width) / 2_frames_per_icon
        w = (w / COMBAT_TILE_WIDTH) / 2;

        // 2 frame minimum (one of each pose, rdy/attack)
        // frame count is multiple of 2
        // the icon frame pair used is indicated by iconIndex
        // adjust for number of frames in icon
        //#ifdef newCombatant
        w /= (m_pCharacter -> icon.NumFrames / 2);
        //#else
        //        w /= (icon.NumFrames / 2);
        //#endif

        h = h / COMBAT_TILE_HEIGHT;

        if (w < 1) w = 1;
        if (h < 1) h = 1;

        width = w;
        height = h;
        //#ifdef newCombatant
        int offset = (m_pCharacter -> iconIndex - 1) * ((width * COMBAT_TILE_WIDTH) * 2);
        if ((offset + (width * COMBAT_TILE_WIDTH)) >= (imagewidth - width))
            m_pCharacter -> iconIndex=1;
        //#else
        //        int offset = (iconIndex - 1) * ((width * COMBAT_TILE_WIDTH) * 2);
        //        if ((offset + (width * COMBAT_TILE_WIDTH)) >= (imagewidth - width))
        //            iconIndex = 1;
        //#endif
    }
  else
    //#ifdef newCombatant
    m_pCharacter -> icon.FreePic();
    //#else
    //    icon.FreePic();
    //#endif}
    */
}

COMBATANT.prototype.GetIsFriendly = function () {
    if (this.m_adjFriendly == 0) return this.friendly;
    if (this.m_adjFriendly == 1) return true;
    if (this.m_adjFriendly == 2) return false;
    return !this.friendly;
}

COMBATANT.prototype.GetCenterX = function() {
    if (this.width <= 1) return this.x;
    if (this.m_iFacing == FACE_WEST)
        return (this.x + (this.width / 2) - 1);
    else
        return (this.x + (this.width / 2));
}

COMBATANT.prototype.GetCenterY = function() {
    if (this.height <= 1) return this.y;
    return (this.y + (this.height / 2) - 1);
}

COMBATANT.prototype.OnEnsureVisible = function() {
    Globals.TRACE("OnEnsureVisible for " + this.self + "\n", );
}

COMBATANT.prototype.SetType = function(flag) {
    this.m_pCharacter.SetType(flag);
}


COMBATANT.prototype.InitFromMonsterDataID = function (monsterID, IsFriendly, items, msack) {
    var pMonster;
    pMonster = monsterData.PeekMonster(monsterData.LocateMonster(monsterID));
    Globals.TRACE(Globals.timeGetTime() + " Afet PeekMonster\n");
    if (pMonster == null) {
        Globals.WriteDebugString("Cannot find data for monster " + monsterID + "\n");
        pMonster = monsterData.PeekMonster(0);
        if (pMonster == null) {
            Globals.WriteDebugString("Combat with no monsters defined");
            Globals.die(0x45acb);
        };
    }
    this.InitFromMonsterDataMonster(pMonster, IsFriendly, items, msack);
}


COMBATANT.prototype.InitFromMonsterDataMonster = function (pMonster, IsFriendly, items, msack) {
    if (this.m_pCharacter != null) {
        Globals.die(0xc33bd);
    }
    this.m_pCharacter = new CHARACTER();
    this.m_pCharacter.m_pCombatant = this;  // Link the character and combatant one to another.
    this.deleteChar = true;
    this.State(individualCombatantState.ICS_None);
    if (pMonster == null) {
        Globals.WriteDebugString("Bogus monster index in InitFromMonsterData()\n");
        this.Clear();
        this.origKey = NO_DUDE;
        return;
    }

    this.SetPartyMember(false);
    this.SetType(MONSTER_TYPE);
    this.m_pCharacter.monsterID = pMonster.MonsterID();

    this.origKey = monsterData.LocateMonster(this.m_pCharacter.monsterID);
    this.m_pCharacter.classID = pMonster.classID;
    this.m_pCharacter.race = pMonster.raceID;
    this.friendly = IsFriendly;
    this.m_adjFriendly = 0;

    this.generateNewCharacter(pMonster.XP_Value, Globals.START_EXP_VALUE); // determines monster AC,THAC0,Hit Points,etc

    Globals.TRACE(Globals.timeGetTime() + " After generateNewCharacter\n");
    this.SetStatus(charStatusType.Okay);
    this.SetAllowPlayerControl(Globals.GetConfigMonsterPlyrControl());
    this.m_pCharacter.SetName(pMonster.Name);
    this.m_pCharacter.SetPermInt(pMonster.Intelligence);
    this.m_pCharacter.SetMagicResistance(pMonster.Magic_Resistance);
    this.m_pCharacter.SetSize(pMonster.Size);
    this.SetMorale(pMonster.Morale);
    this.m_pCharacter.SetUndead(pMonster.undeadType);

    // items specified in combat event for this monster
    this.m_pCharacter.myItems = items;

    // default monster items in monster database
    var pos = pMonster.myItems.GetHeadPosition();
    while (pos != null) {
        this.m_pCharacter.myItems.AddItem(pMonster.myItems.GetNext(pos));
    }

    this.m_pCharacter.money = msack; // combat additional money
    this.m_pCharacter.money.AddMoneySack(pMonster.money); // monster default money

    this.m_pCharacter.icon = pMonster.Icon;

    if (false)  // This was done in generateNewCharacter
    {
        this.determineNbrAttacks();
    };

    this.ReadyBestArmor();
    this.ReadyBestWpn(NO_DUDE);
    this.ReadyBestShield();
    this.determineNbrAttacks(); // Take armor and such into account.
    this.determineAvailAttacks(this.GetNbrAttacks());
    this.determineMaxMovement();

    if (Globals.GetConfigMonsterNoMove()) {
        this.SetMaxMovement(0);
        this.m_iMovement = 0;
        this.m_iNumDiagonalMoves = 0;
    }

    Globals.ASSERT(this.GetAdjHitPoints() > 0);

    {
        var hookParameters = new HOOK_PARAMETERS();
        var scriptContext = new SCRIPT_CONTEXT();
        scriptContext.SetCharacterContext(this.m_pCharacter);
        scriptContext.SetMonsterTypeContext(pMonster);
        SPECAB.RunGlobalScript("Global_CreateMonsterCombatant", SPECAB.CREATE_MONSTER_COMBATANT, true);
        pMonster.RunMonsterScripts(
            SPECAB.CREATE_MONSTER_COMBATANT,
            SPECAB.ScriptCallback_RunAllScripts,
            null,
            "Initializing monster for combat");
    }
}

/*  Questionable */   //PORT NOTE:  Comment copied from original source
COMBATANT.prototype.generateNewCharacter = function(StartExperience, StartExpType) {
    this.m_pCharacter.generateNewCharacter(StartExperience, StartExpType);
}

COMBATANT.prototype.GetType = function () {
    return this.m_pCharacter.GetType();
}

COMBATANT.prototype.SetStatus = function(val) {
    this.m_pCharacter.SetStatus(val);
}

COMBATANT.prototype.SetMorale = function (val) {
    this.m_pCharacter.SetMorale(val);
}


COMBATANT.prototype.ReadyBestArmor = function () {
    this.m_pCharacter.ReadyBestArmor();
};

COMBATANT.prototype.ReadyBestWpn = function(targ) {
    var isLargeTarget = false;
    var dist = 1;

    // get distance to target
    if (targ == NO_DUDE) {
        // no combat targets, nobody to shoot at, so just ready 
        // a hand-to-hand weapon
        dist = 1;
        isLargeTarget = false;
    }
    else {
        var targCOMBATANT;
        targCOMBATANT = this.GetCombatantPtr(targ);
        Globals.ASSERT(targCOMBATANT != null);
        if (targCOMBATANT != null) {
            isLargeTarget = targCOMBATANT.isLargeDude();
            dist = Distance(self, x, y,
                targCOMBATANT.self, targCOMBATANT.x, targCOMBATANT.y);
        }
    }
    this.m_pCharacter.ReadyBestWpn(dist, isLargeTarget);

    // if target is distant but don't have distance weapon
    // then ready hand-to-hand weapon
    if ((dist > 1) && (this.m_pCharacter.myItems.GetReadiedItem(Items.WeaponHand, 0) == Items.NO_READY_ITEM))
        this.m_pCharacter.ReadyBestWpn(1, isLargeTarget);
}

COMBATANT.prototype.ReadyBestShield = function () {
    return this.m_pCharacter.ReadyBestShield();
};

COMBATANT.prototype.determineMaxMovement = function () {
    return this.m_pCharacter.determineMaxMovement();
};

COMBATANT.prototype.GetAdjHitPoints = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS; }
    return this.m_pCharacter.GetAdjHitPoints(flags);
};

COMBATANT.prototype.GetMorale = function () {
    return this.m_pCharacter.GetMorale();
}

COMBATANT.prototype.MoveCombatant = function(newX, newY, allowZeroMoveAttack) {
    var dude = 0;
    var isUpdate = false;
    var moveData = new MOVE_DATA();
    // if already at destination
    if ((newX == this.x) && (newY == this.y))
        return false;

    if (this.GetType() == MONSTER_TYPE) {
        if (this.GetConfigMonsterNoMove())
            return false;
    }

    moveData.combatantIndex = this.self;
    moveData.oldX = this.x;
    moveData.oldY = this.y;
    moveData.newX = this.newX;
    moveData.newY = this.newY;

    var dir = PATH_DIR.GetDir(this.x, this.y, this.newX, this.newY);
    var movePoints = PATH_DIR.GetDist(dir);  // Returns 1 (orthogonal) or 2 (diagonal)
    if (movePoints == 2) {
        this.m_iNumDiagonalMoves++;
        if ((this.m_iNumDiagonalMoves & 1) == 0) movePoints = 1;
    };
    this.FaceDirection(dir);

    dude = NO_DUDE;
    if (Drawtile.coordsOnMap(newX, newY, this.width, this.height)) {
        dude = Drawtile.getCombatantInCell(newX, newY, this.width, this.height, this.self);
    };

    if (((dude != NO_DUDE) && (allowZeroMoveAttack))
        || (this.m_iMovement < this.GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS, "Move Combatant") - (movePoints - 1))
    ) {
        if (Drawtile.coordsOnMap(newX, newY, this.width, this.height)) {
            if (dude != NO_DUDE) {
                if (this.canAttack(dude, -1, -1, 0, Drawtile.Distance6, false)) {
                    this.RemoveAllTargets();
                    this.AddTarget(dude, false);
                    this.StartAttack(dude, 0);
                    //          WriteDebugString("DEBUG - Return to MoveCombatant");
                    isUpdate = true;
                }
            }
            // check for walls and such
            else if (OBSTICAL_TYPE.OBSTICAL_none == Drawtile.ObsticalType(newX, newY, this.width, this.height, false, true, this)) {
                // if moving away from an enemy that has no target,
                // the enemy gets to make an attack at this dude's
                // retreating backside.

                var oldX = 0, oldY = 0;

                this.PlayCombatMove();
                this.m_iMovement += movePoints;

                // remove dude from old location
                Drawtile.placeCombatant(this.x, this.y, NO_DUDE, this.width, this.height);

                Globals.TRACE("MoveCombatant() from " + this.x + "," + this.y + " to " + newX + "," + newY + "\n");

                if ((Math.abs(newX - this.x) > 1) || (Math.abs(newY - this.y) > 1))
                    Globals.TRACE("Moving more than 1 square\n");

                // place dude at new location
                Drawtile.placeCombatant(newX, newY, this.self, this.width, this.height);

                oldX = this.x;
                oldY = this.y;
                this.x = newX;
                this.y = newY;

                if (this.CheckOpponentFreeAttack(oldX, oldY, newX, newY)) {
          // This guys turn isn't over, but once the free
          // attacks are over, he will not start out in CS_Move.
//#ifdef D20140707      // PORT NOTE:   Doesn't seem to be any difference
//                    State(ICS_None);  // Should we skip this if OnAuto()?
//#else
                    this.State(individualCombatantState.ICS_None);
//#endif
                }

                {
                    var actor = new ActorType();
                    var hookParameters = new HOOK_PARAMETERS();
                    var scriptContext = new SCRIPT_CONTEXT();
                    actor = this.GetContextActor();
                    RunTimeIF.SetCharContext(actor);

                    scriptContext.SetCombatantContext(this);
                    this.RunCombatantScripts(
                        SPECAB.ON_STEP,
                        SPECAB.ScriptCallback_LookForChar,
                        "F",
                        "Move Combatant");
                    RunTimeIF.ClearCharContext();
                };

                if (activeSpellList.LingerSpellAffectsTarget(this.self, this.x, this.y, this.width, this.height)) {
                    // A lingering spell is active at the location 
                    // just moved to. 
                    Globals.TRACE(this.self + " moved into at least one active linger spell at location " + this.x + "," + this.y + "\n");

                    activeSpellList.ActivateLingerSpellsOnTarget(this.self, this.x, this.y, this.width, this.height);
                }

                isUpdate = true;
            }
        }
        else {
            isUpdate = true;
            // dude fled map
            this.SetStatus(charStatusType.Fled);
            if (this.friendly)
                Globals.IncNumFriendFlee(); //combatData.numFrndFlee++;
            else
                Globals.IncNumMonsterFlee(); //combatData.numMonFlee++;
            this.EndTurn();
        }
    }

    if ((isUpdate) && (this.charOnCombatMap(false, true))) {
        Drawtile.ForceMapToMoveAlong(this.x, this.y, dir, Math.max(this.width, this.height));
        Globals.PlaceCursorOnCurrentDude();
    }
    combatData.CheckAllAuraPlacements(moveData);
    return isUpdate;
}

COMBATANT.prototype.GetCurrentLevel = function (baseclassID) {
    return this.m_pCharacter.GetCurrentLevel(baseclassID);
}

COMBATANT.prototype.race = function () {
    return this.m_pCharacter.race;
}

COMBATANT.prototype.GetClass = function () {
    return this.m_pCharacter.GetClass();
}

COMBATANT.prototype.CanCastSpells = function () {
    return this.m_pCharacter.CanCastSpells();
}

COMBATANT.prototype.FetchCharacterSpell = function (spellID, pCharSp) {
    return this.m_pCharacter.FetchCharacterSpell(spellID, pCharSp);
}

COMBATANT.prototype.ListShields = function (pcsc) {
    this.m_pCharacter.ListShields(pcsc);
}

COMBATANT.prototype.EvalDiceAsClass = function (dice, baseclassID, pRollerLevel) {
    return this.m_pCharacter.EvalDiceAsClass(dice, baseclassID, pRollerLevel);
}

COMBATANT.prototype.EvalDiceAsClass = function (dice, schoolID, spellLevel, pRollerLevel) {
    return this.m_pCharacter.EvalDiceAsClass(dice, schoolID, spellLevel, pRollerLevel);
}

COMBATANT.prototype.GetStatus = function () {
    return this.m_pCharacter.GetStatus();
}

COMBATANT.prototype.UpdateSpellForAttacks = function (AttacksTaken) {
    this.m_pCharacter.UpdateSpellForAttacks(AttacksTaken);
}

COMBATANT.prototype.SetEncumbrance = function (val) {
    this.m_pCharacter.SetEncumbrance(val);
}

COMBATANT.prototype.determineEffectiveEncumbrance = function () {
    return this.m_pCharacter.determineEffectiveEncumbrance();
}

COMBATANT.prototype.GetAutomatic = function() {
    return this.m_pCharacter.GetAutomatic()
};

COMBATANT.prototype.SetAutomatic = function(flag) {
    this.m_pCharacter.SetAutomatic(flag);
}

COMBATANT.prototype.GetCurrentLevel  = function(baseclassID) {
    return this.m_pCharacter.GetCurrentLevel(baseclassID);
}

COMBATANT.prototype.race = function () {
    return this.m_pCharacter.race;
}

COMBATANT.prototype.GetClass = function () {
    return this.m_pCharacter.GetClass();
}

COMBATANT.prototype.CanCastSpells = function () {
    return this.m_pCharacter.CanCastSpells();
};

COMBATANT.prototype.FetchCharacterSpell = function(spellID, pCharSp) {
    return this.m_pCharacter.FetchCharacterSpell(spellID, pCharSp);
};

COMBATANT.prototype.FaceDirection = function(dir) {
    // translate attacker direction into
    // east/west icon facing
    switch (dir) {
        case PATH_DIR.PathNW:
            this.m_iMoveDir = FACE_NW;
            this.m_iFacing = FACE_WEST;
            break;
        case PATH_DIR.PathWest:
            this.m_iMoveDir = FACE_WEST;
            this.m_iFacing = FACE_WEST;
            break;
        case PATH_DIR.PathSW:
            this.m_iMoveDir = FACE_SW;
            this.m_iFacing = FACE_WEST;
            break;
        case PATH_DIR.PathNE:
            this.m_iMoveDir = FACE_NE;
            this.m_iFacing = FACE_EAST;
            break;
        case PATH_DIR.PathEast:
            this.m_iMoveDir = FACE_EAST;
            this.m_iFacing = FACE_EAST;
            break;
        case PATH_DIR.PathSE:
            this.m_iMoveDir = FACE_SE;
            this.m_iFacing = FACE_EAST;
            break;

        default:
            // if north/south attacker, no need to change facing.
            break;
    }
}

COMBATANT.prototype.GetAdjMaxMovement = function (flags, comment) {
    return this.m_pCharacter.GetAdjMaxMovement(flags, comment);
}

COMBATANT.prototype.PlayCombatMove = function() {
    if (this.GetType() == MONSTER_TYPE)
        monsterData.PlayMove(this.m_pCharacter.monsterID);
    else
        Globals.PlayCombatMove(); //combatData.PlayCombatMove();
}

COMBATANT.prototype.charOnCombatMap = function(unconsciousOK, petrifiedOK) {
    var stype = this.GetAdjStatus();
    if ((this.charUnconscious() && !unconsciousOK)
        || (this.charPetrified() && !petrifiedOK)
        || (stype == charStatusType.Fled)
        || (stype == charStatusType.Gone)
        || (stype == charStatusType.TempGone)
        || (stype == charStatusType.Dead))
        return false;

    return true;
}

COMBATANT.prototype.GetAdjStatus = function (flags) {
    if (!flags) { flags = DEFAULT_SPELL_EFFECT_FLAGS };
    return this.m_pCharacter.GetAdjStatus(flags);
}

COMBATANT.prototype.charUnconscious = function() {
    var stype = this.GetAdjStatus();
    if ((stype == charStatusType.Dying) || (stype == charStatusType.Unconscious)) return true;
    return false;
}

COMBATANT.prototype.charPetrified = function() {
    var stype = this.GetAdjStatus();
    if (stype == charStatusType.Petrified) return true;
    return false;
}



/*
 * 
 *

struct COMBATANT //: public CHARACTER
{
void ComputeCombatViewValues(void);
/* Here we put functions that are needed to access the
 * underlying character.  We used to derive COMBATANT 
 * from character and so the character variables were
 * directly exposed.  Now we need to access them via
 * the m_pCharacter pointer.
 * /
inline void ListShields(COMBAT_SUMMARY_COMBATANT * pcsc){ m_pCharacter -> ListShields(pcsc); };
inline double EvalDiceAsClass(DICEPLUS & dice, const BASECLASS_ID& baseclassID, int * pRollerLevel) const
    { return m_pCharacter-> EvalDiceAsClass(dice, baseclassID, pRollerLevel);};
inline double EvalDiceAsClass(DICEPLUS & dice, const SCHOOL_ID& schoolID, int spellLevel, int * pRollerLevel) const
    { return m_pCharacter-> EvalDiceAsClass(dice, schoolID, spellLevel, pRollerLevel);};
inline charStatusType GetStatus() const { return m_pCharacter-> GetStatus(); };
inline void UpdateSpellForAttacks(int AttacksTaken) { m_pCharacter -> UpdateSpellForAttacks(AttacksTaken); };
inline void SetEncumbrance(int val) { m_pCharacter -> SetEncumbrance(val); };
inline int  determineEffectiveEncumbrance() { return m_pCharacter -> determineEffectiveEncumbrance(); };
inline BOOL GetAutomatic() const { return m_pCharacter-> GetAutomatic(); };
inline void SetAutomatic(BOOL flag) { m_pCharacter -> SetAutomatic(flag); };
inline BYTE GetMaxMovement() const { return m_pCharacter-> GetMaxMovement(); };
inline double GetNbrHD() const { return m_pCharacter-> GetNbrHD(); };
inline void SetMaxMovement(BYTE val) { m_pCharacter -> SetMaxMovement(val); };
inline void ClearQueuedSpecAb() { m_pCharacter -> ClearQueuedSpecAb(); };
inline int  getCharExpWorth() { return m_pCharacter -> getCharExpWorth(); };
inline int  GetMaxHitPoints() const { return m_pCharacter-> GetMaxHitPoints();};
inline int  GetEffectiveAC(void) const { return m_pCharacter-> GetEffectiveAC();};
inline BOOL IsAlive() { return m_pCharacter -> IsAlive(); };
inline genderType GetGender() const { return m_pCharacter-> GetGender(); };
inline void SetGender(genderType val) { m_pCharacter -> SetGender(val); };
inline void SetGender(const CString& gender) { m_pCharacter -> SetGender(gender); };
inline void InitTargeting(spellTargetingType ttype,
    BOOL canTargetFriend,
    BOOL canTargetEnemy,
    BOOL partyfriend,
    DWORD targs,
    DWORD range,
    int   xSize,
    int   ySize,
    BOOL lingering)
{
    m_pCharacter -> InitTargeting(ttype,
        canTargetFriend,
        canTargetEnemy,
        partyfriend,
        targs,
        range,
        xSize,
        ySize,
        lingering);
};
inline int  GetThiefBackstabDamageMultiplier() const { return m_pCharacter-> GetThiefBackstabDamageMultiplier();};
inline void SetHitPoints(int val) { m_pCharacter -> SetHitPoints(val); };
inline int  GetHitPoints() const { return m_pCharacter-> GetHitPoints(); };
inline BOOL HasDeathImmunity() const { return m_pCharacter-> HasDeathImmunity(); };
inline void UpdateSpellForDamage(int DamageTaken) { m_pCharacter -> UpdateSpellForDamage(DamageTaken); };
inline int  GetAdjSpecAb(specialAbilitiesType sa, DWORD * pSource=NULL, CString * pSpellName=NULL) const { return m_pCharacter-> GetAdjSpecAb(sa, pSource, pSpellName);};
inline CString  GetUndeadType() const { return m_pCharacter-> GetUndeadType(); };
inline BOOL IsAnimal() const { return m_pCharacter-> IsAnimal();};
inline BOOL IsAlwaysLarge() const { return m_pCharacter-> IsAlwaysLarge();};
inline BOOL HasVorpalImmunity() const { return m_pCharacter-> HasVorpalImmunity();};
inline void ComputeCharacterViewValues(void) { m_pCharacter -> ComputeCharacterViewValues(); };
inline BYTE GetAdjInt(DWORD flags = DEFAULT_SPELL_EFFECT_FLAGS) const { return m_pCharacter-> GetAdjInt(flags);};
inline int GetAdjTHAC0(DWORD flags = DEFAULT_SPELL_EFFECT_FLAGS) const { return m_pCharacter-> GetAdjTHAC0(flags);};
inline int GetAdjAC(DWORD flags = DEFAULT_SPELL_EFFECT_FLAGS) const { return m_pCharacter-> GetAdjAC(flags);};
inline BOOL GetAdjAllowPlayerControl(DWORD flags = DEFAULT_SPELL_EFFECT_FLAGS) { return m_pCharacter -> GetAdjAllowPlayerControl(flags); };
inline int  GetAdjDmgBonus(DWORD flags = DEFAULT_SPELL_EFFECT_FLAGS) const { return m_pCharacter-> GetAdjDmgBonus(flags); };
inline int  GetAdjHitBonus(const ITEM_ID& weaponID, int distance, DWORD flags = DEFAULT_SPELL_EFFECT_FLAGS) const
    {
        return m_pCharacter-> GetAdjHitBonus(weaponID, distance, flags);
  };
inline BOOL GetAdjDetectingInvisible(DWORD flags = DEFAULT_SPELL_EFFECT_FLAGS) const { return m_pCharacter-> GetAdjDetectingInvisible(flags);};
inline creatureSizeType GetAdjSize(DWORD flags = DEFAULT_SPELL_EFFECT_FLAGS) const { return m_pCharacter-> GetAdjSize(flags);};
enum { MAX_COMBAT_TARGETS = 100 };
int GetUniqueId() { return self; }
void InitFromNPCData(const CHARACTER_ID& characterID, BOOL IsFriendly, const ITEM_LIST & items, const MONEY_SACK & msack);
BOOL isLargeDude() const ;
void RollInitiative(eventSurpriseType surprise);
BOOL charCanTakeAction();
void GetDamageDice(int wpn,
    int * pNum,
    int * pSides,
    int * pBonus,
    BOOL * pNonLethal,
    BOOL IsLarge,
    CString * pSpellName) const ;
int GetAttackBonus(const ITEM_ID& itemID, int distance) const ;
void GetContext(ActorType * pActor, const BASECLASS_ID& baseclassID) const ;
void GetContext(ActorType * pActor, const SCHOOL_ID& schoolID) const ;
void GetContext(ActorType * pActor, const SPELL_ID& spellID) const ;
void GetContext(ActorType * pActor, const SPELL_DATA * pSpell) const ;
void GetContext(ActorType * pActor) const ; // Unknown class; therefore unknown Level.
BOOL CastSpell(const SPELL_ID& spellID, const SPELL_ID& secondarySpellID);
BOOL CastItemSpell(const SPELL_ID& spellID);
void SpellActivate(const PENDING_SPELL & data);
void InstantSpellActivate(const SPELL_ID& attackSpellID,
                            const SPELL_ID& itemSpellID,
    int target,
        ToHitComputation * pToHitComputation);
BOOL IsSpellPending();
void FaceOpponent(int opponent);
void FaceLocation(int x, int y);
void FaceDirection(PATH_DIR dir);
BOOL CheckForGuardingEnemy();
BOOL CheckOpponentFreeAttack(int oldX, int oldY, int newX, int newY);
void FillTargeterList(PATH_DIR dir);
void PlayHit() const ;
void PlayMiss() const ;
void PlayLaunch() const ;
void PlayCombatDeath() const ;
void PlayCombatTurnUndead() const ;
void TakeDamage(int dmg,
    BOOL IsNonLethal,
    InvokeSpellOptions * invokeOptions,
    bool canFinishCasting,
    int * pDeathIndex);
void RestoreToParty();
void /* PRS July 2009 BOOL * / UpdateCombatant();
BOOL Think();
BOOL FreeThink(void);
void HandleCombatRoundMsgExpired();
void HandleTimeDelayMsgExpired(int iDeathIndex);
int  HandleTimeDelayMsgBegin(int extraAttacksAvailable, int * pDeathIndex);
void PostCombatScreenUpdate();
int  HandleCurrState(BOOL zeroMoveAttackOK);
void displayCombatSprite();
void blitDeadSprite();
void blitDyingSprite();
void blitPetrifiedSprite();
BOOL CanAddTarget(int target);
BOOL C_AddTarget(COMBATANT & dude, int range = 0);
BOOL AddMapTarget(int mapx, int mapy, PATH_DIR dir, int dirX, int dirY);
BOOL AddTargetSelf();
void AutoChooseSpellTargets();
void RemoveCurrTarget();
int  GetCurrTarget(bool updateTarget, bool unconsciousOK, bool petrifiedOK);
int  GetNextTarget();
int  GetMaxTargets();
int  GetNumTargets() const { return combattargets.GetCount(); }
BOOL HaveTarget(int target);
BOOL IsAttackPossible(void);
int  makeAttack(int target, int extraAttacksAvailable, int * pDeathIndex);
BOOL DetermineIfBackStab(int wpn, int targ) const ;
void AttackOver();
void StopAttack();
double AttacksRemaining() const { return availAttacks; }
BOOL UseAttackPose();
BOOL NeedMissileAnimation();
void InitMissileAnimation();
BOOL NeedHitAnimation();
void InitHitAnimation();
BOOL NeedSpellCastAnimation();
void InitSpellCastAnimation(int targ);
BOOL NeedSpellInRouteAnimation(int x, int y, BOOL allowSelf);
void InitSpellInRouteAnimation(int x, int y);
void InitSpellInRouteAnimation(int srcx, int srcy, int dstx, int dsty);
BOOL NeedSpellCoverageAnimation();
void InitSpellCoverageAnimation(int x, int y);
BOOL NeedSpellHitAnimation();
void InitSpellHitAnimation(int targ);
BOOL NeedSpellLingerAnimation();
void InitSpellLingerAnimation(/*int mapx, int mapy* /);
void InitSpellLingerAnimation(int targ);
BOOL CanTarget(bool freeAttack);
BOOL CanMove(BOOL allowZeroMove);
BOOL CanCast();
BOOL CanUse();
BOOL CanTurnUndead();
BOOL CanDelay();
BOOL CanBandage();
CString SpecialActionName();
int SpecialActionIndex();
BOOL CanGuard(GUARDING_CASE guardCase);
void Bandage();
void EndTurn(individualCombatantState newState = ICS_None);
void Quick(BOOL Enable);
void Guard();
void DelayAction();
void TurnUndead();
void StartInitialSpellCasting(const SPELL_ID& spellName, const SPELL_ID& secondarySpellName);
void StartInitialItemSpellCasting(const SPELL_ID& spellID);
void EndInitialSpellCasting();
BOOL IsDone(bool freeAttack, LPCSTR comment);
BOOL IsAttacking() const { return (State() == ICS_Attacking); }
BOOL IsGuarding() const { return (State() == ICS_Guarding); }
BOOL IsCasting();
void StopCasting(BOOL EndTurn, bool canFinishCasting);
SPELL_ID  GetSpellIDBeingCast() const { return m_spellIDBeingCast;};
SPELL_ID  GetSecondarySpellIDBeingCast() const { return m_secondarySpellIDBeingCast;};
SPELL_ID GetEffectiveSpellIDBeingCast() const
    {
        if (m_secondarySpellIDBeingCast.IsValidSpell())
{
    return m_secondarySpellIDBeingCast;
};
return m_spellIDBeingCast;
  };
SPELL_ID  GetItemSpellIDBeingCast() const { return m_itemSpellIDBeingCast;};;
void SetSpellIDBeingCast(const SPELL_ID& spellID, const SPELL_ID& secondarySpellID)
{
    m_spellIDBeingCast = spellID;
    m_secondarySpellIDBeingCast = secondarySpellID;
};
void SetItemSpellIDBeingCast(const SPELL_ID& spellID) { m_itemSpellIDBeingCast = spellID; };
BOOL FetchSpellBeingCast(CHARACTER_SPELL * pCharSp=NULL);
CString RunPSScripts(const CString& scriptName, LPCSTR comment);
BOOL OnAuto(bool callAutoActionHook);
void StopMoving() { State(ICS_None); moveX = -1; moveY = -1; }
void moveLeft();
void moveRight();
void moveForward();
void moveBackward();
void moveNW();
void moveNE();
void moveSW();
void moveSE();
BOOL CurrMoveWouldFleeMap();
BOOL TeleportCombatant(int newX, int newY);
BOOL FindPathTo(int destLeft, int destTop,
    int destRight, int destBottom,
    BOOL CheckOccupants,
    BOOL allowZeroMove,
    BOOL moveOriginPoint);
BOOL FindPathAwayFrom(int fleeFrom);
int FindPathToMapNorthEdge(void);
int FindPathToMapEastEdge(void);
int FindPathToMapSouthEdge(void);
int FindPathToMapWestEdge(void);
BOOL FindPathToMapEdge();
BOOL FindPathAlongLine(PATH_DIR dir, int dist);
void ClearPath();
BOOL TakeNextStep(BOOL allowZeroMoveAttack);
BOOL toggleReadyItem(int item);
BOOL delCharacterItem(int index, int qty);
void CheckMorale();
DWORD MakeTargetKey(int dude, int dist) { return ((((DWORD)dist) << 16) | ((DWORD)dude)); }
int   GetTargetKeyDude(DWORD k) const { return (k & 0x0000FFFF); }
int   GetTargetKeyDist(DWORD k) const { return ((k >> 16) & 0x0000FFFF); }
void OnStartCombatantAction();
void OnEndTurn();
CString FormatSpecAbMsg(DWORD sa_state);
// These ModifyXXX functions dynamically alter character
// values based on spell effects or special abilities.
BOOL ModifyAttackRollDice
    (const CHARACTER * pTarget,                        const int num, const int sides, int * pBonus) const ;
BOOL ModifyAttackRollDiceForItem
    (const CHARACTER * pTarget,   const ITEM_ID& itemID, const int num, const int sides, int * pBonus, int distance) const ;
BOOL ModifyAttackRollDiceAsTarget
    (const CHARACTER * pAttacker,                      const int num, const int sides, int * pBonus) const ;
BOOL ModifyAttackRollDiceForItemAsTarget
    (const CHARACTER * pAttacker, const ITEM_ID& itemID, const int num, const int sides, int * pBonus) const ;
BOOL ModifyAttackDamageDice
    (const CHARACTER * pTarget,                        const int num, const int sides, int * pBonus, BOOL * pNonLethal) const ;
BOOL ModifyAttackDamageDiceForItem
    (const CHARACTER * pTarget,   const ITEM_ID& itemID, const int num, const int sides, int * pBonus, BOOL * pNonLethal, int distance) const ;
BOOL ModifyAttackDamageDiceAsTarget
    (const CHARACTER * pAttacker,                      const int num, const int sides, int * pBonus, BOOL * pNonLethal) const ;
void ModifyAttackDamageDiceForItemAsTarget
    (const CHARACTER * pAttacker, const ITEM_ID& itemID, const int num, const int sides, int * pBonus, BOOL * pNonLethal, int toHitRolled) const ;
BOOL ModifyAttackThac0
    (const CHARACTER * pTarget, int * pVal) const ;
BOOL ModifyAttackThac0AsTarget
    (const CHARACTER * pAttacker, int * pVal) const ;
BOOL ModifySaveRoll
    (const CHARACTER * pTarget, int * pRoll) const ;
BOOL ModifySaveRollAsTarget
    (CHARACTER * pAttacker, int * pRoll) const ;
BOOL ModifyACAsTarget
    (const CHARACTER * pAttacker, int * pAC, const ITEM_ID& itemID) const ;

enum SPECIAL_SELF {
    SELF_NewCombatant = -3,
};
inline individualCombatantState State(void) const { return m_ICS;};
void State(individualCombatantState ICS);

};
 */


/*
 * Good Morning,
 * 
 * OK.  Here is what I plan.  This email message
 * will be copied verbatim into the C++ code as
 * comments as a reference.
 * Let me know if it seems wrong.
 *
 * What follows here is a kind of "Pseudo-code"
 * (as if it were being done by hook scripts)
 * and is applied to each combatants; PC, NPC,
 * or monster.  I will attempt to code this
 * algorithm in such a way that hooks could
 * be inserted at any of these points in the
 * hard-coded algorithm.  This may take a couple
 * of days and obviously will require more
 * extensive testing because there are so
 * many possibilities.
 * 
 * $VAR AttacksRemaining;
 * $VAR GuardingAttacksRemaining;
 * $VAR IsGuarding;
 * 
 ** [Guarding-CanGuard]
 * $IF (is monster) $RETURN "N";
 * $IF (rangeWeapon) $RETURN "N";
 * $RETURN "Y";
 * 
 * [Guarding-BeginCombat]
 * GuardingAttacksRemaining = 0;
 * IsGuarding = false;
 * 
 * [Guarding-StartOfRound]
 * AttacksRemaining = <number of attacks allowed>;
 * 
 * [Guarding-Guard]  // combatant enters 'guard' mode
 * GuardingAttacksRemaining = AttacksRemaining
 * IsGuarding = true;
 * 
 * [Guarding-StartOfTurn]  // menu presented for combatant
 * IsGuarding = false;
 * GuardingAttacksRemaining = 0;
 * 
 * [Guarding-CanGuardAttack]  // enemy approaches
 * $IF( !IsGuarding) $RETURN "N";
 * $IF (rangeWeapon) $RETURN "N";
 * $IF(GuardAttacksRemaining <=# 0) $RETURN "N";
 * $IF(AttacksRemaining <=#0) $RETURN "N";
 * $RETURN "Y";
 * 
 * [Guarding-MakeAttack]
 * Decrement GuardAttacksRemaining;
 * $IF(GuardAttacksRemaining <=#0) IsGuarding = false;
 * Decrement AttacksRemaining
 * 
 * [FreeAttack-CanFreeAttack]  // enemy departs
 * $IF(rangeWeapon) $RETURN "N";
 * $RETURN "Y";
 * 
 * [FreeAttack-MakeAttack]
 * Decrement AttacksRemaining;
 * Decrement GuardAttacksRemaining;
 * $IF(GuardAttacksRemaining <=#0) IsGuarding = false;
 * 
 * 
 * [Guarding-MakeAttack]
 * Decrement GuardAttacksRemaining;
 * $IF(GuardAttacksRemaining <=#0) IsGuarding = false;
 * Decrement AttacksRemaining
 * 
 * [CanAttack]
 * $IF (IsGuarding) $RETURN "N";
 * $RETURN "Y";
 * 
 * Now I will attempt to apply this algorithm to your
 * examples.
 * 
 * On 10/17/2011 7:30 PM, Eric Cone wrote:
 * ***> Good afternoon.
 * ***>
 * ***> First things first - terminology.
 * ***>
 * ***> Free attack: an attack that follows all the rules of regular melee
 * ***> physical attacks (with or without weapon weapon, if weapon only range of
 * ***> 1 counts as melee)
 * ***> Guarding attack: a type of free attack that occurs when a combatant
 * ***> moves adjacent to a combatant that is GUARDING. As of this time, only
 * ***> combatants under player control may GUARD. The number of GUARD attacks
 * 
 * Handled by $IF (is monster) in [Guarding-CanGuardAttack]
 * 
 * ***> which may occur between a combatants consecutive turns is equal to it's
 * ***> number of attacks per turn.
 * 
 * Handled by:
 * [Guarding-Guard]
 * [Guarding-StartOfTurn]
 * {Guarding-MakeAttack]
 * 
 * ***>
 * ***> The other kind of free attack, occurs when a combatant moves away from
 * ***> an adjacent square, once per monster moving away.
 * ***>
 * ***> With that in mind, let's use Tom's answers to the questions - he was
 * ***> very thorough, and once the vocabulary is straightened out, I completely
 * ***> agree. It is immediately below-
 * ***>
 * ***> -Eric
 * ***>
 * ***> On Sun, Oct 16, 2011 at 7:40 AM, <Nologgie@aol.com
 * ***> <mailto:Nologgie@aol.com>> wrote:
 * ***>
 * ***>     Good Morning Eric,
 * ***>
 * ***>     Here are the answers I came up with for Paul's questions.
 * ***>
 * ***>     If an enemy is immediately adjacent and moves away then he is
 * ***>     subject to a
 * ***>     free attack.
 * ***>
 * ***>     If an enemy is not immediately adjacent and moves so as to become
 * ***>     immediately adjacent he is subject to a 'guarding' attack, but not a
 * ***>     free attack.
 * ***>
 * ***>     The Exact Rules:
 * ***>
 * ***>     Free Attacks
 * ***>
 * ***>     1. The 'free attack' is actually comprised of the attacker's entire
 * ***>     'attack
 * ***>     routine' for a combat round. This is normally 1 or 2 attacks for
 * ***>     characters. For 'monsters', it may be 8 or more.
 * ***>
 * ***>     2. Free attacks will always occur if the target is valid
 * ***>     (IsValidTarget !=
 * ***>     "N") and the attacker is able to execute a melee attack.
 * ***>     (IsCombatReady !=
 * ***>     "N", and a missile weapon is not readied.)
 * 
 * Handled by [FreeAttack-CanFreeAttack]
 * 
 * 
 * ***>
 * ***>     3. If a free attack occurs before the attacker's normal action for the
 * ***>     round, any attacks executed are deducted from the attacks available
 * ***>     for that
 * ***>     round. If the attacks are depleted it constitutes the combatant's
 * ***>     action for
 * ***>     the round, and the combatant gets no 'turn'.
 * 
 * Handled by [FreeAttack-MakeFreeAttack]
 * You did not specify this so I made a guess
 * that a free attack should also reduce the
 * number of guard attacks remaining.
 * 
 * ***>
 * ***>     4. Free attacks are allowed against fleeing enemies regardless of
 * ***>     whether
 * ***>     the attacker has any 'normal' attacks remaining, so in that sense
 * ***>     they are
 * ***>     unlimited.
 * ***>
 * 
 * Handled by [FreeAttack-CanFreeAttack]
 * 
 * 
 * ***>     Guarding Attacks
 * ***>
 * ***>     Combatants who do not have missile weapons readied may select
 * ***>     'Guard' as a
 * ***>     combat action. If a non-adjacent enemy moves adjacent to a guarding
 * 
 * Handled by [Guarding-CanGuard]
 * 
 * ***>     combatant, the guarding combatant will execute its attack routine.
 * ***>     Any attacks made
 * ***>     will be deducted from those available to the attacker in the combat
 * ***>     round
 * ***>     when guard was set. If the attacks are depleted, guarding status is
 * ***>     removed.
 * 
 * Handled by:
 * [MakeAttack]
 * [FreeAttack-MakeFreeAttack]
 * [Guarding-MakeAttack]
 * 
 * ***>
 * ***>     Example:
 * 
 * [StartOfCombat]
 * GuardAttacksRemaining=0;
 * 
 * 
 * ***>     A combatant selects Guard in round three. If an enemy moves adjacent
 * ***>     prior
 * [Guarding-StartOfRound] Round 3
 * AttacksRemaining = 3; // for example
 * [Guarding-Guard] round 3
 * GuardAttacksRemaining = AttacksRemaining = 3;  //for example
 * [Guarding-GuardAttack] round 3
 * Decrement GuardAttacksRemaining (now = 2)
 * ***>     to the guarding combatant's turn in round four, the guarding
 * ***>     combatant will
 * 
 * [GuardIng-StartOfRound] Round 4
 * AttacksRemaining = 3;
 * 
 * ***>     execute its attack routine, and the attacks will be counted against the
 * ***>     attacks available in round three (when Guard was set). The
 * ***>     combatant's round
 * ***>     four turn remains available.
 * 
 * [Guarding-GuardAttack]
 * Decrement AttacksAvailable (now = 3)
 * Decrement GuardAttacksRemaining (now = 1)
 * 
 * The rest of your explanation assumed that
 * 'free attacks' are completely separate from
 * 'guard attacks' whereas my questions assumed
 * they were two different aspects of the same
 * thing.  So the answers did not apply to the
 * intent of my questions.
 * 
 * 
 * PAul
 */


/*  Email added 20120627
*****************************************************************************************
*****************************************************************************************
*****************************************************************************************
*****************************************************************************************
  
The Exact Rules:

Free Attacks

1. The 'free attack' is actually comprised of the attacker's entire 'attack
routine' for a combat round. This is normally 1 or 2 attacks for
characters. For 'monsters', it may be 8 or more.

2. Free attacks will always occur if the target is valid (IsValidTarget !=
"N") and the attacker is able to execute a melee attack. (IsCombatReady !=
"N", and a missile weapon is not readied.)

3. If a free attack occurs before the attacker's normal action for the
round, any attacks executed are deducted from the attacks available for that
round. If the attacks are depleted it constitutes the combatant's action for
the round, and the combatant gets no 'turn'.

4. Free attacks are allowed against fleeing enemies regardless of whether
the attacker has any 'normal' attacks remaining, so in that sense they are
unlimited.

-----

**Who can execute a free attack?
Any combat-ready combatant without a readied missile weapon.

**When can he execute a free attack?
When an adjacent enemy who is a valid target moves away.

**Against whom can he execute a free attack?
Any adjacent enemy who is a valid target and moves away.

**How often can he execute a free attack?
Once each time any adjacent enemy who is a valid target moves away.

**How many free attacks?
A combatant uses all of his available attacks, until either the adjacent enemy is no longer adjacent, the adjacent enemy is dead, or the number of available attacks reaches zero.

**Is there more than one kind of free attack?
No. It only applies to physical melee attacks as described.

**Do the same rules apply to monsters?
**How are they different?
**Do we need a hook to allow more complicated logic.
The same rules apply to monsters.

*************************************************************

Guarding Attacks

Combatants who do not have missile weapons readied may select 'Guard' as a
combat action. If a non-adjacent enemy moves adjacent to a guarding
combatant, the guarding combatant will execute its attack routine. Any attacks made
will be deducted from those available to the attacker in the combat round
when guard was set. If the attacks are depleted, guarding status is removed.
*Note: Fleeing combatants may not guard, this applies to all - PC, NPC and monster; friendly and enemy.

Example:
A combatant selects Guard in round three. If an enemy moves adjacent prior
to the guarding combatant's turn in round four, the guarding combatant will
execute its attack routine, and the attacks will be counted against the
attacks available in round three (when Guard was set). The combatant's round
four turn remains available.

-----

**Who can execture a guarding attack?
Any combatant who matches the "Guarding Attack" criteria above.

**When can he execute a guarding attack?
When a non-adjacent enemy combatant moves adjacent.

**How often can he execute a guarding attack?
Each time a non-adjacent enemy combatant moves to become adjacent, and the combatant has more than zero available guarding attacks.

**How many guarding attacks?
A combatant uses all of his available attacks, until either the adjacent enemy is no longer adjacent, the adjacent enemy is dead, or the number of available attacks reaches zero.

**Is there more than one kind of guarding attack?
No. It only applies to physical melee attacks as described.

**Do the same rules apply to monsters?
**How are they different?
**Do we need a hook to allow more complicated logic.
Yes, monsters follow the same rules.

************************************************************************

$VAR AttacksRemaining;
$VAR GuardingAttacksRemaining;
$VAR IsGuarding;

[Guarding-CanGuard]
$IF (is monster) $RETURN "N";
$IF (rangeWeapon) $RETURN "N";
$RETURN "Y";

[Guarding-BeginCombat]
GuardingAttacksRemaining = 0;
IsGuarding = false;

[Guarding-StartOfRound]
AttacksRemaining = <number of attacks allowed>;

[Guarding-Guard]  // combatant enters 'guard' mode
GuardingAttacksRemaining = AttacksRemaining
IsGuarding = true;

[Guarding-StartOfTurn]  // menu presented for combatant
IsGuarding = false;
GuardingAttacksRemaining = 0;

[Guarding-CanGuardAttack]  // enemy approaches
$IF( !IsGuarding) $RETURN "N";
$IF (rangeWeapon) $RETURN "N";
$IF(GuardAttacksRemaining <=# 0) $RETURN "N";
$IF(AttacksRemaining <=#0) $RETURN "N";
$RETURN "Y";

[Guarding-MakeAttack]
Decrement GuardAttacksRemaining;
$IF(GuardAttacksRemaining <=#0) IsGuarding = false;
Decrement AttacksRemaining

[FreeAttack-CanFreeAttack]  // enemy departs
$IF(rangeWeapon) $RETURN "N";
$RETURN "Y";

[FreeAttack-MakeFreeAttack]
Decrement AttacksRemaining;
Decrement GuardAttacksRemaining;
$IF(GuardAttacksRemaining <=#0) IsGuarding = false;


[MakeAttack]
Decrement GuardAttacksRemaining;
$IF(GuardAttacksRemaining <=#0) IsGuarding = false;
Decrement AttacksRemaining

[CanAttack]
$IF (IsGuarding) $RETURN "N";
$RETURN "Y";

******

*/


COMBATANT.prototype.FreeAttackDistance = function (i, j, k, l, m, n)
{
    return 1;
}


//*****************************************************************************
//    NAME: COMBATANT::CheckOpponentFreeAttack
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT.prototype.CheckOpponentFreeAttack = function(oldX, oldY, newX, newY) {
    // See the email in the comments immediately above this function

    // More email adde 20120627
    // check clockwise around current location
    // looking for enemy that
    // can make an attack.
    var found = false;
    var maxDude = -1;
    var dude = 0;

    var wasAdjacent = []; 
    var willBeAdjacent = [];
    var tempCOMBATANT;
    var qcomb = QueuedCombatantData;
    var dx = 0, dy = 0;
    for (var idx = 0; idx < Globals.MAX_COMBATANTS; idx++) { wasAdjacent[idx] = 0; } // PORT NOTE:  Was memset(wasAdjacent, 0, MAX_COMBATANTS);
    for (var idx = 0; idx < Globals.MAX_COMBATANTS; idx++) { willBeAdjacent[idx] = 0; } // PORT NOTE:  Was memset(willBeAdjacent, 0, MAX_COMBATANTS);


    for (dx = -1; dx <= this.width; dx++) {
        for (dy = -1; dy <= this.height; dy++) {
            if (Drawtile.coordsOnMap(newX + dx, newY + dy, 1, 1)) {
                dude = Drawtile.getCombatantInCell(newX + dx, newY + dy, 1, 1, this.self);
                if (dude != NO_DUDE) {
                    if (combatData.getCombatantPtr(dude).charOnCombatMap(false, false)) {
                        if (dude > maxDude) maxDude = dude;
                        willBeAdjacent[dude] = 1;  // opponent will be adjacent
                    };
                };
            };
        };
    };
    for (dx = -1; dx <= this.width; dx++) {
        for (dy = -1; dy <= this.height; dy++) {
            if (Drawtile.coordsOnMap(oldX + dx, oldY + dy, 1, 1)) {
                dude = Drawtile.getCombatantInCell(oldX + dx, oldY + dy, 1, 1, this.self);
                if (dude != NO_DUDE) {
                    if (combatData.getCombatantPtr(dude).charOnCombatMap(false, false)) {
                        if (dude > maxDude) maxDude = dude;
                        wasAdjacent[dude] = 1;  // opponent was adjacent
                    };
                };
            };
        };
    };



    qcomb = Globals.GetQueuedCombatants();

    // Perform Guard Attacks first.  They will get
    // pushed onto QComb and then the Free Attacks will
    // get pushed on top of them/
    // In that way, the Free Attacks will be performed first.
    for (dude = 0; dude <= maxDude; dude++) {
        if (willBeAdjacent[dude] && !wasAdjacent[dude]) {
/*#ifdef TraceFreeAttacks       // PORT NOTE: This seemed to be off
            {
                COMBATANT * pOtherCombatant;
                CString direction;
                direction = "Approaches";
                pOtherCombatant = GetCombatantPtr(dude);
                WriteDebugString("TFA - %s %s %s\n", GetName(), direction, pOtherCombatant -> GetName());
            };
#endif*/
            tempCOMBATANT = Globals.GetCombatantPtr(dude);              // PORT NOTE: yes this is different from getCombatantPtr with a lower case g
            Globals.ASSERT(tempCOMBATANT != null);
            if (tempCOMBATANT != null) {
                if (tempCOMBATANT.m_ICS == individualCombatantState.ICS_Casting) {
                    continue;
                };
                if (tempCOMBATANT.GetIsFriendly() != this.GetIsFriendly()) {
                    if (!(combatData.IsValidTarget(tempCOMBATANT, this, null).answer)) {   // PORT NOTE:  the last parameter here can be affected in this method, but is not if null is passed in
//#ifdef TraceFreeAttacks       // PORT NOTE: This seemed to be off
//                        WriteDebugString("'IsValidTarget' script says that this is not a valid target\n");
//#endif
                        continue;
                    };
                    var hookParameters = new HOOK_PARAMETERS();
                    var scriptContext = new SCRIPT_CONTEXT();
                    var result = "";
                    hookParameters[5] = parseInt(tempCOMBATANT.AttacksRemaining());
                    hookParameters[6] = tempCOMBATANT.IsPartyMember() ? "Y" : "N";
                    hookParameters[7] = tempCOMBATANT.OnAuto(false) ? "Y" : "N";
                    scriptContext.SetAttackerContext(tempCOMBATANT);
                    scriptContext.SetTargetContext(this);

                    {
                        var performGuardAttack = false;
                        performGuardAttack = false;
                        if (tempCOMBATANT.canAttack(this.self,
                            newX, newY,
                            tempCOMBATANT.GetNbrAttacks(),
                            this.FreeAttackDistance(),
                            false)) {
/*#ifdef TraceFreeAttacks
                            WriteDebugString("TFA - Call script 'Guarding-CanGuardAttack(%s,%s,%s)'\n",
                                hookParameters[5],
                                hookParameters[6],
                                hookParameters[7]);
#endif*/
                            result = tempCOMBATANT.RunCombatantScripts(
                                SPECAB.GUARDING_CAN_GUARD_ATTACK,
                                SPECAB.ScriptCallback_RunAllScripts,
                                null,
                                "Check opponent free attack");
//#ifdef TraceFreeAttacks
//                            WriteDebugString("TFA - script 'Guarding-CanGuardAttack' returned \"%s\"\n", result);
//#endif
                            scriptContext.Clear();
                            if (!result.IsEmpty()) {
                                if (result[0] == 'Y') {
                                    performGuardAttack = true;
                                };
                            };

                        };
                        if (performGuardAttack) {
//#ifdef TraceFreeAttacks
//                            WriteDebugString("TFA - queueing %s to make guard attack\n", GetCombatantPtr(dude) -> GetName());
//#endif
                            // Restore my original position until after the opponent has made
                            // his free attack.
                            // First, check that we are still at the top of the queue.  There may
                            // have been other Free Attackers.
                            if (qcomb.Top() == this.self)  // There may be attackers befor this one.
                            // So we only do this once.
                            {
                                qcomb.SetXY(newX, newY);
                                // remove dude from new location
                                Drawtile.placeCombatant(this.x, this.y, NO_DUDE, this.width, this.height);
                                this.x = oldX;
                                this.y = oldY;
                                Drawtile.placeCombatant(this.x, this.y, this.self, this.width, this.height); // added 20160927 PRS
                            };
                            // queue up the attackers
                            qcomb.Push(dude, false, 0, 1);
                            // give self as target to each attacker
                            tempCOMBATANT.turnIsDone = false;
                            tempCOMBATANT.RemoveAllTargets();
                            tempCOMBATANT.AddTarget(this.self, true);
                            // set each attacker in attack mode (if possible)
                            found = true;
/*#ifdef TraceFreeAttacks
                        }
                        else {
                            WriteDebugString("TFA - No Guard Attack is possible\n");
#endif*/
                        };
                    };
/*#ifdef TraceFreeAttacks
                }
                else {
                    WriteDebugString("TFA - But they are friends\n");
#endif*/
                };
            };
        };
    }
    for (dude = 0; dude <= maxDude; dude++) {
        if (wasAdjacent[dude] && !willBeAdjacent[dude]) {
/*#ifdef TraceFreeAttacks
            {
                COMBATANT * pOtherCombatant;
                CString direction;
                direction = "Retreats From";
                pOtherCombatant = GetCombatantPtr(dude);
                WriteDebugString("TFA - %s(%d) %s %s(%d)\n", GetName(), self, direction, pOtherCombatant -> GetName(), dude);
            };
#endif*/
            tempCOMBATANT = Globals.GetCombatantPtr(dude);
            Globals.ASSERT(tempCOMBATANT != null);
            if (tempCOMBATANT != null) {
                if (tempCOMBATANT.m_ICS == individualCombatantState.ICS_Casting) {
                    continue;
                };
                if (tempCOMBATANT.GetIsFriendly() != this.GetIsFriendly()) {
                    if (!(combatData.IsValidTarget(tempCOMBATANT, this, null).answer)) {
//#ifdef TraceFreeAttacks
//                        WriteDebugString("'IsValidTarget' script says that this is not a valid target\n");
//#endif
                        continue;
                    };
                    var hookParameters = new HOOK_PARAMETERS();
                    var scriptContext = new SCRIPT_CONTEXT();
                    var result = "";
                    hookParameters[5] = parseInt(tempCOMBATANT.AttacksRemaining());
                    hookParameters[6] = tempCOMBATANT.IsPartyMember() ? "Y" : "N";
                    hookParameters[7] = tempCOMBATANT.OnAuto(false) ? "Y" : "N";
                    hookParameters[8] = parseInt(tempCOMBATANT.GetNbrAttacks());
                    scriptContext.SetAttackerContext(tempCOMBATANT);
                    scriptContext.SetTargetContext(this);

                    {

                        var freeAttackCount = 0;
                        freeAttackCount = 0;
                        if (tempCOMBATANT.canAttack(this.self,
                            oldX, oldY,
                            tempCOMBATANT.m_pCharacter.GetNbrAttacks(),
                            this.FreeAttackDistance(),
                            false)) {
/*#ifdef TraceFreeAttacks
                            WriteDebugString("TFA - Call script 'FreeAttack-CanFreeAttack(%s,%s,%s,%s)\n",
                                hookParameters[5],
                                hookParameters[6],
                                hookParameters[7],
                                hookParameters[8]);
#endif*/
                            result = tempCOMBATANT.RunCombatantScripts(
                                SPECAB.FREEATTACK_CAN_FREE_ATTACK,
                                SPECAB.ScriptCallback_RunAllScripts,
                                null,
                                "Can opponent free attack");
//#ifdef TraceFreeAttacks
//                            WriteDebugString("TFA - Script 'FreeAttack-CanFreeAttack' returned \"%s\"\n",
//                                result);
//#endif
                            scriptContext.Clear();
                            if (!result.IsEmpty()) {
                                if (result[0] == 'Y') {
                                    freeAttackCount = 1 + tempCOMBATANT.AttacksRemaining();
                                }
                                else {
                                    var n = 0;
                                    n = parseInt(result);
                                    if (n > 0) {
                                        freeAttackCount = n;
                                    };
                                };
                            };
                        };
                        if (freeAttackCount) {               
//#ifdef TraceFreeAttacks
//                            WriteDebugString("TFA - queueing %s(%d) to make free attack\n", GetCombatantPtr(dude) -> GetName(), dude);
//#endif
                            // queue up the attackers
                            if (qcomb.Top() == this.self) // There may be several Free Attackers before this one!
                            // So we only do this once.
                            {
                                qcomb.SetXY(newX, newY);
                                // remove dude from new location
                                Drawtile.placeCombatant(this.x, this.y, NO_DUDE, this.width, this.height);
                                this.x = oldX;
                                this.y = oldY;
                                // put dude into original location
                                Drawtile.placeCombatant(this.x, this.y, this.self, this.width, this.height); // added 20160927 PRS
                            };
                            qcomb.Push(dude, false, freeAttackCount, 0);
                            // give self as target to each attacker
                            tempCOMBATANT.turnIsDone = false;
                            tempCOMBATANT.RemoveAllTargets();
                            tempCOMBATANT.AddTarget(this.self, true);
                            // set each attacker in attack mode (if possible)
                            //tempCOMBATANT->StartAttack(self, qcomb->NumFreeAttacks());
                            found = true;
/*#ifdef TraceFreeAttacks
                        }
                        else {
                            WriteDebugString("TFA - No Free Attack is possible\n");
#endif*/
                        };
                    };
                };
            };
        };
    }
    return found;
}



//*****************************************************************************
// NAME: canAttack
//
// PURPOSE:  
// check non-magical attack capabilities
//*****************************************************************************
COMBATANT.prototype.canAttack = function(targ, targetX, targetY, additionalAttacks, DistanceFunction, canAttackSelf) {
    if (this.availAttacks + additionalAttacks <= 0)
        return false;

    if (targ == NO_DUDE)
        return false;

    if ((this.availAttacks + additionalAttacks < 1.0)
        && (Globals.GetCurrentRound() - this.lastAttackRound <= 1))
        return false;

    // cannot attack yourself
    if ((targ == this.self) && !canAttackSelf)
        return false;

    var targCOMBATANT;
    targCOMBATANT = Globals.GetCombatantPtr(targ);
    Globals.ASSERT(targCOMBATANT != null);
    if (targCOMBATANT == null) return false;
    if (targetX < 0) targetX = targCOMBATANT.x;
    if (targetY < 0) targetY = targCOMBATANT.y;

    // if attacking dude on same side of battle
    if (targCOMBATANT.GetIsFriendly() == this.GetIsFriendly()) {
        // AI will never attack dudes on same side
        if (this.OnAuto(false) && !canAttackSelf) // includes all monsters (always auto), and auto dudes.
        {
            return false;
        }
        else if ((targCOMBATANT.GetType() == CHAR_TYPE) && !canAttackSelf) {
            // cannot attack pc party members with weapon
            return false;
        }
        else if ((targCOMBATANT.GetType() == NPC_TYPE) && (!targCOMBATANT.m_pCharacter.IsPreGen)) {
            // if player chooses to attack friendly npc, the npc changes sides,
            // even if unable to perform attack due to following checks.
            //
            // This does not belong here.  We are only attempting to determine
            // if we CAN attack targCOMBATANT.  We have not actually performed
            // any attack yet.  Perhaps this line of code belongs elsewhere.
            // PRS 20140916
            //targCOMBATANT->friendly = !targCOMBATANT->friendly;
            //
            //
            //
            // npc will leave the battle (and the party after combat)?
            //targCOMBATANT.SetMorale(0);
        }
    }
    var dis = DistanceFunction(this.self, this.x, this.y, targ, targetX, targetY);
    var mywpnitemidx = this.m_pCharacter.myItems.GetReadiedItem(Items.WeaponHand, 0);
    if (mywpnitemidx != Items.NO_READY_ITEM) {
        // using a weapon rather than natural attack types (ie claws/jaws/fists)
        var wpn_ID;
        wpn_ID = this.m_pCharacter.myItems.GetItem(mywpnitemidx);

        if (this.m_pCharacter.myItems.GetQty(mywpnitemidx) <= 0)
            return false;

        if (!Items.WpnCanAttackAtRange(wpn_ID, dis))
            return false;

        switch (itemData.GetWpnType(wpn_ID)) {
            case weaponClassType.NotWeapon: // not weapon, must be used, invokes special abilities
                //if (isMagical(wpn_giID))
                if (Items.isMagical(wpn_ID)) {
                    if (this.m_pCharacter.myItems.GetCharges(mywpnitemidx) <= 0)
                        return false;
                }
                else
                    return false;
                break;

            case weaponClassType.HandBlunt: // hand, no throw, blunt
            case weaponClassType.HandCutting: // hand, no throw, cutting
            case weaponClassType.HandThrow: // hand or throw
            case weaponClassType.SlingNoAmmo: // sling
            case weaponClassType.SpellCaster:
            case weaponClassType.SpellLikeAbility:
                //
                // handled by WpnCanAttackAtRange()
                //
                break;

            case weaponClassType.Bow: // bow
            case weaponClassType.Crossbow: // crossbow
                {
                    var myammoitemidx = this.m_pCharacter.myItems.GetReadiedItem(Items.AmmoQuiver, 0);

                    // arrows or bolts must be readied in AmmoQuiver
                    if (myammoitemidx == Items.NO_READY_ITEM)
                        return false;

                    var ammo_ID = this.m_pCharacter.myItems.GetItem(myammoitemidx);

                    if (itemData.GetWpnType(ammo_ID) != weaponClassType.Ammo)
                        return false;

                    // ammo class must match between weapon and ammo
                    var myammoclass = this.m_pCharacter.myItems.GetAmmoClass(myammoitemidx);
                    if (myammoclass == null || myammoclass == "") return false; // might be wand,potion,amulet,etc

                    if (myammoclass != this.m_pCharacter.myItems.GetAmmoClass(mywpnitemidx))
                        return false;

                    // must have non-zero qty of ammo
                    if (this.m_pCharacter.myItems.GetQty(myammoitemidx) <= 0)
                        return false;
                }
                break;

            case Throw:
                //
                // handled by WpnCanAttackAtRange()
                //
                break;

            case weaponClassType.Ammo: // ammo - arrows or bolts
                return false;

            default:
                Globals.WriteDebugString("Unhandled item Wpn_Type in canAttack()\n");
                return false;
        }
    }
    else {
        // use natural attack weapons (ie claws, jaws, paws, fists)
        if (dis > 1)                // PORT NOTE:  - I think this may be a bug.  Should be able to move into a diagonal attack - which would make the distance 2?
            return false;
    }

    // passed all tests so far, now check for line of sight
    if (!Drawtile.HaveLineOfSight(this.GetCenterX(), this.GetCenterY(), targetX, targetY, null))
        return false;

    if (!(combatData.IsValidTarget(this, targCOMBATANT, null).answer)) return false;
    
    if (dis > 1) {
        if (!this.GetAdjDetectingInvisible()) {
            // cannot attack invisible targets with ranged weapons
            if (targCOMBATANT.GetAdjSpecAb(SPECAB.SA_Invisible))
                return false;

            if (!(this.GetUndeadType() == null || this.GetUndeadType() == "")) {
                if (targCOMBATANT.GetAdjSpecAb(SPECAB.SA_InvisibleToUndead))
                    return false;
            }

            if (this.IsAnimal()) {
                if (targCOMBATANT.GetAdjSpecAb(SPECAB.SA_InvisibleToAnimals))
                    return false;
            }
        }
    }

    return true;
}


COMBATANT.prototype.AddTarget = function (newTarget, freeAttack) {

    if (newTarget == NO_DUDE)
        return;

    this.RemoveTarget(newTarget);

    if (this.CanTarget(freeAttack)) {
        // if using weapon, change to new target (only one allowed)

        var targCOMBATANT;
        targCOMBATANT = Globals.GetCombatantPtr(newTarget);
        Globals.ASSERT(targCOMBATANT != null);
        if (targCOMBATANT == null) return;

        var dist = Drawtile.Distance6(this.self, this.x, this.y,
            targCOMBATANT.self, targCOMBATANT.x, targCOMBATANT.y);

        this.combattargets.Insert(newTarget,
            this.MakeTargetKey(newTarget, dist));
    }

    this.SetCurrTarget();
}


COMBATANT.prototype.RemoveTarget = function (target) {
    if (this.combattargets.GetCount() == 0)
        return;

    var pos = combattargets.GetHeadPosition();
    while (pos != null) {
        if (combattargets.PeekAtPos(pos) == targ) {
            combattargets.RemoveAt(pos);
            return;
        }
        else
            combattargets.GetNext(pos);
    }
}
