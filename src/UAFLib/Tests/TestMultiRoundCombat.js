combatData.UpdateCombat();  // Ends the turn we took in the last test case and starts a new round

// Now let the monsters move
combatData.UpdateCombat();

// Have to call it again so that it is not the "start of the turn"?  See the first few lines of COMBAT_DATA.UpdateCombat()
combatData.UpdateCombat();

combatData.HandleCurrState(true);