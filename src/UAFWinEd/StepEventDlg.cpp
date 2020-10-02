/******************************************************************************
* Filename: StepEventDlg.cpp
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
#include "items.h"
#include "Char.h"
#include "StepEventDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStepEventDlg dialog


CStepEventDlg::CStepEventDlg(LEVEL &data, int curr, CWnd* pParent /*=NULL*/)
	: CDialog(CStepEventDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStepEventDlg)
	m_StepCount = 0;
	m_StepEventNum = 1;
	m_StepEventName = _T("");
	m_Zone1 = FALSE;
	m_Zone2 = FALSE;
	m_Zone3 = FALSE;
	m_Zone4 = FALSE;
	m_Zone5 = FALSE;
	m_Zone6 = FALSE;
	m_Zone7 = FALSE;
	m_Zone8 = FALSE;
	m_Zone10 = FALSE;
	m_Zone11 = FALSE;
	m_Zone12 = FALSE;
	m_Zone13 = FALSE;
	m_Zone14 = FALSE;
	m_Zone15 = FALSE;
	m_Zone16 = FALSE;
	m_Zone9 = FALSE;
	//}}AFX_DATA_INIT

  for (int i=0; i<MAX_STEP_EVENTS;i++)
    m_StepEvents[i] = data.stepEvents[i];

  if ((curr <= 0) || (curr > MAX_STEP_EVENTS))
    curr = 1;

  m_StepEventNum = curr;
}

void CStepEventDlg::GetData(LEVEL &data)
{
  for (int i=0; i<MAX_STEP_EVENTS;i++)
    data.stepEvents[i] = m_StepEvents[i];
}

void CStepEventDlg::DoDataExchange(CDataExchange* pDX)
{
   if (!pDX->m_bSaveAndValidate)
   {
    //m_StepEventName.Format("Step Event %i", m_StepEventNum);
      m_StepEventName = m_StepEvents[m_StepEventNum-1].name;
      m_StepCount = m_StepEvents[m_StepEventNum-1].stepCount;
      m_Zone1  = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<0))  != 0; //stepTrigger[0];
      m_Zone2  = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<1))  != 0; //stepTrigger[1];
      m_Zone3  = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<2))  != 0; //stepTrigger[2];
      m_Zone4  = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<3))  != 0; //stepTrigger[3];
      m_Zone5  = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<4))  != 0; //stepTrigger[4];
      m_Zone6  = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<5))  != 0; //stepTrigger[5];
      m_Zone7  = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<6))  != 0; //stepTrigger[6];
      m_Zone8  = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<7))  != 0; //stepTrigger[7];     
      m_Zone9  = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<8))  != 0; //stepTrigger[8];
      m_Zone10 = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<9))  != 0; //stepTrigger[9];
      m_Zone11 = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<10)) != 0; //stepTrigger[10];
      m_Zone12 = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<11)) != 0; //stepTrigger[11];
      m_Zone13 = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<12)) != 0; //stepTrigger[12];
      m_Zone14 = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<13)) != 0; //stepTrigger[13];
      m_Zone15 = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<14)) != 0; //stepTrigger[14];
      m_Zone16 = (m_StepEvents[m_StepEventNum-1].zoneMask & (1<<15)) != 0; //stepTrigger[15];
   }

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStepEventDlg)
	DDX_Control(pDX, IDC_STEPEVENTSPIN, m_StepEventSpin);
	DDX_Text(pDX, IDC_STEPCOUNT, m_StepCount);
	DDX_Text(pDX, IDC_STEPEVENT_NUM, m_StepEventNum);
	DDX_Text(pDX, IDC_StepEventName, m_StepEventName);
	DDX_Check(pDX, IDC_ZONE1, m_Zone1);
	DDX_Check(pDX, IDC_ZONE2, m_Zone2);
	DDX_Check(pDX, IDC_ZONE3, m_Zone3);
	DDX_Check(pDX, IDC_ZONE4, m_Zone4);
	DDX_Check(pDX, IDC_ZONE5, m_Zone5);
	DDX_Check(pDX, IDC_ZONE6, m_Zone6);
	DDX_Check(pDX, IDC_ZONE7, m_Zone7);
	DDX_Check(pDX, IDC_ZONE8, m_Zone8);
	DDX_Check(pDX, IDC_ZONE10, m_Zone10);
	DDX_Check(pDX, IDC_ZONE11, m_Zone11);
	DDX_Check(pDX, IDC_ZONE12, m_Zone12);
	DDX_Check(pDX, IDC_ZONE13, m_Zone13);
	DDX_Check(pDX, IDC_ZONE14, m_Zone14);
	DDX_Check(pDX, IDC_ZONE15, m_Zone15);
	DDX_Check(pDX, IDC_ZONE16, m_Zone16);
	DDX_Check(pDX, IDC_ZONE9, m_Zone9);
	//}}AFX_DATA_MAP

   if (pDX->m_bSaveAndValidate)
   {
      m_StepEvents[m_StepEventNum-1].stepCount= m_StepCount;
      m_StepEvents[m_StepEventNum-1].name = m_StepEventName;
      int zoneMask = 0;
      if(m_Zone1) zoneMask |= (1 << 0);
      if(m_Zone2) zoneMask |= (1 << 1);
      if(m_Zone3) zoneMask |= (1 << 2);
      if(m_Zone4) zoneMask |= (1 << 3);
      if(m_Zone5) zoneMask |= (1 << 4);
      if(m_Zone6) zoneMask |= (1 << 5);
      if(m_Zone7) zoneMask |= (1 << 6);
      if(m_Zone8) zoneMask |= (1 << 7);
      if(m_Zone9) zoneMask |= (1 << 8);
      if(m_Zone10) zoneMask |= (1 << 9);
      if(m_Zone11) zoneMask |= (1 << 10);
      if(m_Zone12) zoneMask |= (1 << 11);
      if(m_Zone13) zoneMask |= (1 << 12);
      if(m_Zone14) zoneMask |= (1 << 13);
      if(m_Zone15) zoneMask |= (1 << 14);
      if(m_Zone16) zoneMask |= (1 << 15);
      m_StepEvents[m_StepEventNum-1].zoneMask = zoneMask;
   }
}


BEGIN_MESSAGE_MAP(CStepEventDlg, CDialog)
	//{{AFX_MSG_MAP(CStepEventDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_STEPEVENTSPIN, OnDeltaposStepeventspin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStepEventDlg message handlers

void CStepEventDlg::OnDeltaposStepeventspin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
   UpdateData(TRUE);

   m_StepEventNum = pNMUpDown->iPos + pNMUpDown->iDelta;

	if (m_StepEventNum < 1)
		m_StepEventNum = MAX_STEP_EVENTS;

	if (m_StepEventNum > MAX_STEP_EVENTS)
		m_StepEventNum = 1;

	UpdateData(FALSE);
	*pResult = 0;
}

void CStepEventDlg::OnOK() 
{
   UpdateData(TRUE);
	CDialog::OnOK();
}

BOOL CStepEventDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_StepEventSpin.SetRange(1, MAX_STEP_EVENTS);
	m_StepEventSpin.SetPos(m_StepEventNum);

   UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
