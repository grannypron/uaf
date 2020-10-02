/******************************************************************************
* Filename: TransferEvent.h
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
#if !defined(AFX_TRANSFEREVENT_H__C0BD9A72_5C56_11D2_AC11_00C04F8E081C__INCLUDED_)
#define AFX_TRANSFEREVENT_H__C0BD9A72_5C56_11D2_AC11_00C04F8E081C__INCLUDED_


/////////////////////////////////////////////////////////////////////////////
// CTransferEvent dialog

class CTransferEvent : public CDialog
{
// Construction
public:
	CTransferEvent(TRANSFER_EVENT_DATA &data, BOOL allowLevel, CWnd* pParent = NULL);   // standard constructor
   void GetData(TRANSFER_EVENT_DATA &data);

// Dialog Data
	//{{AFX_DATA(CTransferEvent)
	enum { IDD = IDD_TRANSFERDLG };
	CButton	m_ChooseArt;
	BOOL	m_AskYesNo;
	CString	m_QuestionText;
	int		m_YesTransfer;
	BOOL	m_DestroyDrow;
	CString	m_TransferText;
	int		m_AttemptsEntry;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransferEvent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   BOOL m_AllowLevel;
   TRANSFER_EVENT_DATA m_data;
   void FormatEPList(int destEP, int destLevel);


	// Generated message map functions
	//{{AFX_MSG(CTransferEvent)
	afx_msg void OnChoosespic();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAskyesnoq();
	afx_msg void OnTeleport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSFEREVENT_H__C0BD9A72_5C56_11D2_AC11_00C04F8E081C__INCLUDED_)
