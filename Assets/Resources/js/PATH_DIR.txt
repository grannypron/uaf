function PATH_DIR() { 

    this.PathNorth = 0;
    this.PathNE = 1;
    this.PathEast = 2;
    this.PathSE = 3;
    this.PathSouth = 4;
    this.PathSW = 5;
    this.PathWest = 6;
    this.PathNW = 7;
    this.PathBAD = -1;

    this.GetDir = function (cx, cy, dx, dy) {
        if (cx > dx) // nw, w, sw
        {
            if (cy > dy) return this.PathNW;
            else if (cy < dy) return this.PathSW;
            return this.PathWest;
        }
        else if (cx < dx) // ne, e, se
        {
            if (cy > dy) return this.PathNE;
            else if (cy < dy) return this.PathSE;
            return this.PathEast;
        }
        else // cx == dx
        {
            if (cy > dy) return this.PathNorth;
            else if (cy < dy) return this.PathSouth;
            return this.PathBAD; // curr == dest
        }
    }

    this.GetDist = function(d) {
        if (d == this.PathNorth || d == this.PathEast || d == this.PathSouth || d == this.PathWest)
            return 1;
        else
            return 2;
    }

}

function PATH_DIRObj() {
    PATH_DIR.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}


