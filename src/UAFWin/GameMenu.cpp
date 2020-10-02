/******************************************************************************
* Filename: GameMenu.cpp
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
#include "Graphics.h"
#include "GameMenu.h" 
#include "Viewport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// defaults for 640x480
int DEFAULT_MENU_HORZ_X = 14;
int DEFAULT_MENU_HORZ_Y = 460;
int DEFAULT_MENU_COMBAT_HORZ_X = 14;
int DEFAULT_MENU_COMBAT_HORZ_Y = 460;
int DEFAULT_COMBAT_STAT_X = 400;
int DEFAULT_COMBAT_STAT_Y = 64;
int DEFAULT_MENU_VERT_X = 200;
int DEFAULT_MENU_VERT_Y = 200;
int DEFAULT_MENU_TEXTBOX_X = 200;
int DEFAULT_MENU_TEXTBOX_Y = 328;

////////////////////////////////////////////////
//
//  Menu Text
//
///////////////////////////////////////////////

//
// menus that use absolute coords
//
//enum { AddCharMenuX=18, AddCharMenuY=18 };
//enum { RaceMenuX=43, RaceMenuY=80 };
//enum { ChangeClassMenuX=RaceMenuX, ChangeClassMenuY=RaceMenuY };
//enum { GenderMenuX=RaceMenuX, GenderMenuY=RaceMenuY };
//enum { ClassMenuX=RaceMenuX, ClassMenuY=RaceMenuY };
//enum { AlignmentMenuX=RaceMenuX, AlignmentMenuY=RaceMenuY };

//
// vertical menus using defaults
//
//enum { StartMenuX=DEFAULT_VERT_X, StartMenuY=DEFAULT_VERT_Y };
//enum { MainMenuX=DEFAULT_VERT_X, MainMenuY=DEFAULT_VERT_Y };

//
// horizontal menus using defaults
//
//enum { QuestMenuX=DEFAULT_HORZ_X, QuestMenuY=DEFAULT_HORZ_Y };
//enum { SmallTownMenuX=DEFAULT_HORZ_X, SmallTownMenuY=DEFAULT_HORZ_Y };
//enum { QYesNoMenuX=DEFAULT_HORZ_X, QYesNoMenuY=DEFAULT_HORZ_Y };
//enum { PressEnterMenuX=DEFAULT_HORZ_X, PressEnterMenuY=DEFAULT_HORZ_Y };
//enum { TransferMenuX=DEFAULT_HORZ_X, TransferMenuY=DEFAULT_HORZ_Y };
//enum { IconPicMenuX=DEFAULT_HORZ_X, IconPicMenuY=DEFAULT_HORZ_Y };
//enum { TextStatementMenuX=DEFAULT_HORZ_X, TextStatementMenuY=DEFAULT_HORZ_Y };
//enum { GPDLEventMenuX=DEFAULT_HORZ_X, GPDLEventMenuY=DEFAULT_HORZ_Y };
//enum { TradeTakerMenuX=DEFAULT_HORZ_X, TradeTakerMenuY=DEFAULT_HORZ_Y };
//enum { GetMoneyQtyX=DEFAULT_HORZ_X, GetMoneyQtyY=DEFAULT_HORZ_Y };
//enum { EmptyMenuX=DEFAULT_HORZ_X, EmptyMenuY=DEFAULT_HORZ_Y };
//enum { GetCharNameX=DEFAULT_HORZ_X, GetCharNameY=DEFAULT_HORZ_Y };
//enum { GetPasswordX=DEFAULT_HORZ_X, GetPasswordY=DEFAULT_HORZ_Y };
//enum { TrainWelcomeMenuX=DEFAULT_HORZ_X, TrainWelcomeMenuY=DEFAULT_HORZ_Y };
//enum { ShopMenuX=DEFAULT_HORZ_X, ShopMenuY=DEFAULT_HORZ_Y };
//enum { BuyMenuX=DEFAULT_HORZ_X, BuyMenuY=DEFAULT_HORZ_Y };
//enum { TempleMenuX=DEFAULT_HORZ_X, TempleMenuY=DEFAULT_HORZ_Y };
//enum { TempleDonateMenuX=DEFAULT_HORZ_X, TempleDonateMenuY=DEFAULT_HORZ_Y };
//enum { TempleHealMenuX=DEFAULT_HORZ_X, TempleHealMenuY=DEFAULT_HORZ_Y };
//enum { TempleCastMenuX=DEFAULT_HORZ_X, TempleCastMenuY=DEFAULT_HORZ_Y };
//enum { TempleGiveMenuX=DEFAULT_HORZ_X, TempleGiveMenuY=DEFAULT_HORZ_Y };
//enum { TavernMenuX=DEFAULT_HORZ_X, TavernMenuY=DEFAULT_HORZ_Y };
//enum { ChooseStatsX=DEFAULT_HORZ_X, ChooseStatsY=DEFAULT_HORZ_Y };
//enum { ViewCharMenuX=DEFAULT_HORZ_X, ViewCharMenuY=DEFAULT_HORZ_Y };
//enum { MoveMenuX=DEFAULT_HORZ_X, MoveMenuY=DEFAULT_HORZ_Y };
//enum { ExitMenuX=DEFAULT_HORZ_X, ExitMenuY=DEFAULT_HORZ_Y };
//enum { MagicMenuX=DEFAULT_HORZ_X, MagicMenuY=DEFAULT_HORZ_Y };
//enum { MemorizeMenuX=DEFAULT_HORZ_X, MemorizeMenuY=DEFAULT_HORZ_Y };
//enum { ScribeMenuX=DEFAULT_HORZ_X, ScribeMenuY=DEFAULT_HORZ_Y };
//enum { EncounterMenuX=DEFAULT_HORZ_X, EncounterMenuY=DEFAULT_HORZ_Y };
//enum { LevelMenuX=DEFAULT_HORZ_X, LevelMenuY=DEFAULT_HORZ_Y };
//enum { AlterMenuX=DEFAULT_HORZ_X, AlterMenuY=DEFAULT_HORZ_Y };
//enum { ItemsMenuX=DEFAULT_HORZ_X, ItemsMenuY=DEFAULT_HORZ_Y };
//enum { AdventureMenuX=DEFAULT_HORZ_X, AdventureMenuY=DEFAULT_HORZ_Y };
//enum { EncampMenuX=DEFAULT_HORZ_X, EncampMenuY=DEFAULT_HORZ_Y };
//enum { SaveGameMenuX=DEFAULT_HORZ_X, SaveGameMenuY=DEFAULT_HORZ_Y };
//enum { CombatMenuX=DEFAULT_HORZ_X, CombatMenuY=DEFAULT_HORZ_Y };
//enum { SpeedMenuX=DEFAULT_HORZ_X, SpeedMenuY=DEFAULT_HORZ_Y };
//enum { AimMenuX=DEFAULT_HORZ_X, AimMenuY=DEFAULT_HORZ_Y };
//enum { TreasureMenuX=DEFAULT_HORZ_X, TreasureMenuY=DEFAULT_HORZ_Y };
//enum { TreasureWelcomeMenuX=DEFAULT_HORZ_X, TreasureWelcomeMenuY=DEFAULT_HORZ_Y };
//enum { VaultMenuX=DEFAULT_HORZ_X, VaultMenuY=DEFAULT_HORZ_Y };


MENU_ENTRY EmptyMenu[EmptyMenuItems] = {
  { "", -1 }
};

MENU_DATA_TYPE EmptyMenuData =
{
  "",
  -1, 0,
  EmptyMenuItems,
  EmptyMenu
};

char *QuestMenuTitle = "CHOOSE:";

MENU_ENTRY QuestMenu[QuestMenuItems] = {
  { "Yes", 0 },
  { "No", 0 }
};

MENU_DATA_TYPE QuestMenuData =
{
  QuestMenuTitle,
  -1, 0,
  QuestMenuItems,
  QuestMenu
};

/*

MENU_ENTRY ChangeClassMenu[ChangeClassMenuItems] = {
  { "CLERIC", 0 },
  { "DRUID", 0 },
  { "FIGHTER", 0 },
  { "RANGER", 0 },
  { "PALADIN", 0 },
  { "MAGIC USER", 0 },
  { "THIEF", 0 },
  { "EXIT", 1 }
};

char *ChangeClassMenuTitle = "CHOOSE NEW CLASS";

MENU_DATA_TYPE ChangeClassMenuData =
{
  ChangeClassMenuTitle,
  ChangeClassMenuX, ChangeClassMenuY,
  ChangeClassMenuItems,
  ChangeClassMenu
};

*/

