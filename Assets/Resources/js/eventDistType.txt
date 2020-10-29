function eventDistType() {
    this.UpClose = 0;
    this.Nearby = 1;
    this.FarAway = 2;
    this.AutoFarAway = 3;
    this.AutoUpClose = 4;  // These last two are only used internally
    this.AutoNearby = 5;   // by the TEXT_EVENT to provide AutoApproach

    this.NUM_DIST_TYPES = 4;
}

function eventDistTypeObj() {
    eventDistType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}

