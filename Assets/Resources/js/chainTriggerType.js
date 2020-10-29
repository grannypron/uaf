function chainTriggerType() {
    this.AlwaysChain = 0;
    this.IfEventHappen = 1;
    this.IfEventNotHappen = 2;
};

function chainTriggerTypeObj() {
    chainTriggerType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}
