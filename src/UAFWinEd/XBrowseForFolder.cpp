// XBrowseForFolder.cpp  Version 1.2
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// Description:
//     XBrowseForFolder.cpp implements XBrowseForFolder(), a function that
//     wraps SHBrowseForFolder().
//
// History
//     Version 1.2 - 2008 February 29
//     - Changed API to allow for initial CSIDL. 
//     - Added option to set dialog caption, suggested by SimpleDivX.
//     - Added option to set root, suggested by Jean-Michel Reghem.
//
//     Version 1.1 - 2003 September 29 (not released)
//     - Added support for edit box
//
//     Version 1.0 - 2003 September 25
//     - Initial public release
//
// License:
//     This software is released into the public domain.  You are free to use
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

// if you don't want to use MFC, comment out the following line:
#include "stdafx.h"

#ifndef __AFX_H__
#include "windows.h"
#include "crtdbg.h"
#include "tchar.h"
#endif

#include "Shlobj.h"
#include "io.h"
#include "XBrowseForFolder.h"

#pragma warning(disable: 4127)	// conditional expression is constant (_ASSERTE)
#pragma warning(disable : 4996)	// disable bogus deprecation warning

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#undef TRACE
#define TRACE __noop

//=============================================================================
// if you want to see the TRACE output, uncomment this line:
//#include "XTrace.h"
//=============================================================================

//=============================================================================
// struct to pass to callback function
//=============================================================================
struct FOLDER_PROPS
{
	LPCTSTR lpszTitle;
	LPCTSTR lpszInitialFolder;
	UINT    ulFlags;
  BOOL    bNewDesign;
};

#ifndef __AFX_H__

///////////////////////////////////////////////////////////////////////////////
// CRect - a minimal CRect class

class CRect : public tagRECT
{
public:
	//CRect() { }
	CRect(int l = 0, int t = 0, int r = 0, int b = 0) 
	{ 
		left = l; 
		top = t; 
		right = r; 
		bottom = b; 
	}
	int Width() const { return right - left; }
	int Height() const { return bottom - top; }
	void SwapLeftRight() { SwapLeftRight(LPRECT(this)); }
	static void SwapLeftRight(LPRECT lpRect) { LONG temp = lpRect->left; 
											   lpRect->left = lpRect->right; 
											   lpRect->right = temp; }
	operator LPRECT() { return this; }
};
#endif

///////////////////////////////////////////////////////////////////////////////
// ScreenToClientX - helper function in case non-MFC
static void ScreenToClientX(HWND hWnd, LPRECT lpRect)
{
	_ASSERTE(::IsWindow(hWnd));
	::ScreenToClient(hWnd, (LPPOINT)lpRect);
	::ScreenToClient(hWnd, ((LPPOINT)lpRect)+1);
}

///////////////////////////////////////////////////////////////////////////////
// MoveWindowX - helper function in case non-MFC
static void MoveWindowX(HWND hWnd, CRect& rect, BOOL bRepaint)
{
	_ASSERTE(::IsWindow(hWnd));
	::MoveWindow(hWnd, rect.left, rect.top,
		rect.Width(), rect.Height(), bRepaint);
}