/*
MENU_ENTRY ChangeClassMenu[ChangeClassMenuItems] = {
   { "CHANGE", 0 },
   { "NEXT", 0 },
   { "PREV", 0 }
};

MENU_DATA_TYPE ChangeClassMenuData =
{
  "",
  -1, 0,
  ChangeClassMenuItems,
  ChangeClassMenu
};
*/

MENU_ENTRY WhoPaysMenu[WhoPaysMenuItems] = 
{
  { "WHO PAYS", 0 },
  { "EXIT", 1 }
};

MENU_DATA_TYPE WhoPaysMenuData =
{
  "",
  -1, 0,
  WhoPaysMenuItems,
  WhoPaysMenu
};

MENU_ENTRY TargetSelectNonCombatMenu[TargetSelectNonCombatMenuItems] = 
{
  { "CAST SPELL ON?", 0 },
  { "EXIT", 1 }
};

MENU_DATA_TYPE TargetSelectNonCombatMenuData =
{
  "CHOOSE 1",
  -1, 0,
  TargetSelectNonCombatMenuItems,
  TargetSelectNonCombatMenu
};

MENU_ENTRY SmallTownMenu[SmallTownMenuItems] = 
{
  { "TEMPLE", 0 },
  { "TRAINING HALL", 9 },
  { "SHOP", 0 },
  { "INN", 0 },
  { "PUB", 0 },
  { "VAULT", 0 },
  { "EXIT", 1 }
};

MENU_DATA_TYPE SmallTownMenuData =
{
  "",
  -1, 0,
  SmallTownMenuItems,
  SmallTownMenu
};

MENU_ENTRY VaultMenu[VaultMenuItems] = {
  { "VIEW", 0 },
  { "TAKE", 0 },
  { "POOL", 0 },
  { "SHARE", 0 },
  { "ITEMS", 0 },
  { "EXIT", 1 }
};

MENU_DATA_TYPE VaultMenuData =
{
  "",
  -1, 0,
  VaultMenuItems,
  VaultMenu
};

MENU_ENTRY QYesNoMenu[QYesNoMenuItems] = {
  { "YES", 0 },
  { "NO", 0 }
};

char *QYesNoMenuTitle = "CHOOSE: ";

MENU_DATA_TYPE QuestionYesNoData =
{ // GS_QuestionYesNo
  QYesNoMenuTitle,
  -1, 0,
  QYesNoMenuItems,
  QYesNoMenu
};

MENU_DATA_TYPE QuestionYesData =
{ // GS_QuestionYes
  "",
  -1, 0,
  PressEnterMenuItems,
  PressEnterMenu
};

MENU_DATA_TYPE QuestionNoData =
{ // GS_QuestionNo
  "",
  -1, 0,
  PressEnterMenuItems,
  PressEnterMenu
};

MENU_ENTRY PressEnterMenu[PressEnterMenuItems] = 
{
  { "PRESS ENTER TO CONTINUE", 7 }
};


MENU_DATA_TYPE PressEnterMenuData =
{
  "",
  -1, 0,
  PressEnterMenuItems,
  PressEnterMenu
};

MENU_ENTRY TransferMenu[TransferMenuItems] = {
  { "YES", 0 },
  { "NO", 0 }
};

MENU_DATA_TYPE TransferMenuData =
{
  "",
  -1, 0,
  TransferMenuItems,
  TransferMenu
};

MENU_ENTRY IconPicMenu[IconPicMenuItems] = {
  { "NEXT", 0 },
  { "PREV", 0 },
  { "SELECT", 0 }
};

MENU_DATA_TYPE GetCharIconMenuData =
{ 
  "",
  -1, 0,
  IconPicMenuItems,
  IconPicMenu
};

MENU_ENTRY TextStatementMenu[TextStatementMenuItems] = {
   { "EXIT", 1 },
   { "PRESS ENTER TO CONTINUE", 7 },
};

MENU_DATA_TYPE TextEventData =
{
  "",
  -1, 0,
  TextStatementMenuItems,
  TextStatementMenu
};

MENU_ENTRY GPDLEventMenu[GPDLEventMenuItems] = {
   { "UP/Down scrolls text", -1 },
   { "SPACE to continue", -1 },
   { "RETURN to terminate", -1 },
   { "Enter Text followed by RETURN", -1 },
   { "  (just RETURN to quit)", -1 }
};

MENU_DATA_TYPE GPDLEventMenuData =
{
  "",
  -1, 0,
  GPDLEventMenuItems,
  GPDLEventMenu
};


MENU_ENTRY TradeTakerMenu[TradeTakerMenuItems] = {
  { "SELECT", 0 },
  { "EXIT", 1 }
};

char *TradeTakerMenuTitle = { "PICK RECIPIENT:" };

MENU_DATA_TYPE TradeTakerMenuData =
{
  TradeTakerMenuTitle,
  -1, 0,
  TradeTakerMenuItems,
  TradeTakerMenu
};

MENU_ENTRY ScriptChoiceMenu[ScriptChoiceMenuItems] = {
  { "SELECT", 0 },
  { "EXIT", 1 }
};

char *ScriptChoiceMenuTitle = { "MAKE A CHOICE:" };

MENU_DATA_TYPE ScriptChoiceMenuData =
{
  ScriptChoiceMenuTitle,
  -1, 0,
  ScriptChoiceMenuItems,
  ScriptChoiceMenu
};

MENU_ENTRY TreasureMenu[TreasureMenuItems] = {
  { "NEXT", 0 },
  { "PREV", 0 },
  { "TAKE", 0 },
  { "HALVE", 0 },
  { "JOIN", 0 },
  { "EXIT", 1 }
};

MENU_DATA_TYPE TakeTreasureMenuData =
{
  "",
  -1, 0,
  TreasureMenuItems,
  TreasureMenu
};

MENU_ENTRY TreasureWelcomeMenu[TreasureWelcomeMenuItems] = {
  { "VIEW", 0 },
  { "TAKE", 0 },
  { "POOL", 0 },
  { "SHARE", 0 },
  { "DETECT", 0 },
  { "EXIT", 1 }
};

MENU_DATA_TYPE GiveTreasureMenuData =
{
  "",
  -1, 0,
  TreasureWelcomeMenuItems,
  TreasureWelcomeMenu
};

