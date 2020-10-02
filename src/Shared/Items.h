/******************************************************************************
* Filename: Items.h
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
#ifndef __ITEMS_H__
#define __ITEMS_H__

//#include "externs.h"
#include "SpecAb.h"
//#include "Spell.h"
#include "SharedQueue.h"
#include "PicData.h"


class CAR;

const int COST            = 0x00000001;
const int RDY             = 0x00000002;
const int QTY             = 0x00000004;
const int DETECTING_MAGIC = 0x00000008;
const int MONEY           = 0x00000010;
const int SPECIAL_ONLY    = 0x00000020; // Special Items Only

const int MAX_ITEMS = 0x00FFFFFF;
const int NO_ITEM = -1;
const int NO_READY_ITEM = -1;
const int MAX_ITEM_NAME = 50;
const int MAX_ITEM_QTY = 65534; // max item qty in one item slot

enum itemRechargeRate 
{ 
  irrNever, // fixed qty of charges, once gone no 'Use' is possible
  irrDaily  // full qty of charges restored each day
};

enum weaponClassType 
{ 
                     // CanAttack?   AttackAdjacent?   AttackRanged?   NeedsAmmoReadied?  ConsumesAmmo?
                     //
  NotWeapon=0,       //   No              No               No                N/A              N/A
  HandBlunt=1,       //   Yes             Yes              Yes               No               No
  HandCutting=2,     //   Yes             Yes              Yes               No               No
  HandThrow=3,       //   Yes             Yes              Yes               No               if range > 1
  SlingNoAmmo=4,     //   Yes             No               Yes               No               No
  Bow=5,             //   Yes             No               Yes               Yes              Yes
  Crossbow=6,        //   Yes             No               Yes               Yes              Yes
  Throw=7,           //   Yes             No               Yes               No               Yes
  Ammo=8,            //   No              N/A              No                N/A              N/A
  SpellCaster=9,     //   Yes             Yes              Yes               No               Yes
  SpellLikeAbility=10//   Yes             YEs              Yes               No               Yes

  //
  //  CanAttack = isWeapon()
  //
  //  AttackRanged = WpnCanAttackAtRange(Range>1)
  //
  //  AttackAdjacent = WpnCanAttackAtRange(Range=1)
  //
  //  NeedsAmmoReadied = WpnUsesAmmo()
  //
  //  ConsumesAmmo = WpnConsumesAmmoAtRange(Range)
  //
};
const int MAX_WEAPON_CLASS = 11;

/*
enum itemLocationType 
{ 
  WeaponHand=0,  // Sword								
  ShieldHand=1,  // Shield
	BodyArmor=2,   // Armor
	Hands=3,       // Guantlets
	Head=4,        // Helm
	Waist=5,       // Belt
	BodyRobe=6,    // Robe
	Back=7,        // Cloak
	Feet=8,        // Boots
	Fingers=9,     // Rings
	AmmoQuiver=10, // Arrows, Bolts
  CannotBeReadied=11, // Potions, Necklaces, mirrors, etc
  Arms=12,
  Legs=13,
  Face=14,
  Neck=15,
  Pack=16,
}; 
const int MAX_LOCATION_CARRIED = 17;
*/


#define base38(a,b,c,d,e,f) ( (((((unsigned)a*38+b)*38+c)*38+d)*38+e)*38+f )
#define BASE38(a,b,c,d,e,f) (base38((a-'A'+12),(b-'A'+12),(c-'A'+12),(d-'A'+12),(e-'A'+12),(f-'A'+12)))

#define blank ('A'-11)

const DWORD WeaponHand = BASE38('W','E','A','P','O','N');
const DWORD ShieldHand = BASE38('S','H','I','E','L','D');
const DWORD BodyArmor  = BASE38('A','R','M','O','R',blank);
const DWORD AmmoQuiver = BASE38('Q','U','I','V','E','R');
const DWORD Hands      = BASE38('H','A','N','D','S',blank);
const DWORD Head       = BASE38('H','E','A','D',blank,blank);
const DWORD Waist      = BASE38('W','A','I','S','T',blank);
const DWORD BodyRobe   = BASE38('R','O','B','E',blank,blank);
const DWORD Back       = BASE38('C','L','O','A','K',blank);
const DWORD Feet       = BASE38('F','E','E','T',blank,blank);
const DWORD Fingers    = BASE38('F','I','N','G','E','R');
const DWORD Cannot     = BASE38('C','A','N','N','O','T');
const DWORD NotReady   = BASE38('N','O','T','R','D','Y');
const DWORD Arms       = BASE38('A','R','M','S',blank,blank);
const DWORD Legs       = BASE38('L','E','G','S',blank,blank);
const DWORD Face       = BASE38('F','A','C','E',blank,blank);
const DWORD Neck       = BASE38('N','E','C','K',blank,blank);
const DWORD Pack       = BASE38('P','A','C','K',blank,blank);
const DWORD Undefined  = BASE38('U','N','D','E','F',blank);



