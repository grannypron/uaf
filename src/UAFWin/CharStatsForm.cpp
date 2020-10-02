/******************************************************************************
* Filename: CharStatsForm.cpp
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

#include "externs.h"
#include "Dungeon.h"
#include "Graphics.h"
#include "GlobalData.h"
#include "GetInput.h"
#include "Viewport.h"
#include "CharStatsForm.h"
#include "GPDLcomp.h"
#include "GPDLexec.h"

void My_free(void *addr);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int LX = 18;
static const int RX = 220;
static int strmod=0;

extern const CString Ability_Dexterity;
extern const CString Ability_Constitution;
extern const CString Ability_Strength;
extern const CString Ability_Intelligence;
extern const CString Ability_Charisma;
extern const CString Ability_Wisdom;



char *CharStatusTypeText[] =
{
  "OKAY", 
  "UNCONSCIOUS", 
  "DEAD", 
  "FLED", 
  "PETRIFIED", 
  "GONE", 
  "ANIMATED", 
  "TEMP GONE",
  "RUNNING", 
  "DYING"  
};

char *CharGenderTypeText[] =
{
  "MALE",
  "FEMALE"
};

char *CharAlignmentTypeText[] =
{
  "LAWFUL GOOD",
  "NEUTRAL GOOD", 
  "CHAOTIC GOOD",
  "LAWFUL NEUTRAL", 
  "TRUE NEUTRAL", 
  "CHAOTIC NEUTRAL",
  "LAWFUL EVIL",    
  "NEUTRAL EVIL", 
  "CHAOTIC EVIL"
};

enum ST_FORM { // Define field names as integers for quicker comparisons
  STF_none,
  STF_white=TEXT_FORM::white,    // White items
  STF_STATUS,
  STF_gender,
  STF_name,
  STF_age,
  STF_HITS,
  STF_maxHits,
  STF_align,
  STF_race,
  STF_class,
  STF_EXP,
  STF_EXP2,
  STF_EXP3,
  STF_exp,
  STF_level,
  STF_COIN1,
  STF_COIN2,
  STF_COIN3,
  STF_COIN4,
  STF_COIN5,
  STF_COIN6,
  STF_COIN7,
  STF_COIN8,
  STF_COIN9,
  STF_COIN10,
  STF_AC,
  STF_THAC0,
  STF_DAMAGE,
  STF_armor,
  STF_weapon,
  STF_ENCUM,
  STF_MOVE,
  STF_green=TEXT_FORM::green,   // Change to Green
  STF_AVAIL,
  STF_avail,
  STF_status,
  STF_hits,
  STF_coin1,
  STF_coin2,
  STF_coin3,
  STF_coin4,
  STF_coin5,
  STF_coin6,
  STF_coin7,
  STF_coin8,
  STF_coin9,
  STF_coin10,
  STF_ac,
  STF_thac0,
  STF_damage,
  STF_encum,
  STF_move,
  STF_STR,
  STF_str,
  STF_INT,
  STF_int,
  STF_WIS,
  STF_wis,
  STF_DEX,
  STF_dex,
  STF_CON,
  STF_con,
  STF_CHA,
  STF_cha,
  STF_Str=TEXT_FORM::green+TEXT_FORM::tab, 
  STF_Int, // Selection rectangle includes more than one field
  STF_Wis, //   ''     ''
  STF_Dex, //   ''     ''
  STF_Con, //   ''     ''
  STF_Cha //   ''     ''
};

#if _MSC_VER >= 1310
#define STF(item,xrel,x,yrel,y) \
  DISPLAY_FORM(STF_##xrel,STF_##yrel,STF_##item,x,y),
#else
#define STF(item,xrel,x,yrel,y) \
  { DISPLAY_FORM(STF_##xrel,STF_##yrel,STF_##item,x,y) },
#endif



// when using relative coords, it is imperative
// that you dont display item B relative to item A 
// before actually displaying item A.
static DISPLAY_FORM statsForm[] =
{
  STF(STATUS   ,none       ,RX    ,none   ,  35)  // The word "STATUS"
  STF(status   ,STATUS+END ,16    ,STATUS ,   0)  // "dead"
  STF(name     ,none       ,LX    ,none   ,  35)  // "RICHARD"
  STF(gender   ,name       ,0     ,name   ,  20)  // "MALE"
  STF(age      ,gender     ,100   ,gender ,   0)  // "17 YEARS"
  STF(HITS     ,STATUS     ,0     ,STATUS ,  20)  // "HIT POINTS"
  STF(hits     ,HITS+END   ,16    ,HITS   ,   0)  // # hit points
  STF(maxHits  ,hits+END   , 1    ,HITS   ,   0)  // "/maxhitpoints"
  STF(align    ,name       ,0     ,age    ,  20)  // "EVIL" 
  STF(race     ,HITS       ,0     ,HITS   ,  20)  // "HUMAN" 
  STF(class    ,name       ,0     ,align  ,  20)  // "RANGER"
  STF(EXP      ,name       ,0     ,class  ,  20)  // 1st baseclass level/experience
  STF(EXP2     ,none       ,LX    ,EXP    ,  20)  // 2nd baseclass level/experience
  STF(EXP3     ,none       ,LX    ,EXP2   ,  20)  // 3rd baseclass level/experience
  STF(exp      ,EXP+END    ,16    ,EXP    ,   0)  // "25460"
  STF(level    ,name       ,0     ,EXP    ,  20)  // "LEVEL 1"
  STF(STR      ,none       ,LX    ,none   , 172)  // "STR"
  STF(str      ,STR        ,35    ,STR    ,   0)  // strength value text
  STF(Str      ,STR+SEL    , 0    ,str+SEL,   0)  // STR + str
  STF(INT      ,STR        , 0    ,STR    ,  20)  // "INT"
  STF(int      ,INT        ,35    ,INT    ,   0)  // intelligence
  STF(Int      ,INT+SEL    , 0    ,int+SEL,   0)  // INT + int
  STF(WIS      ,INT        , 0    ,INT    ,  20)  // "WIS"
  STF(wis      ,WIS        ,35    ,WIS    ,   0)  // wisdom
  STF(Wis      ,WIS+SEL    , 0    ,wis+SEL,   0)  // WIS + wis
  STF(DEX      ,WIS        , 0    ,WIS    ,  20)  // "DEX"
  STF(dex      ,DEX        ,35    ,DEX    ,   0)  // dexterity
  STF(Dex      ,DEX+SEL    , 0    ,dex+SEL,   0)  // DEX + dex
  STF(CON      ,DEX        , 0    ,DEX    ,  20)  // "CON"
  STF(con      ,CON        ,35    ,CON    ,   0)  // constitution
  STF(Con      ,CON+SEL    , 0    ,con+SEL,   0)  // CON + con
  STF(CHA      ,CON        , 0    ,CON    ,  20)  // "CHA"
  STF(cha      ,CHA        ,35    ,CHA    ,   0)  // charisma
  STF(Cha      ,CHA+SEL    , 0    ,cha+SEL,   0)  // CHA + cha
  STF(AVAIL    ,CHA        , 0    ,CHA    ,  20)  // "AVAIL"
  STF(avail    ,cha        , 0    ,AVAIL  ,   0)  // available to distribute
  STF(COIN1    ,none       ,RX    ,none   , 135)  // "PLATINUM"
  STF(coin1    ,COIN1+END  , 50   ,COIN1  ,   0)  // platinum
  STF(COIN2    ,COIN1      , 0    ,COIN1  ,  20)  // "COIN2"
  STF(coin2    ,coin1+RIGHT, 0    ,COIN2  ,   0)  // gold
  STF(COIN3    ,COIN2      , 0    ,COIN2  ,  20)  // "ELECTRUM"
  STF(coin3    ,coin1+RIGHT, 0    ,COIN3  ,   0)  // electrum  
  STF(COIN4    ,COIN3      , 0    ,COIN3  ,  20)  // "SILVER"
  STF(coin4    ,coin1+RIGHT, 0    ,COIN4  ,   0)  // silver  
  STF(COIN5    ,COIN4      , 0    ,COIN4  ,  20)  // "COPPER"
  STF(coin5    ,coin1+RIGHT, 0    ,COIN5  ,   0)  // copper
  STF(COIN6    ,COIN5      , 0    ,COIN5  ,  20)  // "COIN6"
  STF(coin6    ,coin1+RIGHT, 0    ,COIN6  ,   0)  // coin6
  STF(COIN7    ,COIN6      , 0    ,COIN6  ,  20)  // "COIN7"
  STF(coin7    ,coin1+RIGHT, 0    ,COIN7  ,   0)  // coin7
  STF(COIN8    ,COIN7      , 0    ,COIN7  ,  20)  // "COIN8"
  STF(coin8    ,coin1+RIGHT, 0    ,COIN8  ,   0)  // coin8
  STF(COIN9    ,COIN8      , 0    ,COIN8  ,  20)  // "COIN9"
  STF(coin9    ,coin1+RIGHT, 0    ,COIN9  ,   0)  // coin9
  STF(COIN10   ,COIN9      , 0    ,COIN9  ,  20)  // "COIN10"
  STF(coin10   ,coin1+RIGHT, 0    ,COIN10 ,   0)  // coin10
  STF(AC       ,none       ,LX    ,none   , 325)  // "ARMOR CLASS"
  STF(ac       ,AC+END     , 0    ,AC     ,   0)  // armor classs
  STF(THAC0    ,AC         , 0    ,AC     ,  20)  // "THAC0"
  STF(thac0    ,THAC0+END  , 0    ,THAC0  ,   0)  // thac0
  STF(DAMAGE   ,THAC0      , 0    ,THAC0  ,  20)  // "DAMAGE"
  STF(damage   ,DAMAGE+END ,20    ,DAMAGE ,   0)  // 1d4
  STF(weapon   ,DAMAGE     , 0    ,DAMAGE ,  20)  // "battle axe"
  STF(armor    ,weapon     , 0    ,weapon ,  20)  // "plate mail"
  STF(ENCUM    ,none       ,RX    ,none   , 325)  // "ENCUMBRANCE"
  STF(encum    ,ENCUM+END  ,20    ,ENCUM  ,   0)  // encumbrance
  STF(MOVE     ,ENCUM      , 0    ,ENCUM  ,  20)  // "MOVEMENT"
  STF(move     ,encum      , 0    ,MOVE   ,   0)  // movement
  STF(none,none,0,none,0)  // End of list
};

void showCharStats(TEXT_FORM *pForm, CHARACTER& dude)
{ 
   ASSERT(pForm!=NULL);

   BOOL IsAdj;
   CString temp;
   CString temp2;
   CString abbreviation;

   // prs 20191221
//   COLORREF color;
//   COLORREF namecolor;
   FONT_COLOR_NUM colorNum;
   FONT_COLOR_NUM namecolorNum;

//   const COLORREF AdjColor = RGB(255,128,0); // Bright Orange, for stats currently modified by active spells   
   const FONT_COLOR_NUM adjColorNum = brightOrangeColor; // Bright Orange, for stats currently modified by active spells   

   // make sure that highlight flags are set after clearing form
   //pForm->ClearForm();

   int charCurrHp = dude.GetAdjHitPoints();

   if (charCurrHp <= 0)
// prs 20191221      color = Red;
      colorNum = redColor;
   else if (charCurrHp < dude.GetMaxHitPoints())
      colorNum = yellowColor;
   else
      colorNum = greenColor;
   
   if (dude.GetAdjReadyToTrain())
     namecolorNum = blueColor;
   else
     namecolorNum = greenColor;

   pForm->SetText(STF_STATUS, "STATUS");   

   // char name
// prs 20191221   pForm->SetText(STF_name  , dude.GetName(), namecolor);
   pForm->SetText(STF_name, dude.GetName(), namecolorNum);

   // char status
   charStatusType charTrueStatus = dude.GetStatus();
   charStatusType charCurrStatus = dude.GetAdjStatus();   
// prs 20191221   COLORREF statusColor = color;
   FONT_COLOR_NUM statusColorNum = colorNum;
   if (charTrueStatus!=charCurrStatus)
// prs 20191221     statusColor = AdjColor;
      statusColorNum = adjColorNum;

   if (charCurrStatus != Okay)
// prs 20191221     pForm->SetText(STF_status, CharStatusTypeText[charCurrStatus], statusColor);
      pForm->SetText(STF_status, CharStatusTypeText[charCurrStatus], statusColorNum);
   else
// prs 20191221     pForm->SetText(STF_status, CharStatusTypeText[charCurrStatus], Green);
      pForm->SetText(STF_status, CharStatusTypeText[charCurrStatus], greenColor);

   // char gender
   genderType charTrueGender = dude.GetGender();
   genderType charCurrGender = dude.GetAdjGender();
   temp = CharGenderTypeText[charCurrGender];
   if (charCurrGender!=charTrueGender)
// prs 20191221     pForm->SetText(STF_gender,temp, AdjColor);
      pForm->SetText(STF_gender, temp, adjColorNum);
   else
     pForm->SetText(STF_gender,temp);   
   
   // char age
   int charTrueAge = dude.GetAge();
   int charCurrAge = dude.GetAdjAge();
   temp.Format("%i YEARS", charCurrAge);
   if (charTrueAge!=charCurrAge)
// prs 20191221     pForm->SetText(STF_age, temp, AdjColor);
      pForm->SetText(STF_age, temp, adjColorNum);
   else
// prs 20191221     pForm->SetText(STF_age, temp);
      pForm->SetText(STF_age, temp);

   // char hitpoints/maxhitpoints
   int charTrueHp = dude.GetHitPoints();
   pForm->SetText(STF_HITS, "HIT POINTS");    
   if (charCurrHp!=charTrueHp)
   {
     temp.Format("%i", charCurrHp);
// prs 20191221     pForm->SetText(STF_hits, temp, AdjColor);   
     pForm->SetText(STF_hits, temp, adjColorNum);
     temp.Format("/%i", dude.GetMaxHitPoints());
// prs 20191221     pForm->SetText( STF_maxHits, temp, AdjColor);
     pForm->SetText(STF_maxHits, temp, adjColorNum);
   }
   else
   {
     temp.Format("%i", charCurrHp);
// prs 20191221     pForm->SetText(STF_hits, temp, color);   
     pForm->SetText(STF_hits, temp, colorNum);
     temp.Format("/%i", dude.GetMaxHitPoints());
// prs 20191221     pForm->SetText( STF_maxHits, temp, color);
     pForm->SetText(STF_maxHits, temp, colorNum);
   }

   // char alignment   
   alignmentType charCurrAlignment = dude.GetAdjAlignment();
   alignmentType charTrueAlignment = dude.GetAlignment();
   if (charCurrAlignment!=charTrueAlignment)
// prs 20191221     pForm->SetText(STF_align, CharAlignmentTypeText[charCurrAlignment], AdjColor);
     pForm->SetText(STF_align, CharAlignmentTypeText[charCurrAlignment], adjColorNum);
   else
// prs 20191221     pForm->SetText(STF_align, CharAlignmentTypeText[charCurrAlignment]);
     pForm->SetText(STF_align, CharAlignmentTypeText[charCurrAlignment]);

   // char race
   //
   // ********  temp fix here  **********
   //
   // the character doesn't have the race key
   // yet, and that is what raceData.GetRaceName() is
   // expecting. It was not mapping raceType to
   // race text and was always returning 'No Race'
   //
   {
     CString currRaceName, raceName;
     HOOK_PARAMETERS hookParameters;
     SCRIPT_CONTEXT scriptContext;
     scriptContext.SetCharacterContext(&dude);
     //currRaceName =  raceData.GetRaceName(dude.race());
     currRaceName =  raceData.GetRaceName(dude.RaceID());
     hookParameters[5] = currRaceName;
     raceName = dude.RunCharacterScripts(
                                 DISPLAY_CHARACTER_RACE,
                                 ScriptCallback_RunAllScripts, 
                                 NULL,
                                 "Show Character stats");
     if (raceName == "") raceName = currRaceName;
     pForm->SetText( STF_race, raceName);
   };
   //pForm->SetText( STF_race, raceData.GetRaceName(dude.race()));
   
   // char class
   //classType charCurrClass = dude.GetAdjClass();
   //classType charTrueClass = dude.GetClass();
   CLASS_ID charCurrClass = dude.GetAdjClass();
   CLASS_ID charTrueClass = dude.GetClass();
   {
     CString className;
     HOOK_PARAMETERS hookParameters;
     SCRIPT_CONTEXT scriptContext;
     scriptContext.SetCharacterContext(&dude);
     //hookParameters[5] = classData[charCurrClass].m_name;
     hookParameters[5] = classData.PeekClass(charCurrClass)->m_name;
     className = dude.RunCharacterScripts(
                                 DISPLAY_CHARACTER_CLASS,
                                 ScriptCallback_RunAllScripts, 
                                 NULL,
                                 "Show Character stats");
     //if (className == "") className = classData[charCurrClass].m_name;
     if (className == "") className = classData.PeekClass(charCurrClass)->m_name;

     if (charCurrClass!=charTrueClass)
// prs 20191221       pForm->SetText( STF_class, className, AdjColor);
       pForm->SetText(STF_class, className, adjColorNum);
     else
// prs 20191221       pForm->SetText( STF_class, className);
     pForm->SetText(STF_class, className, whiteColor);
   };
     
   // char levels/experience
   IsAdj=FALSE;
/*
   int exp1,exp2,exp3;
   switch (charCurrClass) 
   {
   case C_F:
      temp.Format("LEVEL %i/%i", dude.GetCurrentLevel(ClericFlag), dude.GetCurrentLevel(FighterFlag));
      exp1=dude.GetAdjClericExp();exp2=dude.GetAdjFighterExp();
      temp2.Format("%i/%i", exp1, exp2);
      IsAdj = ((exp1!=dude.GetClericExp())||(exp2!=dude.GetFighterExp()));
      break;
   case C_F_MU:
      temp.Format("LEVEL %i/%i/%i", dude.GetCurrentLevel(ClericFlag),dude.GetCurrentLevel(FighterFlag),dude.GetCurrentLevel(MagicUserFlag));
      exp1=dude.GetAdjClericExp();exp2=dude.GetAdjFighterExp();exp3=dude.GetAdjMagicUserExp();
      temp2.Format("%i/%i/%i", exp1, exp2, exp3);
      IsAdj = ((exp1!=dude.GetClericExp())||(exp2!=dude.GetFighterExp())||(exp3!=dude.GetMagicUserExp()));
      break;
   case C_R:
      temp.Format("LEVEL %i/%i", dude.GetCurrentLevel(ClericFlag), dude.GetCurrentLevel(RangerFlag));
      exp1=dude.GetAdjClericExp();exp2=dude.GetAdjRangerExp();
      temp2.Format("%i/%i", exp1, exp2);
      IsAdj = ((exp1!=dude.GetClericExp())||(exp2!=dude.GetRangerExp()));
      break;
   case C_MU:
      temp.Format("LEVEL %i/%i", dude.GetCurrentLevel(ClericFlag), dude.GetCurrentLevel(MagicUserFlag));
      exp1=dude.GetAdjClericExp();exp2=dude.GetAdjMagicUserExp();
      temp2.Format("%i/%i", exp1, exp2);
      IsAdj = ((exp1!=dude.GetClericExp())||(exp2!=dude.GetMagicUserExp()));
      break;
   case C_T:
      temp.Format("LEVEL %i/%i", dude.GetCurrentLevel(ClericFlag), dude.GetCurrentLevel(ThiefFlag));
      exp1=dude.GetAdjClericExp();exp2=dude.GetAdjThiefExp();
      temp2.Format("%i/%i", exp1, exp2);
      IsAdj = ((exp1!=dude.GetClericExp())||(exp2!=dude.GetThiefExp()));
      break;
   case F_MU:
      temp.Format("LEVEL %i/%i", dude.GetCurrentLevel(FighterFlag),dude.GetCurrentLevel(MagicUserFlag));
      exp1=dude.GetAdjFighterExp();exp2=dude.GetAdjMagicUserExp();
      temp2.Format("%i/%i", exp1, exp2);
      IsAdj = ((exp1!=dude.GetFighterExp())||(exp2!=dude.GetMagicUserExp()));
      break;
   case F_T:
      temp.Format("LEVEL %i/%i", dude.GetCurrentLevel(FighterFlag),dude.GetCurrentLevel(ThiefFlag));
      exp1=dude.GetAdjFighterExp();exp2=dude.GetAdjThiefExp();
      temp2.Format("%i/%i", exp1, exp2);
      IsAdj = ((exp1!=dude.GetFighterExp())||(exp2!=dude.GetThiefExp()));
      break;
   case F_MU_T:
      temp.Format("LEVEL %i/%i/%i", dude.GetCurrentLevel(FighterFlag),dude.GetCurrentLevel(MagicUserFlag),dude.GetCurrentLevel(ThiefFlag));
      exp1=dude.GetAdjFighterExp();exp2=dude.GetAdjMagicUserExp();exp3=dude.GetAdjThiefExp();
      temp2.Format("%i/%i/%i", exp1, exp2, exp3);
      IsAdj = ((exp1!=dude.GetFighterExp())||(exp2!=dude.GetMagicUserExp())||(exp3!=dude.GetThiefExp()));
      break;
   case MU_T:
      temp.Format("LEVEL %i/%i", dude.GetCurrentLevel(MagicUserFlag),dude.GetCurrentLevel(ThiefFlag));
      exp1=dude.GetAdjMagicUserExp();exp2=dude.GetAdjThiefExp();
      temp2.Format("%i/%i", exp1, exp2);
      IsAdj = ((exp1!=dude.GetMagicUserExp())||(exp2!=dude.GetThiefExp()));
      break;
   case F_D:
      temp.Format("LEVEL %i/%i", dude.GetCurrentLevel(FighterFlag),dude.GetCurrentLevel(DruidFlag));
      exp1=dude.GetAdjFighterExp();exp2=dude.GetAdjDruidExp();
      temp2.Format("%i/%i", exp1, exp2);
      IsAdj = ((exp1!=dude.GetFighterExp())||(exp2!=dude.GetDruidExp()));
      break;
   case D_MU:
      temp.Format("LEVEL %i/%i", dude.GetCurrentLevel(DruidFlag),dude.GetCurrentLevel(MagicUserFlag));
      exp1=dude.GetAdjDruidExp();exp2=dude.GetAdjMagicUserExp();
      temp2.Format("%i/%i", exp1, exp2);
      IsAdj = ((exp1!=dude.GetAdjDruidExp())||(exp2!=dude.GetAdjMagicUserExp()));
      break;
   case F_MU_D:
      temp.Format("LEVEL %i/%i/%i", dude.GetCurrentLevel(FighterFlag),dude.GetCurrentLevel(MagicUserFlag),dude.GetCurrentLevel(DruidFlag));
      exp1=dude.GetAdjFighterExp();exp2=dude.GetAdjMagicUserExp();exp3=dude.GetAdjDruidExp();
      temp2.Format("%i/%i/%i", exp1, exp2, exp3);
      IsAdj = ((exp1!=dude.GetFighterExp())||(exp2!=dude.GetMagicUserExp())||(exp3!=dude.GetDruidExp()));
      break;
   case Fighter:
      temp.Format("LEVEL %i", dude.GetCurrentLevel(FighterFlag));
      exp1=dude.GetAdjFighterExp();
      temp2.Format("%i", exp1);
      IsAdj = (exp1!=dude.GetFighterExp());
      break;
   case Cleric:
      temp.Format("LEVEL %i", dude.GetCurrentLevel(ClericFlag));
      exp1=dude.GetAdjClericExp();
      temp2.Format("%i", exp1);
      IsAdj = (exp1!=dude.GetClericExp());
      break;
   case Ranger:
      temp.Format("LEVEL %i", dude.GetCurrentLevel(RangerFlag));
      exp1=dude.GetAdjRangerExp();
      temp2.Format("%i", exp1);
      IsAdj = (exp1!=dude.GetRangerExp());
      break;
   case Paladin:
      temp.Format("LEVEL %i", dude.GetCurrentLevel(PaladinFlag));
      exp1=dude.GetAdjPaladinExp();
      temp2.Format("%i", exp1);
      IsAdj = (exp1!=dude.GetPaladinExp());
      break;
   case MagicUser:
      temp.Format("LEVEL %i", dude.GetCurrentLevel(MagicUserFlag));
      exp1=dude.GetAdjMagicUserExp();
      temp2.Format("%i", exp1);
      IsAdj = (exp1!=dude.GetMagicUserExp());
      break;
   case Thief:
      temp.Format("LEVEL %i", dude.GetCurrentLevel(ThiefFlag));
      exp1=dude.GetAdjThiefExp();
      temp2.Format("%i", exp1);
      IsAdj = (exp1!=dude.GetThiefExp());
      break;
   case Druid:
      temp.Format("LEVEL %i", dude.GetCurrentLevel(DruidFlag));
      exp1=dude.GetAdjDruidExp();
      temp2.Format("%i", exp1);
      IsAdj = (exp1!=dude.GetDruidExp());
      break;
   default:
      temp.Format("LEVEL ?");
      temp2.Format("%s", "??");
      WriteDebugString("Unknown character class in displayCharStats()\n");
      break;
   }
*/



  // Testing 
  {
    int i, n;
    const CLASS_DATA *pClass;
    pClass = classData.PeekClass(charTrueClass);
    n = pClass->GetCount();
    for (i=0; i<n; i++)
    {
      CString baseclassText, expAndLevelText;
      int level, experience;
      const BASE_CLASS_DATA *pBaseclass;
      const BASECLASS_ID *pBaseclassID;
      pBaseclassID = pClass->PeekBaseclassID(i);
      if (pBaseclassID == NULL) continue;
      pBaseclass = baseclassData.PeekBaseclass(*pBaseclassID);
      if (pBaseclass == NULL) continue;
      level = dude.GetCurrentLevel(*pBaseclassID);
      experience = dude.GetCurrExp(*pBaseclassID);
      baseclassText = pBaseclassID->UniqueName();
      expAndLevelText.Format("%d/%d", level, experience);
      {
        CString className;
        HOOK_PARAMETERS hookParameters;
        SCRIPT_CONTEXT scriptContext;
        scriptContext.SetCharacterContext(&dude);
        //hookParameters[5] = classData[charCurrClass].m_name;
        hookParameters[5] = baseclassText;
        hookParameters[6] = expAndLevelText;
        if (!pBaseclass->RunBaseClassScripts(
                                   DISPLAY_CHARACTER_BASECLASS,
                                   ScriptCallback_RunAllScripts, 
                                   NULL,
                                   "Show Charcter stats").IsEmpty()) continue;
        //if (className == "") className = classData[charCurrClass].m_name;
        baseclassText = hookParameters[5];
        expAndLevelText = hookParameters[6];
      };
      pForm->SetText( STF_EXP+i,baseclassText + " " + expAndLevelText /*, namecolor*/);
      //pBaseclassID = classData.PeekBaseclassID(i);
    };
  }







   // char experience
   //pForm->SetText( STF_EXP,"EXPERIENCE");
   //if (IsAdj)
   //  pForm->SetText( STF_exp, temp2, AdjColor); // experience
   //else
   //  pForm->SetText( STF_exp, temp2, namecolor); // experience

   // char level
   //pForm->SetText( STF_level, temp); // level

   // char ability stats  

   // strength
   //pForm->SetText( STF_STR,"STR     "); 
   abbreviation = abilityData.GetShortName("Strength");
   abbreviation += "          ";
   abbreviation = abbreviation.Left(8);
   pForm->SetText( STF_STR,abbreviation); 
   int charCurrStMod = dude.GetLimitedStrMod();
   int charTrueStMod = dude.GetPermStrMod();
   IsAdj = (charCurrStMod!=charTrueStMod);

   int charCurrStat = dude.GetLimitedStr();
   int charTrueStat = dude.GetPermStr();
   IsAdj |= (charCurrStat!=charTrueStat);

   if (charCurrStMod>0)
      temp.Format("    %2i (%i)", charCurrStat, charCurrStMod);
   else 
      temp.Format("    %2i", charCurrStat);

   if (IsAdj)
