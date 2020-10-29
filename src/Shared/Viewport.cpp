 /******************************************************************************
* Filename: Viewport.cpp
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
#include "..\Shared\stdafx.h"

#ifdef UAFEDITOR
//#include "..\UAFWinEd\UAFWinEd.h"
#include "..\UAFWinEd\DlgPicture.h" // to allow config of mapping coords
#else
//#include "externs.h"
#include "..\UAFWin\Dungeon.h"
#include "..\UAFWin\Path.h"
#endif

#include "GlobalData.h"
#include "level.h"
#include "party.h"
#include "Graphics.h"
#include "viewport.h"
#include "FileParse.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

WORD getWallSlotFormat(int wallSlot);
WORD getDoorSlotFormat(int wallSlot);

extern int BgMaskSurf;
extern CString BgMask;
extern int useWallIndex;
extern int useDoorAndOverlayIndex;

int HorzBarTopX = 2;
int HorzBarTopY = 2;

int HorzBarBotX = 2;
int HorzBarBotY = 444;

int HorzBarMidX = 0;
int HorzBarMidY = 309;

int VertBarLeftX = 2;
int VertBarLeftY = 2;
int VertBarRightX = 626;
int VertBarRightY = 2;

int VertBarMidX = 258;
int VertBarMidY = 2;

int HorzBarCombatTopX = 2;
int HorzBarCombatTopY = 2;

int HorzBarCombatBotX = 2;
int HorzBarCombatBotY = 444;

int VertBarCombatLeftX = 2;
int VertBarCombatLeftY = 2;
int VertBarCombatRightX = 626;
int VertBarCombatRightY = 2;

int VertBarCombatMidX = 258;
int VertBarCombatMidY = 2;



int VPFrameX = 34;
int VPFrameY = 40;
RECT ViewportFrameSrc = { 40,36,244,281 };

int VPFrameXOffset = 417;
int VPFrameYOffset = -19;

// these get calculated on the fly,
// all are screen coords
int MapScreenStartX;
int MapScreenEndX;
int MapScreenStartY;
int MapScreenEndY;
int MapScreenWidth;
int MapScreenHeight;
int MapPartyX;
int MapPartyY;

// RECT = left, top, right+1, bottom+1
//      =  x1,   y1,  x2,       y2
RECT VertBarSrc      = {  1,  1,  13, 457 }; // Can be overridden by config.txt
RECT VertBarSrcShort = { 21,  1,  32, 323 }; // Can be overridden by config.txt
RECT VertBarSrc2      = {  -1,  -1,  -1, -1 }; // manikus 6-5-12

RECT HorzBarSrc      = {  0,  0, 636,  14 }; // Can be overridden by config.txt
RECT HorzBarSrc2     = { -1, -1,  -1,  -1 }; // Can be overridden by config.txt
RECT HorzBarSrc3     = { -1, -1,  -1,  -1 }; // Can be overridden by config.txt

RECT VertBarCombatSrc = { 1,  1,  13, 457 }; // Can be overridden by config.txt
RECT VertBarCombatSrcShort = { 21,  1,  32, 323 }; // Can be overridden by config.txt
RECT VertBarCombatSrc2 = { -1,  -1,  -1, -1 }; // manikus 6-5-12

RECT HorzBarCombatSrc = { 0,  0, 636,  14 }; // Can be overridden by config.txt
RECT HorzBarCombatSrc2 = { -1, -1,  -1,  -1 }; // Can be overridden by config.txt


// wall source rects
// these are defaults - used if not over-ridden in config.txt
// ----->>> Can be overridden by config.txt <<<-----
RECT A_WALL_RECT = {7,7,39,218};
RECT B_WALL_RECT = {47,7,79,218};
RECT C_WALL_RECT = {167,7,199,141};
RECT D_WALL_RECT = {247,7,279,141};
RECT E_WALL_RECT = {167,7,279,141};
RECT F_WALL_RECT = {87,7,119,141};
RECT G_WALL_RECT = {127,7,159,141};
RECT H_WALL_RECT = {167,150,215,207};
RECT I_WALL_RECT = {87,149,103,207};
RECT J_WALL_RECT = {223,150,239,168};
RECT K_WALL_RECT = {111,149,127,207};
RECT L_WALL_RECT = {167,150,183,207};
RECT M_WALL_RECT = {199,150,215,207};
RECT N_WALL_RECT = {102,7,118,141};
RECT O_WALL_RECT = {127,7,143,141};
RECT P_WALL_RECT = {132,2,148,21};

// gets calculated at runtime
int A_WIDTH = 0;
int B_WIDTH = 0;
int C_WIDTH = 0;
int D_WIDTH = 0;
int E_WIDTH = 0;
int F_WIDTH = 0;
int G_WIDTH = 0;
int H_WIDTH = 0;
int I_WIDTH = 0;
int J_WIDTH = 0;
int K_WIDTH = 0;
int L_WIDTH = 0;
int M_WIDTH = 0;
int N_WIDTH = 0;
int O_WIDTH = 0;
int P_WIDTH = 0;

// gets calculated at runtime
int A_HEIGHT = 0;
int B_HEIGHT = 0;
int C_HEIGHT = 0;
int D_HEIGHT = 0;
int E_HEIGHT = 0;
int F_HEIGHT = 0;
int G_HEIGHT = 0;
int H_HEIGHT = 0;
int I_HEIGHT = 0;
int J_HEIGHT = 0;
int K_HEIGHT = 0;
int L_HEIGHT = 0;
int M_HEIGHT = 0;
int N_HEIGHT = 0;
int O_HEIGHT = 0;
int P_HEIGHT = 0;


// ----->>> Can be overridden by config.txt <<<-----
point_type A_OFF = {-32,0};  //
point_type B_OFF = {112,0};  //
point_type C_OFF = {0,40};   //
point_type D_OFF = {0,40};   //
point_type E_OFF = {0,40};   //
point_type F_OFF = {-32,40}; //
point_type G_OFF = {48,40};  //
point_type H_OFF = {0,78};   //
point_type I_OFF = {0,78};   //
point_type J_OFF = {0,96};   //
point_type K_OFF = {32,78};  //
point_type L_OFF = {0,78};   //
point_type M_OFF = {0,78};   //
point_type N_OFF = {-16,40}; //
point_type O_OFF = {48,40};  //
point_type P_OFF = J_OFF;    //

int ViewportX = 48;  // these can change in editor based on window position and size,
int ViewportY = 57;  // but these are static in the game
int ViewportWidth = 176;
int ViewportHeight = 211;
int BackgroundX = 48;  // these can change in editor based on window position and size,
int BackgroundY = 57;  // but these are static in the game
int BackgroundWidth = 176;
int BackgroundHeight = 211;
//RECT ViewportBkgrnd = {0,0,176,211};
int PartyNames_nx = 275; //DLD 12/20/00
int PartyNames_ny = 18; // these allow changing the positions upon screen
int PartyNames_wx = 18; // of Party Names, Map Location, Big Pic, and version text
int PartyNames_wy = 18; //
int LocationText_x = 275; //
int LocationText_y = 280; //
int BigPic_w = 612;      //
int BigPic_h = 292;      //
int BigPic_x = 14;      //
int BigPic_y = 16;      //
int VersionText_x = 280; //
int VersionText_y = 301; //end add


SELECTION_PAGE_FORMAT Select_Race_Format;
SELECTION_PAGE_FORMAT Select_Choosestats_Format;
SELECTION_PAGE_FORMAT Select_Class_Format;
SELECTION_PAGE_FORMAT Select_Alignment_Format;
SELECTION_PAGE_FORMAT Select_Gender_Format;
SELECTION_PAGE_FORMAT Select_Main_Function_Format;
SELECTION_PAGE_FORMAT Select_Add_Character_Format;


// This stuff is used to draw the Area View map
//
// RECT = left, top, right+1, bottom+1

// older coords used for compatibility with
// previous AreaViewArt image file.
BOOL UseOldAVStyle = FALSE;

// only the newer area view art
// files have the zone colors
//BOOL UseAVZoneStyle = FALSE;

RECT AV_HO_WallRect = {1,1,7,4}; 
RECT AV_HO_WallSize = {0,0,6,3}; 

RECT AV_VO_WallRect = {1,1,4,7};
RECT AV_VO_WallSize = {0,0,3,6};

RECT AV_HN1_WallRect = {1,1,6,9}; 
RECT AV_HN2_WallRect = {19,1,24,9}; 
RECT AV_HS1_WallRect = {36,1,41,9}; 
RECT AV_HS2_WallRect = {54,1,59,9}; 
RECT AV_H_WallSize = {0,0,5,8}; 

RECT AV_HN_DoorRect = {10,1,16,9};
RECT AV_HS_DoorRect = {45,1,51,9};
RECT AV_H_DoorSize = {0,0,6,8};

RECT AV_VE1_WallRect = {44,12,52,17};
RECT AV_VE2_WallRect = {66,12,74,17};
RECT AV_VW1_WallRect = {1,12,9,17};
RECT AV_VW2_WallRect = {23,12,31,17};
RECT AV_V_WallSize = {0,0,8,5};

RECT AV_VE_DoorRect = {55,12,63,18};
RECT AV_VW_DoorRect = {12,12,20,18};
RECT AV_V_DoorSize = {0,0,8,6};

RECT ArrowSize = {0,0,8,8};      // Can be overridden by config.txt

RECT AV_PartyRect = {17,21,33,37};
RECT AV_PartySize = {0,0,16,16};

// how many area view squares to draw inside viewport frame
//int AV_WIDTH = 11;
//int AV_HEIGHT = 12;

// Arrow source rects
// Match order of FACE_NORTH, etc in externs.h
// ---->>> Can be overridden by config.txt <<<-----
RECT ArrowRects[4] = 
{
  {0,21,8,29},  // north
  {8,21,16,29}, // east
  {8,29,16,37}, // south
  {0,29,8,37}   // west
};

// For each destination block within each square,
// add these offsets to the top,left of 
// the square getting blitted
//
// only left,top (first two) are used
RECT AV_WallOffsetRects[17] = {
  // North
  {0,0,0,0},  // 0 - wall
  {5,0,0,0},  // 1 - door
  {11,0,0,0}, // 2 - wall
  {0,0,0,0},  // 3 - not used

  // South
  {0,8,0,0},  // 4
  {5,8,0,0},  // 5
  {11,8,0,0}, // 6
  {0,0,0,0},   // 7 - not used

  // East
  {8,0,0,0},  // 8
  {8,5,0,0},  // 9
  {8,11,0,0}, // 10
  {0,0,0,0},   // 11 - not used

  //West
  {0,0,0,0}, // 12
  {0,5,0,0}, // 13
  {0,11,0,0}, // 14
  {0,0,0,0}, // 15 - not used

  // Center
  {4,4,0,0} // 16 - arrows
};

RECT AV_WallOffsetRects_Old[17] = {
  // North
  {0,0,0,0},  // 0 - wall
  {5,0,0,0},  // 1 - door
  {10,0,0,0}, // 2 - wall
  {0,0,0,0},  // 3 - not used

  // South
  {0,13,0,0},  // 4
  {5,13,0,0},  // 5
  {10,13,0,0}, // 6
  {0,0,0,0},   // 7 - not used

  // East
  {13,0,0,0},  // 8
  {13,5,0,0},  // 9
  {13,10,0,0}, // 10
  {0,0,0,0},   // 11 - not used

  //West
  {0,0,0,0}, // 12
  {0,5,0,0}, // 13
  {0,10,0,0}, // 14
  {0,0,0,0}, // 15 - not used

  // Center
  {4,4,0,0} // 16 - arrows
};

// these zone rects are part of the
// area view art file, and can
// therefore also be part of the
// wall slot area view art
RECT AV_AreaViewZoneRects[16] = 
{
  {0,39,16,55},   // zone 1
  {18,39,34,55},  // zone 2
  {36,39,52,55},  // zone 3
  {54,39,70,55},  // zone 4
  {72,39,88,55},  // zone 5
  {0,57,16,73},   // zone 6
  {18,57,34,73},  // zone 7
  {36,57,52,73},  // zone 8
  {54,57,70,73},  // zone 9
  {72,57,88,73},  // zone 10
  {0,75,16,91},   // zone 11
  {18,75,34,91},  // zone 12
  {36,75,52,91},  // zone 13
  {54,75,70,91},  // zone 14
  {72,75,88,91},  // zone 15
  {0,93,16,109}   // zone 16
};

// these zone rects are part of the
// art that is included in the zone
// data for each level
RECT AV_ZoneRects[16] = 
{
  {0,1,16,17},   // zone 1
  {18,1,34,17},  // zone 2
  {36,1,52,17},  // zone 3
  {54,1,70,17},  // zone 4
  {72,1,88,17},  // zone 5
  {0,19,16,35},   // zone 6
  {18,19,34,35},  // zone 7
  {36,19,52,35},  // zone 8
  {54,19,70,35},  // zone 9
  {72,19,88,35},  // zone 10
  {0,37,16,53},   // zone 11
  {18,37,34,53},  // zone 12
  {36,37,52,53},  // zone 13
  {54,37,70,53},  // zone 14
  {72,37,88,53},  // zone 15
  {0,55,16,71}   // zone 16
};

//
// holds the coords for drawing tiles in the regular viewport,
// which is composed of 13 viewable squares - no matter which direction
// the player is facing.  These values are x,y offsets from the top,left 
// coord of the viewport
//
// The viewport map is numbered as:
//      0  2  4  3  1
//      5  7  9  8  6
//     15 10 12 11 16
//        13 XX 14
//
//  with the player standing at XX. This numbering
//  allows the tiles to be drawn in order of
//  outside to center, back to front.

point_type viewport_coords[15] = 
// ----->>> Can be overridden by config.txt <<<-----
{
   {   0 , 0  }, //  0
   { 144 , 0  }, //  1
   {  32 , 0  }, //  2
   { 112 , 0  }, //  3
   {  64 , 0  }, //  4

   {   0 , 0  }, //  5
   { 160 , 0  }, //  6
   {  16 , 0  }, //  7
   { 112 , 0  }, //  8
   {  64 , 0  }, //  9

   {   0 , 0  }, // 10
   { 144 , 0  }, // 11
   {  32 , 0  }, // 12

   {   0 , 0  }, // 13 - for 7 wall distant formats
   {   0 , 0  }  // 14
};

point_type viewMap[15];
int wallSubstitutionCache[MAX_WALLSETS];
int wallSubstitutionCacheCount;

WallFormatDataMgr WallFormatMgr;
//WallFormatDataMgr BackgroundFormatMgr;

//*****************************************************************************
//    NAME: InitRect
//
// PURPOSE: 
//
//*****************************************************************************
void InitRect(RECT &rect, int x1, int y1, int x2, int y2)
{
  rect.left = x1;
  rect.right = x2;
  rect.top = y1;
  rect.bottom = y2;
}

//*****************************************************************************
//    NAME: InitPoint
//
// PURPOSE: 
//
//*****************************************************************************
void InitPoint(point_type &pt, int x, int y)
{
  pt.x = x;
  pt.y = y;
}

//*****************************************************************************
//    NAME: ConfigureDefaultWallFormat
//
// PURPOSE: 
//
//*****************************************************************************
void ConfigureDefaultWallFormat()
{
  int x1,y1,x2,y2;
  WALL_FORMAT_TYPE data;

  // default format for backgrounds is empty
  //BackgroundFormatMgr.AddFormat(data); 

  if (ConfigFile.FindTokens("A_WALL_RECT", x1,y1,x2,y2))
    InitRect(A_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("B_WALL_RECT", x1,y1,x2,y2))
    InitRect(B_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("C_WALL_RECT", x1,y1,x2,y2))
    InitRect(C_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("D_WALL_RECT", x1,y1,x2,y2))
    InitRect(D_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("E_WALL_RECT", x1,y1,x2,y2))
    InitRect(E_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("F_WALL_RECT", x1,y1,x2,y2))
    InitRect(F_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("G_WALL_RECT", x1,y1,x2,y2))
    InitRect(G_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("H_WALL_RECT", x1,y1,x2,y2))
    InitRect(H_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("I_WALL_RECT", x1,y1,x2,y2))
    InitRect(I_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("J_WALL_RECT", x1,y1,x2,y2))
    InitRect(J_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("K_WALL_RECT", x1,y1,x2,y2))
    InitRect(K_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("L_WALL_RECT", x1,y1,x2,y2))
    InitRect(L_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("M_WALL_RECT", x1,y1,x2,y2))
    InitRect(M_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("N_WALL_RECT", x1,y1,x2,y2))
    InitRect(N_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("O_WALL_RECT", x1,y1,x2,y2))
    InitRect(O_WALL_RECT, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("P_WALL_RECT", x1,y1,x2,y2))
    InitRect(P_WALL_RECT, x1,y1,x2,y2 );

  if (ConfigFile.FindTokens("A_OFF", x1,y1))
    InitPoint(A_OFF, x1,y1);
  if (ConfigFile.FindTokens("B_OFF", x1,y1))
    InitPoint(B_OFF, x1,y1);
  if (ConfigFile.FindTokens("C_OFF", x1,y1))
    InitPoint(C_OFF, x1,y1);
  if (ConfigFile.FindTokens("D_OFF", x1,y1))
    InitPoint(D_OFF, x1,y1);
  if (ConfigFile.FindTokens("E_OFF", x1,y1))
    InitPoint(E_OFF, x1,y1);
  if (ConfigFile.FindTokens("F_OFF", x1,y1))
    InitPoint(F_OFF, x1,y1);
  if (ConfigFile.FindTokens("G_OFF", x1,y1))
    InitPoint(G_OFF, x1,y1);
  if (ConfigFile.FindTokens("H_OFF", x1,y1))
    InitPoint(H_OFF, x1,y1);
  if (ConfigFile.FindTokens("I_OFF", x1,y1))
    InitPoint(I_OFF, x1,y1);
  if (ConfigFile.FindTokens("J_OFF", x1,y1))
    InitPoint(J_OFF, x1,y1);
  if (ConfigFile.FindTokens("K_OFF", x1,y1))
    InitPoint(K_OFF, x1,y1);
  if (ConfigFile.FindTokens("L_OFF", x1,y1))
    InitPoint(L_OFF, x1,y1);
  if (ConfigFile.FindTokens("M_OFF", x1,y1))
    InitPoint(M_OFF, x1,y1);
  if (ConfigFile.FindTokens("N_OFF", x1,y1))
    InitPoint(N_OFF, x1,y1);
  if (ConfigFile.FindTokens("O_OFF", x1,y1))
    InitPoint(O_OFF, x1,y1);
  if (ConfigFile.FindTokens("P_OFF", x1,y1))
    InitPoint(P_OFF, x1,y1);

  // pre-calc wall dimensions
  A_WIDTH = A_WALL_RECT.right - A_WALL_RECT.left;
  B_WIDTH = B_WALL_RECT.right - B_WALL_RECT.left;
  C_WIDTH = C_WALL_RECT.right - C_WALL_RECT.left;
  D_WIDTH = D_WALL_RECT.right - D_WALL_RECT.left;
  E_WIDTH = E_WALL_RECT.right - E_WALL_RECT.left;
  F_WIDTH = F_WALL_RECT.right - F_WALL_RECT.left;
  G_WIDTH = G_WALL_RECT.right - G_WALL_RECT.left;
  H_WIDTH = H_WALL_RECT.right - H_WALL_RECT.left;
  I_WIDTH = I_WALL_RECT.right - I_WALL_RECT.left;
  J_WIDTH = J_WALL_RECT.right - J_WALL_RECT.left;
  K_WIDTH = K_WALL_RECT.right - K_WALL_RECT.left;
  L_WIDTH = L_WALL_RECT.right - L_WALL_RECT.left;
  M_WIDTH = M_WALL_RECT.right - M_WALL_RECT.left;
  N_WIDTH = N_WALL_RECT.right - N_WALL_RECT.left;
  O_WIDTH = O_WALL_RECT.right - O_WALL_RECT.left;
  P_WIDTH = P_WALL_RECT.right - P_WALL_RECT.left;

  A_HEIGHT = A_WALL_RECT.bottom - A_WALL_RECT.top;
  B_HEIGHT = B_WALL_RECT.bottom - B_WALL_RECT.top;
  C_HEIGHT = C_WALL_RECT.bottom - C_WALL_RECT.top;
  D_HEIGHT = D_WALL_RECT.bottom - D_WALL_RECT.top;
  E_HEIGHT = E_WALL_RECT.bottom - E_WALL_RECT.top;
  F_HEIGHT = F_WALL_RECT.bottom - F_WALL_RECT.top;
  G_HEIGHT = G_WALL_RECT.bottom - G_WALL_RECT.top;
  H_HEIGHT = H_WALL_RECT.bottom - H_WALL_RECT.top;
  I_HEIGHT = I_WALL_RECT.bottom - I_WALL_RECT.top;
  J_HEIGHT = J_WALL_RECT.bottom - J_WALL_RECT.top;
  K_HEIGHT = K_WALL_RECT.bottom - K_WALL_RECT.top;
  L_HEIGHT = L_WALL_RECT.bottom - L_WALL_RECT.top;
  M_HEIGHT = M_WALL_RECT.bottom - M_WALL_RECT.top;
  N_HEIGHT = N_WALL_RECT.bottom - N_WALL_RECT.top;
  O_HEIGHT = O_WALL_RECT.bottom - O_WALL_RECT.top;
  P_HEIGHT = P_WALL_RECT.bottom - P_WALL_RECT.top;

  data.m_Rects[0] = A_WALL_RECT;
  data.m_Rects[1] = B_WALL_RECT;
  data.m_Rects[2] = C_WALL_RECT;
  data.m_Rects[3] = D_WALL_RECT;
  data.m_Rects[4] = E_WALL_RECT;
  data.m_Rects[5] = F_WALL_RECT;
  data.m_Rects[6] = G_WALL_RECT;
  data.m_Rects[7] = H_WALL_RECT;
  data.m_Rects[8] = I_WALL_RECT;
  data.m_Rects[9] = J_WALL_RECT;
  data.m_Rects[10] = K_WALL_RECT;
  data.m_Rects[11] = L_WALL_RECT;
  data.m_Rects[12] = M_WALL_RECT;
  data.m_Rects[13] = N_WALL_RECT;
  data.m_Rects[14] = O_WALL_RECT;
  data.m_Rects[15] = P_WALL_RECT;

  data.m_Offsets[0] = A_OFF;
  data.m_Offsets[1] = B_OFF;
  data.m_Offsets[2] = C_OFF;
  data.m_Offsets[3] = D_OFF;
  data.m_Offsets[4] = E_OFF;
  data.m_Offsets[5] = F_OFF;
  data.m_Offsets[6] = G_OFF;
  data.m_Offsets[7] = H_OFF;
  data.m_Offsets[8] = I_OFF;
  data.m_Offsets[9] = J_OFF;
  data.m_Offsets[10] = K_OFF;
  data.m_Offsets[11] = L_OFF;
  data.m_Offsets[12] = M_OFF;
  data.m_Offsets[13] = N_OFF;
  data.m_Offsets[14] = O_OFF;
  data.m_Offsets[15] = P_OFF;

  data.DistantWallCount = 5;
  for (int i=0;i<15;i++)
    data.viewport_coords[i] = viewport_coords[i];

  WallFormatMgr.AddFormat(data);
}

//*****************************************************************************
//    NAME: ConfigureOptionalWallFormats
//
// PURPOSE: 
//
//*****************************************************************************
void ConfigureOptionalWallFormats()
{
  int x1,y1,x2,y2;
  WALL_FORMAT_TYPE data;
  CString token;
  // the defaults should be loaded first
  ASSERT( WallFormatMgr.GetCount() >= 1 );

  WORD count;
  if (!ConfigFile.FindToken("MAX_ALTERNATE_WALL_FORMATS", count))
    return;
  
  for (int f=0;f<count;f++)
  {
    BOOL success = TRUE;
    data.Clear();

    token.Format("WIDTH_WALL_FORMAT_%i", f+1);
    if (!ConfigFile.FindToken(token, data.ImageWidth))
      success = FALSE;

    token.Format("HEIGHT_WALL_FORMAT_%i", f+1);
    if (!ConfigFile.FindToken(token, data.ImageHeight))
      success = FALSE;

    token.Format("NUM_DISTANT_WALLS_%i", f+1);
    if (!ConfigFile.FindToken(token, data.DistantWallCount))
      data.DistantWallCount = 5; // default

    for (int t=0;t<WALL_FORMAT_TYPE::MAX_SLOT_TYPES && success;t++)
    {
      token.Format("%c%i_WALL_RECT", 'A'+t, f+1);
      if (ConfigFile.FindTokens(token, x1,y1,x2,y2))
        InitRect(data.m_Rects[t], x1,y1,x2,y2 );
      else
        success = FALSE;

      token.Format("%c%i_OFF", 'A'+t, f+1);
      if (ConfigFile.FindTokens(token, x1,y1))
        InitPoint(data.m_Offsets[t], x1,y1);        
      else
        success = FALSE;
    }

    BOOL vc_success = success;
    int max_vc = (data.DistantWallCount == 7) ? 15 : 13;
    for (int vc=0;vc<max_vc && vc_success;vc++)
    {
      token.Format("VIEWPORT_COORD_%i_%i", vc, f+1);
      if (ConfigFile.FindTokens(token, x1,y1))
        InitPoint(data.viewport_coords[vc], x1, y1 );
      else
        vc_success = FALSE;
    }

    if (success && !vc_success)
    {
      // use defaults
      for (int i=0;i<15;i++)
        data.viewport_coords[i] = viewport_coords[i];
    }

    if (success)
    {
      WallFormatMgr.AddFormat(data);
      //BackgroundFormatMgr.AddFormat(data); 
    }
    else
    {
      CString tmp;
      tmp.Format("Wall format %i is incomplete or invalid in %s\n", 
        f+1, ConfigFile.GetFilename());
      MsgBoxInfo(tmp, "Config File Warning");
    }
  }

  WriteDebugString("Total optional wall formats: %i\n", WallFormatMgr.GetCount());
}

//*****************************************************************************
//    NAME: ConfigureWallFormats
//
// PURPOSE: 
//
//*****************************************************************************
void ConfigureWallFormats()
{
  WallFormatMgr.Clear();
  //BackgroundFormatMgr.Clear();
  ConfigureDefaultWallFormat();
  ConfigureOptionalWallFormats();
}

//*****************************************************************************
//    NAME: ConfigureArtCoords
//
// PURPOSE: 
//
//*****************************************************************************
void ConfigureArtCoords()
{
  int x1,y1,x2,y2;

#ifdef UAFEDITOR
  if (ConfigFile.FindTokens("NORTH_OFFSET_1", x1,y1,x2,y2))
    InitRect(WallOffsetRects[0], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("NORTH_OFFSET_2", x1,y1,x2,y2))
    InitRect(WallOffsetRects[1], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("NORTH_OFFSET_3", x1,y1,x2,y2))
    InitRect(WallOffsetRects[2], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("NORTH_OFFSET_4", x1,y1,x2,y2))
    InitRect(WallOffsetRects[3], x1,y1,x2,y2 );
  
  if (ConfigFile.FindTokens("SOUTH_OFFSET_1", x1,y1,x2,y2))
    InitRect(WallOffsetRects[4], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("SOUTH_OFFSET_2", x1,y1,x2,y2))
    InitRect(WallOffsetRects[5], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("SOUTH_OFFSET_3", x1,y1,x2,y2))
    InitRect(WallOffsetRects[6], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("SOUTH_OFFSET_4", x1,y1,x2,y2))
    InitRect(WallOffsetRects[7], x1,y1,x2,y2 );

  if (ConfigFile.FindTokens("EAST_OFFSET_1", x1,y1,x2,y2))
    InitRect(WallOffsetRects[8], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("EAST_OFFSET_2", x1,y1,x2,y2))
    InitRect(WallOffsetRects[9], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("EAST_OFFSET_3", x1,y1,x2,y2))
    InitRect(WallOffsetRects[10], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("EAST_OFFSET_4", x1,y1,x2,y2))
    InitRect(WallOffsetRects[11], x1,y1,x2,y2 );

  if (ConfigFile.FindTokens("WEST_OFFSET_1", x1,y1,x2,y2))
    InitRect(WallOffsetRects[12], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("WEST_OFFSET_2", x1,y1,x2,y2))
    InitRect(WallOffsetRects[13], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("WEST_OFFSET_3", x1,y1,x2,y2))
    InitRect(WallOffsetRects[14], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("WEST_OFFSET_4", x1,y1,x2,y2))
    InitRect(WallOffsetRects[15], x1,y1,x2,y2 );

  if (ConfigFile.FindTokens("CENTER_OFFSET", x1,y1,x2,y2))
    InitRect(WallOffsetRects[16], x1,y1,x2,y2 );

  if (ConfigFile.FindTokens("MAP_OUTER_SQUARE_SIZE", x1,y1,x2,y2))
    InitRect(MapSquareSize, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("MAP_INNER_SQUARE_SIZE", x1,y1,x2,y2))
    InitRect(MapBlockSize, x1,y1,x2,y2 );

  if (ConfigFile.FindTokens("HORZ_SIZE", x1,y1,x2,y2))
    InitRect(H_WallSize, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("HORZOFFSET_SIZE", x1,y1,x2,y2))
    InitRect(HO_WallSize, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("VERT_SIZE", x1,y1,x2,y2))
    InitRect(V_WallSize, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("VERTOFFSET_SIZE", x1,y1,x2,y2))
    InitRect(VO_WallSize, x1,y1,x2,y2 );

  if (ConfigFile.FindTokens("ARROW_SIZE", x1,y1,x2,y2))
    InitRect(ArrowSize, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("ARROW_NORTH", x1,y1,x2,y2))
    InitRect(ArrowRects[0], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("ARROW_EAST", x1,y1,x2,y2))
    InitRect(ArrowRects[1], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("ARROW_SOUTH", x1,y1,x2,y2))
    InitRect(ArrowRects[2], x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("ARROW_WEST", x1,y1,x2,y2))
    InitRect(ArrowRects[3], x1,y1,x2,y2 );

#endif

  if (ConfigFile.FindTokens("VERT_BAR_LONG", x1,y1,x2,y2))
    InitRect(VertBarSrc, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("VERT_BAR_SHORT", x1,y1,x2,y2))
    InitRect(VertBarSrcShort, x1,y1,x2,y2);
  if (ConfigFile.FindTokens("VERT_BAR_LONG_2", x1,y1,x2,y2))	// manikus 6-5-12
    InitRect(VertBarSrc2, x1,y1,x2,y2 );	//
  if (ConfigFile.FindTokens("HORZ_BAR_LONG", x1,y1,x2,y2))
    InitRect(HorzBarSrc, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("HORZ_BAR_LONG_2", x1,y1,x2,y2))
    InitRect(HorzBarSrc2, x1,y1,x2,y2 );
  if (ConfigFile.FindTokens("HORZ_BAR_LONG_3", x1,y1,x2,y2))
    InitRect(HorzBarSrc3, x1,y1,x2,y2 );

  if (ConfigFile.FindTokens("VERT_BAR_COMBAT_LONG", x1, y1, x2, y2))
    InitRect(VertBarCombatSrc, x1, y1, x2, y2);
  if (ConfigFile.FindTokens("VERT_BAR_COMBAT_SHORT", x1, y1, x2, y2))
    InitRect(VertBarCombatSrcShort, x1, y1, x2, y2);
  if (ConfigFile.FindTokens("VERT_BAR_COMBAT_LONG_2", x1, y1, x2, y2))	// manikus 6-5-12
    InitRect(VertBarCombatSrc2, x1, y1, x2, y2);	//
  if (ConfigFile.FindTokens("HORZ_BAR_COMBAT_LONG", x1, y1, x2, y2))
    InitRect(HorzBarCombatSrc, x1, y1, x2, y2);
  if (ConfigFile.FindTokens("HORZ_BAR_COMBAT_LONG_2", x1, y1, x2, y2))
    InitRect(HorzBarCombatSrc2, x1, y1, x2, y2);

  if (ConfigFile.FindTokens("VIEWPORT_RECT", x1,y1,x2,y2))
  {
    ViewportX = x1;
    ViewportY = y1;
    ViewportWidth = x2-x1;
    ViewportHeight = y2-y1;
  }

#ifdef UAFEDITOR
  BackgroundX = 0;
  BackgroundY = 0;
  BackgroundWidth = ViewportWidth;
  BackgroundHeight = ViewportHeight;
#else
  // Manikus tells me this should be ignored in the editor 20170107 PRS
  BackgroundX = ViewportX;
  BackgroundY = ViewportY;
  BackgroundWidth = ViewportWidth;
  BackgroundHeight = ViewportHeight;
  if (ConfigFile.FindTokens("BACKGROUND_RECT", x1,y1,x2,y2))
  {
    BackgroundX = x1;
    BackgroundY = y1;
    BackgroundWidth = x2-x1;
    BackgroundHeight = y2-y1;
  }
#endif


  if (ConfigFile.FindTokens("VIEWPORT_COORD_0", x1,y1))
    InitPoint(viewport_coords[0], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_1", x1,y1))
    InitPoint(viewport_coords[1], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_2", x1,y1))
    InitPoint(viewport_coords[2], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_3", x1,y1))
    InitPoint(viewport_coords[3], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_4", x1,y1))
    InitPoint(viewport_coords[4], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_5", x1,y1))
    InitPoint(viewport_coords[5], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_6", x1,y1))
    InitPoint(viewport_coords[6], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_7", x1,y1))
    InitPoint(viewport_coords[7], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_8", x1,y1))
    InitPoint(viewport_coords[8], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_9", x1,y1))
    InitPoint(viewport_coords[9], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_10", x1,y1))
    InitPoint(viewport_coords[10], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_11", x1,y1))
    InitPoint(viewport_coords[11], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_12", x1,y1))
    InitPoint(viewport_coords[12], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_13", x1,y1))
    InitPoint(viewport_coords[13], x1, y1 );
  if (ConfigFile.FindTokens("VIEWPORT_COORD_14", x1,y1))
    InitPoint(viewport_coords[14], x1, y1 );

  if (ConfigFile.FindTokens("VERT_BAR_MIDDLE", x1,y1))
  {
    VertBarMidX = x1; VertBarMidY = y1;
  }

  if (ConfigFile.FindTokens("HORZ_BAR_MIDDLE", x1,y1))
  {
    HorzBarMidX = x1; HorzBarMidY = y1;
  }

  if (ConfigFile.FindTokens("HORZ_BAR_TOP", x1,y1))
  {
    HorzBarTopX = x1; HorzBarTopY = y1;
  }
  if (ConfigFile.FindTokens("HORZ_BAR_BOTTOM", x1,y1))
  {
    HorzBarBotX = x1; HorzBarBotY = y1;
  }
  if (ConfigFile.FindTokens("VERT_BAR_LEFT", x1,y1))
  {
    VertBarLeftX = x1; VertBarLeftY = y1;
  }
  if (ConfigFile.FindTokens("VERT_BAR_RIGHT", x1,y1))
  {
    VertBarRightX = x1; VertBarRightY = y1;
  }

  if (ConfigFile.FindTokens("VERT_BAR_COMBAT_MIDDLE", x1, y1))
  {
    VertBarCombatMidX = x1; VertBarCombatMidY = y1;
  }
  if (ConfigFile.FindTokens("HORZ_BAR_COMBAT_TOP", x1, y1))
  {
    HorzBarCombatTopX = x1; HorzBarCombatTopY = y1;
  }
  if (ConfigFile.FindTokens("HORZ_BAR_COMBAT_BOTTOM", x1, y1))
  {
    HorzBarCombatBotX = x1; HorzBarCombatBotY = y1;
  }
  if (ConfigFile.FindTokens("VERT_BAR_COMBAT_LEFT", x1, y1))
  {
    VertBarCombatLeftX = x1; VertBarCombatLeftY = y1;
  }
  if (ConfigFile.FindTokens("VERT_BAR_COMBAT_RIGHT", x1, y1))
  {
    VertBarCombatRightX = x1; VertBarCombatRightY = y1;
  }


  if (ConfigFile.FindTokens("VIEWPORT_FRAME", x1,y1))
  {
    VPFrameX = x1, VPFrameY = y1;
  }

  if (ConfigFile.FindTokens("VIEWPORT_FRAME_OFFSET", x1,y1))
  {
    VPFrameXOffset = x1, VPFrameYOffset = y1;
  }

  if (ConfigFile.FindTokens("VIEWPORT_FRAME_SRC", x1,y1,x2,y2))
  {
    InitRect(ViewportFrameSrc, x1,y1,x2,y2);
  }

  //InitRect(ViewportBkgrnd, 0,0,BackgroundWidth,BackgroundHeight );

  ConfigureWallFormats();

  //AV_WIDTH = (ViewportFrameSrc.right-ViewportFrameSrc.left) / SQUARESIZE;//(AV_PartySize.right-AV_PartySize.left);
  //AV_HEIGHT = (ViewportFrameSrc.bottom-ViewportFrameSrc.top) / SQUARESIZE;//(AV_PartySize.bottom-AV_PartySize.top);
}


//*****************************************************************************
//    NAME: validCoords
//
// PURPOSE: 
//
//*****************************************************************************
inline BOOL validCoords(int y, int x)
{
   return (   (x >= 0) && (x < levelData.area_width)
           && (y >= 0) && (y < levelData.area_height));
}


int LookupWallslotSubstitution(int origWallslot)
{
  CString wallScriptName, result;
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  wallScriptName.Format("Wall%04d", origWallslot+1);
  result = RunGlobalScript("WallSubstitutions", wallScriptName, true);
  if (result.IsEmpty()) return origWallslot;
  origWallslot = atoi(result);
  if (origWallslot > 0) origWallslot--;
  return origWallslot;
}

int LookupBackdropSlotSubstitution(int origBackdropSlot)
{
  CString backdropScriptName, result;
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  backdropScriptName.Format("Backdrop%04d", origBackdropSlot+1);
  result = RunGlobalScript("BackdropSubstitutions", backdropScriptName, true);
  if (result.IsEmpty()) return origBackdropSlot;
  origBackdropSlot = atoi(result);
  if (origBackdropSlot > 0) origBackdropSlot--;
  return origBackdropSlot;
}


//*****************************************************************************
//    NAME: getWallSurface
//
// PURPOSE: 
//
//*****************************************************************************
long getWallSurface(int facing, int i, int *wallSlot, WORD *format) // viewmap index
{
   long wallSurface;
   static char TempFacingText[4]={'N','E','S','W'}; // Globaltext.cpp not part of UAFWin
//   int wallSlot;
   static char temp[30];
   char dirChar;
   if (format != NULL)
   {
     *format = DEFAULT_FORMAT_TYPE;
   };
   if ((i<0) || (i>14))  return -1;
   int x = viewMap[i].x;
   int y = viewMap[i].y;   
   if (!validCoords(y, x))  return -1;

   *wallSlot=levelData.area[y][x].walls(facing);

   if (*wallSlot >= MAX_WALLSETS)
   {     
     if (!debugStrings.AlreadyNoted("BWSNgWS"))
     {
       debugSeverity = 5;
       WriteDebugString("Bogus wall slot num %u in getWallSurface()\n", *wallSlot+1);
       WriteDebugString("level:%d; x:%d; y:%d; facing:%c\n",
                         globalData.currLevel+1, x, y, TempFacingText[facing]);
     };
     *wallSlot = 0;
   }
   dirChar=TempFacingText[facing];


    //
    // Search for temporary run-assigned wall slot
    //
/*
	 sprintf(temp,"$Wall,%d,%d,%c",x,y,dirChar);
   A_ASLENTRY_L *pAsl = &globalData.levelInfo.stats[globalData.currLevel].level_asl;
   if (pAsl!=NULL)
   {
     const ASLENTRY *pEntry;
	   pEntry = pAsl->Find(temp);
     if (pEntry != NULL)
	   {
		   CString wall;
		   int slot;
		   wall = *pEntry->pValue();
		   if (sscanf(wall,"%d",&slot)==1) 
		   {
			   if (slot<MAX_WALLSETS) wallSlot=slot;
		   }
	   }
   }
*/
    {
      int tempWallSlot;
      tempWallSlot = globalData.levelInfo.stats[globalData.currLevel].GetMapOverride(WALL_OVERRIDE_INDEX,x,y,facing);
      if (tempWallSlot >= 0)
      {
        *wallSlot = tempWallSlot;
      };
    };
    if (*wallSlot==0) return -1;




  { // Call any wall substitution scripts that might exist.
    // We keep a cache (int wallSubstitutionCache[MAX_WALLSETS]) that is initialized
    // to have zero entries at the time viewMap is created.
    if (wallSubstitutionCacheCount <= *wallSlot)
    {
      for (i = wallSubstitutionCacheCount; i <= *wallSlot; i++)
      {
        wallSubstitutionCache[i] = -1; // Not yet decided if substitution exists
        wallSubstitutionCacheCount = i+1;
      };
    };
    if (wallSubstitutionCache[*wallSlot] == -1)
    { // We need to look for a possible substitution
      wallSubstitutionCache[*wallSlot] = LookupWallslotSubstitution(*wallSlot);
    };
    // A legal substitution exists --- perhaps the identity!
    *wallSlot = wallSubstitutionCache[*wallSlot];
  };




    wallSurface = WallSets[*wallSlot].GetWallSurface();
    if (format != NULL)
    {
      *format = getWallSlotFormat(*wallSlot);
    };
    return wallSurface;
}

