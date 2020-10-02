/******************************************************************************
* Filename: UAFWinEdView.cpp
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
#include "resource.h"

#include "UAFWinEd.h"

#include "MainFrm.h"
#include "UAFWinEdDoc.h"
#include "UAFWinEdView.h"
#include "GlobalData.h"
#include "Level.h"
#include "Viewport.h"
#include "Graphics.h"
#include "EditWallSlots.h"
#include "ZoneDlg.h"
#include "EditBGSlots.h"
#include "EntryPointDlg.h"
#include "AccessDlg.h"
#include "EventViewer.h"
#include "SoundMgr.h"
#include "Party.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// These variables are for horizontal scrolling. 

static int xMinScroll = 0;       // minimum horizontal scroll value 
static int xCurrentScroll = 0;   // current horizontal scroll value 
static int xMaxScroll = 0;       // maximum horizontal scroll value 
 
// These variables are for vertical scrolling. 
 
static int yMinScroll=0;       // minimum vertical scroll value 
static int yCurrentScroll=0;   // current vertical scroll value 
static int yMaxScroll=0;       // maximum vertical scroll value 

const  int LINE_SIZE = 1; // map cells to arrow up/down by
static int PAGE_SIZE = 5; // 

static int MapBufferWidth=0;
static int MapBufferHeight=0;

CList<UndoDataType, UndoDataType&> UndoData;
UndoDataType tempUndo;
int MAX_UNDO_ACTIONS = 10;

static bool wallUndone=false;  // Damned if I know how to do this right.  PAul
static bool leftbuttondown = false;
static bool rightbuttondown = false;
static bool control=false;
static bool shift=false;
static bool hkey=false;
static bool vkey=false;
static unsigned int keypresses[8];

int EDITOR_RESOURCE_ID = IDD_3DPICDLG;

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdView
IMPLEMENT_DYNCREATE(CUAFWinEdView, CFormView)

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdView construction/destruction

CUAFWinEdView::CUAFWinEdView()
  : m_IDD(EDITOR_RESOURCE_ID), CFormView(EDITOR_RESOURCE_ID)
{
   //{{AFX_DATA_INIT(CUAFWinEdView)
   m_ModeText = _T("Wall Mode");
   m_ObText = _T("");
   m_ZoneText = _T("");
   m_CurrX = _T("");
   m_CurrY = _T("");
   m_CurrFacing = _T("");
	m_DesignName = _T("");
	m_LevelName = _T("");
	m_Select1 = FALSE;
	m_Select10 = FALSE;
	m_Select11 = FALSE;
	m_Select12 = FALSE;
	m_Select13 = FALSE;
	m_Select14 = FALSE;
	m_Select15 = FALSE;
	m_Select16 = FALSE;
	m_Select2 = FALSE;
	m_Select3 = FALSE;
	m_Select4 = FALSE;
	m_Select5 = FALSE;
	m_Select6 = FALSE;
	m_Select7 = FALSE;
	m_Select8 = FALSE;
	m_Select9 = FALSE;
	m_BGSlot = _T("");
	m_WallSlot = _T("");
	m_BlockageBlk = FALSE;
	m_BlockageOpen = FALSE;
	m_CellStatusText = _T("");
	m_MaxHeight = _T("0");
	m_MaxWidth = _T("0");
	m_ShowDistantBackdrops = FALSE;
	//}}AFX_DATA_INIT

   m_oldWall=m_oldBlockage=m_oldBG=-1;
   m_oldZone=m_oldMode=m_oldX=m_oldY=-1;
   m_oldFacing=m_oldEP=-1;
   m_oldMaxWidth=m_oldMaxHeight=m_oldLevel=-1;
   m_picX = 0; m_picY=0;
   m_mouseX=m_mouseY=0;
}

CUAFWinEdView::~CUAFWinEdView()
{
}



void CUAFWinEdView::DoDataExchange(CDataExchange* pDX)
{
   CFormView::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CUAFWinEdView)
	DDX_Control(pDX, IDC_TOGGLEBGBANDS, m_ShowBands);
	DDX_Control(pDX, IDC_MAP_SCROLL_PIC, m_MapPicture);
	DDX_Control(pDX, IDC_COLOROPENBLK, m_ColorOpen);
	DDX_Control(pDX, IDC_COLORBLOCKBLK, m_ColorBlk);
	DDX_Control(pDX, IDC_VERT_SCROLL, m_VertScroll);
	DDX_Control(pDX, IDC_HORZ_SCROLL, m_HorzScroll);
	DDX_Control(pDX, IDC_COLOR9, m_Color9);
	DDX_Control(pDX, IDC_COLOR8, m_Color8);
	DDX_Control(pDX, IDC_COLOR7, m_Color7);
	DDX_Control(pDX, IDC_COLOR6, m_Color6);
	DDX_Control(pDX, IDC_COLOR5, m_Color5);
	DDX_Control(pDX, IDC_COLOR4, m_Color4);
	DDX_Control(pDX, IDC_COLOR3, m_Color3);
	DDX_Control(pDX, IDC_COLOR2, m_Color2);
	DDX_Control(pDX, IDC_COLOR16, m_Color16);
	DDX_Control(pDX, IDC_COLOR15, m_Color15);
	DDX_Control(pDX, IDC_COLOR14, m_Color14);
	DDX_Control(pDX, IDC_COLOR13, m_Color13);
	DDX_Control(pDX, IDC_COLOR12, m_Color12);
	DDX_Control(pDX, IDC_COLOR11, m_Color11);
	DDX_Control(pDX, IDC_COLOR10, m_Color10);
	DDX_Control(pDX, IDC_COLOR1, m_Color1);
  DDX_Control(pDX, IDC_CellStatus, m_CellStatus);
	DDX_Control(pDX, IDC_3DPicture, m_PictureFrame);
   DDX_Control(pDX, IDC_BGPIC, m_BGPic);
   DDX_Control(pDX, IDC_ZONECOLOR, m_ZoneColor);
   DDX_Control(pDX, IDC_WALLSLOTPIC, m_WallSlotPic);
   DDX_Control(pDX, IDC_WALLSLOTCOLOR, m_WallSlotColor);
   DDX_Control(pDX, IDC_OBCOLOR, m_ObColor);
   DDX_Control(pDX, IDC_BGCOLOR, m_BGColor);
   DDX_Control(pDX, IDC_PLACERIGHT, m_PlaceRight);
   DDX_Control(pDX, IDC_PLACELEFT, m_PlaceLeft);
   DDX_Control(pDX, IDC_SELECTCURRENT, m_SelectButton);
   DDX_Control(pDX, IDC_PLACECURRENT, m_PlaceButton);
   DDX_Text(pDX, IDC_ModeText, m_ModeText);
   DDX_Text(pDX, IDC_OBTEXT, m_ObText);
   DDX_Text(pDX, IDC_ZONETEXT, m_ZoneText);
   DDX_Text(pDX, IDC_CURRX, m_CurrX);
   DDX_Text(pDX, IDC_CURRY, m_CurrY);
   DDX_Text(pDX, IDC_CURRFACING, m_CurrFacing);
	DDX_Text(pDX, IDC_DESIGNNAME, m_DesignName);
	DDX_Text(pDX, IDC_LEVELNAME, m_LevelName);
	DDX_Check(pDX, IDC_SELECT1, m_Select1);
	DDX_Check(pDX, IDC_SELECT10, m_Select10);
	DDX_Check(pDX, IDC_SELECT11, m_Select11);
	DDX_Check(pDX, IDC_SELECT12, m_Select12);
	DDX_Check(pDX, IDC_SELECT13, m_Select13);
	DDX_Check(pDX, IDC_SELECT14, m_Select14);
	DDX_Check(pDX, IDC_SELECT15, m_Select15);
	DDX_Check(pDX, IDC_SELECT16, m_Select16);
	DDX_Check(pDX, IDC_SELECT2, m_Select2);
	DDX_Check(pDX, IDC_SELECT3, m_Select3);
	DDX_Check(pDX, IDC_SELECT4, m_Select4);
	DDX_Check(pDX, IDC_SELECT5, m_Select5);
	DDX_Check(pDX, IDC_SELECT6, m_Select6);
	DDX_Check(pDX, IDC_SELECT7, m_Select7);
	DDX_Check(pDX, IDC_SELECT8, m_Select8);
	DDX_Check(pDX, IDC_SELECT9, m_Select9);
	DDX_Text(pDX, IDC_BACKDROPSLOT, m_BGSlot);
	DDX_Text(pDX, IDC_WALLSLOT, m_WallSlot);
	DDX_Check(pDX, IDC_BLOCKAGEBLK, m_BlockageBlk);
	DDX_Check(pDX, IDC_BLOCKAGEOPEN, m_BlockageOpen);
	DDX_Text(pDX, IDC_CellStatus, m_CellStatusText);
	DDX_Text(pDX, IDC_MAXHEIGHT, m_MaxHeight);
	DDX_Text(pDX, IDC_MAXWIDTH, m_MaxWidth);
	DDX_Check(pDX, IDC_TOGGLEBGMETHOD, m_ShowDistantBackdrops);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUAFWinEdView, CFormView)
	//{{AFX_MSG_MAP(CUAFWinEdView)
   ON_WM_LBUTTONDOWN()
   ON_WM_RBUTTONDOWN()
  ON_BN_CLICKED(IDC_PLACECURRENT, OnPlacecurrent)
  ON_BN_CLICKED(IDC_PLACELEFT, OnPlaceleft)
  ON_BN_CLICKED(IDC_PLACERIGHT, OnPlaceright)
  ON_BN_CLICKED(IDC_SELECTCURRENT, OnSelectcurrent)
  ON_WM_MOVE()
	ON_BN_CLICKED(IDC_SELECT1, OnSelect1)
	ON_BN_CLICKED(IDC_SELECT10, OnSelect10)
	ON_BN_CLICKED(IDC_SELECT11, OnSelect11)
	ON_BN_CLICKED(IDC_SELECT12, OnSelect12)
	ON_BN_CLICKED(IDC_SELECT13, OnSelect13)
	ON_BN_CLICKED(IDC_SELECT14, OnSelect14)
	ON_BN_CLICKED(IDC_SELECT15, OnSelect15)
	ON_BN_CLICKED(IDC_SELECT16, OnSelect16)
	ON_BN_CLICKED(IDC_SELECT2, OnSelect2)
	ON_BN_CLICKED(IDC_SELECT3, OnSelect3)
	ON_BN_CLICKED(IDC_SELECT4, OnSelect4)
	ON_BN_CLICKED(IDC_SELECT5, OnSelect5)
	ON_BN_CLICKED(IDC_SELECT6, OnSelect6)
	ON_BN_CLICKED(IDC_SELECT7, OnSelect7)
	ON_BN_CLICKED(IDC_SELECT8, OnSelect8)
	ON_BN_CLICKED(IDC_SELECT9, OnSelect9)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BLOCKAGEBLK, OnBlockageblk)
	ON_BN_CLICKED(IDC_BLOCKAGEOPEN, OnBlockageopen)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_ACCESSOPTIONS, OnAccessoptions)
  ON_COMMAND(ID_MAPpop_EditEvent, OnMAPpopEditEvent)
  ON_COMMAND(ID_MAPpopCancel, OnMAPpopCancel)
	ON_BN_CLICKED(IDC_TOGGLEBGMETHOD, OnTogglebgmethod)
	ON_BN_CLICKED(IDC_TOGGLEBGBANDS, OnTogglebgbands)
	ON_WM_LBUTTONUP()
	ON_WM_KILLFOCUS()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CUAFWinEdView::OnInitialUpdate() 
{
   // initialize the DirectDraw object
   if (!GraphicsMgr.InitGraphicsWindowed(AfxGetMainWnd()->m_hWnd))
     return;

   pSndMgr = new SoundMgr(AfxGetMainWnd()->m_hWnd);

   CFormView::OnInitialUpdate();
   GetParentFrame()->RecalcLayout();
   ResizeParentToFit( /*FALSE*/ );

   if (!Initialize())
   {
      WriteDebugString("Failed to initialize CUAFWinEdView in OnInitialUpdate\n");
      return;
   }
   {
      int startSlot = currWallSlot/16 * 16;
      FillControl(&m_Color1,GetWallColor(startSlot+0));
      FillControl(&m_Color2,GetWallColor(startSlot+1));
      FillControl(&m_Color3,GetWallColor(startSlot+2));
      FillControl(&m_Color4,GetWallColor(startSlot+3));
      FillControl(&m_Color5,GetWallColor(startSlot+4));
      FillControl(&m_Color6,GetWallColor(startSlot+5));
      FillControl(&m_Color7,GetWallColor(startSlot+6));
      FillControl(&m_Color8,GetWallColor(startSlot+7));
      FillControl(&m_Color9,GetWallColor(startSlot+8));
      FillControl(&m_Color10,GetWallColor(startSlot+9));
      FillControl(&m_Color11,GetWallColor(startSlot+10));
      FillControl(&m_Color12,GetWallColor(startSlot+11));
      FillControl(&m_Color13,GetWallColor(startSlot+12));
      FillControl(&m_Color14,GetWallColor(startSlot+13));
      FillControl(&m_Color15,GetWallColor(startSlot+14));   
      FillControl(&m_Color16,GetWallColor(startSlot+15));
   };

   m_CellStatus.SetColor(RGB(224,224,192));

   m_ColorBlk.SetColor(GetObstructionColor(BlockedBlk));
   m_ColorOpen.SetColor(GetObstructionColor(OpenBlk));
   ResetView();
   UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdView drawing

