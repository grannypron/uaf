function eventDirType() { 
    this.Any = 0;
    this.North = 1;
    this.South = 2;
    this.East = 3;
    this.West = 4;
    this.N_S = 5;
    this.N_E = 6;
    this.N_W = 7;
    this.S_E = 8;
    this.S_W = 9;
    this.E_W = 10;
    this.N_S_E = 11;
    this.N_S_W = 12;
    this.N_W_E = 13;
    this.W_S_E = 14;
    this.InFront = 15;
    this.NUM_DIR_TYPES = 16;
};

function eventDirTypeObj() {
    entityType.call(this, eventDirType);
    Object.setPrototypeOf(this, entityType.prototype);
}

