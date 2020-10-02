#ifndef __PROPERTY_H__
#define __PROPERTY_H__

#include "..\shared\stdafx.h"
#include "class.h"

enum PropModifier 
{
  pmNONE       = 0,
  pmPCNT       = 1, // percentage of current
  pmCUMULATIVE = 2, // added to current
  pmABS        = 3  // base value is set to this before mods are applied
};

class CPropertyModifier
{
public:

  CPropertyModifier();
  CPropertyModifier(double mod, PropModifier type, int srckey);

  ~CPropertyModifier();

  void Clear();
  bool IsSet() const { return (m_ModType != pmNONE); }

  CPropertyModifier &operator=(const CPropertyModifier &rhs);
  bool operator==(const CPropertyModifier &rhs);
  bool operator!=(const CPropertyModifier &rhs);

  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);

  double ApplyModVal(double CurrVal);

  double GetModVal() const;

  PropModifier GetModType() const;

  int GetSrcKey() const;

private: 

  double m_ModValue;         // modifier applied to base property
  PropModifier m_ModType; // how mod is interpreted 
  int m_ModSrcKey;        // which spell applied this mod
};

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

typedef CList<int, int> PropModKeyListType;

template <class T> class CBaseProperty
{
public:  

  CBaseProperty(); // don't use this one, only for serialization
  CBaseProperty(const char *pName);
  CBaseProperty(const char *pName, T baseval);
  CBaseProperty(const CBaseProperty &rhs);
  virtual ~CBaseProperty();

  bool MatchesNamedProperty(const char *pName);
  const char *GetName();

  void ClearMods();
  bool IsModified() { if (m_AbsMod.IsSet()) return true; return (!m_ModList.IsEmpty()); } 

  void AddMod(double ModVal, PropModifier ModType, int SrcKey);
  void RemoveMods(int SrcKey);

  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);

  T GetProperty() const;

  T SetProperty(const T BaseVal);
  
  T operator=(const T rhs);
  bool operator==(const T rhs);
  bool operator < (const T rhs);
  bool operator <= (const T rhs);
  bool operator > (const T rhs);
  bool operator >= (const T rhs);

  CBaseProperty &operator=(const CBaseProperty &rhs);
  bool operator==(const CBaseProperty &rhs);
  bool operator!=(const CBaseProperty &rhs);

private:

  CString m_Name;      // property name
  T  m_BaseValue;      // base value for this property

  mutable bool m_isModified;
  mutable T m_cachedValue;

  CPropertyModifier m_AbsMod;

  // list of modifiers applied to this property
  CList<CPropertyModifier, CPropertyModifier&> m_ModList;

  void SetRecomputeFlag()
  {
    m_isModified = true;
    m_cachedValue = 0;
  }

};

template <class T>
CBaseProperty<T>::CBaseProperty() :
  m_Name("UNKNOWN"),
  m_BaseValue(0),
  m_isModified(true),
  m_cachedValue(0)
{
  
}

template <class T>
CBaseProperty<T>::CBaseProperty(const char *pName) :
  m_Name(pName),
  m_BaseValue(0),
  m_isModified(true),
  m_cachedValue(0)
{


}

template <class T>
CBaseProperty<T>::CBaseProperty(const char *pName, T baseval) :
  m_Name(pName),
  m_BaseValue(baseval),
  m_isModified(true),
  m_cachedValue(0)
{

}

template <class T>
CBaseProperty<T>::CBaseProperty(const CBaseProperty &rhs)
{
  *this = rhs;
}

template <class T>
CBaseProperty<T>::~CBaseProperty()
{
  ClearMods();    
}

template <class T>
bool CBaseProperty<T>::MatchesNamedProperty(const char *pName)
{
  return (m_Name.CompareNoCase(pName) == 0);
}

template <class T>
const char *CBaseProperty<T>::GetName()
{
  return (m_Name);
}

template <class T>
void CBaseProperty<T>::ClearMods()
{
  m_ModList.RemoveAll();
  m_AbsMod.Clear();  
  SetRecomputeFlag();
}

template <class T>
void CBaseProperty<T>::AddMod(double ModVal, PropModifier ModType, int SrcKey)
{
  SetRecomputeFlag();

  CPropertyModifier mod(ModVal, ModType, SrcKey);

  switch (ModType)
  {
  case pmPCNT:
  case pmABS:
    m_AbsMod = mod;
    break;

  case pmCUMULATIVE:
    m_ModList.AddTail(mod);
    break;
  default:
    die(0xab52d);
    break;
  }
}

