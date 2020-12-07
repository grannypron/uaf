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
Globals.ASSERT("Long Sword" == Warrior.myItems.GetAtPos(0).itemID);
Globals.ASSERT(Warrior.toggleReadyItem(0));
Globals.ASSERT(Warrior.myItems.IsReady(0), "TestInventory.js - 2");
Globals.ASSERT(itemReadiedLocation.WeaponHand.Equals(Warrior.myItems.PeekAtPos(0).readyLocation), "TestInventory.js - 3")
Globals.ASSERT(Warrior.toggleReadyItem(0));
Globals.ASSERT(!Warrior.myItems.IsReady(0), "TestInventory.js - 2.5");
Globals.ASSERT(Warrior.myItems.PeekAtPos(0).itemID == "Long Sword", "TestInventory.js - 4");

Globals.ASSERT(Warrior.maxMovement > 1, "TestInventory.js - 5");  // Shouldn't be completely weighed down

Globals.ASSERT(Warrior.addCharacterItem("Arrow", 10, 0, 0, 0), "TestInventory.js  - 5");


var fiveArrows = new ITEM();
fiveArrows.itemID = "Arrow";
fiveArrows.qty = 5;
fiveArrows.ClearReadyLocation();
fiveArrows.charges = 0;
fiveArrows.identified = false;
fiveArrows.cursed = false;
fiveArrows.paid = 0;
Globals.ASSERT(Warrior.myItems.AddItem(fiveArrows), "TestInventory.js  - 6");
Globals.ASSERT(Warrior.myItems.PeekAtPos(1).qty == 15, "TestInventory.js  - 7");


Warrior.myItems.halveItem(1);
Globals.ASSERT(Warrior.myItems.PeekAtPos(1).qty == 8, "TestInventory.js - 8");
Globals.ASSERT(Warrior.myItems.PeekAtPos(2).qty == 7, "TestInventory.js - 9");

Warrior.myItems.joinItems(1);
Globals.ASSERT(Warrior.myItems.PeekAtPos(1).qty == 15, "TestInventory.js  - 10");

Globals.ASSERT(Warrior.myItems.PeekAtPos(2) == null, "TestInventory.js  - 11");
