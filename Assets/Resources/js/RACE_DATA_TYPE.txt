/** TODO **/
function RACE_DATA_TYPE() {
    // **TODO** **EFFICIENCY** - Rewrite so that .RaceData is an associative array indexed by a monsterID string
    this.m_RaceData = [];          // Array of RACE_DATA

}

RACE_DATA_TYPE.prototype.GetCount = function () {
    return this.m_RaceData.length;
}

RACE_DATA_TYPE.prototype.PeekRace = function (idx) {
    return this.m_RaceData[idx];
}

RACE_DATA_TYPE.prototype.AddRace = function (data) {
    return this.m_RaceData.push(data);
}

RACE_DATA_TYPE.prototype.LocateRace = function (monsterID) {
    for (var idx = 0; idx < this.m_RaceData.length; idx++) {
        if (this.m_RaceData[idx].monsterID = monsterID) {
            return this.m_RaceData[idx];
        }
    }
    return -1;
}

RACE_DATA_TYPE.prototype.GetRaceName = function (raceID) {
    /**TODO This is a stub for now**/
    return raceID;
}