function DIFFICULTY_LEVEL_DATA() {

    this.NUM_DIFF_LVLS = 5;

    this.m_defaultLvl = 0;
    this.m_data = []; for (var i = 0; i < this.NUM_DIFF_LVLS; i++) { this.m_data[i] = new DIFFICULTY_DATA(); }

    this.Clear();
}

DIFFICULTY_LEVEL_DATA.prototype.Clear = function() {
    for (var i = 0; i < DIFFICULTY_LEVEL_DATA.NUM_DIFF_LVLS; i++)
        this.m_data[i].Clear();
    this.m_defaultLvl = 2;
}