struct itemReadiedLocation
{
  //friend struct ITEM;
private:
  DWORD location;
public:
  itemReadiedLocation(DWORD n, const ITEM_ID& itemID, const ITEM_DATA **ppItemDataCache);
  itemReadiedLocation(void){location = NotReady;};
  void Clear(void){location=NotReady;};
  void CanNot(void){location = Cannot;};
  DWORD Synonym(DWORD rdyLoc) const;
  void CheckLocation(const ITEM_ID& itemID, 
                     const ITEM_DATA **ppItemDataCache);
  bool operator == (itemReadiedLocation rdyLoc) const
  {
    return location == rdyLoc.location;
  };
  bool operator == (DWORD rdyLoc) const
  {
    return location == rdyLoc;
  };
  bool operator != (itemReadiedLocation rdyLoc) const
  {
    return location != rdyLoc.location;
  };
  bool operator != (DWORD rdyLoc) const
  {
    return location != rdyLoc;
  };
  //void operator = (DWORD loc);
  DWORD Location(void) const {return location;};
  void Serialize(CAR& car);
  void Serialize(CArchive& car);
};



enum itemClassType { itemType=0, 
                     PlatinumType=1, 
                     ElectrumType=2, 
                     GoldType=3, 
                     SilverType=4, 
                     CopperType=5, 
                     GemType=6, 
                     JewelryType=7,
                     SpecialItemType=8, 
                     SpecialKeyType=9, 
                     QuestType=10,
                     CoinType6=12,
                     CoinType7=13,
                     CoinType8=14,
                     CoinType9=15,
                     CoinType10=16,
                     BogusItemType=11,
                     EquipmentSetType = 17
                    };
const int NUM_ITEMCLASS_TYPES = 18;

class READY_ITEMS //: public CObject
{
public:
//  DECLARE_SERIAL( READY_ITEMS )

  READY_ITEMS() { Clear(); }
  ~READY_ITEMS() { Clear(); }
  READY_ITEMS& operator=(const READY_ITEMS& src);
  BOOL operator==(const READY_ITEMS& src);
  READY_ITEMS(const READY_ITEMS& src) { *this = src; }
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
  void Export(JWriter& jw);
  void Clear();


/*************************************
 *  We used to keep track of the item readied at each possible 
 *  body location.  But the number of locations was enlarged.
 *  And, more importantly, the number of items that could be readied
 *  at each location was made more arbitrary.
 *  The information about 'readied' items is contained in the
 *  item list.  Recording it here was redundant.  We will continue
 *  to serialize this class just so we don't have to worry
 *  about forward/backward compatibility.  But we will search
 *  the item list itself to find readied items.
 */


  //BOOL UnReady(int item);
  //BOOL Ready(int item, itemLocationType type, BYTE hands);
  //BOOL ClearReady(int item, itemLocationType type, BYTE hands);
  //int  GetReady(itemLocationType type) const;
  //BOOL CanReady(itemLocationType type);


	int m_readyWeaponHand;
	int m_readyShieldHand;
	int m_readyMissileAmmo;
	int m_readyArmor;
	int m_readyGauntlets;
	int m_readyHelmet;
	int m_readyBelt;
	int m_readyRobe;
	int m_readyCloak;
	int m_readyBoots;
	int m_readyRing1;
	int m_readyRing2;
};

struct COMMON_ITEM
{
  DWORD FNV_hash; // was GLOBAL_ITEM_ID m_giID;  // exactly one int in size.
	int  qty;
	int  charges;
  int  paid;
	bool identified;
  BYTE cursed;
  // how much was paid for item (important for selling)
  // -1 means not purchased, use base item cost in database
};


struct ITEM;

/* Replaced with CELL_LEVEL_CONTENTS  20181216 PRS

struct DROPPED_ITEM
{
  union itemdata 
  {
    COMMON_ITEM commonItem;// type 1=normal item; Arrow, axe, boots, etc.
  } data;
  unsigned char itemtype;  
  //
  //
  //
  void Copy(const ITEM& item);
};
*/


struct ITEM 
{
  ITEM() { Clear(); }
  void Clear()
  { 
    key=0;
    //m_giID.Clear();
    itemID.Clear();
    readyLocation.Clear();
    //ready=FALSE;
    qty=0;
    identified=FALSE;
    charges=0;
    cursed=FALSE;
    paid=-1; 
    pItemData = NULL;
  };
  void Serialize(CArchive &ar, double ver);
  void Serialize(CAR &ar, double ver);
  itemReadiedLocation ReadyLocation(void) const;
  void ReadyLocation(DWORD rdyLoc);
  void ClearReadyLocation(void){readyLocation.Clear();};
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  bool Import(JReader& jw);
#endif

  BOOL operator==(const ITEM& src) const
  {
    if (&src==this) return TRUE;
    if (key != src.key) return FALSE;
    //if (m_giID != src.m_giID) return FALSE;
    if (itemID != src.itemID) return FALSE;
    //if (ready != src.ready) return FALSE;
    if (ReadyLocation() != src.ReadyLocation()) return FALSE;
    if (qty != src.qty) return FALSE;
    if (identified != src.identified) return FALSE;
    if (charges != src.charges) return FALSE;
    if (cursed != src.cursed) return FALSE;
    if (paid != src.paid) return FALSE;
    return TRUE;
  }
  int key;
	ITEM_ID itemID; // was GLOBAL_ITEM_ID m_giID;  // exactly one int in size.
private:
  itemReadiedLocation readyLocation;
  const ITEM_DATA *pItemData;  // Not Serialized.....temporary cache
  //DWORD readyLocation;
	//int ready;
public:
	int qty;
	int identified;
	int charges;
  BYTE cursed;
  // how much was paid for item (important for selling)
  // -1 means not purchased, use base item cost in database
  int paid; 
  bool m_scriptProcessed; // USed by the GPDL $ForEachItem() function
/*
#ifdef UAFEDITOR
  // Temporary for the editor when converting old designs.
  // Not serialized.
  int preVersionSpellNames_giID;   // Old GLOBAL_ITEM_ID
#endif
*/
};

