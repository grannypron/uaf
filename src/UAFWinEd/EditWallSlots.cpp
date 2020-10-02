/******************************************************************************
* Filename: EditWallSlots.cpp
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
#include "GlobalData.h"
#include "Level.h"
#include "PicSlot.h"
#include "Graphics.h"
#include "WallPicDlg.h"
#include "ColorBox.h"
#include "Spell.h"
#include "SpellDlg.h"
#include "ScrollPicDlg.h"
#include "SpecialKeyBlockage.h"
#include "EditWallSlots.h"
#include "Help.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditWallSlots dialog


CEditWallSlots::CEditWallSlots(CWnd* pParent /*=NULL*/)
	: CDialog(CEditWallSlots::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditWallSlots)
	m_Obstruction = 0;
	m_ImageFormat = 0;
	m_BlendAmount = 0;
	m_UseBlend = FALSE;
	m_DoorFirst = FALSE;
	//}}AFX_DATA_INIT

  m_Obstruction = currBlockage;
  m_Slot = currWallSlot;
}


void CEditWallSlots::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditWallSlots)
	DDX_Control(pDX, IDC_AVART, m_AVArtSelect);
	DDX_Control(pDX, IDC_SLOTTAB, m_SlotTab);
	DDX_Control(pDX, IDC_UNLOCKSPELL, m_UnlockSpell);
  DDX_Control(pDX, IDC_UnlockSpellHelp, m_UnlockSpellHelp);
	DDX_Control(pDX, IDC_CHANGEWALL, m_ChangeWall);
	DDX_Control(pDX, IDC_CHANGEOVERLAY, m_ChangeOverlay);
	DDX_Control(pDX, IDC_CHANGEDOOR, m_ChangeDoor);
	DDX_Control(pDX, IDC_OBCOLOR9, m_ObColor9);
	DDX_Control(pDX, IDC_OBCOLOR8, m_ObColor8);
	DDX_Control(pDX, IDC_OBCOLOR7, m_ObColor7);
	DDX_Control(pDX, IDC_OBCOLOR6, m_ObColor6);
	DDX_Control(pDX, IDC_OBCOLOR5, m_ObColor5);
	DDX_Control(pDX, IDC_OBCOLOR4, m_ObColor4);
	DDX_Control(pDX, IDC_OBCOLOR3, m_ObColor3);
	DDX_Control(pDX, IDC_OBCOLOR2, m_ObColor2);
	DDX_Control(pDX, IDC_OBCOLOR16, m_ObColor16);
	DDX_Control(pDX, IDC_OBCOLOR15, m_ObColor15);
	DDX_Control(pDX, IDC_OBCOLOR14, m_ObColor14);
	DDX_Control(pDX, IDC_OBCOLOR13, m_ObColor13);
	DDX_Control(pDX, IDC_OBCOLOR12, m_ObColor12);
	DDX_Control(pDX, IDC_OBCOLOR11, m_ObColor11);
	DDX_Control(pDX, IDC_OBCOLOR10, m_ObColor10);
	DDX_Control(pDX, IDC_OBCOLOR1, m_ObColor1);
	DDX_Control(pDX, IDC_SLOTCOLOR9, m_Color9);
	DDX_Control(pDX, IDC_SLOTCOLOR8, m_Color8);
	DDX_Control(pDX, IDC_SLOTCOLOR7, m_Color7);
	DDX_Control(pDX, IDC_SLOTCOLOR6, m_Color6);
	DDX_Control(pDX, IDC_SLOTCOLOR5, m_Color5);
	DDX_Control(pDX, IDC_SLOTCOLOR4, m_Color4);
	DDX_Control(pDX, IDC_SLOTCOLOR3, m_Color3);
	DDX_Control(pDX, IDC_SLOTCOLOR2, m_Color2);
	DDX_Control(pDX, IDC_SLOTCOLOR16, m_Color16);
	DDX_Control(pDX, IDC_SLOTCOLOR15, m_Color15);
	DDX_Control(pDX, IDC_SLOTCOLOR14, m_Color14);
	DDX_Control(pDX, IDC_SLOTCOLOR13, m_Color13);
	DDX_Control(pDX, IDC_SLOTCOLOR12, m_Color12);
	DDX_Control(pDX, IDC_SLOTCOLOR11, m_Color11);
	DDX_Control(pDX, IDC_SLOTCOLOR10, m_Color10);
	DDX_Control(pDX, IDC_SLOTCOLOR1, m_Color1);
	DDX_Control(pDX, IDC_SLOT9, m_Slot9);
	DDX_Control(pDX, IDC_SLOT8, m_Slot8);
	DDX_Control(pDX, IDC_SLOT7, m_Slot7);
	DDX_Control(pDX, IDC_SLOT6, m_Slot6);
	DDX_Control(pDX, IDC_SLOT5, m_Slot5);
	DDX_Control(pDX, IDC_SLOT4, m_Slot4);
	DDX_Control(pDX, IDC_SLOT3, m_Slot3);
	DDX_Control(pDX, IDC_SLOT2, m_Slot2);
	DDX_Control(pDX, IDC_SLOT16, m_Slot16);
	DDX_Control(pDX, IDC_SLOT15, m_Slot15);
	DDX_Control(pDX, IDC_SLOT14, m_Slot14);
	DDX_Control(pDX, IDC_SLOT13, m_Slot13);
	DDX_Control(pDX, IDC_SLOT12, m_Slot12);
	DDX_Control(pDX, IDC_SLOT11, m_Slot11);
	DDX_Control(pDX, IDC_SLOT10, m_Slot10);
	DDX_Control(pDX, IDC_SLOT1, m_Slot1);
	DDX_Radio(pDX, IDC_OPEN_BLK, m_Obstruction);
	DDX_Text(pDX, IDC_IMAGEFORMAT, m_ImageFormat);
	DDX_Text(pDX, IDC_OVERLAYBLENDAMOUNT, m_BlendAmount);
	DDX_Check(pDX, IDC_USEOVERLAYBLEND, m_UseBlend);
	DDX_Check(pDX, IDC_DOORFIRST, m_DoorFirst);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditWallSlots, CDialog)
	//{{AFX_MSG_MAP(CEditWallSlots)
	ON_BN_CLICKED(IDC_CHANGEDOOR, OnChangedoor)
	ON_BN_CLICKED(IDC_CHANGEOVERLAY, OnChangeoverlay)
	ON_BN_CLICKED(IDC_CHANGEWALL, OnChangewall)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_DELETESLOT, OnDeleteslot)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_UNLOCKSPELL, OnUnlockspell)
	ON_BN_CLICKED(IDC_UnlockSpellHelp, OnUnlockSpellHelp)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SLOTTAB, OnSelchangeSlottab)
	ON_BN_CLICKED(IDC_AVART, OnAvart)
	ON_BN_CLICKED(IDC_UNLOCKKEYS, OnUnlockkeys)
  ON_BN_CLICKED(IDC_WALL_SLOTS, OnWallSlotsQuestion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDlgWallPicture& CEditWallSlots::GetSlotPic(int index)
{
  int slot=index%16;
  switch (slot)
  {
  case 0: return m_Slot1; break;
  case 1: return m_Slot2; break;
  case 2: return m_Slot3; break;
  case 3: return m_Slot4; break;
  case 4: return m_Slot5; break;
  case 5: return m_Slot6; break;
  case 6: return m_Slot7; break;
  case 7: return m_Slot8; break;
  case 8: return m_Slot9; break;
  case 9: return m_Slot10; break;
  case 10: return m_Slot11; break;
  case 11: return m_Slot12; break;
  case 12: return m_Slot13; break;
  case 13: return m_Slot14; break;
  case 14: return m_Slot15; break;
  case 15: return m_Slot16; break;
  default: ASSERT(FALSE); return m_Slot1;
  }  
}

CColorBox& CEditWallSlots::GetSlotColor(int index)
{
  int slot=index%16;
  switch (slot)
  {
  case 0: return m_Color1; break;
  case 1: return m_Color2; break;
  case 2: return m_Color3; break;
  case 3: return m_Color4; break;
  case 4: return m_Color5; break;
  case 5: return m_Color6; break;
  case 6: return m_Color7; break;
  case 7: return m_Color8; break;
  case 8: return m_Color9; break;
  case 9: return m_Color10; break;
  case 10: return m_Color11; break;
  case 11: return m_Color12; break;
  case 12: return m_Color13; break;
  case 13: return m_Color14; break;
  case 14: return m_Color15; break;
  case 15: return m_Color16; break;
  default: ASSERT(FALSE); return m_Color1;
  }  
}

CColorBox& CEditWallSlots::GetSlotObColor(int index)
{
  int slot=index%16;
  switch (slot)
  {
  case 0: return m_ObColor1; break;
  case 1: return m_ObColor2; break;
  case 2: return m_ObColor3; break;
  case 3: return m_ObColor4; break;
  case 4: return m_ObColor5; break;
  case 5: return m_ObColor6; break;
  case 6: return m_ObColor7; break;
  case 7: return m_ObColor8; break;
  case 8: return m_ObColor9; break;
  case 9: return m_ObColor10; break;
  case 10: return m_ObColor11; break;
  case 11: return m_ObColor12; break;
  case 12: return m_ObColor13; break;
  case 13: return m_ObColor14; break;
  case 14: return m_ObColor15; break;
  case 15: return m_ObColor16; break;
  default: ASSERT(FALSE); return m_ObColor1;
  }  
}

/////////////////////////////////////////////////////////////////////////////
// CEditWallSlots message handlers

void CEditWallSlots::OnChangedoor() 
{

  if (m_Slot <= 0)
    return;

  CWallPicDlg dlg(WallSets[m_Slot].doorFile,WallSets[m_Slot].soundFile);
  dlg.m_UseAV=FALSE;
  dlg.m_ImageFormat = WallSets[m_Slot].SlotType;

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    SET_MODIFIED;
    WallSets[m_Slot].doorFile = dlg.m_Filename;
    WallSets[m_Slot].doorHash=0;
    GraphicsMgr.ReleaseSurface(WallSets[m_Slot].doorSurface);

    //WallSets[m_Slot].doorSurface = GraphicsMgr.AddSurface(dlg.m_Filename, DoorDib);
    WallSets[m_Slot].GetDoorSurface();

    LoadSlot(m_Slot);
    InvalidateSlot(m_Slot);
    ChangeSlotSelections(m_Slot, m_Slot);
  }
}

