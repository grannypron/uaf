/******************************************************************************
* Filename: TeleportDlg.cpp
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

//#include "UAFWinEd.h"
#include "globtext.h"
#include "GlobalData.h"
#include "GameEvent.h"
//#include "SelectPicDlg.h"
#include "TeleportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTeleportDlg dialog


CTeleportDlg::CTeleportDlg(TRANSFER_DATA& data, BOOL AllowLevel, CWnd* pParent /*=NULL*/)
	: CDialog(CTeleportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTeleportDlg)
	m_ExecEvent = FALSE;
	m_DestX = 0;
	m_DestY = 0;
	m_XMax = _T("Max X = 12");
	m_YMax = _T("Max Y = 24");
//	m_LevelText = _T("");
	//}}AFX_DATA_INIT

//   m_LevelNum = globalData.currLevel+1;
   m_AllowLevel = AllowLevel;
   m_teleport = data;
   m_teleport.destEP++;
}


void CTeleportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTeleportDlg)
//	DDX_Control(pDX, IDC_LEVELSPIN, m_LevelSpin);
	DDX_Control(pDX, IDC_FACING, m_FacingList);
	DDX_Control(pDX, IDC_ENTRYPOINTLIST, m_EPList);
	DDX_Control(pDX, IDC_LevelSelect, m_levelSelect);
	DDX_Check(pDX, IDC_EXECUTEEVENT, m_ExecEvent);
	DDX_Text(pDX, IDC_DESTX, m_DestX);
	DDX_Text(pDX, IDC_DESTY, m_DestY);
	DDX_Text(pDX, IDC_XMAX, m_XMax);
	DDX_Text(pDX, IDC_YMAX, m_YMax);
//	DDX_Text(pDX, IDC_LEVELNUM, m_LevelText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTeleportDlg, CDialog)
	//{{AFX_MSG_MAP(CTeleportDlg)