//*****************************************************************************
//    NAME: getDoorSurface
//
// PURPOSE: 
//
//*****************************************************************************
long getDoorSurface(int facing, int i, int *wallSlot, WORD *format) // viewmap index
{
  long surface;
  static char dirChar[] = {'N','E','S','W'};
  if ((i<0) || (i>14))  return -1;
  if (!validCoords(viewMap[i].y,viewMap[i].x)) return -1;
  *wallSlot=levelData.area[viewMap[i].y][viewMap[i].x].walls(facing);
#ifdef UAFEngine
  // not displayed if hidden door
  if (levelData.haveSecretDoor(viewMap[i].x, viewMap[i].y, facing))
    *wallSlot=0;
#endif
  
    {
      int tempWallSlot;
      tempWallSlot = globalData.levelInfo.stats[globalData.currLevel].GetMapOverride(
                      DOOR_OVERRIDE_INDEX,
                      viewMap[i].x,
                      viewMap[i].y,
                      facing);
      if (tempWallSlot >= 0)
      {
        *wallSlot = tempWallSlot;
      };
    };
 
  if (*wallSlot >= MAX_WALLSETS)
  {
    if (!debugStrings.AlreadyNoted("BWSgDS"))
    {
      debugSeverity = 5;
      WriteDebugString("Bogus wall slot num %u in getDoorSurface()\n", *wallSlot+1);
      WriteDebugString("level:%d; x:%d; y:%d; facing:%c\n",
          globalData.currLevel+1, viewMap[i].x, viewMap[i].y, dirChar[facing]);
    }
    *wallSlot = 0;
  }
  //return  WallSets[wallSlot].doorSurface;
  if (*wallSlot==0) return -1;
  surface = WallSets[*wallSlot].GetDoorSurface();
  {
    RECT rect;
    int fmt;
    GraphicsMgr.GetSurfaceRect(surface, &rect);
    fmt = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
    *format = fmt;;
  };
  return surface;
}

