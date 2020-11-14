// Requires TestLoadItems.js

function Deserialize(filename, debug) {
    var character = new CHARACTER()
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
Globals.ASSERT(Warrior.addCharacterItem("Long Sword", 1, 0, 0, 0), "TestInventory.js  - 1");
Globals.ASSERT(!Warrior.myItems.IsReady(0), "TestInventory.js - 1.5");
Globals.ASSERT("Long Sword" == Warrior.myItems.PeekAtPos(0).itemID);
Globals.ASSERT(Warrior.toggleReadyItem(0));
Globals.ASSERT(Warrior.myItems.IsReady(0), "TestInventory.js - 2");
Globals.ASSERT(itemReadiedLocation.WeaponHand.Equals(Warrior.myItems.PeekAtPos(0).readyLocation), "TestInventory.js - 3")
Globals.ASSERT(Warrior.toggleReadyItem(0));
Globals.ASSERT(!Warrior.myItems.IsReady(0), "TestInventory.js - 2.5");
Globals.ASSERT(Warrior.myItems.PeekAtPos(0).itemID == "Long Sword", "TestInventory.js - 4");

combatData.GetCombatant(0).m_pCharacter.addCharacterItem("Shield", 1, 0, 0, 0);