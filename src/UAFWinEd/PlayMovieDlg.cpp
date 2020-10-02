/******************************************************************************
* Filename: PlayMovieDlg.cpp
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
#include "class.h"

#include "GameEvent.h"

#include "PlayMovieDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PlayMovieDlg dialog


PlayMovieDlg::PlayMovieDlg(PLAY_MOVIE_DATA data, BOOL FullOnly, CWnd* pParent /*=NULL*/)
	: CDialog(PlayMovieDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PlayMovieDlg)
	m_PlaybackSize = 0;
	//}}AFX_DATA_INIT
  m_data = data;
  m_FullOnly = FullOnly;
}


void PlayMovieDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PlayMovieDlg)
	DDX_Control(pDX, IDC_CHOOSEMOVIE, m_ChooseMovie);
	DDX_Radio(pDX, IDC_PLAYBACKSIZE, m_PlaybackSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PlayMovieDlg, CDialog)
	//{{AFX_MSG_MAP(PlayMovieDlg)
	ON_BN_CLICKED(IDC_TESTMOVIE, OnTestmovie)
	ON_BN_CLICKED(IDC_CHOOSEMOVIE, OnChoosemovie)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PlayMovieDlg message handlers

BOOL PlayMovieDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_data.m_filename, name, MAX_MEDITBUTTON_TEXT);
  m_ChooseMovie.SetWindowText(name);

  if (m_FullOnly)
  {
    m_data.m_mode = PLAY_MOVIE_DATA::movieFull;
    CWnd *pWnd = GetDlgItem(IDC_PLAYBACKSIZE);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    pWnd = GetDlgItem(IDC_PLAYBACKSIZE2);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    pWnd = GetDlgItem(IDC_PLAYBACKSIZE3);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
  }

  m_PlaybackSize = m_data.m_mode;

  UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void PlayMovieDlg::OnOK() 
{
  UpdateData(TRUE);
  m_data.m_mode = m_PlaybackSize;
	CDialog::OnOK();
}

void PlayMovieDlg::OnTestmovie() 
{
  if (!FileExists(m_data.m_filename))
  {
    CString msg;
    msg.Format("File \'%s\' does not exist", m_data.m_filename);
    MsgBoxError(msg);
    return;
  }

  DWORD result = (DWORD)ShellExecute(this->m_hWnd, 
                                      "open",
                                      m_data.m_filename, 
                                      NULL, NULL, SW_SHOW);
  if (result <= 32)
  {
    CString msg;
    msg.Format("Error %u trying to launch default viewer for \'%s\'", 
                result, m_data.m_filename);
    MsgBoxError(msg);
  }
}

void PlayMovieDlg::OnChoosemovie() 
{
	if (GetFilename(m_data.m_filename, "AVI", TRUE))
  {
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_data.m_filename, name, MAX_MEDITBUTTON_TEXT);
    m_ChooseMovie.SetWindowText(name);
  }	
}
