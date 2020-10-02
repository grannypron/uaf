/******************************************************************************
* Filename: PreviewFileDlg.h
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
#if !defined(AFX_PREVIEWFILEDLG_H__1D054314_0872_11D2_8A46_0000E81D3D27__INCLUDED_)
#define AFX_PREVIEWFILEDLG_H__1D054314_0872_11D2_8A46_0000E81D3D27__INCLUDED_


/////////////////////////////////////////////////////////////////////////////
// CPreviewFileDlg dialog

#include "DlgPicture.h"

class CPreviewFileDlg : public CFileDialog
{
	DECLARE_DYNAMIC(CPreviewFileDlg)

public:
	CPreviewFileDlg(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		              LPCTSTR lpszDefExt = NULL,
		              LPCTSTR lpszFileName = NULL,
		              DWORD dwFlags = (OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST),
		              LPCTSTR lpszFilter = NULL,
		              CWnd* pParentWnd = NULL);
// Attributes
public:
  int m_width;
  int m_height;
  BOOL m_bPreview;
  //BOOL m_MultiSelect;
  CDlgPicture m_DIBStaticCtrl;

protected:
	//{{AFX_MSG(CPreviewFileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPreview();
	//}}AFX_MSG
	virtual void OnFileNameChange();
   void SetPicStats(void);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEWFILEDLG_H__1D054314_0872_11D2_8A46_0000E81D3D27__INCLUDED_)
