/******************************************************************************
* Filename: IconPicPoolDlg.cpp
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
#include "IconPicDlg.h"
#include "IconPicPoolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconPicPoolDlg dialog


CIconPicPoolDlg::CIconPicPoolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIconPicPoolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIconPicPoolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CIconPicPoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIconPicPoolDlg)
	DDX_Control(pDX, IDC_PICLIST, m_PicList);
	DDX_Control(pDX, IDC_VIEWPIC, m_ViewPic);
	DDX_Control(pDX, IDC_DELETEPIC, m_DeletePic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIconPicPoolDlg, CDialog)
	//{{AFX_MSG_MAP(CIconPicPoolDlg)
	ON_BN_CLICKED(IDC_DELETEPIC, OnDeletepic)
	ON_BN_CLICKED(IDC_LOADPIC, OnLoadpic)
	ON_LBN_SETFOCUS(IDC_PICLIST, OnSetfocusPiclist)
	ON_BN_CLICKED(IDC_VIEWPIC, OnViewpic)
	ON_LBN_DBLCLK(IDC_PICLIST, OnDblclkPiclist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconPicPoolDlg message handlers

BOOL CIconPicPoolDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
  ResetPicList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIconPicPoolDlg::ResetPicList(void)
{
  m_PicList.ResetContent();
  POSITION pos = global_IconPicImport.GetHeadPosition();
  CString temp;
  int count = 0;
  int lmax = 0;

  while (pos != NULL)
  {
    temp = global_IconPicImport.GetNext(pos)->filename;
    int len = temp.GetLength();

    if (len > lmax)
      lmax = len;

    if (len > 0)
    {
       m_PicList.AddString(temp);
       count++;
    }
  }
	
  if (count)
  {
    SIZE size;
    CDC *cdc;
    cdc = GetDC();

    GetTextExtentPoint32(cdc->m_hDC, "W", 1, &size);
    m_PicList.SetHorizontalExtent(size.cx*lmax);

    m_PicList.SetCurSel(0);
    m_ViewPic.EnableWindow();
    m_DeletePic.EnableWindow();
  }
  else
  {
    m_PicList.SetHorizontalExtent(0);
    m_ViewPic.EnableWindow(FALSE);
    m_DeletePic.EnableWindow(FALSE);
  }

  UpdateData(FALSE);
}

void CIconPicPoolDlg::OnDeletepic() 
{
  int sel = m_PicList.GetCurSel();
  
  if (sel != LB_ERR)
  {
    SET_MODIFIED;
    int index = sel;

    if (index != LB_ERR)
    {
      POSITION pos = global_IconPicImport.FindIndex(index);

      if (pos != NULL)
      {
        global_IconPicImport.RemoveAt(pos);
        ResetPicList();
      }
      else
        TRACE("NULL pos from FindIndex()\n");
    }
    else
      TRACE("LB_ERR from GetItemData()\n");
  }
  else
    TRACE("LB_ERR from GetCurSel()\n");
}

void CIconPicPoolDlg::OnLoadpic() 
{
  PIC_DATA data;
  data.picType=IconDib;
  data.filename="";
  data.SetDefaults();
  CIconPicDlg dlg(data);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    dlg.GetData(data);

     SET_MODIFIED;
     POSITION pos = global_IconPicImport.GetHeadPosition();
     BOOL found = FALSE;
     while (pos != NULL)
     {
        if (*global_IconPicImport.GetNext(pos) == data)
          found = TRUE;
     }

     if (!found)
     {
        global_IconPicImport.AddTail(data);
        ResetPicList();
     }
  }
}

void CIconPicPoolDlg::OnSetfocusPiclist() 
{
  if ((m_PicList.GetCount() > 0) && (m_PicList.GetCurSel() != LB_ERR))
  {
    m_ViewPic.EnableWindow(TRUE);
    m_DeletePic.EnableWindow(TRUE);
  }
  else
  {
    m_ViewPic.EnableWindow(FALSE);
    m_DeletePic.EnableWindow(FALSE);
  }
}

void CIconPicPoolDlg::OnViewpic() 
{
  int sel = m_PicList.GetCurSel();
  
  if (sel != LB_ERR)
  {
    CString pic;
    m_PicList.GetText(sel, pic);
    int index = sel;

    if (index != LB_ERR)
    {
      POSITION pos = global_IconPicImport.FindIndex(index);

      if (pos != NULL)
      {      
        CIconPicDlg dlg(*global_IconPicImport.GetAt(pos));

        DWORD result = dlg.DoModal();
        if (result == IDOK)
        {
          PIC_DATA tmp;
          dlg.GetData(tmp);
          global_IconPicImport.SetAt(pos, tmp);
          ResetPicList();
        }
      }
      else
        TRACE("NULL pos from FindIndex()\n");
    }
    else
      TRACE("LB_ERR from GetItemData()\n");
  }
  else
    TRACE("LB_ERR from GetCurSel()\n");
}

void CIconPicPoolDlg::OnDblclkPiclist() 
{
  OnViewpic();
}
