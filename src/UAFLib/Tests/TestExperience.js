
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
Globals.debug(Warrior.GetCurrExp("fighter"));
Globals.ASSERT(Warrior.GetCurrExp("fighter") == 100, "TestExperience.js - 2");