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
        this.Path[i] = null;
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
int  GetPath(int startx, int starty,
    int destLeft, int destTop,
    int destRight, int destBottom,
    BOOL occupantsBlock,
    COMBATANT * pCombatantLinger,
    BOOL moveOriginPoint);



void FreePath(int which);

CPathFinder * GetPath(int which);  

*/