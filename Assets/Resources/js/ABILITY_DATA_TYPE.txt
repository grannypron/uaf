function ABILITY_DATA_TYPE() {
    this.m_abilityData = {};  //ABILITY_DATA

    /**TODO
    abilityType m_FindName(const CString& name) const ;
    int LocateAbilityData(const CString& abilityName) const ;
    void Clear(void);
    void LoadUADefaults(void);
    void PreSerialize(BOOL IsStoring);
    int Serialize(CAR & car);
    void CrossReference(class CR_LIST* pCRList);
    void PostSerialize(BOOL IsStoring);
    unsigned int GetCount() { return m_abilityData.GetSize(); }
    int GetDataSize(void);
    double TotalAdjustments(double val);
    CString GetShortName(const CString& abilityName) const ;
    BOOL RollAbility(const CString& abilityName, double & result) const ;
    **/
};

ABILITY_DATA_TYPE.prototype.AddAbility = function (data) {
    this.m_abilityData[data.m_name] = data;
}

ABILITY_DATA_TYPE.prototype.PeekAbilityData = function (abilityName) {
    this.m_abilityData[abilityName];
}

ABILITY_DATA_TYPE.prototype.LocateAbilityData = function (abilityName) {
    this.m_abilityData[abilityName];
}

ABILITY_DATA_TYPE.prototype.GetCount = function () {
    this.m_abilityData.length;
}

ABILITY_DATA_TYPE.prototype.ClearAdjustments = function (val) {
    var i = 0, n = 0;
    n = this.GetCount();
    for (i = 0; i < n; i++) {
        this.GetAbilityData(i).bestSkillAdj = val;
    };
}


ABILITY_DATA_TYPE.prototype.TotalAdjustments = function(val) {
    var i = 0, n = 0;
    var result = 0.0;
    n = this.GetCount();
    result = val;
    for (i = 0; i < n; i++) {
        var pab;
        pab = this.GetAbilityData(i);
        if (pab.bestSkillAdj != val) {
            if (result == val) result = pab.bestSkillAdj;
            else result += pab.bestSkillAdj;
        };
    };
    return result;
}
