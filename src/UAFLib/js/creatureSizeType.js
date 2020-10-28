var creatureSizeType = function() {
    this.Small = 0;
    this.Medium = 1;
    this.Large = 2;
}

function creatureSizeTypeObj() {
    creatureSizeType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}
