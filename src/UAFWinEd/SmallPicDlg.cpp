/******************************************************************************
* Filename: SmallPicDlg.cpp
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

#include "..\Shared\stdafx.h"
#include "resource.h"

//#include "UAFWinEd.h"
//#include "SurfaceMgr.h"
//#include "PicSlot.h"
#include "PicData.h"
#include "ScrollPicDlg.h"
#include "AnimationStyle.h"
#include "SmallPicDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSmallPicDlg dialog


CSmallPicDlg::CSmallPicDlg(PIC_DATA& pic, CWnd* pParent /*=NULL*/)
	: CDialog(CSmallPicDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSmallPicDlg)
	m_TimeDelay = 0;
	m_NumFrames = 0;
	m_FrameWidth = 0;
	m_FrameHeight = 0;
	//}}AFX_DATA_INIT

  m_SmallPic = pic;
  if (m_SmallPic.picType == BogusDib)
    m_SmallPic.picType = SmallPicDib;
  if (m_SmallPic.NumFrames<=1)
    m_SmallPic.flags &= ~PIC_DATA::AF_Loop;
} 

void CSmallPicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSmallPicDlg)
	DDX_Control(pDX, IDC_SELECTFILE, m_SelectFile);
	DDX_Control(pDX, IDC_TOGGLEANIM, m_ToggleAnim);
	DDX_Control(pDX, IDC_UPDATESTATS, m_UpdateStats);
	DDX_Control(pDX, IDC_SmallPicture2, m_SmallPicResult);
	DDX_Text(pDX, IDC_DELAY, m_TimeDelay);
	DDX_Text(pDX, IDC_NUMFRAMES, m_NumFrames);
	DDX_Text(pDX, IDC_FWIDTH, m_FrameWidth);
	DDX_Text(pDX, IDC_FHEIGHT, m_FrameHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSmallPicDlg, CDialog)
	//{{AFX_MSG_MAP(CSmallPicDlg)
	ON_BN_CLICKED(IDC_SELECTFILE, OnSelectfile)
	ON_BN_CLICKED(IDC_VIEWALLPIC, OnViewallpic)
	ON_BN_CLICKED(IDC_UPDATESTATS, OnUpdatestats)
	ON_BN_CLICKED(IDC_TOGGLEANIM, OnToggleanim)
	ON_BN_CLICKED(IDC_NEXTFRAME, OnNextframe)
	ON_BN_CLICKED(IDC_ANIMATIONSTYLE, OnAnimationstyle)
	ON_BN_CLICKED(IDC_CLEARPIC, OnClearpic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmallPicDlg message handlers

BOOL CSmallPicDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  if ((m_SmallPic.timeDelay < 30) && (m_SmallPic.timeDelay != 0))
     m_SmallPic.timeDelay = 2000;

  m_SmallPicResult.CreateBlankPicture();
  m_ToggleAnim.SetWindowText("Start");

  if (m_SmallPic.NumFrames < 1)
    m_SmallPic.NumFrames = 1;
  if (m_SmallPic.FrameHeight < 1)
    m_SmallPic.FrameHeight = ViewportHeight;
  if (m_SmallPic.FrameWidth < 1)
    m_SmallPic.FrameWidth = ViewportWidth;

  m_CurrentFile = m_SmallPic.filename;
  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_CurrentFile, name, MAX_MEDITBUTTON_TEXT);
  m_SelectFile.SetWindowText(name);

  m_TimeDelay = m_SmallPic.timeDelay;
  m_FrameHeight = m_SmallPic.FrameHeight;
  m_FrameWidth = m_SmallPic.FrameWidth;
  m_NumFrames = m_SmallPic.NumFrames;
  if (m_NumFrames == 1)
    m_TimeDelay = 0;
  
  UpdateData(FALSE);

  LoadFile();

  m_SmallPicResult.SetCycleInterval(0);

  /*
  CWnd *pWnd;
  switch (m_SmallPic.picType)
  {
  case IconDib:
    pWnd = GetDlgItem(IDC_FWIDTH);
    if (pWnd) pWnd->EnableWindow(FALSE);
    pWnd = GetDlgItem(IDC_FHEIGHT);
    if (pWnd) pWnd->EnableWindow(FALSE);
    pWnd = GetDlgItem(IDC_ANIMATIONSTYLE);
    if (pWnd) pWnd->EnableWindow(FALSE);
    pWnd = GetDlgItem(IDC_DELAY);
    if (pWnd) pWnd->EnableWindow(FALSE);
    pWnd = GetDlgItem(IDC_TOGGLEANIM);
    if (pWnd) pWnd->EnableWindow(FALSE);    
    break;
  }
  */

  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSmallPicDlg::LoadFile()
{
  //BOOL success = FALSE;
  UpdateData(TRUE);

  if (m_NumFrames == 1)
    m_TimeDelay = 0;

  //m_SmallPic.filename = m_CurrentFile;
  //m_SmallPic.FrameHeight = m_FrameHeight;
  //m_SmallPic.FrameWidth = m_FrameWidth;
  //m_SmallPic.NumFrames = m_NumFrames;
  //m_SmallPic.timeDelay = m_TimeDelay;

  if (m_CurrentFile.IsEmpty())
  {
    m_SmallPicResult.ErasePicture();
    //success = TRUE;
  }
  else
  {
    PIC_DATA data;
    data = m_SmallPic;
    data.filename = m_CurrentFile;
    StripFilenamePath(data.filename);
    data.FrameHeight = m_FrameHeight;
    data.FrameWidth = m_FrameWidth;
    data.NumFrames = m_NumFrames;
    data.timeDelay = m_TimeDelay;

    if (m_SmallPicResult.LoadSourcePictures(data)) // can start animation
    {
       char name[MAX_MEDITBUTTON_TEXT+1];
       getBaseName(m_CurrentFile, name, MAX_MEDITBUTTON_TEXT);
       m_SelectFile.SetWindowText(name);

       // get size of one frame after loading
       CRect srcRect = m_SmallPicResult.GetSrcRect();
       m_SmallPicResult.Invalidate(FALSE);
       //success = TRUE;
    }
    m_SmallPicResult.SetCycleInterval(0);
  }

  UpdateData(FALSE);
}

