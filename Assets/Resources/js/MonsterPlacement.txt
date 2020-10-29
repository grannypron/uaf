function MonsterPlacement() {
    this.directionFromParty = 0; //0=N; 1=E; 2=S; 3=W
    this.placeX = 0;
    this.placeY = 0;
    this.facing = 0;
}

MonsterPlacement.prototype.Clear = function() {
    this.directionFromParty = PATH_DIR.PathBAD;
    this.placeX = -1;
    this.placeY = -1;
};

