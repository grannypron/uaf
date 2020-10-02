// SpecialKeyBlockage.cpp : implementation file
//

#include "..\Shared\stdafx.h"
#include "resource.h"

#include "UAFWinEd.h"
#include "GlobalData.h"
#include "ItemKeyQuestDlg.h"
#include "SpecialKeyBlockage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpecialKeyBlockage dialog


CSpecialKeyBlockage::CSpecialKeyBlockage(BLOCKAGE_KEYS &data, CWnd* pParent /*=NULL*/)
	: CDialog(CSpecialKeyBlockage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecialKeyBlockage)
	m_KeyName1 = _T("");
	m_KeyName2 = _T("");
	m_KeyName3 = _T("");
	m_KeyName4 = _T("");
	m_KeyName5 = _T("");
	m_KeyName6 = _T("");
	m_KeyName7 = _T("");
	m_KeyName8 = _T("");
	//}}AFX_DATA_INIT
  m_data = data;
}


void CSpecialKeyBlockage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecialKeyBlockage)
	DDX_Text(pDX, IDC_KEYNAME1, m_KeyName1);
	DDX_Text(pDX, IDC_KEYNAME2, m_KeyName2);
	DDX_Text(pDX, IDC_KEYNAME3, m_KeyName3);
	DDX_Text(pDX, IDC_KEYNAME4, m_KeyName4);
	DDX_Text(pDX, IDC_KEYNAME5, m_KeyName5);
	DDX_Text(pDX, IDC_KEYNAME6, m_KeyName6);
	DDX_Text(pDX, IDC_KEYNAME7, m_KeyName7);
	DDX_Text(pDX, IDC_KEYNAME8, m_KeyName8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpecialKeyBlockage, CDialog)
	//{{AFX_MSG_MAP(CSpecialKeyBlockage)
	ON_BN_CLICKED(IDC_PICKKEY1, OnPickkey1)
	ON_BN_CLICKED(IDC_PICKKEY2, OnPickkey2)
	ON_BN_CLICKED(IDC_PICKKEY3, OnPickkey3)
	ON_BN_CLICKED(IDC_PICKKEY4, OnPickkey4)
	ON_BN_CLICKED(IDC_PICKKEY5, OnPickkey5)
	ON_BN_CLICKED(IDC_PICKKEY6, OnPickkey6)
	ON_BN_CLICKED(IDC_PICKKEY7, OnPickkey7)
	ON_BN_CLICKED(IDC_PICKKEY8, OnPickkey8)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecialKeyBlockage message handlers

void CSpecialKeyBlockage::OnPickkey1() 
{
  CItemKeyQuestDlg dlg(KEY_FLAG);
	
  if (dlg.DoModal() == IDOK)
  {
    if (dlg.m_selectedID >= 0)
      m_data.SpecialKeys[0] = dlg.m_selectedID;
    else
      m_data.SpecialKeys[0] = 0;

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetSpecialKeyName(m_data.SpecialKeys[0]), name, MAX_MEDITBUTTON_TEXT);   
    m_KeyName1 = name;
    UpdateData(FALSE);
  }	  
}

void CSpecialKeyBlockage::OnPickkey2() 
{
  CItemKeyQuestDlg dlg(KEY_FLAG);
	
  if (dlg.DoModal() == IDOK)
  {
    if (dlg.m_selectedID >= 0)
      m_data.SpecialKeys[1] = dlg.m_selectedID;
    else
      m_data.SpecialKeys[1] = 0;

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetSpecialKeyName(m_data.SpecialKeys[1]), name, MAX_MEDITBUTTON_TEXT);   
    m_KeyName2 = name;
    UpdateData(FALSE);
  }	  	
}

void CSpecialKeyBlockage::OnPickkey3() 
{
  CItemKeyQuestDlg dlg(KEY_FLAG);
	
  if (dlg.DoModal() == IDOK)
  {
    if (dlg.m_selectedID >= 0)
      m_data.SpecialKeys[2] = dlg.m_selectedID;
    else
      m_data.SpecialKeys[2] = 0;

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetSpecialKeyName(m_data.SpecialKeys[2]), name, MAX_MEDITBUTTON_TEXT);   
    m_KeyName3 = name;
    UpdateData(FALSE);
  }	  
}

