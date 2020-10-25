
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

var Warrior = Deserialize("Warrior.chr");
var Rogue = Deserialize("Rogue.chr");

var cWarrior = new COMBATANT();
cWarrior.m_pCharacter = Warrior;

//cWarrior.StartAttack(1);

SPECAB.loadData(specialAbilitiesData, "C:\\Users\\Shadow\\Downloads\\Full_Release_191031\\TutorialDesign.dsn\\Data\\specialAbilities.dat");
Globals.logDebuggingInfo = true;

var combatEventData = new COMBAT_EVENT_DATA();
var combatData = new COMBAT_DATA();    // This is pretty much the combat "map" and all data on it
party.Posx = 10;
party.Posy = 10;
globalData.SetMaxPCs(2);
globalData.SetMinPCs(2);
globalData.SetMaxPartySize(2);

party.addTempToParty(Warrior);
combatEventData.distance = eventDistType.UpClose;
combatEventData.m_UseOutdoorMap = false; // only outdoor stub is in place right now
combatData.InitCombatData(combatEventData);
Globals.debug("combatData.NumCombatants():" + combatData.NumCombatants());

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


//consoleResults.payload = dataStr;
consoleResults.payload = data;

