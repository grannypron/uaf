/** TODO **/
function CLASS_DATA_TYPE() {
    this.m_ClassData = [];
}

CLASS_DATA_TYPE.prototype.PeekClass = function (id) {
    return this.m_ClassData[id];
}


CLASS_DATA_TYPE.prototype.GetCount = function () {
    return this.m_ClassData.length;
}

CLASS_DATA_TYPE.prototype.AddClass = function (data) {
    this.m_ClassData[data.m_name] = data;
}

/*
int GetDataSize(void) const ;
void PreSerialize(BOOL IsStoring);
int Serialize(CAR & car);
void PostSerialize(BOOL IsStoring);
void CrossReference(class CR_LIST* pCRList);
ABILITYLIMITS GetAbilityLimits(const CLASS_ID& classID, const ABILITY_ID& abilityID) const ;
WORD GetAllowedAlignments(const CLASS_ID& classID) const ;
bool IsAllowedRace(const CLASS_ID& classID, const RACE_ID& raceID) const ;
*/