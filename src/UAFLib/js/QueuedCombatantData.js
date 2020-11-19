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
    return this.m_q.GetAtPos(pos).m_bStartOfTurn;
}

QueuedCombatantData.prototype.NotStartOfTurn = function () {
    var pos;
    pos = this.m_q.GetHeadPosition();
    if (pos == null) return;
    this.m_q.GetAtPos(pos).m_bStartOfTurn = false;
    this.m_q.GetAtPos(pos).m_bRestartInterruptedTurn = false;
};

QueuedCombatantData.prototype.RestartInterruptedTurn = function() {
    var pos;
    pos = this.m_q.GetHeadPosition();
    if (pos == null) return false;
    return this.m_q.GetAtPos(pos).m_bRestartInterruptedTurn;
};

QueuedCombatantData.prototype.Push = function(dude, stats, numFreeAttacks, numGuardAttacks) {
    var pos;
    pos = this.m_q.GetHeadPosition();
    if (pos != null) {
        var queuedCombatant = m_q.GetAtPos(pos);
        queuedCombatant.m_bRestartInterruptedTurn = !queuedCombatant.m_bStartOfTurn;
    };
    var temp = new QueuedCombatant(dude, stats, numFreeAttacks, numGuardAttacks);
    temp.m_bStartOfTurn = true;
    this.m_q.AddHead(temp);
}

QueuedCombatantData.prototype.NumFreeAttacks = function(idx) {
    if (!idx) {
        if (this.m_q.GetCount() > 0)
            return this.m_q.GetHead().m_numFreeAttacks;
        else
            return 0;
    } else {
        Globals.ASSERT("False", "Not implemented - NumFreeAttacksByIdx")
    }
}

QueuedCombatantData.prototype.NumGuardAttacks = function (idx) {
    if (!idx) {
        if (this.m_q.GetCount() > 0)
            return this.m_q.GetHead().m_numGuardAttacks;
        else
            return 0;
    } else {
        Globals.ASSERT("False", "Not implemented - NumGuardAttacksByIdx")
    }
}

QueuedCombatantData.prototype.ChangeStats = function() {
    if (this.m_q.GetCount() > 0)
        return this.m_q.GetHead().AffectStats;
    else
        return false;
}
