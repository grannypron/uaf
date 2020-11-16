/******************************************************************************
* Filename: Spell.cpp
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
//#include "..\Shared\ProjectVersion.h"
#ifdef UAFEDITOR
#include "..\UAFwinEd\resource.h"
#else
#include "..\UAFWin\Resource.h"
#endif


//#ifdef UAFEDITOR
//#include "..\UAFWinEd\UAFWinEd.h"
//#else
//#include "externs.h"
//#include "..\UAFWin\dgngame.h"
//#endif

#include "SurfaceMgr.h"
#include "class.h"
#include "PicData.h"
#include "PicSlot.h"
#include "SoundMgr.h"
#include "spell.h"
#include "GlobalData.h"
#ifdef UAFEDITOR
#include "..\UAFwinEd\CrossReference.h"
#endif
#ifdef UAFEngine
#include "..\UAFWin\combatants.h"
#include "..\UAFWin\DrawTile.h"
#endif
#include "party.h"

#include "GPDLOpCodes.h"
#include "GPDLcomp.h"
#include "GPDLexec.h"
#include "..\UAFWin\FormattedText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString ActivationScriptName;
extern CString DeActivationScriptName;
extern BOOL logDebuggingInfo;

extern const double VersionSpellNames;
extern const double VersionSpellIDs;
extern const double VersionSaveIDs;
extern const double PRODUCT_VER;

// store/retrieve data from asl maps
//void StorePicDataAsASL(A_ASLENTRY_L &asl, PIC_DATA& data, const char *prefix, unsigned char flags=ASLF_EDITOR);
void RetrievePicDataFromASL(A_ASLENTRY_L &asl, PIC_DATA& data, const char *prefix);
void StoreIntAsASL(A_ASLENTRY_L &asl, int data, const char *prefix, unsigned char flags=ASLF_EDITOR);
void RetrieveIntFromASL(A_ASLENTRY_L &asl, int &data, const char *prefix);
void StoreStringAsASL(A_ASLENTRY_L &asl, CString &data, const char *prefix, unsigned char flags=ASLF_EDITOR);
void RetrieveStringFromASL(A_ASLENTRY_L &asl, CString &data, const char *prefix);
extern "C" unsigned long KeywordTextToNum(const char *keyword);



//IMPLEMENT_SERIAL( SPELL_DATA, CObject, 1 )
//IMPLEMENT_SERIAL( SPELL_DATA_TYPE, CObject, 1 )

SPELL_DATA_TYPE spellData;

#ifdef UAFEngine
SPELL_TEXT_LIST spellListText;
PENDING_SPELL_LIST pendingSpellList;
ACTIVE_SPELL_LIST activeSpellList;
void setSpellMemorizeItemState(CHARACTER &data);
CBRESULT ScriptCallback_CharDisplayStatus(CBFUNC func, CString *str, void *pkt);


#endif

GPDL spellGPDL;

void SCHOOL_LIST::Add(const SCHOOL_ID& schoolID)
{
  if (LocateSchool(schoolID) >= 0) return;
  schools.Add(const_cast<SCHOOL_ID&>(schoolID));
}

int SCHOOL_LIST::LocateSchool(const SCHOOL_ID& schoolID) const
{
  int i,n;
  n = schools.GetSize();
  for (i=0; i<n; i++)
  {
    if (*PeekSchool(i) == schoolID) return i;
  };
  return -1;
}


BOOL SPELL_ID::IsValidSpell(void) const
{
  //return spellData.FindUniqueName(*this) != NULL;
  return spellData.LocateSpell(*this) >= 0;

}

/*
void GLOBAL_SPELL_ID::Serialize(CAR& car, double version)
{
  if (car.IsStoring())
  {
    car << spellData.GetSpellData(*this)->Name());
  }
  else
    if (version < VersionSpellNames)
    {
      car >> m_key;
    }
    else
    {
      CString name;
      SPELL_DATA *pSpell;
      car >> name;
      pSpell = spellData.GetSpellData(name);
      if (pSpell == NULL)
      {

    };
  Not Implemented(0xc1a3ac, true);
};

void GLOBAL_SPELL_ID::Serialize(CArchive& ar, double version)
{
  Not Implemented(0xc1a3ad, true);
};
*/


