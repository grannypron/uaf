function PATH_DIR() { 

    this.PathNorth = 0;
    this.PathNE = 1;
    this.PathEast = 2;
    this.PathSE = 3;
    this.PathSouth = 4;
    this.PathSW = 5;
    this.PathWest = 6;
    this.PathNW = 7;
    this.PathBAD = -1
};

function PATH_DIRObj() {
    entityType.call(this, PATH_DIR);
    Object.setPrototypeOf(this, entityType.prototype);
}

