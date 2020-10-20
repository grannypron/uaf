function EventSourceType() { 
    this.LevelEventSrc = 0;
    this.GlobalEventSrc = 1;
    this.CopyEventSrc = 2;
};

function EventSourceTypeObj() {
    entityType.call(this, EventSourceType);
    Object.setPrototypeOf(this, entityType.prototype);
}

