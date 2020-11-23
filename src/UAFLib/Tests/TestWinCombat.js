// Requires TestSetupCombat.js
cWarrior = combatData.GetCombatant(0);
Globals.debug("cWarrior.x / cWarrior.y: " + cWarrior.x + " / " + cWarrior.y + " / " + cWarrior.GetStatus());

// Moves into an attack!
cWarrior.MoveCombatant(cWarrior.x - 1, cWarrior.y - 1, false);
cWarrior.MoveCombatant(cWarrior.x, cWarrior.y - 1, false);
// I should have killed something after 30 attacks
for (idxAttack = 0; idxAttack < 30; idxAttack++)
    cWarrior.makeAttack(1, 0, 0);
cWarrior.MoveCombatant(cWarrior.x, cWarrior.y - 1, false);
for (idxAttack = 0; idxAttack < 30; idxAttack++)
    cWarrior.makeAttack(2, 0, 0);
for (idxAttack = 0; idxAttack < 30; idxAttack++)
    cWarrior.makeAttack(3, 0, 0);
combatData.UpdateCombat();