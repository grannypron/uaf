function HOOK_PARAMETERS() {
    this.m_prevHookParameters = null;
    for (var idx = 0; idx < SPECAB.NUMHOOKPARAM; idx++) { this[idx] = ""; } // PORT NOTE:  C++ object has an [] operator, JS kinda does this "natively" so that is why I do this here
    this.m_prevHookParameters = SPECAB.p_hook_parameters;
    SPECAB.p_hook_parameters = this;
};

