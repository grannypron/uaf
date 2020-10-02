/*
Module : SHELLLINK.H
Purpose: Defines the interface for 2 MFC shell wrapper classes, one is for
         IShellLink and the other is for IUniformResourceLocator
Created: PJN / 22-05-1998
History: None

Copyright (c) 1998 by PJ Naughter.  
All rights reserved.

 ********
 ********  This code downloaded from http://www.codeproject.com
 ********

*/


/////////////////////////////// Defines ///////////////////////////////////////
#ifndef __SHELLLINK_H__
#define __SHELLLINK_H__



/////////////////////////////// Includes //////////////////////////////////////
#include <shlobj.h>
#include <intshcut.h>



/////////////////////////////// Classes ///////////////////////////////////////


//Class which contains all the parameters related to shortcut
class CShellLinkInfo //: public CObject
{
public:
//Constructors / Destructors
  CShellLinkInfo();
  CShellLinkInfo(const CShellLinkInfo& sli);
  ~CShellLinkInfo();

//Methods 
  CShellLinkInfo& operator=(const CShellLinkInfo& sli);

//Diagnostic support
#ifdef _DEBUG
  //virtual void Dump(CDumpContext& dc);
#endif

//Variables
  CString      m_sTarget;
  LPITEMIDLIST m_pidl;
  CString      m_sArguments;
  CString      m_sDescription;
  WORD         m_wHotkey;
  CString      m_sIconLocation;
  int          m_nIconIndex;
  int          m_nShowCmd;
  CString      m_sWorkingDirectory;
};


//Class which wraps standard shortcuts i.e. IShellLink
class CShellLink
{
public:
//Constructors / Destructors
  CShellLink();
  virtual ~CShellLink();

//Methods
  BOOL Create(const CShellLinkInfo& sli);
  BOOL Load(const CString& sFilename);
  BOOL Save(const CString& sFilename);
  BOOL Resolve(CWnd* pParentWnd, DWORD dwFlags);

//Accessors
  CString      GetPath() const;
  LPITEMIDLIST GetPathIDList() const;
  CString      GetArguments() const;
  CString      GetDescription() const;
  WORD         GetHotKey() const;
  CString      GetIconLocation() const;
  int          GetIconLocationIndex() const;
  int          GetShowCommand() const;
  CString      GetWorkingDirectory() const;

//Mutators
  void SetPath(const CString& sPath);
  void SetPathIDList(LPITEMIDLIST pidl);
  void SetArguments(const CString& sArguments);
  void SetDescription(const CString& sDescription);
  void SetHotKey(WORD wHotkey);
  void SetIconLocation(const CString& sIconLocation);
  void SetIconLocationIndex(int nIconIndex);
  void SetShowCommand(int nShowCmd);
  void SetWorkingDirectory(const CString& sWorkingDirectory);

protected:
  BOOL Initialise();
  CShellLinkInfo m_sli;
  IShellLink*    m_psl;
  IPersistFile*  m_ppf;
  BOOL           m_bAttemptedInitialise;
};


//Class which wraps internet shortcuts i.e. IUniformResourceLocator
class CUrlShellLink : public CShellLink
{
public:
//Constructors / Destructors
  CUrlShellLink();
  ~CUrlShellLink();

//Methods
  BOOL Create(const CShellLinkInfo& sli);
  BOOL Load(const CString& sFilename);
  BOOL Save(const CString& sFilename);
  BOOL Invoke(CWnd* pParentWnd, DWORD dwFlags, const CString& sVerb);

//Following 4 functions just ASSERT if called
  CString GetArguments() const;
  LPITEMIDLIST GetPathIDList() const;
  void SetArguments(const CString& sArguments);
  void SetPathIDList(LPITEMIDLIST pidl);

protected:
  BOOL Initialise();
  IUniformResourceLocator* m_pURL;
};


#endif //__SHELLLINK_H__

