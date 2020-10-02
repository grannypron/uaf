/******************************************************************************
* Filename: Treas.cpp
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

#include "externs.h"
#include "Dungeon.h"

#include "items.h"
#include "char.h"
#include "GlobalData.h"
#include "level.h"
#include "GameEvent.h"
#include "party.h"
//#include "monster.h"
//#include "combatants.h"
#include "treas.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//*****************************************************************************
// NAME: getTreasureEventItems
//
// PURPOSE:  
//*****************************************************************************
void getTreasureEventItems(GameEvent *event)
{
	int i;
  UINT flags = QTY;
  if (party.PartyIsDetectingMagic())
    flags |= DETECTING_MAGIC;
  if (event == NULL)
    return;

  switch (event->GetEventType())
  {
  case ShopEvent:
  case TempleEvent:
    addPooledMoneyToTreasure(party);
    treasureItemListText.FormatItemListTitle("NOTHING POOLED");
    break;

  case CombatTreasure:
  case GiveTreasure:  
    for (i=0;i<((GIVE_TREASURE_DATA*)event)->moneyRemain.NumCoinTypes();i++)
    {
      itemClassType srcType = MONEY_DATA_TYPE::GetItemClass(i);
      treasureItemListText.Add(((GIVE_TREASURE_DATA*)event)->moneyRemain[i],
                               srcType, 
                               globalData.moneyData.GetName(srcType));
    }

    treasureItemListText.Add(((GIVE_TREASURE_DATA*)event)->moneyRemain.NumGems(),
                             GemType,
                             ((GIVE_TREASURE_DATA*)event)->moneyRemain.Name(GemType));
    treasureItemListText.Add(((GIVE_TREASURE_DATA*)event)->moneyRemain.NumJewelry(),
                             JewelryType,
                             ((GIVE_TREASURE_DATA*)event)->moneyRemain.Name(JewelryType));

    treasureItemListText.AddItemListText(((GIVE_TREASURE_DATA*)event)->itemsRemain,
                                         flags,
                                         Normal,
                                         0);

    if (party.moneyPooled)
      addPooledMoneyToTreasure(party);

    treasureItemListText.FormatItemListTitle("NO TREASURE");
    break;

  case Vault:
    {
      BYTE vaultIdx = ((VAULT_EVENT_DATA*)event)->WhichVault;

      treasureItemListText.FillItemListText(globalData.vault[vaultIdx].items, flags);

      for (i=0;i<globalData.vault[vaultIdx].money.NumCoinTypes();i++)
      {
        itemClassType srcType = MONEY_DATA_TYPE::GetItemClass(i);
        treasureItemListText.Add(globalData.vault[vaultIdx].money[i], srcType, globalData.moneyData.GetName(srcType));
      }

      treasureItemListText.Add(globalData.vault[vaultIdx].money.NumGems(),GemType,globalData.vault[vaultIdx].money.Name(GemType));
      treasureItemListText.Add(globalData.vault[vaultIdx].money.NumJewelry(),JewelryType,globalData.vault[vaultIdx].money.Name(JewelryType));
      treasureItemListText.FormatItemListTitle("NO ITEMS IN VAULT");
    }
    break;

  default:    
    WriteDebugString("Unhandled event type %u in getTreasureEventItems()\n", 
                     event->GetEventType());
    die(0xab4f4);
    break;
  }
}


//*****************************************************************************
// NAME: addPooledMoneyToTreasure
//
// PURPOSE:  
//*****************************************************************************
void addPooledMoneyToTreasure(PARTY& pty)
{
  if (!pty.moneyPooled)
    return;

  treasureItemListText.UseQty = TRUE;

  for (int i=0;i<pty.poolSack.NumCoinTypes();i++)
  {
    if (pty.poolSack.IsActive(globalData.moneyData.GetItemClass(i)))
    {
      itemClassType srcType = MONEY_DATA_TYPE::GetItemClass(i);
      treasureItemListText.Add(pty.poolSack[i],srcType,pty.poolSack.Name(srcType));
    }
  }
  treasureItemListText.Add(pty.poolSack.NumGems(),GemType,pty.poolSack.Name(GemType));
  treasureItemListText.Add(pty.poolSack.NumJewelry(),JewelryType,pty.poolSack.Name(JewelryType));
}

//*****************************************************************************
// NAME: takeTreasureMoney
//
// PURPOSE:  
//*****************************************************************************
BOOL takeTreasureMoney(itemClassType type, int qty, CHARACTER& dude)
{
  if (qty == 0) 
    return FALSE;

	int total = qty; 
  int CoinsPerUnitOfEncumbrance = globalData.moneyData.GetWeight();
  if (CoinsPerUnitOfEncumbrance > 0)
  {
    total = total / CoinsPerUnitOfEncumbrance;
    total = max(1, total);
  }
  else
    total = 0;

  if ((total + dude.GetEncumbrance()) > dude.GetAdjMaxEncumbrance())
  {
		SetMiscError(TooMuchWeight);
    return FALSE;
  }

  int i;
  switch (type) 
  {
  case GemType:
    for (i=0;i<qty;i++)
      dude.money.AddGem();
    break;
  case JewelryType:
    for (i=0;i<qty;i++)
      dude.money.AddJewelry();
    break;
  default:
    dude.money.Add(type, qty);
    break;
  }
  
 	dude.SetEncumbrance(dude.determineEffectiveEncumbrance());
	dude.determineMaxMovement();

  return TRUE;
}


//*****************************************************************************
// NAME: takeTreasure
//
// PURPOSE:  
//*****************************************************************************
//BOOL takeTreasure(GLOBAL_ITEM_ID giID, int qty, int charges, int id, CHARACTER& dude, int cost)
BOOL takeTreasure(const ITEM_ID& itemID, int qty, int charges, int id, CHARACTER& dude, int cost)
{
  //return (dude.addCharacterItem(giID, qty, charges, id, cost));
  return (dude.addCharacterItem(itemID, qty, charges, id, cost));
}
