/******************************************************************************
* Filename: Treas.h
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
#ifndef __TREAS_H_
#define __TREAS_H_ 

#include "items.h"
#include "char.h"
#include "party.h"

// this was used in an early UAF version, back when database info was
// actually read from a Microsoft Access database. This was a start towards
// defining the standard AD&D treasure types.
/*
#define MAX_TREASURE 30

typedef struct _treas_data_ {
	long Cop_Base;
	long Cop_Mult;
	long Cop_Pcnt;
	long Silv_Base;
	long Silv_Mult;
	long Silv_Pcnt;
	long Gold_Base;
	long Gold_Mult;
	long Gold_Pcnt;
	long Elect_Base;
	long Elect_Mult;
	long Elect_Pcnt;
	long Plat_Base;
	long Plat_Mult;
	long Plat_Pcnt;
	long Gems_Base;
	long Gems_Mult;
	long Gems_Pcnt;
	long Magic_Item_1_Qty;
	long Magic_Item_1_Pcnt;
	long Magic_Item_1;
	long Magic_Item_2_Qty;
	long Magic_Item_2_Pcnt;
	long Magic_Item_2;
	long Magic_Item_3_Qty;
	long Magic_Item_3_Pcnt;
	long Magic_Item_3;
	long Magic_Item_4_Qty;
	long Magic_Item_4_Pcnt;
	long Magic_Item_4;
	long Magic_Item_5_Qty;
	long Magic_Item_5_Pcnt;
	long Magic_Item_5;
} TREASURE_DATA;

extern int numTreas;
extern TREASURE_DATA treasureData[MAX_TREASURE];
*/

void getTreasureEventItems(GameEvent *event);

void addPooledMoneyToTreasure(PARTY& pty);
BOOL takeTreasureMoney(itemClassType type, int qty, CHARACTER& dude);
//BOOL takeTreasure(GLOBAL_ITEM_ID giID, int qty, int charges, int id, CHARACTER& dude, int cost);
BOOL takeTreasure(const ITEM_ID& itemID, int qty, int charges, int id, CHARACTER& dude, int cost);

#endif