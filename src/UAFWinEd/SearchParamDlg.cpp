// SearchParamDlg.cpp : implementation file
//

#include "..\Shared\stdafx.h"
#include "uafwined.h"
#include "Mainfrm.h"
#include "SearchParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CSearchParamDlg *pSearchinghDlg = NULL;

CSearchParamDlg *GetSearchDlg(WORD flags)
{
  if (pSearchinghDlg == NULL)
  {
    pSearchinghDlg = new CSearchParamDlg(AfxGetMainWnd());
    pSearchinghDlg->SetModelessFlag(true);
    
    if (flags & ENABLE_EVENT_SRCH_FLAGS)
      pSearchinghDlg->m_ShowEventFlags=true;

    pSearchinghDlg->Create(IDD_SEARCHPARAMS);
  }
  return pSearchinghDlg;
}

void DestroySearchDlg()
{
  if (pSearchinghDlg != NULL)
  {    
    if (::IsWindow(pSearchinghDlg->GetSafeHwnd()))
    {
      pSearchinghDlg->ShowWindow(SW_HIDE);
      pSearchinghDlg->EndDialog(IDCANCEL);      
    }
    delete pSearchinghDlg; 
    pSearchinghDlg = NULL;
  }
}

/////////////////////////////////////////////////////////////////////////////
// CSearchParamDlg dialog


CSearchParamDlg::CSearchParamDlg(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CSearchParamDlg::IDD, pParent), m_pSearchFunc(NULL)
{
	//{{AFX_DATA_INIT(CSearchParamDlg)
	m_SearchText = _T("");
	m_SearchEventText = FALSE;
	m_MatchWholeString = FALSE;
	m_MatchCase = FALSE;
	m_SearchEventId = FALSE;
	//}}AFX_DATA_INIT

  m_ShowEventFlags=false;
}


void CSearchParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchParamDlg)
	DDX_Text(pDX, IDC_SEARCHTEXT, m_SearchText);
	DDX_Check(pDX, IDC_SRCHEVENTTEXT, m_SearchEventText);
	DDX_Check(pDX, IDC_MATCHWHOLESTRING, m_MatchWholeString);
	DDX_Check(pDX, IDC_MATCHCASE, m_MatchCase);
	DDX_Check(pDX, IDC_SEARCHEVENTID, m_SearchEventId);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchParamDlg, CBaseDialog)
	//{{AFX_MSG_MAP(CSearchParamDlg)
	ON_BN_CLICKED(IDC_FIND, OnFind)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchParamDlg message handlers

void CSearchParamDlg::OnFind() 
{
  if (m_pSearchFunc == NULL)
    return;

	UpdateData(TRUE);

  WORD type=0;

  if (m_MatchWholeString)
    type |= SEARCH_MATCH_ENTIRE;
  if (m_MatchCase)
    type |= SEARCH_MATCH_CASE;
  if (m_SearchEventText)
    type |= SEARCH_MATCH_EVENT_TEXT;
  if (m_SearchEventId)
    type |= SEARCH_MATCH_EVENT_ID;

  m_pSearchFunc(m_SearchText, type, m_UserData);
}

void CSearchParamDlg::OnOK() 
{
	//CBaseDialog::OnOK();
  ::PostMessage(AfxGetMainWnd()->m_hWnd,
               (UINT)MSG_CLOSESEARCHDLG, 
               (WPARAM)0, 
               (WPARAM)0);  
}
       
void CSearchParamDlg::SetSearchCallback(SEARCHPROC pFunc, DWORD UserData)
{ 
  m_pSearchFunc = pFunc; 
  m_UserData = UserData;
}

void CSearchParamDlg::ClearSearchCallback()
{ 
  m_pSearchFunc=NULL; 
  m_UserData=0;
}

BOOL CSearchParamDlg::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();

  CWnd *pWnd;
  if (!m_ShowEventFlags)
  {
    m_SearchEventText = FALSE;
    m_SearchEventId = FALSE;

    pWnd = GetDlgItem(IDC_SRCHEVENTTEXT);

    if (pWnd!=NULL)
    {
      pWnd->EnableWindow(FALSE);
      pWnd->ShowWindow(SW_HIDE);
    }

    pWnd = GetDlgItem(IDC_SEARCHEVENTID);

    if (pWnd!=NULL)
    {
      pWnd->EnableWindow(FALSE);
      pWnd->ShowWindow(SW_HIDE);
    }
  }
  else
  {
    m_SearchEventText = TRUE;
    m_SearchEventId = TRUE;
  }

  UpdateData(FALSE);

  GotoDlgCtrl(GetDlgItem(IDC_SEARCHTEXT));
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
