#ifndef __OBJECT_SCRIPTS_H__
#define __OBJECT_SCRIPTS_H__

#include "..\Shared\stdafx.h"
#include "..\Shared\SharedQueue.h"
#include "Externs.h"

class CObjectScript
{
  bool m_Compiled;
  
public:
  CObjectScript();
  ~CObjectScript();
  
  void Clear();
  
  CString m_Source;
  CString m_Binary;

  bool HaveScript() const { return (m_Source.IsEmpty()?false:true); }
  bool IsCompiled() const { return m_Compiled; }

  bool CompileScript(CString funcname);  
};

class CObjectScripts
{
public:

  enum ObjectScriptAction
  {
    OSA_Activate,
    OSA_Deactivate,
    OSA_SavingThrowAttempted,
    OSA_SavingThrowSuccess,
    OSA_SavingThrowFailed,
    OSA_NumActions // must be last enum
  };

private:

  ObjectTypes m_ObjectType;
  OrderedQueue<CObjectScript, OSA_NumActions> m_scripts;
  
  CString GetFuncName(ObjectScriptAction action);

public:
  
  CObjectScripts(ObjectTypes SrcType);
  ~CObjectScripts();
  

  CObjectScript *GetScript(ObjectScriptAction action);

  bool CObjectScripts::AddScript(ObjectScriptAction action);
  bool CObjectScripts::AddScript(CObjectScript &script, ObjectScriptAction action);

  bool CompileScripts(CString basename);

};

#endif // __OBJECT_SCRIPTS_H__