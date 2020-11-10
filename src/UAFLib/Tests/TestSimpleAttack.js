// Requires TestSetupCombat.js & TestCombatMovementAttack.js
combatData.m_aCombatants[0].m_isCombatReady = -1;
cWarrior = combatData.GetCombatant(0);

Globals.SPECAB_HACKS = {};
Globals.SPECAB_HACKS["IsCombatReady"] = function (pkt) { SPECAB.p_hook_parameters[0] = "1";}  // This is to return 1 when COMBATANT.IsDone is called

// Find a combatant next to the warrior
var idxEnemy = -1;
// Not starting 
for (var idx = 1; idx < combatData.m_aCombatants.length; idx++) {
    if (Math.abs(combatData.m_aCombatants[idx].x - cWarrior.x) <= 1 && Math.abs(combatData.m_aCombatants[idx].y - cWarrior.y) <= 1) {
        idxEnemy = idx;
        idx = combatData.m_aCombatants.length;
    }
}

var deathIndex = [];
cWarrior.makeAttack(idxEnemy, 0, deathIndex);
