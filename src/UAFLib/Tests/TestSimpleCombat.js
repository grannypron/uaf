
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


var combatEventData = new COMBAT_EVENT_DATA();
var combatData = new COMBAT_DATA();    // This is pretty much the combat "map" and all data on it
party.Posx = 10;
party.Posy = 10;
party.addTempToParty(cWarrior);
combatEventData.distance = eventDistType.UpClose;
combatEventData.m_UseOutdoorMap = false; // only outdoor stub is in place right now
combatData.InitCombatData(combatEventData);
Globals.debug("combatData.NumCombatants():" + combatData.NumCombatants());

// Combatant.self gets set here:
//UAFWin.exe!COMBATANT:: operator = (COMBATANT & src) Line 194	C++
//UAFWin.exe!COMBAT_DATA:: AddMonstersToCombatants() Line 890	C++
//UAFWin.exe!COMBAT_DATA:: AddCombatants() Line 505	C++
//    > UAFWin.exe!COMBAT_DATA:: InitCombatData(COMBAT_EVENT_DATA * event) Line 219	C++
//UAFWin.exe!COMBAT_EVENT_DATA:: OnInitialEvent() Line 15061	C++

