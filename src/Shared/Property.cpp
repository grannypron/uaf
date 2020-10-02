#include "stdafx.h"
#include "externs.h"


#include "Property.h"


#ifdef UAFEngine
CPropertyModifier::CPropertyModifier()
{
  Clear();
}

CPropertyModifier::CPropertyModifier(double mod, PropModifier type, int srckey) : 
  m_ModValue(mod),
  m_ModType(type),
  m_ModSrcKey(srckey)
{

}

CPropertyModifier::~CPropertyModifier()
{

}

void CPropertyModifier::Clear()
{
  m_ModValue = 0.0;
  m_ModType = pmNONE;
  m_ModSrcKey = -1;
}

CPropertyModifier &CPropertyModifier::operator=(const CPropertyModifier &rhs)
{
  if (this == &rhs) return *this;
  m_ModValue = rhs.m_ModValue;
  m_ModType = rhs.m_ModType;
  m_ModSrcKey = rhs.m_ModSrcKey;
  return *this;
}

bool CPropertyModifier::operator==(const CPropertyModifier &rhs)
{
  if (m_ModValue != rhs.m_ModValue) return false;
  if (m_ModType != rhs.m_ModType) return false;
  if (m_ModSrcKey != rhs.m_ModSrcKey) return false;
  return true;
}

bool CPropertyModifier::operator!=(const CPropertyModifier &rhs)
{
  return ( (*this == rhs) == false );
}


void CPropertyModifier::Serialize(CArchive &ar, double version)
{
  if (ar.IsStoring())
  {
    ar << m_ModValue;
    ar << (int) m_ModType;
    ar << m_ModSrcKey;
  }
  else
  {
    int tmp;
    ar >> m_ModValue;
    ar >> tmp; 
    m_ModType = (PropModifier)tmp;
    ar >> m_ModSrcKey;
  }
}

void CPropertyModifier::Serialize(CAR &ar, double version)
{
  if (ar.IsStoring())
  {
    ar << m_ModValue;
    ar << (int) m_ModType;
    ar << m_ModSrcKey;
  }
  else
  {
    int tmp;
    ar >> m_ModValue;
    ar >> tmp; 
    m_ModType = (PropModifier)tmp;
    ar >> m_ModSrcKey;
  }
}

double CPropertyModifier::ApplyModVal(double CurrVal)
{
  if (pmNONE == m_ModType) 
    return CurrVal;

  switch (m_ModType)
  {
  case pmPCNT:
    {
      double mod = m_ModValue / 100.0;
      return (CurrVal * mod);
    }

  case pmCUMULATIVE:
    {
      return (CurrVal + m_ModValue);
    }

  case pmABS:
    {
      return m_ModValue;
    }
  }

  return CurrVal;
}

double CPropertyModifier::GetModVal() const
{
  return m_ModValue;
}

PropModifier CPropertyModifier::GetModType() const
{
  return m_ModType;
}

int CPropertyModifier::GetSrcKey() const
{
  return m_ModSrcKey;
}
#endif

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

