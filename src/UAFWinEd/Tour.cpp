/******************************************************************************
* Filename: Tour.cpp
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
#include "globtext.h"
#include "GlobalData.h"
#include "GameEvent.h"
#include "Help.h"

#include "Tour.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Tour dialog


Tour::Tour(GUIDED_TOUR& tour, CWnd* pParent /*=NULL*/)
	: CDialog(Tour::IDD, pParent)
{
	//{{AFX_DATA_INIT(Tour)
	m_Col = 0;
	m_Row = 0;
	m_UseStartLoc = FALSE;
	m_Text = _T("");
	m_StepNum = 1;
	m_ExecuteEvent = FALSE;
	m_LevelSize = _T("");
	//}}AFX_DATA_INIT

	m_tour = tour;
}


void Tour::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Tour)
	DDX_Control(pDX, IDC_SPINSTEP, m_SpinStep);
	DDX_Control(pDX, IDC_STEPTAKEN, m_StepTaken);
	DDX_Control(pDX, IDC_FACING, m_Facing);
	DDX_Text(pDX, IDC_COL, m_Col);
	DDX_Text(pDX, IDC_ROW, m_Row);
	DDX_Check(pDX, IDC_STARTLOC, m_UseStartLoc);
	DDX_Text(pDX, IDC_STEPTEXT, m_Text);
	DDX_Text(pDX, IDC_TOURSTEP, m_StepNum);
	DDX_Check(pDX, IDC_EXECUTEEVENT, m_ExecuteEvent);
	DDX_Text(pDX, IDC_LEVELSIZE, m_LevelSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Tour, CDialog)
	//{{AFX_MSG_MAP(Tour)
	ON_CBN_SELCHANGE(IDC_FACING, OnSelchangeFacing)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINSTEP, OnDeltaposSpinstep)
	ON_CBN_SELCHANGE(IDC_STEPTAKEN, OnSelchangeSteptaken)
  ON_BN_CLICKED(IDC_GUIDED_TOUR, OnGuidedTourQuestion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// enum eventStepType { TStep_NoMove, TStep_Pause, TStep_Forward, TStep_Left, TStep_Right };


/////////////////////////////////////////////////////////////////////////////
// Tour message handlers

void Tour::OnSelchangeFacing() 
{
	m_tour.facing =
		(eventFacingType) m_Facing.GetCurSel();
}

void Tour::OnDeltaposSpinstep(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int curr, next;

	curr = pNMUpDown->iPos-1;
	next = curr + pNMUpDown->iDelta;

	if (next < 0)
		next = MAX_TOUR_STEPS-1;

	if (next >= MAX_TOUR_STEPS)
		next = 0;

	UpdateData(TRUE);

	m_tour.steps[curr].step = (eventStepType)m_StepTaken.GetCurSel();
	m_tour.steps[curr].text = m_Text;

	m_StepNum = next+1;

	m_StepTaken.SetCurSel((int)m_tour.steps[next].step);
	m_Text = m_tour.steps[next].text;

	UpdateData(FALSE);
	
	*pResult = 0;
}

void Tour::OnSelchangeSteptaken() 
{
	m_StepNum = m_SpinStep.GetPos();

	m_tour.steps[m_StepNum-1].step =
		(eventStepType) m_StepTaken.GetCurSel();

  if (m_tour.steps[m_StepNum-1].step == TStep_NoMove)
  {
    m_tour.steps[m_StepNum-1].text = "";
    m_Text = "";
    UpdateData(FALSE);
  }
}

void Tour::OnOK() 
{
	int num = m_SpinStep.GetPos()-1;
	UpdateData(TRUE);

	m_tour.facing = (eventFacingType)m_Facing.GetCurSel();
  m_tour.executeEvent = m_ExecuteEvent;
  
  m_Col = min(CurrLevelWidth(), m_Col);
  m_Col = max(0, m_Col);
  m_Row = min(CurrLevelHeight(), m_Row);
  m_Row = max(0, m_Row);

	m_tour.tour_x = m_Col;
	m_tour.tour_y = m_Row;
	m_tour.useStartLoc = m_UseStartLoc;

	m_tour.steps[num].step = (eventStepType)m_StepTaken.GetCurSel();
	m_tour.steps[num].text = m_Text;
	

	CDialog::OnOK();
}

BOOL Tour::OnInitDialog() 
{
	int i;
	CDialog::OnInitDialog();
	
	m_SpinStep.SetRange(1,MAX_TOUR_STEPS);
	m_SpinStep.SetPos(1);

	m_Facing.ResetContent();
	m_StepTaken.ResetContent();

	for (i=0;i<NUM_STEP_TYPES;i++)
		m_StepTaken.AddString(StepText[i]);

	for (i=0;i<NUM_FACING_TYPES;i++)
		m_Facing.AddString(FacingText[i]);
		
	int num = 0;
	m_StepNum = num+1;
	m_Facing.SetCurSel((int)m_tour.facing);
	m_StepTaken.SetCurSel((int)m_tour.steps[num].step);
	m_Col = m_tour.tour_x;
	m_Row = m_tour.tour_y;

  m_Col = min(CurrLevelWidth(), m_Col);
  m_Col = max(0, m_Col);
  m_Row = min(CurrLevelHeight(), m_Row);
  m_Row = max(0, m_Row);

	m_UseStartLoc = m_tour.useStartLoc;
	m_Text = m_tour.steps[num].text;
	m_ExecuteEvent = m_tour.executeEvent;

  m_LevelSize.Format("Current level is %u x %u",
                     CurrLevelWidth(),
                     CurrLevelHeight());
  GotoDlgCtrl(GetDlgItem(IDC_STEPTEXT));

  UpdateData(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Tour::GetTourData(GUIDED_TOUR& tour)
{
	tour = m_tour;
}

void Tour::OnGuidedTourQuestion()
{
  CHelp dlg("GUIDED_TOUR");
  dlg.DoModal();
}
