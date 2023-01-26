/******************************************************************************
* Filename: Disptext.cpp
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

//#include "externs.h"
#include "Dungeon.h"


#include "items.h"
#include "char.h"
#include "class.h"
#include "level.h"
#include "GlobalData.h"
#include "GameEvent.h"
#include "party.h"
#include "Graphics.h"
#include "DispText.h"
//#include "GameMenu.h"
#include "combatants.h"
#include "DrawTile.h"
#include "monster.h"
#include "treas.h"
#include "Viewport.h"
#include "FormattedText.h"
//#include "Getinput.h"

extern int DEFAULT_COMBAT_STAT_X;
extern int DEFAULT_COMBAT_STAT_Y;



#ifdef TRACE_TIMER_DETAILS
class DETAIL_TRACE
{
  FILE *f;
public:
  DETAIL_TRACE(void);
  ~DETAIL_TRACE(void);
//  DWORD    m_id[1000];
//  DWORD    m_data[1000];
//  DWORD    m_time[1000];
//  DWORD    m_virtualTime[1000];
//  int      m_nextIndex;
  void     trace (DWORD id, DWORD data);
};
extern DETAIL_TRACE detailTrace;
#endif


#ifdef TRACE_TIMER_DETAILS
#define DETAILTRACE(a,b) detailTrace.trace((a),(b));
#else
#define DETAILTRACE(a,b)
#endif






#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int ItemsOnPage = 0;
int startItem;

mCArray<RECT, RECT&> InventoryRects;
RECT PartyNameRects[MAX_PARTY_MEMBERS];

CString CombatMsg;
CString timeDelayMsg; // appears in menu-bar area for x seconds

// error strings will be wrapped to fit message box, but
// make sure they fit in one box. There is no processing
// to let the player "Press Return to Continue" to see
// multiple boxes of error text continuation.
//
char *miscErrorText[MAX_MISC_ERROR_MSGS] = 
{
   "No Error",
   "UNKNOWN ERROR",
   "THE CHARACTER IS DEAD",
   "THE CHARACTER IS UNCONSCIOUS",
   "THE CHARACTER HAS FLED",
   "THE CHARACTER IS PETRIFIED",
   "THE CHARACTER IS GONE",
   "THE CHARACTER IS ANIMATED",
   "THE CHARACTER IS TEMPORARILY GONE",
   "THE CHARACTER IS RUNNING AWAY",
   "THE CHARACTER IS DYING",
   "NO MONEY",
   "NOT ENOUGH MONEY",
   "YOU HAVE THE MAXIMUM ITEM TYPES THAT YOU CAN CARRY",
   "YOU ARE NOT STRONG ENOUGH TO CARRY ANY MORE",
   "THIS ITEM TAKES TWO HANDS TO USE",
   "NOT USABLE BY YOUR CLASS",
   "A SIMILAR ITEM IS ALREADY READIED",
   "NO ICONS AVAILABLE",
   "NO SMALL PICTURES AVAILABLE",
   "NO DESIGNS IN DIRECTORY",
   "FAILED TO LOAD LEVEL FROM DISK",
   "INVALID GAME STATE ENCOUNTERED",
   "INVALID EVENT TYPE ENCOUNTERED",
   "INVALID MENU STATE ENCOUNTERED",
   "INVALID TASK STATE ENCOUNTERED",
   "YOUR PARTY CANNOT CONTAIN MORE CHARACTERS",
   "YOUR PARTY CANNOT CONTAIN MORE NPC's",
   "ITEM MUST NOT BE READIED",
   "SOME MONSTER COMBAT ICONS CANNOT BE FOUND",
   "GENERAL GRAPHICS ERROR",
   "DESIGN VERSION HIGHER THAN RUNTIME, LOAD FAILED, PLEASE UPGRADE",
   "SAVED GAME VERSION HIGHER THAN RUNTIME, LOAD FAILED, PLEASE UPGRADE",
   "SAVED CHAR VERSION ERROR, LOAD FAILED",
   "UNABLE TO LOAD FILE",
   "UNABLE TO SAVE FILE",
   "A CHARACTER WITH THIS NAME ALREADY EXISTS",
   "NOT ENOUGH FREE HANDS",
   "CHARACTER COULD NOT BE CREATED",
   "LastErrorType Placeholder"
};

miscErrorType miscError = NoError;
CString errorText;




//*****************************************************************************
// NAME: clearCharItemState
//
// PURPOSE:  
//*****************************************************************************
void clearCharItemState()
{
   startItem = 0;   
   party.activeItem = 0;
   ItemsOnPage = 0;
   charItemListText.Clear();
}


//*****************************************************************************
// NAME: clearTreasureItemState
//
// PURPOSE:  
//*****************************************************************************
void clearTreasureItemState()
{
   treasureItemListText.Clear();
   startItem = 0;   
   party.activeItem = 0;
   ItemsOnPage = 0;
}


//*****************************************************************************
// NAME: clearShopItemState
//
// PURPOSE:  
//*****************************************************************************
void clearShopItemState()
{
   itemListText.Clear();
   ItemsOnPage = 0;
   startItem = 0;   
   party.activeItem = 0;
}


//*****************************************************************************
// NAME: clearSpellItemState
//
// PURPOSE:  
//*****************************************************************************
void clearSpellItemState()
{
   spellListText.Clear();
   ItemsOnPage = 0;
   startItem = 0;   
   party.activeItem = 0;
}


//*****************************************************************************
// NAME: setCharItemTextState
//
// PURPOSE:  
//*****************************************************************************
void setCharItemTextState(CHARACTER &dude, 
                          BOOL (*pFilter)(ITEM  *pItem, void *pFilterPkt), 
                          void *pFilterPkt,
                          int flags)
{
   //if (charItemListText.GetCount() <= 0)
   clearCharItemState();

   flags |= QTY | RDY;
   if (party.PartyIsDetectingMagic())
     flags |= DETECTING_MAGIC;

   if ((flags & SPECIAL_ONLY) == 0)
   {
      charItemListText.FillItemListText(dude.myItems, 
                                        flags,
                                        Normal,
                                        pFilter,
                                        pFilterPkt);
      dude.addCharMoneyToItemList(charItemListText);
   };
   addSpecialItemsToItemList(charItemListText);
   charItemListText.FormatItemListTitle("YOU HAVE NO ITEMS");

   int cnt = charItemListText.GetCount();
   ItemsOnPage = min(Items_Per_Page,cnt);

   if (ItemsOnPage > 0)
   {
      if (party.activeItem >= charItemListText.GetCount())
        party.activeItem = (BYTE)(charItemListText.GetCount()-1);

      if (party.activeItem >= (startItem+ItemsOnPage))
        party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);

      if (party.activeItem < 0)
        party.activeItem = 0;
   }
   else
     clearCharItemState();
}


//*****************************************************************************
// NAME: setShopItemState
//
// PURPOSE:  
//*****************************************************************************
void setShopItemState(ITEM_LIST &list, costFactorType type)
{
   //if (itemListText.GetCount() <= 0)
     clearShopItemState();

   UINT flags = COST | QTY | MONEY;
   itemListText.FillItemListText(list, flags, type);
   itemListText.FormatItemListTitle("THIS SHOP HAS NO ITEMS");
   int cnt = itemListText.GetCount();
   ItemsOnPage = min(Items_Per_Page, cnt);

   if (ItemsOnPage > 0)
   {
      if (party.activeItem >= itemListText.GetCount())
        party.activeItem = (BYTE)(itemListText.GetCount()-1);

      if (party.activeItem >= (startItem+ItemsOnPage))
        party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);

      if (party.activeItem < 0)
        party.activeItem = 0;
   }
   else
   {
     party.activeItem = 0;
     startItem = 0;
     //currShopItem = 0;
   }
}


//*****************************************************************************
// NAME: setSpellItemState
//
// PURPOSE:  
//*****************************************************************************
// PRS July 2009void setSpellItemState(CHARACTER &cdata,  spellBookType *spellbook, DWORD flags, costFactorType type)
void setSpellItemState(CHARACTER &cdata, DWORD flags, costFactorType type)
{
   //if (spellListText.GetCount() <= 0)
     clearSpellItemState();   // Clear spellListText

   //if (spellbook.SpellsKnown() > 0)
   //       spellbook.SortSpells();
   // PRS Jul 2009 spellListText.FillSpellListText(cdata, spellbook, flags, type);
   spellListText.FillSpellListText(cdata, flags, type);
   spellListText.FormatSpellListTitle("NO SPELLS AVAILABLE");
   int cnt = spellListText.GetCount();
   ItemsOnPage = min(Items_Per_Page, cnt);

   if (ItemsOnPage > 0)
   {
      if (party.activeItem >= spellListText.GetCount())
        party.activeItem = (BYTE)(spellListText.GetCount()-1);

      if (party.activeItem >= (startItem+ItemsOnPage))
        party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);

      if (party.activeItem < 0)
        party.activeItem = 0;
   }
   else
   {
     party.activeItem = 0;
     startItem = 0;
     //currShopItem = 0;
   }
}

void setSpellNewLevelItemState(CHARACTER &data)
{
   clearSpellItemState();

   if (data.SpellsKnown() > 0)
     //data.SortSpells();
   
   spellListText.FillNewSpellLevelListText(data);

   int cnt = spellListText.GetCount();
   ItemsOnPage = min(Items_Per_Page, cnt);

   if (ItemsOnPage > 0)
   {
      if (party.activeItem >= spellListText.GetCount())
        party.activeItem = (BYTE)(spellListText.GetCount()-1);
      if (party.activeItem >= (startItem+ItemsOnPage))
        party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);
      if (party.activeItem < 0)
        party.activeItem = 0;
   }
   else
   {
     party.activeItem = 0;
     startItem = 0;
     //currShopItem = 0;
   }
}

void setSpellMemorizeItemState(CHARACTER &data)
{
   //if (spellListText.GetCount() <= 0)
     clearSpellItemState();

   //if (data.SpellsKnown() > 0)
     //data.SortSpells();
   spellListText.FillMemorizeSpellListText(&data);
   int cnt = spellListText.memSum.GetMemorizeItemCount();
   ItemsOnPage = min(Items_Per_Page, cnt);

   if (ItemsOnPage > 0)
   {
      if (party.activeItem >= spellListText.GetCount())
        party.activeItem = (BYTE)(spellListText.GetCount()-1);
      if (party.activeItem >= (startItem+ItemsOnPage))
        party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);
      if (party.activeItem < 0)
        party.activeItem = 0;
   }
   else
   {
     party.activeItem = 0;
     startItem = 0;
     //currShopItem = 0;
   }
}

void setSpellTempleCastItemState(CHARACTER &data, costFactorType type)
{
   //if (spellListText.GetCount() <= 0)
     clearSpellItemState();

   //if (data.SpellsKnown() > 0)
     //data.SortSpells();
   spellListText.FillTempleCastSpellListText(data, type);
   int cnt = spellListText.GetCount();
   ItemsOnPage = min(Items_Per_Page, cnt);

   if (ItemsOnPage > 0)
   {
      if (party.activeItem >= spellListText.GetCount())
        party.activeItem = (BYTE)(spellListText.GetCount()-1);
      if (party.activeItem >= (startItem+ItemsOnPage))
        party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);
      if (party.activeItem < 0)
        party.activeItem = 0;
   }
   else
   {
     party.activeItem = 0;
     startItem = 0;
   }
}

void setSpellCastItemState(CHARACTER &data, CASTING_ENVIRONMENT ce)
{
   //if (spellListText.GetCount() <= 0)
     clearSpellItemState();

   //if (data.SpellsKnown() > 0)
     //data.SortSpells();
   spellListText.FillCastSpellListText(data, ce);
   int cnt = spellListText.GetIntegerCount();
   ItemsOnPage = min(Items_Per_Page, cnt);

   if (ItemsOnPage > 0)
   {
      if (party.activeItem >= spellListText.GetIntegerCount())
        party.activeItem = (BYTE)(spellListText.GetIntegerCount()-1);
      if (party.activeItem >= (startItem+ItemsOnPage))
        party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);
      if (party.activeItem < 0)
        party.activeItem = 0;
   }
   else
   {
     party.activeItem = 0;
     startItem = 0;
   }
}

void setSpellEffectItemState(CHARACTER &data)
{
   //if (spellListText.GetCount() <= 0)
     clearSpellItemState();

   //if (data.SpellsKnown() > 0)
     //data.SortSpells();
   spellListText.FillSpellEffectsListText(data, CHAR_VIEW_DISPLAY);
   int cnt = spellListText.GetEffectCount();
   ItemsOnPage = min(Items_Per_Page, cnt);

   if (ItemsOnPage > 0)
   {
      if (party.activeItem >= spellListText.GetEffectCount())
        party.activeItem = (BYTE)(spellListText.GetEffectCount()-1);
      if (party.activeItem >= (startItem+ItemsOnPage))
        party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);
      if (party.activeItem < 0)
        party.activeItem = 0;
   }
   else
   {
     party.activeItem = 0;
     startItem = 0;
   }
}

//*****************************************************************************
// NAME: setTreasureEventState
//
// PURPOSE:  
//*****************************************************************************
void setTreasureEventState(GameEvent *event)
{
   //if (treasureItemListText.GetCount() <= 0)
   clearTreasureItemState();

   getTreasureEventItems(event);
   int cnt = treasureItemListText.GetCount();
   ItemsOnPage = min(Items_Per_Page,cnt);
  
   if (ItemsOnPage > 0)
   {
      if (party.activeItem >= treasureItemListText.GetCount())
        party.activeItem = (BYTE)(treasureItemListText.GetCount()-1);

      if (party.activeItem >= (startItem+ItemsOnPage))
        party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);

      if (party.activeItem < 0)
        party.activeItem = 0;
   }
   else
   {
     party.activeItem = 0;
     startItem = 0;
     //currShopItem = 0;
   }
}


//*****************************************************************************
// NAME: nextTreasurePage
//
// PURPOSE:  
//*****************************************************************************
void nextTreasurePage()
{
  if (startItem+Items_Per_Page < treasureItemListText.GetCount())
    startItem += Items_Per_Page;  
  int cnt = (treasureItemListText.GetCount()-startItem);
  ItemsOnPage = min(cnt, Items_Per_Page);

  if (party.activeItem >= ItemsOnPage)
    party.activeItem = ItemsOnPage-1;
}


//*****************************************************************************
// NAME: prevTreasurePage
//
// PURPOSE:  
//*****************************************************************************
void prevTreasurePage()
{
  if (startItem-Items_Per_Page >= 0)
    startItem -= Items_Per_Page;
  else
    startItem = 0;
  int cnt = (treasureItemListText.GetCount()-startItem);
  ItemsOnPage = min(cnt, Items_Per_Page);

  if (party.activeItem >= ItemsOnPage)
    party.activeItem = ItemsOnPage-1;
}


//*****************************************************************************
// NAME: allowTreasureItemNextPage
//
// PURPOSE:  
//*****************************************************************************
BOOL allowTreasureItemNextPage()
{
  // only one page of data
  if (treasureItemListText.GetCount() <= Items_Per_Page)
    return FALSE;

  // end of list
  if (startItem > (treasureItemListText.GetCount()-Items_Per_Page)) 
    return FALSE;

  return TRUE;
}


//*****************************************************************************
// NAME: allowTreasureItemPrevPage
//
// PURPOSE:  
//*****************************************************************************
BOOL allowTreasureItemPrevPage()
{
  // only one page of data
  if (treasureItemListText.GetCount() <= Items_Per_Page)
    return FALSE;

  // start of list
  if (startItem < Items_Per_Page) 
    return FALSE;

  return TRUE;
}


//*****************************************************************************
// NAME: nextCharItemsPage
//
// PURPOSE:  
//*****************************************************************************
void nextCharItemsPage()
{
  if (startItem+Items_Per_Page < charItemListText.GetCount())
    startItem += Items_Per_Page;
  int cnt = (charItemListText.GetCount()-startItem);
  ItemsOnPage = min(cnt, Items_Per_Page);

  if (party.activeItem >= ItemsOnPage)
    party.activeItem = ItemsOnPage-1;
}


//*****************************************************************************
// NAME: prevCharItemsPage
//
// PURPOSE:  
//*****************************************************************************
void prevCharItemsPage()
{
  if (startItem-Items_Per_Page >= 0)
    startItem -= Items_Per_Page;
  else
    startItem = 0;
  int cnt = (charItemListText.GetCount()-startItem);
  ItemsOnPage = min(cnt, Items_Per_Page);
  
  if (party.activeItem >= ItemsOnPage)
    party.activeItem = ItemsOnPage-1;
}


//*****************************************************************************
// NAME: allowCharItemNextPage
//
// PURPOSE:  
//*****************************************************************************
BOOL allowCharItemNextPage()
{
  // only one page of data
  if (charItemListText.GetCount() <= Items_Per_Page)
    return FALSE;

  // end of list
  if (startItem > (charItemListText.GetCount()-Items_Per_Page)) 
    return FALSE;

  return TRUE;
}


//*****************************************************************************
// NAME: allowCharItemPrevPage
//
// PURPOSE:  
//*****************************************************************************
BOOL allowCharItemPrevPage()
{
  // only one page of data
  if (charItemListText.GetCount() <= Items_Per_Page)
    return FALSE;

  // start of list
  if (startItem < Items_Per_Page) 
    return FALSE;

  return TRUE;
}


//*****************************************************************************
// NAME: nextShopPage
//
// PURPOSE:  
//*****************************************************************************
void nextShopPage()
{
  if (startItem+Items_Per_Page < itemListText.GetCount())    
    startItem += Items_Per_Page;
  int cnt = (itemListText.GetCount()-startItem);
  ItemsOnPage = min(cnt, Items_Per_Page);

  if (party.activeItem >= ItemsOnPage)
    party.activeItem = ItemsOnPage-1;
}


//*****************************************************************************
// NAME: prevShopPage
//
// PURPOSE:  
//*****************************************************************************
void prevShopPage()
{
  if (startItem-Items_Per_Page >= 0)
    startItem -= Items_Per_Page;
  else
    startItem = 0;
  int cnt = (itemListText.GetCount()-startItem);
  ItemsOnPage = min(cnt, Items_Per_Page);

  if (party.activeItem >= ItemsOnPage)
    party.activeItem = ItemsOnPage-1;
}

//*****************************************************************************
// NAME: allowShopItemNextPage
//
// PURPOSE:  
//*****************************************************************************
BOOL allowShopItemNextPage()
{
  // only one page of data
  if (itemListText.GetCount() <= Items_Per_Page)
    return FALSE;

  // end of list
  if (startItem > (itemListText.GetCount()-Items_Per_Page)) 
    return FALSE;

  return TRUE;
}


//*****************************************************************************
// NAME: allowShopItemPrevPage
//
// PURPOSE:  
//*****************************************************************************
BOOL allowShopItemPrevPage()
{
  // only one page of data
  if (itemListText.GetCount() <= Items_Per_Page)
    return FALSE;

  // start of list
  if (startItem < Items_Per_Page) 
    return FALSE;

  return TRUE;
}


//*****************************************************************************
// NAME: nextSpellPage
//
// PURPOSE:  
//*****************************************************************************
void nextSpellPage(const SELECTION_PAGE_FORMAT *format)
{
  int ipp;
  if (format == NULL) ipp = Items_Per_Page;
  else ipp = format->Lines_Per_Page;
  if (startItem+ipp < spellListText.GetCount())    
    startItem += ipp;
  int cnt = (spellListText.GetCount()-startItem);
  ItemsOnPage = min(cnt, ipp);

  if (party.activeItem >= ItemsOnPage)
    party.activeItem = ItemsOnPage-1;
}


//*****************************************************************************
// NAME: prevSpellPage
//
// PURPOSE:  
//*****************************************************************************
void prevSpellPage(const SELECTION_PAGE_FORMAT *format)
{
  int ipp;
  if (format == NULL) ipp = Items_Per_Page;
  else ipp = format->Lines_Per_Page;
  if (startItem-ipp >= 0)
    startItem -= ipp;
  else
    startItem = 0;
  int cnt = (spellListText.GetCount()-startItem);
  ItemsOnPage = min(cnt, ipp);

  if (party.activeItem >= ItemsOnPage)
    party.activeItem = ItemsOnPage-1;
}

//*****************************************************************************
// NAME: nextLinePage
//
// PURPOSE:  
//*****************************************************************************
void nextLinePage(const SELECTION_PAGE_FORMAT *format)
{
  int ipp;
  if (format == NULL) ipp = Items_Per_Page;
  else ipp = format->Lines_Per_Page;
  if (startItem+ipp < lineListText.GetCount())
    startItem += ipp;
  int cnt = (lineListText.GetCount()-startItem);
  ItemsOnPage = min(cnt, ipp);

  if (party.activeItem >= ItemsOnPage)
    party.activeItem = ItemsOnPage-1;
}


//*****************************************************************************
// NAME: prevLinePage
//
// PURPOSE:  
//*****************************************************************************
void prevLinePage(const SELECTION_PAGE_FORMAT *format)
{
  int ipp;
  if (format == NULL) ipp = Items_Per_Page;
  else ipp = format->Lines_Per_Page;
  if (startItem-ipp >= 0)
    startItem -= ipp;
  else
    startItem = 0;
  int cnt = (lineListText.GetCount()-startItem);
  ItemsOnPage = min(cnt, ipp);

  if (party.activeItem >= ItemsOnPage)
    party.activeItem = ItemsOnPage-1;
}


//*****************************************************************************
// NAME: allowSpellNextPage
//
// PURPOSE:  
//*****************************************************************************
BOOL allowSpellNextPage(const SELECTION_PAGE_FORMAT *format)
{
  int ipp;
  if (format == NULL) ipp = Items_Per_Page;
  else ipp = format->Lines_Per_Page;
  // only one page of data
  if (spellListText.GetCount() <= ipp)
    return FALSE;

  // end of list
  if (startItem > (spellListText.GetCount()-ipp)) 
    return FALSE;

  return TRUE;
}


//*****************************************************************************
// NAME: allowSpellPrevPage
//
// PURPOSE:  
//*****************************************************************************
BOOL allowSpellPrevPage(const SELECTION_PAGE_FORMAT *format)
{
  int ipp;
  if (format == NULL) ipp = Items_Per_Page;
  else ipp = format->Lines_Per_Page;
  // only one page of data
  if (spellListText.GetCount() <= ipp)
    return FALSE;

  // start of list
  if (startItem < ipp) 
    return FALSE;

  return TRUE;
}


//*****************************************************************************
// NAME: currSpellTextIndex
//
// PURPOSE:  
//*****************************************************************************
int currSpellTextIndex()
{
  return startItem+party.activeItem;
}

//*****************************************************************************
// NAME: currLineTextIndex
//
// PURPOSE:  
//*****************************************************************************
int currLineTextIndex()
{
  return startItem+party.activeItem;
}


//*****************************************************************************
// NAME: currShopTextIndex
//
// PURPOSE:  
//*****************************************************************************
int currShopTextIndex()
{
  return startItem+party.activeItem;
}


//*****************************************************************************
// NAME: currCharItemsTextIndex
//
// PURPOSE:  
//*****************************************************************************
int currCharItemsTextIndex()
{
  return startItem+party.activeItem;
}


//*****************************************************************************
// NAME: currTreasureTextIndex
//
// PURPOSE:  
//*****************************************************************************
int currTreasureTextIndex()
{
  return startItem+party.activeItem;
}


BOOL shouldHighlight(int index)
{
  return ((index-startItem)== party.activeItem);
}

//*****************************************************************************
// NAME: displayPoolGoldTotal
//
// PURPOSE:  
//*****************************************************************************
void displayPoolGoldTotal(int fontNumber)
{
  BOOL CurrFontTagSetting = GraphicsMgr.GetEnableFontColorTags();
  GraphicsMgr.EnableFontColorTags(FALSE);

  CString temp;
  int qty;

  if (party.moneyPooled)
    qty = party.GetPoolGoldValue();
  else
    qty = party.getActiveChar().poolCharacterGold();

  temp.Format("AVAILABLE %s: %u", globalData.moneyData.GetName(globalData.moneyData.GetDefaultType()), qty);
  DisplayText(textboxX+(TEXTBOX_WIDTH/3), fontNumber, textboxY+20, temp, whiteColor, FALSE);

  GraphicsMgr.EnableFontColorTags(CurrFontTagSetting);
}

//*****************************************************************************
// NAME: displayCharacterHP
//
// PURPOSE: Displays character name and hitpoints in message box window
//          during temple spell casting.
//*****************************************************************************
void displayCharacterHP(CHARACTER &dude, int fontNumber)
{
  BOOL CurrFontTagSetting = GraphicsMgr.GetEnableFontColorTags();
  GraphicsMgr.EnableFontColorTags(FALSE);

// prs 20191220  COLORREF color;
  FONT_COLOR_NUM colorNum;
  CString temp;
  int hp = dude.GetAdjHitPoints();
  temp.Format("%s     %i/%i", dude.GetName(), hp, dude.GetMaxHitPoints());
  
  if (hp <= 0)
    colorNum = redColor;
  else if (hp < dude.GetMaxHitPoints())
     colorNum=yellowColor;
  else if (dude.GetAdjReadyToTrain())
     colorNum = blueColor;
  else
     colorNum=greenColor;
  DisplayText(textboxX+(TEXTBOX_WIDTH/3), fontNumber, textboxY+20, temp, colorNum, FALSE);

  GraphicsMgr.EnableFontColorTags(CurrFontTagSetting);
}

// figure out if x,y is within one of the displayed party member RECTs
int IntersectPointWithInventory(long x, long y)
{
// If you ain't selecting, don't call!
  POINT pt = { x, y };

  for (int i=0; i < Items_Per_Page; i++)
  {
    if (PtInRect(&InventoryRects.GetAt(i), pt))
      return i;
  }

  return -1;
}

// figure out if x,y is within one of the displayed party member RECTs
int IntersectPointWithParty(long x, long y)
{
  // Don't call me if you ain't selecting party members!
  POINT pt = { x, y };

  for (int i=0; i < party.numCharacters; i++)
  {
    if (PtInRect(&PartyNameRects[i], pt))
      return i;
  }

  return NO_DUDE;
}

void AskAndDisplay(HOOK_PARAMETERS *pHookParameters, 
                   const char      *scriptName, 
                   int             parameterA,    // hookParameter[1]
                   int             parameterB,    // hookParameter[2]
                   int             parameterC,    // hookParameter[3]
                   int             displayX,      // hookParameter[5]
                   int             displayY,      // hookParameter[6]
                   CString         text)          // hookParameter[7]
{                                                 // hookParameter[8] = "White"
  int posX, posY;
// prs 20191220  COLORREF color;
  FONT_COLOR_NUM colorNum;
  (*pHookParameters)[1].Format("%d", parameterA);
  (*pHookParameters)[2].Format("%d", parameterB);
  (*pHookParameters)[3].Format("%d", parameterC);
  (*pHookParameters)[5].Format("%d", displayX);
  (*pHookParameters)[6].Format("%d", displayY);
  (*pHookParameters)[7] = text;
  (*pHookParameters)[8] = "White";
  RunGlobalScript("Global_Display", scriptName, true);
  if ((*pHookParameters)[0] == "N") return;  // Print nothing
  posX = atoi((*pHookParameters)[5]);
  posY = atoi((*pHookParameters)[6]);
  colorNum = ASCII_TO_COLOR((*pHookParameters)[8], whiteColor);
  DisplayText(posX, posY, 0, (*pHookParameters)[7], colorNum, FALSE);
}


//*****************************************************************************
// NAME: displayPartyNames
//
// PURPOSE:  
//*****************************************************************************
void displayPartyNames(PARTY &pty, bool useWX, 
                       bool showNPC, 
                       const SELECTION_PAGE_FORMAT *format,
                       int fontNumber)
{  
  BOOL CurrFontTagSetting = FALSE;
  DETAILTRACE(0x4c1120,0);
  memset(PartyNameRects, 0, sizeof(PartyNameRects));

   // a couple of cases when party data isn't drawn
  if ((globalData.levelInfo.stats[globalData.currLevel].overland)
    && (globalData.useAreaView))
  {
    return;
  };

  if (currPic.picType == BigPicDib)
  {
    return;
  };

  if (format == NULL)
  {
    CurrFontTagSetting = GraphicsMgr.GetEnableFontColorTags();
    DETAILTRACE(0x4c1121, 0);
    GraphicsMgr.EnableFontColorTags(FALSE);
    DETAILTRACE(0x4c1122, 0);
  };
  int LineHeight = GraphicsMgr.GetMaxCharacterHeight()+2;

   int i,x;
   int y;                //DLD 12/20/00 //int y=18;
   int wx=PartyNames_wx; //const int wx=18;
   int nx=PartyNames_nx; //const int nx=275;
   int wy=PartyNames_wy; //
   int ny=PartyNames_ny; //end change/add
   
// prs 20191220   COLORREF color;
   FONT_COLOR_NUM colorNum;
// prs 20191220   COLORREF namecolor;
   FONT_COLOR_NUM namecolorNum;
   CString temp;   

   if (format == NULL)
   {
     x = useWX ? wx : nx;
     y = useWX ? wy : ny; //DLD 12/20/00
     DisplayText(x, y, fontNumber, "NAME", whiteColor, FALSE);
     DETAILTRACE(0x4c1123, 0);
     DisplayText(x + 225, y, fontNumber, " AC", whiteColor, FALSE); //DLD 12/20/00 //displayText(500, y, " AC", White);
     DisplayText(x + 300, y, fontNumber, " HP", whiteColor, FALSE); //displayText(575, y, " HP", White);
   }
   else
   {
     x = format->Header_x;
     y = format->Header_y;
     DisplayColoredText(x, y, fontNumber, format->Header_Format, 
                        whiteColor, FALSE,
                        FALSE, FALSE, TRUE);
     DETAILTRACE(0x4c1123, 0);
     x = format->Header2_x;
     y = format->Header2_y;
     DisplayColoredText(x, y, fontNumber, format->Header2_Format, 
                        whiteColor, FALSE,
                        FALSE, FALSE, TRUE); //DLD 12/20/00 //displayText(500, y, " AC", White);
     x = format->Header3_x;
     y = format->Header3_y;
     DisplayColoredText(x, y, fontNumber, format->Header3_Format, 
                        whiteColor, FALSE,
                        FALSE, FALSE, TRUE); //displayText(575, y, " HP", White);
     y = format->Table_y;
   }
   
   
   
   for (i=0; i<pty.numCharacters;i++)
   {
      if ((!showNPC) && (pty.characters[i].GetType() == NPC_TYPE))
        continue;

      int hp = pty.characters[i].GetAdjHitPoints();
      if (hp <= 0)
        colorNum = redColor;
      else if (hp < pty.characters[i].GetMaxHitPoints())
         colorNum=yellowColor;
      else
         colorNum=greenColor;

      if (pty.characters[i].GetAdjReadyToTrain())
        namecolorNum=blueColor;
      else
        namecolorNum=greenColor;

      if (format == NULL)
      {
        y += LineHeight;//20;
        if (pty.activeCharacter == i)
          DisplayText(x, y, fontNumber, pty.characters[i].GetName(), namecolorNum, FALSE, TRUE);
        else
          DisplayText(x, y, fontNumber, pty.characters[i].GetName(), namecolorNum, FALSE);
      }
      else
      {
        CString cName;
        if (pty.activeCharacter == i)
        {
          cName.Format(format->Table_Selected_Format, pty.characters[i].GetName());
          DisplayColoredText(format->Table_x, y, 
                             fontNumber, 
                             cName, 
                             namecolorNum, FALSE,
                             FALSE, FALSE, TRUE);
        }
        else
        {
          cName.Format(format->Table_Format, pty.characters[i].GetName());
          DisplayColoredText(format->Table_x, y, 
                             fontNumber, 
                             cName, 
                             namecolorNum, FALSE,
                             FALSE, FALSE, TRUE);
        };
      };
      {
        int adjAC, effAC;
        adjAC = pty.characters[i].GetAdjAC();
        effAC = pty.characters[i].GetEffectiveAC();
        if (format == NULL)
        {
          temp.Format("%3i", effAC);
          DisplayText(x + 225, y, 
                      fontNumber, 
                      temp, 
                      (effAC == adjAC) ? namecolorNum : yellowColor,
                      FALSE); //DLD 12/20/00 //displayText(500,y, temp, color);
        }
        else
        {
          temp.Format(format->Table2_Format, effAC);
          DisplayColoredText(format->Table2_x, y, 
                             fontNumber, 
                             temp, (effAC == adjAC) ? namecolorNum : yellowColor,
                             FALSE,
                             FALSE, FALSE, TRUE); //DLD 12/20/00 //displayText(500,y, temp, color);
        };
      };
      
      if (format == NULL)
      {
        temp.Format("%3i", hp);
        DisplayColoredText(x + 300, y, 
                           fontNumber, 
                           temp, 
                           colorNum, FALSE,
                           FALSE, FALSE, TRUE); //DLD 12/20/00 //displayText(575,y, temp, color);
      }
      else
      {
        temp.Format(format->Table3_Format, hp);
        DisplayColoredText(format->Table3_x, y, 
                           fontNumber, 
                           temp, 
                           colorNum, FALSE,
                           FALSE, FALSE, TRUE); //DLD 12/20/00 //displayText(575,y, temp, color);
      }

      if (format == NULL)
      {
        PartyNameRects[i].left = x;
        PartyNameRects[i].top = y;
        PartyNameRects[i].right = x + 300 + GraphicsMgr.GetTextWidth(temp, temp.GetLength()); //DLD 12/20/00 //PartyNameRects[i].right = 575 + GraphicsMgr.GetTextWidth(temp, strlen(temp));
        PartyNameRects[i].bottom = y + GraphicsMgr.GetMaxCharacterHeight();
      }
      else
      {
        PartyNameRects[i].left = format->Table_x;
        PartyNameRects[i].top = y;
        PartyNameRects[i].right = format->Table3_x + GraphicsMgr.GetTextWidth(temp, temp.GetLength()); //DLD 12/20/00 //PartyNameRects[i].right = 575 + GraphicsMgr.GetTextWidth(temp, strlen(temp));
        PartyNameRects[i].bottom = y + GraphicsMgr.GetMaxCharacterHeight();
        y += format->Line_Spacing;
      };
   }
     
   if (x == nx)
   {
     int timeX, timeY;
     int facingX, facingY;
     int searchingX, searchingY;
     CString locationText;
     CString facingText;
     CString timeText;
     CString searchingText;
     {
       HOOK_PARAMETERS hookParameters;

       //********************************Get location and color of party position
       locationText.Format("%i,%i", pty.Posx, pty.Posy);
       AskAndDisplay(&hookParameters, "LocationText", pty.Posx, pty.Posy,0,  LocationText_x, LocationText_y, locationText);


       // *******************************************Get location, text, and color of party facing
       if (!globalData.levelInfo.stats[globalData.currLevel].overland)
       {
         switch (pty.facing) 
         {
         case FACE_NORTH:
            facingText = "North";
            break;
         case FACE_EAST:
            facingText = "East";
            break;
         case FACE_SOUTH:
            facingText = "South";
            break;
         case FACE_WEST:
            facingText = "West";
            break;
         };
       };      
       facingX = LocationText_x + 25 + 40;
       facingY = LocationText_y;


       AskAndDisplay(&hookParameters,"FacingText", pty.Posx, pty.Posy, pty.facing, facingX, facingY, facingText);


       // ******************************Get SEARCHING location, text, and color
       searchingX = LocationText_x + 25 + 85;
       searchingY =  LocationText_y;
       if (pty.searching)
       {
         searchingText = "Searching";
       };
       AskAndDisplay(&hookParameters, "SearchingText", pty.searching, 0, 0, searchingX, searchingY, searchingText);


       // *************************  Get TIME  location, text, and color
       timeX = LocationText_x + 125 + 70;
       timeY = LocationText_y;
       timeText.Format("Day %i %02i:%02i", pty.days, pty.hours, pty.minutes);
       AskAndDisplay(&hookParameters, "TimeText", pty.days, pty.hours, pty.minutes, timeX, timeY, timeText);
      
//     if (!globalData.levelInfo.stats[globalData.currLevel].overland)
//     {
//       displayText(facingX, facingY, facingText, facingColor); //DLD 12/20/00 //displayText(340, 280, temp, White);
//     };
      
//     if (pty.searching)
//        displayText(searchingX,searchingY, searchingText, searchingColor); //DLD 12/20/00 //displayText(385, 280, "Searching", White);
      
//           displayText(timeX, timeY, timeText, timeColor); //DLD 12/20/00 displayText(470, 280, temp, White);
     };
   };
   if (format == NULL)
   {
     DETAILTRACE(0x4c1123, 0);
     GraphicsMgr.EnableFontColorTags(CurrFontTagSetting);
     DETAILTRACE(0x4c1124, 0);
   };
}


DisplayStatusPkt::~DisplayStatusPkt(void)
{
  for (int i=0; i<m_numString; i++)  delete m_pStrings[i]; 
  free(m_pStrings);
  m_pStrings = NULL;
  m_numString = 0;
}


void DisplayStatusPkt::AddString(const CString& str)
{
  m_numString++;
  m_pStrings = (CString **)realloc(m_pStrings, m_numString*sizeof(CString *));
  m_pStrings[m_numString-1] = new CString(str);
}

CBRESULT ScriptCallback_CharDisplayStatus(CBFUNC func, CString *str, void *pkt)
{
  DisplayStatusPkt *pResult;
  pResult = (DisplayStatusPkt *)pkt;
  if (func == CBF_EXAMINESCRIPT)
  {
    if (!str->IsEmpty())
    {
        pResult->AddString((*p_hook_parameters)[6]);
        pResult->AddString((*p_hook_parameters)[7]);
    };
  };
  return CBR_CONTINUE;
}




//*****************************************************************************
// NAME: displayCombatStats
//
// PURPOSE:  
//*****************************************************************************
void displayCombatStats(int dude, int fontNumber)
{ 
   if ((dude < 0) || (dude >= combatData.NumCombatants()))
      return;


  BOOL CurrFontTagSetting = GraphicsMgr.GetEnableFontColorTags();
  GraphicsMgr.EnableFontColorTags(FALSE);
  int LineHeight = GraphicsMgr.GetMaxCharacterHeight()+2;

  int x;
  //x = COMBAT_SCREEN_WIDTH + (VertBarSrc.right - VertBarSrc.left) + 2;
  int y;
  //y= 18;
  x = DEFAULT_COMBAT_STAT_X;
  y = DEFAULT_COMBAT_STAT_Y;
  CString wp, dmg, temp;
// prs 20191220   COLORREF color, color2; 
  FONT_COLOR_NUM colorNum, color2Num;
  const COLORREF AdjColor = RGB(255,128,0); // Bright Orange, for stats currently modified by active spells   
   
  if (combatData.GetCombatant(dude)->GetHitPoints() <= 0)
    color2Num = combatRedColor;
  else if (combatData.GetCombatant(dude)->GetIsFriendly())
    color2Num = combatBlueColor;
  else
    color2Num = combatYellowColor;

  int hp = combatData.GetCombatant(dude)->GetAdjHitPoints();
  if (hp <= 0)
     colorNum = combatRedColor;
  else if (hp < combatData.GetCombatant(dude)->GetMaxHitPoints())
     colorNum = combatYellowColor;
  else
     colorNum = combatGreenColor;
   
  // display name
#ifdef _DEBUG
  CString DisplayName;   
  DisplayName.Format("%s (%i:%i,%i)",
            combatData.GetCombatant(dude)->GetName(), 
            dude,
            combatData.GetCombatant(dude)->x,
            combatData.GetCombatant(dude)->y);
  DisplayText(x,y, fontNumber, DisplayName, color2Num, FALSE);
#else
//  displayText(x,y, combatData.GetCombatant(dude)->GetName(), color2Num);
  DisplayText(x, y, 
              fontNumber, 
              combatData.GetCombatant(dude)->GetName(), 
              color2Num, FALSE);
#endif
   
  // display current hit points
  y += LineHeight;
  DisplayText(x,y, fontNumber, "HITPOINTS ", combatYellowColor, FALSE);
  temp.Format("%3i", hp);
  int hpx = x + GraphicsMgr.GetTextWidth("HITPOINTS ", strlen("HITPOINTS "));
  if (hp!=combatData.GetCombatant(dude)->GetHitPoints())
    DisplayText(hpx,y, fontNumber, temp, illegalColor, FALSE, AdjColor);
  else
    DisplayText(hpx,y, fontNumber, temp, colorNum, FALSE);
   
  // display current armor class
  y += LineHeight;//18;
  DisplayText(x,y, fontNumber, "AC ", combatYellowColor, FALSE);
  int currAC = combatData.GetCombatant(dude)->GetEffectiveAC();

/*  Deleted 28 Feb 2010; We re-defined CHARACTER.AC;
    // Added 29 July 2009 PRS
   if (    (combatData.GetCombatant(dude).GetType() == MONSTER_TYPE)
        ||     (combatData.GetCombatant(dude).GetType() == NPC_TYPE)// Added 28 Feb 2010 PRS
            &&  !combatData.GetCombatant(dude).friendly             // Added 28 Feb 2010 PRS
      )
   {
      currAC += combatData.GetCombatant(dude).myItems.GetProtectModForRdyItems();
   };
    //
    */

  temp.Format("%3i", currAC);
  if (currAC != combatData.GetCombatant(dude)->GetEffectiveAC())
    DisplayText(x+25, y, fontNumber, temp, illegalColor, FALSE, AdjColor);
  else
    DisplayText(x+25, y, fontNumber, temp, combatYellowColor, FALSE);
  y += LineHeight;//18;

   // display current readied weapon
  combatData.getCharWeaponText(dude, wp, dmg);
  if ((!wp.IsEmpty()) && (wp.CompareNoCase("NONE")!=0))
  {
     DisplayText(x,y, fontNumber, wp, combatYellowColor, FALSE);
     y += LineHeight;//18;
  }