///////////////////////////////////////////////////////////////////////
//
// SPELL_EFFECTS_DATA Functions
//
///////////////////////////////////////////////////////////////////////

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::SPELL_EFFECTS_DATA
//
// PURPOSE: 
//
//*****************************************************************************
SPELL_EFFECTS_DATA::SPELL_EFFECTS_DATA() 
{ 
  Clear(); 
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::SPELL_EFFECTS_DATA
//
// PURPOSE: 
//
//*****************************************************************************
SPELL_EFFECTS_DATA::SPELL_EFFECTS_DATA(const SPELL_EFFECTS_DATA &src) 
{ 
  *this=src; 
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::~SPELL_EFFECTS_DATA
//
// PURPOSE: 
//
//*****************************************************************************
SPELL_EFFECTS_DATA::~SPELL_EFFECTS_DATA() 
{ 
  //Clear(); 
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_EFFECTS_DATA::Serialize(CAR &ar, double version)
{ 
  if (ar.IsStoring())
  {
    AS(ar,m_string1);
    ar << flags;
    ar << changeResult;
    AS(ar,m_string2);
    ar << m_sourceOfEffect;
    ar << parent;
    AS(ar, m_string3);
    AS(ar, m_string4);
    AS(ar, m_string5);
    AS(ar, m_string6);
    AS(ar, m_string7); 
    AS(ar, m_string8);
    AS(ar, m_string9); 
    AS(ar, m_string10);
    AS(ar, m_string11); 
    ar << StopTime;
    ar << data;
  }
  else
  {
    CString unused;
    Clear();
    DAS(ar,m_string1);
    affectedAttr_IFKEY = (IF_KEYWORD_INDEX)KeywordTextToNum(AffectedAttr());

    if (version <= _VERSION_0682_)
      ar >> unused; // was changeText
    ar >> flags;
    if (version >= _VERSION_0690_)
      ar >> changeResult;
    
    if (version >= _VERSION_0699_)
    {
      DAS(ar,m_string2);
      ar >> m_sourceOfEffect;
      ar >> parent;
    }
    if (version >= _VERSION_0851_)
    {
      DAS(ar, m_string3);
    }
    
    if (version >= _VERSION_0904_)
    {
      DAS(ar, m_string4);
      DAS(ar, m_string5);       
      DAS(ar, m_string6);
      DAS(ar, m_string7);       

      if (version >= _VERSION_0910_)
      {
        DAS(ar, m_string8);
        DAS(ar, m_string9);       
        DAS(ar, m_string10);
        DAS(ar, m_string11);       
      }
    }    

    if (version >= _VERSION_0906_)
    {
      ar >> StopTime;
    }      

    if (version >= _VERSION_0909_)
    {
      ar >> data;
    }
  };
  changeData.Serialize(ar);
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_EFFECTS_DATA::Serialize(CArchive &ar, double version)
{
  if (ar.IsStoring())
  {
    AS(ar,m_string1);
    ar << flags;
    ar << changeResult;
    AS(ar,m_string2);
    ar << m_sourceOfEffect;
    ar << parent;
    AS(ar, m_string3);
    AS(ar, m_string4);
    AS(ar, m_string5); 
    AS(ar, m_string6);
    AS(ar, m_string7);         
    AS(ar, m_string8);
    AS(ar, m_string9);         
    AS(ar, m_string10);
    AS(ar, m_string11);         
    ar << StopTime;
    ar << data;
  }
  else
  {
    CString unused;
    Clear();
    DAS(ar,m_string1);
    if (version <= _VERSION_0682_)
      ar >> unused; // was changeText
    ar >> flags;
    if (version >= _VERSION_0690_)
      ar >> changeResult;
    
    if (version >= _VERSION_0699_)
    {
      DAS(ar,m_string2);
      ar >> m_sourceOfEffect;
      ar >> parent;
    }
    if (version >= _VERSION_0851_)
    {
      DAS(ar,m_string3);
    }
    if (version >= _VERSION_0904_)
    {
      DAS(ar, m_string4);
      DAS(ar, m_string5);
      DAS(ar, m_string6);
      DAS(ar, m_string7);            

      if (version >= _VERSION_0910_)
      {
        DAS(ar, m_string8);
        DAS(ar, m_string9);            
        DAS(ar, m_string10);
        DAS(ar, m_string11);            
      }
    }        
    if (version >= _VERSION_0906_)
    {
      ar >> StopTime;
    }   
    if (version >= _VERSION_0909_)
    {
      ar >> data;
    }
  }
  changeData.Serialize(ar);
}

#ifdef UAFEDITOR
void SPELL_EFFECTS_DATA::Export(JWriter& jw)
{
  /* Really */ NotImplemented(0xa098, false);
  /*
    AS(ar,affectedAttr);
    ar << flags;
    ar << changeResult;
    AS(ar,activationScript);
    ar << m_sourceOfEffect;
    ar << parent;
    AS(ar, activationBinary);
    AS(ar, modificationScript);
    AS(ar, modificationBinary); 
    AS(ar, savingThrowScript);
    AS(ar, savingThrowBinary);         
    AS(ar, savingThrowFailedScript);
    AS(ar, savingThrowFailedBinary);         
    AS(ar, savingThrowSucceededScript);
    AS(ar, savingThrowSucceededBinary);         
    ar << StopTime;
    ar << data;
  changeData.Serialize(ar);
  */
}
void SPELL_EFFECTS_DATA::Import(JReader& jr)
{
  /* Really */ NotImplemented(0xa099, false);
  /*
    AS(ar,affectedAttr);
    ar << flags;
    ar << changeResult;
    AS(ar,activationScript);
    ar << m_sourceOfEffect;
    ar << parent;
    AS(ar, activationBinary);
    AS(ar, modificationScript);
    AS(ar, modificationBinary); 
    AS(ar, savingThrowScript);
    AS(ar, savingThrowBinary);         
    AS(ar, savingThrowFailedScript);
    AS(ar, savingThrowFailedBinary);         
    AS(ar, savingThrowSucceededScript);
    AS(ar, savingThrowSucceededBinary);         
    ar << StopTime;
    ar << data;
  changeData.Serialize(ar);
  */
}
#endif


//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::PreSerialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_EFFECTS_DATA::PreSerialize(BOOL IsStoring) 
{ 
  changeData.PreSerialize(IsStoring); 
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::operator==
//
// PURPOSE: 
//
//*****************************************************************************
bool SPELL_EFFECTS_DATA::operator==(const SPELL_EFFECTS_DATA &rhs) const
{
  if (AffectedAttr() != rhs.AffectedAttr()) return false;
  if (changeResult!= rhs.changeResult) return false;
  if (ActivationScript() != rhs.ActivationScript()) return false;
  if (ModificationScript() != rhs.ModificationScript()) return false;
  if (SavingThrowScript() != rhs.SavingThrowScript()) return false;
  if (SavingThrowFailedScript() != rhs.SavingThrowFailedScript()) return false;
  if (SavingThrowSucceededScript() != rhs.SavingThrowSucceededScript()) return false;
  if (changeData != rhs.changeData) return false;
  if (flags != rhs.flags) return false;
  // don't include dynamic data
  return true;
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_EFFECTS_DATA::Clear() 
{
  ClearChangeResult();
  m_string1="";
  m_string2="";
  m_string3="";
  m_string4="";
  m_string5="";
  m_string6="";
  m_string7="";
  m_string8="";
  m_string9="";
  m_string10="";
  m_string11="";
  StopTime=0;
  data=0;
  changeData.Clear();
  flags=SPELL_EFFECTS_DATA::EFFECT_TARGET|EFFECT_CUMULATIVE;
  // this data is dynamic and is
  // determined when spell is cast.
  parent=0;
  m_sourceOfEffect=(DWORD)-1;
  m_scriptProcessed = true;
  m_pCaster = NULL;
  m_pTarget = NULL;
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::GetData
//
// PURPOSE: 
//
//*****************************************************************************
DWORD SPELL_EFFECTS_DATA::GetData() const
{
  return data;
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::SetData
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_EFFECTS_DATA::SetData(DWORD val)
{
  data = val;
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::ApplyChange
//
// PURPOSE: 
//
//*****************************************************************************
double SPELL_EFFECTS_DATA::ApplyChange(double src) const
{
  double amount=0.0;
  if (changeResult==-1234567890123456789.0) // not rolled yet
  {
    die(0xab53a); // GetChange() should be called at least once before this
    /* Really */ NotImplemented(0x075186432, false); // should have been done long ago!
    //GetChange();
  }
  // This spell effect is a temporary effect that causes
  // a modification only during the duration of the effect.
  if (flags & EFFECT_NONE)
  {
    amount = src;
  }
  else
  {
    if (flags & EFFECT_PCNT)
      amount = ((changeResult*0.01)*src);
    else if (flags & EFFECT_ABS)
      amount = changeResult;
    else
      amount = (src+changeResult);
  };
  return amount;
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::GetChange
//
// PURPOSE: 
//
//*****************************************************************************
double SPELL_EFFECTS_DATA::GetChange()
{ 
  changeData.Roll(changeResult);
  spellGPDL.SetIntermediateResult(changeResult);
  changeResult += ExecuteModificationScript();
  return changeResult; 
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::ClearChangeResult
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_EFFECTS_DATA::ClearChangeResult() 
{ 
  changeResult=-1234567890123456789.0; 
  spellGPDL.SetIntermediateResult(0.0);
}
   

AttributeScriptContexts::AttributeScriptContexts(SPELL_EFFECTS_DATA *pSpellEffectsData)
{
  m_scriptContexts.SetAttackerContext(pSpellEffectsData->m_pCaster);
  m_scriptContexts.SetTargetContext(pSpellEffectsData->m_pTarget);
}

AttributeScriptContexts::AttributeScriptContexts(SAVING_THROW_DATA *pSavingThrowData)
{
  m_scriptContexts.SetAttackerContext(pSavingThrowData->pCaster);
  m_scriptContexts.SetTargetContext(pSavingThrowData->pTarget);
}

AttributeScriptContexts::~AttributeScriptContexts(void)
{
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::ExecuteActivationScript
//
// PURPOSE: 
//
//*****************************************************************************
bool SPELL_EFFECTS_DATA::ExecuteActivationScript(void)
{
  // The script execution can return "1" (for true) 
  // or empty string (for false). 
  //
  // If true, the script indicates it is
  // activated. Whatever that means depends
  // on the spell effect this script 
  // represents. (returns true)
  //
  // If false, then the script does not
  // want to become active under the 
  // current circumstances. (returns false)
  //
  // If no script is present, then this
  // spell effect is automatically enabled (returns true)
  //
  // If the script cannot be executed for
  // some reason, then this spell effect
  // cannot activate (returns false)
  
  GPDL gpdl;  
  bool success = true;
  if (!ActivationScript().IsEmpty())
  {
    if (ActivationBinary().GetLength()>0)
    {
      AttributeScriptContexts asc(this);
      if (gpdl.ExecuteScript(ActivationBinary(),1)!="1") 
        success = false;
    }
    else
      success = false; // should have been compiled previously!
  }
  
  return success;  
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::ExecuteModificationScript
//
// PURPOSE: 
//
//*****************************************************************************
double SPELL_EFFECTS_DATA::ExecuteModificationScript() 
{
  // The modification script returns a value that is treated as a
  // delta to the existing 'Change By' value for this attribute.
  // Absolute and Percentage settings do not affect how this
  // modification is treated, it is always a delta. 
  //
  // The 'Change By' value is computed first, then this mod is
  // added to it, and then the Delta/Percentage/Absolute flags
  // are used to determine how the input value is changed to it's
  // final value.
  // 
  // This script is optional.
  // Assumes that $AttackerContext() and $TargetContext() are already set up.
  
  double dresult = 0.0;
  
  if (!ModificationScript().IsEmpty())
  {
    if (ModificationBinary().GetLength() > 0)
    {
      AttributeScriptContexts asc(this);
      CString sresult = spellGPDL.ExecuteScript(ModificationBinary(),1);
      dresult = atof(sresult);
    }
  }  
  return dresult;
}

/*  Saving throws are moved to the spell
//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::ExecuteSavingThrowScript
//
// PURPOSE: 
//
//*****************************************************************************
double SPELL_EFFECTS_DATA::ExecuteSavingThrowScript()
{
  // The saving throw script returns a value that is treated as a
  // bonus (plus or minus) to the saving throw rolled by a character
  //
  // This script is optional.
  // Assumes that $AttackerContext() and $TargetContext() are already set up.
  
  double dresult = 0.0;
  
  if (!SavingThrowScript().IsEmpty())
  {
    if (SavingThrowBinary().GetLength() > 0)
    {
      AttributeScriptContexts asc(this);
      CString sresult = spellGPDL.ExecuteScript(SavingThrowBinary(),1);
      dresult = atof(sresult);
    }
  }  
  return dresult;
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::ExecuteSavingThrowFailedScript
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_EFFECTS_DATA::ExecuteSavingThrowFailedScript()
{
  // This script is optional.
    
  if (!SavingThrowFailedScript().IsEmpty())
  {
    if (SavingThrowFailedBinary().GetLength() > 0)
    {
      AttributeScriptContexts asc(this);
      spellGPDL.ExecuteScript(SavingThrowFailedBinary(),1);
    }
  }  
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::ExecuteSavingThrowSucceededScript
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_EFFECTS_DATA::ExecuteSavingThrowSucceededScript()
{
  // This script is optional.
  
  if (!SavingThrowSucceededScript().IsEmpty())
  {
    if (SavingThrowSucceededBinary().GetLength() > 0)
    {
      AttributeScriptContexts asc(this);
      spellGPDL.ExecuteScript(SavingThrowSucceededBinary(),1);
    }
  }  
}
*/
//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::CompileScripts
//
// PURPOSE: 
//
//*****************************************************************************
bool SPELL_EFFECTS_DATA::CompileScripts(const CString &basename)
{
  bool success = true;
#ifdef UAFEDITOR
  ActivationBinary("");  
  ModificationBinary("");
  //SavingThrowBinary("");
  //SavingThrowFailedBinary("");
  //SavingThrowSucceededBinary("");
  const char *allowedEntry[2];
  

  GPDLCOMP gpdlComp;
  
  if (ActivationBinary().IsEmpty())
  {
    if (!ActivationScript().IsEmpty()) 
    {
      CString func(basename);
      func.Replace('|','_');
      func.Replace('-','_');
      func += GetEffectActivationFuncName();
      CString ascript;
      CString scriptbegin;
      CString scriptend;

      scriptbegin.Format("$PUBLIC $FUNC %s() { ", func);
      scriptend.Format(" } %s;", func);
      ascript = scriptbegin + ActivationScript() + scriptend;
    
      allowedEntry[0] = (LPCSTR)func;
      allowedEntry[1] = NULL;     
      ActivationBinary(gpdlComp.CompileScript(ascript, allowedEntry));
    
      if (ActivationBinary()[0]!=0)
      {
        WriteDebugString("Error compiling Activation Script for spell %s\n", basename);
        WriteDebugString("Error message = %s\n", ActivationBinary());
        success = false;
      };
    };
  };

  if (ModificationBinary().IsEmpty())
  {
    if (!ModificationScript().IsEmpty()) 
    {
      CString func(basename);
      func.Replace('|','_');
      func.Replace('-','_');
      func += GetEffectModificationFuncName();
      CString mscript;
      CString scriptbegin;
      CString scriptend;
    
      scriptbegin.Format("$PUBLIC $FUNC %s() { ", func);
      scriptend.Format(" } %s;", func);
      mscript = scriptbegin + ModificationScript() + scriptend;
    
      allowedEntry[0] = (LPCSTR)func;
      allowedEntry[1] = NULL;     
      ModificationBinary(gpdlComp.CompileScript(mscript, allowedEntry));
    
      if (ModificationBinary()[0]!=0) 
      {
        WriteDebugString("Error compiling Modification Script for spell %s\n", basename);
        WriteDebugString("Error message = %s\n", ModificationBinary());
        success = false;
      };
    };
  };
/*
  if (SavingThrowBinary().IsEmpty())
  {
    if (!SavingThrowScript().IsEmpty()) 
    {
      CString func(basename);
      func.Replace('|','_');
      func.Replace('-','_');
      func += GetEffectSavingThrowFuncName();
      CString mscript;
      CString scriptbegin;
      CString scriptend;
    
      scriptbegin.Format("$PUBLIC $FUNC %s() { ", func);
      scriptend.Format(" } %s;", func);
      mscript = scriptbegin + SavingThrowScript() + scriptend;
    
      allowedEntry[0] = (LPCSTR)func;
      allowedEntry[1] = NULL;     
      SavingThrowBinary(gpdlComp.CompileScript(mscript, allowedEntry));
    
      if (SavingThrowBinary()[0]!=0)
      {
        WriteDebugString("Error compiling Saving Throw Script for spell %s\n", basename);
        WriteDebugString("Error message = %s\n", SavingThrowBinary());
        success = false;
      };
    };
  };

  if (SavingThrowFailedBinary().IsEmpty())
  {
    if (!SavingThrowFailedScript().IsEmpty()) 
    {
      CString func(basename);
      func.Replace('|','_');
      func.Replace('-','_');
      func += GetEffectSavingThrowFailedFuncName();
      CString mscript;
      CString scriptbegin;
      CString scriptend;
    
      scriptbegin.Format("$PUBLIC $FUNC %s() { ", func);
      scriptend.Format(" } %s;", func);
      mscript = scriptbegin + SavingThrowFailedScript() + scriptend;
    
      allowedEntry[0] = (LPCSTR)func;
      allowedEntry[1] = NULL;     
      SavingThrowFailedBinary(gpdlComp.CompileScript(mscript, allowedEntry));
    
      if (SavingThrowFailedBinary()[0]!=0)
      {
        WriteDebugString("Error compiling SavingThrowFailed Script for spell %s\n", basename);
        WriteDebugString("Error message = %s\n", SavingThrowFailedBinary());
        success = false;
      };
    }
  };

  if (SavingThrowSucceededBinary().IsEmpty())
  {
    if (!SavingThrowSucceededScript().IsEmpty()) 
    {
      CString func(basename);
      func.Replace('|','_');
      func.Replace('-','_');
      func += GetEffectSavingThrowSucceededFuncName();
      CString mscript;
      CString scriptbegin;
      CString scriptend;
    
      scriptbegin.Format("$PUBLIC $FUNC %s() { ", func);
      scriptend.Format(" } %s;", func);
      mscript = scriptbegin + SavingThrowSucceededScript() + scriptend;
    
      allowedEntry[0] = (LPCSTR)func;
      allowedEntry[1] = NULL;     
      SavingThrowSucceededBinary(gpdlComp.CompileScript(mscript, allowedEntry));
    
      if (SavingThrowSucceededBinary()[0]!=0)
      {
        WriteDebugString("Error compiling SavingThrowSucceeded Script for spell %s\n", basename);
        WriteDebugString("Error message = %s\n", SavingThrowSucceededBinary());
        success = false;
      };

    };
  };

*/
  if (!success)
  {
    ActivationBinary("");
    ModificationBinary("");
    //SavingThrowBinary("");
    //SavingThrowFailedBinary("");
    //SavingThrowSucceededBinary("");
  }
#endif
  return success;
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::GetEffectActivationFuncName
//
// PURPOSE: 
//
//*****************************************************************************
CString SPELL_EFFECTS_DATA::GetEffectActivationFuncName(void)
{
  CString name("");
  name.Format("%s_a_%u", AffectedAttr(), GetTickCount());
  return name;
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::GetEffectModificationFuncName
//
// PURPOSE: 
//
//*****************************************************************************
CString SPELL_EFFECTS_DATA::GetEffectModificationFuncName(void)
{
  CString name("");
  name.Format("%s_m_%u", AffectedAttr(), GetTickCount());
  return name;
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::GetEffectSavingThrowFuncName
//
// PURPOSE: 
//
//*****************************************************************************
CString SPELL_EFFECTS_DATA::GetEffectSavingThrowFuncName(void)
{
  CString name("");
  name.Format("%s_s_%u", AffectedAttr(), GetTickCount());
  return name;
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::GetEffectSavingThrowFailedFuncName
//
// PURPOSE: 
//
//*****************************************************************************
CString SPELL_EFFECTS_DATA::GetEffectSavingThrowFailedFuncName(void)
{
  CString name("");
  name.Format("%s_sf_%u", AffectedAttr(), GetTickCount());
  return name;
}

//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::GetEffectSavingThrowSucceededFuncName
//
// PURPOSE: 
//
//*****************************************************************************
CString SPELL_EFFECTS_DATA::GetEffectSavingThrowSucceededFuncName(void)
{
  CString name("");
  name.Format("%s_ss_%u", AffectedAttr(), GetTickCount());
  return name;
}

#ifdef UAFEngine
//*****************************************************************************
//    NAME: SPELL_EFFECTS_DATA::ProcessTimeSensitiveData
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_EFFECTS_DATA::IsReadyToExpire() const
{
  if (StopTime == 0) return TRUE;

  SPELL_DATA *pSpell = spellData.GetSpell(SourceSpell_ID());
  
  if (pSpell == NULL)
  {
    if ((flags & EFFECT_SCRIPT) == 0)
    {
      die(0xab53b);
      return TRUE;
    };
    TIME_UNITS tu = (TIME_UNITS)data;
    switch(tu)
    {
    case TU_MINUTES:
      return party.getElapsedMinutes() >= StopTime;
      break;
    default:
      /* Really */ NotImplemented(0x551, false);
      break;
    };
    return TRUE;
  }
  
  switch (pSpell->Duration_Rate)
  {
  case byNbrAttacks:
  case byDamageTaken: 
    die(0xab53c);
    return TRUE;
    
  default:
    return (party.getElapsedMinutes() > StopTime);
  }
}
#endif

///////////////////////////////////////////////////////////////////////
//
// Character Spell Functions
//
///////////////////////////////////////////////////////////////////////

//*****************************************************************************
//    NAME: CHARACTER_SPELL::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER_SPELL::Clear() 
{ 
  memorized=0; 
  //spell.Clear(); 
  spellID.Clear(); 
  level=0; 
  selected=0; 
  memTime=0;
  JustMemorized=FALSE;
  /*
#ifdef UAFEDITOR
  preVersionSpellNames_gsID = -1;
#endif
  */
}

//*****************************************************************************
//    NAME: CHARACTER_SPELL::operator=
//
// PURPOSE: 
//
//*****************************************************************************
CHARACTER_SPELL& CHARACTER_SPELL::operator=(const CHARACTER_SPELL &cSpell)
{
  if (&cSpell == this) return *this;
  selected = cSpell.selected;
  memorized = cSpell.memorized;
  //spell = cSpell.spell;
  spellID = cSpell.spellID;
  level = cSpell.level;
  memTime = cSpell.memTime;
  JustMemorized=cSpell.JustMemorized;
/*
#ifdef UAFEDITOR
  preVersionSpellNames_gsID = cSpell.preVersionSpellNames_gsID;
#endif
*/
  return *this;
}

//*****************************************************************************
//    NAME: CHARACTER_SPELL::operator==
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER_SPELL::operator==(const CHARACTER_SPELL &src)
{
  if (&src == this) return TRUE;
  //if (selected != src.selected) return FALSE;
  //if (memorized != src.memorized) return FALSE;
  //if (spell != src.spell) return FALSE;
  if (spellID != src.spellID) return FALSE;
  if (level != src.level) return FALSE;
  //if (JustMemorized != src.JustMemorized) return FALSE;
  //if (memTime != src.memTime) return FALSE; // don't include this
  return TRUE;
}

//*****************************************************************************
//    NAME: CHARACTER_SPELL::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER_SPELL::Serialize(CArchive &ar, double version)
{
  if (ar.IsStoring())
  {
    //spell.Serialize(ar, version);//ar << spell;
    ar << spellID;
    ar << memorized;
    ar << level;
    ar << selected;
  }
  else
  {
    Clear();
    //spell.Serialize(ar, version);//ar >> spell;
#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
      int preVersionSpellNames_ID;
      ar >> preVersionSpellNames_ID;
      spellID = spellData.FindPreVersionSpellNamesSpellID(preVersionSpellNames_ID);
    }
    else
#endif
    {
      ar >> spellID;
    };
    ar >> memorized;
    ar >> level;
    ar >> selected;
  }
}

//*****************************************************************************
//    NAME: CHARACTER_SPELL::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER_SPELL::Serialize(CAR &ar, double version)
{
  if (ar.IsStoring())
  {
    //spell.Serialize(ar, version);//ar << spell;
    ar << spellID;
    ar << memorized;
    ar << level;
    ar << selected;
  }
  else
  {
    Clear();
    //spell.Serialize(ar, version);//ar >> spell;
#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
      ar >> preVersionSpellNames_gsID; // To be convert to SPELL_ID when spellData is available.
    }
    else
#endif
    {
      ar >> spellID;
    };
    ar >> memorized;
    ar >> level;
    ar >> selected;
  }
}

#ifdef UAFEDITOR

extern const char *JKEY_SPELL;
extern const char *JKEY_NAME;
extern const char *JKEY_LEVEL;
const char *JKEY_MEMORIZED = "memorized";
const char *JKEY_SELECTED = "selected";

void CHARACTER_SPELL::Export(JWriter& jw)
{
  jw.StartList();
  jw.Linefeed(false);
  jw.NameAndValue(JKEY_NAME,spellID);
  jw.NameAndValue(JKEY_MEMORIZED,memorized);
  jw.NameAndValue(JKEY_LEVEL,level);
  jw.NameAndValue(JKEY_SELECTED,selected);
  jw.EndList();
  jw.Linefeed(true);
}
bool CHARACTER_SPELL::Import(JReader& jr)
{
  if (!jr.StartList()) return false;
  jr.Linefeed(false);
  jr.NameAndValue(JKEY_NAME,spellID);
  jr.NameAndValue(JKEY_MEMORIZED,memorized);
  jr.NameAndValue(JKEY_LEVEL,level);
  jr.NameAndValue(JKEY_SELECTED,selected);
  jr.EndList();
  jr.Linefeed(true);
  return true;
}
#endif

//*****************************************************************************
//    NAME: CHARACTER_SPELL::IncMemorizedTime
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER_SPELL::IncMemorizedTime(int minuteInc, bool all) 
{ 
  JustMemorized=FALSE;
  if ((selected<=0) || (selected <= memorized)) return FALSE;
  if (!all) memTime += minuteInc; 
  if (all || MemTimeSufficient())
  {
    SetMemorized(all);
    return TRUE;
  }  
  return FALSE;
}

//*****************************************************************************
//    NAME: CHARACTER_SPELL::HaveUnmemorized
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER_SPELL::HaveUnmemorized() const
{
  if (selected <= 0) return FALSE;
  return (selected > memorized);
}

//*****************************************************************************
//    NAME: CHARACTER_SPELL::MemTimeSufficient
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER_SPELL::MemTimeSufficient() 
{ 
  return (memTime >= GetSpellMemorizeTime(level)); 
}

//*****************************************************************************
//    NAME: CHARACTER_SPELL::SetMemorized
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER_SPELL::SetMemorized(bool all) 
{ 
  if ((!selected) || (selected <= memorized)) return;
  if (all)
  {
    memorized = selected;
  }
  else
  {
    memorized++;
  };
  JustMemorized=TRUE;
  memTime=0; 
}

//*****************************************************************************
//    NAME: CHARACTER_SPELL::SetUnMemorized
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER_SPELL::SetUnMemorized() 
{ 
  if ((!selected) || (memorized <= 0)) return;
  memorized--;
}


///////////////////////////////////////////////////////////////////////
//
// Spell List Functions
//
///////////////////////////////////////////////////////////////////////

//*****************************************************************************
//    NAME: SPELL_LIST::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_LIST::Serialize(CArchive &ar, double version)
{
  if (ar.IsStoring())
  {
    /*
    ar << GetCount();
    POSITION p = spells.GetHeadPosition();
    while (p != NULL)
      spells.GetNext(p).Serialize(ar, version);
      //ar << spells.GetNext(p);  
    */
    int i,n;
    n = GetCount();
    ar << n;
    for (i=0; i<n; i++)
    {
      GetCharacterSpell(i)->Serialize(ar, version);
      //spells[i].Serialize(ar, version);
    };
  }
  else
  {
    int count;
    CHARACTER_SPELL spell;
    ar >> count;  
    for (int i=0; i<count; i++)
    {
      //ar >> spell;
      spell.Serialize(ar, version);
      AddSpell(spell);
    }
  }
}

//*****************************************************************************
//    NAME: SPELL_LIST::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_LIST::Serialize(CAR &ar, double version)
{
  if (ar.IsStoring())
  {
    //ar << GetCount();
    //POSITION p = spells.GetHeadPosition();
    //while (p != NULL)
    //  spells.GetNext(p).Serialize(ar, version);
      //ar << spells.GetNext(p);  
    int i,n;
    n = GetCount();
    ar << n;
    for (i=0; i<n; i++)
    {
      GetCharacterSpell(i)->Serialize(ar, version);
      //spells[i].Serialize(ar, version);
    };
  }
  else
  {
    int count;
    CHARACTER_SPELL spell;
    Clear();
    ar >> count;  
    for (int i=0; i<count; i++)
    {
      //ar >> spell;
      spell.Serialize(ar, version);
      AddSpell(spell);
    }
  }
}

#ifdef UAFEDITOR

const char *JKEY_SPELLLIST = "spellList";

void SPELL_LIST::Export(JWriter& jw)
{
  int i,n;
  jw.StartArray(JKEY_SPELLLIST);
  n = GetCount();
  for (i=0; i<n; i++)
  {
    jw.NextEntry();
    GetCharacterSpell(i)->Export(jw);
    //spells[i].Export(jw);
  };
  jw.EndArray();
}
bool SPELL_LIST::Import(JReader& jr)
{
  if (jr.Optional(), !jr.StartArray(JKEY_SPELLLIST)) return false;
  CHARACTER_SPELL spell;
  while (jr.Optional(), jr.NextEntry())
  {
    spell.Import(jr);
    spells.Add(spell);
  };
  jr.EndArray();
  return true;
}

void SPELL_LIST::CrossReference(CR_LIST *pCRList, const CR_REFERENCE *pCRReference) const
{
/*
  POSITION pos;
  const CHARACTER_SPELL *pSpell;
  for (pos = GetUnsortedHeadPosition(); pos!=NULL; PeekUnsortedNext(pos))
  {
    pSpell = &PeekAtPos(pos);
    pCRList->CR_AddSpellReference(pSpell->spell.m_key, pCRReference);
  };
  */
  int i, n;
  const CHARACTER_SPELL *pCharSp;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    pCharSp = PeekCharacterSpell(i);
    pCRList->CR_AddSpellReference(pCharSp->spellID, pCRReference);
  };
}
#endif

//*****************************************************************************
//    NAME: SPELL_LIST::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_LIST::Clear() 
{ 
  spells.RemoveAll();
  //sortedAccess.RemoveAll();
  //m_isSorted = FALSE;
}
//*****************************************************************************
//    NAME: SPELL_LIST::GetCount
//
// PURPOSE: 
//
//*****************************************************************************
int SPELL_LIST::GetCount() const 
{ 
  //return spells.GetCount(); 
  return spells.GetSize(); 
}

void SPELL_LIST::CopySpells(const SPELL_LIST& spellList)
{
  spells.RemoveAll();
  int i, n;
  n = spellList.GetCount();
  for (i=0; i< n; i++)
  {
    spells.Add(*(const_cast<CHARACTER_SPELL *>(spellList.PeekCharacterSpell(i))));
  };
}


//*****************************************************************************
//    NAME: SPELL_LIST::operator=
//
// PURPOSE: 
//
//*****************************************************************************
SPELL_LIST& SPELL_LIST::operator=(const SPELL_LIST &list)
{
  if (this==&list)return *this;
  Clear();
  //spells = list.spells;
  CopySpells(list);
  //SortSpells(); // update ordered list
  //sortedAccess not copied
  return *this;
}

//*****************************************************************************
//    NAME: SPELL_LIST::operator==
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_LIST::operator==(const SPELL_LIST &src)
{
/*
  if (&src == this) return TRUE;
  POSITION p1=spells.GetHeadPosition();
  POSITION p2=const_cast<SPELL_LIST&>(src).spells.GetHeadPosition();
  while (p1!=NULL)
  {
    if (p2==NULL) return FALSE;
    if (!(spells.GetNext(p1)==const_cast<SPELL_LIST&>(src).spells.GetNext(p2))) return FALSE;
  }
  if (p2!=NULL) return FALSE;
  return TRUE;
*/
  int i,m,n;
  m = GetCount();
  n = src.GetCount();
  if (m != n) return FALSE;
  for (i=0; i<n; i++)
  {
    if (!(*GetCharacterSpell(i) == *src.PeekCharacterSpell(i))) return FALSE;
    //if (!(spells[i] == src.spells[i])) return FALSE;
  };
  return TRUE;
}

/*
//*****************************************************************************
//    NAME: SPELL_LIST::GetHeadPosition
//
// PURPOSE: 
//
//*****************************************************************************
POSITION SPELL_LIST::GetHeadPosition() const
{ 
  POSITION pos = sortedAccess.GetHeadPosition();
  
  // The following 
  
  //if (pos == NULL)
  //{
  //  if (spells.GetCount() == 0) return NULL;
  //  SortSpells();
  //  pos = sortedAccess.GetHeadPosition();
  //}
  return pos;
}
*/
/*
//*****************************************************************************
//    NAME: SPELL_LIST::GetAt
//
// PURPOSE: 
//
//*****************************************************************************
CHARACTER_SPELL& SPELL_LIST::GetAtPos(POSITION pos) 
{   
  ASSERT( sortedAccess.GetCount() > 0);
  //POSITION pos2 = spells.FindKeyPos(sortedAccess.GetAt(pos));
  POSITION pos2 = FindIDPos(sortedAccess.GetAt(pos));
  if (pos2 != NULL)
  {
    //return spells.GetAtPos(pos2);
    return spells.GetAt(pos2);
  }
  else
  {
    ASS ERT(FALSE);
    WriteDebugString("Failed to find CHAR_SPELL in SPELL_LIST::GetAt()\n");
    return spells.GetHead();
  }  
}
*/
/*
//*****************************************************************************
//    NAME: SPELL_LIST::GetAt
//
// PURPOSE: 
//
//*****************************************************************************
const CHARACTER_SPELL& SPELL_LIST::PeekAtPos(POSITION pos) const
{   
  ASSERT( sortedAccess.GetCount() > 0);
  //POSITION pos2 = spells.FindKeyPos(sortedAccess.GetAt(pos));
  POSITION pos2 = FindIDPos(sortedAccess.GetAt(pos));
  if (pos2 != NULL)
  {
    //return const_cast<SPELL_LIST *>(this)->spells.GetAtPos(pos2);
    return const_cast<SPELL_LIST *>(this)->spells.GetAt(pos2);
  }
  else
  {
    ASS ERT(FALSE);
    WriteDebugString("Failed to find CHAR_SPELL in SPELL_LIST::GetAt()\n");
    //return spells.PeekHead();
    return PeekHead();
  }  
}
*/
/*

//*****************************************************************************
//    NAME: SPELL_LIST::GetNext
//
// PURPOSE: 
//
//*****************************************************************************
CHARACTER_SPELL& SPELL_LIST::GetNext(POSITION &pos) 
{ 
  ASSERT( sortedAccess.GetCount() > 0);
  //POSITION pos2 = spells.FindKeyPos(sortedAccess.GetNext(pos));
  POSITION pos2 = FindIDPos(sortedAccess.GetNext(pos));
  if (pos2 != NULL)
  {
    //return spells.GetAtPos(pos2);
    return spells.GetAt(pos2);
  }
  else
  {
    ASS ERT(FALSE);
    WriteDebugString("Failed to find CHAR_SPELL in SPELL_LIST::GetNext()\n");
    return spells.GetHead();
  }  
}
*/
/*

//*****************************************************************************
//    NAME: SPELL_LIST::PeekNext
//
// PURPOSE: 
//
//*****************************************************************************
const CHARACTER_SPELL& SPELL_LIST::PeekNext(POSITION &pos) const
{ 
  ASSERT( sortedAccess.GetCount() > 0);
  //POSITION pos2 = spells.FindKeyPos(sortedAccess.GetNext(pos));
  POSITION pos2 = FindIDPos(sortedAccess.GetNext(pos));
  if (pos2 != NULL)
  {
    //return spells.PeekAtPos(pos2);
    return PeekAtPos(pos2);
  }
  else
  {
    ASS ERT(FALSE);
    WriteDebugString("Failed to find CHAR_SPELL in SPELL_LIST::GetNext()\n");
    //return spells.PeekHead();
    return PeekHead();
  }  
}
*/
/*
//*****************************************************************************
//    NAME: SPELL_LIST::GetKeyAt
//
// PURPOSE: 
//
//*****************************************************************************
//GLOBAL_SPELL_ID SPELL_LIST::GetKeyAt(POSITION pos)
SPELL_ID SPELL_LIST::GetIDAt(POSITION pos) const
{ 
  ASSERT( sortedAccess.GetCount() > 0);
  //POSITION pos2 = spells.FindKeyPos(sortedAccess.GetAt(pos));
  POSITION pos2 = FindIDPos(sortedAccess.GetAt(pos));
  if (pos2 != NULL)
  {
    //GLOBAL_SPELL_ID gsID;
    SPELL_ID spellID;
    //gsID.IllegalConversion(spells.GetKeyAt(pos2));
    spellID = spells.GetKeyAt(pos2);
    //return gsID;
    return spellID;
  }
  else
    //return 0;
    //return GLOBAL_SPELL_ID();
    return SPELL_ID();
  return sortedAccess.GetAt(pos);
}
*/
//*****************************************************************************
//    NAME: SPELL_LIST::IsMemorized
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL SPELL_LIST::IsMemorized(GLOBAL_SPELL_ID index) const
BOOL SPELL_LIST::IsMemorized(const SPELL_ID& spellID) const
{
  /*
  POSITION pos;
  if ((pos = spells.FindIDPos(spellID)) != NULL)
    return (spells.PeekAtUnsortedPos(pos).IsMemorized());
  else
    return FALSE;
  */
  int index;
  index = LocateCharacterSpell(spellID);
  if (index < 0) return FALSE;
  return PeekCharacterSpell(index)->IsMemorized();
  //return spells[index].IsMemorized();
}

//*****************************************************************************
//    NAME: SPELL_LIST::DecMemorized
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL SPELL_LIST::DecMemorized(GLOBAL_SPELL_ID index, int memorized_count)
BOOL SPELL_LIST::DecMemorized(const SPELL_ID& spellID, int memorized_count)
{
  int index;
  if (memorized_count==0) return FALSE;
  //POSITION pos;
  //if ((pos = spells.FindKeyPos(index.m_key)) != NULL)
 /*
  if ((pos = this->FindIDPos(spellID)) != NULL)
  {
    for (int i=0;i<memorized_count;i++)
      spells.GetAtPos(pos).SetUnMemorized();
    return TRUE;
  }
  else
    return FALSE;
*/
  index = LocateCharacterSpell(spellID);
  if (index < 0) return FALSE;
  GetCharacterSpell(index)->SetUnMemorized();
  //spells[index].SetUnMemorized();
  return TRUE;
}

//*****************************************************************************
//    NAME: SPELL_LIST::SetSelected
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL SPELL_LIST::SetSelected(GLOBAL_SPELL_ID index, int selected)
BOOL SPELL_LIST::SetSelected(const SPELL_ID& spellID, int selected)
{
  //POSITION pos;
  int index;
  //if ((pos = spells.FindKeyPos(index.m_key)) != NULL)
/*
  if ((pos = spells.FindIDPos(spellID)) != NULL)
  {
    spells.GetAtPos(pos).selected = selected;
    return TRUE;
  }
  else
    return FALSE;
*/
  index = LocateCharacterSpell(spellID);
  if (index < 0) return FALSE;
  GetCharacterSpell(index)->selected = selected;
  //spells[index].selected = selected;
  return TRUE;
}

//*****************************************************************************
//    NAME: SPELL_LIST::IsSelected
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL SPELL_LIST::IsSelected(GLOBAL_SPELL_ID index) const
BOOL SPELL_LIST::IsSelected(const SPELL_ID& spellID) const
{
/*
  POSITION pos;
  //if ((pos = spells.FindKeyPos(index.m_key)) != NULL)
  if ((pos = spells.FindIDPos(spellID)) != NULL)
    return (spells.PeekAtPos(pos).IsSelected());
  else
    return FALSE;
*/
  int index;
  index = LocateCharacterSpell(spellID);
  if (index < 0) return FALSE;
  return PeekCharacterSpell(index)->IsSelected();
  //return spells[index].IsSelected();
}

//*****************************************************************************
//    NAME: SPELL_LIST::HaveSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL SPELL_LIST::HaveSpell(GLOBAL_SPELL_ID index) const
BOOL SPELL_LIST::HaveSpell(const SPELL_ID& spellID) const
{
  return LocateCharacterSpell(spellID) >= 0;
}


//*****************************************************************************
//    NAME: compare
//
// PURPOSE: 
//
//*****************************************************************************
int compareCharacterSpell( const CHARACTER_SPELL *arg1, const CHARACTER_SPELL *arg2 )
{
  /*
    must return
  < 0 elem1 less than elem2 
  = 0 elem1 equivalent to elem2 
  > 0 elem1 greater than elem2 
  */
  const SPELL_DATA *pSpell1, *pSpell2;
  ASSERT( (arg1!=NULL) && (arg2!=NULL) );
  if ((arg1==NULL)||(arg2==NULL)) return 0;
  //CHAR_SPELL_SORT_DATA *pElem1 = (CHAR_SPELL_SORT_DATA*)arg1;
  //CHAR_SPELL_SORT_DATA *pElem2 = (CHAR_SPELL_SORT_DATA*)arg2;
  pSpell1 = spellData.PeekSpell(arg1->spellID);
  pSpell2 = spellData.PeekSpell(arg2->spellID);
  // sort by spell class, then by level, then by spell name
  if ((pSpell1==NULL) || (pSpell2==NULL)) return 1;
  if (pSpell1->schoolID < pSpell2->schoolID) return -1;
  if (pSpell1->schoolID > pSpell2->schoolID) return  1;

  //if (pElem1->type == pElem2->type)
  //{
    if (pSpell1->Level < pSpell2->Level) return -1;
    if (pSpell1->Level > pSpell2->Level) return  1;
    //if (pElem1->level == pElem2->level)
    //{
      if (arg1->spellID < arg2->spellID) return -1;
      if (arg1->spellID > arg2->spellID) return  1;
    //}
  //}

  // type, level, and name are all equal
  return 0;
}




//*****************************************************************************
//    NAME: SPELL_LIST::AddSpell
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_LIST::AddSpell(CHARACTER_SPELL &data)
/*
{
  //if (!HaveSpell(data.spell))
  if (!HaveSpell(data.spellID))
  {
    //BOOL result = (spells.Insert(data, data.spell.m_key));
    BOOL result = (spells.Insert(data, data.spellID));
    m_isSorted = FALSE;
    //if (result) SortSpells();
    return result;
  }
  else
    return TRUE;
}
*/
{
  int start, mid, num;
  start = 0;
  //num = GetCount();
  for (start = 0, num=GetCount(); num > 0;)
  {
    mid = start + num/2;
    if (compareCharacterSpell(&data, GetCharacterSpell(mid)) > 0)
    //if (compareCharacterSpell(&data, &spells[mid]) > 0)
    {
      num = num - mid + start - 1;
      start = mid + 1;
    }
    else
    {
      num = mid - start;
    };
  };
  spells.InsertAt(start, data, 1);
  return TRUE;
};
/*
struct CHAR_SPELL_SORT_DATA
{
  int level;
  int type;
  CString name;
  CHARACTER_SPELL origData;
};
*/
/*
//*****************************************************************************
//    NAME: SPELL_LIST::SortSpells
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_LIST::SortSpells(void)
{
  if (m_isSorted) return;
  m_isSorted = TRUE;
  sortedAccess.RemoveAll();
  if (spellData.GetCount() == 0) return;
  if (spells.GetCount() == 0) return;

  CHAR_SPELL_SORT_DATA tsortspell;
  CArray<CHAR_SPELL_SORT_DATA, CHAR_SPELL_SORT_DATA> sortList;

  POSITION pos = spells.GetHeadPosition();
  while (pos != NULL)
  {
    const CHARACTER_SPELL &tspell = spells.PeekAtPos(pos);
    //if (tspell.spell.IsValidSpell())
    if (tspell.spellID.IsValidSpell())
    {
      //SPELL_DATA *pSdata = spellData.GetSpellData(tspell.spell);
      SPELL_DATA *pSdata = spellData.GetSpellData(tspell.spellID);
      if (pSdata==NULL) 
      {
        //WriteDebugString("Spell book references invalid spellData %u\n", tspell.spell);
        WriteDebugString("Spell book references invalid spellData %s\n", tspell.spellID.UniqueName());
        spells.GetNext(pos);
        continue;
      }
      tsortspell.origData = tspell;
      tsortspell.level = pSdata->Level;
      tsortspell.name = pSdata->Name;
      tsortspell.type = pSdata->schoolMask;
      sortList.SetAtGrow(sortList.GetSize(), tsortspell);
    };
    spells.GetNext(pos);
  }

  if (sortList.GetSize() == 0)
  {
    return;
  }

  if (sortList.GetSize() <= 1) 
  {
    sortedAccess.RemoveAll();
    //sortedAccess.AddTail(sortList[0].origData.spell.m_key);
    sortedAccess.AddTail(sortList[0].origData.spellID);
    return;
  }

  // now all spells are in a array, so
  // start the sorting
  qsort( &sortList[0], sortList.GetSize(), sizeof(CHAR_SPELL_SORT_DATA), compare);
  
  sortedAccess.RemoveAll();

  // now the list is sorted, re-insert into spell list  
  for (int i=0;i<sortList.GetSize();i++)
    //sortedAccess.AddTail(sortList[i].origData.spell.m_key);
    sortedAccess.AddTail(sortList[i].origData.spellID);
}
*/
//*****************************************************************************
//    NAME: SPELL_LIST::RemoveSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL SPELL_LIST::RemoveSpell(GLOBAL_SPELL_ID index)
BOOL SPELL_LIST::RemoveSpell(const SPELL_ID& spellID)
{
  //POSITION pos;
  //if ((pos = spells.FindKeyPos(index.m_key)) != NULL)
  //if ((pos = spells.FindIDPos(spellID)) != NULL)
  //{
  //  spells.RemoveAt(pos);
  //  //SortSpells(); // update ordered list
  //  m_isSorted = FALSE;
  //  return TRUE;
  //}
  //else
  //  return FALSE;
  int index;
  index = LocateCharacterSpell(spellID);
  if (index < 0) return FALSE;
  spells.RemoveAt(index, 1);
  return TRUE;
}

void SPELL_LIST::RemoveSpell(int i)
{
  spells.RemoveAt(i, 1);
}


//*****************************************************************************
//    NAME: SPELL_LIST::RemoveSpellLevel
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_LIST::RemoveSpellLevel(int level)
{
/*
  POSITION currPos;
  POSITION pos = spells.GetHeadPosition();    
  while (pos != NULL)
  {
    currPos = pos;
    spells.GetNext(pos);
    if (level == spells.PeekAtPos(currPos).level) 
      spells.RemoveAt(currPos);
  }
  //SortSpells(); // update ordered list
*/
  int i, n;
  n = spells.GetSize();
  for (i=n-1; i>=0; i--)
  {
    if (PeekCharacterSpell(i)->level == level)
    //if (spells[i].level == level)
    {
      spells.RemoveAt(i);
    };
  };
}

//*****************************************************************************
//    NAME: SPELL_LIST::SpellsMemorized
//
// PURPOSE: 
//
//*****************************************************************************
int SPELL_LIST::SpellsMemorized() const
{
  int count = 0;
  //POSITION pos = spells.GetHeadPosition();    
  //while (pos != NULL)
  //  count += spells.PeekNext(pos)->memorized;
  int i,n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    count += PeekCharacterSpell(i)->memorized;
    //count += spells[i].memorized;
  };
  return count;
}

int SPELL_LIST::LocateCharacterSpell(const SPELL_ID& spellID) const
{
  int i,n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    if (spellID == PeekCharacterSpell(i)->spellID) return i;
  };
  return -1;
}

//const CHARACTER_SPELL *SPELL_LIST::PeekCharacterSpell(const SPELL_ID& spellID) const
//{
//  return PeekCharacterSpell(LocateCharacterSpell(spellID));
//}

//const CHARACTER_SPELL *SPELL_LIST::PeekCharacterSpell(int index) const
//{
//  if (index < 0) return NULL;
//  return &(const_cast <SPELL_LIST *>(this)->spells[index]);
//}

//*****************************************************************************
//    NAME: SPELL_LIST::FetchCharacterSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL SPELL_LIST::GetSpell(GLOBAL_SPELL_ID index, CHARACTER_SPELL &spell)
BOOL SPELL_LIST::FetchCharacterSpell(const SPELL_ID& spellID, CHARACTER_SPELL *pCharSp) const
{
  //POSITION pos;
  //if ((pos = spells.FindKeyPos(index.m_key)) != NULL)
  //if ((pos = spells.FindIDPos(spellID)) != NULL)
  //{
  //  spell = spells.GetAtPos(pos);
  //  return TRUE;
  //}
  //else
  //  return FALSE;
  int index;
  index = LocateCharacterSpell(spellID);
  if (index < 0) return FALSE;
  if (pCharSp != NULL)
  {
    *pCharSp = *PeekCharacterSpell(index);
    //*pCharSp = spells[index];
  };
  return TRUE;
}

//*****************************************************************************
//    NAME: SPELL_LIST::getSelectedCount
//
// PURPOSE: 
//
//*****************************************************************************
//int SPELL_LIST::GetSelectedCount(GLOBAL_SPELL_ID gsID) const
int SPELL_LIST::GetSelectedCount(int index) const
{
/*
  POSITION pos;
  //if ((pos = spells.FindKeyPos(gsID.m_key)) != NULL)
  if ((pos = spells.FindIDPos(spellID)) != NULL)
    return spells.PeekAtPos(pos).selected;
  else
    return 0;
*/
  if (index < 0) return 0;
  return PeekCharacterSpell(index)->selected;
  //return spells[index].selected;
  
}

int SPELL_LIST::GetSelectedCount(const SPELL_ID& spellID) const
{
/*
  POSITION pos;
  //if ((pos = spells.FindKeyPos(gsID.m_key)) != NULL)
  if ((pos = spells.FindIDPos(spellID)) != NULL)
    return spells.PeekAtPos(pos).selected;
  else
    return 0;
*/
  return GetSelectedCount(LocateCharacterSpell(spellID));
  
}


//*****************************************************************************
//    NAME: SPELL_LIST::getMemorizedCount
//
// PURPOSE: 
//
//*****************************************************************************
//int SPELL_LIST::GetMemorizedCount(GLOBAL_SPELL_ID gsID) const
int SPELL_LIST::GetMemorizedCount(int index) const
{
/*
  POSITION pos;
  //if ((pos = spells.FindKeyPos(gsID.m_key)) != NULL)
  if ((pos = spells.FindIDPos(spellID)) != NULL)
    return spells.PeekAtPos(pos).memorized;
  else
    return 0;
*/
  if (index < 0) return 0;
  return PeekCharacterSpell(index)->memorized;
  //return spells[index].memorized;
}

int SPELL_LIST::GetMemorizedCount(const SPELL_ID& spellID) const
{
/*
  POSITION pos;
  //if ((pos = spells.FindKeyPos(gsID.m_key)) != NULL)
  if ((pos = spells.FindIDPos(spellID)) != NULL)
    return spells.PeekAtPos(pos).memorized;
  else
    return 0;
*/
  return GetMemorizedCount(LocateCharacterSpell(spellID));
}

//*****************************************************************************
//    NAME: SPELL_LIST::GetLevel
//
// PURPOSE: 
//
//*****************************************************************************
//int SPELL_LIST::GetMemorizedCount(GLOBAL_SPELL_ID gsID) const
int SPELL_LIST::GetLevel(int index) const
{
/*
  POSITION pos;
  //if ((pos = spells.FindKeyPos(gsID.m_key)) != NULL)
  if ((pos = spells.FindIDPos(spellID)) != NULL)
    return spells.PeekAtPos(pos).memorized;
  else
    return 0;
*/
  if (index < 0) return 0;
  return PeekCharacterSpell(index)->level;
  //return spells[index].level;
}

int SPELL_LIST::GetLevel(const SPELL_ID& spellID) const
{
/*
  POSITION pos;
  //if ((pos = spells.FindKeyPos(gsID.m_key)) != NULL)
  if ((pos = spells.FindIDPos(spellID)) != NULL)
    return spells.PeekAtPos(pos).memorized;
  else
    return 0;
*/
  return GetLevel(LocateCharacterSpell(spellID));
}

//*****************************************************************************
//    NAME: SPELL_LIST::getMemorizedTime
//
// PURPOSE: 
//
//*****************************************************************************
//int SPELL_LIST::GetMemorizedTime(GLOBAL_SPELL_ID gsID) const
int SPELL_LIST::GetMemorizedTime(const SPELL_ID& spellID) const
{
/*
  POSITION pos;
  //if ((pos = spells.FindKeyPos(gsID.m_key)) != NULL)
  if ((pos = spells.FindIDPos(spellID)) != NULL)
    return spells.PeekAtPos(pos).memTime;
  else
    return 0;
*/
  int index;
  index = LocateCharacterSpell(spellID);
  if (index < 0) return 0;
  return PeekCharacterSpell(index)->memTime;
  //return spells[index].memTime;
}

//*****************************************************************************
//    NAME: SPELL_LIST::resetMemorizedTime
//
// PURPOSE: 
//
//*****************************************************************************
//int SPELL_LIST::ResetMemorizedTime(GLOBAL_SPELL_ID gsID)
int SPELL_LIST::ResetMemorizedTime(const SPELL_ID& spellID)
{
/*
  POSITION pos;
  //if ((pos = spells.FindKeyPos(gsID.m_key)) != NULL)
  if ((pos = spells.FindIDPos(spellID)) != NULL)
  {
    int prev = spells.GetAtPos(pos).memTime;
    spells.GetAtPos(pos).memTime = 0;
    return prev;
  }
  else
    return 0;
*/
  int index, prev;
  index = LocateCharacterSpell(spellID);
  if (index < 0) return 0;
  prev = PeekCharacterSpell(index)->memTime;
  //prev = spells[index].memTime;
  GetCharacterSpell(index)->memTime = 0;
  //spells[index].memTime = 0;
  return prev;
}

//*****************************************************************************
//    NAME: SPELL_LIST::incMemorizedTime
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL SPELL_LIST::IncMemorizedTime(GLOBAL_SPELL_ID gsID, int minuteInc)
BOOL SPELL_LIST::IncMemorizedTime(const SPELL_ID& spellID, int minuteInc)
{
/*
  POSITION pos;
  //if ((pos = spells.FindKeyPos(gsID.m_key)) != NULL)
  if ((pos = spells.FindIDPos(spellID)) != NULL)
    return (spells.GetAtPos(pos).IncMemorizedTime(minuteInc, false));
  else
    return FALSE;
*/
  int index;
  index = LocateCharacterSpell(spellID);
  if (index < 0)return FALSE;
  return GetCharacterSpell(index)->IncMemorizedTime(minuteInc, false);
  //return spells[index].IncMemorizedTime(minuteInc, false);
}

//*****************************************************************************
//    NAME: SPELL_LIST::SetSpell
//
// PURPOSE: 
//
//*****************************************************************************
/* Nowhere used!
BOOL SPELL_LIST::SetSpell(GLOBAL_SPELL_ID index, CHARACTER_SPELL &spell)
{
  POSITION pos;
  if ((pos = spells.FindKeyPos(index)) != NULL)
  {
    spells.GetAt(pos) = spell;
    return TRUE;
  }
  else
    return FALSE;
}
*/
//*****************************************************************************
//    NAME: SPELL_LIST::GetRestTime
//
// PURPOSE: 
//
//*****************************************************************************
//DWORD SPELL_LIST::GetRestTime(GLOBAL_SPELL_ID gsID) const
DWORD SPELL_LIST::GetRestTime(const SPELL_ID& spellID) const
{
/*
  POSITION pos;
  //if ((pos = spells.FindKeyPos(gsID.m_key)) != NULL)
  if ((pos = spells.FindIDPos(spellID)) != NULL)
  {
    const CHARACTER_SPELL &cdata = spells.PeekAtPos(pos);
    //SPELL_DATA *sdata = spellData.GetSpellData(cdata.spell);
    SPELL_DATA *sdata = spellData.GetSpellData(cdata.spellID);
    if (sdata != NULL)
      return (GetSpellMemorizeTime(sdata->Level));
  }
  ASS ERT(FALSE);
  return 0;
*/
  return GetRestTime(LocateCharacterSpell(spellID));
}

DWORD SPELL_LIST::GetRestTime(int index) const
{
  if (index < 0) return 0;
  SPELL_DATA *pSpell = spellData.GetSpell(PeekCharacterSpell(index)->spellID);
  //SPELL_DATA *pSpell = spellData.GetSpell(spells[index].spellID);
  if (pSpell != NULL)
  {
    return GetSpellMemorizeTime(pSpell->Level);
  };
  return 0;
};


///////////////////////////////////////////////////////////////////////
//
// Spell Book Functions
//
///////////////////////////////////////////////////////////////////////

//*****************************************************************************
//    NAME: spellBookType::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void spellBookType::Serialize(CArchive &ar, double version)
{  // 0xda2
  spellLimits.Serialize(ar, version);
  list.Serialize(ar, version);
}

//*****************************************************************************
//    NAME: spellBookType::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void spellBookType::Serialize(CAR &ar, double version)
{
  spellLimits.Serialize(ar, version);
  list.Serialize(ar, version);
}

#ifdef UAFEDITOR

const char *JKEY_SPELLBOOK = "spellbook";

void spellBookType::Export(JWriter& jw)
{
  jw.StartList(JKEY_SPELLBOOK);
  spellLimits.Export(jw);
  list.Export(jw);
  jw.EndList();
}

bool spellBookType::Import(JReader& jr)
{
  if (jr.Optional(), !jr.StartList(JKEY_SPELLBOOK)) return false;
  spellLimits.Import(jr);
  list.Import(jr);
  jr.EndList();
  return true;
}

void spellBookType::CrossReference(CR_LIST *pCRList) const
{
  CR_REFERENCE CRReference;
  CRReference.m_referenceName = "SpellBook";
  CRReference.m_referenceType = CR_TYPE_spellBook;
  list.CrossReference(pCRList, &CRReference);
}
#endif


//*****************************************************************************
//    NAME: spellBookType::SpellsKnown
//
// PURPOSE: 
//
//*****************************************************************************
int spellBookType::SpellsKnown() 
{ 
  return list.GetCount(); 
}
//*****************************************************************************
//    NAME: spellBookType::SpellsMemorized
//
// PURPOSE: 
//
//*****************************************************************************
int spellBookType::SpellsMemorized() 
{ 
  return list.SpellsMemorized(); 
}

//*****************************************************************************
//    NAME: spellBookType::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void spellBookType::Clear()
{
  spellLimits.Clear();
  list.Clear();
  spellPrepTimeNeeded=0;
  spellPrepTimeUsed=0;
}

//*****************************************************************************
//    NAME: spellBookType::operator=
//
// PURPOSE: 
//
//*****************************************************************************
spellBookType& spellBookType::operator=(const spellBookType &book)
{
  if (this==&book)return *this;
  spellLimits = book.spellLimits;
  list = book.list;
  spellPrepTimeNeeded=book.spellPrepTimeNeeded;
  spellPrepTimeUsed=book.spellPrepTimeUsed;
  return *this;
}

//*****************************************************************************
//    NAME: spellBookType::operator==
//
// PURPOSE: 
//
//*****************************************************************************
BOOL spellBookType::operator==(const spellBookType &book)
{
  if ((spellLimits == book.spellLimits)==FALSE) return FALSE;
  if ((list == book.list)==FALSE) return FALSE;
  return TRUE;
}

//BOOL spellBookType::GetSpell(GLOBAL_SPELL_ID gsID, CHARACTER_SPELL& cspell)
BOOL spellBookType::FetchCharacterSpell(const SPELL_ID& spellID, CHARACTER_SPELL *pCharSp) const
{
  //return list.GetSpell(gsID, cspell);
  return list.FetchCharacterSpell(spellID, pCharSp);
}

int spellBookType::LocateCharacterSpell(const SPELL_ID& spellID) const
{
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    const CHARACTER_SPELL *pCharSpell;
    pCharSpell = PeekCharacterSpell(i);
    if (pCharSpell->spellID == spellID) return i;
  };
  return -1;
}

const CHARACTER_SPELL *spellBookType::PeekCharacterSpell(const SPELL_ID& spellID) const
{
  return list.PeekCharacterSpell(spellID);
};

const CHARACTER_SPELL *spellBookType::PeekCharacterSpell(int index) const
{
  return list.PeekCharacterSpell(index);
};

CHARACTER_SPELL *spellBookType::GetCharacterSpell(int index)
{
  return list.GetCharacterSpell(index);
};



//*****************************************************************************
//    NAME: spellBookType::isKnown
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL spellBookType::IsKnown(GLOBAL_SPELL_ID index) const 
BOOL spellBookType::IsKnown(const SPELL_ID& spellID) const 
{ 
  //return (list.HaveSpell(index)); 
  return (list.HaveSpell(spellID)); 
}

//*****************************************************************************
//    NAME: spellBookType::isMemorized
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL spellBookType::IsMemorized(GLOBAL_SPELL_ID index) const
BOOL spellBookType::IsMemorized(const SPELL_ID& spellID) const
{ 
  // index is spell book key, not spell database key
  //return list.IsMemorized(index); 
  return list.IsMemorized(spellID); 
}

//*****************************************************************************
//    NAME: spellBookType::isSelected
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL spellBookType::isSelected(GLOBAL_SPELL_ID index) const
BOOL spellBookType::isSelected(const SPELL_ID& spellID) const
{ 
  //return list.IsSelected(index); 
  return list.IsSelected(spellID); 
}

//*****************************************************************************
//    NAME: spellBookType::RemoveSpellLevel
//
// PURPOSE: 
//
//*****************************************************************************
void spellBookType::RemoveSpellLevel(int level) 
{ 
  // all spells, all classes, for this level.
  // only used by editor for cleanup.
//  spellLimits.RemoveSpellLevel(level);
  list.RemoveSpellLevel(level); 
}


/*
//*****************************************************************************
//    NAME: spellBookType::CanKnowSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL spellBookType::CanKnowSpell(WORD CasterClassFlag, WORD SpellClassFlag, WORD CastMask, int SpellLevel)
BOOL spellBookType::CanKnowSpell(//const CLASS_DATA *pClass, 
                                 const SCHOOL_ID& schoolID, //Yes
                                 //const BASECLASS_LIST& spellAllowedBaseclasses, 
                                 int   SpellLevel)
{ 
  //return spellLimits.CanKnowSpell(CasterClassFlag, SpellClassFlag, CastMask, SpellLevel); 
//  return spellLimits.CanKnowSpell(schoolID, SpellLevel); 

  Not Implemented(0xe3b812, false);
  return false;
}
*/


// this will add a spell to the book of known spells (or remove it). Spells that
// cannot be cast yet by a character (due to char level or other limitations) can
// still be added to the book, but cannot be cast by the player until such
// limitations are reached.
//
// spells which exceed the casters capability limitations cannot be added
//
//*****************************************************************************
//    NAME: spellBookType::knowSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL spellBookType::KnowSpell(WORD CasterClassFlag, WORD SpellClassFlag, GLOBAL_SPELL_ID SpellDbKey, int SpellLevel, BOOL known)
//BOOL spellBookType::KnowSpell(WORD CasterClassFlag, 
//                              WORD SpellClassFlag, 
//                              const SPELL_ID& spellID, 
//                              int SpellLevel, 
//                              BOOL known)
BOOL spellBookType::KnowSpellxxx(const SPELL_DATA *pSpell, BOOL known)
{
  if (pSpell == NULL) return FALSE;
  if (known)
  {
    int i, n;
    SPELL_ID spellID;
    // already know it?
    //if (list.HaveSpell(SpellDbKey))
    spellID = pSpell->SpellID();
    if (list.HaveSpell(spellID))
      return TRUE;
    const BASECLASS_LIST *pBaseclassList;
    pBaseclassList = &pSpell->allowedBaseclasses;
    // make sure spell can be put into spell book, even if character is not currently
    // at high enough character level to cast it
    //if (spellLimits.AddSpell(CasterClassFlag, SpellClassFlag, CastMask, SpellLevel))
    //if (spellLimits.KnowSpell(baseclassID, pSpell))
    n = pBaseclassList->GetBaseclassIDCount();
	  i = 0;
	/* Unreachable
    for (i=0; i<n; i++)
    {
//      if (spellLimits.KnowSpell(*pBaseclassList->PeekBaseclassID(i), pSpell))
      {
        break;
      };
    };
	*/
    if (i < n)
    {
      CHARACTER_SPELL data;
      //data.spell = SpellDbKey;
      data.spellID = spellID;
      //data.level = SpellLevel;
      data.level = pSpell->Level;
      data.memorized = 0;
      data.selected = 0;

      if (list.AddSpell(data))
      {
        //list.SortSpells();
        return TRUE;
      }
//      else
        // Decrement spell count for this class and spell level.
        //spellLimits.DecrementSpellCount(CasterClassFlag, SpellLevel);
//        spellLimits.DecrementSpellCount(pSpell);
    }
    return FALSE;
  }
  else // remove spell from book
  {
    // already don't have it?
    //if (!list.HaveSpell(SpellDbKey))
    SPELL_ID spellID;
    spellID = pSpell->SpellID();
    if (!list.HaveSpell(spellID))
      return TRUE;

    //spellLimits.DecrementSpellCount(CasterClassFlag, SpellLevel);
//    spellLimits.DecrementSpellCount(pSpell);
    //if (list.RemoveSpell(SpellDbKey))
    if (list.RemoveSpell(spellID))
    {
      //list.SortSpells();
      return TRUE;
    }
    return FALSE;
  }
  //return TRUE;
}


BOOL spellBookType::KnowSpell(const BASECLASS_ID& baseclassID, 
                              const SCHOOL_ID& spellSchoolID, 
                              const SPELL_ID& spellID, 
                              int SpellLevel, 
                              BOOL known)
{
  const SPELL_DATA *pSpell;
  pSpell = spellData.PeekSpell(spellID);
  if (pSpell == NULL) return FALSE;
  if (known)
  {
    // already know it?
    //if (list.HaveSpell(SpellDbKey))
    if (list.HaveSpell(spellID))
      return TRUE;
    
    //WORD CastMask = spellData.GetSpellCastMask(SpellDbKey);
    //WORD CastMask = spellData.GetSpellCastMask(spellID);
    const BASECLASS_LIST *pBaseclassList;
    pBaseclassList = &pSpell->allowedBaseclasses;
    // make sure spell can be put into spell book, even if character is not currently
    // at high enough character level to cast it
    //if (spellLimits.AddSpell(CasterClassFlag, SpellClassFlag, CastMask, SpellLevel))
//    if (spellLimits.KnowSpell(baseclassID, pSpell))
    {
      CHARACTER_SPELL data;
      //data.spell = SpellDbKey;
      data.spellID = spellID;
      data.level = SpellLevel;
      data.memorized = 0;
      data.selected = 0;

      if (list.AddSpell(data))
      {
        //list.SortSpells();
        return TRUE;
      }
//      else
        // Decrement spell count for this class and spell level.
        //spellLimits.DecrementSpellCount(CasterClassFlag, SpellLevel);
//        spellLimits.DecrementSpellCount(pSpell);
    }
    return FALSE;
  }
  else // remove spell from book
  {
    // already don't have it?
    //if (!list.HaveSpell(SpellDbKey))
    if (!list.HaveSpell(spellID))
      return TRUE;

    //spellLimits.DecrementSpellCount(CasterClassFlag, SpellLevel);
//    spellLimits.DecrementSpellCount(pSpell);
    //if (list.RemoveSpell(SpellDbKey))
    if (list.RemoveSpell(spellID))
    {
      //list.SortSpells();
      return TRUE;
    }
    return FALSE;
  }
}

//*****************************************************************************
//    NAME: spellBookType::CalcRestTime
//
// PURPOSE: 
//
//*****************************************************************************
DWORD spellBookType::CalcRestTime(void)
{  
  spellPrepTimeNeeded = CalcSpellPrepTime();

  DWORD total=0;
/*
  POSITION p = list.GetUnsortedHeadPosition();
  while (p != NULL)
  {
    int selcnt = list.GetUnsortedAt(p).selected;
    int memcnt = list.GetUnsortedAt(p).memorized;
    
    if ((selcnt > 0) && (selcnt > memcnt))
    {     
      //int single = list.GetRestTime(list.GetUnsortedKeyAt(p));
      int single = list.GetRestTime(list.GetUnsortedIDAt(p));
      total += (single * (selcnt-memcnt));
    }
    list.PeekUnsortedNext(p);
  }
*/
  int i, n;
  int single;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    int selcnt;
    int memcnt;
    selcnt = list.GetSelectedCount(i);
    memcnt = list.GetMemorizedCount(i);
    single = list.GetRestTime(i);
    total += (single * (selcnt-memcnt));
  };
  total += spellPrepTimeNeeded;

  return total;
}

//*****************************************************************************
//    NAME: spellBookType::CalcSpellPrepTime
//
// PURPOSE: 
//
//*****************************************************************************
DWORD spellBookType::CalcSpellPrepTime(void)
{
  int highlevel=-1;
/*
  POSITION p = list.GetUnsortedHeadPosition();
  while (p != NULL)
  {
    int s = list.GetUnsortedAt(p).selected;
    int m = list.GetUnsortedAt(p).memorized;
    
    if ((s > 0) && (s > m))
    {     
      if (list.GetUnsortedAt(p).level > highlevel)
        highlevel = list.GetUnsortedAt(p).level;
    }
    list.PeekUnsortedNext(p);
  }
*/
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    int s, m;
    s = list.GetSelectedCount(i);
    m = list.GetMemorizedCount(i);
    if ((s > 0) && (s > m))
    {     
      if (list.GetLevel(i) > highlevel)
      {
        highlevel = list.GetLevel(i);
      };
    }
  };
  if (highlevel > 0)
  {
    return (GetSpellPrepTime(highlevel));
  };
  return 0;
}

//*****************************************************************************
//    NAME: spellBookType::UnselectIfNotMemorized
//
// PURPOSE: 
//
//*****************************************************************************
void spellBookType::UnselectIfNotMemorized(void)
{
/*
  POSITION p = list.GetUnsortedHeadPosition();
  while (p != NULL)
  {
    int s = list.GetUnsortedAt(p).selected;
    int m = list.GetUnsortedAt(p).memorized;

    if ((s > 0) && (s > m))
    {
      list.Selected(p, m);
    }
    list.PeekUnsortedNext(p);
  }
*/
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    int s, m;
    s = list.GetSelectedCount(i);
    m = list.GetMemorizedCount(i);
    if ((s > 0) && (s > m))
    {
      list.Selected(i, m);
    };
  };
}

//*****************************************************************************
//    NAME: spellBookType::UnselectAll
//
// PURPOSE: 
//
//*****************************************************************************
void spellBookType::UnselectAll(void)
{
/*
  POSITION p = list.GetUnsortedHeadPosition();
  while (p != NULL)
  {
    list.Selected(p, FALSE);
    list.PeekUnsortedNext(p);
  };
*/
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    list.Selected(i, FALSE);
  };
}

//*****************************************************************************
//    NAME: spellBookType::selectSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL spellBookType::SelectSpell(GLOBAL_SPELL_ID index, int selected)
BOOL spellBookType::SelectSpell(const SPELL_ID& spellID, int selected)
{
  //if (IsKnown(index))
  if (IsKnown(spellID))
    //return (list.SetSelected(index, selected));
    return (list.SetSelected(spellID, selected));
  else
    return FALSE; // don't know this spell
}

//*****************************************************************************
//    NAME: spellBookType::getSelectedCount
//
// PURPOSE: 
//
//*****************************************************************************
//int spellBookType::GetSelectedCount(GLOBAL_SPELL_ID index) const
int spellBookType::GetSelectedCount(const SPELL_ID& spellID) const
{
  //return list.GetSelectedCount(index);
  return list.GetSelectedCount(spellID);
}

//*****************************************************************************
//    NAME: spellBookType::getMemorizedCount
//
// PURPOSE: 
//
//*****************************************************************************
//int spellBookType::GetMemorizedCount(GLOBAL_SPELL_ID gsID) const
int spellBookType::GetMemorizedCount(const SPELL_ID& spellID) const
{
  //return list.GetMemorizedCount(gsID);
  return list.GetMemorizedCount(spellID);
}

//*****************************************************************************
//    NAME: spellBookType::getMemorizedTime
//
// PURPOSE: 
//
//*****************************************************************************
//int spellBookType::GetMemorizedTime(GLOBAL_SPELL_ID gsID) const
int spellBookType::GetMemorizedTime(const SPELL_ID& spellID) const
{
  //return list.GetMemorizedTime(gsID);
  return list.GetMemorizedTime(spellID);
}

//*****************************************************************************
//    NAME: spellBookType::resetMemorizedTime
//
// PURPOSE: 
//
//*****************************************************************************
//int spellBookType::ResetMemorizedTime(GLOBAL_SPELL_ID gsID)
int spellBookType::ResetMemorizedTime(const SPELL_ID& spellID)
{
  //return list.ResetMemorizedTime(gsID);
  return list.ResetMemorizedTime(spellID);
}

//*****************************************************************************
//    NAME: spellBookType::incMemorizedTime
//
// PURPOSE: 
//
//*****************************************************************************
//int spellBookType::IncMemorizedTime(GLOBAL_SPELL_ID gsID, int minuteInc)
int spellBookType::IncMemorizedTime(const SPELL_ID& spellID, int minuteInc)
{
  //return list.IncMemorizedTime(gsID, minuteInc);
  return list.IncMemorizedTime(spellID, minuteInc);
}

//*****************************************************************************
//    NAME: spellBookType::resetAllMemorizedTime
//
// PURPOSE: 
//
//*****************************************************************************
void spellBookType::ResetAllMemorizedTime()
{
/*
  POSITION p = list.GetUnsortedHeadPosition();
  while (p != NULL)
  {
    list.ResetMemorizedTime(p);
    list.PeekUnsortedNext(p);
  }
*/
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    list.ResetMemorizedTime(i);
  };
}

//*****************************************************************************
//    NAME: spellBookType::incAllMemorizedTime
//
// PURPOSE: 
//
//*****************************************************************************
BOOL spellBookType::IncAllMemorizedTime(int minuteInc, bool all)
{
  if (!all)
  {
  spellPrepTimeUsed += minuteInc;
    if (spellPrepTimeUsed <= spellPrepTimeNeeded)
    {
      return FALSE;
    }
    else if (spellPrepTimeNeeded > 0)
    {
      int delta = spellPrepTimeUsed - spellPrepTimeNeeded;
      if (minuteInc > delta)
        minuteInc -= delta;
    }
  };
  spellPrepTimeNeeded = 0;
  spellPrepTimeUsed = 0;
/*
  POSITION p = list.GetHeadPosition(); // access sorted list
  // should be at least one spell, 
  // and they should be sorted by now
  ASSERT( p != NULL );
  while (p != NULL)
  {
    CHARACTER_SPELL &data = list.GetAtPos(p);
    if (data.HaveUnmemorized())
    {
      BOOL result;
      // Only 1 spell can be memorized at a time.
      // There may be several unmemorized spells, so
      // inc mem time for first spell until it is memorized
      // and then add mem time to next one, and so on.
      result = (data.IncMemorizedTime(minuteInc, all));
      if (!all) return result;
    }
    list.GetNext(p);
  }
*/
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    CHARACTER_SPELL *pCharSp;
    pCharSp = list.GetCharacterSpell(i);
    if (pCharSp ->HaveUnmemorized())
    {
      BOOL result;
      // Only 1 spell can be memorized at a time.
      // There may be several unmemorized spells, so
      // inc mem time for first spell until it is memorized
      // and then add mem time to next one, and so on.
      result = pCharSp->IncMemorizedTime(minuteInc, all);
      if (!all) return result;
    };
  };
  return FALSE;
}

///////////////////////////////////////////////////////////////////////
//
// Spell Data Functions
//
///////////////////////////////////////////////////////////////////////

//*****************************************************************************
//    NAME: SPELL_DATA::SPELL_DATA
//
// PURPOSE: 
//
//*****************************************************************************
SPELL_DATA::SPELL_DATA() : 
    // ASLs named "SPELL_DATA_ATTRIBUTES"
	spell_asl(),
		temp_asl(),
	hCastSound(-1),
  hMissileSound(-1),
  hCoverageSound(-1),
  hHitSound(-1),
  hLingerSound(-1),
  specAbs(true)
{ 
  //m_gsID.Clear();//key = 0;
  Name="";
  Description="";
  CastSound = "";
  MissileSound="";
  CoverageSound="";
  HitSound="";
  LingerSound="";
  CastMsg="";
  ClearSounds();
  ClearArt();
  //schoolMask = MagicUserFlag;
  schoolID = "";
  //CastMask = MagicUserFlag;
  allowedBaseclasses.Clear();
  Level = 0;
  Casting_Time = 0;
  Casting_Time_Type = sctInitiative;
  CanTargetFriend = FALSE;
  CanTargetEnemy = TRUE;
  IsCumulative=TRUE;
  //Combat_Only = FALSE;
  //NotInCombat(FALSE);
  //NotInCamp(FALSE);
  InCombat(TRUE);
  InCamp(TRUE);
  CanBeDispelled=TRUE;
  //UsableByParty=TRUE;
  CanMemorize=TRUE;
  //CanScribeAtLevelChange=FALSE;
  AllowScribe=FALSE;
  //AutoScribeAtLevelChange=FALSE;
  AutoScribe=FALSE;
  Lingers=FALSE;
  LingerOnceOnly=FALSE;
  Save_Vs = Sp;
  Save_Result = NoSave;
  specAbs.Clear();
  m_EffectedAttributes.RemoveAll();
  Duration.Clear();
  EffectDuration.Clear();
  Duration_Rate = Permanent;
  //NumTargets.Clear();
  P1.Clear();
  //TargetRange.Clear();
  P2.Clear();
  P3.Clear();
  P4.Clear();
  P5.Clear();
  Targeting = Self;
  Cast_Cost = 0;
  Cast_Priority = 500;
  SpellBeginSource.Empty();
  SpellBeginBinary.Empty();
  SpellEndSource.Empty();
  SpellEndBinary.Empty();
  SavingThrowSource.Empty();           // 1.0303
  SavingThrowBinary.Empty();           // 1.0303
  SavingThrowSucceededSource.Empty();  // 1.0303
  SavingThrowSucceededBinary.Empty();  // 1.0303
  SavingThrowFailedSource.Empty();     // 1.0303
  SavingThrowFailedBinary.Empty();     // 1.0303
  //
  SpellInitiationSource.Empty();       // 2.60
  SpellInitiationBinary.Empty();       // 2.60
  SpellTerminationSource.Empty();      // 2.60
  SpellTerminationBinary.Empty();       // 2.60
}
//*****************************************************************************
//    NAME: SPELL_DATA::SetSpellAnimDefaults
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::SetSpellAnimDefaults(PIC_DATA &art, const char *file)
{
  if (strcmp(art.filename, "NA") == 0)
  {
    art.Clear();
    return;
  }

  if (art.filename.CompareNoCase("DEFAULT") == 0)
  {
#ifdef UAFEDITOR
    art.filename = ede.TemplateSpellArtDir() + file;
#else
    art.filename = file;
#endif
    art.picType = SpriteDib;
    art.NumFrames = 2;
    art.FrameWidth = COMBAT_TILE_WIDTH;
    art.FrameHeight = COMBAT_TILE_HEIGHT;
    art.style = PIC_DATA::AS_None;
    art.timeDelay = 210;
    art.MaxLoops = 5;
    art.flags = 6;
  }
}


int SPELL_DATA::GetDataSize(void) const
{
  return sizeof(*this) + specAbs.GetDataSize();
}

//*****************************************************************************
//    NAME: SPELL_DATA::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::Clear(BOOL ctor)
{
  //m_gsID.Clear();//key = 0;
  Name="";
  Description="";
  preSpellNameKey = -1;
  CastSound = "";
  MissileSound="";
  CoverageSound="";
  HitSound="";
  LingerSound="";
  CastMsg="";
  ClearSounds();
  ClearArt();
  //schoolMask = MagicUserFlag;
  schoolID = "";
  //CastMask = MagicUserFlag;
  allowedBaseclasses.Clear();
  Level = 0;
  Casting_Time = 0;
  Casting_Time_Type = sctInitiative;
  IsCumulative=TRUE;
  CanTargetFriend = FALSE;
  CanTargetEnemy = TRUE;
  //Combat_Only = FALSE;
  //NotInCombat(FALSE);
  //NotInCamp(FALSE);
  InCombat(TRUE);
  InCamp(TRUE);
  CanBeDispelled=TRUE;
  //UsableByParty=TRUE;
  CanMemorize=TRUE;
  //CanScribeAtLevelChange=FALSE;
  AllowScribe=FALSE;
  //AutoScribeAtLevelChange=FALSE;
  AutoScribe=FALSE;
  Lingers=FALSE;
  LingerOnceOnly=FALSE;
  Save_Vs = Sp;
  Save_Result = NoSave;
  specAbs.Clear();
  m_EffectedAttributes.RemoveAll();
  Duration.Clear();
  EffectDuration.Clear();
  Duration_Rate = Permanent;
  //NumTargets.Clear();
  P1.Clear();
  //TargetRange.Clear();
  P2.Clear();
  P3.Clear();
  P4.Clear();
  P5.Clear();
  Targeting = Self;
  Cast_Cost = 0;
  Cast_Priority = 500;
  spell_asl.Clear();
  SpellBeginSource.Empty();
  SpellBeginBinary.Empty();
  SpellEndSource.Empty();
  SpellEndBinary.Empty();
  SavingThrowSource.Empty();
  SavingThrowBinary.Empty();
  SavingThrowSucceededSource.Empty();
  SavingThrowSucceededBinary.Empty();
  SavingThrowFailedSource.Empty();
  SavingThrowFailedBinary.Empty();
  SpellInitiationSource.Empty();
  SpellInitiationBinary.Empty();
  SpellTerminationSource.Empty();
  SpellTerminationBinary.Empty();
}

//*****************************************************************************
//    NAME: &SPELL_DATA::operator=
//
// PURPOSE: 
//
//*****************************************************************************
SPELL_DATA &SPELL_DATA::operator=(const SPELL_DATA &src)
{
  if (&src == this)
    return *this;
  hCastSound = src.hCastSound;
  hMissileSound=src.hMissileSound;
  hCoverageSound=src.hCoverageSound;
  hHitSound=src.hHitSound;
  hLingerSound=src.hLingerSound;
  //m_gsID = src.m_gsID;//key = src.key;
  preSpellNameKey = src.preSpellNameKey;
  Name=src.Name;
  Description = src.Description;
#ifdef UAFEngine
  uniqueName = src.uniqueName;
#endif
  CastSound = src.CastSound;
  MissileSound=src.MissileSound;
  CoverageSound=src.CoverageSound;
  HitSound=src.HitSound;
  LingerSound=src.LingerSound;
  CastMsg=src.CastMsg;
  //schoolMask = src.schoolMask;
  schoolID = src.schoolID;
  //CastMask = src.CastMask;
  allowedBaseclasses = src.allowedBaseclasses;
  Level = src.Level;
  Casting_Time = src.Casting_Time;
  Casting_Time_Type = src.Casting_Time_Type;
  CanTargetFriend = src.CanTargetFriend;
  CanTargetEnemy = src.CanTargetEnemy;
  IsCumulative = src.IsCumulative;
  //Combat_Only = src.Combat_Only;
  //NotInCombat(src.NotInCombat());
  //NotInCamp(src.NotInCamp());
  InCombat(src.InCombat());
  InCamp(src.InCamp());
  CanBeDispelled=src.CanBeDispelled;
  //UsableByParty=src.UsableByParty;
  CanMemorize=src.CanMemorize;
  //CanScribeAtLevelChange=src.CanScribeAtLevelChange;
  AllowScribe=src.AllowScribe;
  //AutoScribeAtLevelChange=src.AutoScribeAtLevelChange;
  AutoScribe=src.AutoScribe;
  Lingers=src.Lingers;
  LingerOnceOnly=src.LingerOnceOnly;
  Save_Vs = src.Save_Vs;
  Save_Result = src.Save_Result;
  specAbs.Copy(src.specAbs);
  Targeting = src.Targeting;
  Cast_Cost = src.Cast_Cost;
  Cast_Priority = src.Cast_Priority;

  CastArt = src.CastArt;
  MissileArt = src.MissileArt;
  TargCoverageArt = src.TargCoverageArt;
  TargHitArt = src.TargHitArt;
  TargLingerArt = src.TargLingerArt;
  Duration = src.Duration;
  EffectDuration = src.EffectDuration;
  Duration_Rate = src.Duration_Rate;
  P1 = src.P1;
  //NumTargets = src.NumTargets;
  //TargetRange = src.TargetRange;
  P2 = src.P2;
  P3 = src.P3;
  P4 = src.P4;
  P5 = src.P5;

  spell_asl.Copy(src.spell_asl);
  temp_asl.Copy(src.temp_asl);
  m_EffectedAttributes.RemoveAll();
  POSITION pos = src.m_EffectedAttributes.GetHeadPosition();
  while (pos != NULL)
  {
    SPELL_EFFECTS_DATA tmp=*src.m_EffectedAttributes.PeekNext(pos);
    m_EffectedAttributes.AddTail(tmp);
  }
  SpellBeginSource           = src.SpellBeginSource;
  SpellBeginBinary           = src.SpellBeginBinary;
  SpellEndSource             = src.SpellEndSource;
  SpellEndBinary             = src.SpellEndBinary;
  SavingThrowSource          = src.SavingThrowSource;
  SavingThrowBinary          = src.SavingThrowBinary;
  SavingThrowSucceededSource = src.SavingThrowSucceededSource;
  SavingThrowSucceededBinary = src.SavingThrowSucceededBinary;
  SavingThrowFailedSource    = src.SavingThrowFailedSource;
  SavingThrowFailedBinary    = src.SavingThrowFailedBinary;
  SavingThrowFailedSource    = src.SavingThrowFailedSource;
  SavingThrowFailedBinary    = src.SavingThrowFailedBinary;
  SpellInitiationSource      = src.SpellInitiationSource;
  SpellInitiationBinary      = src.SpellInitiationBinary;
  SpellTerminationSource     = src.SpellTerminationSource;
  SpellTerminationBinary     = src.SpellTerminationBinary;
  return *this;
}

//*****************************************************************************
//    NAME: SPELL_DATA::operator==
//
// PURPOSE: 
//
//*****************************************************************************
bool SPELL_DATA::operator==(const SPELL_DATA &src) const
{
  if (&src == this) return true;
  if (hCastSound             != src.hCastSound            ) return false;
  if (hMissileSound          != src.hMissileSound         ) return false;
  if (hCoverageSound         != src.hCoverageSound        ) return false;
  if (hHitSound              != src.hHitSound             ) return false;
  if (hLingerSound           != src.hLingerSound          ) return false;
  //if (m_gsID                 != src.m_gsID                ) return false;
  if (Name                   != src.Name                  ) return false;
  if (Description            != src.Description           ) return false;
#ifdef UAFEngine
  if (uniqueName             != src.uniqueName            ) return false;
#endif
  if (CastSound              != src.CastSound             ) return false;
  if (MissileSound           != src.MissileSound          ) return false;
  if (CoverageSound          != src.CoverageSound         ) return false;
  if (HitSound               != src.HitSound              ) return false;
  if (LingerSound            != src.LingerSound           ) return false;
  if (CastMsg                != src.CastMsg               ) return false;
  //if (schoolMask             != src.schoolMask            ) return false;
  if (schoolID               != src.schoolID              ) return false;
  //if (CastMask               != src.CastMask              ) return false;
  if (allowedBaseclasses     != src.allowedBaseclasses    ) return false;
  if (Level                  != src.Level                 ) return false;
  if (Casting_Time           != src.Casting_Time          ) return false;
  if (Casting_Time_Type      != src.Casting_Time_Type     ) return false;
  if (CanTargetFriend        != src.CanTargetFriend       ) return false;
  if (CanTargetEnemy         != src.CanTargetEnemy        ) return false;
  if (IsCumulative           != src.IsCumulative          ) return false;
  //if (Combat_Only            != src.Combat_Only           ) return false;
  //if (NotInCombat()          != src.NotInCombat()         ) return false;
  //if (NotInCamp()            != src.NotInCamp()           ) return false;
  if (InCombat()             != src.InCombat()            ) return false;
  if (InCamp()               != src.InCamp()              ) return false;
  //if (UsableByParty          != src.UsableByParty         ) return false;
  if (CanMemorize            != src.CanMemorize           ) return false;
  if (CanBeDispelled         != src.CanBeDispelled        ) return false;
  //if (CanScribeAtLevelChange != src.CanScribeAtLevelChange) return false;
  if (AllowScribe            != src.AllowScribe           ) return false;
  //if (AutoScribeAtLevelChange!= src.AutoScribeAtLevelChange)return false;
  if (AutoScribe             != src.AutoScribe            )return false;
  if (Lingers                != src.Lingers               ) return false;
  if (LingerOnceOnly         != src.LingerOnceOnly        ) return false;
  if (Save_Vs                != src.Save_Vs               ) return false;
  if (Save_Result            != src.Save_Result           ) return false;
  if (!(specAbs              == src.specAbs)              ) return false;
  if (Targeting              != src.Targeting             ) return false;  
  if (Cast_Cost              != src.Cast_Cost             ) return false;
  if (Cast_Priority          != src.Cast_Priority         ) return false;

  if (!(CastArt              == src.CastArt)              ) return false;
  if (!(MissileArt           == src.MissileArt)           ) return false;
  if (!(TargCoverageArt      == src.TargCoverageArt)      ) return false;
  if (!(TargHitArt           == src.TargHitArt)           ) return false;
  if (!(TargLingerArt        == src.TargLingerArt)        ) return false;
  if (!(Duration             == src.Duration)             ) return false;
  if (!(EffectDuration       == src.EffectDuration)       ) return false;
  if (Duration_Rate          != src.Duration_Rate         ) return false;
  //if (!(NumTargets           == src.NumTargets)           ) return false;
  if (!(P1                   == src.P1)                   ) return false;
  //if (!(TargetRange          == src.TargetRange)          ) return false;
  if (!(P2                   == src.P2)                   ) return false;
  if (!(P3                   == src.P3)                   ) return false;
  if (!(P4                   == src.P4)                   ) return false;
  if (!(P5                   == src.P5)                   ) return false;
  if (SpellBeginSource           != src.SpellBeginSource          ) return false;
  if (SpellBeginBinary           != src.SpellBeginBinary          ) return false;
  if (SpellEndSource             != src.SpellEndSource            ) return false;
  if (SpellEndBinary             != src.SpellEndBinary            ) return false;
  if (SpellInitiationSource      != src.SpellInitiationSource          ) return false;
  if (SpellInitiationBinary      != src.SpellInitiationBinary          ) return false;
  if (SpellTerminationSource     != src.SpellTerminationSource            ) return false;
  if (SpellTerminationBinary     != src.SpellTerminationBinary            ) return false;
  if (SavingThrowSource          != src.SavingThrowSource         ) return false;
  if (SavingThrowBinary          != src.SavingThrowBinary         ) return false;
  if (SavingThrowSucceededSource != src.SavingThrowSucceededSource) return false;
  if (SavingThrowSucceededBinary != src.SavingThrowSucceededBinary) return false;
  if (SavingThrowFailedSource    != src.SavingThrowFailedSource   ) return false;
  if (SavingThrowFailedBinary    != src.SavingThrowFailedBinary   ) return false;
  if (!(spell_asl            == src.spell_asl)            ) return false;
  if (!(temp_asl             == src.temp_asl)             ) return false;
  if (m_EffectedAttributes.GetCount() != src.m_EffectedAttributes.GetCount()) return false;
  POSITION pos = src.m_EffectedAttributes.GetHeadPosition();
  while (pos != NULL)
    if (!HaveEffectAttr(*src.m_EffectedAttributes.PeekNext(pos))) return false;
  return true;
}

//*****************************************************************************
//    NAME: SPELL_DATA::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::Serialize(CArchive &ar, double ver)
{
//  CObject::Serialize(ar);
  long temp;
  static long Target_Dice;
  static long Target_Dice_Nbr;
  static long Target_Bonus_Level;
  static long Target_Bonus;
  static long Target_Max;
  static long Duration_Dice;
  static long Duration_Dice_Nbr;
  static long Duration_Bonus_Level;
  static long Duration_Bonus;
  static long Duration_Max;
  static long Range_Dice;
  static long Range_Dice_Nbr;
  static long Range_Bonus_Level;
  static long Range_Bonus;
  static long Range_Max;
  static long Attack_Dice;
  static long Attack_Dice_Nbr;
  static long Attack_Bonus;
  static long Attack_Bonus_Level;
  static long Attack_Max;
  static long Damage_Dice;
  static long Damage_Dice_Nbr;
  static long Damage_Bonus;
  static long Damage_Bonus_Level;
  static long Damage_Max;
  static long Protection_Dice;
  static long Protection_Dice_Nbr;
  static long Protection_Bonus;
  static long Protection_Bonus_Level;
  static long Protection_Max;
  static long Heal_Dice;
  static long Heal_Dice_Nbr;
  static long Heal_Bonus;
  static long Heal_Bonus_Level;
  static long Heal_Max;


  if (ar.IsStoring())
  {
    die("We should not be serializing spellData with CArchive");
  }
  else
  {
    //m_gsID.Serialize(ar, ver); //ar >> key;
    ar >> preSpellNameKey;  // Any verion using CArchive.
    DAS(ar,Name);
    if (globalData.version >= _VERSION_528)
    {
        DAS(ar, Description);
    }
#ifdef UAFEngine
    // In the runtime, Name means only the un-decorated
    // part of the name.  For example:  "Fireball|Green" will be
    // read as simply "FireBall".  There can be multiple objects with
    // the same name but once we are at the point where we have the binary 
    // data then everything is identified uniquely and is referenced by the 
    // binary key.  The name is only for printing.
    //
    //  *****   WRONG   ******  PRS 20120211
    // We need the fully qualified name for scripts to reference.
    // So, in the engine, we will add an additional data member to
    // the class SPELL_DATA.  We'll call it IDName.
    //
    if (Name.Find('|') >= 0) Name = Name.Left(Name.Find('|')); // Remove qualifier.
#endif

    DAS(ar,CastSound);
    //ar >> schoolMask;



/* *********** PRS 20191207  The following 'if' is
               an attempt to read an old version 0.5751 game
               named "The Last Days of Autumn".
               I am totally guessing at the version number for the
               'if' statement.  I know that the game "Fury" was version 0.831
               and it worked OK with the later code.  So something changed
               between these two versions.  ***** */
    if (ver < 0.576)
#ifdef UAFEDITOR
    {
      WORD classMask;
      BASECLASS_ID baseclass;
      ar >> classMask;
      if (classMask & ClericFlag)
      {
        baseclass = "cleric";
      }
      else
      {
        baseclass = "magicUser";
      };
      allowedBaseclasses.Add(baseclass);
    }
    else
#endif
    {
      ar >> schoolID;

      //if (ver >= _VERSION_0910_)
      //  ar >> CastMask;
      //else
      //  CastMask = 0;
      allowedBaseclasses.Serialize(ar);
    };
    // this new flag is certainly not going
    // to be correct for older designs
    //
    // try to fix it...
    /*
    if (schoolMask == MagicUserFlag)
    {
      // only MU can cast MU spells
      CastMask = MagicUserFlag;
    }
    else // ClericFlag
    {
      // MU and Cleric are mutually exclusive
      //
      // remove MU flag
      CastMask &= ~MagicUserFlag;
      
      // if that was the only bit set,
      // then at least set the cleric flag
      if (CastMask == 0)
        CastMask = ClericFlag;
    }
    */
    /* *********** PRS 20191207  The following 'if' is
                   an attempt to read an old version 0.5751 game
                   named "The Last Days of Autumn".
                   I am totally guessing at the version number for the
                   'if' statement.  I know that the game "Fury" was version 0.831
                   and it worked OK with the later code.  So something changed
                   between these two versions.  ***** */
    if (ver >= 0.576)
    /* Really */ NotImplemented(0xcc759cb, false);

    ar >> Level;
    ar >> Casting_Time;
    if (ver >= _VERSION_0662_)
    {
      ar >> temp;
      Casting_Time_Type = (spellCastingTimeType)temp;
    }
    ar >> CanTargetFriend;
    if (ver > 0.999725)
    {
      ar >> CanTargetEnemy;
    }
    else
    {
      CanTargetEnemy = TRUE;
    };

    if (ver >= _VERSION_06991_)
      ar >> IsCumulative;

    {
    //ar >> Combat_Only;
      ar >> restrictions;
      if (ver < VersionSpellNames)
      {
        restrictions ^= (IN_COMBAT | IN_CAMP);
      };
    };

    if (ver >= _VERSION_0909_)
      ar >> CanBeDispelled;

    if (ver >= _VERSION_0670_)
      //ar >> UsableByParty;
      ar >> CanMemorize;

    if (ver >= _VERSION_0692_)
      //ar >> CanScribeAtLevelChange;
      ar >> AllowScribe;

    if (ver >= _VERSION_0910_)
      //ar >> AutoScribeAtLevelChange;
      ar >> AutoScribe;

    // for older design versions
    //if ((CanScribeAtLevelChange) && (ver < _VERSION_0910_))
    if ((AllowScribe) && (ver < _VERSION_0910_))
      //AutoScribeAtLevelChange = TRUE;
      AutoScribe = TRUE;
    
    if (ver >= _VERSION_0906_)
    {
      ar >> Lingers;
      ar >> LingerOnceOnly;
    }
    
    ar >> temp;
    Save_Vs = (spellSaveVersusType)temp;

    ar >> temp;
    Save_Result = (spellSaveEffectType)temp;

    ar >> temp;
    Targeting = (spellTargetingType)temp;

    if (ver <= _VERSION_06992_)
    {
      ar >> Target_Dice;
      ar >> Target_Dice_Nbr;
      ar >> Target_Bonus_Level;
      ar >> Target_Bonus;
      ar >> Target_Max;
    }
  
    ar >> temp;
    Duration_Rate = (spellDurationType)temp;

    if (ver <= _VERSION_06992_)
    {
      ar >> Duration_Dice;
      ar >> Duration_Dice_Nbr;
      ar >> Duration_Bonus_Level;
      ar >> Duration_Bonus;
      ar >> Duration_Max;

      ar >> Range_Dice;
      ar >> Range_Dice_Nbr;
      ar >> Range_Bonus_Level;
      ar >> Range_Bonus;
      ar >> Range_Max;

      ar >> Attack_Dice;
      ar >> Attack_Dice_Nbr;
      ar >> Attack_Bonus;
      ar >> Attack_Bonus_Level;
      ar >> Attack_Max;

      ar >> Damage_Dice;
      ar >> Damage_Dice_Nbr;
      ar >> Damage_Bonus;
      ar >> Damage_Bonus_Level;
      ar >> Damage_Max;

      ar >> Protection_Dice;
      ar >> Protection_Dice_Nbr;
      ar >> Protection_Bonus;
      ar >> Protection_Bonus_Level;
      ar >> Protection_Max;

      ar >> Heal_Dice;
      ar >> Heal_Dice_Nbr;
      ar >> Heal_Bonus;
      ar >> Heal_Bonus_Level;
      ar >> Heal_Max;
    }

    ar >> Cast_Cost;
    ar >> Cast_Priority;

    if (ver >= _VERSION_0840_)
    {
      DAS(ar, MissileSound);
      DAS(ar, CoverageSound);
      DAS(ar, HitSound);
      DAS(ar, LingerSound);
    }
    if (ver >= _VERSION_0841_)
    {
      DAS(ar, CastMsg);
    }
    // this data is stored in attribute map
    // MissileArt.Serialize(ar);
    // TargCoverageArt.Serialize(ar);
    // TargHitArt.Serialize(ar);
    // TargLingerArt.Serialize(ar);

    if (ver >= _VERSION_0904_)
    {
      DAS(ar,SpellBeginSource);
      DAS(ar,SpellBeginBinary);
      DAS(ar,SpellEndSource);
      DAS(ar,SpellEndBinary);
    }
    if (ver >= 2.60)
    {
      DAS(ar,SpellInitiationSource);
      DAS(ar,SpellInitiationBinary);
      DAS(ar,SpellTerminationSource);
      DAS(ar,SpellTerminationBinary);
    }
    
    
  }

  specAbs.Serialize(ar, ver, this->Name, "spell");
#ifdef UAFEngine
  if (specAbs.FindAbility("IsASpell") == NULL)
  {
    specAbs.InsertAbility("IsASpell","","","");
  };
#endif
  spell_asl.Serialize(ar, "SPELL_DATA_ATTRIBUTES");
}


//*****************************************************************************
//    NAME: SPELL_DATA::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
#ifdef UAFEDITOR
void SPELL_DATA::AddBaseclass(int castMask, int UserFlag, char *name)
{
  BASECLASS_ID baseclassID;
  if ((castMask & UserFlag) == 0) return;
  baseclassID = name;
  allowedBaseclasses.Add(baseclassID);
}
#endif
void SPELL_DATA::Serialize(CAR &ar, double ver)
{
  long temp;
  static long Target_Dice;
  static long Target_Dice_Nbr;
  static long Target_Bonus_Level;
  static long Target_Bonus;
  static long Target_Max;
  static long Duration_Dice;
  static long Duration_Dice_Nbr;
  static long Duration_Bonus_Level;
  static long Duration_Bonus;
  static long Duration_Max;
  static long Range_Dice;
  static long Range_Dice_Nbr;
  static long Range_Bonus_Level;
  static long Range_Bonus;
  static long Range_Max;
  static long Attack_Dice;
  static long Attack_Dice_Nbr;
  static long Attack_Bonus;
  static long Attack_Bonus_Level;
  static long Attack_Max;
  static long Damage_Dice;
  static long Damage_Dice_Nbr;
  static long Damage_Bonus;
  static long Damage_Bonus_Level;
  static long Damage_Max;
  static long Protection_Dice;
  static long Protection_Dice_Nbr;
  static long Protection_Bonus;
  static long Protection_Bonus_Level;
  static long Protection_Max;
  static long Heal_Dice;
  static long Heal_Dice_Nbr;
  static long Heal_Bonus;
  static long Heal_Bonus_Level;
  static long Heal_Max;

  if (ar.IsStoring())
  {
    CompileScripts();
    if (ver <= _VERSION_0830_)
      Name.Replace('/','|');

    //m_gsID.Serialize(ar, ver); //ar << key;
    ar << preSpellNameKey;
    AS(ar,Name);
    AS(ar, Description);
    AS(ar, CastSound);
    //ar << schoolMask;
    ar << schoolID;
    //ar << CastMask;
    allowedBaseclasses.Serialize(ar);
    ar << Level;
    ar << Casting_Time;
    temp = Casting_Time_Type;
    ar << temp;
    ar << CanTargetFriend;
    ar << CanTargetEnemy;
    ar << IsCumulative;
    //ar << Combat_Only;
    ar << restrictions;
    ar << CanBeDispelled;
    //ar << UsableByParty;
    ar << CanMemorize;
    //ar << CanScribeAtLevelChange;
    ar << AllowScribe;
    //ar << AutoScribeAtLevelChange;
    ar << AutoScribe;
    ar << Lingers;
    ar << LingerOnceOnly;

    temp = Save_Vs;
    ar << temp;
    temp = Save_Result;
    ar << temp;
    temp = Targeting;
    ar << temp;

    temp = Duration_Rate;
    ar << temp;

    ar << Cast_Cost;
    ar << Cast_Priority;

    Duration.Serialize(ar);
    //NumTargets.Serialize(ar);
    P1.Serialize(ar);
    //TargetRange.Serialize(ar);
    P2.Serialize(ar);
    {
      // Introduced in version 0.999432
      P3.Serialize(ar);
      P4.Serialize(ar);
      P5.Serialize(ar);
    };
    ar << m_EffectedAttributes.GetCount();
    POSITION pos = m_EffectedAttributes.GetHeadPosition();
    while (pos != NULL)
      m_EffectedAttributes.GetNext(pos)->Serialize(ar, ver);

    if (ver >= _VERSION_0840_)
      CastArt.Serialize(ar,ver, rte.SpellArtDir());
    MissileArt.Serialize(ar,ver, rte.SpellArtDir());
    TargCoverageArt.Serialize(ar,ver, rte.SpellArtDir());
    TargHitArt.Serialize(ar,ver, rte.SpellArtDir());
    TargLingerArt.Serialize(ar,ver, rte.SpellArtDir());
    AS(ar, MissileSound);
    AS(ar, CoverageSound);
    AS(ar, HitSound);
    AS(ar, LingerSound);
    AS(ar, CastMsg);

    AS(ar,SpellBeginSource);
    AS(ar,SpellBeginBinary);
    AS(ar,SpellEndSource);
    AS(ar,SpellEndBinary); 
    AS(ar,SpellInitiationSource);
    AS(ar,SpellInitiationBinary);
    AS(ar,SpellTerminationSource);
    AS(ar,SpellTerminationBinary); 
    AS(ar,SavingThrowSource); 
    AS(ar,SavingThrowBinary); 
    AS(ar,SavingThrowSucceededSource); 
    AS(ar,SavingThrowSucceededBinary); 
    AS(ar,SavingThrowFailedSource); 
    AS(ar,SavingThrowFailedBinary); 
    EffectDuration.Serialize(ar);
  }
  else
  {
    //m_gsID.Serialize(ar, ver); //ar >> key;
#ifdef UAFEDITOR
    WORD schoolMask = 0;
#endif
    if ( (ver < VersionSpellNames) || (ver >= VersionSaveIDs) )
    {
      ar >> preSpellNameKey;
    }
    else
    {
      preSpellNameKey = -1;
    };
    DAS(ar,Name);
    if (globalData.version >= _VERSION_528)
    {
        DAS(ar, Description);
    }


#ifdef UAFEngine
    // In the runtime, Name means only the un-decorated
    // part of the name.  For example:  "Fireball|Green" will be
    // read as simply "FireBall".  There can be multiple objects with
    // the same name but once we are at the point where we have the binary 
    // data then everything is identified uniquely and is referenced by the 
    // binary key.  The name is only for printing.
    //
    //  *****   WRONG   ******  PRS 20120211
    // We need the fully qualified name for scripts to reference.
    // So, in the engine, we will add an additional data member to
    // the class SPELL_DATA.  We'll call it IDName.
    //
    uniqueName = Name;
    if (Name.Find('|') >= 0) Name = Name.Left(Name.Find('|')); // Remove qualifier.
#endif
    DAS(ar,CastSound);
#ifdef UAFEDITOR
    //ar >> schoolMask;
    if (ver < VersionSpellNames)
    {
      ar >> schoolMask;
      if (schoolMask & MagicUserFlag) schoolID = "Magic User";
      else schoolID = "Cleric";
    }
    else
#endif
    {
      ar >> schoolID;
    }; 
    if (ver >= _VERSION_0910_)
    {
#ifdef UAFEDITOR
      if (ver < VersionSpellNames)
      {
        //ar >> CastMask;
        WORD castMask;
        ar >> castMask;
        if (ver < _VERSION_0930)
        {
          // this new flag is certainly not going
          // to be correct for older designs
          //
          // try to fix it...
          if (schoolMask == MagicUserFlag)
          {
            // only MU can cast MU spells
            castMask = MagicUserFlag;
          }
          else // ClericFlag
          {
            // MU and Cleric are mutually exclusive
            //
            // remove MU flag
            castMask &= ~MagicUserFlag;
      
            // if that was the only bit set,
            // then at least set the cleric flag
            if (castMask == 0)
              castMask = ClericFlag;
          };
        };
        AddBaseclass(castMask, MagicUserFlag, "magicUser");
        AddBaseclass(castMask, ClericFlag,    "cleric");
        AddBaseclass(castMask, ThiefFlag,     "thief");
        AddBaseclass(castMask, FighterFlag,   "fighter");
        AddBaseclass(castMask, PaladinFlag,   "paladin");
        AddBaseclass(castMask, RangerFlag,    "ranger");
        AddBaseclass(castMask, DruidFlag,     "druid");

      }
      else
#endif
      {
        allowedBaseclasses.Serialize(ar);
      };
    }
    else
    {
      //CastMask = AllClasses;
      allowedBaseclasses.Clear();
    };
    ar >> Level;
    ar >> Casting_Time;
    if (ver >= _VERSION_0662_)
    {
      ar >> temp;
      Casting_Time_Type = (spellCastingTimeType)temp;
    }
    ar >> CanTargetFriend;
    if (ver > 0.999725)
    {
      ar >> CanTargetEnemy;
    }
    else
    {
      CanTargetEnemy = TRUE;
    };
    if (ver >= _VERSION_06991_)
      ar >> IsCumulative;

    //ar >> Combat_Only;
    ar >> restrictions;
    if (ver < VersionSpellNames)
    {
      restrictions ^= (IN_COMBAT | IN_CAMP);
    };

    if (ver >= _VERSION_0909_)
      ar >> CanBeDispelled;
    
    if (ver >= _VERSION_0670_)
      //ar >> UsableByParty;
      ar >> CanMemorize;

    if (ver >= _VERSION_0692_)
      //ar >> CanScribeAtLevelChange;
      ar >> AllowScribe;

    if (ver >= _VERSION_0910_)
      //ar >> AutoScribeAtLevelChange;
      ar >> AutoScribe;

    // for older design versions
    //if ((CanScribeAtLevelChange) && (ver < _VERSION_0910_))
    if ((AllowScribe) && (ver < _VERSION_0910_))
      //AutoScribeAtLevelChange = TRUE;
      AutoScribe = TRUE;

    if (ver >= _VERSION_0906_)
    {
      ar >> Lingers;
      ar >> LingerOnceOnly;
    }

    ar >> temp;
    Save_Vs = (spellSaveVersusType)temp;

    ar >> temp;
    Save_Result = (spellSaveEffectType)temp;

    ar >> temp;
    Targeting = (spellTargetingType)temp;

    if (ver <= _VERSION_06992_)
    {
      ar >> Target_Dice;
      ar >> Target_Dice_Nbr;
      ar >> Target_Bonus_Level;
      ar >> Target_Bonus;
      ar >> Target_Max;
    }
  
    ar >> temp;
    Duration_Rate = (spellDurationType)temp;

    if (ver <= _VERSION_06992_)
    {
      ar >> Duration_Dice;
      ar >> Duration_Dice_Nbr;
      ar >> Duration_Bonus_Level;
      ar >> Duration_Bonus;
      ar >> Duration_Max;

      ar >> Range_Dice;
      ar >> Range_Dice_Nbr;
      ar >> Range_Bonus_Level;
      ar >> Range_Bonus;
      ar >> Range_Max;

      ar >> Attack_Dice;
      ar >> Attack_Dice_Nbr;
      ar >> Attack_Bonus;
      ar >> Attack_Bonus_Level;
      ar >> Attack_Max;

      ar >> Damage_Dice;
      ar >> Damage_Dice_Nbr;
      ar >> Damage_Bonus;
      ar >> Damage_Bonus_Level;
      ar >> Damage_Max;

      ar >> Protection_Dice;
      ar >> Protection_Dice_Nbr;
      ar >> Protection_Bonus;
      ar >> Protection_Bonus_Level;
      ar >> Protection_Max;

      ar >> Heal_Dice;
      ar >> Heal_Dice_Nbr;
      ar >> Heal_Bonus;
      ar >> Heal_Bonus_Level;
      ar >> Heal_Max;
    }

    ar >> Cast_Cost;
    ar >> Cast_Priority;

    if (ver >= _VERSION_0670_)
    {
      Duration.Serialize(ar);
      //NumTargets.Serialize(ar);
      P1.Serialize(ar);
      //TargetRange.Serialize(ar);
      P2.Serialize(ar);
      if (ver >= 0.999432)
      {
        P3.Serialize(ar);
        P4.Serialize(ar);
        P5.Serialize(ar);
      };

      int count;
      ar >> count;
      SPELL_EFFECTS_DATA tmp;
      m_EffectedAttributes.RemoveAll();
      for (int i=0; i<count; i++)
      {
        tmp.Serialize(ar, ver);
        AddEffect(tmp);
      }
    }
    else
    {
      CString diceformat;
      diceformat.Format("%ud%u+%u+%u*level min 0 max %u",
                        Duration_Dice_Nbr,
                        Duration_Dice,
                        Duration_Bonus,
                        Duration_Bonus_Level,
                        Duration_Max);
      Duration.DecodeText(diceformat);

      diceformat.Format("%ud%u+%u+%u*level min 0 max %u",
                        Target_Dice_Nbr,
                        Target_Dice,
                        Target_Bonus,
                        Target_Bonus_Level,
                        Target_Max);
      //NumTargets.DecodeText(diceformat);
      P1.DecodeText(diceformat);

      diceformat.Format("%ud%u+%u+%u*level min 0 max %u",
                        Range_Dice_Nbr,
                        Range_Dice,
                        Range_Bonus,
                        Range_Bonus_Level,
                        Range_Max);
      //TargetRange.DecodeText(diceformat);
      P2.DecodeText(diceformat);

      if ((Attack_Dice_Nbr!=0))
      {
        diceformat.Format("%ud%u+%u+%u*level min 0 max %u",
                          Attack_Dice_Nbr,
                          Attack_Dice,
                          Attack_Bonus,
                          Attack_Bonus_Level,
                          Attack_Max);
        SPELL_EFFECTS_DATA seData;
        seData.AffectedAttr("$CHAR_THAC0");
        seData.affectedAttr_IFKEY = CHAR_THAC0;
        seData.changeData.DecodeText(diceformat);
        seData.flags = SPELL_EFFECTS_DATA::EFFECT_TARGET;
        AddEffect(seData);
      }

      if ((Damage_Dice_Nbr!=0))
      {
        diceformat.Format("%ud%u+%u+%u*level min 0 max %u",
                          Damage_Dice_Nbr,
                          Damage_Dice,
                          Damage_Bonus,
                          Damage_Bonus_Level,
                          Damage_Max);
        SPELL_EFFECTS_DATA seData;
        seData.AffectedAttr("$CHAR_DAMAGEBONUS");
        seData.affectedAttr_IFKEY = CHAR_DAMAGEBONUS;
        seData.changeData.DecodeText(diceformat);
        seData.flags = SPELL_EFFECTS_DATA::EFFECT_TARGET;
        AddEffect(seData);
      }

      if ((Protection_Dice_Nbr!=0))
      {
        diceformat.Format("%ud%u+%u+%u*level min 0 max %u",
                          Protection_Dice_Nbr,
                          Protection_Dice,
                          Protection_Bonus,
                          Protection_Bonus_Level,
                          Protection_Max);
        SPELL_EFFECTS_DATA seData;
        seData.AffectedAttr("$CHAR_AC");
        seData.affectedAttr_IFKEY = CHAR_AC;
        seData.changeData.DecodeText(diceformat);
        seData.flags = SPELL_EFFECTS_DATA::EFFECT_TARGET;
        AddEffect(seData);
      }

      if ((Heal_Dice_Nbr!=0))
      {
        diceformat.Format("%ud%u+%u+%u*level min 0 max %u",
                          Heal_Dice_Nbr,
                          Heal_Dice,
                          Heal_Bonus,
                          Heal_Bonus_Level,
                          Heal_Max);
        SPELL_EFFECTS_DATA seData;
        seData.AffectedAttr("$CHAR_HITPOINTS");
        seData.affectedAttr_IFKEY = CHAR_HITPOINTS;
        seData.changeData.DecodeText(diceformat);
        seData.flags = SPELL_EFFECTS_DATA::EFFECT_TARGET;
        AddEffect(seData);
      }
    }
    if (ver >= _VERSION_0840_)
      CastArt.Serialize(ar,ver, rte.SpellArtDir());
    MissileArt.Serialize(ar,ver, rte.SpellArtDir());
    TargCoverageArt.Serialize(ar,ver, rte.SpellArtDir());
    TargHitArt.Serialize(ar,ver, rte.SpellArtDir());
    TargLingerArt.Serialize(ar,ver, rte.SpellArtDir());

    if (ver >= _VERSION_0840_)
    {
      DAS(ar, MissileSound);
      DAS(ar, CoverageSound);
      DAS(ar, HitSound);
      DAS(ar, LingerSound);
    }
    if (ver >= _VERSION_0841_)
    {
      DAS(ar, CastMsg);
    }

    if (ver >= _VERSION_0904_)
    {
      DAS(ar,SpellBeginSource);
      DAS(ar,SpellBeginBinary);
      SpellBeginBinary.Empty();
      DAS(ar,SpellEndSource);
      DAS(ar,SpellEndBinary);      
      SpellEndBinary.Empty();

      if (ver >= 2.6)
      {
        DAS(ar,SpellInitiationSource);
        DAS(ar,SpellInitiationBinary);
        SpellInitiationBinary.Empty();
        DAS(ar,SpellTerminationSource);
        DAS(ar,SpellTerminationBinary);      
        SpellTerminationBinary.Empty();
      };
      if (ver >= 1.0303)
      {
        DAS(ar,SavingThrowSource);
        DAS(ar,SavingThrowBinary);      
        SavingThrowBinary.Empty();
        DAS(ar,SavingThrowSucceededSource);
        DAS(ar,SavingThrowSucceededBinary);      
        SavingThrowSucceededBinary.Empty();
        DAS(ar,SavingThrowFailedSource);
        DAS(ar,SavingThrowFailedBinary);      
        SavingThrowFailedBinary.Empty();
      };
    }

    if (ver >= _VERSION_0906_)
    {
      EffectDuration.Serialize(ar);
    }
  }

  specAbs.Serialize(ar,ver, this->Name, "spell");
#ifdef UAFEngine
  if (specAbs.FindAbility("IsASpell") == NULL)
  {
    this->specAbs.InsertAbility_RO("IsASpell","","","");
  };
#endif
  spell_asl.Serialize(ar, "SPELL_DATA_ATTRIBUTES");
}
//*****************************************************************************
//    NAME: SPELL_DATA::PreSerialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::PreSerialize(BOOL IsStoring) 
{
  if (IsStoring)
  {
    StripFilenamePath(CastSound);
    StripFilenamePath(MissileSound);
    StripFilenamePath(CoverageSound);
    StripFilenamePath(HitSound);
    StripFilenamePath(LingerSound);

    CastArt.PreSerialize(IsStoring);
    MissileArt.PreSerialize(IsStoring);
    TargCoverageArt.PreSerialize(IsStoring);
    TargHitArt.PreSerialize(IsStoring);
    TargLingerArt.PreSerialize(IsStoring);

    // move fields that need to be serialized
    // as attributes into the attribute map  
    //StorePicDataAsASL(spell_asl, CastArt, "CastArt");
    //StorePicDataAsASL(spell_asl, MissileArt, "MissArt");
    //StorePicDataAsASL(spell_asl, TargCoverageArt, "TargCovArt");
    //StorePicDataAsASL(spell_asl, TargHitArt, "TargHitArt");
    //StorePicDataAsASL(spell_asl, TargLingerArt, "TargLingerArt");
  }
}

//*****************************************************************************
//    NAME: SPELL_DATA::PostSerialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::PostSerialize(BOOL IsStoring, double version) 
{
  if (IsStoring)
  {
      spell_asl.Delete("CastArt");
      spell_asl.Delete("MissArt");
      spell_asl.Delete("TargCovArt");
      spell_asl.Delete("TargHitArt");
      spell_asl.Delete("TargLingerArt");
  }
  else
  {
    // retrieve fields that are serialized as
    // attributes from the attribute map

#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
      RetrievePicDataFromASL(spell_asl, CastArt, "CastArt");
      RetrievePicDataFromASL(spell_asl, MissileArt, "MissArt");
      RetrievePicDataFromASL(spell_asl, TargCoverageArt, "TargCovArt");
      RetrievePicDataFromASL(spell_asl, TargHitArt, "TargHitArt");
      RetrievePicDataFromASL(spell_asl, TargLingerArt, "TargLingerArt");
      spell_asl.Delete("CastArt");
      spell_asl.Delete("MissArt");
      spell_asl.Delete("TargCovArt");
      spell_asl.Delete("TargHitArt");
      spell_asl.Delete("TargLingerArt");
    };
#endif
    if (version < 1.0303)
    {
      SPELL_EFFECTS_DATA *sed;
      POSITION pos;
      pos = m_EffectedAttributes.GetHeadPosition();
      while (pos != NULL)
      {


        // Move these three scripts from the effects to the spell proper.
        sed = m_EffectedAttributes.GetNext(pos);
        //if (!sed->SavingThrowScript().IsEmpty())
        //{
        //  if (SavingThrowSource.IsEmpty())
        //  {
        //    SavingThrowSource = sed->SavingThrowScript();
        //  };
        //};
        //sed->SavingThrowScript("");
        //sed->SavingThrowBinary("");
        if (!sed->m_string6.IsEmpty())
        {
          if (SavingThrowSource.IsEmpty())
          {
            SavingThrowSource = sed->m_string6;
            SavingThrowBinary = sed->m_string7;
          };
        };
        SavingThrowBinary.Empty();
        sed->m_string6.Empty();
        sed->m_string7.Empty();



        if (!sed->m_string10.IsEmpty())
        {
          if (SavingThrowSucceededSource.IsEmpty())
          {
            SavingThrowSucceededSource = sed->m_string10;
            SavingThrowSucceededBinary = sed->m_string11;
          };
        };
        sed->m_string10.Empty();
        sed->m_string11.Empty();
        SavingThrowSucceededBinary.Empty();





        if (!sed->m_string8.IsEmpty())
        {
          if (SavingThrowFailedSource.IsEmpty())
          {
            SavingThrowFailedSource = sed->m_string8;
            SavingThrowFailedBinary = sed->m_string9;
          };
        };
        SavingThrowFailedBinary.Empty();
        sed->m_string8.Empty();
        sed->m_string8.Empty();
      };
    };
  }

  //AddFolderToPath(CastSound,     rte.SoundDir());
  //AddFolderToPath(MissileSound,  rte.SoundDir());
  //AddFolderToPath(CoverageSound, rte.SoundDir());
  //AddFolderToPath(HitSound,      rte.SoundDir());
  //AddFolderToPath(LingerSound,   rte.SoundDir());

  CastArt.PostSerialize(IsStoring, version);
  MissileArt.PostSerialize(IsStoring, version);
  TargCoverageArt.PostSerialize(IsStoring, version);
  TargHitArt.PostSerialize(IsStoring, version);
  TargLingerArt.PostSerialize(IsStoring, version);
}


//*****************************************************************************
//    NAME: SPELL_DATA::PreCARSerialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::PreCARSerialize(BOOL IsStoring) 
{
  if (IsStoring)
  {
    StripFilenamePath(CastSound);
    StripFilenamePath(MissileSound);
    StripFilenamePath(CoverageSound);
    StripFilenamePath(HitSound);
    StripFilenamePath(LingerSound);

    CastArt.PreSerialize(IsStoring);
    MissileArt.PreSerialize(IsStoring);
    TargCoverageArt.PreSerialize(IsStoring);
    TargHitArt.PreSerialize(IsStoring);
    TargLingerArt.PreSerialize(IsStoring);
  }
}

bool SPELL_DATA::CompileSavingThrowScript(void)
{
  if (SavingThrowSource.IsEmpty()) return false;
  if (SavingThrowBinary.IsEmpty())
  {
    SavingThrowBinary = CompileScript(SavingThrowSource, "Save");
    /*
    CString basename(Name); // start with spell name
    basename.Remove(' '); // but remove spaces from name
    basename += "_";
    const char *allowedEntry[2];

    CString func(basename);
    func.Replace('|','_');
    func.Replace('-','_');
    //func += GetEffectSavingThrowFuncName();
    func += "Save";
    CString mscript;
    CString scriptbegin;
    CString scriptend;
    
    scriptbegin.Format("$PUBLIC $FUNC %s() { ", func);
    scriptend.Format(" } %s;", func);
    mscript = scriptbegin + SavingThrowSource + scriptend;
    
    allowedEntry[0] = (LPCSTR)func;
    allowedEntry[1] = NULL;     
    SavingThrowBinary(gpdlComp.CompileScript(mscript, allowedEntry));
    */
    if (SavingThrowBinary[0]!=0)
    {
      WriteDebugString("Error compiling Saving Throw Script for spell %s\n", Name);
      WriteDebugString("Error message = %s\n", SavingThrowBinary);
      return false;
    }
    else
    {
      return true;
    };
  }
  else
  {
    return SavingThrowBinary[0] == 0;
  };
}

bool SPELL_DATA::CompileBeginScript(void)
{
  if (SpellBeginSource.IsEmpty()) return false;
  if (SpellBeginBinary.IsEmpty())
  {
    SpellBeginBinary = CompileScript(SpellBeginSource, "Begin");
    if (SpellBeginBinary[0]!=0)
    {
      WriteDebugString("Error compiling Spell Begin Script for spell %s\n", Name);
      WriteDebugString("Error message = %s\n", SpellBeginBinary);
      return false;
    }
    else
    {
      return true;
    };
  }
  else
  {
    return SpellBeginBinary[0] == 0;
  };
}

bool SPELL_DATA::CompileEndScript(void) const
{
  if (SpellEndSource.IsEmpty()) return false;
  if (SpellEndBinary.IsEmpty())
  {
    const_cast<SPELL_DATA *>(this)->SpellEndBinary = CompileScript(SpellEndSource, "End");
    if (SpellEndBinary[0]!=0)
    {
      WriteDebugString("Error compiling Spell End Script for spell %s\n", Name);
      WriteDebugString("Error message = %s\n", SpellEndBinary);
      return false;
    }
    else
    {
      return true;
    };
  }
  else
  {
    return SpellEndBinary[0] == 0;
  };
}

bool SPELL_DATA::CompileInitiationScript(void) const
{
  if (SpellInitiationSource.IsEmpty()) return false;
  if (SpellInitiationBinary.IsEmpty())
  {
    const_cast<SPELL_DATA *>(this)->SpellInitiationBinary = CompileScript(SpellInitiationSource, "Initiation");
    if (SpellInitiationBinary[0]!=0)
    {
      WriteDebugString("Error compiling Spell Initiation Script for spell %s\n", Name);
      WriteDebugString("Error message = %s\n", SpellInitiationBinary);
      return false;
    }
    else
    {
      return true;
    };
  }
  else
  {
    return SpellInitiationBinary[0] == 0;
  };
}

bool SPELL_DATA::CompileTerminationScript(void) const
{
  if (SpellTerminationSource.IsEmpty()) return false;
  if (SpellTerminationBinary.IsEmpty())
  {
    const_cast<SPELL_DATA *>(this)->SpellTerminationBinary = CompileScript(SpellTerminationSource, "Termination");
    if (SpellTerminationBinary[0]!=0)
    {
      WriteDebugString("Error compiling Spell Termination Script for spell %s\n", Name);
      WriteDebugString("Error message = %s\n", SpellTerminationBinary);
      return false;
    }
    else
    {
      return true;
    };
  }
  else
  {
    return SpellTerminationBinary[0] == 0;
  };
}





bool SPELL_DATA::CompileSavingThrowSucceededScript(void)
{
  if (SavingThrowSucceededSource.IsEmpty()) return false;
  if (SavingThrowSucceededBinary.IsEmpty())
  {
    SavingThrowSucceededBinary = CompileScript(SavingThrowSucceededSource, "Save");
    if (SavingThrowSucceededBinary[0]!=0)
    {
      WriteDebugString("Error compiling Saving Throw Succeeded Script for spell %s\n", Name);
      WriteDebugString("Error message = %s\n", SavingThrowSucceededBinary);
      return false;
    }
    else
    {
      return true;
    };
  }
  else
  {
    return SavingThrowSucceededBinary[0] == 0;
  };
}


bool SPELL_DATA::CompileSavingThrowFailedScript(void)
{
  if (SavingThrowFailedSource.IsEmpty()) return false;
  if (SavingThrowFailedBinary.IsEmpty())
  {
    SavingThrowFailedBinary = CompileScript(SavingThrowFailedSource, "Save");
    if (SavingThrowFailedBinary[0]!=0)
    {
      WriteDebugString("Error compiling Saving Throw Failed Script for spell %s\n", Name);
      WriteDebugString("Error message = %s\n", SavingThrowFailedBinary);
      return false;
    }
    else
    {
      return true;
    };
  }
  else
  {
    return SavingThrowFailedBinary[0] == 0;
  };
}






//*****************************************************************************
//    NAME: SPELL_DATA::PostCARSerialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::PostCARSerialize(BOOL IsStoring, double version) 
{
  //AddFolderToPath(CastSound,     rte.SoundDir());
  //AddFolderToPath(MissileSound,  rte.SoundDir());
  //AddFolderToPath(CoverageSound, rte.SoundDir());
  //AddFolderToPath(HitSound,      rte.SoundDir());
  //AddFolderToPath(LingerSound,   rte.SoundDir());

  CastArt.PostSerialize(IsStoring, version);
  MissileArt.PostSerialize(IsStoring, version);
  TargCoverageArt.PostSerialize(IsStoring, version);
  TargHitArt.PostSerialize(IsStoring, version);
  TargLingerArt.PostSerialize(IsStoring, version);
  if (version < 1.0303)
  {
    SPELL_EFFECTS_DATA *sed;
    POSITION pos;
    pos = m_EffectedAttributes.GetHeadPosition();
    while (pos != NULL)
    {
      sed = m_EffectedAttributes.GetNext(pos);

      // Move these three scripts from the effect to the spell
      //if (!sed->SavingThrowScript().IsEmpty())
      //{
      //  if (SavingThrowSource.IsEmpty())
      //  {
      //    SavingThrowSource = sed->SavingThrowScript();
      //  };
      //};
      //sed->SavingThrowScript("");
      //sed->SavingThrowBinary("");
      if (!sed->m_string6.IsEmpty())
      {
        if (SavingThrowSource.IsEmpty())
        {
          SavingThrowSource = sed->m_string6;
          SavingThrowBinary = sed->m_string7;
        };
      };
      sed->m_string6.Empty();
      sed->m_string7.Empty();




      if (!sed->m_string10.IsEmpty())
      {
        if (SavingThrowSucceededSource.IsEmpty())
        {
          SavingThrowSucceededSource = sed->m_string10;
          SavingThrowSucceededBinary = sed->m_string11;
        };
      };
      sed->m_string10;
      sed->m_string11;
      if (!sed->m_string8.IsEmpty())
      {
        if (SavingThrowFailedSource.IsEmpty())
        {
          SavingThrowFailedSource = sed->m_string8;
          SavingThrowFailedBinary = sed->m_string9;
        };
      };
      sed->m_string8.Empty();
      sed->m_string9.Empty();
    };
  };
}

//*****************************************************************************
//    NAME: SPELL_DATA::Save
//
// PURPOSE: 
//
//*****************************************************************************

void SPELL_DATA::Save(CAR& car) const
{
	spell_asl.Save(car, "SPELL_DATA_ATTRIBUTES");
}

//*****************************************************************************
//    NAME: SPELL_DATA::PrepareForRestore
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::PrepareForRestore(void) 
{
	// Just in case no Restore comes along.
	temp_asl.Clear();
}

//*****************************************************************************
//    NAME: SPELL_DATA::Restore
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::Restore(CArchive& ar)
{
  temp_asl.Serialize(ar, "SPELL_DATA_ATTRIBUTES");
}

void SPELL_DATA::Restore(CAR& car)
{
  temp_asl.Serialize(car, "SPELL_DATA_ATTRIBUTES");
}

//*****************************************************************************
//    NAME: SPELL_DATA::CommitRestore
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::CommitRestore(void) 
{
	spell_asl.CommitRestore(&temp_asl);
	temp_asl.Clear();
}

#ifdef UAFEDITOR
void SPELL_DATA::CrossReference(CR_LIST *pCRList) const
{
  CR_REFERENCE CRReference;
  CRReference.m_referenceName = Name;
  CRReference.m_referenceType = CR_TYPE_spell;
  //pCRList->CR_AddResource(CR_TYPE_spell, this->Name, m_gsID.UniqueIdentifier());
  pCRList->CR_AddResource(CR_TYPE_spell, this->Name, 0);
  pCRList->CR_AddSoundReference(CastSound, &CRReference);
  pCRList->CR_AddSoundReference(MissileSound, &CRReference);
  pCRList->CR_AddSoundReference(CoverageSound, &CRReference);
  pCRList->CR_AddSoundReference(HitSound, &CRReference);
  pCRList->CR_AddSoundReference(LingerSound, &CRReference);
  CastArt.CrossReference(pCRList, &CRReference);
  MissileArt.CrossReference(pCRList, &CRReference);
  TargCoverageArt.CrossReference(pCRList, &CRReference);
  TargHitArt.CrossReference(pCRList, &CRReference);
  TargLingerArt.CrossReference(pCRList, &CRReference);
}
#endif

DICEPLUS infinity("999999");


DICEPLUS& SPELL_DATA::TargetHeight(void)
{
  switch (Targeting)
  {
  case AreaSquare:
    return P2;
  case AreaCone:
    return P2;
  case AreaLinePickStart:
    return P2;
  case Self:
    return infinity;
  case SelectedByCount:
    return infinity;
  case WholeParty:
    return infinity;
  case TouchedTargets:
    return infinity;
  case AreaCircle:
    return P2;
  case SelectByHitDice:
    return infinity;
  case AreaLinePickEnd:
    return P2;
  default:
    /* Really */ NotImplemented(0x9c959, false);
    return P1;
  };
}

DICEPLUS& SPELL_DATA::TargetWidth(void)
{
  switch (Targeting)
  {
  case AreaSquare:
    return P1;
  case AreaCone:
    return P1;
  case AreaLinePickStart:
    return P1;
  case Self:
    return infinity;
  case SelectedByCount:
    return infinity;
  case WholeParty:
    return infinity;
  case TouchedTargets:
    return infinity;
  case AreaCircle:
    return P2;
  case SelectByHitDice:
    return infinity;
  case AreaLinePickEnd:
    return P1;
  default:
    /* Really */ NotImplemented(0x9c95c, false);
    return P1;
  };
}

DICEPLUS RollOne("0d0+1");

DICEPLUS& SPELL_DATA::TargetRange(void)
{
  switch (Targeting)
  {
  case AreaSquare:
    return P3;
  case AreaCone:
    return P3;
  case AreaLinePickStart:
    return P3;
  case SelectedByCount:
    return P3;
  case Self:
    return infinity;
  case WholeParty:
    return infinity;
  case TouchedTargets:
    return RollOne;
  case AreaCircle:
    return P3;
  case SelectByHitDice:
    return P3;
  case AreaLinePickEnd:
    return P3;
  default:
    /* Really */ NotImplemented(0x9c95a, false);
    return P1;
  };
}


DICEPLUS& SPELL_DATA::TargetQuantity(void)
{
  switch (Targeting)
  {
  case AreaSquare:
  case AreaCone:
  case AreaLinePickStart:
    return infinity;
  case SelectedByCount:
    return P1;
  case Self:
    return infinity;
  case WholeParty:
    return infinity;
  case TouchedTargets:
    return P1;
  case AreaCircle:
    return P1;
  case SelectByHitDice:
    return P1;
  case AreaLinePickEnd:
    return infinity;
  default:
    /* Really */ NotImplemented(0x9c95b, false);
    return P1;
  };
}


//*****************************************************************************
//    NAME: SPELL_DATA::PlayCast
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::PlayCast()
{
  if (!PlaySoundEffects) return;
  if (!ValidSound(hCastSound))
    hCastSound = LoadSound(CastSound);
  if (!PlaySound(hCastSound,FALSE))
    hCastSound=-1;
}
//*****************************************************************************
//    NAME: SPELL_DATA::PlayMissile
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::PlayMissile()
{
  if (!PlaySoundEffects) return;
  if (!ValidSound(hMissileSound))
    hMissileSound = LoadSound(MissileSound);
  if (!PlaySound(hMissileSound,FALSE))
    hMissileSound=-1;
}
//*****************************************************************************
//    NAME: SPELL_DATA::PlayCoverage
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::PlayCoverage()
{
  if (!PlaySoundEffects) return;
  if (!ValidSound(hCoverageSound))
    hCoverageSound = LoadSound(CoverageSound);
  if (!PlaySound(hCoverageSound,FALSE))
    hCoverageSound=-1;
}
//*****************************************************************************
//    NAME: SPELL_DATA::PlayHit
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::PlayHit()
{
  if (!PlaySoundEffects) return;
  if (!ValidSound(hHitSound))
    hHitSound = LoadSound(HitSound);
  if (!PlaySound(hHitSound,FALSE))
    hHitSound=-1;
}
//*****************************************************************************
//    NAME: SPELL_DATA::PlayLinger
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::PlayLinger()
{
  if (!PlaySoundEffects) return;
  if (!ValidSound(hLingerSound))
    hLingerSound = LoadSound(LingerSound);
  if (!PlaySound(hLingerSound,FALSE))
    hLingerSound=-1;
}
//*****************************************************************************
//    NAME: SPELL_DATA::ClearSounds
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::ClearSounds()
{
  if (hCastSound >= 0)     ClearSound(&hCastSound);
  if (hMissileSound >= 0)  ClearSound(&hMissileSound);
  if (hCoverageSound >= 0) ClearSound(&hCoverageSound);
  if (hHitSound >= 0)      ClearSound(&hHitSound);
  if (hLingerSound >= 0)   ClearSound(&hLingerSound);
  hCastSound = -1;
  hMissileSound=-1;
  hCoverageSound=-1;
  hHitSound=-1;
  hLingerSound=-1;
}

//*****************************************************************************
//    NAME: SPELL_DATA::LoadArt
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::LoadArt()
{
  //CastArt.LoadPicSurfaces("spl_");
  CastArt.LoadPicSurfaces("");
  //MissileArt.LoadPicSurfaces("spl_");
  MissileArt.LoadPicSurfaces("");
  //TargCoverageArt.LoadPicSurfaces("spl_");
  TargCoverageArt.LoadPicSurfaces("");
  //TargHitArt.LoadPicSurfaces("spl_");
  TargHitArt.LoadPicSurfaces("");
  //TargLingerArt.LoadPicSurfaces("spl_");
  TargLingerArt.LoadPicSurfaces("");
}
//*****************************************************************************
//    NAME: SPELL_DATA::ClearArt
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::ClearArt()
{
  CastArt.Clear();
  MissileArt.Clear();
  TargCoverageArt.Clear();
  TargHitArt.Clear();
  TargLingerArt.Clear();
}
//*****************************************************************************
//    NAME: SPELL_DATA::FreeArt
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::FreeArt()
{
  CastArt.FreePic();
  MissileArt.FreePic();
  TargCoverageArt.FreePic();
  TargHitArt.FreePic();
  TargLingerArt.FreePic();
}

//*****************************************************************************
//    NAME: SPELL_DATA::AddEffect
//
// PURPOSE: 
//
//*****************************************************************************
POSITION SPELL_DATA::AddEffect(SPELL_EFFECTS_DATA &src)
{
  if (HaveEffectAttr(src)) return NULL; 
#ifdef UAFEDITOR
  //CompileScript(src);
#endif
  POSITION pos = m_EffectedAttributes.AddTail(src);  
  return pos;
}

//*****************************************************************************
//    NAME: SPELL_DATA::DeleteEffect
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::DeleteEffect(SPELL_EFFECTS_DATA &src)
{
  POSITION pos = m_EffectedAttributes.GetHeadPosition();
  while (pos != NULL)
  {
    if (*m_EffectedAttributes.PeekAt(pos) == src) 
    {
      m_EffectedAttributes.RemoveAt(pos);
      return;
    }
    m_EffectedAttributes.GetNext(pos);
  }
}

//*****************************************************************************
//    NAME: SPELL_DATA::HaveEffectAttr
//
// PURPOSE: 
//
//*****************************************************************************
bool SPELL_DATA::HaveEffectAttr(const SPELL_EFFECTS_DATA &src) const
{
  POSITION pos = m_EffectedAttributes.GetHeadPosition();
  while (pos != NULL)
  {
    if (*m_EffectedAttributes.PeekAt(pos) == src) 
      return true;
    m_EffectedAttributes.PeekNext(pos);
  }
  return false;
}

//*****************************************************************************
//    NAME: SPELL_DATA::CanEnableAffectAttr
//
// PURPOSE: 
//
//*****************************************************************************
bool SPELL_DATA::CanEnableAffectAttr(const CString &attr) const
{
  POSITION pos = m_EffectedAttributes.GetHeadPosition();
  while (pos != NULL)
  {
    if (m_EffectedAttributes.PeekAt(pos)->AffectedAttr().CompareNoCase(attr)==0) 
    {
      if (m_EffectedAttributes.PeekAt(pos)->ApplyChange(0.0) > 0.0) 
        return true;
    }
    m_EffectedAttributes.PeekNext(pos);
  }
  return false;
}

//*****************************************************************************
//    NAME: SPELL_DATA::CompileScript
//
// PURPOSE: 
//
//*****************************************************************************
bool SPELL_DATA::CompileScript(SPELL_EFFECTS_DATA &src)
{
  CString name(""); 

  POSITION pos = m_EffectedAttributes.GetHeadPosition();
  int count=0;
  while (pos != NULL)
  {
    if (*m_EffectedAttributes.GetAt(pos) == src) 
    {
      // all gpdl function names must be unique, so 
      // we create a unique name by using the spell name,
      // spell effect index, and later on, the spell
      // effect's affected attribute name and current
      // clock tick count are appended.
    }
    count++;
    m_EffectedAttributes.GetNext(pos);
  }
  {
    int i, j;
    int nameLen = Name.GetLength();
    char *pName = (char *)malloc(nameLen+2);
    for (i=0, j=0; i<nameLen; i++)
    {
      char c = Name.GetAt(i);
      if ( (c != '$')  && (c != '_') )
      {
        if (c < '0') continue;
        if (c > 'z') continue;
        if ( (c > '9') && (c < 'A')) continue;
        if ( (c > 'Z') && (c < 'a')) continue;
      };
      pName[j++] = c;
    };
    pName[j++] = '_';
    pName[j++] = 0;
    name = pName;
    free(pName);
  };
  return (src.CompileScripts(name));  
}


CString SPELL_DATA::CompileScript(const CString& script, const char *namePostfix) const
{
  
  const_cast<SPELL_DATA *>(this)->SpellBeginBinary="";  
  const_cast<SPELL_DATA *>(this)->SpellEndBinary="";
  
  const char *allowedEntry[2];
  
  
  if (script.IsEmpty()) return "";
  {
    GPDLCOMP gpdlComp;
    CString sname(Name); // start with spell name
    sname.Remove(' '); // but remove spaces from name
    sname.Replace('|', '_');
    sname.Replace('-', '_');
    CString func("");  
    func.Format("%s_b_%u", sname, GetTickCount());
    CString ascript;
    CString scriptbegin;
    CString scriptend;
    
    scriptbegin.Format("$PUBLIC $FUNC %s() { ", func);
    scriptend.Format(" } %s;", func);
    ascript = scriptbegin + script + scriptend;
    
    allowedEntry[0] = (LPCSTR)func;
    allowedEntry[1] = NULL;     
    return gpdlComp.CompileScript(ascript, allowedEntry);    
  }
};  

//*****************************************************************************
//    NAME: SPELL_DATA::CompileScripts
//
// PURPOSE: 
//
//*****************************************************************************
bool SPELL_DATA::CompileScripts()
{

  bool success = true;
#ifdef UAFEDITOR 
  CString sname(Name); // start with spell name
  sname.Remove(' '); // but remove spaces from name
  sname.Replace('|', '_');
  sname.Replace('-', '_');
  //sname += "_";

  CString func("");  
  
  SpellBeginBinary="";  
  SpellEndBinary="";
  
  //const char *allowedEntry[2];
  
  GPDLCOMP gpdlComp;
  
  //if (!SpellBeginSource.IsEmpty()) 
  {
    //func.Format("%s_b_%u", sname, GetTickCount());
    //CString ascript;
    //CString scriptbegin;
    //CString scriptend;
    
    //scriptbegin.Format("$PUBLIC $FUNC %s() { ", func);
    //scriptend.Format(" } %s;", func);
    //ascript = scriptbegin + SpellBeginSource + scriptend;
    
    //allowedEntry[0] = (LPCSTR)func;
    //allowedEntry[1] = NULL;     
    //SpellBeginBinary = gpdlComp.CompileScript(ascript, allowedEntry);
    SpellBeginBinary.Empty();
    //if (SpellBeginBinary[0]!=0) success = false;
    // Let the engine compile on as-needed basis
  }
  
  //if (!SpellEndSource.IsEmpty()) 
  {
    //func.Format("%s_e_%u", sname, GetTickCount());
    //CString mscript;
    //CString scriptbegin;
    //CString scriptend;
    
    //scriptbegin.Format("$PUBLIC $FUNC %s() { ", func);
    //scriptend.Format(" } %s;", func);
    //mscript = scriptbegin + SpellEndSource + scriptend;
    
    //allowedEntry[0] = (LPCSTR)func;
    //allowedEntry[1] = NULL;     
    //SpellEndBinary = gpdlComp.CompileScript(mscript, allowedEntry);
    SpellEndBinary.Empty();
    //if (SpellEndBinary[0]!=0) success = false;
    // Let the engine compile on as-needed basis
  }
  

  //if (!SavingThrowSource.IsEmpty()) 
  {
    /*
    func.Format("%s_b_%u", sname, GetTickCount());
    CString ascript;
    CString scriptbegin;
    CString scriptend;
    
    scriptbegin.Format("$PUBLIC $FUNC %s() { ", func);
    scriptend.Format(" } %s;", func);
    ascript = scriptbegin + SavingThrowSource + scriptend;
    
    allowedEntry[0] = (LPCSTR)func;
    allowedEntry[1] = NULL;     
    SavingThrowBinary = gpdlComp.CompileScript(ascript, allowedEntry);
    
    if (SavingThrowBinary[0]!=0) success = false;
    */
    SavingThrowBinary.Empty();
    //success = CompileSavingThrowScript();
    // Let the engine compile on as-needed basis
  }
  //if (!SavingThrowSucceededSource.IsEmpty()) 
  {
    /*
    func.Format("%s_b_%u", sname, GetTickCount());
    CString ascript;
    CString scriptbegin;
    CString scriptend;
    
    scriptbegin.Format("$PUBLIC $FUNC %s() { ", func);
    scriptend.Format(" } %s;", func);
    ascript = scriptbegin + SavingThrowSucceededSource + scriptend;
    
    allowedEntry[0] = (LPCSTR)func;
    allowedEntry[1] = NULL;     
    SavingThrowSucceededBinary = gpdlComp.CompileScript(ascript, allowedEntry);
    
    if (SavingThrowSucceededBinary[0]!=0) success = false;
    */
    SavingThrowSucceededBinary.Empty();
    //success = CompileSavingThrowSucceededScript();
    // Let the engine compile on as-needed basis
  }
  
  //if (!SavingThrowFailedSource.IsEmpty()) 
  {
    /*
    func.Format("%s_b_%u", sname, GetTickCount());
    CString ascript;
    CString scriptbegin;
    CString scriptend;
    
    scriptbegin.Format("$PUBLIC $FUNC %s() { ", func);
    scriptend.Format(" } %s;", func);
    ascript = scriptbegin + SavingThrowFailedSource + scriptend;
    
    allowedEntry[0] = (LPCSTR)func;
    allowedEntry[1] = NULL;     
    SavingThrowFailedBinary = gpdlComp.CompileScript(ascript, allowedEntry);
    
    if (SavingThrowFailedBinary[0]!=0) success = false;
    */
    SavingThrowFailedBinary.Empty();
    //success = CompileSavingThrowFailedScript();
    // Let the engine compile on as-needed basis
  }
    
   // scripts with each spell effect
   POSITION pos = m_EffectedAttributes.GetHeadPosition();
   while (pos != NULL)
     m_EffectedAttributes.GetNext(pos)->CompileScripts(sname);
  
  
  if (!success)
  {
    SpellBeginBinary="";
    SpellEndBinary="";
  }

#endif
  
  return success;
}

//*****************************************************************************
//    NAME: SPELL_DATA::ExecuteActivationScript
//
// PURPOSE: 
//
//*****************************************************************************
bool SPELL_DATA::ExecuteActivationScript(SPELL_EFFECTS_DATA &src)
{
  return (src.ExecuteActivationScript());
}

//*****************************************************************************
//    NAME: SPELL_DATA::ExecuteModificationScript
//
// PURPOSE: 
//
//*****************************************************************************
int SPELL_DATA::ExecuteModificationScript(SPELL_EFFECTS_DATA &src)
{
  return (src.ExecuteModificationScript());
}

//*****************************************************************************
//    NAME: SPELL_DATA::ExecuteSavingThrowScript
//
// PURPOSE: 
//
//*****************************************************************************
int SPELL_DATA::ExecuteSavingThrowScript(SAVING_THROW_DATA &src)
{
  // The saving throw script returns a value that is treated as a
  // bonus (plus or minus) to the saving throw rolled by a character
  //
  // This script is optional.
  // Assumes that $AttackerContext() and $TargetContext() are already set up.
  int result = 0;

  if (CompileSavingThrowScript())
  {
    AttributeScriptContexts asc(&src);
    CString sresult = spellGPDL.ExecuteScript(SavingThrowBinary,1);
    result = atoi(sresult);
  }  
  return result;
}

//*****************************************************************************
//    NAME: SPELL_DATA::ExecuteSavingThrowFailedScript
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::ExecuteSavingThrowFailedScript(SAVING_THROW_DATA &src, CString *displayText)
{
  // This script is optional.
    
  if (CompileSavingThrowFailedScript())
  {
    HOOK_PARAMETERS hookParameters;
    AttributeScriptContexts asc(&src);
    spellGPDL.ExecuteScript(SavingThrowFailedBinary,1);
    if (displayText != NULL) *displayText += hookParameters[5];
  }  
}

//*****************************************************************************
//    NAME: SPELL_DATA::ExecuteSavingThrowSucceededScript
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::ExecuteSavingThrowSucceededScript(SAVING_THROW_DATA &src, CString *displayText)
{
  // This script is optional.

  if (CompileSavingThrowSucceededScript())
  {
    HOOK_PARAMETERS hookParameters;
    AttributeScriptContexts asc(&src);
    spellGPDL.ExecuteScript(SavingThrowSucceededBinary,1);
    if (displayText != NULL) *displayText += hookParameters[5];
  };
}

//*****************************************************************************
//    NAME: SPELL_DATA::ExecuteSpellBeginScript
//
// PURPOSE: 
//
//*****************************************************************************
bool SPELL_DATA::ExecuteSpellBeginScript(CHARACTER *pAttacker,
                                         CHARACTER *pTarget,
                                         ToHitComputation *pToHitComputation)
{ 
  bool result = true;
  if (!SpellBeginSource.IsEmpty())
  {
    if (CompileBeginScript())
    {
      SCRIPT_CONTEXT scriptContext;
      scriptContext.SetAttackerContext(pAttacker);
      scriptContext.SetTargetContext(pTarget);
      scriptContext.SetSpellContext(this);
      scriptContext.SetToHitComputationContext(pToHitComputation);
      if (spellGPDL.ExecuteScript(SpellBeginBinary,1)!="1")
      {
        result = false;
#ifdef UAFEngine
        if (pToHitComputation != NULL)
        {
          pToHitComputation->BeginSpellScriptFailure(1);
        };
#endif
      }
    }
  }  
  return result;
}

//*****************************************************************************
//    NAME: SPELL_DATA::ExecuteSpellEndScript
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::ExecuteSpellEndScript(CHARACTER *pAttacker, CHARACTER *pTarget) const
{ 
  if (!SpellEndSource.IsEmpty())
  {
    if (CompileEndScript())
    {
      SCRIPT_CONTEXT scriptContext;
      scriptContext.SetAttackerContext(pAttacker);
      scriptContext.SetTargetContext(pTarget);
        spellGPDL.ExecuteScript(SpellEndBinary,1); // don't care about result
    };
  }  
}

#ifdef SpellInitiationScript
//*****************************************************************************
//    NAME: SPELL_DATA::ExecuteSpellInitiationScript
//
// PURPOSE: 
//
//*****************************************************************************
bool SPELL_DATA::ExecuteSpellInitiationScript(CHARACTER *pAttacker) const
{ 
  bool result = true;
  if (!SpellInitiationSource.IsEmpty())
  {
    if (CompileInitiationScript())
    {
      SCRIPT_CONTEXT scriptContext;
      scriptContext.SetAttackerContext(pAttacker);
      scriptContext.SetSpellContext(this);
      if (spellGPDL.ExecuteScript(SpellInitiationBinary,1)!="1")
      {
        result = false;
      };
    };
  }; 
  return result;
}

//*****************************************************************************
//    NAME: SPELL_DATA::ExecuteSpellTerminationScript
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA::ExecuteSpellTerminationScript(CHARACTER *pAttacker) const
{ 
  if (!SpellTerminationSource.IsEmpty())
  {
    if (CompileTerminationScript())
    {
      SCRIPT_CONTEXT scriptContext;
      scriptContext.SetAttackerContext(pAttacker);
      spellGPDL.ExecuteScript(SpellTerminationBinary,1); // don't care about result
    };
  };  
}
#endif




//*****************************************************************************
//    NAME: SPELL_DATA::CanPerformSpecAb
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_DATA::CanPerformSpecAb(specialAbilitiesType sa)
{
  return (specAbs.HaveSpecAb(sa)?TRUE:FALSE);
}

///////////////////////////////////////////////////////////////////////
//
// Spell Data Type Functions
//
///////////////////////////////////////////////////////////////////////

//*****************************************************************************
//    NAME: saveSounds
//
// PURPOSE: 
//
//*****************************************************************************
void saveSounds(SPELL_DATA_TYPE& data)
{
  data.SaveSounds();
}


//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::SaveSounds
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::SaveSounds()
{
  //POSITION p = SpellData.GetHeadPosition();
  //while (p != NULL)
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    //::SaveSound(SpellData.GetAtPos(p).CastSound,     GLOBAL_SOUND, rte.SoundDir());
    //::SaveSound(SpellData.GetAtPos(p).MissileSound,  GLOBAL_SOUND, rte.SoundDir());
    //::SaveSound(SpellData.GetAtPos(p).CoverageSound, GLOBAL_SOUND, rte.SoundDir());
    //::SaveSound(SpellData.GetAtPos(p).HitSound,      GLOBAL_SOUND, rte.SoundDir());
    //::SaveSound(SpellData.GetAtPos(p).LingerSound,   GLOBAL_SOUND, rte.SoundDir());
    ::SaveSound(PeekSpell(i)->CastSound,     GLOBAL_SOUND, rte.SoundDir());
    ::SaveSound(PeekSpell(i)->MissileSound,  GLOBAL_SOUND, rte.SoundDir());
    ::SaveSound(PeekSpell(i)->CoverageSound, GLOBAL_SOUND, rte.SoundDir());
    ::SaveSound(PeekSpell(i)->HitSound,      GLOBAL_SOUND, rte.SoundDir());
    ::SaveSound(PeekSpell(i)->LingerSound,   GLOBAL_SOUND, rte.SoundDir());
    //SpellData.GetNext(p);
  }
}

//*****************************************************************************
//    NAME: saveArt
//
// PURPOSE: 
//
//*****************************************************************************
void saveArt(SPELL_DATA_TYPE& data)
{
  data.SaveArt();
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::SaveArt
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::SaveArt()
{
  //POSITION p = SpellData.GetHeadPosition();
  //while (p != NULL)
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    //::SaveArt(SpellData.GetAtPos(p).CastArt.filename,         SpriteDib, GLOBAL_ART, TRUE, rte.SpellArtDir());
    //::SaveArt(SpellData.GetAtPos(p).MissileArt.filename,      SpriteDib, GLOBAL_ART, TRUE, rte.SpellArtDir());
    //::SaveArt(SpellData.GetAtPos(p).TargCoverageArt.filename, SpriteDib, GLOBAL_ART, TRUE, rte.SpellArtDir());
    //::SaveArt(SpellData.GetAtPos(p).TargHitArt.filename,      SpriteDib, GLOBAL_ART, TRUE, rte.SpellArtDir());
    //::SaveArt(SpellData.GetAtPos(p).TargLingerArt.filename,   SpriteDib, GLOBAL_ART, TRUE, rte.SpellArtDir());
    ::SaveArt(GetSpell(i)->CastArt.filename,         SpriteDib, GLOBAL_ART, TRUE, rte.SpellArtDir());
    ::SaveArt(GetSpell(i)->MissileArt.filename,      SpriteDib, GLOBAL_ART, TRUE, rte.SpellArtDir());
    ::SaveArt(GetSpell(i)->TargCoverageArt.filename, SpriteDib, GLOBAL_ART, TRUE, rte.SpellArtDir());
    ::SaveArt(GetSpell(i)->TargHitArt.filename,      SpriteDib, GLOBAL_ART, TRUE, rte.SpellArtDir());
    ::SaveArt(GetSpell(i)->TargLingerArt.filename,   SpriteDib, GLOBAL_ART, TRUE, rte.SpellArtDir());
    //SpellData.GetNext(p);
  }
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::PreSerialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::PreSerialize(BOOL IsStoring)
{
  // before saving, make sure data that is set to "DEFAULT"
  // is converted to proper filename
  if (IsStoring) ExpandDefaultFilenames();

  //POSITION p = SpellData.GetHeadPosition();
  //while (p!=NULL) 
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
    //SpellData.GetNext(p).PreSerialize(IsStoring);
    GetSpell(i)->PreSerialize(IsStoring);
    //SpellData[i].PreSerialize(IsStoring);
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::PostSerialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::PostSerialize(BOOL IsStoring, double Version)
{
  // after loading, make sure data that is set to "DEFAULT"
  // is converted to proper filename
  if (!IsStoring) ExpandDefaultFilenames();

  //POSITION p = SpellData.GetHeadPosition();
  //while (p!=NULL) 
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
    //SpellData.GetNext(p).PostSerialize(IsStoring, Version);
    GetSpell(i)->PostSerialize(IsStoring, Version);
    //SpellData[i].PostSerialize(IsStoring, Version);
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::PreCARSerialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::PreCARSerialize(BOOL IsStoring)
{
  // before saving, make sure data that is set to "DEFAULT"
  // is converted to proper filename
  if (IsStoring) ExpandDefaultFilenames();

  //POSITION p = SpellData.GetHeadPosition();
  //while (p!=NULL) 
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
    //SpellData.GetNext(p).PreCARSerialize(IsStoring);
    GetSpell(i)->PreCARSerialize(IsStoring);
    //SpellData[i].PreCARSerialize(IsStoring);
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::PostCARSerialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::PostCARSerialize(BOOL IsStoring, double Version)
{
  // after loading, make sure data that is set to "DEFAULT"
  // is converted to proper filename
  if (!IsStoring) ExpandDefaultFilenames();

  //POSITION p = SpellData.GetHeadPosition();
  //while (p!=NULL) 
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
    //SpellData.GetNext(p).PostCARSerialize(IsStoring, Version);
    GetSpell(i)->PostCARSerialize(IsStoring, Version);
    //SpellData[i].PostCARSerialize(IsStoring, Version);
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::ExpandDefaultFilenames
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::ExpandDefaultFilenames()
{  
  //POSITION p = GetHeadPosition();
  //GLOBAL_SPELL_ID key;
  SPELL_ID spellID;

  //while (p != NULL)
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    //SPELL_DATA data = GetAtPos(p);
    //SPELL_DATA data = *PeekSpellData(p);
    SPELL_DATA data = *PeekSpell(i);

    if (data.CastSound.CompareNoCase("DEFAULT")==0)
    {
#ifdef UAFEDITOR
      data.CastSound = ede.TemplateSoundDir() + DEF_CAST_SOUND;
#else
      data.CastSound = DEF_CAST_SOUND;
#endif
    }
    if (data.MissileSound.CompareNoCase("DEFAULT")==0)
    {
#ifdef UAFEDITOR
      data.MissileSound = ede.TemplateSoundDir() + DEF_CAST_SOUND;
#else
      data.MissileSound = DEF_CAST_SOUND;
#endif
    }
    if (data.CoverageSound.CompareNoCase("DEFAULT")==0)
    {
#ifdef UAFEDITOR
      data.CoverageSound = ede.TemplateSoundDir() + DEF_CAST_SOUND;
#else
      data.CoverageSound = DEF_CAST_SOUND;
#endif
    }
    if (data.HitSound.CompareNoCase("DEFAULT")==0)
    {
#ifdef UAFEDITOR
     data.HitSound = ede.TemplateSoundDir() + DEF_HIT_SOUND;
#else
      data.HitSound = DEF_HIT_SOUND;
#endif
    }
    if (data.LingerSound.CompareNoCase("DEFAULT")==0)
    {
#ifdef UAFEDITOR
      data.LingerSound = ede.TemplateSoundDir() + DEF_CAST_SOUND;
#else
      data.LingerSound = DEF_CAST_SOUND;
#endif
    }

    data.SetSpellAnimDefaults(data.CastArt, DEFAULT_STH);
    data.SetSpellAnimDefaults(data.MissileArt, DEFAULT_SIR);
    data.SetSpellAnimDefaults(data.TargCoverageArt, DEFAULT_STC);
    data.SetSpellAnimDefaults(data.TargHitArt, DEFAULT_STH);
    data.SetSpellAnimDefaults(data.TargLingerArt, DEFAULT_STL);

    //key = GetKeyAt(p);
    //SetSpell(key, data);
    //spellID = PeekSpellData(p)->SpellID();
    spellID = PeekSpell(i)->SpellID();
    SetSpell(spellID, data);
    //GetNext(p);
  }
}

#ifdef UAFEDITOR
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::CollapseDefaultFilenames
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::CollapseDefaultFilenames()
{  
  //GLOBAL_SPELL_ID key;
  SPELL_ID spellID;
  //POSITION p = GetHeadPosition();
  CString DefFilename;

  //while (p != NULL)
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    //SPELL_DATA& data = GetAtPos(p);
    //SPELL_DATA& data = *GetSpellData(p);
    SPELL_DATA *pSpell = GetSpell(i);

    DefFilename = ede.TemplateSoundDir() + DEF_CAST_SOUND;
    if (pSpell->CastSound.CompareNoCase(DefFilename)==0)
      pSpell->CastSound = "DEFAULT";
    DefFilename = ede.TemplateSoundDir() + DEF_CAST_SOUND;
    if (pSpell->MissileSound.CompareNoCase(DefFilename)==0)
      pSpell->MissileSound = "DEFAULT";
    DefFilename = ede.TemplateSoundDir() + DEF_CAST_SOUND;
    if (pSpell->CoverageSound.CompareNoCase(DefFilename)==0)
      pSpell->CoverageSound = "DEFAULT";
    DefFilename = ede.TemplateSoundDir() + DEF_HIT_SOUND;
    if (pSpell->HitSound.CompareNoCase(DefFilename)==0)
      pSpell->HitSound = "DEFAULT";
    DefFilename = ede.TemplateSoundDir() + DEF_CAST_SOUND;
    if (pSpell->LingerSound.CompareNoCase(DefFilename)==0)
      pSpell->LingerSound = "DEFAULT";
    
    DefFilename = ede.TemplateSpellArtDir() + DEFAULT_STH;
    if (pSpell->CastArt.filename.CompareNoCase(DefFilename)==0)
      pSpell->CastArt.filename = "DEFAULT";

    DefFilename = ede.TemplateMissileArtDir() + DEFAULT_SIR;
    if (pSpell->MissileArt.filename.CompareNoCase(DefFilename)==0)
      pSpell->MissileArt.filename = "DEFAULT";

    DefFilename = ede.TemplateCombatArtDir() + DEFAULT_STC;
    if (pSpell->TargCoverageArt.filename.CompareNoCase(DefFilename)==0)
      pSpell->TargCoverageArt.filename = "DEFAULT";
    
    DefFilename = ede.TemplateCombatArtDir() + DEFAULT_STH;
    if (pSpell->TargHitArt.filename.CompareNoCase(DefFilename)==0)
      pSpell->TargHitArt.filename = "DEFAULT";
    
    DefFilename = ede.TemplateCombatArtDir() + DEFAULT_STL;
    if (pSpell->TargLingerArt.filename.CompareNoCase(DefFilename)==0)
      pSpell->TargLingerArt.filename = "DEFAULT";

    //key = GetKeyAt(p);
    //spellID = PeekSpellData(p)->SpellID();
    //SetSpell(key, data);
    SetSpell(spellID, *pSpell);
    //GetNext(p);
  }
}

#endif // UAFEDITOR

//*****************************************************************************
//    NAME: saveData
//
// PURPOSE: 
//
//*****************************************************************************
BOOL saveData(SPELL_DATA_TYPE& data)
{
  EditorStatusMsg("Saving spell data...");

  CString fullPath;
  fullPath = rte.DataDir() + SPELL_DB_NAME;
  return saveData(data, fullPath);
}

//*****************************************************************************
//    NAME: saveData
//
// PURPOSE: 
//
//*****************************************************************************
BOOL saveData(SPELL_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  SetFileAttributes(fullPath, FILE_ATTRIBUTE_NORMAL);
  if (!myFile.Open(fullPath, CFile::modeCreate | CFile::modeWrite, &e))
  {
    WriteDebugString("Unable to open spell db file %s, error %i\n", fullPath, e.m_cause);
    return FALSE;
  }   

  // put signal value and version at start of file
  __int64 hdr=0xFABCDEFABCDEFABF;
  double ver = PRODUCT_VER;
  data.version = ver;
  myFile.Write(&hdr, sizeof(hdr));
  myFile.Write(&ver, sizeof(double));

  CAR ar(&myFile, CArchive::store);
  ar.Compress(true); // 

  data.PreCARSerialize(ar.IsStoring());
  data.Serialize(ar, ver);
  data.PostCARSerialize(ar.IsStoring(), globalData.version);
  ar.Close();
  return TRUE;
}

//*****************************************************************************
//    NAME: clearData
//
// PURPOSE: 
//
//*****************************************************************************
void clearData(SPELL_DATA_TYPE& data)
{
  data.Clear();
  data.version = PRODUCT_VER;
}

//*****************************************************************************
//    NAME: loadData
//
// PURPOSE: 
//
//*****************************************************************************
int loadData(SPELL_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;
  BOOL success=TRUE;

  if (!myFile.Open(fullPath, CFile::modeRead, &e))
  {
    WriteDebugString("Unable to open spell db file %s, error %i\n", fullPath, e.m_cause);
    return 0;
  }   

  clearData(data);

  // check for version
  __int64 hdr=0;
  double ver;
  myFile.Read(&hdr, sizeof(hdr));
  if (hdr == 0xFABCDEFABCDEFABF)
  {
    myFile.Read(&ver, sizeof(double));
  }
  else
  {
    // special hdr not present, so reset archive to beginning
    myFile.SeekToBegin();
    // set version to last build that didn't save version
    ver = min(globalData.version, _VERSION_0620_);
    //ver = _VERSION_0620_;
  }

  data.version = ver;  

  // if loading older level data, don't use compression archive
  if (ver < _VERSION_0630_)
  {
    WriteDebugString("Loading pre-0630 spell DB version - %4.7f\n", ver);
    CArchive ar(&myFile, CArchive::load);
    try
    {
      data.PreSerialize(ar.IsStoring());
      data.Serialize(ar, ver);
      data.PostSerialize(ar.IsStoring(), globalData.version);
    }
    catch (...)
    {
        WriteDebugString("Unable to load spell data file %s\n", ar.GetFile()->GetFileName());
        data.Clear();
        success=FALSE;
    }
    ar.Close();
  }
  else
  {
    WriteDebugString("Loading spell DB version: %4.7f\n", ver);
    CAR ar(&myFile, CArchive::load);
    if (ver >= _SPECIAL_ABILITIES_VERSION_)
    {
      ar.Compress(true); // 
    };
    try
    {
      data.PreCARSerialize(ar.IsStoring());
      data.Serialize(ar, ver);
      data.PostCARSerialize(ar.IsStoring(), globalData.version);
    }
    catch (...)
    {
        WriteDebugString("Unable to load spell data file %s\n", ar.GetFile()->GetFileName());
        data.Clear();
        success=FALSE;
    }
    ar.Close();
  }

  if (data.GetCount() <= 0)
     WriteDebugString("No spells in spell db file\n");

  return success; //data.GetCount();
}

int SPELL_DATA_TYPE::GetDataSize(void)
{
  //POSITION pos;
  int size;
  const SPELL_DATA *pEntry;
  //pos = GetHeadPosition();
  size = sizeof(*this);
  //while (pos!=NULL)
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    //pEntry = &GetNext(pos);
    pEntry = PeekSpell(i);
    size += pEntry->GetDataSize();
  }
  return size;
}


//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::Clear() 
{ 
  //CSingleLock sLock(&m_CS, TRUE); 
  SpellData.RemoveAll(); 
  version=0.0; 
}


/*
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::GetNextKey
//
// PURPOSE: 
//
//*****************************************************************************
DWORD SPELL_DATA_TYPE::GetNextKey()
{
  DWORD key=0;
  if (GetCount() > 0)
  {
    key = SpellData.GetTail().m_gsID.m_key;
    if (key >= UINT_MAX)
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is greater than
      // MAX_SPELLS.
      // Gaps occur when deletions are made to the spellData list
      POSITION pos = SpellData.GetHeadPosition();
      BOOL found = FALSE;        
      int prevKey = 0;
      int currKey;
      while ((pos != NULL) && (!found))
      {
        currKey = SpellData.GetNext(pos).m_gsID.m_key;
        if (currKey != prevKey+1)
        {
          key = prevKey+1;
          found = TRUE;
        }
        else
          prevKey = currKey; 
      }
      VERIFY(found); // should never happen
    }
    else
      key++;
  }
  else
    key = 1;
  return key;
}

*/

#ifdef UAFEDITOR
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::Validate
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_DATA_TYPE::Validate(const SPELL_DATA &data,FILE *pFile) const
{
  BOOL success=TRUE;
  if (data.CastSound != "")
  {
    if (!FileExists(data.CastSound))
    {
      WriteValidateLog(pFile, "Spell '%s' missing Cast Sound file '%s'\n",
                       data.Name, data.CastSound);
      success = FALSE;
    }
  }
  if (data.MissileSound != "")
  {
    if (!FileExists(data.MissileSound))
    {
      WriteValidateLog(pFile, "Spell '%s' missing InRoute Sound file '%s'\n",
                       data.Name, data.MissileSound);
      success = FALSE;
    }
  }
  if (data.CoverageSound != "")
  {
    if (!FileExists(data.CoverageSound))
    {
      WriteValidateLog(pFile, "Spell '%s' missing Coverage Sound file '%s'\n",
                       data.Name, data.CoverageSound);
      success = FALSE;
    }
  }
  if (data.HitSound != "")
  {
    if (!FileExists(data.HitSound))
    {
      WriteValidateLog(pFile, "Spell '%s' missing Hit Sound file '%s'\n",
                       data.Name, data.HitSound);
      success = FALSE;
    }
  }
  if (data.LingerSound != "")
  {
    if (!FileExists(data.LingerSound))
    {
      WriteValidateLog(pFile, "Spell '%s' missing Linger Sound file '%s'\n",
                       data.Name, data.LingerSound);
      success = FALSE;
    }
  }

  POSITION pos = data.specAbs.GetHeadPosition();
  while (pos!=NULL)
  {
#ifdef OLD_SPECIAL_ABILITIES
    const SPEC_AB_DATA *spab = data.specAbs.GetNextData(pos);
#else
    const CStringPAIR *spab = data.specAbs.GetNextData(pos);
#endif
    if (spab!=NULL)
    {
#ifdef OLD_SPECIAL_ABILITIES
      if (!spab->script.IsEmpty())
#else
      if (!specialAbilitiesData.FindString(spab->Key(),ActivationScriptName).IsEmpty())
#endif
      {
#ifdef OLD_SPECIAL_ABILITIES
        if (spab->binary.GetLength()==0)
#else
        if (specialAbilitiesData.FindString(spab->Key(),"ActivationScriptBin").GetLength()==0)
#endif
        {
          success = FALSE;
          WriteValidateLog(pFile, "Spell '%s' special ability script '%s' did not compile\n",
#ifdef OLD_SPECIAL_ABILITIES
                           data.Name, spab->script);
#else
                           data.Name, specialAbilitiesData.FindString(spab->Key(),ActivationScriptName));
#endif
        }
      }
    }
  }

  if (!data.SpellBeginSource.IsEmpty())
  {
    if (data.SpellBeginBinary.GetLength() == 0)
    {
      success = FALSE;
      WriteValidateLog(pFile, "Spell '%s', begin script did not compile\n",
        data.Name);      
    }
  }
  if (!data.SpellEndSource.IsEmpty())
  {
    if (data.SpellEndBinary.GetLength() == 0)
    {
      success = FALSE;
      WriteValidateLog(pFile, "Spell '%s', end script did not compile\n",
        data.Name);      
    }
  }

  if (!data.SpellInitiationSource.IsEmpty())
  {
    if (data.SpellInitiationBinary.GetLength() == 0)
    {
      success = FALSE;
      WriteValidateLog(pFile, "Spell '%s', initiation script did not compile\n",
        data.Name);      
    }
  }
  if (!data.SpellTerminationSource.IsEmpty())
  {
    if (data.SpellTerminationBinary.GetLength() == 0)
    {
      success = FALSE;
      WriteValidateLog(pFile, "Spell '%s', termination script did not compile\n",
        data.Name);      
    }
  }
    


  pos = data.m_EffectedAttributes.GetHeadPosition();
  while (pos!=NULL)
  {
    SPELL_EFFECTS_DATA spef = *data.m_EffectedAttributes.PeekNext(pos);
    if (!spef.ActivationScript().IsEmpty())
    {
      if (spef.ActivationBinary().GetLength()==0)
      {
        success = FALSE;
        WriteValidateLog(pFile, "Spell '%s', attribute %s, activate script did not compile\n",
                         data.Name, spef.AffectedAttr());
      }
    }
    if (!spef.ModificationScript().IsEmpty())
    {
      if (spef.ModificationBinary().GetLength()==0)
      {
        success = FALSE;
        WriteValidateLog(pFile, "Spell '%s', attribute %s, modification script did not compile\n",
          data.Name, spef.AffectedAttr());
      }
    }
/*
    if (!spef.SavingThrowScript().IsEmpty())
    {
      if (spef.SavingThrowBinary().GetLength()==0)
      {
        success = FALSE;
        WriteValidateLog(pFile, "Spell '%s', attribute %s, saving throw script did not compile\n",
          data.Name, spef.AffectedAttr());
      }
    }  
    if (!spef.SavingThrowFailedScript().IsEmpty())
    {
      if (spef.SavingThrowFailedBinary().GetLength()==0)
      {
        success = FALSE;
        WriteValidateLog(pFile, "Spell '%s', attribute %s, saving throw failed script did not compile\n",
          data.Name, spef.AffectedAttr());
      }
    }  
    if (!spef.SavingThrowSucceededScript().IsEmpty())
    {
      if (spef.SavingThrowSucceededBinary().GetLength()==0)
      {
        success = FALSE;
        WriteValidateLog(pFile, "Spell '%s', attribute %s, saving throw succeeded script did not compile\n",
          data.Name, spef.AffectedAttr());
      }
    }  
*/
  }
  return success;
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::Validate
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_DATA_TYPE::Validate(FILE *pFile) const
{
  BOOL success = TRUE;
  //POSITION pos = SpellData.GetHeadPosition();
  //while (pos != NULL)
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    //if (!Validate(*SpellData.PeekNext(pos), pFile))
    //if (!Validate(*PeekNext(pos), pFile))
    if (!Validate(*PeekSpell(i), pFile))
      success = FALSE;
  }
  return success;
}

void SPELL_DATA_TYPE::CrossReference(CR_LIST *pCRList) const
{
  //POSITION pos = SpellData.GetHeadPosition();
  //while (pos != NULL)
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    //SpellData.PeekNext(pos)->CrossReference(pCRList);
    //PeekNext(pos)->CrossReference(pCRList);
    PeekSpell(i)->CrossReference(pCRList);
  };
}
/*
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::Validate
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_DATA_TYPE::Validate(int index, FILE *pFile) const
{
  BOOL success = TRUE;
  POSITION pos;
  if ((pos = SpellData.FindKeyPos(index)) != NULL)
  {
    if (!Validate(SpellData.PeekAtPos(pos), pFile))
      success = FALSE;
  }
  else
  {
    WriteValidateLog(pFile, "Bogus spell key %i\n", index);
    success = FALSE;
  }
  return success;
}
*/
#endif
/*
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::PlayCast
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::PlayCast(DWORD index)
{
  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = SpellData.FindKeyPos(index)) != NULL)
    SpellData.GetAtPos(pos).PlayCast();
}
*/
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::ClearSounds
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::ClearSounds()
{
  //CSingleLock sLock(&m_CS, TRUE);
  //POSITION pos;
  //pos = SpellData.GetHeadPosition();
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  //while (pos != NULL)
     //SpellData.GetNext(pos).ClearSounds();
     GetSpell(i)->ClearSounds();
     //SpellData[i].ClearSounds();
}  
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::FreeArt
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::FreeArt()
{
  //CSingleLock sLock(&m_CS, TRUE);
  //POSITION pos;
  //pos = SpellData.GetHeadPosition();
  //while (pos != NULL)
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
     //SpellData.GetNext(pos).FreeArt();
     GetSpell(i)->FreeArt();
     //SpellData[i].FreeArt();
}  
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::GetSpellSchool
//
// PURPOSE: 
//
//*****************************************************************************
//WORD SPELL_DATA_TYPE::GetSpellSchool(GLOBAL_SPELL_ID index) const
//WORD SPELL_DATA_TYPE::GetSpellSchool(const SPELL_ID& spellID) const
SCHOOL_ID SPELL_DATA_TYPE::GetSpellSchool(const SPELL_ID& spellID) const
{
  //POSITION pos;
  //if ((pos = SpellData.FindKeyPos(index.m_key)) != NULL)
  //  return SpellData.PeekAtPos(pos).schoolMask;
  const SPELL_DATA *pSpellData;
  pSpellData = PeekSpell(spellID);
  if (pSpellData != NULL) return pSpellData->schoolID;
  else
    //return 0;
    return SCHOOL_ID();
}

//*****************************************************************************
//    NAME: GetSpellCastMask
//
// PURPOSE: 
//
//*****************************************************************************
//WORD SPELL_DATA_TYPE::GetSpellCastMask(GLOBAL_SPELL_ID index) const
const BASECLASS_LIST *SPELL_DATA_TYPE::GetSpellAllowedBaseclasses(const SPELL_ID& spellID) const
{
  //POSITION pos;
  //if ((pos = SpellData.FindKeyPos(index.m_key)) != NULL)
  //if ((pos = SpellData.FindIDPos(spellID)) != NULL)
  //  return SpellData.PeekAtPos(pos).CastMask;
  const SPELL_DATA *pSpellData;
  pSpellData = PeekSpell(spellID);
  //if (pSpellData != NULL) return pSpellData->CastMask;
  if (pSpellData != NULL) return &pSpellData->allowedBaseclasses;
  else
    return 0;
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::GetSpellLevel
//
// PURPOSE: 
//
//*****************************************************************************
//int SPELL_DATA_TYPE::GetSpellLevel(GLOBAL_SPELL_ID index) const
int SPELL_DATA_TYPE::GetSpellLevel(const SPELL_ID& spellID) const
{
  //POSITION pos;
  //if ((pos = SpellData.FindKeyPos(index.m_key)) != NULL)
  //if ((pos = SpellData.FindIDPos(spellID)) != NULL)
  //  return SpellData.PeekAtPos(pos).Level;
  const SPELL_DATA *pSpellData;
  pSpellData = PeekSpell(spellID);
  if (pSpellData != NULL) return pSpellData->Level;
  else
    return 0;
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::GetSpellCastCost
//
// PURPOSE: 
//
//*****************************************************************************
//int SPELL_DATA_TYPE::GetSpellCastCost(GLOBAL_SPELL_ID index) const
int SPELL_DATA_TYPE::GetSpellCastCost(const SPELL_ID& spellID) const
{
  //POSITION pos;
  //if ((pos = SpellData.FindKeyPos(index.m_key)) != NULL)
  //if ((pos = SpellData.FindIDPos(spellID)) != NULL)
  //  return SpellData.PeekAtPos(pos).Cast_Cost;
  const SPELL_DATA *pSpellData;
  pSpellData = PeekSpell(spellID);
  if (pSpellData != NULL) return pSpellData->Cast_Cost;
  else
    return 0;
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::GetSpellName
//
// PURPOSE: 
//
//*****************************************************************************
//CString SPELL_DATA_TYPE::GetSpellName(GLOBAL_SPELL_ID index) const
CString SPELL_DATA_TYPE::GetSpellName(const SPELL_ID& spellID) const
{
  //POSITION pos;
  //if ((pos = SpellData.FindKeyPos(index.m_key)) != NULL)
  //if ((pos = SpellData.FindIDPos(spellID)) != NULL)
  //  return SpellData.PeekAtPos(pos).Name;
  const SPELL_DATA *pSpellData;
  pSpellData = PeekSpell(spellID);
  if (pSpellData != NULL) return pSpellData->Name;
  else
    return CString("No Spell");
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::HaveSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL SPELL_DATA_TYPE::HaveSpell(GLOBAL_SPELL_ID index) const
BOOL SPELL_DATA_TYPE::HaveSpell(const SPELL_ID& spellID) const
{
  //return (SpellData.FindKeyPos(index.m_key) != NULL);
  //return (SpellData.FindIDPos(spellID) != NULL);
  return PeekSpell(spellID) != NULL;
}


// called when loading from scripts, or
// when adding from editor GUI
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::AddSpell
//
// PURPOSE: 
//
//*****************************************************************************
//GLOBAL_SPELL_ID SPELL_DATA_TYPE::AddSpell(SPELL_DATA &data)
int SPELL_DATA_TYPE::AddSpell(SPELL_DATA *pSpell)
{
  //CSingleLock sLock(&m_CS, TRUE);
  //if (strlen(data.Name) == 0) return NO_SPELL;
  if (strlen(pSpell->Name) == 0) return -1;
  // Do not allow multiple items with same full name.
  //if (m_FindName(data.Name)!=NO_SPELL) return NO_SPELL;
  SPELL_ID spellID;
  //spellID = data.uniqueName;
  spellID = pSpell->SpellID();
  if (LocateSpell(spellID) >= 0) return -1;
  if (GetCount() < MAX_SPELLS)
  {
    // begin/end scripts
    int spellIndex;
    pSpell->CompileScripts();

    {
      CString basename(pSpell->Name); // start with spell name
      basename.Remove(' '); // but remove spaces from name
      basename += "_";

      // scripts with each spell effect
      POSITION pos = pSpell->m_EffectedAttributes.GetHeadPosition();
      while (pos != NULL)
        pSpell->m_EffectedAttributes.GetNext(pos)->CompileScripts(basename);
    };

    //pSpell->m_gsID.m_key = GetNextKey();
    //SpellpSpell->Insert(data, pSpell->m_gsID.m_key);
    spellIndex = SpellData.Add(*pSpell);
    //return pSpell->m_gsID;
    return spellIndex;
  }
  else
    //return NO_SPELL;
    return -1;
}
/*
// called when loading binary monster data that
// was saved with key info
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::AddSpellWithCurrKey
//
// PURPOSE: 
//
//*****************************************************************************
GLOBAL_SPELL_ID SPELL_DATA_TYPE::AddSpellWithCurrKey(SPELL_DATA &data)
{
  CSingleLock sLock(&m_CS, TRUE);
  if (SpellData.GetCount() < MAX_SPELLS)
  {
    SpellData.Insert(data, data.m_gsID.m_key);
    return data.m_gsID;
  }
  else
    return 0;
}
*/
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::SetSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL SPELL_DATA_TYPE::SetSpell(GLOBAL_SPELL_ID gsID, SPELL_DATA &spell)
BOOL SPELL_DATA_TYPE::SetSpell(const SPELL_ID& spellID, SPELL_DATA &spell)
{
  //CSingleLock sLock(&m_CS, TRUE);
  //POSITION pos;
  //if ((pos = SpellData.FindKeyPos(gsID.m_key)) != NULL)
  //if ((pos = SpellData.FindIDPos(spellID)) != NULL)
  //{
  //  SpellData.GetAtPos(pos) = spell;
  //  return TRUE;
  //}
  SPELL_DATA *pSpellData;
  pSpellData = GetSpell(spellID);
  if (pSpellData != NULL)
  {
    *pSpellData = spell;
    return TRUE;
  }
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::RemoveSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL SPELL_DATA_TYPE::RemoveSpell(GLOBAL_SPELL_ID gsID)
BOOL SPELL_DATA_TYPE::RemoveSpell(const SPELL_ID& spellID)
{
  //CSingleLock sLock(&m_CS, TRUE);
  //POSITION pos;
  int i;
  //if ((pos = SpellData.FindKeyPos(gsID.m_key)) != NULL)
  if ((i = LocateSpell(spellID)) != NULL)
  {
    SpellData.RemoveAt(i);
    return TRUE;
  }
  else
    return FALSE;
}





//*****************************************************************************
//    NAME: *SPELL_DATA_TYPE::PeekSpellData
//
// PURPOSE: 
//
//*****************************************************************************


//POSITION SPELL_DATA_TYPE::LocateSpell(const SPELL_ID& spellID) const
int SPELL_DATA_TYPE::LocateSpell(const SPELL_ID& spellID) const
{
  //POSITION pos;
  //for (pos=SpellData.GetHeadPosition(); pos!=NULL; SpellData.PeekNext(pos))
  //for (pos=SpellData.GetHeadPosition(); pos!=NULL; PeekNext(pos))
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    const SPELL_DATA *pSpellData;
    //pSpellData = &SpellData.PeekAtPos(pos);
    pSpellData = PeekSpell(i);
    if (spellID == pSpellData->SpellID()) return i;
  };
  return -1;
}


/*
SPELL_DATA *SPELL_DATA_TYPE::GetSpellData(GLOBAL_SPELL_ID index)
{
  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = SpellData.FindKeyPos(index.m_key)) != NULL)
    return &SpellData.GetAtPos(pos);//return &SpellData.GetAt(index);
  return NULL;
}
*/
/*
//*****************************************************************************
//    NAME: *SPELL_DATA_TYPE::GetSpellData
//
// PURPOSE: 
//
//*****************************************************************************
//SPELL_DATA *SPELL_DATA_TYPE::GetSpellData(GLOBAL_SPELL_ID index)
SPELL_DATA *SPELL_DATA_TYPE::GetSpell(const SPELL_ID& spellID)
{
  //CSingleLock sLock(&m_CS, TRUE);
  //POSITION pos;
  //if ((pos = SpellData.FindKeyPos(index.m_key)) != NULL)
  //if ((pos = SpellData.FindIDPos(spellID)) != NULL)
  //  return &SpellData.GetAtPos(pos);//return &SpellData.GetAt(index);
  int i;
  i = LocateSpell(spellID);
  if (i >= 0) return GetSpell(i);
  else
    return NULL;
}
*/
SPELL_DATA fakeSpell;

#ifdef UAFEDITOR

/*
//*****************************************************************************
//    NAME: *SPELL_DATA_TYPE::GetSpellData
//
// PURPOSE: 
//
//*****************************************************************************
SPELL_DATA *SPELL_DATA_TYPE::GetSpellData(const CString& spellName)
{
  POSITION pos;
  SPELL_DATA sd;
  pos=SpellData.GetHeadPosition(); 
  while (pos!=NULL)
  {
//#ifdef UAFEngine
//    if (SpellData.PeekAtPos(pos).uniqueName==spellName) 
//#else
    if (SpellData.PeekAtPos(pos).Name==spellName) 
//#endif
    {
      return &SpellData.GetAtPos(pos);
    };
    SpellData.GetNext(pos);
  };
  {
    CString msg;
    msg.Format("Attempt to access non-existent spell named \"%s\"", spellName);
    MsgBoxError(msg, "Spell Error", 2);
  };
  return &fakeSpell;
}
*/

#endif
/*
// No locking... for internal use
//*****************************************************************************
//    NAME: 
//
// PURPOSE: 
//
//GLOBAL_SPELL_ID SPELL_DATA_TYPE::m_FindName (const CString& name) const
SPELL_ID SPELL_DATA_TYPE::m_FindName (const CString& name) const
// Return key associated with the named object, or zero
{
  POSITION pos;
  pos=SpellData.GetHeadPosition(); 
  while (pos!=NULL)
  {
    const SPELL_DATA *pSpell;
    //pSpell = &SpellData.PeekAtPos(pos);
    pSpell = PeekSpell(pos);
    if (pSpell->Name==name) 
          //return pSpell->m_gsID;
          return pSpell->SpellID();
    //SpellData.PeekNext(pos);
    PeekNext(pos);
  };
  //return NO_SPELL;
  return SPELL_ID();
}
*/
/*
#ifdef UAFEngine
//GLOBAL_SPELL_ID SPELL_DATA_TYPE::m_FindUniqueName (const CString& uniqueName) const
SPELL_ID SPELL_DATA_TYPE::m_FindUniqueName (const CString& uniqueName) const
// Return key associated with the named object, or zero
{
  POSITION pos;
  pos=SpellData.GetHeadPosition(); 
  while (pos!=NULL)
  {
    const SPELL_DATA *pSpell;
    //pSpell = &SpellData.PeekAtPos(pos);
    pSpell = PeekSpell(pos);
    if (pSpell->uniqueName==uniqueName) 
          //return pSpell->m_gsID;
          return pSpell->SpellID();
    //SpellData.PeekNext(pos);
    PeekNext(pos);
  };
  //return NO_SPELL;
  return SPELL_ID();
}

#endif
*/
/*
// With locking...for public use
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::FindName
//
// PURPOSE: 
//
//*****************************************************************************
//GLOBAL_SPELL_ID SPELL_DATA_TYPE::FindName(const CString& name) const
SPELL_ID SPELL_DATA_TYPE::FindName(const CString& name) const
// Return key associated with the named object, or zero
{
  return m_FindName(name);
}
*/
/*
#ifdef UAFEngine
// With locking...for public use
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::FindIDName
//
// PURPOSE: 
//
//*****************************************************************************
//GLOBAL_SPELL_ID SPELL_DATA_TYPE::FindUniqueName(const CString& uniqueName) const
SPELL_ID SPELL_DATA_TYPE::FindUniqueName(const CString& uniqueName) const
// Return key associated with the named object, or zero
{
  return m_FindUniqueName(uniqueName);
}
#endif
*/
//*****************************************************************************
// NAME: SPELL_DATA_TYPE::operator =
//
// PURPOSE: Make a copy of a SPELL_DATA list
//*****************************************************************************
SPELL_DATA_TYPE& SPELL_DATA_TYPE:: operator = (SPELL_DATA_TYPE& src)
{
  if (&src==this) return *this;
  SpellData.RemoveAll();
  //POSITION p=src.SpellData.GetHeadPosition();
  //while (p!=NULL)
  //{
  //  //AddSpellWithCurrKey(src.SpellData.GetNext(p));
  //  AddSpell(src.SpellData.GetNext(p));
  //};
  SpellData.Append(src.SpellData);
  //pos=src.pos;
  return *this;
}

//*****************************************************************************
// NAME: SPELL_DATA_TYPE::operator ==
//
// PURPOSE: Test that two SPELL_DATA lists are identical...even the ordering.
//          One might like to exclude order but then it is a bit tricky
//          because of the possiblility of two identical entries in a list.
//*****************************************************************************
bool SPELL_DATA_TYPE:: operator == (SPELL_DATA_TYPE& src)
{
  //POSITION p1=SpellData.GetHeadPosition();
  //POSITION p2=src.SpellData.GetHeadPosition();
  int i, j, n;
  SPELL_ID spellID;
  n = GetCount();
  if (n != src.GetCount()) return false;
  //while (p1!=NULL)
  for (i=0; i<n; i++)
  {
    spellID = PeekSpell(i)->SpellID();
    j = src.LocateSpell(spellID);
    if (j < 0) return false;
    //if (p2==NULL) return false;
    //if (!(SpellData.GetNext(p1)==src.SpellData.GetNext(p2))) return false;
    if (*PeekSpell(i) == *src.PeekSpell(j)) continue;
    return false;
  };
  //if (p2!=NULL) return false;
//  if (pos!=src.pos) return false;
  return true;
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::Serialize(CArchive &ar, double ver)
{
//  CObject::Serialize(ar);
  //POSITION p;
  if (ar.IsStoring())
  {
    die("We should not be using CArchive to store spell data");
  }
  else
  {
    //Clear();
    //this->version = ver;
    SPELL_DATA data;
    int count;
    ar >> count;
    for (int i=0;i<count;i++)
    {
      data.Clear();
      data.Serialize(ar, ver);
      //AddSpellWithCurrKey(data);
      //AddSpellWithCurrKey(data);
      AddSpell(&data);
    }
  }
}
//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::Serialize(CAR &car, double ver)
{
  //POSITION p;
  if (car.IsStoring())
  {
    int i, n;

    n = GetCount();
    car << n;
    //p = SpellData.GetHeadPosition();
    //while (p != NULL)
    for (i=0; i<n; i++)
    {
      //SpellData.GetNext(p).Serialize(ar, ver);
      GetSpell(i)->Serialize(car, ver);
      //i--;
    };
    //ASSERT(i == 0);
  }
  else
  {
    //Clear();
    //this->version = ver;

    SPELL_DATA data;
    int count;
    car >> count;
    for (int i=0;i<count;i++)
    {
      data.Clear();
      data.Serialize(car, ver);
      //AddSpellWithCurrKey(data);
      AddSpell(&data);
    }
  }
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::Save
//
// PURPOSE: 
//
//*****************************************************************************


void SPELL_DATA_TYPE::Save(CAR& car) 
{
	car << GetCount();
	//POSITION p=SpellData.GetHeadPosition();
	//while (p!=NULL)
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
	{
    //ar << SpellData.PeekAtPos(p).Name;
    car << PeekSpell(i)->Name;
		//SpellData.GetAtPos(p).m_gsID.Serialize(ar, version); //ar << SpellData.GetAt(p).key;
		//SpellData.PeekAtPos(p).Save(ar);
		PeekSpell(i)->Save(car);
		//SpellData.PeekNext(p);
		//PeekNext(p);
	};
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::Restore
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::Restore(CArchive& ar)
{		
	CString spellName;
	//DWORD spellKey;
  SPELL_ID spellID;
	int i, n, count;
	//POSITION p;
	if (version < _ASL_LEVEL_) return;

	// Let items know they will be asked to CommitRestore.  They
	// can make a copy of their data or otherwise note that 
	// if no Restore comes along they do something special.
	// Probably will want to clear their temporary copy of
	// ATTRIBUTES in case no restore operation clears it.
	//p=SpellData.GetHeadPosition();
  //while (p!=NULL) 
  n = GetCount();
  for (i=0; i<n; i++)
    //SpellData.GetNext(p).PrepareForRestore();
    GetSpell(i)->PrepareForRestore();

	ar >> count;

	for (i=0; i<count; i++)
	{
    int j;
		ar >> spellName;
		//ar >> spellKey;
    spellID = spellName;
		//p=SpellData.GetHeadPosition();
		//while (p!=NULL) 
    {
			//if (SpellData.PeekAtPos(p).Name==spellName)
			j = LocateSpell(spellID);
      if (j >= 0)
			{
				//SpellData.GetAtPos(p).Restore(ar);
				GetSpell(j)->Restore(ar);
			}
			//SpellData.GetNext(p);
		}

		//if (p==NULL)
		if (j < 0)
		{
			SPELL_DATA junk;
			CString msg;
			junk.Restore(ar); // discard
			msg.Format("Restoring Spell \"%s\".  No such spell in design\n",
				spellName);
			WriteDebugString(msg);
		}
	}
}

void SPELL_DATA_TYPE::Restore(CAR& car)
{		
	CString spellName;
	//DWORD spellKey;
  SPELL_ID spellID;
	int i, n, count;
	//POSITION p;

	// Let items know they will be asked to CommitRestore.  They
	// can make a copy of their data or otherwise note that 
	// if no Restore comes along they do something special.
	// Probably will want to clear their temporary copy of
	// ATTRIBUTES in case no restore operation clears it.
	//p=SpellData.GetHeadPosition();
  //while (p!=NULL) 
  n = GetCount();
  for (i=0; i<n; i++)
    //SpellData.GetNext(p).PrepareForRestore();
    GetSpell(i)->PrepareForRestore();

	car >> count;

	for (i=0; i<count; i++)
	{
    int j;
		car >> spellName;
		//ar >> spellKey;
    spellID = spellName;
		//p=SpellData.GetHeadPosition();
		//while (p!=NULL) 
    {
			//if (SpellData.PeekAtPos(p).Name==spellName)
			j = LocateSpell(spellID);
      if (j >= 0)
			{
				//SpellData.GetAtPos(p).Restore(ar);
				GetSpell(j)->Restore(car);
			}
			//SpellData.GetNext(p);
		}

		//if (p==NULL)
		if (j < 0)
		{
			SPELL_DATA junk;
			CString msg;
			junk.Restore(car); // discard
			msg.Format("Restoring Spell \"%s\".  No such spell in design\n",
				spellName);
			WriteDebugString(msg);
		}
	}
}

//*****************************************************************************
//    NAME: SPELL_DATA_TYPE::CommitRestore
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_DATA_TYPE::CommitRestore(void) 
{
	//POSITION p;
	//p=SpellData.GetHeadPosition();
	//while (p!=NULL) 
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
	{
		//SpellData.GetNext(p).CommitRestore();
		GetSpell(i)->CommitRestore();
	};
}


//*****************************************************************************
//    NAME: clearSpellData
//
// PURPOSE: 
//
//*****************************************************************************
void clearSpellData(void)
{
  spellData.Clear();
}


///////////////////////////////////////////////////////////////////////
//
// Spell targeting Data Functions
//
///////////////////////////////////////////////////////////////////////

#ifdef UAFEngine 

//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::SPELL_TARGETING_DATA
//
// PURPOSE: 
//
//*****************************************************************************
SPELL_TARGETING_DATA::SPELL_TARGETING_DATA() 
{ 
  Clear(); 
}

//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::SPELL_TARGETING_DATA
//
// PURPOSE: 
//
//*****************************************************************************
SPELL_TARGETING_DATA::SPELL_TARGETING_DATA(const SPELL_TARGETING_DATA &src) 
{ 
  *this=src; 
}

//*****************************************************************************
//    NAME: &SPELL_TARGETING_DATA::operator=
//
// PURPOSE: 
//
//*****************************************************************************
SPELL_TARGETING_DATA &SPELL_TARGETING_DATA::operator=(const SPELL_TARGETING_DATA &src)
{
  if (this==&src)return *this;
  m_targType=src.m_targType;
  m_SelectingUnits=src.m_SelectingUnits;
  m_canTargetFriend=src.m_canTargetFriend;
  m_canTargetEnemy=src.m_canTargetEnemy;
  m_IsPartyAligned=src.m_IsPartyAligned;
  m_MaxHitDice=src.m_MaxHitDice;
  m_MaxTargets=src.m_MaxTargets;
  m_MaxRange=src.m_MaxRange;
  m_maxRangeX = src.m_maxRangeX;
  m_maxRangeY = src.m_maxRangeY;
  m_MapTargetX=src.m_MapTargetX;
  m_MapTargetY=src.m_MapTargetY;
  m_HDTotal = src.m_HDTotal;
  m_targets.RemoveAll();
  m_targets.SetSize(src.m_targets.GetSize());
  for (int i=0;i<src.m_targets.GetSize(); i++)
    m_targets[i] = src.m_targets[i];

  m_maptarglist.RemoveAll();
  POSITION pos = src.m_maptarglist.GetHeadPosition();
  while (pos != NULL)
  {
    POINT pt = src.m_maptarglist.GetNext(pos);
    m_maptarglist.AddTail(pt);
  }
  return *this;
}
//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::operator==
//
// PURPOSE: 
//
//*****************************************************************************
bool SPELL_TARGETING_DATA::operator==(const SPELL_TARGETING_DATA& src) const
{
  if (m_targType != src.m_targType) return false;
  if (m_SelectingUnits != src.m_SelectingUnits) return false;
  if (m_canTargetFriend != src.m_canTargetFriend) return false;
  if (m_canTargetEnemy != src.m_canTargetEnemy) return false;
  if (m_IsPartyAligned != src.m_IsPartyAligned) return false;
  if (m_MaxHitDice != src.m_MaxHitDice) return false;
  if (m_MaxTargets != src.m_MaxTargets) return false;
  if (m_MaxRange != src.m_MaxRange) return false;
  if (m_maxRangeX != src.m_maxRangeX) return false;
  if (m_maxRangeY != src.m_maxRangeY) return false;
  if (m_MapTargetX != src.m_MapTargetX) return false;
  if (m_MapTargetY != src.m_MapTargetY) return false;
  if (m_HDTotal != src.m_HDTotal) return false;
  if (m_targets.GetSize() != src.m_targets.GetSize()) return false;
  for (int i=0;i<m_targets.GetSize();i++)
    if (m_targets[i] != src.m_targets[i]) return false;
  if (m_maptarglist.GetCount() != src.m_maptarglist.GetCount()) return false;
  POSITION p1=m_maptarglist.GetHeadPosition();
  POSITION p2=src.m_maptarglist.GetHeadPosition();
  while (p1!=NULL)
  {
    if (p2==NULL) return false;
    POINT pt1 = m_maptarglist.GetNext(p1);
    POINT pt2 = src.m_maptarglist.GetNext(p2);
    if (pt1.x != pt2.x) return false;
    if (pt1.y != pt2.y) return false;
  }
  if (p2!=NULL) return false;
  
  return true;
}
//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_TARGETING_DATA::Clear()
{
  m_targType=NowSelf;
  m_SelectingUnits=TRUE;
  m_canTargetFriend=FALSE;
  m_canTargetEnemy = TRUE;
  m_IsPartyAligned=TRUE;
  m_MaxHitDice=0;
  m_MaxTargets=0;
  m_MaxRange=0;
  m_maxRangeX = 99999;
  m_maxRangeY = 99999;
  m_MapTargetX=-1;
  m_MapTargetY=-1;
  m_HDTotal=0.0;
  m_targets.RemoveAll();
  m_maptarglist.RemoveAll();
}
//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::NumTargets
//
// PURPOSE: 
//
//*****************************************************************************
int SPELL_TARGETING_DATA::NumTargets() const
{ 
  return m_targets.GetSize(); 
}

//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::NumMapTargets
//
// PURPOSE: 
//
//*****************************************************************************
int SPELL_TARGETING_DATA::NumMapTargets() const
{ 
  return m_maptarglist.GetCount(); 
}

//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::CanAddTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_TARGETING_DATA::STD_CanAddTarget(int hd, int targDistance) 
{ 
  if (m_SelectingUnits)
  {
    if ((m_MaxTargets > 0) && (NumTargets() >= m_MaxTargets))
      return FALSE;
    if ((m_MaxHitDice > 0) && (hd+m_HDTotal > m_MaxHitDice))
      return FALSE;
    if ((m_MaxRange > 0) && (targDistance > m_MaxRange))
      return FALSE;
  }
  else
  {
    return ((m_MapTargetX<0)&&(m_MapTargetY<0));  
  }
  return TRUE;
}

BOOL SPELL_TARGETING_DATA::IsAlreadyTargeted(int target) const
{
  for (int i=0;i<NumTargets(); i++)
  {
    if (m_targets[i] == target)
      return TRUE; // this target already added
  };
  return FALSE;
}

//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::HDLimitReached
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_TARGETING_DATA::HDLimitReached()
{
  if (    (m_targType!=NowSelectByHitDice)
       && (m_targType!=LingerSelectByHitDice)
     )
  {
    return FALSE;
  };
  return ((m_MaxHitDice > 0) && (m_HDTotal >= m_MaxHitDice));
}

//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::FormatRemainingTargsText
//
// PURPOSE: 
//
//*****************************************************************************
CString SPELL_TARGETING_DATA::FormatRemainingTargsText()
{
  char title[50];

  switch (m_targType)
  {
  case NowSelf:
  case LingerSelf:
  case NowSelectedByCount:
  case LingerSelectedByCount:
  case NowWholeParty:
  case LingerWholeParty:
  case NowTouchedTargets:
  case LingerTouchedTargets:
    {
      int diff = (m_MaxTargets-NumTargets());
      if (IsCombatActive())
      {
        if (diff > GetNumCombatants()) diff = GetNumCombatants();
      }
      else
      {
        if (diff > party.numCharacters) diff = party.numCharacters;
      }
      sprintf(title, "CHOOSE %u TARGETS", diff);
    }
    break;
  case NowSelectByHitDice:
  case LingerSelectByHitDice:
    sprintf(title, "CHOOSE %2.1f HIT DICE", (m_MaxHitDice-m_HDTotal));
    break;
  case NowAreaCircle:
  case LingerAreaCircle:
    strcpy(title, "CHOOSE CENTER OF CIRCLE");
    break;
  case NowAreaLinePickStart:
  case LingerAreaLinePickStart:
    strcpy(title, "CHOOSE START OF LINE");
    break;
  case NowAreaLinePickEnd:  
  case LingerAreaLinePickEnd:  
    strcpy(title, "CHOOSE END OF LINE");
    break;
  case NowAreaSquare:       
  case LingerAreaSquare:       
    strcpy(title, "CHOOSE CENTER OF SQUARE");
    break;
  case NowAreaCone:         
  case LingerAreaCone:         
    strcpy(title, "CHOOSE START OF CONE");
    break;
  default:
    die(0xab53d);
    strcpy(title, "CHOOSE");
    break;
  }
  return (CString(title));
}

//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::ValidNumTargets
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_TARGETING_DATA::ValidNumTargets()
{
  switch (m_targType)
  {
  case NowSelf:
  case LingerSelf:
  case NowSelectedByCount:
  case LingerSelectedByCount:
  case NowWholeParty:
  case LingerWholeParty:
  case NowTouchedTargets:
  case LingerTouchedTargets:
    return (m_MaxTargets > 0);

  case NowSelectByHitDice:
  case LingerSelectByHitDice:
    return (m_MaxHitDice > 0);

  case NowAreaSquare:
  case LingerAreaSquare:
  case NowAreaCone:
  case LingerAreaCone:
  case NowAreaLinePickStart:
  case LingerAreaLinePickStart:
  case NowAreaLinePickEnd:  
  case LingerAreaLinePickEnd:  
  case NowAreaCircle:
  case LingerAreaCircle:
    return ((m_MaxRange>0)&&(m_MaxTargets > 0));  

  default:
    die(0xab53e);
    break;
  }
  return FALSE;
}

//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::AllTargetsChosen
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_TARGETING_DATA::AllTargetsChosen()
{
  switch (m_targType)
  {
  case NowSelf:
  case LingerSelf:
  case NowSelectedByCount:
  case LingerSelectedByCount:
  case NowWholeParty:
  case LingerWholeParty:
  case NowTouchedTargets:
  case LingerTouchedTargets:
    if (NumTargets() >= m_MaxTargets)
      return TRUE;
    if (IsCombatActive())
    {
      if (NumTargets() >= GetNumCombatants())
        return TRUE;
    }
    else
    {
      if (NumTargets() >= party.numCharacters)
        return TRUE;
    }
    break;
  case NowSelectByHitDice:
  case LingerSelectByHitDice:
    if (HDLimitReached()) return TRUE;
    if (IsCombatActive())
    {
      if (NumTargets() >= GetNumCombatants())
        return TRUE;
    }
    else
    {
      if (NumTargets() >= party.numCharacters)
        return TRUE;
    }
    break;
  case NowAreaLinePickStart:
  case LingerAreaLinePickStart:
  case NowAreaLinePickEnd:
  case LingerAreaLinePickEnd:
  case NowAreaSquare:     
  case LingerAreaSquare:     
  case NowAreaCone:       
  case LingerAreaCone:       
  case NowAreaCircle:
  case LingerAreaCircle:
    return ((m_MapTargetX>=0)&&(m_MapTargetY>=0));
    break;

  default:
    die(0xab53f);
    break;
  }
  return FALSE;
}

//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::AddTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_TARGETING_DATA::STD_AddTarget(int target, 
                                         double hd, 
                                         int range, 
                                         int mapX, 
                                         int mapY)
{
  if (m_SelectingUnits==FALSE) return FALSE;
  if (!STD_CanAddTarget(hd,range)) return FALSE; 
  if (IsAlreadyTargeted(target)) return FALSE;
  int index = m_targets.GetSize();
  m_targets.SetAtGrow(index, target);
  if (    (m_targType==NowSelectByHitDice)
       || (m_targType==LingerSelectByHitDice)
     )
  {
    m_HDTotal+=hd;
    TRACE("HD adding %f, total %f, max %f\n",hd, m_HDTotal, m_MaxHitDice);
  };
  if (mapX >= 0)
  {
    m_MapTargetX = mapX;
    m_MapTargetY = mapY;
  };
  return TRUE;
}

/*
//
// data format for map targeting:
// 32 bits, 16 bits each for mapx and mapy: XX XX YY YY
//
void SplitMapTargetingData(DWORD data, WORD &mapx, WORD &mapy)
{
  mapx  = static_cast<WORD>((data >> 16) & 0x0000FFFF);
  mapy  = static_cast<WORD>(data & 0x0000FFFF);
}

void EncodeMapTargetingData(DWORD &data, WORD mapx, WORD mapy)
{
  data = 0;
  mapx  &= 0xFFFF;  data |= (mapx << 16);
  mapy  &= 0xFFFF;  data |= mapy;  
}
*/

CString OverrideTargetingHook(SPELL_DATA *pSpell, 
                              COMBATANT *pAttacker, 
                              COMBATANT *pTarget, 
                              int mapx, int mapy, const char *type)
{
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  CString ans;
  char t[40];
  _itoa(mapx, t, 10); hookParameters[5] = t;
  _itoa(mapy, t, 10); hookParameters[6] = t;
  hookParameters[7] = type;
  scriptContext.SetSpellContext(pSpell->SpellID());
  scriptContext.SetAttackerContext(pAttacker);
  scriptContext.SetTargetContext(pTarget);
  ans = pSpell->RunSpellScripts(OVERRIDE_SPELL_TARGETING, 
                                ScriptCallback_RunAllScripts, 
                                NULL, 
                                "Selecting a spell target");
  return ans;
}

//*****************************************************************************
//    NAME: SPELL_TARGETING_DATA::AddMapTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_TARGETING_DATA::AddMapTarget(int mapx, int mapy)
{
  if (m_SelectingUnits==TRUE) return FALSE;
  if (!STD_CanAddTarget(0,0)) return FALSE;
  m_MapTargetX=mapx;
  m_MapTargetY=mapy;
  return TRUE;
}

///////////////////////////////////////////////////////////////////////
//
// Pending Spell List Functions
//
///////////////////////////////////////////////////////////////////////

//*****************************************************************************
//    NAME: PENDING_SPELL_LIST::GetNextKey
//
// PURPOSE: 
//
//*****************************************************************************
int PENDING_SPELL_LIST::GetNextKey()
{
  int key=-1;
  if (GetCount() > 0)
  {
    key = m_spells.GetTail().key;
    if (key >= INT_MAX)
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is greater than max elements
      // Gaps occur when deletions are made to the list
      POSITION pos = GetHeadPosition();
      BOOL found = FALSE;        
      int prevKey = 0;
      int currKey;
      while ((pos != NULL) && (!found))
      {
        currKey = GetNext(pos).key;
        if (currKey != prevKey+1)
        {
          key = prevKey+1;
          found = TRUE;
        }
        else
          prevKey = currKey; 
      }
      VERIFY(found); // should never happen
    }
    else
      key++;
  }
  else
    key = 1;
  return key;
}

//*****************************************************************************
//    NAME: PENDING_SPELL_LIST::Add
//
// PURPOSE: 
//
//*****************************************************************************
int PENDING_SPELL_LIST::Add(PENDING_SPELL &data)
{
//  CSingleLock sLock(&m_CS, TRUE);
  data.key = GetNextKey();
  m_spells.Insert(data, data.key);
  return data.key;
}

//*****************************************************************************
//    NAME: PENDING_SPELL_LIST::Set
//
// PURPOSE: 
//
//*****************************************************************************
BOOL PENDING_SPELL_LIST::Set(int index, PENDING_SPELL &spell)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos=NULL;
  if ((pos = m_spells.FindKeyPos(index)) != NULL)
  {
    GetAtPos(pos) = spell;
    return TRUE;
  }
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: PENDING_SPELL_LIST::Remove
//
// PURPOSE: 
//
//*****************************************************************************
BOOL PENDING_SPELL_LIST::Remove(int index)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos=NULL;
  if ((pos = m_spells.FindKeyPos(index)) != NULL)
  {
    m_spells.RemoveAt(pos);
    return TRUE;
  }
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: *PENDING_SPELL_LIST::Get
//
// PURPOSE: 
//
//*****************************************************************************
PENDING_SPELL *PENDING_SPELL_LIST::Get(int index)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos=NULL;
  if ((pos = m_spells.FindKeyPos(index)) != NULL)
    return &GetAtPos(pos);
  else
    return NULL;
}

// check to see if casting time has passed, then inform caster
// to activate spell and remove from pending.
//
// ** elapsed time check only needed when in combat **
//
//*****************************************************************************
//    NAME: PENDING_SPELL_LIST::ProcessTimeSensitiveData
//
// PURPOSE: 
//
//*****************************************************************************
bool PENDING_SPELL_LIST::ProcessTimeSensitiveData(int roundInc, int currInitiative)
{
  bool castIt=false;
  //
  // what happens to pending spells when combat ends?
  //
  ASSERT( IsCombatActive() );
  if (IsCombatActive() == FALSE) return false;

  POSITION pos=GetHeadPosition();
  while (pos != NULL)
  {
    POSITION tmp=pos;
    GetNext(tmp);
    const PENDING_SPELL& data = PeekAtPos(pos);

    ASSERT(data.caster.IsValidSrc());
    ASSERT(data.caster.IsCombatSrc());

    switch (data.caster.GetSrc())
    {
    case FLAG_PARTY_MEMBER:
    case FLAG_NPC:
    case FLAG_MONSTER:
      {        
        spellCastingTimeType type = (spellCastingTimeType) data.flags;
        switch (type)
        {
        case sctImmediate:
          castIt=true;
          break;
        case sctInitiative:
          {
            //COMBATANT &dude = GetCombatCharacterContext(data.caster);
            castIt = (((unsigned)currInitiative>=data.waitUntil)||(roundInc>0));
          }
          break;
        case sctRounds:
        case sctTurns:
          castIt = (data.waitUntil <= (DWORD)GetCurrentRound());//CurrCombatRound());
          break;
        }    
      }
      break;

    case FLAG_ITEM:
      // item spells are cast right away
      castIt=true;
      break;
    }
    
    if (castIt)
    {      
      if (logDebuggingInfo)
      {
        WriteDebugString("Activating pending spell for char %i, sdbkey %s, turn %i, round %i, initiative %i\n",
          //data.caster.Instance, data.gsID, combatData.m_iCurrTurn, combatData.m_iCurrRound, combatData.m_iCurrInitiative);
          data.caster.Instance, data.spellID.UniqueName(), combatData.m_iCurrTurn, combatData.m_iCurrRound, combatData.m_iCurrInitiative);
        WriteDebugString("%u pending spells remaining\n", GetCount() - 1);
      };
      SpellActivate(data, false, NULL);
      RemoveAt(pos);
    }   

    pos=tmp;
  } 
  return castIt;
}
///////////////////////////////////////////////////////////////////////
//
// Active Spell List Functions
//
///////////////////////////////////////////////////////////////////////

BOOL SPELL_LINGER_DATA::BlocksCombatant(int mapx, int mapy, COMBATANT *pCombatant, SPELL_DATA *pSpell) const
{
  POSITION pos = mapdata.GetHeadPosition();
  int spellBlocksCombatant;
  spellBlocksCombatant = -1; // Means that we don't yet know.
  while (pos != NULL)
  {
    POINT pt = mapdata.GetAt(pos);
    if ((pt.x == mapx) && (pt.y == mapy))
    {
      if (spellBlocksCombatant < 0)
      {
        {
          CString result;
          HOOK_PARAMETERS hookParameters;
          SCRIPT_CONTEXT scriptContext;
          scriptContext.SetSpellContext(pSpell);
          scriptContext.SetCombatantContext(pCombatant);
          result = pSpell->RunSpellScripts(
                                  SPELL_LINGER_BLOCKAGE, 
                                  ScriptCallback_RunAllScripts, 
                                  NULL,
                                  "Does a lingering spell block combatant?");
          spellBlocksCombatant = 1;
          if ((result.GetLength()> 0) && (result[0] == 'N')) spellBlocksCombatant = 0;
        };
      };
      return spellBlocksCombatant == 1;
    };
    mapdata.GetNext(pos);
  }
  return FALSE;//(EligibleTarget(targ));
}



BOOL ACTIVE_SPELL::LingerSpellBlocksCombatant(int mapx, int mapy, COMBATANT *pCombatant) const
{
  SPELL_DATA *pSpell;
  if (!Lingers) return FALSE;
  //pSpell = spellData.GetSpellData(spellKey);
  pSpell = spellData.GetSpell(spellID);
  return (lingerData.BlocksCombatant(mapx, mapy, pCombatant, pSpell));
}


//*****************************************************************************
//    NAME: ACTIVE_SPELL_LIST::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void ACTIVE_SPELL_LIST::xClear() 
{ 
//  CSingleLock sLock(&m_CS, TRUE); 
  //POSITION pos = GetHeadPosition();
  //while (pos != NULL)
  //  party.RemoveSpellEffect(GetNext(pos).key);
  m_spellList.RemoveAll();
}

//*****************************************************************************
//    NAME: ACTIVE_SPELL_LIST::GetNextKey
//
// PURPOSE: 
//
//*****************************************************************************
int ACTIVE_SPELL_LIST::GetNextKey()
{
  // Search for an unused key.
  POSITION pos = GetHeadPosition();
  int prevKey = 0;
  int currKey;
  while (pos != NULL) 
  {
    currKey = GetNext(pos).key;
    if (currKey != prevKey+1)
    {
      return prevKey+1;
    }
    else
    {
      prevKey = currKey; 
    };
  };
  return prevKey+1;
}

// called when loading from scripts, or
// when adding from editor GUI
//*****************************************************************************
//    NAME: ACTIVE_SPELL_LIST::Add
//
// PURPOSE: 
//
//*****************************************************************************
int ACTIVE_SPELL_LIST::xAdd(ACTIVE_SPELL &data)
{
//  CSingleLock sLock(&m_CS, TRUE);
  //if (strlen(data.Name) == 0) return 0;
  // Do not allow multiple items with same full name.
  //if (m_FindName(data.Name)!=0) return 0;
  data.key = GetNextKey();
  m_spellList.Insert(data, data.key);
  return data.key;
}

// called when loading binary monster data that
// was saved with key info
//*****************************************************************************
//    NAME: ACTIVE_SPELL_LIST::AddWithCurrKey
//
// PURPOSE: 
//
//*****************************************************************************
int ACTIVE_SPELL_LIST::xAddWithCurrKey(ACTIVE_SPELL &data)
{
//  CSingleLock sLock(&m_CS, TRUE);
  //WriteDebugString("ACTIVE_SPELL_LIST::AddWithCurrKey(): key %i, spelldbkey %i\n", 
  //                 data.key,data.spellKey);
  WriteDebugString("ACTIVE_SPELL_LIST::AddWithCurrKey(): key %s, spelldbkey %s\n", 
                   data.spellID.UniqueName(),data.spellID.UniqueName());

  m_spellList.Insert(data, data.key);
  return data.key;
}

//*****************************************************************************
//    NAME: ACTIVE_SPELL_LIST::Set
//
// PURPOSE: 
//
//*****************************************************************************
BOOL ACTIVE_SPELL_LIST::Set(int index, ACTIVE_SPELL &spell)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos=NULL;
  if ((pos = m_spellList.FindKeyPos(index)) != NULL)
  {
    GetAtPos(pos) = spell;
    return TRUE;
  }
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: ACTIVE_SPELL_LIST::Remove
//
// PURPOSE: 
//
//*****************************************************************************
BOOL ACTIVE_SPELL_LIST::Remove(int index, const SPELL_DATA *pSpell)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos=NULL;
  if ((pos = m_spellList.FindKeyPos(index)) != NULL)
  {
    const ACTIVE_SPELL *pActiveSpell;
    pActiveSpell = &PeekAtPos(pos);
    if (pActiveSpell->spellID != pSpell->SpellID())
  {
      die("We don't understand active spells");
    };
    m_spellList.RemoveAt(pos);
    return TRUE;
  }
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: *ACTIVE_SPELL_LIST::Get
//
// PURPOSE: 
//
//*****************************************************************************
ACTIVE_SPELL *ACTIVE_SPELL_LIST::Get(int index)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos=NULL;
  if ((pos = m_spellList.FindKeyPos(index)) != NULL)
    return &GetAtPos(pos);
  else
    return NULL;
}
//*****************************************************************************
//    NAME: &ACTIVE_SPELL_LIST::operator=
//
// PURPOSE: 
//
//*****************************************************************************
ACTIVE_SPELL_LIST &ACTIVE_SPELL_LIST::operator=(ACTIVE_SPELL_LIST &src)
{
  if (this == &src) return *this;
  m_spellList.RemoveAll();
  POSITION pos = src.GetHeadPosition();
  while (pos!=NULL)
    xAddWithCurrKey(src.GetNext(pos));
  return *this;
}
//*****************************************************************************
//    NAME: ACTIVE_SPELL_LIST::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void ACTIVE_SPELL_LIST::Serialize(CAR &ar, double ver)
{
  POSITION p;
  if (ar.IsStoring())
  {
    ar << GetCount();
    p = GetHeadPosition();
    while (p != NULL)
      GetNext(p).Serialize(ar, ver);
  }
  else
  {
    m_spellList.RemoveAll();
    ACTIVE_SPELL data;
    int count;
    ar >> count;
    for (int i=0;i<count;i++)
    {
      data.Clear();
      data.Serialize(ar, ver);
      xAddWithCurrKey(data);
      //WriteDebugString("Loading active spell: k %i, sk %i, st %u, c %u\n",
      //                 data.key, data.spellKey, data.StopTime, data.caster.Instance);
      WriteDebugString("Loading active spell: k %s, sk %s, st %u, c %u\n",
                       data.spellID.UniqueName(), data.spellID.UniqueName(), data.StopTime, data.caster.Instance);
    }
  }
}

//*****************************************************************************
//    NAME: ACTIVE_SPELL_LIST::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void ACTIVE_SPELL_LIST::Serialize(CArchive &ar, double ver)
{
  POSITION p;
  if (ar.IsStoring())
  {
    ar << GetCount();
    p = GetHeadPosition();
    while (p != NULL)
      GetNext(p).Serialize(ar, ver);
  }
  else
  {
    m_spellList.RemoveAll();
    ACTIVE_SPELL data;
    int count;
    ar >> count;
    for (int i=0;i<count;i++)
    {
      data.Clear();
      data.Serialize(ar, ver);
      xAddWithCurrKey(data);
      //WriteDebugString("Loading active spell: k %i, sk %i, st %u, c %u\n",
      //                 data.key, data.spellKey, data.StopTime, data.caster.Instance);
      WriteDebugString("Loading active spell: k %s, st %u, c %u\n",
                       data.spellID.UniqueName(), data.StopTime, data.caster.Instance);
    }
  }
}

//*****************************************************************************
//    NAME: ACTIVE_SPELL_LIST::DeactivateActiveSpell
//
// PURPOSE: 
//
//*****************************************************************************
void ACTIVE_SPELL_LIST::DeactivateActiveSpell(int index, const SPELL_DATA *pSpell)
{ 
  if (pSpell != NULL)
    WriteDebugString("Deactivating spell from ACTIVE_SPELL_LIST: akey %i, spell %s\n", index, pSpell->Name);
  else
    WriteDebugString("Deactivating spell from ACTIVE_SPELL_LIST: akey %i, spell NULL\n", index);

  if (IsCombatActive())
    combatData.RemoveSpellEffect(index, pSpell, true);
  else
    party.RemoveSpellEffect(index, pSpell, true);
  
  // 'linger' spell animations
  RemoveAnimationFromList(index);  
  {
    POSITION pos=NULL;
    const ACTIVE_SPELL *pActiveSpell;
    if ((pos = m_spellList.FindKeyPos(index)) != NULL)
    {
#ifdef SpellInitiationScript
      CHARACTER *pAttacker;
      pActiveSpell = &PeekAtPos(pos);
      pAttacker = GetCurrentlyActiveContext(&pActiveSpell->caster,"ACTIVE_SPELL_LIST::DeactivateActiveSpell");
      if (Remove(index, pSpell))  
      {
        pSpell->ExecuteSpellTerminationScript(pAttacker);
      };
    };
#else
      CHARACTER *pAttacker;
      CHARACTER *pTarget;
      pActiveSpell = &PeekAtPos(pos);
      pAttacker = GetCurrentlyActiveContext(&pActiveSpell->caster);
      pTarget   = GetCurrentlyActiveContext(&pActiveSpell->target);


      if (Remove(index, pSpell))  
      {
        pSpell->ExecuteSpellEndScript(pAttacker, pTarget);
      };
    };
#endif
  };
}

#ifdef SpellInitiationScript
void ACTIVE_SPELL_LIST::ExitCombat(void)
{
  POSITION pos = GetHeadPosition();
  while (pos != NULL)
  {
    ACTIVE_SPELL *pData;
    //SPELL_DATA *pSpell;
    pData = &GetNext(pos);
    //pSpell = spellData.GetSpell(pData->spellID);
    pData->caster.ExitCombat();
    pData->target.ExitCombat();
  };
}
#endif

// check to see if spell is ready to expire, then
// remove effect from all characters
//
//*****************************************************************************
//    NAME: ACTIVE_SPELL_LIST::ProcessTimeSensitiveData
//
// PURPOSE: 
//
//*****************************************************************************
void ACTIVE_SPELL_LIST::ProcessTimeSensitiveData(int roundInc)
{
  POSITION pos=GetHeadPosition();
  while (pos != NULL)
  {
    POSITION tmp=pos;
    GetNext(tmp);

    ACTIVE_SPELL &data = GetAtPos(pos);

    //SPELL_DATA *pSpell = spellData.GetSpellData(data.spellKey);
    SPELL_DATA *pSpell = spellData.GetSpell(data.spellID);
    
    bool SpellOver=false;

    if (pSpell != NULL)
    {     
      switch (pSpell->Duration_Rate)
      {
      case byNbrAttacks:
      case byDamageTaken: 
        if (data.CountTime >= data.StopTime)
          SpellOver=true;
        break;

      default:
        if (party.getElapsedMinutes() > data.StopTime)
          SpellOver=true;
        break;
      }

      if (SpellOver)
        WriteDebugString("Removing active spell cast by %i\n", data.caster.Instance);        
    }
    else
    {
      die(0xab540);
      SpellOver=true;
      WriteDebugString("Removing active spell cast by %i - invalid spell key\n", data.caster.Instance);
    }

    if (SpellOver)
      DeactivateActiveSpell(data.key, pSpell);

    pos=tmp;
  }
}

//*****************************************************************************
//    NAME: ACTIVE_SPELL_LIST::RemoveLingerSpells
//
// PURPOSE: After combat is over, all spells that linger on the combat map
//          need to be removed.
//
//*****************************************************************************
void ACTIVE_SPELL_LIST::RemoveLingerSpells()
{
  POSITION pos = GetHeadPosition();
  POSITION temp;
  while (pos != NULL)
  {
    temp = pos;
    ACTIVE_SPELL &as = GetNext(pos);
    ASSERT( as.key >= 0 );

    if (as.Lingers)
    {
      // 'linger' spell animations
      const SPELL_DATA *pSpell;
      RemoveAnimationFromList(as.key);  
      pSpell = spellData.PeekSpell(as.spellID);
      DeactivateActiveSpell(as.key, pSpell);
      //die(0xab541); // What about running spell end scripts????

      Remove(as.key, NULL);      
    } 
    else
    {
#ifdef SpellInitiationScript
      //die(0xab542); // Should we run spell termination script?
                      // No.  Here is why.
                      // Temporary spells will still be active until the activeSpellList
                      // entry is removed.  Permanent spells ran their end scripts 
                      // immediately after the spell was cast.
#else
      die(0xab542);  // Should we run spell end script
#endif
    };
  }   
}

//*****************************************************************************
//    NAME: ACTIVE_SPELL_LIST::ActivateLingerSpellsOnTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL ACTIVE_SPELL_LIST::ActivateLingerSpellsOnTarget(int targ, int mapx, int mapy, int width, int height)
{
//  CSingleLock sLock(&m_CS, TRUE);
  
  ASSERT( targ != NO_DUDE );

  BOOL AtLeastOneSpellActivated = FALSE;
  
  POSITION pos = GetHeadPosition();
  while (pos != NULL)
  {
    ACTIVE_SPELL &as = GetAtPos(pos);
    ASSERT( as.key >= 0 );

    if (as.LingerSpellAffectsTarget(targ, mapx, mapy, width, height))
    {
      AtLeastOneSpellActivated=TRUE;
      as.lingerData.AddTarget(targ);

      int caster = as.caster.GetInstance();
      ASSERT( caster != NO_DUDE );

      if (caster == NO_DUDE)
        continue;        
      
      COMBATANT *cdude = GetCombatantPtr(caster);
      
      //combatData.InvokeAreaLingerSpellOnCombatant(cdude->self, targ, as.key, false, NULL, NULL, as.spellKey);
      combatData.InvokeAreaLingerSpellOnCombatant(cdude->self, targ, as.key, false, NULL, NULL, as.spellID);
    }

    GetNext(pos);
  }

  return AtLeastOneSpellActivated;
}

BOOL ACTIVE_SPELL_LIST::LingerSpellBlocksCombatant(int mapx, int mapy, COMBATANT *pCombatant) const
{
  POSITION pos = GetHeadPosition();
  while (pos != NULL)
  {
    if (PeekAtPos(pos).LingerSpellBlocksCombatant(mapx, mapy, pCombatant))
      return TRUE;
    PeekNext(pos);
  }
  return FALSE;
};

CString ACTIVE_SPELL_LIST::RunSEScripts(int x, int y, const CString& scriptName, LPCSTR comment)
{
  bool scriptProcessed;
  CString result;
  POSITION pos = GetHeadPosition();  
  for ( ;pos != NULL; GetNext(pos))
  {
    GetAtPos(pos).m_XscriptProcessed = false;
    // Any spell effects that get added will have m_scriptProcessed = true;
  };
  for (scriptProcessed=true; scriptProcessed;)
  {
    scriptProcessed = false;
    for (pos = GetHeadPosition() ;pos != NULL; GetNext(pos))
    {
      ACTIVE_SPELL *pActiveSpell;
      pActiveSpell = &GetAtPos(pos);
      if (!pActiveSpell->m_XscriptProcessed)
      {
        //GLOBAL_SPELL_ID gsID;
        SPELL_ID spellID;
        SPELL_LINGER_DATA *pSLD;
        MAPLOCATIONLIST *pMLL;
        CString SEIdentifier;
        POSITION mapPos;

        pActiveSpell->m_XscriptProcessed = true;
        //gsID = pActiveSpell->spellKey;
        spellID = pActiveSpell->spellID;
        pSLD = &pActiveSpell->lingerData;
        pMLL = &pSLD->mapdata;

        for (mapPos=pMLL->GetHeadPosition(); mapPos!=NULL; pMLL->GetNext(mapPos))
        {
          POINT *pPoint;

          pPoint = &pMLL->GetAt(mapPos);
          if ((pPoint->x == x) && (pPoint->y == y))
          {
            SPELL_DATA *pSpellData;
            //pSpellData = spellData.GetSpellData(gsID);
            pSpellData = spellData.GetSpell(spellID);
            pScriptContext->SetSpellContext(pSpellData);
            result += pSpellData->RunSpellScripts(
                             scriptName, 
                             ScriptCallback_RunAllScripts, 
                             NULL,
                             comment);
          };
        };
        scriptProcessed = true;
        break;
      };
    };
  };
  return result;
}


#endif // ifdef UAFEngine


///////////////////////////////////////////////////////////////////////
//
// Spell Text List Functions
//
///////////////////////////////////////////////////////////////////////

#ifdef UAFEngine

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_TEXT_LIST::Clear()
{
  title = "";
  //m_SpellLevelFilter=0;
  //spellClasses=0;  
  baseclassList.Clear();  
  m_flags=LEVEL_FLAG;
  CostFactor = Normal;
  m_CharData=NULL;
#ifdef newMEMORIZE
  //memset(maxLevelBySchool  ,0, sizeof(maxLevelBySchool));
  //memset(maxSelectBySchool ,0, sizeof(maxSelectBySchool));
  //memset(currSelectBySchool,0, sizeof(currSelectBySchool));
  selectCounts.RemoveAll();
#else
  memset(CurrSelect,0,sizeof(CurrSelect));
  memset(MaxSelect,0,sizeof(MaxSelect));
  memset(MaxLevel, 0, sizeof(MaxLevel));
#endif
  isSelected.RemoveAll();
  spellIDs.RemoveAll();
  spellEffects.RemoveAll();
  ClearArbitraryText();
  races.RemoveAll();
  genders.RemoveAll();
  alignments.RemoveAll();
  classes.RemoveAll();
  integers.RemoveAll();
  POSITIONs.RemoveAll();
}

int CompareSpellsForNewSpellList( const void *arg1, const void *arg2 )
{
  /*
    must return
  < 0 elem1 less than elem2 
  = 0 elem1 equivalent to elem2 
  > 0 elem1 greater than elem2 
  */
  ASSERT( (arg1!=NULL) && (arg2!=NULL) );
  if ((arg1==NULL)||(arg2==NULL)) return 0;

  //SPELL_DATA *pElem1 = spellData.GetSpellData( (GLOBAL_SPELL_ID) (*((DWORD*)arg1)) );
  //SPELL_DATA *pElem2 = spellData.GetSpellData( (GLOBAL_SPELL_ID) (*((DWORD*)arg2)) );
  SPELL_DATA *pElem1 = spellData.GetSpell( (SPELL_ID) (*((SPELL_ID*)arg1)) );
  SPELL_DATA *pElem2 = spellData.GetSpell( (SPELL_ID) (*((SPELL_ID*)arg2)) );

  // sort by spell level, then by spell name
  if (pElem1->Level < pElem2->Level) return -1;
  if (pElem1->Level > pElem2->Level) return  1;

  if (pElem1->Level == pElem2->Level)
  {
    if (pElem1->Name < pElem2->Name) return -1;
    if (pElem1->Name > pElem2->Name) return  1;
  }

  // level and name are both equal
  return 0;
}


//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::FillNewSpellLevelListText
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_TEXT_LIST::FillNewSpellLevelListText(CHARACTER &data)
{
  //int i;
	Clear();
  m_CharData=&data;
  m_flags=NEW_SPELL_LEVEL_FORMAT_FLAG; 
  //spellClasses = SetClassBitsFromCharClass(data.GetAdjClass());
  baseclassList.Clear();
  CLASS_ID classID = data.GetAdjClass();
  const CLASS_DATA *pClass;
  pClass = classData.PeekClass(classID);
  baseclassList = *pClass->BaseclassList();
  
  //CArray<GLOBAL_SPELL_ID, GLOBAL_SPELL_ID> sortarray;
  CArray<SPELL_ID, SPELL_ID> sortarray;

  // only MU spells for now
  // clerics (and derivative classes) have access to all spells automatically
  //

/*
  if (!CanCastMagicUserSpells(data.GetAdjClass()))
*/ /* Really */ NotImplemented(0x8f31891, false);


  {
    die(0xab542);
    return;
  }  

  // When a character advances a level, the spell book is
  // auto updated per the following:
  //
  // Magic Users will automatically be able to select spells that
  //    CanScribeAtLevelChange=TRUE
  //    character is eligible to memorize (intel, level)
  //    is not already in spell book
  //
  // For spells that are not added automatically, the
  // char gets to choose from a list of spells of a 
  // certain level, and pick spells until the 
  // max spell limit for char level is reached.
  //

  // fill in the list of spells that are allowed to be known
  //POSITION pos = spellData.GetHeadPosition();
  //for ( ;pos != NULL; spellData.GetNext(pos))

  /* Unreachable
  int n;
  n = spellData.GetCount();
  for (i=0; i<n; i++)
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    //SPELL_DATA& sdata = spellData.GetAtPos(pos);
    const SPELL_DATA& sdata = *spellData.PeekSpell(i);

    ClearHookParameters();
    scriptContext.SetCharacterContext(&data);
    scriptContext.SetSpellContext(&sdata);
    if (!sdata.RunSpellScripts(CAN_KNOW_SPELL, ScriptCallback_LookForChar, "N"))
    {
      continue;
    };
    if (!data.RunCharacterScripts(CAN_KNOW_SPELL, ScriptCallback_LookForChar, "N"))
    {
      continue;
    };

    // if spell level matches new spell level
    //if ( Not Implemented(0xdc4adc5, false), (sdata.UsableByParty) 
    if ( Not Implemented(0xdc4adc5, false), (sdata.CanMemorize) 
        //&& (sdata.CanScribeAtLevelChange) 
        && (sdata.AllowScribe) 
        //&& (sdata.schoolMask == MagicUserFlag) 
        && (sdata.schoolID == "MagicUser") 
        //&& (sdata.CastMask & spellClasses))
        && (sdata.allowedBaseclasses & baseclassList))  // An overlap
    {
      // fill in the number of spells that *can* be known

      // !!!!!!!! this next line makes the assumption that we are selecting spells for a MU only !!!!!!!!!!
      // which is true for the current rules, but may change when rules are more configurable
	  Not Implemented(0xec5ca6, false);
      //if (data.MaxSpellsForLevel(MagicUserFlag, 
      //                           data.GetCurrentLevel(MagicUserFlag), 
      //                           sdata.Level, 
      //                           data.GetPrimeSpellCastingScore(MagicUserFlag)) > 0)
      {
        // if not already in character's spell book
        //if (!data.HaveSpell(sdata.m_gsID, FALSE))
        if (!data.HaveSpell(sdata.SpellID(), FALSE))
        {
          //if (data.CanKnowSpell(MagicUserFlag, sdata.schoolMask, sdata.CastMask, sdata.Level))
          Not Implemented(0x9d741a, false);
          //if (data.CanKnowSpell(NULL, sdata.schoolID, sdata.allowedBaseclasses, sdata.Level))
          if (data.CanKnowSpell(sdata.schoolID, sdata.Level))
          {          
            //ASSERT( NumClassBits(sdata.schoolMask) == 1 );
            
            // add to temp list so that we can sort the output properly (later)
            //Add(sdata.key);
            //sortarray.Add(sdata.m_gsID);
            sortarray.Add(sdata.SpellID());
          }
        }
      }
    }

  }

  // if more than one element, sort them
  if (sortarray.GetSize() > 1)
    qsort( &sortarray[0], sortarray.GetSize(), sizeof(DWORD), CompareSpellsForNewSpellList);

  // add sorted elements to list
  for (i=0;i<sortarray.GetSize();i++)
    Add(sortarray[i]);

  sortarray.RemoveAll();


  // fill in the number of spells currently known
  //pos = data.GetFirstSpellPosition(); // access ordered list 
  //while (pos != NULL)
  n = data.GetSpellCount();
  for (i=0; i<n; i++)
  {
    //int slevel = data.GetSpellAt(pos).level;
    int slevel = data.PeekCharacterSpell(i)->level;
    
    ASSERT ((slevel>=1)&&(slevel<=MAX_SPELL_LEVEL));
    if (slevel <= 0) slevel = 1;

    //SPELL_DATA *sdata = spellData.GetSpellData(data.GetSpellAt(pos).spell); 
    const SPELL_DATA *pSpell;
    pSpell  = spellData.PeekSpell(data.PeekCharacterSpell(i)->spellID); 

    //ASSERT( NumClassBits(sdata->schoolMask) == 1 );

    //if (sdata->schoolMask == MagicUserFlag)
    //if (pSpell->schoolMask == MagicUserFlag)
#ifdef newMEMORIZE
      //currSelectBySchool[MagicUserSchool][slevel-1] += 1; // inc num spells known at this level
    IncCurrSelectCount(SCHOOL_ID(), slevel);
#else
      CurrSelect[slevel-1][FlagToIndex(GetSpellCastingSchoolForCharBaseclass(sdata->schoolMask))] += 1; // inc num spells known at this level
#endif
    
    //data.GetNextSpell(pos);
  }


  */

/*
  // fill in the number of spells that *can* be known
  for (i=0;i<NumBaseClass;i++)
  {
    WORD flag = (spellClasses & (0x0001 << i));
    if (flag)
    {
      if (GetSpellCastingSchoolForCharBaseclass(flag) == MagicUserFlag)
      {
        int index = FlagToIndex(GetSpellCastingSchoolForCharBaseclass(flag));
#ifdef newMEMORIZE
        //maxLevelBySchool[MagicUserSchool][index] += CurrSpellLevel(flag,
        maxLevelBySchool[MagicUserSchool][index] += MaxSpellLevelForClassAndSchool(flag,
                                                                   data.GetCurrentLevel(flag),
                                                                   data.GetPrimeSpellCastingScore(flag), 
                                                                   GetSpellCastingSchoolForCharBaseclass(flag));
#else
        MaxLevel[index] += CurrSpellLevel(flag,
                                         data.GetCurrentLevel(flag),
                                         data.GetPrimeSpellCastingScore(flag), 
                                         GetSpellCastingSchoolForCharBaseclass(flag));
#endif

#ifdef newMEMORIZE
        for (int j=0;j<maxLevelBySchool[MagicUserSchool][index];j++)
        {
          maxSelectBySchool[MagicUserSchool][j][index] += 
            data.MaxSpellsForLevel(flag, data.GetCurrentLevel(flag), j+1, data.GetPrimeSpellCastingScore(flag));
        }
#else
        for (int j=0;j<MaxLevel[index];j++)
        {
          MaxSelect[j][index] += 
            data.MaxSpellsForLevel(flag, data.GetCurrentLevel(flag), j+1, data.GetPrimeSpellCastingScore(flag));
        }
#endif
      }
    }
  }
  Not Implemented(0x1d5bc9, false);



  FormatSpellListTitle("NO SPELLS AVAILABLE");

  // this flag should be set by now
  ASSERT(UseNewSpellLevelLayout());
  */
}

void MemorizeSummary::Clear(void)
{
  spells.RemoveAll();
}

int CreateSpellAvailabilityList( CHARACTER *pChar,
                                 CArray<AVAILABLE_SPELL, AVAILABLE_SPELL&> *pSpellAvailabilityList,
                                 LPCSTR comment)
{
  int n;
  CLASS_DATA *pClass;
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  AVAILABLE_SPELL avs;
  int maxSpellLevel=0;

  pSpellAvailabilityList->RemoveAll();

  //if (m_dude.GetUniqueId() == 0xff)
  //if (m_dude.CharacterID().IsEmpty())
  if (pChar->uniquePartyID == 0xff)
  {
    scriptContext.SetCreatedCharacterContext(pChar);
    hookParameters[4] = "I";
  }
  else
  {
    scriptContext.SetCharacterContext(pChar);
    hookParameters[4] = "L";
  };
  pClass = classData.GetClass(pChar->GetClass());
  scriptContext.SetClassContext(pClass);
  hookParameters[5].Format("%d",pChar->GetLimitedInt());

  //for (pos = spellData.GetHeadPosition(); pos!=NULL;)
  //int i;
  n = spellData.GetCount();
  for (int i=0; i<n; i++)
  {
    SPELL_DATA *pSpell;
    CString result;
    int probability;
    //pSpell = &spellData.GetNext(pos);
    pSpell = spellData.GetSpell(i);
    if (!pSpell->AllowScribe) continue;
    //if (!m_dude.CanKnowSpell(pClass, pSpell->schoolID, pSpell->allowedBaseclasses, pSpell->Level))
    if (!pChar->CanKnowSpell(pSpell->schoolID, pSpell->Level))
    {
      continue;
    };
    {
      int j, m;
      const BASECLASS_ID *pBaseclassID;
      m = pSpell->allowedBaseclasses.GetCount();
      for (j=0; j<m; j++)
      {
        pBaseclassID = pSpell->allowedBaseclasses.PeekBaseclassID(j);
        if (pClass->LocateBaseclass(*pBaseclassID) >= 0) break;
      };
      if (j == m) continue;  // Cound not find baseclass
    };
    scriptContext.SetSpellContext(pSpell);
    hookParameters[6].Format("%d",pSpell->Level);

    
    result = pClass->RunClassScripts(KNOWABLE_SPELLS, 
                                     ScriptCallback_RunAllScripts, 
                                     NULL,
                                     comment);
    if (result.IsEmpty())
    {
      result = pSpell->RunSpellScripts(KNOWABLE_SPELLS, 
                                       ScriptCallback_RunAllScripts,
                                       NULL,
                                       comment);
    };
    if (result.IsEmpty())
    {
      probability = 100;
    }
    else
    {
      sscanf(result, "%d", &probability);
    };
    if (probability != 0)
    {
      avs.pSpellData = pSpell;
      avs.probability = probability;
      pSpellAvailabilityList->Add(avs);
      if (avs.pSpellData->Level > maxSpellLevel)
      {
        maxSpellLevel = avs.pSpellData->Level;
      };
    };
  };
  return maxSpellLevel;
}


//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::FillMemorizeSpellListText
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_TEXT_LIST::FillMemorizeSpellListText(CHARACTER *pChar)
{
	Clear(); 
  m_flags=MEMORIZE_FORMAT_FLAG;  
  m_CharData = pChar;

  int charSpellIndex;
  const CHARACTER_SPELL *pCharSpell;
  int numCharSpell;  // Total number of spells in spellbook.
  MEMORIZE_ITEM memorizeItem;

  memSum.Clear();
  pChar->UpdateSpellAbility();
  numCharSpell = pChar->GetSpellCount();
  for (charSpellIndex=0; charSpellIndex<numCharSpell; charSpellIndex++)
  {
    memorizeItem.charSpellIndex = charSpellIndex;
    pCharSpell = pChar->PeekCharacterSpell(charSpellIndex);
    memorizeItem.spellDataIndex = spellData.LocateSpell(pCharSpell->spellID);
    if (memorizeItem.spellDataIndex >= 0)
    {
      SCHOOL_ID schoolID;
      const SPELL_DATA *pSpell;
      int level;
      pSpell = spellData.PeekSpell(memorizeItem.spellDataIndex);
      schoolID = pSpell->schoolID;
      level = pSpell->Level;
      const SCHOOL_ABILITY *pSchoolAbility;
      pSchoolAbility = pChar->spellAbility.PeekSchoolAbility(schoolID);
      if (pSchoolAbility == NULL) continue;
      if (level > pSchoolAbility->maxAbilitySpellLevel) continue;
      memorizeItem.available =  pSchoolAbility->base[level-1];
      memorizeItem.available += pSchoolAbility->bonus[level-1];
      {
        // Now we need to add any adjustments
        // created by scripts.
        int i, n;
        n = pChar->GetSpellAdjCount();
        for (i=0; i<n; i++)
        {
          const SPELL_ADJ *pSpellAdj;
          pSpellAdj = pChar->PeekSpellAdj(i);
          if ((pSpellAdj->schoolID == "*") || (pSpellAdj->schoolID == schoolID))
          {
            if ((pSpellAdj->firstLevel<=level) && (pSpellAdj->lastLevel>=level))
            {
              memorizeItem.available *= pSpellAdj->p;
              memorizeItem.available /= 100;
              memorizeItem.available += pSpellAdj->b;
            };
          }
        };
      };
      memorizeItem.numMemorized = pCharSpell->memorized;
      memorizeItem.numSelected  = pCharSpell->selected;
      if (memorizeItem.available > 0)
      {
        memSum.Add(memorizeItem);
      };
    };
  };  
  // Now adjust the available to account for the number
  // already selectd and memorized.
  {
    int i,n;
    n = memSum.GetMemorizeItemCount();
    for (i=0; i<n; i++)
    {
      const MEMORIZE_ITEM *pMemorizeItem;
      const SPELL_DATA *pSpell;
      int reduction;
      int j;
      pMemorizeItem = memSum.PeekMemorizeItem(i);
      reduction = pMemorizeItem->numSelected;
      if (reduction <= 0) continue;
      pSpell = spellData.PeekSpell(pMemorizeItem->spellDataIndex);
      for (j=0; j<n; j++)
      {
        MEMORIZE_ITEM *pMI;
        const SPELL_DATA *pS;
        pMI = memSum.GetMemorizeItem(j);
        pS = spellData.PeekSpell(pMI->spellDataIndex);
        if (pSpell->schoolID != pS->schoolID) continue;
        if (pSpell->Level    != pS->Level)    continue;
        pMI->available -= reduction;
      };
    };
  };
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::FillTempleCastSpellListText
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_TEXT_LIST::FillTempleCastSpellListText(CHARACTER &data, costFactorType type)
{
	Clear(); 
  m_CharData=&data;
  m_flags=TEMPLE_CAST_FORMAT_FLAG|COST_FLAG|LEVEL_FLAG;  
  CostFactor = type;



//  spellClasses = SetClassBitsFromCharClass(data.GetAdjClass());  
//  {
    //POSITION pos = data.GetFirstSpellPosition(); // access ordered list
    // should be at least one spell, 
    // and they should be sorted by now
    int i, n;
    n = data.GetSpellCount();
    //ASSERT( pos != NULL );
    ASSERT( n > 0 );

    //while (pos != NULL)
    for (i=0; i<n; i++)
    {
//      int spellSchool;
      //SPELL_DATA *sdata = spellData.GetSpellData(data.GetSpellAt(pos).spell); 
//      const SPELL_DATA *pSpellData = spellData.PeekSpell(data.PeekCharacterSpell(i)->spellID); 
      //switch (sdata->schoolMask)
//      switch (pSpellData->schoolMask)
//      {
//        case MagicUserFlag: spellSchool = MagicUserSchool; break;
//        case ClericFlag:    spellSchool = ClericSchool;    break;
//        default:            spellSchool = MagicUserSchool; break;
//      };
      //if (data.GetSpellAt(pos).IsMemorized())
      if (data.PeekCharacterSpell(i)->IsMemorized())
      {
        // don't really need CurrSelect[] info when casting a spell that is already in
        // the char's spell book, but just to be consistent...
        //int level = data.GetSpellAt(pos).level;
        int level = data.PeekCharacterSpell(i)->level;
        ASSERT ((level>=1)&&(level<=MAX_SPELL_LEVEL));
        if (level <= 0) level = 1;
        //ASSERT( NumClassBits(sdata->schoolMask) == 1 );
//        ASSERT( NumClassBits(pSpellData->schoolMask) == 1 );
        //currSelectBySchool[spellSchool][level-1] += data.GetSpellAt(pos).selected;
//        currSelectBySchool[spellSchool][level-1] += data.PeekCharacterSpell(i)->selected;
        //Add((DWORD)pos);
        Add(i);
      }
    //data.GetNextSpell(pos);
//    };
//  };

  // don't really need this info when casting a spell that is already in
  // the char's spell book, but just to be consistent...
//  for (int i=0;i<NumBaseClass;i++)
//  {
//    WORD flag = (spellClasses & (0x0001 << i));
//    if (flag)
//    {
//      int index = FlagToIndex(GetSpellCastingSchoolForCharBaseclass(flag));
      //maxLevelBySchool[MagicUserSchool][index] += CurrSpellLevel(flag,data.GetCurrentLevel(flag),data.GetPrimeSpellCastingScore(flag), GetSpellCastingSchoolForCharBaseclass(flag));
//      maxLevelBySchool[MagicUserSchool][index] += MaxSpellLevelForClassAndSchool(flag,data.GetCurrentLevel(flag),data.GetPrimeSpellCastingScore(flag), GetSpellCastingSchoolForCharBaseclass(flag));

//      for (int j=0;j<maxLevelBySchool[MagicUserSchool][index];j++)
//        maxSelectBySchool[MagicUserSchool][j][index] += 
//          data.MaxSpellsForLevel(flag, data.GetCurrentLevel(flag), j+1, data.GetPrimeSpellCastingScore(flag));
//    }
  }



  //FormatSpellListTitle("NO SPELLS AVAILABLE");
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::FillCastSpellListText
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_TEXT_LIST::FillCastSpellListText(CHARACTER &data, CASTING_ENVIRONMENT ce)
{
	Clear(); 
  m_CharData=&data;
  m_flags=CAST_FORMAT_FLAG|LEVEL_FLAG;  




  //spellClasses = SetClassBitsFromCharClass(data.GetAdjClass());  
  {
    //POSITION pos; // access ordered list
    int i, n;
    // should be at least one spell, 
    // and they should be sorted by now

    //for (pos=data.GetFirstSpellPosition(); pos!=NULL; data.GetNextSpell(pos))
    n = data.GetSpellCount();
    for (i=0; i<n; i++)
    {
      //if (data.GetSpellAt(pos).IsMemorized())
      if (data.PeekCharacterSpell(i)->IsMemorized())
      {
        // don't really need CurrSelect[] info when casting a spell that is already in
        // the char's spell book, but just to be consistent...
        //int level = data.GetSpellAt(pos).level;
        int level = data.PeekCharacterSpell(i)->level;
        SPELL_ID spellID;
        ASSERT ((level>=1)&&(level<=MAX_SPELL_LEVEL));
        if (level <= 0) level = 1;
        //SPELL_DATA *sdata = spellData.GetSpellData(data.GetSpellAt(pos).spell); 
        //SPELL_DATA *sdata = spellData.GetSpellData(data.GetSpellAt(pos).spellID); 
        spellID = data.PeekCharacterSpell(i)->spellID;
        const SPELL_DATA *pSpell = spellData.PeekSpell(spellID); 
        if (pSpell == NULL)
        {
          WriteDebugString("%s - Nonexistent spell in character spellbook\n", spellID);
          continue;
        };
        //if (sdata->NotInCombat() && (ce==CAST_ENV_COMBAT)) continue;
        //if (sdata->NotInCamp() && (ce==CAST_ENV_CAMP)) continue;
        //ASSERT( NumClassBits(sdata->schoolMask) == 1 );
        //if (pSpell->NotInCombat() && (ce==CAST_ENV_COMBAT)) continue;
        if (!pSpell->InCombat() && (ce==CAST_ENV_COMBAT)) continue;
        //if (pSpell->NotInCamp() && (ce==CAST_ENV_CAMP)) continue;
        if (!pSpell->InCamp() && (ce==CAST_ENV_CAMP)) continue;
        if (!pSpell->InCamp() && (ce==CAST_ENV_ADVENTURE)) continue;
        //ASSERT( NumClassBits(pSpellData->schoolMask) == 1 );

        //int spellSchool;
        SCHOOL_ID schoolID;
        //switch (sdata->schoolMask)



        /*
        switch (pSpellData->schoolMask)
        {
          case MagicUserFlag: spellSchool = MagicUserSchool; break;
          case ClericFlag:    spellSchool = ClericSchool;    break;
          default:            spellSchool = MagicUserSchool; break;
        };
        */
        schoolID = pSpell->schoolID;      


        //currSelectBySchool[spellSchool][level-1] += data.GetSpellAt(pos).selected;
//        currSelectBySchool[spellSchool][level-1] += data.PeekCharacterSpell(i)->selected;
        //Add((DWORD)pos);
        Add(i);
      }
    };
  };

  // don't really need this info when casting a spell that is already in
  // the char's spell book, but just to be consistent...

/*

  for (int i=0;i<NumBaseClass;i++)
  {
    WORD flag = (spellClasses & (0x0001 << i));
    if (flag)
    {
      int index = FlagToIndex(GetSpellCastingSchoolForCharBaseclass(flag));
#ifdef newMEMORIZE
      //maxLevelBySchool[MagicUserSchool][index] += CurrSpellLevel(flag,data.GetCurrentLevel(flag),data.GetPrimeSpellCastingScore(flag), GetSpellCastingSchoolForCharBaseclass(flag));
      maxLevelBySchool[MagicUserSchool][index] += MaxSpellLevelForClassAndSchool(flag,data.GetCurrentLevel(flag),data.GetPrimeSpellCastingScore(flag), GetSpellCastingSchoolForCharBaseclass(flag));
      for (int j=0;j<maxLevelBySchool[MagicUserSchool][index];j++)
        maxSelectBySchool[MagicUserSchool][j][index] += 
          data.MaxSpellsForLevel(flag, data.GetCurrentLevel(flag), j+1, data.GetPrimeSpellCastingScore(flag));
#else
      MaxLevel[index] += CurrSpellLevel(flag,data.GetCurrentLevel(flag),data.GetPrimeSpellCastingScore(flag), GetSpellCastingSchoolForCharBaseclass(flag));
      for (int j=0;j<MaxLevel[index];j++)
        MaxSelect[j][index] += 
          data.MaxSpellsForLevel(flag, data.GetCurrentLevel(flag), j+1, data.GetPrimeSpellCastingScore(flag));
#endif
    }
  }
*/




  FormatSpellListTitle("NO SPELLS AVAILABLE");
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::Have
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL SPELL_TEXT_LIST::Have(DWORD index)
BOOL SPELL_TEXT_LIST::Have(const SPELL_ID& spellID)
{
  //for (int i=0;i<spells.GetSize(); i++)
  for (int i=0;i<spellIDs.GetSize(); i++)
  {
    //if (spells[i] == index)
    if (spellIDs[i] == spellID)
      return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::ClearArbitraryText
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_TEXT_LIST::ClearArbitraryText()
{
  arbitraryText[0].Empty();
  arbitraryText[1].Empty();
  arbitraryText[2].Empty();
  arbitraryText[3].Empty();
  arbitraryText[4].Empty();
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::FillSpellEffectsListText
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_TEXT_LIST::FillSpellEffectsListText(CHARACTER &data, int flags)
{
	Clear(); 
  m_flags |= flags;
  m_CharData = &data;


  //spellClasses = SetClassBitsFromCharClass(data.GetAdjClass());

  POSITION pos = data.m_spellEffects.GetHeadPosition();
  while (pos != NULL)
  {
    int i;
    const ACTIVE_SPELL *aspell = activeSpellList.Get(data.m_spellEffects.PeekAt(pos)->parent);
    if (aspell != NULL)
    {      
      //SPELL_DATA *sdata = spellData.GetSpellData(aspell->spellKey);
      //SPELL_DATA *sdata = spellData.GetSpellData(aspell->spellID);
      const SPELL_DATA *sdata = spellData.PeekSpell(aspell->spellID);
      if (sdata != NULL)
      {
        SPELL_ID spellID;
        spellID = sdata->SpellID();
        //if (!Have(sdata->m_gsID.m_key))
        if (!Have(spellID))
          //Add(sdata->m_gsID.m_key);
          Add(spellID);
        if (this->m_flags & CHAR_VIEW_DISPLAY)
        {
          DisplayStatusPkt results;
          int oldCount;




          HOOK_PARAMETERS hookParameters;
          SCRIPT_CONTEXT scriptContext;
          ActorType actor;

          data.GetContext(&actor);
          
          SetCharContext(&actor);
          scriptContext.SetCharacterContext(&data);
          scriptContext.SetSpellContext(sdata);

          hookParameters[5] = "DISPLAY";






          oldCount = results.m_numString;
          sdata->RunSpellScripts(CHAR_DISPLAY_STATUS,
                                 ScriptCallback_CharDisplayStatus, 
                                 &results,
                                 "Character Display - Spell Effects");
          if (results.m_numString == oldCount)
          {  // If nothing added, use spell name and level.
            CString alevel;
            alevel.Format("%d", sdata->Level);
            results.AddString(alevel);
            results.AddString(sdata->Name);
          };
          for (i=0; i<results.m_numString; i+=2)
          {
            if (!results.m_pStrings[i]->IsEmpty()  || !results.m_pStrings[i+1]->IsEmpty())
            {
              spellEffects.Add(*results.m_pStrings[i]);
              spellEffects.Add(*results.m_pStrings[i+1]);
            };
          };
        }
      }
    }
    else
    {
      if (!data.m_spellEffects.PeekAt(pos)->ScriptEffectText().IsEmpty())
      {
        CString s;
        spellEffects.Add(s);
        s = data.m_spellEffects.PeekAt(pos)->ScriptEffectText();
        spellEffects.Add(s);
      };
    };
    data.m_spellEffects.GetNext(pos);
  };

  // OK.  We have finished adding the spell effects to the list.
  // Now we will go through the characters Special Abilities to 
  // see if any effects are waiting to be added.



  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    DisplayStatusPkt results;
    ActorType actor;
    int i;
    data.GetContext(&actor);
    SetCharContext(&actor);
    scriptContext.SetCharacterContext(&data);

    hookParameters[5] = "DISPLAY";


    data.RunCharacterScripts(CHAR_DISPLAY_STATUS, 
                             ScriptCallback_CharDisplayStatus, 
                            &results,
                             "Character Display - Spell Effects");

    for (i=0; i<results.m_numString; i+=2)
    {
      if (!results.m_pStrings[i]->IsEmpty()  || !results.m_pStrings[i+1]->IsEmpty())
      {
        spellEffects.Add(*results.m_pStrings[i]);
        spellEffects.Add(*results.m_pStrings[i+1]);
      };
    };
  };

/*
  //for (int i=0;i<NumBaseClass;i++)
  for (int i=0;i<baseclassData.GetCount();i++)
  {
    WORD flag = (spellClasses & (0x0001 << i));
    if (flag)
    {
      int index = FlagToIndex(GetSpellCastingSchoolForCharBaseclass(flag));
      //maxLevelBySchool[MagicUserSchool][index] += CurrSpellLevel(flag,data.GetCurrentLevel(flag),data.GetPrimeSpellCastingScore(flag), GetSpellCastingSchoolForCharBaseclass(flag));
      maxLevelBySchool[MagicUserSchool][index] += MaxSpellLevelForClassAndSchool(flag,data.GetCurrentLevel(flag),data.GetPrimeSpellCastingScore(flag), GetSpellCastingSchoolForCharBaseclass(flag));
      for (int j=0;j<maxLevelBySchool[MagicUserSchool][index];j++)
        maxSelectBySchool[MagicUserSchool][j][index] += 
          data.MaxSpellsForLevel(flag, data.GetCurrentLevel(flag), j+1, data.GetPrimeSpellCastingScore(flag));
    }
  }
*/ //Not Implemented(0x2a78c, false);
  FormatSpellListTitle("NO SPELL EFFECTS");
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::FillSpellListText
//
// PURPOSE: 
//
//*****************************************************************************
// PRS July 2009 void SPELL_TEXT_LIST::FillSpellListText(CHARACTER &cdata, spellBookType *sbook, UINT flags, costFactorType type)
void SPELL_TEXT_LIST::FillSpellListText(CHARACTER &cdata, UINT flags, costFactorType type)
{
	Clear();
  m_CharData = &cdata;
  m_flags = flags;



/*
  spellClasses = SetClassBitsFromCharClass(m_CharData->GetAdjClass());
  CostFactor = type;
  
  // PRS July 2009 POSITION p = sbook->GetHeadPosition(); // access ordered list
  {
    //POSITION p = cdata.GetFirstSpellPosition(); // access ordered list
    int i,n;
    n = cdata.GetSpellCount();
    // should be at least one spell, 
    // and they should be sorted by now
    //ASSERT( p != NULL );

    //while (p != NULL)
    for (i=0; i<n; i++)
    {
      do
      {
        if (flags & SHOW_SELECTED_FLAG)
        {
          // only display spells selected for use
          // PRS July 2009 if (sbook.list.GetAt(p).selected == 0)
          //if (cdata.GetSpellAt(p).selected == 0)
          if (cdata.PeekCharacterSpell(i)->selected == 0)
          {
            //spells.list.GetNext(p);
            //continue;
            //goto bypass_add;
            break;
          }
        }
        else if (flags & SHOW_NOT_SELECTED_FLAG)
        {
          // only display spells not selected for use
          // PRS July 2009 if (sbook.list.GetAt(p).selected > 0)
          //if (cdata.GetSpellAt(p).selected > 0)
          if (cdata.PeekCharacterSpell(i)->selected > 0)
          {
            //spells.list.GetNext(p);
            //continue;
            //goto bypass_add;
            break;
          }
        }

        if (flags & SHOW_MEMORIZED_FLAG)
        {
          // only display memorized spells 
          // PRS July 2009 if (sbook.list.GetAt(p).memorized == 0)
          if (cdata.PeekCharacterSpell(i)->memorized == 0)
          {
            //spells.list.GetNext(p);
            //continue;
            //goto bypass_add;
            break;
          }
        }
        else if (flags & SHOW_NOT_MEMORIZED_FLAG)
        {
          // only display unmemorized spells
          // PRS July 2009 if (sbook.list.GetAt(p).memorized > 0)
          //if (cdata.GetSpellAt(p).memorized > 0)
          if (cdata.PeekCharacterSpell(i)->memorized > 0)
          {
            //spells.list.GetNext(p);
            //continue;
            //goto bypass_add;
            break;
          }
        }

        // PRS July 2009 Add(sbook.list.GetAt(p).spell);
    
        //Add(cdata.GetSpellAt(p).spellID);
        Add(cdata.PeekCharacterSpell(i)->spellID);
        //bypass_add:
      } while (false);
      // PRS July 2009 sbook.list.GetNext(p);
      //cdata.GetNextSpell(p);
    };
  };

  // don't really need this info , but just to be consistent...
  for (int i=0;i<NumBaseClass;i++)
  {
    WORD flag = (spellClasses & (0x0001 << i));
    if (flag)
    {
      int index = FlagToIndex(GetSpellCastingSchoolForCharBaseclass(flag));
#ifdef newMEMORIZE
      //maxLevelBySchool[MagicUserSchool][index] += CurrSpellLevel(flag,m_CharData->GetCurrentLevel(flag),m_CharData->GetPrimeSpellCastingScore(flag), GetSpellCastingSchoolForCharBaseclass(flag));
      maxLevelBySchool[MagicUserSchool][index] += MaxSpellLevelForClassAndSchool(flag,m_CharData->GetCurrentLevel(flag),m_CharData->GetPrimeSpellCastingScore(flag), GetSpellCastingSchoolForCharBaseclass(flag));
      for (int j=0;j<maxLevelBySchool[MagicUserSchool][index];j++)
        maxSelectBySchool[MagicUserSchool][j][index] += 
          m_CharData->MaxSpellsForLevel(flag, m_CharData->GetCurrentLevel(flag), j+1, m_CharData->GetPrimeSpellCastingScore(flag));
#else
      MaxLevel[index] += CurrSpellLevel(flag,m_CharData->GetCurrentLevel(flag),m_CharData->GetPrimeSpellCastingScore(flag), GetSpellCastingSchoolForCharBaseclass(flag));
      for (int j=0;j<MaxLevel[index];j++)
        MaxSelect[j][index] += 
          m_CharData->MaxSpellsForLevel(flag, m_CharData->GetCurrentLevel(flag), j+1, m_CharData->GetPrimeSpellCastingScore(flag));
#endif
    }
  }
*/ /* Really */ NotImplemented(0xfd5fda1, false);



  FormatSpellListTitle();
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::FormatSpellListTitle
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_TEXT_LIST::FormatSpellListTitle(const char *EmptySpellTitle)
{
  const int width = 6;
  if (GetCount() <= 0)
  {
    if (EmptySpellTitle != NULL)
      title = EmptySpellTitle;
    else
      title = "NO SPELLS";
    return;
  }

	text[0] = '\0';

  if (UseMemorizeLayout())
  {
		sprintf(temp, "%-*s", width, "LEVEL");
		strcat(text, temp);
		sprintf(temp, "%-*s", width, "SEL");
		strcat(text, temp);
		sprintf(temp, "%-*s", width, "MEM");
		strcat(text, temp);
	  sprintf(temp, "%-*s", MAX_SPELL_NAME, "     SPELL");
	  strcat(text, temp);
  }
  else if (UseTempleCastLayout())
  {
		sprintf(temp, "%-*s", width, "LEVEL");
		strcat(text, temp);
		sprintf(temp,"%*s", width, "COST");
		strcat(text, temp);
	  sprintf(temp, "%-*s", MAX_SPELL_NAME, "     SPELL");
	  strcat(text, temp);
  }
  else if (UseCastLayout())
  {
		sprintf(temp, "%-*s", width, "LEVEL");
		strcat(text, temp);
		sprintf(temp, "%-*s", width, "MEM");
		strcat(text, temp);
	  sprintf(temp, "%-*s", MAX_SPELL_NAME, "     SPELL");
	  strcat(text, temp);
  }
  else if (UseNewSpellLevelLayout())
  {
		sprintf(temp, "%-*s", width, "LEVEL");
		strcat(text, temp);
		sprintf(temp, "%-*s", width, "SEL");
		strcat(text, temp);
	  sprintf(temp, "%-*s", MAX_SPELL_NAME, "     SPELL");
	  strcat(text, temp);
  }
  else
  {
	  if (UseLevel())
	  {
		  sprintf(temp, "%-*s", width, "LEVEL");
		  strcat(text, temp);
	  }
	  if (UseCost())
	  {
		  sprintf(temp,"%*s", width, "COST");
		  strcat(text, temp);
	  }
	  sprintf(temp, "%-*s", MAX_SPELL_NAME, "     SPELL");
	  strcat(text, temp);
  }
	title = text;
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::GetSpellMemorizeCount
//
// PURPOSE: 
//
//*****************************************************************************
int SPELL_TEXT_LIST::GetSpellMemorizeCount(int index)
{
  if (this->UseTempleCastLayout())
  {
    int idx;
    const CHARACTER_SPELL *pCharSpell;
    idx = *PeekInteger(index);
    //idx = integers[index];
    pCharSpell = m_CharData->PeekCharacterSpell(idx);
    return pCharSpell->memorized;
  };
  if (this->UseArbitraryTextLayout())
  {
    SPELL_ID spellID;
    int idx;
    const CHARACTER_SPELL *pCharSpell;
    spellID = spellIDs[index];
    idx = m_CharData->LocateCharacterSpell(spellID);
    pCharSpell = m_CharData->PeekCharacterSpell(index);
    return pCharSpell->memorized;
  }

  ASSERT( UsesSpellBookPos() );
  if (!UsesSpellBookPos()) return 0; //Temple, Cast, or Memorize layout.
  if (UseCastLayout())
  {
    return (m_CharData->PeekCharacterSpell(*PeekInteger(index))->memorized);
    //return (m_CharData->PeekCharacterSpell(integers[index])->memorized);
  };

    
  //return (m_CharData->GetSpellAt((POSITION)spells[index]).memorized);
  //if (memSum.GetMemorizeItemCount() == 0) return NULL;
  return memSum.PeekMemorizeItem(index)->numMemorized;
}


//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::ToggleSelected
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_TEXT_LIST::ToggleSelected(int index)
{
/*
  ASSERT(UsesSelectionArrays());
  //if (spells.GetSize()==0) return;
  if (spellIDs.GetSize()==0) return;
  isSelected[index] = !isSelected[index];

  int lvl,sindex;
  GetSpellLevelAndSchoolIndex(index, &lvl, &sindex);
  //int level = GetSpellLevel(index);
  //ASSERT(level >= 1);
  //ASSERT( (m_school>=0)&&(m_school<=1) ); 
#ifdef newMEMORIZE
  if (isSelected[index])
  {
    //(currSelectBySchool[sindex][lvl-1])++;
    selectCounts.m_currSelect[sindex][lvl-1]++;
  }
  else
  {
    //(currSelectBySchool[sindex][lvl-1])--;
    selectCounts.m_currSelect[sindex][lvl-1]--;
  };
#else
  if (isSelected[index])
    (CurrSelect[lvl-1][sindex])++;
  else
    (CurrSelect[lvl-1][sindex])--;
#endif
*/ /* Really */ NotImplemented(0x8f2a908, false);


}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::IsSelected
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_TEXT_LIST::IsSelected(int index)
{
  //if (spells.GetSize()==0) return FALSE;
  if (spellIDs.GetSize()==0) return FALSE;
  return isSelected[index];
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::CanSelect
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_TEXT_LIST::CanSelect(int index)
{


/*
  ASSERT( UseNewSpellLevelLayout() || UseMemorizeLayout() );
  //if (spells.GetSize()==0) return FALSE;
  if (spellIDs.GetSize()==0) return FALSE;
  //ASSERT(m_SpellLevelFilter >= 1);
  //ASSERT( (m_school>=0)&&(m_school<=1) ); 
  int lvl,schoolIndex;
  int i, max;
  GetSpellLevelAndSchoolIndex(index, &lvl, &schoolIndex);
#ifdef newMEMORIZE
  for (i=0, max=0; i< MAX_BASE_CLASSES; i++)
  {
    //max += maxSelectBySchool[schoolIndex][lvl-1][i];
    max += selectCounts.m_maxSelect[schoolIndex][lvl-1][i];
  };
  //return (currSelectBySchool[schoolIndex][lvl-1] < max);
  return selectCounts.m_currSelect[schoolIndex][lvl-1] < max;
#else
  return (CurrSelect[lvl-1][schoolIndex] < MaxSelect[lvl-1][schoolIndex]);
#endif
*/ /* Really */ NotImplemented(0x8f431c, false); return false;


}

// Added December 2010 - should eventually replace CanSelect
//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::CanSelectSpell
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_TEXT_LIST::CanSelectSpell(int index)
{

/*
  ASSERT( UseNewSpellLevelLayout() || UseMemorizeLayout() );
  //if (spells.GetSize()==0) return FALSE;
  if (spellIDs.GetSize()==0) return FALSE;
  int baseclass, spellLevel, schoolIndex, maxSelect;
  GetSpellLevelAndSchoolIndex(index, &spellLevel, &schoolIndex);
  maxSelect = 0;
  for (baseclass=0; baseclass<MAX_BASE_CLASSES; baseclass++)
  {
    //maxSelect += maxSelectBySchool[schoolIndex][spellLevel-1][baseclass];
    maxSelect += selectCounts.m_maxSelect[schoolIndex][spellLevel-1][baseclass];
  };
  //return (currSelectBySchool[schoolIndex][spellLevel-1] < maxSelect);
  return selectCounts.m_currSelect[schoolIndex][spellLevel-1] < maxSelect;
*/ //Not Implemented(0xaf51a3, false); return FALSE;
  return TRUE;


}


//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::CanSelectAnything
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SPELL_TEXT_LIST::CanSelectAnything()
{
  ASSERT(UseNewSpellLevelLayout());
  //if (spells.GetSize()==0) return FALSE;
  if (spellIDs.GetSize()==0) return FALSE;

  BOOL found = FALSE;
  for (int lvl=0;lvl<MAX_SPELL_LEVEL;lvl++)
  {
/*
    for (int index=0;index<NumBaseClass;index++)
    {
#ifdef newMEMORIZE
      int max;

      //max = maxSelectBySchool[MagicUserSchool][lvl][index] + maxSelectBySchool[ClericSchool][lvl][index]; 
      max = selectCounts.m_maxSelect[MagicUserSchool][lvl][index] + selectCounts.m_maxSelect[ClericSchool][lvl][index]; 
      //if (currSelectBySchool[MagicUserSchool][lvl] < max)
      if (selectCounts.m_currSelect[MagicUserSchool][lvl] < max)


#else
      if (CurrSelect[lvl][index] < MaxSelect[lvl][index])
#endif
        found=TRUE;
    }
*/ /* Really */ NotImplemented(0x5fda901, false);

  
  }
  return found;
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::GetSpellSelectedCount
//
// PURPOSE: 
//
//*****************************************************************************
int SPELL_TEXT_LIST::GetSpellSelectedCount(int index)
{
  if (this->UseTempleCastLayout())
  {
    int idx;
    const CHARACTER_SPELL *pCharSpell;
    idx = *PeekInteger(index);
    //idx = integers[index];
    pCharSpell = m_CharData->PeekCharacterSpell(idx);
    return pCharSpell->selected;
  };
  ASSERT( !UseNewSpellLevelLayout() );
  if (UseNewSpellLevelLayout()) return 0;
  if (UseCastLayout())
  {
    return (m_CharData->PeekCharacterSpell(*PeekInteger(index))->selected);
    //return (m_CharData->PeekCharacterSpell(integers[index])->selected);
  };

  //return (m_CharData->GetSpellAt((POSITION)spells[index]).selected);
  //return (m_CharData->GetSpellAt(POSITIONs[index]).selected);
  //if (memSum.GetMemorizeItemCount() == 0) return 0;
  return memSum.PeekMemorizeItem(index)->numSelected;
  //return memSum.spells[index].numSelected;
}


int SPELL_TEXT_LIST::LocateSelectCounts(const SCHOOL_ID& schoolID) const
{
  int i, n;
  n = selectCounts.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekSelectCounts(i)->m_schoolID == schoolID) return i;
  };
  return -1;
}

int SPELL_TEXT_LIST::CurrSelect(const SCHOOL_ID& schoolID, int spellLevel) const
{
  int i;
  i = this->LocateSelectCounts(schoolID);
  if (i < 0) return 0;
  return PeekSelectCounts(i)->CurrSelect(spellLevel);
}


//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::IncSpellSelectedCount
//
// PURPOSE: 
//
//*****************************************************************************
int SPELL_TEXT_LIST::IncreaseSpellSelectedCount(int index, int num)
{
  if (UseMemorizeLayout())
  {
    int i, n;
    MEMORIZE_ITEM *pMemorizeItem;
    const SPELL_DATA *pSpell;
    pMemorizeItem = memSum.GetMemorizeItem(index);
    pMemorizeItem->numSelected += num;
    // Now we need to decrease the available counts
    // for all spells of this school and level.
    n = memSum.GetMemorizeItemCount();
    pSpell = spellData.PeekSpell(pMemorizeItem->spellDataIndex);
    for (i=0; i<n; i++)
    {
      MEMORIZE_ITEM *pMI;
      const SPELL_DATA *pS;
      pMI = memSum.GetMemorizeItem(i);
      pS = spellData.PeekSpell(pMI->spellDataIndex);
      if (pS->schoolID != pSpell->schoolID) continue;
      if (pS->Level    != pSpell->Level)    continue;
      pMI->available -= num;
    };
    return 0;
  };

/*
  ASSERT( !UseNewSpellLevelLayout() );
  if (UseNewSpellLevelLayout()) return -1;
  ASSERT(UseMemorizeLayout());
  int total = 
    //m_CharData->GetSpellAt((POSITION)spells[index]).selected + 1;
    m_CharData->PeekCharacterSpell(integers[index])->selected + 1;
  //m_CharData->GetSpellAt((POSITION)spells[index]).selected = total;    
  m_CharData->GetCharacterSpell(integers[index])->selected = total;    

  int spellLevel,schoolIndex;
  GetSpellLevelAndSchoolIndex(index, &spellLevel, &schoolIndex);
  //currSelectBySchool[schoolIndex][spellLevel-1]++;
  selectCounts.m_currSelect[schoolIndex][spellLevel-1]++;
  return total;
*/ /* Really */ NotImplemented(0xbf4a211, false); return 0;

}


int SPELL_TEXT_LIST::IncreaseSpellMemorizedCount(int index, int num)
{
  if (UseMemorizeLayout())
  {
    MEMORIZE_ITEM *pMemorizeItem;
    pMemorizeItem = memSum.GetMemorizeItem(index);
    pMemorizeItem->numMemorized += num;
    // Now we need to decrease the available counts
    // for all spells of this school and level.
    return pMemorizeItem->numMemorized;
  };
  /* Really */ NotImplemented(0xc45a, false);
  return 0;
}




int SPELL_TEXT_LIST::MaxSelect(const SCHOOL_ID& schoolID, int spellLevel) const
{
  // Add up for all baseclasses.
  /* Really */ NotImplemented(0x118ab8, false);
  return 0;
}

int SPELL_TEXT_LIST::IncCurrSelectCount(const SCHOOL_ID& schoolID, int spellLevel) 
{
  /* Really */ NotImplemented(0xff41ca, false);
  return 0;
}



/*
//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::GetSpellLevelAndSchoolIndex
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_TEXT_LIST::GetSpellLevelAndSchoolIndex(int index, int *lvl, int *sindex)
{
  lvl=1;
  sindex=0;
  //GLOBAL_SPELL_ID dbkey = GetSpellDatabaseKey(index);
  SPELL_ID spellID = GetSpellDatabaseID(index);
  SPELL_DATA *sdata = spellData.GetSpellData(spellID);
  ASSERT(sdata!=NULL);
  if (sdata == NULL) return;
  *lvl = sdata->Level;
  *sindex = FlagToIndex(GetSpellCastingSchoolForCharBaseclass(sdata->schoolMask));
}
*/
//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::SpellLevelAndSchoolIndex
//
// PURPOSE: 
//
//*****************************************************************************
void SPELL_TEXT_LIST::GetSpellLevelAndSchoolIndex(int index, int *spellLevel, int *spellSchool)
{

/*
  *spellLevel=1;
  *spellSchool=MagicUserSchool;
  //GLOBAL_SPELL_ID dbkey = GetSpellDatabaseKey(index);
  SPELL_ID spellID = GetSpellDatabaseID(index);
  //SPELL_DATA *sdata = spellData.GetSpellData(dbkey);
  const SPELL_DATA *sdata = spellData.PeekSpell(spellID);
  ASSERT(sdata!=NULL);
  if (sdata == NULL) return;
  *spellLevel = sdata->Level;
  *spellSchool = FlagToIndex(GetSpellCastingSchoolForCharBaseclass(sdata->schoolMask));
*/ 
  
  
  /* Really */ NotImplemented(0xfdcd54, false);
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::GetSpellLevel
//
// PURPOSE: 
//
//*****************************************************************************
int SPELL_TEXT_LIST::GetSpellLevel(int index)
{
  int spellLevel;
  spellLevel=1;
  //GLOBAL_SPELL_ID dbkey = GetSpellDatabaseKey(index);
  //SPELL_DATA *sdata = spellData.GetSpellData(dbkey);
  SPELL_ID spellID = GetSpellDatabaseID(index);
  const SPELL_DATA *sdata = spellData.PeekSpell(spellID);
  ASSERT(sdata!=NULL);
  if (sdata != NULL)
  {
    spellLevel = sdata->Level;
  };
  return spellLevel;
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::GetSpellDatabaseKey
//
// PURPOSE: 
//
//*****************************************************************************
//GLOBAL_SPELL_ID SPELL_TEXT_LIST::GetSpellDatabaseKey(int index)
SPELL_ID SPELL_TEXT_LIST::GetSpellDatabaseID(int index)
{
  //if (UseNewSpellLevelLayout()) return GLOBAL_SPELL_ID(spells[index]);
  if (UseNewSpellLevelLayout()) return spellIDs[index];
  //return (m_CharData->GetSpellAt((POSITION)spells[index]).spell);
  return (m_CharData->PeekCharacterSpell(*PeekInteger(index))->spellID);
  //return (m_CharData->PeekCharacterSpell(integers[index])->spellID);
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::GetSpellBookKey
//
// PURPOSE: 
//
//*****************************************************************************
//GLOBAL_SPELL_ID SPELL_TEXT_LIST::GetSpellBookKey(int index)
SPELL_ID SPELL_TEXT_LIST::GetSpellBookID(int index)
{
  ASSERT( !UseNewSpellLevelLayout() );
  if (UseNewSpellLevelLayout()) return SPELL_ID();
  //return (m_CharData->GetSpellKeyAt((POSITION)spells[index]));
  return (m_CharData->PeekCharacterSpell(*PeekInteger(index))->spellID);
  //return (m_CharData->PeekCharacterSpell(integers[index])->spellID);
}

//*****************************************************************************
//    NAME: SPELL_TEXT_LIST::GetSpellSchool
//
// PURPOSE: 
//
//*****************************************************************************
//int SPELL_TEXT_LIST::GetSpellSchool(int index)
SCHOOL_ID SPELL_TEXT_LIST::GetSpellSchool(int index)
{  
  //GLOBAL_SPELL_ID dbkey = GetSpellDatabaseKey(index);
  SPELL_ID spellID = GetSpellDatabaseID(index);
  //SPELL_DATA *sdata = spellData.GetSpellData(dbkey);
  const SPELL_DATA *sdata = spellData.PeekSpell(spellID);
  ASSERT(sdata!=NULL);
  if (sdata == NULL) return SCHOOL_ID();
  //return (GetSpellCastingSchoolForCharBaseclass(sdata->schoolMask));
  return sdata->schoolID;
}

//*****************************************************************************
//    NAME: *SPELL_TEXT_LIST::GetSpellLevelText
//
// PURPOSE: 
//
//*****************************************************************************
char *SPELL_TEXT_LIST::GetSpellLevelText(int index)
{
  //if (spells.GetSize() == 0) return NULL;
  if (UseMemorizeLayout())
  {
    //sprintf(text, "%i", m_CharData->GetSpellAt((POSITION)spells[index]).level);
    const SPELL_DATA *pSpell;
    pSpell = spellData.PeekSpell(memSum.PeekMemorizeItem(index)->spellDataIndex);
    sprintf(text, "%i", pSpell->Level);
    return text;
  };
  if (UseTempleCastLayout())
  {
    if (spellIDs.GetSize() == 0) return NULL;
    //sprintf(text, "%i", m_CharData->GetSpellAt((POSITION)spells[index]).level);
    sprintf(text, "%i", m_CharData->GetCharacterSpell(*PeekInteger(index))->level);
    //sprintf(text, "%i", m_CharData->GetCharacterSpell(integers[index])->level);
    return text;
  };
  if (UseCastLayout())
  {
    if (integers.GetSize() == 0) return NULL;
    //sprintf(text, "%i", m_CharData->GetSpellAt((POSITION)spells[index]).level);
    sprintf(text, "%i", m_CharData->GetCharacterSpell(*PeekInteger(index))->level);
    //sprintf(text, "%i", m_CharData->GetCharacterSpell(integers[index])->level);
    return text;
  };
  if (UseNewSpellLevelLayout())
  {
    if (spellIDs.GetSize() == 0) return NULL;
    //sprintf(text, "%i", spellData.GetSpellLevel(spells[index]));
    sprintf(text, "%i", spellData.GetSpellLevel(spellIDs[index]));
    return text;
  };
  if (UseLevel())
  {
    if (spellIDs.GetSize() == 0) return NULL;
    //sprintf(text, "%i", spellData.GetSpellLevel(spells[index]));
    sprintf(text, "%i", spellData.GetSpellLevel(spellIDs[index]));
    return text;
  };
  return NULL;
}


char *SPELL_TEXT_LIST::GetSpellEffectLevelText(int index)
{
  sprintf(text, "%s", (LPCSTR)spellEffects[2*index]);
  return text;
}
//*****************************************************************************
//    NAME: *SPELL_TEXT_LIST::GetSpellSelectedText
//
// PURPOSE: 
//
//*****************************************************************************
char *SPELL_TEXT_LIST::GetSpellSelectedText(int index)
{
  //if (spells.GetSize() == 0) return NULL;
  if (UseMemorizeLayout())
  {
    //sprintf(text, "%i", m_CharData->GetSpellAt((POSITION)spells[index]).selected);
    if (memSum.GetMemorizeItemCount() == 0) return NULL;
    sprintf(text, "%i", memSum.PeekMemorizeItem(index)->numSelected);
    //sprintf(text, "%i", memSum.spells[index].numSelected);
  }
  else if (UseNewSpellLevelLayout())
  {
    //int lvl,sindex;
    //GetSpellLevelAndSchoolIndex(index, lvl, sindex);
    if (spellIDs.GetSize() == 0) return NULL;
    sprintf(text, "%s", (isSelected[index]?"Yes":"No"));
    //sprintf(text, "%s", (CurrSelect[lvl-1][sindex]?"Yes":"No"));
  }
  else
    return NULL;
  return text;
}
/*
int SPELL_TEXT_LIST::GetSpellSelectedCount(int index)
{
  //if (spells.GetSize() == 0) return NULL;
  if (UseMemorizeLayout())
  {
    //sprintf(text, "%i", m_CharData->GetSpellAt((POSITION)spells[index]).selected);
    //if (memSum.GetMemorizeItemCount() == 0) return 0;
    return memSum.spells[index].numSelected);
  }
  else
    return 0;
}
*/
//*****************************************************************************
//    NAME: *SPELL_TEXT_LIST::GetSpellMemorizedText
//
// PURPOSE: 
//
//*****************************************************************************
char *SPELL_TEXT_LIST::GetSpellMemorizedText(int index)
{
  //if (spells.GetSize() == 0) return NULL;
  //if (spellIDs.GetSize() == 0) return NULL;
  if (UseMemorizeLayout())
  {
    //sprintf(text, "%i", m_CharData->GetSpellAt((POSITION)spells[index]).memorized);
    if (memSum.GetMemorizeItemCount() == 0) return NULL;
    sprintf(text, "%i", memSum.PeekMemorizeItem(index)->numMemorized);
  }
  else if (UseCastLayout())
  {
    //sprintf(text, "%i", m_CharData->GetSpellAt((POSITION)spells[index]).memorized);
    if (integers.GetSize() == 0) return NULL;
    sprintf(text, "%i", m_CharData->PeekCharacterSpell(*PeekInteger(index))->memorized);
    //sprintf(text, "%i", m_CharData->PeekCharacterSpell(integers[index])->memorized);
  }
  else
    return NULL;
  return text;
}
/*
int SPELL_TEXT_LIST::GetSpellMemorizedCount(int index)
{
  if (UseMemorizeLayout())
  {
    //if (memSum.GetMemorizeItemCount() == 0) return NULL;
    return memSum.PeekMemorizeItem(index)->numMemorized);
  }
  else
    return 0;
}
*/
char *SPELL_TEXT_LIST::GetSpellAvailableText(int index)
{
  //if (spells.GetSize() == 0) return NULL;
  //if (spellIDs.GetSize() == 0) return NULL;
  if (UseMemorizeLayout())
  {
    //sprintf(text, "%i", m_CharData->GetSpellAt((POSITION)spells[index]).memorized);
    if (memSum.GetMemorizeItemCount() == 0) return NULL;
    sprintf(text, "%i", memSum.PeekMemorizeItem(index)->available);
  }
  else
    return NULL;
  return text;
}



char *SPELL_TEXT_LIST::GetSpellSchoolText(int index)
{
  //if (spells.GetSize() == 0) return NULL;
  //if (spellIDs.GetSize() == 0) return NULL;
  if (UseMemorizeLayout())
  {
    const SPELL_DATA *pSpell;
    if (memSum.GetMemorizeItemCount() == 0) return NULL;
    pSpell = spellData.PeekSpell(memSum.PeekMemorizeItem(index)->spellDataIndex);
    sprintf(text,"%s", (LPCSTR)pSpell->schoolID);
  }
    //sprintf(text, "%i", m_CharData->GetSpellAt((POSITION)spells[index]).memorized);
    //sprintf(text, "%i", m_CharData->PeekCharacterSpell(integers[index])->choolID());
  else if (UseCastLayout())
  {
    if (integers.GetSize() == 0) return NULL;
    sprintf(text,"??");
    //sprintf(text, "%i", m_CharData->GetSpellAt((POSITION)spells[index]).memorized);
    //sprintf(text, "%i", m_CharData->PeekCharacterSpell(integers[index])->SchoolID());
  }
  else
    return NULL;
  return text;
}


//*****************************************************************************
//    NAME: *SPELL_TEXT_LIST::GetSpellCostText
//
// PURPOSE: 
//
//*****************************************************************************
char *SPELL_TEXT_LIST::GetSpellCostText(int index)
{
  //if (spells.GetSize() == 0) return NULL;
  if (spellIDs.GetSize() == 0) return NULL;
  if (!UseCost())	return NULL;
  //sprintf(text,"%i", ApplyCostFactor(CostFactor, spellData.GetSpellCastCost(spells[index])));
  sprintf(text,"%i", ApplyCostFactor(CostFactor, spellData.GetSpellCastCost(spellIDs[index])));
  return text;
}
//*****************************************************************************
//    NAME: *SPELL_TEXT_LIST::GetSpellNameText
//
// PURPOSE: 
//
//*****************************************************************************
char *SPELL_TEXT_LIST::GetSpellNameText(int index)
{
  //if (spells.GetSize() == 0) return NULL;
  if (UseMemorizeLayout())
    //sprintf(text,"%s", spellData.GetSpellName(m_CharData->GetSpellAt((POSITION)spells[index]).spell));
  {
    if (memSum.GetMemorizeItemCount() == 0) return NULL;
    sprintf(text,"%s", (LPCSTR)spellData.PeekSpell(memSum.PeekMemorizeItem(index)->spellDataIndex)->Name);
    return text;
  };
  if (UseTempleCastLayout())
  {
    //sprintf(text,"%s", spellData.GetSpellName(m_CharData->GetSpellAt((POSITION)spells[index]).spell));
    if (integers.GetSize() == 0) return NULL;
    sprintf(text,"%s", (LPCSTR)spellData.GetSpellName(m_CharData->PeekCharacterSpell(*PeekInteger(index))->spellID));
    //sprintf(text,"%s", spellData.GetSpellName(m_CharData->PeekCharacterSpell(integers[index])->spellID));
    return text;
  };
  if (UseCastLayout())
  {
    //sprintf(text,"%s", spellData.GetSpellName(m_CharData->GetSpellAt((POSITION)spells[index]).spell));
    if (integers.GetSize() == 0) return NULL;
    sprintf(text,"%s", (LPCSTR)spellData.GetSpellName(m_CharData->PeekCharacterSpell(*PeekInteger(index))->spellID));
    //sprintf(text,"%s", spellData.GetSpellName(m_CharData->PeekCharacterSpell(integers[index])->spellID));
    return text;
  };
  if (UseNewSpellLevelLayout())
  {
    //sprintf(text,"%s", spellData.GetSpellName(spells[index]));
    if (spellIDs.GetSize() == 0) return NULL;
    sprintf(text,"%s", (LPCSTR)spellData.GetSpellName(spellIDs[index]));
    return text;
  };
  {
    //sprintf(text,"%s", spellData.GetSpellName(spells[index]));
    if (spellIDs.GetSize() == 0) return NULL;
    sprintf(text,"%s", (LPCSTR)spellData.GetSpellName(spellIDs[index]));
    return text;
  };
}

char *SPELL_TEXT_LIST::GetSpellEffectText(int index)
{
  sprintf(text, "%s", (LPCSTR)spellEffects[2*index+1]);
  return text;
}


int SPELL_TEXT_LIST::GetSpellIndex(const SPELL_ID& spellID)
{
  int i, num;
  const MEMORIZE_ITEM *pMemItem;
  num = GetCount();
  for (i=0; i<num; i++)
  {
     pMemItem = memSum.PeekMemorizeItem(i);
    //if (spellName == GetSpellNameText(i)) return i;
    if (spellData.PeekSpell(pMemItem->spellDataIndex)->SpellID() == spellID) return i;
  };
  return -1;
}

//*****************************************************************************
//    NAME: *SPELL_TEXT_LIST::GetSpellText
//
// PURPOSE: 
//
//*****************************************************************************
char *SPELL_TEXT_LIST::GetSpellText(int index)
{
  //if (spells.GetSize() == 0) return NULL;
  if (spellIDs.GetSize() == 0) return NULL;

  const int width = 7;  
  temp[0] = '\0';
  text[0] = '\0';

  if (UseMemorizeLayout())
  {
    // level
    //sprintf(temp, "%*i", width, m_CharData->GetSpellAt((POSITION)spells[index]).level);
    sprintf(temp, "%*i", width, m_CharData->GetCharacterSpell(*PeekInteger(index))->level);
    //sprintf(temp, "%*i", width, m_CharData->GetCharacterSpell(integers[index])->level);
		strcat(text, temp);
    // selected
    //sprintf(temp, "%*i", width, m_CharData->GetSpellAt((POSITION)spells[index]).selected);
    sprintf(temp, "%*i", width, m_CharData->PeekCharacterSpell(*PeekInteger(index))->selected);
    //sprintf(temp, "%*i", width, m_CharData->PeekCharacterSpell(integers[index])->selected);
		strcat(text, temp);
    // memorized
    //sprintf(temp, "%*i", width, m_CharData->GetSpellAt((POSITION)spells[index]).memorized);
    sprintf(temp, "%*i", width, m_CharData->PeekCharacterSpell(*PeekInteger(index))->memorized);
    //sprintf(temp, "%*i", width, m_CharData->PeekCharacterSpell(integers[index])->memorized);
		strcat(text, temp);
    // name
    //sprintf(temp,"     %-.*s", MAX_SPELL_NAME, spellData.GetSpellName(m_CharData->GetSpellAt((POSITION)spells[index]).spell));
    sprintf(temp,"     %-.*s", MAX_SPELL_NAME, (LPCSTR)spellData.GetSpellName(m_CharData->PeekCharacterSpell(*PeekInteger(index))->spellID));
    //sprintf(temp,"     %-.*s", MAX_SPELL_NAME, spellData.GetSpellName(m_CharData->PeekCharacterSpell(integers[index])->spellID));
	  strcat(text, temp);
  }
  else if (UseTempleCastLayout())
  {
    // level
    //sprintf(temp, "%*i", width, m_CharData->GetSpellAt((POSITION)spells[index]).level);
    sprintf(temp, "%*i", width, m_CharData->PeekCharacterSpell(*PeekInteger(index))->level);
    //sprintf(temp, "%*i", width, m_CharData->PeekCharacterSpell(integers[index])->level);
		strcat(text, temp);
    // cost
    //DWORD cost = ApplyCostFactor(CostFactor, spellData.GetSpellCastCost(spells[index]));
    DWORD cost = ApplyCostFactor(CostFactor, spellData.GetSpellCastCost(m_CharData->PeekCharacterSpell(*PeekInteger(index))->spellID));
    //DWORD cost = ApplyCostFactor(CostFactor, spellData.GetSpellCastCost(m_CharData->PeekCharacterSpell(integers[index])->spellID));
    sprintf(temp,"%*i", width, cost);
		strcat(text, temp);
    // name
    //sprintf(temp,"     %-.*s", MAX_SPELL_NAME, spellData.GetSpellName(m_CharData->GetSpellAt((POSITION)spells[index]).spell));
    sprintf(temp,"     %-.*s", MAX_SPELL_NAME, (LPCSTR)spellData.GetSpellName(m_CharData->PeekCharacterSpell(*PeekInteger(index))->spellID));
    //sprintf(temp,"     %-.*s", MAX_SPELL_NAME, spellData.GetSpellName(m_CharData->PeekCharacterSpell(integers[index])->spellID));
	  strcat(text, temp);
  }
  else if (UseCastLayout())
  {
    // level
    //sprintf(temp, "%*i", width, m_CharData->GetSpellAt((POSITION)spells[index]).level);
    sprintf(temp, "%*i", width, m_CharData->PeekCharacterSpell(*PeekInteger(index))->level);
    //sprintf(temp, "%*i", width, m_CharData->PeekCharacterSpell(integers[index])->level);
		strcat(text, temp);
    // qty memorized
    //sprintf(temp, "%*i", width, m_CharData->GetSpellAt((POSITION)spells[index]).memorized);
    sprintf(temp, "%*i", width, m_CharData->PeekCharacterSpell(*PeekInteger(index))->memorized);
		strcat(text, temp);
    // name
    //sprintf(temp,"     %-.*s", MAX_SPELL_NAME, spellData.GetSpellName(m_CharData->GetSpellAt((POSITION)spells[index]).spell));
    sprintf(temp,"     %-.*s", MAX_SPELL_NAME, (LPCSTR)spellData.GetSpellName(m_CharData->PeekCharacterSpell(*PeekInteger(index))->spellID));
    //sprintf(temp,"     %-.*s", MAX_SPELL_NAME, spellData.GetSpellName(m_CharData->PeekCharacterSpell(integers[index])->spellID));
	  strcat(text, temp);
  }
  else if (UseNewSpellLevelLayout())
  {
    //int lvl,sindex;
    //GetSpellLevelAndSchoolIndex(index, lvl, sindex);

    // level
    //sprintf(temp, "%*i", width, spellData.GetSpellLevel(spells[index]));
    sprintf(temp, "%*i", width, spellData.GetSpellLevel(spellIDs[index]));
		strcat(text, temp);
    // selected    
    //sprintf(temp, "%*s", width, (CurrSelect[lvl-1][sindex]?"Yes":"No"));
    sprintf(temp, "%*s", width, (isSelected[index]?"Yes":"No"));
		strcat(text, temp);
    // name
    //sprintf(temp,"     %-.*s", MAX_SPELL_NAME, spellData.GetSpellName(spells[index]));
    sprintf(temp,"     %-.*s", MAX_SPELL_NAME, (LPCSTR)spellData.GetSpellName(spellIDs[index]));
	  strcat(text, temp);
  }
  else
  {
    if (UseLevel())
	  {
      //sprintf(temp, "%*i", width, spellData.GetSpellLevel(spells[index]));
      sprintf(temp, "%*i", width, spellData.GetSpellLevel(spellIDs[index]));
		  strcat(text, temp);
	  }
	  if (UseCost())	
    {   
      //DWORD cost = ApplyCostFactor(CostFactor, spellData.GetSpellCastCost(spells[index]));
      DWORD cost = ApplyCostFactor(CostFactor, spellData.GetSpellCastCost(spellIDs[index]));
      sprintf(temp,"%*i", width, cost);
		  strcat(text, temp);
	  }
    //sprintf(temp,"     %-.*s", MAX_SPELL_NAME, spellData.GetSpellName(spells[index]));
    sprintf(temp,"     %-.*s", MAX_SPELL_NAME, (LPCSTR)spellData.GetSpellName(spellIDs[index]));
	  strcat(text, temp);
  }

  return text;
}

int SPELL_TEXT_LIST::MaxSelectByLevel(int level)
{

/*
  int school, result, baseclass;
  result = 0;
  for (school=0; school<MAX_SPELL_SCHOOL; school++)
  {
    for (baseclass=0; baseclass<MAX_BASE_CLASSES; baseclass++)
    {
      //result += maxSelectBySchool[school][level-1][baseclass];
      result += selectCounts.m_maxSelect[school][level-1][baseclass];
    };
  };
  return result;
*/ /* Really */ NotImplemented(0xcf7a21, false); return 0;


}


int SPELL_TEXT_LIST::GetCount() const 
{ 
  if (UseMemorizeLayout())
  {
    return GetMemorizeCount();
  };
  if (UseRaceLayout())
  {
    return GetRaceCount();
  };
  if (UseClassLayout())
  {
    return GetClassCount();
  };
  if (UseTempleCastLayout())
  {
    return integers.GetSize();
  };
  if (UseCastLayout())
  {
    return integers.GetSize();
  };
  return spellIDs.GetSize(); 
};


int SPELL_TEXT_LIST::MaxSelectBySchoolAndLevel(int school, int level)
{



/*
  int result, baseclass;
  result = 0;
  for (baseclass=0; baseclass<MAX_BASE_CLASSES; baseclass++)
  {
    //result += maxSelectBySchool[school][level-1][baseclass];
    result += selectCounts.m_maxSelect[school][level-1][baseclass];
  };
  return result;
*/ /* Really */ NotImplemented(0xdf6a78, false); return 0;


}

struct TEMP_SPELL_DATA
{
  CString name;
  CString school;
  int level;
  int selected;
  int memorized;
  bool operator > (const TEMP_SPELL_DATA& s) const;
};

bool TEMP_SPELL_DATA::operator >(const TEMP_SPELL_DATA& s) const
{
  if (school > s.school) return true;
  if (school < s.school) return false;
  if (level > s.level) return true;
  if (level < s.level) return false;
  return false;
};

// Rebuilt 20160907 PRS /* Deprecated
CString GetSpellbook(CHARACTER *pChar, CString delim)
{
  const CHARACTER_SPELL *pCharSpell;
  char *defaultDelim = "!@#$";
  char delimiters[4];
  CString result;
  const SPELL_DATA *pSpell;
  int spellCount, len, numSpell;
  TEMP_SPELL_DATA *tempSpells;
  SCHOOL_ID curSchool, prevSchool;
//   prevSchool = 12;
  len = delim.GetLength();
  for (int i=0; i<4; i++)
  {
    if (len>i) delimiters[i] = delim[i];
    else delimiters[i] = defaultDelim[i];
  };
  spellCount = pChar->GetSpellCount();
  tempSpells = new TEMP_SPELL_DATA[spellCount];
  numSpell = 0;
  for (int i=0; i<spellCount; i++)
  {
    pCharSpell = pChar->PeekCharacterSpell(i);
    pSpell = spellData.PeekSpell(pCharSpell->spellID);
    if (pSpell != NULL)
    {
      tempSpells[numSpell].name   = pCharSpell->spellID;
      tempSpells[numSpell].school = pSpell->schoolID;
      tempSpells[numSpell].level  = pCharSpell->level;
      tempSpells[numSpell].selected  = pCharSpell->selected;
      tempSpells[numSpell].memorized  = pCharSpell->memorized;
      numSpell++;
    };
  };
   // Let us sort the spells
  {
    int i, j, k, n;
    TEMP_SPELL_DATA temp;
    for (n=numSpell/2; n>0; n/=2)
    {
      for (i=0; i<n; i++)
      {
        for (j=i; j<numSpell-n; j+=n)
        {
          for (k=j; k>=0; k-=n)
          {
            if (!(tempSpells[k] > tempSpells[k+n])) break;
            temp = tempSpells[k];
            tempSpells[k] = tempSpells[k+n];
            tempSpells[k+n] = temp;
          };
        };
      };
    };
  };
   
  prevSchool = "";
//   prevLevel = -99999;
  for (int i=0; i<numSpell; i++)
  {
    int prevLevel;
    CString ascii;
    if (tempSpells[i].school != prevSchool)
    {
      result += delimiters[0];
      result += delimiters[1];
      prevSchool = tempSpells[i].school;
      result += prevSchool;
      prevLevel = -9999;
    };
    if (tempSpells[i].level != prevLevel)
    {
      prevLevel = tempSpells[i].level;
      ascii.Format("%d", prevLevel);
      result += delimiters[1];
      result += delimiters[2];
      result += ascii;
    };
    result += delimiters[2];
    result += delimiters[3];
    result += tempSpells[i].name;
    result += delimiters[3];
    ascii.Format("%d", tempSpells[i].selected);
    result += ascii;
    result += delimiters[3];
    ascii.Format("%d", tempSpells[i].memorized);
    result += ascii;
  };
  delete[] tempSpells;
  return result;
};


CString SelectSpell(CHARACTER *pCharacter, const SPELL_ID& spellID)
{
  int  n;
  CString result;
  CHARACTER_SPELL *pCharSpell;
  /*   PRS 20160908
  //if (pCharacter->SpellsKnown() > 0) pCharacter->SortSpells();
  spellListText.FillMemorizeSpellListText(pCharacter);
  i = spellListText.GetSpellIndex(spellID);
  if (i < 0) return "";
  if (!spellListText.CanSelectSpell(i)) return "";
  n = spellListText.IncreaseSpellSelectedCount(i);
  result.Format("%d",n);
  return result;
  */
  n = pCharacter->GetSpellCount();
  for (int i=0; i<n; i++)
  {
    pCharSpell = pCharacter->GetCharacterSpell(i);
    if (pCharSpell->spellID == spellID)
    {
      pCharSpell->selected++;
      return "1";
    };
  };
  return "";
}


void Memorize(CHARACTER *pChar)
{
  if (pChar == NULL) return;
  if (pChar->CanCastSpells())
  {
    if (pChar->SpellsKnown() > 0)
    {
      // add time delta to all spells being memorized
      // returns TRUE if a spell was memorized
      if (pChar->IncAllMemorizedTime(0,TRUE))
      {
        // find newly memorized spell and announce it
        //POSITION p = pChar->GetFirstSpellPosition(); // access ordered list
        //while (p != NULL)
        int i, n;
        n = pChar->GetSpellCount();
        for (i=0; i<n; i++)
        {
          //if (pChar->GetSpellAt(p).JustMemorized)
          if (pChar->PeekCharacterSpell(i)->JustMemorized)
          {
            //pChar->GetSpellAt(p).JustMemorized=FALSE;
            pChar->GetCharacterSpell(i)->JustMemorized=FALSE;
          }
          //pChar->GetNextSpell(p);
        }
      }
    }
  }
};
 


#endif // UAFEngine


#ifdef UAFEDITOR
SPELL_ID SPELL_DATA_TYPE::FindPreVersionSpellNamesSpellID(int preSpellNameKey) const
{
  //POSITION pos;
  //pos = GetHeadPosition();
  //while (pos != NULL)
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    const SPELL_DATA *pSpell;
    //pSpell = PeekSpellData(pos);
    pSpell = PeekSpell(i);
    if (pSpell->preSpellNameKey == preSpellNameKey)
    {
      return pSpell->SpellID();
    };
    //GetNext(pos);
  };
  MsgBoxInfo("Searching for a Spell that was defined by its 'key'.  We lost that information "
             "at about version 0.998101 of the editor.  If this is a problem for you, please "
             "contact the DC development team for help.");
  return SPELL_ID();
}
#endif
