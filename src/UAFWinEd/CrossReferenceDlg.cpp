/******************************************************************************
* Filename: ImportWarning.cpp
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
#include "resource.h"

#include "UAFWinEd.h"
//#include "globtext.h"
//#include "GlobalData.h"
//#include "GameEvent.h"
//#include "buttons.h"
//#include "SelectPicDlg.h"
#include "class.h"
#include "CrossReference.h"
#include "ImportWarning.h"
#include "Spell.h"
#include "GlobalData.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void *My_malloc(int size);
void *My_realloc(void *buf, int size);
void My_free(void *ptr);

/////////////////////////////////////////////////////////////////////////////
// CrossReference dialog


int CrossReference::CreateSortList(void)
{
  // If count only then count maximum possible entries.
  CR_ENTRY *pCREntry;
  CR_REFERENCE *pCRReference;
  int i, numRef;
  bool firstReference, zeroReference, oneReference, twoReference, exists, missing;
  zeroReference   = m_zeroReference.GetCheck() != 0;
  oneReference    = m_oneReference.GetCheck() != 0;
  twoReference    = m_twoReference.GetCheck() != 0;
  firstReference  = m_firstReference.GetCheck() != 0;
  exists  = m_exists.GetCheck() != 0;
  missing = m_missing.GetCheck() != 0;
  i=0;
  for (pCREntry=m_pCRList->CR_FirstEntry(); pCREntry!=NULL; pCREntry=m_pCRList->CR_NextEntry(pCREntry))
  {
    if (!exists && pCREntry->m_exists) continue;
    if (!missing && !pCREntry->m_exists) continue;
    numRef = pCREntry->CR_NumReference();
    if ( (numRef == 0) && !zeroReference ) continue;
    if ( (numRef == 1) && !oneReference )  continue;
    if ( (numRef >= 2) && !twoReference )  continue;

    pCRReference = pCREntry->CR_FirstReference();
    do 
    {
      if (i >= m_sortArraySize)
      {
        m_sortArraySize += 1000;
        m_sortEntry = (CR_ENTRY **)My_realloc(m_sortEntry, m_sortArraySize * sizeof (m_sortEntry[0]));
        m_sortReference = (CR_REFERENCE **)My_realloc(m_sortReference, m_sortArraySize * sizeof (m_sortReference[0]));
      };
      m_sortEntry[i] = pCREntry;
      m_sortReference[i] = pCRReference;
      i++;
      if (firstReference) break;
      if (pCRReference == NULL) break;
      pCRReference = pCREntry->CR_NextReference(pCRReference);
    } while (pCRReference != NULL);
  }
  return i;
}


CrossReference::CrossReference(CR_LIST *pCRList, CWnd* pParent)
      : CDialog(CrossReference::IDD, pParent)
{
   //{{AFX_DATA_INIT(CrossReference)
   //m_alpha = "";
   //m_numeric = "";
	//}}AFX_DATA_INIT
  int i;
  m_pCRList = pCRList;
  for (i=0; i<6; i++) m_sortColumns[i] = i+1;
  return;
}

CrossReference::~CrossReference(void)
{
  My_free(m_sortEntry);
  My_free(m_sortReference);
  m_sortEntry = NULL;
  m_sortReference = NULL;
  m_sortArraySize = 0;
}

void CrossReference::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CrossReference)
	DDX_Control(pDX, IDC_xRefList, m_xRefList);
  DDX_Control(pDX, IDC_ZeroReference, m_zeroReference); 
  DDX_Control(pDX, IDC_OneReference,  m_oneReference); 
  DDX_Control(pDX, IDC_TwoReference,  m_twoReference); 
  DDX_Control(pDX, IDC_DisplayFirst,  m_firstReference); 
  DDX_Control(pDX, IDC_EXISTS, m_exists); 
  DDX_Control(pDX, IDC_MISSING, m_missing); 
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CrossReference, CDialog)
//{{AFX_MSG_MAP(CrossReference)
	ON_BN_CLICKED(IDC_ZeroReference, OnZeroReference)
	ON_BN_CLICKED(IDC_OneReference,  OnOneReference)
	ON_BN_CLICKED(IDC_TwoReference,  OnTwoReference)
	ON_BN_CLICKED(IDC_DisplayFirst,  OnFirstReference)
	ON_BN_CLICKED(IDC_EXISTS, OnExists)
	ON_BN_CLICKED(IDC_MISSING, OnMissing)
  //ON_LVN_COLUMNCLICK(IDC_xRefList, OnColumnClick)
  ON_NOTIFY(LVN_COLUMNCLICK,IDC_xRefList,OnColumnClick)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_Export, &CrossReference::OnClickedExport)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CrossReference message handlers



char * typeNames[] = {
  "Unknown",
  "ability",
  "baseclass",
  "char",
  "class",
  "Code",
  "Event",     
  "global",    
  "Item",    
  "level",   
  "monster",
  "NPC",
  "PIC file",  
  "race",
  "skill",
  "skillAdj",
  "soundFile",
  "special ability",
  "special item",
  "special key",
  "Spell",    
  "spellBook", 
};





int CrossReference::Col1Compare(int i, int j)
{
  return m_sortEntry[i]->m_resourceType
         - m_sortEntry[j]->m_resourceType;
}
int CrossReference::Col2Compare(int i, int j)
{
  return m_sortEntry[i]->m_resourceID - m_sortEntry[j]->m_resourceID;
}
int CrossReference::Col3Compare(int i, int j)
{
  return m_sortEntry[i]->m_resourceName.CompareNoCase(m_sortEntry[j]->m_resourceName);
}
int CrossReference::Col4Compare(int i, int j)
{
  return m_sortEntry[i]->m_exists - m_sortEntry[j]->m_exists;
}
int CrossReference::Col5Compare(int i, int j)
{
  if (m_sortReference[i] == NULL)
  {
    if (m_sortReference[j] == NULL) return 0;
    return -1;
  };
  if (m_sortReference[j] == NULL) return 1;
  return m_sortReference[i]->m_referenceType
         - m_sortReference[j]->m_referenceType;
}
int CrossReference::Col6Compare(int i, int j)
{
  if (m_sortReference[i] == NULL)
  {
    if (m_sortReference[j] == NULL) return 0;
    return -1;
  };
  if (m_sortReference[j] == NULL) return 1;
  return m_sortReference[i]->m_referenceName.CompareNoCase(m_sortReference[j]->m_referenceName);
}
int CrossReference::Col7Compare(int i, int j)
{
  return m_sortReference[i]->m_referenceID - m_sortReference[j]->m_referenceID;
}

typedef int (CrossReference::*cfunc)(int i, int j);

cfunc comparisons[7] = {  // result = entry[i] - entry[j]
  &CrossReference::Col1Compare,
  &CrossReference::Col2Compare,
  &CrossReference::Col3Compare,
  &CrossReference::Col4Compare,
  &CrossReference::Col5Compare,
  &CrossReference::Col6Compare,
  &CrossReference::Col7Compare
};

bool CrossReference::Greater(int i, int j)
{ // Is entry[i] > entry[j]?
  int k, result, sortColumn;
  for (k=0; k<6; k++)
  {
    sortColumn = abs(m_sortColumns[k]);
    result = (this->*comparisons[sortColumn-1])(i, j);
    if (result == 0) continue;
    return (m_sortColumns[k] ^ result) >= 0;
  };
  return false;
}


void CrossReference::Swap(int i, int j)
{
  CR_ENTRY *tempEnt;
  CR_REFERENCE *tempRef;
  tempEnt = m_sortEntry[i];
  tempRef = m_sortReference[i];
  m_sortEntry[i] = m_sortEntry[j];
  m_sortReference[i] = m_sortReference[j];
  m_sortEntry[j] = tempEnt;
  m_sortReference[j] = tempRef;
}

void CrossReference::Sort(void)
{
  int i, j, k, numString;
  for (numString = m_itemCount/2; numString > 0; numString/=2)
  {
    for (i=0; i<numString; i++)
    {
      // Sort string staring at index i
      for (j=i; j < m_itemCount-numString; j+= numString)
      {
        if (Greater(j, j+numString))
        {
          Swap(j, j+numString);
          for (k=j-numString; k>=0; k-=numString)
          {
            if (Greater(k, k+numString))
            {
              Swap(k, k+numString);
            }
            else
            {
              break;
            };
          };
        };
      };
    };
  };
}

void CrossReference::FillListBox(void)
{
  CWaitCursor cwc;
  int i;
  int result;
//  bool printZeroReference, printOneReference, printTwoReference, exists;
  CR_ENTRY *pCREntry;
  CR_REFERENCE *pCRReference;

  m_itemCount = CreateSortList();
  Sort();

//  printZeroReference = m_zeroReference.GetCheck() != 0;
//  printOneReference  = m_oneReference.GetCheck() != 0;
//  printTwoReference  = m_twoReference.GetCheck() != 0;
//  exists = m_exists.GetCheck() != 0;


  m_xRefList.DeleteAllItems();

  for (i=0; i<m_itemCount; i++)
  {
    CString ID;
    pCREntry = m_sortEntry[i];
    pCRReference = m_sortReference[i];
    ID.Format("%d", pCREntry->m_resourceID);
    result = m_xRefList.InsertItem(i,typeNames[pCREntry->m_resourceType]);
    result = m_xRefList.SetItemText(i,1,ID);
    result = m_xRefList.SetItemText(i,2,pCREntry->m_resourceName);
    result = m_xRefList.SetItemText(i,3,pCREntry->m_exists?"Yes":"No");
    if (pCRReference != NULL)
    {
      result = m_xRefList.SetItemText(i,4,typeNames[pCRReference->m_referenceType]);
      result = m_xRefList.SetItemText(i,5,pCRReference->m_referenceName);
      ID.Format("%d", pCRReference->m_referenceID);
      result = m_xRefList.SetItemText(i, 6, ID);
    };
  };
}


BOOL CrossReference::OnInitDialog() 
{

  int result;
  
  CDialog::OnInitDialog();



  result = m_xRefList.InsertColumn(0, "Res Type");
  result = m_xRefList.InsertColumn(1, "ID");
  result = m_xRefList.InsertColumn(2, "Resource Name");
  result = m_xRefList.InsertColumn(3, "Exists");
  result = m_xRefList.InsertColumn(4, "Ref Type");
  result = m_xRefList.InsertColumn(5, "Reference Name");
  result = m_xRefList.InsertColumn(6, "ID");

  result = m_xRefList.SetColumnWidth(0,80);
  result = m_xRefList.SetColumnWidth(1,60);
  result = m_xRefList.SetColumnWidth(2,170);
  result = m_xRefList.SetColumnWidth(3,50);
  result = m_xRefList.SetColumnWidth(4,60);
  result = m_xRefList.SetColumnWidth(5,170);
  result = m_xRefList.SetColumnWidth(6, 60);

  m_sortEntry     = NULL;
  m_sortReference = NULL;
  m_sortArraySize = 0;

  m_exists.SetCheck(true);
  m_missing.SetCheck(true);
  m_zeroReference.SetCheck(true);
  m_oneReference.SetCheck(true);
  m_twoReference.SetCheck(true);
  m_firstReference.SetCheck(true);


  CreateSortList();
  FillListBox();
    
  return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


afx_msg void CrossReference::OnColumnClick( NMHDR * pNotifyStruct, LRESULT * result )
{
  int i, sortColumn;
  NMLISTVIEW *p = (NMLISTVIEW *)pNotifyStruct;
  sortColumn = p->iSubItem+1;  // Column ordinal
  for (i=0; i<7; i++)
  {
    if (m_sortColumns[i] == sortColumn) break;
  };
  if (i > 6) return;
  for (; i>0; i--)
  {
    m_sortColumns[i] = m_sortColumns[i-1];
  };
  m_sortColumns[0] = sortColumn;
  FillListBox();
}



void CrossReference::OnOK() 
{
   UpdateData(TRUE);
   
   CDialog::OnOK();
}

void CrossReference::OnZeroReference(void)
{
  UpdateData(TRUE);
  FillListBox();
}

void CrossReference::OnOneReference(void)
{
  UpdateData(TRUE);
  FillListBox();
}

void CrossReference::OnTwoReference(void)
{
  UpdateData(TRUE);
  FillListBox();
}

void CrossReference::OnFirstReference(void)
{
  UpdateData(TRUE);
  FillListBox();
}

void CrossReference::OnExists(void)
{
  UpdateData(TRUE);
  FillListBox();
}

void CrossReference::OnMissing(void)
{
  UpdateData(TRUE);
  FillListBox();
}


void CR_ENTRY::CR_AddReference(CR_TYPE referenceType,
                               const CString& referenceName, 
                               int referenceID)
{
  CR_REFERENCE *pCRReference;
  pCRReference = new CR_REFERENCE;
  pCRReference->m_referenceName = referenceName;
  pCRReference->m_referenceType = referenceType;
  pCRReference->m_referenceID = referenceID;

  if (this->m_pFirstReference == NULL) this->m_pFirstReference = pCRReference;
  if (this->m_pLastReference != NULL) this->m_pLastReference->m_pNext = pCRReference;
  pCRReference ->m_pNext= NULL;
  pCRReference->m_pPrev = this ->m_pLastReference;
  this->m_pLastReference = pCRReference;

}

int CR_ENTRY::CR_NumReference(void) const
{
  int num=0;
  CR_REFERENCE *pCRReference;
  for (pCRReference = m_pFirstReference; pCRReference!=NULL; pCRReference=pCRReference->m_pNext)
  {
    num++;
  };
  return num;
}


CR_ENTRY *CR_LIST::CR_AddReference(CR_TYPE resourceType, 
                                   CString resourceName,
                                   int   resourceID,
                                   const CR_REFERENCE *pCRReference)
{
  CR_ENTRY *pCREntry;
  if (resourceName == "") resourceName = "Bogus";
  pCREntry = this->CR_LocateResource(resourceType, resourceName, resourceID);
  if (pCREntry == NULL)
  {
    pCREntry = this->CR_InsertResource(resourceType, resourceName, resourceID, false);
  };
  pCREntry->CR_AddReference(pCRReference->m_referenceType, 
                            pCRReference->m_referenceName,
                            pCRReference->m_referenceID);
  return pCREntry;
}

CR_ENTRY *CR_LIST::CR_InsertResource(CR_TYPE resourceType, const CString& resourceName, int id, bool exists)
{
  // You should already have done a search and discovered that
  // the resource does not exist in the list.
  CR_ENTRY *pCREntry;
  pCREntry = new CR_ENTRY;
  pCREntry->m_exists = exists;
  pCREntry->m_resourceName = resourceName;
  pCREntry->m_resourceType = resourceType;
  pCREntry->m_resourceID = id;

  pCREntry->m_pPrev = this->m_pLast;
  pCREntry->m_pNext = NULL;
  if (this->m_pFirst == NULL) this->m_pFirst = pCREntry;
  if (this->m_pLast != NULL) this->m_pLast->m_pNext = pCREntry;
  this->m_pLast = pCREntry;
  return pCREntry;
}

void CR_LIST::DeleteEntry(CR_ENTRY *pCREntry)
{
  if (this->m_pFirst == pCREntry) this->m_pFirst = pCREntry->m_pNext;
  if (this->m_pLast  == pCREntry) this->m_pLast = pCREntry->m_pPrev;
  if (pCREntry->m_pNext != NULL) pCREntry->m_pNext->m_pPrev = pCREntry->m_pPrev;
  if (pCREntry->m_pPrev != NULL) pCREntry->m_pPrev->m_pNext = pCREntry->m_pNext;
  delete pCREntry;
}

CR_ENTRY *CR_LIST::CR_LocateResource(CR_TYPE resourceType, const CString& resourceName, int id)
{
  CR_ENTRY *pEntry;
  for (pEntry=this->m_pFirst; pEntry != NULL; pEntry=pEntry->m_pNext)
  {
    if ( (pEntry->m_resourceType == resourceType) && (pEntry->m_resourceName.CompareNoCase(resourceName)==0) )
    {
      return pEntry;
    };
  };
  return NULL;
}

void CR_ENTRY::CR_DeleteReference(CR_REFERENCE *pCRReference)
{
  if (this->m_pFirstReference == pCRReference) this->m_pFirstReference = pCRReference->m_pNext;
  if (this->m_pLastReference  == pCRReference) this->m_pLastReference = pCRReference->m_pPrev;
  if (pCRReference->m_pNext != NULL) pCRReference->m_pNext->m_pPrev = pCRReference->m_pPrev;
  if (pCRReference->m_pPrev != NULL) pCRReference->m_pPrev->m_pNext = pCRReference->m_pNext;
  delete pCRReference;
}

CR_ENTRY *CR_LIST::CR_AddResource(CR_TYPE resourceType, 
                                  const CString& resourceName, 
                                  int id)
{
  CR_ENTRY *pCREntry;
  pCREntry = this->CR_LocateResource(resourceType, resourceName, id);
  if (pCREntry != NULL)
  {
    pCREntry->m_exists = true;
    return pCREntry;
  };
  return this->CR_InsertResource(resourceType, resourceName, id, true);
}

void CR_LIST::CR_AddSoundReference(const CString& fname, const CR_REFERENCE *pCRReference)
{
  if (fname == "") return;
  CR_AddReference(CR_TYPE_sound, fname, 0, pCRReference);
}

void CR_LIST::CR_AddPicReference(const CString& fname, const CR_REFERENCE *pCRReference)
{
  if (fname == "") return;
  CR_AddReference(CR_TYPE_picFile, fname, 0, pCRReference);
}


//void CR_LIST::CR_AddSpellReference(GLOBAL_SPELL_ID gsID, const CR_REFERENCE *pCRReference)
void CR_LIST::CR_AddSpellReference(SPELL_ID spellID, const CR_REFERENCE *pCRReference)
{
  CString spellName;
  //SPELL_DATA *pSpellData;
  //if (!gsID.IsValidSpell()) return;
  if (!spellID.IsValidSpell()) return;
  //pSpellData = spellData.GetSpellData(gsID);
  //if (pSpellData != NULL)
  //{
  //  spellName = pSpellData->Name;
  //};
  //CR_AddReference(CR_TYPE_spell, spellData.GetSpellName(gsID), gsID.UniqueIdentifier(), pCRReference);
  CR_AddReference(CR_TYPE_spell, spellData.GetSpellName(spellID), 0, pCRReference);
}

//void CR_LIST::CR_AddItemReference(GLOBAL_ITEM_ID giID, const CR_REFERENCE *pCRReference)
void CR_LIST::CR_AddItemReference(ITEM_ID itemID, const CR_REFERENCE *pCRReference)
{
  CString itemName;
  //SPELL_DATA *pSpellData;
  //if (!giID.IsValidItem()) return;
  if (!itemID.IsValidItem()) return;
  //pSpellData = spellData.GetSpellData(gsID);
  //if (pSpellData != NULL)
  //{
  //  spellName = pSpellData->Name;
  //};
  //CR_AddReference(CR_TYPE_item, itemData.GetItemIdName(giID), giID.UniqueIdentifier(), pCRReference);
  CR_AddReference(CR_TYPE_item, itemData.GetItemIdName(itemID), 0, pCRReference);
}

void CR_LIST::CR_AddSpecialItemReference(SPECIAL_OBJECT_EVENT_DATA *pSitem, const CR_REFERENCE *pCRReference)
{
  SPECIAL_OBJECT_DATA odata;
  switch (pSitem->ItemType)
  {
  case KEY_FLAG:
    globalData.keyData.GetItem(pSitem->index, odata);
    CR_AddReference(CR_TYPE_specialKey, odata.name, pSitem->index, pCRReference);
    break;
  case ITEM_FLAG:
    globalData.specialItemData.GetItem(pSitem->index, odata);
    CR_AddReference(CR_TYPE_specialItem, odata.name, pSitem->index, pCRReference);
    break;
  default:
    CR_AddReference(CR_TYPE_specialItem, "???", pSitem->index, pCRReference);
  };
}


void CR_LIST::CR_AddEventReference(DWORD eventID, const CR_REFERENCE *pCRReference)
{
  CString itemName;
  //SPELL_DATA *pSpellData;
  if (!globalData.eventData.IsValidEvent(eventID)) return;
  //pSpellData = spellData.GetSpellData(gsID);
  //if (pSpellData != NULL)
  //{
  //  spellName = pSpellData->Name;
  //};
//  CR_AddReference(CR_TYPE_item, itemData.GetItemIdName(giID), giID.UniqueIdentifier(), pCRReference);
}

/*

struct XREF_TRIPLET    // a function to retrieve folder name, a list of extensions
                       // separated by '/', and a CrossReference type (CR_TYPE).
{
  CString (RUNTIME_ENVIRONMENT::*func)(void);
  char *ext;
  CR_TYPE type;
};


XREF_TRIPLET xrefTriplet[] = {
  {&RUNTIME_ENVIRONMENT::AreaViewArtDir, "png", CR_TYPE_picFile},
  {&RUNTIME_ENVIRONMENT::SoundDir, "wav/mp3", CR_TYPE_sound} 
};


typedef CString (RUNTIME_ENVIRONMENT::*fnc)(void);

void CrossReferenceResources(CR_LIST *pCRList)
{
  long result, handle;
  int i, slash;
  _finddata_t fdata;
  CString f, extensions;
  for (i=0; i<sizeof(xrefTriplet)/sizeof(xrefTriplet[0]); i++)
  {
    fnc x;    
    x = xrefTriplet[i].func;
    for (extensions=xrefTriplet[i].ext; extensions != "";)
    {
      f = (rte.*x)();  // For now....the \Resources folder
      f += "*.";
      if ((slash = extensions.Find('/')) > 0)
      {
        f += extensions.Left(slash);
        extensions = extensions.Right(extensions.GetLength() - slash - 1);
      }
      else
      {
        f += extensions;
        extensions = "";
      };
        
      for (handle=result=_findfirst(f, &fdata); result!=-1; result=_findnext(handle,&fdata))
      {
        pCRList->CR_AddResource(xrefTriplet[i].type, fdata.name, 0);
      };
      if (handle!=-1) _findclose(handle);
    };
  };
  return;
}

*/

