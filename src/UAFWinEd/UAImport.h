/******************************************************************************
* Filename: UAImport.h
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
#include "char.h"
#include "monster.h"

void ClearItemBuffers(void);
void ClearMonsterBuffers(void);
void ClearImportBuffers(void);
BOOL ImportGameDat(const char *path);
BOOL ImportUAItemsToUAF(const char *path);
BOOL ImportMonsterToUAF(const char *path);
extern int ImportMonsterCount;
extern int ImportNPCCount;
extern bool UseDefaultArtForImport;
