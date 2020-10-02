 
/******************************************************************************
* Filename: Drawtile.cpp
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
//
//  This file has the combat map creation and drawing code
//


#include "..\Shared\stdafx.h"

#define diagonalMap


//#include "externs.h"
#include "Dungeon.h"

#include "items.h"
#include "char.h"
#include "Graphics.h"
#include "GlobalData.h"
#include "level.h"
#include "combatants.h"
#include "PicSlot.h"
#include "PicData.h"
#include "drawtile.h"
#include "Path.h"
#include "party.h"
#include <math.h>
#include "Viewport.h"


void *My_malloc(int size);
void My_free(void *addr);

extern BOOL logDebuggingInfo;
extern int useWallIndex;
extern int useDoorAndOverlayIndex;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG

// enable this to display targeting info on map, debug mode only
#define SHOW_SPELL_TARGET_OUTLINES 1

#endif

long SpriteBkGndArt = -1;


// Used while building combat map from current level data
enum WALLTYPE { NoWall = 0, Wall, OpenDoor, ClosedDoor };

int COMBAT_STAT_BOX_WIDTH = 240;

#define MAP_H_MULTIPLIER 8
#define MAP_V_MULTIPLIER 7

int MAX_TERRAIN_HEIGHT = 50;
int MAX_TERRAIN_WIDTH = 50;

#define MAX_TEMP_HEIGHT ((MAX_TERRAIN_HEIGHT+5)*MAP_V_MULTIPLIER)
#define MAX_TEMP_WIDTH  ((MAX_TERRAIN_WIDTH+5)*MAP_H_MULTIPLIER)

// dynamic 2-dimensional array
//
//TERRAIN_CELL ** array;
//unsigned int i;
//unsigned int rows=8
//unsigned int columns=12;

// ** to allocate it
//array = (TERRAIN_CELL **) malloc(sizeof(TERRAIN_CELL *) * rows);
//for (i = 0; i < rows; i++)
//  array[i] = (TERRAIN_CELL *) malloc(sizeof(TERRAIN_CELL) * columns);

// ** to use the array: array[row][column]
//array[0][1] = 'x';

// ** To Free the memory used by the array
//
//for (i = 0; i< rows; i++)
//   free(array[i]);
//free(array); 

// temps used to build combat map before it is stored
// into 'terrain[][]'
TERRAIN_CELL **tempArea=NULL;
TERRAIN_CELL **tempArea2=NULL;

// each combat map is created as needed 
// using the players location within the area map to determine
// wall placement. outdoor maps are randomly generated.
TERRAIN_CELL **terrain=NULL;   // terrain[MAX_TERRAIN_HEIGHT][MAX_TERRAIN_WIDTH]

COMBAT_SPRITE_DATA combatSprite;

// these pics are continually animated until
// removed from this list
CList<COMBAT_SPRITE_DATA*, COMBAT_SPRITE_DATA*> AnimateList;

#define NO_TERRAIN 0

// dungeon terrain types
// each of these can map to multiple tiles
#define VTT_WALL  1     // Vertical Top Terminator
#define V_WALL    2     // Vertical Wall
#define VBT_WALL  3     // Vertical Bottom Terminator
#define HLT_WALL  4     // Horizontal Left Terminator
#define H_WALL    5     // Horizontal Wall
#define HRT_WALL  6     // Horizontal Right Terminatior
#define ULC_WALL  7     // Upper Left Corner
#define TT_WALL   8     // Top 'T'
#define URC_WALL  9     // Upper Right Corner
#define LLC_WALL  10    // Lower Left Corner
#define BT_WALL   11    // Bottom 'T'
#define LRC_WALL  12    // Lower Right Corner
#define INTER_WALL  13  // Intersection
#define LT_WALL   14    // Left 'T'
#define RT_WALL   15    // Right 'T'

// wilderness terrain types
// each of these can map to multiple tiles
#define WBASE 100

#define DBL_HIGH_1  100
#define DBL_HIGH_2  101
#define DBL_HIGH_3  102
#define DBL_HIGH_4  103
#define DBL_HIGH_5  104

#define SGL_HIGH_1  105
#define SGL_HIGH_2  106
#define SGL_HIGH_3  107
#define SGL_HIGH_4  108
#define SGL_HIGH_5  109
#define SGL_HIGH_6  110
#define SGL_HIGH_7  111
#define SGL_HIGH_8  112
#define SGL_HIGH_9  113
#define SGL_HIGH_10 114
#define SGL_HIGH_11 115
#define SGL_HIGH_12 116
#define SGL_HIGH_13 117
#define SGL_HIGH_14 118
#define SGL_HIGH_15 119
#define SGL_HIGH_16 120
#define SGL_HIGH_17 121
#define SGL_HIGH_18 122
#define SGL_HIGH_19 123
#define SGL_HIGH_20 124
#define SGL_HIGH_21 125
#define SGL_HIGH_22 126
#define SGL_HIGH_23 127
#define SGL_HIGH_24 128
#define SGL_HIGH_25 129

#define SGL_HIGH_26 130
#define SGL_HIGH_27 131
#define SGL_HIGH_28 132
#define SGL_HIGH_29 133
#define SGL_HIGH_30 134

#define NUM_WILDERNESS_TILE_GROUPS ((SGL_HIGH_30-WBASE)+1)

int WildernessTileDensity[NUM_WILDERNESS_TILE_GROUPS] =
//float WildernessTileDensity[NUM_WILDERNESS_TILE_GROUPS] =
{  // Units of 1/10th percent 
   // 20140815 PRS
  5,5,5,5,5, // doubles
  0,0,0,0,0,0,0,0,0,0, // unused singles
  5,5,5,5,5, 5,5,5,5,5, // third row
  10,10,10,10,10, 10,10,0,0,0 // fourth row
  //0.5,0.5,0.5,0.5,0.5, // doubles
  //0,0,0,0,0,0,0,0,0,0, // unused singles
  //0.5,0.5,0.5,0.5,0.5, 0.5,0.5,0.5,0.5,0.5, // third row
  //1.0,1.0,1.0,1.0,1.0, 1.0,1.0,0.0,0.0,0.0 // fourth row
};

// Information in TILE_DATA.  First row won't be used.
//
//struct {
//   int posx;       X position of cell frame in art bitmap
//   int posy;       Y position of cell frame in art bitmap
//  BYTE visibility; Visibility modifier for Line-of-Sight (1=see-thru, 0=blocks visible)
//  BYTE obstacle;   Movement obstacle (1=can move into tile, 0=cannot move into tile)
//  bool enabled;    can we use this tile?
//} 

const int DUNGEON_TILE_COUNT = 26;
const int DUNGEON_EMPTY_TILE = 23;

TILE_DATA DungeonWallTiles[DUNGEON_TILE_COUNT+1] = 
{
  {     0,0,0,0,0 },
  {     2,2,0,0,1 },  // 1
  {    62,2,0,0,1 }, 
  {   122,2,0,0,1 }, 
  {   182,2,0,0,1 }, 
  {   242,2,1,1,1 },  // 5
  {   302,2,0,0,1 }, 
  {   362,2,0,0,1 }, 
  {   422,2,0,0,1 }, 
  {   482,2,1,1,1 }, 
  {   542,2,0,0,1 },  // 10
  {    2,62,1,1,1 }, 
  {   62,62,0,0,1 }, 
  {  122,62,1,1,1 }, 
  {  182,62,0,0,1 }, 
  {  242,62,1,1,1 },  // 15
  {  302,62,0,0,1 },
  {  362,62,1,1,1 },
  {  422,62,0,0,1 },
  {  482,62,0,0,1 },
  {  542,62,0,0,1 },  // 20
  {   2,122,0,0,1 },
  {  62,122,0,0,1 },
  { 122,122,1,1,1 },
  { 182,122,1,1,1 },
  { 242,122,0,0,1 },  // 25
  { 302,122,1,1,0 }
};

const int WILDERNESS_TILE_COUNT = 40;
const int WILDERNESS_EMPTY_TILE = 38;

TILE_DATA WildernessWallTiles[WILDERNESS_TILE_COUNT+1] = 
{
  {     0,0,0,0,0 },
  {     2,2,0,0,1 },   // 1
  {    62,2,0,0,1 }, 
  {   122,2,0,0,1 }, 
  {   182,2,0,0,1 }, 
  {   242,2,0,0,1 }, // 5
  {   302,2,1,0,0 }, 
  {   362,2,1,0,0 }, 
  {   422,2,1,0,0 }, 
  {   482,2,1,0,0 }, 
  {   542,2,1,0,0 }, // 10  
  {    2,62,1,1,1 }, 
  {   62,62,1,1,1 }, 
  {  122,62,1,1,1 }, 
  {  182,62,1,1,1 }, 
  {  242,62,1,1,1 }, // 15
  {  302,62,1,0,0 },
  {  362,62,1,0,0 },
  {  422,62,1,0,0 },
  {  482,62,1,0,0 },
  {  542,62,1,0,0 }, // 20  
  {   2,122,1,0,1 },
  {  62,122,1,0,1 },
  { 122,122,1,0,1 },
  { 182,122,1,1,1 },
  { 242,122,1,1,1 }, // 25
  { 302,122,1,0,1 },
  { 362,122,1,0,1 }, 
  { 422,122,1,0,1 }, 
  { 482,122,1,1,1 }, 
  { 542,122,1,1,1 }, // 30
  {   2,182,1,1,1 }, 
  {  62,182,1,1,1 }, 
  { 122,182,1,1,1 }, 
  { 182,182,1,1,1 }, 
  { 242,182,1,1,1 }, // 35
  { 302,182,1,0,1 },
  { 362,182,1,1,1 },
  { 422,182,1,1,1 },
  { 482,182,1,0,0 },
  { 542,182,1,0,0 } // 40
};

const int MAX_TILES = max(WILDERNESS_TILE_COUNT, DUNGEON_TILE_COUNT);
int CurrentTileCount = 0;
TILE_DATA CurrentTileData[MAX_TILES+1];

//
// world coords are the pixel values encompassing the entire combat map
// screen coords are the pixel values currently visible on the screen
// terrain coords are the coords for each combat tile
//
//*****************************************************************************
//    NAME: TerrainToWorldCoordX
//
// PURPOSE: 
//
//*****************************************************************************
inline int TerrainToWorldCoordX(int tx) { return (tx * COMBAT_TILE_WIDTH);  }
//*****************************************************************************
//    NAME: TerrainToWorldCoordY
//
// PURPOSE: 
//
//*****************************************************************************
inline int TerrainToWorldCoordY(int ty) { return (ty * COMBAT_TILE_HEIGHT); }

//*****************************************************************************
//    NAME: WorldToTerrainCoordX
//
// PURPOSE: 
//
//*****************************************************************************
inline int WorldToTerrainCoordX(int wx) { return (wx / COMBAT_TILE_WIDTH);  }
//*****************************************************************************
//    NAME: WorldToTerrainCoordY
//
// PURPOSE: 
//
//*****************************************************************************
inline int WorldToTerrainCoordY(int wy) { return (wy / COMBAT_TILE_HEIGHT); }

//*****************************************************************************
//    NAME: TerrainToScreenCoordX
//
// PURPOSE: 
//
//*****************************************************************************
int TerrainToScreenCoordX(int tx)
{
  // CombatScreenX is an offset to account for frame width
  // in upper left corner
  return ( ((tx-combatData.m_iStartTerrainX) * COMBAT_TILE_WIDTH) + CombatScreenX );
}

//*****************************************************************************
//    NAME: TerrainToScreenCoordY
//
// PURPOSE: 
//
//*****************************************************************************
int TerrainToScreenCoordY(int ty)
{
  // CombatScreenY is an offset to account for frame height
  // in upper left corner
  return ( ((ty-combatData.m_iStartTerrainY) * COMBAT_TILE_HEIGHT) + CombatScreenY );
}

//*****************************************************************************
//    NAME: WorldToScreenCoordX
//
// PURPOSE: 
//
//*****************************************************************************
inline int WorldToScreenCoordX(int wx)
{
  // CombatScreenX is an offset to account for frame width
  // in upper left corner
  return ( (wx - TerrainToWorldCoordX(combatData.m_iStartTerrainX)) + CombatScreenX );
}

//*****************************************************************************
//    NAME: WorldToScreenCoordY
//
// PURPOSE: 
//
//*****************************************************************************
inline int WorldToScreenCoordY(int wy)
{
  // CombatScreenY is an offset to account for frame height
  // in upper left corner
  return ( (wy - TerrainToWorldCoordY(combatData.m_iStartTerrainY)) + CombatScreenY );
}


#ifdef _DEBUG
// debug only
//*****************************************************************************
//    NAME: GetCellNum
//
// PURPOSE: 
//
//*****************************************************************************
int GetCellNum(int x, int y)
{
  return (terrain[y][x].cell);
}

//*****************************************************************************
//    NAME: &GetTileData
//
// PURPOSE: 
//
//*****************************************************************************
TILE_DATA &GetTileData(int x, int y)
{
  ASSERT( ValidCoords(y,x) );
  return CurrentTileData[GetCellNum(x,y)];
}
#endif

//*****************************************************************************
//    NAME: ProcessMissileSpritePixelPath
//
// PURPOSE: callback function for Line2D()
//
//*****************************************************************************
BOOL ProcessMissileSpritePixelPath(DWORD ptr, int x, int y, BOOL *reflects)
{
  // x,y are in world coords
  COMBAT_SPRITE_DATA *pSpr = (COMBAT_SPRITE_DATA*)ptr;
  if (pSpr == NULL) return FALSE; // terminate search

  int tx = WorldToTerrainCoordX(x);
  int ty = WorldToTerrainCoordY(y);

  if (!ValidCoords(ty,tx)) return FALSE; // terminate search  
  if (!HaveLineOfSight(pSpr->stx,pSpr->sty, tx,ty,reflects)) return FALSE;  // terminate search

  pSpr->pixelPath.AddTail(CPoint(x,y));
  return TRUE;
}

//*****************************************************************************
//    NAME: =
//
// PURPOSE: 
//
//*****************************************************************************
COMBAT_SPRITE_DATA& COMBAT_SPRITE_DATA::operator =(const COMBAT_SPRITE_DATA& src) 
{ 
  if (this==&src) return *this;
  Clear();

  picData = src.picData;
  UsePath = src.UsePath;
  swx = src.swx;
  swy = src.swy;
  dwx = src.dwx;
  dwy = src.dwy;
  stx = src.stx;
  sty = src.sty;
  dtx = src.dtx;
  dty = src.dty;  
  dir = src.dir;
  lastUpdateTime = src.lastUpdateTime;
  TotalDeltaTime = src.TotalDeltaTime;
  MinDeltaTime = src.MinDeltaTime;
  lastUpdateFrame = src.lastUpdateFrame;
  activeSpellKey = src.activeSpellKey;
  spriteAttachedToCombatant = false;
  
  POSITION pos = src.pixelPath.GetHeadPosition();
  while (pos != NULL)
    pixelPath.AddTail(src.pixelPath.GetNext(pos));

  return *this;
}

//*****************************************************************************
//    NAME: initializeInRtCombatSprite
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_SPRITE_DATA::Init(const PIC_DATA &data, BOOL path, CString prefix, int x1, int y1, int x2, int y2, BOOL reflects)
{
  Clear();
  if (data.filename.IsEmpty()) return;

  ASSERT( ValidCoords(y1,x1) );  
  ASSERT( (x1 >= 0) && (x1 < MAX_TERRAIN_WIDTH) );
  ASSERT( (y1 >= 0) && (y1 < MAX_TERRAIN_HEIGHT) );
  
  if (x1 < 0)
    x1 = 0;
  else if (x1 >= MAX_TERRAIN_WIDTH)
    x1 = MAX_TERRAIN_WIDTH - 1;

  if (y1 < 0)
    y1 = 0;
  else if (y1 >= MAX_TERRAIN_HEIGHT)
    y1 = MAX_TERRAIN_HEIGHT - 1;

  UsePath=path;
  picData = data;
  picData.LoopCounter=0;

  stx = x1;
  sty = y1;
  swx = TerrainToWorldCoordX(x1);
  swy = TerrainToWorldCoordY(y1);  
  dir = GetDir(x1, y1, x2, y2);

  if (UsePath)
  {
    //ASSERT( ValidCoords(y2,x2) );
    //
    // If reflects then we need to trace the path one step at a time
    // to see if it strikes a wall before going off-screen.
    if (!reflects)
    {
      if (x2 < 0)
        x2 = 0;
      else if (x2 >= MAX_TERRAIN_WIDTH)
        x2 = MAX_TERRAIN_WIDTH - 1;
  
      if (y2 < 0)
        y2 = 0;
      else if (y2 >= MAX_TERRAIN_HEIGHT)
        y2 = MAX_TERRAIN_HEIGHT - 1;
    };

    MinDeltaTime = 60;
    if (picData.style == PIC_DATA::AS_None) // sequenced
    {
      if (picData.NumFrames > 1) picData.flags |= PIC_DATA::AF_Loop;
      picData.flags &= ~PIC_DATA::AF_MaxLoopCounter;
    }

    dtx = x2;
    dty = y2;
    dwx = TerrainToWorldCoordX(x2);
    dwy = TerrainToWorldCoordY(y2);    

    // over-ride dir when targets have small y delta
    // and big x delta
    int xdelta = abs(x2-x1);
    int ydelta = abs(y2-y1);

    if ( (xdelta) && (ydelta) && ((ydelta / xdelta) < 0.20) )
    {
      switch (dir)
      {
      case PathNE:
      case PathSE:
        dir = PathEast;
        break;
      case PathSW:
      case PathNW:
        dir = PathWest;
        break;  
      }
    }
  }
  else
  {
    MinDeltaTime = picData.timeDelay;
    if (MinDeltaTime < 30) MinDeltaTime = 30;
    // although NPC small pics can loop indefinitely,
    // combat animations cannot. The AF_MaxLoopCounter
    // flag must be used.
    picData.flags |= PIC_DATA::AF_MaxLoopCounter;
    if (picData.MaxLoops <= 0) picData.MaxLoops = 1;
    if ((picData.MaxLoops > 1) && (picData.NumFrames > 1))
      picData.flags |= PIC_DATA::AF_Loop;
  }
  
  //WriteDebugString("114 Calling LoadPicSurfaces(%s)\n", prefix);
  picData.LoadPicSurfaces(prefix);

  if (picData.key < 0)
  {
    debugSeverity = 5;
    WriteDebugString("Failed to load sprite surface \'%s\' in CombatSpriteData::Init()\n", picData.filename);
  };
  if (picData.key >= 0)
  {
    // assumes x1,y1 is not same as x2,y2
    if (picData.style == PIC_DATA::AS_Directional)
      picData.SetFrame(PathDirToFaceDir(dir));

    if (UsePath)
    {  
      if (reflects)
      {
        LineReflects2D((DWORD)this,
                       swx, swy, 
                       dwx, dwy, 
                       ProcessMissileSpritePixelPath);
      }
      else
      {
        Line2D((DWORD)this, 
               swx, swy, 
               dwx, dwy, 
               ProcessMissileSpritePixelPath);
      };
    }
  }

//#ifdef _DEBUG
//  WriteDebugString("Loaded %s with MinDelta %u\n", picData.filename, MinDeltaTime);
//#endif
  lastUpdateTime = virtualGameTime;
}

//*****************************************************************************
//    NAME: COMBAT_SPRITE_DATA::Init
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_SPRITE_DATA::Init(const PIC_DATA &data, CString prefix, int x1, int y1, PATH_DIR direction)
{
  Clear();
  if (data.filename.IsEmpty()) return;

  ASSERT( ValidCoords(y1,x1) );  
  ASSERT( (x1 >= 0) && (x1 < MAX_TERRAIN_WIDTH) );
  ASSERT( (y1 >= 0) && (y1 < MAX_TERRAIN_HEIGHT) );
  
  if (x1 < 0)
    x1 = 0;
  else if (x1 >= MAX_TERRAIN_WIDTH)
    x1 = MAX_TERRAIN_WIDTH - 1;
  
  if (y1 < 0)
    y1 = 0;
  else if (y1 >= MAX_TERRAIN_HEIGHT)
    y1 = MAX_TERRAIN_HEIGHT - 1;
  
  UsePath=FALSE;
  picData = data;
  picData.LoopCounter=0;

  stx = x1;
  sty = y1;
  swx = TerrainToWorldCoordX(x1);
  swy = TerrainToWorldCoordY(y1);  
  dir = direction;

  MinDeltaTime = picData.timeDelay;
  if (MinDeltaTime < 30) MinDeltaTime = 30;
  // although NPC small pics can loop indefinitely,
  // combat animations cannot. The AF_MaxLoopCounter
  // flag must be used.
  picData.flags |= PIC_DATA::AF_MaxLoopCounter;
  if (picData.MaxLoops <= 0) picData.MaxLoops = 1;
  if ((picData.MaxLoops > 1) && (picData.NumFrames > 1))
    picData.flags |= PIC_DATA::AF_Loop;
  
  //WriteDebugString("115 Calling LoadPicSurfaces(%s)\n", prefix);
  picData.LoadPicSurfaces(prefix);

  if (picData.key < 0)
  {
    debugSeverity = 5;
    WriteDebugString("Failed to load sprite surface \'%s\' in CombatSpriteData::Init()\n", picData.filename);
  };

  if (picData.key >= 0)
  {
    if (picData.style == PIC_DATA::AS_Directional)
      picData.SetFrame(PathDirToFaceDir(dir));
  }

//#ifdef _DEBUG
//  WriteDebugString("Loaded %s with MinDelta %u\n", picData.filename, MinDeltaTime);
//#endif
  lastUpdateTime = virtualGameTime;
}

//*****************************************************************************
//    NAME: COMBAT_SPRITE_DATA::Init
//
// PURPOSE: Used for spell animations (combat only) that
//          do not move from start location
//
//*****************************************************************************
void COMBAT_SPRITE_DATA::Init(const PIC_DATA &data, CString prefix, int x1, int y1)
{
  Clear();
  if (data.filename.IsEmpty()) return;
  
  ASSERT( ValidCoords(y1,x1) );  
  ASSERT( (x1 >= 0) && (x1 < MAX_TERRAIN_WIDTH) );
  ASSERT( (y1 >= 0) && (y1 < MAX_TERRAIN_HEIGHT) );

  if (x1 < 0)
    x1 = 0;
  else if (x1 >= MAX_TERRAIN_WIDTH)
    x1 = MAX_TERRAIN_WIDTH - 1;
  
  if (y1 < 0)
    y1 = 0;
  else if (y1 >= MAX_TERRAIN_HEIGHT)
    y1 = MAX_TERRAIN_HEIGHT - 1;

  UsePath=FALSE;
  picData = data;
  picData.LoopCounter=0;
  
  stx = x1;
  sty = y1;
  swx = TerrainToWorldCoordX(x1);
  swy = TerrainToWorldCoordY(y1);  
  
  MinDeltaTime = picData.timeDelay;
  if (MinDeltaTime < 30) MinDeltaTime = 30;

  // style not supported for stationary animations
  picData.style &= ~PIC_DATA::AS_Directional;

  // these things loop until external trigger stops it
  picData.flags &= ~PIC_DATA::AF_Loop;
  picData.flags &= ~PIC_DATA::AF_MaxLoopCounter;
  picData.MaxLoops = 0;     

  if (picData.NumFrames > 1)
    picData.flags |= PIC_DATA::AF_LoopForever;

  //WriteDebugString("116 Calling LoadPicSurfaces(%s)\n", prefix);
  picData.LoadPicSurfaces(prefix);
  
  if (picData.key < 0)
  {
    debugSeverity = 5;
    WriteDebugString("Failed to load sprite surface \'%s\' in CombatSpriteData::Init()\n", picData.filename);
  };
    
//#ifdef _DEBUG
//  WriteDebugString("Loaded sprite %s with Key %i, MinDelta %u\n", picData.filename, picData.key, MinDeltaTime);
//#endif

  lastUpdateTime = virtualGameTime;
}

//*****************************************************************************
//    NAME: COMBAT_SPRITE_DATA::Init
//
// PURPOSE: Used for spell animations (combat only) that
//          are attached to a combatant
//
//*****************************************************************************
void COMBAT_SPRITE_DATA::Init(const PIC_DATA &data, CString prefix, int combatantIndex)
{
  Clear();
  if (data.filename.IsEmpty()) return;
  
  UsePath=FALSE;
  picData = data;
  spriteAttachedToCombatant = true;
  picData.LoopCounter=0;
  
  stx = combatantIndex;
  sty = -1;
  swx = combatantIndex;
  swy = -1;  
  
  MinDeltaTime = picData.timeDelay;
  if (MinDeltaTime < 30) MinDeltaTime = 30;

  // style not supported for stationary animations
  picData.style &= ~PIC_DATA::AS_Directional;

  // these things loop until external trigger stops it
  picData.flags &= ~PIC_DATA::AF_Loop;
  picData.flags &= ~PIC_DATA::AF_MaxLoopCounter;
  picData.MaxLoops = 0;     

  if (picData.NumFrames > 1)
    picData.flags |= PIC_DATA::AF_LoopForever;

  //WriteDebugString("116 Calling LoadPicSurfaces(%s)\n", prefix);
  picData.LoadPicSurfaces(prefix);
  
  if (picData.key < 0)
  {
    debugSeverity = 5;
    WriteDebugString("Failed to load sprite surface \'%s\' in CombatSpriteData::Init()\n", picData.filename);
  };
    
//#ifdef _DEBUG
//  WriteDebugString("Loaded sprite %s with Key %i, MinDelta %u\n", picData.filename, picData.key, MinDeltaTime);
//#endif

  lastUpdateTime = virtualGameTime;
}
//*****************************************************************************
//    NAME: Update
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_SPRITE_DATA::Update()
{
  BOOL result = FALSE;
  LONGLONG currTime = virtualGameTime;
  LONGLONG timeDelta = currTime - lastUpdateTime;
  lastUpdateTime=currTime;
  TotalDeltaTime += timeDelta;
  if (TotalDeltaTime < MinDeltaTime) return FALSE;  

  if (picData.style == PIC_DATA::AS_None) // sequenced
  {
    if (!picData.IsLastFrame())
    {
      picData.SetNextFrame(); // wraps to first frame if end reached
      TotalDeltaTime = 0;
      result=TRUE;
    }
    else // reached last frame of animation
    {
      // loop forever, so keep advancing to next frame
      if (picData.flags & PIC_DATA::AF_LoopForever)
      {
        picData.SetNextFrame(); // wraps to first frame if end reached
        TotalDeltaTime = 0;        
        return TRUE;
      }
      
      // if last frame and not looping, then we're done
      if ((picData.flags & PIC_DATA::AF_Loop)==0)    
        return FALSE;
     
      // AF_KeypressBeforeLoop is ignored for
      // combat animations

      // loop count throttled?
      picData.LoopCounter++;        
      if (picData.flags & PIC_DATA::AF_MaxLoopCounter)
      {
        if (picData.LoopCounter < picData.MaxLoops)
        {
          picData.SetNextFrame();
          TotalDeltaTime = 0;
        }
        else
          picData.SetFirstFrame();
      }
      else // no loop maximum
      {
        if (!UsePath) die(0xab4dc); // shouldn't happen for combat sprites
        picData.SetNextFrame();
        TotalDeltaTime = 0;        
      }
      result = TRUE;
    }
  }
  else // directional
  {
    // no need to change frames
  }

  if (!UsePath) return result;

  pixelPath.RemoveHead();

  if (pixelPath.IsEmpty())
    return FALSE;

  CPoint ppt = pixelPath.GetHead();
  // convert pixel coords to terrain
  int tx = WorldToTerrainCoordX(ppt.x);
  int ty = WorldToTerrainCoordY(ppt.y);

  // move combat map if needed to ensure the missile
  // is on screen
  if (!BothOnScreen(tx,ty, dtx, dty))
    ForceMapToFollowMissile(tx, ty, GetDir(tx,ty, dtx,dty));  
  return TRUE;
}

//*****************************************************************************
//    NAME: COMBAT_SPRITE_DATA::IsDone
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_SPRITE_DATA::IsSpriteDone()
{
  if (UsePath)
  {
    // animations that follow a path can loop until
    // the destination is reached (AS_NONE), or
    // are directional
    return (pixelPath.GetCount() <= 0);
  }
  else
  {
    if (((picData.style==PIC_DATA::AS_None))&&(picData.flags & PIC_DATA::AF_Loop))
    {
      // although NPC small pics can loop indefinitely,
      // combat animations cannot. The AF_MaxLoopCounter
      // flag must be used.
      ASSERT(picData.flags & PIC_DATA::AF_MaxLoopCounter);
      return (picData.LoopCounter>=picData.MaxLoops);
    }
    else if (((picData.style==PIC_DATA::AS_None))&&(picData.flags & PIC_DATA::AF_LoopForever))
    {
      return FALSE; // does not auto stop, loop until animation stopped by other code
    }
    else
      return ((picData.IsLastFrame())&&(TotalDeltaTime >= MinDeltaTime));
  }
//Unreachable  return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_SPRITE_DATA::Display
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_SPRITE_DATA::Display()
{
  if (picData.key == -1) return;

  CPoint ppt;
  if (UsePath)
  {
    if (pixelPath.GetCount() <= 0)
      return;
    ppt = pixelPath.GetHead();
  }
  else
  {
    if (spriteAttachedToCombatant)
    {
      int dude;
      COMBATANT *pCombatant;
      dude = swx;
      pCombatant = combatData.GetCombatant(dude);
      if (pCombatant == NULL) return;
      if (pCombatant->IsAlive())
      {
        ppt.x = pCombatant->x;
        ppt.y = pCombatant->y;
      }
      else
      {
        return;
      };
    }
    else
    {
      ppt.x = swx;
      ppt.y = swy;
    };
  }

  if (picData.AllowCentering)
  {
    // adjust sprite to center itself over screen coordinates,
    // but only if the frame size is 3 or more in width/height
    //
    if (picData.FrameWidth >= (3*COMBAT_TILE_WIDTH))
    {
      double fwidth = picData.FrameWidth / COMBAT_TILE_WIDTH;
      double offset = ceil( fwidth / 2.0 ) - 1.0;
      offset *= COMBAT_TILE_WIDTH;
      ppt.x -= offset;
    }

    if (picData.FrameHeight >= (3*COMBAT_TILE_HEIGHT))
    {
      double fheight = picData.FrameHeight / COMBAT_TILE_HEIGHT;
      double offset = ceil( fheight / 2.0 ) - 1.0;
      offset *= COMBAT_TILE_HEIGHT;
      ppt.y -= offset;
    }
  }

  GraphicsMgr.SetClipRect(&combatData.m_combatRect);  
  
  if (picData.UseAlpha)
  {
    GraphicsMgr.BlitAlphaSprite(picData.AlphaValue,
                                WorldToScreenCoordX(ppt.x), 
                                WorldToScreenCoordY(ppt.y),
                                picData.key);
  }
  else
  {
    if (spriteAttachedToCombatant)
    {
      GraphicsMgr.BlitSprite(
        TerrainToScreenCoordX(ppt.x), 
        TerrainToScreenCoordY(ppt.y),
        picData.key);
    }
    else
    {
      GraphicsMgr.BlitSprite(
        WorldToScreenCoordX(ppt.x), 
        WorldToScreenCoordY(ppt.y),
        picData.key);
    };
  }
  
  GraphicsMgr.SetClipRect(&combatData.m_fullRect);
}

//*****************************************************************************
//    NAME: COMBAT_SPRITE_DATA::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_SPRITE_DATA::Clear()
{
  picData.Clear();
  stx=sty=dtx=dty=0;
  swx=swy=dwx=dwy=0;
  lastUpdateTime=0;
  TotalDeltaTime=0;
  MinDeltaTime=0;
  lastUpdateFrame=0;
  dir=PathNorth;
  pixelPath.RemoveAll();
  UsePath=TRUE;
  activeSpellKey=-1;
  spriteAttachedToCombatant = false;
}

//*****************************************************************************
//    NAME: BothOnScreen
//
// PURPOSE: 
//
//*****************************************************************************
BOOL BothOnScreen(int tx1, int ty1, int tx2, int ty2)
{
  return (ValidTerrainViewportCoords(tx1, ty1) && ValidTerrainViewportCoords(tx2, ty2));
}

//*****************************************************************************
//    NAME: ValidTerrainViewportCoords
//
// PURPOSE: 
//
//*****************************************************************************
BOOL ValidTerrainViewportCoords(int tx, int ty)
{
  return (   (tx >= combatData.m_iStartTerrainX)
          && (tx < (combatData.m_iStartTerrainX + TILES_HORZ))
          && (ty >= combatData.m_iStartTerrainY)
          && (ty < (combatData.m_iStartTerrainY + TILES_VERT)));
}

//*****************************************************************************
//    NAME: PathDirToFaceDir
//
// PURPOSE: 
//
//*****************************************************************************
int PathDirToFaceDir(PATH_DIR dir)
{
  switch (dir)
  {
  case PathNorth:
    return FACE_NORTH;
  case PathNE:
    return FACE_NE;
  case PathEast:
    return FACE_EAST;
  case PathSE:
    return FACE_SE;
  case PathSouth:
    return FACE_SOUTH;
  case PathSW:
    return FACE_SW;
  case PathWest:
    return FACE_WEST;
  case PathNW:
    return FACE_NW;
  }
  return FACE_NORTH;
}

//*****************************************************************************
//    NAME: PathDirLeft
//
// PURPOSE: 
//
//*****************************************************************************
PATH_DIR PathDirLeft(PATH_DIR dir)
{
  switch (dir)
  {
  case PathNorth: return PathNW;
  case PathNE: return PathNorth;
  case PathEast: return PathNE;
  case PathSE:  return PathEast;
  case PathSouth: return PathSE;
  case PathSW: return PathSouth;
  case PathWest: return PathSW;
  case PathNW: return PathWest;
  }
  return PathNorth;
}
//*****************************************************************************
//    NAME: PathDirRight
//
// PURPOSE: 
//
//*****************************************************************************
PATH_DIR PathDirRight(PATH_DIR dir)
{
  switch (dir)
  {
  case PathNorth: return PathNE;
  case PathNE: return PathEast;
  case PathEast: return PathSE;
  case PathSE:  return PathSouth;
  case PathSouth: return PathSW;
  case PathSW: return PathWest;
  case PathWest: return PathNW;
  case PathNW: return PathNorth;
  }
  return PathNorth;
}
//*****************************************************************************
//    NAME: FaceDirToPathDir
//
// PURPOSE: 
//
//*****************************************************************************
PATH_DIR FaceDirToPathDir(int dir)
{
  switch (dir)
  {
  case FACE_NORTH:
    return PathNorth;
  case FACE_NE:
    return PathNE;
  case FACE_EAST:
    return PathEast;
  case FACE_SE:
    return PathSE;
  case FACE_SOUTH:
    return PathSouth;
  case FACE_SW:
    return PathSW;
  case FACE_WEST:
    return PathWest;
  case FACE_NW:
    return PathNW;
  }
  return PathNorth;
}

//*****************************************************************************
//    NAME: EventDirToPathDir
//
// PURPOSE: 
//
//*****************************************************************************
PATH_DIR EventDirToPathDir(eventDirType dir)
{
  switch (dir)
  {
  case North:
    return PathNorth;
  case N_E:
    return PathNE;
  case East:
    return PathEast;
  case S_E:
    return PathSE;
  case South:
    return PathSouth;
  case S_W:
    return PathSW;
  case West:
    return PathWest;
  case N_W:
    return PathNW;
  case Any:
  case N_S:
  case N_S_E:
  case N_W_E:
    return PathNorth;
  case E_W:
  case N_S_W:
  case W_S_E:
    return PathWest;
  case InFront:
    return (FaceDirToPathDir(party.facing));
  }
  return PathNorth;
}

//*****************************************************************************
// NAME:    ValidTempCoords
//
// PURPOSE: temp coords are size of world in temp map coords,
//          and are only used during combat map construction 
//
//*****************************************************************************
BOOL ValidTempCoords(int y,int x)
{
  if (    (x>=0) 
       && (x<MAX_TEMP_WIDTH)
       && (y>=0)
       && (y<MAX_TEMP_HEIGHT)
     )
  {
    return TRUE;
  };
  return FALSE;
}

//*****************************************************************************
//    NAME: ValidWorldCoords
//
// PURPOSE: world coords are size of world in pixels
//
//*****************************************************************************
BOOL ValidWorldCoords(int y,int x)
{
   if ((x < 0) || (x >= MAX_WORLD_WIDTH))  return FALSE;
   if ((y < 0) || (y >= MAX_WORLD_HEIGHT)) return FALSE;   
   return TRUE;
}

//*****************************************************************************
// NAME:    ValidCoords
//
// PURPOSE: terrain coords are size of world in map squares
//
// RETURNS: None
//*****************************************************************************
BOOL ValidCoords(int y,int x)
{
   if ((x < 0) || (x >= MAX_TERRAIN_WIDTH))  return FALSE;
   if ((y < 0) || (y >= MAX_TERRAIN_HEIGHT)) return FALSE;   
   return TRUE;
}

//*****************************************************************************
// NAME:    isEmpty
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL isEmpty(int y, int x)
{
   if (!ValidCoords(y,x)) return FALSE;
   return (terrain[y][x].cell == NO_TERRAIN);
}

//*****************************************************************************
//    NAME: IsDungeonTileEnabled
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IsDungeonTileEnabled(int cell)
{
  return (DungeonWallTiles[cell].tile_enabled);
}

//*****************************************************************************
//    NAME: IsWildernessTileEnabled
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IsWildernessTileEnabled(int cell)
{
  return (WildernessWallTiles[cell].tile_enabled);
}

//*****************************************************************************
// NAME:    SetDungeon
//
// PURPOSE: 
//*****************************************************************************
void SetDungeon(int y, int x, int cell)
{
  if (!ValidCoords(y,x)) return;
  if ((cell < 1) || (cell > DUNGEON_TILE_COUNT)) return;
  if (!IsDungeonTileEnabled(cell)) return;
  terrain[y][x].cell = cell;
}

//*****************************************************************************
// NAME:    SetWilderness
//
// PURPOSE: 
//*****************************************************************************
void SetWilderness(int y, int x, int cell)
{
   if (!ValidCoords(y,x)) return;
   if ((cell < 1)||(cell > WILDERNESS_TILE_COUNT)) return;
   if (!IsWildernessTileEnabled(cell)) return;
   terrain[y][x].cell = cell;
}

//*****************************************************************************
//    NAME: HaveVisibility
//
// PURPOSE: 
//
//*****************************************************************************
BOOL HaveVisibility(int y, int x, BOOL *reflects)
{
  if (reflects != NULL)
  {
    *reflects = FALSE;
  };
  if (!ValidCoords(y,x)) return FALSE;
  int cell = terrain[y][x].cell;
  if ((cell < 1)||(cell > CurrentTileCount)) return FALSE;  
  if (CurrentTileData[cell].tile_invisible > 0)
  {
    return TRUE;
  }
  else
  {
    if (reflects != NULL)
    {
      *reflects = TRUE;
    };
    return FALSE;
  };
}

//*****************************************************************************
//    NAME: HaveMovability
//
// PURPOSE: 
//
//*****************************************************************************
BOOL HaveMovability(int y, int x)
{
  if (!ValidCoords(y,x)) return FALSE;
  int cell = terrain[y][x].cell;
  if ((cell < 1)||(cell > CurrentTileCount)) return FALSE;  
  return (CurrentTileData[cell].tile_passable > 0);
}

//
// check cells for blockage or exceed map size
// blockage = combat icon, or wall cell with obstacle > 0
// w,h default to 1
// combatant defaults to NO_DUDE
// max size 4x4 combat icon ==> w=4, h=4
// x,y gives top left corner for size > 1x1
//
//*****************************************************************************
// NAME:    isObstical
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
OBSTICAL_TYPE ObsticalType(int x, int y, int w, int h, 
                         BOOL CheckOccupants, 
                         BOOL ignoreCurrentCombatant, 
                         COMBATANT *pCombatantLinger)
{
  int i,j, dude;
  
  for (i=0;i<h;i++)
  {
    for (j=0;j<w;j++)
    {
      if (!coordsOnMap(x+j,y+i, 1,1)) return OBSTICAL_offMap;
      if (!HaveMovability(y+i,x+j))   return OBSTICAL_wall;
      
      if (CheckOccupants)
      {
        dude = getCombatantInCell(x+j, y+i, 1, 1);
        if ((dude != NO_DUDE) && (!ignoreCurrentCombatant || (dude != combatData.GetCurrCombatant())))
          return OBSTICAL_occupied;
        if (pCombatantLinger != NULL)
        {
          if (activeSpellList.LingerSpellBlocksCombatant(x+j, y+i, pCombatantLinger)) return OBSTICAL_lingeringSpell;
        };
      }
    }
  }
  
  return OBSTICAL_none;
}

//*****************************************************************************
// NAME: getHallWidth
//
// PURPOSE:  
//*****************************************************************************
int getHallWidth(int x, int y, int dir)
{
   const BOOL CheckOccupants = FALSE;
   int offset = 0;
   int total = 0;
   
   switch (dir) 
   {      
   case FACE_NORTH:
   case FACE_SOUTH:
      while (OBSTICAL_none == ObsticalType(x+offset,y,1,1, CheckOccupants, TRUE, NULL))
         offset++;
      
      total = offset;
      offset = 1;
      
      while (OBSTICAL_none == ObsticalType(x-offset,y,1,1,CheckOccupants, TRUE, NULL))
         offset++;
      
      total += (offset-1);
      WriteDebugString("Horz hall width %u in getHallWidth()\n", total);
      break;
      
   case FACE_EAST:
   case FACE_WEST:
      while (OBSTICAL_none == ObsticalType(x,y+offset,1,1,CheckOccupants, TRUE, NULL))
         offset++;
      
      total = offset;
      offset = 1;
      
      while (OBSTICAL_none == ObsticalType(x,y-offset,1,1,CheckOccupants, TRUE, NULL))
         offset++;
      
      total += (offset-1);
      WriteDebugString("Vert hall width %u in getHallWidth()\n", total);
      break;
   }
   
   return total;
}


//*****************************************************************************
// NAME:    coordsOnMap
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL coordsOnMap(int x, int y, int width, int height)
{
  int i,j;
  for (i=0; i<height; i++)
  {
    for (j=0; j<width; j++)
    {
       if (((x+j) < 0) || ((x+j) >= MAX_TERRAIN_WIDTH))  return FALSE;
       if (((y+i) < 0) || ((y+i) >= MAX_TERRAIN_HEIGHT)) return FALSE;
    }
  }
  return TRUE;
}

//*****************************************************************************
// NAME: placeCombatant
//
// PURPOSE:  
//*****************************************************************************
void placeCombatant(int x, int y, int dude, int w, int h)
{
   int i,j;
  
   if ((w <= 0) || (h <= 0))
   {
     if (!debugStrings.AlreadyNoted(CString("BWH01")))
     {
       WriteDebugString("Bogus w,h for icon in placeCombatant()\n");
       WriteDebugString("Combatant's name is %s\n", combatData.GetCombatant(dude)->GetName());
     };
   };

   for (i=0;i<h;i++)
   {
      for (j=0;j<w;j++)
      {
         if (ValidCoords(y+i, x+j))
            terrain[y+i][x+j].tileIndex = dude;
      }
   }
}

//*****************************************************************************
//    NAME: placeDyingCombatant
//
// PURPOSE: 
//
//*****************************************************************************
void placeDyingCombatant(int x, int y, int dude, int w, int h)
{
  int i,j;
  
  if ((w <= 0) || (h <= 0))
  {
     if (!debugStrings.AlreadyNoted(CString("BWH02")))
     {
       WriteDebugString("Bogus w,h for icon in placeDyingCombatant()\n");
       WriteDebugString("Combatant's name is %s\n", combatData.GetCombatant(dude)->GetName());
     };
  }
  
  for (i=0;i<h;i++)
  {
    for (j=0;j<w;j++)
    {
      if (ValidCoords(y+i, x+j))
        terrain[y+i][x+j].tileIndexDead = dude;
    }
  }
}

//*****************************************************************************
// NAME: getCombatantInCell
//
// PURPOSE:  
//*****************************************************************************
int getCombatantInCell(int x, int y, int w, int h, int ignoreDude)
{  
  for (int i=0; i<h; i++)
  {
    for (int j=0; j<w; j++)
    {
       if (ValidCoords(y+i, x+j))
       {
         if (   (terrain[y+i][x+j].tileIndex != NO_DUDE)
             && (terrain[y+i][x+j].tileIndex != ignoreDude))
         {
           if (terrain[y+i][x+j].tileIndex < combatData.NumCombatants()) //m_aCombatants.GetSize())
             return terrain[y+i][x+j].tileIndex;
           else
             terrain[y+i][x+j].tileIndex = NO_DUDE;
         }
       }
    }
  }
  return NO_DUDE;
}

//*****************************************************************************
//    NAME: getDeadCombatantInCell
//
// PURPOSE: 
//
//*****************************************************************************
int getDeadCombatantInCell(int x, int y, int w, int h, int ignoreDude)
{
  for (int i=0; i<h; i++)
  {
    for (int j=0; j<w; j++)
    {
      if (ValidCoords(y+i, x+j))
      {
        if (   (terrain[y+i][x+j].tileIndexDead != NO_DUDE)
            && (terrain[y+i][x+j].tileIndexDead != ignoreDude))
        {
          if (terrain[y+i][x+j].tileIndexDead < combatData.NumCombatants()) //m_aCombatants.GetSize())
            return terrain[y+i][x+j].tileIndexDead;
          else
            terrain[y+i][x+j].tileIndexDead = NO_DUDE;
        }
      }
    }
  }
  return NO_DUDE;
}


//*****************************************************************************
// NAME: distance
//
// PURPOSE:  
//*****************************************************************************
int Distance(int attacker, int sX, int sY, int attackee, int dX, int dY)
{
  int tmpSrcX=sX, tmpSrcY=sY;
  int tmpDstX=dX, tmpDstY=dY;
  int origSX = sX;
  int origDX = dX;

  // for icons bigger than 1x1, we need to adjust the
  // src and dst x,y to be the closest possible for computing
  // distance between icons.
  //
  // the starting x,y for each icon is the upper left corner
  //
   if (sX != dX)
   {
      if (sX < dX)
      {
        sX++;
        while (   (sX != dX) 
               && (ValidCoords(sY, sX)) 
               && (getCombatantInCell(sX, sY,1,1) == attacker))
        {
          sX++;
        }

        tmpSrcX = sX-1;

        dX--;
        while (   (tmpSrcX != dX) 
               && (ValidCoords(dY, dX)) 
               && (getCombatantInCell(dX, dY,1,1) == attackee))
        {
          dX--;
        }

        tmpDstX = dX+1;
      }
      else
      {
        sX--;
        while (   (sX != dX) 
               && (ValidCoords(sY, sX)) 
               && (getCombatantInCell(sX, sY,1,1) == attacker))
        {
          sX--;
        }

        tmpSrcX = sX+1;

        dX++;
        while (   (tmpSrcX != dX) 
               && (ValidCoords(dY, dX)) 
               && (getCombatantInCell(dX, dY,1,1) == attackee))
        {
          dX++;
        }

        tmpDstX = dX-1;
      }
   }

   sX = origSX;
   dX = origDX;

   if (sY != dY)
   {
      if (sY < dY)
      {
        sY++;
        while (   (sY != dY) 
               && (ValidCoords(sY, sX)) 
               && (getCombatantInCell(sX, sY,1,1) == attacker))
        {
          sY++;
        }

        tmpSrcY = sY-1;

        dY--;
        while (   (tmpSrcY != dY) 
               && (ValidCoords(dY, dX)) 
               && (getCombatantInCell(dX, dY,1,1) == attackee))
        {
          dY--;
        }

        tmpDstY = dY+1;
      }
      else
      {
        sY--;
        while (   (sY != dY) 
               && (ValidCoords(sY, sX)) 
               && (getCombatantInCell(sX, sY,1,1) == attacker))
        {
          sY--;
        }

        tmpSrcY = sY+1;

        dY++;
        while (   (tmpSrcY != dY) 
               && (ValidCoords(dY, dX)) 
               && (getCombatantInCell(dX, dY,1,1) == attackee))
        {
          dY++;
        }

        tmpDstY = dY-1;
      }
   }
   
   return Distance(tmpSrcX, tmpSrcY, tmpDstX, tmpDstY);
}

// this one computes distance between absolute sx,sy and
// closest point of attackee icon at dx,dy
//*****************************************************************************
//    NAME: distance
//
// PURPOSE: 
//
//*****************************************************************************
int Distance(int sX, int sY, int attackee, int dX, int dY)
{
  int tmpSrcX=sX, tmpSrcY=sY;
  int tmpDstX=dX, tmpDstY=dY;
  int origSX = sX;
  int origDX = dX;

  // for icons bigger than 1x1, we need to adjust the
  // src and dst x,y to be the closest possible for computing
  // distance between icons.
  //
  // the starting x,y for each icon is the upper left corner
  //
   if (sX != dX)
   {
      if (sX < dX)
      {
        dX--;
        while (   (tmpSrcX != dX) 
               && (ValidCoords(dY, dX)) 
               && (getCombatantInCell(dX, dY,1,1) == attackee))
        {
          dX--;
        }
        tmpDstX = dX+1;
      }
      else
      {
        dX++;
        while (   (tmpSrcX != dX) 
               && (ValidCoords(dY, dX)) 
               && (getCombatantInCell(dX, dY,1,1) == attackee))
        {
          dX++;
        }
        tmpDstX = dX-1;
      }
   }

   sX = origSX;
   dX = origDX;

   if (sY != dY)
   {
      if (sY < dY)
      {
        dY--;
        while (   (tmpSrcY != dY) 
               && (ValidCoords(dY, dX)) 
               && (getCombatantInCell(dX, dY,1,1) == attackee))
        {
          dY--;
        }
        tmpDstY = dY+1;
      }
      else
      {
        dY++;
        while (   (tmpSrcY != dY) 
               && (ValidCoords(dY, dX)) 
               && (getCombatantInCell(dX, dY,1,1) == attackee))
        {
          dY++;
        }
        tmpDstY = dY-1;
      }
   }
   
   return (Distance(tmpSrcX, tmpSrcY, tmpDstX, tmpDstY));
}

//*****************************************************************************
// NAME:    distance
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int Distance(int sx, int sy, int dx, int dy)
{
   int x = sx - dx;
   int y = sy - dy;
     
   double temp = sqrt((double)((x*x)+(y*y)));
   
   int result = (int)floor(temp+0.5);   
   return result;
}


//*****************************************************************************
// NAME: IsWallAt
//
// PURPOSE:  
//*****************************************************************************
BOOL IsWallAt(int x, int y, int direction)  // North, east, south, west
{
  static int rearrange[] = {0,2,1,3};
  int wallSlot, ovWallSlot;
  int blockage, ovBlockage;
  int cacheIndex;
  cacheIndex = 4 * (levelData.area_width * y + x) + direction;
  /*
  switch(direction)
  {
    case 0:  wallSlot = levelData.area[y][x].wall[0];
             blockage = levelData.area[y][x].blockage[0];
             break;
    case 1:  wallSlot = levelData.area[y][x].wall[2];
             blockage = levelData.area[y][x].blockage[2];
             break;
    case 2:  wallSlot = levelData.area[y][x].wall[1];
             blockage = levelData.area[y][x].blockage[1];
             break;
    case 3:  wallSlot = levelData.area[y][x].wall[3];
             blockage = levelData.area[y][x].blockage[3];
             break;
    default: wallSlot = -1; blockage = -1;
  }; //switch
  */
  wallSlot = levelData.area[y][x].wall[rearrange[direction]];
  blockage = levelData.area[y][x].blockage[rearrange[direction]];


  //if (wallSlot >= 0) wallSlot = 1;

  ovWallSlot = globalData.levelInfo.stats[globalData.currLevel].GetMapOverride(WALL_OVERRIDE_INDEX,x,y,direction);
  if (ovWallSlot >= 0) 
  {
    wallSlot = ovWallSlot+1;  // Ensure > 0
  };
  ovBlockage = globalData.levelInfo.stats[globalData.currLevel].GetMapOverride(BLOCKAGE_OVERRIDE,x,y,direction);
  if (ovBlockage >= 0)
  {
    blockage = ovBlockage;
  };
  
  return (wallSlot > 0) && (blockage != OpenBlk);
};


