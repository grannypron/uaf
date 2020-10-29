function eventTriggerType() {
    this.AlwaysTrigger = 0;
    this.PartyHaveItem = 1; this.PartyNotHaveItem = 2;
    this.Daytime = 4; this.Nighttime = 3;
    this.RandomChance = 5;
    this.PartySearching = 6; this.PartyNotSearching = 7;
    this.FacingDirection = 8;
    this.QuestComplete = 9; this.QuestFailed = 10; this.QuestInProgress = 11;
    this.PartyDetectingTraps = 12; this.PartyNotDetectingTraps = 13;
    this.PartySeeInvisible = 14; this.PartyNotSeeInvisible = 15;
    this.ClassInParty = 16; this.ClassNotInParty = 17;
    this.RaceInParty = 18; this.RaceNotInParty = 19;
    this.QuestPresent = 20; this.QuestNotPresent = 21;
    this.GenderInParty = 22; this.GenderNotInParty = 23;
    this.FacingDirectionAnyTime = 24;
    this.NPCInParty = 25; this.NPCNotInParty = 26;
    this.ExecuteGPDL = 27;
    this.SpellMemorized = 28;
    this.PartyAtXY = 29;
    this.BaseclassInParty = 30; this.BaseclassNotInParty = 31;
    this.PartyHaveSpecialItem = 32; this.PartyNotHaveSpecialItem = 33;
    this.PartyHaveSpecialKey = 34; this.PartyNotHaveSpecialKey = 35;
    this.QuestStageEqual = 36; this.QuestStageNotEqual = 37;
    this.LAST_EVENT_TRIGGER = 38;

};

function eventTriggerTypeObj() {
    eventTriggerType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}
