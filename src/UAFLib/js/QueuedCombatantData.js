function QueuedCombatantData() {
    this.m_q = new CList();
    this.Clear();
}

QueuedCombatantData.prototype.Clear = function () {
    this.m_q.RemoveAll();
}

QueuedCombatantData.prototype.Top = function () {
    if (this.m_q.GetCount() > 0)
        return this.m_q.GetHead().Dude;
    else
        return NO_DUDE;
}

QueuedCombatantData.prototype.StartOfTurn = function() {
    var pos;
    pos = this.m_q.GetHeadPosition();
    if (pos == null) return false;
    return m_q.GetAt(pos).m_bStartOfTurn;
}

QueuedCombatantData.prototype.RestartInterruptedTurn = function() {
    var pos;
    pos = this.m_q.GetHeadPosition();
    if (pos == null) return false;
    return this.m_q.GetAt(pos).m_bRestartInterruptedTurn;
};

QueuedCombatantData.prototype.Push = function(dude, stats, numFreeAttacks, numGuardAttacks) {
    var pos;
    pos = this.m_q.GetHeadPosition();
    if (pos != null) {
        var queuedCombatant = m_q.GetAt(pos);
        queuedCombatant.m_bRestartInterruptedTurn = !queuedCombatant.m_bStartOfTurn;
    };
    var temp = new QueuedCombatant(dude, stats, numFreeAttacks, numGuardAttacks);
    temp.m_bStartOfTurn = true;
    this.m_q.AddHead(temp);
}