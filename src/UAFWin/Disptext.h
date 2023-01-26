/******************************************************************************
* Filename: Disptext.h
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
#ifndef __DISPTEXT_H__
#define __DISPTEXT_H__

#include "externs.h"
#include "char.h"
#include "TextForm.h"


// variables used to maintain the state of various item viewers
//const int ITEM_SEP = 200;
const int ITEMY = 60;
//const int ITEMX = 75;

extern int ItemsOnPage;
extern int startItem;
extern CString CombatMsg;
extern CString timeDelayMsg;
extern mCArray<RECT, RECT&> InventoryRects;

void setCharItemTextState(CHARACTER &dude, 
                          BOOL (*pFilter)(ITEM *pItem, void *pFilterPkt),
                          void *pFilterPkt,
                          int flags);
void setShopItemState(ITEM_LIST &list, costFactorType type=Normal);
void setSpellEffectItemState(CHARACTER &data);
void setSpellMemorizeItemState(CHARACTER &data);
void setSpellNewLevelItemState(CHARACTER &data);
void setSpellTempleCastItemState(CHARACTER &data, costFactorType type);
void setSpellCastItemState(CHARACTER &data, CASTING_ENVIRONMENT ce);
// PRS July 2009 void setSpellItemState(CHARACTER &cdata, spellBookType *list, DWORD flags, costFactorType type=Normal);
void setSpellItemState(CHARACTER &cdata, DWORD flags, costFactorType type=Normal);
void setTreasureEventState(GameEvent *event);


struct SELECTION_PAGE_FORMAT;

void clearCharItemState();
void clearTreasureItemState();
void clearShopItemState();
void clearSpellItemState();
void nextTreasurePage();
void nextCharItemsPage();
void nextShopPage();
void nextSpellPage(const SELECTION_PAGE_FORMAT *format);
void nextLinePage(const SELECTION_PAGE_FORMAT *format);
void prevTreasurePage();
void prevCharItemsPage();
void prevShopPage();
void prevSpellPage(const SELECTION_PAGE_FORMAT *format);
void prevLinePage(const SELECTION_PAGE_FORMAT *format);
int currSpellTextIndex();
int currLineTextIndex();
int currShopTextIndex();
int currCharItemsTextIndex();
int currTreasureTextIndex();
BOOL allowTreasureItemNextPage();
BOOL allowTreasureItemPrevPage();
BOOL allowCharItemNextPage();
BOOL allowCharItemPrevPage();
BOOL allowShopItemNextPage();
BOOL allowShopItemPrevPage();
BOOL allowSpellNextPage(const SELECTION_PAGE_FORMAT *format);
BOOL allowSpellPrevPage(const SELECTION_PAGE_FORMAT *format);
BOOL shouldHighlight(int index);

//void displayShop(void);
void resetShopState(SHOP &shop);
void displayCombatStats(int dude, int fontNumber);
//void displayTreasure();
//void displaySpells();
void displayPartyNames(PARTY &pty, bool useWX, 
                       bool showNPC, 
                       const SELECTION_PAGE_FORMAT *format,
                       int fontNumber);
//void displayCharacterItems(CHARACTER &dude);
void displayCharacterHP(CHARACTER &dude, int fontNumber);
void displayPoolGoldTotal(int fontNumber);


int IntersectPointWithParty(long x, long y);
int IntersectPointWithInventory(long x, long y);

#ifdef _DEBUG
void displayCursorStats(int fontNumber); // combat debug data
#endif


enum LINE_FORM_INPUT_MESSAGE_TYPE
{
  LINEFORM_initial,
  LINEFORM_refresh,
  //LISTFORM_mouseClick,
  //LISTFORM_key,      
  LINEFORM_exit      
};


int handleLineFormInput(LINE_FORM_INPUT_MESSAGE_TYPE msg,
                        LINE_TEXT_LIST &data,
                        int P1=0, 
                        int P2=0);

struct LINE_TEXT_LIST
{
  LINE_TEXT_LIST() { Clear(); }
  ~LINE_TEXT_LIST() { Clear(); }

  enum 
  {
    USE_SIMPLE_LAYOUT = 0x0001,
  };


  void Clear();
  int GetCount() const { return lines.GetSize(); }
  CString GetListIndex(int index) const { return lines[index]; };
  int GetListIndex(const CString& lineName);

  void            ToggleSelected              (int index);
  BOOL            IsSelected                  (int index);
  //int  NumSelected();

  void Add(CString line) { lines.Add(line);};
  const char *GetLineText(int index);
  const char *GetLineLine(int index);
  CString title;
  BOOL IsSimpleLayout(void) { return (m_flags & USE_SIMPLE_LAYOUT); };
  void UseSimpleLayout() {m_flags = USE_SIMPLE_LAYOUT;};
private:

  // this will contain either the SpellDbKey,
  // or a POSITION for access to character's spell book
  //
  // FillNewSpellLevelListText:  SpellDbKey
  // FillCastSpellListText:      character's spell book POS
  // FillTempleCastSpellListText character's spell book POS
  // FillMemorizeSpellListText:  character's spell book POS
  // FillSpellEffectsListText:   SpellDbKey
  // FillSpellListText:          SpellDbKey
  int m_flags;
  CArray<CString, CString&> lines;
};



#endif