// We are going to crossreference ** ALL ** files in the
// resources directory.  The code commented out above
// can be used to crossreference only particular file
// types in particular directories.
void CrossReferenceResources(CR_LIST *pCRList)
{
  long result, handle;
  _finddata_t fdata;
  CString f, extensions;
  f = rte.BackgroundArtDir();  // For now....the \Resources folder
  f += "*.*";
  for (handle=result=_findfirst(f, &fdata); result!=-1; result=_findnext(handle,&fdata))
  {
    CString ext;
    CR_TYPE type;
    ext = strrchr(fdata.name, '.');
    ext.MakeLower();
    type = CR_TYPE_unknown;
    if (ext == ".") continue;
    if (ext == "..") continue;
    if (ext == ".png") type = CR_TYPE_picFile;
    if (ext == ".wav") type = CR_TYPE_sound;
    if (ext == ".mp3") type = CR_TYPE_sound;
    pCRList->CR_AddResource(type, fdata.name, 0);
  };
  if (handle!=-1) _findclose(handle);
  return;
}


void CrossReferenceOneChar(CR_LIST *pCRList, const CString& filename)
{
  CHARACTER tempChar;
  BOOL success;
  success = tempChar.serializeCharacter(false, filename);
  if (success)
  {
//    CR_REFERENCE crRef;
//    crRef.m_referenceType = CR_TYPE_char;
//    crRef.m_referenceName = tempChar.GetName();
    tempChar.CrossReference(pCRList);
  };
}


