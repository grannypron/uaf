/******************************************************************************
* Filename: GameMenu.h
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
#ifndef __GAMEMENU_H__
#define __GAMEMENU_H__

extern int DEFAULT_MENU_HORZ_X;
extern int DEFAULT_MENU_HORZ_Y;
extern int DEFAULT_MENU_COMBAT_HORZ_X;
extern int DEFAULT_MENU_COMBAT_HORZ_Y;
extern int DEFAULT_MENU_VERT_X;
extern int DEFAULT_MENU_VERT_Y;
extern int DEFAULT_MENU_TEXTBOX_X;
extern int DEFAULT_MENU_TEXTBOX_Y;

struct MENU_ENTRY
{
  char *label; // text displayed
  char index;  // which char corresponds to keyboard input. 
               // -1 means none, 0..len-1 means indicated char will be
               // displayed in different color that indicates it can be
               // selected to via keyboard
};

struct MENU_DATA_TYPE
{
	char *title;
	int x, y;      // x=-1 = use DEFAULT_MENU_HORZ
                 // x=-2 = use DEFAULT_MENU_VERT
                 // x=-3 = use DEFAULT_MENU_TEXTBOX
                 // x>=0 = use specified absolute coords
	int NumItems;
	MENU_ENTRY *items;
};

#define USE_DEFAULT_HORZ_MENU -1
#define USE_DEFAULT_VERT_MENU -2
#define USE_DEFAULT_TEXTBOX_MENU -3


//
// menus that use absolute coords
//
enum { AddCharMenuX=18, AddCharMenuY=18 };

enum { RaceMenuX=43, RaceMenuY=18/*80*/ };
#define RaceMenuItems 7
extern MENU_ENTRY RaceMenu[RaceMenuItems];
extern char *RaceMenuTitle;

enum { GenderMenuX=RaceMenuX, GenderMenuY=RaceMenuY };
#define GenderMenuItems 3
extern MENU_ENTRY GenderMenu[GenderMenuItems];
extern char *GenderMenuTitle;

enum { ClassMenuX=RaceMenuX, ClassMenuY=RaceMenuY };
#define ClassMenuItems 20
extern MENU_ENTRY ClassMenu[ClassMenuItems];
extern char *ClassMenuTitle;

enum { AlignmentMenuX=RaceMenuX, AlignmentMenuY=RaceMenuY };
#define AlignmentMenuItems 10
extern MENU_ENTRY AlignmentMenu[AlignmentMenuItems];
extern char *AlignmentMenuTitle;

enum { ChangeClassMenuX=RaceMenuX, ChangeClassMenuY=RaceMenuY };
const int ChangeClassMenuItems=3;
extern MENU_ENTRY ChangeClassMenu[ChangeClassMenuItems];
extern char *ChangeClassMenuTitle;

//enum { WhoPaysMenuX=RaceMenuX, ChangeClassMenuY=RaceMenuY };
const int WhoPaysMenuItems=2;
extern MENU_ENTRY WhoPaysMenu[WhoPaysMenuItems];

const int TargetSelectNonCombatMenuItems=2;
extern MENU_ENTRY TargetSelectNonCombatMenu[TargetSelectNonCombatMenuItems];

//
// vertical menus using defaults
//

//enum { StartMenuX=DEFAULT_VERT_X, StartMenuY=DEFAULT_VERT_Y };
#define StartMenuItems 4
extern MENU_ENTRY StartMenu[StartMenuItems];

//enum { MainMenuX=DEFAULT_VERT_X, MainMenuY=DEFAULT_VERT_Y };
#define MainMenuItems 12
extern MENU_ENTRY MainMenu[MainMenuItems];

//
// horizontal menus using defaults
//

//enum { QuestMenuX=DEFAULT_HORZ_X, QuestMenuY=DEFAULT_HORZ_Y };
const int QuestMenuItems = 2;
extern MENU_ENTRY QuestMenu[QuestMenuItems];
extern char *QuestMenuTitle;

//enum { SmallTownMenuX=DEFAULT_HORZ_X, SmallTownMenuY=DEFAULT_HORZ_Y };
const int SmallTownMenuItems=7;
extern MENU_ENTRY SmallTownMenu[SmallTownMenuItems];

//enum { QYesNoMenuX=DEFAULT_HORZ_X, QYesNoMenuY=DEFAULT_HORZ_Y };
const int QYesNoMenuItems=2;
extern MENU_ENTRY QYesNoMenu[QYesNoMenuItems];
extern char *QYesNoMenuTitle;