//*****************************************************************************
//    NAME: getOverlaySurface
//
// PURPOSE: 
//
//*****************************************************************************
long getOverlaySurface(int facing, int i, int *wallSlot, WORD *format) // viewmap index
{
  int surface;
  static char dirChar[] = {'N', 'E', 'S', 'W'};
  if ((i<0) || (i>14))  return -1;
  if (!validCoords(viewMap[i].y,viewMap[i].x)) return -1;
  *wallSlot=levelData.area[viewMap[i].y][viewMap[i].x].walls(facing);
 
  
    {
      int tempWallSlot;
      tempWallSlot = globalData.levelInfo.stats[globalData.currLevel].GetMapOverride(
                      OVERLAY_OVERRIDE_INDEX,
                      viewMap[i].x,
                      viewMap[i].y,
                      facing);
      if (tempWallSlot >= 0)
      {
        *wallSlot = tempWallSlot;
      };
    };
  
  if (*wallSlot >= MAX_WALLSETS)
  {
     if (!debugStrings.AlreadyNoted("BWSNgOS"))
     {
       debugSeverity = 5;
       WriteDebugString("Bogus wall slot num %u in getOverlaySurface()\n", *wallSlot+1);
       WriteDebugString("level:%d; x:%d; y:%d; facing:%c\n",
           globalData.currLevel+1, viewMap[i].x, viewMap[i].y, dirChar[facing]);
     };
    *wallSlot = 0;
  }
  //return WallSets[wallSlot].overlaySurface;
  surface = WallSets[*wallSlot].GetOverlaySurface();
  if (*wallSlot==0) return -1;
  {
    RECT rect;
    int fmt;
    GraphicsMgr.GetSurfaceRect(surface, &rect);
    fmt = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
    *format = fmt;;
  };
  return surface;
}

/*
WORD getBackgroundSlotFormat(int facing, int i) // viewmap index
{
  if ((i<0) || (i>14))  return DEFAULT_FORMAT_TYPE;
  if (!validCoords(viewMap[i].y,viewMap[i].x)) return DEFAULT_FORMAT_TYPE;
  BYTE wallSlot=levelData.area[viewMap[i].y][viewMap[i].x].backgrounds(facing);
  if ((wallSlot<=0) || (wallSlot >= MAX_BACKGROUNDS)) return DEFAULT_FORMAT_TYPE;
  // slot type cannot be determined if the image is not loaded
  BackgroundSets[wallSlot].GetBackgroundSurface();
  return BackgroundSets[wallSlot].SlotType;
}
*/

//*****************************************************************************
//    NAME: getWallSlotFormat
//
// PURPOSE: 
//
//*****************************************************************************
WORD getWallSlotFormat(int wallSlot) // viewmap index
{
//  if ((i<0) || (i>14))  return DEFAULT_FORMAT_TYPE;
//  if (!validCoords(viewMap[i].y,viewMap[i].x)) return DEFAULT_FORMAT_TYPE;
//  BYTE wallSlot=levelData.area[viewMap[i].y][viewMap[i].x].walls(facing);
  if ((wallSlot<=0) || (wallSlot >= MAX_WALLSETS)) return DEFAULT_FORMAT_TYPE;
  // slot type cannot be determined if the image is not loaded
  if (WallSets[wallSlot].GetWallSurface() < 0)
    if (WallSets[wallSlot].GetDoorSurface() < 0)
      WallSets[wallSlot].GetOverlaySurface();
//  {
//    SurfaceType srcType;
//    long srcSurf = WallSets[wallSlot].GetWallSurface();
//    RECT rect;
//    int slotType;
//    GraphicsMgr.GetSurfaceRect(srcSurf, &rect);
//    slotType = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
//    return slotType;
//  };

  return WallSets[wallSlot].SlotType;
}


//*****************************************************************************
//    NAME: getDoorSlotFormat
//
// PURPOSE: 
//
//*****************************************************************************
WORD getDoorSlotFormat(int wallSlot) // viewmap index
{
//  if ((i<0) || (i>14))  return DEFAULT_FORMAT_TYPE;
//  if (!validCoords(viewMap[i].y,viewMap[i].x)) return DEFAULT_FORMAT_TYPE;
//  BYTE wallSlot=levelData.area[viewMap[i].y][viewMap[i].x].walls(facing);
  if ((wallSlot<=0) || (wallSlot >= MAX_WALLSETS)) return DEFAULT_FORMAT_TYPE;
  // slot type cannot be determined if the image is not loaded
  if (WallSets[wallSlot].GetWallSurface() < 0)
    if (WallSets[wallSlot].GetDoorSurface() < 0)
      WallSets[wallSlot].GetOverlaySurface();
//  {
//    SurfaceType srcType;
//    long srcSurf = WallSets[wallSlot].GetWallSurface();
//    RECT rect;
//    int slotType;
//    GraphicsMgr.GetSurfaceRect(srcSurf, &rect);
//    slotType = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
//    return slotType;
//  };

  return WallSets[wallSlot].SlotType;
}


//*****************************************************************************
//    NAME: RenderDoorBeforeOverlay
//
// PURPOSE: 
//
//*****************************************************************************
BOOL RenderDoorBeforeOverlay(int facing, int i)
{
  if ((i<0) || (i>14))  return FALSE;
  if (!validCoords(viewMap[i].y,viewMap[i].x)) return FALSE;
  BYTE wallSlot=levelData.area[viewMap[i].y][viewMap[i].x].walls(facing);
  if (wallSlot >= MAX_WALLSETS) return FALSE;
  return  WallSets[wallSlot].doorFirst;
}

//*****************************************************************************
//    NAME: RenderDistantBackgrounds
//
// PURPOSE: 
//
//*****************************************************************************
BOOL RenderDistantBackgrounds(int facing, int i) // viewmap index
{
  if ((i<0) || (i>14))  return FALSE;
  if (!validCoords(viewMap[i].y,viewMap[i].x)) return FALSE;
  return levelData.area[viewMap[i].y][viewMap[i].x].ShowDistantBG;
}

//*****************************************************************************
//    NAME: RenderDistantBackgroundsInBands
//
// PURPOSE: 
//
//*****************************************************************************
BOOL RenderDistantBackgroundsInBands(int facing, int i) // viewmap index
{
  if ((i<0) || (i>14))  return FALSE;
  if (!validCoords(viewMap[i].y,viewMap[i].x)) return FALSE;
  return levelData.area[viewMap[i].y][viewMap[i].x].DistantBGInBands;
}


//*****************************************************************************
//    NAME: getWallSlot
//
// PURPOSE: 
//
//*****************************************************************************
BYTE getWallSlot(int facing, int i)
{
  char dirChar[] = {'N', 'E', 'S', 'W'};
  if ((i<0) || (i>14))  return 0;
  if (!validCoords(viewMap[i].y,viewMap[i].x)) return 0;
  BYTE slot = levelData.area[viewMap[i].y][viewMap[i].x].walls(facing);
  if (slot >= MAX_WALLSETS)
  {
     if (!debugStrings.AlreadyNoted("BWSNgWSlot"))
     {
       debugSeverity = 5;
       WriteDebugString("Bogus wall slot num %u in getWallSlot()\n", slot+1);
       WriteDebugString("level:%d; x:%d; y:%d; facing:%c\n",
          globalData.currLevel+1, viewMap[i].x, viewMap[i].y, dirChar[facing]);
     };
    slot = 0;
  }
  return slot;
}

//*****************************************************************************
//    NAME: getBackgroundSlot
//
// PURPOSE: 
//
//*****************************************************************************
BYTE getBackgroundSlot(int facing, int i)
{
  if ((i<0) || (i>14))  return 0;
  if (!validCoords(viewMap[i].y,viewMap[i].x)) return 0;
  BYTE slot = levelData.area[viewMap[i].y][viewMap[i].x].backgrounds(facing);
  {
    int tempSlot;
    tempSlot = globalData.levelInfo.stats[globalData.currLevel].GetMapOverride(
                    BACKGROUND_OVERRIDE_INDEX,
                    viewMap[i].x,
                    viewMap[i].y,
                    facing);
    if (tempSlot >= 0)
    {
      slot = tempSlot;
    };
  };

  // Call any backdrop substitution scripts that might exist.
  // We need to look for a possible substitution
  slot = LookupBackdropSlotSubstitution(slot);




  if (slot >= MAX_BACKGROUNDS)
  {
    slot = 0;
    WriteDebugString("Bogus backdrop slot num %u in getBackgroundSlot()\n", slot);
  }
  return slot;
}

//*****************************************************************************
//    NAME: IsTimeForAltBackdrop
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IsTimeForAltBackdrop(int slot)
{
  ASSERT( (slot >= 0) && (slot < MAX_BACKGROUNDS) );
  if (!BackgroundSets[slot].useAltBackground) return FALSE;  
  // useBG = TRUE if current time falls within specified start and end times
  return (IsTriggerTimeInCurrentTime(BackgroundSets[slot].StartTime,
                                     BackgroundSets[slot].EndTime));
}

//*****************************************************************************
//    NAME: getBackgroundSurface
//
// PURPOSE: 
//
//*****************************************************************************
long getBackgroundSurface(int facing, int i) // viewmap index
{
  long surface=-1;
  if ((i<0) || (i>14))  return -1;
  if (!validCoords(viewMap[i].y,viewMap[i].x)) return -1;
  BYTE wallSlot = getBackgroundSlot(facing, i);

  if (IsTimeForAltBackdrop(wallSlot))
  {
    surface = BackgroundSets[wallSlot].GetBackgroundAltSurface();   
    // if no alt background, use default
    if (surface < 0)
      surface = BackgroundSets[wallSlot].GetBackgroundSurface();
  }
  else
     surface = BackgroundSets[wallSlot].GetBackgroundSurface();
  return surface;
}

// wall and door surfaces
//*****************************************************************************
//    NAME: BltSurface
//
// PURPOSE: 
//
//*****************************************************************************
void BltSurface(int x, int y, int type, long srcSurface, long dstSurface, WORD format)
{
  // dstSurface uses neg nbrs for primary and backbuffer surfaces
  if (srcSurface <= 0)
    return;

  RECT srcRect = WallFormatMgr.GetRect(format, type);
  point_type offset = WallFormatMgr.GetOffset(format, type);
  WORD width = WallFormatMgr.GetWidth(format, type);
  WORD height = WallFormatMgr.GetHeight(format, type);

  RECT dstRect;
  dstRect.left = x + offset.x;
  dstRect.top = y + offset.y;
  dstRect.right = dstRect.left + width;
  dstRect.bottom = dstRect.top + height;

  GraphicsMgr.BlitImage(dstSurface, srcSurface, &srcRect, &dstRect);  
}

