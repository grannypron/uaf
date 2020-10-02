// GameResChange.cpp : implementation file
//

#include "..\Shared\stdafx.h"
#include "resource.h"
#include "externs.h"
#include "uafwined.h"
#include "GameResChange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameResChange dialog


CGameResChange::CGameResChange(CWnd* pParent /*=NULL*/)
	: CDialog(CGameResChange::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGameResChange)
	m_640Config = _T("");
	m_800Config = _T("");
	m_1024Config = _T("");
	m_ChosenRes = 0;
	m_ResText = _T("");
	m_InstallText = _T("");
	m_DesignPath = _T("");
	m_UpdateDesign = FALSE;
	//}}AFX_DATA_INIT
}


void CGameResChange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameResChange)
	DDX_Text(pDX, IDC_640CONFIG, m_640Config);
	DDX_Text(pDX, IDC_800CONFIG, m_800Config);
	DDX_Text(pDX, IDC_1024CONFIG, m_1024Config);
	DDX_Radio(pDX, IDC_RES640, m_ChosenRes);
	DDX_Text(pDX, IDC_RESTEXT, m_ResText);
	DDX_Text(pDX, IDC_INSTALLTEXT, m_InstallText);
	DDX_Text(pDX, IDC_DESIGNPATH, m_DesignPath);
	DDX_Check(pDX, IDC_UPDATEDESIGNCONFIG, m_UpdateDesign);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGameResChange, CDialog)
	//{{AFX_MSG_MAP(CGameResChange)
	ON_BN_CLICKED(ID_UPDATEGAMERES, OnUpdategameres)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameResChange message handlers


void CGameResChange::OnUpdategameres() 
{
  UpdateData(TRUE);
  screenRes oldRes = m_GameScreenRes;
	m_GameScreenRes=(screenRes)m_ChosenRes;
  
  CString srcFile("");
  CString bak("");

  //enum screenRes { res640x480=0, res800x600=1, res1024x768=2, res1152x864=3, res1280x1024=4 };
  switch (m_GameScreenRes)
  {
  case res640x480:  srcFile=m_640Config;  break;
  case res800x600:  srcFile=m_800Config;  break;
  case res1024x768: srcFile=m_1024Config; break;
  }

  switch (oldRes)
  {
  case res640x480:  bak="640"; break;
  case res800x600:  bak="800"; break;
  case res1024x768: bak="1024";break;
  }

  CString msg;
  m_InstallText = ede.TemplateDir() + "Data\\config.txt";
  m_DesignPath =  rte.DataDir() + "config.txt";
  if (backup(m_InstallText, bak) != 0)
  {
    CDialog::OnOK();
    return;
  };
  if (backup(m_DesignPath, bak) != 0)
  {
    CDialog::OnOK();
    return;
  };
  if (CopyFile(srcFile, m_InstallText, false))
  {
    msg.Format("The update was successful.\n\n"
               "\'%s\' has been copied to \'%s\'\n\n"
               "The original \'%sconfig.txt\' has been renamed to \'%sconfig.%s\'\n\n"
               "You will need to restart the editor before you can use the new settings.",
               srcFile, m_InstallText,m_InstallText,m_InstallText, bak);   
    
    AfxMessageBox(msg, MB_ICONINFORMATION);
  }
  else
  {
    // put the orig setting back in the registry
    m_GameScreenRes=oldRes;          
    msg.Format("Failed to copy \'%s\' file to \'%s\'!", srcFile, m_InstallText);
    AfxMessageBox(msg,MB_ICONERROR);
  }
  
  if (m_UpdateDesign)
  {
    if (!CopyFile(srcFile, m_DesignPath, false))
    {
      // put the orig setting back in the registry
      msg.Format("Failed to update design file\nCould not copy file \'%s\' to \'%s\'!", srcFile, m_DesignPath);
      AfxMessageBox(msg,MB_ICONERROR);
    }
  }

  m_ChosenRes = m_GameScreenRes;
  switch (m_ChosenRes)
  {
  case res640x480:  m_ResText="640x480";  break;
  case res800x600:  m_ResText="800x600";  break;
  case res1024x768: m_ResText="1024x768"; break;
  }

  UpdateData(FALSE);
  CDialog::OnOK();
}

BOOL CGameResChange::OnInitDialog() 
{
	CDialog::OnInitDialog();

  // Win95 does not have this function!

  m_ChosenRes = m_GameScreenRes;

  switch (m_GameScreenRes)
  {
  case res640x480:  m_ResText="640x480";  break;
  case res800x600:  m_ResText="800x600";  break;
  case res1024x768: m_ResText="1024x768"; break;
  }
  m_640Config.Format("%sData\\config640.txt", ede.TemplateDir());
  m_800Config.Format("%sData\\config800.txt", ede.TemplateDir());
  m_1024Config.Format("%sData\\config1024.txt", ede.TemplateDir());
  BOOL ex640 = FileExists(m_640Config);
  BOOL ex800 = FileExists(m_800Config);
  BOOL ex1024 = FileExists(m_1024Config);

  if ( !ex640 || !ex800 || !ex1024 )
  {
    CString msg="The following config.txt files don't exist!\n\n";
    if (!ex640)  { msg += m_640Config; msg += "\n"; }
    if (!ex800)  { msg += m_800Config; msg += "\n"; }
    if (!ex1024) { msg += m_1024Config; msg += "\n\n"; }
    msg += "You cannot successfully use these higher resolution settings";
    AfxMessageBox(msg, MB_ICONERROR);
  }

//#ifndef _DEBUG

  int count=0;
  CWnd *pWnd;
  if (!ex640)
  {
    pWnd=GetDlgItem(IDC_RES640);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
  }
  else
  {
    count++;
    SetFileAttributes(m_640Config, FILE_ATTRIBUTE_NORMAL);
  }

  if (!ex800)
  {
    pWnd=GetDlgItem(IDC_RES800);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
  }
  else
  {
    count++;
    SetFileAttributes(m_800Config, FILE_ATTRIBUTE_NORMAL);
  }

  if (!ex1024)
  {
    pWnd=GetDlgItem(IDC_RES1024);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
  }
  else
  {
    count++;
    SetFileAttributes(m_1024Config, FILE_ATTRIBUTE_NORMAL);
  }

  if (count < 2)
  {
    pWnd=GetDlgItem(ID_UPDATEGAMERES);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
  }
//#endif // _DEBUG

  UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGameResChange::OnOK() 
{

	CDialog::OnOK();
}
