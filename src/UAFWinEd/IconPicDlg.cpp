/******************************************************************************
* Filename: IconPicDlg.cpp
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
#include "PicData.h"
#include "ScrollPicDlg.h"
#include "IconPicDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconPicDlg dialog


CIconPicDlg::CIconPicDlg(PIC_DATA data, CWnd* pParent /*=NULL*/)
	: CDialog(CIconPicDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIconPicDlg)
	m_Frames = 1;
	//}}AFX_DATA_INIT

  m_data = data;
  if (m_data.picType == BogusDib)
    m_data.picType = IconDib;
  m_data.flags &= ~PIC_DATA::AF_Loop;
}


void CIconPicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIconPicDlg)
	DDX_Control(pDX, IDC_SELECTFILE, m_SelectFile);
	DDX_Control(pDX, IDC_ICONPICTURE, m_IconPicture);
	DDX_Text(pDX, IDC_FRAMES, m_Frames);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIconPicDlg, CDialog)
	//{{AFX_MSG_MAP(CIconPicDlg)
	ON_BN_CLICKED(IDC_SELECTFILE, OnSelectfile)
	ON_BN_CLICKED(IDC_VIEWALLPIC, OnViewallpic)
	ON_BN_CLICKED(IDC_CLEARICON, OnClearicon)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconPicDlg message handlers


BOOL CIconPicDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
  m_IconPicture.CreateBlankPicture();
  LoadFile();
  m_Frames=m_data.NumFrames;
  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIconPicDlg::LoadFile()
{
  if (m_data.filename.IsEmpty())
  {
    m_IconPicture.ErasePicture();
  }
  else
  {
    if (m_IconPicture.LoadPicture(m_data.filename, IconDib, rte.IconArtDir(), ede.TemplateIconArtDir()))
    {
      m_IconPicture.Invalidate(FALSE);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_data.filename, name, MAX_MEDITBUTTON_TEXT);
      m_SelectFile.SetWindowText(name);
    }
  }
}

void CIconPicDlg::OnSelectfile() 
{
  CString file(m_data.filename);
  
  if (GetFilenameWithPreview(IconDib, file))
  {
    m_data.filename = file;
    LoadFile();
  }
}

void CIconPicDlg::OnViewallpic() 
{
  CScrollPicDlg dlg(m_data.filename, rte.WallArtDir(), "");
  dlg.DoModal();		
}

void CIconPicDlg::OnOK() 
{
  UpdateData(TRUE);	
  m_data.NumFrames=m_Frames;
  m_data.FrameHeight = m_IconPicture.GetSrcRect().Height();
  m_data.FrameWidth  = m_IconPicture.GetSrcRect().Width()/m_data.NumFrames;
  m_data.flags &= ~PIC_DATA::AF_Loop;
	CDialog::OnOK();
}

void CIconPicDlg::OnClearicon() 
{
	m_data.Clear();
  LoadFile();
}
