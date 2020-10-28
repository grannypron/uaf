function EventSourceType() { 
    this.LevelEventSrc = 0;
    this.GlobalEventSrc = 1;
    this.CopyEventSrc = 2;
};

function EventSourceTypeObj() {
    EventSourceType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}

