/******************************************************************************
* Filename: EditBGSlots.h
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
#ifndef __EditBGSlot_H__
#define __EditBGSlot_H__

#include "ColorBox.h"
#include "DlgPicture.h"

/////////////////////////////////////////////////////////////////////////////
// CEditBGSlots dialog

class CEditBGSlots : public CDialog
{
// Construction
public:
	CEditBGSlots(CWnd* pParent = NULL);   // standard constructor
   int m_Slot;

// Dialog Data
	//{{AFX_DATA(CEditBGSlots)
	enum { IDD = IDD_BGSETPIC };
  CTabCtrl	m_SlotTab;
	CButton	m_ChangeBG;
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
	CDlgPicture	m_Slot9;
	CDlgPicture	m_Slot8;
	CDlgPicture	m_Slot7;
	CDlgPicture	m_Slot6;
	CDlgPicture	m_Slot5;
	CDlgPicture	m_Slot4;
	CDlgPicture	m_Slot3;
	CDlgPicture	m_Slot2;
	CDlgPicture	m_Slot16;
	CDlgPicture	m_Slot15;
	CDlgPicture	m_Slot14;
	CDlgPicture	m_Slot13;
	CDlgPicture	m_Slot12;
	CDlgPicture	m_Slot11;
	CDlgPicture	m_Slot10;
	CDlgPicture	m_Slot1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditBGSlots)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void ChangeSlotSelections(int oldSlot, int newSlot);
  void LoadSlot(int index);
  void EmptySlot(int index);
  void HandleLeftButton(UINT nFlags, CPoint point);
  CDlgPicture& GetSlotPic(int index);
  CColorBox& GetSlotColor(int index);
  void LoadSlotTab();

	// Generated message map functions
	//{{AFX_MSG(CEditBGSlots)
	afx_msg void OnChangeBG();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnOK();
	afx_msg void OnDeleteslot();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeSlottab(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnBGSlotsQuestion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