MENU_ENTRY GetMoneyQtyMenu = { NULL, -1 };
char *GetMoneyQtyTitle = { "HOW MANY WILL YOU TAKE?: " };
MENU_DATA_TYPE GetMoneyQtyMenuData =
{
  GetMoneyQtyTitle,
  -1, 0,
  GetMoneyQtyItems,
  NULL,
};

char *GetPasswordTitle = { "ENTER PASSWORD:" };
char *GetCharNameTitle = { "ENTER CHARACTER NAME:" };

char *ChooseStatsMenuTitle = { "" }; // Filled in at run-time
MENU_ENTRY ChooseStatsMenu[ChooseStatsMenuItems] = {
     {  "ReRoll", 0 },
     {  "Accept", 0 }
};

MENU_DATA_TYPE ChooseStatsMenuData =
{
  ChooseStatsMenuTitle,
  -1, 0,
  ChooseStatsMenuItems,
  ChooseStatsMenu
};

MENU_DATA_TYPE AddCharacterMenuData =
{
  "",
  AddCharMenuX, AddCharMenuY,
  0,
  ChooseStatsMenu
};

/*
char *RaceMenuTitle = { "Choose a character race:" };
MENU_ENTRY RaceMenu[RaceMenuItems] = {
      { "Human", 0 },
      { "Elf", 0 },
      { "Dwarf", 0 },
      { "Gnome", 0 },
      { "Half-Elf", 1 },
      { "Halfling", 3 },
      { "Exit", 1 }
};

MENU_DATA_TYPE RaceMenuData =
{
  RaceMenuTitle,
  RaceMenuX, RaceMenuY,
  RaceMenuItems,
  RaceMenu
};
*/
/*
char *GenderMenuTitle = { "Choose a character gender:" };
MENU_ENTRY GenderMenu[GenderMenuItems] = {
      { "Male", 0 },
      { "Female", 0 },
      { "Exit", 1 }
};

MENU_DATA_TYPE GenderMenuData =
{
  GenderMenuTitle,
  GenderMenuX, GenderMenuY,
  GenderMenuItems,
  GenderMenu
};
*/
/*
char *ClassMenuTitle = { "Choose a character class:" };
MENU_ENTRY ClassMenu[ClassMenuItems] = {
   { "Fighter", 0 },
   { "Cleric", 0 },
   { "Ranger", 0 },
   { "Paladin", 0 },
   { "Magic User", 0 },
   { "Thief", 0 },
   { "Druid", 0 },
   { "Cleric/Fighter", -1 },
   { "Cleric/Fighter/Magic User", -1 },
   { "Cleric/Ranger", -1 },
   { "Cleric/Magic User", -1 },
   { "Cleric/Thief", -1 },
   { "Fighter/Magic User", -1 },
   { "Fighter/Thief", -1 },
   { "Fighter/Magic User/Thief", -1 },
   { "Magic User/Thief", -1 },
   { "Fighter/Druid", -1 },
   { "Druid/Magic User", -1 },
   { "Fighter/Magic User/Druid", -1 },
   { "Exit", 1 }
};

MENU_DATA_TYPE ClassMenuData =
{
  ClassMenuTitle,
  ClassMenuX, ClassMenuY,
  ClassMenuItems,
  ClassMenu
};
*/
/*
char *AlignmentMenuTitle = { "Choose a character alignment:" };
MENU_ENTRY AlignmentMenu[AlignmentMenuItems] = {
   { "Lawful Good", -1 },
   { "Neutral Good", -1 },
   { "Chaotic Good", -1 },
   { "Lawful Neutral", -1 },
   { "True Neutral", -1 },
   { "Chaotic Neutral", -1 },
   { "Lawful Evil", -1 },
   { "Neutral Evil", -1 },
   { "Chaotic Evil", -1 },
   { "Exit", 1 }
};

MENU_DATA_TYPE AlignmentMenuData =
{
  AlignmentMenuTitle,
  AlignmentMenuX, AlignmentMenuY,
  AlignmentMenuItems,
  AlignmentMenu
};
*/
char *TrainWelcomeMenuTitle = "DO YOU WANT TO TRAIN?: ";

MENU_ENTRY TrainWelcomeMenu[TrainWelcomeMenuItems] = {
  { "YES", 0 },
  { "NO", 0 }
};

MENU_DATA_TYPE TrainWelcomeMenuData =
{
  TrainWelcomeMenuTitle,
  -1, 0,
  TrainWelcomeMenuItems,
  TrainWelcomeMenu
};

MENU_ENTRY StartMenu[StartMenuItems] = {
   { "Create Character", 0 },
   { "Add Character", 0 },
   { "Load Saved Game", 0 },
   { "Exit From Game",  1 }
};

MENU_DATA_TYPE StartMenuData =
{
  "",
  -2, 0,
  StartMenuItems,
  StartMenu
};

MENU_ENTRY MainMenu[MainMenuItems] = {
  /*
   { "Create Character", 0 },
   { "Delete Character", 0 },
   { "Modify Character", 0 },
   { "Train Character", 0 },
   { "Human Change Class", 0 },
   { "View Character", 0 },
   { "Add Character", 0 },
   { "Remove Character", 0 },
   { "Load Saved Game", 0 },
   { "Save Current Game", 0 },
   { "Begin Adventuring", 0 },
   { "Exit From Game", 1 }
  */
   { "Add Character", 0 },
   { "Remove Character", 0 },
   { "Modify Character", 0 },
   { "Train Character", 0 },
   { "Change Class", 0 },
   { "View Character", 0 },
   { "Create Character", 0 },
   { "Delete Character", 0 },  
   { "Load Saved Game", 0 },
   { "Save Current Game", 0 },
   { "Begin Adventuring", 0 },
   { "Exit From Game", 1 }
};

MENU_DATA_TYPE MainMenuData =
{
  "",
  -2, 0,
  MainMenuItems,
  MainMenu
};

