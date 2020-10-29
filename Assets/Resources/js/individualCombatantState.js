var individualCombatantState = function() {

    this.ICS_None = 0;
    this.ICS_Casting = 1;
    this.ICS_Attacking = 2;
    this.ICS_Guarding = 3;
    this.ICS_Bandaging = 4;
    this.ICS_Using = 5;
    this.ICS_Moving = 6;
    this.ICS_Turning = 7;
    this.ICS_Fleeing = 8;
    this.ICS_Fled = 9;
    this.ICS_ContinueGuarding = 10; // ContinueGuarding means that a combatant who
// was guarding at the end of a round continues
// to guard until his initiative arrives.in
// the new round.
    this.ICS_Petrified = 11;  // Not used as an ICS_STATE...only for script
    this.ICS_Dying = 12;      // Not used as an ICS_STATE...only for script
    this.ICS_Unconscious = 13;// Not used as an ICS_STATE...only for script
    this.ICS_Dead = 14;       // Not used as an ICS_STATE...only for script
    this.ICS_Gone = 15;       // Not used as an ICS_STATE...only for script
// Don't forget to update overallCombatState!
}