//*****************************************************************************
// NAME: IsNorthWall
//
// PURPOSE:  
//*****************************************************************************
BOOL IsNorthWall(int x, int y)
{
  return IsWallAt(x, y, 0);
}


//*****************************************************************************
// NAME: IsEastWall
//
// PURPOSE:  
//*****************************************************************************
BOOL IsEastWall(int x, int y)
{
  return IsWallAt(x, y, 1);
//  return (   (levelData.area[y][x].eastWall > 0)
//          && (levelData.area[y][x].eastBlockage != OpenBlk));
}

//*****************************************************************************
// NAME: IsSouthWall
//
// PURPOSE:  
//*****************************************************************************
BOOL IsSouthWall(int x, int y)
{
  return IsWallAt(x, y, 2);
//  return (   (levelData.area[y][x].southWall > 0)
//          && (levelData.area[y][x].southBlockage != OpenBlk));
}

//*****************************************************************************
// NAME: IsWestWall
//
// PURPOSE:  
//*****************************************************************************
BOOL IsWestWall(int x, int y)
{
  return IsWallAt(x, y, 3);
//  return (   (levelData.area[y][x].westWall > 0)
//          && (levelData.area[y][x].westBlockage != OpenBlk));
}

WALLTYPE GetDoorAt(int x, int y, int direction)
{
  static int rearrange[] = {0,2,1,4};
  int wallSlot, ovWallSlot;
  int blockage, ovBlockage;
  wallSlot = levelData.area[y][x].wall[rearrange[direction]];
  blockage = levelData.area[y][x].blockage[rearrange[direction]];
  ovWallSlot = globalData.levelInfo.stats[globalData.currLevel].GetMapOverride(WALL_OVERRIDE_INDEX,x,y,direction);
  if (ovWallSlot >= 0) 
  {
    wallSlot = ovWallSlot+1;  // Ensure > 0
  };
  ovBlockage = globalData.levelInfo.stats[globalData.currLevel].GetMapOverride(BLOCKAGE_OVERRIDE,x,y,direction);
  if (ovBlockage >= 0)
  {
    blockage = ovBlockage;
  };
  if (wallSlot <= 0) 
  {
    return NoWall;
  };
  if (wallSlot >= MAX_WALLSETS)
  {
    return NoWall;
  };
  if (WallSets[wallSlot].doorFile == "")
  {
    return NoWall;
  };
  return  (blockage == OpenBlk) ? OpenDoor : ClosedDoor;
}

