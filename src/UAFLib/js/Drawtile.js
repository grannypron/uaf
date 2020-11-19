
var DUNGEON_TILE_COUNT = 26;
var DUNGEON_EMPTY_TILE = 23;

var DungeonWallTiles =
    [
  [ 0, 0, 0, 0, 0 ],
[ 2, 2, 0, 0, 1 ],  // 1
[ 62, 2, 0, 0, 1 ],
[ 122, 2, 0, 0, 1 ],
[ 182, 2, 0, 0, 1 ],
[ 242, 2, 1, 1, 1 ],  // 5
[ 302, 2, 0, 0, 1 ],
[ 362, 2, 0, 0, 1 ],
[ 422, 2, 0, 0, 1 ],
[ 482, 2, 1, 1, 1 ],
[ 542, 2, 0, 0, 1 ],  // 10
[ 2, 62, 1, 1, 1 ],
[ 62, 62, 0, 0, 1 ],
[ 122, 62, 1, 1, 1 ],
[ 182, 62, 0, 0, 1 ],
[ 242, 62, 1, 1, 1 ],  // 15
[ 302, 62, 0, 0, 1 ],
[ 362, 62, 1, 1, 1 ],
[ 422, 62, 0, 0, 1 ],
[ 482, 62, 0, 0, 1 ],
[ 542, 62, 0, 0, 1 ],  // 20
[ 2, 122, 0, 0, 1 ],
[ 62, 122, 0, 0, 1 ],
[ 122, 122, 1, 1, 1 ],
[ 182, 122, 1, 1, 1 ],
[ 242, 122, 0, 0, 1 ],  // 25
[ 302, 122, 1, 1, 0 ]
];

var WILDERNESS_TILE_COUNT = 40;
var WILDERNESS_EMPTY_TILE = 38;

var WildernessWallTiles =
    [
  [ 0, 0, 0, 0, 0 ],
[ 2, 2, 0, 0, 1 ],   // 1
[ 62, 2, 0, 0, 1 ],
[ 122, 2, 0, 0, 1 ],
[ 182, 2, 0, 0, 1 ],
[ 242, 2, 0, 0, 1 ], // 5
[ 302, 2, 1, 0, 0 ],
[ 362, 2, 1, 0, 0 ],
[ 422, 2, 1, 0, 0 ],
[ 482, 2, 1, 0, 0 ],
[ 542, 2, 1, 0, 0 ], // 10  
[ 2, 62, 1, 1, 1 ],
[ 62, 62, 1, 1, 1 ],
[ 122, 62, 1, 1, 1 ],
[ 182, 62, 1, 1, 1 ],
[ 242, 62, 1, 1, 1 ], // 15
[ 302, 62, 1, 0, 0 ],
[ 362, 62, 1, 0, 0 ],
[ 422, 62, 1, 0, 0 ],
[ 482, 62, 1, 0, 0 ],
[ 542, 62, 1, 0, 0 ], // 20  
[ 2, 122, 1, 0, 1 ],
[ 62, 122, 1, 0, 1 ],
[ 122, 122, 1, 0, 1 ],
[ 182, 122, 1, 1, 1 ],
[ 242, 122, 1, 1, 1 ], // 25
[ 302, 122, 1, 0, 1 ],
[ 362, 122, 1, 0, 1 ],
[ 422, 122, 1, 0, 1 ],
[ 482, 122, 1, 1, 1 ],
[ 542, 122, 1, 1, 1 ], // 30
[ 2, 182, 1, 1, 1 ],
[ 62, 182, 1, 1, 1 ],
[ 122, 182, 1, 1, 1 ],
[ 182, 182, 1, 1, 1 ],
[ 242, 182, 1, 1, 1 ], // 35
[ 302, 182, 1, 0, 1 ],
[ 362, 182, 1, 1, 1 ],
[ 422, 182, 1, 1, 1 ],
[ 482, 182, 1, 0, 0 ],
[ 542, 182, 1, 0, 0 ] // 40
];

function Drawtile() {
    this.locMap = [];
    this.MAX_TERRAIN_HEIGHT = 50;
    this.MAX_TERRAIN_WIDTH = 50;
    this.terrain = [];            // PORT NOTE:  Moved this global variable into the scope of this singleton 
    this.MAX_TILES = Math.max(WILDERNESS_TILE_COUNT, DUNGEON_TILE_COUNT);
    this.CurrentTileCount = 0;      // PORT NOTE:  Moved this global variable into the scope of this singleton 
    this.CurrentTileData = []; // PORT NOTE:  Moved this global variable into the scope of this singleton 
}

Drawtile.prototype.ClearEmptyCellSearch = function () {
    this.locMap = [];
}

Drawtile.prototype.EventDirToPathDir = function(dir) {
    switch (dir) {
        case eventDirType.North:
            return PATH_DIR.PathNorth;
        case eventDirType.N_E:
            return PATH_DIR.PathNE;
        case eventDirType.East:
            return PATH_DIR.PathEast;
        case eventDirType.S_E:
            return PATH_DIR.PathSE;
        case eventDirType.South:
            return PATH_DIR.PathSouth;
        case eventDirType.S_W:
            return PATH_DIR.PathSW;
        case eventDirType.West:
            return PATH_DIR.PathWest;
        case eventDirType.N_W:
            return PATH_DIR.PathNW;
        case eventDirType.Any:
        case eventDirType.N_S:
        case eventDirType.N_S_E:
        case eventDirType.N_W_E:
            return PATH_DIR.PathNorth;
        case eventDirType.E_W:
        case eventDirType.N_S_W:
        case eventDirType.W_S_E:
            return PATH_DIR.PathWest;
        case eventDirType.InFront:
            return (DrawtileFaceDirToPathDir(party.facing));
    }
    return PATH_DIR.PathNorth;
}

Drawtile.prototype.FaceDirToPathDir = function(dir) {
    switch (dir) {
        case FACE_NORTH:
            return PathNorth;
        case FACE_NE:
            return PATH_DIR.PathNE;
        case FACE_EAST:
            return PATH_DIR.PathEast;
        case FACE_SE:
            return PathSE;
        case FACE_SOUTH:
            return PATH_DIR.PathSouth;
        case FACE_SW:
            return PATH_DIR.PathSW;
        case FACE_WEST:
            return PATH_DIR.PATH_DIR.PathWest;
        case FACE_NW:
            return PATH_DIR.PathNW;
    }
    return PATH_DIR.PathNorth;
}




