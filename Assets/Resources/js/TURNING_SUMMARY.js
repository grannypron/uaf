function TURNING_SUMMARY() {
    this.turnData = [];
    this.undeadTypes = [];
    this.turnDataSize = 0;  // -1 if invalid
    this.baseclassID = ""; // The one that needs the lowest roll to succeed.
    this.baseclassLevel = 0;       // Associated with baseclassID.
    this.combatantIndex = 0;
    this.rolled = 0;        // result of rolling 1d20.  
    this.turnLevel = 0;            // Associated with baseclassID;
}



TURNING_SUMMARY.prototype.Clear = function () {
    this.turnDataSize = -1;
}
TURNING_SUMMARY.prototype.IsValid = function () {
    return this.turnDataSize >= 0;
}

//BOOL IsTurningPossible(void); /**TODO**/

TURNING_SUMMARY.prototype.GetSize = function () {
    return this.turnDataSize;
}

TURNING_SUMMARY.prototype.Validate = function () {
    this.turnDataSize = 0;
}

TURNING_SUMMARY.prototype.ClearUndeadTypes = function () {
    this.undeadTypes = [];
}

TURNING_SUMMARY.prototype.TurnData = function(monsterID) {
    return this.GetTurnData(monsterID);
}

TURNING_SUMMARY.prototype.Insert = function() {
    var pTurnData;
    this.turnDataSize++;
    //if (this.turnDataSize > this.turnData.GetSize()) this.turnData.SetSize(this.turnDataSize);  //**TODO**
    pTurnData = this.turnData[this.turnDataSize - 1];
    //pTurnData = &turnData[turnDataSize-1];
    pTurnData.Clear();
    return pTurnData;
};
