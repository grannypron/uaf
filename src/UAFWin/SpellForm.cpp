/******************************************************************************
* Filename: SpellForm.cpp
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
#include "DispText.h"
#include "Spell.h"
#include "FormattedText.h"
#include "SpellForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static const int pagesize=Items_Per_Page;

static const int labelsX=18;
static const int labelsY=18;

static const int itemsY=36;

// 
// all lower = text field
// all upper = field label (static)
// first upper, rest lower = highlight area

enum ST_SPELLFORM { // Define field names as integers for quicker comparisons
  STSF_none,
  STSF_white=TEXT_FORM::white,    // White items  

  STSF_SCHOOL,
  STSF_LEVEL,
  STSF_SELECT,
  STSF_MEMORIZE,
  STSF_AVAILABLE,
  STSF_COST,
  STSF_NAME, 

  STSF_MUAVAIL,
  STSF_muavail,
  STSF_CLERICAVAIL,
  STSF_clericavail,
  STSF_THIEFAVAIL,
  STSF_thiefavail,
  STSF_FIGHTERAVAIL,
  STSF_fighteravail,
  STSF_PALADINAVAIL,
  STSF_paladinavail,
  STSF_RANGERAVAIL,
  STSF_rangeravail,
  STSF_DRUIDAVAIL,  
  STSF_druidavail,
  STSF_TEXT1,
  STSF_TEXT2,
  STSF_TEXT3,
  STSF_TEXT4,
  STSF_TEXT5,

  STSF_school,
  STSF_level,
  STSF_select,
  STSF_memorize,
  STSF_available,
  STSF_cost,
  STSF_name,

  STSF_REPEAT,
  STSF_repeat=TEXT_FORM::autorepeat
};

//_MSC_VER >= 1400 : this is Visual C++ 2005
//_MSC_VER >= 1310 : this is Visual c++ .NET 2003
//_MSC_VER >  1300 : this is Visual C++ .NET 2002

#if _MSC_VER >= 1310
#define STSF(item,xrel,x,yrel,y) \
  DISPLAY_FORM(STSF_##xrel,STSF_##yrel,STSF_##item,x,y),
#else
#define STSF(item,xrel,x,yrel,y) \
  { DISPLAY_FORM(STSF_##xrel,STSF_##yrel,STSF_##item,x,y) },
#endif

#define STSFC(item,xrel,x,yrel,y,c,s) \
  DISPLAY_FORM(STSF_##xrel,STSF_##yrel,STSF_##item,x,y,c,s),


// when using relative coords, it is imperative
// that you dont display item B relative to item A 
// before actually displaying item A.
static DISPLAY_FORM spellForm[] =
{
  //
  //   enum      xrel            x          yrel       y
  // 
  STSF(LEVEL    ,none           ,labelsX   ,none     ,labelsY)  // "LEVEL"
  STSF(SELECT   ,LEVEL+END      ,5         ,none     ,labelsY)  // "SELECTED"
  STSF(MEMORIZE ,SELECT+END     ,5         ,none     ,labelsY)  // "MEMORIZED"
  STSF(COST     ,MEMORIZE+END   ,5         ,none     ,labelsY)  // "COST"
  STSF(NAME     ,COST+END       ,40        ,none     ,labelsY)  // "SPELL"

  // TEXTBOX_WIDTH varies at runtime depending on
  // current resolution mode (640,800,1024).
  //
  // The MUAVAIL x,y values (non-relative) are updated on the fly below,
  // so these numbers don't mean anything.
  STSF(MUAVAIL      ,none+RIGHTJUST,0         ,none             ,0)
  STSF(muavail      ,MUAVAIL+END   ,0         ,MUAVAIL          ,0)

  // the rest are drawn relative to the first pair
  STSF(CLERICAVAIL  ,MUAVAIL+RIGHT ,0         ,MUAVAIL+END      ,0)
  STSF(clericavail  ,muavail       ,0         ,CLERICAVAIL      ,0)
  STSF(THIEFAVAIL   ,MUAVAIL+RIGHT ,0         ,CLERICAVAIL+END  ,0)
  STSF(thiefavail   ,muavail       ,0         ,THIEFAVAIL       ,0)
  STSF(FIGHTERAVAIL ,MUAVAIL+RIGHT ,0         ,THIEFAVAIL+END   ,0)
  STSF(fighteravail ,muavail       ,0         ,FIGHTERAVAIL     ,0)
  STSF(PALADINAVAIL ,MUAVAIL+RIGHT ,0         ,FIGHTERAVAIL+END ,0)
  STSF(paladinavail ,muavail       ,0         ,PALADINAVAIL     ,0)
  STSF(RANGERAVAIL  ,MUAVAIL+RIGHT ,0         ,FIGHTERAVAIL+END ,0)//manikus 7-20-07
  STSF(rangeravail  ,muavail       ,0         ,RANGERAVAIL      ,0)
  STSF(DRUIDAVAIL   ,MUAVAIL+RIGHT ,0         ,CLERICAVAIL+END  ,0)//manikus 7-20-07
  STSF(druidavail   ,muavail       ,0         ,DRUIDAVAIL       ,0)
  /*Druid and Ranger spell availabitly was moved up from bottom to avoid being displayed over border graphics.*/

  // Repeat lines must be last
  // This one says, take the next 5 lines and repeat them 'pagesize' times.
  // The enums (fieldname) will be auto incremented
  STSF(REPEAT   ,repeat         ,5       ,none     ,pagesize) // auto repeat the rest

  STSF(level    ,LEVEL+RIGHT    ,0       ,none     ,itemsY)   // "1"
  STSF(select   ,SELECT+RIGHT   ,0       ,none     ,itemsY)   // "Yes"
  STSF(memorize ,MEMORIZE+RIGHT ,0       ,none     ,itemsY)   // "1"
  STSF(cost     ,COST+RIGHT     ,0       ,none     ,itemsY)   // "100"  
  STSF(name     ,NAME           ,0       ,none     ,itemsY)   // "Cure Light Wounds"
  
  STSF(none,none,0,none,0)  // End of list
};

