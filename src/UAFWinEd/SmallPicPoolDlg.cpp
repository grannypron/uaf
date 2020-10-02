/******************************************************************************
* Filename: SmallPicPoolDlg.cpp
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
#include "PicData.h"
#include "SmallPicDlg.h"
#include "SmallPicPoolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmallPicPoolDlg dialog


CSmallPicPoolDlg::CSmallPicPoolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSmallPicPoolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSmallPicPoolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSmallPicPoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSmallPicPoolDlg)
	DDX_Control(pDX, IDC_DELETEPIC, m_DeletePic);
	DDX_Control(pDX, IDC_VIEWPIC, m_ViewPic);
	DDX_Control(pDX, IDC_PICLIST, m_PicList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSmallPicPoolDlg, CDialog)
	//{{AFX_MSG_MAP(CSmallPicPoolDlg)
	ON_BN_CLICKED(IDC_LOADPIC, OnLoadpic)
	ON_BN_CLICKED(IDC_VIEWPIC, OnViewpic)
	ON_LBN_SETFOCUS(IDC_PICLIST, OnSetfocusPiclist)
	ON_BN_CLICKED(IDC_DELETEPIC, OnDeletepic)
	ON_LBN_DBLCLK(IDC_PICLIST, OnDblclkPiclist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmallPicPoolDlg message handlers

BOOL CSmallPicPoolDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
  ResetPicList();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSmallPicPoolDlg::ResetPicList(void)
{
  m_PicList.ResetContent();
  POSITION pos = global_SmallPicImport.GetHeadPosition();
  CString temp;
  int count = 0;
  int lmax = 0;

  while (pos != NULL)
  {
    temp = global_SmallPicImport.GetNext(pos)->filename;

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

void CSmallPicPoolDlg::OnLoadpic() 
{
  PIC_DATA data;
  data.Clear();

  CSmallPicDlg dlg(data);

  DWORD result = dlg.DoModal();

  if (result == IDOK)
  {
    SET_MODIFIED;
    data = dlg.m_SmallPic;
    data.picType = SmallPicDib;

    if (strlen(data.filename))
    {
       POSITION pos = global_SmallPicImport.GetHeadPosition();
       BOOL found = FALSE;

       while (pos != NULL)
       {
          if (*global_SmallPicImport.GetNext(pos) == data)
            found = TRUE;
       }

       if (!found)
       {
          global_SmallPicImport.AddTail(data);
          ResetPicList();
       }
    }
  }
}

void CSmallPicPoolDlg::OnViewpic() 
{
  int sel = m_PicList.GetCurSel();
  
  if (sel != LB_ERR)
  {
    CString pic;
    m_PicList.GetText(sel, pic);
    int index = sel;

    if (index != LB_ERR)
    {
      POSITION pos = global_SmallPicImport.FindIndex(index);

      if (pos != NULL)
      {
        PIC_DATA data = *global_SmallPicImport.GetAt(pos);

        CSmallPicDlg dlg(data);

        DWORD result = dlg.DoModal();

        if (result == IDOK)
        {
          data = dlg.m_SmallPic;
          data.picType = SmallPicDib;
          global_SmallPicImport.SetAt(pos, data);
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

void CSmallPicPoolDlg::OnSetfocusPiclist() 
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

void CSmallPicPoolDlg::OnDeletepic() 
{
  int sel = m_PicList.GetCurSel();
  
  if (sel != LB_ERR)
  {
    SET_MODIFIED;
    int index = sel;

    if (index != LB_ERR)
    {
      POSITION pos = global_SmallPicImport.FindIndex(index);
      if (pos != NULL)
      {
        global_SmallPicImport.RemoveAt(pos);
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

void CSmallPicPoolDlg::OnDblclkPiclist() 
{
  OnViewpic();
}