void CUAFWinEdView::OnDraw(CDC* pDC)
{
	//CUAFWinEdDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
	
  static int oldLevel = -99;
  oldLevel = globalData.currLevel;
}

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdView diagnostics

#ifdef _DEBUG
void CUAFWinEdView::AssertValid() const
{
	CFormView::AssertValid();
}

void CUAFWinEdView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CUAFWinEdDoc* CUAFWinEdView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUAFWinEdDoc)));
	return (CUAFWinEdDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdView message handlers
BOOL CUAFWinEdView::Initialize(void)
{
  // this stuff uses the EditorArt string to set default
  // art/sound paths, and must be called after the config.txt
  // file is loaded.
  clearData(spellData);
  clearData(monsterData);
  clearData(itemData);

  BOOL ReloadSuccess = FALSE;

  if (   (EditorOptionsData.m_AutoReloadDesign)
      && (EditorOptionsData.m_LastSaveSuccess)
      && (!EditorOptionsData.m_LastDesign.IsEmpty()))
  {
    CString testDesign;
    testDesign = rte.RootDir() + EditorOptionsData.m_LastDesign + ".dsn\\game.dat";

    // check to see if game.dat is present before trying to load a design
    if (FileExists(testDesign))
    {
      ReloadSuccess = 
        ((CMainFrame*)(AfxGetMainWnd()))->LoadDesign(EditorOptionsData.m_LastDesign, EditorOptionsData.m_LastDesignLevel);
    }
    if (!ReloadSuccess)
      EditorOptionsData.m_LastDesign=""; // don't try this one again
  }

  if (!ReloadSuccess)
  {
    // Perhaps we can load the design listed on the command line.
    CString testDesign;
    testDesign = rte.DataDir() + "game.dat";

    // check to see if game.dat is present before trying to load a design
    if (FileExists(testDesign))
    {
      ReloadSuccess = 
        ((CMainFrame*)(AfxGetMainWnd()))->LoadDesign(rte.DesignDir(), -1);
    };
  };


  if (!ReloadSuccess)
  {
    CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
    if (cmdLine.m_initializeGame)
    {
    flushOldImages();
    clearDesign();    
      pApp->LoadDefaultDBFiles(); 
      SetModified();
    }
    else
    {
      MsgBoxInfo("Cannot find 'game.dat' and no initial option on command line");
      return FALSE;
    };
  }

  if (!m_PictureFrame.Initialize())
  {
    WriteDebugString("Failed call to m_PictureFrame.Initialize() in CUAFWinEdView::Initialize\n");
    return FALSE;
  }

   m_oldBG = -1;
   m_oldWall = -1;
   CheckButtonStates();   
   Invalidate(FALSE);

//#define TEST_SPELL_LIMITS

#ifdef TEST_SPELL_LIMITS

  CHARACTER dude;
  dude.SetAlignment(ChaoticNeutral);
  dude.race(Human);
  dude.SetGender(Male);
  dude.SetClass(Cleric);
  dude.generateNewCharacter(GetDesignCharStartExp(),GetDesignCharStartExpType());
  ASSERT( dude.GetMaxHitPoints() > 0 );
  dude.SetName("Super Spud");
  //dude.SetWis(16);
  //dude.UpdateSpellAbility();
  
  int classflag = CharClassToClassFlag(dude.GetClass());

  TRACE("CAN USE SPELLS: %i\n", dude.spells.spellLimits.ValidSpellClass(classflag));
  {
    //To do this right.....we need to trapse through the character's baseclasses,
    // make a list of all the spell schools, and then get the prime score
    // for each of those schools.
    qSpellSchoolList = quickPeek.SpellSchoolList(dude.classID);
    n = qSpellSchoolList.GetSpellSchoolCount();
    for (i = 0; i<n; i++)
    {
      schoolID = qSchoolList.GetSchoolID(i);.
      TRACE("PRIME SCORE: %i\n", dude.GetPrimeSpellCastingScore(classflag, schoolID));
    };
  };
  for (int i=1;i<=30;i++)
  {
    for (int j=1;j<=MAX_SPELL_LEVEL;j++)
    {
      int max = dude.spells.spellLimits.MaxSpellsForLevel(classflag,i, j, dude.GetPrimeSpellCastingScore(classflag));
      TRACE("MAX SPELLS for spell lvl %i, chlvl %i = %i\n", j,i,max);
    }
  }  
#endif

  return TRUE;
}

void CUAFWinEdView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
  if (m_MapPicture.GetSafeHwnd() == NULL)
    return;

   // map view
   const ASLENTRY *pEntry;
   CString aString;
   pEntry = levelData.level_asl.Find("$ACCESSPARAMS");
   if (pEntry == NULL)
   {
     CAccessDlg dlg;
     aString=dlg.m_accessParameters;
     levelData.level_asl.Insert("$ACCESSPARAMS",
                                 aString,
                                 ASLF_EDITOR|ASLF_SYSTEM);
   }
   else 
   {
     aString=*pEntry->Value();
   };
   levelAccess.ComputeAccess(currX, currY, aString);
   m_MapPicture.Invalidate(FALSE);

   // 3d view
   m_PictureFrame.SetCurrPos(currX, currY, currFacing);
   m_PictureFrame.Invalidate(FALSE);

   CheckButtonStates();
   
   OnActivateView(TRUE, this, this);

  // Force the window to redraw.
  Invalidate(FALSE);
}

BOOL CUAFWinEdView::MouseToMap(CPoint pt, UINT &x, UINT &y, UINT *dx, UINT *dy)
// Puts remainder at dx and dy if you request them.
{
  CRect MapRect;  
  m_MapPicture.GetWindowRect(&MapRect);
  ClientToScreen(&pt);

  if (MapRect.PtInRect(pt))
  {
    int mapx, mapy;
    mapx = (pt.x-MapRect.left-2);
    mapy = (pt.y-MapRect.top-2);
    if ( (mapx<0) || (mapy<0)) return false;
    x = xCurrentScroll + mapx / SQUARESIZE;
    y = yCurrentScroll + mapy / SQUARESIZE;
    if (dx!=NULL) *dx = mapx % SQUARESIZE;
    if (dy!=NULL) *dy = mapy % SQUARESIZE;
    if (x>=globalData.levelInfo.stats[globalData.currLevel].area_width)return false;
    if (y>=globalData.levelInfo.stats[globalData.currLevel].area_height)return false;
    return TRUE;
  }
  return FALSE;
}

void CUAFWinEdView::MapToMouse(CPoint& pt, UINT x, UINT y)
{
  // Undo what MouseToMap has done and provide point at center of square
  CRect MapRect;
  int mapx, mapy;
  m_MapPicture.GetWindowRect(&MapRect);
  mapx=(x-xCurrentScroll)*SQUARESIZE+SQUARESIZE/2;
  mapy=(y-yCurrentScroll)*SQUARESIZE+SQUARESIZE/2;
  pt.x=mapx+2+MapRect.left;
  pt.y=mapy+2+MapRect.top;
}

BOOL CUAFWinEdView::MouseToPicture(CPoint pt, UINT& Square)
{
  // if using a dungeon map,
  // Determines which square of 3-d picture   0 through 10   mouse was in
  //   000001111122222
  //   333444555666777
  //   8888899999aaaaa
  CRect PictRect;
  m_PictureFrame.GetWindowRect(&PictRect);
  ClientToScreen(&pt);

  if (PictRect.PtInRect(pt))
  {
    if (m_mapType != OVERLAND_TYPE)
    {
      int x, y, mapx, mapy;
      mapx=(pt.x-PictRect.left);
      mapy=(pt.y-PictRect.top);
      y=3*mapy/(PictRect.bottom-PictRect.top);
      if (y>2) return FALSE;
      if (y==1) 
      {
        x=5*mapx/(PictRect.right-PictRect.left);
        if (x>4) return FALSE;
        Square=3+x;
      }
      else
      {
        x=3*mapx/(PictRect.right-PictRect.left);
        if (x>2) return FALSE;
        Square=3*y+x;
        if (y==2) Square+=2;
      };
      return TRUE;
    }
    else
    {
      // should probably use this for placement also
      return FALSE;
    }
  };
  return FALSE;
}

int CUAFWinEdView::MousePointToWall(UINT dx, UINT dy)
{
  if (!m_KwikKlik) return currFacing;
  // Just to make you puzzle a bit, Richard.  No if statement!  
  // The assembly code for this looks really nice because
  // the three main sub-expressions can be evaluated in parallel
  // and there are no conditional branches.  PRS
  //return (((dx-dy)&512)|((((dy-dx)&(SQUARESIZE-dx-dy))|((dx-dy)&(dy-SQUARESIZE+dx)))&256))>>8;
  // fixed the case where dx==dy
  return (((dx-dy)&512)|((((dy-dx-1)&(SQUARESIZE-dx-dy))|((dx-dy)&(dy-SQUARESIZE+dx)))&256))>>8;
}


