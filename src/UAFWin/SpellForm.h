/******************************************************************************
* Filename: SpellForm.h
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
#ifndef __SPELLFORM_H__
#define __SPELLFORM_H__

#include "TextForm.h"
#include "Spell.h"

enum SPELL_FORM_INPUT_MESSAGE_TYPE
{
  SPELLFORM_initial,
  SPELLFORM_refresh,
  SPELLFORM_mouseClick,
  SPELLFORM_key,      
  SPELLFORM_exit      
};

void displaySpells(SPELL_TEXT_LIST &data);
int handleSpellFormInput(SPELL_FORM_INPUT_MESSAGE_TYPE msg,
                         SPELL_TEXT_LIST &data,
                         DISPLAY_FORM *pDisplayForm=NULL);

#endif
