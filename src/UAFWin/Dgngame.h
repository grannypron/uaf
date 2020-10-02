/******************************************************************************
* Filename: Dgngame.h
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
#ifndef DGNGAME_H_
#define DGNGAME_H_

#include "dungeon.h"
#include "Thread.h"

BOOL serializeGame(BOOL save, int num);
BOOL readDatabase(void);
void scheduler( void );
BOOL InitGame(HWND hwnd);
void ShutdownGame(void);
void InitGameStructures(void);
void ResetAdventureData(void);
BOOL LoadOffscreenWalkingSurfaces(void);
BOOL LoadOffscreenCombatSurfaces(BOOL UseOutdoor);
BOOL DesignAvail(const char *path);
BOOL InitDesignData();
void StartBackgroundMusic();
void StartBackgroundZoneOnlyMusic();
void StopBackgroundMusic();


#endif

