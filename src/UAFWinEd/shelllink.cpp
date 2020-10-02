/*
Module : SHELLLINK.CPP
Purpose: Implementation for an MFC class encapsulation of IShellLink
Created: PJN / 09-12-1998
History: PJN / 12-12-1998 1. Fixed a bug in CUrlShellLink::Save where m_sTarget was not being set.
                          2. Updated the trace comments in the code to include the function from 
                          which the function fails.
                          
Copyright (c) 1998 by PJ Naughter.  
All rights reserved.

 ********
 ********  This code downloaded from http://www.codeproject.com
 ********

*/

//////////////// Includes ////////////////////////////////////////////
#include "..\Shared\stdafx.h"
#include "ShellLink.h"
//#define INITGUID
//#include <initguid.h>
#include <shlguid.h>



//////////////// Macros / Locals /////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////// Implementation //////////////////////////////////////

CShellLinkInfo::CShellLinkInfo()
{
  //Set up some reasonable defaults
  m_pidl = NULL;
  m_wHotkey = 0;
  m_nIconIndex = 0;
  m_nShowCmd = SW_SHOW;
}

CShellLinkInfo::CShellLinkInfo(const CShellLinkInfo& sli)
{
  *this = sli;
}

CShellLinkInfo::~CShellLinkInfo()
{
  // Get the shell's allocator. 
  IMalloc* pMalloc;
  HRESULT hRes = SHGetMalloc(&pMalloc);
  if (!SUCCEEDED(hRes)) 
  {
    TRACE(_T("CShellLinkInfo::~CShellLinkInfo, Failed to get the shell's IMalloc interface, HRESULT was %x\n"), hRes);
    return;
  }

  //Free the pidl
  if (m_pidl)
  {
    pMalloc->Free(m_pidl);
    m_pidl = NULL;
  }

  // Release the pointer to IMalloc
  pMalloc->Release(); 
}

CShellLinkInfo& CShellLinkInfo::operator=(const CShellLinkInfo& sli)
{
  if (this==&sli)return *this;
  m_sTarget = sli.m_sTarget;
  m_pidl = sli.m_pidl;
  m_sArguments = sli.m_sArguments;
  m_sDescription = sli.m_sDescription;
  m_wHotkey = sli.m_wHotkey;
  m_sIconLocation = sli.m_sIconLocation;
  m_nIconIndex = sli.m_nIconIndex;
  m_nShowCmd = sli.m_nShowCmd;
  m_sWorkingDirectory = sli.m_sWorkingDirectory;

  return *this;
}

/*
#ifdef _DEBUG
void CShellLinkInfo::Dump(CDumpContext& dc)
{
  CObject::Dump(dc);

  dc << _T("\nm_sTarget = ") << m_sTarget
     << _T("\nm_pidl = ") << m_pidl
     << _T("\nm_sArguments = ") << m_sArguments
     << _T("\nm_sDescription = ") << m_sDescription
     << _T("\nm_wHotkey = ") << m_wHotkey
     << _T("\nm_sIconLocation = ") << m_sIconLocation
     << _T("\nm_nIconIndex = ") << m_nIconIndex
     << _T("\nm_nShowCmd = ") << m_nShowCmd
     << _T("\nm_sWorkingDirectory = ") << m_sWorkingDirectory;
}
#endif
*/

CShellLink::CShellLink()
{
  m_psl = NULL;
  m_ppf = NULL;
  m_bAttemptedInitialise = FALSE;
}

CShellLink::~CShellLink()
{
  if (m_ppf)
    m_ppf->Release();
  if (m_psl)
    m_psl->Release();
}

BOOL CShellLink::Initialise()
{
  BOOL bSuccess = FALSE;
  if (m_bAttemptedInitialise)
    bSuccess = (m_psl != NULL);
  else
  {
    //Instantiate the COM class
    HRESULT hRes = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*) &m_psl);
    if (SUCCEEDED(hRes))
    {
      //Also get a pointer to IPersistFile
      hRes = m_psl->QueryInterface(IID_IPersistFile, (LPVOID*) &m_ppf);
      bSuccess = SUCCEEDED(hRes);
      if (!bSuccess)
        TRACE(_T("CShellLink::Initialise, Failed in call to QueryInterface for IPersistFile, HRESULT was %x\n"), hRes);        
    }
    else
      TRACE(_T("CShellLink::Initialise, Failed in call to CoCreateInstance for IShellLink, HRESULT was %x\n"), hRes);

    m_bAttemptedInitialise = TRUE;
  }

  return bSuccess;
}

