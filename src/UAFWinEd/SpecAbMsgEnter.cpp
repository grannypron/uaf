// SpecAbMsgEnter.cpp : implementation file
//

#include "..\Shared\stdafx.h"
#include "resource.h"
#include "uafwined.h"
#include "SpecAbMsgEnter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpecAbMsgEnter dialog


CSpecAbMsgEnter::CSpecAbMsgEnter(CString msg, CWnd* pParent /*=NULL*/)
	: CDialog(CSpecAbMsgEnter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecAbMsgEnter)
	m_Msg = _T("");
	//}}AFX_DATA_INIT
  m_Msg=msg;
}


void CSpecAbMsgEnter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecAbMsgEnter)
	DDX_Text(pDX, IDC_SPECABMSG, m_Msg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpecAbMsgEnter, CDialog)
	//{{AFX_MSG_MAP(CSpecAbMsgEnter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecAbMsgEnter message handlers

BOOL CSpecAbMsgEnter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