// only overlay surface
//*****************************************************************************
//    NAME: BltOverlaySurface
//
// PURPOSE: 
//
//*****************************************************************************
void BltOverlaySurface(int x, int y, int type, long srcSurface, long dstSurface, WORD format, int slot)
{
  // dstSurface uses neg nbrs for primary and backbuffer surfaces
  if (srcSurface <= 0)
    return;

  RECT srcRect = WallFormatMgr.GetRect(format, type);
  point_type offset = WallFormatMgr.GetOffset(format, type);
  WORD width = WallFormatMgr.GetWidth(format, type);
  WORD height = WallFormatMgr.GetHeight(format, type);

  RECT dstRect;
  dstRect.left = x + offset.x;
  dstRect.top = y + offset.y;
  dstRect.right = dstRect.left + width;
  dstRect.bottom = dstRect.top + height;
  
  if ((WallSets[slot].BlendOverlay) && (slot != 0))
  {
    // no stretching when using alpha blend
    GraphicsMgr.BlitAlphaImage(WallSets[slot].BlendAmount, dstRect.left, dstRect.top, srcSurface, &srcRect, dstSurface);      
  }
  else
    GraphicsMgr.BlitImage(dstSurface, srcSurface, &srcRect, &dstRect);  
}

//*****************************************************************************
//    NAME: BltBackgroundSurface
//
// PURPOSE: 
//
//*****************************************************************************
void BltBackgroundSurface(long dstSurface, long srcSurface, RECT *pSrcRect, RECT *pDstRect, int slot)
{
  // dstSurface uses neg nbrs for primary and backbuffer surfaces
  if (srcSurface <= 0) return;

  // if in alt time period, srcSurface will be alt backdrop surface
  // else srcSurface will be regular backdrop surface
  //
  // if use alt AND use alpha blend
  // then 
  //    if in alt time period
  //    then
  //       draw regular backdrop
  //       if use transp
  //       then
  //         draw alpha blend with trans color
  //       else
  //         draw alpha blend
  //       end if
  //    else
  //       draw regular backdrop
  //    end if
  // else if use alpha blend
  // then
  //    draw regular backdrop
  //    if use transp
  //    then
  //       draw alpha blend with trans color
  //    else
  //       draw alpha blend
  //    end if
  // else if use alt (time only)  <--- this one is taken care of by last 'else'
  // then                              since srcSurface is alt backdrop during
  //    if in alt time period          alt time period.
  //    then
  //      if use transp
  //      then
  //        draw alt backdrop using trans color
  //      else
  //        draw alt backdrop
  //      end if
  //    end if
  // else
  //    draw regular backdrop
  // end if
  //    

  long altsurf=-1;
  if (BackgroundSets[slot].useAltBackground)
    altsurf = BackgroundSets[slot].GetBackgroundAltSurface();

  if ((BackgroundSets[slot].useAlphaBlend) && (BackgroundSets[slot].useAltBackground))
  {
    if (IsTimeForAltBackdrop(slot))
    {
      // blit regular backdrop
      GraphicsMgr.BlitImage(dstSurface, BackgroundSets[slot].GetBackgroundSurface(), pSrcRect, pDstRect);  
      // now blit alternate backdrop using alpha-blend
      // no stretching when using alpha
      if (BackgroundSets[slot].useTransparency)
        GraphicsMgr.BlitAlphaTransImage(BackgroundSets[slot].AlphaBlendPcnt, pDstRect->left, pDstRect->top, srcSurface, pSrcRect, dstSurface);      
      else
        GraphicsMgr.BlitAlphaImage(BackgroundSets[slot].AlphaBlendPcnt, pDstRect->left, pDstRect->top, srcSurface, pSrcRect, dstSurface);      
    }
    else
      GraphicsMgr.BlitImage(dstSurface, srcSurface, pSrcRect, pDstRect);  
  }
  else if ((BackgroundSets[slot].useAlphaBlend) && (altsurf >= 0) && (slot != 0))
  {
    // blit regular backdrop
    GraphicsMgr.BlitImage(dstSurface, srcSurface, pSrcRect, pDstRect);  
    // now blit alternate backdrop using alpha-blend
    // no stretching when using alpha
    if (BackgroundSets[slot].useTransparency)
      GraphicsMgr.BlitAlphaTransImage(BackgroundSets[slot].AlphaBlendPcnt, pDstRect->left, pDstRect->top, altsurf, pSrcRect, dstSurface);      
    else
      GraphicsMgr.BlitAlphaImage(BackgroundSets[slot].AlphaBlendPcnt, pDstRect->left, pDstRect->top, altsurf, pSrcRect, dstSurface);      
  }
  else if ((BackgroundSets[slot].useTransparency) && (altsurf >= 0) && (slot != 0))
  {
    // blit regular backdrop
    GraphicsMgr.BlitImage(dstSurface, srcSurface, pSrcRect, pDstRect);  
    // now blit alternate backdrop using trans color
    GraphicsMgr.BlitTransImage(dstSurface, altsurf, pSrcRect, pDstRect);
  }
  else
    GraphicsMgr.BlitImage(dstSurface, srcSurface, pSrcRect, pDstRect);  
}


//*****************************************************************************
//    NAME: BlockedForAreaView
//
// PURPOSE: 
//
//*****************************************************************************
BOOL BlockedForAreaView(BlockageType bt, int x, int y, BYTE facing)
{ 
  switch  (bt)
  {
  case BlockedBlk:
    return TRUE; // no door
    
  case FalseDoorBlk:
  case OpenBlk:
  case LockedBlk:
  case LockedKey1:
  case LockedKey2:
  case LockedKey3: 
  case LockedKey4:
  case LockedKey5: 
  case LockedKey6:
  case LockedKey7: 
  case LockedKey8:
  case LockedWizardBlk:
    return FALSE; // door always visible, or not present   

  case LockedSecretBlk:
  case LockedWizardSecretBlk:    
    // if secret door is not visible
    return (party.blockageData.IsSecret(GetCurrentLevel(),x,y,facing));
  }
  return TRUE; // invalid blockage
}



struct DRAW_AREA_WALL
{
  BOOL drawWall;
  BOOL drawBlockage;
};

//*****************************************************************************
//    NAME: GetWallASL
//
// PURPOSE: 
//
//*****************************************************************************
void GetWallASL(AREA_MAP_DATA *pSpot, int x, int y, int facing, DRAW_AREA_WALL *pDAW) 
{
	//static CString wallStr;
  static char dirChar[4]={'N','E','S','W'}; // Globaltext.cpp not part of UAFWin
  //static char temp[30];
	int wallSlot, tempOvr;	

  pDAW->drawBlockage = FALSE;
  pDAW->drawWall = FALSE;

  if (!validCoords(y, x))  return;
  wallSlot=levelData.area[y][x].walls(facing);
  if (wallSlot >= MAX_WALLSETS)
  {
     if (!debugStrings.AlreadyNoted("BWSNgWA"))
     {
       debugSeverity = 5;
       WriteDebugString("Bogus wall slot num %u in getWallASL()\n", wallSlot+1);
       WriteDebugString("level:%d; x:%d; y:%d; facing:%c\n",
                         globalData.currLevel+1, x, y, dirChar[facing]);
     };
   wallSlot = 0;
  }

 // char dir=TempFacingText[facing];


  //
  // Get temporary wall-slot override
  //
  /*
	sprintf(temp,"$Wall,%d,%d,%c",x,y,dir);
  A_ASLENTRY_L *pAsl = &globalData.levelInfo.stats[globalData.currLevel].level_asl;
  if (pAsl!=NULL)
  {
    const ASLENTRY *pEntry;
	  pEntry = pAsl->Find(temp);
    if (pEntry != NULL)
	  {
		  wallStr = *pEntry->pValue();
		  if (sscanf(wallStr,"%d",&wallSlot)!=1) return -1;
		  if (wallSlot>=MAX_WALLSETS) return -1;
		  return wallSlot;
	  }
  }
  */
  {
    int tempWallSlot;
    tempWallSlot = globalData.levelInfo.stats[globalData.currLevel].GetMapOverride(WALL_OVERRIDE_INDEX,x,y,facing);
    if (tempWallSlot >= 0)
    {
      wallSlot = tempWallSlot;
    };
  };
  if (wallSlot>=MAX_WALLSETS) wallSlot=0;
  if ((wallSlot != 0) && WallSets[wallSlot].drawAV)
  {
    pDAW->drawWall = TRUE;
    if (BlockedForAreaView(pSpot->blockages(facing),x,y,FACE_NORTH))
    {
      pDAW->drawBlockage = TRUE;
    };
  };
  // OK. That is all we need to know about the wall and the map.
  // Now we will check for any drawing/blockage overrides.
  tempOvr = globalData.levelInfo.stats[globalData.currLevel].GetMapOverride(BLOCKAGE_OVERRIDE,x,y,facing);
  if (tempOvr >= 0)
  {
    int overrideBlock, overrideDrawing;

    overrideBlock = tempOvr & 0x0f;
    switch (overrideBlock)
    {
    case BlockedBlk:
      pDAW->drawBlockage=TRUE; // no door
    
    case FalseDoorBlk:
    case OpenBlk:
    case LockedBlk:
    case LockedKey1:
    case LockedKey2:
    case LockedKey3: 
    case LockedKey4:
    case LockedKey5: 
    case LockedKey6:
    case LockedKey7: 
    case LockedKey8:
    case LockedWizardBlk:
      pDAW->drawBlockage=FALSE; // door always visible, or not present   

    case LockedSecretBlk:
    case LockedWizardSecretBlk:    
      // if secret door is not visible
      pDAW->drawBlockage = party.blockageData.IsSecret(GetCurrentLevel(),x,y,facing);
    default:
      pDAW->drawBlockage = TRUE; // invalid blockage
    };


    overrideDrawing = (tempOvr >> 4) & 0x07;
    switch (overrideDrawing)
    {
    case 0: break;
    case 1: pDAW->drawBlockage=FALSE; pDAW->drawWall=FALSE; break;
    case 2:                           pDAW->drawWall=TRUE;  break;
    case 3: pDAW->drawBlockage=FALSE;                       break;
    case 4: pDAW->drawBlockage=FALSE; pDAW->drawWall=TRUE;  break;
    case 5: pDAW->drawBlockage=TRUE;                        break;
    case 6: pDAW->drawBlockage=TRUE;  pDAW->drawWall=TRUE;  break;
    };
  };  
}

//*****************************************************************************
//    NAME: GetAVSurface
//
// PURPOSE: 
//
//*****************************************************************************
long GetAVSurface(int x, int y, int facing)
{
  static char dirChar[] = {'N','E','S','W'};
  if (!validCoords(y, x))  return globalData.MapArtSurf;
  int wallSlot=levelData.area[y][x].walls(facing);
  if (wallSlot >= MAX_WALLSETS)
  {
     if (!debugStrings.AlreadyNoted("BWSNgAVS"))
     {
       debugSeverity = 5;
       WriteDebugString("Bogus wall slot num %u in getAVSurface()\n", wallSlot+1);
       WriteDebugString("level:%d; x:%d; y:%d; facing:%c\n",
                         globalData.currLevel+1, x, y, dirChar[facing]);
     };
     wallSlot = 0;
  }

  if (wallSlot==0) return globalData.MapArtSurf;
  long surf = WallSets[wallSlot].GetAVArtSurface();
  if (surf > 0) return surf;
  else return globalData.MapArtSurf;
}

//*****************************************************************************
//    NAME: GetAVSurface
//
// PURPOSE: 
//
//*****************************************************************************
long GetAVSurface(int zone)
{
  return (levelData.zoneData.GetAreaViewArt());
}

//*****************************************************************************
//    NAME: GetAVUseZones
//
// PURPOSE: 
//
//*****************************************************************************
BOOL GetAVUseZones(int zone)
{
  return (levelData.zoneData.GetAreaViewArt() > 0);
}

//*****************************************************************************
//    NAME: GetAVUseZones
//
// PURPOSE: 
//
//*****************************************************************************
BOOL GetAVUseZones(int x, int y, BYTE &facing)
{
  static char dirChar[] = {'N','E','S','W'};
  if (!validCoords(y, x))
  {
    facing=party.facing;
    return globalData.UseAVZones;
  }
  int wallSlot=levelData.area[y][x].walls(facing);
  if (wallSlot >= MAX_WALLSETS)
  {
     if (!debugStrings.AlreadyNoted("BWSNgAVUZ"))
     {
       debugSeverity = 5;
       WriteDebugString("Bogus wall slot num %u in getAVUseZones()\n", wallSlot+1);
       WriteDebugString("level:%d; x:%d; y:%d; facing:%c\n",
                         globalData.currLevel+1, x, y, dirChar[facing]);
     };
     wallSlot = 0;
  }

  if (!WallSets[wallSlot].AVZoneStyle) wallSlot=0;

  if ((wallSlot >= MAX_WALLSETS)||(wallSlot<=0))
  {    
    wallSlot=levelData.area[y][x].walls(FACE_NORTH);
    if (!WallSets[wallSlot].AVZoneStyle) wallSlot=0;
    else facing = FACE_NORTH;
  }
  if ((wallSlot >= MAX_WALLSETS)||(wallSlot<=0))
  {
    wallSlot=levelData.area[y][x].walls(FACE_EAST);
    if (!WallSets[wallSlot].AVZoneStyle) wallSlot=0;
    else facing = FACE_EAST;
  }
  if ((wallSlot >= MAX_WALLSETS)||(wallSlot<=0))
  {
    wallSlot=levelData.area[y][x].walls(FACE_SOUTH);
    if (!WallSets[wallSlot].AVZoneStyle) wallSlot=0;
    else facing = FACE_SOUTH;
  }
  if ((wallSlot >= MAX_WALLSETS)||(wallSlot<=0))
  {
    wallSlot=levelData.area[y][x].walls(FACE_WEST);
    if (!WallSets[wallSlot].AVZoneStyle) wallSlot=0;
    else facing = FACE_WEST;
  }

  if ((wallSlot >= MAX_WALLSETS)||(wallSlot<=0))
  {
    facing=party.facing;
    return globalData.UseAVZones;
  }

  return WallSets[wallSlot].AVZoneStyle;
}


// i, j     correspond to the x,y indexes for levelData.area[y][x]. i=x,j=y
// x, y     are screen coords
// surface  is destination surf ptr
//*****************************************************************************
//    NAME: DrawAreaViewSquare
//
// PURPOSE: 
//
//*****************************************************************************
void DrawAreaViewSquare(int i, int j, 
						int x, int y, 
						AREA_MAP_DATA &spot, long int surface)
{
  RECT tempRect;
  DRAW_AREA_WALL daw;
  //int wall;
  
  //
  // Blit( dstSurf, srcSurf, srcRect, dstRect);
  //
//  for (int kkk=0; kkk<1000; kkk++)  
// This 'for' loop caused a delay in player movement of about 4 seconds which 
// says to me that this business of looking up attributes takes about
// 16 milliseconds for the entire map.  About 1/60 second.  I have a
// Pentium 166 and I compiled the debug version of the program.  
// If the delay on slower machines is a problem we can
// build a map each time a level is loaded to eliminate this delay.  I'll
// leave it this way for now.


  if (GetAVUseZones(spot.zone))
  {
    // use 1-16 squares of 16x16 from ZONE_DATA area view art
    long srcSurface = GetAVSurface(spot.zone);
    TranslateRect(x, y, AV_PartySize, tempRect);
    GraphicsMgr.BlitImage(surface, srcSurface, &AV_ZoneRects[spot.zone], &tempRect);
  }
  else
  {
    // since ZONE_DATA should always load default av art,
    // this won't be called unless it cant be loaded or the
    // designer removed it intentionally
    BYTE AVZoneFacing = party.facing;
    if (GetAVUseZones(i,j, AVZoneFacing))
    {
      // use 1-16 squares of 16x16 from WallSlotMemType area view art
      long srcSurface = GetAVSurface(i,j,AVZoneFacing);
      TranslateRect(x, y, AV_PartySize, tempRect);
      GraphicsMgr.BlitImage(surface, srcSurface, &AV_AreaViewZoneRects[spot.zone], &tempRect);
    }
  }
  
  GetWallASL(&spot, i, j, FACE_NORTH, &daw);
  //wall=GetWallASL(i,j,FACE_NORTH);
  //if (wall<0) wall=spot.northWall;
  //if (wall>0)
  if (daw.drawWall)
  {
    long srcSurface = GetAVSurface(i,j,FACE_NORTH);
    TranslateRect(x+AV_WallOffsetRects[0].left, y+AV_WallOffsetRects[0].top, AV_H_WallSize, tempRect);
    //                    dst       src                    src             dst
    GraphicsMgr.BlitImage(surface, srcSurface, &AV_HN1_WallRect, &tempRect);
    TranslateRect(x+AV_WallOffsetRects[2].left, y+AV_WallOffsetRects[2].top, AV_H_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, srcSurface, &AV_HN2_WallRect, &tempRect);

    //if (spot.northBlockage != OpenBlk)
    //if (BlockedForAreaView(spot.northBlockage,i,j,FACE_NORTH))
    if (daw.drawBlockage)
    {
      TranslateRect(x+AV_WallOffsetRects[1].left, y+AV_WallOffsetRects[1].top, AV_H_DoorSize, tempRect);
      GraphicsMgr.BlitImage(surface, srcSurface, &AV_HN_DoorRect, &tempRect);
    }
  }

  //wall=GetWallASL(i,j,FACE_SOUTH);
  GetWallASL(&spot, i,j,FACE_SOUTH, &daw);
  //if (wall<0) wall=spot.southWall;
  //if (wall>0)
  if (daw.drawWall)
  {
    long srcSurface = GetAVSurface(i,j,FACE_SOUTH);
    TranslateRect(x+AV_WallOffsetRects[4].left, y+AV_WallOffsetRects[4].top, AV_H_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, srcSurface, &AV_HS1_WallRect, &tempRect);
    TranslateRect(x+AV_WallOffsetRects[6].left, y+AV_WallOffsetRects[6].top, AV_H_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, srcSurface, &AV_HS2_WallRect, &tempRect);

    //if (spot.southBlockage != OpenBlk)
    //if (BlockedForAreaView(spot.southBlockage,i,j,FACE_SOUTH))
    if (daw.drawBlockage)
    {
      TranslateRect(x+AV_WallOffsetRects[5].left, y+AV_WallOffsetRects[5].top, AV_H_DoorSize, tempRect);
      GraphicsMgr.BlitImage(surface, srcSurface, &AV_HS_DoorRect, &tempRect);
    }
  }
  GetWallASL(&spot, i,j,FACE_EAST, &daw);
  //wall=GetWallASL(i,j,FACE_EAST);
  //if (wall<0) wall=spot.eastWall;
  //if (wall>0)
  if (daw.drawWall)
  {
    long srcSurface = GetAVSurface(i,j,FACE_EAST);
    TranslateRect(x+AV_WallOffsetRects[8].left, y+AV_WallOffsetRects[8].top, AV_V_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, srcSurface, &AV_VE1_WallRect, &tempRect);
    TranslateRect(x+AV_WallOffsetRects[10].left, y+AV_WallOffsetRects[10].top, AV_V_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, srcSurface, &AV_VE2_WallRect, &tempRect);

    //if (spot.eastBlockage != OpenBlk)
    //if (BlockedForAreaView(spot.eastBlockage,i,j,FACE_EAST))
    if (daw.drawBlockage)
    {
      TranslateRect(x+AV_WallOffsetRects[9].left, y+AV_WallOffsetRects[9].top, AV_V_DoorSize, tempRect);
      GraphicsMgr.BlitImage(surface, srcSurface, &AV_VE_DoorRect, &tempRect);
    }
  }
  GetWallASL(&spot,i,j,FACE_WEST,&daw);
  //wall=GetWallASL(i,j,FACE_WEST);
  //if (wall<0) wall=spot.westWall;
  //if (wall>0)
  if (daw.drawWall)
  {
    long srcSurface = GetAVSurface(i,j,FACE_WEST);
    TranslateRect(x+AV_WallOffsetRects[12].left, y+AV_WallOffsetRects[12].top, AV_V_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, srcSurface, &AV_VW1_WallRect, &tempRect);
    TranslateRect(x+AV_WallOffsetRects[14].left, y+AV_WallOffsetRects[14].top, AV_V_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, srcSurface, &AV_VW2_WallRect, &tempRect);

    //if (spot.westBlockage != OpenBlk)
    //if (BlockedForAreaView(spot.westBlockage,i,j,FACE_WEST))
    if (daw.drawBlockage)
    {
      TranslateRect(x+AV_WallOffsetRects[13].left, y+AV_WallOffsetRects[13].top, AV_V_DoorSize, tempRect);
      GraphicsMgr.BlitImage(surface, srcSurface, &AV_VW_DoorRect, &tempRect);
    }
  }
}

