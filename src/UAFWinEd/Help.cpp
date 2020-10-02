/******************************************************************************
* Filename: EditText.cpp
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
#include "Help.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CFont Font;




/////////////////////////////////////////////////////////////////////////////
// CEditText dialog


CHelp::CHelp(const CString& helpKey, 
             CWnd* pParent /*=NULL*/)	: CDialog(CHelp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditText)
	m_Text = _T("");
	//}}AFX_DATA_INIT
  m_helpKey=helpKey;
}

CHelp::~CHelp()
{
}

void CHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditText)
	DDX_Text(pDX, IDC_HelpText, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelp, CDialog)
	//{{AFX_MSG_MAP(CHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditText message handlers

BOOL CHelp::OnInitDialog() 
{
  FILE *f;
  int keyLen;
  char *line;
  //CWnd *textWindow;
  m_Text = "This is all the help you get!";
  keyLen = m_helpKey.GetLength();
  f = fopen(ede.EditorHelpDir() + "Help.txt", "r");
  if (f == NULL)
  {
    m_Text = "Cannot find the 'Help.txt' file in the EditorResources folder";
  }
  else
  {
    line = (char *)malloc(100000);
    if (line != NULL)
    {
      bool matches;
      matches = false;
      m_Text.Empty();
      while (fgets(line, 99999, f) != NULL)
      {
        if (line[0] == '{')
        {
          char *col;
          col = strchr(line, '}');
          if (col != NULL)
          {
            if (col - line - 1 >= keyLen)
            {
              matches = memcmp(line+1, m_helpKey, keyLen) == 0;
            }
            else
            {
              matches = false;
            };
          }
          else
          {
            if (matches)
            {
              int len;
              len = strlen(line);
              if (line[len-1] == '\012')
              {
                line[len-1] = '\015';
                line[len] = '\012';
                line[len+1] = 0;
              };
              m_Text += line;
              m_Text += '\015';
            };
          };
        }
        else
        {
          if (matches)
          {
            int len;
            len = strlen(line);
            if (line[len-1] == '\012')
            {
              line[len-1] = '\015';
              line[len] = '\012';
              line[len+1] = 0;
            };
            m_Text += line;
            m_Text += '\015';
          };
        };
      };
    };
    free(line);
    fclose(f);
  };
  if (m_Text.IsEmpty())
  {
    m_Text = CString("No help found in 'Help.txt' for topic \"") + m_helpKey + '"' ;
  }
  else
  {
    m_Text = CString('{') + m_helpKey + "}\015\012" + m_Text;
  };
	CDialog::OnInitDialog();
  //textWindow = GetDlgItem(IDC_HelpText);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHelp::OnOK() 
{
  // copy text from edit window to script before exiting
	CDialog::OnOK();
}