void CSmallPicDlg::OnOK() 
{
  UpdateData(TRUE);   

  if (m_NumFrames == 1)
    m_TimeDelay = 0;

  if ((m_TimeDelay < 30) && (m_TimeDelay != 0))
     m_TimeDelay = 2000;

  m_SmallPic.filename = m_CurrentFile;
  m_SmallPic.timeDelay = m_TimeDelay;
  m_SmallPic.FrameHeight = m_FrameHeight;
  m_SmallPic.FrameWidth = m_FrameWidth;
  m_SmallPic.NumFrames = m_NumFrames;
  if (m_SmallPic.NumFrames<=1)
    m_SmallPic.flags &= ~PIC_DATA::AF_Loop;

  CDialog::OnOK();
}

void CSmallPicDlg::OnSelectfile() 
{
  UpdateData(TRUE);

  CString file(m_SmallPic.filename);
  
  if (GetFilenameWithPreview(m_SmallPic.picType, file))
  {
     m_CurrentFile = file;     
     UpdateData(FALSE);
     LoadFile();	
  }
}

void CSmallPicDlg::OnViewallpic() 
{
  UpdateData(TRUE);
  CScrollPicDlg dlg(m_CurrentFile, rte.WallArtDir(), "");
  dlg.DoModal();	
}

void CSmallPicDlg::OnUpdatestats() 
{
  int frame;
  UpdateData(TRUE);

  if (m_NumFrames < 1) m_NumFrames = 1;
  if (m_NumFrames == 1) m_TimeDelay = 0;
  if ((m_TimeDelay < 30) && (m_TimeDelay != 0))
    m_TimeDelay = 2000;
  if (m_FrameWidth < 1) m_FrameWidth = 1;
  if (m_FrameHeight < 1) m_FrameHeight = 1;

  //this->m_SmallPicResult.SetNextFrame();
  frame = this->m_SmallPicResult.GetFrame();
  UpdateData(FALSE);
  LoadFile();
  this->m_SmallPicResult.SetFrame(frame);
  UpdateData(FALSE);
}


