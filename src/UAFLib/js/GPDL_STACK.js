function GPDL_STACK() {
    this.GPDL_STACKSIZE = 4;
    this.m_stack = []; //of GPDL size GPDL_STACKSIZE
    for (var idx = 0; idx < this.GPDL_STACKSIZE; idx++) { this.m_stack[idx] = new GPDL(); }
    this.m_activeGPDL = 0;
}

GPDL_STACK.prototype.Push = function() {
    this.m_activeGPDL++;
    if (this.m_activeGPDL >= this.GPDL_STACKSIZE) {
        Globals.die("GPDL recursion limit");
    }
}

GPDL_STACK.prototype.Pop = function() {
    if (this.m_activeGPDL == -1) {
        Globals.die("GPDL recursion underflow");
    };
    this.m_activeGPDL--;
}

GPDL_STACK.prototype.activeGPDL = function() {
    return this.m_stack[this.m_activeGPDL];  // PORT NOTE:  Original code was return m_stack+m_activeGPDL;    Not sure what that was about
}