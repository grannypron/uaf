// PORT NOTE:  Rewrote this to be backed by an associative array.  This appeared to me to be just a way of tracking a set of items that were indexed by an ordinal and have a binary on/off status

function TAG_LIST(num) {
    this.numItems = num;
    this.items = [];
}

TAG_LIST.prototype.Clear = function() {
    this.items = [];
}

TAG_LIST.prototype.Set = function (i) {
    this.items[i] = 1;
}

TAG_LIST.prototype.ClearItem = function (i) {
    this.items[i] = null;
}

TAG_LIST.prototype.FindOpenTag = function () {
    for (var i = 0; i < this.items.length; i++) {
        if (this.items[i] != 1) {
            return i;
        }
    }
    return -1;
}


