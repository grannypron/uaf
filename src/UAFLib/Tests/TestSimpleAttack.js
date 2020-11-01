// Requires TestSetupCombat.js & TestCombatMovementAttack.js
cWarrior = combatData.GetCombatant(0);


// Find a combatant next to the warrior
var idxEnemy = 0;
for (var idx = 0; idx < combatData.m_aCombatants.length; idx++) {
    if (Math.abs(combatData.m_aCombatants[idx].x - cWarrior.x) <= 1 && Math.abs(combatData.m_aCombatants[idx].y - cWarrior.y) <= 1) {
        idxEnemy = idx;
        idx = combatData.m_aCombatants.length;
    }
}

var deathIndex = [];
cWarrior.makeAttack(idxEnemy, 0, deathIndex);
