function PARTY() {
    this.stateStack = new TASK_STATE_DATA();
    this.name = "";
    this.adventuring = false;
    this.areaView = false;
    this.searching = false;
    this.looking = false; // stays true during one game loop only
    this.resting = false;
    this.level = 0;   //BYTE
    this.speed = 0;   //BYTE
    this.facing = 0;   //BYTE
    this.activeCharacter = 0;   //BYTE  // index into PARTY::characters array
    this.activeItem = 0;   //BYTE
    this.tradeItem = 0;   //BYTE
    this.tradeGiver = 0;   //BYTE
    this.skillLevel = 0;   //BYTE
    this.numCharacters = 0;   //BYTE
    this.moneyPooled;
    this.Posx = 0;
    this.Posy = 0;
    this.PrevPosx = 0;
    this.PrevPosy = 0;
    this.PrevRelFacing = 0;
    this.tradeQty = 0;
    this.days = 0;
    this.hours = 0;
    this.minutes = 0;
    this.drinkPoints = 0;
    this.ActiveScripter = 0;
    this.DayMusic = 0;
    //********  These variables used to be defined as static 
    //********  variables in PARTY::ProcessTimeSensitiveData().
    //********  That made them impossible to see in the debugger.
    //********  But, more importantly, they are lost when the
    //********  player does a save/reload.  Someday, we must
    //********  include these variables in the PARTY :Serialize.
    this.minutesRested;
//#ifdef OLD_AUTO_HEAL                          //PORT NOTE: ???
//    int minutesUntilAutoHeal;
//#endif
    //********
    this.poolSack = new MONEY_SACK();
    this.eventFlags = new EVENT_TRIGGER_DATA();
    this.visitData = null;//new VISIT_DATA();
    this.blockageData = new BLOCKAGE_STATUS();
    this.journal = null;//JOURNAL_DATA();
    this.characters = [MAX_PARTY_MEMBERS];
    this.party_asl = A_ASLENTRY_L;
}

PARTY.prototype.GetNextUniquePartyID = function () {
    var i;
    var used = [MAX_PARTY_MEMBERS];
    //memset(used, MAX_PARTY_MEMBERS + 1, sizeof(used)); // set to impossible value  // PORT NOTE: Ignored

    // collect current uniquePartyID values
    for (i = 0; i < this.numCharacters; i++)
        used[i] = this.characters[i].uniquePartyID;

    // Search for value that isn't already used by party member.
    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        var found = false;
        for (u = 0; (u < MAX_PARTY_MEMBERS) && (!found); u++)
        {
            if (used[u] == i)
                found = true;
        }
        if (!found) return i;
    }
    Globals.die(0xab529); // should not be possible to reach this
    return this.numCharacters; // old default behavior
}

PARTY.prototype.addTempToParty = function(luckyDude)
{
    // To leave room for future NPC additions to party,
    // only allow up to GetMaxPartyCharacters() of player
    // created characters into the party list.
    //
    for (i = 0; i < this.numCharacters; i++)
    {
        if (this.characters[i].IsSameCharacter(luckyDude))
            return this.characters[i];
    }
    if (this.numCharacters < Globals.GetMaxPartyMembers()) {
        this.characters[this.numCharacters] = luckyDude;
        this.characters[this.numCharacters].uniquePartyID = this.GetNextUniquePartyID();
        this.characters[this.numCharacters].SetPartyMember();
        if (this.characters[this.numCharacters].GetType() != NPC_TYPE)
            this.characters[this.numCharacters].origIndex = -1;
        this.characters[this.numCharacters].UpdateSpellAbility();
        this.numCharacters++;
        return this.characters[this.numCharacters - 1];
    };
    return null;
}

PARTY.prototype.GetPartyFacing = function()
{
    return (this.facing);
}