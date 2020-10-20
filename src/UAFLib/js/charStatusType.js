var charStatusType = {};
charStatusType.Unconscious = 1; // 1  less than one hp, but not losing hp (dying)
charStatusType.Dead = 2;        // 2  <= -10 hp
charStatusType.Fled = 3;        // 3 
charStatusType.Petrified = 4;   // 4
charStatusType.Gone = 5;        // 5
charStatusType.Animated = 6;    // 6
charStatusType.TempGone = 7;    // 7
charStatusType.Running = 8;     // 8
charStatusType.Dying = 9;       // 9 less than one hp, and losing 1 hp per round (combat only)

function charStatusTypeObj() {
    entityType.call(this, charStatusType);
    Object.setPrototypeOf(this, entityType.prototype);
}

