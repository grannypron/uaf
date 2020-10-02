/******************************************************************************
* Filename: Traindlg.h
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
// CTrainingHall dialog

#include "SortableListCtrl.h"

struct BASECLASSDEF
{
  BASECLASS_ID baseclassID;
  int minLevel;
  int maxLevel;
  BOOL present;
  CString whateverelsethosenutswant; //Can be encoded here
};

class CTrainingHall : public CDialog
{
// Construction
public:
  CTrainingHall(TRAININGHALL& halls, CWnd* pParent = NULL);   // standard constructor
	void GetHallData(TRAININGHALL& halls); 
  void CTrainingHall::AddToBaseclassList(int availIndex);

// Dialog Data
	//{{AFX_DATA(CTrainingHall)
	enum { IDD = IDD_TRAININGHALL };
	CButton	m_ChooseTrainer;
  CListCtrl m_BaseclassList;
	BOOL	m_ForceExit;
	//BOOL	m_Clerics;
	//BOOL	m_Druids;
	//BOOL	m_Fighters;
	//BOOL	m_MagicUsers;
	//BOOL	m_Paladins;
	//BOOL	m_Rangers;
	CString	m_Text;
	BOOL	m_Thiefs;
	int		m_Cost;
	CString	m_CoinName;
  CArray<BASECLASSDEF, BASECLASSDEF&> m_availBaseclasses;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrainingHall)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	TRAININGHALL m_halls;

	// Generated message map functions
	//{{AFX_MSG(CTrainingHall)
	afx_msg void OnTrainer();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
  virtual void OnEdit();
  virtual void OnAdd();
  virtual void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CTrainable : public CDialog
{
// Construction
public:
	CTrainable(CArray<BASECLASSDEF, BASECLASSDEF&> *pAvailBaseclass, CWnd* pParent = NULL);   // standard constructor
	//void GetHallData(TRAININGHALL& halls); 

// Dialog Data
	//{{AFX_DATA(CTrainingHall)
	enum { IDD = IDD_EditTrainable };
	CComboBox	m_baseclassName;
	int       m_minLevel;
	int		    m_maxLevel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrainingHall)

  BASECLASSDEF result;


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
  CArray<BASECLASSDEF, BASECLASSDEF&> *pAvailBaseclass;
protected:



	// Generated message map functions
	//{{AFX_MSG(CTrainingHall)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	
  DECLARE_MESSAGE_MAP()
};