void CSmallPicDlg::OnToggleanim() 
{
  UpdateData(TRUE);

  if (m_NumFrames == 1)
    m_TimeDelay = 0;

  if (m_NumFrames <= 1)
  {
    m_ToggleAnim.SetWindowText("Start");
    m_SmallPicResult.SetCycleInterval(0);
    return;
  }

  CString title;
  m_ToggleAnim.GetWindowText(title);
  if (title == "Start")
  {
	  m_ToggleAnim.SetWindowText("Stop");
    m_SmallPicResult.SetCycleInterval(m_TimeDelay);
  }
  else
  {
    m_ToggleAnim.SetWindowText("Start");
    m_SmallPicResult.SetCycleInterval(0);
  }
  UpdateData(FALSE);
}

void CSmallPicDlg::OnNextframe() 
{
  CString title;
  m_ToggleAnim.GetWindowText(title);
  if (title == "Start")
  {
    m_SmallPicResult.SetNextFrame();
    m_SmallPicResult.Invalidate(FALSE);
  }
}

void CSmallPicDlg::OnAnimationstyle() 
{
  CAnimationStyle dlg;
  dlg.m_AnimStyle = m_SmallPic.style;
  dlg.m_WaitForKeypress = (m_SmallPic.flags & PIC_DATA::AF_KeypressBeforeLoop)?TRUE:FALSE;
  dlg.m_LimitLoops = (m_SmallPic.flags & PIC_DATA::AF_MaxLoopCounter)?TRUE:FALSE;
  dlg.m_NumLoops = m_SmallPic.MaxLoops;
  dlg.m_Loop = (m_SmallPic.flags & PIC_DATA::AF_Loop)?TRUE:FALSE;
  dlg.m_UseAlpha = m_SmallPic.UseAlpha;
  dlg.m_AlphaValue = m_SmallPic.AlphaValue;

  if (dlg.DoModal() == IDOK)
  {    
    m_SmallPic.style = dlg.m_AnimStyle;	
    m_SmallPic.flags=PIC_DATA::AF_None;
    if (dlg.m_WaitForKeypress)
      m_SmallPic.flags |= PIC_DATA::AF_KeypressBeforeLoop;
    if (dlg.m_LimitLoops)
    {
      m_SmallPic.flags |= PIC_DATA::AF_MaxLoopCounter;
      m_SmallPic.MaxLoops = dlg.m_NumLoops;
    }
    else
      m_SmallPic.MaxLoops = 1;

    if (dlg.m_Loop)
    {
      m_SmallPic.flags |= PIC_DATA::AF_Loop;
      if (m_SmallPic.MaxLoops <= 0)
        m_SmallPic.MaxLoops = 1;
    }

    m_SmallPicResult.SetFlags(m_SmallPic.flags);
    m_SmallPicResult.SetMaxLoops(m_SmallPic.MaxLoops);

    m_SmallPic.UseAlpha = dlg.m_UseAlpha;
    m_SmallPic.AlphaValue = dlg.m_AlphaValue;

    m_SmallPicResult.GetSmallPic().UseAlpha = dlg.m_UseAlpha;
    m_SmallPicResult.GetSmallPic().AlphaValue = dlg.m_AlphaValue;
  }
}

void CSmallPicDlg::OnClearpic() 
{
  CString title;
  m_ToggleAnim.GetWindowText(title);
  if (title != "Start")
  {
    m_ToggleAnim.SetWindowText("Start");
    m_SmallPicResult.SetCycleInterval(0);
  }
  UpdateData(TRUE);
	m_SmallPic.Clear();
  m_NumFrames=1;
  m_CurrentFile="";
  UpdateData(FALSE);
  OnUpdatestats();

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_CurrentFile, name, MAX_MEDITBUTTON_TEXT);
  m_SelectFile.SetWindowText(name);
}