// i, j correspond to the x,y indexes for levelData.area[y][x]. i=x,j=y
// x, y are screen coords
//*****************************************************************************
//    NAME: DrawOldAreaViewSquare
//
// PURPOSE: 
//
//*****************************************************************************
void DrawOldAreaViewSquare(int i, int j, 
						int x, int y, 
						AREA_MAP_DATA &spot, long int surface)
{
  RECT tempRect;
  //int wall;
  DRAW_AREA_WALL daw;
  //
  // Blit( dstSurf, srcSurf, srcRect, dstRect);
  //
//  for (int kkk=0; kkk<1000; kkk++)  
// This 'for' loop caused a delay in player movement of about 4 seconds which 
// says to me that this business of looking up attributes takes about
// 16 milliseconds for the entire map.  About 1/60 second.  I have a
// Pentium 166 and I compiled the debug version of the program.  
// If the delay on slower machines is a problem we can
// build a map each time a level is loaded to eliminate this delay.  I'll
// leave it this way for now.

  GetWallASL(&spot, i,j,FACE_NORTH, &daw);
  //wall=GetWallASL(i,j,FACE_NORTH);
  //if (wall<0) wall=spot.northWall;
  //if (wall>0)
  if (daw.drawWall)
  {
    TranslateRect(x+AV_WallOffsetRects_Old[0].left, y+AV_WallOffsetRects_Old[0].top, AV_HO_WallSize, tempRect);
    //                    dst       src                    src             dst
    GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_HO_WallRect, &tempRect);
    TranslateRect(x+AV_WallOffsetRects_Old[2].left, y+AV_WallOffsetRects_Old[2].top, AV_HO_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_HO_WallRect, &tempRect);

    //if (spot.northBlockage != OpenBlk)
    //if (BlockedForAreaView(spot.northBlockage,i,j,FACE_NORTH))
    if (daw.drawBlockage)
    {
      TranslateRect(x+AV_WallOffsetRects_Old[1].left, y+AV_WallOffsetRects_Old[1].top, AV_HO_WallSize, tempRect);
      GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_HO_WallRect, &tempRect);
    }
  }

  GetWallASL(&spot, i,j,FACE_SOUTH, &daw);
  //wall=GetWallASL(i,j,FACE_SOUTH);
  //if (wall<0) wall=spot.southWall;
  //if (wall>0)
  if (daw.drawWall)
  {
    TranslateRect(x+AV_WallOffsetRects_Old[4].left, y+AV_WallOffsetRects_Old[4].top, AV_HO_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_HO_WallRect, &tempRect);
    TranslateRect(x+AV_WallOffsetRects_Old[6].left, y+AV_WallOffsetRects_Old[6].top, AV_HO_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_HO_WallRect, &tempRect);

    //if (spot.southBlockage != OpenBlk)
    //if (BlockedForAreaView(spot.southBlockage,i,j,FACE_SOUTH))
    if (daw.drawBlockage)
    {
      TranslateRect(x+AV_WallOffsetRects_Old[5].left, y+AV_WallOffsetRects_Old[5].top, AV_HO_WallSize, tempRect);
      GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_HO_WallRect, &tempRect);
    }
  }
  GetWallASL(&spot, i,j,FACE_EAST, &daw);
  //wall=GetWallASL(i,j,FACE_EAST);
  //if (wall<0) wall=spot.eastWall;
  //if (wall>0)
  if (daw.drawWall)
  {
    TranslateRect(x+AV_WallOffsetRects_Old[8].left, y+AV_WallOffsetRects_Old[8].top, AV_VO_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_VO_WallRect, &tempRect);
    TranslateRect(x+AV_WallOffsetRects_Old[10].left, y+AV_WallOffsetRects_Old[10].top, AV_VO_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_VO_WallRect, &tempRect);

    //if (spot.eastBlockage != OpenBlk)
    //if (BlockedForAreaView(spot.eastBlockage,i,j,FACE_EAST))
    if (daw.drawBlockage)
    {
      TranslateRect(x+AV_WallOffsetRects_Old[9].left, y+AV_WallOffsetRects_Old[9].top, AV_VO_WallSize, tempRect);
      GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_VO_WallRect, &tempRect);
    }
  }
  GetWallASL(&spot, i,j,FACE_WEST, &daw);
  //wall=GetWallASL(i,j,FACE_WEST);
  //if (wall<0) wall=spot.westWall;
  //if (wall>0)
  if (daw.drawBlockage)
  {
    TranslateRect(x+AV_WallOffsetRects_Old[12].left, y+AV_WallOffsetRects_Old[12].top, AV_VO_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_VO_WallRect, &tempRect);
    TranslateRect(x+AV_WallOffsetRects_Old[14].left, y+AV_WallOffsetRects_Old[14].top, AV_VO_WallSize, tempRect);
    GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_VO_WallRect, &tempRect);

    //if (spot.westBlockage != OpenBlk)
    //if (BlockedForAreaView(spot.westBlockage,i,j,FACE_WEST))
    if (daw.drawBlockage)
    {
      TranslateRect(x+AV_WallOffsetRects_Old[13].left, y+AV_WallOffsetRects_Old[13].top, AV_VO_WallSize, tempRect);
      GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_VO_WallRect, &tempRect);
    }
  }
}

//*****************************************************************************
//    NAME: updateAreaView
//
// PURPOSE: 
//
//*****************************************************************************
void updateAreaView(int plyrX, int plyrY, int facing, long int surface)
{
   int x, y;
   int startX, startY, endX, endY;
   RECT tempRect;
   
   if (!LoadAreaViewArt(globalData.m_MapArt, globalData.MapArtSurf, globalData.UseAVZones))
   {
     WriteDebugString("Failed LoadAreaViewArt() in updateAreaView()\n");
     return;
   }

   if (!GraphicsMgr.ValidSurface(surface))
   {
      WriteDebugString("Bogus dest surface in updateAreaView()\n");
      return;
   }

   // how many squares are visible?
   int AV_WIDTH = ViewportWidth/SQUARESIZE;
   int AV_HEIGHT = ViewportHeight/SQUARESIZE;

   if ((plyrX+(AV_WIDTH/2)) < levelData.area_width)
      startX = plyrX-(AV_WIDTH/2);
   else
      startX = levelData.area_width-AV_WIDTH;

   if ((plyrY+(AV_HEIGHT/2)) < levelData.area_height)
      startY = plyrY-(AV_HEIGHT/2);
   else
      startY = levelData.area_height-AV_HEIGHT;

   if (startX < 0) startX = 0;
   if (startY < 0) startY = 0;

   endX = startX+AV_WIDTH;
   endY = startY+AV_HEIGHT;
   if (endX > levelData.area_width)  endX = levelData.area_width;
   if (endY > levelData.area_height) endY = levelData.area_height;

   for (int i=startY;i<endY;i++)
   {
      for (int j=startX;j<endX;j++)
      {   
         x = ((j-startX)*SQUARESIZE)+OFFSET+ViewportX;
         y = ((i-startY)*SQUARESIZE)+OFFSET+ViewportY;
         
         MappingType mtype = levelData.zoneData.zones[levelData.area[i][j].zone].allowMap;
         if (cmdLine.m_forceAV) mtype = AreaMapping;

         if (mtype == AreaMapping)
         {
           if (UseOldAVStyle)
             DrawOldAreaViewSquare(j,i,x,y, levelData.area[i][j], surface);
           else
             DrawAreaViewSquare(j,i,x,y, levelData.area[i][j], surface);

#ifdef UAFEngine
           if (EditorMode()) // only true while testing design from editor
#endif
           {
             // display event markers if in debug mode
             if (levelData.haveEvent(j,i))
             {
               RECT EventMarkerSize = {6,6,10,10};
               TranslateRect(x, y, EventMarkerSize, tempRect);
               GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_PartyRect, &tempRect);
             }
           }
         }
         else if (mtype == AutoMapping)
         {
            if (party.visitData.IsVisited(GetCurrentLevel(), j,i))
            {
              if (UseOldAVStyle)
                DrawOldAreaViewSquare(j,i,x,y, levelData.area[i][j], surface);
              else
                DrawAreaViewSquare(j,i,x,y, levelData.area[i][j], surface);

#ifdef UAFEngine
               if (EditorMode()) // only true while testing design from editor, or running engine _DEBUG build
#endif
               {
                 // display event markers if in debug mode
                 if (levelData.haveEvent(j,i))
                 {
                   RECT EventMarkerSize = {6,6,10,10};
                   TranslateRect(x, y, EventMarkerSize, tempRect);
                   GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &AV_PartyRect, &tempRect);
                 }
               }
            }
         }

         if ((i==plyrY) && (j==plyrX))
         {
           TranslateRect(x+AV_WallOffsetRects[16].left, y+AV_WallOffsetRects[16].top, ArrowSize, tempRect);
           GraphicsMgr.BlitImage(surface, globalData.MapArtSurf, &ArrowRects[facing], &tempRect);
         }
      }
   }
}

//*****************************************************************************
//    NAME: UpdateOutdoorView
//
// PURPOSE: 
//
//*****************************************************************************
void UpdateOutdoorView(int plyrX, int plyrY, int facing, long int dstSurface, BOOL ShowLarge)
{
  // make sure outdoor surface is loaded
  //if (!GraphicsMgr.ValidSurface(BackgroundSets[1].backgroundSurface))
    BackgroundSets[1].GetBackgroundSurface();

  if (!LoadAreaViewArt(globalData.m_MapArt, globalData.MapArtSurf, globalData.UseAVZones))
  {
    WriteDebugString("Failed LoadAreaViewArt() in UpdateOutdoorView()\n");
    return;
  }

  if (ShowLarge)
  {
    //               calc start x just inside left vert frame
    MapScreenStartX = VertBarLeftX+(VertBarSrc.right-VertBarSrc.left)+2;
    //               calc end x just inside right vert frame
    MapScreenEndX   = VertBarRightX-2;

    //               calc y just below top horz frame
    MapScreenStartY = HorzBarTopY+(HorzBarSrc.bottom-HorzBarSrc.top)+2;
    //               calc y just above middle horz frame
    MapScreenEndY   = HorzBarMidY-2;

    MapScreenWidth = MapScreenEndX-MapScreenStartX+1;
    MapScreenHeight = MapScreenEndY-MapScreenStartY+1;
  }
  else
  {
    MapScreenStartX = ViewportX;
    MapScreenStartY = ViewportY;
    MapScreenEndX   = ViewportX+ViewportWidth;
    MapScreenEndY   = ViewportY+ViewportHeight;

    MapScreenWidth = ViewportWidth;
    MapScreenHeight = ViewportHeight;
  }

  // how many squares are visible?
  int ViewSquareWidth = MapScreenWidth/SQUARESIZE;
  int ViewSquareHeight = MapScreenHeight/SQUARESIZE;

  // in map coords (levelData.area[startY][startX])
  int startX, startY;

  // center image around party x,y unless the party
  // is near an edge.
  if ((plyrX+(ViewSquareWidth/2)) < levelData.area_width)
    startX = plyrX-(ViewSquareWidth/2);
  else
    startX = levelData.area_width-ViewSquareWidth;

  if ((plyrY+(ViewSquareHeight/2)) < levelData.area_height)
    startY = plyrY-ViewSquareHeight/2;
  else
    startY = levelData.area_height-ViewSquareHeight;

  if (startX < 0) startX = 0;
  if (startY < 0) startY = 0;

  // upper,left of source image to be copied
  // each x,y in levelData equals SQUARESIZE pixels
  int srcX = (startX*SQUARESIZE);
  int srcY = (startY*SQUARESIZE);

  // screen x,y where player arrow should be drawn
  MapPartyX = ((plyrX-startX)*SQUARESIZE)+MapScreenStartX+OFFSET;
  MapPartyY = ((plyrY-startY)*SQUARESIZE)+MapScreenStartY+OFFSET;

  RECT srcRect;
  RECT dstRect;

  srcRect.left   = srcX;
  srcRect.top    = srcY;
  srcRect.right  = srcRect.left + MapScreenWidth;
  srcRect.bottom = srcRect.top + MapScreenHeight;

  // must clamp srcRect to be within image dimensions
  if (srcRect.right > BackgroundSets[1].surfRect.right)
    srcRect.right = BackgroundSets[1].surfRect.right;
  if (srcRect.bottom > BackgroundSets[1].surfRect.bottom)
    srcRect.bottom = BackgroundSets[1].surfRect.bottom;

  dstRect.left   = MapScreenStartX;
  dstRect.top    = MapScreenStartY;
  dstRect.right  = dstRect.left + MapScreenWidth;
  dstRect.bottom = dstRect.top + MapScreenHeight;

  // dst rect should not be stretched to fit viewport window.
  // the graphics code will do this automatically if the src and dst
  // rects don't have the same width and height
  if ((srcRect.right-srcRect.left) != (dstRect.right-dstRect.left))
    dstRect.right = dstRect.left + (srcRect.right-srcRect.left);
  if ((srcRect.bottom-srcRect.top) != (dstRect.bottom-dstRect.top))
    dstRect.bottom = dstRect.top + (srcRect.bottom-srcRect.top);

  // copy portion of outdoor map to viewport
  GraphicsMgr.BlitImage(dstSurface, BackgroundSets[1].GetBackgroundSurface(), &srcRect, &dstRect);

  // draw party arrow
  TranslateRect(MapPartyX, MapPartyY, AV_PartySize, dstRect);
  GraphicsMgr.BlitImage(dstSurface, globalData.MapArtSurf, &AV_PartyRect, &dstRect);
}