BOOL CUAFWinEdView::ClickInsideGrid(CPoint point)
{
  CRect MapRect;
  m_MapPicture.GetWindowRect(&MapRect);
  ClientToScreen(&point);
  return (MapRect.PtInRect(point));
}


void CUAFWinEdView::HandleLeftClick(UINT squareX, 
                                    UINT squareY,
                                    UINT dx,
                                    UINT dy)
{

  //WriteDebugString("HandleLeftClick(x=%i, y=%i)\n",
  //                 squareX, squareY);

   switch (m_mode) 
   {
   case WALL_MODE:
     {
       int facing;
       CDataExchange DX(this, false);
       if (m_KwikKlik)
       {
         facing=MousePointToWall(dx, dy);
         //TRACE("MousePointToWall: facing %i, dx/dy: %i,%i\n", facing, dx,dy);
       }
       else
       {
         facing=currFacing;
       };

       PlaceWall(facing, squareX, squareY);
       UpdateBlockageBox(&DX);
     };
     break;

   case BG_MODE: 
     {
       int facing;
       CDataExchange DX(this, false);
       if (m_KwikKlik)
       {
         facing=MousePointToWall(dx, dy);
       }
       else
       {
         facing=currFacing;
       };
       PlaceBackground(facing, squareX, squareY);
       UpdateBlockageBox(&DX);
     }
     break;

   case START_LOC_MODE:
   case EVENT_MODE:
   case ZONE_MODE:
   case ENTRY_POINT_MODE:
   case BLOCKAGE_MODE:
      PlaceFront(currFacing, squareX, squareY);
      break;
   }
}



void CUAFWinEdView::HandlePictureLeftClick(UINT square)
{
  switch (m_mode)
  {
  case WALL_MODE:
    switch (square)
    {
    case 0: ProcessKeyPress(m_hWnd,VK_LEFT); break;
    case 2: ProcessKeyPress(m_hWnd,VK_RIGHT); break;
    case 3: PlaceWall(currFacing+3,currX,currY); break;
    case 4: 
      PlaceWall(currFacing+3,currX+deltaX[currFacing],currY+deltaY[currFacing]);
      break;
    case 5: PlaceWall(currFacing+0,currX,currY); break;
    case 6: 
      PlaceWall(currFacing+1,currX+deltaX[currFacing],currY+deltaY[currFacing]); 
      break;
    case 7: PlaceWall(currFacing+1,currX,currY); break;
    case 9: ProcessKeyPress(m_hWnd,VK_DOWN); break;
    }
    break; // wall mode
  }
}

void CUAFWinEdView::HandlePictureRightClick(UINT square)
{
  int wall, y, x, d=0;
  switch (m_mode)
  {
  case WALL_MODE:
    wall=-1;
    switch (square)
    {
    case 0: Move(3); break;
    case 1: Move(0); break;
    case 2: Move(1); break;
    case 3: wall=3; d=0; break;
    case 4: wall=3; d=1; break;
    case 5: wall=0; d=0; break;
    case 6: wall=1; d=1; break;
    case 7: wall=1; d=0; break;
    case 9: Move(2); break;
    };
    if (wall>=0)
    {
      wall=(currFacing+wall)&3;
      y=currY+d*deltaY[currFacing];
      x=currX+d*deltaX[currFacing];
      PlaceWall (wall, x, y, 0, 0);
    };
    break;
  };
}

void CUAFWinEdView::OnPlacecurrent() 
{
   BOOL old = DoubleFillWallSlot;
   DoubleFillWallSlot=FALSE;
   PlaceFront(currFacing, currX, currY);
   DoubleFillWallSlot=old;
   CDataExchange DX(this, FALSE);
   UpdateBlockageBox(&DX);
   m_MapPicture.Invalidate(FALSE);
}

void CUAFWinEdView::OnPlaceleft() 
{
   BOOL old = DoubleFillWallSlot;
   DoubleFillWallSlot=FALSE;
   PlaceLeft(currX, currY);
   DoubleFillWallSlot=old;
   CDataExchange DX(this, FALSE);
   UpdateBlockageBox(&DX);
   m_MapPicture.Invalidate(FALSE);
}

void CUAFWinEdView::OnPlaceright() 
{
   BOOL old = DoubleFillWallSlot;
   DoubleFillWallSlot=FALSE;
   PlaceRight(currX, currY);
   DoubleFillWallSlot=old;
   CDataExchange DX(this, FALSE);
   UpdateBlockageBox(&DX);
   m_MapPicture.Invalidate(FALSE);
}

void CUAFWinEdView::SetSlotButtonText(int curr)
{
  int StartSlot = (curr/16)*16 + 1;
  CWnd *pWnd;
  CString tmp;

  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT1);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT2);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT3);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT4);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT5);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT6);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT7);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT8);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT9);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT10);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT11);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT12);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT13);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT14);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT15);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
  tmp.Format("%u", StartSlot); StartSlot++;
  pWnd = GetDlgItem(IDC_SELECT16);
  if (pWnd!=NULL) pWnd->SetWindowText(tmp);
}

void CUAFWinEdView::SelectWall(void)
{
  //int oldStartSlot = currWallSlot/16;
  ((CMainFrame*)(AfxGetMainWnd()))->OnViewWallsets();
  //int newStartSlot = currWallSlot/16;
  //if (oldStartSlot != newStartSlot)
    SetSlotButtonText(currWallSlot);
  CheckButtonStates();
  UpdateData(FALSE);
}

void CUAFWinEdView::SelectBackground(void)
{
  //int oldStartSlot = currBkGnd/16;
  ((CMainFrame*)(AfxGetMainWnd()))->OnViewBackdrops();
  //int newStartSlot = currBkGnd/16;
  //if (oldStartSlot != newStartSlot)
    SetSlotButtonText(currBkGnd);
  CheckButtonStates();
  UpdateData(FALSE);
}

void CUAFWinEdView::SelectZone(void)
{
  ((CMainFrame*)(AfxGetMainWnd()))->OnViewZones();
}

void CUAFWinEdView::SelectEntryPoint(void)
{
  ((CMainFrame*)(AfxGetMainWnd()))->OnViewEntrypoints();
}

void CUAFWinEdView::OnSelectcurrent() 
{
   switch (m_mode) {
   case START_LOC_MODE:
      break;
   case WALL_MODE:
      SelectWall();
      break;
   case EVENT_MODE:
      break;
   case BG_MODE:
      SelectBackground();
      break;
   case ZONE_MODE:
      SelectZone();
      break;
   case ENTRY_POINT_MODE:
      SelectEntryPoint();
      break;
   }
}

void CUAFWinEdView::OnMove(int x, int y) 
{
   CFormView::OnMove(x, y);
}