Drawtile.prototype.GenerateIndoorCombatMap = function (partyX, partyY, dir) {
    //STUB:

    for (i = 0; i < this.MAX_TERRAIN_HEIGHT; i++) {
        this.terrain[i] = [];
        for (j = 0; j < this.MAX_TERRAIN_WIDTH; j++) {
            this.terrain[i][j] = new TERRAIN_CELL();
            this.terrain[i][j].tileIndex = NO_DUDE;
            this.terrain[i][j].tileIndexDead = NO_DUDE;
            this.CurrentTileCount++;
            this.CurrentTileData[this.CurrentTileCount] = new TILE_DATA();
            this.CurrentTileData[this.CurrentTileCount].tile_posy = j;
            this.CurrentTileData[this.CurrentTileCount].tile_posx = i;
            this.CurrentTileData[this.CurrentTileCount].tile_invisible = 0;
            this.CurrentTileData[this.CurrentTileCount].tile_passable = 1;
            this.CurrentTileData[this.CurrentTileCount].tile_enabled = 1;
            this.terrain[i][j].cell = this.CurrentTileCount;

        }
    }
    return { partyX: Math.floor(this.MAX_TERRAIN_WIDTH/2), partyY: Math.floor(this.MAX_TERRAIN_HEIGHT / 2) };    // PORT NOTE:  Dealing with pass-by-reference parameters


/*****************************************************************************
// NAME:    GenerateIndoorCombatMap
//
// PURPOSE: 
// translates the current area map into equivalent combat dungeon terrain tile numbers


**TODO**
void GenerateIndoorCombatMap(int & partyX, int & partyY, PATH_DIR dir)
{
    int tx, ty, i, j;
    int countY, countX, partyCountX, partyCountY;
    int areaMapX;
    int areaMapY;

    int areaMapStartY = (partyY - (MAX_TERRAIN_HEIGHT / 2)) - 2;
    int areaMapStartX = (partyX - (MAX_TERRAIN_WIDTH / 2)) - 2;


    //   int areaMapEndY = partyY + (MAX_TERRAIN_HEIGHT/2) + 2;
    int areaMapEndX = partyX + (MAX_TERRAIN_WIDTH / 2) + 2;

    // Now, because we display a diagonal section of the 
    // map, we will move startX and endX half the distance 
    // to the right.

    partyCountX = partyX - areaMapStartX; // Where the party will be located on the temp maps
    partyCountY = partyY - areaMapStartY;


#ifdef diagonalMap
    areaMapStartX += MAX_TERRAIN_WIDTH / 2;
    areaMapEndX += MAX_TERRAIN_WIDTH / 2;
#endif


#ifndef diagonalMap
    // The diagonalMap method assumes that the combat map is a torous.
    // We don't limit ourselves at the edges; we wrap around!

    BOOL lBound = FALSE;
    BOOL rBound = FALSE;
    BOOL tBound = FALSE;
    BOOL bBound = FALSE;

    if (areaMapStartY < 0) {
        areaMapStartY = 0;
        tBound = TRUE;
    }

    if (areaMapStartX < 0) {
        areaMapStartX = 0;
        lBound = TRUE;
    }

    if (areaMapEndY >= levelData.area_height) {
        areaMapEndY = levelData.area_height - 1;
        bBound = TRUE;
    }

    if (areaMapEndX >= levelData.area_width) {
        areaMapEndX = levelData.area_width - 1;
        rBound = TRUE;
    }
#endif  // !diagonalMap
   

#ifndef diagonalMap
    // No need to apply limits if the combat takes place
    // on a torus.  For very small maps we will see the same
    // terrain several times in each direction.


    // Try to ensure max map data is used.
    // These checks get triggered when party is near
    // edges of map.
    if ((areaMapEndX - areaMapStartX) < MAX_TERRAIN_WIDTH - 1) {
        if (lBound && !rBound)
            areaMapEndX = areaMapStartX + MAX_TERRAIN_WIDTH - 1;
        else if (rBound && !lBound)
            areaMapStartX = areaMapEndX - MAX_TERRAIN_WIDTH + 1;
    }

    if ((areaMapEndY - areaMapStartY) < MAX_TERRAIN_HEIGHT - 1) {
        if (tBound && !bBound)
            areaMapEndY = areaMapStartY + MAX_TERRAIN_HEIGHT - 1;
        else if (bBound && !tBound)
            areaMapStartY = areaMapEndY - MAX_TERRAIN_HEIGHT + 1;
    }

    // just one more sanity check
    if (areaMapStartY < 0)
        areaMapStartY = 0;

    if (areaMapStartX < 0)
        areaMapStartX = 0;

    if (areaMapEndY >= levelData.area_height)
        areaMapEndY = levelData.area_height - 1;

    if (areaMapEndX >= levelData.area_width)
        areaMapEndX = levelData.area_width - 1;


#endif // !diagonalMap

    int charCombatStartY = partyY;
    int charCombatStartX = partyX;

    // only need to allocate the combat map once
    if (terrain == NULL) {
        DWORD tsize = sizeof(TERRAIN_CELL *) * MAX_TERRAIN_HEIGHT;
        terrain = (TERRAIN_CELL **)My_malloc(tsize);

        for (i = 0; i < MAX_TERRAIN_HEIGHT; i++) {
            DWORD size = sizeof(TERRAIN_CELL) * MAX_TERRAIN_WIDTH;
            tsize += size;
            terrain[i] = (TERRAIN_CELL *)My_malloc(size);
        }
        if (logDebuggingInfo) {
            WriteDebugString("Allocated %u bytes for combat terrain map\n", tsize);
        };
    }

    for (i = 0; i < MAX_TERRAIN_HEIGHT; i++) {
        for (j = 0; j < MAX_TERRAIN_WIDTH; j++) {
            memset(& terrain[i][j], 0, sizeof(TERRAIN_CELL));
            terrain[i][j].tileIndex = NO_DUDE;
            terrain[i][j].tileIndexDead = NO_DUDE;
        }
    }

    memset(CurrentTileData, 0, sizeof(CurrentTileData));
    // copy dungeon tile data to current data
    CurrentTileCount = DUNGEON_TILE_COUNT;
    for (i = 0; i <= CurrentTileCount; i++)
        CurrentTileData[i] = DungeonWallTiles[i];


    // The MAX_TEMP_HEIGHT+1 and MAX_TEMP_WIDTH+1 are because
    // we assign values to the right and bottom walls of each cell.


    // ********** EXAMPLE *****  MAX_TERRAIN_HEIGHT = 50
    // ************************  MAP_V_MULTIPLIER = 7
    // ************************  MAX_TEMP_HEIGHT = 55*7 = 385
    // ************************  We allocate 386 rows for tempArea and tempArea2

    tempArea = (TERRAIN_CELL **) My_malloc(sizeof(TERRAIN_CELL *) * (MAX_TEMP_HEIGHT + 1));
    tempArea2 = (TERRAIN_CELL **) My_malloc(sizeof(TERRAIN_CELL *) * (MAX_TEMP_HEIGHT + 1));


    // ********** EXAMPLE *****  MAX_TERRAIN_WIDTH = 50
    // ************************  MAX_H_MULTIPLIER  = 8
    // ************************  MAP_TEMP_WIDTH = 55*8 = 440
    // ************************  Wea allocate and clear 441 cells in each row.

    for (i = 0; i <= MAX_TEMP_HEIGHT; i++) {
        tempArea[i] = (TERRAIN_CELL *) My_malloc(sizeof(TERRAIN_CELL) * (MAX_TEMP_WIDTH + 1));
        tempArea2[i] = (TERRAIN_CELL *) My_malloc(sizeof(TERRAIN_CELL) * (MAX_TEMP_WIDTH + 1));
        memset(tempArea[i], 0, sizeof(TERRAIN_CELL) * (MAX_TEMP_WIDTH + 1));
        memset(tempArea2[i], 0, sizeof(TERRAIN_CELL) * (MAX_TEMP_WIDTH + 1));
    };


    // Translate orthogonal map data into isometric terrain squares

    // We waste two columns of areaTemp2 on the left and two colums on
    // the right and one row at the top and one row at the bottom
    // because it makes it unnecessary to have special cases
    // or tests at the edges.
    areaMapY = (areaMapStartY - 1 + 100 * levelData.area_height) % levelData.area_height;
    ty = 0;

    //
    // ************ EXAMPLE *********  countY = 0, 1,  2, ...... ,51 
    // ******************************  ty     = 0, 7, 14, ...... ,357

    // Cache to avoid looking up wall and door overrides multiple times for each x,y pair.
    // Cache values for (y*area_width+x)[direction] -- north,east,south,west
    //
    // Removed....I just discovered that overrides are already nicly cached if you 
    // use the function  'GetMapOverride'.
    //int *doorCache;
    //doorCache = (int *)My_malloc(4*levelData.area_width*levelData.area_height*sizeof(int));
    //int *wallCache;
    //wallCache = (int *)My_malloc(4*levelData.area_width*levelData.area_height*sizeof(int));
    //memset(doorCache, 0xff, (4*levelData.area_width*levelData.area_height*sizeof(int)));
    //memset(wallCache, 0xff, (4*levelData.area_width*levelData.area_height*sizeof(int)));
    for (countY = 0; countY < MAX_TERRAIN_HEIGHT + 2; countY++) {
        // ************** EXAMPLE ********  tx = 8, 7, 6, 5, 4, 3, 2, 1, 8, 7, 6, ....
        tx = 8 - countY % MAP_H_MULTIPLIER;
        areaMapX = (areaMapStartX - countY - 1 + countY / 8 + 100 * levelData.area_width) % levelData.area_width;
        for (countX = 0; countX < MAX_TERRAIN_WIDTH + 2; countX++) {
            // *********** EXAMPLE ********  countX = 0, 1, 2, ... 50, 51
            // ****************************  tx  = 8, 58, 108, 158, ... 8+51*8 = 416
            // ****************************  tx  = 7, 57, 107, 157, ... 7+51*8 = 415
            // ****************************  tx  = 1, 51, 101, 151, ... 1+51*8 = 409



            WALLTYPE doorType;
            //************************************************
            if ((countX == partyCountX) && (countY == partyCountY)) {
                //charCombatStartX = tx;
                charCombatStartX = tx + 2 * MAP_H_MULTIPLIER;
                charCombatStartY = ty + MAP_V_MULTIPLIER * 3 / 2;
            };

            //************************************************
            if (IsNorthWall(areaMapX, areaMapY)) {
                for (int w = 0; w < MAP_H_MULTIPLIER; w++)
                {
                    {
                        int x, y;
                        x = tx + w;
                        y = ty;
                        // ***** EXAMPLE ****  1 <= tx <= 416;  1 <= x <= 423
                        // ******************  0 <= ty <= 357;  0 <= y <= 357
                        TA2(x, y) = Wall;
                    };
                };
            };
            //************************************************
            if ((doorType = IsNorthDoor(areaMapX, areaMapY)) != NoWall) {
                for (int w = 0; w < MAP_H_MULTIPLIER; w++)
                {
                    int x, y;
                    x = tx + w;
                    y = ty;
                    // ***** EXAMPLE ****  1 <= tx <= 416;  1 <= x <= 423
                    // ******************  0 <= ty <= 357;  0 <= y <= 357
                    TA2(x, y) = Wall;
                };
                if (doorType == OpenDoor) {
                    // ***** EXAMPLE ****  1 <= tx <= 416;  6 <= tx+5 <= 421
                    // ******************  0 <= ty <= 357;  0 <= y <= 357
                    TA2(tx + 4, ty) = NoWall;
                    TA2(tx + 5, ty) = NoWall;
                };
            };
            //************************************************
            if (IsSouthWall(areaMapX, areaMapY)) {
                for (int w = 0; w < MAP_H_MULTIPLIER; w++)
                {
                    {
                        int x, y;
                        x = tx + w + MAP_H_MULTIPLIER - 1;
                        y = ty + MAP_V_MULTIPLIER;
                        // ***** EXAMPLE ****  1 <= tx <= 416;  8 <= x <= 430
                        // ******************  0 <= ty <= 357;  7 <= y <= 364
                        TA2(x, y) = Wall;
                    };
                };
            };
            //************************************************
            if ((doorType = IsSouthDoor(areaMapX, areaMapY)) != NoWall) {
                int w = 0;
                for (w = 0; w < MAP_H_MULTIPLIER; w++) {
                    int x, y;
                    x = tx + w + MAP_H_MULTIPLIER - 1;
                    y = ty + MAP_V_MULTIPLIER;
                    // ***** EXAMPLE ****  1 <= tx <= 416;  8 <= x <= 430
                    // ******************  0 <= ty <= 357;  7 <= y <= 364
                    TA2(x, y) = Wall;
                };
                if (doorType == OpenDoor) {
                    int x, y;
                    x = tx + w + MAP_H_MULTIPLIER - 1;
                    y = ty + MAP_V_MULTIPLIER;
                    // ***** EXAMPLE ****  1 <= tx <= 416;  13 <= x+5 <= 435
                    // ******************  0 <= ty <= 357;  7 <= y <= 364
                    TA2(x + 4, ty) = NoWall;
                    TA2(x + 5, ty) = NoWall;
                };
            };
            //************************************************
            if (IsWestWall(areaMapX, areaMapY)) {
                for (int w = 0; w < MAP_H_MULTIPLIER; w++)
                {
                    {
                        int x, y;
                        x = tx + w;
                        y = ty + w;
                        TA2(x, y) = Wall;
                    };
                };
            };
            //************************************************
            if ((doorType = IsWestDoor(areaMapX, areaMapY)) != NoWall) {
                for (int w = 0; w < MAP_H_MULTIPLIER; w++)
                {
                    int x, y;
                    x = tx + w;
                    y = ty + w;
                    TA2(x, y) = Wall;
                };
                if (doorType == OpenDoor) {
                    TA2(tx + 3, ty + 3) = NoWall;
                    TA2(tx + 4, ty + 4) = NoWall;
                };
            };
            //************************************************
            if (IsEastWall(areaMapX, areaMapY)) {
                for (int w = 0; w < MAP_H_MULTIPLIER; w++)
                {
                    {
                        int x, y;
                        x = tx + MAP_H_MULTIPLIER + w;
                        y = ty + w;
                        TA2(x, y) = Wall;
                    };
                };
            };
            //************************************************
            if ((doorType = IsEastDoor(areaMapX, areaMapY)) != NoWall) {
                for (int w = 0; w < MAP_H_MULTIPLIER; w++)
                {
                    int x, y;
                    x = tx + MAP_H_MULTIPLIER + w;
                    y = ty + w;
                    //              TA2(x,y) = doorType;
                    TA2(x, y) = Wall;
                };
                if (doorType == OpenDoor) {
                    int x, y;
                    x = tx + MAP_H_MULTIPLIER;
                    y = ty;
                    TA2(x + 3, y + 3) = NoWall;
                    TA2(x + 4, y + 4) = NoWall;
                };
            };


            tx += MAP_H_MULTIPLIER;
            areaMapX = (areaMapX + 1) % levelData.area_width;
        }

        ty += MAP_V_MULTIPLIER;
        areaMapY = (areaMapY + 1) % levelData.area_height;
    }
    //My_free(wallCache); wallCache = NULL;
    //My_free(doorCache); doorCache = NULL;
    // translate expanded area map into terrain tile types   
    //BOOL found = FALSE;
    ty = MAP_V_MULTIPLIER;

    //  FILE *f;
    //  f = fopen("tempArea2","w");
    for (countY = 0; countY < MAX_TERRAIN_HEIGHT * MAP_V_MULTIPLIER; countY++, ty++) {
        tx = 16;
        //    fprintf(f, "(%d,%d) ", tx, ty);
        for (countX = 0; countX < MAX_TERRAIN_WIDTH * MAP_H_MULTIPLIER; countX++, tx++) {
            //      fprintf(f,"%d", TA2(tx, ty));
            //if ((!found) && (ty == charCombatStartY) && (tx == charCombatStartX))
            //{
            //found = TRUE;
            //charCombatStartY = ty;
            //charCombatStartX = tx;
            //};

            if ((TA2(tx, ty) == Wall)
                //     || (TA2(tx-2,ty-2) == Wall) && (TA2(tx-1,ty) == Wall)
            ) {
                // This sets the cell type based on the number
                // junctures at each square.
                //
                // ie: Does this square contain a 4-way intersect?,
                // a 3-way? is the third leg point N,S,E, or W? etc.
                // 
                // The returned type is something like VTT_WALL, which
                // means Vertical Top Terminator.
                tempArea[ty][tx].cell = getTerrainWallType(tx, ty);
            };
            //if (tempArea2[srcY][srcX].cell == OpenDoor)
            //{
            // This sets the cell type based on the number
            // junctures at each square.
            //
            // ie: Does this square contain a 4-way intersect?,
            // a 3-way? is the third leg point N,S,E, or W? etc.
            // 
            // The returned type is something like VTT_WALL, which
            // means Vertical Top Terminator.
            //  tempArea[ty][tx].cell = getTerrainWallType(srcX,srcY);
            //}
            //if (tempArea2[srcY][srcX].cell == ClosedDoor)
            //{
            // This sets the cell type based on the number
            // junctures at each square.
            //
            // ie: Does this square contain a 4-way intersect?,
            // a 3-way? is the third leg point N,S,E, or W? etc.
            // 
            // The returned type is something like VTT_WALL, which
            // means Vertical Top Terminator.
            //  tempArea[ty][tx].cell = getTerrainWallType(srcX,srcY);
            //}
        };
        //    fprintf(f,"\n");

    };
    //  fclose(f);

    // now we need to convert the intersection types, such as VTT_WALL,
    // into indexes into the combat terrain image. Each type will be
    // represented by 1 or more terrain images.
    ConvertTempMapToCombatTerrain(charCombatStartX, charCombatStartY);

    int fx = 0xffff;
    int fy = 0xffff;
    int lx = 0;
    int ly = 0;
    ty = 0;

    // fill holes in terrain map with floor tiles
    while (ty < MAX_TERRAIN_HEIGHT) {
        tx = 0;
        while (tx < MAX_TERRAIN_WIDTH) {
            if (isEmpty(ty, tx)) {
                SetDungeon(ty, tx, DUNGEON_EMPTY_TILE);
            }
            else {
                // for diagnostics purposes, find upper left and lower right wall
                if (tx < fx) fx = tx;
                if (tx > lx) lx = tx;
                if (ty < fy) fy = ty;
                if (ty > ly) ly = ty;
            }
            tx++;
        }
        ty++;
    }
    if (logDebuggingInfo) {
        WriteDebugString("Indoor Combat Map: F %u,%u, L %u,%u\n", fx, fy, lx, ly);
    };

#ifdef diagonalMap
    if (!findEmptyCell(charCombatStartX, charCombatStartY, 1, 1, dir, NULL))
#else
    if (!findEmptyCell(charCombatStartX, charCombatStartY, 1, 1, dir, NULL))
#endif
    {
        WriteDebugString("Failed to locate char start loc in combat map\n");

        die(0xab4dd);
        charCombatStartX = 0;
        charCombatStartY = 0;

        if (!findEmptyCell(charCombatStartX, charCombatStartY, 1, 1, dir, NULL))
            WriteDebugString("Failed to relocate char start loc to 0,0\n");
    }
    //#ifndef diagonalMap
    partyX = charCombatStartX;
    partyY = charCombatStartY;
    //#endif
    for (i = 0; i <= MAX_TEMP_HEIGHT; i++)
        My_free(tempArea[i]);
    My_free(tempArea);
    tempArea = NULL;
    for (i = 0; i <= MAX_TEMP_HEIGHT; i++)
        My_free(tempArea2[i]);
    My_free(tempArea2);
    tempArea2 = NULL;
}
*****************************************************************************/
}

