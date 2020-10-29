/** TODO **/
/** PORT NOTE: I just couldn't port over another one of these list types - it is too easy to just move it directly into the weakly typed JS array */
function A_CStringPAIR_L() {
    this.mItems = [];
}

A_CStringPAIR_L.prototype.Clear = function () {
    this.mItems = [];
}

A_CStringPAIR_L.prototype.GetStartPosition = function () {
    return 0;
}
A_CStringPAIR_L.prototype.GetNextAssoc = function (pos) {
    return this.mItems[pos];
}

A_CStringPAIR_L.prototype.NextPos = function (pos) {
    pos = pos + 1;
    if (pos >= this.mItems.length) {
        return null;
    } else {
        return pos;
    }
}

A_CStringPAIR_L.prototype.GetCount = function () {
    this.mItems.length;
}


A_CStringPAIR_L.prototype.InsertKeyValue = function (key, value) {
    // PORT NOTE:  Rewrite to make this simpler - seems like a collection that replaces if the entry is already present
    for (var i = 0; i < this.mItems.length; i++) {
        var cstrPair = this.mItems[i];
        if (key == mItems[i].m_key && value == mItems[i].m_key) {
            return true; // PORT NOTE: Since CStringPAIR is just two strings, no need to reinsert this into the list
        }
    }
    var CStringPAIR = {};
    CStringPAIR.m_key = key;
    CStringPAIR.m_value = value;
    this.mItems.push(CStringPAIR);
    return false;
}

A_CStringPAIR_L.prototype.SerializeCAR = function (car) {
    if (car.IsStoring()) {
        for (var i = 0; i < this.mItems.length; i++) {
            car.writeString(this.m_items[i].m_key);
            car.writeString(this.m_items[i].m_value);
        };
        return car;
    }
    else {
        this.Clear();
        var count = car.readInt();
        for (; count != 0; count--) {
            var key, value;
            key = car.readString();
            value = car.readString();
            var item = {};
            item.m_key = key;
            item.m_value = value;
            this.mItems.push(item);
        };
        return car;
    };
}