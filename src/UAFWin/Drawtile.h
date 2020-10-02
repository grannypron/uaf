/******************************************************************************
* Filename: Drawtile.h
* Copyright (c) 2000, UAF Development Team (email CocoaSpud@hotmail.com)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/
#ifndef __DRAWTILE_H__
#define __DRAWTILE_H__

const double VALUE_OF_PI = 3.141592653589795;

extern long SpriteBkGndArt;

const RECT CursorSrc        = { 0,0,48,48 };
const RECT DeadCharSrcA     = { 0,0,48,48 };
const RECT PetrifiedCharSrcA= { 0,0,48,48 };
const RECT DeadCharSrcB     = { 48,0,96,48 };

struct COMBAT_SPRITE_DATA
{
  COMBAT_SPRITE_DATA() { Clear(); }
  COMBAT_SPRITE_DATA(const COMBAT_SPRITE_DATA &src) { *this = src; }
  
  void Clear();
  void Init(const PIC_DATA &data, BOOL path, CString prefix, int x1, int y1, int x2, int y2, BOOL reflects);
   // no path, forced dir
  void Init(const PIC_DATA &data, CString prefix, int x1, int y1, PATH_DIR direction);
  // adds to AnimateList, for stationary animations
  void Init(const PIC_DATA &data, CString prefix, int x1, int y1);
  void Init(const PIC_DATA &data, CString prefix, int combatantIndex);

  COMBAT_SPRITE_DATA& operator =(const COMBAT_SPRITE_DATA& src);
    
  BOOL Update();
  void Display();
  BOOL IsSpriteDone();

  PIC_DATA picData;
  BOOL UsePath;
  bool spriteAttachedToCombatant;

  int swx, swy;
  int dwx, dwy;

  int stx, sty;
  int dtx, dty;

  PATH_DIR dir;
  LONGLONG lastUpdateTime;
  DWORD    TotalDeltaTime;
  DWORD    MinDeltaTime;
  BYTE     lastUpdateFrame;
  int      activeSpellKey;

  CList<CPoint, CPoint> pixelPath;
};

extern COMBAT_SPRITE_DATA combatSprite;

// Used for combat terrain map wall set data
struct TILE_DATA
{
	int tile_posx;         
	int tile_posy;         
	BYTE tile_invisible; //visibility;  
	BYTE tile_passable; //obstacle;    
  BYTE tile_enabled; 
};

struct TERRAIN_CELL
{
  short int cell;
  // dying dude is drawn before regular dude!
	short int tileIndex; // combat index of dude in this cell
  short int tileIndexDead; // combat index of dying dude in this cell
};

/*
// size of combatant stat box
enum { COMBAT_STAT_BOX_WIDTH = 240 };
*/


enum OBSTICAL_TYPE
{
  OBSTICAL_none           = 0,
  OBSTICAL_wall           = 1,
  OBSTICAL_occupied       = 2,
  OBSTICAL_offMap         = 3,
  OBSTICAL_lingeringSpell = 4,
};

// size of combat menu box
enum { COMBAT_MENU_HEIGHT = 20 };

// upper left corner of viewable area, just inside the combat border
//enum { CombatScreenX = 14, CombatScreenY = 16 };
//enum { CombatScreenX = 14, CombatScreenY = 14 };	// manikus 5-30-11
extern int CombatScreenX;
extern int CombatScreenY;

//#define COMBAT_SCREEN_WIDTH  (SCREEN_WIDTH-COMBAT_STAT_BOX_WIDTH) 
//#define COMBAT_SCREEN_HEIGHT ((SCREEN_HEIGHT-COMBAT_MENU_HEIGHT)-(HorzBarSrc.bottom-HorzBarSrc.top))

// number of viewable char tiles in both directions
// screen size is determined at runtime
//#define TILES_HORZ  (( (( SCREEN_WIDTH-CombatScreenX) - COMBAT_STAT_BOX_WIDTH) ) / COMBAT_TILE_WIDTH)
//#define TILES_VERT  (( ((SCREEN_HEIGHT-CombatScreenY) - COMBAT_MENU_HEIGHT)    ) / COMBAT_TILE_HEIGHT)
extern int TILES_HORZ;
extern int TILES_VERT;

// Size of the world in pixels
#define MAX_WORLD_WIDTH  (MAX_TERRAIN_WIDTH  * COMBAT_TILE_WIDTH)
#define MAX_WORLD_HEIGHT (MAX_TERRAIN_HEIGHT * COMBAT_TILE_HEIGHT)

extern TERRAIN_CELL **terrain;

int TerrainToScreenCoordX(int tx);
int TerrainToScreenCoordY(int ty);

BOOL HaveLineOfSight(int x0, int y0, int x1, int y1, BOOL *reflects);

void GenerateIndoorCombatMap(int &plyrX, int &plyrY, PATH_DIR dir); 
void GenerateOutdoorCombatMap(int &plyrX, int &plyrY, PATH_DIR dir); 

void displayCombatWalls();
void displayCombatBorders(BOOL displayMiddleBar=TRUE);

void EnsureVisible(int tx, int ty, BOOL forceCenter=FALSE);
void EnsureVisibleCursor(BOOL forceCenter = FALSE);
void EnsureVisibleTarget(int targ, BOOL forceCenter=FALSE);
void moveMap(PATH_DIR dir, int MoveSize);
BOOL ForceMapToMoveAlong(int x, int y,  PATH_DIR dir, int MoveSize);
BOOL ForceMapToFollowMissile(int x, int y, PATH_DIR dir);