BOOL CShellLink::Create(const CShellLinkInfo& sli)
{
  if (!Initialise())
    return FALSE;

  m_sli = sli;

  return TRUE;
}

BOOL CShellLink::Save(const CString& sFilename)
{
  if (!Initialise())
    return FALSE;

  //Validate our parameters
  if (m_sli.m_pidl == NULL)
    ASSERT(!m_sli.m_sTarget.IsEmpty());
  ASSERT(!sFilename.IsEmpty());
  ASSERT(m_psl);
  ASSERT(m_ppf);

  BOOL bSuccess = FALSE;
  HRESULT hRes;

  //Convert the path to a UNICODE string
  WCHAR wszPath[MAX_PATH];
#ifndef _UNICODE
  ::MultiByteToWideChar(CP_ACP, 0, sFilename, -1, wszPath, MAX_PATH);
#else
  _tcscpy(wszPath, sFilename);
#endif

  //Set the various link values
  if (m_sli.m_pidl)
  {
    hRes = m_psl->SetIDList(m_sli.m_pidl);
    if (!SUCCEEDED(hRes))
      TRACE(_T("CShellLink::Save, Failed in call to IShellLink::SetIDList, HRESULT was %x\n"), hRes);
  }
  else
  {
    hRes = m_psl->SetPath(m_sli.m_sTarget);
    if (!SUCCEEDED(hRes))
      TRACE(_T("CShellLink::Save, Failed in call to IShellLink::SetPath, HRESULT was %x\n"), hRes);
  }

  hRes = m_psl->SetWorkingDirectory(m_sli.m_sWorkingDirectory);
  if (!SUCCEEDED(hRes))
    TRACE(_T("CShellLink::Save, Failed in call to IShellLink::SetWorkingDirectory, HRESULT was %x\n"), hRes);

  hRes = m_psl->SetIconLocation(m_sli.m_sIconLocation, m_sli.m_nIconIndex);
  if (!SUCCEEDED(hRes))
    TRACE(_T("CShellLink::Save, Failed in call to IShellLink::SetIconLocation, HRESULT was %x\n"), hRes);

  hRes = m_psl->SetDescription(m_sli.m_sDescription);
  if (!SUCCEEDED(hRes))
    TRACE(_T("CShellLink::Save, Failed in call to IShellLink::SetDescription, HRESULT was %x\n"), hRes);

  hRes = m_psl->SetArguments(m_sli.m_sArguments);
  if (!SUCCEEDED(hRes))
    TRACE(_T("CShellLink::Save, Failed in call to IShellLink::SetArguments, HRESULT was %x\n"), hRes);

  hRes = m_psl->SetHotkey(m_sli.m_wHotkey);
  if (!SUCCEEDED(hRes))
    TRACE(_T("CShellLink::Save, Failed in call to IShellLink::SetHotkey, HRESULT was %x\n"), hRes);

  hRes = m_psl->SetShowCmd(m_sli.m_nShowCmd);
  if (!SUCCEEDED(hRes))
    TRACE(_T("CShellLink::Save, Failed in call to IShellLink::SetShowCmd, HRESULT was %x\n"), hRes);

  //Save the link to file
  hRes = m_ppf->Save(wszPath, TRUE);
  if (SUCCEEDED(hRes))
    bSuccess = TRUE;
  else
    TRACE(_T("CShellLink::Save, Failed in call to IPersistFile::Save, HRESULT was %x\n"), hRes);

  return bSuccess;
}

