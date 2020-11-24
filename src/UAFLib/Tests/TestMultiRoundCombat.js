Globals.SPECAB_HACKS = {};
Globals.SPECAB_HACKS["FreeAttack-CanFreeAttack"] = function (pkt) { SPECAB.p_hook_parameters[0] = "N"; }

var originalIsDone = COMBATANT.prototype.IsDone;
var combatantReady = -1;
COMBATANT.prototype.IsDone = function (freeAttack, comment) {
    //Override IsDone so that I can control who moves
    if (this.self != combatantReady) {
        return true;
    }
    return false;
}

// Move player away so that monsters move toward
var cWarrior = combatData.m_aCombatants[0];
cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y);
cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y);
cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y);
cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y);
combatData.m_aCombatants[0].EndTurn();


Globals.debug("****Now monsters moving***");
combatData.m_aCombatants[1].m_target = -1;
combatData.m_aCombatants[2].m_target = -1;
combatData.m_aCombatants[3].m_target = -1;
combatData.m_eSurprise = eventSurpriseType.PartySurprised;  // This will force the monsters to go first
combatData.StartNewRound();  // Ends the turn we took in the last test case and starts a new round

// Now let a monster move
for (var idxMonster = 1; idxMonster < 4; idxMonster++) {
    combatantReady = idxMonster;
    combatData.UpdateCombat();
    combatData.QComb.NotStartOfTurn();
    combatData.UpdateCombat();              // Why do I have to call this again?
    combatData.HandleCurrState(true);   
    var m_iDeathIndex = -1;
    combatData.HandleTimeDelayMsgBegin(0, m_iDeathIndex); // return from a timer pause that let the user see the roll/message
    Globals.ASSERT(combatData.m_aCombatants[idxMonster].State() == individualCombatantState.ICS_Moving); 
    combatData.m_aCombatants[combatData.GetCurrCombatant()].EndTurn();
}

