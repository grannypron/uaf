/******************************************************************************
* Filename: UAFWinEdDoc.cpp
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

#include "UAFWinEd.h"
#include "UAFWinEdDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdDoc

IMPLEMENT_DYNCREATE(CUAFWinEdDoc, CDocument)

BEGIN_MESSAGE_MAP(CUAFWinEdDoc, CDocument)
	//{{AFX_MSG_MAP(CUAFWinEdDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdDoc construction/destruction

CUAFWinEdDoc *pDoc;

CUAFWinEdDoc::CUAFWinEdDoc() : CDocument()
{
	// TODO: add one-time construction code here
  pDoc = this;
}

CUAFWinEdDoc::~CUAFWinEdDoc()
{
  pDoc = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdDoc serialization

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdDoc diagnostics

#ifdef _DEBUG
void CUAFWinEdDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUAFWinEdDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUAFWinEdDoc commands

