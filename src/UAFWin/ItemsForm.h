/******************************************************************************
* Filename: ItemsForm.h
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
#ifndef __ITEMSFORM_H__
#define __ITEMSFORM_H__

#include "TextForm.h"
#include "items.h"

enum ITEMS_FORM_INPUT_MESSAGE_TYPE
{
  ITEMSFORM_initial, 
  ITEMSFORM_refresh, 
  ITEMSFORM_mouseClick,
  ITEMSFORM_key,       
  ITEMSFORM_exit       
};

void displayItems(ITEM_TEXT_LIST &data);
int handleItemsFormInput(ITEMS_FORM_INPUT_MESSAGE_TYPE msg,
                         ITEM_TEXT_LIST &data,
                         int P1=0, 
                         int P2=0);

#endif
