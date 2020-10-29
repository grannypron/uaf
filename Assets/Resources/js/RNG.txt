function DefaultRNG() {
}

DefaultRNG.prototype.newDWORD = function () {
    return this.newInt();  // I don't think it really matters if this is a 32-bit number range, will just use an int for this implementation
}

DefaultRNG.prototype.newInt = function () {
    var min = 0;
    var max = Number.MAX_SAFE_INTEGER;
    return Math.floor(Math.random() * (max - min + 1)) + min;
}