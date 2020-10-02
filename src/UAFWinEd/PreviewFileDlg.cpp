/******************************************************************************
* Filename: PreviewFileDlg.cpp
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
#include <sys/types.h> 
#include <sys/stat.h>

#include "UAFWinEd.h"
#include "DlgPicture.h"
#include "PreviewFileDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL IsAFolder(LPCSTR fname)
{
  struct _tstat st;
  if (_tstat(fname, &st) == 0)
  {
    if (st.st_mode & _S_IFDIR)
      return TRUE;
  }
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPreviewFileDlg

IMPLEMENT_DYNAMIC(CPreviewFileDlg, CFileDialog)

CPreviewFileDlg::CPreviewFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
                                 DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
  m_ofn.Flags |= OFN_ENABLETEMPLATE;
  m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_FILEOPENPREVIEW);
  //m_MultiSelect = m_ofn.Flags & OFN_ALLOWMULTISELECT;
  m_bPreview = TRUE;
  m_width = 0;
  m_height = 0;
  //m_bVistaStyle = FALSE;
}


BEGIN_MESSAGE_MAP(CPreviewFileDlg, CFileDialog)
//{{AFX_MSG_MAP(CPreviewFileDlg)
ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPreviewFileDlg::OnInitDialog() 
{
  CFileDialog::OnInitDialog();
  
  if ((m_width > 0) && (m_height > 0))
  {
    CString title, temp;
    GetWindowText(title);
    temp.Format(": Target Pic Size -> %i x %i",
                m_width, m_height);
    title += temp;
    SetWindowText(title);
  }
  
  m_DIBStaticCtrl.SubclassDlgItem(IDC_IMAGE, this);
  //SetPicStats();
  CWnd *pWnd = GetDlgItem(IDC_PREVIEW);
  
  if (pWnd != NULL)
    pWnd->PostMessage(BM_SETCHECK, TRUE);
  
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreviewFileDlg::OnFileNameChange() 
{
  //UpdateData(TRUE);
  //OnInitDialog();


  if (m_DIBStaticCtrl.m_hWnd == NULL)
  {
    m_DIBStaticCtrl.SubclassDlgItem(IDC_IMAGE, this);
  };

  if (m_bPreview)
  {
    // ignore folder selection
    CString file = GetFileName();
    
    if (!file.IsEmpty())
    {
      file = GetPathName();

      if ((FileExists(file)) && (!IsAFolder(file)))
      {
        m_DIBStaticCtrl.LoadPicture(file, BufferDib, "", ""); // the control will handle errors
        SetPicStats();
      }
    }
  }

  CFileDialog::OnFileNameChange();
}

void CPreviewFileDlg::OnPreview() 
{
  //UpdateData(TRUE);
  
  m_bPreview = !m_bPreview;
  m_DIBStaticCtrl.ErasePicture();
  
  if (m_bPreview)
  {    
    // ignore folder selection
    CString file = GetFileName();
    
    if (!file.IsEmpty())
    {
      file = GetPathName();
      
      if ((FileExists(file)) && (!IsAFolder(file)))
      {
        m_DIBStaticCtrl.LoadPicture(file, BufferDib, "", ""); // the control will handle errors
        SetPicStats();
      }
    }
  }
  //UpdateData(FALSE);
}

void CPreviewFileDlg::SetPicStats(void)
{
  int w, h, bits;  
  m_DIBStaticCtrl.GetStats(w,h,bits);
  
  char temp[25];
  sprintf(temp, "%i", w);
  CWnd *pWnd;
  pWnd = GetDlgItem(IDC_PICWIDTH);
  if (pWnd != NULL)
    pWnd->SendMessage(WM_SETTEXT, 0, (LPARAM)(LPCSTR)temp);
  
  sprintf(temp, "%i", h);
  pWnd = GetDlgItem(IDC_PICHEIGHT);
  if (pWnd != NULL)
    pWnd->SendMessage(WM_SETTEXT, 0, (LPARAM)(LPCSTR)temp);

  //sprintf(temp, "%i", bits);   
  //GetDlgItem(IDC_BITDEPTH)->SendMessage(WM_SETTEXT, 0, (LPARAM)(LPCSTR)temp);
}
