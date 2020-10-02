/******************************************************************************
* Filename: DesignList.cpp
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
#include "GlobalData.h"
#include "Level.h"
//#include "PicSlot.h"
#include "DesignList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignList dialog


CDesignList::CDesignList(CString name, CWnd* pParent /*=NULL*/)
	: CDialog(CDesignList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignList)
	m_Name = _T("");
	//}}AFX_DATA_INIT
  m_Name = name;
}


void CDesignList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignList)
	DDX_Control(pDX, IDC_DESIGNLIST, m_List);
	DDX_Text(pDX, IDC_DESIGNNAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignList, CDialog)
	//{{AFX_MSG_MAP(CDesignList)
	ON_NOTIFY(NM_DBLCLK, IDC_DESIGNLIST, OnDblclkDesignlist)
	ON_NOTIFY(NM_CLICK, IDC_DESIGNLIST, OnClickDesignlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignList message handlers

BOOL CDesignList::OnInitDialog() 
{
	CDialog::OnInitDialog();
 
  //m_List.ResetContent();

	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Folder Name";
	lvc.cx = 200;
	lvc.fmt = LVCFMT_LEFT;
	m_List.InsertColumn(0, &lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Design Name";
	lvc.cx = 250;
	lvc.fmt = LVCFMT_LEFT;
  m_List.InsertColumn(1, &lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 2;
	lvc.pszText = "Ver";
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;
  m_List.InsertColumn(2, &lvc, CListSort::dtDEC, true);

  m_List.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
  
  char fullPath[_MAX_PATH+1];
  strcpy(fullPath, rte.RootDir());
  strcat(fullPath, "*.dsn");
 
  BOOL added = FALSE;
  int selected = -1;
  CString folder = rte.RootDir();
  StripFilenamePath(folder);
  if (!folder.IsEmpty())
  {
    CString saveName;
    // if filaname ends with '\', strip it
    if ((folder.GetLength() > 3) && (folder[folder.GetLength()-1] == '\\'))
    {
      saveName = folder.Left(folder.GetLength()-1);
      folder = saveName;
    }
    int index = folder.ReverseFind('.');
    if (index >= 1)
    {
      saveName = folder.Left(index);
      folder = saveName;
    }  
  }

  WIN32_FIND_DATA findData;
  HANDLE hSearch = FindFirstFile(fullPath, &findData);

  if (hSearch != INVALID_HANDLE_VALUE)
  {
    do 
    {
      if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        CString gamedat;
        gamedat = rte.DataDir() + findData.cFileName; //.Format("%s%s\\Data\\game.dat", global_rootDir, findData.cFileName);

        char *ptr = strchr(findData.cFileName, '.');
        if (ptr != NULL) *ptr = '\0';

        if (strlen(findData.cFileName) > 0)
        {
          double ver;
          CString temp;

          GetDesignVersion(gamedat, ver, temp);

          if (ver > 0.0)
          {
            AddToAvailList(findData.cFileName, temp, ver);
         
            //int index = m_List.AddString(findData.cFileName);
            if (folder.CompareNoCase(findData.cFileName)==0)
              selected = m_List.GetItemCount()-1;
            added = TRUE;
          }
        }
      }
    } while (FindNextFile(hSearch, &findData));
  }

  FindClose(hSearch);
	
  if (added)
  {
    if (selected < 0) selected = 0;
    m_List.SelectItem(selected, true);
    //OnSelchangeDesignlist();
  }

  UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/*
void CDesignList::OnSelchangeDesignlist() 
{
  int sel = m_List.GetCurSel();

  if (sel != LB_ERR)
  {
    CString temp="";
    m_List.GetText(sel, temp);
    if (temp != "")
    {
      m_Name = temp;
      UpdateData(FALSE);
    }
  }	
}
*/

void CDesignList::OnOK() 
{
  UpdateData(TRUE);
  EndDialog(IDOK);
}

void CDesignList::AddToAvailList(CString FolderName, CString DesignName, double version)
{
  int listIdx = m_List.GetItemCount();

  char fname[_MAX_PATH+1];
  char dname[_MAX_PATH+1];
  char ver[25];

  strncpy(fname, FolderName, _MAX_PATH);
  fname[_MAX_PATH]='\0';
  strncpy(dname, DesignName, _MAX_PATH);
  dname[_MAX_PATH]='\0';

  sprintf(ver, "%6.4f", version);

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = fname;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = 0;

  m_List.InsertItem(&item);
  m_List.SetItemText(listIdx, 1, dname); 
  m_List.SetItemText(listIdx, 2, ver); 
}

void CDesignList::OnDblclkDesignlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
  POSITION pos = m_List.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_List.GetNextSelectedItem(pos);
    m_Name = m_List.GetItemText(nItem, 0);
    UpdateData(FALSE);
  }

  OnOK();
  
	*pResult = 0;
}

void CDesignList::OnClickDesignlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
  POSITION pos = m_List.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_List.GetNextSelectedItem(pos);
    m_Name = m_List.GetItemText(nItem, 0);
    UpdateData(FALSE);
  }

	*pResult = 0;
}


void CDesignList::GetDesignVersion(const char *fname, double &ver, CString &name)
{
  CFile myFile;
  ver=0.0;
  name="";
  
  try
  {
    if (!myFile.Open(fname, CFile::modeRead))
    {
      TRACE("Open Error: Unable to determine design version from file %s\n", fname);
      return;
    }   

    CArchive ar(&myFile, CArchive::load);  
    ar >> ver;
    ar >> name;
    if (name == "*")
      name="";
    ar.Close();
  }
  catch (...)
  {
    TRACE("Unable to determine design version from file %s\n", fname);
  }

  if (ver <= 0.0)
          TRACE("Error %u trying to determine design version from file %s\n", 
           GetLastError(), fname);
}