//enum { PressEnterMenuX=DEFAULT_HORZ_X, PressEnterMenuY=DEFAULT_HORZ_Y };
const int PressEnterMenuItems=1;
extern MENU_ENTRY PressEnterMenu[PressEnterMenuItems];
extern MENU_ENTRY PressEnterMenu2[PressEnterMenuItems];

//enum { TransferMenuX=DEFAULT_HORZ_X, TransferMenuY=DEFAULT_HORZ_Y };
const int TransferMenuItems=2;
extern MENU_ENTRY TransferMenu[TransferMenuItems];

//enum { IconPicMenuX=DEFAULT_HORZ_X, IconPicMenuY=DEFAULT_HORZ_Y };
const int IconPicMenuItems=3;
extern MENU_ENTRY IconPicMenu[IconPicMenuItems];

//enum { TextStatementMenuX=DEFAULT_HORZ_X, TextStatementMenuY=DEFAULT_HORZ_Y };
const int TextStatementMenuItems = 2;
extern MENU_ENTRY TextStatementMenu[TextStatementMenuItems];

//enum { GPDLEventMenuX=DEFAULT_HORZ_X, GPDLEventMenuY=DEFAULT_HORZ_Y };
const int GPDLEventMenuItems = 5;
extern MENU_ENTRY GPDLEventMenu[GPDLEventMenuItems];

//enum { TradeTakerMenuX=DEFAULT_HORZ_X, TradeTakerMenuY=DEFAULT_HORZ_Y };
#define TradeTakerMenuItems 2
extern MENU_ENTRY TradeTakerMenu[TradeTakerMenuItems];
extern char *TradeTakerMenuTitle;

//enum { ScriptChoiceMenuX=DEFAULT_HORZ_X, ScriptChoiceMenuY=DEFAULT_HORZ_Y };
#define ScriptChoiceMenuItems 2
extern MENU_ENTRY ScriptChoiceMenu[ScriptChoiceMenuItems];
extern char *ScriptChoiceMenuTitle;

//enum { GetMoneyQtyX=DEFAULT_HORZ_X, GetMoneyQtyY=DEFAULT_HORZ_Y };
#define GetMoneyQtyItems 0
//extern MENU_ENTRY GetMoneyQtyMenu;
extern char *GetMoneyQtyTitle;

//enum { EmptyMenuX=DEFAULT_HORZ_X, EmptyMenuY=DEFAULT_HORZ_Y };
#define EmptyMenuItems 1
extern MENU_ENTRY EmptyMenu[EmptyMenuItems];

//enum { GetCharNameX=DEFAULT_HORZ_X, GetCharNameY=DEFAULT_HORZ_Y };
extern char *GetCharNameTitle;

//enum { GetPasswordX=DEFAULT_HORZ_X, GetPasswordY=DEFAULT_HORZ_Y };
extern char *GetPasswordTitle;

//enum { TrainWelcomeMenuX=DEFAULT_HORZ_X, TrainWelcomeMenuY=DEFAULT_HORZ_Y };
extern char *TrainWelcomMenuTitle;
#define TrainWelcomeMenuItems 2
extern MENU_ENTRY TrainWelcomeMenu[TrainWelcomeMenuItems];

//enum { ShopMenuX=DEFAULT_HORZ_X, ShopMenuY=DEFAULT_HORZ_Y };
#define ShopMenuItems 8
extern MENU_ENTRY ShopMenu[ShopMenuItems];

//enum { BuyMenuX=DEFAULT_HORZ_X, BuyMenuY=DEFAULT_HORZ_Y };
#define BuyMenuItems 4
extern MENU_ENTRY BuyMenu[BuyMenuItems];

//enum { TempleMenuX=DEFAULT_HORZ_X, TempleMenuY=DEFAULT_HORZ_Y };
#define TempleMenuItems 7
extern MENU_ENTRY TempleMenu[TempleMenuItems];

//enum { TempleDonateMenuX=DEFAULT_HORZ_X, TempleDonateMenuY=DEFAULT_HORZ_Y };
#define TempleDonateMenuItems 6
extern MENU_ENTRY TempleDonateMenu[TempleDonateMenuItems];

//enum { TempleHealMenuX=DEFAULT_HORZ_X, TempleHealMenuY=DEFAULT_HORZ_Y };
#define TempleHealMenuItems 8
extern MENU_ENTRY TempleHealMenu[TempleHealMenuItems];

