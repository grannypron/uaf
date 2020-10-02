/******************************************************************************
* Filename: UAFWinEdDoc.h
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

#if !defined(AFX_UAFWINEDDOC_H__56306A0D_E92E_11D3_891D_00105AA4563A__INCLUDED_)
#define AFX_UAFWINEDDOC_H__56306A0D_E92E_11D3_891D_00105AA4563A__INCLUDED_


class CUAFWinEdDoc : public CDocument
{
protected: // create from serialization only
	CUAFWinEdDoc();
	DECLARE_DYNCREATE(CUAFWinEdDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUAFWinEdDoc)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUAFWinEdDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CUAFWinEdDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UAFWINEDDOC_H__56306A0D_E92E_11D3_891D_00105AA4563A__INCLUDED_)
