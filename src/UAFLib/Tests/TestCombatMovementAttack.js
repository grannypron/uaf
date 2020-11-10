// Requires TestSetupCombat.js
cWarrior = combatData.GetCombatant(0);
Globals.debug("cWarrior.x / cWarrior.y: " + cWarrior.x + " / " + cWarrior.y + " / " + cWarrior.GetStatus());

//**TODO** I think moving to a diagonal attack should work here - see comment at COMBATANT::canAttack() -- cWarrior.MoveCombatant(11, 10, false);

// Moves into an attack!
cWarrior.MoveCombatant(cWarrior.x - 1, cWarrior.y - 1, false);
var origX = cWarrior.x, origY = cWarrior.y;
cWarrior.MoveCombatant(cWarrior.x, cWarrior.y - 1, false);

// Still at the same location
Globals.ASSERT(cWarrior.x == origX, "TestCombatMovementAttack.js");
Globals.ASSERT(cWarrior.y == origY, "TestCombatMovementAttack.js");

// And Attacking a gobby
Globals.ASSERT(cWarrior.State() == individualCombatantState.ICS_Attacking, "TestCombatMovementAttack.js - cWarrior.State() == individualCombatantState.ICS_Attacking");