///////////////////////////////////////////////////////////////////////////////
// SizeBrowseDialog - resize dialog, move controls
static void SizeBrowseDialog(HWND hWnd, FOLDER_PROPS *fp)
{
	TRACE(_T("in void SizeBrowseDialog\n"));

	// find the folder tree and make dialog larger
	HWND hwndTree = FindWindowEx(hWnd, NULL, _T("SysTreeView32"), NULL);

	if (!hwndTree)
	{
		// ... this usually means that BIF_NEWDIALOGSTYLE is enabled.
		// Then the class name is as used in the code below.
		hwndTree = FindWindowEx(hWnd, NULL, _T("SHBrowseForFolder ShellNameSpace Control"), NULL);
		TRACE(_T("SHBrowseForFolder ShellNameSpace Control: hwndTree=%X\n"), hwndTree);
	}

	CRect rectDlg;

	_ASSERTE(IsWindow(hwndTree));

	if (hwndTree)
	{
		// check if edit box
		int nEditHeight = 0;
		HWND hwndEdit = FindWindowEx(hWnd, NULL, _T("Edit"), NULL);
		TRACE(_T("hwndEdit=%x\n"), hwndEdit);
		CRect rectEdit;
		if (hwndEdit && (fp->ulFlags & BIF_EDITBOX))
		{
			::GetWindowRect(hwndEdit, &rectEdit);
			ScreenToClientX(hWnd, &rectEdit);
			nEditHeight = rectEdit.Height();
		}
		else if (hwndEdit)
		{
			::MoveWindow(hwndEdit, 20000, 20000, 10, 10, FALSE);
			::ShowWindow(hwndEdit, SW_HIDE);
			hwndEdit = 0;
		}

		// make the dialog larger
		::GetWindowRect(hWnd, &rectDlg);
		rectDlg.right += 40;
		rectDlg.bottom += 30;
		if (hwndEdit)
			rectDlg.bottom += nEditHeight + 5;
		MoveWindowX(hWnd, rectDlg, TRUE);
		::GetClientRect(hWnd, &rectDlg);

		int hMargin = 10;
		int vMargin = 10;

		// check if new dialog style - this means that there will be a resizing
		// grabber in lower right corner
//		if (fp->ulFlags & BIF_NEWDIALOGSTYLE)
//			hMargin = ::GetSystemMetrics(SM_CXVSCROLL);

		// move the Cancel button
		CRect rectCancel;
		HWND hwndCancel = ::GetDlgItem(hWnd, IDCANCEL);
		if (hwndCancel)
			::GetWindowRect(hwndCancel, &rectCancel);
		ScreenToClientX(hWnd, &rectCancel);
		int h = rectCancel.Height();
		int w = rectCancel.Width();
		rectCancel.bottom = rectDlg.bottom - vMargin;//nMargin;
		rectCancel.top = rectCancel.bottom - h;
		rectCancel.right = rectDlg.right - hMargin; //(scrollWidth + 2*borderWidth);
		rectCancel.left = rectCancel.right - w;
		if (hwndCancel)
		{
			//TRACERECT(rectCancel);
			MoveWindowX(hwndCancel, rectCancel, FALSE);
      if (fp->bNewDesign)
      {
        SetWindowText(hwndCancel,"New Design");
      };
		}

		// move the OK button
		CRect rectOK(0, 0, 0, 0);
		HWND hwndOK = ::GetDlgItem(hWnd, IDOK);
		if (hwndOK)
			::GetWindowRect(hwndOK, &rectOK);
		ScreenToClientX(hWnd, &rectOK);
		rectOK.bottom = rectCancel.bottom;
		rectOK.top = rectCancel.top;
		rectOK.right = rectCancel.left - 10;
		rectOK.left = rectOK.right - w;
		if (hwndOK)
		{
			MoveWindowX(hwndOK, rectOK, FALSE);
		}

		// expand the folder tree to fill the dialog
		CRect rectTree;
		::GetWindowRect(hwndTree, &rectTree);
		ScreenToClientX(hWnd, &rectTree);
		if (hwndEdit)
		{
			rectEdit.left = hMargin;
			rectEdit.right = rectDlg.right - hMargin;
			rectEdit.top = vMargin;
			rectEdit.bottom = rectEdit.top + nEditHeight;
			MoveWindowX(hwndEdit, rectEdit, FALSE);
			rectTree.top = rectEdit.bottom + 5;
		}
		else
		{
			rectTree.top = vMargin;
		}
		rectTree.left = hMargin;
		rectTree.bottom = rectOK.top - 10;//nMargin;
		rectTree.right = rectDlg.right - hMargin;
		//TRACERECT(rectTree);
		MoveWindowX(hwndTree, rectTree, FALSE);
	}
	else
	{
		TRACE(_T("ERROR - tree control not found.\n"));
		//_ASSERTE(hwndTree);
	}
}