void CEditWallSlots::OnChangeoverlay() 
{
  if (m_Slot <= 0)
    return;

  CWallPicDlg dlg(WallSets[m_Slot].overlayFile,WallSets[m_Slot].soundFile);
  dlg.m_UseAV=FALSE;
  dlg.m_ImageFormat = WallSets[m_Slot].SlotType;

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    SET_MODIFIED;
    WallSets[m_Slot].overlayFile = dlg.m_Filename;
    WallSets[m_Slot].overlayHash=0;
    GraphicsMgr.ReleaseSurface(WallSets[m_Slot].overlaySurface);
    
    WallSets[m_Slot].GetOverlaySurface();
    //WallSets[m_Slot].overlaySurface = GraphicsMgr.AddSurface(dlg.m_Filename, OverlayDib);

    LoadSlot(m_Slot);
    InvalidateSlot(m_Slot);
    ChangeSlotSelections(m_Slot, m_Slot);
  }
}

void CEditWallSlots::OnChangewall() 
{
  if (m_Slot <= 0)
    return;

  CWallPicDlg dlg(WallSets[m_Slot].wallFile,WallSets[m_Slot].soundFile);
  dlg.m_UseAV=TRUE;
  dlg.m_DrawAV = WallSets[m_Slot].drawAV;
  dlg.m_ImageFormat = WallSets[m_Slot].SlotType;

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    SET_MODIFIED;
    WallSets[m_Slot].wallFile = dlg.m_Filename;
    WallSets[m_Slot].soundFile = dlg.m_Sound;
    WallSets[m_Slot].wallHash=0;
    WallSets[m_Slot].drawAV = dlg.m_DrawAV;

    GraphicsMgr.ReleaseSurface(WallSets[m_Slot].wallSurface);
    //WallSets[m_Slot].wallSurface = GraphicsMgr.AddSurface(dlg.m_Filename, WallDib);
    WallSets[m_Slot].GetWallSurface();

    LoadSlot(m_Slot);
    InvalidateSlot(m_Slot);
    ChangeSlotSelections(m_Slot, m_Slot);
  }
}

