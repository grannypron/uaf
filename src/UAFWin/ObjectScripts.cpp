#include "stdafx.h"

#include "externs.h"

#include "ObjectScripts.h"



#include "GPDLOpCodes.h"
#include "GPDLcomp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void die(int);

CObjectScript::CObjectScript() :
  m_Compiled(false)
{

}

CObjectScript::~CObjectScript()
{
  Clear();
}

void CObjectScript::Clear()
{
  m_Compiled = false;
  m_Source.Empty();
  m_Binary.Empty();
}

bool CObjectScript::CompileScript(CString funcname)
{

  m_Compiled = false;
  m_Binary.Empty();

  if (!HaveScript()) 
    return true;

  m_Compiled = true;

#ifdef UAFEDITOR 

  const char *allowedEntry[2];  
    
  CString scriptbegin;
  CString scriptend;

  scriptbegin.Format("$PUBLIC $FUNC %s() { ", funcname);
  scriptend.Format(" } %s;", funcname);

  CString ActualScript = scriptbegin + m_Source + scriptend;
  
  allowedEntry[0] = (LPCSTR)funcname;
  allowedEntry[1] = NULL;     

  GPDLCOMP gpdlComp;
  m_Binary = gpdlComp.CompileScript(ActualScript, allowedEntry);
  
  if (m_Binary[0] != 0) 
  {
    m_Compiled = false;
    WriteDebugString("Failed to compile script: \'%s\'\n", funcname);
  }
  
#endif
  
  if (!m_Compiled)
    m_Binary.Empty();   
  
  return m_Compiled;
}

CObjectScripts::CObjectScripts(ObjectTypes SrcType) :
  m_ObjectType(SrcType)
{

}

CObjectScripts::~CObjectScripts()
{

}

CString CObjectScripts::GetFuncName(ObjectScriptAction action)
{
  CString funcname("");
  CString timestr;

  switch (m_ObjectType)
  {
  case ObjectItem: funcname = "i_"; break;
  case ObjectSpell: funcname = "s_"; break;
  case ObjectMonster: funcname = "m_"; break;
  case ObjectCharacter: funcname = "c_"; break;    
  default: die(0xab4e3);
  }

  switch (action)
  {
  case OSA_Activate: funcname += "act_"; break;
  case OSA_Deactivate: funcname += "deact_"; break;
  case OSA_SavingThrowAttempted: funcname += "st_attempt"; break;
  case OSA_SavingThrowSuccess: funcname += "st_success"; break;
  case OSA_SavingThrowFailed: funcname += "st_failed"; break;

  default: die(0xab4e4);
  }

  timestr.Format("%u", GetTickCount());
  funcname += timestr;

  return funcname;
}


bool CObjectScripts::CompileScripts(CString basename)
{
  CString funcname(basename); // start with basename
  funcname.Remove(' ');       // remove spaces from name
  funcname.Replace('|', '_'); // replace separator with underscore

  bool success = true;

  POSITION pos = m_scripts.GetHeadPosition();
  while (pos != NULL)
  {
    funcname += GetFuncName((ObjectScriptAction)m_scripts.GetKeyAt(pos));

    m_scripts.GetAtPos(pos).CompileScript(funcname);
    if (!m_scripts.PeekAtPos(pos).IsCompiled())
      success = false;

    m_scripts.GetNext(pos);
  }

  return success;
}


CObjectScript *CObjectScripts::GetScript(ObjectScriptAction action)
{
  POSITION pos;
  if ((pos = m_scripts.FindKeyPos((DWORD)action)) == NULL)
    return NULL;
  
  return (&m_scripts.GetAtPos(pos));
}

bool CObjectScripts::AddScript(ObjectScriptAction action)
{
  CObjectScript script; // empty, no script yet
  return (AddScript(script, action));
}

bool CObjectScripts::AddScript(CObjectScript &script, ObjectScriptAction action)
{
  POSITION pos;
  if ((pos = m_scripts.FindKeyPos((DWORD)action)) == NULL)
    return (m_scripts.Insert(script, (DWORD)action)?true:false);

  return false;
}
