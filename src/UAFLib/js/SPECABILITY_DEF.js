function SPECABILITY_DEF() {
    this.m_specAbName = "";
    this.m_abilityStrings = new A_ASLENTRY_L();
}

SPECABILITY_DEF.prototype.Find = function (key) {
    return this.m_abilityStrings.Find(key);
}

SPECABILITY_DEF.prototype.Insert = function(key, value, flags)
{
    this.m_abilityStrings.Insert(key, value, flags);
}