template <class T>
void CBaseProperty<T>::RemoveMods(int SrcKey)
{
  SetRecomputeFlag();

  if (m_AbsMod.GetSrcKey() == SrcKey)
    m_AbsMod.Clear();

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

template <class T>
T CBaseProperty<T>::GetProperty() const
{  
  if (!m_isModified)
    return m_cachedValue;

  T result;

  if (m_AbsMod.GetModType() == pmABS)
  {
    result = m_AbsMod.GetModVal();
    m_cachedValue = result;
    m_isModified = false;
    return result;
  }

  double base = m_BaseValue;
  double mod;
  POSITION pos = m_ModList.GetHeadPosition();
  while (pos != NULL)
  {
    switch (m_ModList.GetAt(pos).GetModType())
    {
    case pmPCNT:
    {
      mod = ((double)m_ModList.GetAt(pos).GetModVal() / 100.0);
      mod = ((double)m_BaseValue * mod);      
    }
    break;

    case pmCUMULATIVE:
      mod = m_ModList.GetAt(pos).GetModVal();      
      break;
    }

    base += mod;
    
    m_ModList.GetNext(pos);
  }

  result = (T)base;

  m_cachedValue = result;
  m_isModified = false;

  return (result);
}


template <class T>
T CBaseProperty<T>::SetProperty(const T BaseVal)
{
  SetRecomputeFlag();
  T temp = m_BaseValue;
  m_BaseValue = BaseVal;
  return temp;
}

template <class T>
void CBaseProperty<T>::Serialize(CAR &ar, double version)
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
}

template <class T>
void CBaseProperty<T>::Serialize(CArchive &ar, double version)
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
}

template <class T>
T CBaseProperty<T>::operator=(const T rhs)
{
  return (SetProperty(rhs));
}

template <class T>
bool CBaseProperty<T>::operator==(const T rhs)
{
  return (GetProperty() == rhs);
}

template <class T>
bool CBaseProperty<T>::operator < (const T rhs)
{
  return (GetProperty() < rhs);
}

template <class T>
bool CBaseProperty<T>::operator <= (const T rhs)
{
  return (GetProperty() <= rhs);
}

template <class T>
bool CBaseProperty<T>::operator > (const T rhs)
{
  return (GetProperty() > rhs);
}

template <class T>
bool CBaseProperty<T>::operator >= (const T rhs)
{
  return (GetProperty() >= rhs);
}

template <class T>
CBaseProperty<T> &CBaseProperty<T>::operator=(const CBaseProperty<T> &rhs)
{
  if (this == &rhs) return *this;

  SetRecomputeFlag();

  m_BaseValue = rhs.m_BaseValue;
  m_Name = rhs.m_Name;
  m_AbsMod = rhs.m_AbsMod;

  m_ModList.RemoveAll();

  POSITION pos = rhs.m_ModList.GetHeadPosition();
  while (pos != NULL)
  {
    const CPropertyModifier& tmp = rhs.m_ModList.GetAt(pos);
    m_ModList.AddTail( const_cast<CPropertyModifier&>(tmp) );
    rhs.m_ModList.GetNext(pos);
  }

  return *this;
}

template <class T>
bool CBaseProperty<T>::operator==(const CBaseProperty<T> &rhs)
{
  // exclude dynamic values from this comparison
  if (m_Name.CompareNoCase(rhs.m_Name) != 0) return false;
  if (m_BaseValue != m_BaseValue) return false;  

  return true;
}

template <class T>
bool CBaseProperty<T>::operator!=(const CBaseProperty<T> &rhs)
{
  return ( (*this == rhs) == false );
}

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

template <class T> class CPropertyContainer
{
public:  

  CPropertyContainer() { }
  virtual ~CPropertyContainer() { Clear(); }
  
  void Clear(); // removes all properties

  void AddMod(const char *pName, double ModVal, PropModifier ModType, int SrcKey);

  void UpdateProperty(CBaseProperty<T> &prop);
  CBaseProperty<T> *GetProperty(const char *pName);
  bool RemoveProperty(const char *pName);

  void ClearAllMods();  
  void ClearMods(const char *pName); // only for given property 
  void RemoveMods(int SrcKey); // all mods that match SrcKey
   
  void GetSpellKeyList(PropModKeyListType &list);
  
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
    
  CPropertyContainer &operator=(const CPropertyContainer &rhs);
  bool operator==(const CPropertyContainer &rhs);
  bool operator!=(const CPropertyContainer &rhs);
  
private:
   
  // list of spells currently modifying items in this list
  PropModKeyListType m_SrcKeys; // need xref between keys in use and prop records!
  bool IsKeyInList(int SrcKey);

  POSITION FindProperty(const char *pName);
  mCList<CBaseProperty<T>, CBaseProperty<T>&> m_PropList;
};