BOOL CUAFWinEdView::CheckButtonStates(void)
{
   char temp[50];
   BOOL changes = FALSE;
   //TRACE("CheckButtonStates\n");
   
   if (m_WallSlotPic.m_hWnd == NULL)
     return FALSE;

   CDataExchange DX(this, FALSE);
   
   if (globalData.designName.IsEmpty())
   {
      m_DesignName = "Default Design";
      changes = TRUE;
      DDX_Text(&DX, IDC_DESIGNNAME, m_DesignName);
   }
   else if (globalData.designName != m_DesignName)
   {       
      if (globalData.currLevel < 0)
        m_DesignName = "(Invalid) " + globalData.designName;
      else
        m_DesignName = globalData.designName;

      changes = TRUE;
      DDX_Text(&DX, IDC_DESIGNNAME, m_DesignName);
   }

   if ((globalData.currLevel >= 0) && (m_LevelName != globalData.levelInfo.stats[globalData.currLevel].level_name))
   {
      m_LevelName = globalData.levelInfo.stats[globalData.currLevel].level_name;
      changes = TRUE;
      DDX_Text(&DX, IDC_LEVELNAME, m_LevelName);
   }
   else
   {
     if (globalData.currLevel < 0)
     {
        m_LevelName = "Invalid Level";
        changes = TRUE;
        DDX_Text(&DX, IDC_LEVELNAME, m_LevelName);
     }
   }

   if (m_oldLevel != globalData.currLevel)
   {
     if (globalData.levelInfo.stats[globalData.currLevel].used)
     {
       m_oldLevel = globalData.currLevel;
       m_MaxWidth.Format("%u", globalData.levelInfo.stats[globalData.currLevel].area_width);
       m_MaxHeight.Format("%u",globalData.levelInfo.stats[globalData.currLevel].area_height);
     }
   }
   int mw=atoi(m_MaxWidth);
   if ((m_oldMaxWidth != mw) || (mw != globalData.levelInfo.stats[globalData.currLevel].area_width))
   {
     changes = TRUE;
     m_MaxWidth.Format("%u", globalData.levelInfo.stats[globalData.currLevel].area_width);
     m_oldMaxWidth=atoi(m_MaxWidth);
     DDX_Text(&DX, IDC_MAXWIDTH, m_MaxWidth);
   }
   int mh = atoi(m_MaxHeight);
   if ((m_oldMaxHeight != mh) || (mh != globalData.levelInfo.stats[globalData.currLevel].area_height))
   {
     changes = TRUE;
     m_MaxHeight.Format("%u",globalData.levelInfo.stats[globalData.currLevel].area_height);
     m_oldMaxHeight=atoi(m_MaxHeight);
     DDX_Text(&DX, IDC_MAXHEIGHT, m_MaxHeight);
   }

   if (   (currWallSlot != m_oldWall) 
       && (m_WallSlotPic.m_hWnd != NULL)
       && (m_mapType != OVERLAND_TYPE))
   {
      changes = TRUE;
      sprintf(temp, "Wall %i", currWallSlot+1);
      m_WallSlot = temp;
      m_WallSlotColor.SetColor(GetWallColor(currWallSlot));
      m_WallSlotPic.LoadSourcePictures(WallSets[currWallSlot].doorFile, 
                                       WallSets[currWallSlot].overlayFile,
                                       WallSets[currWallSlot].wallFile,
                                       WallSets[currWallSlot].doorFirst);
      
      m_WallSlotPic.Invalidate(FALSE);
      SetCurrentButton(currWallSlot);      
   }

   if (m_oldBlockage != currBlockage) // wall blockage, not overland blockage
   {
      changes = TRUE;
      m_ObText = BlockageText[currBlockage];
      m_ObColor.SetColor(GetObstructionColor(currBlockage));
      DDX_Text(&DX, IDC_OBTEXT, m_ObText);
      m_ObColor.Invalidate(FALSE);
      SetCurrentBlockage(currBlockage);
   }
   
   if ((currBkGnd != m_oldBG) && (m_BGPic.m_hWnd != NULL) && (m_mapType != OVERLAND_TYPE))
   {
      changes = TRUE;
      sprintf(temp, "Backdrop %i", currBkGnd+1);
      m_BGSlot = temp;
      m_BGColor.SetColor(GetBackdropColor(currBkGnd));
      m_BGPic.LoadPicture(
                   BackgroundSets[currBkGnd].backgroundFile, 
                   BackGndDib, 
                   rte.BackgroundArtDir(), 
                   ede.TemplateBackgroundArtDir());      
      
      m_BGPic.Invalidate(FALSE);
      m_BGColor.Invalidate(FALSE);
      SetCurrentButton(currBkGnd);
   }

   if (m_oldZone != currZone)
   {
      changes = TRUE;
      m_ZoneText = levelData.zoneData.zones[currZone].zoneName;
      m_ZoneColor.SetColor(GetZoneColor(currZone));
      m_ZoneColor.Invalidate(FALSE);
      DDX_Text(&DX, IDC_ZONETEXT, m_ZoneText);
      SetCurrentButton(currZone);
   }
   
   if (m_oldEP != currEntryPoint)
   {
      changes = TRUE;
      SetCurrentButton(currEntryPoint);
   }

   if (m_oldX != currX)
   {
      changes = TRUE;
      sprintf(temp, "%i", currX);
      m_CurrX = temp;
      DDX_Text(&DX, IDC_CURRX, m_CurrX);
   }
   
   if (m_oldY != currY)
   {
      changes = TRUE;
      sprintf(temp, "%i", currY);
      m_CurrY = temp;
      DDX_Text(&DX, IDC_CURRY, m_CurrY);
   }
   
   if (m_oldFacing != currFacing)
   {
      changes = TRUE;
      m_CurrFacing=FacingText[currFacing];
      
      DDX_Text(&DX, IDC_CURRFACING, m_CurrFacing);
   }   

   if ( (m_oldFacing != currFacing) ||
        (m_oldX != currX) ||
        (m_oldY != currY) ||
         wallUndone) 
   {
     UpdateBlockageBox(&DX);
     wallUndone=false;
   };

    changes = TRUE;
    BOOL enable8, enable16;
    COLORREF (*GetColorFunc)(int);

    switch (m_mode)
    {
    case BLOCKAGE_MODE:
       enable8 = FALSE;
       enable16 = FALSE;
       m_ModeText = "Overland Blockage";
       DDX_Text(&DX, IDC_ModeText, m_ModeText);
       m_SelectButton.EnableWindow(FALSE);
       m_PlaceRight.EnableWindow(FALSE);
       m_PlaceLeft.EnableWindow(FALSE);
       m_PlaceButton.EnableWindow(FALSE);
       GetColorFunc = NULL;
       break;

    case START_LOC_MODE:
       enable8 = FALSE;
       enable16 = FALSE;
       m_ModeText = "Start Loc Mode";
       DDX_Text(&DX, IDC_ModeText, m_ModeText);
       m_SelectButton.EnableWindow(FALSE);
       m_PlaceRight.EnableWindow(FALSE);
       m_PlaceLeft.EnableWindow(FALSE);
       m_PlaceButton.EnableWindow(TRUE);
       m_PlaceButton.SetWindowText("Place Start Loc");
       GetColorFunc = NULL;
       break;
       
    case WALL_MODE:
       enable8 = TRUE;
       enable16 = TRUE;
       m_ModeText = "Wall Mode";
       DDX_Text(&DX, IDC_ModeText, m_ModeText);
       m_SelectButton.EnableWindow(TRUE);
       m_SelectButton.SetWindowText("Select Wall Slot");
       m_PlaceButton.SetWindowText("Front Wall");
       m_PlaceRight.EnableWindow(TRUE);
       m_PlaceLeft.EnableWindow(TRUE);
       m_PlaceButton.EnableWindow(TRUE);
       SetCurrentButton(currWallSlot);
       GetColorFunc = GetWallColor;
       break;
       
    case EVENT_MODE:
       enable8 = FALSE;
       enable16 = FALSE;
       m_ModeText = "Event Mode";
       DDX_Text(&DX, IDC_ModeText, m_ModeText);
       m_SelectButton.EnableWindow(FALSE);
       m_PlaceRight.EnableWindow(FALSE);
       m_PlaceLeft.EnableWindow(FALSE);
       m_PlaceButton.EnableWindow(TRUE);
       GetColorFunc = NULL;
       break;
       
    case ZONE_MODE:
       enable8 = TRUE;
       enable16 = TRUE;
       m_ModeText = "Zone Mode";
       DDX_Text(&DX, IDC_ModeText, m_ModeText);
       m_SelectButton.EnableWindow(TRUE);
       m_SelectButton.SetWindowText("Select Zone");
       m_PlaceRight.EnableWindow(FALSE);
       m_PlaceLeft.EnableWindow(FALSE);
       m_PlaceButton.EnableWindow(TRUE);
       m_PlaceButton.SetWindowText("Place Zone");
       SetCurrentButton(currZone);
       GetColorFunc = GetZoneColor;
       break;


    case ENTRY_POINT_MODE:
       enable8 = TRUE;
       enable16 = FALSE;
       m_ModeText = "Entry Point Mode";
       DDX_Text(&DX, IDC_ModeText, m_ModeText);
       m_SelectButton.EnableWindow(TRUE);
       m_SelectButton.SetWindowText("Select Entry Pt");
       m_PlaceRight.EnableWindow(FALSE);
       m_PlaceLeft.EnableWindow(FALSE);
       m_PlaceButton.EnableWindow(TRUE);
       m_PlaceButton.SetWindowText("Place Entry Pt");
       SetCurrentButton(currEntryPoint);
       GetColorFunc = GetEPColor;
       break;
       
    case BG_MODE:
       enable8 = TRUE;
       enable16 = TRUE;
       m_ModeText = "Backdrop Mode";
       DDX_Text(&DX, IDC_ModeText, m_ModeText);
       m_SelectButton.EnableWindow(TRUE);
       m_SelectButton.SetWindowText("Select Backdrop");
       m_PlaceButton.SetWindowText("Place Backdrop");
       m_PlaceRight.EnableWindow(FALSE);
       m_PlaceLeft.EnableWindow(FALSE);
       m_PlaceButton.EnableWindow(TRUE);
       SetCurrentButton(currBkGnd);
       GetColorFunc = GetBackdropColor;
       break;
       
    default:
       enable8 = FALSE;
       enable16 = FALSE;
       WriteDebugString("Unhandled edit mode\n");
       m_ModeText = "Unknown Mode";
       DDX_Text(&DX, IDC_ModeText, m_ModeText);
       GetColorFunc = NULL;
       break;
    }    

    if (m_mapType == OVERLAND_TYPE)
    {
       m_SelectButton.EnableWindow(FALSE);
       m_PlaceRight.EnableWindow(FALSE);
       m_PlaceLeft.EnableWindow(FALSE);
       m_PlaceButton.EnableWindow(FALSE);
    }
   
    if (GetColorFunc != NULL)
    {
      int startSlot = currWallSlot/16 * 16;
      FillControl(&m_Color1,GetColorFunc(startSlot+0));
      FillControl(&m_Color2,GetColorFunc(startSlot+1));
      FillControl(&m_Color3,GetColorFunc(startSlot+2));
      FillControl(&m_Color4,GetColorFunc(startSlot+3));
      FillControl(&m_Color5,GetColorFunc(startSlot+4));
      FillControl(&m_Color6,GetColorFunc(startSlot+5));
      FillControl(&m_Color7,GetColorFunc(startSlot+6));
      FillControl(&m_Color8,GetColorFunc(startSlot+7));
      FillControl(&m_Color9,GetColorFunc(startSlot+8));
      FillControl(&m_Color10,GetColorFunc(startSlot+9));
      FillControl(&m_Color11,GetColorFunc(startSlot+10));
      FillControl(&m_Color12,GetColorFunc(startSlot+11));
      FillControl(&m_Color13,GetColorFunc(startSlot+12));
      FillControl(&m_Color14,GetColorFunc(startSlot+13));
      FillControl(&m_Color15,GetColorFunc(startSlot+14));   
      FillControl(&m_Color16,GetColorFunc(startSlot+15));
    }

    CWnd *pWnd;
    
    BOOL enableBlockage = (m_mode == WALL_MODE);
    pWnd = GetDlgItem(IDC_BLOCKAGEBLK);
    if (pWnd != NULL)
       pWnd->EnableWindow(enableBlockage);
    pWnd = GetDlgItem(IDC_BLOCKAGEOPEN);
    if (pWnd != NULL)
       pWnd->EnableWindow(enableBlockage);

    pWnd = GetDlgItem(IDC_COLOR1);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);
    pWnd = GetDlgItem(IDC_COLOR2);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);
    pWnd = GetDlgItem(IDC_COLOR3);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);
    pWnd = GetDlgItem(IDC_COLOR4);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);
    pWnd = GetDlgItem(IDC_COLOR5);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);
    pWnd = GetDlgItem(IDC_COLOR6);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);
    pWnd = GetDlgItem(IDC_COLOR7);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);      
    pWnd = GetDlgItem(IDC_COLOR8);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);

    pWnd = GetDlgItem(IDC_SELECT1);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);
    pWnd = GetDlgItem(IDC_SELECT2);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);
    pWnd = GetDlgItem(IDC_SELECT3);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);
    pWnd = GetDlgItem(IDC_SELECT4);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);
    pWnd = GetDlgItem(IDC_SELECT5);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);
    pWnd = GetDlgItem(IDC_SELECT6);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);
    pWnd = GetDlgItem(IDC_SELECT7);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);      
    pWnd = GetDlgItem(IDC_SELECT8);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable8);

    pWnd = GetDlgItem(IDC_COLOR9);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_COLOR10);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_COLOR11);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_COLOR12);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_COLOR13);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_COLOR14);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_COLOR15);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_COLOR16);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);

    pWnd = GetDlgItem(IDC_SELECT9);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_SELECT10);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_SELECT11);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_SELECT12);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_SELECT13);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_SELECT14);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_SELECT15);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
    pWnd = GetDlgItem(IDC_SELECT16);
    if (pWnd != NULL)
       pWnd->EnableWindow(enable16);
     
   if (m_mode == EVENT_MODE)
   {
      if (levelData.area[currY][currX].eventExists)
         m_PlaceButton.SetWindowText("Edit Event");
      else
         m_PlaceButton.SetWindowText("Place Event");
   }

   pWnd = GetDlgItem(IDC_TOGGLEBGMETHOD);
   if (pWnd != NULL)
   {
     ((CButton*)pWnd)->SetCheck(levelData.area[currY][currX].ShowDistantBG);
     pWnd->EnableWindow(m_mapType != OVERLAND_TYPE);
   }

   pWnd = GetDlgItem(IDC_TOGGLEBGBANDS);
   if (pWnd != NULL)
   {
     ((CButton*)pWnd)->SetCheck(levelData.area[currY][currX].DistantBGInBands);
     pWnd->EnableWindow(m_mapType != OVERLAND_TYPE);
   }

   if (m_WallSlotPic.m_hWnd != NULL)
      m_oldWall=currWallSlot;

   if (m_BGPic.m_hWnd != NULL)
      m_oldBG=currBkGnd;

  if (!levelData.area[currY][currX].ShowDistantBG)
  {
    levelData.area[currY][currX].DistantBGInBands=FALSE;
    m_ShowBands.ShowWindow(SW_HIDE);
    m_ShowBands.EnableWindow(FALSE);    
  }
  else
  {
    m_ShowBands.ShowWindow(SW_SHOW);
    m_ShowBands.EnableWindow(TRUE);    
  }
   
   m_oldBlockage=currBlockage;
   m_oldZone=currZone;
   m_oldEP = currEntryPoint;
   m_oldMode=m_mode;
   m_oldX=currX;
   m_oldY=currY;
   m_oldFacing = currFacing;
   
   return changes;
}

