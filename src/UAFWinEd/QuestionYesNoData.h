/******************************************************************************
* Filename: QuestionYesNoData.h
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
#if !defined(AFX_QUESTIONYESNODATA_H__8F4640C1_586A_11D3_BF9A_00A0CC25685D__INCLUDED_)
#define AFX_QUESTIONYESNODATA_H__8F4640C1_586A_11D3_BF9A_00A0CC25685D__INCLUDED_



/////////////////////////////////////////////////////////////////////////////
// CQuestionYesNoData dialog

class CQuestionYesNoData : public CDialog
{
// Construction
public:
	CQuestionYesNoData(QUESTION_YES_NO& event, CWnd* pParent = NULL);   // standard constructor
  void GetData(QUESTION_YES_NO& event);

// Dialog Data
	//{{AFX_DATA(CQuestionYesNoData)
	enum { IDD = IDD_QYESNO };
	CComboBox	m_YesChainOption;
	CComboBox	m_NoChainOption;
	CButton	m_SeeArt;
	CString	m_NoText;
	CString	m_Text;
	CString	m_YesText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuestionYesNoData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  QUESTION_YES_NO m_event;

	// Generated message map functions
	//{{AFX_MSG(CQuestionYesNoData)
	virtual BOOL OnInitDialog();
	afx_msg void OnChooseart();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUESTIONYESNODATA_H__8F4640C1_586A_11D3_BF9A_00A0CC25685D__INCLUDED_)