//extern CArchive& operator <<( CArchive& ar, const ITEM& slot );
//extern CArchive& operator >>( CArchive& ar, ITEM& slot );

class ITEM_LIST //: public CObject
{

  READY_ITEMS rdyItems_Deprecated;  // Each item in itemData has a 'readied location'.
                                    // This allows us to have an indeterminate number of locations.
                                    // Locations are identified by base 38 name.
	OrderedQueue<ITEM, MAX_ITEMS> m_items;
  bool itemsModified;  // If list order changed or items added or deleted.
  int GetNextKey();
  //void SetReady(int index, BOOL ready);
  void SetReady(int index, DWORD rdyLoc);

public:
  //static BOOL IsMoneyItem(GLOBAL_ITEM_ID giID) { return giID.IsMoney(); }
  static BOOL IsMoneyItem(const ITEM_ID& itemID)
  { 
    return itemID.IsMoney();      ;
  };

  void Modified(bool b) {itemsModified = b;};
  bool IsModified(void) { return itemsModified;};

//  DECLARE_SERIAL( ITEM_LIST )
  ITEM_LIST() { Clear(); }
  ITEM_LIST(const ITEM_LIST& src) 
  { 
    *this = const_cast<ITEM_LIST&>(src); 
  }
  ~ITEM_LIST() { Clear(); }
  void Clear() 
  { 
    if (GetCount()) itemsModified = true; 
    m_items.RemoveAll(); 
    rdyItems_Deprecated.Clear(); 
  }  ;
  int GetCount() const { return m_items.GetCount(); }
  void Serialize(CArchive &ar, double ver);
  void Serialize(CAR &ar, double ver);
#ifdef UAFEDITOR
  void Export(JWriter& jw, const char *name);
  bool Import(JReader& jr, const char *name);
#endif
  void CrossReference(CR_LIST *pCRList, const CR_REFERENCE *pCRReference) const;

  POSITION GetHeadPosition() const { return m_items.GetHeadPosition(); }
  const ITEM *PeekFirst(POSITION& pos) const
  {
    pos = GetHeadPosition();
    return (pos==NULL)?NULL:&PeekAtPos(pos);
  }

        ITEM& GetAtPos (POSITION pos)       { return m_items.GetAtPos(pos); }
  const ITEM& PeekAtPos(POSITION pos) const { return m_items.PeekAtPos(pos); }


  ITEM& GetNext(POSITION &pos) { return m_items.GetNext(pos); }
  const ITEM *PeekNext(POSITION &pos) const 
  {
    if (pos == NULL) return NULL;
    return & (const_cast <ITEM_LIST*>(this)->m_items.GetNext(pos)); 
  };
  int GetKeyAt(POSITION pos) { return m_items.GetKeyAt(pos); }

  BOOL ValidItemListIndex(int index);
  // called when loading from scripts
  int AddItem(const ITEM &data, BOOL AutoJoin=TRUE);
  // called when loading binary data that
  // was saved with key info
  int AddItemWithCurrKey(const ITEM &data);
  void Append(ITEM_LIST& newItems);
  BOOL DeleteItem(int index);
  BOOL GetItem(int index, ITEM &data);
  BOOL SetItem(int index, ITEM &data);
  ITEM_LIST& operator=(const ITEM_LIST& src);
  BOOL operator==(const ITEM_LIST& src) const;
  //BOOL CanReady(int item, BYTE ItemMask, CHARACTER *pChar);
  miscErrorType CanReadyKey(int itemKey, CHARACTER *pChar) const;
  miscErrorType CanReadyItem(const ITEM *pItem, CHARACTER *pChar) const;
  BOOL CanReady(DWORD rdyLoc, CHARACTER *pChar, const ITEM *pItem) const;
  BOOL CanUnReady(int itemKey);
  BOOL UnReady(int itemKey);
  //BOOL ClearReadyByType(itemLocationType type);
  //BOOL Ready(int item, BYTE ItemMask, CHARACTER *pChar);
  BOOL Ready(int itemKey, CHARACTER *pChar, DWORD rdyLoc);
  //BOOL Ready(const ITEM_ID& itemID, itemReadiedType irt, BYTE itemMask, CHARACTER *pChar);
  BOOL Ready(const ITEM_ID& itemID, DWORD rdyLoc, CHARACTER *pChar);
  int  GetReadiedCount(DWORD rdyLoc) const;