void CUAFWinEdView::UpdateBlockageBox(CDataExchange *pDX) 
{
  unsigned int x,y;
  CString zone;
  CString backdrop;
  CString event;
  CString BLOCK1, BLOCK2, BLOCK3, BLOCK4;
  if (m_BLOCK_Type)
  {
    x=m_mouseX; y=m_mouseY;
  } 
  else
  {
    x=currX; y=currY;
  }; 
  CString cellNum;
  int wallNum[4];
  int bgNum[4];
  const char *(*pBLOCK);
  const char *pBlocks[4];

  int dir;

  m_CellStatusText="";
  if ((x != 0xffffffff) && (y != 0xffffffff))
  {
    AREA_MAP_DATA *pArea;
    pArea=&levelData.area[y][x];

    if (m_BLOCK_Type)
    {
      dir=0;
      pBLOCK=&FacingText[0];
    }
    else
    {
      dir=currFacing;
      pBLOCK=&RelativeDirectionText[4*dir];
    };
    for (int i=0; i<4; i++)
    {
      pBlocks[i] = BlockageText[pArea->blockages(dir+i+3)];
      wallNum[i] = pArea->walls(dir+i+3)+1;
      bgNum[i]   = pArea->backgrounds(dir+i+3)+1;
    };

    cellNum.Format("** Square %i , %i **", x, y); 

    if (pArea->zone >= 0)
    {
      zone.Format("Zone %i", pArea->zone+1);
    }
    else
    {
      zone="";
    };

    if (pArea->bkgrnd >= 0)
    {
      if (m_mapType != OVERLAND_TYPE)
      {
        if (bgNum[0] > 1)
          BLOCK1.Format("Backdrop %d", bgNum[0]);
        else
          BLOCK1.Format("Backdrop None", bgNum[0]);

        if (bgNum[1] > 1)
          BLOCK2.Format("Backdrop %d", bgNum[1]);
        else
          BLOCK2.Format("Backdrop None", bgNum[1]);

        if (bgNum[2] > 1)
          BLOCK3.Format("Backdrop %d", bgNum[2]);
        else
          BLOCK3.Format("Backdrop None", bgNum[2]);
        
        if (bgNum[3] > 1)
          BLOCK4.Format("Backdrop %d", bgNum[3]);
        else
          BLOCK4.Format("Backdrop None", bgNum[3]);

        backdrop.Format("%s - %s\r\n"
                        "%s - %s\r\n"
                        "%s - %s\r\n"
                        "%s - %s",
                        pBLOCK[3],
                        BLOCK1,
                        pBLOCK[0],
                        BLOCK2,
                        pBLOCK[1],
                        BLOCK3,
                        pBLOCK[2],
                        BLOCK4);
      }
      else
      {
        if (pArea->bkgrnd)
          backdrop.Format("Blocked");
        else
          backdrop.Format("Not Blocked");
      }
    }
    else
      backdrop="";

    event="";
    if (pArea->eventExists)
    {
      GameEvent *data;
        if ((data = levelData.eventData.GetFirstEvent(x, y)) != NULL)
        {
          event.Format("Event %s",
                GetEventIdDescription(data->GetEventId(), data->GetEventSource()));
        };
    };


    if (m_mapType == DUNGEON_TYPE)
    {  
      if (wallNum[0] > 1)
        BLOCK1.Format("Wall %d", wallNum[0]);
      else
        BLOCK1.Format("Wall None", wallNum[0]);

      if (wallNum[1] > 1)
        BLOCK2.Format("Wall %d", wallNum[1]);
      else
        BLOCK2.Format("Wall None", wallNum[1]);

      if (wallNum[2] > 1)
        BLOCK3.Format("Wall %d", wallNum[2]);
      else
        BLOCK3.Format("Wall None", wallNum[2]);
      
      if (wallNum[3] > 1)
        BLOCK4.Format("Wall %d", wallNum[3]);
      else
        BLOCK4.Format("Wall None", wallNum[3]);

      m_CellStatusText.Format("%s\r\n"
                              "%s - %s\r\n%s\r\n"
                              "%s - %s\r\n%s\r\n"
                              "%s - %s\r\n%s\r\n"
                              "%s - %s\r\n%s\r\n"
                              "%s\r\n"
                              "%s\r\n"
                              "%s",
                              cellNum,
                              pBLOCK[3],
                              BLOCK1,
                              pBlocks[0],
                              pBLOCK[0],
                              BLOCK2,
                              pBlocks[1],
                              pBLOCK[1],
                              BLOCK3,
                              pBlocks[2],
                              pBLOCK[2],
                              BLOCK4,
                              pBlocks[3],
                              zone,
                              backdrop,
                              event);
    }
    else
    {
      m_CellStatusText.Format("%s\r\n"
                              "%s\r\n"
                              "%s\r\n"
                              "%s",
                              cellNum,
                              zone,
                              backdrop,
                              event);
    }
  };  // Now that was a nice, long if.

  DDX_Text(pDX, IDC_CellStatus, m_CellStatusText);
}   

void CUAFWinEdView::SetCurrentButton(int curr)
{
  UpdateData(TRUE);
   m_Select1 = FALSE;
   m_Select2 = FALSE;
   m_Select3 = FALSE;
   m_Select4 = FALSE;
   m_Select5 = FALSE;
   m_Select6 = FALSE;
   m_Select7 = FALSE;
   m_Select8 = FALSE;
   m_Select9 = FALSE;
   m_Select10 = FALSE;
   m_Select11 = FALSE;
   m_Select12 = FALSE;
   m_Select13 = FALSE;
   m_Select14 = FALSE;
   m_Select15 = FALSE;
   m_Select16 = FALSE;

   switch ((curr%16)+1)
   {
   case 1:
      m_Select1 = TRUE;
      break;
   case 2:
      m_Select2 = TRUE;
      break;
   case 3:
      m_Select3 = TRUE;
      break;
   case 4:
      m_Select4 = TRUE;
      break;
   case 5:
      m_Select5 = TRUE;
      break;
   case 6:
      m_Select6 = TRUE;
      break;
   case 7:
      m_Select7 = TRUE;
      break;
   case 8:
      m_Select8 = TRUE;
      break;
   case 9:
      m_Select9 = TRUE;
      break;
   case 10:
      m_Select10 = TRUE;
      break;
   case 11:
      m_Select11 = TRUE;
      break;
   case 12:
      m_Select12 = TRUE;
      break;
   case 13:
      m_Select13 = TRUE;
      break;
   case 14:
      m_Select14 = TRUE;
      break;
   case 15:
      m_Select15 = TRUE;
      break;
   case 16:
      m_Select16 = TRUE;
      break;
   }

  CDataExchange DX(this, FALSE);

	DDX_Check(&DX, IDC_SELECT1, m_Select1);
	DDX_Check(&DX, IDC_SELECT10, m_Select10);
	DDX_Check(&DX, IDC_SELECT11, m_Select11);
	DDX_Check(&DX, IDC_SELECT12, m_Select12);
	DDX_Check(&DX, IDC_SELECT13, m_Select13);
	DDX_Check(&DX, IDC_SELECT14, m_Select14);
	DDX_Check(&DX, IDC_SELECT15, m_Select15);
	DDX_Check(&DX, IDC_SELECT16, m_Select16);
	DDX_Check(&DX, IDC_SELECT2, m_Select2);
	DDX_Check(&DX, IDC_SELECT3, m_Select3);
	DDX_Check(&DX, IDC_SELECT4, m_Select4);
	DDX_Check(&DX, IDC_SELECT5, m_Select5);
	DDX_Check(&DX, IDC_SELECT6, m_Select6);
	DDX_Check(&DX, IDC_SELECT7, m_Select7);
	DDX_Check(&DX, IDC_SELECT8, m_Select8);
	DDX_Check(&DX, IDC_SELECT9, m_Select9);
}

void CUAFWinEdView::SetCurrentBlockage(int curr)
{
  UpdateData(TRUE);
   m_BlockageBlk = FALSE;
   m_BlockageOpen = FALSE;

   switch (curr)
   {
   case OpenBlk:
      m_BlockageOpen = TRUE;
      break;
   case BlockedBlk:
      m_BlockageBlk = TRUE;
      break;
   }

  CDataExchange DX(this, FALSE);
	DDX_Check(&DX, IDC_BLOCKAGEOPEN, m_BlockageOpen);
	DDX_Check(&DX, IDC_BLOCKAGEBLK, m_BlockageBlk);

}
void CUAFWinEdView::SetCurrentVal(int curr)
{
   switch (m_mode) 
   {
   case START_LOC_MODE:
      break;
   case WALL_MODE:
     {
      int StartSlot = (currWallSlot/16)*16;
      currWallSlot = curr+StartSlot;
      if (currWallSlot == 0) currBlockage = OpenBlk;
     }
     break;
   case EVENT_MODE:
      break;
   case BG_MODE:
     {
      //currBkGnd = curr;
      int StartSlot = (currBkGnd/16)*16;
      currBkGnd = curr+StartSlot;
     }
      break;
   case ZONE_MODE:
      currZone = curr;
      break;
   case ENTRY_POINT_MODE:
      currEntryPoint = curr;
      break;
   }
}