// prs 20191221     pForm->SetText( STF_str, temp, AdjColor);
     pForm->SetText(STF_str, temp, adjColorNum);
   else
     pForm->SetText( STF_str, temp);

   pForm->SetText( STF_Str,"");

   // intelligence
   charCurrStat = dude.GetLimitedInt();
   charTrueStat = dude.GetPermInt();
   IsAdj = (charCurrStat!=charTrueStat);
   //pForm->SetText( STF_INT,"INT     ");
   abbreviation = abilityData.GetShortName("Intelligence");
   abbreviation += "          ";
   abbreviation = abbreviation.Left(8);
   pForm->SetText( STF_INT,abbreviation); 
   temp.Format("    %2i", charCurrStat);
   if (IsAdj)
// prs 20191221     pForm->SetText( STF_int, temp, AdjColor);
     pForm->SetText(STF_int, temp, adjColorNum);
   else
     pForm->SetText( STF_int, temp);
   pForm->SetText( STF_Int,"");
   
   // wisdom
   charCurrStat = dude.GetLimitedWis();
   charTrueStat = dude.GetPermWis();
   IsAdj = (charCurrStat!=charTrueStat);
   //pForm->SetText( STF_WIS,"WIS     ");
   abbreviation = abilityData.GetShortName("Wisdom");
   abbreviation += "          ";
   abbreviation = abbreviation.Left(8);
   pForm->SetText( STF_WIS,abbreviation); 
   temp.Format("    %2i", charCurrStat);
   if (IsAdj)
