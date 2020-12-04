/** Requires TestLoadSAs.js  **/


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
    loadRaces();
    loadAbilities();
    loadClasses();
    loadMonsters();
    loadBaseClassStats();
}

function loadRaces() {
    var data = new RACE_DATA();
    data.m_name = "Monster";
    raceData.AddRace(data);
}

function loadBaseClassStats() {
    var bcd = new BASE_CLASS_DATA();
    bcd.m_name = "fighter";
    baseclassData.Add(bcd);
}
function loadMonsters() {

    monsterData.MonsterData[0] = new MONSTER_DATA();
    monsterData.MonsterData[0].Name = "Kobold";
    monsterData.MonsterData[0].monsterID = "Kobold";
    monsterData.MonsterData[0].classID = "Fighter";
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

loadLibraryStub();


var Warrior = Deserialize("Warrior.chr");
Warrior.SetStatus(charStatusType.Okay);


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

Warrior.m_pCombatant = combatData.m_aCombatants[0];
Warrior.determineNbrAttacks();

var actor;
var hookParameters = new HOOK_PARAMETERS();
var scriptContext = new SCRIPT_CONTEXT();

actor = Warrior.GetContextActor();
RunTimeIF.SetCharContext(actor);
scriptContext.SetCombatantContext(combatData.m_aCombatants[0]);
var minmax = [- 1, 9999999];
var result = combatData.m_aCombatants[0].RunCombatantScripts(SPECAB.GET_NUMBER_OF_ATTACKS,
        SPECAB.ScriptCallback_MinMax,
        minmax,
        "Determine number of attacks");

Globals.ASSERT(result != null && result != undefined, "TestSpecialAbilities.js - 1");
Globals.ASSERT(result == "", "TestSpecialAbilities.js - 2");  // This is bugged in the version I have and the default returns 1.0 anyway