function MonsterArrangement() {

    this.active = false;
    this.turtleX = 0;              // Screen coordinates; Relative to partyX.
    this.turtleY = 0;              // Screen coordinates; Relative to partyY.
    this.turtleWidth = 0;          // Defaults to 1.
    this.turtleHeight = 0;
    this.partyX = 0;               // Screen coordinates; As seen by monsters.
    this.partyY = 0;               // Screen coordinates; As seen by monsters.
    this.curDirFromParty = 0;      // 0=N; 1=E; 2=S; 3=W
    this.lineOfSight = false;      // Enforce the line-of-sight rule
    this.partyAccessible = false;  // Enforce the accessible by party rule
    this.distance = 0              // Square of minimum distance from partyX/Y (or negative).
    this.totalNumMonster = 0;
    this.currentMonster = 0;       // combatData m_aCombatants[currentMonster] being placed next
    this.numMonster = [];          // In each direction
    this.numParty = 0;
    this.limitMinX = 0;
    this.limitMinY = 0;
    this.limitMaxX = 0;
    this.limitMaxY = 0;//Screen coordinates; 
    this.partyMinX = 0;
    this.partyMaxX = 0;
    this.partyMinY = 0;
    this.partyMaxY = 0;//Screen coordinates; 
    this.turtleStack = 0;   // Room for 2 pushes
    this.dx = [];
    this.dy = [];
    this.partyPositions = []; for (var idx = 0; idx < Globals.MAX_COMBATANTS; idx++) { this.partyPositions[idx] = new PARTY_POSITIONS(); } //Screen coordinates; // PORT NOTE:  C++ initializes this, and so I do here
    this.monsterPlacement = [];     // MonsterPlacement
    this.distanceFromParty = [];         // An entry for each cell on the map.
                                              // Index = MAX_TERRAIN*WIDTH * y + x
                                              // Walking distance.  0-->253   254=not computed  255=Not accessible.
                                              // In the final result 254 and 255 are not accessible.
}


MonsterArrangement.prototype.Activate = function (numCombatants) {
    var i;
    if (this.active) {
        Globals.die(UAFUtil.ByteFromHexString("0xc3111a5"));
    };
    this.numMonster[0] = 0; this.numMonster[1] = 0; this.numMonster[2] = 0; this.numMonster[3] = 0;
    this.numParty = 0;
    this.partyMinX = UAFUtil.ByteFromHexString("0x7fffffff"); this.partyMinY = UAFUtil.ByteFromHexString("0x7fffffff");
    this.partyMaxX = UAFUtil.ByteFromHexString("0x80000000"); this.partyMaxY = UAFUtil.ByteFromHexString("0x80000000");
    for (i = 0; i < numCombatants; i++) { this.monsterPlacement.push(new MonsterPlacement()); }   //PORT NOTE:  realloc
    for (i = 0; i < numCombatants; i++) this.monsterPlacement[i].Clear();
    this.active = true;
    for (i = 0; i < Drawtile.MAX_TERRAIN_WIDTH * Drawtile.MAX_TERRAIN_HEIGHT; i++) { this.distanceFromParty[i] = 0;}
}

MonsterArrangement.prototype.deActivate = function () {
    this.active = false;
    monsterPlacement = null;
    distanceFromParty = null;
}