// Requires TestSetupCombat.js
combatData.m_aCombatants[0].m_isCombatReady = -1;

var numAttacks = 0;
// Inject a proxy to determine how many attacks were made - I go by the sound here because it's just easier ¯\_(:)_/¯
itemData.GetItemFromID("Long Bow").hHitSound = "sound_Hit";
itemData.GetItemFromID("Long Bow").hMissSound = "sound_Miss";
var origPlaySound = SoundMgrObj.prototype.PlaySound;
SoundMgrObj.prototype.PlaySound = function (file) {
    if (file == "sound_Hit" || file == "sound_Miss") {
        numAttacks++;
    }
    return origPlaySound(file);
};


cWarrior = combatData.GetCombatant(0);
// Move a few steps away
for (var idxMove = 0; idxMove < 3; idxMove++) {
    cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y, false);
}

var enemyIdx = 1;  // Arbitrarily chosen
var canAttack = cWarrior.canAttack(enemyIdx, -1, -1, 0, Drawtile.Distance6, false);
// Combatant has no ranged weapons.  Should not be able to attack at range
// Warning:  Nothing prevents a call to makeAttack here and the calculations will be made regardless of range.  That is why I did not
// add a call to makeAttack in this test case.
Globals.ASSERT(canAttack == false, "TestRangedAttack.js - 1");

cWarrior.m_pCharacter.addCharacterItem("Long Bow", 1, 0, 0, 0);
cWarrior.m_pCharacter.addCharacterItem("Arrow", 50, 0, 0, 0);
cWarrior.m_pCharacter.ReadyBestWpn(5, false);
cWarrior.m_pCharacter.myItems.SetReady(1, itemReadiedLocation.AmmoQuiver);
// Are the items that we readied ready?
Globals.debug("----:.m_items.PeekAtPos(pos).GetReadyLocation():" + cWarrior.m_pCharacter.myItems.m_items.PeekAtPos(0).GetReadyLocation().location + "/" + itemReadiedLocation.NotReady.location);
Globals.ASSERT(cWarrior.m_pCharacter.myItems.IsReady(0), "TestRangedAttack.js - 2");
Globals.ASSERT(cWarrior.m_pCharacter.myItems.IsReady(1), "TestRangedAttack.js - 3");

// Is the bow readied in the weapon hand?  This check is taken from canAttack()
Globals.ASSERT(cWarrior.m_pCharacter.myItems.GetReadiedItem(itemReadiedLocation.WeaponHand, 0) != NO_READY_ITEM, "TestRangedAttack.js - 4");

canAttack = cWarrior.canAttack(enemyIdx, -1, -1, 0, Drawtile.Distance6, false);
// Are we able to attack from this distance?
Globals.ASSERT(canAttack, "TestRangedAttack.js - 5");

numAttacks = 0;
cWarrior.makeAttack(1, 0, 0);
Globals.ASSERT(numAttacks > 0, "TestRangedAttack.js - 6");

// Should have one less arrow now
Globals.ASSERT(cWarrior.m_pCharacter.myItems.GetQty(1) == 49, "TestRangedAttack.js - 7");

// Put a "wall" in between the player and the monsters 
for (var idxWall = -2; idxWall < 2; idxWall++) {
    var cell = Drawtile.terrain[cWarrior.y + idxWall][cWarrior.x - 2].cell;
    Drawtile.CurrentTileData[cell].tile_invisible = 0;
}

// Now the player cannot attack because he has no visibility
canAttack = cWarrior.canAttack(enemyIdx, -1, -1, 0, Drawtile.Distance6, false);
Globals.ASSERT(!canAttack, "TestRangedAttack.js - 8");

// Remove that "wall"
for (var idxWall = -2; idxWall < 2; idxWall++) {
    var cell = Drawtile.terrain[cWarrior.y + idxWall][cWarrior.x - 2].cell;
    Drawtile.CurrentTileData[cell].tile_invisible = 1;
}

// Player can attack again
canAttack = cWarrior.canAttack(enemyIdx, -1, -1, 0, Drawtile.Distance6, false);
Globals.ASSERT(canAttack, "TestRangedAttack.js - 9");

// Move a lot of steps away
// but first, give him lots of moves
cWarrior.m_pCharacter.maxMovement = 100;
for (var idxMove = 0; idxMove < 20; idxMove++) {
    cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y + 1, false);
}

// Player is too far away for ranged attack
canAttack = cWarrior.canAttack(enemyIdx, -1, -1, 0, Drawtile.Distance6, false);
Globals.ASSERT(!canAttack, "TestRangedAttack.js - 10");