// when using relative coords, it is imperative
// that you dont display item B relative to item A 
// before actually displaying item A.
DISPLAY_FORM initialSpellForm[] =
{
  //
  //   enum      xrel            x          yrel       y
  // 
  STSF(LEVEL    ,none           ,labelsX   ,none     ,labelsY)  // "LEVEL"
  //STSF(SELECT   ,LEVEL+END      ,5         ,none     ,labelsY)  // "SELECTED"
  //STSF(MEMORIZE ,SELECT+END     ,5         ,none     ,labelsY)  // "MEMORIZED"
  //STSF(COST     ,MEMORIZE+END   ,5         ,none     ,labelsY)  // "COST"
  STSF(NAME     ,LEVEL+END       ,40        ,none     ,labelsY)  // "SPELL"

  // TEXTBOX_WIDTH varies at runtime depending on
  // current resolution mode (640,800,1024).
  //
  STSF(TEXT1        ,none          ,0         ,none             ,0)
  STSF(TEXT2        ,TEXT1         ,0         ,TEXT1+END        ,0)
  STSF(TEXT3        ,TEXT1         ,0         ,TEXT2+END        ,0)
  STSF(TEXT4        ,TEXT1         ,0         ,TEXT3+END        ,0)
  STSF(TEXT5        ,TEXT1         ,0         ,TEXT4+END        ,0)

  // the rest are drawn relative to the first pair
  //STSF(CLERICAVAIL  ,MUAVAIL+RIGHT ,0         ,MUAVAIL+END      ,0)
  //STSF(clericavail  ,muavail       ,0         ,CLERICAVAIL      ,0)
  //STSF(THIEFAVAIL   ,MUAVAIL+RIGHT ,0         ,CLERICAVAIL+END  ,0)
  //STSF(thiefavail   ,muavail       ,0         ,THIEFAVAIL       ,0)
  //STSF(FIGHTERAVAIL ,MUAVAIL+RIGHT ,0         ,THIEFAVAIL+END   ,0)
  //STSF(fighteravail ,muavail       ,0         ,FIGHTERAVAIL     ,0)
  //STSF(PALADINAVAIL ,MUAVAIL+RIGHT ,0         ,FIGHTERAVAIL+END ,0)
  //STSF(paladinavail ,muavail       ,0         ,PALADINAVAIL     ,0)
  //STSF(RANGERAVAIL  ,MUAVAIL+RIGHT ,0         ,FIGHTERAVAIL+END ,0)//manikus 7-20-07
  //STSF(rangeravail  ,muavail       ,0         ,RANGERAVAIL      ,0)
  //STSF(DRUIDAVAIL   ,MUAVAIL+RIGHT ,0         ,CLERICAVAIL+END  ,0)//manikus 7-20-07
  //STSF(druidavail   ,muavail       ,0         ,DRUIDAVAIL       ,0)
  /*Druid and Ranger spell availabitly was moved up from bottom to avoid being displayed over border graphics.*/

  // Repeat lines must be last
  // This one says, take the next 5 lines and repeat them 'pagesize' times.
  // The enums (fieldname) will be auto incremented
  STSF(REPEAT   ,repeat         ,5       ,none     ,pagesize) // auto repeat the rest

  STSF(level    ,LEVEL+RIGHT    ,0       ,none     ,itemsY)   // "1"
  STSF(select   ,SELECT+RIGHT   ,0       ,none     ,itemsY)   // "Yes"
  STSF(memorize ,MEMORIZE+RIGHT ,0       ,none     ,itemsY)   // "1"
  STSF(cost     ,COST+RIGHT     ,0       ,none     ,itemsY)   // "100"  
  STSF(name     ,NAME           ,0       ,none     ,itemsY)   // "Cure Light Wounds"
  
  STSF(none,none,0,none,0)  // End of list
};