// prs 20191221     pForm->SetText(STF_int, temp, AdjColor);
     pForm->SetText(STF_int, temp, adjColorNum);
   else
     pForm->SetText( STF_wis, temp);
   pForm->SetText( STF_Wis, "");
   
   // dexterity
   charCurrStat = dude.GetLimitedDex();
   charTrueStat = dude.GetPermDex();
   IsAdj = (charCurrStat!=charTrueStat);
   //pForm->SetText( STF_DEX,"DEX     ");
   abbreviation = abilityData.GetShortName("Dexterity");
   abbreviation += "          ";
   abbreviation = abbreviation.Left(8);
   pForm->SetText( STF_DEX,abbreviation); 
   temp.Format("    %2i", charCurrStat);
   if (IsAdj)
// prs 20191221     pForm->SetText( STF_dex, temp, AdjColor);
     pForm->SetText(STF_dex, temp, adjColorNum);
   else
     pForm->SetText( STF_dex, temp);
   pForm->SetText( STF_Dex, "");

   // constitution
   charCurrStat = dude.GetLimitedCon();
   charTrueStat = dude.GetPermCon();
   IsAdj = (charCurrStat!=charTrueStat);
   //pForm->SetText( STF_CON,"CON     ");
   abbreviation = abilityData.GetShortName("Constitution");
   abbreviation += "          ";
   abbreviation = abbreviation.Left(8);
   pForm->SetText( STF_CON,abbreviation); 
   temp.Format("    %2i", charCurrStat);
   if (IsAdj)
