/******************************************************************************
* Filename: TitleScreenData.cpp
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

#include "TitleScreenConfig.h"
#include "TitleScreenData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitleScreenData dialog


CTitleScreenData::CTitleScreenData(TITLE_SCREEN_DATA &data, CWnd* pParent /*=NULL*/)
	: CDialog(CTitleScreenData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTitleScreenData)
	m_Delay = 0;
	//}}AFX_DATA_INIT
  m_data = data;
}


void CTitleScreenData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTitleScreenData)
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	DDX_Text(pDX, IDC_TITLEDELAY, m_Delay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTitleScreenData, CDialog)
	//{{AFX_MSG_MAP(CTitleScreenData)
	ON_BN_CLICKED(IDC_ADDTITLE, OnAddtitle)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_BN_CLICKED(IDC_DELETETITLE, OnDeletetitle)
	ON_BN_CLICKED(IDC_EDITTITLE, OnEdittitle)
	ON_BN_CLICKED(IDC_MOVETITLEDOWN, OnMovetitledown)
	ON_BN_CLICKED(IDC_MOVETITLEUP, OnMovetitleup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTitleScreenData message handlers

void CTitleScreenData::OnAddtitle() 
{
  TITLE_SCREEN data;
  CTitleScreenConfig dlg(data);

  if (dlg.DoModal()==IDOK)
  {
    dlg.GetData(data);
    POSITION pos = m_data.AddTitle(data);
    AddToAvailList(data, pos);
    SET_MODIFIED;
  }	
}

void CTitleScreenData::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdittitle();	
	*pResult = 0;
}

void CTitleScreenData::OnDeletetitle() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    SET_MODIFIED;
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    POSITION apos = (POSITION)m_AvailList.GetItemData(nItem);    

    m_data.Titles.RemoveAt(apos);    
    m_AvailList.DeleteAllItems();

    int a_count = 0;
    pos = m_data.Titles.GetHeadPosition();
    while (pos != NULL)
    {
      AddToAvailList(m_data.Titles.GetAt(pos), pos);
      m_data.Titles.GetNext(pos);
      a_count++;
    }

    if (a_count > 0)
    {
      //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_AvailList.EnsureVisible(0, FALSE);
      m_AvailList.SelectItem(0, TRUE);
    }	
  }		
}

void CTitleScreenData::OnEdittitle() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    POSITION pos2 = (POSITION)m_AvailList.GetItemData(nItem);

    TITLE_SCREEN data = m_data.Titles.GetAt(pos2);
    CTitleScreenConfig dlg(data);

    if (dlg.DoModal()==IDOK)
    {
      dlg.GetData(data);
      m_data.Titles.SetAt(pos2, data);
      UpdateAvailList(nItem, data, pos2);
      SET_MODIFIED;
    }	
  }
}

BOOL CTitleScreenData::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Filename";
	lvc.cx = 200;
	lvc.fmt = LVCFMT_LEFT;
	m_AvailList.InsertColumn(0,&lvc, CListSort::dtSTRING, false);

	lvc.iSubItem = 1;
	lvc.pszText = "Use Transparency";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(1,&lvc, CListSort::dtSTRING, false);

	lvc.iSubItem = 2;
	lvc.pszText = "Blend With Prev";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(2,&lvc, CListSort::dtSTRING, false);

	lvc.iSubItem = 3;
	lvc.pszText = "Display Style";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(3,&lvc, CListSort::dtSTRING, false);

	m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  int a_count = 0;
  POSITION pos = m_data.Titles.GetHeadPosition();
  while (pos != NULL)
  {
    AddToAvailList(m_data.Titles.GetAt(pos), pos);
    m_data.Titles.GetNext(pos);
    a_count++;
  }

  if (a_count > 0)
  {
    //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_AvailList.SelectItem(0, TRUE);
    m_AvailList.SelectItem(0, TRUE);
  }		

  m_Delay = m_data.Timeout/1000;
  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTitleScreenData::OnMovetitledown() 
{
  POSITION pos1 = m_AvailList.GetFirstSelectedItemPosition();
  if (pos1 != NULL)
  {
    SET_MODIFIED;
    int nItem = m_AvailList.GetNextSelectedItem(pos1);
    POSITION pos = (POSITION)m_AvailList.GetItemData(nItem);    
    if (pos == NULL) return;

    if (pos == m_data.Titles.GetTailPosition())
      return; // already in last position

    POSITION nextPos = pos;
    TITLE_SCREEN data = m_data.Titles.GetNext(nextPos);
    
    if (nextPos!=NULL)
    {
      m_data.Titles.RemoveAt(pos);
      m_data.Titles.InsertAfter(nextPos, data);
    }
    m_AvailList.DeleteAllItems();

    int a_count = 0;
    pos = m_data.Titles.GetHeadPosition();
    while (pos != NULL)
    {
      AddToAvailList(m_data.Titles.GetAt(pos), pos);
      m_data.Titles.GetNext(pos);
      a_count++;
    }

    if (a_count > 0)
    {
      //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_AvailList.EnsureVisible(0, FALSE);
      m_AvailList.SelectItem(0, TRUE);
    }	
  }		
}

