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
cWarrior.m_pCharacter.myItems.SetReady(1, itemReadiedLocation.AmmoQuiver.location);
// Are the items that we readied ready?
Globals.ASSERT(cWarrior.m_pCharacter.myItems.IsReady(0), "TestRangedAttack.js - 2");
Globals.ASSERT(cWarrior.m_pCharacter.myItems.IsReady(1), "TestRangedAttack.js - 3");

// Is the bow readied in the weapon hand?  This check is taken from canAttack()
Globals.ASSERT(cWarrior.m_pCharacter.myItems.GetReadiedItem(Items.WeaponHand, 0) != NO_READY_ITEM, "TestRangedAttack.js - 4");

canAttack = cWarrior.canAttack(enemyIdx, -1, -1, 0, Drawtile.Distance6, false);
// Are we able to attack from this distance?
Globals.ASSERT(canAttack, "TestRangedAttack.js - 5");

numAttacks = 0;
cWarrior.makeAttack(1, 0, 0);
Globals.ASSERT(numAttacks > 0, "TestRangedAttack.js - 6");