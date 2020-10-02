/******************************************************************************
* Filename: SoundChooser.cpp
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
#include "SoundChooser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSoundChooser dialog


CSoundChooser::CSoundChooser(CString filename, BYTE filter, CWnd* pParent /*=NULL*/)
	: CDialog(CSoundChooser::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSoundChooser)
	m_Filename = _T("");
	m_Loop = FALSE;
	m_Filetypes = _T("");
	//}}AFX_DATA_INIT

  //m_filter = filter;
  m_filter = ALL_SOUND_TYPES;
  m_Filename = filename;
  m_hPlayIcon = NULL;
  m_hStopIcon = NULL;
  m_buf = -1;
  isWave = true;
}


void CSoundChooser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoundChooser)
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_PLAY, m_Play);
	DDX_Text(pDX, IDC_FILENAME, m_Filename);
	DDX_Check(pDX, IDC_LOOP, m_Loop);
	DDX_Text(pDX, IDC_FILETYPESALLOWED, m_Filetypes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSoundChooser, CDialog)
	//{{AFX_MSG_MAP(CSoundChooser)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_FILEBROWSE, OnFilebrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundChooser message handlers


void CSoundChooser::OnStop() 
{
  ClearSound(m_buf);
}

void CSoundChooser::OnPlay() 
{
  OnStop();
	UpdateData(TRUE);

  if (!m_Filename.IsEmpty())
  {
    m_buf = LoadSound(m_Filename);
    if (m_buf >= 0)
      PlaySound(m_buf, m_Loop);
    else
    {
      CString temp;
      temp.Format("Failed to load %s\n", m_Filename);
      MsgBoxError(temp, "Sound Error");
    }
  }
} 

void CSoundChooser::OnFilebrowse() 
{
  OnStop();

  BOOL fileChosen = FALSE;

  //int index;
  //static CString lastDir;
  static BOOL WasWave = TRUE;

  CString defaultFilename(m_Filename);
  //CString defaultDir;
  //CString szFilter;
  CString def;

  /*
  index = m_Filename.ReverseFind('\\');
  if (index > 1)
     defaultFilename = m_Filename.Right(m_Filename.GetLength() - index - 1);
  else
     defaultFilename = "";
  */

  //szFilter = szAllSndFilter;
  if (WasWave)
    def = "WAV";
  else
    def = "MID";

  fileChosen = GetSoundFilename(defaultFilename, def);

/*
   if (defaultFilename.IsEmpty())
   {
      if (lastDir.IsEmpty())
         defaultDir = m_installDir;
      else
         defaultDir = lastDir;
   }
   else
   {
      index = defaultFilename.ReverseFind('\\');
      if (index > 3)
         defaultDir = defaultFilename.Left(index);
      else
         defaultDir = m_installDir;
   }
  CFileDialog dlg(TRUE, def, defaultFilename, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter);
  dlg.m_ofn.lpstrInitialDir = (LPCSTR)defaultDir;
  DWORD result = dlg.DoModal();
  if (result == IDOK)
  {
     defaultFilename = dlg.GetPathName();
     index = defaultFilename.ReverseFind('\\');
     if (index > 3)
        lastDir = defaultFilename.Left(index);
     fileChosen = TRUE;
  }
*/

  if (fileChosen) 
  {   
    WasWave = IsWaveFile(defaultFilename);
    if (fileChosen)
    {
      m_Filename = defaultFilename;
      StripFilenamePath(m_Filename);
      UpdateData(FALSE);
    }
  }
}

BOOL CSoundChooser::OnInitDialog() 
{
	CDialog::OnInitDialog();

  m_hPlayIcon = ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PLAYARROW), IMAGE_ICON,0,0,LR_DEFAULTCOLOR);
  m_hStopIcon = ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PLAYSTOP), IMAGE_ICON,0,0,LR_DEFAULTCOLOR);

  m_Play.SetIcon( (HICON)m_hPlayIcon );
  m_Stop.SetIcon( (HICON)m_hStopIcon );
	
  //if ((m_filter & ALL_SOUND_TYPES) == ALL_SOUND_TYPES)
    m_Filetypes = "Choose WAV, MP3, or MIDI files";
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSoundChooser::PostNcDestroy() 
{
  if (m_hPlayIcon != NULL)
    DestroyIcon((HICON)m_hPlayIcon);

  if (m_hStopIcon != NULL)
    DestroyIcon((HICON)m_hStopIcon);

  pSndMgr->Clear();

	CDialog::PostNcDestroy();
}