void CTitleScreenData::OnMovetitleup() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    SET_MODIFIED;
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    pos = (POSITION)m_AvailList.GetItemData(nItem);    
    if (pos == NULL) return;

    if (pos == m_data.Titles.GetHeadPosition())
      return; // already in first position

    POSITION prevPos = pos;
    TITLE_SCREEN data = m_data.Titles.GetPrev(prevPos);
    
    if (prevPos!=NULL)
    {
      m_data.Titles.RemoveAt(pos);
      m_data.Titles.InsertBefore(prevPos, data);
    }
    m_AvailList.DeleteAllItems();

    int a_count = 0;
    pos = m_data.Titles.GetHeadPosition();
    while (pos != NULL)
    {
      AddToAvailList(m_data.Titles.GetAt(pos), pos);
      m_data.Titles.GetNext(pos);
      a_count++;
    }

    if (a_count > 0)
    {
      //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_AvailList.EnsureVisible(0, FALSE);
      m_AvailList.SelectItem(0, TRUE);
    }	
  }	
}

void CTitleScreenData::AddToAvailList(TITLE_SCREEN &data, POSITION pos)
{
  int listIdx = m_AvailList.GetItemCount();

  char name[_MAX_PATH+1];
  char trans[5];
  char blend[5];
  char style[20];

  getBaseName(data.TitleBgArt, name, _MAX_PATH, TRUE);
  //strncpy(name, data.TitleBgArt, _MAX_PATH);
  //name[_MAX_PATH]='\0';
  if (data.UseBlend)
    strcpy(blend, "Yes");
  else
    strcpy(blend, "No");
  if (data.UseTrans)
    strcpy(trans, "Yes");
  else
    strcpy(trans, "No");
    //enum { tsSplash=0, tsFadeIn=1, tsMovie=2 };

  switch (data.DisplayBy)
  {
  case TITLE_SCREEN::tsSplash:
    strcpy(style, "Normal");
    break;
  case TITLE_SCREEN::tsFadeIn:
    strcpy(style, "Fade In");
    break;
  case TITLE_SCREEN::tsMovie:
    strcpy(style, "Movie");
    break;
  default:
    strcpy(style, "ERROR");
  }

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = name;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = (DWORD)pos;

  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemText(listIdx, 1, trans); 
  m_AvailList.SetItemText(listIdx, 2, blend); 
  m_AvailList.SetItemText(listIdx, 3, style); 
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}

void CTitleScreenData::UpdateAvailList(int listIdx, TITLE_SCREEN &data, POSITION pos)
{
  char name[_MAX_PATH+1];
  char trans[5];
  char blend[5];
  char style[20];

  getBaseName(data.TitleBgArt, name, _MAX_PATH, TRUE);
  //strncpy(name, data.TitleBgArt, _MAX_PATH);
  name[_MAX_PATH]='\0';
  if (data.UseBlend)
    strcpy(blend, "Yes");
  else
    strcpy(blend, "No");
  if (data.UseTrans)
    strcpy(trans, "Yes");
  else
    strcpy(trans, "No");
    enum { tsSplash=0, tsFadeIn=1, tsMovie=2 };

  switch (data.DisplayBy)
  {
  case TITLE_SCREEN::tsSplash:
    strcpy(style, "Normal");
    break;
  case TITLE_SCREEN::tsFadeIn:
    strcpy(style, "Fade In");
    break;
  case TITLE_SCREEN::tsMovie:
    strcpy(style, "Movie");
    break;
  }
  m_AvailList.SetItemText(listIdx, 0, name); 
  m_AvailList.SetItemText(listIdx, 1, trans); 
  m_AvailList.SetItemText(listIdx, 2, blend); 
  m_AvailList.SetItemText(listIdx, 3, style); 
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}



void CTitleScreenData::OnOK() 
{
  UpdateData(TRUE);
  m_data.Timeout = m_Delay*1000;	
	CDialog::OnOK();
}
