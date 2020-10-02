/******************************************************************************
* Filename: ItemDB.h
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
#ifndef __ITEMDB_H__
#define __ITEMDB_H__

enum CONFIG_ITEM_STATUS;
class CONFIG_FILE;
class RACE_DATA_TYPE;
class ABILITY_DATA_TYPE;
class CLASS_DATA_TYPE;
class BASE_CLASS_DATA_TYPE;
class TRAIT_DATA_TYPE;


int ReadItemDB(LPCSTR file, 
               ITEM_DATA_TYPE *itemsPointer, 
               CONFIG_FILE *pConfigFile=NULL);
int ReadItemDB(CString &file, 
               ITEM_DATA_TYPE *itemsPointer,
               CONFIG_FILE *pConfigFile=NULL);
int WriteItemDB(LPCTSTR file, 
                ITEM_DATA_TYPE *itemsPointer,
                CONFIG_FILE *pConfigFile);

int ReadMonsterDB(CString &file,
                  MONSTER_DATA_TYPE *monstersPointer,
                  CONFIG_FILE *pConfigFile=NULL);
int ReadMonsterDB(LPCSTR file,
                  MONSTER_DATA_TYPE *monstersPointer,
                  CONFIG_FILE *pConfigFile=NULL);
int WriteMonsterDB(LPCTSTR file,
                   MONSTER_DATA_TYPE *monstersPointer,
                   CONFIG_FILE *pConfigFile);
#ifdef OLD_SPECIAL_ABILITIES
#else
int ReadSpecialAbilitiesDB(LPCSTR file, 
                           A_SPECABILITY_DEF_L *itemsPointer, 
                           CONFIG_FILE *pConfigFile=NULL);

int WriteSpecAbDB(LPCTSTR file,
                   A_SPECABILITY_DEF_L *specAbPointer,
                   CONFIG_FILE *pConfigFile);
#endif


int ReadSpellDB(CString &file,
                SPELL_DATA_TYPE *spellsPointer,
                CONFIG_FILE *pConfigFile=NULL);
int ReadSpellDB(LPCSTR file,
                SPELL_DATA_TYPE *spellsPointer,
                CONFIG_FILE *pConfigFile=NULL);
int WriteSpellDB(LPCTSTR file,
                 SPELL_DATA_TYPE *spellsPointer,
                 CONFIG_FILE *pConfigFile);


int ReadRaceDB(CString &file,
               RACE_DATA_TYPE *racesPointer,
               CONFIG_FILE *pConfigFile=NULL);
int ReadRaceDB(LPCSTR file,
               RACE_DATA_TYPE *racesPointer,
               CONFIG_FILE *pConfigFile=NULL);
int WriteRaceDB(LPCTSTR file,
                RACE_DATA_TYPE *racesPointer,
                CONFIG_FILE *pConfigFile);

int ReadAbilityDB(CString &file,
                  ABILITY_DATA_TYPE *abilitiesPointer,
                  CONFIG_FILE *pConfigFile=NULL);
int ReadAbilityDB(LPCSTR file,
                  ABILITY_DATA_TYPE *abilitiesPointer,
                  CONFIG_FILE *pConfigFile=NULL);
int WriteAbilityDB(LPCTSTR file,
                   ABILITY_DATA_TYPE *abiliiesPointer,
                   CONFIG_FILE *pConfigFile);


int ReadSpellgroupDB(CString &file,
                     SPELLGROUP_DATA_TYPE *spellgroupPointer,
                     CONFIG_FILE *pConfigFile=NULL);
int ReadSpellgroupDB(LPCSTR file,
                     SPELLGROUP_DATA_TYPE *spellgroupPointer,
                     CONFIG_FILE *pConfigFile=NULL);
int WriteSpellgroupDB(LPCTSTR file,
                      SPELLGROUP_DATA_TYPE *spellgroupPointer,
                      CONFIG_FILE *pConfigFile);


int ReadBaseclassDB(CString &file,
                    BASE_CLASS_DATA_TYPE *baseclassesPointer,
                    CONFIG_FILE *pConfigFile=NULL);
int ReadBaseclassDB(LPCSTR file,
                    BASE_CLASS_DATA_TYPE *baseclassesPointer,
                    CONFIG_FILE *pConfigFile=NULL);
int WriteBaseclassDB(LPCTSTR file,
                     BASE_CLASS_DATA_TYPE *baseclassesPointer,
                     CONFIG_FILE *pConfigFile);

int ReadClassDB(CString &file,
                CLASS_DATA_TYPE *clasesPointer,
                CONFIG_FILE *pConfigFile=NULL);
int ReadClassDB(LPCSTR file,
                CLASS_DATA_TYPE *classesPointer,
                CONFIG_FILE *pConfigFile=NULL);
int WriteClassDB(LPCTSTR file,
                 CLASS_DATA_TYPE *classesPointer,
                 CONFIG_FILE *pConfigFile);

int ReadTraitDB(CString &file,
                TRAIT_DATA_TYPE *traitsPointer,
                CONFIG_FILE *pConfigFile=NULL);
int ReadTraitDB(LPCSTR file,
                TRAIT_DATA_TYPE *traitsPointer,
                CONFIG_FILE *pConfigFile=NULL);
int WriteTraitDB(LPCTSTR file,
                 TRAIT_DATA_TYPE *traitsPointer,
                 CONFIG_FILE *pConfigFile);

#endif 