//enum { TempleCastMenuX=DEFAULT_HORZ_X, TempleCastMenuY=DEFAULT_HORZ_Y };
#define TempleCastMenuItems 4
extern MENU_ENTRY TempleCastMenu[TempleCastMenuItems];

#define SelectMenuItems 4
extern MENU_ENTRY SelectMenu[SelectMenuItems];

//enum { TempleGiveMenuX=DEFAULT_HORZ_X, TempleGiveMenuY=DEFAULT_HORZ_Y };
extern char *TempleGiveMenuTitle;

//enum { TavernMenuX=DEFAULT_HORZ_X, TavernMenuY=DEFAULT_HORZ_Y };
#define TavernMenuItems 4
extern MENU_ENTRY TavernMenu[TavernMenuItems];

//enum { ChooseStatsX=DEFAULT_HORZ_X, ChooseStatsY=DEFAULT_HORZ_Y };
#define ChooseStatsMenuItems 2
extern MENU_ENTRY ChooseStatsMenu[ChooseStatsMenuItems];
extern char *ChooseStatsMenuTitle;

//enum { ViewCharMenuX=DEFAULT_HORZ_X, ViewCharMenuY=DEFAULT_HORZ_Y };
#define ViewCharMenuItems 8
extern MENU_ENTRY ViewCharMenu[ViewCharMenuItems];

//enum { MoveMenuX=DEFAULT_HORZ_X, MoveMenuY=DEFAULT_HORZ_Y };
#define MoveMenuItems 1
extern MENU_ENTRY MoveMenu[MoveMenuItems];

//enum { ExitMenuX=DEFAULT_HORZ_X, ExitMenuY=DEFAULT_HORZ_Y };
#define ExitMenuItems 1
extern MENU_ENTRY ExitMenu[ExitMenuItems];

#define DisplaySpellEffectsMenuItems 3
extern MENU_ENTRY DisplaySpellEffectsMenu[DisplaySpellEffectsMenuItems];

#define DisplayJournalMenuItems 5
extern MENU_ENTRY DisplayJournalMenu[DisplayJournalMenuItems];

#define RestMenuItems 7
extern MENU_ENTRY RestMenu[RestMenuItems];

//enum { MagicMenuX=DEFAULT_HORZ_X, MagicMenuY=DEFAULT_HORZ_Y };
#define MagicMenuItems 6
extern MENU_ENTRY MagicMenu[MagicMenuItems];

#define CastMenuItems 4
extern MENU_ENTRY CastMenu[CastMenuItems];

#define SelectFromListMenuItems 3
extern MENU_ENTRY SelectFromListMenu[SelectFromListMenuItems];

#define CastTargetMenuItems 2
extern MENU_ENTRY CastTargetMenu[CastTargetMenuItems];

//enum { MemorizeMenuX=DEFAULT_HORZ_X, MemorizeMenuY=DEFAULT_HORZ_Y };
#define MemorizeMenuItems 6
extern MENU_ENTRY MemorizeMenu[MemorizeMenuItems];

#define InitialSpellsMenuItems 3
extern MENU_ENTRY InitialSpellsMenu[InitialSpellsMenuItems];

#define LearnSpellsMenuItems 3
extern MENU_ENTRY LearnSpellsMenu[LearnSpellsMenuItems];

#define LevelUpLearnSpellsMenuItems 3
extern MENU_ENTRY LevelUpLearnSpellsMenu[LevelUpLearnSpellsMenuItems];

#define MemorizeVerifyMenuItems 4
extern MENU_ENTRY MemorizeVerifyMenu[MemorizeVerifyMenuItems];

//enum { ScribeMenuX=DEFAULT_HORZ_X, ScribeMenuY=DEFAULT_HORZ_Y };
#define ScribeMenuItems 4
extern MENU_ENTRY ScribeMenu[ScribeMenuItems];

//enum { EncounterMenuX=DEFAULT_HORZ_X, EncounterMenuY=DEFAULT_HORZ_Y };
#define EncounterMenuItems 4
extern MENU_ENTRY EncounterMenu[EncounterMenuItems];

//enum { LevelMenuX=DEFAULT_HORZ_X, LevelMenuY=DEFAULT_HORZ_Y };
#define LevelMenuItems 5
extern MENU_ENTRY LevelMenu[LevelMenuItems];

#define SoundMenuItems 11
extern MENU_ENTRY SoundMenu[SoundMenuItems];

