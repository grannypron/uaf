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
Globals.ASSERT(Warrior.addCharacterItem("Buckler", 1, 0, 0, 0), "TestReadyBest.js  - 1");
Globals.ASSERT(Warrior.addCharacterItem("Shield|1", 1, 0, 0, 0), "TestReadyBest.js  - 2");
Globals.ASSERT(!Warrior.myItems.IsReady(0), "TestReadyBest.js - 3");
Globals.ASSERT(!Warrior.myItems.IsReady(1), "TestReadyBest.js - 4");
Warrior.ReadyBestShield();
Globals.ASSERT(!Warrior.myItems.IsReady(0), "TestReadyBest.js - 5");
Globals.ASSERT(Warrior.myItems.IsReady(1), "TestReadyBest.js - 6");

Globals.ASSERT(Warrior.addCharacterItem("Leather Armor", 1, 0, 0, 0), "TestReadyBest.js  - 7");
Globals.ASSERT(Warrior.addCharacterItem("Plate Mail", 1, 0, 0, 0), "TestReadyBest.js  - 8");
Globals.ASSERT(!Warrior.myItems.IsReady(2), "TestReadyBest.js - 9");
Globals.ASSERT(!Warrior.myItems.IsReady(3), "TestReadyBest.js - 10");
Warrior.ReadyBestArmor();
Globals.ASSERT(!Warrior.myItems.IsReady(2), "TestReadyBest.js - 11");
Globals.ASSERT(Warrior.myItems.IsReady(3), "TestReadyBest.js - 12");

Globals.ASSERT(Warrior.addCharacterItem("Two-Handed Sword", 1, 0, 0, 0), "TestReadyBest.js  - 13");
Warrior.myItems.SetReady(0, itemReadiedLocation.NotReady);
Warrior.myItems.SetReady(1, itemReadiedLocation.NotReady);
Warrior.myItems.SetReady(2, itemReadiedLocation.NotReady);
Warrior.myItems.SetReady(3, itemReadiedLocation.NotReady);
Warrior.ReadyBestWpn(1, false);
Globals.ASSERT(Warrior.myItems.IsReady(4), "TestReadyBest.js - 14");
Globals.ASSERT(!Warrior.myItems.IsReady(1), "TestReadyBest.js - 15");
Globals.ASSERT(Warrior.myItems.GetReadiedItem(itemReadiedLocation.WeaponHand, 0) == 4, "TestReadyBest.js - 16");
Warrior.ReadyBestShield();
// No shield should be readied because we no hands left from the two-handed sword
Globals.ASSERT(!Warrior.myItems.IsReady(0), "TestReadyBest.js - 17");
Globals.ASSERT(!Warrior.myItems.IsReady(1), "TestReadyBest.js - 18");
// Try it this way, too
Warrior.toggleReadyItem(1);
Globals.ASSERT(!Warrior.myItems.IsReady(0), "TestReadyBest.js - 19");
Globals.ASSERT(!Warrior.myItems.IsReady(1), "TestReadyBest.js - 20");
Globals.ASSERT(Warrior.myItems.GetReadiedItem(itemReadiedLocation.WeaponHand, 0) == 4, "TestReadyBest.js - 21");
Globals.ASSERT(Warrior.myItems.IsReady(4), "TestReadyBest.js - 22");
Globals.ASSERT(!Warrior.myItems.IsReady(1), "TestReadyBest.js - 23"); 
