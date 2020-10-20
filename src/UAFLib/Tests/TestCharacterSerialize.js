var character = new CHARACTER();


function Deserialize(filename, debug) {
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
    var result = character.getInfo();
    Globals.debug(result);
    return result;
}

// Serialize
/*var ar = new CAR(null, 6.0);
var log = System.Console.WriteLine;
character.SerializeCAR(ar, 6.0);*/

/* These came from running the DC original source */
var DRUID_RESULT = "name=Druid;pos=255;THAC0=20;AC=6;age=21;maxage=93;HP=8;maxHP=8;enc=20;maxenc=1750;maxmove=12;STR=10;INT=10;WIS=18;DEX=18;CON=15;CHA=15;race=Human;gender=female;class=Druid;align=True Neutral;";
var PRIESTESS_RESULT = "name=Priestess;pos=1;THAC0=20;AC=6;age=19;maxage=77;HP=4;maxHP=4;enc=1;maxenc=1750;maxmove=12;STR=10;INT=10;WIS=18;DEX=18;CON=18;CHA=10;race=Human;gender=female;class=Cleric;align=Lawful Good;";
var ROGUE_RESULT = "name=Rogue;pos=255;THAC0=20;AC=6;age=22;maxage=82;HP=7;maxHP=7;enc=1;maxenc=2250;maxmove=12;STR=13;INT=12;WIS=12;DEX=18;CON=18;CHA=10;race=Human;gender=female;class=Thief;align=Neutral Good;";
var WARRIOR_RESULT = "name=Warrior;pos=0;THAC0=18;AC=6;age=21;maxage=87;HP=14;maxHP=14;enc=1;maxenc=8000;maxmove=12;STR=18;INT=10;WIS=10;DEX=18;CON=18;CHA=10;race=Human;gender=female;class=Fighter;align=Lawful Good;";

Globals.ASSERT(Deserialize("Druid.chr") == DRUID_RESULT, "TestCharacterSerialize.js");
Globals.ASSERT(Deserialize("Priestess.chr") == PRIESTESS_RESULT, "TestCharacterSerialize.js");
Globals.ASSERT(Deserialize("Rogue.chr") == ROGUE_RESULT, "TestCharacterSerialize.js");
Globals.ASSERT(Deserialize("Warrior.chr") == WARRIOR_RESULT, "TestCharacterSerialize.js");
