function EVENT_CONTROL() {

	this.eventStatusUnused = 0;
	this.eventResultUnused = 0;
	this.chainTrigger = chainTriggerType.getByNumber(0);
	this.eventTrigger = eventTriggerType.getByNumber(0);
	this.facing = eventDirType.getByNumber(0);
	this.raceID = "";
	this.classBaseclassID = "";
	this.gender = genderType.getByNumber(0); // stored as attribute
	this.itemID = "";
	this.quest = 0;
	this.chance = 0;
	this.specialItem = 0;
	this.specialKey = 0;
	this.characterID = "";
	this.partyX = 0;
	this.partyY = 0;
	this.memorizedSpellID = "";
	this.memorizedSpellClass = "";
	this.memorizedSpellLevel = "";
	this.gpdlData = 0;
	this.gpdlIsBinary = false;

	this.eventcontrol_asl = new A_ASLENTRY_L();
	this.temp_asl = new A_ASLENTRY_L();   

}


EVENT_CONTROL.prototype.Clear = function()
{
	this.eventStatusUnused = 0;
	this.eventResultUnused = 0;
	this.onceOnly = false;
	this.chainTrigger = chainTriggerType.AlwaysChain;
	this.eventTrigger = eventTriggerType.AlwaysTrigger;
	this.facing = eventDirType.Any;
	//race = Human;
	this.raceID = "";
	//charClass = Cleric;
	this.classBaseclassID.classID = "";
	this.classBaseclassID.baseclassID = "";
	//m_giID.Clear();
	this.itemID = "";
	this.quest = -1;
	this.chance = 100;
	this.gender = genderType.Male;
	this.specialItem = -1;
	this.specialKey = -1;
	//npc=-1;
	this.characterID = "";
	this.gpdlData = "";
	this.gpdlIsBinary = false;
	this.partyX = this.partyY = -1;
	//memorizedSpellDbKey.Clear();
	this.memorizedSpellID = "";
	this.memorizedSpellClass = 0;
	this.memorizedSpellLevel = 0;
	this.eventcontrol_asl.Clear();
	this.temp_asl.Clear();
}