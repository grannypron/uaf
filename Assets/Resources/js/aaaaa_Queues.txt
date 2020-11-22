function UnorderedQueue() {
    CList.call(this);
    Object.setPrototypeOf(this, CList.prototype);
}

function OrderedQueue() {
    CList.call(this);
    Object.setPrototypeOf(this, CList.prototype);

    this.Insert = function (data, key) {
        this.SetAtGrow(key, data);                                  // PORT NOTE:  Simplified this
    }

    this.FindKeyPos = function (i) {
        if (i >= this.GetCount() || i < 0)                     // PORT NOTE: I removed the idea of keys for a simple index
            return null;
        else
            return i;
    }

    this.GetNext = function (i) {
        return this.GetAtPos(i);                                // PORT NOTE: No way to advance pointer automatically - no pass-by-reference parameters
    }
}


/*
OrderedQueue.prototype.PeekAtPos = function(pos) {
    return this.GetAtPos(pos);
}
OrderedQueue.prototype.PeekNext = function (pos) {
    return this.GetAtPos(pos + 1);
}
*/