// when using relative coords, it is imperative
// that you dont display item B relative to item A 
// before actually displaying item A.
DISPLAY_FORM memorizeSpellForm[] =
{
  //
  //   enum      xrel            x          yrel       y
  // 
  STSFC(SCHOOL   ,none           ,labelsX   ,none     ,labelsY,1,0)  // SCHOOL
  STSFC(LEVEL    ,SCHOOL+END     ,5         ,none     ,labelsY,2,5)  // "LEVEL"
  STSFC(SELECT   ,LEVEL+END      ,5         ,none     ,labelsY,3,5)  // "SELECTED"
  STSFC(MEMORIZE ,SELECT+END     ,5         ,none     ,labelsY,4,5)  // "MEMORIZED"
  STSFC(AVAILABLE,MEMORIZE+END   ,5         ,none     ,labelsY,5,5)  // "MEMORIZED"
  //STSF(COST     ,MEMORIZE+END  ,5         ,none     ,labelsY)  // "COST"
  STSFC(NAME     ,AVAILABLE+END  ,40        ,none     ,labelsY,6,5)  // "SPELL"

  // TEXTBOX_WIDTH varies at runtime depending on
  // current resolution mode (640,800,1024).
  //
  //STSF(TEXT1        ,none          ,0         ,none             ,0)
  //STSF(TEXT2        ,TEXT1         ,0         ,TEXT1+END        ,0)
  //STSF(TEXT3        ,TEXT1         ,0         ,TEXT2+END        ,0)
  //STSF(TEXT4        ,TEXT1         ,0         ,TEXT3+END        ,0)
  //STSF(TEXT5        ,TEXT1         ,0         ,TEXT4+END        ,0)

  // the rest are drawn relative to the first pair
  //STSF(CLERICAVAIL  ,MUAVAIL+RIGHT ,0         ,MUAVAIL+END      ,0)
  //STSF(clericavail  ,muavail       ,0         ,CLERICAVAIL      ,0)
  //STSF(THIEFAVAIL   ,MUAVAIL+RIGHT ,0         ,CLERICAVAIL+END  ,0)
  //STSF(thiefavail   ,muavail       ,0         ,THIEFAVAIL       ,0)
  //STSF(FIGHTERAVAIL ,MUAVAIL+RIGHT ,0         ,THIEFAVAIL+END   ,0)
  //STSF(fighteravail ,muavail       ,0         ,FIGHTERAVAIL     ,0)
  //STSF(PALADINAVAIL ,MUAVAIL+RIGHT ,0         ,FIGHTERAVAIL+END ,0)
  //STSF(paladinavail ,muavail       ,0         ,PALADINAVAIL     ,0)
  //STSF(RANGERAVAIL  ,MUAVAIL+RIGHT ,0         ,FIGHTERAVAIL+END ,0)//manikus 7-20-07
  //STSF(rangeravail  ,muavail       ,0         ,RANGERAVAIL      ,0)
  //STSF(DRUIDAVAIL   ,MUAVAIL+RIGHT ,0         ,CLERICAVAIL+END  ,0)//manikus 7-20-07
  //STSF(druidavail   ,muavail       ,0         ,DRUIDAVAIL       ,0)
  /*Druid and Ranger spell availabitly was moved up from bottom to avoid being displayed over border graphics.*/

  // Repeat lines must be last
  // This one says, take the next 6 lines and repeat them 'pagesize' times.
  // The enums (fieldname) will be auto incremented by TEST_FORM::repeatIncr
  STSFC(REPEAT   ,repeat         ,6       ,none     ,pagesize, 0,0) // auto repeat the rest

  STSFC(school   ,SCHOOL         ,0       ,none     ,itemsY,1,0)   // Magic User
  STSFC(level    ,LEVEL+RIGHT    ,0       ,none     ,itemsY,2,0)   // "1"
  STSFC(select   ,SELECT+RIGHT   ,0       ,none     ,itemsY,3,0)   // "Yes"
  STSFC(memorize ,MEMORIZE+RIGHT ,0       ,none     ,itemsY,4,0)   // "1"
  STSFC(available,AVAILABLE+RIGHT,0       ,none     ,itemsY,5,0)   // "1"
  //STSF(cost     ,COST+RIGHT     ,0       ,none     ,itemsY)   // "100"  
  STSFC(name     ,NAME           ,0       ,none     ,itemsY,6,0)   // "Cure Light Wounds"
  
  STSFC(none,none,0,none,0,0,0)  // End of list
};



