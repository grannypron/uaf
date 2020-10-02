/******************************************************************************
* Filename: SoundEventDlg.cpp
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
#include "SoundChooser.h"
#include "items.h"
#include "char.h"
#include "SoundEventDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoundEventDlg dialog


CSoundEventDlg::CSoundEventDlg(SOUND_EVENT &data, CWnd* pParent /*=NULL*/)
	: CDialog(CSoundEventDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSoundEventDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_data = data;
}


void CSoundEventDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoundEventDlg)
	DDX_Control(pDX, IDC_SOUNDLIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSoundEventDlg, CDialog)
	//{{AFX_MSG_MAP(CSoundEventDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundEventDlg message handlers

void CSoundEventDlg::OnAdd() 
{
  CString temp;
  CSoundChooser dlg(temp, ALL_SOUND_TYPES);

  if (dlg.DoModal() == IDOK)
  {
    temp = dlg.m_Filename;
    AddSound(temp);
  }	
}

void CSoundEventDlg::OnRemove() 
{
  int index = m_List.GetCurSel();
  if (index != LB_ERR)
    RemoveSound(index);
}

void CSoundEventDlg::OnEdit() 
{
  int index = m_List.GetCurSel();
  if (index < 0) return;
  CString temp;
  m_List.GetText(index, temp);
  CSoundChooser dlg(temp, ALL_SOUND_TYPES);
  if (dlg.DoModal() == IDOK)
  {
    temp = dlg.m_Filename;
    m_List.DeleteString(index);
    m_List.InsertString(index, temp);
  }	
}

BOOL CSoundEventDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_List.ResetContent();

  POSITION pos = m_data.sounds.GetHeadPosition();
  while (pos != NULL)
    m_List.AddString(m_data.sounds.GetNext(pos));
	
  // set max width so horz scroll bar
  // will show if needed
  CString str;
  CSize   sz;
  int     dx=0;
  CDC*    pDC = m_List.GetDC();
  for (int i=0;i < m_List.GetCount();i++)
  {
     m_List.GetText( i, str );
     sz = pDC->GetTextExtent(str);

     if (sz.cx > dx)
        dx = sz.cx;
  }

  m_List.ReleaseDC(pDC);
  m_List.SetHorizontalExtent(dx);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSoundEventDlg::AddSound(CString &sound)
{
  //if (FindSound(sound))
  //  return;

  m_List.AddString(sound);
  //m_data.sounds.AddTail(sound);

  // set max width so horz scroll bar
  // will show if needed
  CString str;
  CSize   sz;
  int     dx=0;
  CDC*    pDC = m_List.GetDC();
  for (int i=0;i < m_List.GetCount();i++)
  {
     m_List.GetText( i, str );
     sz = pDC->GetTextExtent(str);

     if (sz.cx > dx)
        dx = sz.cx;
  }

  m_List.ReleaseDC(pDC);
  m_List.SetHorizontalExtent(dx);
}


void CSoundEventDlg::RemoveSound(int index)
{
  //CString name;
  //m_List.GetText(index, name);
  m_List.DeleteString(index);
  
  /*
  POSITION pos = m_data.sounds.GetHeadPosition();
  bool found = false;

  while ((!found) && (pos != NULL))
  {
    if (m_data.sounds.GetAt(pos) == name)
    {
      found = true;
      m_data.sounds.RemoveAt(pos);
    }

    if (!found)
      m_data.sounds.GetNext(pos);
  }
  */
}

bool CSoundEventDlg::FindSound(CString &sound)
{
  POSITION pos = m_data.sounds.GetHeadPosition();
  while (pos != NULL)
  {
    if (m_data.sounds.GetNext(pos) == sound)
      return true;
  }
  return false;
}

void CSoundEventDlg::OnOK() 
{
  m_data.sounds.RemoveAll();
  for (int i=0;i<m_List.GetCount();i++)
  {
    CString temp;
    m_List.GetText(i, temp);
    m_data.sounds.AddTail(temp);
  }
	CDialog::OnOK();
}