  // GetReady is deprecated.......
  // Body locations can have several items 'readied'.  GetReady() does not
  // deal with this.  GetReady() has been replaced by GetReadiedItem().
  //int  GetReady(itemLocationType type) const;
  //int  GetReadiedItem(itemReadiedLocation rdyLoc, int readiedItemCount) const;
  int  GetReadiedItem(DWORD               rdyLoc, int readiedItemCount) const;
  BOOL IsReady(int index) const;
  BOOL IsCursed(int index) const;
  void RemoveCurse(int index);
  //BOOL HaveItem(GLOBAL_ITEM_ID giID);
  BOOL HaveItem(const ITEM_ID& itemID);
  ITEM           *GetITEM(int index);
  //GLOBAL_ITEM_ID  GetItem(int index)const;
  ITEM_ID  GetItem(int index) const;
  //int GetListKeyByItemKey(GLOBAL_ITEM_ID giID) const;
  int GetListKeyByItemName(const ITEM_ID& itemID) const;
  BOOL GetId(int index) const;
  int GetQty(int index) const;
  int GetCharges(int index) const;
  int GetPurchasePrice(int index) const;
  void SetPurchasePrice(int index, int price);
  CString GetAmmoClass(int index) const;
  void SetId(int index, BOOL id);
  void SetQty(int index, int qty);
  int  AdjustQty(int index, int qty);
  void SetCharges(int index, int chrgs);
  void ResetItemCharges(BOOL IsNewDay);

  void halveItem(int index);
  void joinItems(int index);
  //int findItem(GLOBAL_ITEM_ID giID, int index) const;
  int findItem(const ITEM_ID& itemID, int index) const;
  //BOOL addItem(GLOBAL_ITEM_ID giID, int qty, int numCharges, BOOL id, int cost);
  BOOL addItem(const ITEM_ID& itemID, int qty, int numCharges, BOOL id, int cost);
  BOOL deleteItem(int index, int qty);

  int GetProtectModForRdyItems() const;
  int GetAttackModForRdyItems() const;
};

struct ITEM_TEXT 
{
  ITEM_TEXT() 
  { 
    origIndex=-1;
    //m_giID.Clear();
    itemID.Clear();
    charges=0;
    qty=0;
    data=-1;
    identified=FALSE;
    ready=FALSE;
    label="";
    itemClass=BogusItemType; 
  };
	int origIndex;
  //GLOBAL_ITEM_ID m_giID;  //A single int
  ITEM_ID itemID;
  int charges;
  int qty;
  int data;
  BOOL identified;
  BOOL ready;
  itemClassType itemClass; 
	CString label;
};

class ITEM_DATA;

struct ITEM_TEXT_LIST 
{
  ITEM_TEXT_LIST() { Clear(); }
  ~ITEM_TEXT_LIST() { items.RemoveAll(); }

  void Clear()
  {
    title = "";
    items.RemoveAll();
    UseRdy = TRUE;
    UseQty = TRUE;
    UseCost = FALSE;
    UseMoney=FALSE;
    DetectingMagic = FALSE;
    CostFactor=Normal;
    RdyOffset=0;
    QtyOffset=0;
    CostOffset=0;
    NameOffset=0;
  }

  //void UpdateQty(int index, int qty) { items[index].qty += qty; if (items[index].qty <= 0) Remove(index); }
  void UpdateIdentified(int index, BOOL id) { items[index].identified = id; }
  void UpdateCharges(int index, int charges) { items[index].charges += charges; }
  void UpdateIsReady(int index, BOOL ready);
  void UpdateDetectMagic(BOOL detecting);

  int GetCount() const { return items.GetSize(); }
  //GLOBAL_ITEM_ID GetItemIndex(int index) 
  //{ 
  //  ASSERT(items[index].m_giID.IsValidItem());
  //  return items[index].m_giID;
  //}
  ITEM_ID GetItemID(int index)
  {
    return items[index].itemID;
  };
  int GetCharIndex(int index) const { return items[index].origIndex; }
  int GetItemQty(int index) const { return items[index].qty; }
  int GetItemCharges(int index) const { return items[index].charges; }
  int GetItemIdentified(int index) const { return items[index].identified; }
  int GetItemData(int index) const { return items[index].data; }

  itemClassType GetItemClass(int index) const 
  { 
    if ((index >= 0) && (index < GetCount()))
      return items[index].itemClass;
    else
      return BogusItemType;
  }

  BOOL ItemCanBeHalved(int index);
  BOOL ItemCanBeJoined(int index);

  void FillItemListText(ITEM_LIST &list, 
                        UINT flags, 
                        costFactorType type=Normal,
                        BOOL (*filter)(ITEM *pItem, void *filterPkt)=NULL, 
                        void *filterPkt=NULL);
  void AddItemListText(ITEM_LIST &list, UINT flags, costFactorType type=Normal, int data=-1);
  void AddItemText(ITEM &addItem, UINT flags, costFactorType type=Normal);

  void Add(int qty, 
           itemClassType type, 
           const char *label, 
           int origIndex=0, 
           BOOL id=FALSE, 
           int charges=0, 
           //GLOBAL_ITEM_ID giID=GLOBAL_ITEM_ID(), 
           const ITEM_ID& itemID = ITEM_ID(),
           BOOL ready=FALSE, 
           int data=-1);
  //void Remove(int index) { items.RemoveAt(index); }

  char *GetItemQtyText(int index);
  char *GetItemRdyText(int index);
  char *GetItemCostText(int index);
  char *GetItemNameText(int index);
  char *GetItemText(int index);

  void FormatItemListTitle(const char *EmptyListTitle=NULL);

  BOOL UseRdy;
  BOOL UseQty;
  BOOL UseCost;
  BOOL UseMoney;
  BOOL DetectingMagic;
  costFactorType CostFactor;

  int RdyOffset;
  int QtyOffset;
  int CostOffset;
  int NameOffset;

  enum { MAX_ITEM_TEXT_LEN = MAX_ITEM_NAME+100 };
	char temp[MAX_ITEM_TEXT_LEN+1];
  char text[MAX_ITEM_TEXT_LEN+1];