/*
CBaseProperty::CBaseProperty() :
  m_Name("UNKNOWN"),
  m_BaseValue(0.0)
{
  
}

CBaseProperty::CBaseProperty(const char *pName) :
  m_Name(pName),
  m_BaseValue(0.0)
{


}

CBaseProperty::CBaseProperty(const char *pName, double baseval) :
  m_Name(pName),
  m_BaseValue((double)baseval)
{

}

CBaseProperty::CBaseProperty(const CBaseProperty &rhs)
{
  *this = rhs;
}

CBaseProperty::~CBaseProperty()
{
  ClearMods();    
}

bool CBaseProperty::MatchesNamedProperty(const char *pName)
{
  return (m_Name.CompareNoCase(pName) == 0);
}

const char *CBaseProperty::GetName()
{
  return (m_Name);
}

void CBaseProperty::ClearMods()
{
  m_ModList.RemoveAll();
  m_AbsMod.Clear();
  m_PcntMod.Clear();
}

void CBaseProperty::AddMod(double ModVal, PropModifier ModType, int SrcKey)
{
  CPropertyModifier mod(ModVal, ModType, SrcKey);

  switch (ModType)
  {
  case pmPCNT:
  case pmABS:
    AddSingletonMod(mod);
    break;

  case pmCUMULATIVE:
    AddMod(mod);
    break;
  default:
    ASS ERT(FALSE);
    break;
  }
}

void CBaseProperty::RemoveMods(int SrcKey)
{
  if (m_AbsMod.GetSrcKey() == SrcKey)
    m_AbsMod.Clear();

  if (m_PcntMod.GetSrcKey() == SrcKey)
    m_PcntMod.Clear();

  POSITION pos = m_ModList.GetHeadPosition();
  bool done = false;

  while (!done)
  {
    while (pos != NULL)
    {
      if (m_ModList.GetAt(pos).GetSrcKey() == SrcKey)
      {
        m_ModList.RemoveAt(pos);
        break; // restart search from beginning
      }

      m_ModList.GetNext(pos);
    }

    // got all thru the list and didn't find any matching
    done = true;
  }
}

double CBaseProperty::GetProperty() const
{
  double base = m_BaseValue;

  if (m_AbsMod.GetModType() == pmABS)
  {
    base = m_AbsMod.GetModVal();
  }

  if (m_PcntMod.GetModType() == pmPCNT)
  {
    double mod = m_PcntMod.GetModVal() / 100.0;
    base = (base * mod);
  }

  POSITION pos = m_ModList.GetHeadPosition();
  while (pos != NULL)
    base += m_ModList.GetNext(pos).GetModVal();

  return ((double)base);
}

BYTE CBaseProperty::GetPropertyAsBYTE() const
{
  double val = GetProperty();
  return ((BYTE)val);
}

int CBaseProperty::GetPropertyAsINT() const
{
  double val = GetProperty();
  return ((int)val);
}

unsigned int CBaseProperty::GetPropertyAsUINT() const
{
  double val = GetProperty();
  return ((unsigned int)val);
}

double CBaseProperty::SetProperty(double BaseVal)
{
  double temp = m_BaseValue;
  m_BaseValue = BaseVal;
  return temp;
}

// only one of this type allowed in list
void CBaseProperty::AddSingletonMod(CPropertyModifier &mod)
{
  switch (mod.GetModType())
  {
  case pmABS:
    m_AbsMod = mod;
    break;

  case pmPCNT:
    m_PcntMod = mod;
    break;
  }
}

// any number of this type allowed in list
void CBaseProperty::AddMod(CPropertyModifier &mod)
{
  m_ModList.AddTail(mod);
}

void CBaseProperty::Serialize(CAR &ar, double version)
{
  if (ar.IsStoring())
  {
    AS(ar, m_Name);
    ar << m_BaseValue;
    ar << m_ModList.GetCount();

    POSITION pos = m_ModList.GetHeadPosition();
    while (pos != NULL)
      m_ModList.GetNext(pos).Serialize(ar, version);
  }
  else
  {
    int count;
    DAS(ar, m_Name);
    ar >> m_BaseValue;
    ar >> count;

    for (int i=0;i<count;i++)
    {
      CPropertyModifier mod;
      mod.Serialize(ar, version);
      m_ModList.AddTail(mod);
    }
  }

  m_AbsMod.Serialize(ar, version);
  m_PcntMod.Serialize(ar, version);
}

void CBaseProperty::Serialize(CArchive &ar, double version)
{
  if (ar.IsStoring())
  {
    AS(ar, m_Name);
    ar << m_BaseValue;
    ar << m_ModList.GetCount();
    
    POSITION pos = m_ModList.GetHeadPosition();
    while (pos != NULL)
      m_ModList.GetNext(pos).Serialize(ar, version);
  }
  else
  {
    int count;
    DAS(ar, m_Name);
    ar >> m_BaseValue;
    ar >> count;
    
    for (int i=0;i<count;i++)
    {
      CPropertyModifier mod;
      mod.Serialize(ar, version);
      m_ModList.AddTail(mod);
    }
  }
  
  m_AbsMod.Serialize(ar, version);
  m_PcntMod.Serialize(ar, version);
}

double CBaseProperty::operator=(const double rhs)
{
  m_BaseValue = rhs;
  return m_BaseValue;
}

bool CBaseProperty::operator==(const double rhs)
{
  return (GetProperty() == rhs);
}

bool CBaseProperty::operator < (const double rhs)
{
  return (GetProperty() < rhs);
}

bool CBaseProperty::operator <= (const double rhs)
{
  return (GetProperty() <= rhs);
}

bool CBaseProperty::operator > (const double rhs)
{
  return (GetProperty() > rhs);
}

bool CBaseProperty::operator >= (const double rhs)
{
  return (GetProperty() >= rhs);
}

CBaseProperty &CBaseProperty::operator=(const CBaseProperty &rhs)
{
  if (this == &rhs) return *this;

  m_BaseValue = rhs.m_BaseValue;
  m_Name = rhs.m_Name;
  m_AbsMod = rhs.m_AbsMod;
  m_PcntMod = rhs.m_PcntMod;

  m_ModList.RemoveAll();

  POSITION pos = rhs.m_ModList.GetHeadPosition();
  while (pos != NULL)
    m_ModList.AddTail( rhs.m_ModList.GetNext(pos) );

  return *this;
}

bool CBaseProperty::operator==(const CBaseProperty &rhs)
{
  // exclude dynamic values from this comparison
  if (m_BaseValue != m_BaseValue) return false;
  if (m_Name.CompareNoCase(rhs.m_Name) != 0) return false;

  return true;
}

bool CBaseProperty::operator!=(const CBaseProperty &rhs)
{
  return ( (*this == rhs) == false );
}
*/
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