Drawtile.prototype.GenerateOutdoorCombatMap = function () {
/*****************************************************************************
// NAME:    GenerateOutdoorCombatMap
//
// PURPOSE: 
// translates the current area map into equivalent combat dungeon terrain tile numbers

**TODO**
void GenerateOutdoorCombatMap(int & partyX, int & partyY, PATH_DIR dir)
{
    WORD i, j;

    int charCombatStartY = MAX_TERRAIN_HEIGHT / 2;
    int charCombatStartX = MAX_TERRAIN_WIDTH / 2;
    // only need to allocate the combat map once
    if (terrain == NULL) {
        terrain = (TERRAIN_CELL **) My_malloc(sizeof(TERRAIN_CELL *) * MAX_TERRAIN_HEIGHT);
        for (i = 0; i < MAX_TERRAIN_HEIGHT; i++)
            terrain[i] = (TERRAIN_CELL *) My_malloc(sizeof(TERRAIN_CELL) * MAX_TERRAIN_WIDTH);
    }

    for (i = 0; i < MAX_TERRAIN_HEIGHT; i++) {
        for (j = 0; j < MAX_TERRAIN_WIDTH; j++) {
            memset(& terrain[i][j], 0, sizeof(TERRAIN_CELL));
            terrain[i][j].tileIndex = NO_DUDE;
            terrain[i][j].tileIndexDead = NO_DUDE;
        }
    }

    memset(CurrentTileData, 0, sizeof(CurrentTileData));

    tempArea = (TERRAIN_CELL **) My_malloc(sizeof(TERRAIN_CELL *) * MAX_TEMP_HEIGHT);
    for (i = 0; i < MAX_TEMP_HEIGHT; i++)
        tempArea[i] = (TERRAIN_CELL *) My_malloc(sizeof(TERRAIN_CELL) * MAX_TEMP_WIDTH);

    for (i = 0; i < MAX_TEMP_HEIGHT; i++)
        for (j = 0; j < MAX_TEMP_WIDTH; j++)
            memset(& tempArea[i][j], 0, sizeof(TERRAIN_CELL));

    // copy wilderness tile data to current data
    CurrentTileCount = WILDERNESS_TILE_COUNT;
    for (i = 0; i <= CurrentTileCount; i++)
        CurrentTileData[i] = WildernessWallTiles[i];

    //float NumTiles = MAX_TEMP_HEIGHT * MAX_TEMP_WIDTH;
    int NumTiles = MAX_TEMP_HEIGHT * MAX_TEMP_WIDTH;
    DWORD NumTileGroups = NUM_WILDERNESS_TILE_GROUPS;

    // Each group type has a density rating.
    // Loop through all groups
    //   - Determine qty of each group to be placed on map
    //   - Place at random locations on map

    for (i = 0; i < NumTileGroups; i++) {
        // if non-zero density
        if (WildernessTileDensity[i] > 0) {
            //float pcnt = WildernessTileDensity[i] / 100.0;
            int tenthPcnt = WildernessTileDensity[i];
            //DWORD qty = (DWORD)(NumTiles * pcnt);
            DWORD qty = (NumTiles * tenthPcnt) / 1000;
            //       WriteDebugString("DEBUG - NumTiles=%u; tenthPcnt=%u; qty=%u\n", NumTiles, tenthPcnt, qty);

            // convert index into group type
            int grp = i + WBASE;

            WriteDebugString("Creating Wilderness Group %u: %u over %u tiles\n",
                grp, qty, (DWORD)NumTiles);
            // place qty of this tile group on map
            // at random locations
            for (j = 0; j < qty; j++) {
                // determine destination x,y
                int x = RollDice(MAX_TEMP_WIDTH, 1, 0) - 1;
                int y = RollDice(MAX_TEMP_HEIGHT, 1, 0) - 1;

                switch (grp) {
                    case DBL_HIGH_1:
                    case DBL_HIGH_2:
                    case DBL_HIGH_3:
                    case DBL_HIGH_4:
                    case DBL_HIGH_5:
                        // these groups use 2 cells
                        if (ValidTempCoords(y, x) && ValidTempCoords(y - 1, x)) {
                            if ((tempArea[y][x].cell == NO_TERRAIN)
                                && (tempArea[y - 1][x].cell == NO_TERRAIN))
                                tempArea[y][x].cell = grp;
                        }
                        break;

                    default:
                        // these groups use 1 cell
                        if (tempArea[y][x].cell == NO_TERRAIN)
                            tempArea[y][x].cell = grp;
                        break;
                }
            }
        }
    }

    // now we need to convert the tile group types
    // into indexes into the combat terrain image. Each type will be
    // represented by 1 or more terrain images.
    ConvertTempMapToCombatTerrain(charCombatStartX, charCombatStartY);

    WORD fx = 0xffff;
    WORD fy = 0xffff;
    WORD lx = 0;
    WORD ly = 0;
    WORD y = 0;

    // fill holes in terrain map with floor tiles
    while (y < MAX_TERRAIN_HEIGHT) {
        WORD x = 0;
        while (x < MAX_TERRAIN_WIDTH) {
            if (isEmpty(y, x)) {
                SetWilderness(y, x, WILDERNESS_EMPTY_TILE);
            }
            else {
                // for diagnostics purposes, find upper left and lower right wall
                if (x < fx) fx = x;
                if (x > lx) lx = x;
                if (y < fy) fy = y;
                if (y > ly) ly = y;
            }
            x++;
        }
        y++;
    }

    WriteDebugString("Outdoor Combat Map: F %u,%u, L %u,%u\n", fx, fy, lx, ly);

    if (!findEmptyCell(charCombatStartX, charCombatStartY, 1, 1, dir, NULL)) {
        WriteDebugString("Failed to locate char start loc in combat map\n");

        die(0xab4de);
        charCombatStartX = 0;
        charCombatStartY = 0;

        if (!findEmptyCell(charCombatStartX, charCombatStartY, 1, 1, dir, NULL))
            WriteDebugString("Failed to relocate char start loc to 0,0\n");
    }

    partyX = charCombatStartX;
    partyY = charCombatStartY;

    for (i = 0; i < MAX_TEMP_HEIGHT; i++)
        My_free(tempArea[i]);
    My_free(tempArea);
    tempArea = NULL;
}

*****************************************************************************/
}