//*****************************************************************************
// NAME: IsNorthDoor
//
// PURPOSE:  
//*****************************************************************************
WALLTYPE IsNorthDoor(int x, int y)
{
  return GetDoorAt(x, y, 0);
//  if (levelData.area[y][x].wall[0] <= 0) return NoWall;
//  if (WallSets[levelData.area[y][x].wall[0]].doorFile == "") return NoWall;
//  return (levelData.area[y][x].blockage[0] == OpenBlk)?OpenDoor:ClosedDoor;
}


//*****************************************************************************
// NAME: IsEastDoor
//
// PURPOSE:  
//*****************************************************************************
WALLTYPE IsEastDoor(int x, int y)
{
  return GetDoorAt(x, y, 1);
//  if (levelData.area[y][x].wall[2] <= 0) return NoWall;
//  if (WallSets[levelData.area[y][x].wall[2]].doorFile == "") return NoWall;
//  return (levelData.area[y][x].blockage[2] == OpenBlk)?OpenDoor:ClosedDoor;
}


//*****************************************************************************
// NAME: IsSouthDoor
//
// PURPOSE:  
//*****************************************************************************
WALLTYPE IsSouthDoor(int x, int y)
{
  return GetDoorAt(x, y, 2);
//  if (levelData.area[y][x].wall[1] <= 0) return NoWall;
//  if (WallSets[levelData.area[y][x].wall[1]].doorFile == "") return NoWall;
//  return (levelData.area[y][x].blockage[1] == OpenBlk)?OpenDoor:ClosedDoor;
}


//*****************************************************************************
// NAME: IsWestDoor
//
// PURPOSE:  
//*****************************************************************************
WALLTYPE IsWestDoor(int x, int y)
{
  return GetDoorAt(x, y, 3);
//  if (levelData.area[y][x].wall[3] <= 0) return NoWall;
//  if (WallSets[levelData.area[y][x].wall[3]].doorFile == "") return NoWall;
//  return (levelData.area[y][x].blockage[3] == OpenBlk)?OpenDoor:ClosedDoor;
}



//*****************************************************************************
// NAME: ConvertTempMapToCombatTerrain
//
// PURPOSE:  fill terrain[][] with combat tiles using tempArea[][] as source
//*****************************************************************************
void ConvertTempMapToCombatTerrain(int &startX, int &startY)
{
   int countX, countY, sx, sy, tx, ty;  

   // startX,startY is the party starting position in the
   // temp map. Convert a block of temp map centered
   // around the party position.
   //int mapStartY = (startY - (MAX_TERRAIN_HEIGHT/2)) - 10;
   //int mapStartX = (startX - (MAX_TERRAIN_WIDTH/2)) - 10;
   //int mapEndY = mapStartY + MAX_TERRAIN_HEIGHT + 10;
   //int mapEndX = mapStartX + MAX_TERRAIN_WIDTH + 10;


   //if (mapStartX < 0) mapStartX = 0;
   //if (mapStartY < 0) mapStartY = 0;   
   //if (mapEndX >= MAX_TEMP_WIDTH)  mapEndX = MAX_TEMP_WIDTH-1;
   //if (mapEndY >= MAX_TEMP_HEIGHT) mapEndY = MAX_TEMP_HEIGHT-1;

   // make start coords in middle of terrain map
//#ifndef diagonalMap
   //startX = min(startX, startX-mapStartX);
   //startY = min(startY, startY-mapStartY);
//#endif

//  FILE *f;
//  f = fopen("tempArea.txt","w");

  sy = startY - MAX_TERRAIN_HEIGHT/2;   
//  fprintf(f, "%d\n", sy);
  ty = 0;
  for (countY=0; countY<MAX_TERRAIN_HEIGHT; countY++, sy++, ty++)
  {
    sx = startX - MAX_TERRAIN_WIDTH/2;     
    tx = 0;
//    fprintf(f,"%d\n", sx);
    for (countX=0; countX<MAX_TERRAIN_WIDTH; countX++, sx++, tx++)
    {
//      fprintf(f," %c",(tempArea[sy][sx].cell==0)?'.':'*');
      switch (tempArea[sy][sx].cell)
      {
      case NO_TERRAIN:
        // ignore empty tiles until map fully configured
        break;
         
      case VTT_WALL:
        SetDungeon(ty,tx+1,1);
        SetDungeon(ty+1,tx+1,2);
        SetDungeon(ty+2,tx+1,5);
        break;
      case V_WALL:
        if (tempArea[sy-1][sx-1].cell == LRC_WALL)
          break;
        else if (tempArea[sy-1][sx-1].cell == BT_WALL)
          break;
        else if (tempArea[sy-1][sx-1].cell == RT_WALL)
          break;
        else if (tempArea[sy-1][sx-1].cell == VTT_WALL)
          break;          
         
        if (isEmpty(ty-1,tx))
        {
          SetDungeon(ty-1,tx,14);
        }
        else 
        {
          if (tempArea[sy-1][sx-1].cell == INTER_WALL)
          {
            SetDungeon(ty-1,tx,3);
            SetDungeon(ty,tx,4);
            SetDungeon(ty+1,tx,5);
            break;
          }

          if (tempArea[sy-2][sx].cell == HRT_WALL)
            SetDungeon(ty-1,tx,22);
          else if (tempArea[sy-2][sx].cell == ULC_WALL)
            SetDungeon(ty-1,tx,22);
          else
            SetDungeon(ty-1,tx,7);
        }

        SetDungeon(ty,tx,4);
        SetDungeon(ty+1,tx,5);
        break;
      case VBT_WALL:
        if (isEmpty(ty-1,tx))
          SetDungeon(ty-1,tx,14);
        else
          SetDungeon(ty-1,tx,7);
        SetDungeon(ty,tx,8);
        SetDungeon(ty+1,tx,9);
        break;
      case INTER_WALL:
        if (isEmpty(ty-1,tx))
          SetDungeon(ty-1,tx,14);
        else
          SetDungeon(ty-1,tx,7);
        SetDungeon(ty,tx,10);
        SetDungeon(ty+1,tx,15);
        break;         
      case HLT_WALL:
        SetDungeon(ty,tx,16);
        SetDungeon(ty+1,tx,17);
        break;
      case H_WALL:
        if (tempArea[sy][sx-1].cell == LRC_WALL)
          break;
        else if (tempArea[sy][sx-1].cell == BT_WALL)
          break;
        else if (tempArea[sy][sx-1].cell == RT_WALL)
          break;
        SetDungeon(ty,tx,6);
        SetDungeon(ty+1,tx,11);
        break;
      case HRT_WALL:
        SetDungeon(ty,tx,18);
        SetDungeon(ty+1,tx,24);
        break;
      case ULC_WALL:
        if (isEmpty(ty-1,tx))
          SetDungeon(ty-1,tx,14);
        else
          SetDungeon(ty-1,tx,7);
        SetDungeon(ty,tx,20);
        SetDungeon(ty+1,tx,24);
        break;
      case TT_WALL:
        if (isEmpty(ty-1,tx))
          SetDungeon(ty-1,tx,14);
        else
          SetDungeon(ty-1,tx,7);
        SetDungeon(ty,tx,6);
        SetDungeon(ty+1,tx,11);
        break;
      case URC_WALL:
        if (isEmpty(ty-1,tx))
          SetDungeon(ty-1,tx,14);
        else
          SetDungeon(ty-1,tx,7);
        SetDungeon(ty,tx, 12);
        SetDungeon(ty+1,tx,13);
        break;
      case LT_WALL:
        if (isEmpty(ty-1,tx))
          SetDungeon(ty-1,tx,14);
        else
          SetDungeon(ty-1,tx,7);
        SetDungeon(ty,tx,10);
        SetDungeon(ty+1,tx,15);
        break;
      case RT_WALL:
        //if (isEmpty(ty-1,tx))
        SetDungeon(ty-1,tx,14);
        //else
        //SetDungeon(ty-1,tx,7);
        SetDungeon(ty,tx,4);
        SetDungeon(ty+1,tx,5);
        SetDungeon(ty,tx+1,3);
        SetDungeon(ty+1,tx+1,4);
        SetDungeon(ty+2,tx+1,5);
        break;
      case LLC_WALL:
        SetDungeon(ty,tx,10);
        SetDungeon(ty+1,tx,15);
        break;
      case BT_WALL:
        SetDungeon(ty,tx,10);
        SetDungeon(ty+1,tx,15);
        SetDungeon(ty,tx+1,3);
        SetDungeon(ty+1,tx+1,4);
        SetDungeon(ty+2,tx+1,5);
        break;
      case LRC_WALL:
        SetDungeon(ty,tx+1,19);
        SetDungeon(ty+1,tx+1,2);
        SetDungeon(ty+2,tx+1,5);
        break;

        // wilderness types
      case DBL_HIGH_1:
        SetWilderness(ty-1,tx,1);
        SetWilderness(ty,tx,11);
        break;
      case DBL_HIGH_2:
        SetWilderness(ty-1,tx,2);
        SetWilderness(ty,tx,12);
        break;
      case DBL_HIGH_3:
        SetWilderness(ty-1,tx,3);
        SetWilderness(ty,tx,13);
        break;
      case DBL_HIGH_4:
        SetWilderness(ty-1,tx,4);
        SetWilderness(ty,tx,14);
        break;
      case DBL_HIGH_5:
        SetWilderness(ty-1,tx,5);
        SetWilderness(ty,tx,15);
        break;

      case SGL_HIGH_1:
        SetWilderness(ty,tx,6);
        break;
      case SGL_HIGH_2:
        SetWilderness(ty,tx,7);
        break;
      case SGL_HIGH_3:
        SetWilderness(ty,tx,8);
        break;
      case SGL_HIGH_4:
        SetWilderness(ty,tx,9);
        break;
      case SGL_HIGH_5:
        SetWilderness(ty,tx,10);
        break;
      case SGL_HIGH_6:
        SetWilderness(ty,tx,16);
        break;
      case SGL_HIGH_7:
        SetWilderness(ty,tx,17);
        break;
      case SGL_HIGH_8:
        SetWilderness(ty,tx,18);
        break;
      case SGL_HIGH_9:
        SetWilderness(ty,tx,19);
        break;
      case SGL_HIGH_10:
        SetWilderness(ty,tx,20);
        break;
      case SGL_HIGH_11:
        SetWilderness(ty,tx,21);
        break;
      case SGL_HIGH_12:
        SetWilderness(ty,tx,22);
        break;
      case SGL_HIGH_13:
        SetWilderness(ty,tx,23);
        break;
      case SGL_HIGH_14:
        SetWilderness(ty,tx,24);
        break;
      case SGL_HIGH_15:
        SetWilderness(ty,tx,25);
        break;
      case SGL_HIGH_16:
        SetWilderness(ty,tx,26);
        break;
      case SGL_HIGH_17:
        SetWilderness(ty,tx,27);
        break;
      case SGL_HIGH_18:
        SetWilderness(ty,tx,28);
        break;
      case SGL_HIGH_19:
        SetWilderness(ty,tx,29);
        break;
      case SGL_HIGH_20:
        SetWilderness(ty,tx,30);
        break;
      case SGL_HIGH_21:
        SetWilderness(ty,tx,31);
        break;
      case SGL_HIGH_22:
        SetWilderness(ty,tx,32);
        break;
      case SGL_HIGH_23:
        SetWilderness(ty,tx,33);
        break;
      case SGL_HIGH_24:
        SetWilderness(ty,tx,34);
        break;
      case SGL_HIGH_25:
        SetWilderness(ty,tx,35);
        break;
      case SGL_HIGH_26:
        SetWilderness(ty,tx,36);
        break;
      case SGL_HIGH_27:
        SetWilderness(ty,tx,37);
        break;
      case SGL_HIGH_28:
        SetWilderness(ty,tx,38);
        break;
      case SGL_HIGH_29:
        SetWilderness(ty,tx,39);
        break;
      case SGL_HIGH_30:
        SetWilderness(ty,tx,40);
        break;
      default:
        WriteDebugString("Unhandled combat wall type %i in ConvertTempMapToCombatTerrain()\n",
          tempArea[sy][sx].cell);
        break;
      };              
    };
  };  
//  fclose(f);
  startX = MAX_TERRAIN_WIDTH/2;
  startY = MAX_TERRAIN_HEIGHT/2;
}

//#define TA2(x,y) tempArea2[y][x].cell
SHORT& TA2(int x, int y)
{
  return tempArea2[y][x].cell;
};