// prs 20191221     pForm->SetText(STF_con, temp, AdjColor);
     pForm->SetText(STF_con, temp, adjColorNum);
   else
     pForm->SetText(STF_con, temp);
   pForm->SetText( STF_Con, "");
   
   // charisma
   charCurrStat = dude.GetLimitedCha();
   charTrueStat = dude.GetPermCha();
   IsAdj = (charCurrStat!=charTrueStat);
   //pForm->SetText( STF_CHA,"CHA    ");
   abbreviation = abilityData.GetShortName("Charisma");
   abbreviation += "          ";
   abbreviation = abbreviation.Left(8);
   pForm->SetText( STF_CHA,abbreviation); 
   temp.Format("    %2i", charCurrStat);
   if (IsAdj)
// prs 20191221     pForm->SetText( STF_cha, temp, AdjColor);
     pForm->SetText(STF_cha, temp, adjColorNum);
   else
     pForm->SetText( STF_cha, temp);
   pForm->SetText( STF_Cha, "");
   
   struct SCharMoneyDataTemp
   {
     SCharMoneyDataTemp() { moneyName=""; moneyAmount=0; }
     CString moneyName;
     unsigned int moneyAmount;
   };

   int currMoneyArrayIdx = 0;
   SCharMoneyDataTemp charMoneyDataTempArray[10];

   if ((dude.money.IsActive(PlatinumType)) && (dude.money[PlatinumType] > 0))
   {
     charMoneyDataTempArray[currMoneyArrayIdx].moneyName = globalData.moneyData.GetName(PlatinumType);
     charMoneyDataTempArray[currMoneyArrayIdx].moneyAmount = dude.money[PlatinumType];
     currMoneyArrayIdx++;
   }
   if ((dude.money.IsActive(GoldType)) && (dude.money[GoldType] > 0))
   {
     charMoneyDataTempArray[currMoneyArrayIdx].moneyName = globalData.moneyData.GetName(GoldType);
     charMoneyDataTempArray[currMoneyArrayIdx].moneyAmount = dude.money[GoldType];
     currMoneyArrayIdx++;
   }
   if ((dude.money.IsActive(ElectrumType)) && (dude.money[ElectrumType] > 0))
   {
     charMoneyDataTempArray[currMoneyArrayIdx].moneyName = globalData.moneyData.GetName(ElectrumType);
     charMoneyDataTempArray[currMoneyArrayIdx].moneyAmount = dude.money[ElectrumType];
     currMoneyArrayIdx++;
   }
   if ((dude.money.IsActive(SilverType)) && (dude.money[SilverType] > 0))
   {
     charMoneyDataTempArray[currMoneyArrayIdx].moneyName = globalData.moneyData.GetName(SilverType);
     charMoneyDataTempArray[currMoneyArrayIdx].moneyAmount = dude.money[SilverType];
     currMoneyArrayIdx++;
   }
   if ((dude.money.IsActive(CopperType)) && (dude.money[CopperType] > 0))
   {
     charMoneyDataTempArray[currMoneyArrayIdx].moneyName = globalData.moneyData.GetName(CopperType);
     charMoneyDataTempArray[currMoneyArrayIdx].moneyAmount = dude.money[CopperType];
     currMoneyArrayIdx++;
   }
   if ((dude.money.IsActive(CoinType6)) && (dude.money[CoinType6] > 0))
   {
     charMoneyDataTempArray[currMoneyArrayIdx].moneyName = globalData.moneyData.GetName(CoinType6);
     charMoneyDataTempArray[currMoneyArrayIdx].moneyAmount = dude.money[CoinType6];
     currMoneyArrayIdx++;
   }
   if ((dude.money.IsActive(CoinType7)) && (dude.money[CoinType7] > 0))
   {
     charMoneyDataTempArray[currMoneyArrayIdx].moneyName = globalData.moneyData.GetName(CoinType7);
     charMoneyDataTempArray[currMoneyArrayIdx].moneyAmount = dude.money[CoinType7];
     currMoneyArrayIdx++;
   }
   if ((dude.money.IsActive(CoinType8)) && (dude.money[CoinType8] > 0))
   {
     charMoneyDataTempArray[currMoneyArrayIdx].moneyName = globalData.moneyData.GetName(CoinType8);
     charMoneyDataTempArray[currMoneyArrayIdx].moneyAmount = dude.money[CoinType8];
     currMoneyArrayIdx++;
   }
   if ((dude.money.IsActive(CoinType9)) && (dude.money[CoinType9] > 0))
   {
     charMoneyDataTempArray[currMoneyArrayIdx].moneyName = globalData.moneyData.GetName(CoinType9);
     charMoneyDataTempArray[currMoneyArrayIdx].moneyAmount = dude.money[CoinType9];
     currMoneyArrayIdx++;
   }
   if ((dude.money.IsActive(CoinType10)) && (dude.money[CoinType10] > 0))
   {
     charMoneyDataTempArray[currMoneyArrayIdx].moneyName = globalData.moneyData.GetName(CoinType10);
     charMoneyDataTempArray[currMoneyArrayIdx].moneyAmount = dude.money[CoinType10];
     currMoneyArrayIdx++;
   }

   for (int i=0; i<currMoneyArrayIdx; i++)
   {
       ST_FORM coinNameIdx = STF_none;
       ST_FORM coinAmountIdx = STF_none;

       switch (i)
       {
       case 0:
         coinNameIdx = STF_COIN1;
         coinAmountIdx = STF_coin1;
         break;
       case 1:
         coinNameIdx = STF_COIN2;
         coinAmountIdx = STF_coin2;
         break;
       case 2:
         coinNameIdx = STF_COIN3;
         coinAmountIdx = STF_coin3;
         break;
       case 3:
         coinNameIdx = STF_COIN4;
         coinAmountIdx = STF_coin4;
         break;
       case 4:
         coinNameIdx = STF_COIN5;
         coinAmountIdx = STF_coin5;
         break;
       case 5:
         coinNameIdx = STF_COIN6;
         coinAmountIdx = STF_coin6;
         break;
       case 6:
         coinNameIdx = STF_COIN7;
         coinAmountIdx = STF_coin7;
         break;
       case 7:
         coinNameIdx = STF_COIN8;
         coinAmountIdx = STF_coin8;
         break;
       case 8:
         coinNameIdx = STF_COIN9;
         coinAmountIdx = STF_coin9;
         break;
       case 9:
         coinNameIdx = STF_COIN10;
         coinAmountIdx = STF_coin10;
         break;
       }

     if (i < currMoneyArrayIdx)
     {
       pForm->SetText( coinNameIdx, charMoneyDataTempArray[i].moneyName);
       temp.Format("%6i", charMoneyDataTempArray[i].moneyAmount);
       pForm->SetText( coinAmountIdx, temp);
     }
     else
     {
       pForm->SetText( coinNameIdx, "");
       pForm->SetText( coinAmountIdx, "");
     }
   }

   // char AC
   charCurrStat = dude.GetAdjAC();
   charTrueStat = dude.GetEffectiveAC();
   IsAdj = (charCurrStat!=charTrueStat);
   pForm->SetText( STF_AC, "ARMOR CLASS");
   temp.Format("%5i", charTrueStat);
   if (IsAdj)
// prs 20191221     pForm->SetText( STF_ac, temp, AdjColor);
     pForm->SetText(STF_ac, temp, adjColorNum);
   else
     pForm->SetText( STF_ac, temp);

   // char THAC0
   charCurrStat = dude.GetAdjTHAC0();
   charTrueStat = dude.GetTHAC0();
   IsAdj = (charCurrStat!=charTrueStat);
   pForm->SetText( STF_THAC0, "THAC0");
   temp.Format("%5i", charCurrStat);
   if (IsAdj)
// prs 20191221     pForm->SetText( STF_thac0, temp, AdjColor);
     pForm->SetText(STF_thac0, temp, adjColorNum);
   else
     pForm->SetText( STF_thac0, temp);

   // char weapon damage
   pForm->SetText( STF_DAMAGE, "DAMAGE"); 
   dude.getCharWeaponText(temp, temp2);  
   if (temp.GetLength()>0) // wpn name
   {
      pForm->SetText( STF_weapon, temp);

      if (temp2.GetLength()>0) // wpn damage
      {
        temp.Format("%6s", temp2);
        pForm->SetText( STF_damage, temp);
      }
      else
// prs 20191221        pForm->SetText( STF_damage, "", Black);
        pForm->SetText(STF_damage, "", blackColor);
   }
   else
   {
     pForm->SetText( STF_weapon, "", blackColor);
     pForm->SetText( STF_damage, "", blackColor);
   }
   
   // char worn armor
   dude.getCharArmorText(temp);   
   if (temp.GetLength()>0)
     pForm->SetText( STF_armor, temp);
   else
    pForm->SetText( STF_armor, "", blackColor);
 
   // char encumbrance
   pForm->SetText( STF_ENCUM, "ENCUMBRANCE");   
   temp.Format("%5i", dude.GetEncumbrance());
   pForm->SetText( STF_encum, temp);

   // char movement rate
   charCurrStat = dude.displayValues.AdjMaxMovement; //dude.GetAdjMaxMovement();
   charTrueStat = dude.displayValues.MaxMovement;    //dude.GetMaxMovement();
   IsAdj = (charCurrStat!=charTrueStat);
   pForm->SetText( STF_MOVE, "MOVEMENT");   
   temp.Format("%5i", charCurrStat);
   if (IsAdj)
// prs 20191221     pForm->SetText( STF_move, temp, AdjColor);
     pForm->SetText(STF_move, temp, adjColorNum);
   else
     pForm->SetText( STF_move, temp);
}

