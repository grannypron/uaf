/******************************************************************************
* Filename: TitleScreenConfig.cpp
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
#include "ScrollPicDlg.h"
#include "TitleScreenConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL IsMovieFile(const char *file)
{
  CString temp = file;
  temp.MakeLower();
  return (strstr(temp, ".avi") != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CTitleScreenConfig dialog


CTitleScreenConfig::CTitleScreenConfig(TITLE_SCREEN &data, CWnd* pParent /*=NULL*/)
  : CDialog(CTitleScreenConfig::IDD, pParent)
{
  //{{AFX_DATA_INIT(CTitleScreenConfig)
  m_UseBlend = FALSE;
  m_DisplayStyle = 0;
  m_UseTrans = FALSE;
  //}}AFX_DATA_INIT
  m_data = data;
}


void CTitleScreenConfig::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTitleScreenConfig)
  DDX_Control(pDX, IDC_SELECTTITLEFILENAME, m_SelectTitle);
  DDX_Check(pDX, IDC_BLENDWITHPREV, m_UseBlend);
  DDX_Radio(pDX, IDC_DISPLAYSTYLE, m_DisplayStyle);
  DDX_Check(pDX, IDC_USETRANS, m_UseTrans);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTitleScreenConfig, CDialog)
  //{{AFX_MSG_MAP(CTitleScreenConfig)
  ON_BN_CLICKED(IDC_DISPLAYSTYLE3, OnDisplaystyle3)
  ON_BN_CLICKED(IDC_DISPLAYSTYLE2, OnDisplaystyle2)
  ON_BN_CLICKED(IDC_DISPLAYSTYLE, OnDisplaystyle)
  ON_BN_CLICKED(IDC_SELECTTITLEFILENAME, OnSelecttitlefilename)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTitleScreenConfig message handlers

void CTitleScreenConfig::OnDisplaystyle3() 
{
  CWnd *pWnd = GetDlgItem(IDC_BLENDWITHPREV);
  if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
  pWnd = GetDlgItem(IDC_USETRANS);
  if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
}

void CTitleScreenConfig::OnDisplaystyle2() 
{
  CWnd *pWnd = GetDlgItem(IDC_BLENDWITHPREV);
  if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
  pWnd = GetDlgItem(IDC_USETRANS);
  if (pWnd!=NULL) pWnd->EnableWindow(TRUE); 
}

void CTitleScreenConfig::OnDisplaystyle() 
{
  CWnd *pWnd = GetDlgItem(IDC_BLENDWITHPREV);
  if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
  pWnd = GetDlgItem(IDC_USETRANS);
  if (pWnd!=NULL) pWnd->EnableWindow(TRUE);   
}

void CTitleScreenConfig::OnSelecttitlefilename() 
{
  UpdateData(TRUE);

  if (m_DisplayStyle == TITLE_SCREEN::tsMovie)
  {
    PLAY_MOVIE_DATA data;
    data.m_filename = m_data.TitleBgArt;
    data.m_mode = PLAY_MOVIE_DATA::movieFull;
    PlayMovieDlg dlg(data, TRUE);
    if (dlg.DoModal()==IDOK)
    {
      dlg.GetData(data);
      m_data.TitleBgArt=data.m_filename;
    }
  }
  else
  {
    CScrollPicDlg dlg(m_data.TitleBgArt, rte.TitleArtDir(), "");
    if (dlg.DoModal()==IDOK)
      m_data.TitleBgArt = dlg.m_Filename;
  }

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_data.TitleBgArt, name, MAX_MEDITBUTTON_TEXT);
  m_SelectTitle.SetWindowText(name);

  if (   (m_DisplayStyle == TITLE_SCREEN::tsMovie)
      && (!IsMovieFile(m_data.TitleBgArt)))
  {
    // remove movie style flag if not an AVI file
    m_DisplayStyle = TITLE_SCREEN::tsFadeIn;
  }

  UpdateData(FALSE);
}

BOOL CTitleScreenConfig::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  m_UseBlend = m_data.UseBlend;
  m_DisplayStyle = m_data.DisplayBy;
  m_UseTrans = m_data.UseTrans;

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_data.TitleBgArt, name, MAX_MEDITBUTTON_TEXT);
  m_SelectTitle.SetWindowText(name);

  if (m_DisplayStyle == TITLE_SCREEN::tsMovie)
  {
    CWnd *pWnd = GetDlgItem(IDC_BLENDWITHPREV);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    pWnd = GetDlgItem(IDC_USETRANS);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);  
  }

  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CTitleScreenConfig::OnOK() 
{
  UpdateData(TRUE);

  m_data.UseBlend=m_UseBlend;
  m_data.DisplayBy=m_DisplayStyle;
  m_data.UseTrans=m_UseTrans;

  if (   (m_DisplayStyle == TITLE_SCREEN::tsMovie)
      && (!IsMovieFile(m_data.TitleBgArt)))
  {
    // remove movie style flag if not an AVI file
    m_data.DisplayBy = TITLE_SCREEN::tsFadeIn;
  }
   
  CDialog::OnOK();
}
