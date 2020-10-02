/******************************************************************************
* Filename: Viewport.h
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
#ifndef __VIEWPORT_H__
#define __VIEWPORT_H__

#include "Graphics.h"
#include "PicSlot.h"

//  Coords of viewport - not the viewport frame
//  all tiles will be drawn relative to the top,left coords (x,y)
extern int ViewportX;
extern int ViewportY;
extern int ViewportWidth;
extern int ViewportHeight;
extern int BackgroundX;
extern int BackgroundY;
extern int BackgroundWidth;
extern int BackgroundHeight;
extern int PartyNames_nx; //DLD 12/20/00 
extern int PartyNames_ny; // 
extern int PartyNames_wx; // 
extern int PartyNames_wy; // 
extern int LocationText_x; // 
extern int LocationText_y; // 
extern int BigPic_w;      // 
extern int BigPic_h;      // 
extern int BigPic_x;      // 
extern int BigPic_y;      // 
extern int VersionText_x; // 
extern int VersionText_y; //End Add 

struct SELECTION_PAGE_FORMAT
{
  SELECTION_PAGE_FORMAT(void)
  {
    Title_x = 18;
    Title_y = 18;
    Title_Format = "%s";
    Header_x = 18;
    Header_y = 50;
    Table_x = 18;
    Table_y = 70;
    Table_Format = "%s";
    Table_Selected_Format = "/h%s";
    Menu_x = -2; // Default vertical menu.
    Menu_y = 0;
    Lines_Per_Page = 14;
    Line_Spacing = 18;
    Menu_Spacing = 18;
  };
  int      Title_x;
  int      Title_y;
  CString  Title_Format;
  int      Header_x;
  int      Header_y;
  int      Header2_x;
  int      Header2_y;
  int      Header3_x;
  int      Header3_y;
  CString  Header_Format;
  CString  Header2_Format;
  CString  Header3_Format;
  int      Table_x;
  int      Table_y;
  int      Table2_x;
  int      Table2_y;
  int      Table3_x;
  int      Table3_y;
  CString  Table_Format;
  CString  Table2_Format;
  CString  Table3_Format;
  CString  Table_Selected_Format;
  CString  Table2_Selected_Format;
  CString  Table3_Selected_Format;
  int      Menu_x;
  int      Menu_y;
  int      Menu_Spacing;
  int      Lines_Per_Page;
  int      Line_Spacing;
};

extern SELECTION_PAGE_FORMAT Select_Race_Format;
extern SELECTION_PAGE_FORMAT Select_Choosestats_Format;
extern SELECTION_PAGE_FORMAT Select_Class_Format;
extern SELECTION_PAGE_FORMAT Select_Alignment_Format;
extern SELECTION_PAGE_FORMAT Select_Gender_Format;
extern SELECTION_PAGE_FORMAT Select_Main_Function_Format;
extern SELECTION_PAGE_FORMAT Select_Add_Character_Format;
//   Coords of the frame that surrounds the viewport, and the horz & vert crossbars

extern int HorzBarTopX;
extern int HorzBarTopY;
extern int HorzBarBotX;
extern int HorzBarBotY;
extern int VertBarLeftX;
extern int VertBarLeftY;
extern int VertBarRightX;
extern int VertBarRightY;
extern int VertBarMidX;
extern int VertBarMidY;
extern int HorzBarMidX;
extern int HorzBarMidY;
extern int VPFrameX;
extern int VPFrameY;
extern int VPFrameXOffset;
extern int VPFrameYOffset;


extern int HorzBarCombatTopX;
extern int HorzBarCombatTopY;
extern int HorzBarCombatBotX;
extern int HorzBarCombatBotY;
extern int VertBarCombatLeftX;
extern int VertBarCombatLeftY;
extern int VertBarCombatRightX;
extern int VertBarCombatRightY;
extern int VertBarCombatMidX;
extern int VertBarCombatMidY;


// used for area mapping, and doing the math that allows
// determination of which square on the map was clicked by the mouse
enum { SQUARESIZE = 16, OFFSET = 0 };	// manikus 6-4-12

//
// holds the coords for drawing tiles in the regular viewport
//
struct point_type 
{
  int x,y;
};

void ConfigureArtCoords();
void drawBorder();
void drawViewport(BOOL drawMiddleVertBar, BOOL drawFrame, BOOL drawViewportPic, BOOL drawMiddleHorzBar);
void drawViewportFrame(int x, int y);
void DetermineView(int facing,int x, int y);
void updateViewport(int plyrX, int plyrY, int facing, long int dstSurface, BOOL force3Dview);
void updateAreaView(int plyrX, int plyrY, int facing, long int dstSurface=BACKBUFFERNUM);
int Mouse2Viewport(int x, int y); // 3 rows of 3 rectangles in viewport.
int Mouse2OverlandMap(int x, int y);
void CheckMultHorzBarFormat();

inline void TranslateRect(int x, int y, const RECT &origRect, RECT &newRect)
{
  newRect.top = origRect.top + y;
  newRect.bottom = origRect.bottom + y;
  newRect.left = origRect.left + x;
  newRect.right = origRect.right + x;
}

const BYTE DEFAULT_FORMAT_TYPE = 0;

struct WALL_FORMAT_TYPE
{
  WALL_FORMAT_TYPE() { Clear(); }

  void Clear() 
  { 
    memset(m_Rects,0,sizeof(m_Rects)); 
    memset(m_Offsets,0,sizeof(m_Offsets)); 
    memset(viewport_coords,0,sizeof(viewport_coords)); 
    ImageHeight=ImageWidth=0;
    DistantWallCount=5;
  }
  enum { MAX_SLOT_TYPES=16 };
  WORD ImageWidth;
  WORD ImageHeight;
  WORD DistantWallCount;
  RECT m_Rects[MAX_SLOT_TYPES];
  point_type m_Offsets[MAX_SLOT_TYPES];
  point_type viewport_coords[15];
};

class WallFormatDataMgr
{
public:
  WallFormatDataMgr() { Clear(); }
  ~WallFormatDataMgr() { Clear(); }

  WORD GetCount() { return m_Data.GetSize(); }
  void Clear()
  {
    m_Data.RemoveAll();
  }

  BYTE GetFormat(WORD width, WORD height)
  {
    BYTE format = DEFAULT_FORMAT_TYPE;
    // skip first format which is the default layout
    for (int i=1;i<m_Data.GetSize();i++)
    {
      if (   (width == m_Data[i].ImageWidth) 
          && (height == m_Data[i].ImageHeight))
        return i;
    }
    return format;
  }
  BOOL AddFormat(WALL_FORMAT_TYPE &data)
  {
    // make sure default data is added first
    m_Data.Add(data); return TRUE;
  }
  WORD GetDistantWallCount(BYTE Format)
  {
    ASSERT (Format < m_Data.GetSize());
    if (Format >= m_Data.GetSize()) Format = 0;
    return (m_Data[Format].DistantWallCount);
  }
  RECT &GetRect(BYTE Format, BYTE Type)
  {
    ASSERT (Format < m_Data.GetSize());
    ASSERT (Type <= WALL_FORMAT_TYPE::MAX_SLOT_TYPES);
    Type--;
    if (Format >= m_Data.GetSize()) Format = 0;
    return (m_Data[Format].m_Rects[Type]);
  }
  point_type GetOffset(BYTE Format, BYTE Type)
  {
    ASSERT (Format < m_Data.GetSize());
    ASSERT (Type <= WALL_FORMAT_TYPE::MAX_SLOT_TYPES);
    Type--;
    if (Format >= m_Data.GetSize()) Format = 0;
    return (m_Data[Format].m_Offsets[Type]);
  }
  point_type GetSquareOrigin(BYTE Format, BYTE Square)
  {
    ASSERT (Format < m_Data.GetSize());
    ASSERT (Square < 15);
    if (Format >= m_Data.GetSize()) Format = 0;
    if (Square >= 15) Square = 0;
    return (m_Data[Format].viewport_coords[Square]);
  }
  WORD GetWidth(BYTE Format, BYTE Type) 
  {    
    ASSERT (Format < m_Data.GetSize());
    ASSERT (Type <= WALL_FORMAT_TYPE::MAX_SLOT_TYPES);
    Type--;
    if (Format >= m_Data.GetSize()) Format = 0;
    return (m_Data[Format].m_Rects[Type].right-m_Data[Format].m_Rects[Type].left);
  }
  WORD GetHeight(BYTE Format, BYTE Type)
  {    
    ASSERT (Format < m_Data.GetSize());
    ASSERT (Type <= WALL_FORMAT_TYPE::MAX_SLOT_TYPES);
    Type--;
    if (Format >= m_Data.GetSize()) Format = 0;
    return (m_Data[Format].m_Rects[Type].bottom-m_Data[Format].m_Rects[Type].top);
  }

private:
  CArray<WALL_FORMAT_TYPE,WALL_FORMAT_TYPE&> m_Data;
};

extern WallFormatDataMgr WallFormatMgr;

enum { 
  DRAW_A_WALL = 1,
  DRAW_B_WALL = 2,
  DRAW_C_WALL = 3,
  DRAW_D_WALL = 4,
  DRAW_E_WALL = 5,
  DRAW_F_WALL = 6,
  DRAW_G_WALL = 7,
  DRAW_H_WALL = 8,
  DRAW_I_WALL = 9,
  DRAW_J_WALL = 10,
  DRAW_K_WALL = 11,
  DRAW_L_WALL = 12,
  DRAW_M_WALL = 13,
  DRAW_N_WALL = 14,
  DRAW_O_WALL = 15,
  DRAW_P_WALL = 16
};

extern RECT ArrowSize;
extern RECT ArrowRects[4];

// viewport/combat bars source rect
extern RECT VertBarSrc;
extern RECT VertBarSrcShort;
extern RECT HorzBarSrc;
extern RECT VertBarCombatSrc;
extern RECT VertBarCombatSrc2;
extern RECT VertBarCombatSrcShort;
extern RECT HorzBarCombatSrc;
extern RECT HorzBarCombatSrc2;


#endif