BOOL CEditWallSlots::OnInitDialog() 
{
	CDialog::OnInitDialog();
  int i;

  m_UseBlend = WallSets[m_Slot].BlendOverlay;
  m_BlendAmount = WallSets[m_Slot].BlendAmount;
  m_DoorFirst = WallSets[m_Slot].doorFirst;

  TCITEM tabData;
  char label[50];

  m_SlotTab.DeleteAllItems();

  // should be an even multiple of 16
  ASSERT((MAX_WALLSETS % 16) == 0);
  int NumTabs = MAX_WALLSETS / 16;
  for (i=0;i<NumTabs;i++)
  {
    int start = (i*16)+1;
    int stop = start+15;
    sprintf(label, "Walls %u-%u", start,stop);
    tabData.mask = TCIF_TEXT;
    tabData.iImage=-1;
    tabData.pszText=label;
    tabData.cchTextMax=strlen(label);
    tabData.lParam=0;
    m_SlotTab.InsertItem(i,&tabData);
  }

  m_SlotTab.SetCurFocus(m_Slot/16);
  {
    int startSlot;
    startSlot = m_Slot/16 * 16;
    // no matter how many wallsets, only show 16 at a time
    for (i=0;i<16;i++)
    {
      GetSlotPic(i).CreateBlankPicture();
      FillControl(&GetSlotObColor(i),GetObstructionColor(startSlot+i));
      FillControl(&GetSlotColor(i),GetWallColor(startSlot+i));
    }
  };

  UpdateData(FALSE);

  LoadSlotTab();

  ChangeSlotSelections(m_Slot, m_Slot);

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(spellData.GetSpellName(WallSets[m_Slot].UnlockSpellID), 
              name, MAX_MEDITBUTTON_TEXT);
  m_UnlockSpell.SetWindowText(name);       
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditWallSlots::LoadSlot(int index)
{
  // leave slot 0 blank (=black, color 0)
  if (index == 0) 
  {
    EmptySlot(index);
    WallSets[index].Clear();
    return;
  }

  BYTE format=DEFAULT_FORMAT_TYPE;  
  int slot=index%16;
  
  CDlgWallPicture &pic = GetSlotPic(slot);
  pic.LoadSourcePictures(WallSets[index].doorFile, 
                         WallSets[index].overlayFile,
                         WallSets[index].wallFile,
                         WallSets[index].doorFirst);
  format = pic.m_format;
  //UpdateData(TRUE);
  WallSets[index].SlotType = format;
  m_ImageFormat = format;
  {
    int startSlot;
    startSlot = index/16 * 16;
    // no matter how many wallsets, only show 16 at a time
    for (int i=0;i<16;i++)
    {
      GetSlotPic(i).CreateBlankPicture();
      FillControl(&GetSlotObColor(i),GetObstructionColor(startSlot+i));
      FillControl(&GetSlotColor(i),GetWallColor(startSlot+i));
    }
  };
  UpdateData(FALSE);
}

void CEditWallSlots::EmptySlot(int index)
{
  GetSlotPic(index).ErasePicture();
}

void CEditWallSlots::InvalidateSlot(int index)
{
  int slot=index%16;
  GetSlotPic(slot).Invalidate(FALSE);
}


void CEditWallSlots::ChangeSlotSelections(int oldSlot, int newSlot)
{
  CRect oldRect(0,0,0,0);
  BOOL enable = (newSlot > 0);
  int slot;

  m_ChangeDoor.EnableWindow(enable);
  m_ChangeOverlay.EnableWindow(enable);
  m_ChangeWall.EnableWindow(enable);

  if (oldSlot != newSlot)
  {
    // unselect old slot
    slot=oldSlot%16;
    CDlgWallPicture &oldpic = GetSlotPic(slot);
    oldpic.Highlight(FALSE);

    // invalidate border region so it
    // removes old border
    oldpic.GetWindowRect(&oldRect);
    ScreenToClient(&oldRect);
    if (oldRect != CRect(0,0,0,0))
    {
      oldRect.InflateRect(4,4); // tracker width is 4 (DlgPicture.cpp)
      InvalidateRect(oldRect);
    }
  }

  // select new slot
  slot=newSlot%16;
  CDlgWallPicture &newpic = GetSlotPic(slot);
  newpic.Highlight(TRUE);

  // invalidate border region so it
  // draws new border
  newpic.GetWindowRect(&oldRect);
  ScreenToClient(&oldRect);
  if (oldRect != CRect(0,0,0,0))
  {
    oldRect.InflateRect(4,4); // tracker width is 4 (DlgPicture.cpp)
    InvalidateRect(oldRect);
  }

  m_ImageFormat = WallSets[newSlot].SlotType;
  m_UseBlend = WallSets[newSlot].BlendOverlay;
  m_BlendAmount = WallSets[newSlot].BlendAmount;
  m_DoorFirst = WallSets[newSlot].doorFirst;
  m_UnlockSpell.SetWindowText(WallSets[newSlot].UnlockSpellID);
  UpdateData(FALSE);
}

void CEditWallSlots::HandleLeftButton(UINT nFlags, CPoint point)
{
  UpdateData(TRUE);
  WallSets[m_Slot].BlendOverlay = m_UseBlend;
  WallSets[m_Slot].BlendAmount = m_BlendAmount;
  WallSets[m_Slot].doorFirst = m_DoorFirst;
  int oldSlot = m_Slot;
  ClientToScreen(&point);
  CRect hitRect(point, CSize(1,1));
  CRect rect, sRect;
  int baseSlot = m_SlotTab.GetCurFocus();

  for (int i=0;i<16;i++)
  {
    GetSlotPic(i).GetWindowRect(&sRect);
    if (rect.IntersectRect(hitRect, sRect))
    {
      m_Slot = i + (baseSlot*16);
      m_ImageFormat = WallSets[m_Slot].SlotType;
      m_UseBlend = WallSets[m_Slot].BlendOverlay;
      m_BlendAmount = WallSets[m_Slot].BlendAmount;
      m_DoorFirst = WallSets[m_Slot].doorFirst;

      char name[MAX_MEDITBUTTON_TEXT+1];
      if (m_AVArtSelect.m_hWnd != NULL)
      {
         getBaseName(WallSets[m_Slot].AVArtFile, name, MAX_MEDITBUTTON_TEXT);
         m_AVArtSelect.SetWindowText(name);
      }

      UpdateData(FALSE);
      break;
    }
  }

  if (oldSlot != m_Slot)
    ChangeSlotSelections(oldSlot, m_Slot);
}

void CEditWallSlots::OnLButtonUp(UINT nFlags, CPoint point) 
{
  HandleLeftButton(nFlags, point);
	CDialog::OnLButtonUp(nFlags, point);
}

void CEditWallSlots::OnOK() 
{
  UpdateData(TRUE);
  if (m_Slot == 0) m_Obstruction = OpenBlk;
  currBlockage = m_Obstruction;
  currWallSlot = m_Slot;
  WallSets[currWallSlot].BlendOverlay = m_UseBlend;
  WallSets[currWallSlot].BlendAmount = m_BlendAmount;  
  WallSets[currWallSlot].doorFirst = m_DoorFirst;
  for (int i=0;i<MAX_WALLSETS;i++)
  {
    if (WallSets[i].overlayFile=="")
    {
      WallSets[i].BlendOverlay = FALSE;
      WallSets[i].BlendAmount = 0;  
    }
  }
	CDialog::OnOK();
}

void CEditWallSlots::OnDeleteslot() 
{
   BOOL erase = FALSE;

   if (m_Slot <= 0)
      return;

   if (CheckLevelForWallSlot(m_Slot))
   {
      if (MsgBoxYesNo("This slot is used by the current level.\nDelete slot anyway?",
                       "Slot in use warning") == IDYES)
         erase = TRUE;
   }
   else
   {
      if (MsgBoxYesNo("Delete wall slot?",
                       "Slot Deletion Warning") == IDYES)
         erase = TRUE;      
   }
      
   if (erase)
   {
      EmptySlot(m_Slot);	
      WallSets[m_Slot].Clear();
      InvalidateSlot(m_Slot);
   }
}

void CEditWallSlots::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
  HandleLeftButton(nFlags, point);
	CDialog::OnLButtonDblClk(nFlags, point);
  OnChangewall();
}