  CString title;
  CArray<ITEM_TEXT, ITEM_TEXT&> items;
};


extern ITEM_TEXT_LIST itemListText;
extern ITEM_TEXT_LIST charItemListText;
extern ITEM_TEXT_LIST treasureItemListText;
extern ITEM_TEXT_LIST scriptChoiceListText;

class CR_LIST;
struct CONFIG_PKT;

class ITEM_DATA //: public CObject
{
friend BOOL ImportUAItemsToUAF(const char *path);
friend static void DitemScribable(CONFIG_PKT *pkt);
friend static void DitemDroppable(CONFIG_PKT *pkt);
friend class CMagicalItemProp;
friend class ITEM_DATA_TYPE;

public:
//  DECLARE_SERIAL( ITEM_DATA )
    ITEM_DATA(void);
  ITEM_DATA(ITEM_DATA &src) :
  // ASLs are named "ITEM_DATA_ATTRIBUTES"
    item_asl(),
    temp_asl(),
	  hHitSound(-1), 
    hMissSound(-1), 
    hLaunchSound(-1), 
    specAbs(true) { *this = src; };

  ~ITEM_DATA() { ClearSounds(); Clear(TRUE); }

  BOOL IsValidItem(void) const;
  void Serialize(CArchive &ar, double ver);
  void Serialize(CAR &ar, double ver);
  void Save(CAR& car) const;
  void PrepareForRestore(void);
  void Restore(CArchive& ar);
  void Restore(CAR& car);
  void CommitRestore(void);

  ActorType GetContext() const;

#ifdef UAFEDITOR
  CString GetItemEventText();
  void CrossReference(CR_LIST *pCRList) const;
#endif

  void SetASL(CString key, CString value, BYTE flags);
  BOOL GetASL(CString key, CString &value) const;

  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring, double version);

  void Clear(BOOL ctor=FALSE);

  ITEM_DATA& operator =(const ITEM_DATA& src);
  bool operator == (const ITEM_DATA& src) const;
  int  GetDataSize(void) const;
  void PlayHit();
  void PlayMiss();
  void PlayLaunch();
  void ClearSounds();
  void LoadArt();
  void ClearArt(); // release surface and clear pic data
  void FreeArt(); // release surface

#ifdef UAFEngine
  void CastSpell(int spell) const;
  void SpellActivate(const PENDING_SPELL &data) const;
#endif

  //GLOBAL_ITEM_ID m_giID;

  //*********************************************************************************
  // In the editor, uniqueName means the fully qualified name sith                 **
  // optional modifier.  Example: BatttleAxe|Dull // PRS 20110107  Battle Axe(Dull)**.
  // When we write the binary items.dat file, the modifiers                        **
  // are taken off.                                                                **
  //                                                                               **
  // PRS 20110107                                                                  **
  // Sorry about that.  We shall be using the unique names                         **
  // in scripts.  Scripts need to uniquely fetch and compare names.                **
  // When we load the items database we will construct a 'common name' by          **
  // removing the vertal bar and qualifier from the unique name                    **
  // What we are going to do here is to make the name a private member,            **
  // provide a function to get the unique name and a function                      ** 
  // to get the decorated name.                                                    ** 
private:                                                                 //        **
  CString  m_uniqueName;                                                 //        **
	CString  m_commonName;                                                 //        **
  //CString  Id_Name;                                                    //        **
  CString  m_idName;                                                     //        **
public:                                                                  //        **
  //CString GetItemName() const;                                         //        **
  CString  IdName(void)const{return m_idName;};                          //        **
  void     SetIdName(const CString& name);                               //        **
  CString  CommonName(void)const{return m_commonName;}; //Not serialized.//        **
  DWORD    FNV_Hash; // Not Serialized....Computed when initialized.               **
  inline   CString  UniqueName(void)const{return m_uniqueName;};         //        **
  void     SetUniqueName(const CString& name);                           //        **
  ITEM_ID  ItemID(void)  const {ITEM_ID x; x=m_uniqueName;return x;};    //        **
  SPELL_ID SpellID(void) const {return spellID;};                        //        **
  void     SpellID(const SPELL_ID& s){spellID = s;};                     //        **
  int      preSpellNameKey; // See comment at defintion of VersionSaveIDs          **
#ifdef UAFEDITOR
  int      preSpellNamesUsability;
#endif
  //*********************************************************************************

#ifdef OldDualClass20180126
  BOOL IsUsableByClass(const CLASS_ID& classID) const;
#else
  BOOL IsUsableByClass(const CHARACTER *pChar) const;
#endif
#ifdef OldDualClass20180126
  BOOL IsUsableByPrevClass(const CHARACTER *pChar) const;
#endif
  BOOL IsUsableByBaseclass(const BASECLASS_ID& baseclassID) const;


  CString  AmmoType;
  CString  HitSound;
  CString  MissSound;
  CString  LaunchSound;
  int   hHitSound;
  int   hMissSound;
  int   hLaunchSound;

  long  Experience;
  long	Cost;
	long	Encumbrance;
	long	Attack_Bonus;
	BOOL	Cursed;
	long	Bundle_Qty;
	long	Num_Charges;
	DWORD	Location_Readied;  // A base-38 name!
	long	Hands_to_Use;  
	long	Dmg_Dice_Sm;
	long	Nbr_Dice_Sm;
	long	Dmg_Bonus_Sm;
	long	Dmg_Dice_Lg;
	long	Nbr_Dice_Lg;
	long	Dmg_Bonus_Lg;
	double ROF_Per_Round;
  long  Protection_Base;
	long	Protection_Bonus;
  weaponClassType	Wpn_Type;
  int   m_priorityAI; // Not serialized.  Initialized from Special Ability.