//CString BuildAvailSpellsForBaseclass(WORD BaseclassFlag, SPELL_TEXT_LIST &data)
CString BuildAvailSpellsForBaseclass(BASECLASS_ID baseclassID, SPELL_TEXT_LIST &data)
{
  SCHOOL_ID schoolID;
  BOOL NonZeroAvail = FALSE;
  CString temp;
  CString result("");






  const BASE_CLASS_DATA *pBaseclass;
  pBaseclass = baseclassData.PeekBaseclass(baseclassID);
  if (pBaseclass == NULL) return result;
  int i, n;
  n = pBaseclass->GetCastingInfoCount();
  for (i=0; i<n; i++)
  {
    //int classindex = FlagToIndex(GetSpellCastingSchoolForCharBaseclass(ClassFlag));
    schoolID = pBaseclass->PeekCastingInfo(i)->schoolID;
    for (int spellLevel=0;spellLevel<MAX_SPELL_LEVEL/*data.MaxLevel[classindex]*/;spellLevel++)
    {
#ifdef newMEMORIZE
      //if (data.maxSelectBySchool[classindex][j] > 0)
      if (data.MaxSelect(schoolID, spellLevel) > 0)
#else
      if (data.MaxSelect[j][classindex] > 0)
#endif
      {
#ifdef newMEMORIZE
        //temp.Format("   %u", data.maxSelectBySchool[classindex][j]-data.currSelectBySchool[classindex][j]);
        temp.Format("   %u", data.MaxSelect(schoolID, spellLevel) - data.CurrSelect(schoolID, spellLevel));
#else
        temp.Format("   %u", data.MaxSelect[j][classindex]-data.CurrSelect[j][classindex]);
#endif
        result += temp;
        NonZeroAvail = TRUE;
      }
      else
      {
        result += "   0";
      }
    };
  };
  




  if (!NonZeroAvail)
    result.Empty();
  
  return result;
}


//CString BuildAvailSpellsForSchool(int spellSchool, SPELL_TEXT_LIST &spellTextList)
CString BuildAvailSpellsForSchool(const SCHOOL_ID& schoolID, SPELL_TEXT_LIST &spellTextList)
{
  BOOL NonZeroAvail = FALSE;
  CString temp;
  CString result("");

  //int classindex = FlagToIndex(GetSpellCastingSchoolForCharBaseclass(ClassFlag));
  
  for (int spellLevel=1; spellLevel<=MAX_SPELL_LEVEL; spellLevel++)
  {
    int max, selected;
    //max = 0;
    //for (charBaseClass=0; charBaseClass<MAX_BASE_CLASSES; charBaseClass++)
    //{
      //max      += spellTextList.maxSelectBySchool [spellSchool][spellLevel-1][charBaseClass] ;
      max  = spellTextList.MaxSelect(schoolID, spellLevel-1) ;
    //};
    //selected = spellTextList.currSelectBySchool[spellSchool][spellLevel-1];
    selected = spellTextList.CurrSelect(schoolID,spellLevel-1);
    if (max - selected > 0)
    {
      temp.Format("   %2u", max-selected);
      result += temp;
      NonZeroAvail = TRUE;
    }
    else
    {
      result += "    0";
    };
  }
  
  if (!NonZeroAvail)
    result.Empty();
 
  return result;
}


