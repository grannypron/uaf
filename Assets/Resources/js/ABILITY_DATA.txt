
function ABILITY_DATA() {

    this.m_name = "";
    this.m_abbreviation = "";
    this.m_roll = new DICEPLUS();
    this.m_modify = 0;  //BYTE  // Ability score to modify.  Like Strength = 18(34)
    this.m_specAbs = new SPECIAL_ABILITIES();
    this.bestSkillAdj = 0; // double  // Not serialized.  Used temporarily by GetAdjSkillValue.

    /*
    void Clear(void);
    int GetDataSize(void) const ;
    void PreSerialize(BOOL IsStoring);
    int Serialize(CAR & car, const CString& version);
    void PostSerialize(BOOL IsStoring);
      //void AddAdjust(ADJUSTMENT& adjust);
    void AddAdjust(CString & name);
    BOOL RollAbility(double & result) const ;
    */
}
