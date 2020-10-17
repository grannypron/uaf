/** TODO **/
/** PORT NOTE: I just couldn't port over another one of these list types - it is too easy to just move it directly into the weakly typed JS array */
function A_CStringPAIR_L() {
    this.mItems = [];
}

A_CStringPAIR_L.prototype.Clear = function () {
    this.mItems = [];
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