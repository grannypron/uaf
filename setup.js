
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
}

function loadRaces() {
var data = new RACE_DATA();
data.m_name = "Monster";
raceData.AddRace(data);
}

function loadMonsters() {

monsterData.MonsterData[0] = new MONSTER_DATA();
monsterData.MonsterData[0].Name = "Kobold";
monsterData.MonsterData[0].monsterID = "Kobold";
monsterData.MonsterData[0].classID = "Fighter";
//icon file = icon_Kobold.png, 0, 2, 48, 48, 0, 1, 2
//miss sound = sound_Miss.wav
//hit sound = sound_Hit.wav
//move sound = sound_MonsterMoveStep.wav
//death sound = sound_MonsterDeathMedium.wav
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
monsterData.MonsterData[0].m_type = MONSTER_TYPE;

//Class = Fighter
monsterData.MonsterData[0].Race = RACE_DATA_TYPE.Monster;
/*form = none
penalty = GnomeTHAC0 + RangerDmg
immunity = none
Misc Options = can be held / charmed
item = Dagger
item = Buckler
attack = 4, 1, 0, attacks,
Undead = none
*/

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



var Warrior = new CHARACTER();
Warrior.classID = "Fighter";
Warrior.hitPoints = 10;
Warrior.maxHitPoints = 20;
Warrior.age = 20;
Warrior.maxAge = 100;
Warrior.alignment = 0;
Warrior.encumbrance = 10;





var cWarrior = new COMBATANT();
cWarrior.m_pCharacter = Warrior;

//cWarrior.StartAttack(1);

loadLibraryStub();
//SPECAB.loadData(specialAbilitiesData, "C:\\Users\\Shadow\\Downloads\\Full_Release_191031\\TutorialDesign.dsn\\Data\\specialAbilities.dat");
Globals.logDebuggingInfo = true;

var combatEventData = new COMBAT_EVENT_DATA();
combatEventData.monsters = new MONSTER_EVENT_DATA();
var monsterEvent = new MONSTER_EVENT();
monsterEvent.UseQty = MONSTER_EVENT.meUseQty;
monsterEvent.UseQty = MONSTER_EVENT.meUsePercent;
monsterEvent.qtyDiceSides = 10;
monsterEvent.qtyDiceQty = 1;
monsterEvent.qtyBonus = 2;
monsterEvent.qty = 10;
monsterEvent.qty = 3;
monsterEvent.monsterID = "Kobold";
monsterEvent.m_type = MONSTER_TYPE;
combatEventData.distance = eventDistType.UpClose;
combatEventData.monsters.Add(monsterEvent);
//combatEventData.randomMonster = true;   // This seems to cause an NPE at Line 306 of COMBAT_DATA - "pSaveCharPointer is null"
combatEventData.UseQty = MONSTER_EVENT.meUsePercent;

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

var dataStr = "";
var data = [];
for (i = 0; i < Drawtile.MAX_TERRAIN_HEIGHT; i++) {
    dataStr += "[";
    data[i] = [];
    for (j = 0; j < Drawtile.MAX_TERRAIN_WIDTH; j++) {
        dataStr += Drawtile.terrain[i][j].tileIndex + ",";
        data[i][j] = Drawtile.terrain[i][j].tileIndex;
    }
    dataStr += "]\n";
}

Globals.debug(dataStr);


//consoleResults.payload = dataStr;
consoleResults.payload = data;
