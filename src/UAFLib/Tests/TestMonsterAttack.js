
Globals.SPECAB_HACKS = {};
var setMonsterReady = -1;
var callCount = 0;
// This hack will make the m_isCombatReady flag set to each of them as set by the setMonsterReady variable
Globals.SPECAB_HACKS["IsCombatReady"] = function (pkt) {
    if (setMonsterReady == -1) {
        SPECAB.p_hook_parameters[0] = "";
    } else {
        if (callCount == (setMonsterReady - 1)) {
            SPECAB.p_hook_parameters[0] = "";
            callCount = 0;
        } else {
            SPECAB.p_hook_parameters[0] = "1";
            callCount++;
        }
    }
    return CBRESULT.CBR_STOP;
}



Globals.SPECAB_HACKS["FreeAttack-CanFreeAttack"] = function (pkt) { SPECAB.p_hook_parameters[0] = "N"; }

// Move player away so that monsters move toward
var cWarrior = combatData.m_aCombatants[0];
cWarrior.MoveCombatant(cWarrior.x - 1, cWarrior.y - 1);
combatData.m_aCombatants[0].EndTurn();


Globals.debug("****Now monsters moving***");
combatData.m_aCombatants[1].m_target = -1;
combatData.m_aCombatants[2].m_target = -1;
combatData.m_aCombatants[3].m_target = -1;
combatData.m_eSurprise = eventSurpriseType.PartySurprised;  // This will force the monsters to go first
combatData.StartNewRound();  // Ends the turn we took in the last test case and starts a new round

// Now let a monster move
for (var idxMonster = 1; idxMonster < 4; idxMonster++) {
    setMonsterReady = idxMonster;
    combatData.UpdateCombat();          
    combatData.QComb.NotStartOfTurn();
    combatData.UpdateCombat();              // Why do I have to call this again?
    combatData.HandleCurrState(true);   
    var m_iDeathIndex = -1;
    combatData.HandleTimeDelayMsgBegin(m_iDeathIndex); // return from a timer pause that let the user see the roll/message
    combatData.m_aCombatants[combatData.GetCurrCombatant()].EndTurn();
}