private:
  int   m_usageFlags;
enum 
{
  USAGE_usable     = 0x00000001,
  USAGE_scribable  = 0x00000002,
  USAGE_notMagical = 0x00000004,
};
public:
  inline BOOL IsUsable(void) const{return (m_usageFlags&USAGE_usable)!=0;};
  inline void IsUsable(BOOL usable){if(usable)m_usageFlags|=USAGE_usable;else m_usageFlags&=~USAGE_usable;};
  inline BOOL IsScribable(void) const{return (m_usageFlags&USAGE_scribable)!=0;};
  inline BOOL IsNotMagical(void) const {return (m_usageFlags&USAGE_notMagical)!=0;};
  inline void IsNotMagical(BOOL t){if(t)m_usageFlags|=USAGE_notMagical;else m_usageFlags&=~USAGE_notMagical;};
private:
  inline void IsScribable(BOOL scribable){if(scribable)m_usageFlags|=USAGE_scribable;else m_usageFlags&=~USAGE_scribable;};


public:
  BOOL  CanBeHalvedJoined;
  BOOL  CanBeTradeDropSoldDep;
	//long	Usable_by_Class;
  mCArray<BASECLASS_ID, BASECLASS_ID&> usableByBaseclass;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Baseclass,           /* array data name      */ \
                                  BASECLASS_ID,        /* Search variable type */ \
                                  baseclassID,         /* search variable name */ \
                                  usableByBaseclass,   /* array name           */ \
                                  BASECLASS_ID,        /* array data type      */ \
                                  ITEM_DATA)           /* type of *this        */



#ifdef UAFEDITOR
  void AddUsableBaseclass(int usableFlags, 
                          int baseclassFlag, 
                          int baseclassType, 
                          const CString& name,
                          double ver);
#endif


	long	RangeMax;
#ifdef UAFEngine
  long  RangeShort; // Computed...not serialized
  long  RangeMedium; // Computed...not serialized
#endif
  //GLOBAL_SPELL_ID  m_gsID;
  SPELL_ID spellID;
  //long  Spell_Level;
  //long  Spell_Class;
  DWORD m_useEvent;
  DWORD ExamineEvent;
  CString ExamineLabel;
  CString attackMsg;
  itemRechargeRate Recharge_Rate;
  BOOL IsNonLethal;
  SPECIAL_ABILITIES specAbs;
  CString RunItemScripts(LPCSTR     scriptName, 
                         CBRESULT (*fnc)(CBFUNC func, CString *scriptResult, void *pkt), 
                         void      *pkt,
                         LPCSTR     comment) const
  {
        return specAbs.RunScripts(
                            scriptName, 
                            fnc, 
                            pkt,
                            comment,
                            ScriptSourceType_Item,
                            m_uniqueName);
  };
  
  PIC_DATA  MissileArt; // in-route sprite
  PIC_DATA  HitArt;     // target hit sprite

  A_ASLENTRY_L item_asl;
  A_ASLENTRY_L temp_asl;
#ifdef UAFEDITOR
  // These temporary members are not serialized.
  // They are for the editor when reading old designs that
  // contain GLOBAL_SPELL_ID and GLOBAL_ITEM_ID
//  int preVersionSpellNames_giID;  // Our old GLOBAL_ITEM_ID
  int preVersionSpellNames_gsID;  // The old GLOBAL_SPELL_ID
#endif
};

class CR_LIST;

class ITEM_DATA_TYPE //: public CObject
{
  //POSITION pos;
  //CCriticalSection m_CS;
  //OrderedQueue<ITEM_DATA, MAX_ITEMS> ItemData;
  //CList<ITEM_DATA, ITEM_DATA&> ItemData;
  mCArray<ITEM_DATA, ITEM_DATA&> ItemData;
  //int GetNextKey();

public:

  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Item,           /* array data name      */ \
                                  ITEM_ID,        /* Search variable type */ \
                                  itemID,         /* search variable name */ \
                                  ItemData,       /* array name           */ \
                                  ITEM_DATA,      /* array data type      */ \
                                  ITEM_DATA_TYPE) /* type of *this        */

  

  int GetCount(void) const {return ItemData.GetSize();};
//  DECLARE_SERIAL( ITEM_DATA_TYPE )
  ITEM_DATA_TYPE() { Clear(); }
  ~ITEM_DATA_TYPE() { Clear(); }
  void Clear();
  ITEM_DATA_TYPE& operator =(ITEM_DATA_TYPE& src) ;
  bool operator == (ITEM_DATA_TYPE& src) const;
  
  void Serialize(CArchive &ar);  
  void Serialize(CAR &ar);
  void Save(CAR& car) const;
  void Restore(CArchive& ar);
  void Restore(CAR& car);
  void CommitRestore(void);
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring, double version);

  //DWORD GetCount() { CSingleLock sLock(&m_CS, TRUE); return ItemData.GetCount(); }
  int GetDataSize(void) ;
  void SaveSounds();
  void SaveArt() ;

  void ExpandDefaultFilenames();