CString defaultCharStats =
"$VAR char;"
"$VAR name;"
"$VAR age;"
"$VAR sex;"
"$VAR align;"
"$VAR alignList;"
"$VAR alignName;"
"$VAR class;"
"$VAR cbnList; "
"$VAR cbnCount; "
"$VAR cbn; "
"$VAR cbn2; "
"$VAR cbn3; "
"$VAR hbl; "
"$VAR hbn; "
"$VAR lvl;"
"$VAR lvl2;"
"$VAR lvl3;"
"$VAR xp;"
"$VAR xp2;"
"$VAR xp3;"
"$VAR status;"
"$VAR hp;"
"$VAR maxHp;"
"$VAR race;"
"$VAR ac;"
"$VAR acEff;"
"$VAR acAdj;"
"$VAR thac0;"
"$VAR thac0Adj;"
"$VAR str;"
"$VAR strMod;"
"$VAR int;"
"$VAR wis;"
"$VAR dex;"
"$VAR con;"
"$VAR cha;"
"$VAR dmg; "
"$VAR dmgBns; "
"$VAR enc;  "
"$VAR mvmt;"
"$VAR weight;"
"$VAR height;"
"$VAR wpn;"
"$VAR shield;"
"$VAR armor;"
"$VAR sWidth; "
"char = $IndexOf($CharacterContext());"
"name = $GET_CHAR_NAME(char);"
"age = $GET_CHAR_AGE(char);"
"$DEBUG(\"Age1=\" + age);"
"sex = $GET_CHAR_SEX(char);"
"align = $GET_CHAR_ALIGNMENT(char);"
"alignList = \"$LAWFUL GOOD$NEUTRAL GOOD$CHAOTIC GOOD$LAWFUL NEUTRAL$TRUE NEUTRAL$CHAOTIC NEUTRAL$LAWFUL EVIL$NEUTRAL EVIL$CHAOTIC EVIL\";"
"alignName = $DelimitedStringSubstring(alignList,align);        "
"class = $GET_CHAR_CLASS(char);"
"cbnList = $DAT_Class_Baseclasses(class);"
"cbnCount = $DelimitedStringCount(cbnList);"
"cbn = $DelimitedStringSubstring(cbnList,0);"
"cbn2 = $DelimitedStringSubstring(cbnList,1);"
"cbn3 = $DelimitedStringSubstring(cbnList,2);"
"hbn = $GetHighestLevelBaseclass(char);"
"hbl = $GetBaseclassLevel(char,hbn);"
"xp = $GET_CHAR_Exp(char,cbn);"
"xp2 = $GET_CHAR_Exp(char,cbn2);"
"xp3 = $GET_CHAR_Exp(char,cbn3);"
"lvl = $GetBaseclassLevel(char,cbn);"
"lvl2 = $GetBaseclassLevel(char,cbn2);"
"lvl3 = $GetBaseclassLevel(char,cbn3);"
"status = $GET_CHAR_STATUS(char);"
"hp = $GET_CHAR_HITPOINTS(char);"
"maxHp = $GET_CHAR_MAXHITPOINTS(char);"
"race = $GET_CHAR_RACE(char);"
"ac = $GET_CHAR_AC(char);"
"acEff = $GET_CHAR_EFFAC(char);"
"acAdj = $GET_CHAR_ADJAC (char);"
"thac0 = $GET_CHAR_THAC0(char);"
"thac0Adj = $GET_CHAR_ADJTHAC0(char);"
"str = $GET_CHAR_LIMITED_STR(char);"
"strMod = $GET_CHAR_LIMITED_STRMOD(char);"
"int = $GET_CHAR_LIMITED_INT(char);"
"wis = $GET_CHAR_LIMITED_WIS(char);"
"dex = $GET_CHAR_LIMITED_DEX(char);"
"con = $GET_CHAR_LIMITED_CON(char);"
"cha = $GET_CHAR_LIMITED_CHA(char);"
"dmgBns = $GET_CHAR_DAMAGEBONUS(char);"
"enc = $GET_CHAR_ENC(char);"
"mvmt = $GET_CHAR_MAXMOVE(char);"
"weight = $GET_CHAR_ASL(char,\"charWeight\");"
"height = $GET_CHAR_ASL(char,\"charHeight\");"
"$IF (weight == \"\")"
   "{"
       "$SET_CHAR_ASL(char,\"charWeight\",$DAT_Race_Weight($CharacterContext()));"
       "weight = $GET_CHAR_ASL(char,\"charWeight\");"
   "};"
"$IF (height == \"\")"
   "{"
       "$SET_CHAR_ASL(char,\"charHeight\",$DAT_Race_Height($CharacterContext()));"
       "height = $GET_CHAR_ASL(char,\"charHeight\");"
   "};"

"wpn = $GET_CHAR_Ready($CharacterContext(),\"WEAPON\",0);"
"$DEBUG(\"Age2=\" + age);"
"shield = $GET_CHAR_Ready($CharacterContext(),\"SHIELD\",0);"
"armor = $GET_CHAR_Ready($CharacterContext(),\"ARMOR\",0);"
"sWidth = $GET_CONFIG(\"Screen_Width\");"
""
"$IF (sWidth ==# 640)"
   "{"
       "$GrSet(\"LeftCol\",18, 18);"
       "$GrSet(\"RightCol\", 220, 18);"
       "$GrSet(\"Skills\", 417, 270);"
       "$GrSet(\"Ability\", 50, 0);"
       "$GrSet(\"StatusTab\", 105, 0);"
       "$GrSet(\"SkillsTab\", 125, 0);"
       "$GrSet(\"EncumbTab\",125, 0);"
       "$GrSet(\"TextHeight\", 0, 18);"
       "$GrSetLinefeed(\"TextHeight\");"
   "};"
"$DEBUG(\"Age1003=\" + age + \"sWidth=\" + sWidth);"
"$IF (sWidth ==# 800)"
   "{"
       "$GrSet(\"LeftCol\",20,20);"
       "$GrSet(\"RightCol\",260,20);"
       "$GrSet(\"Skills\", 450, 380);"
       "$GrSet(\"Ability\",75,0);"
       "$GrSet(\"StatusTab\",130,0);"
       "$GrSet(\"SkillsTab\", 150, 0);"
       "$GrSet(\"EncumbTab\",150,0);"
       "$GrSet(\"TextHeight\",0,20);"
       "$GrSetLinefeed(\"TextHeight\");"
   "};"
"$DEBUG(\"Age1016=\" + age);"
"$IF (sWidth ==# 1024)"
   "{"
       "$GrSet(\"LeftCol\",20,20);"
       "$GrSet(\"RightCol\", 300,20);"
       "$GrSet(\"Skills\", 608, 490);"
       "$GrSet(\"Ability\", 75, 0);"
       "$GrSet(\"StatusTab\", 150, 0);"
       "$GrSet(\"SkillsTab\", 175, 0);"
       "$GrSet(\"EncumbTab\",175, 0);"
       "$GrSet(\"TextHeight\",0,20);"
       "$GrSetLinefeed(\"TextHeight\");"
   "};"
"$DEBUG(\"Age1029=\" + age + \"  chartype=\" + $GET_CHAR_TYPE(char));"
"$IF ($GET_CHAR_TYPE(char) != \"@PC@\" && $GET_CHAR_TYPE(char) != \"@NPC@\")"
   "{"
       "$GrMoveTo(\"LeftCol\");"
       "$GrFormat(\"SL\");"
       "$GrColor(\"YELLOW\");"
       "$GrPrtLF($UpCase(name));"
       "$GrMark(\"Level\");"
       "$GrMoveTo(\"RightCol\");"
       "$GrColor(\"WHITE\");"
       "$GrPrint(\"STATUS\");"
       "$IF (status >=# 2) {$GrColor(\"RED\"); $GrTab(\"StatusTab\"); $GrPrtLF(\"DEAD\");};"
       "$IF (status ==# 1) {$GrColor(\"ORANGE\"); $GrTab(\"StatusTab\"); $GrPrtLF(\"UNCONSCIOUS\");};"
       "$IF (status ==# 0) {$GrColor(\"GREEN\"); $GrTab(\"StatusTab\"); $GrPrtLF(\"OKAY\");};"
       "$GrColor(\"WHITE\");"
       "$GrPrint(\"HIT POINTS\");"
       "$IF (hp <=# 0) {$GrColor(\"RED\"); $GrTab(\"StatusTab\"); $GrPrint(hp);};"
       "$IF ((hp >=# 1) && (hp <# maxHp)) {$GrColor(\"ORANGE\"); $GrTab(\"StatusTab\"); $GrPrint(hp);};"
       "$IF (hp ==# maxHp) {$GrColor(\"GREEN\"); $GrTab(\"StatusTab\"); $GrPrint(hp);};"
       "$GrColor(\"GREEN\");"
       "$GrPrtLF(\"/\" + maxHp);"
       "$GrColor(\"WHITE\");"
       "$GrPrtLF($UpCase(race));"
       "$GrMoveTo(\"Level\");"
       "$GrMove(\"TextHeight\"); "
       "$GrMove(\"TextHeight\"); "
       "$GrMove(\"TextHeight\"); "
       "$GrMove(\"TextHeight\"); "
       "$GrColor(\"WHITE\");"
       "$GrMark(\"Strength\");"
       "$GrPrtLF(\" \");"
       "$GrPrtLF(\"INT\");"
       "$GrPrtLF(\" \");"
       "$GrPrtLF(\" \");"
       "$GrPrtLF(\" \");"
       "$GrPrtLF(\" \");"
       "$GrMark(\"Armor\");"
       "$GrMoveTo(\"Strength\");"
       "$GrMove(\"Ability\");"
       "$GrColor(\"GREEN\");"
       "$GrPrtLF(\" \");"
       "$GrPrtLF(int);"
       "$GrPrtLF(\" \");"
       "$GrPrtLF(\" \");"
       "$GrPrtLF(\" \");"
       "$GrPrtLF(\" \");"
       "$GrMoveTo(\"Armor\");"
       "$GrMove(\"TextHeight\"); "
       "$GrColor(\"WHITE\");"
       "$GrPrint(\"ARMOR CLASS\");"
       "$IF (ac !=# acEff) {$GrColor(\"ORANGE\"); $GrTab(\"StatusTab\"); $GrPrtLF(acEff);};"
       "$IF (ac ==# acEff) {$GrColor(\"GREEN\"); $GrTab(\"StatusTab\"); $GrPrtLF(ac);};"
       "$GrColor(\"WHITE\");"
       "$GrPrint(\"THAC0\");"
       "$IF (thac0 !=# thac0Adj) {$GrColor(\"ORANGE\"); $GrTab(\"StatusTab\"); $GrPrtLF(thac0Adj);};"
       "$IF (thac0 ==# thac0Adj) {$GrColor(\"GREEN\"); $GrTab(\"StatusTab\"); $GrPrtLF(thac0);};"
       "$RETURN;"
   "};"