BOOL CShellLink::Load(const CString& sFilename)
{
  if (!Initialise())
    return FALSE;

  //Validate our parameters
  ASSERT(!sFilename.IsEmpty());
  ASSERT(m_psl);
  ASSERT(m_ppf);

  BOOL bSuccess = FALSE;

  //Convert the path to a UNICODE string
  WCHAR wszPath[MAX_PATH];
#ifndef _UNICODE
  ::MultiByteToWideChar(CP_ACP, 0, sFilename, -1, wszPath, MAX_PATH);
#else
  _tcscpy(wszPath, sFilename);
#endif

  //Load the link from file
  HRESULT hRes = m_ppf->Load(wszPath, STGM_READ);
  if (SUCCEEDED(hRes))
  {
    //Get the various link values
    TCHAR szBuf[_MAX_PATH];
    WIN32_FIND_DATA fd;

    hRes = m_psl->GetPath(szBuf, _MAX_PATH, &fd, SLGP_UNCPRIORITY);
    if (SUCCEEDED(hRes))
      m_sli.m_sTarget = szBuf;
    else
      TRACE(_T("CShellLink::Load, Failed in call to IShellLink::GetPath, HRESULT was %x\n"), hRes);

    hRes = m_psl->GetIDList(&m_sli.m_pidl);
    if (!SUCCEEDED(hRes))
      TRACE(_T("CShellLink::Load, Failed in call to IShellLink::GetIDList, HRESULT was %x\n"), hRes);

    hRes = m_psl->GetWorkingDirectory(szBuf, _MAX_PATH);
    if (SUCCEEDED(hRes))
      m_sli.m_sWorkingDirectory = szBuf;
    else
      TRACE(_T("CShellLink::Load, Failed in call to IShellLink::GetWorkingDirectory, HRESULT was %x\n"), hRes);

    hRes = m_psl->GetIconLocation(szBuf, _MAX_PATH, &m_sli.m_nIconIndex);
    if (SUCCEEDED(hRes))
      m_sli.m_sIconLocation = szBuf;
    else
      TRACE(_T("CShellLink::Load, Failed in call to IShellLink::GetIconLocation, HRESULT was %x\n"), hRes);

    hRes = m_psl->GetDescription(szBuf, _MAX_PATH);
    if (SUCCEEDED(hRes))            
      m_sli.m_sDescription = szBuf;
    else
      TRACE(_T("CShellLink::Load, Failed in call to IShellLink::GetDescription, HRESULT was %x\n"), hRes);

    hRes = m_psl->GetArguments(szBuf, _MAX_PATH);
    if (SUCCEEDED(hRes))
      m_sli.m_sArguments = szBuf;
    else
      TRACE(_T("CShellLink::Load, Failed in call to IShellLink::GetArguments, HRESULT was %x\n"), hRes);

    hRes = m_psl->GetHotkey(&m_sli.m_wHotkey);
    if (!SUCCEEDED(hRes))
      TRACE(_T("CShellLink::Load, Failed in call to IShellLink::GetHotkey, HRESULT was %x\n"), hRes);

    hRes = m_psl->GetShowCmd(&m_sli.m_nShowCmd);
    if (!SUCCEEDED(hRes))
      TRACE(_T("CShellLink::Load, Failed in call to IShellLink::GetShowCmd, HRESULT was %x\n"), hRes);

    bSuccess = TRUE;
  }
  else
    TRACE(_T("CShellLink::Load, Failed in call to IPersistFile::Load, HRESULT was %x\n"), hRes);

  return bSuccess;
}

BOOL CShellLink::Resolve(CWnd* pParentWnd, DWORD dwFlags)
{
  if (!Initialise())
    return FALSE;

  //Validate our parameters
  ASSERT(!m_sli.m_sTarget.IsEmpty());
  ASSERT(m_psl);
  ASSERT(m_ppf);

  BOOL bSuccess = FALSE;

  //Do the actual link resolve
  HWND hWnd = NULL;
  if (pParentWnd)
    hWnd = pParentWnd->GetSafeHwnd();
  HRESULT hRes = m_psl->Resolve(hWnd, dwFlags);

  if (SUCCEEDED(hRes))
    bSuccess = TRUE;
  else
    TRACE(_T("CShellLink::Resolve, Failed in call to IShellLink::Resolve, HRESULT was %x\n"), hRes);

  return bSuccess;
}

