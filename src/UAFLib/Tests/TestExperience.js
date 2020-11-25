
function Deserialize(filename, debug) {
    var character = new CHARACTER();
    var path = "C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\Tests\\" + filename;
    var fs = System.IO.File.OpenRead(path);
    var ar = new CAR(fs, 6.0);

    //Read the first 16 header bytes because this isn't covered in the CHARACTER's serialize method
    ar.readInt(); ar.readInt(); ar.readInt(); ar.readInt();

    if (debug) {
        ar.MFCSerializer.debug = true;
    }
    character.SerializeCAR(ar, 6.0);
    fs.Close();
    return character;
}

function loadLibraryStub() {
    loadAbilities();
    loadClasses();
    loadMonsters();
    loadRaces();
}

function loadRaces() {
    var data = new RACE_DATA();
    data.m_name = "Monster";
    raceData.AddRace(data);
}

function loadClasses() {
    var data = new CLASS_DATA();
    data.m_name = "Fighter";
    data.m_baseclasses.baseclasses.push("fighter");
    classData.AddClass(data);

    data2 = new CLASS_DATA();
    data2.m_name = "Thief";
    data2.m_baseclasses.baseclasses.push("thief");
    classData.AddClass(data2);

}


function loadMonsters() {

    monsterData.MonsterData[0] = new MONSTER_DATA();
    monsterData.MonsterData[0].Name = "Kobold";
    monsterData.MonsterData[0].monsterID = "Kobold";
    monsterData.MonsterData[0].classID = "Fighter";
    monsterData.MonsterData[0].raceID = "Monster";
    monsterData.MonsterData[0].intelligence = 9;
    monsterData.MonsterData[0].Armor_Class = 10;
    monsterData.MonsterData[0].Movement = 6;
    monsterData.MonsterData[0].Hit_Dice = 0.500000;
    monsterData.MonsterData[0].UseHitDice = true;
    monsterData.MonsterData[0].Hit_Dice_Bonus = 0;
    monsterData.MonsterData[0].THAC0 = 20;
    monsterData.MonsterData[0].Magic_Resistance = 0;
    monsterData.MonsterData[0].Size = creatureSizeType.Medium;
    monsterData.MonsterData[0].Morale = 25;
    monsterData.MonsterData[0].XP_Value = 7;
    monsterData.MonsterData[0].MoveSound = new SOUND_BYTE();
    monsterData.MonsterData[0].MoveSound.hSound = "sound_CharMove";
    monsterData.MonsterData[0].m_type = MONSTER_TYPE;
    monsterData.MonsterData[0].Race = RACE_DATA_TYPE.Monster;
    monsterData.MonsterData[0].attackData.monsterAttackDetails.mList[0] = new ATTACK_DETAILS()
    monsterData.MonsterData[0].attackData.monsterAttackDetails.mList[0].sides = 4;
    monsterData.MonsterData[0].attackData.monsterAttackDetails.mList[0].nbr = 1;
    monsterData.MonsterData[0].attackData.monsterAttackDetails.mList[0].bonus = 0;

}

function loadAbilities() {
    loadAbility("Strength");
    loadAbility("Intelligence");
    loadAbility("Wisdom");
    loadAbility("Dexterity");
    loadAbility("Charisma");
}

function loadAbility(abilityName) {
    var data = new ABILITY_DATA();
    data.m_name = abilityName;
    data.m_abbreviation = abilityName.substr(0, 3).toUpperCase();
    data.m_roll.m_Text = ""
    abilityData.AddAbility(data);
}


var bcd = new BASE_CLASS_DATA();
bcd.m_name = "fighter";
baseclassData.Add(bcd);

var Warrior = Deserialize("Warrior.chr");
Warrior.SetStatus(charStatusType.Okay);
var bcs = new BASECLASS_STATS();
bcs.baseclassID = "fighter";
bcs.pBaseclassData = bcd;
Warrior.baseclassStats.push(bcs);

loadLibraryStub();
SPECAB.loadData(specialAbilitiesData, "C:\\Users\\Shadow\\Downloads\\Full_Release_191031\\TutorialDesign.dsn\\Data\\specialAbilities.dat");

Globals.logDebuggingInfo = true;


party.addTempToParty(Warrior);

Globals.ASSERT(Warrior.GetCurrExp("fighter") == 0, "TestExperience.js - 1");
Warrior.giveCharacterExperience(100, false);
Globals.ASSERT(Warrior.GetCurrExp("fighter") == 100, "TestExperience.js - 2");


Globals.debug(Warrior.GetCurrExp("fighter"));




var combatEventData = new COMBAT_EVENT_DATA();
combatEventData.monsters = new MONSTER_EVENT_DATA();
var monsterEvent = new MONSTER_EVENT();
monsterEvent.UseQty = MONSTER_EVENT.meUseQty;
monsterEvent.qty = 3;
monsterEvent.monsterID = "Kobold";
monsterEvent.m_type = MONSTER_TYPE;
combatEventData.monsters.Add(monsterEvent);

var combatData = new COMBAT_DATA();    // This is pretty much the combat "map" and all data on it
party.Posx = 10;
party.Posy = 10;
globalData.SetMaxPCs(2);
globalData.SetMinPCs(2);
globalData.SetMaxPartySize(2);


party.addTempToParty(Warrior);
combatEventData.distance = eventDistType.UpClose;
combatEventData.m_UseOutdoorMap = false; // only outdoor stub is in place right now
combatEventData.direction = eventDirType.North;
combatData.InitCombatData(combatEventData);



Globals.ASSERT(combatData.m_aCombatants[1].getCharExpWorth() > 0);