template <class T>
void CPropertyContainer<T>::Clear()
{
  m_PropList.RemoveAll();
}

template <class T>
void CPropertyContainer<T>::UpdateProperty(CBaseProperty<T> &prop)
{
  POSITION pos = FindProperty(prop.GetName());

  if (pos != NULL) 
    m_PropList.SetAt(pos, prop);
  else
    m_PropList.AddTail(prop);    
}
  
template <class T>
CBaseProperty<T> *CPropertyContainer<T>::GetProperty(const char *pName)
{
  if (pName == NULL) return NULL;    
  POSITION pos = FindProperty(pName);
  
  if (pos != NULL) 
    return (&m_PropList.GetAt(pos));
  
  return NULL;    
}

template <class T>
bool CPropertyContainer<T>::RemoveProperty(const char *pName)
{
  if (pName == NULL) return false;    
  POSITION pos = FindProperty(pName);
  
  if (pos != NULL) 
    m_PropList.RemoveAt(pos);
  
  return (pos != NULL);
}

template <class T>
void CPropertyContainer<T>::ClearAllMods()
{
  POSITION pos = m_PropList.GetHeadPosition();
  while (pos != NULL)
    m_PropList.GetNext(pos).ClearMods();
}

template <class T>
void CPropertyContainer<T>::ClearMods(const char *pName) // only for given property 
{
  if (pName == NULL) return;    
  POSITION pos = FindProperty(pName);
  
  if (pos != NULL)
    m_PropList.GetAt(pos).ClearMods();
}

template <class T>
void CPropertyContainer<T>::RemoveMods(int SrcKey) // all mods that match SrcKey
{
  POSITION pos = m_PropList.GetHeadPosition();
  while (pos != NULL)
    m_PropList.GetNext(pos).RemoveMods(SrcKey);

  if (IsKeyInList(SrcKey))
    m_SrcKeys.AddTail(SrcKey);

}
 
template <class T>
void CPropertyContainer<T>::Serialize(CArchive &ar, double version)
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
      die(0xab52e);

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
      CBaseProperty<T> prop;
      
      prop.Serialize(ar, version);

      m_PropList.AddTail(prop);
    }
  }
}

template <class T>
void CPropertyContainer<T>::Serialize(CAR &ar, double version)
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
      die(0xab52f);
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
      CBaseProperty<T> prop;
      
      prop.Serialize(ar, version);
      
      m_PropList.AddTail(prop);
    }
  }
}

  
template <class T>
CPropertyContainer<T> &CPropertyContainer<T>::operator=(const CPropertyContainer<T> &rhs)
{
  if (this == &rhs) return *this;
  m_PropList.RemoveAll();
  POSITION pos = rhs.m_PropList.GetHeadPosition();
  while (pos != NULL)
    m_PropList.AddTail( rhs.m_PropList.GetNext(pos) );
  return *this;
}

template <class T>
bool CPropertyContainer<T>::operator==(const CPropertyContainer<T> &rhs)
{
  ASSERT( FALSE ); // do we really need this?
  return false;
}

template <class T>
bool CPropertyContainer<T>::operator!=(const CPropertyContainer<T> &rhs)
{
  ASSERT( FALSE ); // do we really need this?
  return false;
}
 
template <class T>
POSITION CPropertyContainer<T>::FindProperty(const char *pName)
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

template <class T>
void CPropertyContainer<T>::AddMod(const char *pName, double ModVal, PropModifier ModType, int SrcKey)
{
  if (pName == NULL) return;    
  POSITION pos = FindProperty(pName);
  
  if (pos != NULL)
    m_PropList.GetAt(pos).AddMod(ModVal, ModType, SrcKey);

  if (!IsKeyInList(SrcKey))
    m_SrcKeys.AddTail(SrcKey);
}

template <class T>
bool CPropertyContainer<T>::IsKeyInList(int SrcKey)
{
  POSITION pos = m_SrcKeys.GetHeadPosition();
  while (pos != NULL)
  {
    if (m_SrcKeys.GetNext(pos) == SrcKey)
      return true;
  }
  return false;
}

template <class T>
void CPropertyContainer<T>::GetSpellKeyList(PropModKeyListType &list)
{
  list.RemoveAll();
  POSITION pos = m_SrcKeys.GetHeadPosition();
  while (pos != NULL)
    list.AddTail(m_SrcKeys.GetNext(pos));
}


#endif // __PROPERTY_H__
