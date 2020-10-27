function chainTriggerType() {
    this.AlwaysChain = 0;
    this.IfEventHappen = 1;
    this.IfEventNotHappen = 2;
};

function chainTriggerTypeObj() {
    entityType.call(this, chainTriggerType);
    Object.setPrototypeOf(this, entityType.prototype);
}