"$DEBUG(\"Age1085=\" + age);"
""
"$DEBUG(\"Age1086=\" + age);"
"$GrMoveTo(\"LeftCol\");"
"$GrFormat(\"SL\");"
"$GrColor(\"YELLOW\");"
"$GrPrtLF($UpCase(name));"
"$GrColor(\"WHITE\");"
"$GrPrint($UpCase(sex));"
"$GrTab(\"StatusTab\");"
"$DEBUG(\"Age6=\" + age);"
"$GrColor(\"GREEN\");"
"$GrPrint(age);"
"$GrColor(\"WHITE\");"
"$GrPrtLF(\" YEARS\");"
"$GrPrtLF(alignName);\n"
"$GrPrtLF($UpCase(class));"
"$IF (cbnCount ==# 3)"
   "{"
       "$GrPrint(\"LEVEL/XP \" + lvl + \"/\" + xp);"
       "$GrPrint(\" \" + lvl2 + \"/\" + xp2);"
       "$GrPrtLF(\" \" + lvl3 + \"/\" + xp3);"
   "};"
"$IF (cbnCount ==# 2 && cbnList != \"-?-?-\")"
   "{"
       "$GrPrint(\"LEVEL/XP \" + lvl + \"/\" + xp);"
       "$GrPrtLF(\" \" + lvl2 + \"/\" + xp2);"
   "};"
"$IF (cbnCount ==# 1)"
   "{"
       "$GrPrtLF(\"LEVEL/XP \" + lvl + \"/\" + xp);"
   "};"
"$GrMark(\"Level\");"
"$GrMoveTo(\"RightCol\");"
"$GrColor(\"WHITE\");"
"$GrPrint(\"STATUS\");\n"
"$IF (status >=# 2)"
   "{"
       "$GrColor(\"RED\");"
       "$GrTab(\"StatusTab\");"
       "$GrPrtLF(\"DEAD\");"
   "};"
"$IF (status ==# 1)"
   "{"
       "$GrColor(\"ORANGE\");"
       "$GrTab(\"StatusTab\");"
       "$GrPrtLF(\"UNCONSCIOUS\");"
   "};"
"$IF (status ==# 0)"
   "{"
       "$GrColor(\"GREEN\");"
       "$GrTab(\"StatusTab\");"
       "$GrPrtLF(\"OKAY\");"
   "};"
"$GrColor(\"WHITE\");\n"
"$GrPrint(\"HIT POINTS\");"
"$IF (hp <=# 0)"
   "{"
       "$GrColor(\"RED\");"
       "$GrTab(\"StatusTab\");"
       "$GrPrint(hp);"
   "}"
"$ELSE"
   "{"
       "$IF ((hp >=# 1) && (hp <# maxHp))"
           "{"
               "$GrColor(\"ORANGE\");"
               "$GrTab(\"StatusTab\");"
               "$GrPrint(hp);"
           "}"
       "$ELSE"
           "{"
               "$GrColor(\"GREEN\");"
               "$GrTab(\"StatusTab\");"
               "$GrPrint(hp);"
           "};"
   "};\n"
"$GrColor(\"GREEN\");"
"$GrPrtLF(\"/\" + maxHp);"
"$GrColor(\"WHITE\");"
"$GrPrtLF($UpCase(race));"
"$GrColor(\"GREEN\");"
"$GrPrint(weight);"
"$GrColor(\"WHITE\");"
"$GrPrint(\" LBS\");"
"$GrColor(\"GREEN\");"
"$GrTab(\"StatusTab\");"
"$GrPrint(height /# 12);"
"$GrColor(\"WHITE\");"
"$GrPrint(\"'\");"
"$GrColor(\"GREEN\");"
"$GrPrint(height %# 12);\n"
"$GrColor(\"WHITE\");"
"$GrPrtLF(\"\\\"\");"
"$GrMoveTo(\"Level\");"
"$GrMove(\"TextHeight\"); "
"$GrColor(\"WHITE\");"
"$GrMark(\"Strength\");"
"$GrPrtLF(\"STR\");"
"$GrPrtLF(\"INT\");"
"$GrPrtLF(\"WIS\");"
"$GrPrtLF(\"DEX\");"
"$GrPrtLF(\"CON\");"
"$GrPrtLF(\"CHA\");"
"$GrMark(\"Armor\");"
"$GrMoveTo(\"Strength\");"
"$GrMove(\"Ability\");"
"$GrColor(\"GREEN\");"
"$IF (strMod >=# 1){$GrPrtLF(str + \"(\" + strMod + \")\");}"
"$ELSE{$GrPrtLF(str);};"
"$GrPrtLF(int);"
"$GrPrtLF(wis);"
"$GrPrtLF(dex);"
"$GrPrtLF(con);"
"$GrPrtLF(cha);\n"
"$GrMoveTo(\"Armor\");"
"$GrMove(\"TextHeight\"); "
"$GrColor(\"WHITE\");"
"$GrPrint(\"ARMOR CLASS\");"
"$IF (ac !=# acEff)"
   "{"
       "$GrColor(\"ORANGE\");"
       "$GrTab(\"StatusTab\");"
       "$GrPrtLF(acEff);"
   "}"
"$ELSE"
   "{"
       "$GrColor(\"GREEN\");"
       "$GrTab(\"StatusTab\");"
       "$GrPrtLF(ac);"
   "};"
"$GrColor(\"WHITE\");"
"$GrPrint(\"THAC0\");"
"$IF (thac0 !=# thac0Adj)"
   "{"
       "$GrColor(\"ORANGE\");"
       "$GrTab(\"StatusTab\");"
       "$GrPrtLF(thac0Adj);"
   "}"
"$ELSE"
   "{"
       "$GrColor(\"GREEN\");"
       "$GrTab(\"StatusTab\");"
       "$GrPrtLF(thac0);"
   "};"
"$GrColor(\"WHITE\");"
"$GrPrint(\"DAMAGE\");"
"$GrColor(\"GREEN\");"
"$GrTab(\"StatusTab\");"
"$IF (dmgBns ># 0){$GrPrtLF(\"1D2+\" + dmgBns);}"
"$ELSE {$GrPrtLF(\"1D2\");};"
"$GrColor(\"WHITE\");"
"$GrMove(\"TextHeight\"); "
""
"$IF ((wpn == \"\") && (shield == \"\") && (armor == \"\")){$GrPrtLF(\"NONE\");};"
"$IF (wpn != \"\"){$GrPrtLF($UpCase($DAT_Item_IDName(wpn)));};"
"$IF (shield != \"\"){$GrPrtLF($UpCase($DAT_Item_IDName(shield)));};"
"$IF (armor != \"\"){$GrPrtLF($UpCase($DAT_Item_IDName(armor)));};"
"$GrSet(\"Encumbrance\", \"RightCol\", \"Armor\");"
"$GrMoveTo(\"Encumbrance\");"
"$GrMove(\"TextHeight\");"
"$GrColor(\"WHITE\");"
"$GrPrint(\"ENCUMBRANCE\");"
"$GrColor(\"GREEN\");"
"$GrTab(\"EncumbTab\");"
"$GrPrtLF(enc);"
"$GrColor(\"WHITE\");"
"$GrPrint(\"MOVEMENT\");"
"$GrColor(\"GREEN\");"
"$GrTab(\"EncumbTab\");"
"$GrPrtLF(mvmt);"
""
"$IF (class == \"Assassin\" || class == \"Thief\")"
   "{"
       "$GrMoveTo(\"Skills\");"
       "$GrColor(\"WHITE\");"
       "$GrPrtLF(\"SKILLS\");"
       "$GrMark(\"Pick\");"
       "$GrPrtLF(\"Pick Pockets\");"
       "$GrPrtLF(\"Open Locks\");"
       "$GrPrtLF(\"Find Traps\");"
       "$GrPrtLF(\"Move Silently\");"
       "$GrPrtLF(\"Hide in Shadows\");"
       "$GrPrtLF(\"Hear Noise\");"
       "$GrPrtLF(\"Climb Walls\");"
       "$GrPrtLF(\"Read Languages\");"
       "$GrMoveTo(\"Pick\");"
       "$GrMove(\"SkillsTab\");"
       "$GrColor(\"GREEN\");"
       "$GrPrtLF($SkillAdj($CharacterContext(),\"PickPockets\",\"\",\"F\",\"\"));"
       "$GrPrtLF($SkillAdj($CharacterContext(),\"OpenLocks\",\"\",\"F\",\"\"));"
       "$GrPrtLF($SkillAdj($CharacterContext(),\"FindTraps\",\"\",\"F\",\"\"));"
       "$GrPrtLF($SkillAdj($CharacterContext(),\"MoveSilent\",\"\",\"F\",\"\"));"
       "$GrPrtLF($SkillAdj($CharacterContext(),\"HideInShadows\",\"\",\"F\",\"\"));"
       "$GrPrtLF($SkillAdj($CharacterContext(),\"HearNoise\",\"\",\"F\",\"\"));"
       "$GrPrtLF($SkillAdj($CharacterContext(),\"ClimbWalls\",\"\",\"F\",\"\"));"
       "$GrPrtLF($SkillAdj($CharacterContext(),\"ReadLanguages\",\"\",\"F\",\"\"));"
   "};"
;











static CString statsASLValue;;
static int statsASLFlags;
extern CString frontEnd, backEnd;
extern const char *SAentries[];

struct GRDEF
{
  CString name;
  int x;
  int y;
};



struct GR_CONTROL
{
  mCArray<GRDEF, GRDEF&> grDef;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(GrDef,             /* array data name      */ \
                                  CString,           /* Search variable type */ \
                                  name,              /* search variable name */ \
                                  grDef,             /* array name           */ \
                                  GRDEF,             /* array data type      */ \
                                  GR_CONTROL)        /* type of *this        */
  int linefeedX, linefeedY;
  int anchorX, anchorY;
  int cursorX, cursorY;
  void Clear(void);
// prs 20191221  unsigned int color;
  FONT_COLOR_NUM colorNum;
  CString format;
  GRDEF *Find(const CString& name, bool create);
};

void GR_CONTROL::Clear(void)
{
  grDef.RemoveAll();
  linefeedX = 0;
  linefeedY = 12;
  anchorX = 0;
  anchorY = 0;
  format.Empty();
}

GR_CONTROL grc;


int GR_CONTROL::LocateGrDef(const CString& name) const
{
  int i, n;
  n = grDef.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekGrDef(i)->name == name)
    {
      return i;
    };
  }
  return -1;
}