//*****************************************************************************
//    NAME: RenderSquare0
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare0(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  int wallSlot;
  WORD format;
  int p_width=0;  
  BOOL doorFirst=RenderDoorBeforeOverlay(facing,0);
  srcSurface = getWallSurface(facing, 0 , &wallSlot, &format);
  int WallCount = WallFormatMgr.GetDistantWallCount(format);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 0);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (WallCount==5)
  {
    p_width = WallFormatMgr.GetWidth(format, DRAW_P_WALL);

    if (srcSurface > 0)
    {           
     if (   (getWallSurface(facing, 13, &wallSlot, NULL) > 0) 
         || (getWallSurface(left, 0, &wallSlot, NULL) > 0)
         || (getWallSurface(right, 13, &wallSlot, NULL) > 0)
         || (!validCoords(viewMap[13].y,viewMap[13].x)))
     {
       BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface, format);
     };
     BltSurface(pt.x+p_width, pt.y, DRAW_J_WALL, srcSurface, dstSurface, format);
    }

    if (!doorFirst)
    {
      srcSurface = getOverlaySurface(facing, 0, &wallSlot, &format);
      if (srcSurface > 0)
      {
       if (   (getWallSurface(facing, 13, &wallSlot, NULL) > 0) 
           || (getWallSurface(left, 0, &wallSlot, NULL) > 0)
           || (getWallSurface(right, 13, &wallSlot, NULL) > 0)
           || (!validCoords(viewMap[13].y,viewMap[13].x)))
       {
         BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,0));
       };
       BltOverlaySurface(pt.x+p_width,pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,0));
      }

      srcSurface = getDoorSurface(facing,0, &wallSlot, &format);            
      if (srcSurface > 0)
        BltSurface(pt.x+p_width, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);
    }
    else
    {
      srcSurface = getDoorSurface(facing,0, &wallSlot, &format);            
      if (srcSurface > 0)
        BltSurface(pt.x+p_width, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);

      srcSurface = getOverlaySurface(facing,0, &wallSlot, &format);
      if (srcSurface > 0)
      {
       if (   (getWallSurface(facing, 13, &wallSlot, NULL) > 0) 
           || (getWallSurface(left, 0, &wallSlot, NULL) > 0)
           || (getWallSurface(right, 13, &wallSlot, NULL) > 0)
           || (!validCoords(viewMap[13].y,viewMap[13].x)))
         BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,0));
       BltOverlaySurface(pt.x+p_width,pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,0));
      }
    }
  }
  else
  {
    srcSurface = getWallSurface(facing,0, &wallSlot, &format);
    if (srcSurface > 0)
      BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface, format);

    if (!doorFirst)
    {
      srcSurface = getOverlaySurface(facing,0, &wallSlot, &format);
      if (srcSurface > 0)
        BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,0));

      srcSurface = getDoorSurface(facing,0, &wallSlot, &format);            
      if (srcSurface > 0)
        BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);
    }
    else
    {
      srcSurface = getDoorSurface(facing,0, &wallSlot, &format);            
      if (srcSurface > 0)
        BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);

      srcSurface = getOverlaySurface(facing,0, &wallSlot, &format);
      if (srcSurface > 0)
        BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,0));
    }
  }

  srcSurface = getWallSurface(left, 0, &wallSlot, &format);
  //format = getWallSlotFormat(left,0);
  WallCount = WallFormatMgr.GetDistantWallCount(format);
  pt = WallFormatMgr.GetSquareOrigin(format, 0);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (WallCount == 7)
  {
    // left wall    
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format);

    if (!RenderDoorBeforeOverlay(left,0))
    {
      srcSurface = getOverlaySurface(left,0, &wallSlot, &format);
      if (srcSurface > 0)
       BltOverlaySurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format,getWallSlot(left,0));

      srcSurface = getDoorSurface(left,0, &wallSlot, &format);            
      if (srcSurface > 0)
       BltSurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format);
    }
    else
    {
      srcSurface = getDoorSurface(left,0, &wallSlot, &format);            
      if (srcSurface > 0)
       BltSurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format);

      srcSurface = getOverlaySurface(left,0, &wallSlot, &format);
      if (srcSurface > 0)
       BltOverlaySurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format,getWallSlot(left,0));
    }
  }
}

//*****************************************************************************
//    NAME: RenderSquare1
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare1(int left, int facing, int right, long dstSurface)
{
  int p_width=0;
  long srcSurface;
  WORD format;
  int wallSlot;

  //format = getWallSlotFormat(facing,1);
  srcSurface = getWallSurface(facing, 1, &wallSlot, &format);
  int WallCount = WallFormatMgr.GetDistantWallCount(format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,1);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 1);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (WallCount==5)
  {
    p_width = WallFormatMgr.GetWidth(format, DRAW_P_WALL);

    if (srcSurface > 0)
    {
     BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);
     if (   (getWallSurface(facing,14, &wallSlot, NULL) > 0) 
         || (getWallSurface(right,1, &wallSlot, NULL) > 0)
         || (getWallSurface(left,14, &wallSlot, NULL) > 0)
         || (!validCoords(viewMap[14].y,viewMap[14].x)))
       BltSurface(pt.x+p_width, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);
    }

    if (!doorFirst)
    {
      srcSurface = getOverlaySurface(facing,1, &wallSlot, &format);
      if (srcSurface > 0)
      {
       BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,1));
       if (   (getWallSurface(facing,14, &wallSlot, NULL) > 0) 
           || (getWallSurface(right,1, &wallSlot, NULL) > 0)
           || (getWallSurface(left,14, &wallSlot, NULL) > 0)
           || (!validCoords(viewMap[14].y,viewMap[14].x)))
         BltOverlaySurface(pt.x+p_width, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,1));
      }

      srcSurface = getDoorSurface(facing,1, &wallSlot, &format);            
      if (srcSurface > 0)
       BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);
    }
    else
    {
      srcSurface = getDoorSurface(facing,1, &wallSlot, &format);            
      if (srcSurface > 0)
       BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);

      srcSurface = getOverlaySurface(facing,1, &wallSlot, &format);
      if (srcSurface > 0)
      {
       BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,1));
       if (   (getWallSurface(facing,14, &wallSlot, NULL) > 0) 
           || (getWallSurface(right,1, &wallSlot, NULL) > 0)
           || (getWallSurface(left,14, &wallSlot, NULL) > 0)
           || (!validCoords(viewMap[14].y,viewMap[14].x)))
         BltOverlaySurface(pt.x+p_width, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,1));
      }
    }
  }
  else
  {
    srcSurface = getWallSurface(facing, 1, &wallSlot, &format);
    if (srcSurface > 0)
      BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface, format);

    if (!doorFirst)
    {
      srcSurface = getOverlaySurface(facing,1, &wallSlot, &format);
      if (srcSurface > 0)
        BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,1));

      srcSurface = getDoorSurface(facing,1, &wallSlot, &format);
      if (srcSurface > 0)
        BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);
    }
    else
    {
      srcSurface = getDoorSurface(facing,1, &wallSlot, &format);
      if (srcSurface > 0)
        BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);

      srcSurface = getOverlaySurface(facing,1, &wallSlot, &format);
      if (srcSurface > 0)
        BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,1));
    }
  }

  //format = getWallSlotFormat(right, 1);
  srcSurface = getWallSurface(right,1, &wallSlot, &format);
  WallCount = WallFormatMgr.GetDistantWallCount(format);
  pt = WallFormatMgr.GetSquareOrigin(format, 1);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (WallCount == 7)
  {
    // right wall
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format);

    if (!RenderDoorBeforeOverlay(right,1))
    {
      srcSurface = getOverlaySurface(right,1, &wallSlot, &format);
      if (srcSurface > 0)
       BltOverlaySurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format,getWallSlot(right,1));

      srcSurface = getDoorSurface(right,1, &wallSlot, &format);            
      if (srcSurface > 0)
       BltSurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format);
    }
    else
    {
      srcSurface = getDoorSurface(right,1, &wallSlot, &format);            
      if (srcSurface > 0)
       BltSurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format);

      srcSurface = getOverlaySurface(right,1, &wallSlot, &format);
      if (srcSurface > 0)
       BltOverlaySurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format,getWallSlot(right,1));
    }
  }
}
//*****************************************************************************
//    NAME: RenderSquare2
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare2(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;
  int p_width=0;
  int j_width=0;

  // front wall
  //format = getWallSlotFormat(facing, 2);
  srcSurface = getWallSurface(facing,2, &wallSlot, &format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,2);
  int WallCount = WallFormatMgr.GetDistantWallCount(format);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 2);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (WallCount==5)
  {
    p_width = WallFormatMgr.GetWidth(format, DRAW_P_WALL);
    j_width = WallFormatMgr.GetWidth(format, DRAW_J_WALL);

    if (srcSurface > 0)
    {
      if ((getWallSurface(facing,0, &wallSlot, NULL) > 0) || (getWallSurface(left,2, &wallSlot, NULL) > 0))
      {
        BltSurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);
      };
      BltSurface(pt.x+j_width, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);
    }

    if (!doorFirst)
    {
      srcSurface = getOverlaySurface(facing,2, &wallSlot, &format);
      if (srcSurface > 0)
      {
        if ((getWallSurface(facing,0, &wallSlot, NULL) > 0) || (getWallSurface(left,2, &wallSlot, NULL) > 0))
        {
          BltOverlaySurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,2));
        };
        BltOverlaySurface(pt.x+j_width, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,2));
      }
      {
        srcSurface = getDoorSurface(facing,2, &wallSlot, &format);            
        if (srcSurface > 0)
        {
          BltSurface(pt.x+p_width, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);
        };
      };
    }
    else
    {
      srcSurface = getDoorSurface(facing,2, &wallSlot, &format);            
      if (srcSurface > 0)
      {
        BltSurface(pt.x+p_width, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);
      };

      srcSurface = getOverlaySurface(facing,2, &wallSlot, &format);
      if (srcSurface > 0)
      {
       if ((getWallSurface(facing,0, &wallSlot, NULL) > 0) || (getWallSurface(left,2, &wallSlot, NULL) > 0))
       {
         BltOverlaySurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,2));
       };
       BltOverlaySurface(pt.x+j_width, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,2));
      }
    }
  }
  else
  {
    srcSurface = getWallSurface(facing,2, &wallSlot, &format);
    if (srcSurface > 0)
      BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface, format);

    if (!doorFirst)
    {
      srcSurface = getOverlaySurface(facing,2, &wallSlot, &format);
      if (srcSurface > 0)
        BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,2));

      srcSurface = getDoorSurface(facing,2, &wallSlot, &format);
      if (srcSurface > 0)
        BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);
    }
    else
    {
      srcSurface = getDoorSurface(facing,2, &wallSlot, &format);
      if (srcSurface > 0)
        BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);

      srcSurface = getOverlaySurface(facing,2, &wallSlot, &format);
      if (srcSurface > 0)
        BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,2));
    }
  }

  // left wall
  //format = getWallSlotFormat(left,2);
  WallCount = WallFormatMgr.GetDistantWallCount(format);

  // this is hacked for backwards compatibility if WC=5,
  // otherwise it would use square 2 origin
  if (WallCount == 5)
    pt = WallFormatMgr.GetSquareOrigin(format, 7);
  else
    pt = WallFormatMgr.GetSquareOrigin(format, 2);

  pt.x += ViewportX;
  pt.y += ViewportY;

  srcSurface = getWallSurface(left,2, &wallSlot, &format);
  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format);

  if (!RenderDoorBeforeOverlay(left,2))
  {
    srcSurface = getOverlaySurface(left,2, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format,getWallSlot(left,2));

    srcSurface = getDoorSurface(left,2, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(left,2, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(left,2, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format,getWallSlot(left,2));
  }
}
//*****************************************************************************
//    NAME: RenderSquare3
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare3(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;
  int j_width=0;

  // front wall
  //format = getWallSlotFormat(facing, 3);
  srcSurface = getWallSurface(facing, 3, &wallSlot, &format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,3);
  int WallCount = WallFormatMgr.GetDistantWallCount(format);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 3);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (WallCount==5)
  {
    j_width = WallFormatMgr.GetWidth(format, DRAW_J_WALL);

    if (srcSurface > 0)
    {
     BltSurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);
     if ((getWallSurface(facing,1, &wallSlot, NULL) > 0) || (getWallSurface(right,3, &wallSlot, NULL) > 0))
       BltSurface(pt.x+j_width, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);
    }

    if (!doorFirst)
    {
      srcSurface = getOverlaySurface(facing,3, &wallSlot, &format);
      if (srcSurface > 0)
      {
       BltOverlaySurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,3));
       if ((getWallSurface(facing, 1, &wallSlot, NULL) > 0) || (getWallSurface(right, 3, &wallSlot, NULL) > 0))
         BltOverlaySurface(pt.x+j_width, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,3));
      }

      srcSurface = getDoorSurface(facing,3, &wallSlot, &format);
      if (srcSurface > 0)
       BltSurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);
    }
    else
    {
      srcSurface = getDoorSurface(facing,3, &wallSlot, &format);
      if (srcSurface > 0)
       BltSurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);

      srcSurface = getOverlaySurface(facing,3, &wallSlot, &format);
      if (srcSurface > 0)
      {
       BltOverlaySurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,3));
       if ((getWallSurface(facing,1, &wallSlot, NULL) > 0) || (getWallSurface(right,3, &wallSlot, NULL) > 0))
         BltOverlaySurface(pt.x+j_width, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,3));
      }
    }
  }
  else
  {
    srcSurface = getWallSurface(facing,3, &wallSlot, &format);
    if (srcSurface > 0)
      BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface, format);

    if (!doorFirst)
    {
      srcSurface = getOverlaySurface(facing,3, &wallSlot, &format);
      if (srcSurface > 0)
        BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,3));

      srcSurface = getDoorSurface(facing,3, &wallSlot, &format);
      if (srcSurface > 0)
        BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);
    }
    else
    {
      srcSurface = getDoorSurface(facing,3, &wallSlot, &format);
      if (srcSurface > 0)
        BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);

      srcSurface = getOverlaySurface(facing,3, &wallSlot, &format);
      if (srcSurface > 0)
        BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,3));
    }
  }

  // right wall
  //format = getWallSlotFormat(right,3);
  WallCount = WallFormatMgr.GetDistantWallCount(format);
  pt = WallFormatMgr.GetSquareOrigin(format, 3);
  pt.x += ViewportX;
  pt.y += ViewportY;

  srcSurface = getWallSurface(right, 3, &wallSlot, &format);
  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format);

  if (!RenderDoorBeforeOverlay(right,3))
  {
    srcSurface = getOverlaySurface(right,3, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format,getWallSlot(right,3));

    srcSurface = getDoorSurface(right,3, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(right,3, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(right,3, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format,getWallSlot(right,3));
  }
}
//*****************************************************************************
//    NAME: RenderSquare4
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare4(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;
  int j_width=0;

  // front wall
  //format = getWallSlotFormat(facing,4);
  srcSurface = getWallSurface(facing,4, &wallSlot, &format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,4);
  int WallCount = WallFormatMgr.GetDistantWallCount(format);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 4);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (WallCount==5)
  {
    j_width = WallFormatMgr.GetWidth(format, DRAW_J_WALL);

    if (srcSurface > 0)
    {
      if (getWallSurface(facing,2, &wallSlot, NULL) > 0)
      {
        BltSurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);
      };
      if (getWallSurface(facing,4, &wallSlot, NULL) > 0)
      {
        BltSurface(pt.x+j_width, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);
      };
      if (getWallSurface(facing,3, &wallSlot, NULL) > 0)
      {
        BltSurface(pt.x+(2*j_width), pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);
      };
    };


    if (!doorFirst)
    {
      srcSurface = getOverlaySurface(facing,4, &wallSlot, &format);
      if (srcSurface > 0)
      {
       if (getWallSurface(facing,2, &wallSlot, NULL) > 0)
         BltOverlaySurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,4));
       BltOverlaySurface(pt.x+j_width, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,4));
       if (getWallSurface(facing,3, &wallSlot, NULL) > 0)
         BltOverlaySurface(pt.x+(2*j_width), pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,4));
      }

      srcSurface = getDoorSurface(facing,4, &wallSlot, &format);            
      if (srcSurface > 0)
       BltSurface(pt.x+j_width, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);
    }
    else
    {
      srcSurface = getDoorSurface(facing,4, &wallSlot, &format);            
      if (srcSurface > 0)
        BltSurface(pt.x+j_width, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);

      srcSurface = getOverlaySurface(facing,4, &wallSlot, &format);
      if (srcSurface > 0)
      {
       if (getWallSurface(facing,2, &wallSlot, NULL) > 0)
         BltOverlaySurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,4));
       BltOverlaySurface(pt.x+j_width, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,4));
       if (getWallSurface(facing,3, &wallSlot, NULL) > 0)
         BltOverlaySurface(pt.x+(2*j_width), pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,4));
      }
    }


  }
  else

  {
    srcSurface = getWallSurface(facing,4, &wallSlot, &format);
    if (srcSurface > 0)
      BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface, format);

    if (!doorFirst)
    {
      srcSurface = getOverlaySurface(facing,4, &wallSlot, &format);
      if (srcSurface > 0)
        BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,4));

      srcSurface = getDoorSurface(facing,4, &wallSlot, &format);
      if (srcSurface > 0)
        BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);
    }
    else
    {
      srcSurface = getDoorSurface(facing,4, &wallSlot, &format);
      if (srcSurface > 0)
        BltSurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format);

      srcSurface = getOverlaySurface(facing,4, &wallSlot, &format);
      if (srcSurface > 0)
        BltOverlaySurface(pt.x, pt.y, DRAW_P_WALL, srcSurface, dstSurface,format,getWallSlot(facing,4));
    }
  }



  // left wall
  //format = getWallSlotFormat(left,4);
  WallCount = WallFormatMgr.GetDistantWallCount(format);
  pt = WallFormatMgr.GetSquareOrigin(format, 4);
  pt.x += ViewportX;
  pt.y += ViewportY;

  srcSurface = getWallSurface(left,4, &wallSlot, &format);
  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format);

  if (!RenderDoorBeforeOverlay(left,4))
  {
    srcSurface = getOverlaySurface(left,4, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format,getWallSlot(left,4));

    srcSurface = getDoorSurface(left,4, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(left,4, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(left,4, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_I_WALL, srcSurface, dstSurface,format,getWallSlot(left,4));
  };



  // right wall
  //format = getWallSlotFormat(right,4);
  WallCount = WallFormatMgr.GetDistantWallCount(format);
  pt = WallFormatMgr.GetSquareOrigin(format, 4);
  pt.x += ViewportX;
  pt.y += ViewportY;

  srcSurface = getWallSurface(right,4, &wallSlot, &format);
  if (srcSurface > 0)
  {
    BltSurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format);
  };


  if (!RenderDoorBeforeOverlay(right,4))
  {
    srcSurface = getOverlaySurface(right,4, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format,getWallSlot(right,4));

    srcSurface = getDoorSurface(right,4, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(right,4, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(right,4, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_K_WALL, srcSurface, dstSurface,format,getWallSlot(right,4));
  };

}
//*****************************************************************************
//    NAME: RenderSquare5
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare5(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;

  //format = getWallSlotFormat(facing,5);
  srcSurface = getWallSurface(facing,5, &wallSlot, &format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,5);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 5);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_M_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(facing,5, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_M_WALL, srcSurface, dstSurface,format,getWallSlot(facing,5));

    srcSurface = getDoorSurface(facing,5, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_M_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(facing,5, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_M_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(facing,5, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_M_WALL, srcSurface, dstSurface,format,getWallSlot(facing,5));
  }
}
//*****************************************************************************
//    NAME: RenderSquare6
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare6(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;

//  format = getWallSlotFormat(facing,6);
  srcSurface = getWallSurface(facing,6, &wallSlot, &format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,6);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 6);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_L_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(facing,6, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_L_WALL, srcSurface, dstSurface,format,getWallSlot(facing,6));

    srcSurface = getDoorSurface(facing,6, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_L_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(facing,6, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_L_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(facing,6, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_L_WALL, srcSurface, dstSurface,format,getWallSlot(facing,6));
  }
}
//*****************************************************************************
//    NAME: RenderSquare7
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare7(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;

  // front wall
//  format = getWallSlotFormat(facing,7);
  srcSurface = getWallSurface(facing,7, &wallSlot, &format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,7);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 7);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(facing,7, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format,getWallSlot(facing,7));

    srcSurface = getDoorSurface(facing,7, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(facing,7, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(facing,7, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format,getWallSlot(facing,7));
  }

  // left wall
//  format = getWallSlotFormat(left,7);
  doorFirst = RenderDoorBeforeOverlay(left,7);
  pt = WallFormatMgr.GetSquareOrigin(format, 7);
  pt.x += ViewportX;
  pt.y += ViewportY;

  srcSurface = getWallSurface(left,7, &wallSlot, &format);
  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_N_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(left,7, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_N_WALL, srcSurface, dstSurface,format,getWallSlot(left,7));

    srcSurface = getDoorSurface(left,7, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_N_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(left,7, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_N_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(left,7, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_N_WALL, srcSurface, dstSurface,format,getWallSlot(left,7));
  }
}
//*****************************************************************************
//    NAME: RenderSquare8
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare8(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;

  // front
//  format = getWallSlotFormat(facing,8);
  srcSurface = getWallSurface(facing,8, &wallSlot, &format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,8);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 8);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(facing,8, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format,getWallSlot(facing,8));

    srcSurface = getDoorSurface(facing,8, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(facing,8, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(facing,8, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format,getWallSlot(facing,8));
  }

  // right
//  format = getWallSlotFormat(right,8);
  doorFirst = RenderDoorBeforeOverlay(right,8);
  pt = WallFormatMgr.GetSquareOrigin(format, 8);
  pt.x += ViewportX;
  pt.y += ViewportY;

  srcSurface = getWallSurface(right,8, &wallSlot, &format);
  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_O_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(right,8, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_O_WALL, srcSurface, dstSurface,format,getWallSlot(right,8));

    srcSurface = getDoorSurface(right,8, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_O_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(right,8, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_O_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(right,8, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_O_WALL, srcSurface, dstSurface,format,getWallSlot(right,8));
  }
}
//*****************************************************************************
//    NAME: RenderSquare9
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare9(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;

//  format = getWallSlotFormat(facing,9);
  srcSurface = getWallSurface(facing,9, &wallSlot, &format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,9);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 9);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(facing,9, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format,getWallSlot(facing,9));

    srcSurface = getDoorSurface(facing,9, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(facing,9, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(facing,9, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_H_WALL, srcSurface, dstSurface,format,getWallSlot(facing,9));
  }

  // left
  //format = getWallSlotFormat(left,9);
  doorFirst = RenderDoorBeforeOverlay(left,9);
  pt = WallFormatMgr.GetSquareOrigin(format, 9);
  pt.x += ViewportX;
  pt.y += ViewportY;

  srcSurface = getWallSurface(left,9, &wallSlot, &format);
  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_F_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(left,9, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_F_WALL, srcSurface, dstSurface,format,getWallSlot(left,9));

    srcSurface = getDoorSurface(left,9, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_F_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(left,9, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_F_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(left,9, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_F_WALL, srcSurface, dstSurface,format,getWallSlot(left,9));
  }

  // right
//  format = getWallSlotFormat(right,9);
  doorFirst = RenderDoorBeforeOverlay(right,9);
  pt = WallFormatMgr.GetSquareOrigin(format, 9);
  pt.x += ViewportX;
  pt.y += ViewportY;

  srcSurface = getWallSurface(right,9, &wallSlot, &format);
  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_G_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(right,9, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_G_WALL, srcSurface, dstSurface,format,getWallSlot(right,9));

    srcSurface = getDoorSurface(right,9, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_G_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(right,9, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_G_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(right,9, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_G_WALL, srcSurface, dstSurface,format,getWallSlot(right,9));
  }
}
//*****************************************************************************
//    NAME: RenderSquare10
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare10(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;

//  format = getWallSlotFormat(facing,10);
  srcSurface = getWallSurface(facing,10, &wallSlot, &format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,10);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 10);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_D_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(facing,10, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_D_WALL, srcSurface, dstSurface,format,getWallSlot(facing,10));

    srcSurface = getDoorSurface(facing,10, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_D_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(facing,10, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_D_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(facing,10, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_D_WALL, srcSurface, dstSurface,format,getWallSlot(facing,10));
  }
}
//*****************************************************************************
//    NAME: RenderSquare11
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare11(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;

//  format = getWallSlotFormat(facing,11);
  srcSurface = getWallSurface(facing,11, &wallSlot, &format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,11);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 11);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_C_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(facing,11, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_C_WALL, srcSurface, dstSurface,format,getWallSlot(facing,11));

    srcSurface = getDoorSurface(facing,11, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_C_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(facing,11, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_C_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(facing,11, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_C_WALL, srcSurface, dstSurface,format,getWallSlot(facing,11));
  }
}
//*****************************************************************************
//    NAME: RenderSquare12
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare12(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;

  srcSurface = getWallSurface(facing,12, &wallSlot, &format);
//  format = getWallSlotFormat(facing,12);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,12);
  point_type pt = WallFormatMgr.GetSquareOrigin(format, 12);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_E_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(facing,12, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_E_WALL, srcSurface, dstSurface,format,getWallSlot(facing,12));

    srcSurface = getDoorSurface(facing,12, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_E_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(facing,12, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_E_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(facing,12, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_E_WALL, srcSurface, dstSurface,format,getWallSlot(facing,12));
  }

  // left
//  format = getWallSlotFormat(left,12);
  doorFirst = RenderDoorBeforeOverlay(left,12);
  pt = WallFormatMgr.GetSquareOrigin(format, 12);
  pt.x += ViewportX;
  pt.y += ViewportY;

  srcSurface = getWallSurface(left,12, &wallSlot, &format);
  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_A_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(left,12, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_A_WALL, srcSurface, dstSurface,format,getWallSlot(left,12));

    srcSurface = getDoorSurface(left,12, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_A_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(left,12, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_A_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(left,12, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_A_WALL, srcSurface, dstSurface,format,getWallSlot(left,12));
  }

  // right
  //format = getWallSlotFormat(right,12);
  doorFirst = RenderDoorBeforeOverlay(right,12);
  pt = WallFormatMgr.GetSquareOrigin(format, 12);
  pt.x += ViewportX;
  pt.y += ViewportY;

  srcSurface = getWallSurface(right,12, &wallSlot, &format);
  if (srcSurface > 0)
   BltSurface(pt.x, pt.y, DRAW_B_WALL, srcSurface, dstSurface,format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(right,12, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_B_WALL, srcSurface, dstSurface,format,getWallSlot(right,12));

    srcSurface = getDoorSurface(right,12, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_B_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(right,12, &wallSlot, &format);            
    if (srcSurface > 0)
     BltSurface(pt.x, pt.y, DRAW_B_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(right,12, &wallSlot, &format);
    if (srcSurface > 0)
     BltOverlaySurface(pt.x, pt.y, DRAW_B_WALL, srcSurface, dstSurface,format,getWallSlot(right,12));
  }
}
//*****************************************************************************
//    NAME: RenderSquare13
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare13(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;
//  format = getWallSlotFormat(facing,13);
  srcSurface = getWallSurface(facing,13, &wallSlot, &format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,13);
  int WallCount = WallFormatMgr.GetDistantWallCount(format);

  if (WallCount == 5) return;

  point_type pt = WallFormatMgr.GetSquareOrigin(format, 13);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (srcSurface > 0)
    BltSurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface, format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(facing,13, &wallSlot, &format);
    if (srcSurface > 0)
      BltOverlaySurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,13));

    srcSurface = getDoorSurface(facing,13, &wallSlot, &format);
    if (srcSurface > 0)
      BltSurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(facing,13, &wallSlot, &format);
    if (srcSurface > 0)
      BltSurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(facing,13, &wallSlot, &format);
    if (srcSurface > 0)
      BltOverlaySurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,13));
  }
}
//*****************************************************************************
//    NAME: RenderSquare14
//
// PURPOSE: 
//
//*****************************************************************************
void RenderSquare14(int left, int facing, int right, long dstSurface)
{
  long srcSurface;
  WORD format;
  int wallSlot;
//  format = getWallSlotFormat(facing,14);
  srcSurface = getWallSurface(facing,14, &wallSlot, &format);
  BOOL doorFirst = RenderDoorBeforeOverlay(facing,14);
  int WallCount = WallFormatMgr.GetDistantWallCount(format);
  if (WallCount == 5) return;

  point_type pt = WallFormatMgr.GetSquareOrigin(format, 14);
  pt.x += ViewportX;
  pt.y += ViewportY;

  if (srcSurface > 0)
    BltSurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface, format);

  if (!doorFirst)
  {
    srcSurface = getOverlaySurface(facing,14, &wallSlot, &format);
    if (srcSurface > 0)
      BltOverlaySurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,14));

    srcSurface = getDoorSurface(facing,14, &wallSlot, &format);
    if (srcSurface > 0)
      BltSurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);
  }
  else
  {
    srcSurface = getDoorSurface(facing,14, &wallSlot, &format);
    if (srcSurface > 0)
      BltSurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format);

    srcSurface = getOverlaySurface(facing,14, &wallSlot, &format);
    if (srcSurface > 0)
      BltOverlaySurface(pt.x, pt.y, DRAW_J_WALL, srcSurface, dstSurface,format,getWallSlot(facing,14));
  }
}