Drawtile.prototype.ObsticalType = function(x, y, w, h, CheckOccupants, ignoreCurrentCombatant, pCombatantLinger) {
    var i, j, dude;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            if (!this.coordsOnMap(x + j, y + i, 1, 1)) return OBSTICAL_TYPE.OBSTICAL_offMap;
            if (!this.HaveMovability(y + i, x + j)) return OBSTICAL_TYPE.OBSTICAL_wall;

            if (CheckOccupants) {
                dude = Drawtile.getCombatantInCell(x + j, y + i, 1, 1);
                if ((dude != NO_DUDE) && (!ignoreCurrentCombatant || (dude != combatData.GetCurrCombatant())))
                    return OBSTICAL_TYPE.OBSTICAL_occupied;
                if (pCombatantLinger != null) {
                    if (activeSpellList.LingerSpellBlocksCombatant(x + j, y + i, pCombatantLinger)) return OBSTICAL_TYPE.OBSTICAL_lingeringSpell;
                };
            }
        }
    }
    return OBSTICAL_TYPE.OBSTICAL_none;
}

Drawtile.prototype.coordsOnMap = function (x, y, width, height) {
    var i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            if (((x + j) < 0) || ((x + j) >= Drawtile.MAX_TERRAIN_WIDTH)) return false;
            if (((y + i) < 0) || ((y + i) >= Drawtile.MAX_TERRAIN_HEIGHT)) return false;
        }
    }
    return true;
}

