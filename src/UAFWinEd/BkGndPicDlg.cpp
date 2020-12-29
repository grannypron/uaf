/******************************************************************************
* Filename: BkGndPicDlg.cpp
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

//#include "externs.h"
//#include "UAFWinEd.h"
//#include "SurfaceMgr.h"
#include "class.h"
#include "PicSlot.h"
#include "ScrollPicDlg.h"
#include "SoundChooser.h"
#include "BkGndPicDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBkGndPicDlg dialog


CBkGndPicDlg::CBkGndPicDlg(BackgroundSlotMemType &data, CWnd* pParent /*=NULL*/)
	: CDialog(CBkGndPicDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBkGndPicDlg)
	m_EndTime = 0;
	m_StartTime = 0;
	m_UseBg2 = FALSE;
	m_AltFilename = _T("");
	m_Filename = _T("");
	m_BlendPcnt = 0;
	m_UseAlphaBlend = FALSE;
	m_UseTransparency = FALSE;
	m_SuppressStepSound = FALSE;
	//}}AFX_DATA_INIT
	m_data = data;
	m_FrameWidth = 0;
	m_FrameHeight = 0;
	m_NumFrames = 0;
	m_timeDelay = 0;
}


void CBkGndPicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBkGndPicDlg)
	DDX_Control(pDX, IDC_SELECTSOUND, m_SelectSound);
	DDX_Control(pDX, IDC_BGPicture2, m_Picture2);
	DDX_Control(pDX, IDC_BGPicture, m_Picture);
	DDX_Text(pDX, IDC_BGENDTIME, m_EndTime);
	DDV_MinMaxInt(pDX, m_EndTime, 0, 2359);
	DDX_Text(pDX, IDC_BGTIMESTART, m_StartTime);
	DDV_MinMaxInt(pDX, m_StartTime, 0, 2359);
	DDX_Check(pDX, IDC_USEBG2, m_UseBg2);
	DDX_Text(pDX, IDC_ALTFILENAME, m_AltFilename);
	DDX_Text(pDX, IDC_DEFAULTFILENAME, m_Filename);
	DDX_Text(pDX, IDC_BLENDPCNT, m_BlendPcnt);
	DDX_Check(pDX, IDC_USEALPHABLEND, m_UseAlphaBlend);
	DDX_Check(pDX, IDC_USETRANSPARENCY, m_UseTransparency);
	DDX_Check(pDX, IDC_SUPPRESSSTEPSOUND, m_SuppressStepSound);
	DDX_Text(pDX, IDC_BGFWIDTH, m_FrameWidth);
	DDX_Text(pDX, IDC_BGFHEIGHT, m_FrameHeight);
	DDX_Text(pDX, IDC_BGNUMFRAMES, m_NumFrames);
	DDX_Text(pDX, IDC_BGDELAY, m_timeDelay);
	DDX_Control(pDX, IDC_BGTOGGLEANIM, m_ToggleAnim);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBkGndPicDlg, CDialog)
	//{{AFX_MSG_MAP(CBkGndPicDlg)
	ON_BN_CLICKED(IDC_USEBG2, OnUsebg2)
	ON_BN_CLICKED(IDC_SELECTDEFAULT, OnSelectdefault)
	ON_BN_CLICKED(IDC_SELECTALT, OnSelectalt)
	ON_BN_CLICKED(IDC_SELECTSOUND, OnSelectsound)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BGVIEWALLPIC, &CBkGndPicDlg::OnBnClickedBgviewallpic)
	ON_BN_CLICKED(IDC_BGUPDATESTATS, &CBkGndPicDlg::OnBnClickedBgupdatestats)
	ON_BN_CLICKED(IDC_BGTOGGLEANIM, &CBkGndPicDlg::OnBnClickedBgtoggleanim)
	ON_BN_CLICKED(IDC_BGNEXTFRAME, &CBkGndPicDlg::OnBnClickedBgnextframe)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBkGndPicDlg message handlers

BOOL CBkGndPicDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	 
  m_Picture.CreateBlankPicture();
  m_Picture2.CreateBlankPicture();

 	m_EndTime = m_data.EndTime;
	m_StartTime = m_data.StartTime;
	m_UseBg2 = m_data.useAltBackground;
	m_AltFilename = m_data.backgroundFileAlt;
	m_Filename = m_data.backgroundFile;
	m_BlendPcnt = m_data.AlphaBlendPcnt;
	m_UseAlphaBlend = m_data.useAlphaBlend;
  m_UseTransparency = m_data.useTransparency;
  m_Sound = m_data.soundFile;
  m_SuppressStepSound = m_data.suppressStepSound;

  m_FrameWidth = m_data.FrameWidth;
  m_FrameHeight = m_data.FrameHeight;
  m_NumFrames = m_data.NumFrames;
  m_timeDelay = m_data.timeDelay;

  LoadFile(m_Filename);
  LoadAltFile(m_AltFilename);	

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_Sound, name, MAX_MEDITBUTTON_TEXT);
  m_SelectSound.SetWindowText(name);

  if (m_NumFrames <= 0) {
	  m_NumFrames = 1;
  }
  m_Picture.SetFrame(1);
  UpdateSrcRectForFrame();

  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBkGndPicDlg::OnUsebg2() 
{
   UpdateData(TRUE);
}

void CBkGndPicDlg::OnSelectdefault() 
{
  UpdateData(TRUE);
  CString file = m_Filename;  
  if (GetFilenameWithPreview(BackGndDib,file))
     LoadFile(file);
}

