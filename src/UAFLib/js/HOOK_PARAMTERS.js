function HOOK_PARAMETERS() {
    this.m_prevHookParameters = null;
    this.m_hookParameters = [SPECAB.NUMHOOKPARAM];
    this.m_prevHookParameters = SPECAB.p_hook_parameters;
    SPECAB.p_hook_parameters = this;
};