Drawtile.prototype.HaveMovability = function (y, x) {
    if (!this.ValidCoords(y, x)) return false;
    var cell = this.terrain[y][x].cell;
    if ((cell < 1) || (cell > this.CurrentTileCount)) return false;
    return (this.CurrentTileData[cell].tile_passable > 0);
}

Drawtile.prototype.ValidCoords = function(y, x) {
    if ((x < 0) || (x >= Drawtile.MAX_TERRAIN_WIDTH)) return false;
    if ((y < 0) || (y >= Drawtile.MAX_TERRAIN_HEIGHT)) return false;
    return true;
}


Drawtile.prototype.EnsureVisibleTargetTargetForceCenter = function(targ, forceCenter) {
    this.EnsureVisibleXYForceCenter(combatData.GetCombatant(targ).GetCenterX(),
        combatData.GetCombatant(targ).GetCenterY(),
        forceCenter);
    combatData.GetCombatant(targ).OnEnsureVisible();
}

Drawtile.prototype.placeCombatant = function (x, y, dude, w, h) {
    var i, j;

    if ((w <= 0) || (h <= 0)) {
         if (!debugStrings.AlreadyNoted("BWH01")) {
            Globals.WriteDebugString("Bogus w,h for icon in placeCombatant()\n");
            Globals.WriteDebugString("Combatant's name is " + combatData.GetCombatant(dude).GetName() + "\n");
        }
    }

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            if (Drawtile.ValidCoords(y + i, x + j))
                this.terrain[y + i][x + j].tileIndex = dude;
        }
    }
}