///////////////////////////////////////////////////////////////////////////////
// BrowseCallbackProc - SHBrowseForFolder callback function
static int CALLBACK BrowseCallbackProc(HWND hWnd,		// Window handle to the browse dialog box
									   UINT uMsg,		// Value identifying the event
									   LPARAM lParam,	// Value dependent upon the message 
									   LPARAM lpData)	// Application-defined value that was 
														// specified in the lParam member of the 
														// BROWSEINFO structure
{
	switch (uMsg)
	{
		case BFFM_INITIALIZED:		// sent when the browse dialog box has finished initializing. 
		{
			TRACE(_T("hWnd=%X\n"), hWnd);

			// remove context help button from dialog caption
			LONG lStyle = ::GetWindowLong(hWnd, GWL_STYLE);
			lStyle &= ~DS_CONTEXTHELP;
			::SetWindowLong(hWnd, GWL_STYLE, lStyle);
			lStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
			lStyle &= ~WS_EX_CONTEXTHELP;
			::SetWindowLong(hWnd, GWL_EXSTYLE, lStyle);

			FOLDER_PROPS *fp = (FOLDER_PROPS *) lpData;
			if (fp)
			{
				if (fp->lpszInitialFolder && (fp->lpszInitialFolder[0] != _T('\0')))
				{
					// set initial directory
					::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)fp->lpszInitialFolder);
				}

				if (fp->lpszTitle && (fp->lpszTitle[0] != _T('\0')))
				{
					// set window caption
					::SetWindowText(hWnd, fp->lpszTitle);
				}
			}

			SizeBrowseDialog(hWnd, fp);
		}
		break;

		case BFFM_SELCHANGED:		// sent when the selection has changed
		{
			TCHAR szDir[MAX_PATH*2] = { 0 };

			// fail if non-filesystem
			BOOL bRet = SHGetPathFromIDList((LPITEMIDLIST) lParam, szDir);
			if (bRet)
			{
				// fail if folder not accessible
				if (_taccess(szDir, 00) != 0)
				{
					bRet = FALSE;
				}
				else
				{
					SHFILEINFO sfi;
					::SHGetFileInfo((LPCTSTR)lParam, 0, &sfi, sizeof(sfi), 
							SHGFI_PIDL | SHGFI_ATTRIBUTES);
					TRACE(_T("dwAttributes=0x%08X\n"), sfi.dwAttributes);

					// fail if pidl is a link
					if (sfi.dwAttributes & SFGAO_LINK)
					{
						TRACE(_T("SFGAO_LINK\n"));
						bRet = FALSE;
					}
				}
			}

			// if invalid selection, disable the OK button
			if (!bRet)
			{
				::EnableWindow(GetDlgItem(hWnd, IDOK), FALSE);
			}

			TRACE(_T("szDir=%s\n"), szDir);
		}
		break;
	}
         
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// XBrowseForFolder()
//
// Purpose:     Invoke the SHBrowseForFolder API.  If lpszInitialFolder is
//              supplied, it will be the folder initially selected in the tree 
//              folder list.  Otherwise, the initial folder will be set to the 
//              current directory.  The selected folder will be returned in 
//              lpszBuf.
//
// Parameters:  hWnd              - handle to the owner window for the dialog
//              lpszInitialFolder - initial folder in tree;  if NULL, the initial
//                                  folder will be the current directory; if
//                                  if this is a CSIDL, must be a real folder.
//              nRootFolder       - optional CSIDL of root folder for tree;
//                                  -1 = use default.
//              lpszCaption       - optional caption for folder dialog
//              lpszBuf           - buffer for the returned folder path
//              dwBufSize         - size of lpszBuf in TCHARs
//              bEditBox          - TRUE = include edit box in dialog
//              newDesign         = "New Design" instead of "Cancel"
//
// Returns:     BOOL - TRUE = success;  FALSE = user hit Cancel
//
BOOL XBrowseForFolder(HWND hWnd,
					  LPCTSTR lpszInitialFolder,
					  int nRootFolder,
					  LPCTSTR lpszCaption,
					  LPTSTR lpszBuf,
					  DWORD dwBufSize,
					  BOOL bEditBox, /* =FALSE*/
            BOOL bNewDesign /* =FALSE */)
{
	_ASSERTE(lpszBuf);
	_ASSERTE(dwBufSize >= MAX_PATH);

	if (lpszBuf == NULL || dwBufSize < MAX_PATH)
		return FALSE;

	ZeroMemory(lpszBuf, dwBufSize*sizeof(TCHAR));

	BROWSEINFO bi = { 0 };

	// check if there is a special root folder
	LPITEMIDLIST pidlRoot = NULL;
	if (nRootFolder != -1)
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(hWnd, nRootFolder, &pidlRoot)))
			bi.pidlRoot = pidlRoot;
	}

	TCHAR szInitialPath[MAX_PATH*2] = { _T('\0') };
	if (lpszInitialFolder)
	{
		// is this a folder path string or a csidl?
		if (HIWORD(lpszInitialFolder) == 0)
		{
			// csidl
			int nFolder = LOWORD((UINT)(UINT_PTR)lpszInitialFolder);
			TRACE(_T("csidl:  nFolder=0x%X\n"), nFolder);
			SHGetSpecialFolderPath(hWnd, szInitialPath, nFolder, FALSE);
		}
		else
		{
			// string
			_tcsncpy(szInitialPath, lpszInitialFolder, 
						sizeof(szInitialPath)/sizeof(TCHAR)-2);
		}
		TRACE(_T("szInitialPath=<%s>\n"), szInitialPath);
	}

	if ((szInitialPath[0] == _T('\0')) && (bi.pidlRoot == NULL))
	{
		// no initial folder and no root, set to current directory
		::GetCurrentDirectory(sizeof(szInitialPath)/sizeof(TCHAR)-2, 
				szInitialPath);
	}

	FOLDER_PROPS fp;

	bi.hwndOwner = hWnd;
	bi.ulFlags   = BIF_RETURNONLYFSDIRS;	// do NOT use BIF_NEWDIALOGSTYLE, 
											// or BIF_STATUSTEXT
	if (bEditBox)
		bi.ulFlags |= BIF_EDITBOX;
	//bi.ulFlags  |= BIF_NONEWFOLDERBUTTON;
	bi.lpfn      = BrowseCallbackProc;
	bi.lParam    = (LPARAM) &fp;

	fp.lpszInitialFolder = szInitialPath;
	fp.lpszTitle = lpszCaption;
	fp.ulFlags = bi.ulFlags;
  fp.bNewDesign = bNewDesign;

	BOOL bRet = FALSE;

	LPITEMIDLIST pidlFolder = SHBrowseForFolder(&bi);

	if (pidlFolder)
	{
		TCHAR szBuffer[MAX_PATH*2] = { _T('\0') };

		if (SHGetPathFromIDList(pidlFolder, szBuffer))
		{
			_tcsncpy(lpszBuf, szBuffer, dwBufSize-1);
			bRet = TRUE;
		}
		else
		{
			TRACE(_T("SHGetPathFromIDList failed\n"));
		}
	}

	// free up pidls
	IMalloc *pMalloc = NULL; 
	if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc) 
	{
		if (pidlFolder)
			pMalloc->Free(pidlFolder);  
		if (pidlRoot)
			pMalloc->Free(pidlRoot);  
		pMalloc->Release(); 
	}

	return bRet;
}