GRDEF *GR_CONTROL::Find(const CString& name, bool create)
{
  int i;
  i = LocateGrDef(name);
  if (i >= 0) return GetGrDef(i);
  if (!create) return NULL;
  {
    GRDEF gd;
    i = grDef.GetSize();
    gd.name = name;
    gd.x = 0;
    gd.y = 0;
    grDef.SetAtGrow(i, gd);
    return GetGrDef(i);
  };
}

CString GrSet(const CString& name, const CString& x, const CString& y)
{
  GRDEF *pName, *pX, *pY;
  pName = grc.Find(name, true);
  pX =  grc.Find(x, false);
  pY =  grc.Find(y, false);
  if (pX == NULL)
  {
    pName->x = atoi(x);
  }
  else
  {
    pName->x = pX->x;
  }
  if (pY == NULL)
  {
    pName->y = atoi(y);
  }
  else
  {
    pName->y = pY->y;
  };
  return "";
}
CString GrSetLinefeed(const CString& name)
{
  GRDEF *pGD;
  pGD = grc.Find(name, false);
  if (pGD != NULL)
  {
    grc.linefeedX = pGD->x;
    grc.linefeedY = pGD->y;
  };
  return "";
}

CString GrMoveTo(const CString& name)
{
  GRDEF *pGD;
  pGD = grc.Find(name, false);
  if (pGD != NULL)
  {
    grc.cursorX = grc.anchorX = pGD->x;
    grc.cursorY = grc.anchorY = pGD->y;
  };
  return "";
}

CString GrMove(const CString& name)
{
  GRDEF *pGD;
  pGD = grc.Find(name, false);
  if (pGD != NULL)
  {
    grc.anchorX += pGD->x;
    grc.anchorY += pGD->y;
    grc.cursorX = grc.anchorX;
    grc.cursorY = grc.anchorY;
  };
  return "";
}

CString GrTab(const CString& name)
{
  GRDEF *pGD;
  pGD = grc.Find(name, false);
  if (pGD != NULL)
  {
    grc.cursorX = grc.anchorX + pGD->x;
    grc.cursorY = grc.anchorY + pGD->y;
  };
  return "";
}

CString GrPic(const CString& name)
{
  return "";
}

CString GrPrint(const CString& text, int fontNumber, BOOL customColor)
{
  DisplayText( grc.cursorX, grc.cursorY, 
               fontNumber, 
               text, 
               grc.colorNum, customColor, 
               false, false, true);
  grc.cursorX += GraphicsMgr.GetTextWidth(text, text.GetLength(), false);
  return "";
}

CString GrPrtLF(const CString& text, int fontNumber, BOOL customColor)
{
  DisplayText(grc.cursorX, grc.cursorY, 
              fontNumber, 
              text, 
              grc.colorNum, customColor,
              false, false, true);
  grc.anchorX += grc.linefeedX;
  grc.anchorY += grc.linefeedY;
  grc.cursorX = grc.anchorX;
  grc.cursorY = grc.anchorY;
  return "";
}

CString GrMark(const CString& name)
{
  GRDEF *pGD;
  pGD = grc.Find(name, true);
  pGD->x = grc.cursorX;
  pGD->y = grc.cursorY;
  return "";
}

CString GrFormat(const CString& name)
{
  grc.format = name;
  return "";
}

CString GrColor(const CString& name, int c)
{
  if (name == "WHITE") 
  {
// prs 20191221    gc.color = White;
    grc.colorNum = whiteColor;
  } 
  else if (name == "GREEN")
  {
    grc.colorNum = greenColor;
  }
  else if (name == "YELLOW")
  {
    grc.colorNum = yellowColor;
  }
  else if (name == "ORANGE")
  {
    grc.colorNum = orangeColor;
  }
  else if (name == "RED")
  {
    grc.colorNum = redColor;
  }
  else if (name == "CYAN")
  {
    grc.colorNum = cyanColor;
  }
  else if (name == "MAGENTA")
  {
    grc.colorNum = magentaColor;
  }
  else if (name == "SILVER")
  {
    grc.colorNum = silverColor;
  }
  else if (name == "BLACK")
  {
    grc.colorNum = blackColor;
  }
  else if (name == "BLUE")
  {
    grc.colorNum = blueColor;
  }
  else
  {
    grc.colorNum = whiteColor;
  };
  
  return "";
}


void showCharStats(CHARACTER& dude, bool createdCharacter)
{ 
  GPDL gpdl;
  grc.Clear();
  if (statsASLValue.IsEmpty())
  {
    GPDLCOMP gpdlcomp;
    CString temp;
    {
      // First, we will search the global Special Abilities for
      // an appropriate script.
      SPECABILITY_DEF  *pSpecAb;
      statsASLValue = defaultCharStats;   // In case we find no script.
      statsASLFlags = SPECAB_SCRIPT;
      ASLENTRY *pSpecString;




      pSpecAb = specialAbilitiesData.FindAbility("Global_Display");
      if (pSpecAb != NULL)
      {
        pSpecString = pSpecAb->Find(CHARACTER_VIEW_SCREEN);
        if (pSpecString != NULL)
        {
          if (pSpecString->Flags() == SPECAB_SCRIPT)
          {
            GPDLCOMP gpdlComp;
            temp = gpdlComp.CompileScript(frontEnd+pSpecString->Value()+backEnd, SAentries);
            if (temp[0] != 0)
            {
              WriteDebugString("* * * * Script Error in %s[%s]\n", pSpecAb->Name(),CHARACTER_VIEW_SCREEN);
            }
            else
            {
              statsASLValue = temp;
              statsASLFlags = SPECAB_BINARYCODE;
            };
          };
        };
      };
      if (statsASLFlags != SPECAB_BINARYCODE)
      {
        temp = gpdlcomp.CompileScript(frontEnd + statsASLValue + backEnd, SAentries);
        if (temp[0] != 0)
        {
          WriteDebugString("* * * * Script Error in \"CharacterViewScreen\" script\n");
          WriteDebugString("%s\n", temp);
          statsASLFlags = SPECAB_SCRIPTERROR;
        }
        else
        {
          statsASLValue = temp;
          statsASLFlags = SPECAB_BINARYCODE;
        };
      };
    };
  };
  if (statsASLFlags == SPECAB_BINARYCODE)
  {
    gpdlStack.Push();
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
//    if (nullSA)
//    {
//      CStringPAIR SAcontext(SAname, "");
//      origSA = pScriptContext->specAb;
//      pOrigContext = pScriptContext->context;
//      pScriptContext->SetAbility(NULL, &SAcontext);
//      pScriptContext->SetSA_ScriptName(scriptName);
//    };
    if (createdCharacter)
    {
      scriptContext.SetCreatedCharacterContext(&dude);
    }
    else
    {
      scriptContext.SetCharacterContext(&dude);
    };
    pScriptContext->SetSA_Source_Type(ScriptSourceType_Character);
    pScriptContext->SetSA_Source_Name(dude.GetName());
    pScriptContext->SetSA_ScriptName(CHARACTER_VIEW_SCREEN);
    (*p_hook_parameters)[0] = gpdlStack.activeGPDL()->ExecuteScript(statsASLValue,1, NULL, 0); 
    //hookParameters[0] = gpdlStack[++activeGPDL].ExecuteScript(pSpecString->Value(),1, NULL, 0); 
    gpdlStack.Pop();
//    if (nullSA)
//    {
//      pScriptContext->specAb = origSA;
//    };
//    //callbackResult = ScriptCallback_RunAllScripts(CBF_EXAMINESCRIPT, &(*p_hook_parameters)[0], NULL);
//    };
  }
//  else
//  {
//    ScriptCallback_RunAllScripts(CBF_DEFAULT, &(*p_hook_parameters)[0], NULL);
//  };
//  return (*p_hook_parameters)[0];
//  
//  grDef.RemoveAll();
}



void displayCharStats(CHARACTER &dude)
{
#ifdef OldCharStats
  TEXT_FORM StatsForm(statsForm);
  StatsForm.ClearForm();
  showCharStats(&StatsForm,dude);
  StatsForm.Display();
#else
  showCharStats(dude, false);
#endif
}
 

void displayCreatedCharStats(CHARACTER &dude)
{
#ifdef OldCharStats
  TEXT_FORM StatsForm(statsForm);
  StatsForm.ClearForm();
  showCharStats(&StatsForm,dude);
  StatsForm.Display();
#else
  showCharStats(dude, true);
#endif
}
 

extern bool AllowModifyStats;

class CH
{ // A class to hold a pointer to a character.
  // We need the destructor to release the memory
  // when the program terminates without going through
  // the normal menu-handling process. Otherwise a
  // memory leak warning is issued.
public:
  CHARACTER *c;
  CH(void) { c=NULL;};
  ~CH(void) { if (c) My_free(c);};
};


static bool STF_highlight (TEXT_FORM *frm, 
                      int *oldSelection,
                      int newSelection) {
  // See if new selection is valid.
  // If so, set old selection.
  // If old selection changes, highlight it and return true.
  int hi1=-1, hi2=-1;
  switch (newSelection)
  {
  case STF_Str:
    hi1=STF_STR;
    hi2=STF_str;
    break;
  case STF_Int:
    hi1=STF_INT;
    hi2=STF_int;
    break;
  case STF_Wis:
    hi1=STF_WIS;
    hi2=STF_wis;
    break;
  case STF_Dex:
    hi1=STF_DEX;
    hi2=STF_dex;
    break;
  case STF_Con:
    hi1=STF_CON;
    hi2=STF_con;
    break;
  case STF_Cha:
    hi1=STF_CHA;
    hi2=STF_cha;
    break;
   default:  // leave things as they are.
    newSelection=-1;
  };
  if ((newSelection!=-1) && (newSelection!=*oldSelection))
  {
    frm->Highlight(hi1);
    frm->Highlight(hi2);
    *oldSelection=newSelection;
    return true;
  };
  return false;
}