Drawtile.prototype.EnsureVisibleXYForceCenter = function(tx, ty, forceCenter) {
    if (!forceCenter) {
        // the '=' is used so that if icons are on the 
        // edge of the visible map the visible portion
        // will still be shifted by one from the icon location,
        // giving a 1 square gap between icon and map edge.
        if (tx <= combatData.m_iStartTerrainX)
            combatData.m_iStartTerrainX = tx - 1;
        else if (tx >= (combatData.m_iStartTerrainX + Globals.TILES_HORZ - 1))
            combatData.m_iStartTerrainX = (tx - Globals.TILES_HORZ) + 2;

        if (ty <= combatData.m_iStartTerrainY)
            combatData.m_iStartTerrainY = ty - 1;
        else if (ty >= (combatData.m_iStartTerrainY + Globals.TILES_VERT - 1))
            combatData.m_iStartTerrainY = (ty - Globals.TILES_VERT) + 2;
    }
    else {
        combatData.m_iStartTerrainX = tx - (Globals.TILES_HORZ / 2);
        combatData.m_iStartTerrainY = ty - (Globals.TILES_VERT / 2);
    }

    if (combatData.m_iStartTerrainX < 0)
        combatData.m_iStartTerrainX = 0;
    else if ((combatData.m_iStartTerrainX + Globals.TILES_HORZ) > Drawtile.MAX_TERRAIN_WIDTH)
        combatData.m_iStartTerrainX = Drawtile.MAX_TERRAIN_WIDTH - Globals.TILES_HORZ + 1;

    if (combatData.m_iStartTerrainY < 0)
        combatData.m_iStartTerrainY = 0;
    else if ((combatData.m_iStartTerrainY + Globals.TILES_VERT) > Drawtile.MAX_TERRAIN_HEIGHT)
        combatData.m_iStartTerrainY = Drawtile.MAX_TERRAIN_HEIGHT - Globals.TILES_VERT + 1;
}

