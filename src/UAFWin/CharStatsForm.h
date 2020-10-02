/******************************************************************************
* Filename: CharStatsForm.h
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
#ifndef __CHARSTATSFORM_H__
#define __CHARSTATSFORM_H__

//#include "externs.h"
#include "char.h"
#include "TextForm.h"

enum CHAR_STATS_INPUT_MESSAGE_TYPE
{
  CHOOSESTATS_initial,    // (character)
  CHOOSESTATS_refresh,    // (character) redraw the screen
  CHOOSESTATS_mouseClick, // (character, x, y)
  CHOOSESTATS_key,        // (character, key)
  CHOOSESTATS_exit        // (void)
};

struct SELECTION_PAGE_FORMAT;

void displayCharStats(CHARACTER &dude);
void displayCreatedCharStats(CHARACTER &dude);
int  handleChooseStatsInput(CHAR_STATS_INPUT_MESSAGE_TYPE msg,
                            CHARACTER *dude, 
                            int P1, 
                            int P2,
                            const SELECTION_PAGE_FORMAT *format
                            );
extern char *CharStatusTypeText[];
extern char *CharGenderTypeText[];
extern char *CharAlignmentTypeText[];

#endif