//enum { AlterMenuX=DEFAULT_HORZ_X, AlterMenuY=DEFAULT_HORZ_Y };
#define AlterMenuItems 9
extern MENU_ENTRY AlterMenu[AlterMenuItems];

//enum { ItemsMenuX=DEFAULT_HORZ_X, ItemsMenuY=DEFAULT_HORZ_Y };
#define ItemsMenuItems 14
extern MENU_ENTRY ItemsMenu[ItemsMenuItems];

//enum { AdventureMenuX=DEFAULT_HORZ_X, AdventureMenuY=DEFAULT_HORZ_Y };
#define AdventureMenuItems 7
extern MENU_ENTRY AdventureMenu[AdventureMenuItems];

//enum { EncampMenuX=DEFAULT_HORZ_X, EncampMenuY=DEFAULT_HORZ_Y };
#define EncampMenuItems 12
extern MENU_ENTRY EncampMenu[EncampMenuItems];

//enum { SaveGameMenuX=DEFAULT_HORZ_X, SaveGameMenuY=DEFAULT_HORZ_Y };
#define SaveGameMenuItems 11
#define MAX_SAVE_GAME_SLOTS (SaveGameMenuItems-1)
extern MENU_ENTRY SaveGameMenu[SaveGameMenuItems];

//enum { CombatMenuX=DEFAULT_HORZ_X, CombatMenuY=DEFAULT_HORZ_Y };
#define CombatMenuItems 15
extern MENU_ENTRY CombatMenu[CombatMenuItems];

//enum { CombatItemsMenuX=DEFAULT_HORZ_X, CombatItemsMenuY=DEFAULT_HORZ_Y };
//#define CombatItemsMenuItems 6
//extern MENU_ENTRY CombatItemsMenu[CombatItemsMenuItems];

//enum { SpeedMenuX=DEFAULT_HORZ_X, SpeedMenuY=DEFAULT_HORZ_Y };
#define SpeedMenuItems 9
extern MENU_ENTRY SpeedMenu[SpeedMenuItems];

//enum { AimMenuX=DEFAULT_HORZ_X, AimMenuY=DEFAULT_HORZ_Y };
#define AimMenuItems 6
extern MENU_ENTRY AimMenu[AimMenuItems];

#define AimManualMenuItems 2
extern MENU_ENTRY AimManualMenu[AimManualMenuItems];

//enum { TreasureMenuX=DEFAULT_HORZ_X, TreasureMenuY=DEFAULT_HORZ_Y };
#define TreasureMenuItems 6
extern MENU_ENTRY TreasureMenu[TreasureMenuItems];

//enum { TreasureWelcomeMenuX=DEFAULT_HORZ_X, TreasureWelcomeMenuY=DEFAULT_HORZ_Y };
#define TreasureWelcomeMenuItems 6
extern MENU_ENTRY TreasureWelcomeMenu[TreasureWelcomeMenuItems];

//enum { VaultMenuX=DEFAULT_HORZ_X, VaultMenuY=DEFAULT_HORZ_Y };
#define VaultMenuItems 6
extern MENU_ENTRY VaultMenu[VaultMenuItems];

#define AppraiseSelectMenuItems 3
extern MENU_ENTRY AppraiseSelectMenu[AppraiseSelectMenuItems];

#define AppraiseEvaluateMenuItems 2
extern MENU_ENTRY AppraiseEvaluateMenu[AppraiseEvaluateMenuItems];

struct SELECTION_PAGE_FORMAT;

class CMyMenu 
{

private:

	enum { Item_Separation=5,
         Max_Menu_Items=20,
			   Max_Item_Len=85 };
	
  const SELECTION_PAGE_FORMAT *format;
	BOOL useTitle;
  BOOL useHeader;
	BOOL useActive;
  BOOL AddedPressEnter;
  BOOL TitleWasEnabled;
  BOOL ActiveWasEnabled;
  int  PrevOrient;
  int  PrevStartX;
  int  PrevStartY;
	char title[Max_Item_Len];
  char header[Max_Item_Len];
	int orient;

	int startX;
	int startY;

  int titleX;
  int titleY;

  int headerX;
  int headerY;

	int numItems;
	int activeItem;
	int itemSeparation;

//	COLORREF textColor;          //prs 20191220 
  FONT_COLOR_NUM textColorNum;   //prs 20191220 
  int            textFontNum;    //prs 20191220 
//	COLORREF defTextColor;       //prs 20191220 
  FONT_COLOR_NUM defTextColorNum;//prs 20191220 
  int            defTextFontNum; //prs 20191220 

// prs 20191221	COLORREF highlightText;
  FONT_COLOR_NUM highlightTextNum;

