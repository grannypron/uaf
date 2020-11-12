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
Globals.debug("Warrior.myItems.IsReady(0):" + Warrior.myItems.IsReady(0));
Globals.debug("Warrior.myItems.PeekAtPos(0).itemID:" + Warrior.myItems.PeekAtPos(0).itemID);
Warrior.toggleReadyItem(0);
Globals.ASSERT(Warrior.myItems.IsReady(0), "TestInventory.js - 2");
Warrior.toggleReadyItem(0);
Globals.ASSERT(!Warrior.myItems.IsReady(0), "TestInventory.js - 2.5");
Globals.ASSERT(itemReadiedLocation.WeaponHand.Equals(Warrior.myItems.PeekAtPos(0).Location_Readied), "TestInventory.js - 3")
Globals.debug("itemReadiedLocation.WeaponHand:" + itemReadiedLocation.WeaponHand.location);
Globals.debug(Warrior.myItems.PeekAtPos(0).GetReadyLocation().location);
Globals.ASSERT(Warrior.myItems.PeekAtPos(0).itemID == "Long Sword", "TestInventory.js - 4");
Globals.ASSERT(Warrior.myItems.PeekAtPos(0).GetReadyLocation() == itemReadiedLocation.WeaponHand, "TestInventory.js - 5");