CString CShellLink::GetPath() const
{
  return m_sli.m_sTarget;
}

CString CShellLink::GetArguments() const
{
  return m_sli.m_sTarget;
}

CString CShellLink::GetDescription() const
{
  return m_sli.m_sDescription;
}

WORD CShellLink::GetHotKey() const
{
  return m_sli.m_wHotkey;
}

CString CShellLink::GetIconLocation() const
{
  return m_sli.m_sIconLocation;
}

int CShellLink::GetIconLocationIndex() const
{
  return m_sli.m_nIconIndex;
}

LPITEMIDLIST CShellLink::GetPathIDList() const
{
  return m_sli.m_pidl;
}

int CShellLink::GetShowCommand() const
{
  return m_sli.m_nShowCmd;
}

CString CShellLink::GetWorkingDirectory() const
{
  return m_sli.m_sWorkingDirectory;
}

void CShellLink::SetPath(const CString& sPath)
{
  m_sli.m_sTarget = sPath;
}

void CShellLink::SetArguments(const CString& sArguments)
{
  m_sli.m_sArguments = sArguments;
}

void CShellLink::SetDescription(const CString& sDescription)
{
  m_sli.m_sDescription = sDescription;
}

void CShellLink::SetHotKey(WORD wHotkey)
{
  m_sli.m_wHotkey = wHotkey;
}

void CShellLink::SetIconLocation(const CString& sIconLocation)
{
  m_sli.m_sIconLocation = sIconLocation;
}

void CShellLink::SetIconLocationIndex(int nIconIndex)
{
  m_sli.m_nIconIndex = nIconIndex;
}

void CShellLink::SetPathIDList(LPITEMIDLIST pidl)
{
  m_sli.m_pidl = pidl;
}

void CShellLink::SetShowCommand(int nShowCmd)
{
  m_sli.m_nShowCmd = nShowCmd;
}

void CShellLink::SetWorkingDirectory(const CString& sWorkingDirectory)
{
  m_sli.m_sWorkingDirectory = sWorkingDirectory;
}


CUrlShellLink::CUrlShellLink()
{
  m_pURL = NULL;
}

BOOL CUrlShellLink::Create(const CShellLinkInfo& sli)
{
  //Validate our parameters
  ASSERT(sli.m_sArguments.IsEmpty()); //Arguments are not supported for Internet shortcuts
  ASSERT(sli.m_pidl == NULL);         //pidls are not supported for Internet shortcuts

  if (!Initialise())
    return FALSE;

  m_sli = sli;

  return TRUE;
}

CUrlShellLink::~CUrlShellLink()
{
  if (m_pURL)
    m_pURL->Release();
}

BOOL CUrlShellLink::Initialise()
{
  BOOL bSuccess = FALSE;
  if (m_bAttemptedInitialise)
    bSuccess = (m_pURL != NULL);
  else
  {
    //Instantiate the COM class
    HRESULT hRes = ::CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER, IID_IUniformResourceLocator, (LPVOID*) &m_pURL);
    if (SUCCEEDED(hRes))
    {
      //Also get a pointer to IPersistFile
      hRes = m_pURL->QueryInterface(IID_IPersistFile, (LPVOID*) &m_ppf);
      if (SUCCEEDED(hRes))
      {
        //Also get a pointer to IShellLink
        hRes = m_pURL->QueryInterface(IID_IShellLink, (LPVOID*) &m_psl);
        if (SUCCEEDED(hRes))
          bSuccess = TRUE;
        else
          TRACE(_T("CUrlShellLink::Initialise, Failed in call to QueryInterface for IShellLink, HRESULT was %x\n"), hRes);        
      }
      else
        TRACE(_T("CUrlShellLink::Initialise, Failed in call to QueryInterface for IPersistFile, HRESULT was %x\n"), hRes);        
    }
    else
      TRACE(_T("CUrlShellLink::Initialise, Failed in call to CoCreateInstance for IInternetShortcut, HRESULT was %x\n"), hRes);

    m_bAttemptedInitialise = TRUE;
  }

  return bSuccess;
}