void CrossReferenceSavedCharacters(CR_LIST *pCRList)
{
  long result, handle;
  _finddata_t fdata;
  CString f, extensions;
  f = rte.SaveDir();  //
  f += "*.chr";        
  for (handle=result=_findfirst(f, &fdata); result!=-1; result=_findnext(handle,&fdata))
  {
    CrossReferenceOneChar(pCRList, rte.SaveDir() + fdata.name);
  };
  if (handle!=-1) _findclose(handle);
  return;
}

/*   The following have calls to special abilities cross-reference:
         * ITEM_DATA
         * MONSTER_DATA
         * RACE_DATA
         * ABILITY_DATA
         * BASE_CLASS_DATA
         * TRAIT_DATA
         * SPELLGROUP_DATA
         * CLASS_DATA
         * SPELL_DATA
         * CHARACTER
*/


void SPECIAL_ABILITIES::CrossReference(CR_LIST* pCRList,
                                       CR_REFERENCE* pCRReference) const
{
  POSITION pos;
  const CStringPAIR* pEntry;
  pos = m_specialAbilities.GetStartPosition();
  while (pos != NULL)
  {
    m_specialAbilities.GetNextAssoc(pos, &pEntry);
    pCRList->CR_AddReference(
      CR_TYPE_specialAbility,
      pEntry->Key(),
      0,
      pCRReference);
  };
}