void CUAFWinEdView::OnSelect1() 
{
	SetCurrentVal(0);
   CheckButtonStates();
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect10() 
{
	SetCurrentVal(9);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect11() 
{
	SetCurrentVal(10);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect12() 
{
	SetCurrentVal(11);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect13() 
{
	SetCurrentVal(12);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect14() 
{
	SetCurrentVal(13);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect15() 
{
	SetCurrentVal(14);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect16() 
{
	SetCurrentVal(15);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect2() 
{
	SetCurrentVal(1);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect3() 
{
	SetCurrentVal(2);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect4() 
{
	SetCurrentVal(3);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect5() 
{
	SetCurrentVal(4);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect6() 
{
	SetCurrentVal(5);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect7() 
{
	SetCurrentVal(6);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect8() 
{
	SetCurrentVal(7);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::OnSelect9() 
{
	SetCurrentVal(8);
   CheckButtonStates();	
   UpdateData(FALSE);
}

void CUAFWinEdView::MoveMapLeft()
{
  OnHScroll(SB_LINELEFT, 0, &m_HorzScroll);
}
void CUAFWinEdView::MoveMapRight()
{
  OnHScroll(SB_LINERIGHT, 0, &m_HorzScroll);
}

void CUAFWinEdView::MoveMapUp()
{
  OnVScroll(SB_LINELEFT, 0, &m_VertScroll);
}

void CUAFWinEdView::MoveMapDown()
{
  OnVScroll(SB_LINERIGHT, 0, &m_VertScroll);
}


void CUAFWinEdView::MoveMapHorz(int newPos)
{
  if (newPos >= (xCurrentScroll+MapBufferWidth))
  {
    xCurrentScroll = min((newPos-MapBufferWidth)+1, xMaxScroll);
    m_HorzScroll.SetScrollPos(xCurrentScroll);
    m_MapPicture.SetVisible(xCurrentScroll, yCurrentScroll);	
    m_MapPicture.Invalidate(FALSE);
  }
  else if (newPos < xCurrentScroll)
  {
    xCurrentScroll = max(newPos,0);
    m_HorzScroll.SetScrollPos(xCurrentScroll);
    m_MapPicture.SetVisible(xCurrentScroll, yCurrentScroll);	
    m_MapPicture.Invalidate(FALSE);
  }
}

void CUAFWinEdView::MoveMapVert(int newPos)
{
  if (newPos >= (yCurrentScroll+MapBufferHeight))
  {
    yCurrentScroll = min((newPos-MapBufferHeight)+1, yMaxScroll);
    m_VertScroll.SetScrollPos(yCurrentScroll);
    m_MapPicture.SetVisible(xCurrentScroll, yCurrentScroll);	
    m_MapPicture.Invalidate(FALSE);
  }
  else if (newPos < yCurrentScroll)
  {
    yCurrentScroll = max(newPos,0);
    m_VertScroll.SetScrollPos(yCurrentScroll);
    m_MapPicture.SetVisible(xCurrentScroll, yCurrentScroll);
    m_MapPicture.Invalidate(FALSE);
  }
}


void CUAFWinEdView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  if (pScrollBar != &m_HorzScroll)
  {
    CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
    m_MapPicture.Invalidate(FALSE);
    Invalidate(FALSE);
    return;
  }

  int xNewPos;    // new position 
  switch (nSBCode)
  {
  case SB_LEFT: // far left
    xNewPos = 0;
    break;
  case SB_RIGHT: // far right
    xNewPos = xMaxScroll;
    break;
  case SB_LINELEFT: // arrow left
    xNewPos = xCurrentScroll - LINE_SIZE; 
    break;
  case SB_LINERIGHT: // arrow right
    xNewPos = xCurrentScroll + LINE_SIZE; 
    break;
  case SB_PAGELEFT: // click-in-track left
    xNewPos = xCurrentScroll - PAGE_SIZE; 
    break;
  case SB_PAGERIGHT: // click-in-track right
    xNewPos = xCurrentScroll + PAGE_SIZE; 
    break;
  case SB_THUMBPOSITION: // button dragged to nPos
    xNewPos = nPos;
    break;
  default: //SB_THUMBTRACK, eg.
    xNewPos = xCurrentScroll;
    break;
  }

  xNewPos = max(0, xNewPos); 
  xNewPos = min(xMaxScroll, xNewPos); 

  // If the current position does not change, do not scroll.
  //if (xNewPos == xCurrentScroll) 
  //    return; 

  // Reset the current scroll position. 
  xCurrentScroll = xNewPos; 
  m_HorzScroll.SetScrollPos(xCurrentScroll);
  m_MapPicture.SetVisible(xCurrentScroll, yCurrentScroll);
  m_MapPicture.Invalidate(FALSE);
}

void CUAFWinEdView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  if (pScrollBar != &m_VertScroll)
  {
    CFormView::OnVScroll(nSBCode, nPos, pScrollBar);
    m_MapPicture.Invalidate(FALSE);
    Invalidate(FALSE);
    return;
  }

  int yNewPos=0;

  switch (nSBCode)
  {
  case SB_LEFT: // far top
    yNewPos = 0;
    break;
  case SB_RIGHT: // far bottom
    yNewPos = yMaxScroll;
    break;
  case SB_LINELEFT: // arrow up
    yNewPos = yCurrentScroll - LINE_SIZE; 
    break;
  case SB_LINERIGHT: // arrow right
    yNewPos = yCurrentScroll + LINE_SIZE; 
    break;
  case SB_PAGELEFT: // click-in-track up
    yNewPos = yCurrentScroll - PAGE_SIZE; 
    break;
  case SB_PAGERIGHT: // click-in-track down
    yNewPos = yCurrentScroll + PAGE_SIZE; 
    break;
  case SB_THUMBPOSITION: // button dragged to nPos
    yNewPos = nPos;
    break;
  default:
    yNewPos = yCurrentScroll;
    break;
  }

  yNewPos = max(0, yNewPos); 
  yNewPos = min(yMaxScroll, yNewPos); 

  //if ((yNewPos + (MapBufferHeight-1)) > yMaxScroll)
  //  return;

  // If the current position does not change, do not scroll.
  //if (yNewPos == yCurrentScroll) 
  //    return; 

  // Reset the current scroll position. 
  yCurrentScroll = yNewPos; 
  m_VertScroll.SetScrollPos(yCurrentScroll);
  m_MapPicture.SetVisible(xCurrentScroll, yCurrentScroll);	
  m_MapPicture.Invalidate(FALSE);
}

void CUAFWinEdView::SetSBData()
{
  SCROLLINFO data;

  // first, figure out map size in 
  // cells wide and cells height
  CRect BufferRect;
  m_MapPicture.GetClientRect(&BufferRect);
  MapBufferWidth = BufferRect.Width() / SQUARESIZE;
  MapBufferHeight = BufferRect.Height() / SQUARESIZE;

  // lookup actual map size in cells
  int ActualMapWidth = 
    globalData.levelInfo.stats[globalData.currLevel].area_width;
  int ActualMapHeight = 
    globalData.levelInfo.stats[globalData.currLevel].area_height;

  memset(&data, 0, sizeof(data));
  data.cbSize = sizeof(data);
  data.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;

  if (m_mapType == OVERLAND_TYPE)
    xMaxScroll = max(ActualMapWidth-MapBufferWidth, 0); 
  else
    xMaxScroll = max(PAGE_SIZE*((ActualMapWidth-MapBufferWidth+PAGE_SIZE+1)/PAGE_SIZE), 0); 

  //xMaxScroll = ActualMapWidth; 
  xCurrentScroll = min(xCurrentScroll, xMaxScroll); 
  data.nMin   = xMinScroll; 
  data.nMax   = xMaxScroll; 
  data.nPage  = min(PAGE_SIZE,xMaxScroll);
  data.nPos   = xCurrentScroll; 
  
  if (xMaxScroll > 0)
  {
    m_HorzScroll.ShowScrollBar(TRUE);
    m_HorzScroll.SetScrollInfo(&data);
  }
  else
  {
    m_HorzScroll.ShowScrollBar(FALSE);
  }

  if (m_mapType == OVERLAND_TYPE)
    yMaxScroll = max(ActualMapHeight - MapBufferHeight, 0); 
  else
    yMaxScroll = max(PAGE_SIZE*((ActualMapHeight-MapBufferHeight+PAGE_SIZE+1)/PAGE_SIZE), 0); 
  //yMaxScroll = ActualMapHeight; 
  yCurrentScroll = min(yCurrentScroll, yMaxScroll); 
  memset(&data, 0, sizeof(data));
  data.cbSize = sizeof(data); 
  data.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
  data.nMin   = yMinScroll; 
  data.nMax   = yMaxScroll; 
  data.nPage  = min(PAGE_SIZE,yMaxScroll);
  data.nPos   = yCurrentScroll; 

  if (yMaxScroll > 0)
  {
    m_VertScroll.ShowScrollBar(TRUE);
    m_VertScroll.SetScrollInfo(&data);
  }
  else
  {
    m_VertScroll.ShowScrollBar(FALSE);
  }
}

void CUAFWinEdView::ResetView()
{

  xMinScroll=0;
  xMaxScroll=0;
  xCurrentScroll=0;
  yMinScroll=0;
  yMaxScroll=0;
  yCurrentScroll=0;  

  SetSBData();
  m_MapPicture.SetVisible(xCurrentScroll, yCurrentScroll);
  if ((currX < 0) || (currX >= MapBufferWidth))
    currX = 0;
  if ((currY < 0) || (currY >= MapBufferHeight))
    currY = 0;

  m_oldWall=m_oldBlockage=m_oldBG=m_oldZone=m_oldMode=m_oldX=m_oldY=m_oldFacing=m_oldEP=-1;

  if (m_mapType == OVERLAND_TYPE)
  {
    CString path;
    path = ede.TemplateOverlandArtDir() + OVERLANDART;
    {
      CString art = ede.TemplateOverlandArtDir();
      if (FindImageWithValidExtInFolder(path, art, FALSE))
      {
        if (!m_MapPicture.LoadBackground(BackgroundSets[1].backgroundFile, path))
          WriteDebugString("Failed to LoadBackground(%s) in CUAFWinEdView::ResetView()\n",
                           BackgroundSets[1].backgroundFile);
      }
      else
        WriteDebugString("Failed to LoadBackground(%s) in CUAFWinEdView::ResetView()\n", path);
    };
  }
  else
  {
    CString path;
    CString name;
    name = MAPART;
    path = name;
    {
      CString art = ede.EditorMapArtDir();
      if (FindImageWithValidExtInFolder(path, art, FALSE))
      {
        if (!m_MapPicture.LoadPicture(name, rte.MapArtDir(), ede.EditorMapArtDir()))
          WriteDebugString("Failed to LoadPicture(%s) in CUAFWinEdView::ResetView()\n", path);
      }
      else
        WriteDebugString("Failed to LoadPicture(%s) in CUAFWinEdView::ResetView()\n", path);
    };
  }

  CheckButtonStates();

  m_MapPicture.Invalidate(FALSE);
  m_WallSlotPic.Invalidate(FALSE);
  m_ObColor.Invalidate(FALSE);
  m_BGPic.Invalidate(FALSE);
  m_BGColor.Invalidate(FALSE);
  m_ZoneColor.Invalidate(FALSE);
  Invalidate(FALSE);
}

void CUAFWinEdView::OnBlockageblk() 
{
	 currBlockage = BlockedBlk;
   CheckButtonStates();
   UpdateData(FALSE);
}

void CUAFWinEdView::OnBlockageopen() 
{
	 currBlockage = OpenBlk;
   CheckButtonStates();
   UpdateData(FALSE);
}

BOOL CUAFWinEdView::PreTranslateMessage(MSG* pMsg) 
{
  static BOOL init=false;
  unsigned int mask;
  if (!init)
  {
    memset(keypresses,0,sizeof(keypresses));
    init=true;
  };
  
  BOOL processed = FALSE;
  
  if (pMsg != NULL)
  {
    switch (pMsg->message)
    {
    case WM_KEYUP:
     {
       if (pMsg->wParam==VK_SHIFT) shift=false;
       else if (pMsg->wParam==VK_CONTROL) control=false;
       else if ((pMsg->wParam=='h')||(pMsg->wParam=='H')) {hkey=false;processed=TRUE;}
       else if ((pMsg->wParam=='v')||(pMsg->wParam=='V')) {vkey=false;processed=TRUE;}
       mask=1<<(pMsg->wParam&0x1f);
       keypresses[(pMsg->wParam&0xff)/32]&=(mask^0xffffffff);
       //TRACE("CTRL %i\n", control);BOOL
     }
     break;
    case WM_KEYDOWN:
     {
       if (pMsg->wParam==VK_SHIFT) shift=true;
       else if (pMsg->wParam==VK_CONTROL) control=true;
       else if ((pMsg->wParam=='h')||(pMsg->wParam=='H')) {hkey=true;processed=TRUE;}
       else if ((pMsg->wParam=='V')||(pMsg->wParam=='V')) {vkey=true;processed=TRUE;}
       mask=1<<(pMsg->wParam&0x1f);
       if ((keypresses[(pMsg->wParam&0xff)/32]&mask) == 0)
       {
         processed=ProcessKeyPress(pMsg->hwnd, pMsg->wParam, control, shift); 
         // menu shortcuts using CTRL, as in CTRL-L, will
         // invoke a dialog window which steals input focus
         // from this loop. If CTRL is down when the window
         // is invoked, the WM_KEYUP message never comes
         // here and the local 'control' var is left on.
         //shift=false;
         //control=false;
       }
       keypresses[(pMsg->wParam&0xff)/32] |= mask;
       //TRACE("CTRL %i\n", control);
     }
     break;
     /* formview will not receive these two messages
    case WM_KILLFOCUS:
      memset(keypresses,0,sizeof(keypresses));
      shift=false;
      control=false;
      break;
    case WM_SETFOCUS:
      memset(keypresses,0,sizeof(keypresses));
      shift=false;
      control=false;
      break;
    */
    }
  }
  
  if (processed) 
  {
    return TRUE;
  };
	return FALSE; //CFormView::PreTranslateMessage(pMsg);
}

BOOL CUAFWinEdView::ProcessKeyPress(HWND hWnd, 
                                    WPARAM nChar,
                                    bool Control,
                                    bool Shift)
{
   BOOL isChange = FALSE;
   BOOL processed = FALSE;
   
   int oldX = currX;
   int oldY = currY;
   //int oldFacing = currFacing;
   if ((hWnd == m_HorzScroll.m_hWnd) || (hWnd == m_VertScroll.m_hWnd))
     return FALSE;

   if (Control) return FALSE;

   switch (nChar) 
   {
   case 'H': 
   case 'V': processed=TRUE; break;
   case 'F': PlaceWall(currFacing,currX,currY);   processed=TRUE;break;
   case 'R': PlaceWall(currFacing+1,currX,currY); processed=TRUE; break;
   case 'L': PlaceWall(currFacing-1,currX,currY); processed=TRUE; break;
   case VK_UP: Move(0); processed=TRUE;break;
   case VK_DOWN:  currFacing = (currFacing+2)&3; isChange=TRUE; processed=TRUE;break;       
   case VK_LEFT:  currFacing = (currFacing+3)&3; isChange=TRUE; processed=TRUE;break;
   case VK_RIGHT: currFacing = (currFacing+1)&3; isChange=TRUE; processed=TRUE;break;
   }
   
   if (isChange)
   {
     party.facing = currFacing;

     if (oldX != currX)
       MoveMapHorz(currX);
     if (oldY != currY)
       MoveMapVert(currY);
       
     CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
       ((CUAFWinEdDoc*)(pApp->GetDocPtr()))->UpdateAllViews(NULL);
   }
   return processed;
}



void CUAFWinEdView::Move(int direction)
// direction is relative to currFacing (clockwise)
{
  int oldX, oldY;
  //bool isChange=false;
  oldX=currX;
  oldY=currY;
  direction = (currFacing + direction)&3;
  AREA_MAP_DATA *pArea=&levelData.area[currY][currX];
  if (PassThruWalls || (pArea->blockages(direction) == OpenBlk))
    MoveTo (currX+deltaX[direction], currY+deltaY[direction]);
}

void CUAFWinEdView::MoveTo(int x, int y)
{
  //bool isChange=false;
  int oldX, oldY;
  if ((x!=currX) || (y!=currY))
  {
    oldX=currX;
    oldY=currY;
    currY = y;
    currX = x;
    currY = (currY+levelData.area_height)%levelData.area_height;
    currX = (currX+levelData.area_width)%levelData.area_width;
    if (oldX != currX)
      MoveMapHorz(currX);
    if (oldY != currY)
      MoveMapVert(currY);
       
    CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
       ((CUAFWinEdDoc*)(pApp->GetDocPtr()))->UpdateAllViews(NULL);
  };
}


void PlaceEvent(UINT squareX,UINT squareY)
{
  if (m_mapType == OVERLAND_TYPE)
  {
    if (levelData.area[squareY][squareX].bkgrnd)
    {
      MsgBoxInfo("Events cannot be placed in a blocked square");
      return;
    }
  }  

  CEventViewer dlg(squareX, squareY);
  dlg.DoModal();

  levelData.area[squareY][squareX].eventExists = 
    (levelData.eventData.GetFirstEvent(squareX, squareY) != NULL);
}

void PlaceFront(int facing, int x, int y)
{
   if (globalData.currLevel < 0)
      return;
     
   switch (m_mode) 
   {
   case START_LOC_MODE:
      if (m_mapType == OVERLAND_TYPE)
      {
        if (levelData.area[y][x].bkgrnd)
        {
          MsgBoxInfo("Player start position cannot be placed in a blocked square");
          return;
        }
      }  

      globalData.startX = (BYTE)x;
      globalData.startY = (BYTE)y;
      globalData.startFacing = (BYTE)facing;
      globalData.startLevel = globalData.currLevel;
      SET_MODIFIED;
      break;
   case WALL_MODE:
      PlaceWall(facing, x, y);
      break;
   case EVENT_MODE:
      PlaceEvent(x, y);
      break;
   case BG_MODE:
      PlaceBackground(facing, x,y);
     break;
   case ZONE_MODE:
      if (levelData.area[y][x].zone != (BYTE)currZone)
      {
        SET_MODIFIED;
        levelData.area[y][x].zone = (BYTE)currZone;      
      }
      break;
   case ENTRY_POINT_MODE:
      if (m_mapType == OVERLAND_TYPE)
      {
        if (levelData.area[y][x].bkgrnd)
        {
          MsgBoxInfo("Entry points cannot be placed in a blocked square");
          return;
        }
      }  

      globalData.levelInfo.stats[globalData.currLevel].entryPoints[currEntryPoint].x = x;
      globalData.levelInfo.stats[globalData.currLevel].entryPoints[currEntryPoint].y = y;
      SET_MODIFIED;
      break;
   case BLOCKAGE_MODE: 
      PlaceBlockage(x, y, TRUE);
      break;
   }
      
   CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
     ((CUAFWinEdDoc*)(pApp->GetDocPtr()))->UpdateAllViews(NULL);   
}

void PlaceLeft(int x, int y)
{
   if (globalData.currLevel < 0)
      return;
   
   int leftSide = (currFacing+3)&3;
   
   switch (m_mode)
   {
   case START_LOC_MODE:
      break;
   case WALL_MODE:
      PlaceWall(leftSide, x, y);
      break;
   case EVENT_MODE:
      break;
   case BG_MODE:
      break;
   case ZONE_MODE:
      break;
   }
     
   CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
     ((CUAFWinEdDoc*)(pApp->GetDocPtr()))->UpdateAllViews(NULL);
}


void PlaceRight(int x, int y)
{
   if (globalData.currLevel < 0)
      return;
   
   int rightSide = (currFacing+1)&3;
   switch (m_mode)
   {
   case START_LOC_MODE:
      break;
   case WALL_MODE:
      PlaceWall(rightSide, x, y);
      break;
   case EVENT_MODE:
      break;
   case BG_MODE:
      break;
   case ZONE_MODE:
      break;
   }
   
   CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
     ((CUAFWinEdDoc*)(pApp->GetDocPtr()))->UpdateAllViews(NULL);
}

void PlaceBlockage(int x, int y, BOOL isBlock)
{
  if (levelData.area[y][x].bkgrnd == isBlock)
    return;

  if (levelData.area[y][x].eventExists)
  {
    MsgBoxInfo("Blockage cannot be placed over an event");
    return;
  }

  // for overland maps only
  tempUndo.type = BLOCKAGE_MODE;
  tempUndo.x = x;
  tempUndo.y = y;
  tempUndo.facing = 0;
  tempUndo.doubleFill = FALSE;
  tempUndo.prevFill = levelData.area[y][x].bkgrnd;
  tempUndo.prevDoubleFill = 0;
  AddUndoData(tempUndo);

  levelData.area[y][x].bkgrnd = isBlock;
  CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
   ((CUAFWinEdDoc*)(pApp->GetDocPtr()))->UpdateAllViews(NULL);
  SET_MODIFIED;
}

void PlaceBackground(int whichSide, int x, int y, int bg)
{
  whichSide &= 3; // Because we will use it as subscript.
  x=(x+levelData.area_width)%levelData.area_width;
  y=(y+levelData.area_height)%levelData.area_height;
  if (bg==-1) bg=currBkGnd;
  
  AREA_MAP_DATA *pArea = &levelData.area[y][x];    


  tempUndo.type = BG_MODE;
  tempUndo.x = x;
  tempUndo.y = y;
  tempUndo.doubleFill = FALSE;
  tempUndo.prevDoubleFill = 0;

  if (!QuadFillBGSlot)
  {
    if (pArea->backgrounds(whichSide) == (BYTE)bg)
      return;

    // place backdrop on one side of square only
    tempUndo.facing = whichSide;    
    tempUndo.prevFill = pArea->backgrounds(whichSide);    
    AddUndoData(tempUndo);
    pArea->backgrounds(whichSide) = (BYTE)bg;
  }
  else
  {
    int samecount=0;
    int i;
    for (i=0;i<4;i++)
    {      
      if (pArea->backgrounds(i) == (BYTE)bg)
        samecount++;
    }

    if (samecount==4)
      return;

    // place same backdrop on all sides of square
    for (i=0;i<4;i++)
    {      
      tempUndo.facing = i;
      tempUndo.prevFill = pArea->backgrounds(i);
      AddUndoData(tempUndo);
      pArea->backgrounds(i) = (BYTE)bg;
    }
  }

  CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
   ((CUAFWinEdDoc*)(pApp->GetDocPtr()))->UpdateAllViews(NULL);
  SET_MODIFIED;
}

void PlaceWall(int whichSide, int x, int y, int wall, int blockage)
// whichSide is absolute, not relative to currFacing..
{
   whichSide &= 3; // Because we will use it as subscript.
   x=(x+levelData.area_width)%levelData.area_width;
   y=(y+levelData.area_height)%levelData.area_height;
   if (wall==-1) wall=currWallSlot;
   if (blockage==-1) blockage=currBlockage;
   
   AREA_MAP_DATA *pArea = &levelData.area[y][x];

   // don't add undo action if same wall already placed
   if (   (pArea->walls(whichSide) == (BYTE)wall)
       && (pArea->blockages(whichSide) == (BlockageType)blockage))
   {
     if (DoubleFillWallSlot)
     {
       y=(y+deltaY[whichSide]+levelData.area_height)%levelData.area_height;
       x=(x+deltaX[whichSide]+levelData.area_width)%levelData.area_width;
       pArea=&levelData.area[y][x];
       if (   (pArea->walls(whichSide+2) == (BYTE)wall)
           && (pArea->blockages(whichSide+2) == (BlockageType)blockage))
       {
         return;
       }
     }
     else
      return;
   }

   tempUndo.type = WALL_MODE;
   tempUndo.x = x;
   tempUndo.y = y;
   tempUndo.facing = whichSide;
   tempUndo.doubleFill = FALSE;      
   tempUndo.prevFill = pArea->walls(whichSide);
   tempUndo.prevBlockage = pArea->blockages(whichSide);

   pArea->walls(whichSide) = (BYTE)wall;
   pArea->blockages(whichSide) = (BlockageType)blockage;
   
   // place wall on opposite side also
   if (DoubleFillWallSlot)
   {
     y=(y+deltaY[whichSide]+levelData.area_height)%levelData.area_height;
     x=(x+deltaX[whichSide]+levelData.area_width)%levelData.area_width;
     pArea=&levelData.area[y][x];
     tempUndo.doubleFill = TRUE;   
     tempUndo.prevDoubleFill = pArea->walls(whichSide+2);
     tempUndo.prevDoubleBlockage = pArea->blockages(whichSide+2);
     pArea->walls(whichSide+2) = (BYTE)wall;
     pArea->blockages(whichSide+2) = (BlockageType)blockage;
   };

   CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
     ((CUAFWinEdDoc*)(pApp->GetDocPtr()))->UpdateAllViews(NULL);
   SET_MODIFIED;
   AddUndoData(tempUndo);
}

void UndoBackground(UndoDataType &data)
{
  AREA_MAP_DATA *pArea=&levelData.area[data.y][data.x];
  pArea->backgrounds(data.facing) = (BYTE)data.prevFill;
  SET_MODIFIED;
}

void UndoWall(UndoDataType &data)
{
  AREA_MAP_DATA *pArea=&levelData.area[data.y][data.x];
  pArea->walls(data.facing) = (BYTE)data.prevFill;
  pArea->blockages(data.facing) = (BlockageType)data.prevBlockage;

  // place wall on opposite side also
  if (data.doubleFill)
  {
    int x=(data.x+deltaX[data.facing]+levelData.area_width)%levelData.area_width;
    int y=(data.y+deltaY[data.facing]+levelData.area_height)%levelData.area_height;
    pArea=&levelData.area[y][x];
    pArea->walls(data.facing+2) = (BYTE)data.prevDoubleFill;
    pArea->blockages(data.facing+2) = (BlockageType)data.prevDoubleBlockage;
  }
  
   CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
     ((CUAFWinEdDoc*)(pApp->GetDocPtr()))->UpdateAllViews(NULL);
   SET_MODIFIED;
}

void UndoAction(UndoDataType &data)
{
  switch (data.type)
  {
  case WALL_MODE:
    wallUndone=true;
    UndoWall(data);
    break;
  case BG_MODE:
  case BLOCKAGE_MODE:
    UndoBackground(data);
    break;
  }

  data.Clear();     
}

void AddUndoData(UndoDataType &data)
{
  if (UndoData.GetCount() >= MAX_UNDO_ACTIONS)
    UndoData.RemoveHead();
  UndoData.AddTail(data);
}

void CUAFWinEdView::OnAccessoptions() 
{
  CAccessDlg dlg;
  const ASLENTRY *aParam;
  CString oldParams;
  aParam = levelData.level_asl.Find("$ACCESSPARAMS");
  if (aParam != NULL)
  {
    dlg.m_accessParameters=aParam->Value();
    oldParams=dlg.m_accessParameters;
  };
  if (dlg.DoModal()==IDOK)
  {
    if (oldParams!=dlg.m_accessParameters)
    {
      levelData.level_asl.Insert("$ACCESSPARAMS",
                                 dlg.m_accessParameters,
                                 ASLF_EDITOR|ASLF_SYSTEM);
      CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
          ((CUAFWinEdDoc*)(pApp->GetDocPtr()))->UpdateAllViews(NULL);
      SET_MODIFIED;
    };
  };
}

void CUAFWinEdView::OnMAPpopEditEvent()
{
  PlaceEvent(m_PopUpX, m_PopUpY);
}

void CUAFWinEdView::OnMAPpopCancel()
{
}

void CUAFWinEdView::OnTogglebgmethod() 
{
  levelData.area[currY][currX].ShowDistantBG = 
    !levelData.area[currY][currX].ShowDistantBG;

  if (!levelData.area[currY][currX].ShowDistantBG)
  {
    levelData.area[currY][currX].DistantBGInBands=FALSE;
    m_ShowBands.ShowWindow(SW_HIDE);
    m_ShowBands.EnableWindow(FALSE);    
  }
  else
  {
    m_ShowBands.ShowWindow(SW_SHOW);
    m_ShowBands.EnableWindow(TRUE);    
  }

  CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
      ((CUAFWinEdDoc*)(pApp->GetDocPtr()))->UpdateAllViews(NULL);
  SET_MODIFIED;
}

void CUAFWinEdView::OnTogglebgbands() 
{
  levelData.area[currY][currX].DistantBGInBands =
    !levelData.area[currY][currX].DistantBGInBands;
  CUAFWinEdApp *pApp = reinterpret_cast<CUAFWinEdApp*>(AfxGetApp());
      ((CUAFWinEdDoc*)(pApp->GetDocPtr()))->UpdateAllViews(NULL);
  SET_MODIFIED;
}

// left mouse button down
void CUAFWinEdView::OnLButtonDown(UINT nFlags, CPoint point) 
{
  UINT squareX, squareY, dx, dy;

  if (MouseToMap(point, squareX, squareY, &dx, &dy))
  {
    if (nFlags & MK_CONTROL) // CTRL-LMBTN moves arrow to clicked square
    {
      MoveTo(squareX, squareY);
    }
    else 
    {
      switch (m_mode)
      {
      case WALL_MODE:
      case BG_MODE: 
      case ZONE_MODE:
      case BLOCKAGE_MODE:
        leftbuttondown=true;
        break;      
      case START_LOC_MODE:
      case EVENT_MODE:
      case ENTRY_POINT_MODE:
        leftbuttondown=false;
        break;
      default:
        leftbuttondown=false;
        break;
      }

      HandleLeftClick(squareX, squareY, dx, dy);
    }
  }
  else if (m_KwikKlik && MouseToPicture(point, squareX))
  {
    HandlePictureLeftClick(squareX);
  }
  else
  {
    leftbuttondown=false;
    rightbuttondown=false;
  }

  CFormView::OnLButtonDown(nFlags, point);
  m_MapPicture.Invalidate(FALSE);
}

void CUAFWinEdView::OnRButtonDown(UINT nFlags, CPoint point) 
{
  // If the user clicked in the dungeon grid...
  rightbuttondown=false;
  UINT squareX, squareY;
  UINT xOffset, yOffset; // Pixel offset within cell.
  int wall;  // Which wall are we talking about?
  if (((nFlags & MK_CONTROL)!=0)
      && MouseToMap(point, squareX, squareY, &xOffset, &yOffset))
  {
    CMenu menu;
    VERIFY(menu.LoadMenu(IDR_MAPpopup));
    CMenu* pPopup = menu.GetSubMenu(0);
    ASSERT(pPopup != NULL);
    MapToMouse(point, squareX, squareY);
    m_PopUpX=squareX;
    m_PopUpY=squareY;  // For menu selection handler
    pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
  }
  else 
  {
   switch (m_mode)
    {
    case WALL_MODE:
      rightbuttondown=true;
      if (m_KwikKlik)
      {
        if (MouseToMap(point, squareX, squareY, &xOffset, &yOffset))
        {
          wall=MousePointToWall(xOffset, yOffset);
          PlaceWall(wall, squareX, squareY, 0, 0);
          CDataExchange DX(this, false);
          UpdateBlockageBox(&DX);
        }
        else if (MouseToPicture(point, squareX))
          HandlePictureRightClick(squareX);
      };
      break;

    case BG_MODE:
      if (m_KwikKlik)
      {
        if (MouseToMap(point, squareX, squareY, &xOffset, &yOffset))
        {
          wall=MousePointToWall(xOffset, yOffset);
          PlaceBackground(wall, squareX, squareY, 0);
          CDataExchange DX(this, false);
          UpdateBlockageBox(&DX);
        }
      }
      break;

    case BLOCKAGE_MODE:      
      if (MouseToMap(point, squareX, squareY, &xOffset, &yOffset))
      { 
        PlaceBlockage(squareX, squareY, FALSE);
        CDataExchange DX(this, false);
        UpdateBlockageBox(&DX);
      }
      break;
    };
    CFormView::OnRButtonDown(nFlags, point);
  };
  m_MapPicture.Invalidate(FALSE);
}

// left mouse button up
void CUAFWinEdView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	leftbuttondown=false;
	CFormView::OnLButtonUp(nFlags, point);
}

void CUAFWinEdView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	rightbuttondown=false;	
	CFormView::OnRButtonUp(nFlags, point);
}

void CUAFWinEdView::OnMouseMove(UINT nFlags, CPoint point) 
{
  unsigned int newX,newY,dx,dy;
  if (!MouseToMap(point, newX, newY, &dx, &dy)) 
  {
    newX=0xffffffff;
    newY=0xffffffff;
    leftbuttondown=false;
    rightbuttondown=false;
  };

  if ((m_mouseX != newX) || (m_mouseY!=newY))  
  {
    if (m_BLOCK_Type)  
    { 
      m_mouseX=newX;
      m_mouseY=newY;
      CDataExchange DX(this, false);
      UpdateBlockageBox(&DX);
    }
  }

  if (leftbuttondown)
  {  
     if (vkey)
     {
       int facing=MousePointToWall(dx, dy);
       if ((facing == 1)||(facing == 3))
         HandleLeftClick(newX, newY, dx, dy);
     }
     else if (hkey)
     {
       int facing=MousePointToWall(dx, dy);
       if ((facing == 0)||(facing == 2))
         HandleLeftClick(newX, newY, dx, dy);
     }     
     else
       HandleLeftClick(newX, newY, dx, dy);
  }
  else if (rightbuttondown)
  {
    if (m_KwikKlik)
    {
      int facing=MousePointToWall(dx, dy);
      PlaceWall(facing, newX, newY, 0, 0);
      CDataExchange DX(this, false);
      UpdateBlockageBox(&DX);
    }
  }

  m_mouseX=newX;
  m_mouseY=newY;
	CFormView::OnMouseMove(nFlags, point);
}

void CUAFWinEdView::OnKillFocus(CWnd* pNewWnd) 
{
	CFormView::OnKillFocus(pNewWnd);
	
  memset(keypresses,0,sizeof(keypresses));
  shift=false;
  control=false;
  hkey=false;
  vkey=false;
  leftbuttondown=false;
  rightbuttondown=false;
}

LRESULT CUAFWinEdView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	return CFormView::WindowProc(message, wParam, lParam);
}
