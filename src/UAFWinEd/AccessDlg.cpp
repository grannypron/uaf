/******************************************************************************
* Filename: AccessDlg.cpp
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
// AccessDlg.cpp : implementation file
//
#include "..\Shared\stdafx.h"
#include "..\Shared\Externs.h"
#include "resource.h"

//#include "UAFWinEd.h"
//#include "level.h"
#include "AccessDlg.h"
//#include "GlobalData.h"

void *My_malloc(int size);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


LEVEL_ACCESS levelAccess;

static CString defaultParameters =
    "1100100000000000"  // Reds
    "1100011100000000"  // Greens
    "1100000011111111"; // Blues

/////////////////////////////////////////////////////////////////////////////
// CAccessDlg dialog


CAccessDlg::CAccessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAccessDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAccessDlg)
	//}}AFX_DATA_INIT
  // To avoid Class Wizard crash.
  for (int i=0; i<48; i++) m_Check[i]=FALSE;
  m_accessParameters=defaultParameters;
}



void CAccessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAccessDlg)
	DDX_Control(pDX, IDC_Red, m_Red);
	DDX_Control(pDX, IDC_Green, m_Green);
	DDX_Control(pDX, IDC_Blue, m_Blue);
	//}}AFX_DATA_MAP
  // To avoid the Class Wizard crash.
	DDX_Check(pDX, IDC_CHECK1, m_Check[0]);
	DDX_Check(pDX, IDC_CHECK2, m_Check[1]);
	DDX_Check(pDX, IDC_CHECK3, m_Check[2]);
	DDX_Check(pDX, IDC_CHECK4, m_Check[3]);
	DDX_Check(pDX, IDC_CHECK5, m_Check[4]);
	DDX_Check(pDX, IDC_CHECK6, m_Check[5]);
	DDX_Check(pDX, IDC_CHECK7, m_Check[6]);
	DDX_Check(pDX, IDC_CHECK8, m_Check[7]);
	DDX_Check(pDX, IDC_CHECK9, m_Check[8]);
	DDX_Check(pDX, IDC_CHECK10, m_Check[9]);
	DDX_Check(pDX, IDC_CHECK11, m_Check[10]);
	DDX_Check(pDX, IDC_CHECK12, m_Check[11]);
	DDX_Check(pDX, IDC_CHECK13, m_Check[12]);
	DDX_Check(pDX, IDC_CHECK14, m_Check[13]);
	DDX_Check(pDX, IDC_CHECK15, m_Check[14]);
	DDX_Check(pDX, IDC_CHECK16, m_Check[15]);
	DDX_Check(pDX, IDC_CHECK17, m_Check[16]);
	DDX_Check(pDX, IDC_CHECK18, m_Check[17]);
	DDX_Check(pDX, IDC_CHECK19, m_Check[18]);
	DDX_Check(pDX, IDC_CHECK20, m_Check[19]);
	DDX_Check(pDX, IDC_CHECK21, m_Check[20]);
	DDX_Check(pDX, IDC_CHECK22, m_Check[21]);
	DDX_Check(pDX, IDC_CHECK23, m_Check[22]);
	DDX_Check(pDX, IDC_CHECK24, m_Check[23]);
	DDX_Check(pDX, IDC_CHECK25, m_Check[24]);
	DDX_Check(pDX, IDC_CHECK26, m_Check[25]);
	DDX_Check(pDX, IDC_CHECK27, m_Check[26]);
	DDX_Check(pDX, IDC_CHECK28, m_Check[27]);
	DDX_Check(pDX, IDC_CHECK29, m_Check[28]);
	DDX_Check(pDX, IDC_CHECK30, m_Check[29]);
	DDX_Check(pDX, IDC_CHECK31, m_Check[30]);
	DDX_Check(pDX, IDC_CHECK32, m_Check[31]);
	DDX_Check(pDX, IDC_CHECK33, m_Check[32]);
	DDX_Check(pDX, IDC_CHECK34, m_Check[33]);
	DDX_Check(pDX, IDC_CHECK35, m_Check[34]);
	DDX_Check(pDX, IDC_CHECK36, m_Check[35]);
	DDX_Check(pDX, IDC_CHECK37, m_Check[36]);
	DDX_Check(pDX, IDC_CHECK38, m_Check[37]);
	DDX_Check(pDX, IDC_CHECK39, m_Check[38]);
	DDX_Check(pDX, IDC_CHECK40, m_Check[39]);
	DDX_Check(pDX, IDC_CHECK41, m_Check[40]);
	DDX_Check(pDX, IDC_CHECK42, m_Check[41]);
	DDX_Check(pDX, IDC_CHECK43, m_Check[42]);
	DDX_Check(pDX, IDC_CHECK44, m_Check[43]);
	DDX_Check(pDX, IDC_CHECK45, m_Check[44]);
	DDX_Check(pDX, IDC_CHECK46, m_Check[45]);
	DDX_Check(pDX, IDC_CHECK47, m_Check[46]);
	DDX_Check(pDX, IDC_CHECK48, m_Check[47]);
}


BEGIN_MESSAGE_MAP(CAccessDlg, CDialog)
	//{{AFX_MSG_MAP(CAccessDlg)
	ON_BN_CLICKED(IDC_CHECK1, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK2, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK3, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK4, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK5, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK6, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK7, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK8, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK9, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK10, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK11, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK12, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK13, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK14, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK15, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK16, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK17, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK18, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK19, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK20, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK21, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK22, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK23, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK24, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK25, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK26, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK27, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK28, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK29, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK30, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK31, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK32, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK33, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK34, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK35, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK36, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK37, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK38, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK39, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK40, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK41, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK42, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK43, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK44, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK45, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK46, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK47, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK48, OnCheckBox)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAccessDlg message handlers


BOOL CAccessDlg::OnInitDialog() 
{
  m_Red.SetColor(RGB(255,0,0));
  m_Green.SetColor(RGB(0,255,0));
  m_Blue.SetColor(RGB(0,0,255));
	
  for (int i=0; i<48; i++)
    if (m_accessParameters.GetLength()>i)
      m_Check[i]=m_accessParameters[i]!='0';
    else
      m_Check[i]=FALSE;

  // inits dialog windows
	CDialog::OnInitDialog();
  
  CWnd *pWnd;
  CString keyNames[8];
  keyNames[4]="four";
  GetKeyNames(globalData,&keyNames);
  pWnd = GetDlgItem(IDC_ACCESSKEY1);
  if (keyNames[0]!="")
    if (pWnd != NULL) pWnd->SetWindowText(keyNames[0]);
  pWnd = GetDlgItem(IDC_ACCESSKEY2);
  if (keyNames[1]!="")
    if (pWnd != NULL) pWnd->SetWindowText(keyNames[1]);
  pWnd = GetDlgItem(IDC_ACCESSKEY3);
  if (keyNames[2]!="")
    if (pWnd != NULL) pWnd->SetWindowText(keyNames[2]);
  pWnd = GetDlgItem(IDC_ACCESSKEY4);
  if (keyNames[3]!="")
    if (pWnd != NULL) pWnd->SetWindowText(keyNames[3]);
  pWnd = GetDlgItem(IDC_ACCESSKEY5);
  if (keyNames[4]!="")
    if (pWnd != NULL) pWnd->SetWindowText(keyNames[4]);
  pWnd = GetDlgItem(IDC_ACCESSKEY6);
  if (keyNames[5]!="")
    if (pWnd != NULL) pWnd->SetWindowText(keyNames[5]);
  pWnd = GetDlgItem(IDC_ACCESSKEY7);
  if (keyNames[6]!="")
    if (pWnd != NULL) pWnd->SetWindowText(keyNames[6]);
  pWnd = GetDlgItem(IDC_ACCESSKEY8);
  if (keyNames[7]!="")
    if (pWnd != NULL) pWnd->SetWindowText(keyNames[7]);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAccessDlg::OnCheckBox() 
{
	// TODO: Add your control notification handler code here
  char bools[49];
  UpdateData(true);
  for (int i=0; i<48; i++) bools[i]=m_Check[i]?'1':'0';
  bools[48]=0;
  m_accessParameters=bools;
}


void LEVEL_ACCESS::ComputeOneAccess(int sx, int sy, int mask, CString types)
{
  if ((m_stack==NULL)||(m_access==NULL)) return;
  int i;
  int x=sx, y=sy, x1, y1;
  i=0;
  int Width, Height;
  Width=GetAreaWidth(levelData);
  Height=GetAreaHeight(levelData);
  m_stack[i]=-1;
  m_access[y*Width+x]|=mask;
  while (i>=0)
  {
    m_stack[i]++;
    if (m_stack[i]>3)
    {
      i--;
      x=(x+Width-deltaX[m_stack[i]])%Width;
      y=(y+Height-deltaY[m_stack[i]])%Height;
      continue;
    };
    x1=(x+Width+deltaX[m_stack[i]])%Width;
    y1=(y+Height+deltaY[m_stack[i]])%Height;
    if (m_access[y1*Width+x1]&mask) continue;
    if (types[GetAreaBlockages(levelData,x,y,m_stack[i])]=='1')
    {
      x=x1; y=y1;
      m_access[y*Width+x]|=mask;
      i++;
      m_stack[i]=-1;
    };
  };
}

void LEVEL_ACCESS::ComputeAccess (int x, int y, const CString& aTypes)
{
  int size=GetAreaWidth(levelData)*GetAreaHeight(levelData);
  if (size==0) return;
  if ((m_access!=NULL) && (m_size < size))
  {
    My_free (m_access);
    m_access=NULL;
    m_size=0;
  };
  if (m_access==NULL)
  {
    m_access=(char *)My_malloc(size*sizeof (int));
    if (m_access==NULL) 
    {
      MsgBoxError("Cannot allocate memory for access computations");
      m_size=0;
      return;
    };
    m_size=size;
  };
//  for (int j=0; j<1000; j++)  // Caused 46-second delay on 50x50 on my 166MHz pentium.
//  {                           // 1/20-th of a second.  So I won't worry right now.
  memset(m_access,0,size);  // Clear it out.
  m_stack=(char *)My_malloc(GetAreaWidth(levelData)*GetAreaHeight(levelData));
  if (m_stack==NULL)
  {
    MsgBoxError("Cannot allocate memory for access computations");
    return;
  };
  if (m_accessMode)
    for (int i=0; i<3; i++) ComputeOneAccess(x, y, 4>>i,aTypes.Mid(i*16,16));
  My_free (m_stack); m_stack=NULL;
//  };
}

int LEVEL_ACCESS::GetAccess(int x, int y)
{
  if (!m_accessMode) return 0;
  if (m_access==NULL) return 0;
  int index=y*GetAreaWidth(levelData)+x;
  ASSERT(index<m_size);
  if (index>=m_size) return 0;
  ASSERT (m_access[index]<8);
  return m_access[index]&7;
}

void CAccessDlg::OnDefault() 
{
  m_accessParameters=defaultParameters;
  for (int i=0; i<48; i++) 
    m_Check[i] = (m_accessParameters[i]=='0'?false:true);
  UpdateData(false);
}