void RACE_DATA::CrossReference(CR_LIST* pCRList) const
{
  CR_REFERENCE CRReference;
  pCRList->CR_AddResource(CR_TYPE_race, this->Name(), 0);
  CRReference.m_referenceType = CR_TYPE_race;
  CRReference.m_referenceName = this->Name();
  m_specAbs.CrossReference(pCRList, &CRReference);
}


void ABILITY_DATA::CrossReference(CR_LIST* pCRList)
{
  CR_REFERENCE CRReference;
  pCRList->CR_AddResource(CR_TYPE_ability, this->m_name, 0);
  CRReference.m_referenceType = CR_TYPE_ability;
  CRReference.m_referenceName = this->m_name;
  m_specAbs.CrossReference(pCRList, &CRReference);
}



void BASE_CLASS_DATA::CrossReference(CR_LIST* pCRList) const
{
  int i, n;
  CR_REFERENCE CRReference;
  CRReference.m_referenceType = CR_TYPE_baseclass;
  CRReference.m_referenceName = this->m_name;
  pCRList->CR_AddResource(CR_TYPE_baseclass, this->m_name, 0);
  n = m_abilityReq.GetSize();
  for (i = 0; i < n; i++)
  {
    PeekAbilityReq(i)->CrossReference(pCRList, &CRReference);
  };
  n = m_allowedRaces.GetSize();
  for (i = 0; i < n; i++)
  {
    const RACE_DATA* pRace;
    pCRList->CR_AddReference(CR_TYPE_race, *PeekRaceID(i), 0, &CRReference);
    pRace = raceData.PeekRace(*PeekRaceID(i));
    if (pRace != NULL)
    {
      pRace->CrossReference(pCRList);
    };
  };
  m_specAbs.CrossReference(pCRList, &CRReference);
  n = m_skills.GetSize();
  for (i = 0; i < n; i++)
  {
    PeekSkill(i)->CrossReference(pCRList, &CRReference);
  };
  n = m_skillAdjustmentsAbility.GetSize();
  for (i = 0; i < n; i++)
  {
    PeekSkillAdjAbility(i)->CrossReference(pCRList, &CRReference);
  };
  n = m_skillAdjustmentsBaseclass.GetSize();
  for (i = 0; i < n; i++)
  {
    PeekSkillAdjBaseclass(i)->CrossReference(pCRList, &CRReference);
  };
  n = m_skillAdjustmentsRace.GetSize();
  for (i = 0; i < n; i++)
  {
    PeekSkillAdjRace(i)->CrossReference(pCRList, &CRReference);
  };
  n = m_skillAdjustmentsScript.GetSize();
  for (i = 0; i < n; i++)
  {
    PeekSkillAdjScript(i)->CrossReference(pCRList, &CRReference);
  };
  n = m_bonusXP.GetSize();
  for (i = 0; i < n; i++)
  {
    PeekBonusXP(i)->CrossReference(pCRList, &CRReference);
  };
}


