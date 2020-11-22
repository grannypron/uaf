/** TODO **/
function BASE_CLASS_DATA_TYPE() {
    this.m_baseclassData = [];
}


// PORT NOTE: rewrote this
BASE_CLASS_DATA_TYPE.prototype.GetByBaseclassID = function (baseclassID) {
    for (i = 0; i < this.m_baseclassData.length; i++) {
        if (baseclassID == this.m_baseclassData[i].m_name()) {
            return this.m_baseclassData[i];
        }
    }
    return null;
}



