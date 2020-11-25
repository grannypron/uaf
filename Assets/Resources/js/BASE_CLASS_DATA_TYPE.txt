/** TODO **/
function BASE_CLASS_DATA_TYPE() {
    this.m_baseclassData = [];   // Array of BASE_CLASS_DATA
}


// PORT NOTE: rewrote this
BASE_CLASS_DATA_TYPE.prototype.GetByBaseclassID = function (baseclassID) {
    for (i = 0; i < this.m_baseclassData.length; i++) {
        if (baseclassID == this.m_baseclassData[i].m_name) {
            return this.m_baseclassData[i];
        }
    }
    return null;
}

BASE_CLASS_DATA_TYPE.prototype.PeekBaseclass = function (baseclassID) {
    return this.GetByBaseclassID(baseclassID);
}

BASE_CLASS_DATA_TYPE.prototype.Add = function (bcd) {
    return this.m_baseclassData.push(bcd);
}



