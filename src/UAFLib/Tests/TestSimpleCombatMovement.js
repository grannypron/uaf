// Requires TestSetupCombat.js
cWarrior = combatData.GetCombatant(0);
Globals.debug("cWarrior.x / cWarrior.y: " + cWarrior.x + " / " + cWarrior.y);

// Move nowhere
cWarrior.MoveCombatant(9, 10, false);
Globals.ASSERT(cWarrior.x == 9);
Globals.ASSERT(cWarrior.y == 10);

// One step movement a few times
cWarrior.MoveCombatant(10, 11, false);
Globals.ASSERT(cWarrior.x == 10);
Globals.ASSERT(cWarrior.y == 11);

cWarrior.MoveCombatant(10, 12, false);
Globals.ASSERT(cWarrior.x == 10);
Globals.ASSERT(cWarrior.y == 12);

// Diagonal movement
cWarrior.MoveCombatant(9, 13, false);
Globals.ASSERT(cWarrior.x == 9);
Globals.ASSERT(cWarrior.y == 13);

// Jump over some squares (this is allowed by the engine)
cWarrior.MoveCombatant(15, 2, false);
Globals.ASSERT(cWarrior.x == 15);
Globals.ASSERT(cWarrior.y == 2);

