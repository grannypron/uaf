// Requires TestSetupCombat.js
cWarrior = combatData.GetCombatant(0);
Globals.debug("cWarrior.x / cWarrior.y: " + cWarrior.x + " / " + cWarrior.y);

//**TODO** I think moving to a diagonal attack should work here - see comment at COMBATANT::canAttack() -- cWarrior.MoveCombatant(11, 10, false);

// Moves into an attack!
cWarrior.MoveCombatant(10, 10, false);
cWarrior.MoveCombatant(11, 10, false);

// Still at the same location
Globals.ASSERT(cWarrior.x == 10);
Globals.ASSERT(cWarrior.y == 10);