void CBkGndPicDlg::OnSelectalt() 
{
  UpdateData(TRUE);
  CString file = m_AltFilename;  
  if (GetFilenameWithPreview(BackGndDib,file))
     LoadAltFile(file);
}

void CBkGndPicDlg::LoadFile(CString file)
{
   BOOL success = FALSE;
  if (file.IsEmpty())
  {
    m_Picture.ErasePicture();
    success = TRUE;
  }
  else
  {
    if (m_Picture.LoadPicture(file, BackGndDib, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir()))
    {
      m_Picture.Invalidate(FALSE);
      success = TRUE;
    }
  }

   if (success)
   {
      m_Filename = file;
      UpdateData(FALSE);
   }
}

void CBkGndPicDlg::LoadAltFile(CString file)
{
   BOOL success = FALSE;

  if (file.IsEmpty())
  {
    m_Picture2.ErasePicture();
    success = TRUE;
  }
  else
  {
    if (m_Picture2.LoadPicture(file, BackGndDib, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir()))
    {
      m_Picture2.Invalidate(FALSE);
      success = TRUE;
    }
  }

   if (success)
   {
      m_AltFilename = file;
      UpdateData(FALSE);
   }
}

void CBkGndPicDlg::OnOK() 
{
	UpdateData(TRUE);
 	m_data.EndTime = m_EndTime;
	m_data.StartTime = m_StartTime;
	m_data.useAltBackground = m_UseBg2;
	m_data.backgroundFileAlt = m_AltFilename;
	m_data.backgroundFile = m_Filename;
	m_data.AlphaBlendPcnt = m_BlendPcnt;
	m_data.useAlphaBlend = m_UseAlphaBlend;
  m_data.useTransparency = m_UseTransparency;
  m_data.suppressStepSound = m_SuppressStepSound;
  m_data.soundFile = m_Sound;
  
  if (m_data.backgroundFileAlt=="")
  {
    m_data.useAlphaBlend=FALSE;
    m_data.useAltBackground=FALSE;
    m_data.useTransparency=FALSE;
    m_data.AlphaBlendPcnt=0;
    m_data.EndTime=0;
    m_data.StartTime=0;
  }
  m_data.FrameWidth = m_FrameWidth;
  m_data.FrameHeight = m_FrameHeight;
  m_data.NumFrames = m_NumFrames;
  m_data.timeDelay = m_timeDelay;
  CTimer::StopTimer();
  CDialog::OnOK();
}

void CBkGndPicDlg::OnSelectsound() 
{
  CSoundChooser dlg(m_Sound, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    m_Sound = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_Sound, name, MAX_MEDITBUTTON_TEXT);
    m_SelectSound.SetWindowText(name);
  }		
}


void CBkGndPicDlg::OnBnClickedBgviewallpic()
{
	UpdateData(TRUE);
	CScrollPicDlg dlg(m_Filename, rte.WallArtDir(), "");
	dlg.DoModal();
}


void CBkGndPicDlg::OnBnClickedBgupdatestats()
{
	int frame;
	UpdateData(TRUE);

	if (m_NumFrames < 1) m_NumFrames = 1;
	if (m_NumFrames == 1) { m_timeDelay = 0; }
	if ((m_timeDelay < 30) && (m_timeDelay != 0))
		m_timeDelay = 2000;
	if (m_FrameWidth < 1) m_FrameWidth = 1;
	if (m_FrameHeight < 1) m_FrameHeight = 1;

	frame = this->m_Picture.GetFrame();
	UpdateData(FALSE);
	LoadFile(m_Filename);

	UpdateSrcRectForFrame();
	this->m_Picture.SetFrame(frame);
}

void CBkGndPicDlg::UpdateSrcRectForFrame() {
	if (m_NumFrames > 1) {
		// Change the position of the src image's viewed rectangle by the frame of that is specified if there is more than one
		CRect framedRect;
		framedRect = this->m_Picture.GetSrcRect();
		framedRect.left = m_FrameWidth * (this->m_Picture.GetFrame() - 1);
		framedRect.right = framedRect.left + m_FrameWidth;
		this->m_Picture.SetSrcRect(framedRect);
	}
	else {
		m_Picture.SetFrame(1);
	}
}

void CBkGndPicDlg::OnBnClickedBgtoggleanim()
{
	if (m_NumFrames <= 1)
	{
		m_ToggleAnim.SetWindowText("Start");
		return;
	}

	CString title;
	m_ToggleAnim.GetWindowText(title);
	if (title == "Start")
	{
		if (CTimer::Active())
			StopTimer();

		StartPeriodicTimer(m_timeDelay);
		m_ToggleAnim.SetWindowText("Stop");
	}
	else
	{
		StopTimer();
		m_ToggleAnim.SetWindowText("Start");
	}
}

VOID CBkGndPicDlg::OnTimerEvent(UINT TimerId) {
	AdvanceBGFrame();
}


void CBkGndPicDlg::OnBnClickedBgnextframe()
{
	CString title;
	m_ToggleAnim.GetWindowText(title);
	if (title == "Start")
	{
		AdvanceBGFrame();
	}
}

void CBkGndPicDlg::AdvanceBGFrame() {
	if (m_Picture.GetFrame() < m_NumFrames && m_Picture.GetFrame() > 0) {
		m_Picture.SetFrame(m_Picture.GetFrame() + 1);
	}
	else {
		m_Picture.SetFrame(1);
	}
	UpdateSrcRectForFrame();
	m_Picture.Invalidate(FALSE);

}