void CEditWallSlots::OnUnlockSpellHelp(void)
{
  CString message;
  message = "There are two ways to define a spell to\n"
            "open spell-locked doors\n"
            "\n"
            "(1) Specify the name of a spell.  If any party\n"
            "member has that spell memorized then the door\n"
            "will be opened.\n"
            "\n"
            "(2) Specify the name of a Hook script.  The\n"
            "engine will search all the Special Abilities of\n"
            "all the party's memorized spells and will run any\n"
            "scipts with the specified name.  If any such script\n"
            "returns a value starting with 'Y', the door will open.\n"
            "\n"
            "See the documentation for a more complete description\n"
            "of the \"Unlock Spelled Doors\" hook.\n"
            "\n"
            "Hint: To easily specify a script name, you can create\n"
            "a spell with that name and assign it to a non-existent\n"
            "spell school so that it can never be used as a spell.";

  MsgBoxInfo(message, "Unlock Spell Help");
}

void CEditWallSlots::OnUnlockspell() 
{
  spellBookType m_books;
  m_books.spellLimits.SetToMaxLimits();

  // add single spell to book
  if (   (WallSets[m_Slot].UnlockSpellID.IsValidSpell()) )
      //PRS July 2009 && (WallSets[m_Slot].UnlockSpell != NO_SPELL))
  {


/*
    m_books.KnowSpell(WallSets[m_Slot].UnlockSpellClass, //baseclass
                      WallSets[m_Slot].UnlockSpellClass, //school           
                      WallSets[m_Slot].UnlockSpellID,    //spellID
                      WallSets[m_Slot].UnlockSpellLevel, //spell Level
                      TRUE);
*/
    BASECLASS_ID baseclassID;
    SCHOOL_ID schoolID;
    int spellLevel;
    SPELL_ID spellID;
    const SPELL_DATA *pSpell;

    spellID = WallSets[m_Slot].UnlockSpellID;
    pSpell = spellData.PeekSpell(spellID);
    schoolID = pSpell->schoolID;
    baseclassID = ""; //*pSpell->allowedBaseclasses.PeekBaseclassID(0);
    spellLevel = pSpell->Level;

    //m_books.KnowSpell(baseclassData.PeekBaseclass(baseclassID),
    //                  schoolID,           
    //                  spellID,
    //                  spellLevel,
    //                  TRUE);
    m_books.KnowSpell(baseclassID,
                      schoolID,           
                      spellID,
                      spellLevel,
                      TRUE);
    //NotImplemented(0xd341a9, false);

  }

  // allow any spell class to be chosen
  CSpell dlg(m_books, 1);

  if (dlg.DoModal() == IDOK)
  {
    //WallSets[m_Slot].UnlockSpellClass = 0;
    //WallSets[m_Slot].UnlockSpellLevel = 0;
    WallSets[m_Slot].UnlockSpellID.Clear();      

    dlg.GetSpellList(m_books);
/*
    POSITION pos = m_books.GetHeadPosition();
    if (pos != NULL)
    {
      SPELL_DATA *data;
      if ((data = spellData.GetSpellData(m_books.PeekAtPos(pos).spell)) != NULL)
      {
        WallSets[m_Slot].UnlockSpellClass = data->schoolMask;
        WallSets[m_Slot].UnlockSpellLevel = data->Level;
        WallSets[m_Slot].UnlockSpell = m_books.PeekAtPos(pos).spell;      
      }
    }
*/
    {
      int i,n;
      n = m_books.GetCount();
      for (i=0; i<n; i++)
      {
        const SPELL_DATA *pSpellData;
        if ((pSpellData = spellData.PeekSpell(m_books.PeekCharacterSpell(i)->spellID)) != NULL)
        {
          //WallSets[m_Slot].UnlockSpellClass = pSpellData->schoolMask;
          //WallSets[m_Slot].UnlockSpellLevel = pSpellData->Level;
          WallSets[m_Slot].UnlockSpellID = m_books.PeekCharacterSpell(i)->spellID; 
        };
      };
    };

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(spellData.GetSpellName(WallSets[m_Slot].UnlockSpellID), 
                name, MAX_MEDITBUTTON_TEXT);
    m_UnlockSpell.SetWindowText(name);     
  }
}