#ifdef UAFEDITOR
  void CollapseDefaultFilenames();
#endif

  //POSITION GetHeadPosition() { return ItemData.GetHeadPosition(); }
  //POSITION LocateItemData(const ITEM_ID& itemID) const;

  //ITEM_DATA& GetAtPos (POSITION pos)       { return ItemData.GetAtPos(pos); }
  //ITEM_DATA& GetAtPos (POSITION pos)       { return ItemData.GetAt(pos); }
  //ITEM_DATA *GetItemData(const ITEM_ID& itemID);
  //const ITEM_DATA& PeekAtPos(POSITION pos) const { return ItemData.PeekAtPos(pos); }
  //const ITEM_DATA *PeekItemData(POSITION pos) const { return &(const_cast <ITEM_DATA_TYPE *>(this)->ItemData.GetAt(pos)); }
  //const ITEM_DATA *PeekNext (POSITION& pos) const { return &(const_cast <ITEM_DATA_TYPE *>(this)->ItemData.GetNext(pos));};

  //ITEM_DATA& GetNext(POSITION &pos) { return ItemData.GetNext(pos); }
  //GLOBAL_ITEM_ID GetKeyAt(POSITION pos) { return ItemData.GetKeyAt(pos); }
  //ITEM_ID GetIDAt(POSITION pos) const {return PeekItemData(pos)->ItemID(); };

  BOOL Validate(const ITEM_DATA &data, FILE *pFile) const;
  //BOOL Validate(GLOBAL_ITEM_ID giID, FILE *pFile) const; // specific item
  BOOL Validate(const ITEM_ID& itemID, FILE *pFile) const; // specific item
  BOOL Validate(FILE *pFile); // all items
  void CrossReference(CR_LIST *pCRList);
  //void PlayHit    (GLOBAL_ITEM_ID giID);
  void PlayHit    (const ITEM_ID& itemID);
  //void PlayMiss   (GLOBAL_ITEM_ID giID);
  void PlayMiss   (const ITEM_ID& itemID);
  //void PlayLaunch (GLOBAL_ITEM_ID giID);
  void PlayLaunch (const ITEM_ID& itemID);
  void ClearSounds();
  void FreeArt();
  /*
  CString GetItemUniqueName (GLOBAL_ITEM_ID giID) const;
  CString GetItemCommonName (GLOBAL_ITEM_ID giID) const;
  CString GetItemIdName     (GLOBAL_ITEM_ID giID) const;
  long    GetItemCost       (GLOBAL_ITEM_ID giID) const;
  long    GetItemExpWorth   (GLOBAL_ITEM_ID giID) const;
  double  GetItemROF        (GLOBAL_ITEM_ID giID) const;
  long    GetItemRange      (GLOBAL_ITEM_ID giID) const;
  long    GetItemBundleQty  (GLOBAL_ITEM_ID giID) const;
  BOOL    GetItemUsable     (GLOBAL_ITEM_ID giID) const;
  CString GetAmmoClass      (GLOBAL_ITEM_ID giID) const;
  weaponClassType GetWpnType(GLOBAL_ITEM_ID giID) const;
  BOOL HaveItem             (GLOBAL_ITEM_ID giID) const; 
  */
  CString GetItemUniqueName (const ITEM_ID& itemID) const;
  CString GetItemCommonName (const ITEM_ID& itemID) const;
  CString GetItemIdName     (const ITEM_ID& itemID) const;
  long    GetItemCost       (const ITEM_ID& itemID) const;
  long    GetItemExpWorth   (const ITEM_ID& itemID) const;
  double  GetItemROF        (const ITEM_ID& itemID) const;
  long    GetItemRange      (const ITEM_ID& itemID) const;
  long    GetItemBundleQty  (const ITEM_ID& itemID) const;
  BOOL    GetItemUsable     (const ITEM_ID& itemID) const;
  CString GetAmmoClass      (const ITEM_ID& itemID) const;
  weaponClassType GetWpnType(const ITEM_ID& itemID) const;
  BOOL HaveItem             (const ITEM_ID& itemID) const; 
  // called when loading from scripts, or
  // when adding from editor GUI
  //GLOBAL_ITEM_ID AddItem(ITEM_DATA &data);
  int AddItem(const ITEM_DATA &data);
  // called when loading binary data that
  // was saved with key info
  //GLOBAL_ITEM_ID AddItemWithCurrKey(ITEM_DATA &data);
/*
  BOOL SetItem          (GLOBAL_ITEM_ID giID, ITEM_DATA &data);
  BOOL RemoveItem       (GLOBAL_ITEM_ID giID);
  BOOL DeleteItemEvents (GLOBAL_ITEM_ID giID);
  BOOL GetItemAttribute (GLOBAL_ITEM_ID giID, CString key, CString value) const;
  BOOL SetItemAttribute (GLOBAL_ITEM_ID giID, CString key, CString value);
  //BOOL GetItemData(int index, ITEM_DATA &data);
  ITEM_DATA *GetItemData(GLOBAL_ITEM_ID giID);
  */
  BOOL SetItem          (const ITEM_ID& itemID, ITEM_DATA &data);
  BOOL RemoveItem       (const ITEM_ID& itemID);
  BOOL DeleteItemEvents (const ITEM_ID& itemID);
  BOOL GetItemAttribute (const ITEM_ID& itemID, CString key, CString value) const;
  BOOL SetItemAttribute (const ITEM_ID& itemID, CString key, CString value);
  //BOOL GetItemData(int index, ITEM_DATA &data);
  //ITEM_DATA *GetItemData(const ITEM_ID& itemID);
  //const ITEM_DATA *PeekItemData(const ITEM_ID& itemID) const
  //{
  //  POSITION pos;
  //  pos = LocateItemData(itemID);
  //  if (pos == NULL) return NULL;
  //  return PeekItemData(pos);
  //};

  
  //BOOL IsValidItem(const ITEM_ID& itemID);

   ActorType GetContext(int index);
