/** TODO **/
function PATH_MANAGER(num) {
    this.MAXPATHS = 100;
    if (!num) {
        num = this.MAXPATHS;
    }
    this.DestLeft = 0;
    this.DestTop = 0;
    this.DestRight = 0;
    this.DestBottom = 0;
    this.StartX = 0;
    this.StartY = 0;
    this.Path = [num]; //CPathFinder *;
    this.Tags = new TAG_LIST(num);  //TAGptr  // for determining which paths are already taken
    this.MaxPaths = num;
    this.CurrPath = 0;
    this.OccupantsBlock = true;
    this.pathWidth = 1;   //BYTE
    this.pathHeight = 1;   //BYTE

}

PATH_MANAGER.prototype.Clear = function() {
    this.pathWidth = 1;
    this.pathHeight = 1;
    this.OccupantsBlock = true;
    for (var i = 0; i < this.MaxPaths; i++)
        this.Path[i] = new CPathFinder();
    this.Tags.Clear();
    this.CurrPath = 0;
    this.StartX = 0;
    this.StartY = 0;
    this.DestLeft = 0;
    this.DestTop = 0;
    this.DestRight = 0;
    this.DestBottom = 0;
}

PATH_MANAGER.prototype.SetPathSize = function (w, h) {
    this.pathWidth = w;
    this.pathHeight = h;
}

PATH_MANAGER.prototype.GetPathIntIntIntInt = function(startx, starty, destLeft, destTop, destRight, destBottom, occupantsBlock, pCombatantLinger,
    moveOriginPoint)     // 'moveOriginPoint' means that the point (startX,startY)
// must be moved into the destinatiomn rectangle.
// Otherwise, any part of the path rectangle being moved into
// destination rectangle will suffice. 
{
    var OccupantsBlock = occupantsBlock;

    if ((!moveOriginPoint &&
        ((startx + this.pathWidth - 1 < destLeft)
        || (starty + this.pathHeight - 1 < destTop)
            || (startx > destRight)
            || (starty > destBottom)
        )
    )
        ||
        (moveOriginPoint &&
            ((startx < destLeft)
                || (starty < destTop)
                || (startx > destRight)
                || (starty > destBottom)
            )
        )
    ) {
        this.StartX = startx;
        this.StartY = starty;
        this.DestLeft = destLeft;
        this.DestTop = destTop;
        this.DestRight = destRight;
        this.DestBottom = destBottom;

        if (this.FindPath(pCombatantLinger, moveOriginPoint)) {
            return this.CurrPath;
        }
        else {
            return 1;
            //**TODO*** Stub to make sure I get things on the battlefield
            //return -1;
        }
    }
    else {
        return -1;// we are already there
    };
}


PATH_MANAGER.prototype.FindPath = function(pCombatantLinger, moveOriginPoint) {
    this.CurrPath = this.GetAvailablePath();
    if (this.CurrPath == -1) return false;
    this.Path[this.CurrPath].Clear();

    this.Path[this.CurrPath].SetData(null);
    this.Path[this.CurrPath].SetValid(this.GetValid());
    this.Path[this.CurrPath].SetCost(this.GetCost());
    this.Path[this.CurrPath].SetSize(Drawtile.MAX_TERRAIN_WIDTH, Drawtile.MAX_TERRAIN_HEIGHT);


    var Found = false;
    /** TODO** - Stub 
    var Found = this.Path[this.CurrPath].GeneratePath(this.StartX, this.StartY, this.pathWidth, this.pathHeight,
        this.DestLeft, this.DestTop,
        this.DestRight, this.DestBottom,
        pCombatantLinger,
        moveOriginPoint);
        **/

    if (!Found) {
        this.Tags.Clear(this.CurrPath);
        this.Path[this.CurrPath].Clear();
    }
    else
        this.Tags.Set(this.CurrPath);

    return Found;
}

PATH_MANAGER.prototype.GetAvailablePath = function()
{
    var val = this.Tags.FindOpenTag();
    if (val > -1) return val;

    for (var i = 0; i < this.MaxPaths; i++)
    {
        if (!this.Path[i].InUse()) {
            this.Tags.Clear(i);
            return i;
        }
    }
    return -1;
}


PATH_MANAGER.prototype.GetValid = function(x, y, ud, pCombatantLinger) {
    //**TODO** Stub
    return true;

    var checkIfOccupied = pathMgr.GetOccBlock();

    if (!this.ValidCoords(y, x))
        return false;

    /* ****************************** *****************************
     * This made no sense to me.  Perhaps it was meant to not check blocking
     * at the destination itself.  But this code seems incomplete and backward!
     *
     *
     * if ((x < pathMgr.DestLeft) || (y < pathMgr.DestTop))
     * { 
     *   checkIfOccupied = FALSE;
     * }
     * *******************************************************************
     */
    var valid = (OBSTICAL_TYPE.OBSTICAL_none == Drawtile.ObsticalType(x, y,
        pathMgr.GetPathWidth(), pathMgr.GetPathHeight(),
        checkIfOccupied,
        true,
        pCombatantLinger));
    return valid;
}

PATH_MANAGER.prototype.GetCost = function(sx, sy, dx, dy, ud) {
    return 1;//GetDist(GetDir(sx, sy, dx, dy));
    /**TODO** Stub above
    var d2;
    d2 = (sx - dx) * (sx - dx) + (sy - dy) * (sy - dy);
    return 5 * d2 + 5; */
}


PATH_MANAGER.prototype.FreePath = function (which) {
    /**TODO** - Stub */
}

/**TODO
// usage functions
int   GetAvailablePath();// returns -1 if none avail, 0+ for index
bool  FindPath(COMBATANT * pCombatantLinger,
    BOOL moveOriginPoint);   // as opposed to 'moveOriginRectangle'
PATH_MANAGER & operator=(const PATH_MANAGER &src); // no implementation


PATH_MANAGER(int num = MAXPATHS);// max number of paths
~PATH_MANAGER();

int  GetStepsRemaining(int which);
BOOL GetOccBlock() { return OccupantsBlock; }
BYTE GetPathWidth() { return pathWidth; }
BYTE GetPathHeight() { return pathHeight; }
void SetPathSize(int w, int h) { pathWidth = w; pathHeight = h; }

void Clear();


*  20160612  PRS
 *  We discovered that the 5th and 6th parameters are not treated as 'width' and 'height'
 * but rather as the lower right corner of a rectangle.
 * So we changed the definition and had to change many calls to this function
 * which had "1,1" as the 5th and 6th parameters.
 * I worry......this is a major difference.  How did we survive this long?  Am I confused?
 * Well, here goes.  I'll try to leave a clear 'undo' trail behind.  Just in case.
 *
 * int  GetPath (int startx,    int starty, 
 *               int destx,     int desty,
 *               int destWidth, int destHeight,
 *               BOOL occupantsBlock,
 *               COMBATANT *pCombatantLinger);
 *




CPathFinder * GetPath(int which);  

*/