void CSpecialKeyBlockage::OnPickkey4() 
{
  CItemKeyQuestDlg dlg(KEY_FLAG);
	
  if (dlg.DoModal() == IDOK)
  {
    if (dlg.m_selectedID >= 0)
      m_data.SpecialKeys[3] = dlg.m_selectedID;
    else
      m_data.SpecialKeys[3] = 0;

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetSpecialKeyName(m_data.SpecialKeys[3]), name, MAX_MEDITBUTTON_TEXT);   
    m_KeyName4 = name;
    UpdateData(FALSE);
  }	  	
}

void CSpecialKeyBlockage::OnPickkey5() 
{
  CItemKeyQuestDlg dlg(KEY_FLAG);
	
  if (dlg.DoModal() == IDOK)
  {
    if (dlg.m_selectedID >= 0)
      m_data.SpecialKeys[4] = dlg.m_selectedID;
    else
      m_data.SpecialKeys[4] = 0;

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetSpecialKeyName(m_data.SpecialKeys[4]), name, MAX_MEDITBUTTON_TEXT);   
    m_KeyName5 = name;
    UpdateData(FALSE);
  }	  	
}

void CSpecialKeyBlockage::OnPickkey6() 
{
  CItemKeyQuestDlg dlg(KEY_FLAG);
	
  if (dlg.DoModal() == IDOK)
  {
    if (dlg.m_selectedID >= 0)
      m_data.SpecialKeys[5] = dlg.m_selectedID;
    else
      m_data.SpecialKeys[5] = 0;

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetSpecialKeyName(m_data.SpecialKeys[5]), name, MAX_MEDITBUTTON_TEXT);   
    m_KeyName6 = name;
    UpdateData(FALSE);
  }	  	
}

void CSpecialKeyBlockage::OnPickkey7() 
{
  CItemKeyQuestDlg dlg(KEY_FLAG);
	
  if (dlg.DoModal() == IDOK)
  {
    if (dlg.m_selectedID >= 0)
      m_data.SpecialKeys[6] = dlg.m_selectedID;
    else
      m_data.SpecialKeys[6] = 0;

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetSpecialKeyName(m_data.SpecialKeys[6]), name, MAX_MEDITBUTTON_TEXT);   
    m_KeyName7 = name;
    UpdateData(FALSE);
  }	  	
}

void CSpecialKeyBlockage::OnPickkey8() 
{
  CItemKeyQuestDlg dlg(KEY_FLAG);
	
  if (dlg.DoModal() == IDOK)
  {
    if (dlg.m_selectedID >= 0)
      m_data.SpecialKeys[7] = dlg.m_selectedID;
    else
      m_data.SpecialKeys[7] = 0;

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetSpecialKeyName(m_data.SpecialKeys[7]), name, MAX_MEDITBUTTON_TEXT);   
    m_KeyName8 = name;
    UpdateData(FALSE);
  }	  
}

BOOL CSpecialKeyBlockage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(GetSpecialKeyName(m_data.SpecialKeys[0]), name, MAX_MEDITBUTTON_TEXT);   
  m_KeyName1 = name;
  getBaseName(GetSpecialKeyName(m_data.SpecialKeys[1]), name, MAX_MEDITBUTTON_TEXT);   
  m_KeyName2 = name;
  getBaseName(GetSpecialKeyName(m_data.SpecialKeys[2]), name, MAX_MEDITBUTTON_TEXT);   
  m_KeyName3 = name;
  getBaseName(GetSpecialKeyName(m_data.SpecialKeys[3]), name, MAX_MEDITBUTTON_TEXT);   
  m_KeyName4 = name;
  getBaseName(GetSpecialKeyName(m_data.SpecialKeys[4]), name, MAX_MEDITBUTTON_TEXT);   
  m_KeyName5 = name;
  getBaseName(GetSpecialKeyName(m_data.SpecialKeys[5]), name, MAX_MEDITBUTTON_TEXT);   
  m_KeyName6 = name;
  getBaseName(GetSpecialKeyName(m_data.SpecialKeys[6]), name, MAX_MEDITBUTTON_TEXT);   
  m_KeyName7 = name;
  getBaseName(GetSpecialKeyName(m_data.SpecialKeys[7]), name, MAX_MEDITBUTTON_TEXT);   
  m_KeyName8 = name;

  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
