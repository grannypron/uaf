// Requires TestSetupCombat.js
combatData.m_aCombatants[0].m_isCombatReady = -1;

var numAttacks = 0;
// Inject a proxy to determine how many attacks were made - I go by the sound here because it's just easier ¯\_(:)_/¯
itemData.GetItemFromID("Javelin").hHitSound = "sound_Hit";
itemData.GetItemFromID("Javelin").hMissSound = "sound_Miss";
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
Globals.ASSERT(canAttack == false, "TestThrownWeaponAttack.js - 1");

cWarrior.m_pCharacter.addCharacterItem("Javelin", 3, 0, 0, 0);
cWarrior.m_pCharacter.ReadyBestWpn(5, false);
// Are the items that we readied ready?
Globals.ASSERT(cWarrior.m_pCharacter.myItems.IsReady(0), "TestThrownWeaponAttack.js - 2");

// Is the bow readied in the weapon hand?  This check is taken from canAttack()
Globals.ASSERT(cWarrior.m_pCharacter.myItems.GetReadiedItem(itemReadiedLocation.WeaponHand, 0) != NO_READY_ITEM, "TestThrownWeaponAttack.js - 4");

canAttack = cWarrior.canAttack(enemyIdx, -1, -1, 0, Drawtile.Distance6, false);
// Are we able to attack from this distance?
Globals.ASSERT(canAttack, "TestThrownWeaponAttack.js - 5");

numAttacks = 0;
cWarrior.makeAttack(1, 0, 0);
Globals.ASSERT(numAttacks > 0, "TestThrownWeaponAttack.js - 6");

// Should have one less thrown weapon now
Globals.ASSERT(cWarrior.m_pCharacter.myItems.GetQty(0) == 2, "TestThrownWeaponAttack.js - 7");

// And there should be one on the "ground" now
Globals.ASSERT(combatData.hurledWeapons.GetAtPos(0).itemID == "Javelin", "TestThrownWeaponAttack.js - 8");
Globals.ASSERT(combatData.hurledWeapons.GetAtPos(0).qty == 1, "TestThrownWeaponAttack.js - 9");