//*****************************************************************************
// NAME:    getTerrainWallType
//
// PURPOSE: 
//
//*****************************************************************************
int getTerrainWallType(int srcX, int srcY)
{
  int wallCount = 0;
  int wallType = NO_TERRAIN;
  BOOL north=FALSE;
  BOOL south=FALSE;
  BOOL east=FALSE;
  BOOL west=FALSE;


  if (TA2(srcX-1,srcY-1) == Wall)
  {  
    north = TRUE;
    wallCount++;
  };

  if (TA2(srcX+1, srcY+1) == Wall)
  {
   south = TRUE;
   wallCount++;
  };

  if (TA2(srcX+1, srcY) == Wall)
  {
    east = TRUE;
    wallCount++;
  };

  if (TA2(srcX-1, srcY) == Wall) 
  {
    west = TRUE;
    wallCount++;
  };
   
  switch (wallCount) 
  {      
      // wall terminators (end caps)
  case 1: 
     if (north)
        wallType = VBT_WALL;
     else if (south)
        wallType = VTT_WALL;
     else if (east)
        wallType = HLT_WALL;
     else
        wallType = HRT_WALL;
     break;
      
  case 2: 
    if (north && south)
        wallType = V_WALL;
    else if (east && west)
         wallType = H_WALL;
    else if (north && west)
         wallType = ULC_WALL;
    else if (north && east)
         wallType = URC_WALL;
    else if (south && west)
         wallType = LLC_WALL;
    else
         wallType = LRC_WALL;
    break;
      
      // T-intersections
  case 3: 
    if (!north)
         wallType = BT_WALL;
    else if (!south)
         wallType = TT_WALL;
    else if (!east)
         wallType = LT_WALL;
    else
         wallType = RT_WALL;
    break;
      
      // 4-way intersection
  case 4: 
    wallType = INTER_WALL;
    break;
      
  default: 
      WriteDebugString("Invalid wallCount in getTerrainWallType\n");
      break;
  };
   
  return wallType;   
}