//	ON_NOTIFY(UDN_DELTAPOS, IDC_LEVELSPIN, OnDeltaposLevelspin)
	ON_CBN_SELCHANGE(IDC_FACING, OnSelchangeFacing)
	ON_CBN_SELCHANGE(IDC_ENTRYPOINTLIST, OnSelchangeEntrypointlist)
  ON_CBN_KILLFOCUS(IDC_LevelSelect, OnLevelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTeleportDlg message handlers

void CTeleportDlg::OnOK() 
{
  int data, lev;
  char text[100];
  UpdateData(TRUE);
  
  m_levelSelect.GetWindowText(text, 99);

  {
    int i;
    lev = 0;
    i = 0;
    while((text[i]>='0') && (text[i]<='9'))
    {
      lev = 10*lev + text[i]-'0';
      i++;
    };
    if (i == 0)
    {
      MsgBoxError("The destination level specification must start with a decimal integer");
      return;
    };
  };
  m_teleport.destLevel = lev-1;



  data = m_EPList.GetItemData(m_EPList.GetCurSel());
//  m_teleport.destLevel = m_LevelNum-1;

  if (data == 9999)
  {
    m_teleport.destEP = -2;
    m_teleport.destX = 0;
    m_teleport.destY = 0;
  }
  else if (data == 9998)
  {
    m_teleport.destX = m_DestX;
    m_teleport.destY = m_DestY;
    m_teleport.destEP = -3;
  }
  else if (data == 0)
  {
    m_teleport.destX = m_DestX;
    m_teleport.destY = m_DestY;
    m_teleport.destEP = -1;
  }
  else
  {
    m_teleport.destEP = data-1;
    m_teleport.destX = 0;
    m_teleport.destY = 0;
  }

  m_teleport.execEvent = m_ExecEvent;
  m_teleport.m_facing = (eventFacingType)m_FacingList.GetCurSel();

	CDialog::OnOK();
}


void CTeleportDlg::OnLevelChange(void)
{
  char text[100];
  int i, lev;
  UpdateData(true);
  m_XMax = "Max X = ???";
  m_YMax = "Max Y = ???";
  m_levelSelect.GetWindowTextA(text,99);
  i = 0;
  lev = 0;
  while ((text[i]>='0') && (text[i]<='9'))
  {
    lev = 10*lev + text[i]-'0';
    i++;
  };
  if ( (i > 0) && (lev<MAX_LEVELS) )
  {
    if (globalData.levelInfo.stats[lev-1].used)
    {
      m_XMax.Format("Max X = %d", globalData.levelInfo.stats[lev-1].area_width-1);
      m_YMax.Format("Max Y = %d", globalData.levelInfo.stats[lev-1].area_height-1);
    };
  };
  UpdateData(false);
}

/*
void CTeleportDlg::OnDeltaposLevelspin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
   UpdateData(TRUE);

   int level = pNMUpDown->iPos + pNMUpDown->iDelta;
   
   if (level > MAX_LEVELS) level = 1;
   if (level < 1) level = MAX_LEVELS;

//   FormatEPList(0, level-1);

//   m_LevelNum = level;
//   m_LevelText.Format("%i: %s", m_LevelNum,globalData.levelInfo.stats[m_LevelNum-1].level_name);
   m_XMax.Format("X (curr width %u)", globalData.levelInfo.stats[m_LevelNum-1].area_width);
   m_YMax.Format("Y (curr height %u)", globalData.levelInfo.stats[m_LevelNum-1].area_height);

   UpdateData(FALSE);
	
	*pResult = 0;
}
*/
BOOL CTeleportDlg::OnInitDialog() 
{
   int i, levelNum;
   CDialog::OnInitDialog();
   
//   m_LevelSpin.SetRange(1,MAX_LEVELS);
   UDACCEL acceldata;
   acceldata.nInc=1;
   acceldata.nSec=0;
//   m_LevelSpin.SetAccel(1, &acceldata);

   m_ExecEvent = m_teleport.execEvent;
   levelNum = m_teleport.destLevel;
//   m_LevelSpin.SetPos(m_teleport.destLevel+1); 
   m_DestX = m_teleport.destX;
   m_DestY = m_teleport.destY;

   if (m_DestX < 0) m_DestX = 0;
   if (m_DestY < 0) m_DestY = 0;

   UpdateData(FALSE);

   FillLevelList(levelNum);

   FormatEPList(m_teleport.destEP, m_teleport.destLevel);
  
   m_FacingList.ResetContent();
   
   for (i=0;i<NUM_FACING_TYPES;i++)
      m_FacingList.AddString(FacingText[i]);
   m_FacingList.AddString("Unchanged");
   i++;
   
   m_FacingList.SetCurSel((int)m_teleport.m_facing);
   
   CWnd *pWnd = GetDlgItem(IDC_LEVELSPIN);
   if (pWnd != NULL)
      pWnd->EnableWindow(m_AllowLevel);

   GotoDlgCtrl(GetDlgItem(IDC_ENTRYPOINTLIST));

//   if ((m_LevelNum <= 0) || (m_LevelNum >= MAX_LEVELS))
//   {
//     m_LevelText.Format("%i: %s", 1, "");
//     m_XMax.Format("X (curr width %u)", 1);
//     m_YMax.Format("Y (curr height %u)", 1);
//   }
//   else
//   {
//   m_LevelText.Format("%i: %s", m_LevelNum,globalData.levelInfo.stats[m_LevelNum-1].level_name);
//   m_XMax.Format("X (curr width %u)", globalData.levelInfo.stats[m_LevelNum-1].area_width);
//   m_YMax.Format("Y (curr height %u)", globalData.levelInfo.stats[m_LevelNum-1].area_height);
//   };
 
  UpdateData(FALSE);
   
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTeleportDlg::FillLevelList(int defaultLevel) 
{
  int i, j, k;
  if (defaultLevel < 0) defaultLevel = globalData.currLevel;
  k = 0;
  m_levelSelect.ResetContent();
  for (i=0; i<MAX_LEVELS; i++)
  {
    if (globalData.levelInfo.stats[i].used)
    {
      CString txt;
      txt.Format("%d) - %s",i+1,globalData.levelInfo.stats[i].level_name);
      j = m_levelSelect.AddString(txt);
      if (i == defaultLevel) k = j;
  
    };
  }
  m_levelSelect.SetCurSel(k);
}


void CTeleportDlg::OnSelchangeFacing() 
{
   m_teleport.m_facing = 
      (eventFacingType) m_FacingList.GetCurSel();	
}

void CTeleportDlg::FormatEPList(int destEP, int destLevel)
{
   m_EPList.ResetContent();
   CString temp;

   m_EPList.AddString("Use X,Y");
   m_EPList.SetItemData(0, 0);

   m_EPList.AddString("To Party's Square");
   m_EPList.SetItemData(1,9999);

   m_EPList.AddString("Script");
   m_EPList.SetItemData(2,9998);

   for (int i=0;i<MAX_ENTRY_POINTS;i++)
   {
     if ((destLevel < 0) || (destLevel >= MAX_LEVELS))
     {
       temp.Format("%d   (0, 0)", i+1);
     }
     else
     {
      temp.Format("%i    (%i, %i)", 
                  i+1,
                  globalData.levelInfo.stats[destLevel].entryPoints[i].x,
                  globalData.levelInfo.stats[destLevel].entryPoints[i].y);
     };

      int index = m_EPList.AddString(temp);
      m_EPList.SetItemData(index, i+1);
   }

   int curr = 0;
   if (destEP == -1) curr = 1;
   if (destEP == -2) curr = 2;
   for (int e=3;e<m_EPList.GetCount();e++)
   {
      int data = m_EPList.GetItemData(e);
      if (data == destEP)
      {
        curr = e;
        break;
      }
   }

   m_EPList.SetCurSel(curr);

   if (destEP == -1)  //Party Square
   {
     m_teleport.destX = 0;
     m_teleport.destY = 0;
   } 
   else if (destEP == 0) // XY
   {
     m_teleport.destX = m_DestX;
     m_teleport.destY = m_DestY;
   }
   else if (destEP == -2)  // Script
   {
     m_teleport.destX = m_DestX;
     m_teleport.destY = m_DestY;
   }
   else
   {
     m_teleport.destX = 0;
     m_teleport.destY = 0;
   }

   m_teleport.destEP = destEP;
   m_teleport.destLevel = destLevel;

   OnSelchangeEntrypointlist();
}

void CTeleportDlg::GetData(TRANSFER_DATA& data)
{
  data = m_teleport;
}

void CTeleportDlg::OnSelchangeEntrypointlist() 
{
  UpdateData(TRUE);
  m_teleport.destEP = m_EPList.GetCurSel();
  if ((m_teleport.destEP != 0) && (m_teleport.destEP != 2))
  {
    m_DestX = 0;
    m_DestY = 0;

    CWnd *pWnd = GetDlgItem(IDC_DESTX);
    if (pWnd != NULL)
      pWnd->EnableWindow(FALSE);
    pWnd = GetDlgItem(IDC_DESTY);
    if (pWnd != NULL)
      pWnd->EnableWindow(FALSE);
  }
  else
  {
    CWnd *pWnd = GetDlgItem(IDC_DESTX);
    if (pWnd != NULL)
      pWnd->EnableWindow(TRUE);
    pWnd = GetDlgItem(IDC_DESTY);
    if (pWnd != NULL)
      pWnd->EnableWindow(TRUE);
  }
  UpdateData(FALSE);
}
