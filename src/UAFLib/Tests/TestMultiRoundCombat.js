// Tell the monsters who they are trying to fight
combatData.m_aCombatants[0].EndTurn();
combatData.m_aCombatants[1].m_target = 0;
combatData.m_aCombatants[2].m_target = 0;
combatData.m_aCombatants[3].m_target = 0;

combatData.m_eSurprise = eventSurpriseType.PartySurprised;  // This will force the monsters to go first
combatData.StartNewRound();  // Ends the turn we took in the last test case and starts a new round
// Now let the monsters move
combatData.UpdateCombat();
// Have to call it again so that it is not the "start of the turn"?  See the first few lines of COMBAT_DATA.UpdateCombat()
combatData.UpdateCombat();
combatData.HandleCurrState(true);
var m_iDeathIndex = -1;
combatData.HandleTimeDelayMsgBegin(m_iDeathIndex); // return from a timer pause that let the user see the roll/message



// Disable free attacks for this test case
Globals.SPECAB_HACKS["FreeAttack-CanFreeAttack"] = function (pkt) { SPECAB.p_hook_parameters[0] = "N"; }  // This is to return 1 when COMBATANT.IsDone is called



cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y);
cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y);
cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y);
cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y);
cWarrior.MoveCombatant(cWarrior.x + 1, cWarrior.y);
combatData.m_aCombatants[0].EndTurn();




combatData.m_aCombatants[1].m_target = -1;
combatData.m_aCombatants[2].m_target = -1;
combatData.m_aCombatants[3].m_target = -1;
combatData.m_eSurprise = eventSurpriseType.PartySurprised;  // This will force the monsters to go first
combatData.StartNewRound();  // Ends the turn we took in the last test case and starts a new round
// Now let the monsters move
combatData.UpdateCombat();
// Have to call it again so that it is not the "start of the turn"?  See the first few lines of COMBAT_DATA.UpdateCombat()
combatData.UpdateCombat();
combatData.HandleCurrState(true);
var m_iDeathIndex = -1;
combatData.HandleTimeDelayMsgBegin(m_iDeathIndex); // return from a timer pause that let the user see the roll/message
