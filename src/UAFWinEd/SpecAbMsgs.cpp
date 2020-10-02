// SpecAbMsgs.cpp : implementation file
//

#include "..\Shared\stdafx.h"
#include "resource.h"

#include "UAFWinEd.h"
#include "GlobText.h"
#include "SpecAbMsgEnter.h"

#include "SpecAbMsgs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpecAbMsgs dialog


CSpecAbMsgs::CSpecAbMsgs(const SPECIAL_ABILITIES& list, specialAbilitiesType sa, CWnd* pParent /*=NULL*/)
	: m_list(false), CDialog(CSpecAbMsgs::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecAbMsgs)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_list.Copy(list);
  m_sa = sa;
}


void CSpecAbMsgs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecAbMsgs)
	DDX_Control(pDX, IDC_SPECABMSGLIST, m_SpecAbMsgList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpecAbMsgs, CDialog)
	//{{AFX_MSG_MAP(CSpecAbMsgs)
	ON_BN_CLICKED(IDC_EDITMSG, OnEditmsg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecAbMsgs message handlers

void CSpecAbMsgs::OnOK() 
{
  DWORD MsgTypes = 0;

  for (int i=0;i<m_SpecAbMsgList.GetItemCount();i++)
  {
    int satype = m_SpecAbMsgList.GetItemData(i);
    BOOL enable = m_SpecAbMsgList.GetCheck(i);
    if (enable)
    {
      CString msg = m_SpecAbMsgList.GetItemText(i,1);
      m_list.SetMsgFormat(m_sa, satype, msg);
      MsgTypes |= satype;
    }
  }
  m_list.SetMsgTypes(m_sa, MsgTypes);
  
	CDialog::OnOK();
}

BOOL CSpecAbMsgs::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_SpecAbMsgList.SetExtendedStyle(LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT);
	
	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Msg Type";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
	m_SpecAbMsgList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Msg Format";
	lvc.cx = 320;
	lvc.fmt = LVCFMT_LEFT;
  m_SpecAbMsgList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

  m_SpecAbMsgList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  DWORD types = m_list.GetMsgTypes(m_sa);
  for (int t=1;t<NUM_SPECAB_MSGS;t++)
  {
    DWORD flag = 1<<t;
    int saindex = ConvertSpecAbFlagToIndex(flag);
    if (saindex > 0)
    {
      int listIdx = m_SpecAbMsgList.GetItemCount();

      LVITEM item;
	    item.mask = LVIF_TEXT | LVIF_PARAM;
	    item.iItem = listIdx;
	    item.iSubItem = 0;
	    item.state = 0; 
	    item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	    item.pszText = const_cast<char *>(SpecAbMsgText[saindex]);
	    item.cchTextMax = 0;
	    item.iImage = 0;
	    item.lParam = flag; 

      m_SpecAbMsgList.InsertItem(&item);
      m_SpecAbMsgList.SetItemText(listIdx, 1, m_list.GetMsgFormat(m_sa, flag));    
      m_SpecAbMsgList.SetCheck(listIdx, (types&flag)?TRUE:FALSE);
    }
  }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpecAbMsgs::OnEditmsg() 
{
  POSITION pos = m_SpecAbMsgList.GetFirstSelectedItemPosition();
  
  if (pos != NULL)
  {
    int nItem = m_SpecAbMsgList.GetNextSelectedItem(pos);
    DWORD flag = m_SpecAbMsgList.GetItemData(nItem);
    CString msg = m_list.GetMsgFormat(m_sa, flag);
    CSpecAbMsgEnter dlg(msg);	
    if (dlg.DoModal()==IDOK)
    {
      m_list.SetMsgFormat(m_sa, flag, dlg.m_Msg);
      m_SpecAbMsgList.SetItemText(nItem, 1, dlg.m_Msg);
    }
  }
}
