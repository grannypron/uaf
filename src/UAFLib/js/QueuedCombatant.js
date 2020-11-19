function QueuedCombatant(dude, stats, numFreeAttacks, numGuardAttacks) {
    this.Dude = dude;
    this.AffectStats = stats;
    this.m_numFreeAttacks = numFreeAttacks;
    this.m_numGuardAttacks = numGuardAttacks;
    this.m_delayedX = -1;
    this.m_delayedY = -1;
    this.m_bStartOfTurn = false;// For the combatant at top-of-queue.  Set
        // true when combatant first reaches top-of-queue,
        // Set false when UpdateCombat is called again
        // for the same combatant.
    this.m_bRestartInterruptedTurn = false;
        // Like m_bStartOfTurn except this is a chaacter
        // whose turn was interrupted and he has just returned
        // to the top-of-queue.  Cleared when UpdateCombat is called
}