BOOL CUrlShellLink::Save(const CString& sFilename)
{
  if (!Initialise())
    return FALSE;

  //Validate our parameters
  ASSERT(!sFilename.IsEmpty());
  ASSERT(!m_sli.m_sTarget.IsEmpty());

  //URL Links do not support arguments, everthing should be 
  //included in m_sli.m_sTarget and m_sli.m_sArguments should
  //always be empty
  ASSERT(m_sli.m_sArguments.IsEmpty());

  ASSERT(m_pURL);
  ASSERT(m_psl);
  ASSERT(m_ppf);

  BOOL bSuccess = FALSE;

  //Convert the path to a UNICODE string
  WCHAR wszPath[_MAX_PATH];
#ifndef _UNICODE
  MultiByteToWideChar(CP_ACP, 0, sFilename, -1, wszPath, _MAX_PATH);
#else
  _tcscpy(wszPath, sFilename);
#endif

  //Set the various arguments
  HRESULT hRes = m_psl->SetWorkingDirectory(m_sli.m_sTarget);
  if (!SUCCEEDED(hRes))
    TRACE(_T("CUrlShellLink::Save, Failed in call to IShellLink::SetPath, HRESULT was %x\n"), hRes);

  hRes = m_psl->SetWorkingDirectory(m_sli.m_sWorkingDirectory);
  if (!SUCCEEDED(hRes))
    TRACE(_T("CUrlShellLink::Save, Failed in call to IShellLink::SetWorkingDirectory, HRESULT was %x\n"), hRes);

  hRes = m_psl->SetIconLocation(m_sli.m_sIconLocation, m_sli.m_nIconIndex);
  if (!SUCCEEDED(hRes))
    TRACE(_T("CUrlShellLink::Save, Failed in call to IShellLink::SetIconLocation, HRESULT was %x\n"), hRes);

  hRes = m_psl->SetDescription(m_sli.m_sDescription);
  if (!SUCCEEDED(hRes))
    TRACE(_T("CUrlShellLink::Save, Failed in call to IShellLink::SetDescription, HRESULT was %x\n"), hRes);

  hRes = m_psl->SetHotkey(m_sli.m_wHotkey);
  if (!SUCCEEDED(hRes))
    TRACE(_T("CUrlShellLink::Save, Failed in call to IShellLink::SetHotkey, HRESULT was %x\n"), hRes);

  hRes = m_psl->SetShowCmd(m_sli.m_nShowCmd);
  if (!SUCCEEDED(hRes))
    TRACE(_T("CUrlShellLink::Save, Failed in call to IShellLink::SetShowCmd, HRESULT was %x\n"), hRes);

  //Save the link to file
  hRes = m_ppf->Save(wszPath, TRUE);
  if (SUCCEEDED(hRes))
    bSuccess = TRUE;
  else
    TRACE(_T("CUrlShellLink::Save, Failed in call to IPersistFile::Save, HRESULT was %x\n"), hRes);

  return bSuccess;
}

