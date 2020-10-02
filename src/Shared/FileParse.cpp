/******************************************************************************
* Filename: FileParse.cpp
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

#ifdef UAFEDITOR
#else
#endif
#include "FileParse.h"
#ifdef UAFEngine
#include "specab.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString emptyString;
extern A_CStringPAIR_L globalSA_debug;

FileParse ConfigFile;
A_CStringPAIR_L ConfigFile_DB;


BOOL FindToken(FileParse& file, CString Token, CString &Value, bool Remove)
{
  return file.FindToken(Token, Value, Remove);
}

FileParse& FileParse::operator =(const FileParse& src)
{
  if (&src == this)
    return *this;
  //m_file = src.m_file; // file shouldn't be open during operator=()
  m_filename = src.m_filename;
  memcpy(m_tempToken, src.m_tempToken, sizeof(m_tempToken));
  m_fileError = "";
  m_tlist.RemoveAll();
  for (int i=0;i<src.m_tlist.GetSize();i++)
  {
    TokenDataType temp = src.m_tlist[i];
    m_tlist.SetAtGrow(i, temp);
  }
  return *this;
}  

BOOL FileParse::GetTokenAt(int index, CString &Token, CString &Value)
{
  if ((index < 0) || (index >= GetCount()))
    return FALSE;

  Token = m_tlist[index].Token;
  Value = m_tlist[index].Value;
  return TRUE;
}

DWORD FileParse::FlushTokens()
{
  DWORD count=0;
  for (int i=0;i<GetCount();i++)
  {
    if (m_tlist[i].Remove)
    {
      m_tlist.RemoveAt(i);
      count++;
    }
  }
  m_tlist.FreeExtra();
  return count;
}

BOOL FileParse::FindToken(CString Token, CString &Value, bool Remove)
{
  Value = "";
  for (int i=0;i<GetCount();i++)
  {
    if (m_tlist[i].Token.CompareNoCase(Token) == 0)
    {
      Value = m_tlist[i].Value;
      m_tlist[i].Remove = Remove;
      return TRUE;
    }
  }
  return FALSE;
}

BOOL FileParse::FindToken(CString Token, char *Value, int ValueLen, bool Remove)
{
  if ((Value == NULL) || (ValueLen <= 0))
    return FALSE;

  Value[0] = '\0';
  for (int i=0;i<GetCount();i++)
  {
    if (m_tlist[i].Remove) continue;
    if (m_tlist[i].Token.CompareNoCase(Token) == 0)
    {
      strncpy(Value, m_tlist[i].Value, ValueLen);
      Value[ValueLen-1] = '\0';
      m_tlist[i].Remove = Remove;
      return TRUE;
    }
  }
  return FALSE;
}

BOOL FileParse::FindToken(CString Token, WORD &Value, bool Remove)
{  
  if (FindToken(Token, m_tempToken, sizeof(m_tempToken),Remove))
  {
    Value = (WORD)atoi(m_tempToken);
    return TRUE;
  }
  return FALSE;
}

BOOL FileParse::FindToken(CString Token, BYTE &Value, bool Remove)
{
  if (FindToken(Token, m_tempToken, sizeof(m_tempToken),Remove))
  {
    WORD tempVal = (WORD)atoi(m_tempToken);
    Value = (tempVal & 0x00FF);
    return TRUE;
  }
  return FALSE;
}

BOOL FindConfigToken(const CString& name, CString& value)
{
  const CStringPAIR *pEntry;
  pEntry = ConfigFile_DB.Find(name);
  if (pEntry == NULL)
  {
    value.Empty();
    return false;
  }
  else
  {
    value = pEntry->Value();
  };
  return true;
}

BOOL FileParse::FindToken(CString Token, int &Value, bool Remove)
{
  if (FindToken(Token, m_tempToken, sizeof(m_tempToken),Remove))
  {
    Value = atoi(m_tempToken);
    return TRUE;
  }
  return FALSE;
}

BOOL FileParse::FindToken(CString Token, DWORD &Value, bool Remove)
{
  if (FindToken(Token, m_tempToken, sizeof(m_tempToken),Remove))
  {
    Value = (DWORD)atol(m_tempToken);
    return TRUE;
  }
  return FALSE;
}

BOOL FileParse::FindTokens(CString Token, int &Val1, int &Val2, int &Val3, int &Val4, bool Remove)
{
  char strval[MAX_VALUE_LEN+1];
  strval[0] = '\0';
  Val1=Val2=Val3=Val4=0;

  if (FindToken(Token, m_tempToken, sizeof(m_tempToken),Remove))
  {
    const char CommaChar = 44; // ,
    int len;

    char* PrevCommaIdx=NULL;
    char* CommaIdx = strchr(m_tempToken, CommaChar);

    if (CommaIdx != NULL)
    {
      len = CommaIdx-m_tempToken;
      if (len < MAX_VALUE_LEN-1)
      {
        strncpy(strval, m_tempToken, len);
        strval[len] = '\0';
        Val1 = atol(strval);
      }
    }
    else
      return FALSE;

    PrevCommaIdx = CommaIdx+1;
    CommaIdx = strchr(PrevCommaIdx, CommaChar);

    if (CommaIdx != NULL)
    {
      len = CommaIdx-PrevCommaIdx;
      if (len < MAX_VALUE_LEN-1)
      {
        strncpy(strval, PrevCommaIdx, len);
        strval[len] = '\0';
        Val2 = atol(strval);
      }
    }
    else
      return FALSE;
    
    PrevCommaIdx = CommaIdx+1;
    CommaIdx = strchr(PrevCommaIdx, CommaChar);

    if (CommaIdx != NULL)
    {
      len = CommaIdx-PrevCommaIdx;
      if (len < MAX_VALUE_LEN-1)
      {
        strncpy(strval, PrevCommaIdx, len);
        strval[len] = '\0';
        Val3 = atol(strval);
      }
    }
    else
      return FALSE;

    PrevCommaIdx = CommaIdx+1;
    strncpy(strval, PrevCommaIdx, MAX_VALUE_LEN);
    strval[MAX_VALUE_LEN-1] = '\0';
    Val4 = atol(strval);    

    return TRUE;
  }
  return FALSE;
}

BOOL FileParse::FindTokens(CString Token, int &Val1, int &Val2, int &Val3, bool Remove)
{
  char strval[MAX_VALUE_LEN+1];
  strval[0] = '\0';
  Val1=Val2=Val3=0;

  if (FindToken(Token, m_tempToken, sizeof(m_tempToken),Remove))
  {
    const char CommaChar = 44; // ,
    int len;

    char* PrevCommaIdx=NULL;
    char* CommaIdx = strchr(m_tempToken, CommaChar);

    if (CommaIdx != NULL)
    {
      len = CommaIdx-m_tempToken;
      if (len < MAX_VALUE_LEN-1)
      {
        strncpy(strval, m_tempToken, len);
        strval[len] = '\0';
        Val1 = atol(strval);
      }
    }
    else
      return FALSE;

    PrevCommaIdx = CommaIdx+1;
    CommaIdx = strchr(PrevCommaIdx, CommaChar);

    if (CommaIdx != NULL)
    {
      len = CommaIdx-PrevCommaIdx;
      if (len < MAX_VALUE_LEN-1)
      {
        strncpy(strval, PrevCommaIdx, len);
        strval[len] = '\0';
        Val2 = atol(strval);
      }
    }
    else
      return FALSE;
    
    PrevCommaIdx = CommaIdx+1;
    strncpy(strval, PrevCommaIdx, MAX_VALUE_LEN);
    strval[MAX_VALUE_LEN-1] = '\0';
    Val3 = atol(strval);    

    return TRUE;
  }
  return FALSE;
}

BOOL FileParse::FindTokens(CString Token, int &Val1, int &Val2, bool Remove)
{
  char strval[MAX_VALUE_LEN+1];
  strval[0] = '\0';
  Val1=Val2=0;

  if (FindToken(Token, m_tempToken, sizeof(m_tempToken),Remove))
  {
    const char CommaChar = 44; // ,
    int len;

    char* PrevCommaIdx=NULL;
    char* CommaIdx = strchr(m_tempToken, CommaChar);

    if (CommaIdx != NULL)
    {
      len = CommaIdx-m_tempToken;
      if (len < MAX_VALUE_LEN-1)
      {
        strncpy(strval, m_tempToken, len);
        strval[len] = '\0';
        Val1 = atol(strval);
      }
    }
    else
      return FALSE;

    PrevCommaIdx = CommaIdx+1;
    strncpy(strval, PrevCommaIdx, MAX_VALUE_LEN);
    strval[MAX_VALUE_LEN-1] = '\0';
    Val2 = atol(strval);    

    return TRUE;
  }
  return FALSE;
}

BOOL FileParse::FindTokens(CString Token,
                           int& Val1,
                           CString& Val2,
                           int& Val3, int& Val4, int& Val5,
                           bool Remove)
{
  char strval[MAX_VALUE_LEN + 1];
  strval[0] = '\0';
  Val2.Empty();
  Val1 = Val3 = Val4 = Val5 = 0;

  if (FindToken(Token, m_tempToken, sizeof(m_tempToken), Remove))
  {
    const char CommaChar = 44; // ,
    int len;

    char* PrevCommaIdx = NULL;
    char* CommaIdx = strchr(m_tempToken, CommaChar);

    if (CommaIdx != NULL)
    {
      len = CommaIdx - m_tempToken;
      if (len < MAX_VALUE_LEN - 1)
      {
        strncpy(strval, m_tempToken, len);
        strval[len] = '\0';
        Val1 = atol(strval);
      }
    }
    else
      return FALSE;

    PrevCommaIdx = CommaIdx + 1;
    CommaIdx = strchr(PrevCommaIdx, CommaChar);

    if (CommaIdx != NULL)
    {
      len = CommaIdx - PrevCommaIdx;
      if (len < MAX_VALUE_LEN - 1)
      {
        strncpy(strval, PrevCommaIdx, len);
        strval[len] = '\0';
        Val2 = strval;
      }
    }
    else
      return FALSE;

    PrevCommaIdx = CommaIdx + 1;
    CommaIdx = strchr(PrevCommaIdx, CommaChar);

    if (CommaIdx != NULL)
    {
      len = CommaIdx - PrevCommaIdx;
      if (len < MAX_VALUE_LEN - 1)
      {
        strncpy(strval, PrevCommaIdx, len);
        strval[len] = '\0';
        Val3 = atol(strval);
      }
    }
    else
      return FALSE;

    PrevCommaIdx = CommaIdx + 1;
    CommaIdx = strchr(PrevCommaIdx, CommaChar);

    if (CommaIdx != NULL)
    {
      len = CommaIdx - PrevCommaIdx;
      if (len < MAX_VALUE_LEN - 1)
      {
        strncpy(strval, PrevCommaIdx, len);
        strval[len] = '\0';
        Val4 = atol(strval);
      }
    }
    else
      return FALSE;

    PrevCommaIdx = CommaIdx + 1;
    strncpy(strval, PrevCommaIdx, MAX_VALUE_LEN);
    strval[MAX_VALUE_LEN - 1] = '\0';
    Val5 = atol(strval);

    return TRUE;
  }
  return FALSE;
}

BOOL FileParse::FindTokens(CString Token, int& Val1, CString& Val2, bool Remove)
{
  char strval[MAX_VALUE_LEN + 1];
  strval[0] = '\0';
  Val1 = 0;
  Val2.Empty();

  if (FindToken(Token, m_tempToken, sizeof(m_tempToken), Remove))
  {
    const char CommaChar = 44; // ,
    int len;

    char* PrevCommaIdx = NULL;
    char* CommaIdx = strchr(m_tempToken, CommaChar);

    if (CommaIdx != NULL)
    {
      len = CommaIdx - m_tempToken;
      if (len < MAX_VALUE_LEN - 1)
      {
        strncpy(strval, m_tempToken, len);
        strval[len] = '\0';
        Val1 = atol(strval);
      }
    }
    else
      return FALSE;

    PrevCommaIdx = CommaIdx + 1;
    strncpy(strval, PrevCommaIdx, MAX_VALUE_LEN);
    strval[MAX_VALUE_LEN - 1] = '\0';
    Val2 = strval;

    return TRUE;
  };
  return FALSE;
}


CString GetSaName(CString src)
{
  int i;
  i = src.GetLength();
  if (i < 3) return emptyString;
  if (src[0] != '"') return emptyString;
  if (src[i-1] != '"') return emptyString;
  return src.Mid(1,i-2);
}

BOOL FileParse::Parse(const char *filename, int SearchDepth)
{
  Clear();

  BOOL result = TRUE;
  char *EqIndex;
  TokenDataType data;
 
  const char EqualsChar = 61; // =
  const char Comment[] = "//";

  char buffer[1025];
  int len;
  int count=0;
  CFileException fexception;

  char path[_MAX_PATH+1];
  CString temp;
  BOOL found = FALSE;

  strcpy(path, filename);

  len = strlen(path);
  if ((len > 0) && (path[len-1] == '\\'))
  {
    path[len-1] = 0;
  };
  // search current folder, and then higher folders up
  // to search depth
  if (FileExists(path))
  {
    found = TRUE;
  }
  else
  {
    CString msg;
    msg.Format("Did not find config file at: \"%s\"\n", path);
    WriteDebugString(msg);
    found = false;
  };
  if (!found)
  {
    WriteDebugString("Unable to locate config.txt file\n");
    return FALSE;
  }

  count = 0;

  try
  {
    if (m_file.Open(path, (CFile::modeRead | CFile::shareDenyNone), &fexception))
    {
      // save filename 
      m_filename = path;

      // read the first line and discard it.
#ifdef UAFEngine
      globalSA_debug.Clear();
#endif
      while (m_file.ReadString(buffer, 1024))
      {
        if (buffer[0] == '$')
        {
#ifdef UAFEngine
          continue;
#endif
#ifdef UAFEDITOR
#endif
        };
        if (strncmp(buffer, Comment, strlen(Comment)) != 0)
        {
          EqIndex = strchr(buffer, EqualsChar);

          if (  (EqIndex == NULL) && (strlen(buffer)<1000))
          {
            strcat(buffer, " = 0");
            EqIndex = strchr(buffer, EqualsChar);
          };

          if (EqIndex != NULL)
          {
            len = EqIndex-buffer;
            strncpy(m_tempToken, buffer, len);
            m_tempToken[len-1] = '\0';
            data.Token = m_tempToken;

            strncpy(m_tempToken, EqIndex+1, MAX_VALUE_LEN);
            m_tempToken[MAX_VALUE_LEN-1] = '\0';
            data.Value = m_tempToken;

            // trim space,newline,tabs
            data.Token.TrimRight();
            data.Value.TrimRight();
            data.Token.TrimLeft();
            data.Value.TrimLeft();
#ifdef UAFEngine
            if (data.Token == "DebugSpecialAbility")
            {
              // The Value should be a quoted string
              CString saName, saFile;
              saName = GetSaName(data.Value);
              if (saName.IsEmpty())
              {
                WriteDebugString("Illegal 'DebugSpecialAbility' found in config file");
              }
              else
              {
                globalSA_debug.Insert(saName, emptyString);
              };
              continue;
            };
#endif
            m_tlist.Add(data);
            count++;
          }
        }
      }

      if (m_file.m_hFile != CFile::hFileNull)
        m_file.Close();
    }
    else
    {
      SetFileError(&fexception);
      result = FALSE;
    }
  }
  catch (CFileException* pFE)
  {
    SetFileError(pFE);
    pFE->Delete();
    if (m_file.m_hFile != CFile::hFileNull)
      m_file.Close();
    result = FALSE;
  }
  catch (...)
  {
    WriteDebugString("Unhandled exception in FileParse::Parse(%s)\n",path);
    result = FALSE;
  }

  if (m_file.m_hFile != CFile::hFileNull)
    m_file.Close();

//  sprintf(m_tempToken, "%u tokens read from file %s\n", 
//          count, path);
//  WriteDebugString(m_tempToken);


  TRACE("Config file found at %s\n%u tokens read from file.\n", 
          m_file.GetFilePath(), count);

  return result;
}

void FileParse::SetFileError(CFileException* pFE)
{
  switch (pFE->m_cause)
  {
    case	CFileException::generic:
      m_fileError = "Generic";
      break;
    case	CFileException::fileNotFound:
      m_fileError = "File not found";
      break;
    case	CFileException::badPath:
      m_fileError = "Invalid path";
      break;
    case	CFileException::tooManyOpenFiles:
      m_fileError = "Too many open files";
      break;
    case	CFileException::accessDenied:
      m_fileError = "Access denied";
      break;
    case	CFileException::invalidFile:
      m_fileError = "Invalid file";
      break;
    case	CFileException::badSeek:
      m_fileError = "Bad seek";
      break;
    case	CFileException::hardIO:
      m_fileError = "Hardware I/O fault";
      break;
    case	CFileException::sharingViolation:
      m_fileError = "Sharing violation";
      break;
    case	CFileException::lockViolation:
      m_fileError = "Lock violation";
      break;
    case	CFileException::endOfFile:
      m_fileError = "End of file";
      break;
    default:
      m_fileError = "Unknown";
  }

  CString errorStr;
  errorStr.Format("FileParse: File error '%s' for file '%s'\n", 
                  m_fileError, pFE->m_strFileName);
  WriteDebugString(errorStr);
}