void ABILITY_REQ::CrossReference(CR_LIST* pCRList, CR_REFERENCE* pCRReference)const
{
  pCRList->CR_AddResource(CR_TYPE_ability, this->m_abilityID, 0);
}

void SKILL::CrossReference(CR_LIST* pCRList, CR_REFERENCE* pCRReference) const
{
  pCRList->CR_AddResource(CR_TYPE_skill, this->skillID, 0);
}


void BONUS_XP::CrossReference(CR_LIST* pCRList, CR_REFERENCE* pCRReference)const
{
  if (abilityID.IsEmpty()) return;
  {
    pCRList->CR_AddReference(CR_TYPE_ability, abilityID, 0, pCRReference);
  };
}


void SKILL_ADJUSTMENT_ABILITY::CrossReference(CR_LIST* pCRList, 
                                              CR_REFERENCE* pCRReference) const
{
  CR_REFERENCE CRReference;
  pCRList->CR_AddResource(CR_TYPE_skillAdj, this->abilityID, 0);
  CRReference.m_referenceName = skillID;
  CRReference.m_referenceType = CR_TYPE_skillAdj;
  pCRList->CR_AddReference(CR_TYPE_ability, abilityID, 0, &CRReference);
}


void SKILL_ADJUSTMENT_BASECLASS::CrossReference
                        (CR_LIST* pCRList, CR_REFERENCE* pCRReference)const
{
  CR_REFERENCE CRReference;
  pCRList->CR_AddResource(CR_TYPE_skillAdj, skillID, 0);
  pCRList->CR_AddReference(CR_TYPE_skillAdj, skillID, 0, pCRReference);
  CRReference.m_referenceName = skillID;
  CRReference.m_referenceType = CR_TYPE_skillAdj;
  pCRList->CR_AddReference(CR_TYPE_baseclass, baseclassID, 0, &CRReference);
}