#ifdef _DEBUG
  temp.Format("Attacks: %f", combatData.GetCombatant(dude)->AttacksRemaining());
  DisplayText(x,y, fontNumber, temp, whiteColor, FALSE);
  y += LineHeight;//18;
  temp.Format("Cursor: %u, %u", combatData.m_iCursorX, combatData.m_iCursorY);
  DisplayText(x,y, fontNumber, temp, whiteColor, FALSE);
  y += LineHeight;//18;

#endif

  /*
  POSITION pos = combatMsgs.msgs.GetHeadPosition();
  while (pos != NULL)
  {
    displayText(x,y, combatMsgs.msgs.GetNext(pos), White);
    y += 18;
  }
  */

  {
    // We moved this here from OnStartCombatAction so that
    // targets will also have their stats displayed.

    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    ActorType actor;
    int i;
    DisplayStatusPkt results;
    COMBATANT *pCombatant;
    pCombatant = combatData.GetCombatant(dude);
    SetCharContext(&actor);
    scriptContext.SetCharacterContext(pCombatant->m_pCharacter);
    scriptContext.SetCombatantContext(pCombatant);

    hookParameters[5] = "COMBAT";

    {
        //For Each dude->SpellEffect
      POSITION p;
      //GLOBAL_SPELL_ID gsID;
      SPELL_ID spellID;
      SPELL_DATA *pSpell;
      const SPELL_EFFECTS_DATA *pSpellEffect;
      //pCombatant->SortSpells();
         // access ordered list
      for ( p = pCombatant->m_pCharacter->m_spellEffects.GetHeadPosition();
            p != NULL; 
            pCombatant->m_pCharacter->m_spellEffects.GetNext(p))
      {
         int oldCount;
         pSpellEffect = pCombatant->m_pCharacter->m_spellEffects.PeekAt(p);
         //gsID = pSpellEffect->SourceSpell_ID();
         spellID = pSpellEffect->SourceSpell_ID();
         //pSpell = spellData.GetSpellData(gsID);
         pSpell = spellData.GetSpell(spellID);
         if (pSpell == NULL) continue;
  
         oldCount = results.m_numString;
         pSpell->RunSpellScripts(CHAR_DISPLAY_STATUS,
                 ScriptCallback_CharDisplayStatus,
                 &results,
                 "Displaying character status");
        if (results.m_numString == oldCount)
        {  // If nothing added, use spell name.
          results.AddString("");
          results.AddString(pSpell->Name);
        };
      };       
    };

    pCombatant->m_pCharacter->RunCharacterScripts(CHAR_DISPLAY_STATUS, 
                                    ScriptCallback_CharDisplayStatus, 
                                    &results,
                                    "Displaying character status");
    pCombatant->RunCombatantScripts(CHAR_DISPLAY_STATUS, 
                                    ScriptCallback_CharDisplayStatus, 
                                    &results,
                                    "Displaying character status");
    ClearCharContext();
    for (i=1; i<results.m_numString; i+=2)
    {
      CString *msg;
      msg = results.m_pStrings[i];
      if (!msg->IsEmpty())
      {
        DisplayText(x, y, fontNumber, *msg, whiteColor, FALSE);
        y += LineHeight;//18;
      };
    };
  };
  y+=(LineHeight*2);
  for (int i=0;i<combatTextData.numLines;i++)
  {
    //displayText(x,y, combatTextData.text[i], White);
    DisplayText(x,y, fontNumber, combatTextData.GetTextLine(i)->text, whiteColor, FALSE);
    y += LineHeight;//18;
  }
  GraphicsMgr.EnableFontColorTags(CurrFontTagSetting);
} 

