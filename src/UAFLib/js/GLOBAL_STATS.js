function GLOBAL_STATS() {
    this.version = 6.0;
    this.moneyData = new MONEY_DATA_TYPE();

    this.minPCs = 1;           // PORT NOTE: Defaulting to a value here because I don't see (yet) where it gets set
    this.maxParty_maxPCs = 0;  // Upper 16 bits = maxPartySize; Lower = maxNumPCs
    this.m_diffLvlData = new DIFFICULTY_LEVEL_DATA();
}

GLOBAL_STATS.prototype.GetMaxPartyMembers = function () {
    return this.maxParty_maxPCs >> 16;
}

GLOBAL_STATS.prototype.GetMinPCs = function()         { return this.minPCs; };
GLOBAL_STATS.prototype.GetMaxPartySize = function()   { return this.maxParty_maxPCs >> 16; };
GLOBAL_STATS.prototype.GetMaxPCs = function()         { return this.maxParty_maxPCs & 0xffff; };
GLOBAL_STATS.prototype.SetMinPCs = function(n)       { this.minPCs = n; };
GLOBAL_STATS.prototype.SetMaxPartySize = function(n) { this.maxParty_maxPCs = (this.maxParty_maxPCs & 0xffff) | (n << 16); };
GLOBAL_STATS.prototype.SetMaxPCs = function(n)       { this.maxParty_maxPCs = (this.maxParty_maxPCs & 0xffff0000) | n; };
