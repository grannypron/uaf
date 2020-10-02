/******************************************************************************
* Filename: SelectPicDlg.h
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

/////////////////////////////////////////////////////////////////////////////
// CSelectPicDlg dialog

class CSelectPicDlg : public CDialog
{
// Construction
public:
	CSelectPicDlg(PIC_DATA& data, DWORD flags, CWnd* pParent = NULL);   // standard constructor
   void GetData(PIC_DATA& data);

// Dialog Data
	//{{AFX_DATA(CSelectPicDlg)
	enum { IDD = IDD_PICSELECTDLG };
	CButton	m_LoadPic;
	int		m_PicType;
	CString	m_Filename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectPicDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   PIC_DATA m_pic;
   DWORD m_flags;

   void LoadBigPic(void);
   void LoadSmallPic(void);
   void LoadSpritePic(void);
   void SetPicType(DWORD type, BOOL SetDefaults);


	// Generated message map functions
	//{{AFX_MSG(CSelectPicDlg)
	afx_msg void OnBigpic();
	afx_msg void OnLoadpic();
	afx_msg void OnNopic();
	afx_msg void OnSmallpic();
	afx_msg void OnSpritepic();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
