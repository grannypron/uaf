function UnorderedQueue() {
    CList.call(this);
    Object.setPrototypeOf(this, CList.prototype);
}

function OrderedQueue() {
    this.items = {};
}


OrderedQueue.prototype.Insert = function(obj, key) {
    this.items[key] = obj;
}
OrderedQueue.prototype.RemoveAll = function (obj, key) {
    this.items = [];
}
OrderedQueue.prototype.IsEmpty = function () {
    return this.items.length <= 0;
}
OrderedQueue.prototype.GetCount = function () {
    return this.items.length;
}
OrderedQueue.prototype.GetHeadPosition = function () {
    return this.items.length == 0 ? null : 0;
}
OrderedQueue.prototype.GetAtPos = function (i) {
    return this.items[i];
}
OrderedQueue.prototype.RemoveHead = function () {
    this.items.pop();
}