void CEditWallSlots::OnSelchangeSlottab(NMHDR* pNMHDR, LRESULT* pResult) 
{	
  LoadSlotTab();
	*pResult = 0;
}

void CEditWallSlots::LoadSlotTab()
{
  CWaitCursor cursor;
  int tab = m_SlotTab.GetCurFocus();
  int StartSlot = (tab * 16);
  int StopSlot = StartSlot+16;

  for (int i=StartSlot;i<StopSlot;i++)
    LoadSlot(i);

  // if m_Slot is not in range of current tab...
  if ((m_Slot < StartSlot) || (m_Slot >= StopSlot))
  {
    // We just jumped to the next tab and we need to adjust
    // the current slot.
    //
    // If m_Slot was 1 (tab 0, slots 0-15), and we jump to tab 1
    // we change m_Slot to 17 (tab 1, slots 16-31)

    m_Slot = StartSlot + (m_Slot%16);
  }

  m_ImageFormat = WallSets[m_Slot].SlotType;
  m_UseBlend = WallSets[m_Slot].BlendOverlay;
  m_BlendAmount = WallSets[m_Slot].BlendAmount;
  m_DoorFirst = WallSets[m_Slot].doorFirst;

  char name[MAX_MEDITBUTTON_TEXT+1];
  if (m_AVArtSelect.m_hWnd != NULL)
  {
     getBaseName(WallSets[m_Slot].AVArtFile, name, MAX_MEDITBUTTON_TEXT);
     m_AVArtSelect.SetWindowText(name);
  }

  UpdateData(FALSE);
}

