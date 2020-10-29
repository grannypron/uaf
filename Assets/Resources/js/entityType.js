function entityType(obj) {
    for (var key in obj) {
        this[key] = obj[key];
    }
};
entityType.prototype.getByString = function (str) {
    return this[str];
};

entityType.prototype.getByNumber = function (num) {
    for (var key in this) {
        if (this[key] == num) {
            return key;
        }
    }
};

entityType.prototype.get = entityType.prototype.getByNumber;