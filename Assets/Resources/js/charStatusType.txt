function charStatusType() {
    this.Okay = 0,        // 0
    this.Unconscious = 1; // 1  less than one hp, but not losing hp (dying)
    this.Dead = 2;        // 2  <= -10 hp
    this.Fled = 3;        // 3 
    this.Petrified = 4;   // 4
    this.Gone = 5;        // 5
    this.Animated = 6;    // 6
    this.TempGone = 7;    // 7
    this.Running = 8;     // 8
    this.Dying = 9;       // 9 less than one hp, and losing 1 hp per round (combat only)
}

function charStatusTypeObj() {
    charStatusType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}

