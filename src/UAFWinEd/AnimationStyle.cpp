/******************************************************************************
* Filename: AnimationStyle.cpp
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

//#include "uafwined.h"
#include "AnimationStyle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnimationStyle dialog


CAnimationStyle::CAnimationStyle(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimationStyle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnimationStyle)
	m_AnimStyle = 0;
	m_WaitForKeypress = FALSE;
	m_LimitLoops = FALSE;
	m_NumLoops = 1;
	m_Loop = FALSE;
	m_UseAlpha = FALSE;
	m_AlphaValue = 0;
	//}}AFX_DATA_INIT
}


void CAnimationStyle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnimationStyle)
	DDX_Radio(pDX, IDC_DEFAULTANIM, m_AnimStyle);
	DDX_Check(pDX, IDC_INPUTBEFORELOOP, m_WaitForKeypress);
	DDX_Check(pDX, IDC_LIMITLOOPING, m_LimitLoops);
	DDX_Text(pDX, IDC_NUMLOOPS, m_NumLoops);
	DDX_Check(pDX, IDC_LOOP, m_Loop);
	DDX_Check(pDX, IDC_USEALPHA, m_UseAlpha);
	DDX_Text(pDX, IDC_ALPHAVALUE, m_AlphaValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnimationStyle, CDialog)
	//{{AFX_MSG_MAP(CAnimationStyle)
	ON_BN_CLICKED(IDC_LIMITLOOPING, OnLimitlooping)
	ON_BN_CLICKED(IDC_DEFAULTANIM, OnDefaultanim)
	ON_BN_CLICKED(IDC_DIRECTIONALANIM, OnDirectionalanim)
	ON_BN_CLICKED(IDC_EACHTARGETANIM, OnEachtargetanim)
	ON_BN_CLICKED(IDC_RADIUSANIM, OnRadiusanim)
	ON_BN_CLICKED(IDC_INPUTBEFORELOOP, OnInputbeforeloop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimationStyle message handlers

BOOL CAnimationStyle::OnInitDialog() 
{
	CDialog::OnInitDialog();
  UpdateControls();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnimationStyle::OnDefaultanim() 
{
  UpdateData(TRUE);
  m_Loop=TRUE;
  UpdateData(FALSE);
  UpdateControls();
}

void CAnimationStyle::OnDirectionalanim() 
{
  UpdateControls();
}

void CAnimationStyle::OnEachtargetanim() 
{
  UpdateControls();
}

void CAnimationStyle::OnRadiusanim() 
{
  UpdateControls();
}

void CAnimationStyle::OnInputbeforeloop() 
{	
  UpdateControls();
}

void CAnimationStyle::OnLimitlooping() 
{	
  UpdateControls();
}

void CAnimationStyle::UpdateControls()
{
  /*
    AS_None=0,        // all frames are sequenced
    AS_Directional=1, // 1 frame per direction: N,E,S,W,NW,NE,SW,SE
    AS_Radius=2,      // single frame covers target area radius
    AS_EachTarget=3   // single frame covers each target character only
  */

  UpdateData(TRUE);
  CWnd *pwnd=NULL;
  switch (m_AnimStyle)
  {
  case 0:	
    pwnd = GetDlgItem(IDC_LIMITLOOPING);
    if (pwnd!=NULL) pwnd->EnableWindow(TRUE);
    pwnd = GetDlgItem(IDC_NUMLOOPS);
    if (pwnd!=NULL) pwnd->EnableWindow(TRUE);    
    pwnd = GetDlgItem(IDC_INPUTBEFORELOOP);
    if (pwnd!=NULL) pwnd->EnableWindow(TRUE);	    
    pwnd = GetDlgItem(IDC_LOOP);
    if (pwnd!=NULL) pwnd->EnableWindow(TRUE);	    
    break;
  case 1:
    pwnd = GetDlgItem(IDC_LIMITLOOPING);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
    pwnd = GetDlgItem(IDC_NUMLOOPS);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);    
    pwnd = GetDlgItem(IDC_INPUTBEFORELOOP);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);	
    pwnd = GetDlgItem(IDC_LOOP);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);	
	  m_WaitForKeypress = FALSE;
	  m_LimitLoops = FALSE;
	  m_NumLoops = 1;
    m_Loop=FALSE;
    break;
  case 2:
    pwnd = GetDlgItem(IDC_LIMITLOOPING);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
    pwnd = GetDlgItem(IDC_NUMLOOPS);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);    
    pwnd = GetDlgItem(IDC_INPUTBEFORELOOP);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);		
    pwnd = GetDlgItem(IDC_LOOP);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);	
	  m_WaitForKeypress = FALSE;
	  m_LimitLoops = FALSE;
	  m_NumLoops = 1;
    m_Loop=FALSE;
    break;
  case 3:
    pwnd = GetDlgItem(IDC_LIMITLOOPING);
    if (pwnd!=NULL) pwnd->EnableWindow(TRUE);
    pwnd = GetDlgItem(IDC_NUMLOOPS);
    if (pwnd!=NULL) pwnd->EnableWindow(TRUE);    
    pwnd = GetDlgItem(IDC_INPUTBEFORELOOP);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);		
    pwnd = GetDlgItem(IDC_LOOP);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);	
	  m_WaitForKeypress = FALSE;
    m_Loop=FALSE;
    break;
  }

  if (m_Loop)
  {
    if (m_LimitLoops)
    {
      if (m_NumLoops<=0)
        m_NumLoops=1;
    }
  }

  if (m_LimitLoops)
  {
    m_WaitForKeypress = FALSE;
    pwnd = GetDlgItem(IDC_INPUTBEFORELOOP);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
  }
  if (m_WaitForKeypress)
  {
    m_LimitLoops = FALSE;
    m_NumLoops = 1;
    m_Loop=TRUE;
    pwnd = GetDlgItem(IDC_LIMITLOOPING);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
    pwnd = GetDlgItem(IDC_NUMLOOPS);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);    
  }

  UpdateData(FALSE);
}
