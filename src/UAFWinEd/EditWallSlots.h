/******************************************************************************
* Filename: EditWallSlots.h
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
#ifndef __EDITWALLSLOT_H__
#define __EDITWALLSLOT_H__

#include "ColorBox.h"
#include "DlgPicture.h"

/////////////////////////////////////////////////////////////////////////////
// CEditWallSlots dialog

class CEditWallSlots : public CDialog
{
// Construction
public:
	CEditWallSlots(CWnd* pParent = NULL);   // standard constructor
   int m_Slot;

// Dialog Data
	//{{AFX_DATA(CEditWallSlots)
	enum { IDD = IDD_WALLSETPIC };
	CButton	m_AVArtSelect;
	CTabCtrl	m_SlotTab;
	CButton	m_UnlockSpell;
  CButton m_UnlockSpellHelp;
	CButton	m_ChangeWall;
	CButton	m_ChangeOverlay;
	CButton	m_ChangeDoor;
	CColorBox	m_ObColor9;
	CColorBox	m_ObColor8;
	CColorBox	m_ObColor7;
	CColorBox	m_ObColor6;
	CColorBox	m_ObColor5;
	CColorBox	m_ObColor4;
	CColorBox	m_ObColor3;
	CColorBox	m_ObColor2;
	CColorBox	m_ObColor16;
	CColorBox	m_ObColor15;
	CColorBox	m_ObColor14;
	CColorBox	m_ObColor13;
	CColorBox	m_ObColor12;
	CColorBox	m_ObColor11;
	CColorBox	m_ObColor10;
	CColorBox	m_ObColor1;
	CColorBox	m_Color9;
	CColorBox	m_Color8;
	CColorBox	m_Color7;
	CColorBox	m_Color6;
	CColorBox	m_Color5;
	CColorBox	m_Color4;
	CColorBox	m_Color3;
	CColorBox	m_Color2;
	CColorBox	m_Color16;
	CColorBox	m_Color15;
	CColorBox	m_Color14;
	CColorBox	m_Color13;
	CColorBox	m_Color12;
	CColorBox	m_Color11;
	CColorBox	m_Color10;
	CColorBox	m_Color1;
	CDlgWallPicture	m_Slot9;
	CDlgWallPicture	m_Slot8;
	CDlgWallPicture	m_Slot7;
	CDlgWallPicture	m_Slot6;
	CDlgWallPicture	m_Slot5;
	CDlgWallPicture	m_Slot4;
	CDlgWallPicture	m_Slot3;
	CDlgWallPicture	m_Slot2;
	CDlgWallPicture	m_Slot16;
	CDlgWallPicture	m_Slot15;
	CDlgWallPicture	m_Slot14;
	CDlgWallPicture	m_Slot13;
	CDlgWallPicture	m_Slot12;
	CDlgWallPicture	m_Slot11;
	CDlgWallPicture	m_Slot10;
	CDlgWallPicture	m_Slot1;
	int		m_Obstruction;
	int		m_ImageFormat;
	int		m_BlendAmount;
	BOOL	m_UseBlend;
	BOOL	m_DoorFirst;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditWallSlots)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void ChangeSlotSelections(int oldSlot, int newSlot);
  void LoadSlot(int index);
  void InvalidateSlot(int index);
  void EmptySlot(int index);
  void HandleLeftButton(UINT nFlags, CPoint point);
  CDlgWallPicture& GetSlotPic(int index);
  CColorBox& GetSlotColor(int index);
  CColorBox& GetSlotObColor(int index);
  void LoadSlotTab();

	// Generated message map functions
	//{{AFX_MSG(CEditWallSlots)
	afx_msg void OnChangedoor();
	afx_msg void OnChangeoverlay();
	afx_msg void OnChangewall();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnOK();
	afx_msg void OnDeleteslot();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnUnlockspell();
  afx_msg void OnUnlockSpellHelp();
	afx_msg void OnSelchangeSlottab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAvart();
	afx_msg void OnUnlockkeys();
  afx_msg void OnWallSlotsQuestion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
