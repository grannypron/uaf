// PORT NOTE:  Rewrote this to be backed by an associative array.  This appeared to me to be just a way of tracking a set of items that were indexed by an ordinal and have a binary on/off status

function TAG_LIST() {
    this.items = {};
}

TAG_LIST.prototype.Clear = function() {
    this.items = {};
}

TAG_LIST.prototype.Set = function (i) {
    this.items[i] = 1;
}

TAG_LIST.prototype.ClearItem = function (i) {
    this.items[i] = null;
}


