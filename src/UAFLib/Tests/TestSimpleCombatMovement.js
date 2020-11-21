// Requires TestSetupCombat.js
combatData.m_eSurprise = eventSurpriseType.MonsterSurprised;
cWarrior = combatData.GetCombatant(0);
Globals.debug("cWarrior.x / cWarrior.y: " + cWarrior.x + " / " + cWarrior.y);

// Move nowhere
cWarrior.MoveCombatant(cWarrior.x, cWarrior.y, false);
Globals.ASSERT(cWarrior.x == cWarrior.x, "TestSimpleCombatMovement");
Globals.ASSERT(cWarrior.y == cWarrior.y, "TestSimpleCombatMovement");

// One step movement a few times
var origX = cWarrior.x; var origY = cWarrior.y;
cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y, false);
Globals.ASSERT(cWarrior.x == (origX + 1), "TestSimpleCombatMovement");
Globals.ASSERT(cWarrior.y == origY, "TestSimpleCombatMovement");

var origX = cWarrior.x; var origY = cWarrior.y;
cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y, false);
Globals.ASSERT(cWarrior.x == (origX + 1), "TestSimpleCombatMovement");
Globals.ASSERT(cWarrior.y == origY, "TestSimpleCombatMovement");

// Diagonal movement
var origX = cWarrior.x; var origY = cWarrior.y;
cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y + 1, false);
Globals.ASSERT(cWarrior.x == (origX + 1), "TestSimpleCombatMovement");
Globals.ASSERT(cWarrior.y == (origY + 1), "TestSimpleCombatMovement");

// Jump over some squares (this is allowed by the engine)
cWarrior.MoveCombatant(15, 2, false);
Globals.ASSERT(cWarrior.x == 15, "TestSimpleCombatMovement");
Globals.ASSERT(cWarrior.y == 2), "TestSimpleCombatMovement";

