var creatureSizeType = {};
creatureSizeType.Small = 0;
creatureSizeType.Medium = 1;
creatureSizeType.Large = 2;

function creatureSizeTypeObj() {
    entityType.call(this, creatureSizeType);
    Object.setPrototypeOf(this, entityType.prototype);
}

var creatureSizeType = new creatureSizeTypeObj(creatureSizeType);