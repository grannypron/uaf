/** TODO **/
function COMBAT_SUMMARY() {
    this.m_thinkCount = 0;
    this.m_filenum = 0;
    this.m_he = 0; // The current non-me combatant.
    this.pActionA = null;  //COMBAT_SUMMARY_ACTION
    this.pActionB = null;  //COMBAT_SUMMARY_ACTION
    this.m_combatants = [];
    this.reachableCells = 0;  // **TODO** int*? 

    this.Clear();
}

COMBAT_SUMMARY.prototype.Clear = function () {
    this.m_combatants = [];
    this.m_availableActions = [];
    this.pActionA = null; this.pActionB = null;
    //memset(reachableCells, 0xff, MAX_TERRAIN_WIDTH * MAX_TERRAIN_HEIGHT * sizeof(reachableCells[0]));  **TODO**  ??
};
