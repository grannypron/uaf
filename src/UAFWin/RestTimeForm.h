/******************************************************************************
* Filename: RestTimeForm.h
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
#ifndef __RESTTIMEFORM_H__
#define __RESTTIMEFORM_H__

#include "externs.h"
#include "TextForm.h"
#include "GameEvent.h"

#define RTF_DAYS_FIELD  0
#define RTF_HOURS_FIELD  1
#define RTF_MINUTES_FIELD  2

enum RESTTIME_INPUT_MESSAGE_TYPE
{
  RESTTIME_initial,    
  RESTTIME_refresh,    
  RESTTIME_mouseClick, 
  RESTTIME_key,
  RESTTIME_activate,
  RESTTIME_exit        
};

void displayRestTime(REST_MENU_DATA *pData);
int  handleRestTimeInput(RESTTIME_INPUT_MESSAGE_TYPE msg,
                         REST_MENU_DATA *pData,
                         int P1=0, 
                         int P2=0);

#endif