void SKILL_ADJUSTMENT_RACE::CrossReference
               (CR_LIST* pCRList, CR_REFERENCE* pCRReference)const
{
  CR_REFERENCE CRReference;
  pCRList->CR_AddResource(CR_TYPE_skillAdj, skillID, 0);
  pCRList->CR_AddReference(CR_TYPE_skillAdj, skillID, 0, pCRReference);
  CRReference.m_referenceName = skillID;
  CRReference.m_referenceType = CR_TYPE_skillAdj;
  pCRList->CR_AddReference(CR_TYPE_race, raceID, 0, &CRReference);
}


void SKILL_ADJUSTMENT_SCRIPT::CrossReference
                 (CR_LIST* pCRList, CR_REFERENCE* pCRReference)const
{
  NotImplemented(0x551a9d, false);
}

void CLASS_DATA::CrossReference(CR_LIST* pCRList)
{
  int i, n;
  CR_REFERENCE CRReference;
  pCRList->CR_AddResource(CR_TYPE_class, m_name, 0);
  CRReference.m_referenceName = this->m_name;
  CRReference.m_referenceType = CR_TYPE_class;
  n = m_baseclasses.GetCount();
  for (i = 0; i < n; i++)
  {
    pCRList->CR_AddReference(CR_TYPE_baseclass, 
                             m_baseclasses.PeekBaseclassID(i)->GetString(),
                             0,
                             &CRReference);
    const BASE_CLASS_DATA* pBaseclass;
    pBaseclass = baseclassData.PeekBaseclass(*m_baseclasses.PeekBaseclassID(i));
      if (pBaseclass != NULL)
      {
        pBaseclass->CrossReference(pCRList);
      };
  };
  m_specialAbilities.CrossReference(pCRList, &CRReference);
  {
    int j, m;
    m = hitDiceLevelBonus.GetSize();
    for (j = 0; j < m; j++)
    {
      PeekHitDiceLevelBonus(j)->CrossReference(pCRList, &CRReference);
    };
  };
  strengthBonusDice.CrossReference(pCRList, &CRReference);
  m_startingEquipment.CrossReference(pCRList, &CRReference);
  pCRList->CR_AddReference(CR_TYPE_baseclass, hitDiceBaseclassID, 0, &CRReference);
}