	BOOL displayItem[Max_Menu_Items];
  char shortcutIndex[Max_Menu_Items]; // -1, 0..Max_Item_Len
	char menu[Max_Menu_Items][Max_Item_Len];
  RECT menuRects[Max_Menu_Items];
	BOOL initCharSize;
  int MapKeyCode;
  int MapKeyCodeItem;

  void setOrient(int dir) { orient=dir; }
  int  getOrient() { return orient; }
	void WriteText(int x, int y, LPCSTR text, int fontNumber);
	void WriteText(int x, int y, int item, int fontNumber);
  void DrawFont(int x, int y, int baseFontNumber, LPCSTR text, BOOL highlight, int shortcutIdx);

  int getFirstDisplayableItem() 
  { 
    for (int i=0;i<Max_Menu_Items;i++) 
      if (displayItem[i]) return i; 
    return -1; 
  }

public:

	CMyMenu();
	~CMyMenu();

	void setMenuItems(const int qty,  MENU_ENTRY *items);
	void reset(const SELECTION_PAGE_FORMAT *form);
	int addMenuItem(LPCSTR text, char shortcut=-1);
	void changeMenuItem(int item, LPCSTR newText, char shortcut=-1);
  void changeMenuItem(LPCSTR item, LPCSTR newText, char shortcut = -1);
  void changeMenuText(A_ASLENTRY_L *pASL, const GameEvent *pEvent);
  LPCSTR getMenuItem(int item) {if ((item>0)&&(item<=Max_Menu_Items)) return &menu[item-1][0]; else return NULL; }
  void changeMenuTitle(LPCSTR newText);
	void deleteLastMenuItem(void);
  
	//void setDefaultTextColor(COLORREF color) { defTextColor=color; }
	
  void setHorzOrient( void ) { orient=0; }
	void setVertOrient( void ) { orient=1; }  
  BOOL IsHorzOrient() { return (orient == 0); }
  BOOL IsVertOrient() { return (orient == 1); }

	void setUseActive(BOOL useIt) { useActive = useIt; }
	void setStartCoord(const int x, const int y, BOOL combat);
	void setItemSeparation(int sep) { itemSeparation=sep; }

	void setTitle(LPCSTR string); 
  void setHeader(LPCSTR string);
  LPCSTR getTitle(void) const { return title; }
  void enableTitle(BOOL enable);
  void setTitleXY(int x, int y);
  void setHeaderXY(int x, int y);

  void AddOverrideMenu(const char *text, const int shortcut, BOOL active);
  void RemoveOverrideMenu();

	void ReverseVideo(void);
	void NormalVideo(void);
	
	void setItemActive(const int item);
  BOOL isItemActive(const int item);
	void setCurrentItem(const int item);
	void setAllItemsActive(void);
  void setAllItemsInactive(void);
	void setItemInactive(const int item);
	int  currentItem(void) { return activeItem+1; }
	int  totalItems(void) { return numItems; }

	void nextItem(void);
	void prevItem(void);

	void DisplayMenu(int fontNumber);

  void setMenu(MENU_DATA_TYPE& menuEnt, 
              const SELECTION_PAGE_FORMAT *form,
              BOOL combat,
              GameEvent *pGameEvent,
              LPCSTR eventType);
  int IntersectPointWithMenu(long x, long y);
  int LookupShortcut(const char key);
  BOOL FirstLettersUnique();
  void SetFirstLettersShortcuts(BOOL OnlyIfMenuItemHasNoCurrentShortcut=FALSE);
  void AttemptToCreateUniqueShortcut(int menuIndex);
  void MapKeyCodeToMenuItem(int code, int item);
  int ConvertKeyCodeToMenuItem(int code);
};

extern class CMyMenu menu;

extern MENU_DATA_TYPE StartMenuData;
extern MENU_DATA_TYPE MainMenuData;
extern MENU_DATA_TYPE RaceMenuData;
extern MENU_DATA_TYPE GenderMenuData;
extern MENU_DATA_TYPE ClassMenuData;
extern MENU_DATA_TYPE AlignmentMenuData;
extern MENU_DATA_TYPE ChooseStatsMenuData;
extern MENU_DATA_TYPE GetCharIconMenuData;
extern MENU_DATA_TYPE GetCharSmallPicMenuData;
extern MENU_DATA_TYPE AddCharacterMenuData;
extern MENU_DATA_TYPE AdventureMenuData;
extern MENU_DATA_TYPE MoveMenuData;
extern MENU_DATA_TYPE SaveMenuData;
extern MENU_DATA_TYPE LoadMenuData;
extern MENU_DATA_TYPE TextEventData;
extern MENU_DATA_TYPE QuestionYesNoData;
extern MENU_DATA_TYPE QuestionYesData;
extern MENU_DATA_TYPE QuestionNoData;