void showItems(TEXT_FORM *pForm, SPELL_TEXT_LIST &spellTextList)
{ 
   ASSERT(pForm!=NULL);

   // make sure that highlight flags are set after clearing form
   pForm->ClearForm();
   
   memset(InventoryRects, 0, sizeof(InventoryRects));
   int ir_idx = 0;   

   if (spellTextList.UseMemorizeLayout())
   {
     pForm->SetText( STSF_SCHOOL,   "SCHOOL");
	   pForm->SetText( STSF_LEVEL,    "LEV");
	   pForm->SetText( STSF_SELECT,   "SEL");
	   pForm->SetText( STSF_MEMORIZE, "MEM");
     pForm->SetText( STSF_AVAILABLE,"AVAIL");
     //pForm->SetText( STSF_COST,"");
   }
   else if (spellTextList.UseCastLayout())
   {
	    pForm->SetText( STSF_LEVEL,"LEVEL");
      pForm->SetText( STSF_SELECT,"");
      if (spellTextList.memorizedText.IsEmpty())
      {
	      pForm->SetText( STSF_MEMORIZE,"MEMORIZED");
      }
      else
      {
	      pForm->SetText( STSF_MEMORIZE,spellTextList.memorizedText);
      };
      pForm->SetText( STSF_COST,"");
   }
   else if (spellTextList.UseTempleCastLayout())
   {
	    pForm->SetText( STSF_LEVEL,"LEVEL");
      pForm->SetText( STSF_SELECT,"");
	    pForm->SetText( STSF_MEMORIZE,"");
      pForm->SetText( STSF_COST,"COST");
   }
   else if (spellTextList.UseNewSpellLevelLayout())
   {
	    pForm->SetText( STSF_LEVEL,"LEVEL");
	    pForm->SetText( STSF_SELECT,"SELECT");
	    pForm->SetText( STSF_MEMORIZE,"");
      pForm->SetText( STSF_COST,"");
   }
   else
   {
	    if (spellTextList.UseLevel())
 	      pForm->SetText( STSF_LEVEL,"LEVEL");
      else
        pForm->SetText( STSF_LEVEL,"");
	    pForm->SetText( STSF_SELECT,"");
	    pForm->SetText( STSF_MEMORIZE,"");
	    if (spellTextList.UseCost())
		    pForm->SetText( STSF_COST,"COST");
      else
        pForm->SetText( STSF_COST,"");
   }
   pForm->SetText( STSF_NAME,"SPELL");

   if ((spellTextList.UseMemorizeLayout()) || (spellTextList.UseNewSpellLevelLayout()))
   {
     // display available spell slots for each spell school

     // TEXTBOX_WIDTH varies at runtime depending on
     // current resolution mode (640,800,1024)
     int mux = textboxX+(TEXTBOX_WIDTH/4);
     int muy = textboxY+10;
     pForm->SetXY(STSF_MUAVAIL, mux,    muy); // 'MAGIC USER SPELLS:'
     pForm->SetXY(STSF_muavail, 50/*mux+50*/, 0);   // '7 5 3 3 2 2 1 1'
     // ... the rest are relative to the two above fields

     CString format;




/*
     else
     {
       pForm->SetText( STSF_MUAVAIL,"MAGIC USER SPELLS:");
       pForm->EnableItem(STSF_MUAVAIL, false);
       pForm->SetText( STSF_muavail,"");
     }

     // Cleric spells
     flag = (spellTextList.spellClasses & ClericFlag);
     if (flag)
     {
       CString AvailSpells = BuildAvailSpellsForBaseclass(flag, spellTextList);
       if (!AvailSpells.IsEmpty())
       {
         CString temp(classspellTextList[ClassFlagToCharClass(flag)].m_name);
         temp += " Spells: ";
         pForm->SetText( STSF_CLERICAVAIL,temp);
         pForm->SetText( STSF_clericavail,AvailSpells);
       }
       else
       {
         pForm->SetText( STSF_CLERICAVAIL,"");
         pForm->SetText( STSF_clericavail,"");
       }
     }
     else
     {
       pForm->SetText( STSF_CLERICAVAIL,"");
       pForm->SetText( STSF_clericavail,"");
     }

     // Thief spells
     flag = (spellTextList.spellClasses & ThiefFlag);
     if (flag)
     {
       CString AvailSpells = BuildAvailSpellsForBaseclass(flag, spellTextList);
       if (!AvailSpells.IsEmpty())
       {
         CString temp(classspellTextList[ClassFlagToCharClass(flag)].m_name);
         temp += " Spells: ";
         pForm->SetText( STSF_THIEFAVAIL,temp);
         pForm->SetText( STSF_thiefavail,AvailSpells);
       }
       else
       {
         pForm->SetText( STSF_THIEFAVAIL,"");
         pForm->SetText( STSF_thiefavail,"");
       }
     }
     else
     {
       pForm->SetText( STSF_THIEFAVAIL,"");
       pForm->SetText( STSF_thiefavail,"");
     }

     // Fighter spells
     flag = (spellTextList.spellClasses & FighterFlag);
     if (flag)
     {
       CString AvailSpells = BuildAvailSpellsForBaseclass(flag, spellTextList);
       if (!AvailSpells.IsEmpty())
       {
         CString temp(classspellTextList[ClassFlagToCharClass(flag)].m_name);
         temp += " Spells: ";
         pForm->SetText( STSF_FIGHTERAVAIL,temp);
         pForm->SetText( STSF_fighteravail,AvailSpells);
       }
       else
       {
         pForm->SetText( STSF_FIGHTERAVAIL,"");
         pForm->SetText( STSF_fighteravail,"");
       }
     }
     else
     {
       pForm->SetText( STSF_FIGHTERAVAIL,"");
       pForm->SetText( STSF_fighteravail,"");
     }

     // Paladin spells
     flag = (spellTextList.spellClasses & PaladinFlag);
     if (flag)
     {
       CString AvailSpells = BuildAvailSpellsForbaseClass(flag, spellTextList);
       if (!AvailSpells.IsEmpty())
       {
         CString temp(classspellTextList[ClassFlagToCharClass(flag)].m_name);
         temp += " Spells: ";
         pForm->SetText( STSF_PALADINAVAIL,temp);
         pForm->SetText( STSF_paladinavail,AvailSpells);
       }
       else
       {
         pForm->SetText( STSF_PALADINAVAIL,"");
         pForm->SetText( STSF_paladinavail,"");
       }
     }
     else
     {
       pForm->SetText( STSF_PALADINAVAIL,"");
       pForm->SetText( STSF_paladinavail,"");
     }

     // Ranger spells
     flag = (spellTextList.spellClasses & RangerFlag);
     if (flag)
     {
       CString AvailSpells = BuildAvailSpellsForBaseclass(flag, spellTextList);
       if (!AvailSpells.IsEmpty())
       {
         CString temp(classspellTextList[ClassFlagToCharClass(flag)].m_name);
         temp += " Spells: ";
         pForm->SetText( STSF_RANGERAVAIL,temp);
         pForm->SetText( STSF_rangeravail,AvailSpells);
       }
       else
       {
         pForm->SetText( STSF_RANGERAVAIL,"");
         pForm->SetText( STSF_rangeravail,"");
       }
     }
     else
     {
       pForm->SetText( STSF_RANGERAVAIL,"");
       pForm->SetText( STSF_rangeravail,"");
     }

     // Druid spells
     flag = (spellTextList.spellClasses & DruidFlag);
     if (flag)
     {
       CString AvailSpells = BuildAvailSpellsForbaseClass(flag, spellTextList);
       if (!AvailSpells.IsEmpty())
       {
         CString temp(classspellTextList[ClassFlagToCharClass(flag)].m_name);
         temp += " Spells: ";
         pForm->SetText( STSF_DRUIDAVAIL,temp);
         pForm->SetText( STSF_druidavail,AvailSpells);
       }
       else
       {
         pForm->SetText( STSF_DRUIDAVAIL,"");
         pForm->SetText( STSF_druidavail,"");
       }
     }
     else
     {
       pForm->SetText( STSF_DRUIDAVAIL,"");
       pForm->SetText( STSF_druidavail,"");
     }
*/
   }
   else if (spellListText.UseArbitraryTextLayout())
   {







     // display available spell slots for each spell school

     // TEXTBOX_WIDTH varies at runtime depending on
     // current resolution mode (640,800,1024)
     int mux = textboxX; //+(TEXTBOX_WIDTH/4);
     int muy = textboxY+10;
     pForm->SetXY(STSF_TEXT1, mux,    muy); // 'MAGIC USER SPELLS:'
     //pForm->SetXY(STSF_muavail, 50/*mux+50*/, 0);   // '7 5 3 3 2 2 1 1'
     // ... the rest are relative to the two above fields

     WORD flag;
     CString format;

     // Magic User spells
     flag =  1; //(data.spellClasses & MagicUserFlag);
     if (flag)
     {            
       {
         //temp = " Any Text: ";
         //pForm->SetText( STSF_MUAVAIL,temp);
         pForm->SetText( STSF_TEXT1, spellListText.arbitraryText[0]);
         pForm->SetText( STSF_TEXT2, spellListText.arbitraryText[1]);
         pForm->SetText( STSF_TEXT3, spellListText.arbitraryText[2]);
         pForm->SetText( STSF_TEXT4, spellListText.arbitraryText[3]);
       };
     };
   }









   else
   {
     pForm->SetText( STSF_MUAVAIL,"");
     pForm->SetText( STSF_muavail,"");
     pForm->SetText( STSF_CLERICAVAIL,"");
     pForm->SetText( STSF_clericavail,"");
     pForm->SetText( STSF_THIEFAVAIL,"");
     pForm->SetText( STSF_thiefavail,"");
     pForm->SetText( STSF_FIGHTERAVAIL,"");
     pForm->SetText( STSF_fighteravail,"");
     pForm->SetText( STSF_PALADINAVAIL,"");
     pForm->SetText( STSF_paladinavail,"");
     pForm->SetText( STSF_RANGERAVAIL,"");
     pForm->SetText( STSF_rangeravail,"");
     pForm->SetText( STSF_DRUIDAVAIL,"");       
     pForm->SetText( STSF_druidavail,"");
   }

   // blank out all repeat items
   for (int i=0;i<pagesize;i++)
   {
      int EnumOffset = (i*5);
      pForm->SetText(STSF_level+EnumOffset,    "");
      pForm->SetText(STSF_select+EnumOffset,   "");
      pForm->SetText(STSF_memorize+EnumOffset, "");
      pForm->SetText(STSF_cost+EnumOffset,     "");
      pForm->SetText(STSF_name+EnumOffset,     "");
   }


   int totalListItems;
   if (spellTextList.m_flags & CHAR_VIEW_DISPLAY)
   {
     totalListItems = spellTextList.GetEffectCount();
   }
   else if (spellListText.m_flags & MEMORIZE_FORMAT_FLAG)
   {
     totalListItems = spellTextList.memSum.GetMemorizeItemCount();
   }
   else if (spellListText.m_flags & CAST_FORMAT_FLAG)
   {
     totalListItems = spellListText.GetIntegerCount();
   }
   else
   {
     totalListItems = spellTextList.GetCount();
   };
   int currItem = startItem;
   int displaycount = 0;
   CRect tmpRECT,totalRECT;

   while ((currItem < totalListItems) && (displaycount < pagesize))
   {      
     int EnumOffset = (displaycount*TEXT_FORM::repeatIncr);

      if (spellTextList.UseSpellEffectList())
      {
        totalRECT = pForm->SetText(STSF_level+EnumOffset,  spellTextList.GetSpellEffectLevelText(currItem));
        tmpRECT = pForm->SetText(STSF_select+EnumOffset,   spellTextList.GetSpellSelectedText(currItem));
        totalRECT.UnionRect(totalRECT, tmpRECT);

        tmpRECT = pForm->SetText(STSF_memorize+EnumOffset, spellTextList.GetSpellMemorizedText(currItem));
        totalRECT.UnionRect(totalRECT, tmpRECT);

        tmpRECT = pForm->SetText(STSF_cost+EnumOffset,     spellTextList.GetSpellCostText(currItem));
        totalRECT.UnionRect(totalRECT, tmpRECT);
      }
      else if (spellTextList.UseMemorizeLayout())
      {
        totalRECT = pForm->SetText(STSF_school+EnumOffset, spellTextList.GetSpellSchoolText(currItem));
        tmpRECT = pForm->SetText(STSF_level+EnumOffset,  spellTextList.GetSpellLevelText(currItem));
        totalRECT.UnionRect(totalRECT, tmpRECT);
        tmpRECT = pForm->SetText(STSF_select+EnumOffset,   spellTextList.GetSpellSelectedText(currItem));
        totalRECT.UnionRect(totalRECT, tmpRECT);
        tmpRECT = pForm->SetText(STSF_memorize+EnumOffset, spellTextList.GetSpellMemorizedText(currItem));
        totalRECT.UnionRect(totalRECT, tmpRECT);
        tmpRECT = pForm->SetText(STSF_available+EnumOffset, spellTextList.GetSpellAvailableText(currItem));
        totalRECT.UnionRect(totalRECT, tmpRECT);
      }
      else if (spellTextList.UseCastLayout())
      {
        //totalRECT = pForm->SetText(STSF_school+EnumOffset, spellTextList.GetSpellSchoolText(currItem));
        totalRECT = pForm->SetText(STSF_level+EnumOffset,  spellTextList.GetSpellLevelText(currItem));
        //totalRECT.UnionRect(totalRECT, tmpRECT);
        //tmpRECT = pForm->SetText(STSF_select+EnumOffset,   spellTextList.GetSpellSelectedText(currItem));
        //totalRECT.UnionRect(totalRECT, tmpRECT);
        tmpRECT = pForm->SetText(STSF_memorize+EnumOffset, spellTextList.GetSpellMemorizedText(currItem));
        totalRECT.UnionRect(totalRECT, tmpRECT);
        //tmpRECT = pForm->SetText(STSF_available+EnumOffset, spellTextList.GetSpellAvailableText(currItem));
        //totalRECT.UnionRect(totalRECT, tmpRECT);
      }
      else
      {
        totalRECT = pForm->SetText(STSF_level+EnumOffset,  spellTextList.GetSpellLevelText(currItem));
        tmpRECT = pForm->SetText(STSF_select+EnumOffset,   spellTextList.GetSpellSelectedText(currItem));
        totalRECT.UnionRect(totalRECT, tmpRECT);

        tmpRECT = pForm->SetText(STSF_memorize+EnumOffset, spellTextList.GetSpellMemorizedText(currItem));
        totalRECT.UnionRect(totalRECT, tmpRECT);

        tmpRECT = pForm->SetText(STSF_cost+EnumOffset,     spellTextList.GetSpellCostText(currItem));
        totalRECT.UnionRect(totalRECT, tmpRECT);
      };


      //int selcnt=0;       
      //int memcnt=0; 
      //if (spellTextList.GetSpellSelectedText(currItem) != NULL)
      //  selcnt = atoi(spellTextList.GetSpellSelectedText(currItem));
      //if (spellTextList.GetSpellMemorizedText(currItem) != NULL)
      //  memcnt = atoi(spellTextList.GetSpellMemorizedText(currItem));


      {
// prs 20191221        COLORREF color;
        FONT_COLOR_NUM colorNum;
        if (spellListText.UseArbitraryTextLayout())
        {
// prs 20191221          color = White;
          colorNum = whiteColor;
        }
        else if (spellListText.UseSpellEffectList())
        {
// 20191221          color = White;
          colorNum = whiteColor;
        }
        else
        {
          //COLORREF Orange = RGB(255,128,0);
          int memcnt, selcnt;
          memcnt = spellListText.GetSpellMemorizeCount(currItem);
          selcnt = spellListText.GetSpellSelectedCount(currItem);

          if (memcnt == 0)
          {
// prs 20191221            if (selcnt == 0) color = White;
//            else color = Yellow;
            if (selcnt == 0) colorNum = whiteColor;
            else colorNum = yellowColor;
          }
          else
          {
            /* prs 20191221
            if(selcnt == 0) color = White;
            else
            {
              if (selcnt == memcnt) color = Green;
              else if (selcnt < memcnt) color = Green;
              else color = Green;
            };
            */
            if (selcnt == 0) colorNum = whiteColor;
            else
            {
              if (selcnt == memcnt) colorNum = greenColor;
              else if (selcnt < memcnt) colorNum = greenColor;
              else colorNum = greenColor;
            };
          };
        };
        //color = White;
        if (spellTextList.UseSpellEffectList())
        {
          tmpRECT = pForm->SetText(STSF_name+EnumOffset, 
                                   spellTextList.GetSpellEffectText(currItem), 
// prs 20191221                                   color);
                                   colorNum);
        }
        else if (spellTextList.UseMemorizeLayout())
        {
          tmpRECT = pForm->SetText(STSF_name+EnumOffset, 
                                   spellTextList.GetSpellNameText(currItem), 
// prs 20191221                                   color);
                                   colorNum);
        }
        else
        {
          tmpRECT = pForm->SetText(STSF_name+EnumOffset, 
                                   spellTextList.GetSpellNameText(currItem), 
// prs 20191221                                   color);
                                   colorNum);
        };
      };

      totalRECT.UnionRect(totalRECT, tmpRECT);

      //pForm->SetText(STSF_Row+EnumOffset, "");

      pForm->Highlight(STSF_name+EnumOffset, shouldHighlight(currItem)?true:false);
      
      CopyRect(&InventoryRects[ir_idx], &totalRECT);

      ir_idx++;
      currItem++;
      displaycount++;
   }
}