Drawtile.prototype.getCombatantInCell = function(x, y, w, h, ignoreDude) {
    for (var i = 0; i < h; i++)
    {
        for (var j = 0; j < w; j++)
        {
            if (this.ValidCoords(y + i, x + j)) {
                if ((this.terrain[y + i][x + j].tileIndex != NO_DUDE)
                    && (this.terrain[y + i][x + j].tileIndex != ignoreDude)) {
                    if (this.terrain[y + i][x + j].tileIndex < combatData.NumCombatants()) //m_aCombatants.GetSize())
                        return this.terrain[y + i][x + j].tileIndex;
                    else
                        terrain[y + i][x + j].tileIndex = NO_DUDE;
                }
            }
        }
    }
    return NO_DUDE;
}


Drawtile.prototype.IsLineOfSight = function (x0, y0, x1, y1) {
    var flags = 0, octant = 0;
    var xDistance = 0, yDistance = 0;
    xDistance = x1 - x0;
    yDistance = y1 - y0;
    // Divide the circle into 8 octants.
    octant = (xDistance > 0) ? 0 : 4;
    if (yDistance > 0) octant += 2;
    if (Math.abs(yDistance) > Math.abs(xDistance)) octant += 1;
    switch (octant) {                     //  bigd 
        //     littled                   larger
        case 0:
            flags = this.TestLineOfSight(x0, y0, 1, 0, 0, -1, xDistance, -yDistance);
            break;
        case 1:
            flags = this.TestLineOfSight(x1, y1, 0, 1, -1, 0, -yDistance, xDistance);
            break;
        case 2:
            flags = this.TestLineOfSight(x0, y0, 1, 0, 0, 1, xDistance, yDistance);
            break;
        case 3:
            flags = this.TestLineOfSight(x0, y0, 0, 1, 1, 0, yDistance, xDistance);
            break;
        case 4:
            flags = this.TestLineOfSight(x1, y1, 1, 0, 0, 1, -xDistance, -yDistance);
            break;
        case 5:
            flags = this.TestLineOfSight(x1, y1, 0, 1, 1, 0, -yDistance, -xDistance);
            break;
        case 6:
            flags = this.TestLineOfSight(x1, y1, 1, 0, 0, -1, -xDistance, yDistance);
            break;
        case 7:
            flags = this.TestLineOfSight(x0, y0, 0, 1, -1, 0, yDistance, -xDistance);
            break;
    };
    return flags == 0;
}


Drawtile.prototype.TestLineOfSight = function(x0, y0, dx1, dy1, dx2, dy2, c, dc) {
    // Return non-zero if wall encountered.
    var flags = 0;
    var i = 0;
    var cMax = 2 * c;
    var numStep = c;
    c += dc;
    dc *= 2;
    for (i = 0; (i < numStep) && (flags == 0); i++) {
        // Examine the left and right cells at x = x0
        //
        // Examine left side of line.

        if ((i | c) != 0) {
            flags |= this.TestLineOfSight(x0, y0);
        };

        x0 += dx1; y0 += dy1;

        //
        // Examine right side of line. 
        //
        if (i != numStep - 1) {
            flags |= this.TestLineOfSight(x0, y0);
        };

        c += dc;
        if (c >= cMax) { x0 += dx2; y0 += dy2; c -= cMax; };
    };
    return flags;
}

Drawtile.prototype.ForceMapToMoveAlong = function(x, y, dir, MoveSize) {
    var force = false;
    switch (dir) {
        case PATH_DIR.PathNorth:
            force = (y <= combatData.m_iStartTerrainY + MoveSize);
            break;
        case PATH_DIR.PathEast:
            force = (x >= combatData.m_iStartTerrainX + Globals.TILES_HORZ - (MoveSize + 1));
            break;
        case PATH_DIR.PathSouth:
            force = (y >= combatData.m_iStartTerrainY + Globals.TILES_VERT - (MoveSize + 1));
            break;
        case PATH_DIR.PathWest:
            force = (x <= combatData.m_iStartTerrainX + MoveSize);
            break;
        case PATH_DIR.PathNW:
            force = ((y <= combatData.m_iStartTerrainY + MoveSize) || (x <= combatData.m_iStartTerrainX + MoveSize));
            break;
        case PATH_DIR.PathNE:
            force = ((y <= combatData.m_iStartTerrainY + MoveSize) || (x >= combatData.m_iStartTerrainX + Globals.TILES_HORZ - (MoveSize + 1)));
            break;
        case PATH_DIR.PathSW:
            force = ((y >= combatData.m_iStartTerrainY + Globals.TILES_VERT - (MoveSize + 1)) || (x <= combatData.m_iStartTerrainX + MoveSize));
            break;
        case PATH_DIR.PathSE:
            force = ((y >= combatData.m_iStartTerrainY + Globals.TILES_VERT - (MoveSize + 1)) || (x >= combatData.m_iStartTerrainX + Globals.TILES_HORZ - (MoveSize + 1)));
            break;
    }

    if (force)
        this.moveMap(dir, MoveSize);

    return force;
}

Drawtile.prototype.moveMap = function(dir, MoveSize) {
    UIEventManager.centerMap();
}