//*****************************************************************************
// NAME:    GenerateIndoorCombatMap
//
// PURPOSE: 
// translates the current area map into equivalent combat dungeon terrain tile numbers
//*****************************************************************************
void GenerateIndoorCombatMap(int &partyX, int &partyY, PATH_DIR dir)
{
   int tx,ty,i,j;
   int countY, countX, partyCountX, partyCountY;
   int areaMapX;
   int areaMapY;
     
   int areaMapStartY = (partyY - (MAX_TERRAIN_HEIGHT/2)) - 2;
   int areaMapStartX = (partyX - (MAX_TERRAIN_WIDTH/2)) - 2;


//   int areaMapEndY = partyY + (MAX_TERRAIN_HEIGHT/2) + 2;
   int areaMapEndX = partyX + (MAX_TERRAIN_WIDTH/2) + 2;

   // Now, because we display a diagonal section of the 
   // map, we will move startX and endX half the distance 
   // to the right.

   partyCountX = partyX - areaMapStartX; // Where the party will be located on the temp maps
   partyCountY = partyY - areaMapStartY;


#ifdef diagonalMap
   areaMapStartX += MAX_TERRAIN_WIDTH/2;
   areaMapEndX   += MAX_TERRAIN_WIDTH/2;
#endif


#ifndef diagonalMap
    // The diagonalMap method assumes that the combat map is a torous.
    // We don't limit ourselves at the edges; we wrap around!

   BOOL lBound=FALSE;
   BOOL rBound=FALSE;
   BOOL tBound=FALSE;
   BOOL bBound=FALSE;

   if (areaMapStartY < 0)
   {
      areaMapStartY = 0;
      tBound=TRUE;
   }
   
   if (areaMapStartX < 0)
   {
      areaMapStartX = 0;
      lBound=TRUE;
   }

   if (areaMapEndY >= levelData.area_height)
   {
      areaMapEndY = levelData.area_height-1;
      bBound=TRUE;
   }
   
   if (areaMapEndX >= levelData.area_width)
   {
      areaMapEndX = levelData.area_width-1;
      rBound=TRUE;
   }
#endif  // !diagonalMap
   

#ifndef diagonalMap
   // No need to apply limits if the combat takes place
   // on a torus.  For very small maps we will see the same
   // terrain several times in each direction.


   // Try to ensure max map data is used.
   // These checks get triggered when party is near
   // edges of map.
   if ((areaMapEndX - areaMapStartX) < MAX_TERRAIN_WIDTH-1)
   {
     if (lBound && !rBound)
       areaMapEndX = areaMapStartX+MAX_TERRAIN_WIDTH-1;
     else if (rBound && !lBound)
       areaMapStartX = areaMapEndX-MAX_TERRAIN_WIDTH+1;
   }

   if ((areaMapEndY - areaMapStartY) < MAX_TERRAIN_HEIGHT-1)
   {
     if (tBound && !bBound)
       areaMapEndY = areaMapStartY+MAX_TERRAIN_HEIGHT-1;
     else if (bBound && !tBound)
       areaMapStartY = areaMapEndY-MAX_TERRAIN_HEIGHT+1;
   }

   // just one more sanity check
   if (areaMapStartY < 0)
      areaMapStartY = 0;
   
   if (areaMapStartX < 0)
      areaMapStartX = 0;

   if (areaMapEndY >= levelData.area_height)
      areaMapEndY = levelData.area_height-1;
   
   if (areaMapEndX >= levelData.area_width)
      areaMapEndX = levelData.area_width-1;


#endif // !diagonalMap

   int charCombatStartY = partyY;
   int charCombatStartX = partyX;
   
   // only need to allocate the combat map once
   if (terrain == NULL)
   {
     DWORD tsize = sizeof(TERRAIN_CELL *) * MAX_TERRAIN_HEIGHT;
     terrain = (TERRAIN_CELL **)My_malloc(tsize);

     for (i = 0; i < MAX_TERRAIN_HEIGHT; i++)
     {
       DWORD size = sizeof(TERRAIN_CELL) * MAX_TERRAIN_WIDTH;
       tsize += size;
       terrain[i] = (TERRAIN_CELL *)My_malloc(size);
     }
     if (logDebuggingInfo)
     {
     WriteDebugString("Allocated %u bytes for combat terrain map\n", tsize);
     };
   }

   for (i=0;i<MAX_TERRAIN_HEIGHT;i++)
   {
     for (j=0;j<MAX_TERRAIN_WIDTH;j++)
     {
       memset(&terrain[i][j], 0, sizeof(TERRAIN_CELL));
       terrain[i][j].tileIndex = NO_DUDE;
       terrain[i][j].tileIndexDead = NO_DUDE;
     }
   }

   memset(CurrentTileData, 0, sizeof(CurrentTileData));   
   // copy dungeon tile data to current data
   CurrentTileCount = DUNGEON_TILE_COUNT;
   for (i=0;i<=CurrentTileCount;i++)
     CurrentTileData[i] = DungeonWallTiles[i];


  // The MAX_TEMP_HEIGHT+1 and MAX_TEMP_WIDTH+1 are because
  // we assign values to the right and bottom walls of each cell.


  // ********** EXAMPLE *****  MAX_TERRAIN_HEIGHT = 50
  // ************************  MAP_V_MULTIPLIER = 7
  // ************************  MAX_TEMP_HEIGHT = 55*7 = 385
  // ************************  We allocate 386 rows for tempArea and tempArea2

  tempArea = (TERRAIN_CELL **) My_malloc(sizeof(TERRAIN_CELL *) * (MAX_TEMP_HEIGHT+1));
  tempArea2 = (TERRAIN_CELL **) My_malloc(sizeof(TERRAIN_CELL *) * (MAX_TEMP_HEIGHT+1));

  
  // ********** EXAMPLE *****  MAX_TERRAIN_WIDTH = 50
  // ************************  MAX_H_MULTIPLIER  = 8
  // ************************  MAP_TEMP_WIDTH = 55*8 = 440
  // ************************  Wea allocate and clear 441 cells in each row.

  for (i = 0; i <= MAX_TEMP_HEIGHT; i++)
  {
    tempArea[i]  = (TERRAIN_CELL *) My_malloc(sizeof(TERRAIN_CELL) * (MAX_TEMP_WIDTH+1));
    tempArea2[i] = (TERRAIN_CELL *) My_malloc(sizeof(TERRAIN_CELL) * (MAX_TEMP_WIDTH+1));
    memset(tempArea[i],  0,                   sizeof(TERRAIN_CELL) * (MAX_TEMP_WIDTH+1));
    memset(tempArea2[i], 0,                   sizeof(TERRAIN_CELL) * (MAX_TEMP_WIDTH+1));
  };


  // Translate orthogonal map data into isometric terrain squares

  // We waste two columns of areaTemp2 on the left and two colums on
  // the right and one row at the top and one row at the bottom
  // because it makes it unnecessary to have special cases
  // or tests at the edges.
  areaMapY = (areaMapStartY-1 + 100*levelData.area_height) % levelData.area_height;
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
  for (countY=0; countY<MAX_TERRAIN_HEIGHT+2; countY++)
  {
     // ************** EXAMPLE ********  tx = 8, 7, 6, 5, 4, 3, 2, 1, 8, 7, 6, ....
     tx = 8 - countY % MAP_H_MULTIPLIER;
     areaMapX = (areaMapStartX-countY-1+countY/8 + 100*levelData.area_width) % levelData.area_width;
     for (countX=0; countX<MAX_TERRAIN_WIDTH+2; countX++)
     {
        // *********** EXAMPLE ********  countX = 0, 1, 2, ... 50, 51
        // ****************************  tx  = 8, 58, 108, 158, ... 8+51*8 = 416
        // ****************************  tx  = 7, 57, 107, 157, ... 7+51*8 = 415
        // ****************************  tx  = 1, 51, 101, 151, ... 1+51*8 = 409



        WALLTYPE doorType;
        //************************************************
        if ((countX == partyCountX) && (countY == partyCountY))
        {
          //charCombatStartX = tx;
          charCombatStartX = tx + 2*MAP_H_MULTIPLIER; 
          charCombatStartY = ty + MAP_V_MULTIPLIER*3/2;
        };

        //************************************************
        if (IsNorthWall(areaMapX, areaMapY))
        {
          for (int w=0;w<MAP_H_MULTIPLIER;w++)
          {
            {
              int x, y;
              x = tx+w;
              y = ty;
              // ***** EXAMPLE ****  1 <= tx <= 416;  1 <= x <= 423
              // ******************  0 <= ty <= 357;  0 <= y <= 357
              TA2(x,y) = Wall;
            };
          };
        };
       //************************************************
        if ((doorType = IsNorthDoor(areaMapX, areaMapY)) != NoWall)
        {
         for (int w=0;w<MAP_H_MULTIPLIER;w++)
          {
              int x, y;
              x = tx+w;
              y = ty;
              // ***** EXAMPLE ****  1 <= tx <= 416;  1 <= x <= 423
              // ******************  0 <= ty <= 357;  0 <= y <= 357
              TA2(x,y) = Wall;
          };
          if (doorType == OpenDoor)
          {
              // ***** EXAMPLE ****  1 <= tx <= 416;  6 <= tx+5 <= 421
              // ******************  0 <= ty <= 357;  0 <= y <= 357
              TA2(tx+4,ty) = NoWall;
              TA2(tx+5,ty) = NoWall;
          };
        };
        //************************************************
        if (IsSouthWall(areaMapX, areaMapY))
        {
          for (int w=0;w<MAP_H_MULTIPLIER;w++)
          {
            {
              int x,y;
              x = tx+w+MAP_H_MULTIPLIER-1;
              y = ty+MAP_V_MULTIPLIER;
              // ***** EXAMPLE ****  1 <= tx <= 416;  8 <= x <= 430
              // ******************  0 <= ty <= 357;  7 <= y <= 364
              TA2(x,y) = Wall;
            };
          };
        };
        //************************************************
        if ((doorType = IsSouthDoor(areaMapX, areaMapY)) != NoWall)
        {
	        int w = 0;
          for (w=0;w<MAP_H_MULTIPLIER;w++)
          {
              int x, y;
              x = tx+w+MAP_H_MULTIPLIER-1;
              y = ty+MAP_V_MULTIPLIER;
              // ***** EXAMPLE ****  1 <= tx <= 416;  8 <= x <= 430
              // ******************  0 <= ty <= 357;  7 <= y <= 364
              TA2(x,y) = Wall;
          };
          if (doorType == OpenDoor)
          {
              int x, y;
              x = tx+w+MAP_H_MULTIPLIER-1;
              y = ty+MAP_V_MULTIPLIER;
              // ***** EXAMPLE ****  1 <= tx <= 416;  13 <= x+5 <= 435
              // ******************  0 <= ty <= 357;  7 <= y <= 364
              TA2(x+4,ty) = NoWall;
              TA2(x+5,ty) = NoWall;
          };
        };
        //************************************************
        if (IsWestWall(areaMapX, areaMapY))
        {
           for (int w=0;w<MAP_H_MULTIPLIER;w++)
           {
             {
               int x, y;
               x = tx+w;
               y = ty+w;
               TA2(x,y) = Wall;
             };
           };
        };
        //************************************************
        if ((doorType = IsWestDoor(areaMapX, areaMapY)) != NoWall)
        {
           for (int w=0;w<MAP_H_MULTIPLIER;w++)
           {
               int x, y;
               x = tx+w;
               y = ty+w;
               TA2(x,y) = Wall;
           };
           if (doorType == OpenDoor)
           {
               TA2(tx+3,ty+3) = NoWall;
               TA2(tx+4,ty+4) = NoWall;
           };
        };
        //************************************************
        if (IsEastWall(areaMapX, areaMapY))
        {
          for (int w=0;w<MAP_H_MULTIPLIER;w++)
          {
            {
              int x, y;
              x = tx+MAP_H_MULTIPLIER+w;
              y = ty+w;
              TA2(x,y) = Wall;
            };
          };            
        };
        //************************************************
        if ((doorType = IsEastDoor(areaMapX, areaMapY)) != NoWall)
        {
          for (int w=0;w<MAP_H_MULTIPLIER;w++)
          {
              int x, y;
              x = tx+MAP_H_MULTIPLIER+w;
              y = ty+w;
//              TA2(x,y) = doorType;
              TA2(x,y) = Wall;
          };
          if (doorType == OpenDoor)
          {
              int x, y;
              x = tx+MAP_H_MULTIPLIER;
              y = ty;
              TA2(x+3,y+3) = NoWall;
              TA2(x+4,y+4) = NoWall;
          };            
        };
         
         
        tx += MAP_H_MULTIPLIER;
        areaMapX = (areaMapX+1)%levelData.area_width;
     }

     ty += MAP_V_MULTIPLIER;
     areaMapY = (areaMapY+1)%levelData.area_height;
  }
  //My_free(wallCache); wallCache = NULL;
  //My_free(doorCache); doorCache = NULL;
   // translate expanded area map into terrain tile types   
   //BOOL found = FALSE;
  ty = MAP_V_MULTIPLIER;
   
//  FILE *f;
//  f = fopen("tempArea2","w");
  for (countY=0; countY<MAX_TERRAIN_HEIGHT*MAP_V_MULTIPLIER; countY++, ty++)
  {
    tx = 16;
//    fprintf(f, "(%d,%d) ", tx, ty);
    for(countX=0; countX<MAX_TERRAIN_WIDTH*MAP_H_MULTIPLIER; countX++,tx++)
    {
//      fprintf(f,"%d", TA2(tx, ty));
         //if ((!found) && (ty == charCombatStartY) && (tx == charCombatStartX))
         //{
            //found = TRUE;
            //charCombatStartY = ty;
            //charCombatStartX = tx;
         //};
              
         if ( (TA2(tx,ty) == Wall)
         //     || (TA2(tx-2,ty-2) == Wall) && (TA2(tx-1,ty) == Wall)
            )

         {
           // This sets the cell type based on the number
           // junctures at each square.
           //
           // ie: Does this square contain a 4-way intersect?,
           // a 3-way? is the third leg point N,S,E, or W? etc.
           // 
           // The returned type is something like VTT_WALL, which
           // means Vertical Top Terminator.
           tempArea[ty][tx].cell = getTerrainWallType(tx,ty);
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
   
   int fx=0xffff;
   int fy=0xffff;
   int lx=0;
   int ly=0;
   ty = 0;

   // fill holes in terrain map with floor tiles
   while (ty < MAX_TERRAIN_HEIGHT)
   {
      tx = 0;
      while (tx < MAX_TERRAIN_WIDTH)
      {
         if (isEmpty(ty,tx))
         {
            SetDungeon(ty,tx,DUNGEON_EMPTY_TILE);
         }
         else
         {
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
   if (logDebuggingInfo)
   {
     WriteDebugString("Indoor Combat Map: F %u,%u, L %u,%u\n", fx, fy, lx, ly);
   };

#ifdef diagonalMap
   if (!findEmptyCell(charCombatStartX, charCombatStartY, 1,1, dir, NULL))
#else
   if (!findEmptyCell(charCombatStartX, charCombatStartY, 1,1, dir, NULL))
#endif
   {     
     WriteDebugString("Failed to locate char start loc in combat map\n");

     die(0xab4dd);
     charCombatStartX=0;
     charCombatStartY=0;

     if (!findEmptyCell(charCombatStartX, charCombatStartY, 1,1, dir, NULL))
       WriteDebugString("Failed to relocate char start loc to 0,0\n");
   }
//#ifndef diagonalMap
  partyX = charCombatStartX;
  partyY = charCombatStartY;
//#endif
  for (i=0; i<= MAX_TEMP_HEIGHT; i++)
    My_free(tempArea[i]);
  My_free(tempArea); 
  tempArea=NULL;
  for (i=0; i<= MAX_TEMP_HEIGHT; i++)
    My_free(tempArea2[i]);
  My_free(tempArea2); 
  tempArea2=NULL;
}

//*****************************************************************************
// NAME:    GenerateOutdoorCombatMap
//
// PURPOSE: 
// translates the current area map into equivalent combat dungeon terrain tile numbers
//*****************************************************************************
void GenerateOutdoorCombatMap(int &partyX, int &partyY, PATH_DIR dir)
{
   WORD i,j;   

   int charCombatStartY = MAX_TERRAIN_HEIGHT/2;
   int charCombatStartX = MAX_TERRAIN_WIDTH/2;
   // only need to allocate the combat map once
   if (terrain == NULL)
   {
      terrain = (TERRAIN_CELL **) My_malloc(sizeof(TERRAIN_CELL *) * MAX_TERRAIN_HEIGHT);
      for (i=0; i < MAX_TERRAIN_HEIGHT; i++)
        terrain[i] = (TERRAIN_CELL *) My_malloc(sizeof(TERRAIN_CELL) * MAX_TERRAIN_WIDTH);
   }

   for (i=0;i<MAX_TERRAIN_HEIGHT;i++)
   {
     for (j=0;j<MAX_TERRAIN_WIDTH;j++)
     {
       memset(&terrain[i][j], 0, sizeof(TERRAIN_CELL));
       terrain[i][j].tileIndex = NO_DUDE;
       terrain[i][j].tileIndexDead = NO_DUDE;
     }
   }

   memset(CurrentTileData, 0, sizeof(CurrentTileData));      

  tempArea = (TERRAIN_CELL **) My_malloc(sizeof(TERRAIN_CELL *) * MAX_TEMP_HEIGHT);
  for (i=0; i < MAX_TEMP_HEIGHT; i++)
    tempArea[i] = (TERRAIN_CELL *) My_malloc(sizeof(TERRAIN_CELL) * MAX_TEMP_WIDTH);

  for (i=0;i<MAX_TEMP_HEIGHT;i++)
    for (j=0;j<MAX_TEMP_WIDTH;j++)
      memset(&tempArea[i][j], 0, sizeof(TERRAIN_CELL));

   // copy wilderness tile data to current data
   CurrentTileCount = WILDERNESS_TILE_COUNT;
   for (i=0;i<=CurrentTileCount;i++)
     CurrentTileData[i] = WildernessWallTiles[i];

   //float NumTiles = MAX_TEMP_HEIGHT * MAX_TEMP_WIDTH;
   int NumTiles = MAX_TEMP_HEIGHT * MAX_TEMP_WIDTH;
   DWORD NumTileGroups = NUM_WILDERNESS_TILE_GROUPS;

   // Each group type has a density rating.
   // Loop through all groups
   //   - Determine qty of each group to be placed on map
   //   - Place at random locations on map

   for (i=0;i<NumTileGroups;i++)
   {
     // if non-zero density
     if (WildernessTileDensity[i] > 0)
     {
       //float pcnt = WildernessTileDensity[i] / 100.0;
       int tenthPcnt = WildernessTileDensity[i];
       //DWORD qty = (DWORD)(NumTiles * pcnt);
       DWORD qty = (NumTiles * tenthPcnt)/1000;
//       WriteDebugString("DEBUG - NumTiles=%u; tenthPcnt=%u; qty=%u\n", NumTiles, tenthPcnt, qty);

       // convert index into group type
       int grp = i+WBASE;

       WriteDebugString("Creating Wilderness Group %u: %u over %u tiles\n",
                        grp, qty, (DWORD)NumTiles);
       // place qty of this tile group on map
       // at random locations
       for (j=0;j<qty;j++)
       {
         // determine destination x,y
         int x = RollDice(MAX_TEMP_WIDTH,  1, 0)-1;
         int y = RollDice(MAX_TEMP_HEIGHT, 1, 0)-1;

         switch (grp)
         {
         case DBL_HIGH_1:
         case DBL_HIGH_2:
         case DBL_HIGH_3:
         case DBL_HIGH_4:
         case DBL_HIGH_5:
           // these groups use 2 cells
           if ( ValidTempCoords(y,x) && ValidTempCoords(y-1,x))
           {
              if (   (tempArea[y][x].cell==NO_TERRAIN) 
                  && (tempArea[y-1][x].cell==NO_TERRAIN))
                tempArea[y][x].cell = grp;
           }
           break;
           
         default:
           // these groups use 1 cell
           if (tempArea[y][x].cell==NO_TERRAIN)
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
   
   WORD fx=0xffff;
   WORD fy=0xffff;
   WORD lx=0;
   WORD ly=0;
   WORD y = 0;

   // fill holes in terrain map with floor tiles
   while (y < MAX_TERRAIN_HEIGHT)
   {
      WORD x = 0;
      while (x < MAX_TERRAIN_WIDTH)
      {
         if (isEmpty(y,x))
         {
            SetWilderness(y,x,WILDERNESS_EMPTY_TILE);
         }
         else
         {
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

   WriteDebugString("Outdoor Combat Map: F %u,%u, L %u,%u\n", fx,fy, lx,ly);

   if (!findEmptyCell(charCombatStartX, charCombatStartY, 1,1, dir, NULL))
   {     
     WriteDebugString("Failed to locate char start loc in combat map\n");

     die(0xab4de);
     charCombatStartX=0;
     charCombatStartY=0;

     if (!findEmptyCell(charCombatStartX, charCombatStartY, 1,1, dir, NULL))
       WriteDebugString("Failed to relocate char start loc to 0,0\n");
   }

   partyX = charCombatStartX;
   partyY = charCombatStartY;

  for (i=0; i< MAX_TEMP_HEIGHT; i++)
    My_free(tempArea[i]);
  My_free(tempArea); 
  tempArea=NULL;
}


//*****************************************************************************
// NAME:    displayCombatBorders
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void displayCombatBorders(BOOL displayMiddleBar)
{
   // destX, destY, SrcSurfaceNum, src RECT, UseTransparency?
   //GraphicsMgr.BlitImage(  HorzBarTopX,   HorzBarTopY, HBarCombArt.surface,&HorzBarSrc);
   //GraphicsMgr.BlitImage(  HorzBarBotX,   HorzBarBotY, HBarCombArt.surface,&HorzBarSrc);
   //GraphicsMgr.BlitImage( VertBarLeftX,  VertBarLeftY, VBarCombArt.surface,&VertBarSrc);
   //GraphicsMgr.BlitImage(VertBarRightX, VertBarRightY, VBarCombArt.surface,&VertBarSrc);
   
   //if (displayMiddleBar)
   //   GraphicsMgr.BlitImage(COMBAT_SCREEN_WIDTH,0,VBarCombArt.surface,&VertBarSrc);

   GraphicsMgr.BlitImage(HorzBarCombatTopX, HorzBarCombatTopY, HBarCombArt.surface, &HorzBarCombatSrc);
   GraphicsMgr.BlitImage(HorzBarCombatBotX, HorzBarCombatBotY, HBarCombArt.surface, &HorzBarCombatSrc2);
   GraphicsMgr.BlitImage(VertBarCombatLeftX, VertBarCombatLeftY, VBarCombArt.surface, &VertBarCombatSrc);
   GraphicsMgr.BlitImage(VertBarCombatRightX, VertBarCombatRightY, VBarCombArt.surface, &VertBarCombatSrc2);

   if (displayMiddleBar)
     GraphicsMgr.BlitImage(VertBarCombatMidX, VertBarCombatMidY, VBarCombArt.surface, &VertBarCombatSrcShort);


}


//*****************************************************************************
// NAME:    displayCombatWalls
//
// PURPOSE: blit background terrain objects to backbuffer.
//
// RETURNS: None
//*****************************************************************************
void displayCombatWalls()
{ 
   int terX, terY;    // x,y in combat terrain coords for tile to be drawn
   int scnY;    // x,y in screen coords for tile to be drawn
   RECT srcRect;      // clipping rect for source tile bitmap
   int cell;
   
   GraphicsMgr.SetClipRect(&combatData.m_combatRect);  

   terY = combatData.m_iStartTerrainY-2;
   while (terY < (combatData.m_iStartTerrainY+TILES_VERT+2))
   {
     scnY = TerrainToScreenCoordY(terY); // no need to recompute this for every terX
     terX = combatData.m_iStartTerrainX-2;
     while (terX < (combatData.m_iStartTerrainX+TILES_HORZ +2))
     {
       if (ValidCoords(terY, terX))
       {
          cell = terrain[terY][terX].cell;
          srcRect.left   = CurrentTileData[cell].tile_posx;
          srcRect.top    = CurrentTileData[cell].tile_posy;
          srcRect.right  = CurrentTileData[cell].tile_posx + COMBAT_TILE_WIDTH;
          srcRect.bottom = CurrentTileData[cell].tile_posy + COMBAT_TILE_HEIGHT;

          GraphicsMgr.BlitImage(TerrainToScreenCoordX(terX),scnY,
                               CombatWallArt.surface,
                               &srcRect);
       }
       terX++;
     }
     terY++;
   }

   GraphicsMgr.SetClipRect(&combatData.m_fullRect);      
}

#ifdef SHOW_SPELL_TARGET_OUTLINES

// only works with dungeon tile set. wilderness
// tile index will display something else.
//*****************************************************************************
//    NAME: displayDotOnMap
//
// PURPOSE: 
//
//*****************************************************************************
void displayDotOnMap(int tx, int ty)
{ 
   RECT srcRect;      // clipping rect for source tile bitmap
   int cell=26;       // unused marker tile in dungeon wall tiles
   
   if (!ValidTerrainViewportCoords(tx, ty)) return;

   GraphicsMgr.SetClipRect(&combatData.m_combatRect);  

   // determine screen coords where tile is drawn
   int scnX = TerrainToScreenCoordX(tx);
   int scnY = TerrainToScreenCoordY(ty);

   srcRect.left   = CurrentTileData[cell].tile_posx;
   srcRect.top    = CurrentTileData[cell].tile_posy;
   srcRect.right  = CurrentTileData[cell].tile_posx + COMBAT_TILE_WIDTH;
   srcRect.bottom = CurrentTileData[cell].tile_posy + COMBAT_TILE_HEIGHT;

  GraphicsMgr.BlitImage(scnX,scnY,
                       CombatWallArt.surface,
                       &srcRect,
                       FALSE,FRONTBUFFERNUM);

  GraphicsMgr.SetClipRect(&combatData.m_fullRect);   
}

#endif


//*****************************************************************************
//    NAME: LineReflects2D
//
// PURPOSE: 
//
//*****************************************************************************
void LineReflects2D (DWORD data, int x0, int y0, int x1, int y1, BOOL (*callback)(DWORD,int,int, BOOL *))
{
  int numStep;
  int speed = 16;

  // starting point of line
  int x = x0, y = y0;

  // direction of line
  int dx = x1-x0, dy = y1-y0;

  // increment or decrement depending on direction of line
  int sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
  int sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));

  // decision parameters for voxel selection
  if ( dx < 0 ) dx = -dx;
  if ( dy < 0 ) dy = -dy;
  int ax = 2*dx, ay = 2*dy;
  int decx, decy;

  // determine largest direction component, single-step related variable
  int max = dx, var = 0;
  if ( dy > max ) var = 1;

  int counter = 0;

  // traverse Bresenham line
  switch (var)
  {
  case 0:  // single-step in x-direction
    numStep = x1 - x;
    if (numStep < 0) numStep = -numStep;
    for (decy=ay-dx; numStep>0; x += sx, decy += ay, numStep--)
    {
      if ((counter % speed) == 0)
      {
        BOOL reflects;
        // process pixel
        if (!callback(data, x,y, &reflects))
        {
          if (reflects)
          {
            decy -=ay;  // Undo change
            x -= sx;    // Undo change
            sx = -sx;   // Try other direction
            sy = -sy;
          }
          else
          {
            return;
          };
        };
      }
      counter++;

      // take Bresenham step
      if ( x == x1 ) break;
      if ( decy >= 0 ) { decy -= ax; y += sy; }
    }
    break;
  case 1:  // single-step in y-direction
    numStep = y1 - y;
    if (numStep < 0) numStep = -numStep;
    for (decx=ax-dy; numStep>0; y += sy, decx += ax, numStep--)
    {
      if ((counter % speed) == 0)
      {
        BOOL reflects;
        // process pixel
        if (!callback(data, x,y, &reflects))
        {
          if (reflects)
          {
            decx -=ax;  // Undo change
            y -= sy;    // Undo change
            sx = -sx;
            sy = -sy;   // Try other direction
          }
          else
          {
            return;
          };
        };
      }
      counter++;

      // take Bresenham step
      if ( y == y1 ) break;
      if ( decx >= 0 ) { decx -= ay; x += sx; }
    }
    break;
  }
}





//*****************************************************************************
//    NAME: Line2D
//
// PURPOSE: 
//
//*****************************************************************************
void Line2D (DWORD data, int x0, int y0, int x1, int y1, BOOL (*callback)(DWORD,int,int, BOOL *))
{
  int speed = 16;

  // starting point of line
  int x = x0, y = y0;

  // direction of line
  int dx = x1-x0, dy = y1-y0;

  // increment or decrement depending on direction of line
  int sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
  int sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));

  // decision parameters for voxel selection
  if ( dx < 0 ) dx = -dx;
  if ( dy < 0 ) dy = -dy;
  int ax = 2*dx, ay = 2*dy;
  int decx, decy;

  // determine largest direction component, single-step related variable
  int max = dx, var = 0;
  if ( dy > max ) var = 1;

  int counter = 0;

  // traverse Bresenham line
  switch (var)
  {
  case 0:  // single-step in x-direction
    for (decy=ay-dx; ; x += sx, decy += ay)
    {
      if ((counter % speed) == 0)
      {
        // process pixel
        if (!callback(data, x,y, NULL))
          return;
      }
      counter++;

      // take Bresenham step
      if ( x == x1 ) break;
      if ( decy >= 0 ) { decy -= ax; y += sy; }
    }
    break;
  case 1:  // single-step in y-direction
    for (decx=ax-dy; ; y += sy, decx += ax)
    {
      if ((counter % speed) == 0)
      {
        // process pixel
        if (!callback(data, x,y, NULL))
          return;
      }
      counter++;

      // take Bresenham step
      if ( y == y1 ) break;
      if ( decx >= 0 ) { decx -= ay; x += sx; }
    }
    break;
  }
}

//*****************************************************************************
//    NAME: Line2DSpell
//
// PURPOSE: 
//
//*****************************************************************************
void Line2DSpell (DWORD data, int x0, int y0, int x1, int y1, BOOL (*callback)(DWORD,int,int))
{
  int speed = 1;

  // starting point of line
  int x = x0, y = y0;

  // direction of line
  int dx = x1-x0, dy = y1-y0;

  // increment or decrement depending on direction of line
  int sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
  int sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));

  // decision parameters for voxel selection
  if ( dx < 0 ) dx = -dx;
  if ( dy < 0 ) dy = -dy;
  int ax = 2*dx, ay = 2*dy;
  int decx, decy;

  // determine largest direction component, single-step related variable
  int max = dx, var = 0;
  if ( dy > max ) var = 1;

  int counter = 0;

  // traverse Bresenham line
  switch (var)
  {
  case 0:  // single-step in x-direction
    for (decy=ay-dx; ; x += sx, decy += ay)
    {
      if ((counter % speed) == 0)
      {
        // process pixel
        if (!callback(data, x,y))
          return;
      }
      counter++;

      // take Bresenham step
      if ( x == x1 ) break;
      if ( decy >= 0 ) { decy -= ax; y += sy; }
    }
    break;
  case 1:  // single-step in y-direction
    for (decx=ax-dy; ; y += sy, decx += ax)
    {
      if ((counter % speed) == 0)
      {
        // process pixel
        if (!callback(data, x,y))
          return;
      }
      counter++;

      // take Bresenham step
      if ( y == y1 ) break;
      if ( decx >= 0 ) { decx -= ay; x += sx; }
    }
    break;
  }
}

int TestLineOfSight(int x, int y)
{
  int flags = 0;  // Set to 1 if wall encountered.
  if (ValidCoords(y,x))
  {
    int cell = terrain[y][x].cell;
    if ((cell > 0) && (cell < CurrentTileCount))
    {
      if (CurrentTileData[cell].tile_invisible == 0) flags |= 1;
    };
  };
  return flags;
}


int TestLineOfSight(int x0, int y0, 
                    int dx1, int dy1, 
                    int dx2, int dy2, 
                    int c, int dc)
{
  // Return non-zero if wall encountered.
  int flags = 0;
  int i;
  int cMax = 2 * c;
  int numStep = c;
  c += dc;
  dc *= 2;
  for (i=0; (i<numStep) && (flags==0); i++)
  {
    // Examine the left and right cells at x = x0
    //
    // Examine left side of line.

    if ((i|c) != 0)
    {
      flags |= TestLineOfSight(x0, y0);
    };

    x0+=dx1; y0+=dy1;

    //
    // Examine right side of line. 
    //
    if (i != numStep-1)
    {
      flags |= TestLineOfSight(x0, y0);
    };

    c += dc;
    if (c >= cMax) {x0+=dx2; y0+=dy2; c-=cMax;};
  };
  return flags;
}


BOOL IsLineOfSight(int x0, int y0, int x1, int y1)
{
  int flags=0, octant;
  int xDistance, yDistance;
  xDistance = x1 - x0;
  yDistance = y1 - y0;
  // Divide the circle into 8 octants.
  octant = (xDistance > 0) ? 0 : 4;
  if (yDistance > 0) octant += 2;
  if (abs(yDistance) > abs(xDistance)) octant += 1;
  switch (octant)
  {                     //  bigd 
                        //     littled                   larger
  case 0:
    flags = TestLineOfSight(x0, y0,  1,  0,  0, -1,  xDistance, -yDistance);
    break;
  case 1:
    flags = TestLineOfSight(x1, y1,  0,  1, -1,  0, -yDistance,  xDistance);
    break;
  case 2:
    flags = TestLineOfSight(x0, y0,  1,  0,  0,  1,  xDistance,  yDistance);
    break;
  case 3:
    flags = TestLineOfSight(x0, y0,  0,  1,  1,  0,  yDistance,  xDistance);
    break;
  case 4:
    flags = TestLineOfSight(x1, y1,  1,  0,  0,  1, -xDistance, -yDistance);
    break;
  case 5:
    flags = TestLineOfSight(x1, y1,  0,  1,  1,  0, -yDistance, -xDistance);
    break;
  case 6:
    flags = TestLineOfSight(x1, y1,  1,  0,  0, -1, -xDistance,  yDistance);
    break;
  case 7:
    flags = TestLineOfSight(x0, y0,  0,  1, -1,  0,  yDistance, -xDistance);
    break;
  };
  return flags == 0;
}

//*****************************************************************************
//    NAME: 
//
// PURPOSE: 
// This ignores combatants on the map. It only checks for
// terrain tiles that cannot be seen through.
//
//*****************************************************************************
BOOL HaveLineOfSight(int x0, int y0, int x1, int y1, BOOL *reflects)
{
    // starting point of line
    int x = x0, y = y0;

    // direction of line
    int dx = x1-x0, dy = y1-y0;

    // increment or decrement depending on direction of line
    int sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
    int sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));

    // decision parameters for voxel selection
    if ( dx < 0 ) dx = -dx;
    if ( dy < 0 ) dy = -dy;
    int ax = 2*dx, ay = 2*dy;
    int decx, decy;

    // determine largest direction component, single-step related variable
    int max = dx, var = 0;
    if ( dy > max ) { var = 1; }

    // traverse Bresenham line
    switch ( var )
    {
    case 0:  // single-step in x-direction
        for (decy=ay-dx; ; x += sx, decy += ay)
        {
           // process pixel
           if (!(x == x0 && y == y0) && !(x == x1 && y == y1))
           {
             if (!HaveVisibility(y,x, reflects))
               return FALSE;
           }

           // take Bresenham step
           if ( x == x1 ) break;
           if ( decy >= 0 ) { decy -= ax; y += sy; }
        }
        break;
    case 1:  // single-step in y-direction
        for (decx=ax-dy; ; y += sy, decx += ax)
        {
           // process pixel
           if (!(x == x0 && y == y0) && !(x == x1 && y == y1))
           {
             if (!HaveVisibility(y,x, reflects))
               return FALSE;
           }

           // take Bresenham step
           if ( y == y1 ) break;
           if ( decx >= 0 ) { decx -= ay; x += sx; }
        }
        break;
    }

    return TRUE;
}

//*****************************************************************************
//    NAME: EnsureVisible
//
// PURPOSE: 
//
//*****************************************************************************
void EnsureVisible(int tx, int ty, BOOL forceCenter)
{
  if (!forceCenter)
  {
    // the '=' is used so that if icons are on the 
    // edge of the visible map the visible portion
    // will still be shifted by one from the icon location,
    // giving a 1 square gap between icon and map edge.
    if (tx <= combatData.m_iStartTerrainX)
      combatData.m_iStartTerrainX = tx-1;
    else if (tx >= (combatData.m_iStartTerrainX + TILES_HORZ - 1))
      combatData.m_iStartTerrainX = (tx - TILES_HORZ) + 2;

    if (ty <= combatData.m_iStartTerrainY)
      combatData.m_iStartTerrainY = ty-1;
    else if (ty >= (combatData.m_iStartTerrainY + TILES_VERT - 1))
      combatData.m_iStartTerrainY = (ty -  TILES_VERT) + 2;
  }
  else
  {    
    combatData.m_iStartTerrainX = tx - (TILES_HORZ/2);
    combatData.m_iStartTerrainY = ty - (TILES_VERT/2);
  }

  if (combatData.m_iStartTerrainX < 0)
    combatData.m_iStartTerrainX = 0;
  else if ((combatData.m_iStartTerrainX + TILES_HORZ) > MAX_TERRAIN_WIDTH)
    combatData.m_iStartTerrainX = MAX_TERRAIN_WIDTH - TILES_HORZ + 1;

  if (combatData.m_iStartTerrainY < 0)
    combatData.m_iStartTerrainY = 0;
  else if ((combatData.m_iStartTerrainY + TILES_VERT) > MAX_TERRAIN_HEIGHT)
    combatData.m_iStartTerrainY = MAX_TERRAIN_HEIGHT - TILES_VERT + 1;
}

//*****************************************************************************
// NAME: EnsureVisibleCursor (forceCenter)
//
// PURPOSE:  if force=TRUE, center cursor in screen, else ensure visible
//*****************************************************************************
void EnsureVisibleCursor(BOOL forceCenter)
{    
  // offscreen cursor? then make visible and center it
  if (!ValidTerrainViewportCoords(combatData.m_iCursorX, combatData.m_iCursorY))
    forceCenter=TRUE;

  EnsureVisible(combatData.m_iCursorX, combatData.m_iCursorY, forceCenter);
}

//*****************************************************************************
//    NAME: EnsureVisibleTarget
//
// PURPOSE: 
//
//*****************************************************************************
void EnsureVisibleTarget(int targ, BOOL forceCenter)
{
  EnsureVisible(combatData.GetCombatant(targ)->GetCenterX(),
                combatData.GetCombatant(targ)->GetCenterY(),
                forceCenter);
  combatData.GetCombatant(targ)->OnEnsureVisible();
}

//*****************************************************************************
//    NAME: moveMap
//
// PURPOSE: 
//
//*****************************************************************************
void moveMap(PATH_DIR dir, int MoveSize)
{
  switch (dir)
  {
  case PathNorth:
    combatData.m_iStartTerrainY = max(combatData.m_iStartTerrainY-MoveSize, 0);  
    break;
  case PathEast:
    if ((combatData.m_iStartTerrainX + TILES_HORZ) < MAX_TERRAIN_WIDTH)
      combatData.m_iStartTerrainX += MoveSize;
    break;
  case PathSouth:
    if ((combatData.m_iStartTerrainY + TILES_VERT) < MAX_TERRAIN_HEIGHT)
      combatData.m_iStartTerrainY += MoveSize;
    break;
  case PathWest:
    combatData.m_iStartTerrainX = max(combatData.m_iStartTerrainX-MoveSize, 0);  
    break;
  case PathNW:
    moveMap(PathNorth, MoveSize);
    moveMap(PathWest, MoveSize);
    break;
  case PathNE:
    moveMap(PathNorth, MoveSize);
    moveMap(PathEast, MoveSize);
    break;
  case PathSW:
    moveMap(PathSouth, MoveSize);
    moveMap(PathWest, MoveSize);
    break;
  case PathSE:
    moveMap(PathSouth, MoveSize);
    moveMap(PathEast, MoveSize);
    break;
  }

  /*
  combatData.startTerrainX = max(0, combatData.startTerrainX);
  combatData.startTerrainX = min(combatData.startTerrainX, MAX_TERRAIN_WIDTH - TILES_HORZ);
  combatData.startTerrainY = max(0, combatData.startTerrainY);
  combatData.startTerrainY = min(combatData.startTerrainY, MAX_TERRAIN_HEIGHT - TILES_VERT);
  */
  if (combatData.m_iStartTerrainX < 0)
    combatData.m_iStartTerrainX = 0;
  else if ((combatData.m_iStartTerrainX + TILES_HORZ) > MAX_TERRAIN_WIDTH)
    combatData.m_iStartTerrainX = MAX_TERRAIN_WIDTH - TILES_HORZ + 1;

  if (combatData.m_iStartTerrainY < 0)
    combatData.m_iStartTerrainY = 0;
  else if ((combatData.m_iStartTerrainY + TILES_VERT) > MAX_TERRAIN_HEIGHT)
    combatData.m_iStartTerrainY = MAX_TERRAIN_HEIGHT - TILES_VERT + 1;
}

//*****************************************************************************
//    NAME: ForceMapToMoveAlong
//
// PURPOSE: 
//
//*****************************************************************************
BOOL ForceMapToMoveAlong(int x, int y,  PATH_DIR dir, int MoveSize)
{
  BOOL force = FALSE;
  switch (dir)
  {
  case PathNorth:
    force = (y <= combatData.m_iStartTerrainY+MoveSize);
    break;
  case PathEast:
    force = (x >= combatData.m_iStartTerrainX+TILES_HORZ-(MoveSize+1));
    break;
  case PathSouth:
    force = (y >= combatData.m_iStartTerrainY+TILES_VERT-(MoveSize+1));
    break;
  case PathWest:
    force = (x <= combatData.m_iStartTerrainX+MoveSize);
    break;
  case PathNW:
    force = ((y <= combatData.m_iStartTerrainY+MoveSize) || (x <= combatData.m_iStartTerrainX+MoveSize));
    break;
  case PathNE:
    force = ((y <= combatData.m_iStartTerrainY+MoveSize) || (x >= combatData.m_iStartTerrainX+TILES_HORZ-(MoveSize+1)));
    break;
  case PathSW:
    force = ((y >= combatData.m_iStartTerrainY+TILES_VERT-(MoveSize+1)) || (x <= combatData.m_iStartTerrainX+MoveSize));
    break;
  case PathSE:
    force = ((y >= combatData.m_iStartTerrainY+TILES_VERT-(MoveSize+1)) || (x >= combatData.m_iStartTerrainX+TILES_HORZ-(MoveSize+1)));
    break;
  }

  if (force)
    moveMap(dir, MoveSize);

  return force;
}

//*****************************************************************************
//    NAME: AdjustXMap
//
// PURPOSE: 
//
//*****************************************************************************
void AdjustXMap(int x, PATH_DIR dir)
{
  // if moving towards north
  if (IsEastDir(dir))
    combatData.m_iStartTerrainX = x;
  else // moving south
    combatData.m_iStartTerrainX = x-TILES_HORZ+1;

  if (combatData.m_iStartTerrainX < 0)
    combatData.m_iStartTerrainX = 0;
  else if ((combatData.m_iStartTerrainX + TILES_HORZ) > MAX_TERRAIN_WIDTH)
    combatData.m_iStartTerrainX = MAX_TERRAIN_WIDTH - TILES_HORZ + 1;
}

//*****************************************************************************
//    NAME: AdjustYMap
//
// PURPOSE: 
//
//*****************************************************************************
void AdjustYMap(int y, PATH_DIR dir)
{
  // if moving towards north
  if (IsNorthDir(dir))
    combatData.m_iStartTerrainY = y-TILES_VERT+1;
  else // moving south
    combatData.m_iStartTerrainY = y;

  if (combatData.m_iStartTerrainY < 0)
    combatData.m_iStartTerrainY = 0;
  else if ((combatData.m_iStartTerrainY + TILES_VERT) > MAX_TERRAIN_HEIGHT)
    combatData.m_iStartTerrainY = MAX_TERRAIN_HEIGHT - TILES_VERT + 1;
}

//*****************************************************************************
//    NAME: ForceMapToFollowMissile
//
// PURPOSE: 
//
//*****************************************************************************
BOOL ForceMapToFollowMissile(int x, int y, PATH_DIR dir)
{
  BOOL force = FALSE;

  int left = combatData.m_iStartTerrainX;
  int right = (combatData.m_iStartTerrainX+TILES_HORZ)-1;
  int top = combatData.m_iStartTerrainY;
  int bottom = (combatData.m_iStartTerrainY+TILES_VERT)-1;

  if ((y <= top)||(y >= bottom))
  {
    AdjustYMap(y, dir);

    // if moving into a corner
    if ((IsEastDir(dir)) && (x >= (right-2)))
      AdjustXMap(x, dir);
    else if ((IsWestDir(dir)) && (x <= (left+2)))
      AdjustXMap(x, dir);
  }

  if ((x <= left)||(x >= right))
  {
    AdjustXMap(x, dir);

    // if moving into a corner
    if ((IsSouthDir(dir)) && (y >= (bottom-2)))
      AdjustYMap(y, dir);
    else if ((IsNorthDir(dir)) && (y <= (top+2)))
      AdjustYMap(y, dir);
  }

  return force;
}


CMap<DWORD,DWORD, DWORD,DWORD> locMap;

//*****************************************************************************
//    NAME: ClearEmptyCellSearch
//
// PURPOSE: 
//
//*****************************************************************************
void ClearEmptyCellSearch()
{
  locMap.RemoveAll();
}

#ifdef newMonsterArrangement
//*****************************************************************************
// NAME: findEmptyCell
//
// PURPOSE:  
// clockwise search of surrounding squares looking for empty cell(s)
//*****************************************************************************
BOOL findEmptyCell(int &x, int &y, int w, int h, PATH_DIR dir, COMBATANT *pCombatantLinger)
{   
   if (OBSTICAL_none == ObsticalType(x,y, w,h, TRUE, TRUE, pCombatantLinger))
      return TRUE;  

   int radius = 1;      
   int maxRadius = max(MAX_TERRAIN_WIDTH-1, MAX_TERRAIN_HEIGHT-1);

   int startX, delStartX;
   int startY;
   int endX;
   int endY;
   int i;
   int j;
   
   DWORD hashkey;
   DWORD tmp;

   BOOL found=FALSE;
         
   while ((!found) && (radius < maxRadius))
   {
     // startX,startY always runs along line from 0,0 to -x,y
     startX = max(0,x-radius);
     startY = max(0,y-radius);

     // endX,endY always runs along line from 0,0 to x,-y
     endX   = min(MAX_TERRAIN_WIDTH-1,  x+radius);
     endY   = min(MAX_TERRAIN_HEIGHT-1, y+radius);

     // the square formed by sx,sy, ex,ey will be searched
     // for an empty square.

     // the square will increase in size each time no empty     
     // square is found, up to maxRadius.

     // clamp endX, endY to proper boundaries,
     // where dir is relative to starting x,y
     switch (dir)
     {
     case PathNorth:
       delStartX = 0;
       if (startY < y) startY = y;
       break;
     case PathNE:
       die(0xab4df);
       delStartX = 0;
       if (endX > x)   endX = x;
       if (startY < y) startY = y;       
       break;
     case PathEast:
       delStartX = 1;
       if (endX > x)   endX = x;
       startX -= radius;
       break;
     case PathSE:
       die(0xab4e0);
       delStartX = 0;
       if (endY > y)   endY = y;
       if (endX > x)   endX = x;       
       break;
     case PathSouth:
       delStartX = 0;
       if (endY > y)   endY = y;
       break;
     case PathSW:
       die(0xab4e1);
       delStartX = 0;
       if (endY > y)   endY = y;
       if (startX < x) startX = x;       
       break;
     case PathWest:
       delStartX = 1;
       if (startX < x) startX = x;       
       startX -= radius;
       break;
     case PathNW:
       die(0xab4e2);
       delStartX = 0;
       if (startY < y) startY = y;       
       if (startX < x) startX = x;       
       break;
     default:
       delStartX = 0;
       // no adjustments
       break;
     }

     i=startY;
     j=startX;

     //WriteDebugString("starting search for empty cell at %i,%i radius %i\n", i,j, radius);

     while ((!found) && (i <= endY))
     {
        j=startX;
        startX += delStartX;
        while ((!found) && (j <= endX))
        {
          // make sure we haven't tried this coord already
          hashkey = ((i<<16) | j);
          if (!locMap.Lookup(hashkey, tmp))
          {
             if (OBSTICAL_none == ObsticalType(j,i, w,h, TRUE, TRUE, pCombatantLinger))
             {
                if (dir != PathBAD)
                {


                  // 20160612 PRS
                  //int hPath = pathMgr.GetPath(j, i, 
                  //                           combatData.m_iPartyStartX, 
                  //                           combatData.m_iPartyStartY, 
                  //                           1, 1,
                  //                           FALSE,
                  //                           pCombatantLinger);
                  int hPath = pathMgr.GetPath(j, i, 
                                             combatData.m_iPartyStartX, 
                                             combatData.m_iPartyStartY, 
                                             combatData.m_iPartyStartX, 
                                             combatData.m_iPartyStartY, 
                                             FALSE,
                                             pCombatantLinger,
                                             FALSE);


                  if (hPath >= 0)
                  {
                    found = TRUE;
                    pathMgr.FreePath(hPath);
                  }
                }
                else
                  found = TRUE;

                if (found)
                {
                  x = j;
                  y = i;
                }
                else // no path, don't try this coord again
                {
                  hashkey = ((i<<16) | j);
                  locMap.SetAt(hashkey, hashkey);
                  //WriteDebugString("%i,%i failed (path) - adding to cmap\n", i,j);
                }
             }
             else // obstical, don't try this coord again
             {
                hashkey = ((i<<16) | j);
                locMap.SetAt(hashkey, hashkey);
                //WriteDebugString("%i,%i failed (obst) - adding to cmap\n", i,j);
             }
          }
          //else
          //  WriteDebugString("%i,%i skipped, already in cmap\n", i,j);

          j++;
        }

        i++;
     }

     radius++;
   }

   return found;
}
#else
//*****************************************************************************
// NAME: findEmptyCell
//
// PURPOSE:  
// clockwise search of surrounding squares looking for empty cell(s)
//*****************************************************************************
BOOL findEmptyCell(int &x, int &y, int w, int h, PATH_DIR dir, COMBATANT *pCombatantLinger)
{   
   if (OBSTICAL_none == ObsticalType(x,y, w,h, TRUE, TRUE, pCombatantLinger))
      return TRUE;  

   int radius = 1;      
   int maxRadius = max(MAX_TERRAIN_WIDTH-1, MAX_TERRAIN_HEIGHT-1);

   int startX;
   int startY;
   int endX;
   int endY;
   int i;
   int j;
   
   DWORD hashkey;
   DWORD tmp;

   BOOL found=FALSE;
         
   while ((!found) && (radius < maxRadius))
   {
     // startX,startY always runs along line from 0,0 to -x,y
     startX = max(0,x-radius);
     startY = max(0,y-radius);

     // endX,endY always runs along line from 0,0 to x,-y
     endX   = min(MAX_TERRAIN_WIDTH-1,  x+radius);
     endY   = min(MAX_TERRAIN_HEIGHT-1, y+radius);

     // the square formed by sx,sy, ex,ey will be searched
     // for an empty square.

     // the square will increase in size each time no empty     
     // square is found, up to maxRadius.

     // clamp endX, endY to proper boundaries,
     // where dir is relative to starting x,y
     switch (dir)
     {
     case PathNorth:
       if (endY > y)   endY = y;
       break;
     case PathNE:
       if (startX < x) startX = x;
       if (endY > y)   endY = y;       
       break;
     case PathEast:
       if (startX < x) startX = x;
       break;
     case PathSE:
       if (startY < y) startY = y;
       if (startX < x) startX = x;       
       break;
     case PathSouth:
       if (startY < y) startY = y;
       break;
     case PathSW:
       if (startY < y) startY = y;
       if (endX > x)   endX = x;       
       break;
     case PathWest:
       if (endX > x)   endX = x;       
       break;
     case PathNW:
       if (endY > y)   endY = y;       
       if (endX > x)   endX = x;       
       break;
     default:
       // no adjustments
       break;
     }

     i=startY;
     j=startX;

     //WriteDebugString("starting search for empty cell at %i,%i radius %i\n", i,j, radius);

     while ((!found) && (i <= endY))
     {
        j=startX;
        while ((!found) && (j <= endX))
        {
          // make sure we haven't tried this coord already
          hashkey = ((i<<16) | j);
          if (!locMap.Lookup(hashkey, tmp))
          {
             if (OBSTICAL_none == ObsticalType(j,i, w,h, TRUE, TRUE, pCombatantLinger))
             {
                if (dir != PathBAD)
                {
                  int hPath = pathMgr.GetPath(j, i, 
                                             combatData.m_iPartyStartX, 
                                             combatData.m_iPartyStartY, 
                                             FALSE,
                                             pCombatantLinger);
                  if (hPath >= 0)
                  {
                    found = TRUE;
                    pathMgr.FreePath(hPath);
                  }
                }
                else
                  found = TRUE;

                if (found)
                {
                  x = j;
                  y = i;
                }
                else // no path, don't try this coord again
                {
                  hashkey = ((i<<16) | j);
                  locMap.SetAt(hashkey, hashkey);
                  //WriteDebugString("%i,%i failed (path) - adding to cmap\n", i,j);
                }
             }
             else // obstical, don't try this coord again
             {
                hashkey = ((i<<16) | j);
                locMap.SetAt(hashkey, hashkey);
                //WriteDebugString("%i,%i failed (obst) - adding to cmap\n", i,j);
             }
          }
          //else
          //  WriteDebugString("%i,%i skipped, already in cmap\n", i,j);

          j++;
        }

        i++;
     }

     radius++;
   }

   return found;
}
#endif


//*****************************************************************************
//    NAME: IntersectPointWithCombatMap
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IntersectPointWithCombatMap(long mx, long my, int &tx, int &ty)
{
  if (!IsCombatActive()) return FALSE;

  RECT terrainRect = { CombatScreenX, CombatScreenY, 
                       TILES_HORZ*COMBAT_TILE_WIDTH, TILES_VERT*COMBAT_TILE_HEIGHT };

  POINT pt = { mx, my };
  // see if mouse coords are inside combat terrain rect
  if (!PtInRect(&terrainRect, pt))
    return FALSE;

  // zero mouse coords with respect to terrain 0,0
  mx -= terrainRect.left;
  my -= terrainRect.top;

  tx = (mx / COMBAT_TILE_WIDTH) + combatData.m_iStartTerrainX;
  ty = (my / COMBAT_TILE_HEIGHT) + combatData.m_iStartTerrainY;
  WriteDebugString("Mouse hit combat terrain %u,%u\n", tx, ty);
  return TRUE;
}

//  Q2  |  Q1  
//  ----+----
//  Q3  |  Q4
//
// x1,y1 is location of center of reference point (such as a combatant)
// x2,y2 is location of center of point for which quadrant is being determined
//
//*****************************************************************************
//    NAME: GetQuadrant
//
// PURPOSE: 
//
//*****************************************************************************
int GetQuadrant(int x1, int y1, int x2, int y2)
{
  int x3 = x1-x2;
  int y3 = y1-y2;
  if ((x3 > 0) && (y3 > 0)) return 1;
  if ((x3 < 0) && (y3 > 0)) return 2;
  if ((x3 > 0) && (y3 < 0)) return 3;
  if ((x3 < 0) && (y3 < 0)) return 4;
  return 0;
}

//*****************************************************************************
//    NAME: GetQuadrantDegreeOffset
//
// PURPOSE: 
//
//*****************************************************************************
int GetQuadrantDegreeOffset(int quadrant)
{
  switch (quadrant)
  {
  case 1: return 0;
  case 2: return 90;
  case 3: return 180;
  case 4: return 270;
  default: return 0;
  }
}

//*****************************************************************************
//    NAME: DegreesToRadians
//
// PURPOSE: 
//
//*****************************************************************************
double DegreesToRadians(double degrees)
{
  // 1 deg = PI/180 radians
  const double RAD_MULTIPLIER = VALUE_OF_PI / 180.0;
  return (degrees * RAD_MULTIPLIER);
}

//*****************************************************************************
//    NAME: RadiansToDegrees
//
// PURPOSE: 
//
//*****************************************************************************
double RadiansToDegrees(double radians)
{
  // 1 radian = 180/PI
  const double DEGREE_MULTIPLIER = 180.0 / VALUE_OF_PI;
  return (radians * DEGREE_MULTIPLIER);  
}

//*****************************************************************************
// NAME: GetRotatedPoint
//
// PURPOSE: Return the X,Y coordinate of a point rotated around a given point a
//          given number of radians. Default Rotation Point is 0,0.
//
//*****************************************************************************
void GetRotatedPoint(double& rPointX, double& rPointY, double degrees, double RotatePointX/*=0*/, double RotatePointY/*=0*/) 
{ 
  double Radians = DegreesToRadians(degrees);
  double cosRads = cos(Radians);
  double sinRads = sin(Radians);
  double dTexpX = rPointX;
  double dTexpY = rPointY;
  rPointX = RotatePointX + (dTexpX - RotatePointX) * cosRads - (dTexpY - RotatePointY) * sinRads;        
  rPointY = RotatePointY + (dTexpX - RotatePointX) * sinRads + (dTexpY - RotatePointY) * cosRads;
}

//*****************************************************************************
// NAME: GetAngle
//
// PURPOSE: Returns the angle (in degrees) of a line between two points and the X axis.
//*****************************************************************************
double GetAngle(double dX1, double dY1, double dX2, double dY2)
{
  // atan2(y,x) = arctan(y/x)
  double RadianAngle = atan2((dY2 - dY1),(dX2 - dX1));
  return (RadiansToDegrees(RadianAngle));
}

// x,y should be center of caster and target
//*****************************************************************************
//    NAME: GetSpellTargetingDir
//
// PURPOSE: 
//
//*****************************************************************************
PATH_DIR GetSpellTargetingDir(int cx, int cy, int tx, int ty)
{
  int dist = Distance(cx, cy, tx, ty);
  PATH_DIR dir = GetDir(cx,cy, tx,ty);

  if (dist == 0) return PathBAD;
  if (dist <= 2) return dir;

  // else we need to make possible corrections...
  
  switch (dir)
  {
  case PathNorth:
  case PathSouth:
  case PathEast:
  case PathWest:
    return dir;

  case PathNE:
    if (abs(cy-ty) <= 1) 
      return PathEast;
    else if (abs(cx-tx) <= 1) 
      return PathNorth;
    else
      return dir;
  
  case PathSE:
    if (abs(cy-ty) <= 1) 
      return PathEast;
    else if (abs(cx-tx) <= 1) 
      return PathSouth;
    else
      return dir;
  
  case PathSW:
    if (abs(cy-ty) <= 1) 
      return PathWest;
    else if (abs(cx-tx) <= 1) 
      return PathSouth;
    else
      return dir;
  
  case PathNW:
    if (abs(cy-ty) <= 1) 
      return PathWest;
    else if (abs(cx-tx) <= 1) 
      return PathNorth;
    else
      return dir;

  default:
    return PathBAD;
  }
}

/*
A fail-proof method is to compute the barycentric coordinates.  For a
triangle {(x1,y1), (x2,y2), (x3,y3)} and some point (x0,y0), calculate
 
b0 = (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1)
b1 = ((x2 - x0) * (y3 - y0) - (x3 - x0) * (y2 - y0)) / b0 
b2 = ((x3 - x0) * (y1 - y0) - (x1 - x0) * (y3 - y0)) / b0
b3 = 1 - b1 - b2
 
Then if b1, b2, and b3 are all > 0, (x0,y0) is strictly inside the
triangle; if bi = 0 and the other two coordinates are positive, (x0,y0)
lies on the edge opposite (xi,yi); if bi and bj = 0, (x0,y0) lies on
(xk,yk); if bi < 0, (x0,y0) lies outside the edge opposite (xi,yi);
if all three coordinates are negative, something else is wrong.  This
method does not depend on the cyclic order of the vertices.

If b0 is zero, the triangle has an area of zero because all
three vertices are coincident or colinear.  Notice that because
b1 + b2 + b3 = 1, if bi is > 0 the other coordinates will compensate, 
and not all three can be negative.

If b1 = b2 = b3 then (x0,y0) is at the centroid (center of gravity)
of the triangle, x0 = (x1 + x2 + x3) /3, and y0 = (y1 + y2 + y3) /3.
*/
//*****************************************************************************
//    NAME: IsPointInTriangle
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IsPointInTriangle(double x0,double y0, double x1,double y1, double x2,double y2, double x3, double y3, int onLine)
{
  // onLine bits
  // bit 0 - include points on the x2-x3 line
  // bit 1 - include points on the x1-x3 line
  // bit 2 - include points on the a1-x2 line
  bool inX1X2, inX1X3, inX2X3;
  
  double b0 = (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);
  if (b0 == 0) return FALSE; // triangle area is zero

  // PRS  20110309
  // I modified this to compute everything scaled by b0.
  // This avoids dividing by b0.


  double b1 = ((y3-y1) * (x0-x1) - (x3-x1) * (y0-y1)); //   / b0;
  double b2 = ((y1-y2) * (x0-x1) - (x1-x2) * (y0-y1)); //   / b0;
  if (b0 < 0)
  {
    b0 = -b0;
    b1 = -b1;
    b2 = -b2;
  };
  double b3 = b0 - b1 - b2;

// How could this happen???  PRS 20110309  if ( (b1 + b2 + b3) != 1) return FALSE;


  // check for x0,y0 on edge of triangle

  inX2X3 = (b3 > 0) || (b3 == 0) && (onLine & 1);
  inX1X3 = (b1 > 0) || (b1 == 0) && (onLine & 2);
  inX1X2 = (b2 > 0) || (b2 == 0) && (onLine & 4);



  return inX1X2 && inX1X3 && inX2X3;
}

//*****************************************************************************
//    NAME: IsPointInSquare
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IsPointInSquare(int x0, int y0, int x1,int y1, int x2,int y2)
{
  // make sure x1,y1 is upper left and x2,y2 is lower right
  ASSERT( x1 < x2 );
  ASSERT( y1 < y2 );
  
  if (x0 < x1) return FALSE;
  if (x0 > x2) return FALSE;
  if (y0 < y1) return FALSE;
  if (y0 > y2) return FALSE;
  return TRUE;
}


BOOL DoRectanglesOverlap(int x1A, int y1A, int x2A, int y2A, 
                         int x1B, int y1B, int x2B, int y2B)
{
  if (x2A < x1B) return FALSE;
  if (x1A > x2B) return FALSE;
  if (y2A < y1B) return FALSE;
  if (y1A > y2B) return FALSE;
  return TRUE;
}

//*****************************************************************************
//    NAME: IsPointInTargList
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IsPointInTargList(MAPLOCATIONLIST &list, POINT &pt)
{
  POSITION pos = list.GetHeadPosition();
  while (pos != NULL)
  {
    if (   (list.GetAt(pos).x==pt.x)
        && (list.GetAt(pos).y==pt.y))
      return TRUE;

    list.GetNext(pos);
  }
  return FALSE;
}

//*****************************************************************************
//    NAME: IsDudeInTargList
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IsDudeInTargList(MAPTARGETLIST &list, int dude)
{
  POSITION pos = list.GetHeadPosition();
  while (pos != NULL)
  {
    if (list.GetNext(pos)==dude)
      return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
//    NAME: JoinLists
//
// PURPOSE: 
//
//*****************************************************************************
void JoinLists(MAPTARGETLIST &DestList, MAPTARGETLIST &SrcList)
{
  POSITION pos = SrcList.GetHeadPosition();
  while (pos!=NULL)
  {
    if (!IsDudeInTargList(DestList, SrcList.GetAt(pos)))
      DestList.AddTail(SrcList.GetNext(pos));
  }
}

//*****************************************************************************
//    NAME: GetCombatantsInSquare
//
// PURPOSE: 
//
//*****************************************************************************
//int GetCombatantsInSquare(MAPTARGETLIST &list, int x1, int y1, int x2, int y2)
int GetCombatantsInSquare(MAPTARGETLIST &list, MAPLOCATIONLIST &locations)
{
  list.RemoveAll();

//  // make sure x1,y1 is upper left and x2,y2 is lower right
//  ASSERT( x1 <= x2 );
//  ASSERT( y1 <= y2 );

  // it might be quicker to scan list of targets for
  // position inside square, than to check each point
  // in square to see if a combatant is standing there.
  //
  //int totalsquares = ( (x2-x1+1) * (y2-y1+1) );
//  if (combatData.NumCombatants() <= totalsquares)
//  {
//#ifdef SHOW_SPELL_TARGET_OUTLINES
//    displayDotOnMap(x1,y1);
//    displayDotOnMap(x2,y2);
//#endif
//
//    for (int i=0; i<combatData.NumCombatants(); i++)
//    {
//      COMBATANT *pCombatant;
//      int cx1, cy1, cx2, cy2;
//      pCombatant = combatData.GetCombatant(i);
//      cx1 = pCombatant->x;
//      cy1 = pCombatant->y;
//      cx2 = cx1+pCombatant->width-1;
//      cy2 = cy1+pCombatant->height-1;
//      if (DoRectanglesOverlap(cx1, cy1, cx2, cy2,
//                              x1,y1, x2,y2))
//      {
//        if (!IsDudeInTargList(list, i))
//        {
//          list.AddTail(i);
//
//#ifdef SHOW_SPELL_TARGET_OUTLINES
//          displayDotOnMap(combatData.GetCombatant(i)->x,combatData.GetCombatant(i)->y);
//#endif
//        }
//      }
//    }
//  }
//  else
  {
    // scan each square for combatant
    //for (int x=x1;x<=x2;x++)
    POSITION pos;
    for (pos=locations.GetHeadPosition(); pos!=NULL; locations.GetNext(pos))
    {
      int x, y;
      POINT& point = locations.GetAt(pos);
      x = point.x;
      y = point.y;
      //for (int y=y1;y<=y2;y++)
      {
        int dude = getCombatantInCell(x,y,1,1);
        if (dude != NO_DUDE)
        {
          if (!IsDudeInTargList(list, dude))
          {
            list.AddTail(dude);
#ifdef SHOW_SPELL_TARGET_OUTLINES
            displayDotOnMap(combatData.GetCombatant(dude)->x,combatData.GetCombatant(dude)->y);
#endif            
          }
        }
      }
    }
  }
  return (list.GetCount());
}

//*****************************************************************************
//    NAME: GetMapTilesInSquare
//
// PURPOSE: 
//
//*****************************************************************************
int GetMapTilesInSquare(MAPLOCATIONLIST &list, int x1, int y1, int x2, int y2)
{
  list.RemoveAll();
  
  // make sure x1,y1 is upper left and x2,y2 is lower right
  ASSERT( x1 <= x2 );
  ASSERT( y1 <= y2 );
  if (x1 < 0) x1 = 0;
  if (y1 < 0) y1 = 0;
  if (x2 > MAX_TERRAIN_WIDTH-1)  x2 = MAX_TERRAIN_WIDTH-1;
  if (y2 > MAX_TERRAIN_HEIGHT-1) y2 = MAX_TERRAIN_HEIGHT-1;
  for (int x=x1;x<=x2;x++)
  {
    for (int y=y1;y<=y2;y++)
    {
      POINT pt = { x,y };
      list.AddTail( pt );
    }
  }

  return (list.GetCount());
}

//*****************************************************************************
//    NAME: GetCombatantsInRectangle
//
// PURPOSE: 
//
//*****************************************************************************
//int GetCombatantsInRectangle(MAPTARGETLIST &list, int sx, int sy, int dirX, int dirY, int width, int height)
int GetCombatantsInRectangle(MAPTARGETLIST &list, MAPLOCATIONLIST& locations)
{
  // sx,sy defines center of square
  list.RemoveAll();
//  if ((width <= 0)||(height <= 0)) return 0;

//  // for larger squares, offset sx,sy to upper left corner.
//  double half = width/2.0;
//  if (half > 1.0)
//    sx -= ((int)half);
  
//  half = height/2.0;  
//  if (half > 1.0)
//    sy -= ((int)half);
//
//  int ex = sx+width-1;
//  int ey = sy+height-1;
//  GetCombatantsInSquare(list, sx,sy,ex,ey);
  GetCombatantsInSquare(list, locations);
  return (list.GetCount());
}

//*****************************************************************************
//    NAME: GetMapTilesInRectangle
//
// PURPOSE: 
//
//*****************************************************************************

int deltax[4] = {0,1,0,-1};
int deltay[4] = {-1,0,1,0};

int GetMapTilesInRectangle(MAPLOCATIONLIST &list, 
                           int x0, int y0, 
                           int dirX, int dirY, 
                           int width, int height, 
                           bool forceNonZero)
{
  int targetX, targetY;
  int curCellEntry, numCellEntry;
  int Cp, Cn;
  int D2, D2d2p, D2d2n;
  int maxCells;
  CArray<POINT, POINT&> cellArray;
  CArray<unsigned char, unsigned char&> testedCells;
  POINT point;
  // sx,sy defines center of square
  list.RemoveAll();
  if (forceNonZero)
  {
    if (width < 1) width = 1;
    if (height < 1) height = 1;
  };
  if ((width <= 0)||(height <= 0)) return 0;

  
/*  
 *  Totally rewritten to center the rectangle and
 *  rotate it for the various directions.
 *  Width is normal to casting direction; Height is parallel.
 *
 * // for larger squares, offset sx,sy to upper left corner.
 * double half = width/2.0;
 * if (half > 1.0)
 *   sx -= ((int)half);
 * 
 * half = height/2.0;  
 * if (half > 1.0)
 *   sy -= ((int)half);
 * 
 * int ex = sx+width-1;
 * int ey = sy+height-1;
 * GetMapTilesInSquare(list, sx,sy,ex,ey);
 * return (list.GetCount());
 */

  if ( (dirX==0) && (dirY==0)) dirX = 1;
  //
  D2 = dirX*dirX + dirY*dirY;
  targetX = x0*4;
  targetY = y0*4;
  targetX += (x0<0)?-1:1;
  targetY += (y0<0)?-1:1;
  //
  // Conpute the two perpendicular lines through target.
  //
  // dirX * y + dirY * x + Cp = 0
  Cp = -(dirX * targetY + dirY*targetX);
  //
  //
  // dirX * x - dirY * y + Cn = 0
  Cn = dirY * targetY - dirX * targetX;
  //
  // Now we can start with the target and expand
  // outward, recording all cells within the specified
  // width and height.
  //
  D2d2p = height*2;  // Allowable distance in height
  D2d2p = D2*D2d2p*D2d2p;
  D2d2n = width*2;   // Allowable distance in width
  D2d2n = D2*D2d2n*D2d2n;
  dirX = 4*dirX;
  dirY = 4*dirY;
  maxCells = width*height;
  cellArray.SetSize(maxCells);
  testedCells.SetSize(MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT);
  memset(&testedCells[0], 0, MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT);
  numCellEntry = 1;
  cellArray[0].x = x0;
  cellArray[0].y = y0;
  testedCells[MAX_TERRAIN_WIDTH*y0+x0] = 1;
  for (curCellEntry=0; curCellEntry<numCellEntry; curCellEntry++)
  {
    // Look in four directions and add any cells
    // that are within range and have not already been added.
    int xc, yc, x, y;
    int i;
    int d;
    xc = cellArray[curCellEntry].x;
    yc = cellArray[curCellEntry].y;
    for (i=0; i<4; i++)
    {
      x = xc + deltax[i];
      y = yc + deltay[i];
      if (x < 0) continue;
      if (x >= MAX_TERRAIN_WIDTH) continue;
      if (y < 0) continue; 
      if (y >= MAX_TERRAIN_HEIGHT) continue;
      if (testedCells[MAX_TERRAIN_WIDTH*y+x] != 0) continue;
      testedCells[MAX_TERRAIN_WIDTH*y+x] = 1;
      d = dirX*y + dirY*x + Cp;
      if (d*d >= D2d2n) continue;
      d = dirX*x - dirY*y + Cn;
      if (d*d >= D2d2p) continue;
      point.x = x;
      point.y = y;
      cellArray.SetAtGrow(numCellEntry++, point);
    }
  };
  {
    int i;
    for (i=0; i<numCellEntry; i++)
    {
      list.AddTail(cellArray[i]);
    };
  };
  return list.GetCount();
}

//*****************************************************************************
//    NAME: GetCombatantsInCircle
//
// PURPOSE: 
//
//*****************************************************************************
int GetCombatantsInCircle(MAPTARGETLIST &list, int sx, int sy, int radius)
{
  // sx,sy defines center of circle
  list.RemoveAll();
  if (radius < 0) return 0;

  MAPTARGETLIST squarelist;
  MAPLOCATIONLIST locations;
  
  int squaresize = radius * 2;
  squaresize |= 1; // make it odd so that there is 'radius' on both sides of target

  //GetCombatantsInRectangle(squarelist, sx,sy, squaresize,squaresize);
  // direction in not important for a circle.
  //GetCombatantsInRectangle(squarelist, sx,sy, PathEast, squaresize,squaresize);
  //GetCombatantsInRectangle(squarelist, sx,sy, 1, 0, squaresize,squaresize);
  GetMapTilesInRectangle(locations, sx, sy, 1, 0, squaresize, squaresize, false);
  GetCombatantsInRectangle(squarelist, locations);
  if (squarelist.GetCount()==0) return 0;
  // now prune the targets that may be beyond radius
  POSITION pos=squarelist.GetHeadPosition();
  while (pos!=NULL)
  {
    int dude;
    COMBATANT *pCombatant;
    int combatantX, combatantY;
    //int combatantWidth, combatantHeight;
    dude = squarelist.GetNext(pos);
    pCombatant = combatData.GetCombatant(dude);
    combatantX      = pCombatant->x;
    combatantY      = pCombatant->y;
    //combatantWidth  = pCombatant->width;
    //combatantHeight = pCombatant->height;
    if (    Distance(sx, sy, dude, combatantX, combatantY) <= radius)
    {
      list.AddTail(dude);
    };
  }
  return (list.GetCount());
}

//*****************************************************************************
//    NAME: GetMapTilesInCircle
//
// PURPOSE: 
//
//*****************************************************************************
int GetMapTilesInCircle(MAPLOCATIONLIST &list, int sx, int sy, int radius)
{
  // sx,sy defines center of circle
  list.RemoveAll();
  if (radius < 0) return 0;
  
  MAPLOCATIONLIST squarelist;
  
  int squaresize = radius * 2;
  squaresize |= 1; // make it odd so that there is 'radius' on both sides of target
  
  //GetMapTilesInRectangle(squarelist, sx,sy, squaresize,squaresize);
  // Direction is not important for a circle
  //GetMapTilesInRectangle(squarelist, sx,sy, PathEast, squaresize,squaresize);
  GetMapTilesInRectangle(squarelist, sx,sy, 1, 0, squaresize,squaresize, false);
  if (squarelist.GetCount()==0) return 0;
  // now prune the targets that may be beyond radius
  POSITION pos=squarelist.GetHeadPosition();
  while (pos!=NULL)
  {
    POINT pt = squarelist.GetNext(pos);
    int distFromCenter = Distance(sx, sy, pt.x, pt.y);
    if (distFromCenter <= radius)
      list.AddTail(pt);
  }
  return (list.GetCount());
}

/* 
 *  These originals worked approximately only for the 8 major directions.
 *  I have attempted to make it work exactly for arbitrary directions.
 * //*****************************************************************************
 * //    NAME: GetCombatantsInCone
 * //
 * // PURPOSE: 
 * //
 * //*****************************************************************************
 * int GetCombatantsInCone(MAPTARGETLIST &list, int Px, int Py, int length, int width, PATH_DIR dir)
 * {
 *   // starts at sx,sy, extends 'length' squares, and is 'width' wide at max length.
 *   //
 *   //           -A
 *   //       -    -
 *   //   -        -
 *   // P -------->Length
 *   //   -        -
 *   //       -    -
 *   //           -B
 *   //
 *   // P = sx,sy
 *   //
 *   // As you can see, the cone is really a 
 *   // triangle defined by points P,A,B
 *   //
 *   // 'width' defines the distance between A and B
 *   // 'length' is the height of the triangle
 *   // 
 * 
 *   list.RemoveAll();
 *   if ((length <= 0)||(width <= 0)) return 0;
 * 
 *   MAPTARGETLIST squarelist;
 * 
 *   int Ax,Ay, Bx,By, Lx,Ly;
 *   double diag_tmp = ((double)width)/1.5;
 *   int diag_width = (int)floor(diag_tmp);
 * 
 *   switch (dir)
 *   {
 *   case PathNorth:
 *     {
 *       Lx=Px;
 *       Ly=Py-length;
 *       width &= ~0x01; // ensure even number
 *       width /= 2;
 *       Ax=Lx-width;
 *       Ay=Ly;
 *       Bx=Lx+width;
 *       By=Ly;
 *     }
 *     break;
 *   case PathNE:
 *     {
 *       Lx=Px+length;
 *       Ly=Py-length;
 *       width = diag_width;
 *       width &= ~0x01; // ensure even number
 *       width /= 2;
 *       Ax=Lx-width;
 *       Ay=Ly-width;
 *       Bx=Lx+width;
 *       By=Ly+width;
 *     }
 *     break;
 *   case PathEast:
 *     {
 *       Lx=Px+length;
 *       Ly=Py;
 *       width &= ~0x01; // ensure even number
 *       width /= 2;
 *       Ax=Lx;
 *       Ay=Ly-width;
 *       Bx=Lx;
 *       By=Ly+width;
 *     }
 *     break;
 *   case PathSE:
 *     {
 *       Lx=Px+length;
 *       Ly=Py+length;
 *       width = diag_width;
 *       width &= ~0x01; // ensure even number
 *       width /= 2;
 *       Ax=Lx+width;
 *       Ay=Ly-width;
 *       Bx=Lx-width;
 *       By=Ly+width;
 *     }
 *     break;
 *   case PathSouth:
 *     {
 *       Lx=Px;
 *       Ly=Py+length;
 *       width &= ~0x01; // ensure even number
 *       width /= 2;
 *       Ax=Lx-width;
 *       Ay=Ly;
 *       Bx=Lx+width;
 *       By=Ly;
 *     }
 *     break;
 *   case PathSW:
 *     {
 *       Lx=Px-length;
 *       Ly=Py+length;
 *       width = diag_width;
 *       width &= ~0x01; // ensure even number
 *       width /= 2;
 *       Ax=Lx-width;
 *       Ay=Ly-width;
 *       Bx=Lx+width;
 *       By=Ly+width;
 *     }
 *     break;
 *   case PathWest:
 *     {
 *       Lx=Px-length;
 *       Ly=Py;
 *       width &= ~0x01; // ensure even number
 *       width /= 2;
 *       Ax=Lx;
 *       Ay=Ly-width;
 *       Bx=Lx;
 *       By=Ly+width;
 *     }
 *     break;
 *   case PathNW:
 *     {
 *       Lx=Px-length;
 *       Ly=Py-length;
 *       width = diag_width;
 *       width &= ~0x01; // ensure even number
 *       width /= 2;
 *       Ax=Lx+width;
 *       Ay=Ly-width;
 *       Bx=Lx-width;
 *       By=Ly+width;
 *     }
 *     break;
 *   default:
 *     return 0;
 *     break;
 *   }
 * 
 *   // clip to terrain boundaries
 *   if (Lx<0) Lx=0;
 *   if (Lx >= MAX_TERRAIN_WIDTH) Lx=MAX_TERRAIN_WIDTH-1;
 *   if (Ly<0) Ly=0;
 *   if (Ly >= MAX_TERRAIN_HEIGHT) Ly=MAX_TERRAIN_HEIGHT-1;
 * 
 *   if (Ax<0) Ax=0;
 *   if (Ax >= MAX_TERRAIN_WIDTH) Ax=MAX_TERRAIN_WIDTH-1;
 *   if (Ay<0) Ay=0;
 *   if (Ay >= MAX_TERRAIN_HEIGHT) Ay=MAX_TERRAIN_HEIGHT-1;
 * 
 *   if (Bx<0) Bx=0;
 *   if (Bx >= MAX_TERRAIN_WIDTH) Bx=MAX_TERRAIN_WIDTH-1;
 *   if (By<0) By=0;
 *   if (By >= MAX_TERRAIN_HEIGHT) By=MAX_TERRAIN_HEIGHT-1;
 * 
 * #ifdef SHOW_SPELL_TARGET_OUTLINES
 *   displayDotOnMap(Px,Py);
 *   displayDotOnMap(Lx,Ly);
 *   displayDotOnMap(Ax,Ay);
 *   displayDotOnMap(Bx,By);
 * #endif
 * 
 *   // Now we know the triangle points P,A,B in terms
 *   // of terrain coords.
 * 
 *   // The Point-In-Triangle test is slow. I think it is faster to check each
 *   // combatant's coords rather than determine every point inside the 
 *   // triangle and see if a combatant is standing there. To reduce the
 *   // number of combatants to check out, determine the bounding square
 *   // that surrounds the triangle, and only check the combatants in that
 *   // square.
 *   //
 * 
 *   // figure out the bounding square
 *   int x1 = min(Px,Ax);
 *   x1 = min(x1,Bx);
 *   int y1 = min(Py,Ay);
 *   y1 = min(y1,By);
 * 
 *   int x2 = max(Px,Ax);
 *   x2 = max(x2,Bx);
 *   int y2 = max(Py,Ay);
 *   y2 = max(y2,By);
 * 
 *   GetCombatantsInSquare(squarelist, x1,y1,x2,y2);
 * 
 *   // now check each targ in list to see if it
 *   // is inside the triange PAB
 * 
 *   POSITION pos = squarelist.GetHeadPosition();
 *   while (pos!=NULL)
 *   {
 *     int targ=squarelist.GetNext(pos);
 *     BOOL inside = IsPointInTriangle(combatData.GetCombatant(targ)->x, 
 *                                     combatData.GetCombatant(targ)->y,
 *                                     Px,Py,
 *                                     Ax,Ay,
 *                                     Bx,By);
 *     if (inside)
 *       list.AddTail(targ);
 *   }
 * 
 *   return (list.GetCount());
 * }
 */


int SquareRoot(int x)
{
  int oldy, newy;
  newy = x;
  do
  {
    oldy = newy;
    newy = (oldy + x/oldy)/2;
  } while (oldy != newy);
  return newy;
}

//#define SCALE_FACTOR 210  // Must contain a factor of 2 so width/2 is exact.


/*
 *  These originals worked approximately only for the 8 major directions.
 *  I have attempted to make it work exactly for arbitrary directions.
 */
//*****************************************************************************
//    NAME: GetCombatantsandTilesInCone
//
// PURPOSE: 
//
//*****************************************************************************
int GetCombatantsAndTilesInCone(MAPTARGETLIST   &clist,   // combatant list
                                MAPLOCATIONLIST &tlist,   // tile list
                                int Cx, int Cy,           // Caster location
                                int Tx, int Ty,           // Target location
                                int length, int width,    // height and base of triangle
                                bool forceNonzero)
{
  // Caster (Cx, Cy) casts spell at point P (Px, Py) creating a triangle
  // starts at Target (Tx,Ty), extends 'length' squares, and is 'width' wide at max length.
  //
  //                 -A
  //             -    -
  //         -        -
  // C-----T -------->L
  //         -        -
  //             -    -
  //                 -B
  //
  // P = sx,sy
  //
  // As you can see, the cone is really a 
  // triangle defined by points T,A,B
  //
  // 'width' defines the distance between A and B
  // 'length' is the height of the triangle (T - L)
  // 
  // 
  double Ax,Ay, Bx,By, Lx,Ly, D;
  double sinT, cosT;  // Angles from C to T (and from T to L)
  //int Ux, Uy;
  int targ;
  BOOL inside;
  
  clist.RemoveAll();
  tlist.RemoveAll();
  if (forceNonzero)
  {
    if (length < 1) length = 1;
    if (width < 1) width = 1;
  };
  if ((length <= 0)||(width <= 0)) return 0;

  if (width > MAX_TERRAIN_HEIGHT) width = MAX_TERRAIN_HEIGHT;
  if (width > MAX_TERRAIN_WIDTH)  width = MAX_TERRAIN_WIDTH;
  if (length > MAX_TERRAIN_HEIGHT) length = MAX_TERRAIN_HEIGHT;
  if (length > MAX_TERRAIN_WIDTH)  length = MAX_TERRAIN_WIDTH;
  //length = length - 1;  // FRUAers count posts rather than rails.
  //width  = width - 1;

  D = sqrt((double) (Tx-Cx)*(Tx-Cx)+(Ty-Cy)*(Ty-Cy));
  sinT =  (Ty-Cy)/D;
  cosT =  (Tx-Cx)/D;
  Lx     =  Tx + (length-0.000001) * cosT;
  Ly     =  Ty + (length-0.000001) * sinT;
  Ax     =  Lx - (width/2.0) * sinT;
  Ay     =  Ly + (width/2.0) * cosT;
  Bx     =  Lx + (width/2.0) * sinT;
  By     =  Ly - (width/2.0) * cosT;

  // Now we know the location of the three points (T, A, and B) that define the triangle.
  // clip to terrain boundaries
  /*  Don't limit to the map......wrap around the torus
  if (Lx<0) Lx=0;
  if (Lx >= MAX_TERRAIN_WIDTH*SCALE_FACTOR) Lx=MAX_TERRAIN_WIDTH*SCALE_FACTOR-1;
  if (Ly<0) Ly=0;
  if (Ly >= MAX_TERRAIN_HEIGHT*SCALE_FACTOR) Ly=MAX_TERRAIN_HEIGHT*SCALE_FACTOR-1;
 
  if (Ax<0) Ax=0;
  if (Ax >= MAX_TERRAIN_WIDTH*SCALE_FACTOR) Ax=MAX_TERRAIN_WIDTH*SCALE_FACTOR-1;
  if (Ay<0) Ay=0;
  if (Ay >= MAX_TERRAIN_HEIGHT*SCALE_FACTOR) Ay=MAX_TERRAIN_HEIGHT*SCALE_FACTOR-1;
 
  if (Bx<0) Bx=0;
  if (Bx >= MAX_TERRAIN_WIDTH*SCALE_FACTOR) Bx=MAX_TERRAIN_WIDTH*SCALE_FACTOR-1;
  if (By<0) By=0;
  if (By >= MAX_TERRAIN_HEIGHT*SCALE_FACTOR) By=MAX_TERRAIN_HEIGHT*SCALE_FACTOR-1;
  */
 
 #ifdef SHOW_SPELL_TARGET_OUTLINES
   displayDotOnMap(Bx, By);
 #endif
  // The Point-In-Triangle test is slow. I think it is faster to check each
  // combatant's coords rather than determine every point inside the 
  // triangle and see if a combatant is standing there. To reduce the
  // number of combatants to check out, determine the bounding square
  // that surrounds the triangle, and only check the combatants in that
  // square.
  //
   //
  {
    int x,y;
    int left, right, top, bottom;
    left   = min(Tx,min(Ax,Bx));
    right  = max(Tx,max(Ax,Bx));
    top    = min(Ty,min(Ay,By));
    bottom = max(Ty,max(Ay,By));

    if (left   < 0) left   = 0;
    if (right  < 0) right  = 0;
    if (top    < 0) top    = 0;
    if (bottom < 0) bottom = 0;
    if (left   >= MAX_TERRAIN_WIDTH)  left   = MAX_TERRAIN_WIDTH-1;
    if (right  >= MAX_TERRAIN_WIDTH)  right  = MAX_TERRAIN_WIDTH-1;
    if (top    >= MAX_TERRAIN_HEIGHT) top    = MAX_TERRAIN_HEIGHT-1;
    if (bottom >= MAX_TERRAIN_HEIGHT) bottom = MAX_TERRAIN_HEIGHT-1;


    for (x=left; x<=right; x++)
    {
      for (y=top; y<=bottom; y++)
      {
        if (IsPointInTriangle ( x,  y,
                                Tx, Ty,
                                Ax, Ay,
                                Bx, By,
                                7         // Include points on the line
                              ))
        {
          POINT pt;
          pt.x = x;
          pt.y = y;
          tlist.AddTail(pt);
        };
      };
    };
  };
  

  for (targ=0; targ<combatData.NumCombatants(); targ++)
  {
    COMBATANT *pCombatant;
    pCombatant = combatData.GetCombatant(targ);
    inside = FALSE;
    for (int w=0; (w<pCombatant->width) && !inside; w++)
    {
      for (int h=0; (h<pCombatant->height) && !inside; h++)
      {
        inside = IsPointInTriangle(  (pCombatant->x+w), 
                                          (pCombatant->y+w),
                                          Tx,Ty,
                                          Ax,Ay,
                                          Bx,By,
                                          1  // Include points on the A-B line
                                       );  
        {
          POSITION pos;
          pos = tlist.GetHeadPosition();
          while (pos != NULL)
          {
            POINT *pt;
            pt = &tlist.GetNext(pos);
            if ((pt->x == pCombatant->x+w) && (pt->y == pCombatant->y+h))
            {
              inside = TRUE;
              break;
            };
          };
        };
      };
    };
    if (inside)
    {
      clist.AddTail(targ);
    };
  }
 
  return (clist.GetCount());
}


//*****************************************************************************
//    NAME: GetMapTilesInCone
//
// PURPOSE: 
//
//*****************************************************************************
int GetMapTilesInCone(MAPLOCATIONLIST &list, int Px, int Py, int length, int width, PATH_DIR dir)
{
  // starts at sx,sy, extends 'length' squares, and is 'width' wide at max length.
  //
  //           -A
  //       -    -
  //   -        -
  // P -------->length
  //   -        -
  //       -    -
  //           -B
  //
  // P = sx,sy
  //
  // As you can see, the cone is really a 
  // triangle defined by points P,A,B
  //
  // 'width' defines the distance between A and B
  // 'length' is the height of the triangle
  // 

  list.RemoveAll();
  if ((length <= 0)||(width <= 0)) return 0;

  MAPLOCATIONLIST squarelist;

  int Ax,Ay, Bx,By, Lx,Ly;
  double diag_tmp = ((double)width)/1.5;
  int diag_width = (int)floor(diag_tmp);

  switch (dir)
  {
  case PathNorth:
    {
      Lx=Px;
      Ly=Py-length;
      width &= ~0x01; // ensure even number
      width /= 2;
      Ax=Lx-width;
      Ay=Ly;
      Bx=Lx+width;
      By=Ly;
    }
    break;
  case PathNE:
    {
      Lx=Px+length;
      Ly=Py-length;
      width = diag_width;
      width &= ~0x01; // ensure even number
      width /= 2;
      Ax=Lx-width;
      Ay=Ly-width;
      Bx=Lx+width;
      By=Ly+width;
    }
    break;
  case PathEast:
    {
      Lx=Px+length;
      Ly=Py;
      width &= ~0x01; // ensure even number
      width /= 2;
      Ax=Lx;
      Ay=Ly-width;
      Bx=Lx;
      By=Ly+width;
    }
    break;
  case PathSE:
    {
      Lx=Px+length;
      Ly=Py+length;
      width = diag_width;
      width &= ~0x01; // ensure even number
      width /= 2;
      Ax=Lx+width;
      Ay=Ly-width;
      Bx=Lx-width;
      By=Ly+width;
    }
    break;
  case PathSouth:
    {
      Lx=Px;
      Ly=Py+length;
      width &= ~0x01; // ensure even number
      width /= 2;
      Ax=Lx-width;
      Ay=Ly;
      Bx=Lx+width;
      By=Ly;
    }
    break;
  case PathSW:
    {
      Lx=Px-length;
      Ly=Py+length;
      width = diag_width;
      width &= ~0x01; // ensure even number
      width /= 2;
      Ax=Lx-width;
      Ay=Ly-width;
      Bx=Lx+width;
      By=Ly+width;
    }
    break;
  case PathWest:
    {
      Lx=Px-length;
      Ly=Py;
      width &= ~0x01; // ensure even number
      width /= 2;
      Ax=Lx;
      Ay=Ly-width;
      Bx=Lx;
      By=Ly+width;
    }
    break;
  case PathNW:
    {
      Lx=Px-length;
      Ly=Py-length;
      width = diag_width;
      width &= ~0x01; // ensure even number
      width /= 2;
      Ax=Lx+width;
      Ay=Ly-width;
      Bx=Lx-width;
      By=Ly+width;
    }
    break;
  default:
    return 0;
    break;
  }

  // clip to terrain boundaries
  if (Lx<0) Lx=0;
  if (Lx >= MAX_TERRAIN_WIDTH) Lx=MAX_TERRAIN_WIDTH-1;
  if (Ly<0) Ly=0;
  if (Ly >= MAX_TERRAIN_HEIGHT) Ly=MAX_TERRAIN_HEIGHT-1;

  if (Ax<0) Ax=0;
  if (Ax >= MAX_TERRAIN_WIDTH) Ax=MAX_TERRAIN_WIDTH-1;
  if (Ay<0) Ay=0;
  if (Ay >= MAX_TERRAIN_HEIGHT) Ay=MAX_TERRAIN_HEIGHT-1;

  if (Bx<0) Bx=0;
  if (Bx >= MAX_TERRAIN_WIDTH) Bx=MAX_TERRAIN_WIDTH-1;
  if (By<0) By=0;
  if (By >= MAX_TERRAIN_HEIGHT) By=MAX_TERRAIN_HEIGHT-1;

  // Now we know the triangle points P,A,B in terms
  // of terrain coords.
  //

  // figure out the bounding square
  int x1 = min(Px,Ax);
  x1 = min(x1,Bx);
  int y1 = min(Py,Ay);
  y1 = min(y1,By);

  int x2 = max(Px,Ax);
  x2 = max(x2,Bx);
  int y2 = max(Py,Ay);
  y2 = max(y2,By);

  GetMapTilesInSquare(squarelist, x1,y1,x2,y2);

  // now check each point in list to see if it
  // is inside the triange PAB

  POSITION pos = squarelist.GetHeadPosition();
  while (pos!=NULL)
  {
    POINT pt = squarelist.GetNext(pos);
    BOOL inside = IsPointInTriangle( pt.x, pt.y,
                                     Px,Py,
                                     Ax,Ay,
                                     Bx,By,
                                     1       // include the AB line as 'Inside'
                                    );
    if (inside)
      list.AddTail(pt);
  }

  return (list.GetCount());
}

//*****************************************************************************
//    NAME: ProcessLine2DTargets
//
// PURPOSE: 
//
//*****************************************************************************
BOOL ProcessLine2DTargets(DWORD ptr, int x, int y)
{
  int tx = WorldToTerrainCoordX(x);
  int ty = WorldToTerrainCoordY(y);
  
  if (!ValidCoords(ty,tx)) return FALSE; // terminate search

#ifdef SHOW_SPELL_TARGET_OUTLINES
  displayDotOnMap(tx,ty);
#endif

  int dude = getCombatantInCell(tx,ty,1,1);
  if (dude == NO_DUDE) return TRUE;

  MAPTARGETLIST *pList = (MAPTARGETLIST*)ptr;
  if (pList==NULL) return FALSE; // terminate search

  if (IsDudeInTargList(*pList, dude)) return TRUE;
  pList->AddTail(dude);
  return TRUE;
}

//*****************************************************************************
//    NAME: ProcessLine2DMapTiles
//
// PURPOSE: 
//
//*****************************************************************************
BOOL ProcessLine2DMapTiles(DWORD ptr, int x, int y)
{
  int tx = WorldToTerrainCoordX(x);
  int ty = WorldToTerrainCoordY(y);
  
  if (!ValidCoords(ty,tx)) return FALSE; // terminate search
    
  POINT pt = { tx,ty };
  
  MAPLOCATIONLIST *pList = (MAPLOCATIONLIST*)ptr;
  if (pList==NULL) return FALSE; // terminate search
  
  if (IsPointInTargList(*pList, pt)) return TRUE;
  pList->AddTail(pt);
  return TRUE;
}

//*****************************************************************************
//    NAME: GetCombatantsInLine
//
// PURPOSE: 
//
//*****************************************************************************
int GetCombatantsInLine(MAPTARGETLIST &list, int sx, int sy, int dx, int dy)
{
  list.RemoveAll();
  if (sx<0) sx=0;
  if (sx>MAX_TERRAIN_WIDTH) sx=MAX_TERRAIN_WIDTH-1;
  if (sy<0) sy=0;
  if (sy>MAX_TERRAIN_HEIGHT) sy=MAX_TERRAIN_HEIGHT-1;
 
  if (dx<0) dx=0;
  if (dx>MAX_TERRAIN_WIDTH) dx=MAX_TERRAIN_WIDTH-1;
  if (dy<0) dy=0;
  if (dy>MAX_TERRAIN_HEIGHT) dy=MAX_TERRAIN_HEIGHT-1;

  // offset map coords to center of icon (assumes 1x1 icon)
  // larger icons should pass center of themselves as mapx,mapy
  int swx = TerrainToWorldCoordX(sx);//+(COMBAT_TILE_WIDTH/2);
  int swy = TerrainToWorldCoordY(sy);//+(COMBAT_TILE_HEIGHT/2);  
  int dwx = TerrainToWorldCoordX(dx);//+(COMBAT_TILE_WIDTH/2);
  int dwy = TerrainToWorldCoordY(dy);//+(COMBAT_TILE_HEIGHT/2);  

  Line2DSpell((DWORD)&list, 
              swx, swy, 
              dwx, dwy, 
              ProcessLine2DTargets);

  return (list.GetCount());
}

//*****************************************************************************
//    NAME: GetMapTilesInLine
//
// PURPOSE: 
//
//*****************************************************************************
int GetMapTilesInLine(MAPLOCATIONLIST &list, int sx, int sy, int dx, int dy)
{
  list.RemoveAll();
  if (sx<0) sx=0;
  if (sx>MAX_TERRAIN_WIDTH) sx=MAX_TERRAIN_WIDTH-1;
  if (sy<0) sy=0;
  if (sy>MAX_TERRAIN_HEIGHT) sy=MAX_TERRAIN_HEIGHT-1;
  
  if (dx<0) dx=0;
  if (dx>MAX_TERRAIN_WIDTH) dx=MAX_TERRAIN_WIDTH-1;
  if (dy<0) dy=0;
  if (dy>MAX_TERRAIN_HEIGHT) dy=MAX_TERRAIN_HEIGHT-1;
  
  // offset map coords to center of icon (assumes 1x1 icon)
  // larger icons should pass center of themselves as mapx,mapy
  int swx = TerrainToWorldCoordX(sx);//+(COMBAT_TILE_WIDTH/2);
  int swy = TerrainToWorldCoordY(sy);//+(COMBAT_TILE_HEIGHT/2);  
  int dwx = TerrainToWorldCoordX(dx);//+(COMBAT_TILE_WIDTH/2);
  int dwy = TerrainToWorldCoordY(dy);//+(COMBAT_TILE_HEIGHT/2);  
  
  Line2DSpell((DWORD)&list, 
              swx, swy, 
              dwx, dwy, 
              ProcessLine2DMapTiles);
  
  return (list.GetCount());
}

//*****************************************************************************
//    NAME: GetCombatantsInLine
//
// PURPOSE: 
//
//*****************************************************************************
int GetCombatantsInLine(MAPTARGETLIST &list, int sx, int sy, int dx, int dy, int width)
{
  list.RemoveAll();
  if (width <= 0) return 0;
  GetCombatantsInLine(list, sx,sy, dx,dy);  
  return (list.GetCount());
}

//*****************************************************************************
//    NAME: GetMapTilesInLine
//
// PURPOSE: 
//
//*****************************************************************************
int GetMapTilesInLine(MAPLOCATIONLIST &list, int sx, int sy, int dx, int dy, int width)
{
  list.RemoveAll();
  if (width <= 0) return 0;  
  GetMapTilesInLine(list, sx,sy, dx,dy);  
  return (list.GetCount());
}

//*****************************************************************************
//    NAME: GetCombatantsInLine
//
// PURPOSE: 
//
//*****************************************************************************
int GetCombatantsInLine(MAPTARGETLIST &list, int sx, int sy, int length, int width, PATH_DIR dir)
{
  list.RemoveAll();
  if ((length <= 0)||(width <= 0)) return 0;

  if (sx<0) sx=0;
  if (sx>MAX_TERRAIN_WIDTH) sx=MAX_TERRAIN_WIDTH-1;
  if (sy<0) sy=0;
  if (sy>MAX_TERRAIN_HEIGHT) sy=MAX_TERRAIN_HEIGHT-1;
 
  int dx=sx,dy=sy;//,ex,ey;
  //double half;

  switch (dir)
  {
  case PathNorth:
    dx = sx;
    dy = sy-length;
    break;
  case PathNE:
    dx = sx+length;
    dy = sy-length;
    break;
  case PathEast:
    dx = sx+length;
    dy = sy;
    break;
  case PathSE:
    dx = sx+length;
    dy = sy+length;
    break;
  case PathSouth:
    dx = sx;
    dy = sy+length;
    break;
  case PathSW:
    dx = sx-length;
    dy = sy+length;
    break;
  case PathWest:
    dx = sx-length;
    dy = sy;
    break;
  case PathNW:
    dx = sx-length;
    dy = sy-length;
    break;
  }

  if (dx<0) dx=0;
  if (dx>MAX_TERRAIN_WIDTH) dx=MAX_TERRAIN_WIDTH-1;
  if (dy<0) dy=0;
  if (dy>MAX_TERRAIN_HEIGHT) dy=MAX_TERRAIN_HEIGHT-1;

  GetCombatantsInLine(list, sx,sy, dx,dy);

  //
  // need the following only if line width can be greater than 1
  //
  /*
  switch (dir)
  {
  case PathNorth:
    {
      half = width/2.0;  
      if (half > 1.0) sx -= ((int)half);  
      ex = sx+width;
      dy = sy - length;

      while (sx <= ex)
      {
        dx = sx;
        if (GetCombatantsInLine(templist, sx,sy, dx,dy) > 0)
          JoinLists(list, templist);
        sx++;
      }
    }
    break;
  case PathNE:
    {
      int startX=sx, startY=sy;
      half = width/2.0;  

      ey = sy;
      if (half >= 1.0) sy -= ((int)half);
      while (sy <= ey)
      {        
        dx = sx+length;
        dy = sy-length;
        if (GetCombatantsInLine(templist, sx,sy, dx,dy) > 0)
          JoinLists(list, templist);
        sy++;
      }

      if (half > 1.0)
      {
        // do the other leg if width high enough
        sy = startY; 
        sx = startX; ex = sx + ((int)half);
        
        while (sx <= ex)
        {        
          dx = sx+length;
          dy = sy-length;
          if (GetCombatantsInLine(templist, sx,sy, dx,dy) > 0)
            JoinLists(list, templist);
          sx++;
        }
      }
    }
    break;
  case PathEast:
    {
      half = width/2.0;  
      if (half > 1.0) sy -= ((int)half);  
      ey = sy+width;
      dx = sx + length;

      while (sy <= ey)
      {
        dy = sy;        
        if (GetCombatantsInLine(templist, sx,sy, dx,dy) > 0)
          JoinLists(list, templist);
        sy++;
      }
    }
    break;
  case PathSE:
    {
      int startX=sx, startY=sy;
      half = width/2.0;  
      
      if (half >= 1.0) ey = sy + ((int)half);
      else ey = sy;

      while (sy <= ey)
      {        
        dx = sx+length;
        dy = sy+length;
        if (GetCombatantsInLine(templist, sx,sy, dx,dy) > 0)
          JoinLists(list, templist);
        sy++;
      }

      if (half > 1.0)
      {
        // do the other leg if width high enough
        sy = startY; 
        sx = startX; ex = sx + ((int)half);
        
        while (sx <= ex)
        {        
          dx = sx+length;
          dy = sy+length;
          if (GetCombatantsInLine(templist, sx,sy, dx,dy) > 0)
            JoinLists(list, templist);
          sx++;
        }
      }
    }
    break;
  case PathSouth:
    {
      half = width/2.0;  
      if (half > 1.0) sx -= ((int)half);  
      ex = sx+width;
      dy = sy + length;

      while (sx <= ex)
      {
        dx = sx;
        if (GetCombatantsInLine(templist, sx,sy, dx,dy) > 0)
          JoinLists(list, templist);
        sx++;
      }
    }
    break;
  case PathSW:
    {
      int startX=sx, startY=sy;
      half = width/2.0;  
      
      if (half >= 1.0) ey = sy + ((int)half);
      else ey = sy;

      while (sy <= ey)
      {        
        dx = sx-length;
        dy = sy+length;
        if (GetCombatantsInLine(templist, sx,sy, dx,dy) > 0)
          JoinLists(list, templist);
        sy++;
      }

      if (half > 1.0)
      {
        // do the other leg if width high enough
        sy = startY; 
        ex = startX;
        sx = ex - ((int)half);
        
        while (sx <= ex)
        {        
          dx = sx-length;
          dy = sy+length;
          if (GetCombatantsInLine(templist, sx,sy, dx,dy) > 0)
            JoinLists(list, templist);
          sx++;
        }
      }
    }
    break;
  case PathWest:
    {
      half = width/2.0;  
      if (half > 1.0) sy -= ((int)half);  
      ey = sy+width;
      dx = sx - length;

      while (sy <= ey)
      {
        dy = sy;        
        if (GetCombatantsInLine(templist, sx,sy, dx,dy) > 0)
          JoinLists(list, templist);
        sy++;
      }
    }
    break;
  case PathNW:
    {
      int startX=sx, startY=sy;
      half = width/2.0;  

      ey = sy;
      if (half >= 1.0) sy -= ((int)half);
      while (sy <= ey)
      {        
        dx = sx-length;
        dy = sy-length;
        if (GetCombatantsInLine(templist, sx,sy, dx,dy) > 0)
          JoinLists(list, templist);
        sy++;
      }

      if (half > 1.0)
      {
        // do the other leg if width high enough
        sy = startY; 
        ex = startX; sx = ex - ((int)half);
        
        while (sx <= ex)
        {        
          dx = sx-length;
          dy = sy-length;
          if (GetCombatantsInLine(templist, sx,sy, dx,dy) > 0)
            JoinLists(list, templist);
          sx++;
        }
      }
    }
    break;
  }
  */

  return (list.GetCount());
}

//*****************************************************************************
//    NAME: GetMapTilesInLine
//
// PURPOSE: 
//
//*****************************************************************************
int GetMapTilesInLine(MAPLOCATIONLIST &list, int sx, int sy, int length, int width, PATH_DIR dir)
{
  list.RemoveAll();
  if ((length <= 0)||(width <= 0)) return 0;
  
  if (sx<0) sx=0;
  if (sx>MAX_TERRAIN_WIDTH) sx=MAX_TERRAIN_WIDTH-1;
  if (sy<0) sy=0;
  if (sy>MAX_TERRAIN_HEIGHT) sy=MAX_TERRAIN_HEIGHT-1;
  
  int dx=sx,dy=sy;//,ex,ey;
  //double half;
  
  switch (dir)
  {
  case PathNorth:
    dx = sx;
    dy = sy-length;
    break;
  case PathNE:
    dx = sx+length;
    dy = sy-length;
    break;
  case PathEast:
    dx = sx+length;
    dy = sy;
    break;
  case PathSE:
    dx = sx+length;
    dy = sy+length;
    break;
  case PathSouth:
    dx = sx;
    dy = sy+length;
    break;
  case PathSW:
    dx = sx-length;
    dy = sy+length;
    break;
  case PathWest:
    dx = sx-length;
    dy = sy;
    break;
  case PathNW:
    dx = sx-length;
    dy = sy-length;
    break;
  }
  
  if (dx<0) dx=0;
  if (dx>MAX_TERRAIN_WIDTH) dx=MAX_TERRAIN_WIDTH-1;
  if (dy<0) dy=0;
  if (dy>MAX_TERRAIN_HEIGHT) dy=MAX_TERRAIN_HEIGHT-1;
  
  GetMapTilesInLine(list, sx,sy, dx,dy);

  return (list.GetCount());
}

//*****************************************************************************
//    NAME: ClearAnimateList
//
// PURPOSE: 
//
//*****************************************************************************
void ClearAnimateList()
{
  POSITION pos = AnimateList.GetHeadPosition();
  while (pos != NULL)
  {
    POSITION temp = pos;
    AnimateList.GetNext(pos);
    COMBAT_SPRITE_DATA *pSprite = AnimateList.GetAt(temp);
    AnimateList.RemoveAt(temp);    
    delete pSprite;
  }
  AnimateList.RemoveAll();
}

//*****************************************************************************
//    NAME: DisplayAnimateList
//
// PURPOSE: 
//
//*****************************************************************************
void DisplayAnimateList()
{
  POSITION pos = AnimateList.GetHeadPosition();
  while (pos != NULL)
  {    
    AnimateList.GetAt(pos)->Display();
    AnimateList.GetNext(pos);
  }
}

//*****************************************************************************
//    NAME: UpdateAnimateList
//
// PURPOSE: 
//
//*****************************************************************************
bool UpdateAnimateList()
{
  bool result = false;
  POSITION pos = AnimateList.GetHeadPosition();
  while (pos != NULL)
  {
    if (!(AnimateList.GetAt(pos))->IsSpriteDone())
    {
      if (AnimateList.GetAt(pos)->Update())
        result = true;
    }
    
    AnimateList.GetNext(pos);
  } 
  
  pos = AnimateList.GetHeadPosition();
  while (pos != NULL)
  {
    POSITION temp = pos;
    AnimateList.GetNext(pos);
    COMBAT_SPRITE_DATA *pSprite = AnimateList.GetAt(temp);
    if (pSprite->IsSpriteDone())
    {
      AnimateList.RemoveAt(temp);    
      delete pSprite;
      result = true;
    }
  }
  return result;
}

//*****************************************************************************
//    NAME: RemoveAnimationFromList
//
// PURPOSE: 
//
//*****************************************************************************
void RemoveAnimationFromList(int key)
{
  WriteDebugString("Trying to remove spell animation with active spell key %i\n", key);

  POSITION pos = AnimateList.GetHeadPosition();
  while (pos != NULL)
  {
    POSITION temp = pos;
    AnimateList.GetNext(pos);    
    COMBAT_SPRITE_DATA *pSprite = AnimateList.GetAt(temp);
    if (key == pSprite->activeSpellKey)
    {
      WriteDebugString("Removed spell animation with active spell key %i\n", key);
      AnimateList.RemoveAt(temp);    
      delete pSprite;
    }
  }
}