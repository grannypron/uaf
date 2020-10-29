function GameEvent() {
    this.event = eventType.getByNumber(0);
    this.id = 0;  //DWORD
    this.x = 0;
    this.y = 0;
    this.control = new EVENT_CONTROL();
    this.pic = new PIC_DATA();
    this.pic2 = new PIC_DATA();
    this.text = "";
    this.text2 = "";
    this.text3 = "";
    this.chainEventHappen = 0;  //DWORD
    this.chainEventNotHappen = 0;   //DWORD

    this.timer = 0;  //LONGLONG   // virtualGameTime at which an OnTimer event should be posted. (or zero).
    this.timerID = 0;   //UINT

    //BOOL IsGlobal; // member of level events, or global events?
    this.m_source = EventSourceType.getByNumber(0);
    this.event_asl = new A_ASLENTRY_L();
    this.temp_asl = new A_ASLENTRY_L();
}

GameEvent.prototype.GetEventId = function () {
    return this.id;
}

GameEvent.prototype.Clear = function() {
    this.WarnOfClearingWithChainsPresent(this);

    this.event = eventType.NoEvent;
    this.id = 0;
    this.x = -1;
    this.y = -1;
    this.text = "";
    this.text2 = "";
    this.text3 = "";
    this.pic.Clear();
    this.pic2.Clear();
    this.control.Clear();
    this.chainEventHappen = 0;
    this.chainEventNotHappen = 0;
    //IsGlobal=FALSE;
    this.m_source = EventSourceType.LevelEventSrc;
    this.event_asl.Clear();
    this.temp_asl.Clear();
    this.timer = 0;
}

GameEvent.prototype.WarnOfClearingWithChainsPresent = function(pEvent) {
    //PORT NOTE:  No code outside of editor
}