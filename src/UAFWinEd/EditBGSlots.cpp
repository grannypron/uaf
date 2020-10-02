/******************************************************************************
* Filename: EditBGSlots.cpp
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
#include "BkGndPicDlg.h"
#include "ColorBox.h"
#include "EditBGSlots.h"
#include "Graphics.h"
#include "Help.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditBGSlots dialog


CEditBGSlots::CEditBGSlots(CWnd* pParent /*=NULL*/)
	: CDialog(CEditBGSlots::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditBGSlots)
	//}}AFX_DATA_INIT

  m_Slot = currBkGnd;
}

void CEditBGSlots::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditBGSlots)
  DDX_Control(pDX, IDC_SLOTTAB, m_SlotTab);
	DDX_Control(pDX, IDC_CHANGEBG, m_ChangeBG);
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
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditBGSlots, CDialog)
	//{{AFX_MSG_MAP(CEditBGSlots)
	ON_BN_CLICKED(IDC_CHANGEBG, OnChangeBG)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_DELETESLOT, OnDeleteslot)
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY(TCN_SELCHANGE, IDC_SLOTTAB, OnSelchangeSlottab)
  ON_BN_CLICKED(IDC_BG_SLOTS, OnBGSlotsQuestion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDlgPicture& CEditBGSlots::GetSlotPic(int index)
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

CColorBox& CEditBGSlots::GetSlotColor(int index)
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
/////////////////////////////////////////////////////////////////////////////
// CEditBGSlots message handlers

void CEditBGSlots::OnChangeBG() 
{
  CBkGndPicDlg dlg(BackgroundSets[m_Slot]); // destructor will release backdrop surfaces for that slot
  if (dlg.DoModal() == IDOK)
  {
    SET_MODIFIED;
    GraphicsMgr.ReleaseSurface(BackgroundSets[m_Slot].backgroundSurface);
    GraphicsMgr.ReleaseSurface(BackgroundSets[m_Slot].backgroundSurfaceAlt);
    
    BackgroundSets[m_Slot] = dlg.m_data;    
    BackgroundSets[m_Slot].backgroundFileHash=0;
    BackgroundSets[m_Slot].backgroundFileAltHash=0;
    BackgroundSets[m_Slot].GetBackgroundSurface();
    BackgroundSets[m_Slot].GetBackgroundAltSurface();

    LoadSlot(m_Slot);
    ChangeSlotSelections(m_Slot, m_Slot);
  }
  else
  {
    GraphicsMgr.ReleaseSurface(BackgroundSets[m_Slot].backgroundSurface);
    GraphicsMgr.ReleaseSurface(BackgroundSets[m_Slot].backgroundSurfaceAlt);

    BackgroundSets[m_Slot].backgroundFileHash=0;
    BackgroundSets[m_Slot].backgroundFileAltHash=0;
    BackgroundSets[m_Slot].GetBackgroundSurface();
    BackgroundSets[m_Slot].GetBackgroundAltSurface();
  }
}

BOOL CEditBGSlots::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  TCITEM tabData;
  char label[50];
  int i;
  m_SlotTab.DeleteAllItems();

  // should be an even multiple of 16
  ASSERT((MAX_BACKGROUNDS % 16) == 0);
  int NumTabs = MAX_BACKGROUNDS / 16;
  for (i=0;i<NumTabs;i++)
  {
    int start = (i*16)+1;
    int stop = start+15;
    sprintf(label, "Backgrounds %u-%u", start,stop);
    tabData.mask = TCIF_TEXT;
    tabData.iImage=-1;
    tabData.pszText=label;
    tabData.cchTextMax=strlen(label);
    tabData.lParam=0;    
    m_SlotTab.InsertItem(i,&tabData);
  }

  m_SlotTab.SetCurFocus(m_Slot/16);

  for (i=0;i<16;i++)
  {
    GetSlotPic(i).CreateBlankPicture();
    FillControl(&GetSlotColor(i),GetBackdropColor(i));
  }  

  UpdateData(FALSE);
  LoadSlotTab();
  ChangeSlotSelections(m_Slot, m_Slot);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditBGSlots::LoadSlot(int index)
{
  // leave slot 0 blank (=black, color 0)
  if (index == 0) 
  {
    EmptySlot(index);
    BackgroundSets[index].Clear();
    return;
  }
  GetSlotPic(index).LoadPicture(
                           BackgroundSets[index].backgroundFile, 
                           BackGndDib, 
                           rte.BackgroundArtDir(), 
                           ede.TemplateBackgroundArtDir());
}


void CEditBGSlots::ChangeSlotSelections(int oldSlot, int newSlot)
{
  CRect oldRect(0,0,0,0);
  BOOL enable = (newSlot > 0);
  m_ChangeBG.EnableWindow(enable);

  // un-select old slot
  CDlgPicture &oldPic = GetSlotPic(oldSlot);
  oldPic.Highlight(FALSE);
  oldPic.GetWindowRect(&oldRect);
  ScreenToClient(&oldRect);

  if (oldRect != CRect(0,0,0,0))
  {
    oldRect.InflateRect(4,4);
    InvalidateRect(oldRect);
  }

  // select new slot
  GetSlotPic(newSlot).Highlight(TRUE);
}

void CEditBGSlots::EmptySlot(int index)
{
  GetSlotPic(index).ErasePicture();
}

void CEditBGSlots::HandleLeftButton(UINT nFlags, CPoint point)
{
  UpdateData(TRUE);
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
      break;
    }
  }
  if (oldSlot != m_Slot)
    ChangeSlotSelections(oldSlot, m_Slot);
}

void CEditBGSlots::OnLButtonUp(UINT nFlags, CPoint point) 
{
   HandleLeftButton(nFlags, point);   
	CDialog::OnLButtonUp(nFlags, point);
}

void CEditBGSlots::OnOK() 
{
  UpdateData(TRUE);
  currBkGnd = m_Slot;
	CDialog::OnOK();
}

void CEditBGSlots::OnDeleteslot() 
{
   BOOL erase = FALSE;

   if (m_Slot <= 0)
      return;

   if (CheckLevelForBackgroundSlot(m_Slot))
   {
      if (MsgBoxYesNo("This slot is used by the current level.\nDelete slot anyway?",
                       "Slot in use warning") == IDYES)
         erase = TRUE;
   }
   else
   {
      if (MsgBoxYesNo("Delete background slot?",
                       "Slot Deletion Warning") == IDYES)
         erase = TRUE;      
   }
      
   if (erase)
   {
      EmptySlot(m_Slot);
      BackgroundSets[m_Slot].Clear();
   }
}

void CEditBGSlots::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   HandleLeftButton(nFlags, point); 
	CDialog::OnLButtonDblClk(nFlags, point);
   OnChangeBG();
}

void CEditBGSlots::LoadSlotTab()
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
  UpdateData(FALSE);
}

void CEditBGSlots::OnSelchangeSlottab(NMHDR* pNMHDR, LRESULT* pResult) 
{
  LoadSlotTab();
	*pResult = 0;
}

void CEditBGSlots::OnBGSlotsQuestion() 
{
  CHelp dlg("BACKGROUND_SLOTS");
  dlg.DoModal();
}
