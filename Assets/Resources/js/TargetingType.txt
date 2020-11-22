function TargetingType() {
    this.NowSelf = 0;
    this.NowSelectedByCount=  1;
    this.NowWholeParty = 2;
    this.NowTouchedTargets = 3;
    this.NowAreaCircle = 4;
    this.NowSelectByHitDice = 5;
    this.NowAreaLinePickStart = 6;
    this.NowAreaLinePickEnd = 7;
    this.NowAreaSquare = 8;
    this.NowAreaCone = 9;
    this.LingerSelf = 10;
    this.LingerSelectedByCount = 11;
    this.LingerWholeParty = 12;
    this.LingerTouchedTargets = 13;
    this.LingerAreaCircle = 14;
    this.LingerSelectByHitDice = 15;
    this.LingerAreaLinePickStart = 16;
    this.LingerAreaLinePickEnd = 17;
    this.LingerAreaSquare = 18;
    this.LingerAreaCone = 19;
};

function TargetingTypeObj() {
    TargetingType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}

