var individualCombatantState = {};

individualCombatantState.ICS_None = 0;
individualCombatantState.ICS_Casting = 1;
individualCombatantState.ICS_Attacking = 2;
individualCombatantState.ICS_Guarding = 3;
individualCombatantState.ICS_Bandaging = 4;
individualCombatantState.ICS_Using = 5;
individualCombatantState.ICS_Moving = 6;
individualCombatantState.ICS_Turning = 7;
individualCombatantState.ICS_Fleeing = 8;
individualCombatantState.ICS_Fled = 9;
individualCombatantState.ICS_ContinueGuarding = 10; // ContinueGuarding means that a combatant who
// was guarding at the end of a round continues
// to guard until his initiative arrives.in
// the new round.
individualCombatantState.ICS_Petrified = 11;  // Not used as an ICS_STATE...only for script
individualCombatantState.ICS_Dying = 12;      // Not used as an ICS_STATE...only for script
individualCombatantState.ICS_Unconscious = 13;// Not used as an ICS_STATE...only for script
individualCombatantState.ICS_Dead = 14;       // Not used as an ICS_STATE...only for script
individualCombatantState.ICS_Gone = 15;       // Not used as an ICS_STATE...only for script
// Don't forget to update overallCombatState!