BOOL CUrlShellLink::Load(const CString& sFilename)
{
  if (!Initialise())
    return FALSE;

  //Validate our parameters
  ASSERT(!sFilename.IsEmpty());
  ASSERT(m_pURL);
  ASSERT(m_psl);
  ASSERT(m_ppf);

  BOOL bSuccess = FALSE;

  //Convert the path to a UNICODE string
  WCHAR wszPath[MAX_PATH];
#ifndef _UNICODE
  ::MultiByteToWideChar(CP_ACP, 0, sFilename, -1, wszPath, MAX_PATH);
#else
  _tcscpy(wszPath, sFilename);
#endif

  //Load the link from file
  HRESULT hRes = m_ppf->Load(wszPath, STGM_READ);
  if (SUCCEEDED(hRes))
  {
    //Get the various link values
    TCHAR szBuf[_MAX_PATH];
    WIN32_FIND_DATA fd;

    hRes = m_psl->GetPath(szBuf, _MAX_PATH, &fd, SLGP_UNCPRIORITY);
    if (SUCCEEDED(hRes))
      m_sli.m_sTarget = szBuf;
    else
      TRACE(_T("CUrlShellLink::Load, Failed in call to IShellLink::GetPath, HRESULT was %x\n"), hRes);

    hRes = m_psl->GetWorkingDirectory(szBuf, _MAX_PATH);
    if (SUCCEEDED(hRes))
      m_sli.m_sWorkingDirectory = szBuf;
    else
      TRACE(_T("CUrlShellLink::Load, Failed in call to IShellLink::GetWorkingDirectory, HRESULT was %x\n"), hRes);

    hRes = m_psl->GetIconLocation(szBuf, _MAX_PATH, &m_sli.m_nIconIndex);
    if (SUCCEEDED(hRes))
      m_sli.m_sIconLocation = szBuf;
    else
      TRACE(_T("CUrlShellLink::Load, Failed in call to IShellLink::GetIconLocation, HRESULT was %x\n"), hRes);

    //WINBUG: URL shortcuts always seem to return a desciption the same as the name of
    //file in which the shortcut is stored
    hRes = m_psl->GetDescription(szBuf, _MAX_PATH);
    if (SUCCEEDED(hRes))            
      m_sli.m_sDescription = szBuf;
    else
      TRACE(_T("CUrlShellLink::Load, Failed in call to IShellLink::GetDescription, HRESULT was %x\n"), hRes);

    hRes = m_psl->GetHotkey(&m_sli.m_wHotkey);
    if (!SUCCEEDED(hRes))
      TRACE(_T("CUrlShellLink::Load, Failed in call to IShellLink::GetHotkey, HRESULT was %x\n"), hRes);

    hRes = m_psl->GetShowCmd(&m_sli.m_nShowCmd);
    if (!SUCCEEDED(hRes))
      TRACE(_T("CUrlShellLink::Load, Failed in call to IShellLink::GetShowCmd, HRESULT was %x\n"), hRes);

    bSuccess = TRUE;
  }
  else
    TRACE(_T("CUrlShellLink::Load, Failed in call to IPersistFile::Load, HRESULT was %x\n"), hRes);

  return bSuccess;
}

BOOL CUrlShellLink::Invoke(CWnd* pParentWnd, DWORD dwFlags, const CString& sVerb)
{
  //Validate our parameters
  ASSERT(!m_sli.m_sTarget.IsEmpty());
  ASSERT(m_pURL);
  ASSERT(m_psl);
  ASSERT(m_ppf);

  BOOL bSuccess = FALSE;

  URLINVOKECOMMANDINFO urlicmi;
  urlicmi.dwcbSize = sizeof(URLINVOKECOMMANDINFO);
  urlicmi.dwFlags = dwFlags;  
  urlicmi.hwndParent = NULL;
  if (pParentWnd)
    urlicmi.hwndParent = pParentWnd->GetSafeHwnd();          
  urlicmi.pcszVerb = sVerb;

  //Invoke the verb on the URL
  HRESULT hRes = m_pURL->InvokeCommand(&urlicmi);
  if (SUCCEEDED(hRes))
    bSuccess = TRUE;
  else
    TRACE(_T("CUrlShellLink::Invoke, Failed in call to IUniformResourceLocator::Invoke, HRESULT was %x\n"), hRes);

  return bSuccess;
}

void CUrlShellLink::SetArguments(const CString& /*sArguments*/)
{
  //Arguments are not supported for Internet shortcuts
  ASSERT(FALSE); 
}

CString CUrlShellLink::GetArguments() const
{
  ASSERT(FALSE);  //Arguments are not supported for Internet shortcuts
  return CString();
}

LPITEMIDLIST CUrlShellLink::GetPathIDList() const
{
  //pidls are not supported for Internet shortcuts
  ASSERT(FALSE); 
  return NULL;
}

void CUrlShellLink::SetPathIDList(LPITEMIDLIST /*pidl*/)
{
  //pidls are not supported for Internet shortcuts
  ASSERT(FALSE); 
}

