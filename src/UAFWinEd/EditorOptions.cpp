/******************************************************************************
* Filename: EditorOptions.cpp
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

#include "uafwined.h"
#include "EditorOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditorOptions dialog


CEditorOptions::CEditorOptions(CEditorOptionsData &data, CWnd* pParent /*=NULL*/)
	: CDialog(CEditorOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditorOptions)
//	m_ReloadDesign = FALSE;
	m_DblFillWall = FALSE;
	m_DynaCell = FALSE;
	m_KwikKlik = FALSE;
	m_TileMap = FALSE;
	m_PassWall = FALSE;
	m_QuadFillBg = FALSE;
	m_EditorScreenRes = 0;
	//}}AFX_DATA_INIT
  m_data = data;
}


void CEditorOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditorOptions)
//	DDX_Check(pDX, IDC_RELOADDESIGN, m_ReloadDesign);
	DDX_Check(pDX, IDC_DOUBLEFILLWALL, m_DblFillWall);
	DDX_Check(pDX, IDC_ENABLEDYNACELL, m_DynaCell);
	DDX_Check(pDX, IDC_ENABLEKWIKKLICK, m_KwikKlik);
	DDX_Check(pDX, IDC_ENABLETILEMAP, m_TileMap);
	DDX_Check(pDX, IDC_ENABLEPASSWALLS, m_PassWall);
	DDX_Check(pDX, IDC_QUADFILLBG, m_QuadFillBg);
	DDX_Radio(pDX, IDC_EDITORRES640, m_EditorScreenRes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditorOptions, CDialog)
	//{{AFX_MSG_MAP(CEditorOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditorOptions message handlers

BOOL CEditorOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
//	m_ReloadDesign = m_data.m_AutoReloadDesign;
  m_DblFillWall = m_data.m_LastDoubleFillWall;
  m_DynaCell = m_data.m_LastBlockType;
  m_KwikKlik = m_data.m_LastKwikKlik;
  m_TileMap = m_data.m_LastTileMap;
  m_PassWall = m_data.m_LastPassWalls;
  m_QuadFillBg = m_data.m_LastQuadFileBg;
  m_EditorScreenRes = m_data.m_EditorScreenRes;

  CWnd *pWnd;

  switch (m_GameScreenRes)
  {
  case res640x480:

    if (m_EditorScreenRes==res1152x864)
    {
      // switch editor to legal mode for this game res
      m_EditorScreenRes=res640x480;
    }

    pWnd=GetDlgItem(IDC_EDITORRES1152);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    break;

  case res800x600:
    if (   (m_EditorScreenRes==res640x480)
        || (m_EditorScreenRes==res1152x864))
    {
      // switch editor to legal mode for this game res
      m_EditorScreenRes=res800x600;
    }

    pWnd=GetDlgItem(IDC_EDITORRES640);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    pWnd=GetDlgItem(IDC_EDITORRES1152);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    break;

  case res1024x768:
    if (   (m_EditorScreenRes==res640x480)
        || (m_EditorScreenRes==res800x600))
    {
      // switch editor to legal mode for this game res
      m_EditorScreenRes=res1024x768;
    }

    pWnd=GetDlgItem(IDC_EDITORRES640);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    pWnd=GetDlgItem(IDC_EDITORRES800);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    break;

  default:
    pWnd=GetDlgItem(IDC_EDITORRES640);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    pWnd=GetDlgItem(IDC_EDITORRES800);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    pWnd=GetDlgItem(IDC_EDITORRES1024);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    pWnd=GetDlgItem(IDC_EDITORRES1152);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    pWnd=GetDlgItem(IDC_EDITORRES1280);
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
    break;
  }

  UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditorOptions::OnOK() 
{
	UpdateData(TRUE);

//  m_data.m_AutoReloadDesign = m_ReloadDesign;
  m_data.m_LastDoubleFillWall=m_DblFillWall;
  m_data.m_LastBlockType=m_DynaCell;
  m_data.m_LastKwikKlik=m_KwikKlik;
  m_data.m_LastTileMap=m_TileMap;
  m_data.m_LastPassWalls=m_PassWall;
  m_data.m_LastQuadFileBg=m_QuadFillBg;

  if ((DWORD)m_EditorScreenRes!=m_data.m_EditorScreenRes)
  {
    MessageBox("You must restart the editor before\nyour screen res changes will take effect",
               "Editor Screen Resolution Change");
  }
  m_data.m_EditorScreenRes=m_EditorScreenRes;
	
	CDialog::OnOK();
}
