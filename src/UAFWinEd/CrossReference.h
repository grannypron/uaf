/******************************************************************************
* Filename: CrossReference.h
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

enum CR_TYPE  // Should be in alphabetical order
{
  CR_TYPE_unknown        = 0,
  CR_TYPE_ability        = 1,
  CR_TYPE_baseclass      = 2,
  CR_TYPE_char           = 3,
  CR_TYPE_class          = 4,
  CR_TYPE_code           = 5,
  CR_TYPE_event          = 6,
  CR_TYPE_global         = 7,
  CR_TYPE_item           = 8,
  CR_TYPE_level          = 9,
  CR_TYPE_monster        = 10,
  CR_TYPE_NPC            = 11,
  CR_TYPE_picFile        = 12,
  CR_TYPE_race           = 13,
  CR_TYPE_skill          = 14,
  CR_TYPE_skillAdj       = 15,
  CR_TYPE_sound          = 16,
  CR_TYPE_specialAbility = 17,
  CR_TYPE_specialItem    = 18,
  CR_TYPE_specialKey     = 19,
  CR_TYPE_spell          = 20,
  CR_TYPE_spellBook      = 21,
};


class CR_REFERENCE
{
  friend class CR_ENTRY;
public:
  CR_REFERENCE(void) { m_referenceID = 0; m_referenceType = CR_TYPE_unknown; };
  CString       m_referenceName;
  CR_TYPE       m_referenceType;
  int           m_referenceID;
private:
  CR_REFERENCE *m_pNext;
  CR_REFERENCE *m_pPrev;
};



class CR_ENTRY
{
  friend class CR_LIST;
public:
  CR_ENTRY(void){m_pNext=NULL;m_pPrev=NULL;m_pFirstReference=NULL;m_pLastReference=NULL;m_exists=false;};
  ~CR_ENTRY(void){while (m_pFirstReference!=NULL){CR_DeleteReference(m_pFirstReference);};};
  void CR_AddReference(CR_TYPE referenceType, const CString& referenceName, int referenceID);
  CR_REFERENCE *CR_FirstReference(void) const{return m_pFirstReference;};
  CR_REFERENCE *CR_NextReference(CR_REFERENCE *pCRReference)const{return pCRReference->m_pNext;};
  void CR_DeleteReference(CR_REFERENCE *pCRReference);
  int  CR_NumReference(void) const;
private:
  CR_ENTRY     *m_pNext;
  CR_ENTRY     *m_pPrev;
public:
  CString       m_resourceName;
  bool          m_exists;
  CR_TYPE       m_resourceType;
  int           m_resourceID;
private:
  CR_REFERENCE *m_pFirstReference;
  CR_REFERENCE *m_pLastReference;
};


class SPECIAL_OBJECT_EVENT_DATA;

class CR_LIST
{
  CR_ENTRY *m_pFirst;
  CR_ENTRY *m_pLast;
  void DeleteEntry(CR_ENTRY *pEnt);
public:
  CR_LIST(void){m_pFirst=NULL; m_pLast=NULL;};
  ~CR_LIST(void){while(m_pFirst!=NULL)DeleteEntry(m_pFirst);};
private:
  CR_ENTRY *CR_InsertResource(CR_TYPE resourceType, const CString& resourceName, int id, bool exists);
  CR_ENTRY *CR_LocateResource(CR_TYPE resourceType, const CString& resourceName, int id);
public:
  CR_ENTRY *CR_AddResource(CR_TYPE resourceType, const CString& resourceName, int id);
//  CR_ENTRY *CR_FindResource(CR_TYPE resourceType, const CString& resourceName, int id);
  CR_ENTRY *CR_FirstEntry(void){return m_pFirst;};
  CR_ENTRY *CR_NextEntry(CR_ENTRY *pCREntry){return pCREntry->m_pNext;};
  CR_ENTRY *CR_AddReference(CR_TYPE             resourceType, 
                            CString             resourceName, 
                            int                 resourceID,
                            const CR_REFERENCE *pCRReference);
  void CR_AddSoundReference(const CString& fname, const CR_REFERENCE *pCRReference);
  void CR_AddPicReference  (const CString& fname, const CR_REFERENCE *pCRReference);
  //void CR_AddSpellReference(GLOBAL_SPELL_ID gsID, const CR_REFERENCE *pCRReference);
  //void CR_AddItemReference (GLOBAL_ITEM_ID  gsID, const CR_REFERENCE *pCRReference);
  void CR_AddSpellReference(SPELL_ID spellID, const CR_REFERENCE *pCRReference);
  void CR_AddItemReference (ITEM_ID  itemID, const CR_REFERENCE *pCRReference);
  void CR_AddSpecialItemReference (SPECIAL_OBJECT_EVENT_DATA *pSitem, const CR_REFERENCE *pCRReference);
  void CR_AddEventReference (DWORD eventID, const CR_REFERENCE *pCRReference);
};


class GameEventList;

class CR_EVENT_INFO
{
public:
  CR_LIST       *m_pCRList;
  int            m_level;
  CString        m_anchorLocation;
  CR_REFERENCE   m_CRReference;
  GameEventList *m_pEventList;
};



/////////////////////////////////////////////////////////////////////////////
// CrossReference dialog

class CrossReference : public CDialog
{
// Construction
public:
	CrossReference(CR_LIST *pCRList, CWnd* pParent = NULL);   // standard constructor
  ~CrossReference(void);
  void FillListBox(void);
// Dialog Data
	//{{AFX_DATA(CrossReference)
	enum { IDD = IDD_CrossReference };
	CListCtrl	m_xRefList;
  CButton   m_zeroReference;
  CButton   m_oneReference;
  CButton   m_twoReference;
  CButton   m_firstReference;
  CButton   m_exists;
  CButton   m_missing;
	//}}AFX_DATA
  CR_LIST *m_pCRList;
  int m_sortColumns[6]; // column[0] is major key
  int m_itemCount;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CrossReference)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
  public:
   void Sort(void);
   int Col1Compare(int i, int j);
   int Col2Compare(int i, int j);
   int Col3Compare(int i, int j);
   int Col4Compare(int i, int j);
   int Col5Compare(int i, int j);
   int Col6Compare(int i, int j);
   int Col7Compare(int i, int j);
   bool Greater(int i, int j);
   void Swap(int i, int j);
   CR_ENTRY     **m_sortEntry;
   CR_REFERENCE **m_sortReference;
   int m_sortArraySize;
   int CreateSortList(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CrossReference)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
  //afx_msg void OnColumnClick();
  afx_msg void OnColumnClick( NMHDR * pNotifyStruct, LRESULT * result );

  afx_msg void OnZeroReference();
  afx_msg void OnOneReference();
  afx_msg void OnTwoReference();
  afx_msg void OnFirstReference();
  afx_msg void OnExists();
  afx_msg void OnMissing();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnClickedExport();
};


