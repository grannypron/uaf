/******************************************************************************
* Filename: ItemsForm.cpp
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
#include "GlobalData.h"
#include "DispText.h"
//#include "Items.h"
#include "ItemsForm.h"
#include "TextForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int pagesize=Items_Per_Page;

static const int labelsX=18;
static const int labelsY=18;

static const int itemsX=18;
static const int itemsY=36;

static const int moneyX=450;
static const int moneyY=230;

// 
// all lower = text field
// all upper = field label (static)
// first upper, rest lower = highlight area

enum ST_ITEMSFORM { // Define field names as integers for quicker comparisons
  STIF_none,
  STIF_white=TEXT_FORM::white,    // White items  
  //STIF_title,
  STIF_READY,
  STIF_QUANTITY,
  STIF_COST,
  STIF_NAME,
   
  STIF_MONEYLABEL,
  STIF_PLAT,
  STIF_GOLD,
  STIF_ELEC,
  STIF_SILVER,
  STIF_COPPER,
  STIF_COIN6,
  STIF_COIN7,
  STIF_COIN8,
  STIF_COIN9,
  STIF_COIN10,
  STIF_plat,
  STIF_gold,
  STIF_elec,
  STIF_silver,
  STIF_copper,
  STIF_coin6,
  STIF_coin7,
  STIF_coin8,
  STIF_coin9,
  STIF_coin10,

  STIF_ready,
  STIF_quantity,
  STIF_cost,
  STIF_name,
  STIF_Row,

  STIF_REPEAT,
  STIF_repeat=TEXT_FORM::autorepeat
};

#if _MSC_VER >= 1310
#define STIF(item,xrel,x,yrel,y) \
  DISPLAY_FORM(STIF_##xrel,STIF_##yrel,STIF_##item,x,y),
#else
#define STIF(item,xrel,x,yrel,y) \
  { DISPLAY_FORM(STIF_##xrel,STIF_##yrel,STIF_##item,x,y) },
#endif

// when using relative coords, it is imperative
// that you dont display item B relative to item A 
// before actually displaying item A.
static DISPLAY_FORM itemsForm[] =
{
  //
  //   enum      xrel           x        yrel       y
  // 
  STIF(READY    ,none          ,labelsX ,none     ,labelsY)  // "READY"
  STIF(QUANTITY ,READY+END     ,50      ,none     ,labelsY)  // "QTY"
  STIF(COST     ,QUANTITY+END  ,50      ,none     ,labelsY)  // "COST"
  STIF(NAME     ,COST+END      ,15      ,none     ,labelsY)  // "NAME"

  //STIF(MONEYLABEL,none         ,moneyX  ,none     , moneyY-20)  // 'Dude's Money'
  STIF(MONEYLABEL,none         ,labelsX ,none     ,  moneyY+19*10)  // 'Dude's Name

  STIF(PLAT      ,none         ,moneyX  ,none     , moneyY)  // "PLATINUM"
  STIF(plat      ,PLAT+END     , 50     ,PLAT     ,   0)  // platinum
  STIF(GOLD      ,PLAT         , 0      ,PLAT     ,  20)  // "GOLD"
  STIF(gold      ,plat+RIGHT   , 0      ,GOLD     ,   0)  // gold
  STIF(ELEC      ,PLAT         , 0      ,GOLD     ,  20)  // "ELECTRUM"
  STIF(elec      ,plat+RIGHT   , 0      ,ELEC     ,   0)  // electrum  
  STIF(SILVER    ,PLAT         , 0      ,ELEC     ,  20)  // "SILVER"
  STIF(silver    ,plat+RIGHT   , 0      ,SILVER   ,   0)  // silver  
  STIF(COPPER    ,PLAT         , 0      ,SILVER   ,  20)  // "COPPER"
  STIF(copper    ,plat+RIGHT   , 0      ,COPPER   ,   0)  // copper
  STIF(COIN6     ,PLAT         , 0      ,COPPER   ,  20)  // "COIN6"
  STIF(coin6     ,plat+RIGHT   , 0      ,COIN6    ,   0)  // coin6
  STIF(COIN7     ,PLAT         , 0      ,COIN6    ,  20)  // "COIN7"
  STIF(coin7     ,plat+RIGHT   , 0      ,COIN7    ,   0)  // coin7
  STIF(COIN8     ,PLAT         , 0      ,COIN7    ,  20)  // "COIN8"
  STIF(coin8     ,plat+RIGHT   , 0      ,COIN8    ,   0)  // coin8
  STIF(COIN9     ,PLAT         , 0      ,COIN8    ,  20)  // "COIN9"
  STIF(coin9     ,plat+RIGHT   , 0      ,COIN9    ,   0)  // coin9
  STIF(COIN10    ,PLAT         , 0      ,COIN9    ,  20)  // "COIN10"
  STIF(coin10    ,plat+RIGHT   , 0      ,COIN10   ,   0)  // coin10  

  // Repeat lines must be last
  // This one says, take the next 5 lines and repeat them 'pagesize' times.
  // The enums (fieldname) will be auto incremented
  STIF(REPEAT   ,repeat        ,5       ,none     ,pagesize) // auto repeat the rest

  STIF(ready    ,READY         ,0       ,none     ,itemsY)   // "YES/NO"
  STIF(quantity ,QUANTITY+RIGHT,0       ,none     ,itemsY)   // "10"
  STIF(cost     ,COST+RIGHT    ,0       ,none     ,itemsY)   // "100"
  STIF(name     ,NAME          ,0       ,none     ,itemsY)   // "Long Sword of Sillyness"  
  STIF(Row      ,ready+SEL     ,0       ,name+SEL ,0)        // rdy+qty+cost+name
  
  STIF(none,none,0,none,0)  // End of list
};

void showItems(TEXT_FORM *pForm, ITEM_TEXT_LIST &data)
{ 
   ASSERT(pForm!=NULL);

   // make sure that highlight flags are set after clearing form
   //pForm->ClearForm();

   int totalListItems = data.GetCount();
   memset(InventoryRects, 0, sizeof(InventoryRects));
   int ir_idx = 0;   

   //pForm->SetText(STIF_title, data.title);  
   
   if (data.UseRdy)
     pForm->SetText( STIF_READY,"READY");
   else
     pForm->SetText( STIF_READY,"");
   
   if (data.UseQty)
     pForm->SetText( STIF_QUANTITY,"QTY");
   else
     pForm->SetText( STIF_QUANTITY,"");

   if (data.UseCost)
     pForm->SetText( STIF_COST,"COST");
   else
     pForm->SetText( STIF_COST,"");

   pForm->SetText( STIF_NAME,"NAME");
 
   int platinum = 0;
   int gold = 0;
   int electrum = 0;
   int silver = 0;
   int copper = 0;
   int uc6=0;
   int uc7=0;
   int uc8=0;
   int uc9=0;
   int uc10=0;
   CString temp="";

   if (data.UseMoney)
   {     
     CHARACTER& dude = party.getActiveChar(); 
     if (party.moneyPooled)
     {  
        temp = "POOLED MONEY";
        platinum = party.poolSack[PlatinumType] + dude.money[PlatinumType];
        gold = party.poolSack[GoldType] + dude.money[GoldType];
        electrum = party.poolSack[ElectrumType] + dude.money[ElectrumType];
        silver = party.poolSack[SilverType] + dude.money[SilverType];
        copper = party.poolSack[CopperType] + dude.money[CopperType];
        uc6 = party.poolSack[CoinType6] + dude.money[CoinType6];
        uc7 = party.poolSack[CoinType7] + dude.money[CoinType7];
        uc8 = party.poolSack[CoinType8] + dude.money[CoinType8];
        uc9 = party.poolSack[CoinType9] + dude.money[CoinType9];
        uc10 = party.poolSack[CoinType10] + dude.money[CoinType10];     
     }
     else 
     {
        //temp.Format("%s\'s MONEY", dude.GetName());
        temp.Format("%s", dude.GetName());

        platinum = dude.money[PlatinumType];
        gold = dude.money[GoldType];
        electrum = dude.money[ElectrumType];
        silver = dude.money[SilverType];
        copper = dude.money[CopperType];
        uc6 = dude.money[CoinType6];
        uc7 = dude.money[CoinType7];
        uc8 = dude.money[CoinType8];
        uc9 = dude.money[CoinType9];
        uc10 = dude.money[CoinType10];
     }

     pForm->SetText( STIF_MONEYLABEL, temp);

     if (dude.money.IsActive(PlatinumType))
     {
        pForm->SetText( STIF_PLAT, globalData.moneyData.GetName(PlatinumType));
        temp.Format("%6i", platinum);
        pForm->SetText( STIF_plat, temp);
     }
     else
     {
       CString tmpname(globalData.moneyData.GetName(PlatinumType));
       if (tmpname.IsEmpty()) tmpname="PLATINUM";
       pForm->SetText( STIF_PLAT, globalData.moneyData.GetName(PlatinumType));
       pForm->EnableItem(STIF_PLAT ,false);
       pForm->SetText( STIF_plat, "");
     }
 
     if (dude.money.IsActive(GoldType))
     {
        pForm->SetText( STIF_GOLD, globalData.moneyData.GetName(GoldType));
        temp.Format("%6i", gold);
        pForm->SetText( STIF_gold, temp);
     }
     else
     {
       pForm->SetText( STIF_GOLD, "");
       pForm->SetText( STIF_gold, "");
     }
 
     if (dude.money.IsActive(ElectrumType))
     {
        pForm->SetText( STIF_ELEC, globalData.moneyData.GetName(ElectrumType));
        temp.Format("%6i", electrum);
        pForm->SetText( STIF_elec, temp);
     }
     else
     {
       pForm->SetText( STIF_ELEC, "");
       pForm->SetText( STIF_elec, "");
     }
 
     if (dude.money.IsActive(SilverType))
     {
        pForm->SetText( STIF_SILVER, globalData.moneyData.GetName(SilverType));
        temp.Format("%6i", silver);
        pForm->SetText( STIF_silver, temp);
     }
     else
     {
       pForm->SetText( STIF_SILVER, "");
       pForm->SetText( STIF_silver, "");
     }
 
     if (dude.money.IsActive(CopperType))
     {  
        pForm->SetText( STIF_COPPER, globalData.moneyData.GetName(CopperType));
        temp.Format("%6i", copper);
        pForm->SetText( STIF_copper, temp);
     }
     else
     {
       pForm->SetText( STIF_COPPER, "");
       pForm->SetText( STIF_copper, "");
     }

     if (dude.money.IsActive(CoinType6))
     {  
       pForm->SetText( STIF_COIN6, globalData.moneyData.GetName(CoinType6));
       temp.Format("%6i", uc6);
       pForm->SetText( STIF_coin6, temp);
     }
     else
     {
       pForm->SetText( STIF_COIN6, "");
       pForm->SetText( STIF_coin6, "");
     }

     if (dude.money.IsActive(CoinType7))
     {  
       pForm->SetText( STIF_COIN7, globalData.moneyData.GetName(CoinType7));
       temp.Format("%6i", uc7);
       pForm->SetText( STIF_coin7, temp);
     }
     else
     {
       pForm->SetText( STIF_COIN7, "");
       pForm->SetText( STIF_coin7, "");
     }

     if (dude.money.IsActive(CoinType8))
     {  
       pForm->SetText( STIF_COIN8, globalData.moneyData.GetName(CoinType8));
       temp.Format("%6i", uc8);
       pForm->SetText( STIF_coin8, temp);
     }
     else
     {
       pForm->SetText( STIF_COIN8, "");
       pForm->SetText( STIF_coin8, "");
     }

     if (dude.money.IsActive(CoinType9))
     {  
       pForm->SetText( STIF_COIN9, globalData.moneyData.GetName(CoinType9));
       temp.Format("%6i", uc9);
       pForm->SetText( STIF_coin9, temp);
     }
     else
     {
       pForm->SetText( STIF_COIN9, "");
       pForm->SetText( STIF_coin9, "");
     }

     if (dude.money.IsActive(CoinType10))
     {  
       pForm->SetText( STIF_COIN10, globalData.moneyData.GetName(CoinType10));
       temp.Format("%6i", uc10);
       pForm->SetText( STIF_coin10, temp);
     }
     else
     {
       pForm->SetText( STIF_COIN10, "");
       pForm->SetText( STIF_coin10, "");
     }
   }

   // blank out all repeat items
   for (int i=0;i<pagesize;i++)
   {
      int EnumOffset = (i*5);
      pForm->SetText(STIF_ready+EnumOffset,    "");
      pForm->SetText(STIF_quantity+EnumOffset, "");
      pForm->SetText(STIF_cost+EnumOffset,     "");
      pForm->SetText(STIF_name+EnumOffset,     "");
      pForm->SetText(STIF_Row+EnumOffset,      "");
   }

   int currItem = startItem;
   int displaycount = 0;
   CRect tmpRECT,totalRECT;
   while ((currItem<totalListItems) && (displaycount<pagesize))
   {     
     int EnumOffset = (displaycount*TEXT_FORM::repeatIncr);
      totalRECT = pForm->SetText(STIF_ready+EnumOffset,    data.GetItemRdyText(currItem));

      tmpRECT = pForm->SetText(STIF_quantity+EnumOffset, data.GetItemQtyText(currItem));
      totalRECT.UnionRect(totalRECT, tmpRECT);

      tmpRECT = pForm->SetText(STIF_cost+EnumOffset,     data.GetItemCostText(currItem));
      totalRECT.UnionRect(totalRECT, tmpRECT);
      
      tmpRECT = pForm->SetText(STIF_name+EnumOffset,     data.GetItemNameText(currItem));
      totalRECT.UnionRect(totalRECT, tmpRECT);
      
      pForm->SetText(STIF_Row+EnumOffset,      "");

      pForm->Highlight(STIF_name+EnumOffset, shouldHighlight(currItem)?true:false);
      
      CopyRect(&InventoryRects[ir_idx], &totalRECT);

      ir_idx++;
      currItem++;
      displaycount++;
   }
}

static bool STIF_highlight (TEXT_FORM *frm, 
                            int *oldSelection,
                            int newSelection) 
{
  // See if new selection is valid.
  // If so, set old selection.
  // If old selection changes, highlight it and return true.
  if (newSelection<0) return false;
  for (int i=0;i<frm->NumItems();i++)
  {
    if (newSelection==(STIF_Row+i))
    {
      if (newSelection != *oldSelection)
      {
        frm->Highlight(STIF_ready+i);
        frm->Highlight(STIF_quantity+i);
        frm->Highlight(STIF_cost+i);
        frm->Highlight(STIF_name+i);
        *oldSelection=newSelection;
        return true;
      }
    }
  }
  return false;
}

void displayItems(ITEM_TEXT_LIST &data)
{
  TEXT_FORM ItemsForm(itemsForm);
  ItemsForm.ClearForm();
  showItems(&ItemsForm,data);
  ItemsForm.Display(0);
}

int handleItemsFormInput(ITEMS_FORM_INPUT_MESSAGE_TYPE msg,
                         ITEM_TEXT_LIST &data,
                         int P1, 
                         int P2)
// Returns 0x0001 if you should display menu and flip surfaces.
// Returns 0x0002 if we handled a keystroke and you can ignore it
{
  TEXT_FORM ItemsForm(itemsForm);
  //static int currentSelection;
  //int newSelection;
  int result=0;
  bool flip=false, redraw=false;

  switch (msg)
  {
  case ITEMSFORM_initial:
    //currentSelection=ItemsForm.FirstSelectable();
    redraw=true;
    break;
  case ITEMSFORM_refresh:
    redraw=true;
    break;
    /*
  case ITEMSFORM_mouseClick:
    {
    ItemsForm.ClearForm();
    int junk=-1;
    STIF_highlight(&ItemsForm,&junk,currentSelection);
    showItems(&ItemsForm, data);  // To define rectangles.
    newSelection=ItemsForm.MouseClick(P1, P2);  // P1=x, P2=y
    if (STIF_highlight(&ItemsForm,&currentSelection,newSelection)) flip=true;
    }
    break;
  */
  case ITEMSFORM_exit:
    break;
  /*
  case ITEMSFORM_key:
    switch(P1)
    {
    case KC_UP:
      //party.prevItem();
      //result|=2;
      break;
    case KC_DOWN:      
      //party.nextItem();
      //result|=2;
      break;
    };
    break;
  */
  default:
    ASSERT(0);
    return 0;  // Internal error
  };
  if (redraw)
  {     
    ItemsForm.ClearForm();
    //int junk=-1;
    //STIF_highlight(&ItemsForm,&junk,currentSelection);
    showItems(&ItemsForm, data);  // To define rectangles.
    flip=true;
  };
  if (flip)
  {
    //GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);   
    //drawBorder();
    ItemsForm.Display(0);
  };
  return result|(flip?1:0);
}
