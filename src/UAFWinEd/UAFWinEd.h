/******************************************************************************
* Filename: UAFWinEd.h
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

#if !defined(AFX_UAFWINED_H__56306A07_E92E_11D3_891D_00105AA4563A__INCLUDED_)
#define AFX_UAFWINED_H__56306A07_E92E_11D3_891D_00105AA4563A__INCLUDED_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

//#include "resource.h"       // main symbols

//#include "tfxdatatip.h"
#include "GlobText.h"
#include "ColorBox.h"

extern COLORREF m_Colors[MAX_COLOR_SLOTS];
COLORREF GetWallColor(int slot);
COLORREF GetBackdropColor(int slot);
COLORREF GetObstructionColor(int slot);
COLORREF GetZoneColor(int slot);
COLORREF GetEPColor(int slot);

HBRUSH GetWallColorBrush(int slot);
HBRUSH GetBackdropColorBrush(int slot);
HBRUSH GetObstructionColorBrush(int slot);
HBRUSH GetZoneColorBrush(int slot);
HBRUSH GetEPColorBrush(int slot);

extern BOOL PassThruWalls;
extern BOOL DoubleFillWallSlot;
extern BOOL QuadFillBGSlot;

extern int currBkGnd;
extern int currCombat;
extern int currWallSlot;
extern int currBlockage;
extern int currZone;
extern int currEntryPoint;
extern int currX;
extern int currY;
extern int currFacing;

extern int m_mode;
extern int m_mapType;
extern bool m_accessMode;
extern BOOL MODIFIED_FLAG;

void SetModified(void);
void ClearModified(void);
BOOL Modified(void);
#define SET_MODIFIED   SetModified()
#define CLEAR_MODIFIED ClearModified()
#define IS_MODIFIED    Modified()

extern BOOL m_KwikKlik;
extern BOOL m_BLOCK_Type;
extern BOOL m_TileMap;
//struct LEVEL;

// this mode determines which type of object will
// be placed on the map.
enum { 
  EVENT_MODE,        // overland, dungeon
  WALL_MODE,         // dungeon
  ZONE_MODE,         // overland, dungeon
  BG_MODE,           // dungeon
  START_LOC_MODE,    // overland, dungeon
  ENTRY_POINT_MODE,  // overland, dungeon
  BLOCKAGE_MODE,     // overland
  EMPTY 
};

// this mode determines which type of map the
// objects will be added to.
enum {
  DUNGEON_TYPE,
  OVERLAND_TYPE
};

struct UndoDataType
{
  UndoDataType() { Clear(); }
  void Clear() { type=EMPTY;x=0;y=0;doubleFill=FALSE; 
            prevFill=0;facing=0;prevDoubleFill=0;
            prevBlockage=0;prevDoubleBlockage=0; }
  int type;
  int x,y;
  int facing;
  int prevBlockage;
  int prevDoubleBlockage;
  int prevFill;
  int prevDoubleFill;
  BOOL doubleFill;
};

extern CList<UndoDataType, UndoDataType&> UndoData;
void AddUndoData(UndoDataType &data);
extern int MAX_UNDO_ACTIONS;

class ResourceSaveType
{
public:
   ResourceSaveType() { Clear(); }
   ~ResourceSaveType() { Clear(); }
   void Clear() { name=""; saveName=""; }

   ResourceSaveType& operator =(const ResourceSaveType& src) 
   {
     if (&src == this) 
       return *this;
     Clear();
     if (!src.name.IsEmpty()) name=src.name;
     if (!src.saveName.IsEmpty()) saveName=src.saveName;
     return *this;
   }

   ResourceSaveType(const ResourceSaveType& src) { *this=src; }

   CString name;
   CString saveName;
};

#include "UAFWinEdDoc.h"

// how many characters of text are displayed in option buttons
const int MAX_MEDITBUTTON_TEXT = 15;
const int MAX_QUESTTRIGGERNAME_TEXT = 35;

class CEditorOptionsData
{
public:

  CEditorOptionsData();
  ~CEditorOptionsData() { }
  void Clear();
  CEditorOptionsData& operator =(const CEditorOptionsData& src);
  CEditorOptionsData(const CEditorOptionsData& src) { *this=src; }
  BOOL Serialize(BOOL IsStoring); // save/load using registry

  BOOL m_AutoReloadDesign;
  BOOL m_LastSaveSuccess;
  BOOL m_LastLoadSuccess;
  CString m_LastDesign;
  DWORD m_EditorScreenRes; // what res is the editor running in?
  DWORD m_LastDesignLevel;
  DWORD m_LastDesignX;
  DWORD m_LastDesignY;
  DWORD m_LastBkgnd;
  DWORD m_LastWall;
  DWORD m_LastFacing;
  DWORD m_LastMode;
  DWORD m_LastKwikKlik;
  DWORD m_LastBlockType;
  DWORD m_LastTileMap;
  DWORD m_LastPassWalls;
  DWORD m_LastDoubleFillWall;
  DWORD m_LastQuadFileBg;
  DWORD m_LastAreaViewMode;
  DWORD m_LastAutoLevelSaveWarn;
  DWORD m_LastEventTypeUsed;
};

extern CEditorOptionsData EditorOptionsData;

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdApp:
// See UAFWinEd.cpp for the implementation of this class
//

class CUAFWinEdApp : public CWinApp
{
public:
	CUAFWinEdApp();

//  void WinHelp( DWORD dwData, UINT nCmd = HELP_CONTEXT );
  CUAFWinEdDoc* m_pDoc;

  CUAFWinEdDoc *GetDocPtr();
  void StatusMsg(CString text);
  void HELP(int helpID);
  void SetTitle(void);
  CString LoadDefaultDBFiles();
  bool OpenDesign(CString filename);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUAFWinEdApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CUAFWinEdApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppExit();
	afx_msg void OnAPPHelpWallEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



void FillControl(CColorBox *pWnd, COLORREF fillColor);
void getBaseName(const char *full, char *base, int baseLen, BOOL LeaveExt=FALSE);
void fillDefaultLevelData(LEVEL &data,
                          int level, 
                          int width=0x7fffffff,  // Use MAX_AREA_WIDTH
                          int height=0x7fffffff, // Use MAX_AREA_HEIGHT
                          BOOL overland=FALSE, 
                          BOOL Fill=TRUE);
void fillDefaultArtSlots(LEVEL &data, BOOL overland);
long int FindFirstLoadedBackground();
long int FindFirstLoadedWall();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UAFWINED_H__56306A07_E92E_11D3_891D_00105AA4563A__INCLUDED_)