#ifdef _DEBUG
// debug only, shows details about terrain under cursor
// while aiming manually
void displayCursorStats(int fontNumber)
{
   BOOL CurrFontTagSetting = GraphicsMgr.GetEnableFontColorTags();
   GraphicsMgr.EnableFontColorTags(FALSE);

   //const int x = COMBAT_SCREEN_WIDTH+18;
   const int x = DEFAULT_COMBAT_STAT_X;
   //int y = 150; // closer to the bottom
   int y = DEFAULT_COMBAT_STAT_Y;

   TILE_DATA data;
   int cellNum = GetCellNum(combatData.m_iCursorX,combatData.m_iCursorY);
   data = GetTileData(combatData.m_iCursorX,combatData.m_iCursorY);

   CString tmp;

   tmp.Format("Cursor X,Y: %i,%i", combatData.m_iCursorX,combatData.m_iCursorY);
   DisplayText(x,y, fontNumber, tmp, whiteColor, FALSE);   
   y += 18;

   tmp.Format("Cell: %i", cellNum);
   DisplayText(x,y, fontNumber, tmp, whiteColor, FALSE);
   y += 18;

   tmp.Format("enab: %i", data.tile_enabled);
   DisplayText(x,y, fontNumber, tmp, whiteColor, FALSE);
   y += 18;

   tmp.Format("X,Y: %i,%i", data.tile_posx, data.tile_posy);
   DisplayText(x,y, fontNumber, tmp, whiteColor, FALSE);
   y += 18;

   tmp.Format("visi: %i", data.tile_invisible);
   DisplayText(x,y, fontNumber, tmp, whiteColor, FALSE);
   y += 18;

   tmp.Format("obst: %i", data.tile_passable);
   DisplayText(x,y, fontNumber, tmp, whiteColor, FALSE);
   
   GraphicsMgr.EnableFontColorTags(CurrFontTagSetting);
} 
#endif