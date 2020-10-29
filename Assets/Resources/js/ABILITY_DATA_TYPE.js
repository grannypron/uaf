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
    void ClearAdjustments(double val);
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