Drawtile.prototype.Distance6 = function(attacker, sX, sY, attackee, dX, dY) {
    var tmpSrcX = sX, tmpSrcY = sY;
    var tmpDstX = dX, tmpDstY = dY;
    var origSX = sX;
    var origDX = dX;

    // for icons bigger than 1x1, we need to adjust the
    // src and dst x,y to be the closest possible for computing
    // distance between icons.
    //
    // the starting x,y for each icon is the upper left corner
    //
    if (sX != dX) {
        if (sX < dX) {
            sX++;
            while ((sX != dX)
                && (Drawtile.ValidCoords(sY, sX))
                && (Drawtile.getCombatantInCell(sX, sY, 1, 1) == attacker)) {
                sX++;
            }

            tmpSrcX = sX - 1;

            dX--;
            while ((tmpSrcX != dX)
                && (Drawtile.ValidCoords(dY, dX))
                && (Drawtile.getCombatantInCell(dX, dY, 1, 1) == attackee)) {
                dX--;
            }

            tmpDstX = dX + 1;
        }
        else {
            sX--;
            while ((sX != dX)
                && (Drawtile.ValidCoords(sY, sX))
                && (Drawtile.getCombatantInCell(sX, sY, 1, 1) == attacker)) {
                sX--;
            }

            tmpSrcX = sX + 1;

            dX++;
            while ((tmpSrcX != dX)
                && (Drawtile.ValidCoords(dY, dX))
                && (Drawtile.getCombatantInCell(dX, dY, 1, 1) == attackee)) {
                dX++;
            }

            tmpDstX = dX - 1;
        }
    }

    sX = origSX;
    dX = origDX;

    if (sY != dY) {
        if (sY < dY) {
            sY++;
            while ((sY != dY)
                && (Drawtile.ValidCoords(sY, sX))
                && (Drawtile.getCombatantInCell(sX, sY, 1, 1) == attacker)) {
                sY++;
            }

            tmpSrcY = sY - 1;

            dY--;
            while ((tmpSrcY != dY)
                && (Drawtile.ValidCoords(dY, dX))
                && (Drawtile.getCombatantInCell(dX, dY, 1, 1) == attackee)) {
                dY--;
            }

            tmpDstY = dY + 1;
        }
        else {
            sY--;
            while ((sY != dY)
                && (Drawtile.ValidCoords(sY, sX))
                && (Drawtile.getCombatantInCell(sX, sY, 1, 1) == attacker)) {
                sY--;
            }

            tmpSrcY = sY + 1;

            dY++;
            while ((tmpSrcY != dY)
                && (Drawtile.ValidCoords(dY, dX))
                && (Drawtile.getCombatantInCell(dX, dY, 1, 1) == attackee)) {
                dY++;
            }

            tmpDstY = dY - 1;
        }
    }

    return Drawtile.Distance4(tmpSrcX, tmpSrcY, tmpDstX, tmpDstY);
}


Drawtile.prototype.Distance4 = function(sx, sy, dx, dy)
{
    var x = sx - dx;
    var y = sy - dy;

    var temp = Math.sqrt(((x * x) + (y * y)));

    var result = Math.floor(temp + 0.5);
    return result;
}

Drawtile.prototype.HaveLineOfSight = function(x0, y0, x1, y1, reflects) {
    // starting point of line
    var x = x0, y = y0;

    // direction of line
    var dx = x1 - x0, dy = y1 - y0;

    // increment or decrement depending on direction of line
    var sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
    var sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));

    // decision parameters for voxel selection
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    var ax = 2 * dx, ay = 2 * dy;
    var decx, decy;

    // determine largest direction component, single-step related variable
    var max = dx, _var = 0;    //PORT NOTE:  Had to rename variable called var
    if (dy > max) { _var = 1; }

    // traverse Bresenham line
    switch ( _var ) {
        case 0:  // single-step in x-direction
            for (decy = ay - dx; ; x += sx, decy += ay) {
                // process pixel
                if (!(x == x0 && y == y0) && !(x == x1 && y == y1)) {
                    if (!Drawtile.HaveVisibility(y, x, reflects))
                        return false;
                }

                // take Bresenham step
                if (x == x1) break;
                if (decy >= 0) { decy -= ax; y += sy; }
            }
            break;
        case 1:  // single-step in y-direction
            for (decx = ax - dy; ; y += sy, decx += ax) {
                // process pixel
                if (!(x == x0 && y == y0) && !(x == x1 && y == y1)) {
                    if (!Drawtile.HaveVisibility(y, x, reflects))
                        return FALSE;
                }

                // take Bresenham step
                if (y == y1) break;
                if (decx >= 0) { decx -= ay; x += sx; }
            }
            break;
    }

    return true;
}

Drawtile.prototype.placeDyingCombatant = function(x, y, dude, w, h) {
    var i = 0, j = 0;

    if ((w <= 0) || (h <= 0)) {
        if (!debugStrings.AlreadyNoted(CString("BWH02"))) {
            Globals.WriteDebugString("Bogus w,h for icon in placeDyingCombatant()\n");
            Globals.WriteDebugString("Combatant's name is " + combatData.GetCombatant(dude).GetName() + "\n", );
        };
    }

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            if (this.ValidCoords(y + i, x + j))
                this.terrain[y + i][x + j].tileIndexDead = dude;
        }
    }
}

Drawtile.prototype.EnsureVisibleTarget = function(targ, forceCenter) {
    this.EnsureVisible(combatData.GetCombatant(targ).GetCenterX(),
        combatData.GetCombatant(targ).GetCenterY(),
        forceCenter);
    combatData.GetCombatant(targ).OnEnsureVisible();
}


Drawtile.prototype.EnsureVisible = function(tx, ty, forceCenter)
{
    if (!forceCenter) {
        // the '=' is used so that if icons are on the 
        // edge of the visible map the visible portion
        // will still be shifted by one from the icon location,
        // giving a 1 square gap between icon and map edge.
        if (tx <= combatData.m_iStartTerrainX)
            combatData.m_iStartTerrainX = tx - 1;
        else if (tx >= (combatData.m_iStartTerrainX + Globals.TILES_HORZ - 1))
            combatData.m_iStartTerrainX = (tx - Globals.TILES_HORZ) + 2;

        if (ty <= combatData.m_iStartTerrainY)
            combatData.m_iStartTerrainY = ty - 1;
        else if (ty >= (combatData.m_iStartTerrainY + Globals.TILES_VERT - 1))
            combatData.m_iStartTerrainY = (ty - Globals.TILES_VERT) + 2;
    }
    else {
        combatData.m_iStartTerrainX = tx - (Globals.TILES_HORZ / 2);
        combatData.m_iStartTerrainY = ty - (Globals.TILES_VERT / 2);
    }

    if (combatData.m_iStartTerrainX < 0)
        combatData.m_iStartTerrainX = 0;
    else if ((combatData.m_iStartTerrainX + Globals.TILES_HORZ) > Drawtile.MAX_TERRAIN_WIDTH)
        combatData.m_iStartTerrainX = Drawtile.MAX_TERRAIN_WIDTH - Globals.TILES_HORZ + 1;

    if (combatData.m_iStartTerrainY < 0)
        combatData.m_iStartTerrainY = 0;
    else if ((combatData.m_iStartTerrainY + Globals.TILES_VERT) > Drawtile.MAX_TERRAIN_HEIGHT)
        combatData.m_iStartTerrainY = Drawtile.MAX_TERRAIN_HEIGHT - Globals.TILES_VERT + 1;
}