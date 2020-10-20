function PARTY() {
    var stateStack = new TASK_STATE_DATA();
    var name = "";
    var adventuring = false;
    var areaView = false;
    var searching = false;
    var looking = false; // stays true during one game loop only
    var resting = false;
    var level = 0;   //BYTE
    var speed = 0;   //BYTE
    var facing = 0;   //BYTE
    var activeCharacter = 0;   //BYTE  // index into PARTY::characters array
    var activeItem = 0;   //BYTE
    var tradeItem = 0;   //BYTE
    var tradeGiver = 0;   //BYTE
    var skillLevel = 0;   //BYTE
    var numCharacters = 0;   //BYTE
    var moneyPooled;
    var Posx = 0;
    var Posy = 0;
    var PrevPosx = 0;
    var PrevPosy = 0;
    var PrevRelFacing = 0;
    var tradeQty = 0;
    var days = 0;
    var hours = 0;
    var minutes = 0;
    var drinkPoints = 0;
    var ActiveScripter = 0;
    var DayMusic = 0;
    //********  These variables used to be defined as static 
    //********  variables in PARTY::ProcessTimeSensitiveData().
    //********  That made them impossible to see in the debugger.
    //********  But, more importantly, they are lost when the
    //********  player does a save/reload.  Someday, we must
    //********  include these variables in the PARTY :Serialize.
    var minutesRested;
//#ifdef OLD_AUTO_HEAL                          //PORT NOTE: ???
//    int minutesUntilAutoHeal;
//#endif
    //********
    var poolSack = new MONEY_SACK();
    var eventFlags = new EVENT_TRIGGER_DATA();
    var visitData = new VISIT_DATA();
    var blockageData = new BLOCKAGE_STATUS();
    var journal = JOURNAL_DATA();
    var characters = [MAX_PARTY_MEMBERS];
    var party_asl = A_ASLENTRY_L;
}

PARTY.prototype.GetNextUniquePartyID = function () {
    var i;
    var used = [MAX_PARTY_MEMBERS];
    //memset(used, MAX_PARTY_MEMBERS + 1, sizeof(used)); // set to impossible value  // PORT NOTE: Ignored

    // collect current uniquePartyID values
    for (i = 0; i < this.numCharacters; i++)
        used[i] = characters[i].uniquePartyID;

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
        if (characters[i].IsSameCharacter(luckyDude))
            return characters[i];
    }

    if (numCharacters < this.GetMaxPartyMembers()) {
        characters[numCharacters] = luckyDude;
        characters[numCharacters].uniquePartyID = GetNextUniquePartyID();
        characters[numCharacters].SetPartyMember();
        if (characters[numCharacters].GetType() != NPC_TYPE)
            characters[numCharacters].origIndex = -1;
        characters[numCharacters].UpdateSpellAbility();
        numCharacters++;
        return characters[numCharacters - 1];
    };
    return null;
}