void CEditWallSlots::OnAvart() 
{
  CScrollPicDlg dlg(WallSets[m_Slot].AVArtFile, rte.WallArtDir(), "");  

  if (dlg.DoModal()==IDOK)
  {
    WallSets[m_Slot].AVArtFile = dlg.m_Filename;
    StripFilenamePath(WallSets[m_Slot].AVArtFile);
    CString defavart;
    defavart = ede.TemplateAreaArtDir() + AREAVIEWART; //Format("%s%s", global_editorResourceDir, AREAVIEWART);
    if (dlg.m_Filename.CompareNoCase(defavart)==0)
      WallSets[m_Slot].AVArtFile="";
      
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(WallSets[m_Slot].AVArtFile, name, MAX_MEDITBUTTON_TEXT);
    m_AVArtSelect.SetWindowText(name);
  }
}

void CEditWallSlots::OnUnlockkeys() 
{
  // set to default special keys if index slot is empty
  for (int i=0;i<MAX_SPECIAL_KEYS;i++)
  {
    if (levelData.blockageKeys.SpecialKeys[i]<=0)
      levelData.blockageKeys.SpecialKeys[i] = globalData.keyData.GetItemKeyUsingSequence(i+1);
  }

  CSpecialKeyBlockage dlg(levelData.blockageKeys);
  if (dlg.DoModal()==IDOK)
  {
    SET_MODIFIED;
    dlg.GetData(levelData.blockageKeys);
  }
}

void CEditWallSlots::OnWallSlotsQuestion() 
{
  CHelp dlg("WALL_SLOTS");
  dlg.DoModal();
}