//*****************************************************************************
//    NAME: RenderWalls
//
// PURPOSE: 
//
//*****************************************************************************
void RenderWalls(int facing, long int dstSurface)
{
  int left =(facing+3)&3;
  int right=(facing+1)&3;

  // must ensure that images are drawn from outside to inside, 
  // and back to front.
  //
  // in each square, blit the wall, then overlay, then door
  RenderSquare13(left, facing, right, dstSurface);
  RenderSquare14(left, facing, right, dstSurface);

  for (int i=0; i<13; i++)
  {
    switch (i) 
    {
    case 0: RenderSquare0 (left, facing, right, dstSurface); break; // forward 2 left 3
    case 1: RenderSquare1 (left, facing, right, dstSurface); break; // forward 2 right 2
    case 2: RenderSquare2 (left, facing, right, dstSurface); break; // forward 2 left 1
    case 3: RenderSquare3 (left, facing, right, dstSurface); break; // forward 2 right 1
    case 4: RenderSquare4 (left, facing, right, dstSurface); break; // forward 2
    case 5: RenderSquare5 (left, facing, right, dstSurface); break; // forward 1 left 2
    case 6: RenderSquare6 (left, facing, right, dstSurface); break; // forward 1 right 2
    case 7: RenderSquare7 (left, facing, right, dstSurface); break; // forward 1 left 1
    case 8: RenderSquare8 (left, facing, right, dstSurface); break; // forward 1 right 1
    case 9: RenderSquare9 (left, facing, right, dstSurface); break; // forward 1
    case 10:RenderSquare10(left, facing, right, dstSurface); break; // left 1
    case 11:RenderSquare11(left, facing, right, dstSurface); break; // right 1
    case 12:RenderSquare12(left, facing, right, dstSurface); break; // current cell
    }
  }
}

//*****************************************************************************
//    NAME: RenderBackground
//
// PURPOSE: 
//
//*****************************************************************************
void RenderBackground(int facing, long int dstSurface)
{
  WORD format;
  RECT dstRect;
  RECT srcRect;
  RECT imageSrcRect;
  long srcSurface;

  // check to see if distant background rendering is
  // enabled in the current square where player is
  // standing
  if (!RenderDistantBackgrounds(facing, 12))
  {
    srcSurface = getBackgroundSurface(facing, 12);

    if (srcSurface > 0)
    { 
      GraphicsMgr.GetSurfaceRect(srcSurface, &imageSrcRect);

      dstRect.left = BackgroundX;
      dstRect.top = BackgroundY;
      dstRect.right = dstRect.left + BackgroundWidth;
      dstRect.bottom = dstRect.top + BackgroundHeight;

      BltBackgroundSurface(dstSurface, srcSurface, &imageSrcRect, &dstRect, getBackgroundSlot(facing, 12));
    }
  }
  else
  {
    BOOL inBands = RenderDistantBackgroundsInBands(facing, 12);
    int wallSlot;
    // There are 5 bands of background image drawn, separated by 4 horz
    // boundary lines.
    // Boundaries go like this:
    // top of viewport (we know this offset = 0)
    // VERT_A_OFFSET matches top of E_WALL_RECT
    // VERT_B_OFFSET matches top of H_WALL_RECT
    // VERT_C_OFFSET matches bottom of H_WALL_RECT
    // VERT_D_OFFSET matches bottom of E_WALL_RECT
    // bottom of viewport (we know this offset = ViewportHeight
    //
    // Use the wall format data to determine image x,y and thus line y.
    //
    // Background in square 12 is drawn in 2 bands top->A, and D->bottom
    // Background in square 9 is drawn in 2 bands A->B, and C->D
    // Background in square 4 is drawn in 1 band B-C
    // All bands cover full width from ViewportX to ViewportX+ViewportWidth
    //

    // determine band offsets
//    int format = getWallSlotFormat(facing,9);
    getWallSurface(facing,4, &wallSlot, &format);
    point_type origin12 = WallFormatMgr.GetSquareOrigin(format, 12);
    point_type e_offset = WallFormatMgr.GetOffset(format, DRAW_E_WALL);
    int VERT_OFFSET_A = origin12.y + e_offset.y;
    int VERT_OFFSET_D = VERT_OFFSET_A + WallFormatMgr.GetHeight(format, DRAW_E_WALL);

//    format = getWallSlotFormat(facing,9);
    point_type origin9 = WallFormatMgr.GetSquareOrigin(format, 9);
    point_type h_offset = WallFormatMgr.GetOffset(format, DRAW_H_WALL);
    int VERT_OFFSET_B = origin9.y + h_offset.y;
    int VERT_OFFSET_C = VERT_OFFSET_B + WallFormatMgr.GetHeight(format, DRAW_H_WALL);

    int AHEIGHT = VERT_OFFSET_B - VERT_OFFSET_A;
    int BHEIGHT = VERT_OFFSET_C - VERT_OFFSET_B;
    int CHEIGHT = VERT_OFFSET_D - VERT_OFFSET_C;
    int DHEIGHT = ViewportHeight-VERT_OFFSET_D;

    if (inBands)
    {
      // all bands run full width of viewport
      dstRect.left = ViewportX;
      dstRect.right = dstRect.left + ViewportWidth;

      srcSurface = getBackgroundSurface(facing, 4);
      if (srcSurface > 0)
      { 
        GraphicsMgr.GetSurfaceRect(srcSurface, &imageSrcRect);

        // single band between B->C
        srcRect.left = imageSrcRect.left;
        srcRect.right = imageSrcRect.right;
        srcRect.top = imageSrcRect.top + VERT_OFFSET_B;
        srcRect.bottom = srcRect.top + BHEIGHT;
  
        dstRect.top = ViewportY + VERT_OFFSET_B;    
        dstRect.bottom = dstRect.top + BHEIGHT;
    
        BltBackgroundSurface(dstSurface, srcSurface, &srcRect, &dstRect, getBackgroundSlot(facing, 4));
      }

      srcSurface = getBackgroundSurface(facing, 9);
      if (srcSurface > 0)
      {
        GraphicsMgr.GetSurfaceRect(srcSurface, &imageSrcRect);

        // single band between A->B
        srcRect.left = imageSrcRect.left;
        srcRect.right = imageSrcRect.right;
        srcRect.top = imageSrcRect.top + VERT_OFFSET_A;
        srcRect.bottom = srcRect.top + AHEIGHT;

        dstRect.top = ViewportY + VERT_OFFSET_A;    
        dstRect.bottom = dstRect.top + AHEIGHT;
    
        BltBackgroundSurface(dstSurface, srcSurface, &srcRect, &dstRect, getBackgroundSlot(facing, 9));

        // single band between C->D
        srcRect.left = imageSrcRect.left;
        srcRect.right = imageSrcRect.right;
        srcRect.top = imageSrcRect.top + VERT_OFFSET_C;
        srcRect.bottom = srcRect.top + CHEIGHT;
    
        dstRect.top = ViewportY + VERT_OFFSET_C;    
        dstRect.bottom = dstRect.top + CHEIGHT;
    
        BltBackgroundSurface(dstSurface, srcSurface, &srcRect, &dstRect, getBackgroundSlot(facing, 9));
      }

      srcSurface = getBackgroundSurface(facing, 12);
      if (srcSurface > 0)
      {
        GraphicsMgr.GetSurfaceRect(srcSurface, &imageSrcRect);

        // single band between top->A
        srcRect.left = imageSrcRect.left;
        srcRect.right = imageSrcRect.right;
        srcRect.top = imageSrcRect.top;
        srcRect.bottom = srcRect.top + VERT_OFFSET_A;
    
        dstRect.top = ViewportY;    
        dstRect.bottom = dstRect.top + VERT_OFFSET_A;
    
        BltBackgroundSurface(dstSurface, srcSurface, &srcRect, &dstRect, getBackgroundSlot(facing, 12));

        // single band between D->bottom
        srcRect.left = imageSrcRect.left;
        srcRect.right = imageSrcRect.right;
        srcRect.top = imageSrcRect.top + VERT_OFFSET_D;
        srcRect.bottom = imageSrcRect.bottom;
    
        dstRect.top = ViewportY + VERT_OFFSET_D;    
        dstRect.bottom = dstRect.top + DHEIGHT;
    
        BltBackgroundSurface(dstSurface, srcSurface, &srcRect, &dstRect, getBackgroundSlot(facing, 12));
      }
    }
    else // not in bands
    {
      // first fill background with normal backdrop image
      srcSurface = getBackgroundSurface(facing, 12);
      if (srcSurface > 0)
      {    
        GraphicsMgr.GetSurfaceRect(srcSurface, &imageSrcRect);
        dstRect.left = ViewportX;
        dstRect.top = ViewportY;
        dstRect.right = dstRect.left + ViewportWidth;
        dstRect.bottom = dstRect.top + ViewportHeight;

        BltBackgroundSurface(dstSurface, srcSurface, &imageSrcRect, &dstRect, getBackgroundSlot(facing, 12));
      }

      WORD ewidth = WallFormatMgr.GetWidth(format, DRAW_E_WALL);     
      dstRect.left = ViewportX+origin12.x+e_offset.x;
      dstRect.right = dstRect.left + ewidth;

      srcSurface = getBackgroundSurface(facing, 9);
      if (srcSurface > 0)
      {
        GraphicsMgr.GetSurfaceRect(srcSurface, &imageSrcRect);

        // single band between A->B
        srcRect.left = imageSrcRect.left+(dstRect.left-ViewportX);
        srcRect.right = srcRect.left+ewidth;//imageSrcRect.right;
        srcRect.top = imageSrcRect.top + VERT_OFFSET_A;
        //srcRect.bottom = srcRect.top + AHEIGHT;

        dstRect.top = ViewportY + VERT_OFFSET_A;    
        //dstRect.bottom = dstRect.top + AHEIGHT;
    
        //BltBackgroundSurface(dstSurface, srcSurface, &srcRect, &dstRect, getBackgroundSlot(facing, 9));

        // single band between C->D
        //srcRect.left = imageSrcRect.left+(dstRect.left-ViewportX);
        //srcRect.right = srcRect.left+ewidth;//imageSrcRect.right;
        //srcRect.top = imageSrcRect.top + VERT_OFFSET_C;
        srcRect.bottom = srcRect.top + (CHEIGHT+AHEIGHT+BHEIGHT);
    
        //dstRect.top = ViewportY + VERT_OFFSET_C;    
        dstRect.bottom = dstRect.top + (CHEIGHT+AHEIGHT+BHEIGHT);
    
        BltBackgroundSurface(dstSurface, srcSurface, &srcRect, &dstRect, getBackgroundSlot(facing, 9));
      }

      WORD hwidth = WallFormatMgr.GetWidth(format, DRAW_H_WALL);     
      dstRect.left = ViewportX+origin9.x+h_offset.x;
      dstRect.right = dstRect.left + hwidth;

      srcSurface = getBackgroundSurface(facing, 4);
      if (srcSurface > 0)
      { 
        GraphicsMgr.GetSurfaceRect(srcSurface, &imageSrcRect);

        // single band between B->C
        srcRect.left = imageSrcRect.left+(dstRect.left-ViewportX);
        srcRect.right = srcRect.left+hwidth;
        srcRect.top = imageSrcRect.top + VERT_OFFSET_B;
        srcRect.bottom = srcRect.top + BHEIGHT;
  
        dstRect.top = ViewportY + VERT_OFFSET_B;    
        dstRect.bottom = dstRect.top + BHEIGHT;
    
        BltBackgroundSurface(dstSurface, srcSurface, &srcRect, &dstRect, getBackgroundSlot(facing, 4));
      }
    }
  }
}

