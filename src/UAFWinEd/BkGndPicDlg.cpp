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
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBkGndPicDlg, CDialog)
	//{{AFX_MSG_MAP(CBkGndPicDlg)
	ON_BN_CLICKED(IDC_USEBG2, OnUsebg2)
	ON_BN_CLICKED(IDC_SELECTDEFAULT, OnSelectdefault)
	ON_BN_CLICKED(IDC_SELECTALT, OnSelectalt)
	ON_BN_CLICKED(IDC_SELECTSOUND, OnSelectsound)
	//}}AFX_MSG_MAP
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

  LoadFile(m_Filename);
  LoadAltFile(m_AltFilename);	

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_Sound, name, MAX_MEDITBUTTON_TEXT);
  m_SelectSound.SetWindowText(name);

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
