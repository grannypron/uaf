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
