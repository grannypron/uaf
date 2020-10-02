#if !defined(AFX_PLAYMOVIEDLG_H__DE200A89_49C3_4ACE_A0FA_E454AD02C5AB__INCLUDED_)
#define AFX_PLAYMOVIEDLG_H__DE200A89_49C3_4ACE_A0FA_E454AD02C5AB__INCLUDED_
/******************************************************************************
* Filename: QListOptions.h
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
// PlayMovieDlg dialog

class PlayMovieDlg : public CDialog
{
// Construction
public:
	PlayMovieDlg(PLAY_MOVIE_DATA data, BOOL FullOnly=FALSE, CWnd* pParent = NULL);   // standard constructor
  void GetData(PLAY_MOVIE_DATA& data) { data=m_data; }

// Dialog Data
	//{{AFX_DATA(PlayMovieDlg)
	enum { IDD = IDD_PLAYMOVIEFULLSCREEN };
	CButton	m_ChooseMovie;
	int		m_PlaybackSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PlayMovieDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  PLAY_MOVIE_DATA m_data;
  BOOL m_FullOnly;

	// Generated message map functions
	//{{AFX_MSG(PlayMovieDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTestmovie();
	afx_msg void OnChoosemovie();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYMOVIEDLG_H__DE200A89_49C3_4ACE_A0FA_E454AD02C5AB__INCLUDED_)