MENU_ENTRY ViewCharMenu[ViewCharMenuItems] = {
   { "ITEMS", 0 },
   { "DISPLAY", 0 },//{ "SPELLS", 0 },
   { "TRADE", 0 }, // not used
   { "DROP", 0 },  // not used
   { "LAY", 0 },
   { "CURE", 0 },
   { "EXAMINE", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE ViewCharMenuData =
{
  "",
  -1, 0,
  ViewCharMenuItems,
  ViewCharMenu
};


MENU_ENTRY MoveMenu[MoveMenuItems] = {
   { "EXIT", 1 }
};

MENU_DATA_TYPE MoveMenuData =
{
  "",
  -1, 0,
  MoveMenuItems,
  MoveMenu
};

MENU_ENTRY ExitMenu[ExitMenuItems] = {
   { "EXIT", 1 }
};

MENU_DATA_TYPE ExitMenuData =
{
  "",
  -1, 0,
  ExitMenuItems,
  ExitMenu
};


MENU_ENTRY ShopMenu[ShopMenuItems] = {
   { "BUY", 0 },
   { "ITEMS", 0 },
   { "VIEW", 0 },
   { "TAKE", 0 },
   { "POOL", 0 },
   { "SHARE", 0 },
   { "APPRAISE", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE ShopMenuData =
{
  "",
  -1, 0,
  ShopMenuItems,
  ShopMenu
};


MENU_ENTRY TempleMenu[TempleMenuItems] = {
   { "HEAL", 0 },
   { "DONATE", 0 },
   { "VIEW", 0 },
   { "TAKE", 0 },
   { "POOL", 0 },
   { "SHARE", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE TempleMenuData =
{
  "",
  -1, 0,
  TempleMenuItems,
  TempleMenu
};

MENU_ENTRY TempleDonateMenu[TempleDonateMenuItems] = {
   { "TAKE", 0 },
   { "POOL", 0 },
   { "SHARE", 0 },
   { "APPR", 0 },
   { "GIVE", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE TempleDonateMenuData =
{
  "",
  -1, 0,
  TempleDonateMenuItems,
  TempleDonateMenu
};

MENU_ENTRY TempleHealMenu[TempleHealMenuItems] = {
   { "CAST", 0 },
   { "VIEW", 0 },
   { "FIX", 0 },
   { "TAKE", 0 },
   { "POOL", 0 },
   { "SHARE", 0 },
   { "APPR", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE TempleHealMenuData =
{
  "",
  -1, 0,
  TempleHealMenuItems,
  TempleHealMenu
};

MENU_ENTRY TempleCastMenu[TempleCastMenuItems] = {
   { "CAST", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE TempleCastMenuData =
{
  "",
  -1, 0,
  TempleCastMenuItems,
  TempleCastMenu
};

char *TempleGiveMenuTitle = { "HOW MUCH WILL YOU GIVE?: " };

MENU_ENTRY TavernMenu[TavernMenuItems] = {
   { "FIGHT", 0 },
   { "DRINK", 0 },
   { "LISTEN", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE TavernMenuData =
{
  "",
  -1, 0,
  TavernMenuItems,
  TavernMenu
};

MENU_ENTRY BuyMenu[BuyMenuItems] = {
   { "BUY", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE BuyMenuData =
{
  "",
  -1, 0,
  BuyMenuItems,
  BuyMenu
};

MENU_ENTRY RestMenu[RestMenuItems] = {
   { "REST", 0 },
   { "DAYS", 0 },
   { "HOURS", 0 },
   { "MINS", 0 },
   { "ADD", 0 },
   { "SUB", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE RestMenuData =
{
  "",
  -1, 0,
  RestMenuItems,
  RestMenu
};

MENU_ENTRY MagicMenu[MagicMenuItems] = {
   { "CAST", 0 },
   { "MEMORIZE", 0 },
   { "SCRIBE", 0 },
   { "DISPLAY", 0 },
   { "REST", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE MagicMenuData =
{
  "",
  -1, 0,
  MagicMenuItems,
  MagicMenu
};

MENU_ENTRY CastMenu[CastMenuItems] = {
   { "CAST", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE CastMenuData =
{
  "",
  -1, 0,
  CastMenuItems,
  CastMenu
};

MENU_ENTRY SelectFromListMenu[SelectFromListMenuItems] = {
   { "SELECT", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
};

MENU_DATA_TYPE SelectFromListMenuData =
{
  "",
  -1, 0,
  SelectFromListMenuItems,
  SelectFromListMenu
};

MENU_ENTRY CastTargetMenu[CastTargetMenuItems] = {
   { "SELECT", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE CastTargetMenuData =
{
  "",
  -1, 0,
  CastTargetMenuItems,
  CastTargetMenu
};

MENU_ENTRY DisplaySpellEffectsMenu[DisplaySpellEffectsMenuItems] = {
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE DisplaySpellEffectsMenuData =
{
  "",
  -1, 0,
  DisplaySpellEffectsMenuItems,
  DisplaySpellEffectsMenu
};

MENU_ENTRY DisplayJournalMenu[DisplayJournalMenuItems] = {
   { "NEXT", 0 },
   { "PREV", 0 },
   { "FIRST", 0 },
   { "LAST", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE DisplayJournalMenuData =
{
  "",
  -1, 0,
  DisplayJournalMenuItems,
  DisplayJournalMenu
};

MENU_ENTRY MemorizeMenu[MemorizeMenuItems] = {
   { "SELECT", 0 },
   { "UNSELECT", 0 },
   { "FORGET", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE MemorizeMenuData =
{
  "",
  -1, 0,
  MemorizeMenuItems,
  MemorizeMenu
};

MENU_ENTRY InitialSpellsMenu[InitialSpellsMenuItems] = {
   { "SELECT", 0 },
   { "NEXT", 0 },
   { "PREV", 0 }
};

MENU_DATA_TYPE InitialSpellsMenuData =
{
  "",
  -1, 0,
  InitialSpellsMenuItems,
  InitialSpellsMenu
};

MENU_ENTRY LearnSpellsMenu[LearnSpellsMenuItems] = {
   { "LEARN", 0 },
   { "NEXT", 0 },
   { "PREV", 0 }
};

MENU_DATA_TYPE LearnSpellsMenuData =
{
  "",
  -1, 0,
  LearnSpellsMenuItems,
  LearnSpellsMenu
};

MENU_ENTRY LevelUpLearnSpellsMenu[LevelUpLearnSpellsMenuItems] = {
   { "LEARN", 0 },
   { "NEXT", 0 },
   { "PREV", 0 }
};

MENU_DATA_TYPE LevelUpLearnSpellsMenuData =
{
  "",
  -1, 0,
  LevelUpLearnSpellsMenuItems,
  LevelUpLearnSpellsMenu
};



MENU_ENTRY MemorizeVerifyMenu[MemorizeVerifyMenuItems] = {
   { "KEEP", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "RESELECT", 0 }
};

MENU_DATA_TYPE MemorizeVerifyMenuData =
{
  "SPELLS OK?",
  -1, 0,
  MemorizeVerifyMenuItems,
  MemorizeVerifyMenu
};

MENU_ENTRY ScribeMenu[ScribeMenuItems] = {
   { "SCRIBE", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE ScribeMenuData =
{
  "",
  -1, 0,
  ScribeMenuItems,
  ScribeMenu
};

// this one is filled in with event button text
MENU_ENTRY EncounterMenu[EncounterMenuItems] = {
   { "COMBAT", -1 },
   { "WAIT", -1 },
   { "FLEE", -1 },
   { "ADVANCE", -1 }
};

MENU_DATA_TYPE EncounterMenuData =
{
  "",
  -1, 0,
  EncounterMenuItems,
  EncounterMenu
};

/*
MENU_ENTRY LevelMenu[LevelMenuItems] = {
   { "NOVICE", 0 },
   { "SQUIRE", 0 },
   { "VETERAN", 0 },
   { "ADEPT", 0 },
   { "CHAMPION", 0 }
};

MENU_DATA_TYPE LevelMenuData =
{
  "",
  -1, 0,
  LevelMenuItems,
  LevelMenu
};
*/

MENU_ENTRY SoundMenu[SoundMenuItems] = {
   { "0", 0 },
   { "1", 0 },
   { "2", 0 },
   { "3", 0 },
   { "4", 0 },
   { "5", 0 },
   { "6", 0 },
   { "7", 0 },
   { "8", 0 },
   { "9", 0 },
   { "10", -1 }
};

MENU_DATA_TYPE SoundMenuData =
{
  "CHOOSE VOLUME LEVEL:",
  -1, 0,
  SoundMenuItems,
  SoundMenu
};

MENU_ENTRY AlterMenu[AlterMenuItems] = {
   { "ORDER", 0 },
   { "DROP", 0 },
   { "SPEED", 0 },
   { "ICON", 0 },
   { "PIC", 0 },
   { "LEVEL", 0 },
   { "VOLUME", 0 },
   { "MUSIC", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE AlterMenuData =
{
  "",
  -1, 0,
  AlterMenuItems,
  AlterMenu
};

MENU_ENTRY ItemsMenu[ItemsMenuItems] = {
   { "READY", 0 },
   { "USE", 0 },
   { "TRADE", 0 },
   { "DROP", 0 },
   { "DEPOSIT", 3 },
   { "HALVE", 0 },
   { "JOIN", 0 },
   { "SELL", 0 },
   { "ID ITEM", 0 },
   { "EXAMINE", 0 }, // normal items
   { "EXAMINE", 0 }, // special items and keys
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE ItemsMenuData =
{
  "",
  -1, 0,
  ItemsMenuItems,
  ItemsMenu
};

MENU_ENTRY AdventureMenu[AdventureMenuItems] = {
   { "AREA"     , 0 },
   { "CAST"     , 0 },
   { "VIEW"     , 0 },
   { "ENCAMP"   , 0 },
   { "SEARCH"   , 0 },
   { "LOOK"     , 0 },
   { "INVENTORY", 0 }
};

MENU_DATA_TYPE AdventureMenuData =
{
  "",
  -1, 0,
  AdventureMenuItems,
  AdventureMenu
};

MENU_ENTRY EncampMenu[EncampMenuItems] = {
   { "SAVE",    0 },
   { "LOAD",    0 },
   { "VIEW",    0 },
   { "MAGIC",   0 },
   { "REST",    0 },
   { "ALTER",   0 },
   { "FIX",     0 },
   { "TALK",    0 },
   { "JOURNAL", 0 },
   { "ZAP",     0 },
   { "EXIT",    1 },
   { "QUIT",    0 }
};

MENU_DATA_TYPE EncampMenuData =
{
  "",
  -1, 0,
  EncampMenuItems,
  EncampMenu
};

MENU_ENTRY SaveGameMenu[SaveGameMenuItems] = {
   { "A", 0 },
   { "B", 0 },
   { "C", 0 },
   { "D", 0 },
   { "E", 0 },
   { "F", 0 },
   { "G", 0 },
   { "H", 0 },
   { "I", 0 },
   { "J", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE SaveMenuData =
{
  "",
  -1, 0,
  SaveGameMenuItems,
  SaveGameMenu
};

MENU_DATA_TYPE LoadMenuData =
{
  "",
  -1, 0,
  SaveGameMenuItems,
  SaveGameMenu
};

MENU_ENTRY CombatMenu[CombatMenuItems] = {
   { "MOVE",    0 },
   { "AIM",     0 },
   { "USE",     0 },
   { "CAST",    0 },
   { "TURN",    0 },
   { "GUARD",   0 },
   { "QUICK",   0 },
   { "DELAY",   0 },
   { "BANDAGE", 0 },
   { "VIEW",    0 },
   { "SPEED",   1 },
   { "WIN",     0 },
   { "READY",   0 },
   { "END",     0 },
   { "SWEEP",   0 },
};

MENU_DATA_TYPE CombatMenuData =
{
  "",
  -1, 0,
  CombatMenuItems,
  CombatMenu
};

MENU_ENTRY SpeedMenu[SpeedMenuItems] = {
   { "1", 0 },
   { "2", 0 },
   { "3", 0 },
   { "4", 0 },
   { "5", 0 },
   { "6", 0 },
   { "7", 0 },
   { "8", 0 },
   { "9", 0 }
};

MENU_DATA_TYPE SpeedMenuData =
{
  "",
  -1, 0,
  SpeedMenuItems,
  SpeedMenu
};

MENU_ENTRY AimMenu[AimMenuItems] = {
   { "NEXT", 0 },
   { "PREV", 0 },
   { "MANUAL", 0 },
   { "TARGET", 0 },
   { "CENTER", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE AimMenuData =
{
  "",
  -1, 0,
  AimMenuItems,
  AimMenu
};

MENU_ENTRY AimManualMenu[AimManualMenuItems] = {
   { "TARGET", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE AimManualMenuData =
{
  "",
  -1, 0,
  AimManualMenuItems,
  AimManualMenu
};

MENU_DATA_TYPE GetCharSmallPicMenuData =
{ 
  "",
  -1, 0,
  IconPicMenuItems,
  IconPicMenu
};

MENU_DATA_TYPE AddNPCData =
{
  "",
  -1, 0,
  PressEnterMenuItems,
  PressEnterMenu
};

MENU_DATA_TYPE RemoveNPCData =
{
  "",
  -1, 0,
  PressEnterMenuItems,
  PressEnterMenu
};

MENU_DATA_TYPE NPCSaysData =
{
  "",
  -1, 0,
  PressEnterMenuItems,
  PressEnterMenu
};

MENU_DATA_TYPE PartyDiedMenuData =
{
  "",
  -1, 0,
  PressEnterMenuItems,
  PressEnterMenu
};

MENU_DATA_TYPE CombatResultsMenuData =
{
  "",
  -1, 0,
  PressEnterMenuItems,
  PressEnterMenu
};

MENU_ENTRY AppraiseSelectMenu[AppraiseSelectMenuItems] =
{
  { "GEMS", -1 },
  { "JEWELRY", -1 },
  { "EXIT", 1 }
};

MENU_DATA_TYPE AppraiseSelectMenuData =
{
  "APPRAISE:",
  -1, 0,
  AppraiseSelectMenuItems,
  AppraiseSelectMenu
};

MENU_ENTRY AppraiseEvaluateMenu[AppraiseEvaluateMenuItems] =
{
  { "SELL", 0 },
  { "KEEP", 0 }
};

MENU_DATA_TYPE AppraiseEvaluateMenuData =
{
  "",
  -1, 0,
  AppraiseEvaluateMenuItems,
  AppraiseEvaluateMenu
};


MENU_ENTRY SelectMenu[SelectMenuItems] = {
   { "SELECT", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
};

MENU_DATA_TYPE SelectMenuData =
{
  "",
  -1, 0,
  SelectMenuItems,
  SelectMenu
};



CMyMenu::CMyMenu()
{
  reset(NULL);
}

CMyMenu::~CMyMenu()
{
}

void CMyMenu::reset(const SELECTION_PAGE_FORMAT *form)
{
  format = form;
  if ((format != NULL) && (format->Menu_x == 0))
  {
    format = NULL;
  };
  useTitle=FALSE;
  useActive=TRUE;
  initCharSize=FALSE;
  AddedPressEnter=FALSE;
  TitleWasEnabled=FALSE;
  ActiveWasEnabled=FALSE;
  orient=1;
  title[0] = '\0';
  startX=0;
  startY=0;
  titleX = -1;
  titleY = -1;
  numItems=0;
  MapKeyCode=0;
  MapKeyCodeItem=-1;
  activeItem=0;
// prs 20191220  textColor=White;
  textColorNum = whiteColor;
// prs 20191221  highlightText=Black;
  highlightTextNum = blackColor;
  // prs 20191220    defTextColor=White;
  defTextColorNum = whiteColor;
  itemSeparation=Item_Separation;

  memset(menu, '\0', Max_Menu_Items*Max_Item_Len);

  for (int i=0;i<Max_Menu_Items;i++)
  {
     displayItem[i]=FALSE;
     shortcutIndex[i]=-1;
     menuRects[i].left = menuRects[i].right = 0;
     menuRects[i].top = menuRects[i].bottom = 0;
  }
}

int CMyMenu::addMenuItem(LPCSTR ptext, char shortcut)
{
   if (numItems < (Max_Menu_Items))
   {
      numItems++;
      strncpy(menu[numItems-1], ptext, Max_Item_Len);
      menu[numItems-1][Max_Item_Len-1] = '\0';
      displayItem[numItems-1] = TRUE;
      shortcutIndex[numItems-1] = shortcut;
      return (numItems);
   }
   return -1;
}

void CMyMenu::deleteLastMenuItem(void)
{
   if (numItems > 0)
   {
      memset(menu[numItems-1], '\0', strlen(menu[numItems-1]));
      displayItem[numItems-1] = FALSE;
      shortcutIndex[numItems-1] = -1;
      numItems--;
      if (!displayItem[activeItem])
      {
        activeItem = getFirstDisplayableItem();
        if (activeItem == -1) activeItem = 0;
      }
   }
}

void CMyMenu::changeMenuText(A_ASLENTRY_L *pASL, const GameEvent *pEvent )
{
  SUBSTRINGS ss;
  CString head, tail;
  CString oldText, newText, count;
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  ss = pASL->RunScripts(EVENT_MENU_TEXT, 
                        ScriptCallback_RunAllScripts, 
                        "menu",
                        ScriptSourceType_Event,
                        pEvent->GetEventTypeName());
  while (ss.HeadAndTail(&head, &tail))
  {
    SUBSTRINGS mod;
    int n;
    ss = tail;
    mod = head;
    if (!mod.HeadAndTail(&oldText, &newText)) continue;
    mod = newText;
    if (!mod.HeadAndTail(&newText, &tail)) continue;
    mod = tail;
    n = 0;
    if (mod.HeadAndTail(&count, &tail))
    {
      n = atoi(count);
    };
    changeMenuItem(oldText, newText, n);
  }
}

void CMyMenu::changeMenuItem(LPCSTR item, LPCSTR newText, char shortcut)
{
  int i;
  for (i = 0; i < numItems; i++)
  {
    if (strcmp(item, menu[i]) == 0)
    {
      changeMenuItem(i+1, newText, shortcut);
      return;
    };
  };
  if (strcmp(item, title)== 0 )
  {
    changeMenuTitle(newText);
  };
}

void CMyMenu::changeMenuItem(int item, LPCSTR newText, char shortcut)
{
   if ((item > 0) && (item <= numItems))
   {
      strncpy(menu[item-1], newText, Max_Item_Len);
      menu[item-1][Max_Item_Len-1] = '\0';
      shortcutIndex[item-1] = shortcut;
   };
}

void CMyMenu::changeMenuTitle(LPCSTR newText)
{
  strncpy(title, newText, Max_Item_Len);
}


void CMyMenu::setMenuItems(const int qty, MENU_ENTRY *items)
{
   numItems = min(qty, Max_Menu_Items);
   activeItem=0;
   useTitle=FALSE;
   AddedPressEnter = FALSE;
   TitleWasEnabled=FALSE;
   ActiveWasEnabled=FALSE;
// prs 20191220      textColor=defTextColor;
   textColorNum = defTextColorNum;
   setHorzOrient();
   initCharSize=FALSE;
   itemSeparation=Item_Separation;
   useActive = TRUE;

   memset(menu, '\0', Max_Menu_Items*Max_Item_Len);

   int i;
   for (i=0; i<numItems;i++)
   {
     strncpy(menu[i], items[i].label, Max_Item_Len);
     menu[i][Max_Item_Len-1] = '\0';
     displayItem[i]=TRUE;
     shortcutIndex[i] = items[i].index;
   }

   // disable remaining menu items
   for ( ;i<Max_Menu_Items;i++)
   {
      displayItem[i]=FALSE;
      shortcutIndex[i]=-1;
   }
}

void CMyMenu::nextItem(void)
{
   if (numItems == 0)
      return;

  if ((activeItem < 0) || (activeItem >= numItems))
  {
    activeItem = getFirstDisplayableItem();
    if (activeItem == -1) activeItem = 0;
  }

  int oldActive = activeItem;

   activeItem = ((activeItem+1) % numItems);

   int count=0;

   while ((count < numItems) && (!displayItem[activeItem]))
   {
     activeItem = ((activeItem+1) % numItems);    
     count++;
   }

  if (count == numItems)
  {
    //WriteDebugString("Wrapped full loop in menu::nextItem()\n");
    activeItem = oldActive;
  }
}

void CMyMenu::prevItem(void)
{
   if (numItems == 0)
      return;

  if ((activeItem < 0) || (activeItem >= numItems))
  {
    activeItem = getFirstDisplayableItem();
    if (activeItem == -1) activeItem = 0;
  }

  int firstActive = 0;
  int count = 0;

  // find first active item in list
  while ((firstActive < numItems) && (!displayItem[firstActive]))
  {
    firstActive++;
  }

  if (firstActive == numItems)
  {
    //WriteDebugString("No active items in menu::prevItem()\n");
    return;
  }

  // if current == first active item
  // then wrap current to last item
  // else curr--

   if (activeItem == firstActive)
      activeItem = numItems-1;
   else
      activeItem = ((activeItem-1)%numItems);

   count=0;

  // if curr now points to inactive item, back up to next active item
  while ((count < numItems) && (!displayItem[activeItem]))
  {
      activeItem--;
    if (activeItem < 0)
      activeItem = numItems-1;
    count++;
  }

  if (count == numItems)
  {
    //WriteDebugString("Backed up full loop in menu::prevItem()\n");
    activeItem = firstActive;
  }
}

void CMyMenu::setCurrentItem(const int item) 
{ 
  if (item == -314159) 
  {
    activeItem = -314159;
    return;
  };
  if ((item>0) && (item<=Max_Menu_Items)) 
    activeItem = item-1;
  if (!displayItem[activeItem])
  {
    activeItem = getFirstDisplayableItem();
    if (activeItem == -1) activeItem = 0;
  }
}

void CMyMenu::setAllItemsActive(void)
{
   for (int i=0;i<Max_Menu_Items;i++)
   {
      if (i<numItems)
         displayItem[i]=TRUE;
      else
         displayItem[i]=FALSE;
   }
}

void CMyMenu::setAllItemsInactive(void)
{
   for (int i=0;i<Max_Menu_Items;i++)
   {
      setItemInactive(i);
   }
   activeItem=0;
}


BOOL CMyMenu::isItemActive(const int item) 
{
  if ((item>0) && (item<=Max_Menu_Items)) 
    return displayItem[item-1];
  return FALSE;
}



void CMyMenu::setItemActive(const int item) 
{
  if ((item>0) && (item<=Max_Menu_Items)) 
    displayItem[item-1]=TRUE;

  // if activeItem is not displayable, find first menu
  // that is.
  if (!displayItem[activeItem])
  {
    activeItem = 0;
    if (numItems == 0)
      return;

    // find first active item in list
    int firstActive = 0;
    while ((firstActive < numItems) && (!displayItem[firstActive]))
      firstActive++;

    if (firstActive == numItems)
      return;
    activeItem = firstActive;
  }
}

void CMyMenu::setItemInactive(const int item)
{
  if ((item > 0) && (item <= numItems))
    displayItem[item-1] = FALSE;

  if ((item-1) == activeItem)
  {
    nextItem();

    if (activeItem >= (numItems-1))
       prevItem();
  }
}

void CMyMenu::DisplayMenu(int fontNumber)
{
  if (numItems == 0)
      return;
  if (activeItem != -314159)
  {
    if ((activeItem < 0) || (activeItem >= numItems))
    {
      activeItem = getFirstDisplayableItem();
      if (activeItem == -1) activeItem = 0;
    }
  };

   int count = 0;
   int i=0;
   while ((i < numItems) && (count == 0))
   {
     if (displayItem[i])
       count++;
     i++;
   }

   if (count == 0)
     return;


   SIZE sz;
   int txtHeight = GraphicsMgr.GetMaxCharacterHeight();

   if (!initCharSize)
   {
      if ((orient == 0) && (itemSeparation > 0))
        itemSeparation += (GraphicsMgr.GetCharacterWidth(' ')+2);
      initCharSize=TRUE;
   }

   int x=startX;
   int y=startY;  

   if (useTitle)
   {
     if (titleX == -1)
     {
// prs 20191221       displayColoredText(x, y, title);
       DisplayColoredText(x, y, fontNumber, title, 
                          whiteColor, FALSE,
                          FALSE, FALSE, TRUE);
       sz.cx = GraphicsMgr.GetTextWidth(title, strlen(title));
       x += (10 + itemSeparation + sz.cx);
     }
     else
     {
// prs 20191221       displayColoredText(titleX, titleY, title);
       DisplayColoredText(titleX, titleY, fontNumber, title, 
                          whiteColor, FALSE,
                          FALSE, FALSE, TRUE);
     };
   }
   if (useHeader)
   {
// prs 20191221     displayColoredText(headerX, headerY, header);
     DisplayColoredText(headerX, headerY, fontNumber, header, 
                        whiteColor, FALSE,
                        FALSE, FALSE, TRUE);
   }


   for (i=0; i < numItems; i++)
   {
      if (displayItem[i])
      {
         WriteText(x,y, i, fontNumber); 

         // save screen coords for displayed menu option
         menuRects[i].left = x;
         menuRects[i].top = y;         

         sz.cx = GraphicsMgr.GetTextWidth(menu[i], strlen(menu[i]), ((useActive)&&(i==activeItem)) );

         menuRects[i].right = x + sz.cx;
         menuRects[i].bottom = y + txtHeight;

         if (orient == 0)
            x += (itemSeparation + sz.cx);
         else
            y += (itemSeparation + txtHeight);
      }
      else
      {
        menuRects[i].left = menuRects[i].right = 0;
        menuRects[i].top = menuRects[i].bottom = 0;
      }
   }
}

void CMyMenu::setTitle(LPCSTR string) 
{
   if ((string) && (strlen(string)))
   {
      memset(title, '\0', Max_Item_Len);
      strncpy(title,string,Max_Item_Len);
      title[Max_Item_Len-1] = '\0';
      useTitle=TRUE;
   }
   else
      useTitle=FALSE;
}; 

void CMyMenu::setTitleXY(int x, int y)
{
  titleX = x;
  titleY = y;
}

void CMyMenu::setHeader(LPCSTR string)
{
  if ((string) && (strlen(string)))
  {
    memset(header, '\0', Max_Item_Len);
    strncpy(header, string, Max_Item_Len);
    header[Max_Item_Len - 1] = '\0';
    useHeader = TRUE;
  }
  else
    useHeader = FALSE;
};

void CMyMenu::setHeaderXY(int x, int y)
{
  headerX = x;
  headerY = y;
}


void CMyMenu::enableTitle(BOOL enable)
{
   useTitle = enable;

   if (strlen(title) == 0)
      useTitle = FALSE;
}

void CMyMenu::ReverseVideo(void)
{
// prs 20191221  highlightText = White; // white
  highlightTextNum = whiteColor; // white
// prs 20191220     defTextColor=Black; // black
  defTextColorNum = blackColor;
}

void CMyMenu::NormalVideo(void)
{
// prs 20191221   highlightText=Black; // black
   highlightTextNum = blackColor; // black
// prs 20191220     defTextColor=White; // white
   defTextColorNum = whiteColor;
}

void CMyMenu::WriteText(int x, int y, int item, int fontNumber)
{
  if ((item < 0) || (item >= numItems))
  {
    WriteDebugString("Bogus item num in WriteText()\n");
    return;
  }

   char *ptext = menu[item];
   
   if ((ptext == NULL) || (strlen(ptext) == 0))
      return;

   if ( (!useActive) || (item != activeItem) )
     DrawFont(x,y, fontNumber, ptext, FALSE, shortcutIndex[item]);
   else 
     DrawFont(x,y, fontNumber, ptext, TRUE, shortcutIndex[item]);
}

void CMyMenu::WriteText(int x, int y, LPCSTR ptext, int fontNumber)
{
   DrawFont(x,y, fontNumber, ptext, FALSE, -1);
}

void CMyMenu::DrawFont(int x, int y, int baseFontNumber, LPCSTR ptext, BOOL highlight, int shortcutIdx)
{
   long int SrcFont;
   FONT_COLOR_NUM colorNum = whiteColor;
   int font;

   GraphicsMgr.HighlightText(FALSE);

   if (highlight)
   {
     SrcFont = globalData.HighlightFont;
   }
   else
   {
     switch (textColorNum)
     {
     case combatYellowColor: font = 1; colorNum = yellowColor;   break;
     case combatRedColor:    font = 1; colorNum = redColor;      break;
     case combatBlueColor:   font = 1; colorNum = blueColor;     break;
     case combatGreenColor:  font = 1; colorNum = greenColor;    break;
     case whiteColor:
     case greenColor:
     case blueColor:
     case redColor:
     case orangeColor:
     case yellowColor:
     case blackColor:
     case magentaColor:
     case cyanColor:
     case silverColor:       font = 0; colorNum = textColorNum;  break;
     default:                          colorNum = illegalColor;  break;
     };
     if (colorNum == illegalColor)
     {
       WriteDebugString("Bogus handle to requested font in CMyMenu::DrawFont()\n");
       return;
     }
 // prs 20191227    SrcFont = globalData.availableFonts[0].fontNum[colorNum];
     SrcFont = globalData.fontLib.GetFont(baseFontNumber, colorNum, FALSE);
   };


   if (!UseKeyboardShortcuts)
     shortcutIdx = -1;

   BOOL CurrFontTagSetting = GraphicsMgr.GetEnableFontColorTags();
   GraphicsMgr.EnableFontColorTags(FALSE);
   GraphicsMgr.DrawFont(x,y,
                        baseFontNumber,
                        colorNum,
                        ptext, 
                        SrcFont, 
                        FALSE/*slow?*/, 
                        TRUE/*backbuffer?*/, 
                        TRUE/*transparent?*/, 
                        shortcutIdx);
   GraphicsMgr.EnableFontColorTags(CurrFontTagSetting);
}

void CMyMenu::setStartCoord(const int x, const int y, BOOL combat) 
{   
  // x=-1 means use DEFAULT_MENU_HORZ
  // x=-2 means use DEFAULT_MENU_VERT
  // x=-3 means use DEFAULT_MENU_TEXTBOX
  // x>=0 means use specified absolute coords
  if (x == USE_DEFAULT_HORZ_MENU)
  {
    if (combat)
    {
      startX = DEFAULT_MENU_COMBAT_HORZ_X;
      startY = DEFAULT_MENU_COMBAT_HORZ_Y;
    }
    else
    {
      startX = DEFAULT_MENU_HORZ_X;
      startY = DEFAULT_MENU_HORZ_Y;
    };
  }
  else if (x == USE_DEFAULT_VERT_MENU)
  {
    startX=DEFAULT_MENU_VERT_X; 
    startY=DEFAULT_MENU_VERT_Y; 
  }
  else if (x == USE_DEFAULT_TEXTBOX_MENU)
  {
    startX=DEFAULT_MENU_TEXTBOX_X; 
    startY=DEFAULT_MENU_TEXTBOX_Y; 
  }
  else
  {
    startX=x; 
    startY=y; 
  }
}

void CMyMenu::setMenu(MENU_DATA_TYPE& menuEnt, 
                      const SELECTION_PAGE_FORMAT *form, 
                      BOOL combat,
                      GameEvent *pGameEvent,
                      LPCSTR eventType)
{
  format = form;
  if ((format != NULL) && (format->Menu_x == 0))
  {
    format = NULL;
  }
  useHeader = FALSE;
  // prs 20191220  textColor=White;
  textColorNum = whiteColor;
// prs 20191221  highlightText=Black;
  highlightTextNum = blackColor;
  // prs 20191220  defTextColor=White;
  defTextColorNum = whiteColor;
  MapKeyCode=0;
  MapKeyCodeItem=-1;
  if ((format == NULL) || IsHorzOrient())
  {
    if (combat && (DEFAULT_MENU_COMBAT_HORZ_X >= 0))
    {
      setStartCoord(DEFAULT_MENU_COMBAT_HORZ_X, DEFAULT_MENU_COMBAT_HORZ_Y, combat);
    }
    else
    {
      setStartCoord(menuEnt.x, menuEnt.y, combat);
    };
  }
  else
  {
    setStartCoord(format->Menu_x, format->Menu_y, combat);
  };
  setMenuItems(menuEnt.NumItems, menuEnt.items);
  setTitle(menuEnt.title);
  setAllItemsActive();
  if (pGameEvent != NULL)
  {
    changeMenuText(&pGameEvent->GetEventControl().eventcontrol_asl, pGameEvent);
  }
}


// figure out if x,y is within one of the displayed menu RECTs
int CMyMenu::IntersectPointWithMenu(long x, long y)
{
  POINT pt = { x, y };

  for (int i=0; i < numItems; i++)
  {
    if (displayItem[i])
    {
      if (PtInRect(&menuRects[i], pt))
        return i;
    }
  }

  return -1;
}

void CMyMenu::MapKeyCodeToMenuItem(int code, int item)
{
  MapKeyCode=0;
  MapKeyCodeItem=-1;
  if (code <= 0) return;
  if (item <= 0) return;
  if (item > numItems) return;
  MapKeyCode=code;
  MapKeyCodeItem=item;
}

int CMyMenu::ConvertKeyCodeToMenuItem(int code)
{
  if (MapKeyCode<=0) return 0;
  if (code==MapKeyCode) return MapKeyCodeItem;
  return 0;
}

int CMyMenu::LookupShortcut(const char key)
{
  if (!UseKeyboardShortcuts)
    return -1;

  for (int i=0; i < numItems; i++)
  {
    if ((displayItem[i]) && (shortcutIndex[i] >= 0))
    {
      if (strnicmp(&menu[i][shortcutIndex[i]], &key, 1) == 0)
        return i;
    }
  }

  return -1;  
}

BOOL CMyMenu::FirstLettersUnique()
{
  for (int i=0; i < numItems; i++)
  {
    if (displayItem[i])
    {
      for (int y=i+1;y<numItems;y++)
      {
        if (menu[i][0] == menu[y][0])
          return FALSE;
      }
    }
  }
  return TRUE;
}

void CMyMenu::AttemptToCreateUniqueShortcut(int menuIndex)
{
  /* If the menu item has no current shortcut
   * If the menu item is active
   *   Examine all the active menu items to determine the possible shortcut letters.
   *   Examine the menu item to see if it can use andy of the available letters.
   *   Assign the shorcut  if possible
   */
  if (shortcutIndex[menuIndex-1] >= 0) return;  //Shortcut already exists
  if (!isItemActive(menuIndex)) return;      // No shortcut is needed
  {
    int i;
    char availableLetters[26];      // value of 0 == available
    memset(availableLetters,0,26);  // all are available
    for (i=0; i<numItems; i++)
    {
      char letter;
      if (!isItemActive(i+1)) continue;      // This item uses no letters
      if (shortcutIndex[i] < 0) continue; // THis item uses no letters
      letter = menu[i][shortcutIndex[i]];
      if (letter >= 'a') letter -= 'a'-'A';
      if (letter > 'Z') continue;
      if (letter < 'A') continue;
      availableLetters[letter-'A'] = 1;   // Mark letter not available;
    };
    // Now we work our way through the new menu item searching for a 
    // letter that has not already been used.
    for (i=0; ;i++)
    {
      char letter;
      letter = menu[menuIndex-1][i];
      if (letter == 0) break;
      if (letter >= 'a') letter -= 'a'-'A';
      if (letter > 'Z') continue;
      if (letter < 'A') continue;
      if (availableLetters[letter-'A'] != 0) continue;   // Mark not available;
      shortcutIndex[menuIndex-1] = i;
      break;
    }
  };
}


void CMyMenu::SetFirstLettersShortcuts(BOOL OnlyIfMenuItemHasNoCurrentShortcut)
{
  if (numItems <= 0) return;

  int i;

  if (numItems == 1)
  {
    shortcutIndex[0]=0;
    return;
  }

  if (!FirstLettersUnique())
    return;

  for (i=0;i<numItems;i++)
  {
    if (OnlyIfMenuItemHasNoCurrentShortcut)
    {
      // only set first letter as shortcut if 
      // this menu item currently has no shortcut
      if (shortcutIndex[i] < 0)
        shortcutIndex[i] = 0;
    }
    else
    {
      // set all menu items to use first letter
      shortcutIndex[i]=0;
    }
  }
}

void CMyMenu::AddOverrideMenu(const char *ptext, const int shortcut, BOOL active)
{
  if (AddedPressEnter) return;
  WriteDebugString("AddOverRideMenu(%s,%i)\n", ptext,shortcut);
  AddedPressEnter = TRUE;
  TitleWasEnabled = useTitle;
  ActiveWasEnabled = useActive;
  PrevOrient = getOrient();
  PrevStartX = startX;
  PrevStartY = startY;
  setStartCoord(DEFAULT_MENU_HORZ_X, DEFAULT_MENU_HORZ_Y, FALSE);
  setHorzOrient();
  setUseActive(active);
  enableTitle(FALSE);
  addMenuItem(ptext, shortcut);
  setAllItemsInactive();
  setItemActive(numItems);
  setCurrentItem(numItems);
}

void CMyMenu::RemoveOverrideMenu()
{
  if (!AddedPressEnter) return;
  WriteDebugString("RemoveOverRideMenu()\n");
  AddedPressEnter = FALSE;
  setOrient(PrevOrient);
  setStartCoord(PrevStartX, PrevStartY, FALSE);
  enableTitle(TitleWasEnabled);
  setUseActive(ActiveWasEnabled);
  deleteLastMenuItem();
  setAllItemsActive();
  setCurrentItem(1);
}