extern MENU_DATA_TYPE QuestMenuData;
extern MENU_DATA_TYPE ChangeClassMenuData;
extern MENU_DATA_TYPE SmallTownMenuData;
extern MENU_DATA_TYPE QYesNoMenuData;
extern MENU_DATA_TYPE PressEnterMenuData;
extern MENU_DATA_TYPE TransferMenuData;
extern MENU_DATA_TYPE TradeTakerMenuData;
extern MENU_DATA_TYPE EmptyMenuData;
extern MENU_DATA_TYPE GetPasswordMenuData;
extern MENU_DATA_TYPE WhoPaysMenuData;
extern MENU_DATA_TYPE TargetSelectNonCombatMenuData;
//extern MENU_DATA_TYPE WhoTriesMenuData;
extern MENU_DATA_TYPE TrainMenuData;
extern MENU_DATA_TYPE TrainWelcomeMenuData;
extern MENU_DATA_TYPE ShopMenuData;
extern MENU_DATA_TYPE BuyMenuData;
extern MENU_DATA_TYPE TempleMenuData;
extern MENU_DATA_TYPE TempleDonateMenuData;
extern MENU_DATA_TYPE TempleHealMenuData;
extern MENU_DATA_TYPE TempleCastMenuData;
extern MENU_DATA_TYPE SelectMenuData;
//extern MENU_DATA_TYPE TempleGiveMenu;
extern MENU_DATA_TYPE TavernMenuData;
extern MENU_DATA_TYPE ViewCharMenuData;
extern MENU_DATA_TYPE ShopViewCharMenuData;
extern MENU_DATA_TYPE ExitMenuData;
extern MENU_DATA_TYPE MagicMenuData;
extern MENU_DATA_TYPE MemorizeMenuData;
extern MENU_DATA_TYPE InitialSpellsMenuData;
extern MENU_DATA_TYPE LearnSpellsMenuData;
extern MENU_DATA_TYPE LevelUpLearnSpellsMenuData;
extern MENU_DATA_TYPE ScribeMenuData;
extern MENU_DATA_TYPE EncounterMenuData;
extern MENU_DATA_TYPE LevelMenuData;
extern MENU_DATA_TYPE SoundMenuData;
extern MENU_DATA_TYPE AlterMenuData;
extern MENU_DATA_TYPE ItemsMenuData;
extern MENU_DATA_TYPE EncampMenuData;
extern MENU_DATA_TYPE CombatMenuData;
extern MENU_DATA_TYPE CombatViewCharMenuData;
//extern MENU_DATA_TYPE CombatItemsMenuData;
extern MENU_DATA_TYPE SpeedMenuData;
extern MENU_DATA_TYPE AimMenuData;
extern MENU_DATA_TYPE AimManualMenuData;
extern MENU_DATA_TYPE TakeTreasureMenuData;
extern MENU_DATA_TYPE ScriptChoiceMenuData;
extern MENU_DATA_TYPE GiveTreasureMenuData;
extern MENU_DATA_TYPE VaultMenuData;
extern MENU_DATA_TYPE GetMoneyQtyMenuData;
extern MENU_DATA_TYPE AddNPCData;
extern MENU_DATA_TYPE RemoveNPCData;
extern MENU_DATA_TYPE NPCSaysData;
extern MENU_DATA_TYPE GPDLEventMenuData;
extern MENU_DATA_TYPE PartyDiedMenuData;
extern MENU_DATA_TYPE CombatResultsMenuData;
extern MENU_DATA_TYPE DisplaySpellEffectsMenuData;
extern MENU_DATA_TYPE RestMenuData;
extern MENU_DATA_TYPE CastMenuData;
extern MENU_DATA_TYPE SelectFromListMenuData;
extern MENU_DATA_TYPE CastTargetMenuData;
extern MENU_DATA_TYPE MemorizeVerifyMenuData;
extern MENU_DATA_TYPE AppraiseSelectMenuData;
extern MENU_DATA_TYPE AppraiseEvaluateMenuData;
extern MENU_DATA_TYPE DisplayJournalMenuData;

#endif