#ifdef UAFEngine
  //void CastSpell(int index, int spell) const;
  void CastSpell(const ITEM_ID& itemID, int spell) const;
  //void SpellActivate(int index, const PENDING_SPELL &data) const;
  void SpellActivate(const ITEM_ID& itemID, const PENDING_SPELL &data) const;
#endif

  void AddAmmoType(CString data);
  void RemoveAmmoType(CString data);
  int HaveAmmoType(CString data);
  BOOL HaveItemThatUsesAmmoType(CString data) const;

  CArray<CString, CString> m_AmmoTypes;
  double version;

private:
  // No locking... for internal use
  //GLOBAL_ITEM_ID m_FindName (const CString& name) const;
  //CString m_FindName (const CString& name) const;

public:
  // With locking...for public use
  //GLOBAL_ITEM_ID FindName(const CString& name);
  //CString FindName(const CString& name);

#ifdef UAFEDITOR
  // Special goodies to convert old designs
  ITEM_ID FindPreVersionSpellNamesItemID(int preVersionSpellNames_giID) const;
  void FixPreSpellNamesUsability(void);
#endif

};


//int getItemEncumbrance(GLOBAL_ITEM_ID giID, int qty);
int getItemEncumbrance(const ITEM_ID& itemID, int qty);

// uses known item weapon type
BOOL isWeapon(weaponClassType type);
BOOL WpnUsesAmmo(weaponClassType type);
BOOL WpnConsumesSelfAsAmmo(weaponClassType type);

// checks by item class type
BOOL itemCanBeTraded(itemClassType type);
BOOL itemCanBeDropped(itemClassType type);
BOOL itemCanBeDeposited(itemClassType type);
BOOL itemCanBeJoined(itemClassType type);
BOOL itemCanBeHalved(itemClassType type);
BOOL itemCanBeReadied(itemClassType type);
BOOL itemIsMoney(itemClassType type); 
BOOL itemCanBeSold(itemClassType type);

// checks by item database key
/*
BOOL itemCanBeTraded    (GLOBAL_ITEM_ID giID);
BOOL itemCanBeDropped   (GLOBAL_ITEM_ID giID);
BOOL itemCanBeDeposited (GLOBAL_ITEM_ID giID);
BOOL itemCanBeSold      (GLOBAL_ITEM_ID giID);
BOOL itemCanBeJoined    (GLOBAL_ITEM_ID giID);
BOOL itemCanBeHalved    (GLOBAL_ITEM_ID giID);
BOOL itemCanBeReadied   (GLOBAL_ITEM_ID giID);
BOOL itemUsesRdySlot    (GLOBAL_ITEM_ID giID);

BOOL isWeapon               (GLOBAL_ITEM_ID giID);
BOOL WpnUsesAmmo            (GLOBAL_ITEM_ID giID);
BOOL isMagical              (GLOBAL_ITEM_ID giID);
BOOL WpnCanAttackAtRange    (GLOBAL_ITEM_ID giID, int Range);
BOOL WpnConsumesAmmoAtRange (GLOBAL_ITEM_ID giID, int Range);
BOOL WpnConsumesSelfAsAmmo  (GLOBAL_ITEM_ID giID);
*/
BOOL itemCanBeTraded    (const ITEM_ID& itemID);
BOOL itemCanBeDropped   (const ITEM_ID& itemID);
BOOL itemCanBeDeposited (const ITEM_ID& itemID);
BOOL itemCanBeSold      (const ITEM_ID& itemID);
BOOL itemCanBeJoined    (const ITEM_ID& itemID);
BOOL itemCanBeHalved    (const ITEM_ID& itemID);
BOOL itemCanBeReadied   (const ITEM_ID& itemID);
BOOL itemUsesRdySlot    (const ITEM_ID& itemID);

BOOL isWeapon               (const ITEM_ID& itemID);
BOOL WpnUsesAmmo            (const ITEM_ID& itemID);
BOOL isMagical              (const ITEM_ID& itemID);
BOOL WpnCanAttackAtRange    (const ITEM_ID& itemID, int Range);
BOOL WpnConsumesAmmoAtRange (const ITEM_ID& itemID, int Range);
BOOL WpnConsumesSelfAsAmmo  (const ITEM_ID& itemID);
// checks by item database record
BOOL itemUsesRdySlot(const ITEM_DATA *pData);

struct ITEM_CONTEXT
{
  //GLOBAL_ITEM_ID m_giID;
  ITEM_DATA *m_pItemData;
  CString Name(void);
  ITEM_CONTEXT(void){Clear();};
  void Clear(void)
  {
    //m_giID.Clear();
    m_pItemData = NULL;
  };
};


//extern ITEM_CONTEXT itemContext;
//void SetItemContext(GLOBAL_ITEM_ID giID);
//void ClearItemContext(void);


#endif