/*
void CPropertyContainer::Clear()
{
  m_PropList.RemoveAll();
}

void CPropertyContainer::UpdateProperty(CBaseProperty &prop)
{
  POSITION pos = FindProperty(prop.GetName());

  if (pos != NULL) 
    m_PropList.SetAt(pos, prop);
  else
    m_PropList.AddTail(prop);    
}
  
CBaseProperty *CPropertyContainer::GetProperty(const char *pName)
{
  if (pName == NULL) return NULL;    
  POSITION pos = FindProperty(pName);
  
  if (pos != NULL) 
    return (&m_PropList.GetAt(pos));
  
  return NULL;    
}

bool CPropertyContainer::RemoveProperty(const char *pName)
{
  if (pName == NULL) return false;    
  POSITION pos = FindProperty(pName);
  
  if (pos != NULL) 
    m_PropList.RemoveAt(pos);
  
  return (pos != NULL);
}


void CPropertyContainer::ClearAllMods()
{
  POSITION pos = m_PropList.GetHeadPosition();
  while (pos != NULL)
    m_PropList.GetNext(pos).ClearMods();
}

void CPropertyContainer::ClearMods(const char *pName) // only for given property 
{
  if (pName == NULL) return;    
  POSITION pos = FindProperty(pName);
  
  if (pos != NULL)
    m_PropList.GetAt(pos).ClearMods();
}

void CPropertyContainer::RemoveMods(int SrcKey) // all mods that match SrcKey
{
  POSITION pos = m_PropList.GetHeadPosition();
  while (pos != NULL)
    m_PropList.GetNext(pos).RemoveMods(SrcKey);

  if (IsKeyInList(SrcKey))
    m_SrcKeys.AddTail(SrcKey);

}
 
void CPropertyContainer::Serialize(CArchive &ar, double version)
{
  if (ar.IsStoring())
  {
    CString hdr("CPropertyContainer");
    AS(ar, hdr);

    ar << m_PropList.GetCount();
    POSITION pos = m_PropList.GetHeadPosition();
    while (pos != NULL)
      m_PropList.GetNext(pos).Serialize(ar, version);
  }
  else
  {
    CString hdr;
    DAS(ar, hdr);
    
    if (hdr.Compare("CPropertyContainer") != 0)
    {
      // not in sync with file format!
      ASS ERT(FALSE);

      CString msg;
      msg.Format("Encountered unexpected data in file while reading CPropertyContainer data");                  
      WriteDebugString(msg);
      MsgBoxError(msg);
        throw 7;
        // I replaced the AfsThrowArchiveExecption because I could not
        // figure out how tatch it properly.  It caused a memory leak.
      //AfxThrowArchiveException(CArchiveException::badIndex,
      //                         ar.GetFile()->GetFileName());
    }

    int count;
    ar >> count;

    // made up number, but suspicious if that many are present at this point
    ASSERT( count < 1000 ); 

    for (int i=0;i<count;i++)
    {
      CBaseProperty prop;
      
      prop.Serialize(ar, version);

      m_PropList.AddTail(prop);
    }
  }
}

void CPropertyContainer::Serialize(CAR &ar, double version)
{
  if (ar.IsStoring())
  {
    CString hdr("CPropertyContainer");
    AS(ar, hdr);
    
    ar << m_PropList.GetCount();
    POSITION pos = m_PropList.GetHeadPosition();
    while (pos != NULL)
      m_PropList.GetNext(pos).Serialize(ar, version);
  }
  else
  {
    CString hdr;
    DAS(ar, hdr);

    if (hdr.Compare("CPropertyContainer") != 0)
    {
      // not in sync with file format!
      ASS ERT(FALSE);
      CString msg;
      msg.Format("Encountered unexpected data in file while reading CPropertyContainer data");                  
      WriteDebugString(msg);
      MsgBoxError(msg);
        throw 7;
        // I replaced the AfsThrowArchiveExecption because I could not
        // figure out how tatch it properly.  It caused a memory leak.
      //AfxThrowArchiveException(CArchiveException::badIndex,
      //  ar.GetFile()->GetFileName());
    }

    int count;
    ar >> count;
    
    // made up number, but suspicious if that many are present at this point
    ASSERT( count < 1000 ); 
    
    for (int i=0;i<count;i++)
    {
      CBaseProperty prop;
      
      prop.Serialize(ar, version);
      
      m_PropList.AddTail(prop);
    }
  }
}

  
CPropertyContainer &CPropertyContainer::operator=(const CPropertyContainer &rhs)
{
  if (this == &rhs) return *this;
  m_PropList.RemoveAll();
  POSITION pos = rhs.m_PropList.GetHeadPosition();
  while (pos != NULL)
    m_PropList.AddTail( rhs.m_PropList.GetNext(pos) );
  return *this;
}

bool CPropertyContainer::operator==(const CPropertyContainer &rhs)
{
  ASSERT( FALSE ); // do we really need this?
  return false;
}

bool CPropertyContainer::operator!=(const CPropertyContainer &rhs)
{
  ASSERT( FALSE ); // do we really need this?
  return false;
}
 
POSITION CPropertyContainer::FindProperty(const char *pName)
{
  if (pName == NULL) return NULL;
  POSITION pos = m_PropList.GetHeadPosition();
  while (pos != NULL)
  {
    if (m_PropList.GetAt(pos).MatchesNamedProperty(pName))
      return pos;

    m_PropList.GetNext(pos);
  }
  return NULL;
}

void CPropertyContainer::AddMod(const char *pName, double ModVal, PropModifier ModType, int SrcKey)
{
  if (pName == NULL) return;    
  POSITION pos = FindProperty(pName);
  
  if (pos != NULL)
    m_PropList.GetAt(pos).AddMod(ModVal, ModType, SrcKey);

  if (!IsKeyInList(SrcKey))
    m_SrcKeys.AddTail(SrcKey);
}

bool CPropertyContainer::IsKeyInList(int SrcKey)
{
  POSITION pos = m_SrcKeys.GetHeadPosition();
  while (pos != NULL)
  {
    if (m_SrcKeys.GetNext(pos) == SrcKey)
      return true;
  }
  return false;
}

void CPropertyContainer::GetSpellKeyList(PropModKeyListType &list)
{
  list.RemoveAll();
  POSITION pos = m_SrcKeys.GetHeadPosition();
  while (pos != NULL)
    list.AddTail(m_SrcKeys.GetNext(pos));
}
*/