void displaySpells(SPELL_TEXT_LIST &data)
{
  TEXT_FORM SpellForm(spellForm);
  SpellForm.ClearForm();
  showItems(&SpellForm,data);
  SpellForm.Display(0);
}

int handleSpellFormInput(SPELL_FORM_INPUT_MESSAGE_TYPE msg,
                         SPELL_TEXT_LIST &data,
                         DISPLAY_FORM *pDisplayForm)
// Returns 0x0001 if you should display menu and flip surfaces.
// Returns 0x0002 if we handled a keystroke and you can ignore it
{
  TEXT_FORM SpellForm((pDisplayForm!=NULL)? pDisplayForm : spellForm);
  int result=0;
  bool flip=false, redraw=false;

  switch (msg)
  {
  case SPELLFORM_initial:
    redraw=true;
    break;
  case SPELLFORM_refresh:
    redraw=true;
    break;
  case SPELLFORM_exit:
    break;
  default:
    ASSERT(0);
    return 0;  // Internal error
  };
  if (redraw)
  {     
    SpellForm.ClearForm();
    showItems(&SpellForm, data);  // To define rectangles.
    flip=true;
  };
  if (flip)
  {
    SpellForm.Display(0);
  };

  CString spellDesc;
  SPELL_ID spellID;
  spellID = spellListText.GetSpellID(currSpellTextIndex());
  if (!spellID.IsEmpty()) {
      int spellIdx = spellData.LocateSpell(spellID);
      if (spellIdx >= 0) {
          SPELL_DATA* spell;
          TEXT_DISPLAY_DATA txtData;
          spell = spellData.GetSpell(spellIdx);
          spellDesc.Format("%s\r\n%s", spell->Name, spell->Description);
          FormatDisplayText(txtData, spellDesc, false, false, false);
          DisplayFormattedTextAtPos(400, 100, txtData, whiteColor, 0);
      }
  }
  return result|(flip?1:0);
}