void SPELLGROUP_DATA::CrossReference(CR_LIST* pCRList)
{
  NotImplemented(0x551a9f, false);
}

void DICEPLUS::CrossReference(CR_LIST *pCRList, CR_REFERENCE *pCRReference) const
{
  // Are there any class, race, etc references in these expressions?
  //AS(car, m_Text);
  //AS(car, m_Bin);
}

void HIT_DICE_LEVEL_BONUS::CrossReference
        (CR_LIST *pCRList, CR_REFERENCE *pCRReference)const
{
  pCRList->CR_AddReference(CR_TYPE_baseclass, baseclassID, 0, pCRReference);
  pCRList->CR_AddReference(CR_TYPE_ability, ability, 0, pCRReference);
}

void A_SPECABILITY_DEF_L::CrossReference(CR_LIST* pCRList) const
{
  POSITION pos;
  SPECABILITY_DEF* pSpecAb;
  pos = GetStartPosition();
  while (pos != NULL)
  {
    pSpecAb = GetNext(pos);
    pCRList->CR_AddResource(CR_TYPE_specialAbility, pSpecAb->Name(), 0);
  };
}

void CrossReference::OnClickedExport()
{
  CR_ENTRY* pCREntry;
  CR_REFERENCE* pCRReference;
  FILE* f;
  int i;
  f = fopen(LPCSTR(rte.DataDir() + "CrossReference.txt"), "w");
  if (f == NULL)
  {
    CString msg;
    msg = "Unable to open " + rte.DataDir() + "CrossReference.txt";
    MsgBoxInfo(msg, "Attempt to export");
    return;
  };
// Sort should already be completed
//  m_itemCount = CreateSortList();
//  Sort();
  for (i = 0; i < m_itemCount; i++)
  {
    CString ID;
    pCREntry = m_sortEntry[i];
    pCRReference = m_sortReference[i];

    ID.Format("%d", pCREntry->m_resourceID);

    //result = m_xRefList.InsertItem(i, typeNames[pCREntry->m_resourceType]);
    fprintf(f, "%s\n", typeNames[pCREntry->m_resourceType]);

    //result = m_xRefList.SetItemText(i, 1, ID);
    fprintf(f, "%s\n", LPCSTR(ID));

    //result = m_xRefList.SetItemText(i, 2, pCREntry->m_resourceName);
    fprintf(f, "%s\n", LPCSTR(pCREntry->m_resourceName));

    //result = m_xRefList.SetItemText(i, 3, pCREntry->m_exists ? "Yes" : "No");
    fprintf(f, "%s\n", pCREntry->m_exists ? "Yes" : "No");

    if (pCRReference != NULL)
    {
      //result = m_xRefList.SetItemText(i, 4, typeNames[pCRReference->m_referenceType]);
      fprintf(f, "%s\n", typeNames[pCRReference->m_referenceType]);

      //result = m_xRefList.SetItemText(i, 5, pCRReference->m_referenceName);
      fprintf(f, "%s\n", LPCSTR(pCRReference->m_referenceName));
      fprintf(f, "%d\n", pCRReference->m_referenceID);
    }
    else
    {
      fprintf(f, "<empty>\n<empty>\n<empty>\n");
    };
    fprintf(f, "\n");
  };
  fclose(f);
}
