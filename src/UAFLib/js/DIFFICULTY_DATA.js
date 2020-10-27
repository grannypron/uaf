function DIFFICULTY_DATA() {
    this.m_Name = "";
    this.m_ModifyHD = false;
    this.m_ModifyQty = false;
    this.m_ModifyMonsterExp = false;
    this.m_ModifyAllExp = false;
    this.m_HDAmount = 0;  // char
    this.m_QtyAmount = 0;  // char
    this.m_MExpAmount = 0;  // char
    this.m_AExpAmount = 0;  // char

    this.Clear();
}

DIFFICULTY_DATA.prototype.Clear = function() {
    this.m_Name = "";
    this.m_ModifyHD = this.m_ModifyQty = this.m_ModifyMonsterExp = this.m_ModifyAllExp = false;
    this.m_HDAmount = this.m_QtyAmount = this.m_MExpAmount = this.m_AExpAmount = 0;
}
