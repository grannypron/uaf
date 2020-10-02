/******************************************************************************
* Filename: Screen.h
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
#ifndef DGNSCRN_H_
#define DGNSCRN_H_

class GPDL;

void blitView(DWORD flags, BOOL drawViewportPic, PIC_DATA *pPic);
BOOL Splash();
BOOL Credits();

void UpdateSmall(void);
void UpdateBig(void);

struct SELECTION_PAGE_FORMAT;
void DisplayTextAndFlip(int fontNumber);
void UpdateStartScreen(const SELECTION_PAGE_FORMAT *format, int fontNumber);
void UpdateIconPickScreen(void);
void UpdateSmallPicScreen(void);
void UpdateAdventureScreen(void);
void UpdateMoveScreen(void);
void UpdateTextEventScreen(GameEvent *pEvent, char specialGraphicCircumstance);
void UpdateQuestionYesNoScreen(char specialGraphicsCircumstance);
void UpdateVaultScreen(char specialGraphicsCircumstance);
void UpdateGiveTreasureScreen(char specialGraphicsCircumstance);
void UpdateViewCharacterScreen(void);
void UpdateCombatViewCharacterScreen(void);
void UpdateEncampScreen(char specialGraphicsCircumstance);
void UpdateTransferMenuScreen(char specialGraphicsCircumstance);
void UpdateTavernMenuScreen(char specialGraphicsCircumstance);
void UpdateGiveDamageScreen(char specialGraphicsCircumstance);
void UpdateGainExpScreen(char specialGraphicsCircumstance);
void UpdateHealPartyScreen(char specialGraphicsCircumstance);
void UpdateWhoPaysScreen(char specialGraphicsCircumstance);
void UpdatePassTimeScreen(char specialGraphicsCircumstance);
void UpdateSmallTownScreen(char specialGraphicsCircumstance);
void UpdateAddNPCScreen(char specialGraphicsCircumstance);
void UpdateRemoveNPCScreen(char specialGraphicsCircumstance);
void UpdateNPCSaysScreen(char specialGraphicsCircumstance);
void UpdateShopScreen(char specialGraphicsCircumstance);
void UpdateSpecialItemKeyScreen(char specialGraphicsCircumstance);
void UpdateGPDLScreen(GPDL *pGPDL, PIC_DATA *pPic);
void UpdateTakePartyItemsScreen(char specialGraphicsCircumstance);
void UpdateTrainHallWelcomeScreen(char specialGraphicsCircumstance);
void UpdateWhoTriesScreen(char specialGraphicsCircumstance);
void UpdateTempleScreen(char specialGraphicsCircumstance);
void UpdateTempleGiveScreen(void);
void UpdatePasswordScreen(char specialGraphicsCircumstance);
void UpdateQuestionListScreen(char specialGraphicsCircumstance);
void UpdateQuestionButtonScreen(char specialGraphicsCircumstance);
void UpdateEncounterScreen(char specialGraphicsCircumstance);
void UpdateQuestScreen(char specialGraphicsCircumstance);
void UpdateGuidedTourScreen(char specialGraphicCircumstance);
void UpdateGuidedTourMoveScreen(char specialGraphicCircumstance);
void UpdateSpeedCombatScreen(void);
void UpdateSpeedScreen(char specialGraphicsCircumstance);
void UpdateCombatRoundDelayScreen(void);
void UpdateCombatTimeDelayMsg(int dude=-1);
void UpdateSpriteAnimateScreen(int dude);
void UpdateCombatMoveScreen(void);
void UpdateCombatAimManualScreen(void);
void UpdateCombatAimScreen(void);
void UpdateCombatMainMenuScreen(void);
void UpdateHumanChangeClassScreen(void);
void UpdateTradeTakerSelectScreen(char specialGraphicsCircumstance);
void UpdateCombatTradeTakerSelectScreen(void);
void UpdateAlterGameScreen(char specialGraphicsCircumstance);
void UpdateAlterOrderScreen(char specialGraphicsCircumstance);
void UpdatePartyDiedScreen(void);
void UpdateCombatResultsScreen(void);
void UpdateMagicMenuScreen(char specialGraphicsCircumstance);
void UpdateScribeMenuScreen(char specialGraphicsCircumstance);
void UpdateCombatMagicMenuScreen(void);
void UpdateRestMenuScreen(void);
void UpdateAppraiseSelectMenuScreen(void);
void UpdateAppraiseEvaluateMenuScreen(void);
void UpdateAskYesNoMenuScreen(void);
void UpdateTargetSelectNonCombatScreen(char specialGraphicsCircumstance);
void UpdateRandomEventScreen(char specialGraphicsCircumstance);
void UpdateDisplayPartyJournalScreen(void);
void UpdateMovieBigPicWindow(void);
void UpdateMovieSmallPicWindow(void);
struct SELECTION_PAGE_FORMAT;
void UpdateRaceMenuScreen(const SELECTION_PAGE_FORMAT& format);
void UpdateGenderMenuScreen(const SELECTION_PAGE_FORMAT& format);
void UpdateClassMenuScreen(const SELECTION_PAGE_FORMAT& format);
void UpdateAlignmentMenuScreen(const SELECTION_PAGE_FORMAT& format);
#endif