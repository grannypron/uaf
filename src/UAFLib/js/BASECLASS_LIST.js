function BASECLASS_LIST() {
    this.baseclasses = [];  // BASECLASS_ID
};

BASECLASS_LIST.prototype.Clear = function () {
    this.baseclasses = [];
};

BASECLASS_LIST.prototype.GetCount = function() {
    return this.baseclasses.length;
}

BASECLASS_LIST.prototype.GetAtPos = function (idx) {
    return this.baseclasses[idx];
}

/**TODO
void Remove(const BASECLASS_ID& baseclassID);
void Serialize(CArchive & ar);
void Serialize(CAR & car);
*/