OBSTICAL_TYPE ObsticalType(int x, int y, int w, int h, 
                           BOOL CheckOccupants, 
                           BOOL ignoreCurrentCombatant,
                           COMBATANT *pCombatantLinger);
BOOL ValidCoords(int y,int x);
BOOL HaveMovability(int y, int x);
BOOL findEmptyCell(int &x, int &y, int w, int h,PATH_DIR dir, COMBATANT *pCombatantLinger);
int getHallWidth(int combatX, int combatY, int dir);
BOOL coordsOnMap(int x, int y, int w, int h);
int getCombatantInCell(int x, int y, int w, int h, int ignoreDude=NO_DUDE);
int getDeadCombatantInCell(int x, int y, int w, int h, int ignoreDude=NO_DUDE);
void placeCombatant(int x, int y, int dude, int w, int h);
void placeDyingCombatant(int x, int y, int dude, int w, int h);
int Distance(int attacker, int sX, int sY, int attackee, int dX, int dY);
int Distance(int sX, int sY, int attackee, int dX, int dY);
int Distance(int sx, int sy, int dx, int dy);
//void CreateStraightLinePath(UINT x1, UINT y1, UINT x2, UINT y2);

#define SQUARE_MAP_TARGET 1
#define CIRCLE_MAP_TARGET 2
#define CONE_MAP_TARGET   3
#define LINE_MAP_TARGET   4

// return nbr of dudes in list
//int GetCombatantsInRectangle(MAPTARGETLIST &list, int sx, int sy, int dirX, int dirY, int width, int height);
int GetCombatantsInRectangle(MAPTARGETLIST &list, MAPLOCATIONLIST& locations);
int GetCombatantsInCircle(MAPTARGETLIST &list, int sx, int sy, int radius);
//int GetCombatantsInCone(MAPTARGETLIST &list, int sx, int sy, int length, int width, PATH_DIR dir);
int GetCombatantsAndTilesInCone(MAPTARGETLIST &clist, 
                                MAPLOCATIONLIST &tlist, 
                                int Cx, int Cy, 
                                int Tx, int Ty, 
                                int length, int width,
                                bool forceNonzero);
int GetCombatantsInLine(MAPTARGETLIST &list, int sx, int sy, int dx, int dy);
int GetCombatantsInLine(MAPTARGETLIST &list, int sx, int sy, int length, int width, PATH_DIR dir);
int GetCombatantsInLine(MAPTARGETLIST &list, int sx, int sy, int dx, int dy, int width);

int GetMapTilesInRectangle(MAPLOCATIONLIST &list,
                           int sx, int sy, 
                           int dirX, int dirY, 
                           int width, int height,
                           bool forceNonzero);
int GetMapTilesInCircle(MAPLOCATIONLIST &list, int sx, int sy, int radius);
int GetMapTilesInCone(MAPLOCATIONLIST &list, int sx, int sy, int length, int width, PATH_DIR dir);
int GetMapTilesInLine(MAPLOCATIONLIST &list, int sx, int sy, int dx, int dy);
int GetMapTilesInLine(MAPLOCATIONLIST &list, int sx, int sy, int length, int width, PATH_DIR dir);
int GetMapTilesInLine(MAPLOCATIONLIST &list, int sx, int sy, int dx, int dy, int width);

//void GetMapLocationList(MAPLOCATIONLIST &list);

PATH_DIR GetSpellTargetingDir(int cx, int cy, int tx, int ty);
double GetAngle(double dX1, double dY1, double dX2, double dY2);
void GetRotatedPoint(double& rPointX, double& rPointY, double degrees, double RotatePointX=0, double RotatePointY=0);
double DegreesToRadians(double degrees);
double RadiansToDegrees(double radians);



BOOL IntersectPointWithCombatMap(long mx, long my, int &tx, int &ty);
int PathDirToFaceDir(PATH_DIR dir);
PATH_DIR FaceDirToPathDir(int dir);
PATH_DIR EventDirToPathDir(eventDirType dir);
PATH_DIR PathDirLeft(PATH_DIR dir);
PATH_DIR PathDirRight(PATH_DIR dir);
void Line2D (DWORD data, int x0, int y0, int x1, int y1, BOOL (*callback)(DWORD,int,int, BOOL *));
void LineReflects2D (DWORD data, int x0, int y0, int x1, int y1, BOOL (*callback)(DWORD,int,int, BOOL *));
BOOL ValidTerrainViewportCoords(int tx, int ty);
BOOL BothOnScreen(int tx1, int ty1, int tx2, int ty2);

#ifdef _DEBUG
int GetCellNum(int x, int y);
TILE_DATA &GetTileData(int x, int y);
#endif

void ClearEmptyCellSearch();

// these pics are continually animated until
// removed from this list
extern CList<COMBAT_SPRITE_DATA*, COMBAT_SPRITE_DATA*> AnimateList;

void ClearAnimateList();
void DisplayAnimateList();
bool UpdateAnimateList();
void RemoveAnimationFromList(int key);

//*****************************************************************************
//*****************************************************************************



#endif