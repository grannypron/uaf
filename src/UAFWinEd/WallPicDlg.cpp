/******************************************************************************
* Filename: WallPicDlg.cpp
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

#include "UAFWinEd.h"
#include "SurfaceMgr.h"
//#include "PicSlot.h"
#include "SoundChooser.h"
#include "WallPicDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWallPicDlg dialog


CWallPicDlg::CWallPicDlg(CString filename, CString sound, CWnd* pParent /*=NULL*/)
	: CDialog(CWallPicDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWallPicDlg)
	m_ImageFormat = DEFAULT_FORMAT_TYPE;
	m_DrawAV = TRUE;
	//}}AFX_DATA_INIT
  m_Filename = filename;
  m_Sound = sound;
  m_UseAV=TRUE;
}


void CWallPicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWallPicDlg)
	DDX_Control(pDX, IDC_SELECTSOUND, m_SelectSound);
	DDX_Control(pDX, IDC_SELECTFILE, m_SelectFile);
	DDX_Control(pDX, IDC_WallPicture, m_Picture);
	DDX_Text(pDX, IDC_IMAGEFORMAT, m_ImageFormat);
	DDX_Check(pDX, IDC_DRAWAV, m_DrawAV);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWallPicDlg, CDialog)
	//{{AFX_MSG_MAP(CWallPicDlg)
	ON_BN_CLICKED(IDC_SELECTFILE, OnSelectfile)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_SELECTSOUND, OnSelectsound)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWallPicDlg message handlers

BOOL CWallPicDlg::OnInitDialog() 
{
	CDialog::OnInitDialog(); 

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_Sound, name, MAX_MEDITBUTTON_TEXT);
  m_SelectSound.SetWindowText(name);

  if (!m_UseAV)
  {
    CWnd *pWnd = GetDlgItem(IDC_DRAWAV);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
  }

  UpdateData(FALSE);
  LoadFile(m_Filename);	 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CWallPicDlg::LoadFile(CString file)
{
  BOOL success = FALSE;
  m_Picture.ErasePicture();

  if (file.IsEmpty())
  {
    m_ImageFormat = DEFAULT_FORMAT_TYPE;    
    success = TRUE;
  }
  else
  {
    if (m_Picture.LoadSourcePicture(file, WallDib))
    {
      m_ImageFormat = m_Picture.m_format;
      success = TRUE;
    }
  }

  if (success)
  {
     m_Filename = file;

     char name[MAX_MEDITBUTTON_TEXT+1];
     getBaseName(m_Filename, name, MAX_MEDITBUTTON_TEXT);
     m_SelectFile.SetWindowText(name);

     UpdateData(FALSE);
  }
  m_Picture.Invalidate(FALSE);
}

void CWallPicDlg::OnSelectfile() 
{
  CString file = m_Filename;
  
  if (GetFilenameWithPreview(WallDib, file))
     LoadFile(file);
}

void CWallPicDlg::OnClear() 
{
	m_Filename = "";
  m_Picture.LoadSourcePicture(m_Filename, WallDib);
  m_ImageFormat = m_Picture.m_format;
  m_DrawAV=TRUE;
  UpdateData(FALSE);
}

void CWallPicDlg::OnSelectsound() 
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
