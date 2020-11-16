function GPDL_EVENT() {
    this.WaitForReturn = true;
    this.HighlightText = false;
    this.ForceBackup = false;
    this.KillSound = false;
    this.sound = "";
    this.hSound = -1;
    this.m_talkGPDL = null;

    /*
    GPDL_EVENT(TEXT_EVENT_DATA * txtEvnt);
    BOOL ForcePartyBackup() { return ForceBackup; }
    BOOL WaitUntilReturnPressed() { return WaitForReturn; }
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    void OnUpdateUI(void);
    */
}