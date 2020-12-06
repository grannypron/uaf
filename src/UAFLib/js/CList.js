function CList() {
    this.mList = [];
}

CList.prototype.GetHeadPosition = function () {
    if (this.mList == null || this.mList.length == 0) {
        return null;
    }
    return 0;
}

CList.prototype.GetCount = function () {
    return this.mList.length;
}

CList.prototype.PeekAt = function (idx) {
    return this.mList[idx];
}

CList.prototype.PeekNext = function (idx) {
    if (idx >= this.mList.length || idx < 0)
        return null;
    else {
        return this.mList[idx + 1];
    }
}

CList.prototype.GetAtPos = function (idx) {
    if (idx >= this.mList.length || idx < 0)
        return null;
    else
        return this.PeekAt(idx);
        
}

CList.prototype.PeekAtPos = function (idx) {
    if (idx >= this.mList.length || idx < 0)
        return null;
    else
        return this.PeekAt(idx);
}

CList.prototype.NextPos = function (idx) {
    var idx = idx + 1;
    if (idx >= this.mList.length) {
        return null;
    } else {
        return idx;     // PORT NOTE: Obviously, this is not really necessary to have, but I wanted to keep the idea of the linked list with pointers so as to keep the code as close as possible to the original on the first conversion
    }
}

CList.prototype.FindKeyPos = function (key) {
    for (var idx = 0; idx < this.mList.length; idx++) {
        if (this.mList[idx] == key) {
            return idx;
        }
    }
}

CList.prototype.RemoveAll = function () {
    this.mList = [];
}

CList.prototype.IsEmpty = function () {
    return this.mList.length == 0;
}

CList.prototype.RemoveHead = function () {
    return this.mList.pop();
}

CList.prototype.Add = function (itm) {
    return this.mList.push(itm);
}

CList.prototype.SetAtGrow = function (length, itm) {
    return this.mList.push(itm);
}

CList.prototype.AddHead = function (newElement) {
    // PORT NOTE: Simplified
    this.mList.unshift(newElement);
    return 0;
}

CList.prototype.GetHead = function () {
    return this.mList[0];
}

CList.prototype.RemoveAt = function (idx) {
    this.mList.splice(idx, 1);
}