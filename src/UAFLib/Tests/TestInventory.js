
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
    return character;
}

// Requires TestCharacterSerialize.js
var Warrior = Deserialize("Warrior.chr");
Globals.ASSERT(Warrior.addCharacterItem("Long Sword", 1, 0, 0, 0), "TestInventory.js");
Warrior.toggleReadyItem(0);

