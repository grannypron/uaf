/******************************************************************************
* Filename: DesignStatsDlg.cpp
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
//#include "..\Shared\Version.h"
#include "resource.h"

#include "GlobalData.h"
#include "Monster.h"
#include "Level.h"
#include "DesignStatsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignStatsDlg dialog


CDesignStatsDlg::CDesignStatsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignStatsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignStatsDlg)
	m_Events = _T("");
	m_Items = _T("");
	m_Levels = _T("");
	m_Monsters = _T("");
	m_NPCs = _T("");
	m_Quests = _T("");
	m_SizeOnDisk = _T("");
	m_Spells = _T("");
	m_SpItems = _T("");
	m_SpKeys = _T("");
	m_Files = _T("");
	//}}AFX_DATA_INIT
}


void CDesignStatsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignStatsDlg)
	DDX_Text(pDX, IDC_EVENTCOUNT, m_Events);
	DDX_Text(pDX, IDC_ITEMCOUNT, m_Items);
	DDX_Text(pDX, IDC_LEVELCOUNT, m_Levels);
	DDX_Text(pDX, IDC_MONSTERCOUNT, m_Monsters);
	DDX_Text(pDX, IDC_NPCCOUNT, m_NPCs);
	DDX_Text(pDX, IDC_QUESTCOUNT, m_Quests);
	DDX_Text(pDX, IDC_SIZEONDISK, m_SizeOnDisk);
	DDX_Text(pDX, IDC_SPELLCOUNT, m_Spells);
	DDX_Text(pDX, IDC_SPITEMCOUNT, m_SpItems);
	DDX_Text(pDX, IDC_SPKEYCOUNT, m_SpKeys);
	DDX_Text(pDX, IDC_FILECOUNT, m_Files);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignStatsDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignStatsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignStatsDlg message handlers

BOOL CDesignStatsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Events.Format("%u (current level only)", levelData.eventData.GetCount());
  m_Items.Format("%u", itemData.GetCount());
  m_Monsters.Format("%u", monsterData.GetCount());
  m_Spells.Format("%u", spellData.GetCount());
  m_NPCs.Format("%u", globalData.charData.GetCount());
  m_Quests.Format("%u", globalData.questData.GetCount());
  m_SpItems.Format("%u", globalData.specialItemData.GetCount());
  m_SpKeys.Format("%u", globalData.keyData.GetCount());
  m_Levels.Format("%u", globalData.levelInfo.numLevels);
	
  DWORD total = 0;
  DWORD files = 0;
  char searchPath[_MAX_PATH+1];

  CString designPath = GetDesignPath();
  CString filePath;

  strcpy(searchPath, designPath);
  strcat(searchPath, "*.*");
  
  WIN32_FIND_DATA findData;
  HANDLE hSearch = FindFirstFile(searchPath, &findData);

  if (hSearch != INVALID_HANDLE_VALUE)
  {
    do 
    {
      if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0)
      {
        filePath = designPath + findData.cFileName;
        total += FileSize(filePath);
        files++;
      }

    } while (FindNextFile(hSearch, &findData));
  }

  FindClose(hSearch);

#define KILOBYTE 1024
#define MEGABYTE (1024*KILOBYTE)

  CString scale("");
  double size=total;
  if (total < KILOBYTE)
  {
    scale="bytes";
  }
  else if (total < MEGABYTE)
  {
    scale = "KB";
    size = size / KILOBYTE;
  }
  else
  {
    scale = "MB";
    size = size / MEGABYTE;
  }
  m_SizeOnDisk.Format("%.2f %s", size, scale);
  m_Files.Format("%u", files);

  UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