//*****************************************************************************
//    NAME: UpdateIndoorView
//
// PURPOSE: 
//
//*****************************************************************************
void UpdateIndoorView(int plyrX, int plyrY, int facing, long int dstSurface)
{
  DetermineView(facing, plyrX, plyrY);
  wallSubstitutionCacheCount = 0;
  RenderBackground(facing, dstSurface);  
  RenderWalls(facing, dstSurface);
/*
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    RunGlobalScript("UpdateIndoorView", "SpecialGraphic", true);
    if (!hookParameters[3].IsEmpty())
    {
      CString fname;
      long l;
      RECT srcRect, dstRect;

      srcRect.left     = atoi(hookParameters[6]);
      srcRect.top      = atoi(hookParameters[7]);
      srcRect.right    = atoi(hookParameters[8]);
      srcRect.bottom   = atoi(hookParameters[9]);

      dstRect.left     = atoi(hookParameters[4]);
      dstRect.top      = atoi(hookParameters[5]);
      dstRect.right    = dstRect.left + srcRect.right - srcRect.left;
      dstRect.bottom   = dstRect.top + srcRect.bottom - srcRect.top;

      fname = hookParameters[3];
      l = GraphicsMgr.AddFileSurface(fname, SmallPicDib, rte.PicArtDir() );
      GraphicsMgr.BlitImage(dstSurface, l, &srcRect, &dstRect);
      GraphicsMgr.ReleaseSurface(l);
    };
  }
  */

#define DisplayCellContents
#ifdef UAFEngine
#ifdef DisplayCellContents
  {
    CELL_LEVEL_CONTENTS* pContents;
    pContents = &globalData.levelInfo.stats[globalData.currLevel].m_cellContents;
    if (pContents->GetCount(party.Posx, party.Posy))

    //HOOK_PARAMETERS hookParameters;
    //SCRIPT_CONTEXT scriptContext;
    //RunGlobalScript("UpdateIndoorView", "SpecialGraphic", true);
    //if (!hookParameters[3].IsEmpty())
    {
      CString fname;
      long l;
      RECT srcRect, dstRect;

      srcRect.left = 0;
      srcRect.top = 0;
      srcRect.right = 47;
      srcRect.bottom = 47;

      dstRect.left = 180;
      dstRect.top = 210;
      dstRect.right = dstRect.left + srcRect.right - srcRect.left;
      dstRect.bottom = dstRect.top + srcRect.bottom - srcRect.top;

      fname = "item_Hammer.png";
      l = GraphicsMgr.AddFileSurface(fname, SmallPicDib, rte.PicArtDir());
      GraphicsMgr.BlitImage(dstSurface, l, &srcRect, &dstRect);
      GraphicsMgr.ReleaseSurface(l);
    };
  }
#endif
#endif




  int zone = levelData.GetCurrZone(plyrX,plyrY);

  if (   (globalData.AutoDarkenViewport)
      && (levelData.zoneData.zones[zone].AllowAutoDarken)
      && (IsTriggerTimeInCurrentTime(globalData.StartDarken, globalData.EndDarken)))
  {
    RECT imageSrcRect = {  ViewportX,  ViewportY,  ViewportX+ViewportWidth, ViewportY+ViewportHeight };
    GraphicsMgr.DarkenDestSurface(globalData.AutoDarkenAmount, &imageSrcRect, dstSurface);
  }
}

///////////////////////////////////////////////////////////////////////
// updateViewport - draws the viewport tiles
//                  that represent the walls, doors, etc. The players 
//                  coords are used to decide which map squares can be
//                  seen, and then the contents of each square gets
//                  blitted to the viewport area surface.
///////////////////////////////////////////////////////////////////////

void updateViewport(int plyrX, int plyrY, int facing, long int dstSurface, BOOL force3Dview)
{
  if ((levelData.area_width == 0) || (levelData.area_height == 0))
  {
    WriteDebugString("Cannot draw viewport, levelData is bogus\n");
    return;
  }

  if (   (plyrX < 0) || (plyrY < 0) 
      || (plyrX >= levelData.area_width) || (plyrY >= levelData.area_height))
  {
    WriteDebugString("Cannot draw viewport, party x,y is bogus\n");
    return;
  }

  if (globalData.levelInfo.stats[globalData.currLevel].overland)
  {
    UpdateOutdoorView(plyrX, plyrY, facing, dstSurface, globalData.useAreaView);
  }
  else
  {
#ifdef UAFEngine // editor uses DirectX Clipper object
    RECT fullRect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
    RECT viewportRect = { ViewportX, ViewportY, ViewportX+ViewportWidth+1, ViewportY+ViewportHeight+1 };
    GraphicsMgr.SetClipRect(&viewportRect);  
#endif
    if (globalData.useAreaView && !force3Dview)
      updateAreaView(plyrX, plyrY, facing, dstSurface);
    else
      UpdateIndoorView(plyrX, plyrY, facing, dstSurface);
#ifdef UAFEngine
    GraphicsMgr.SetClipRect(&fullRect);
#endif
  }
}


//*****************************************************************************
//    NAME: DetermineView
//
// PURPOSE: 
//
//*****************************************************************************
void DetermineView(int direction, int x, int y)
{
  int left  = (direction+3)&3;
  int right = (direction+1)&3;
  int front = (direction+0)&3;
  int dxl=deltaX[left];
  int dxf=deltaX[front];
  int dxr=deltaX[right];
  int dyl=deltaY[left];
  int dyf=deltaY[front];
  int dyr=deltaY[right];

   // Cell 0 is Forward 2 and left 2.
   viewMap[0].x = x+(dxf+dxl)*2;
   viewMap[0].y = y+(dyf+dyl)*2;
   // Cell 1 is forward 2 and right 2.
   viewMap[1].x = x+(dxf+dxr)*2;
   viewMap[1].y = y+(dyf+dyr)*2;
   // Cell 2 is forward 2 and left 1.
   viewMap[2].x = x+dxf+dxf+dxl;
   viewMap[2].y = y+dyf+dyf+dyl;
   // Cell 3 is forward 2 and right 1.
   viewMap[3].x = x+dxf+dxf+dxr;
   viewMap[3].y = y+dyf+dyf+dyr;
   // Cell 4 is forward 2.
   viewMap[4].x = x+dxf+dxf;
   viewMap[4].y = y+dyf+dyf;
   // Cell 5 is forward 1 and left 2.
   viewMap[5].x = x+dxf+dxl+dxl;
   viewMap[5].y = y+dyf+dyl+dyl;
   // Cell 6 is forward 1 and right 2.
   viewMap[6].x = x+dxf+dxr+dxr;
   viewMap[6].y = y+dyf+dyr+dyr;
   // Cell 7 is forward 1 and left 1.
   viewMap[7].x = x+dxf+dxl;
   viewMap[7].y = y+dyf+dyl;
   // Cell 8 is forward 1 and right 1.
   viewMap[8].x = x+dxf+dxr;
   viewMap[8].y = y+dyf+dyr;
   // Cell 9 is forward 1.
   viewMap[9].x = x+dxf;
   viewMap[9].y = y+dyf;
   // Cell 10 is left 1.
   viewMap[10].x = x+dxl;
   viewMap[10].y = y+dyl;
   // Cell 11 is right 1.
   viewMap[11].x = x+dxr;
   viewMap[11].y = y+dyr;
   // Cell 12 is right (no pun intended) here.
   viewMap[12].x = x;
   viewMap[12].y = y;

   // For these two it is significant if they exceed the map boundaries
   // so don't wrap them
   //
   // Cell 13 is Forward 2 and left 3.
   viewMap[13].x = x+(dxf+dxf+dxl+dxl+dxl);
   viewMap[13].y = y+(dyf+dyf+dyl+dyl+dyl);
   // Cell 14 is Forward 2 and right 3.
   viewMap[14].x = x+(dxf+dxf+dxr+dxr+dxr);
   viewMap[14].y = y+(dyf+dyf+dyr+dyr+dyr);

   for (int i=0; i<13; i++)
   {
     // Wrap around the torus.
     viewMap[i].x=(viewMap[i].x+levelData.area_width)%levelData.area_width;
     viewMap[i].y=(viewMap[i].y+levelData.area_height)%levelData.area_height;
   };   
} // DetermineView

///////////////////////////////////////////////////////////////////////
// drawViewport - draws the crossbars and viewport window
//                to the back buffer
///////////////////////////////////////////////////////////////////////
void drawViewport(BOOL drawMiddleVertBar, BOOL drawFrame, BOOL drawViewportPic, BOOL drawMiddleHorzBar)
{
  BOOL IsLargeOverland = 
    (   (globalData.levelInfo.stats[globalData.currLevel].AVStyle==OnlyAreaView)
     && (globalData.levelInfo.stats[globalData.currLevel].overland));

  if (IsLargeOverland)
  {
    drawMiddleVertBar=FALSE;
    drawFrame=FALSE;
  }

  if (drawMiddleVertBar)
    GraphicsMgr.BlitImage(VertBarMidX,VertBarMidY, VBarVPArt.surface, &VertBarSrcShort);

  if (drawFrame && drawViewportPic)
    drawViewportFrame(VPFrameX,VPFrameY, &FrameVPArt);

  if (drawMiddleHorzBar)
  {
    //                    destX,       destY,       source surface,    src RECT
    if (HorzBarSrc2.bottom-HorzBarSrc2.top > 0)
      GraphicsMgr.BlitImage(HorzBarMidX, HorzBarMidY, HBarVPArt.surface, &HorzBarSrc2);
    else
      GraphicsMgr.BlitImage(HorzBarMidX, HorzBarMidY, HBarVPArt.surface, &HorzBarSrc);  
  };
}

///////////////////////////////////////////////////////////////////////
// drawBorder - draws the outer borders to the back buffer
///////////////////////////////////////////////////////////////////////
void drawBorder(void)
{
#ifdef DebugFull
  WriteDebugString("!!drawBorder()\n");
#endif
   //                   destX, destY, SrcSurfaceNum,   src RECT
   GraphicsMgr.BlitImage(  VertBarLeftX,  VertBarLeftY, VBarVPArt.surface, &VertBarSrc );
   GraphicsMgr.BlitImage( VertBarRightX, VertBarRightY, VBarVPArt.surface, &VertBarSrc2 ); // manikus 6-5-12

   GraphicsMgr.BlitImage( HorzBarTopX, HorzBarTopY, HBarVPArt.surface, &HorzBarSrc );

   if (HorzBarSrc3.bottom-HorzBarSrc3.top > 0)
     GraphicsMgr.BlitImage( HorzBarBotX, HorzBarBotY, HBarVPArt.surface, &HorzBarSrc3 );   
   else
     GraphicsMgr.BlitImage( HorzBarBotX, HorzBarBotY, HBarVPArt.surface, &HorzBarSrc );   
}

//*****************************************************************************
//    NAME: drawViewportFrame
//
// PURPOSE: 
//
//*****************************************************************************
void drawViewportFrame(int x, int y, PicDataType* pic)
{
  GraphicsMgr.BlitImage(x,y, pic->surface, &ViewportFrameSrc);
}

#ifdef UAFEngine
//*****************************************************************************
//    NAME: Mouse2Viewport
//
// PURPOSE: 
//
//*****************************************************************************
int Mouse2Viewport(int x, int y)
{
  // If x,y not in viewport return -1;
  // Return 0 through 8 for the 3x3 matrix
  // of rectangles in the viewport
  //  
  // 0  1  2
  // 3  4  5
  // 6  7  8

  // make x,y relative to viewport
  x -= ViewportX;
  if ((x <0 ) || (x >= ViewportWidth)) return -1;
  y -= ViewportY;
  if ((y < 0) || (y >= ViewportHeight)) return -1;

  x = 3 * x / ViewportWidth;
  y = 3 * y / ViewportHeight;

  return 3 * y + x;
}

// return relative direction to mouse click from
// current party position on overland map.
//
// 0-8 for NW-SE 
//*****************************************************************************
//    NAME: Mouse2OverlandMap
//
// PURPOSE: 
//
//*****************************************************************************
int Mouse2OverlandMap(int x, int y)
{
  // make x,y relative to map origin
  x -= MapScreenStartX;
  if ((x <0 ) || (x >= MapScreenWidth)) return -1;
  y -= MapScreenStartY;
  if ((y < 0) || (y >= MapScreenHeight)) return -1;
    
  if (UseCustomCursor)
  {
    x += (CursorArt.FrameWidth/2); // offset to center of cursor
    y += (CursorArt.FrameHeight/2);
  }

  // make party coords relative to map origin
  int px = MapPartyX - MapScreenStartX + 8; // offset to center of image
  int py = MapPartyY - MapScreenStartY + 8;

  // if pixel-level direction finding is used
  // it takes a miracle mouse click to move
  // N,S,E, and W. Change screen coords into
  // SQUARESIZE boxes and then find direction.
  x /= SQUARESIZE;
  y /= SQUARESIZE;
  px /= SQUARESIZE;
  py /= SQUARESIZE;

  //
  // NW  N  NE
  // W      E
  // SW  S  SE
  //  
  switch (GetDir(px,py,  x,y))
  {
  case PathNW:     return 0;
  case PathNorth:  return 1;
  case PathNE:     return 2;
  case PathWest:   return 3;
  case PathBAD:    return 4; // px=x and py=y, bad for pathfinding, ok here
  case PathEast:   return 5;
  case PathSW:     return 6; 
  case PathSouth:  return 7;
  case PathSE:     return 8;
  }  
  return -1;
}
#endif

//*****************************************************************************
//    NAME: CheckMultHorzBarFormat
//
// PURPOSE: 
//
//*****************************************************************************
void CheckMultHorzBarFormat()
{
  RECT tmpRect;
  BOOL success = GraphicsMgr.GetSurfaceRect(HBarVPArt.surface, &tmpRect);
  
  if (   (!success) 
      || ((tmpRect.bottom - tmpRect.top) <= 15)) // only one horz bar, not 3
  {
    // disable extended horz bar usage by clearing source rects
    HorzBarSrc2.top=HorzBarSrc2.bottom=HorzBarSrc2.left=HorzBarSrc2.right=-1;
    HorzBarSrc3.top=HorzBarSrc3.bottom=HorzBarSrc3.left=HorzBarSrc3.right=-1;  
  }
}

//*****************************************************************************
//    NAME: CheckMultVertBarFormat
//
// PURPOSE: determine if only one long vertical bar is present in 
// source graphic
//
//*****************************************************************************
void CheckMultVertBarFormat()
{
  RECT tmpRect;
  BOOL success = GraphicsMgr.GetSurfaceRect(VBarVPArt.surface, &tmpRect);
  
  if (   (!success) 
      || ((tmpRect.right - tmpRect.left) <= 31)) // only one long and short vert bar
  {
    // disable extended vert bar usage by clearing source rects
    VertBarSrc2.top=VertBarSrc2.bottom=VertBarSrc2.left=VertBarSrc2.right=-1;
  }
}
