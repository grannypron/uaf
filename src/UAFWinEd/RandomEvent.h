#if !defined(AFX_RANDOMEVENT_H__3D287462_B508_4022_881E_334A8735D452__INCLUDED_)
#define AFX_RANDOMEVENT_H__3D287462_B508_4022_881E_334A8735D452__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RandomEvent.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRandomEvent dialog

class CRandomEvent : public CDialog
{
// Construction
public:
	CRandomEvent(RANDOM_EVENT_DATA &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(RANDOM_EVENT_DATA &data) { data = m_data; }

// Dialog Data
	//{{AFX_DATA(CRandomEvent)
	enum { IDD = IDD_RANDOMEVENT };
	CButton	m_SeeArt;
	CString	m_Text;
	BYTE	m_Chance1;
	BYTE	m_Chance10;
	BYTE	m_Chance11;
	BYTE	m_Chance12;
	BYTE	m_Chance13;
	BYTE	m_Chance2;
	BYTE	m_Chance3;
	BYTE	m_Chance4;
	BYTE	m_Chance5;
	BYTE	m_Chance6;
	BYTE	m_Chance7;
	BYTE	m_Chance8;
	BYTE	m_Chance9;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRandomEvent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  RANDOM_EVENT_DATA m_data;
	// Generated message map functions
	//{{AFX_MSG(CRandomEvent)
	virtual BOOL OnInitDialog();
	afx_msg void OnSeeart();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RANDOMEVENT_H__3D287462_B508_4022_881E_334A8735D452__INCLUDED_)