bool STF_IncrStat(int selection, int *avail, CHARACTER *dude)
{
  // Returns true if something has changed.
  if (*avail==0) return false;
  BYTE orig, final;
  ABILITY_ID abilityID;
  switch (selection)
  {
  case STF_Str:
    //if (dude->GetStr()>=ABILITYMAXBASE(dude->GetAbilityLimits(0))) return false;
    orig=dude->GetPermStr();
    if (orig>=ABILITYMAXBASE(dude->GetAbilityLimits(abilityID=Ability_Strength))) return false;
    dude->SetPermStr(orig+1);
    dude->UpdateStats(FALSE);
    final = dude->GetPermStr();
    //if (orig != dude->GetStr()) 
    {
      if (final==18)
      {
        if (strmod==0) 
        {
           //strmod = RollDice(100,1,0.0);
          const CLASS_DATA *pClass;
          pClass = classData.PeekClass(dude->GetClass());
          if (pClass != NULL)
          {
            double roll;
            pClass->strengthBonusDice.Roll(roll);
            strmod = roll;
            if (strmod <= 0) strmod = 0;
          };
        };
        dude->SetPermStrMod(strmod);
      }
    }
    break;
  case STF_Int:
    //if (dude->GetInt()>=ABILITYMAXBASE(dude->GetAbilityLimits(1))) return false;
    orig=dude->GetPermInt();
    if (orig>=ABILITYMAXBASE(dude->GetAbilityLimits(abilityID=Ability_Intelligence))) return false;
    dude->SetPermInt(orig+1);
    dude->UpdateStats(FALSE);
    final = dude->GetPermInt();
    //if (orig != dude->GetInt())
    break;
  case STF_Wis:
    //if (dude->GetWis()>=ABILITYMAXBASE(dude->GetAbilityLimits(2))) return false;
    orig=dude->GetPermWis();
    if (orig>=ABILITYMAXBASE(dude->GetAbilityLimits(abilityID=Ability_Wisdom))) return false;
    dude->SetPermWis(orig+1);
    dude->UpdateStats(FALSE);
    final = dude->GetPermWis();
    //if (orig != dude->GetWis()) 
    break;
  case STF_Dex:
    //if (dude->GetDex()>=ABILITYMAXBASE(dude->GetAbilityLimits(3))) return false;
    orig=dude->GetPermDex();
    if (orig>=ABILITYMAXBASE(dude->GetAbilityLimits(abilityID=Ability_Dexterity))) return false;
    dude->SetPermDex(orig+1);
    dude->UpdateStats(FALSE);
    final = dude->GetPermDex();
    //if (orig != dude->GetDex()) 
    break;
  case STF_Con:
    //if (dude->GetCon()>=ABILITYMAXBASE(dude->GetAbilityLimits(4))) return false;
    orig=dude->GetPermCon();
    if (orig>=ABILITYMAXBASE(dude->GetAbilityLimits(abilityID=Ability_Constitution))) return false;
    dude->SetPermCon(orig+1);
    dude->UpdateStats(FALSE);
    final = dude->GetPermCon();
    //if (orig != dude->GetCon()) 
    break;
  case STF_Cha:
    //if (dude->GetCha()>=ABILITYMAXBASE(dude->GetAbilityLimits(5))) return false;
    orig=dude->GetPermCha();
    if (orig>=ABILITYMAXBASE(dude->GetAbilityLimits(abilityID=Ability_Charisma))) return false;
    dude->SetPermCha(orig+1);
    dude->UpdateStats(FALSE);
    final = dude->GetPermCha();
    //if (orig != dude->GetCha())
    break;
  default: return false;
  };
  *avail += orig - final;
  return true;
}

bool STF_DecrStat(int selection, int *avail, CHARACTER *dude)
{
  // Returns true if something has changed.
  BYTE orig;
  ABILITY_ID abilityID;
  switch (selection )
  {
  case STF_Str:
    //if (dude->GetStr()<=ABILITYMINBASE(dude->GetAbilityLimits(0))) return false;
    orig=dude->GetPermStr();
    if (orig<=ABILITYMINBASE(dude->GetAbilityLimits(abilityID=Ability_Strength))) return false;
    if (orig==18) dude->SetPermStrMod(0);
    dude->SetPermStr(orig-1);
    dude->UpdateStats(FALSE);
    //if (orig != dude->GetStr())
    {
      (*avail)++;    
    };
    break;
  case STF_Int:
    //if (dude->GetInt()<=ABILITYMINBASE(dude->GetAbilityLimits(1))) return false;
    orig=dude->GetPermInt();
    if (orig<=ABILITYMINBASE(dude->GetAbilityLimits(abilityID=Ability_Intelligence))) return false;
    dude->SetPermInt(orig-1);
    dude->UpdateStats(FALSE);
    //if (orig != dude->GetInt())
    {
      (*avail)++;
    };
    break;
  case STF_Wis:
    //if (dude->GetWis()<=ABILITYMINBASE(dude->GetAbilityLimits(2))) return false;
    orig=dude->GetPermWis();
    if (orig<=ABILITYMINBASE(dude->GetAbilityLimits(abilityID=Ability_Wisdom))) return false;
    dude->SetPermWis(orig-1);
    dude->UpdateStats(FALSE);
    //if (orig != dude->GetWis())
    {
      (*avail)++;
    };
    break;
  case STF_Dex:
    //if (dude->GetDex()<=ABILITYMINBASE(dude->GetAbilityLimits(3))) return false;
    orig=dude->GetPermDex();
    if (orig<=ABILITYMINBASE(dude->GetAbilityLimits(abilityID=Ability_Dexterity))) return false;
    dude->SetPermDex(orig-1);
    dude->UpdateStats(FALSE);
    //if (orig != dude->GetDex())
    {
      (*avail)++;
    };
    break;
  case STF_Con:
    //if (dude->GetCon()<=ABILITYMINBASE(dude->GetAbilityLimits(4))) return false;
    orig=dude->GetPermCon();
    if (orig<=ABILITYMINBASE(dude->GetAbilityLimits(abilityID=Ability_Constitution))) return false;
    dude->SetPermCon(orig-1);
    dude->UpdateStats(FALSE);
    //if (orig != dude->GetCon())
    {
      (*avail)++;
    };
    break;
  case STF_Cha:
    //if (dude->GetCha()<=ABILITYMINBASE(dude->GetAbilityLimits(5))) return false;
    orig=dude->GetPermCha();
    if (orig<=ABILITYMINBASE(dude->GetAbilityLimits(abilityID=Ability_Charisma))) return false;
    dude->SetPermCha(orig-1);
    dude->UpdateStats(FALSE);
    //if (orig != dude->GetCha())
    {
      (*avail)++;
    };
    break;
  default: return false;
  };  
  return true;
}

int handleChooseStatsInput(CHAR_STATS_INPUT_MESSAGE_TYPE msg,
                            CHARACTER *dude,
                            int P1, 
                            int P2,
                            const SELECTION_PAGE_FORMAT *format)
// Returns 0x0001 if you should display menu and flip surfaces.
// Returns 0x0002 if we handled a keystroke and you can ignore it
{
  TEXT_FORM StatsForm(statsForm);
  static bool chooseActive=false;
  static int currentSelection;
  static int availPoints;
  static CH origChar;
  int newSelection;
  int result=0;
  bool flip=false, redraw=false;

  // Set the xy coordinates of the five sections of the screen
  StatsForm.SetXY(STF_name,   format->Title_x, format->Title_y);
  StatsForm.SetXY(STF_STATUS, format->Header_x, format->Header_y);
  StatsForm.SetXY(STF_STR,    format->Header2_x, format->Header2_y);
  StatsForm.SetXY(STF_AC,     format->Header3_x, format->Header3_y);
  StatsForm.SetXY(STF_ENCUM,  format->Menu_x, format->Menu_y);

  switch (msg)
  {
  case CHOOSESTATS_initial:
    ASSERT(chooseActive==false);
    chooseActive=true;
    currentSelection=-1;
    availPoints=0;
    origChar.c=new CHARACTER(*dude);
    redraw=true;
    strmod=0;
    break;
  case CHOOSESTATS_refresh:
    ASSERT(chooseActive);
    ASSERT(dude!=NULL);
    redraw=true;
    break;
  case CHOOSESTATS_mouseClick:
    {
    ASSERT(chooseActive);
    ASSERT(dude!=NULL);
    if (!AllowModifyStats) return false; // Ignore mouse
    StatsForm.ClearForm();
    int junk=-1;
    STF_highlight(&StatsForm,&junk,currentSelection);
    showCharStats(&StatsForm, *dude);  // To define rectangles.
    newSelection=StatsForm.MouseClick(P1, P2);  // P1=x, P2=y
    if (STF_highlight(&StatsForm,&currentSelection,newSelection)) flip=true;
    }
    break;
  case CHOOSESTATS_exit:
    chooseActive=false;
    if (origChar.c!=NULL)
    {
      delete origChar.c;
      origChar.c=NULL;
    };
    break;
  case CHOOSESTATS_key:
    ASSERT(chooseActive);
    if (!AllowModifyStats) break;
    switch(P1)
    {
    case KC_TAB:
      newSelection=StatsForm.Tab(currentSelection);
      if (STF_highlight(&StatsForm,&currentSelection,newSelection)) redraw=true;
      result|=2;
      break;
    case KC_PLUS:
    case KC_UP:
      if (STF_IncrStat(currentSelection,&availPoints,dude))
      {
        redraw=true;
      };
      {
        dude->maxHitPoints = 0;
        dude->DetermineNewCharMaxHitPoints(dude->hitpointSeed);
        dude->hitPoints = dude->maxHitPoints;
      };
      result|=2;
      break;
    case KC_MINUS:
    case KC_DOWN:
      if (STF_DecrStat(currentSelection,&availPoints,dude))
      {
        redraw=true;
      };
      {
        dude->maxHitPoints = 0;
        dude->DetermineNewCharMaxHitPoints(dude->hitpointSeed);
        dude->hitPoints = dude->maxHitPoints;
      };
      result|=2;
      break;
    };
    break;
  default:
    ASSERT(0);
    return 0;  // Internal error
  };
  if (redraw)
  {     
    StatsForm.ClearForm();
    int junk=-1;
    STF_highlight(&StatsForm,&junk,currentSelection);
    showCharStats(&StatsForm, *dude);
    if (availPoints)
    {
      char temp[20];
      StatsForm.SetText(STF_AVAIL,"AVAIL");
      sprintf(temp,"    %d",availPoints);
      StatsForm.SetText(STF_avail,temp);
    };
    //STF_highlight(&StatsForm,&junk,currentSelection);
    flip=true;
  };
  if (flip)
  {
    GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);   
    drawBorder();
    drawViewportFrame(VPFrameXOffset, VPFrameY+VPFrameYOffset);
    StatsForm.Display(0);
  };
  return result|(flip?1:0);
}
