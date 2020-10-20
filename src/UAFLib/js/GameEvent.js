function COMBAT_EVENT_DATA() {
    this.event = eventType.getByNumber(0);
    this.id = 0;  //DWORD
    this.x = 0;
    this.y = 0;
    this.control = null; // EVENT_CONTROL
    this.pic = null;  // PIC_DATA 
    this.pic2 = null;  // PIC_DATA 
    this.text = "";
    this.text2 = "";
    this.text3 = "";
    this.chainEventHappen = 0;  //DWORD
    this.chainEventNotHappen = 0;   //DWORD

    this.timer = 0;  //LONGLONG   // virtualGameTime at which an OnTimer event should be posted. (or zero).
    this.timerID = 0;   //UINT

    //BOOL IsGlobal; // member of level events, or global events?
    this.m_source = EventSourceType.getByNumber(0);
    this.event_asl = null;    //A_ASLENTRY_L
    this.temp_asl = null;     //A_ASLENTRY_L
}
